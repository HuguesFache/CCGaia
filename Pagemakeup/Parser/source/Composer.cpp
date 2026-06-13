/*
//	File:	Composer.cpp
//
//	Date:	7-Nov-2005
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// Project includes
#include "GlobalDefs.h"
#include "Composer.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "IDocHelper.h"
#include "IItemManager.h"
#include "IItemTransform.h"
#include "TCLError.h"
#include "ErrorUtils.h"
#include "GraphicTypes.h"
#include "PMLineSeg.h"

// Interfaces includes
#include "IGeometry.h"
#include "IHierarchy.h"
#include "IUnitOfMeasure.h"
#include "IDocumentLayer.h"
#include "IPageList.h"
#include "IDocument.h"
#include "ILayoutUtils.h"
#include "ILayerUtils.h"
#include "IPathUtils.h"
#include "ICommand.h"
#include "TransformUtils.h"
#include "ISpreadList.h"
#include "ISpread.h"
#include "IGraphicAttributeUtils.h"
#include "IGraphicStyleDescriptor.h"
#include "ITransformFacade.h"
#include "IFrameUtils.h"
#include "ITextUtils.h"
#include "ITextModel.h"
#include "IAttributeStrand.h"
#include "ITextAttrRealNumber.h" 
#include "ITextAttrScalableMeasure.h"
#include "ITextAttributes.h"
#include "ITextModelCmds.h"
#include "IFrameList.h"
#include "IWaxLine.h"
#include "IWaxLineShape.h"
#include "IWaxStrand.h"
#include "IWaxIterator.h"
#include "IFrameListComposer.h"
#include "IStyleInfo.h"
#include "ISwatchUtils.h"
#include "IIntData.h"
#include "ITextFrameColumn.h"
#include "IGeometryFacade.h"
#include "TextAttributeRunIterator.h"

#if !SERVER
#include "ILayoutUIUtils.h"
#endif

//#include "CAlert.h"

/* Constructor
*/
Func_BW::Func_BW(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BW::Func_BW -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		ref = reader->ExtractTCLRef(kCurrentTCLRef);
		unit = reader->ExtractChar();
		round = reader->ExtractInt(1,0,10);
		direction = reader->ExtractInt(DIR_UP,DIR_DOWN,DIR_OPT);		
		tolerance = reader->ExtractReel('d','+',0,0);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error) 
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_BW(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_BW::~Func_BW()
{
}

/* IDCall_BW
*/
void Func_BW::IDCall_BW(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error(kErrNilInterface);

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		if(ref == kCurrentTCLRef)
			ref = parser->getCurItem();

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
			break;
		
		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
			break;

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		UID itemUID = parser->getItem(ref);

		// Get the depth of the item (in points)
		InterfacePtr<IGeometry> itemGeo (db, itemUID , UseDefaultIID());
		if(itemGeo == nil)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		PMReal depth = itemGeo->GetStrokeBoundingBox().Height();

		// BUG FIX 04/04/07 : when stroke alignment is set to "Stroke Inside", path bounding box differs a little from stroke bounding box
		int32 strokeAlign = 0;
		InterfacePtr<IGraphicStyleDescriptor> graphicDesc (itemGeo, UseDefaultIID());
		Utils<IGraphicAttributeUtils>()->GetStrokeAlignment(strokeAlign,graphicDesc);

		if(strokeAlign == kStrokeAlignInside)
			depth = itemGeo->GetPathBoundingBox().Height();
		// BUG FIX

		PMReal oldDepth = depth;

		ClassID measureID = kInvalidClass;
		switch (unit) {
			case 'P':
			case 'p':	
				measureID = kPicasBoss;
				break;
        
			case 'D':
			case 'd':
				measureID = kPointsBoss;
				break;

			case 'C':
			case 'c':
				measureID = kCicerosBoss;
				break;
		        
			case 'M':
			case 'm':
				measureID = kCentimetersBoss;
				break;
		        
			case 'N':
			case 'n':
				measureID = kMillimetersBoss;
				break;
		         
			case 'i':
			case 'I':
				measureID = kInchesBoss;
				depth *= 4;
				break;

			default:
				measureID = kInvalidClass;
				break;
		}
	   
		InterfacePtr<IUnitOfMeasure> uom (nil);
		if(measureID != kInvalidClass)
		{
			InterfacePtr<IUnitOfMeasure> uomTmp ((IUnitOfMeasure *)::CreateObject(measureID,IID_IUNITOFMEASURE));

			if(uomTmp == nil)
			{
				error = PMString(kErrConv);
				break;
			}

			uom = uomTmp;
		}
		else
		{
			error = PMString(kErrConv);
			break;
		}


		// Do the conversion
		depth = uom->PointsToUnits(depth);

		int32 depthInt = ::ToInt32(depth);

		int32 nearest = depthInt / round;
		switch(direction) 
		{
			case DIR_OPT :
				tolerance = uom->PointsToUnits(tolerance);
				
				if( depth - (nearest * round) <= tolerance) // if down round respects tolerance, use it otherwise round up
					nearest = nearest * round;
				else
					nearest = (nearest + 1) * round;
				break;
				
			case DIR_UP :			
				nearest = (nearest + 1) * round; // NOTE : bug in case depth was already rounded by chance (don't increment nearest)
				break;

			case DIR_DOWN :
			default :
				nearest = nearest * round;
				break;
		}

		// Convert the value back to points
		depth = uom->UnitsToPoints(PMReal(nearest));

		if(unit == 'i' || unit == 'I')
			depth /= 4;

		// Resize the item
		if(itemTransform->ResizeFrameCmd(UIDRef(db,itemUID),0, depth - oldDepth) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Select the item
		if((status = itemManager->SelectPageItem(UIDRef(db,itemUID))) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

	}while(false);


	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurItem(ref);
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);		
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_DW::Func_DW(ITCLParser *  parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BW::Func_BW -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		ref = reader->ExtractTCLRef(kCurrentTCLRef);
		offset = reader->ExtractReel('d','+',0,0);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error) 
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DW(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_DW::~Func_DW()
{
}

/* IDCall_DW
*/
void Func_DW::IDCall_DW(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error(kErrNilInterface);

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();
		UIDRef curDocRef = ::GetUIDRef(parser->getCurDoc());

		if(ref == kCurrentTCLRef)
			ref = parser->getCurItem();

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<IDocHelper> docHelper (parser->QueryDocHelper());
		if(docHelper == nil)
			break;

		InterfacePtr<IItemTransform> itemTransform (parser->QueryItemTransform());
		if(itemTransform == nil)
			break;

		UID itemUID = parser->getItem(ref);

		// Get the depth of the item (in points)
		InterfacePtr<IGeometry> itemGeo (db, itemUID , UseDefaultIID());
		if(itemGeo == nil)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IHierarchy> itemHier (itemGeo, UseDefaultIID());

		InterfacePtr<IGeometry> ownerPage (db, Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier),UseDefaultIID());
		PMRect ownerPageBB = ownerPage->GetPathBoundingBox();

		// Compute new height of the document
		PMPoint frameBottom = itemGeo->GetStrokeBoundingBox().LeftBottom();

		// BUG FIX 11/04/07 : when stroke alignment is set to "Stroke Inside", path bounding box differs a little from stroke bounding box
		int32 strokeAlign = 0;
		InterfacePtr<IGraphicStyleDescriptor> graphicDesc (itemGeo, UseDefaultIID());
		Utils<IGraphicAttributeUtils>()->GetStrokeAlignment(strokeAlign,graphicDesc);
		
		if(strokeAlign == kStrokeAlignInside)
			frameBottom = itemGeo->GetPathBoundingBox().LeftBottom();
		// BUG FIX

		::TransformInnerPointToPasteboard(itemGeo, &frameBottom);

		PMPoint pageBottom = ownerPageBB.LeftBottom();
		::TransformInnerPointToPasteboard(ownerPage, &pageBottom);

		PMReal diff = frameBottom.Y() - pageBottom.Y() + offset;

		PMReal newHeight = ownerPageBB.Height() + diff;
		
		// Keep the current width
		PMReal width = ownerPageBB.Width();

		PMRect newPageDim(0,0,width,newHeight);

		if(docHelper->SetPageSizeCmd(curDocRef,newPageDim, (width > newHeight)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Reset items position
		PMPoint delta (0, -diff/2); // items have been moved vertically of diff/2

		UIDList allItemsOnSpread(db);
		InterfacePtr<ISpreadList> spreadList (curDocRef, UseDefaultIID());
		
		int32 nbSpreads = spreadList->GetSpreadCount();
		int32 count = 0;
		for(int32 i = 0 ; i < nbSpreads ; ++i)
		{
			InterfacePtr<ISpread> spread (db, spreadList->GetNthSpreadUID(i), UseDefaultIID());
			int32 nbPages = spread->GetNumPages();
			for(int32 j = 0 ; j < nbPages ; ++j)
			{
				UIDList itemsOnPage(db);
				spread->GetItemsOnPage(j,&itemsOnPage,kFalse,kTrue);
				allItemsOnSpread.Append(itemsOnPage);
			}
			
			status = Utils<Facade::ITransformFacade>()->TransformItems( allItemsOnSpread, Transform::PasteboardCoordinates(), kZeroPoint, 
					 Transform::TranslateBy(delta.X(), delta.Y()));

			if(status != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			count += allItemsOnSpread.Length();
			allItemsOnSpread.Clear();
		}

#if !SERVER
		Utils<ILayoutUIUtils>()->InvalidateViews(parser->getCurDoc());
#endif

	}while(false);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurItem(ref);
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);		
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_B3::Func_B3(ITCLParser *  parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_B3::Func_B3 -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		ref = reader->ExtractTCLRef(kDefaultTCLRef);
		boxType = reader->ExtractInt(TYPE_TEXT,TYPE_TEXT,TYPE_IMAGE);
		back = reader->ExtractBooleen(kTrue);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error) 
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_B3(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_B3::~Func_B3()
{
}

/* IDCall_B3
*/
void Func_B3::IDCall_B3(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error(kErrNilInterface);

	UIDRef frameRef = UIDRef::gNull;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();
		UIDRef curDocRef = ::GetUIDRef(parser->getCurDoc());

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
			break;

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
			break;

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		// Get current page dimensions
		int16 curPageNum = parser->getCurPage();

		InterfacePtr<IPageList> pageList (curDocRef, UseDefaultIID());

		InterfacePtr<IGeometry> curPageGeo (db, pageList->GetNthPageUID(curPageNum-1),UseDefaultIID());

		PMRect frameDim = curPageGeo->GetPathBoundingBox();

		PMPointList points; 
		points.push_back(frameDim.LeftTop());
		points.push_back(frameDim.RightBottom());
		
		InterfacePtr<IDocumentLayer> activeLayer (Utils<ILayerUtils>()->QueryDocumentActiveLayer(parser->getCurDoc()));
		if(activeLayer == nil)
			break;
		
		UID ownerSpreadLayer = itemManager->GetPBCoordAbs(parser->getCurDoc(),curPageNum,points,activeLayer);
		
		// Create frame according to type passed in parameter		
		switch(boxType)
		{
			case TYPE_TEXT : frameRef = itemManager->CreateTextFrameCmd(db,points,ownerSpreadLayer,1,12); break;

			case TYPE_IMAGE : 
				PMRect boundsRect(points[0],points[1]);
				frameRef =  Utils<IPathUtils>()->CreateRectangleSpline(UIDRef(db,ownerSpreadLayer),boundsRect, INewPageItemCmdData::kGraphicFrameAttributes); 
				break;				
		}

		if(frameRef == UIDRef::gNull)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		// Send the new frame to back if required
		if(back)
		{
			if(itemTransform->SendToBackCmd(frameRef) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
				
		// Select the item
		if((status = itemManager->SelectPageItem(frameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

	}while(false);


	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setItem(ref,frameRef.GetUID());
			parser->setCurItem(ref);
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);		
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_AW::Func_AW(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_AW::Func_AW -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		ref = reader->ExtractTCLRef(kCurrentTCLRef);
		threshold = reader->ExtractReel3('+','D',5,0,1,100);
		maxKerningValue = reader->ExtractInt(500,0,3000);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error) 
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_AW(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}


void Func_AW::IDCall_AW(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error(kErrNilInterface);

	Text::VerticalJustification vj = Text::kVJInvalid;
	UIDRef vjFrameRef = UIDRef::gNull;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();
		
		// Check whether the frame is a text frame which is not overset
		if(ref == kCurrentTCLRef)
			ref = parser->getCurItem();
		
		UID refUID = parser->getItem(ref);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IHierarchy> itemHier (db, refUID, UseDefaultIID());

		int32 textFlags;
		if(!Utils<IFrameUtils>()->IsTextFrame(itemHier, &textFlags))
		{
			error = PMString(kErrTextBoxRequired);
			break;
		}		

		if(textFlags & IFrameUtils::kTF_Overset)
		{
			error = PMString(kErrOversetFrame);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		// Get text model
		InterfacePtr<IMultiColumnTextFrame> txtFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
		InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());
		
		// Get last frame of story position
		InterfacePtr<IFrameList> frameList (txtModel->QueryFrameList());
		InterfacePtr<IWaxStrand> waxStrand (frameList, UseDefaultIID());
		InterfacePtr<ITextFrameColumn> lastTxtFrameCol (frameList->QueryNthFrame(frameList->GetFrameCount()-1));
		
		InterfacePtr<IHierarchy> lastSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(lastTxtFrameCol));
		InterfacePtr<IGeometry> lastSplineGeo (lastSpline, UseDefaultIID());

		PMReal strokeWeight = lastSplineGeo->GetStrokeBoundingBox().Bottom() - lastSplineGeo->GetPathBoundingBox().Bottom();
		
		PMRect bbox = lastSplineGeo->GetStrokeBoundingBox();
		::TransformInnerRectToPasteboard(lastSplineGeo, &bbox);

		PMReal lastFrameBottom = bbox.Bottom();

		// Temporary remove vertical justification for the adjustement process
		InterfacePtr<IMultiColumnTextFrame> lastMulticol (db, lastSpline->GetChildUID(0), UseDefaultIID());
		vj = lastMulticol->GetVerticalJustification();
		vjFrameRef = ::GetUIDRef(lastMulticol);

		InterfacePtr<ICommand> vertJustCmd(CmdUtils::CreateCommand(kSetFrameVertJustifyCmdBoss)); 
		vertJustCmd->SetItemList(UIDList(vjFrameRef));

		InterfacePtr<IIntData> vertJustCmdIntData(vertJustCmd, UseDefaultIID()); 
		vertJustCmdIntData->Set(Text::kVJTop);
			
		if(CmdUtils::ProcessCommand(vertJustCmd) != kSuccess)	
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Compute difference between last line position and frame bottom
		// Recompose thru the frame so that we get a valid waxline
		InterfacePtr<IFrameListComposer> frameComposer (lastTxtFrameCol, UseDefaultIID());
		if(frameComposer == nil)
			break;

		frameComposer->RecomposeThruLastFrame();
	
		PMReal diff = lastFrameBottom - this->GetFrameLastLineAbsolutePosition(waxStrand, lastTxtFrameCol) - strokeWeight;

		status = kSuccess;
		while(diff > threshold && (status == kSuccess))
		{			
			// Determine scale factor from total height of story frames compared to empty space
			PMReal totalHeight = this->ComputeTotalLeading(waxStrand);

			PMReal yFactor = 1 + (diff / totalHeight);
			if(yFactor > 2.0)
			{
				yFactor = 2.0;
				status = kFailure;
			}

			if(this->ApplyScaleFactorToLeading(txtModel, yFactor))
			{
				// Text contains autoleading attribute was found, set up a vertical justication instead
				status = kSuccess;
				vj = Text::kVJJustify;
				break;
			}
	
			frameComposer->RecomposeThruLastFrame();

			diff = lastFrameBottom - this->GetFrameLastLineAbsolutePosition(waxStrand, lastTxtFrameCol) - strokeWeight;						
		}

		if(status != kSuccess)
		{
			// Try to adjust on kerning
			int32 infKerningValue = 0, supKerningValue = maxKerningValue, currentKerningValue = maxKerningValue/4; // correspond to 100 in the UI
			InterfacePtr<ITextModelCmds> modelCmds(txtModel, UseDefaultIID()); 		
			
			bool16 isOverset = kFalse;
			do
			{									
				InterfacePtr<ITextAttrRealNumber> kernAttr ((ITextAttrRealNumber *) ::CreateObject(kTextAttrTrackKernBoss, IID_ITEXTATTRREALNUMBER));
				kernAttr->Set(PMReal(currentKerningValue)/1000.0);

				boost::shared_ptr<AttributeBossList> toApply (new AttributeBossList());
				toApply->ApplyAttribute(kernAttr);
				InterfacePtr<ICommand> applyTrackKernCmd (modelCmds->ApplyCmd(0, txtModel->GetPrimaryStoryThreadSpan() - 1, toApply, kCharAttrStrandBoss));
				applyTrackKernCmd->SetUndoability(ICommand::kAutoUndo);
				CmdUtils::ProcessCommand(applyTrackKernCmd);

				frameComposer->RecomposeThruLastFrame();
				
				isOverset = Utils<ITextUtils>()->IsOverset(frameList);
				if(isOverset)
					supKerningValue = currentKerningValue;

				else
				{
					infKerningValue = currentKerningValue;
					
					diff = lastFrameBottom - this->GetFrameLastLineAbsolutePosition(waxStrand, lastTxtFrameCol) - strokeWeight;
				}

				currentKerningValue = (infKerningValue  + supKerningValue) /2;
				
			} while(currentKerningValue != infKerningValue || isOverset);

			status = kSuccess;

		}
				
	}while(false);


	if (seq != nil)
	{
		if(vjFrameRef != UIDRef::gNull && vj !=  Text::kVJInvalid)
		{
			// Reset vertical justification setting to its original value
			InterfacePtr<ICommand> vertJustCmd(CmdUtils::CreateCommand(kSetFrameVertJustifyCmdBoss)); 
			vertJustCmd->SetItemList(UIDList(vjFrameRef));

			InterfacePtr<IIntData> vertJustCmdIntData(vertJustCmd, UseDefaultIID()); 
			vertJustCmdIntData->Set(vj);
			CmdUtils::ProcessCommand(vertJustCmd);
		}

		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurItem(ref);
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);		
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);
}

bool16 Func_AW::ApplyScaleFactorToLeading(ITextModel * txtModel, PMReal yFactor)
{
	TextIndex pos = 0;
	int32 runLength = 0;

	InterfacePtr<ITextModelCmds> modelCmds(txtModel, UseDefaultIID()); 
	InterfacePtr<IAttributeStrand> paraStrand (static_cast<IAttributeStrand* >(txtModel->QueryStrand(kParaAttrStrandBoss, IID_IATTRIBUTESTRAND)));
	InterfacePtr<IAttributeStrand> charStrand (static_cast<IAttributeStrand* >(txtModel->QueryStrand(kCharAttrStrandBoss, IID_IATTRIBUTESTRAND)));		

	IDataBase * db = ::GetDataBase(txtModel);
	K2Vector<ICommand *> cmdPool;

	while(pos < txtModel->GetPrimaryStoryThreadSpan() - 1)
	{
		DataWrapper<AttributeBossList> localCharOverrides = charStrand->GetLocalOverrides(pos, &runLength);
		// Check whether lead attribute is overriden
		InterfacePtr<ITextAttrRealNumber>  scaleLead (nil);				
		if(localCharOverrides.get()->ContainsAttribute(kTextAttrLeadBoss))
		{
			scaleLead = InterfacePtr<ITextAttrRealNumber> ((ITextAttrRealNumber *)localCharOverrides.get()->QueryByClassID(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER ));					
		}
		else
		{
			// Get lead attribute value from styles (check character style before, then paragraph style
			InterfacePtr<ITextAttributes> txtAttr (db, charStrand->GetStyleUID(pos, &runLength), UseDefaultIID());
			scaleLead = InterfacePtr<ITextAttrRealNumber> ((ITextAttrRealNumber *)txtAttr->QueryByClassID(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER ));
			if(!scaleLead)
			{
				InterfacePtr<ITextAttributes> paraTxtAttr (db, paraStrand->GetStyleUID(pos, nil), UseDefaultIID());
				InterfacePtr<IStyleInfo> styleInfo (paraTxtAttr, UseDefaultIID());

				scaleLead = InterfacePtr<ITextAttrRealNumber> ((ITextAttrRealNumber *)paraTxtAttr->QueryByClassID(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER ));

				while(!scaleLead) // Look in paragraph style hierarchy while not found
				{							
					styleInfo = InterfacePtr<IStyleInfo> (db, styleInfo->GetBasedOn(), UseDefaultIID());
					paraTxtAttr = InterfacePtr<ITextAttributes> (styleInfo, UseDefaultIID());
					scaleLead = InterfacePtr<ITextAttrRealNumber> ((ITextAttrRealNumber *)paraTxtAttr->QueryByClassID(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER ));
				}
			}					
		}

		if(scaleLead == nil)
		{
			// Should never occur !
			break;
		}

		if(scaleLead->Get() != -1)
		{
			InterfacePtr<ITextAttrRealNumber>  newScaleLead (::CreateObject2<ITextAttrRealNumber>(kTextAttrLeadBoss));
			newScaleLead->Set(scaleLead->Get() * yFactor);

			// Apply new leading value to the paragrah
			boost::shared_ptr<AttributeBossList> toApply (new AttributeBossList());
			toApply->ApplyAttribute(newScaleLead);
			InterfacePtr<ICommand> applyLeadCmd (modelCmds->ApplyCmd(pos, runLength, toApply, kCharAttrStrandBoss));
			applyLeadCmd->SetUndoability(ICommand::kAutoUndo);		
			cmdPool.push_back(applyLeadCmd.forget());
		}
		else
			break;			
			
		pos += runLength;
	}			
	
	if(pos >= txtModel->GetPrimaryStoryThreadSpan() - 1)
	{
		for(int32 i = 0 ; i < cmdPool.size() ; ++i)
		{
			CmdUtils::ProcessCommand(cmdPool[i]);
			cmdPool[i]->Release();
		}
		return kFalse; // No autoleading attribute found, scaling has been processed successfully
	}
	else
	{
		for(int32 i = 0 ; i < cmdPool.size() ; ++i)
			cmdPool[i]->Release();
	
		return kTrue; // don't process any command when autoleading attribute is found
	}

	
}

PMReal Func_AW::GetFrameLastLineAbsolutePosition(IWaxStrand * waxStrand, ITextFrameColumn * frame)
{		
	K2::scoped_ptr<IWaxIterator> waxIterator (waxStrand->NewWaxIterator());

	IWaxLine * lastLine = waxIterator->GetFirstWaxLine(frame->TextStart() + frame->TextSpan()-1);
	InterfacePtr<IWaxLineShape> waxLineShape(lastLine, UseDefaultIID());

	PMLineSeg lineSeg;
	waxLineShape->GetSelectionLine(&lineSeg);

	PMPoint lineBottomPoint = lineSeg.BottomPoint();
	lastLine->GetToPasteboardMatrix().Transform(&lineBottomPoint);

	return lineBottomPoint.Y();
}

PMReal Func_AW::ComputeTotalLeading(IWaxStrand * waxStrand)
{
	PMReal totalLeading = 0.0;

	K2::scoped_ptr<IWaxIterator> waxIterator (waxStrand->NewWaxIterator());
	IWaxLine * line = waxIterator->GetFirstWaxLine(0);
	while(line)
	{	
		totalLeading += line->GetLineHeight();
		line = waxIterator->GetNextWaxLine();
	}

	return totalLeading;
}

/* Constructor
*/
Func_MI::Func_MI(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_MI::Func_MI -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		ref = reader->ExtractTCLRef(kCurrentTCLRef);
		type = reader->ExtractInt(0,0,2);
		corner = reader->ExtractInt(0,0,5);
		initialWidth = reader->ExtractReel('D','+',0,0);
		initialHeight = reader->ExtractReel('D','+',0,0);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error) 
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_MI(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

void Func_MI::IDCall_MI(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();
		
		// Check whether the frame is an image frame which is not empty
		if(ref == kCurrentTCLRef)
			ref = parser->getCurItem();
		
		UID refUID = parser->getItem(ref);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IHierarchy> itemHier (db, refUID, UseDefaultIID());

		int32 textFlags;
		if(Utils<IFrameUtils>()->IsTextFrame(itemHier, &textFlags))
		{
			// It might be an image frame which was converted in text frame because the link was missing during TCL execution
			// So don't consider it as an error
			status = kSuccess;
			break;
		}

		if(itemHier->GetChildCount() == 0)
		{
			// OK, No process has to be done
			status = kSuccess;
			break;
		}

		// Get stroke width if any
		PMReal strokeWeight = 0;
		InterfacePtr<IGraphicStyleDescriptor> graphicDesc (itemHier, UseDefaultIID());
		Utils<IGraphicAttributeUtils>()->GetVisibleStrokeWeight(strokeWeight,graphicDesc);

		// Get geometry of both the image frame and its container
		InterfacePtr<IGeometry> imageGeo (db, itemHier->GetChildUID(0), UseDefaultIID());
		InterfacePtr<IGeometry> itemGeo (itemHier, UseDefaultIID());
		
		// Convert to same coordinate space to compute differences
		PMRect itemBBox = itemGeo->GetStrokeBoundingBox();
		PMRect imageBBox =  imageGeo->GetStrokeBoundingBox();
		
		::TransformInnerRectToPasteboard(itemGeo, &itemBBox);
		::TransformInnerRectToPasteboard(imageGeo, &imageBBox);		
	
		PMPoint delta(kZeroPoint);
		if(type == 2)
		{
			// Adjust image offset according the previous dimension of the frame			
			switch(corner)
			{
				case 0 : break;
				case 1 : delta.X() = itemBBox.Width() - initialWidth; break;
				case 2 : delta.Y() = itemBBox.Height() - initialHeight; break;
				case 3 : 
					delta.X() = itemBBox.Width() - initialWidth;
					delta.Y() = itemBBox.Height() - initialHeight;
					break;	
				case 4 : 
					delta.X() = (itemBBox.Width() - initialWidth)/2.0;
					delta.Y() = (itemBBox.Height() - initialHeight)/2.0;
					break;

				case 5 : // Special case : adjust left top corner offset to (0,0)
					delta = itemBBox.LeftTop() - imageBBox.LeftTop();
				default:
					break;
			}					
			
			// Just move image, no resize
			ErrorCode moveImageCmd = Utils<Facade::ITransformFacade>()->TransformItems( UIDList(imageGeo), Transform::PasteboardCoordinates(), kZeroPoint, 
					 Transform::TranslateBy(delta.X(), delta.Y()));
			
			if(moveImageCmd != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}						
		}
		else 
		{
			PMRect initialItemBBox = itemBBox;
			initialItemBBox.Right() = itemBBox.Left() + initialWidth;
			initialItemBBox.Bottom() = itemBBox.Top() + initialHeight;

			// Compute ratio
			PMReal coefficientX = itemBBox.Width()/ initialWidth;
			PMReal coefficientY = itemBBox.Height()/ initialHeight;			
			
			// Compute biggest ratio		
			PMReal biggestRatio = 1;
			if(coefficientY >= coefficientX)				
				biggestRatio = coefficientY;
			else
				biggestRatio = coefficientX;	

			PMPoint newAnchor(kZeroPoint), initialAnchor(kZeroPoint), initialOffset(kZeroPoint);

			if(type == 1) // Adjust image proportionnally using biggest ratio
			{											
				switch(corner)
				{
					case 0 : 		
						initialAnchor = imageBBox.LeftTop();
						initialOffset = imageBBox.LeftTop() - initialItemBBox.LeftTop();
						newAnchor = itemBBox.LeftTop()+ (biggestRatio * initialOffset);					
						break;
					case 1 : 
						initialAnchor = imageBBox.RightTop();
						initialOffset = imageBBox.RightTop() - initialItemBBox.RightTop();									
						newAnchor = itemBBox.RightTop() + (biggestRatio * initialOffset);					
						break;
					case 2 : 
						initialAnchor = imageBBox.LeftBottom();
						initialOffset = imageBBox.LeftBottom() - initialItemBBox.LeftBottom();
						newAnchor = itemBBox.LeftBottom() + (biggestRatio * initialOffset);					
						break;
					case 3 : 
						initialAnchor = imageBBox.RightBottom();
						initialOffset = imageBBox.RightBottom() - initialItemBBox.RightBottom();
						newAnchor = itemBBox.RightBottom() + (biggestRatio * initialOffset);					
						break;	
					case 4 : 
						initialAnchor = imageBBox.GetCenter();
						initialOffset = imageBBox.GetCenter() - initialItemBBox.GetCenter();					
						newAnchor = itemBBox.GetCenter() + (biggestRatio * initialOffset);					
						break;

					case 5 : // Special case : adjust left top corner offset to (0,0)
						initialAnchor = imageBBox.LeftTop();
						newAnchor = itemBBox.LeftTop();
					default:
						break;
				}	
				

				delta = newAnchor - initialAnchor;
				
				ErrorCode moveImageCmd = Utils<Facade::ITransformFacade>()->TransformItems( UIDList(imageGeo), Transform::PasteboardCoordinates(), kZeroPoint, 
					 Transform::TranslateBy(delta.X(), delta.Y()));
			
				if(moveImageCmd != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}						
				
				if(corner == 5) // Special case : fit to frame proportionnaly so that there is no blank left after transformation
				{
					if(this->FitImageInFrame(imageGeo, itemBBox, strokeWeight, 0, kTrue) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}

					// Center frame in view
					InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
					if(itemTransform->CenterContentCmd(::GetUIDRef(imageGeo)) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}
				}
				else
				{											
					ReferencePointLocation refPoint;
					refPoint.referencePoint = newAnchor;

					ErrorCode resizeCmd =  Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(imageGeo), Transform::PasteboardCoordinates(), 
																						  Geometry::OuterStrokeBounds(), refPoint.referencePoint, 
																						  Geometry::MultiplyBy(biggestRatio, biggestRatio));					

					if(resizeCmd != kSuccess)			
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}	
				}
				
			} else // Type == 0 : resize image so that it fits within its container
			{
				PMPoint refPointBeforeMI(kZeroPoint), refPointAfterMI(kZeroPoint); 
				switch(corner)
				{
					case 0 : 		
						initialAnchor = imageBBox.RightBottom();			
						newAnchor = itemBBox.RightBottom();
						refPointBeforeMI = initialItemBBox.LeftTop();
						refPointAfterMI = itemBBox.LeftTop();
						break;
					case 1 : 
						initialAnchor = imageBBox.LeftBottom();			
						newAnchor = itemBBox.LeftBottom();		
						refPointBeforeMI = initialItemBBox.RightTop();
						refPointAfterMI = itemBBox.RightTop();
						break;
					case 2 : 
						initialAnchor = imageBBox.RightTop();			
						newAnchor = itemBBox.RightTop();
						refPointBeforeMI = initialItemBBox.LeftBottom();
						refPointAfterMI = itemBBox.LeftBottom();
						break;
					case 3 : 
						initialAnchor = imageBBox.LeftTop();			
						newAnchor = itemBBox.LeftTop();			
						refPointBeforeMI = initialItemBBox.RightBottom();
						refPointAfterMI = itemBBox.RightBottom();
						break;

					case 4 : 
						initialAnchor = imageBBox.GetCenter();
						initialOffset = imageBBox.GetCenter() - initialItemBBox.GetCenter();					
						newAnchor.X() = itemBBox.GetCenter().X() + (coefficientX * initialOffset.X());				
						newAnchor.Y() = itemBBox.GetCenter().Y() + (coefficientY * initialOffset.Y());
						break;

					case 5 : // Special case : adjust left top corner offset to (0,0)
						initialAnchor = imageBBox.LeftTop();
						newAnchor = itemBBox.LeftTop();
					default:
						break;
				}	

				delta = newAnchor - initialAnchor;
				
				ErrorCode moveImageCmd = Utils<Facade::ITransformFacade>()->TransformItems( UIDList(imageGeo), Transform::PasteboardCoordinates(), kZeroPoint, 
					 Transform::TranslateBy(delta.X(), delta.Y()));
			
				if(moveImageCmd != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}					

				if(corner == 4) // Handle center case : resize image using the same ratio as its container
				{
					ReferencePointLocation refPoint;
					refPoint.referencePoint = newAnchor;
					ErrorCode resizeCmd =  Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(imageGeo), Transform::PasteboardCoordinates(), 
						Geometry::OuterStrokeBounds(), refPoint.referencePoint, Geometry::MultiplyBy(coefficientX, coefficientY));					

					if(resizeCmd != kSuccess)			
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}	
				}
				else if(corner == 5)
				{							
					if(this->FitImageInFrame(imageGeo, itemBBox, strokeWeight, 0, kFalse) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}
				}
				else
				{
					// Compute ratio for image resizing so that the reference point is at the same position after resizing
					coefficientX = (refPointAfterMI.X() - newAnchor.X()) / (refPointBeforeMI.X() - initialAnchor.X());
					coefficientY = (refPointAfterMI.Y() - newAnchor.Y()) / (refPointBeforeMI.Y() - initialAnchor.Y());
					
					// Resize image
					ReferencePointLocation refPoint;
					refPoint.referencePoint = newAnchor;
					ErrorCode resizeCmd =  Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(imageGeo), Transform::PasteboardCoordinates(), 
						Geometry::OuterStrokeBounds(), refPoint.referencePoint, Geometry::MultiplyBy(coefficientX, coefficientY));					

					if(resizeCmd != kSuccess)			
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}							
				}
			}
		}

		status = kSuccess;

	}while(false);

	if (status == kSuccess)
		parser->setCurItem(ref);
	else
		throw TCLError(error);
}

ErrorCode Func_MI::FitImageInFrame(IGeometry * imageGeo, const PBPMRect& frameBBox, const PMReal& strokeWeight, const int8& anchor, const bool16& proportional)
{
	ErrorCode status = kFailure;
	do
	{
		PMReal xOffset = 0, yOffset = 0;

		// Get up to date image bounding box
		PMRect imageBBox =  imageGeo->GetStrokeBoundingBox();			
		::TransformInnerRectToPasteboard(imageGeo, &imageBBox);		

		// Compute offsets for image resizing
		ReferencePointLocation refPoint;			
		switch(anchor)
		{
			case 0 : 
				refPoint.referencePoint = imageBBox.LeftTop();
				xOffset = frameBBox.Right() - imageBBox.Right() - strokeWeight;
				yOffset = frameBBox.Bottom() - imageBBox.Bottom() - strokeWeight;
				break;

			case 1 : 
				refPoint.referencePoint = imageBBox.RightTop();
				xOffset = imageBBox.Left() - frameBBox.Left() - strokeWeight;
				yOffset = frameBBox.Bottom() - imageBBox.Bottom() - strokeWeight;
				break;

			case 2 : 
				refPoint.referencePoint = imageBBox.LeftBottom();
				xOffset = frameBBox.Right() - imageBBox.Right() - strokeWeight;
				yOffset = imageBBox.Top() - frameBBox.Top() - strokeWeight;
				break;

			case 3 : 
				refPoint.referencePoint = imageBBox.RightBottom();		
				xOffset = imageBBox.Left() - frameBBox.Left() - strokeWeight;
				yOffset = imageBBox.Top() - frameBBox.Top() - strokeWeight;
				break;			

			default:
				break;
		}

		if(proportional) // Proportionnal resizing : we use biggest ratio to resize the image so there is no blank after transformation ("full proportional fit")
		{
			PMReal imageWidth = (imageBBox.Right() - imageBBox.Left()); //imageBBox.Width() is not the real image's height if its scale is not 100 %
			PMReal imageHeight = (imageBBox.Bottom() - imageBBox.Top()); // same for height

			if(xOffset >= yOffset)
			{
				PMReal xFactor =  (imageWidth+ xOffset) / imageWidth;
				yOffset = (imageHeight * xFactor) - imageHeight;
			}
			else
			{
				PMReal yFactor =  (imageHeight+ yOffset) / imageHeight;
				xOffset = (imageWidth * yFactor) - imageWidth;
			}
		}

		// Resize image
		ErrorCode resizeCmd =  Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(imageGeo), Transform::PasteboardCoordinates(), 
				Geometry::OuterStrokeBounds(), refPoint.referencePoint, Geometry::AddTo(xOffset, yOffset));

		if(resizeCmd != kSuccess)
			break;		

		status = kSuccess;

	}while(kFalse);		

	return status;
}

/* Constructor
*/
Func_IB::Func_IB(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_IB::Func_IB -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		bleed = reader->ExtractReel3('+','N',5,0, 0,200);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error) 
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_IB(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_IB
*/
void Func_IB::IDCall_IB(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error(kErrNilInterface);

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		// Go through each items (except those that are in masters hierarchy)
		InterfacePtr<ISpreadList> spreadList (parser->getCurDoc(), UseDefaultIID());
		
		int32 nbSpreads = spreadList->GetSpreadCount();
		for(int32 i = 0 ; i < nbSpreads ; ++i)
		{
			InterfacePtr<ISpread> spread (db, spreadList->GetNthSpreadUID(i), UseDefaultIID());
			int32 nbPages = spread->GetNumPages();
			for(int32 j = 0 ; j < nbPages ; ++j)
			{
				UIDList itemsOnPage(db);
				spread->GetItemsOnPage(j,&itemsOnPage,kFalse,kFalse);
				
				// Detect graphic item that are in page border
				InterfacePtr<IGeometry> pageGeo (spread->QueryNthPage(j));
				
				for(int32 itemIndex = 0 ; itemIndex < itemsOnPage.Length() ; ++itemIndex)
				{
					InterfacePtr<IHierarchy> itemHier (itemsOnPage.GetRef(itemIndex), UseDefaultIID());
					if(Utils<IFrameUtils>()->IsTextFrame(itemHier)) // Not interested in text frames
						continue;

					InterfacePtr<IGraphicStyleDescriptor> graphicDesc (itemHier, UseDefaultIID());

					// Check whether a stroke color is applied
					UID colorUID = kInvalidUID;
					Utils<IGraphicAttributeUtils>()->GetStrokeRenderingUID(colorUID,graphicDesc);

					if(colorUID != Utils<ISwatchUtils>()->GetNoneSwatchUID(db))
						continue;

					if(itemHier->GetChildCount() == 0)
					{
						// This is an empty graphic frame, check whether a fill color is applied
						colorUID = kInvalidUID;
						Utils<IGraphicAttributeUtils>()->GetFillRenderingUID(colorUID,graphicDesc);
						if(colorUID == Utils<ISwatchUtils>()->GetNoneSwatchUID(db))
							continue;
					}

					// Check whether the frame is in the document border
					PMRect pageBorders = pageGeo->GetStrokeBoundingBox();
					::TransformInnerRectToPasteboard(pageGeo, &pageBorders);

					InterfacePtr<IGeometry> itemGeo (itemHier, UseDefaultIID());
					PMRect itemBorders = itemGeo->GetStrokeBoundingBox();
					::TransformInnerRectToPasteboard(itemGeo, &itemBorders);

					ReferencePointLocation refPoint;
					if(pageBorders.Top() == itemBorders.Top())
					{
						refPoint.referencePoint = itemBorders.LeftBottom();
						Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(itemGeo), Transform::PasteboardCoordinates(), 
										Geometry::PathBounds(), refPoint.referencePoint, Geometry::AddTo(Geometry::KeepCurrentValue(), bleed));
						
						itemBorders.Top() -= bleed;
					}

					if(pageBorders.Bottom() == itemBorders.Bottom())
					{
						refPoint.referencePoint = itemBorders.LeftTop();
						Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(itemGeo), Transform::PasteboardCoordinates(), 
										Geometry::PathBounds(), refPoint.referencePoint, Geometry::AddTo(Geometry::KeepCurrentValue(), bleed));
						itemBorders.Bottom() += bleed;
					}

					if((pageBorders.Left() == itemBorders.Left()) && (j == 0))
					{
						refPoint.referencePoint = itemBorders.RightTop();
						Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(itemGeo), Transform::PasteboardCoordinates(), 
										Geometry::PathBounds(), refPoint.referencePoint, Geometry::AddTo(bleed,Geometry::KeepCurrentValue()));
						itemBorders.Left() -= bleed;
					}

					if((pageBorders.Right() == itemBorders.Right()) && (j == nbPages-1))
					{
						refPoint.referencePoint = itemBorders.LeftTop();
						Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(itemGeo), 
							                                           Transform::PasteboardCoordinates(), 
																	   Geometry::PathBounds(), 
																	   refPoint.referencePoint, 
																	   Geometry::AddTo(bleed,Geometry::KeepCurrentValue()));
					}
				}		
			}						
		}

		status = kSuccess;

	}while(false);


	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);		
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);
}


/* Constructor
*/
Func_AS::Func_AS(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)		
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();

		ref = reader->ExtractTCLRef(kCurrentTCLRef);
		threshold = reader->ExtractReel3('+','D',0,0,1,1000);
		minSize = reader->ExtractReel2('+',6,0,MIN_FONTSIZE,MAX_FONTSIZE);
		maxSize = reader->ExtractReel2('+',72,0,minSize, MAX_FONTSIZE);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error) 
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_AS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_AS
*/
void Func_AS::IDCall_AS(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error(kErrNilInterface);

	Text::VerticalJustification vj = Text::kVJInvalid;
	UIDRef vjFrameRef = UIDRef::gNull;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();
		
		// Check whether the frame is a text frame which is not overset
		if(ref == kCurrentTCLRef)
			ref = parser->getCurItem();
		
		UID refUID = parser->getItem(ref);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IHierarchy> itemHier (db, refUID, UseDefaultIID());
		if(!Utils<IFrameUtils>()->IsTextFrame(itemHier))
		{
			error=PMString(kErrInvalidItem);
			break;
		}		
				
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		// Get text model		
		InterfacePtr<IMultiColumnTextFrame> txtFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
		InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());
		
		// Get last frame of story position
		InterfacePtr<IFrameList> frameList (txtModel->QueryFrameList());
		InterfacePtr<IWaxStrand> waxStrand (frameList, UseDefaultIID());
		InterfacePtr<ITextFrameColumn> lastTxtFrameCol (frameList->QueryNthFrame(frameList->GetFrameCount()-1));	
		InterfacePtr<IHierarchy> lastSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(lastTxtFrameCol));

		// Temporary remove vertical justification for the adjustement process		
		InterfacePtr<IMultiColumnTextFrame> lastMulticol (db, lastSpline->GetChildUID(0), UseDefaultIID());
		
		
		// Compute difference between last line position and frame bottom
		// Recompose thru the frame so that we get a valid waxline
		InterfacePtr<IFrameListComposer> frameComposer (lastTxtFrameCol, UseDefaultIID());
		frameComposer->RecomposeThruLastFrame();


	    if(! Utils<ITextUtils>()->IsOverset(frameList)){
			status=kSuccess;
			break;
		}
		
		vj = lastMulticol->GetVerticalJustification();
		vjFrameRef = ::GetUIDRef(lastMulticol);

		InterfacePtr<ICommand> vertJustCmd(CmdUtils::CreateCommand(kSetFrameVertJustifyCmdBoss)); 
		vertJustCmd->SetItemList(UIDList(vjFrameRef));

		InterfacePtr<IIntData> vertJustCmdIntData(vertJustCmd, UseDefaultIID()); 
		vertJustCmdIntData->Set(Text::kVJTop);
			
		if(CmdUtils::ProcessCommand(vertJustCmd) != kSuccess)	
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Get max font size used in this story
		PMReal currentMaxSize =0; 
		this->GetFontSizeBounds(txtModel, nil, &currentMaxSize);
		PMReal maxSizeValue = maxSize, minSizeValue = minSize;

		PMReal newMaxSize = (minSizeValue + maxSizeValue)/2;
		
		bool16 isOverset = kFalse;
		do
		{									
			// Compute factor
			PMReal factor =  newMaxSize/currentMaxSize;

			this->ApplyScaleFactorToFontSize(txtModel, factor);
			frameComposer->RecomposeThruLastFrame();

			currentMaxSize = newMaxSize;
			
			isOverset = Utils<ITextUtils>()->IsOverset(frameList);
			if(isOverset)
			
				maxSizeValue = newMaxSize;
			else		
				minSizeValue = newMaxSize;											

			newMaxSize = (minSizeValue  + maxSizeValue)/2;
			
		} while( (maxSizeValue-minSizeValue) >= 0.2);

		if(isOverset)
		{
		
			// Try to resize the smallest value found
			this->ApplyScaleFactorToFontSize(txtModel, minSizeValue/currentMaxSize);
			frameComposer->RecomposeThruLastFrame();

			if(Utils<ITextUtils>()->IsOverset(frameList) && threshold != 0)
			{
				
				error = PMString(kErrASCannotAdjust);
				break;
			}
		}
	
		if(threshold == 0 )
		{
		
			// Compute difference between frame bottom and last line, then check whether the threshold is respected
			InterfacePtr<IGeometry> lastSplineGeo (lastSpline, UseDefaultIID());
			PMReal strokeWeight = lastSplineGeo->GetStrokeBoundingBox().Bottom() - lastSplineGeo->GetPathBoundingBox().Bottom();
		
			PMRect bbox = lastSplineGeo->GetStrokeBoundingBox();
			::TransformInnerRectToPasteboard(lastSplineGeo, &bbox);
	
	
	
			PMReal lastFrameBottom = bbox.Bottom();
	
			if( (lastFrameBottom - Func_AW::GetFrameLastLineAbsolutePosition(waxStrand, lastTxtFrameCol) - strokeWeight) >= threshold)
			{
				error = PMString(kErrASCannotAdjust);
				break;
			}		
	
			// Finally check whether inferior bound is respected too
			PMReal newMinSize = 0;
			this->GetFontSizeBounds(txtModel, &newMinSize, nil);
	
			if(newMinSize < minSize)
			{
				error = PMString(kErrASCannotAdjust);
				break;
	
			}
		}
		
		status = kSuccess;
	
	}while(false);


	if (seq != nil)
	{
		if(vjFrameRef != UIDRef::gNull && vj !=  Text::kVJInvalid)
		{
			// Reset vertical justification setting to its original value
			InterfacePtr<ICommand> vertJustCmd(CmdUtils::CreateCommand(kSetFrameVertJustifyCmdBoss)); 
			vertJustCmd->SetItemList(UIDList(vjFrameRef));

			InterfacePtr<IIntData> vertJustCmdIntData(vertJustCmd, UseDefaultIID()); 
			vertJustCmdIntData->Set(vj);
			CmdUtils::ProcessCommand(vertJustCmd);
		}

		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurItem(ref);
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);		
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);
}

void Func_AS::ApplyScaleFactorToFontSize(ITextModel * txtModel, PMReal factor)
{
	InterfacePtr<ITextModelCmds> modelCmds(txtModel, UseDefaultIID()); 

	InDesign::TextRange txtRange (txtModel, 0,  txtModel->GetPrimaryStoryThreadSpan()-1);

	// Use TextAttributeRunIterator to find our kTextAttrPointSizeBoss
	K2Vector<InDesign::TextRange> textRanges;
	textRanges.push_back(txtRange);

	K2Vector<ClassID> attributeClasses;
	attributeClasses.push_back(kTextAttrPointSizeBoss);

	// Construct the iterator.
	TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());	
	while(runIter)
	{
		RangeData range = runIter.GetRunRange();

		InterfacePtr<ITextAttrRealNumber> currentFontSize ((ITextAttrRealNumber *)runIter->QueryByClassID(kTextAttrPointSizeBoss, IID_ITEXTATTRREALNUMBER ));
		
		if(currentFontSize != nil)
		{
			InterfacePtr<ITextAttrRealNumber>  newFontSize (::CreateObject2<ITextAttrRealNumber>(kTextAttrPointSizeBoss));
			newFontSize->Set(currentFontSize->Get() * factor);

			// Apply new font size value to the paragrah
			boost::shared_ptr<AttributeBossList> toApply (new AttributeBossList());
			toApply->ApplyAttribute(newFontSize);
			InterfacePtr<ICommand> applyFontSizeCmd (modelCmds->ApplyCmd(range, toApply, kCharAttrStrandBoss));			
			CmdUtils::ProcessCommand(applyFontSizeCmd);
		}
		++runIter;
	}					
}

void Func_AS::GetFontSizeBounds(ITextModel * txtModel, PMReal * minSize, PMReal * maxSize)
{
	if(minSize)
		*minSize = MAX_FONTSIZE;

	if(maxSize)
		*maxSize = MIN_FONTSIZE;

	InDesign::TextRange txtRange (txtModel, 0,  txtModel->GetPrimaryStoryThreadSpan()-1);

	// Use TextAttributeRunIterator to find our kTextAttrPointSizeBoss
	K2Vector<InDesign::TextRange> textRanges;
	textRanges.push_back(txtRange);

	K2Vector<ClassID> attributeClasses;
	attributeClasses.push_back(kTextAttrPointSizeBoss);

	// Construct the iterator.
	TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());	
	while(runIter)
	{
		RangeData range = runIter.GetRunRange();

		InterfacePtr<ITextAttrRealNumber> currentFontSize ((ITextAttrRealNumber *)runIter->QueryByClassID(kTextAttrPointSizeBoss, IID_ITEXTATTRREALNUMBER ));
		
		if(currentFontSize != nil)
		{
			InterfacePtr<ITextAttrRealNumber>  newFontSize (::CreateObject2<ITextAttrRealNumber>(kTextAttrPointSizeBoss));
			if(maxSize && currentFontSize->Get() > *maxSize)
				*maxSize = currentFontSize->Get();
			
			if(minSize && currentFontSize->Get() < *minSize)
				*minSize = currentFontSize->Get();
		}
		++runIter;
	}	
}