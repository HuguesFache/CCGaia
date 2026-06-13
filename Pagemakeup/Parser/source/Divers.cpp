/*
//	File:	Divers.cpp
//
//	Date:	24-Apr-2003
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"
#include "ILayoutUtils.h"
#include "ErrorUtils.h"

// Project includes
#include "GlobalDefs.h"
#include "Divers.h"
#include "Blocs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "IItemManager.h"
#include "IItemTransform.h"
#include "ITextFrameHelper.h"
#include "TCLError.h"
#include "TransformUtils.h"
#include "DocFrameworkID.h"
#include "PrintID.h"
#include "FileUtils.h"
#include "TextAttributeRunIterator.h"
#include "IReverseTCL.h"
#include "ExportUtils.h"
#include "PDFID.h"

//Interfaces Includes
#include "IGeometry.h"
#include "IPageItemUtils.h"
#include "ITransform.h"
#include "IDocument.h"
#include "IDocumentLayer.h"
#include "IPageList.h"
#include "ILayerUtils.h"
#include "ILayoutControlData.h"
#include "IExportProvider.h"
#include "ICoreFilename.h"
#include "IEpsDocumentName.h"
#include "IFrameList.h"
#include "ITextUtils.h"
#include "IMultiColumnTextFrame.h"
#include "IFrameListComposer.h"
#include "IFrameUtils.h"
#include "IHierarchy.h"
#include "ITextModelCmds.h"
#include "IOwnedItem.h"
#include "IPageItemTypeUtils.h"
#include "ITextParcelListComposer.h"
#include "ITextAttrRealNumber.h"
#include "ISpread.h"
#include "IXPAttributeFacade.h"
#include "GraphicTypes.h"
#include "IXPAttributeSuite.h"
#include "IEPSExportPreferences.h"
#include "IStringData.h"
#include "IGraphicAttrRealNumber.h"
#include "IGraphicAttrPoint.h"
#include "IGraphicAttributeUtils.h"
#include "IApplyMultAttributesCmdData.h"
#include "IGradientFill.h"
#include "IGraphicStyleDescriptor.h"
#include "IMasterSpreadList.h"
#include "ISpreadList.h"
#include "IApplyMasterCmdData.h"
#include "IMasterPage.h"
#include "IOverrideMasterPageItemData.h"
#include "IMasterSpread.h"
#include "IFrameUtils.h"
#include "IPageCmdData.h"
#include "IControllingPageItem.h"
#include "IMasterSpreadUtils.h"
#include "IAutoFlowCmdData.h"
#include "IDataLinkReference.h"
#include "IItemStrand.h"
#include "ILayerList.h"
#include "IGuidePrefs.h"

#include "IWorkspace.h"
#include "IGenStlEdtListMgr.h"

/* Constructor
*/
Func_DU::Func_DU(ITCLParser *  parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DU::Func_DU -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		Unite = reader->ExtractUnite();
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
		IDCall_DU(reader);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_DU::~Func_DU()
{
}

/* IDCall_DU
*/
void Func_DU::IDCall_DU(ITCLReader * reader)
{
	reader->SetUniteCourante(Unite);
}

/* Constructor
*/
Func_AN::Func_AN(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_AN::Func_AN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->FiltreBlanc();
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
		IDCall_AN(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_AN::~Func_AN()
{
}


/* IDCall_AN
*/
void Func_AN::IDCall_AN(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_AN::IDCall_AN-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		parser->InsertCharacter(kTextChar_PageNumber);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}




/* Constructor
*/
Func_CS::Func_CS(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CS::Func_CS -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		table = reader->ExtractInt(0,MIN_TABLE,MAX_TABLE);
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
		IDCall_CS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_CS::~Func_CS()
{
}


/* IDCall_CS
*/
void Func_CS::IDCall_CS(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		parser->setTranscodeTable(table);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_CC::Func_CC(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_C::Func_CC -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		code = reader->ExtractInt(1,MIN_CODECHAR,MAX_CODECHAR);
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
		IDCall_CC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_CC::~Func_CC()
{
}


/* IDCall_CC
*/
void Func_CC::IDCall_CC(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		parser->InsertCharacter(code);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_CM::Func_CM(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CM::Func_CM -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		//reader->MustEgal();
		//reader->ExtractString(255);
		reader->FiltreFin();
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
	if(! reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_CM::~Func_CM()
{
}


/* Constructor
*/
Func_DT::Func_DT(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DT::Func_DT -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		msg = reader->ExtractString(255);
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
		IDCall_DT(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_DT::~Func_DT()
{
}


/* IDCall_DT
*/
void Func_DT::IDCall_DT(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		msg.SetTranslatable(kFalse);
		int16 rep = CAlert::ModalAlert( msg,PMString("OK"),PMString("Cancel"),kNullString,1,CAlert::eWarningIcon) ;

		if (rep == 1) // if the user clicked on "OK" button
			status = kSuccess ;
		else
			error = PMString(kErrUserBreak);
			

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}





/* Constructor
*/
Func_EV::Func_EV(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_EV::Func_EV -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		
		
		reader->MustEgal();

		first = reader->ExtractTCLRef(kDefaultTCLRef);

		last = reader->ExtractTCLRef(kDefaultTCLRef);

		minSpace = reader->ExtractReel('P','+',0,0);

		maxSpace = reader->ExtractReel('P','+',0,0);

		maxDepth = reader->ExtractReel('P','+',0,0);

		fillingFrame = reader->ExtractBooleen(kFalse);

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
		IDCall_EV(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_EV::~Func_EV()
{
}


/* IDCall_EV
*/
void Func_EV::IDCall_EV(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_EV::IDCall_EV -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		// Get helper interfaces

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_EV::IDCall_EV -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_EV::IDCall_EV -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}
		

		IDocument * doc = parser->getCurDoc();
		IDataBase * db = parser->getCurDataBase();

		int32 nbFrame = last - first ;
		
		if (nbFrame < 1)
		{		
			ASSERT_FAIL("Func_EV::IDCall_EV -> txtAttrHelper nil");
			error = PMString(kErrNotEnoughArgument);
			break;
		}

		PMReal depth = 0 ;

		for (TCLRef i = first ; i <= last ; ++i)
		{
			UID refUID = parser->getItem(i) ;
			if (refUID == kInvalidUID)
			{
				ASSERT_FAIL("Func_EV::IDCall_EV -> UID Frame nil");
				error = PMString(kErrInvalidItem);
				break;
			}

			InterfacePtr<IGeometry> itemGeom(UIDRef(db,refUID), IID_IGEOMETRY) ;
			if (itemGeom == nil)
			{
				ASSERT_FAIL("Func_EV::IDCall_EV -> UID Frame nil");
				error = PMString(kErrNilInterface);
				break;
			}

			PMRect innerBBox = itemGeom->GetStrokeBoundingBox() ;
			depth = depth + innerBBox.Height() ;

		}
		
		PMReal amountToAdd = (maxDepth - depth ) / nbFrame ;

		//analysis of the amount to add between the frames
		//if the amount is smaller than the minimum space, we quit
		if (amountToAdd < minSpace)
		{
			error = PMString(kErrMinSpaceTooBig);
			break;
		}

		bool createFrame = kFalse ;
		
		//if the amount is bigger than the maximum space we had to recalculate it
		if (amountToAdd > maxSpace)
		{
			if (!fillingFrame)
				amountToAdd = (maxSpace+minSpace)/2 ;
			else
			{
				createFrame = kTrue ;
				nbFrame = nbFrame + 1 ;
				amountToAdd = (maxDepth - depth) / nbFrame ;

				if (amountToAdd <= minSpace)
				{
					error = PMString(kErrMinSpaceTooBig);
					break;
				}
				
				if (amountToAdd > maxSpace)
					amountToAdd = (maxSpace+minSpace)/2 ;
			}
		}

		//get the position of the first frame
		UID refUID = parser->getItem(first) ;
		if (refUID == kInvalidUID)
		{
			ASSERT_FAIL("Func_EV::IDCall_EV -> UID Frame nil");
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IGeometry> itemGeom(UIDRef(db,refUID), IID_IGEOMETRY) ;
		if (itemGeom == nil)
		{
			ASSERT_FAIL("Func_EV::IDCall_EV -> UID Frame nil");
			error = PMString(kErrNilInterface);
			break;
		}

		PMPoint pt (itemGeom->GetStrokeBoundingBox().LeftTop());
		::TransformInnerPointToPasteboard(itemGeom,&pt);

		PMReal top = pt.Y();
		PMReal pos = top + itemGeom->GetStrokeBoundingBox().Height() + amountToAdd ;
			
		PMReal xCB ;
		PMReal widthCB ;

		TCLRef start = first;
		++start;
		for ( TCLRef j = start ; j <= last ; ++j)
		{
			refUID = parser->getItem(j) ;
			if (refUID == kInvalidUID)
			{
				ASSERT_FAIL("Func_EV::IDCall_EV -> UID Frame nil");
				error = PMString(kErrInvalidItem);
				break;
			}

			InterfacePtr<IGeometry> itemGeom(UIDRef(db,refUID), IID_IGEOMETRY) ;
			if (itemGeom == nil)
			{
				ASSERT_FAIL("Func_EV::IDCall_EV -> UID Frame nil");
				error = PMString(kErrNilInterface);
				break;
			}

			PMPoint p (itemGeom->GetStrokeBoundingBox().LeftTop());
			::TransformInnerPointToPasteboard(itemGeom,&p);

			p.Y(pos);
			
			if(itemTransform->MoveCmd(UIDRef(db,refUID),p) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			pos = pos + itemGeom->GetStrokeBoundingBox().Height() +  amountToAdd ;

			xCB = p.X() ;
			widthCB = itemGeom->GetStrokeBoundingBox().Width() ; 

		}
		
		//if we have to  create a frame at the end
		if (createFrame)
		{
			PMPoint p1(xCB, pos);
			PMPoint p2(xCB+widthCB, top + maxDepth);
			PMPointList points ;
			points.push_back(p1);
			points.push_back(p2);

			InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(doc));
			if(activeLayer == nil)
			{
				ASSERT_FAIL("Func_EV::IDCall_EV -> activeLayer nil");
				error = PMString(kErrNilInterface);		
				break;
			}

			UID ownerSpreadLayer = itemManager->GetOwnerSpreadLayer(doc,parser->getCurPage(),activeLayer);
			if(ownerSpreadLayer == kInvalidUID)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			UIDRef txtFrameRef = itemManager->CreateTextFrameCmd(db,points,ownerSpreadLayer,1,1);
			if(txtFrameRef == UIDRef(nil,kInvalidUID))
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		status = kSuccess ;

	}while(false);
	
	if (seq != nil)
	{
		if (status == kSuccess)
			CmdUtils::EndCommandSequence(seq);
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
Func_EX::Func_EX(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_EX::Func_EX -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}
	
	try
	{
		reader->MustEgal();
		fileName = reader->ExtractString(255);
		eraseExisting = reader->ExtractBooleen(kTrue);
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
		IDCall_EX(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* Destructor
 */
Func_EX::~Func_EX()
{
}

/* IDCall_EX
 */
void Func_EX::IDCall_EX(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");
	
	do
	{
		// Check if there is a document open
		IDocument * doc = parser->getCurDoc();
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		if((status = ExportUtils::IDMLExport(doc, fileName, eraseExisting)) != kSuccess)
			error = ErrorUtils::PMGetGlobalErrorString();
		
	} while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_ET::Func_ET(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_ET::Func_ET -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		
		if(reader->TestEgal())
		{
			startPage = reader->ExtractInt(1,0,MAX_PAGE);
			endPage = reader->ExtractInt(MAX_PAGE,0,MAX_PAGE);		
			eraseExisting = reader->ExtractBooleen(kTrue);	

			checkMissingPict = reader->ExtractBooleen(kTrue);
			updateMissingPict = reader->ExtractBooleen(kTrue);
		}
		else
		{
			startPage = 1;
			endPage = -1;
			eraseExisting = kTrue;
			checkMissingPict = kTrue;
			updateMissingPict = kTrue;
		}
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
		IDCall_ET(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_ET::~Func_ET()
{
}


/* IDCall_ET
*/
void Func_ET::IDCall_ET(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");
	
	PMString formatName("EPS", PMString::kUnknownEncoding);

	do
	{
		IDocument * doc = parser->getCurDoc();
		IDataBase  *db = parser->getCurDataBase();
		
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		if(startPage == 0)
			startPage = parser->getCurPage();
		
		if(endPage == 0)
			endPage = parser->getCurPage();
		else if(endPage == MAX_PAGE)
		{
			InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
			if(pageList == nil)
			{
				error = PMString(kErrNilInterface);	
				break;
			}
			
			endPage = pageList->GetPageCount();
		}

		// Check missing pictures		
		bool16 linkStatus = kSuccess;
		if(checkMissingPict)
		{			
			// Iterate through all page items to pick up data link information on image frame (this is the only way to get link on embedded images)			
			InterfacePtr<ILayerList> layerList (doc, UseDefaultIID());
			int32 nbLayers = layerList->GetCount();
			int32 startContentLayer, endContentLayer;
		
			InterfacePtr<IGuidePrefs> guidePrefs (doc->GetDocWorkSpace(), UseDefaultIID());
			if(guidePrefs && guidePrefs->GetGuidesInBack())
			{			
				startContentLayer = nbLayers + 1;
				endContentLayer = (2 * nbLayers) - 1;			
			}
			else
			{			
				startContentLayer = 2;
				endContentLayer = nbLayers;		
			}
				
			// Iterate masters first
			InterfacePtr<IMasterSpreadList> masterSpreadList (doc, UseDefaultIID());

			int32 nbMasters = masterSpreadList->GetMasterSpreadCount();
			for(int32 i = 0 ; i < nbMasters && linkStatus == kSuccess ; ++i)
			{															
				// Iterator through spread layers (1st z-order level)
				InterfacePtr<IHierarchy> spreadHier (db, masterSpreadList->GetNthMasterSpreadUID(i), UseDefaultIID());				
			
				for(int32 j = startContentLayer ; j <= endContentLayer && linkStatus == kSuccess ; ++j)
				{
					InterfacePtr<IHierarchy> spreadLayerHier (db, spreadHier->GetChildUID(j), UseDefaultIID());
					int32 nbItems = spreadLayerHier->GetChildCount();
					
					// Iterate through childs (the page items) of the spread layer (2nd z-order level)
					for(int32 itemIndex = 0 ; itemIndex < nbItems ; ++itemIndex)
					{
						InterfacePtr<IHierarchy> itemHier (db, spreadLayerHier->GetChildUID(itemIndex), UseDefaultIID());
						if(!HandleItem(itemHier, parser))
						{
							linkStatus = kFailure;
							break;
						}
					}
				}				
			}
			
			// Iterate pages
			InterfacePtr<ISpreadList> spreadList (doc, UseDefaultIID());
			
			int32 nbSpreads = spreadList->GetSpreadCount();
			for(int32 i = 0 ; i < nbSpreads && linkStatus == kSuccess ; ++i)
			{							
				InterfacePtr<IHierarchy> spreadHier (db, spreadList->GetNthSpreadUID(i), UseDefaultIID());
				
				for(int32 j = startContentLayer ; j <= endContentLayer && linkStatus == kSuccess ; ++j)
				{
					InterfacePtr<IHierarchy> spreadLayerHier (db, spreadHier->GetChildUID(j), UseDefaultIID());	
					int32 nbItems = spreadLayerHier->GetChildCount();
				
					for(int32 itemIndex = 0 ; itemIndex < nbItems ; ++itemIndex)
					{
						InterfacePtr<IHierarchy> itemHier (db, spreadLayerHier->GetChildUID(itemIndex), UseDefaultIID());
						if(!HandleItem(itemHier, parser))
						{
							linkStatus = kFailure;
							break;
						}
					}
				}
			}
		}				

		if(linkStatus == kSuccess)
		{
			if((status = ExportUtils::EpsExport(doc,eraseExisting,kFalse,startPage,endPage)) != kSuccess)
				error = ErrorUtils::PMGetGlobalErrorString();
		}
		else
			status = kSuccess; // There are some missing links but we don't consider it as a blocking error

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
}

/* HandleItem
*/
bool16 Func_ET::HandleItem(IHierarchy * itemHier, ITCLParser *  parser)
{
	IDataBase * db = ::GetDataBase(itemHier);
	if(Utils<IPageItemTypeUtils>()->IsGraphicFrame(itemHier)) // case graphic frame
	{
		if(itemHier->GetChildCount() == 1)
		{						
			InterfacePtr<IHierarchy> imageHier (itemHier->QueryChild(0));

			InterfacePtr<IDataLinkReference> linkref (imageHier, UseDefaultIID());
			if(linkref && linkref->GetUID() != kInvalidUID)
			{
				InterfacePtr<IDataLink> dataLink(db, linkref->GetUID(), IID_IDATALINK);
				IDataLink::StateType linkState = dataLink->GetStoredState(nil,nil);
				if(linkState == IDataLink::kLinkMissing)
				{
					PMString linkMsg(kErrEpsMissingLink);
					linkMsg.Translate();
					parser->WriteLogFile(linkMsg);
					return kFalse;
				}
				else if(linkState == IDataLink::kLinkOutOfDate && updateMissingPict)
				{
					UIDRef imageRef = ::GetUIDRef(imageHier);

					// Reimport image, keeping scale and offset of the existing image
					InterfacePtr<IGeometry> frameGeo (itemHier, UseDefaultIID());
					InterfacePtr<IGeometry> imageFrameGeo (imageHier, UseDefaultIID());
				
					PMPoint imageLeftTop = imageFrameGeo->GetStrokeBoundingBox().LeftTop();
					PMPoint leftTop = frameGeo->GetStrokeBoundingBox().LeftTop();
					::TransformInnerPointToPasteboard(frameGeo, &leftTop);
					::TransformInnerPointToPasteboard(imageFrameGeo, &imageLeftTop);

					PMReal decH = imageLeftTop.X() - leftTop.X();
					PMReal decV = imageLeftTop.Y() - leftTop.Y();

					InterfacePtr<ITransform> imageTransform (imageFrameGeo, UseDefaultIID());
					PMReal echH = imageTransform->GetItemScaleX();
					PMReal echV = imageTransform->GetItemScaleY();
		
					InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
					InterfacePtr<IItemTransform> itemTransform (parser->QueryItemTransform());
					if(itemManager->ImportImageInGraphicFrameCmd(::GetUIDRef(itemHier),*dataLink->GetFullName(),imageRef) == kSuccess)
					{
						// Scale the image
						InterfacePtr<IGeometry> imGeo (imageHier,UseDefaultIID());		

						// The reference point is the image left top corner in image coordinate system
						if(itemTransform->Scale(imageRef,echH,echV,PMPoint(imGeo->GetStrokeBoundingBox().LeftTop())) != kSuccess)
						{
							PMString error = ErrorUtils::PMGetGlobalErrorString();
							throw TCLError(error);
						}
						
						// Calculate the new position of the left top corner, then move the image
						PMPoint leftTop (frameGeo->GetStrokeBoundingBox().LeftTop());
						PMPoint newLeftTop (leftTop.X() + decH, leftTop.Y() + decV);
						::TransformInnerPointToPasteboard(frameGeo,&newLeftTop);

						if(itemTransform->MoveCmd(imageRef,newLeftTop) != kSuccess)
						{
							PMString error = ErrorUtils::PMGetGlobalErrorString();
							throw TCLError(error);
						}
					}
				}				
				// else link state is OK or link is embedded
			}		
		}
	}
	else if(Utils<IPageItemTypeUtils>()->IsTextFrame(itemHier)) // case text frame
	{
		// Iterate through text model looking for inline frames
		InterfacePtr<IMultiColumnTextFrame> txtFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
		
		InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());
		
		InterfacePtr<IItemStrand> itemStrand ((IItemStrand *) txtModel->QueryStrand(kOwnedItemStrandBoss, IID_IITEMSTRAND));
		
		UIDList inlineItems(db);
		itemStrand->GetAllItems(0, txtModel->GetPrimaryStoryThreadSpan(), &inlineItems);
		
		int32 nbInlines = inlineItems.Length();
		for(int32 i = 0 ; i < nbInlines ; ++i)
		{
			InterfacePtr<IHierarchy> inlineHier (db, inlineItems[i], UseDefaultIID());

			InterfacePtr<IHierarchy> inlineChildHier (db, inlineHier->GetChildUID(0), UseDefaultIID());
			return HandleItem(inlineChildHier, parser);
		}
		
	}
	else if(Utils<IPageItemTypeUtils>()->IsGroup(itemHier)) // case group item
	{
		int32 nbChild = itemHier->GetChildCount();
		// Iterate through each child
		for(int32 i = 0 ; i < nbChild ; ++i)
		{
			InterfacePtr<IHierarchy> childHier (db, itemHier->GetChildUID(i), UseDefaultIID());
			return HandleItem(childHier, parser);
		}
	}

	return kTrue;
}


/* Constructor
*/
Func_EI::Func_EI(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_EI::Func_EI -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		
		reader->MustEgal();

		fileName = reader->ExtractString(255);

		fileExt = reader->ExtractString(255);
	
		format = reader->ExtractInt(1, 1, 2);
		readerSpread = reader->ExtractBooleen(kFalse);
		psLevel = reader->ExtractInt(1, 1, 2);
		colorSpace = reader->ExtractInt(1, 1, 5);
		preview = reader->ExtractInt(1, 1, 3);
		includeFonts = reader->ExtractInt(1, 1, 3);
		
		bmpSampling = reader->ExtractInt(1, 1, 2);
		
		bleed = reader->ExtractBooleen(kFalse);
		bTop = reader->ExtractReel('D','+',0,0);
		bBottom = reader->ExtractReel('D','+',0,0);
		bInside = reader->ExtractReel('D','+',0,0);
		bOutside = reader->ExtractReel('D','+',0,0);
		
		opiReplace = reader->ExtractBooleen(kFalse);
		omitPDF = reader->ExtractBooleen(kFalse);
		omitEPS = reader->ExtractBooleen(kFalse);
		omitBmp = reader->ExtractBooleen(kFalse);
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
		IDCall_EI(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_EI::~Func_EI()
{
}


/* IDCall_EI
*/
void Func_EI::IDCall_EI(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDocument * doc = parser->getCurDoc();
		
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		// Store other preferences into the document
		InterfacePtr<ICommand> exportPrefCmd (CmdUtils::CreateCommand(kPrsSetDocEpsExportPrefsCmdBoss));
		if(exportPrefCmd == nil)
		{
			error = PMString(kErrNilInterface);	
			break;
		}

		exportPrefCmd->SetItemList(UIDList(doc));

		// Store file name and file extension into the document
		InterfacePtr<IEpsDocumentName> epsDocName (exportPrefCmd, UseDefaultIID());
		if (epsDocName == nil)
		{
			error = PMString(kErrNilInterface);	
			break;
		}

		epsDocName->setEpsFileName(fileName);
		epsDocName->setEpsFileExt(fileExt);
		
		InterfacePtr<IEPSExportPreferences> exportPrefs (exportPrefCmd,UseDefaultIID());
		if(exportPrefs == nil)
		{
			error = PMString(kErrNilInterface);	
			break;
		}
		
		if(format == 1)
			exportPrefs->SetEPSExDataFormat(IEPSExportPreferences::kExportASCIIData);
		else
			exportPrefs->SetEPSExDataFormat(IEPSExportPreferences::kExportBinaryData);
		
		if(readerSpread)
			exportPrefs->SetEPSExReaderSpread(IEPSExportPreferences::kExportReaderSpreadON);
		else
			exportPrefs->SetEPSExReaderSpread(IEPSExportPreferences::kExportReaderSpreadOFF);
		
		if(psLevel == 1)
			exportPrefs->SetEPSExPSLevel(IEPSExportPreferences::kExportPSLevel2);
		else
			exportPrefs->SetEPSExPSLevel(IEPSExportPreferences::kExportPSLevel3);
		
		switch(colorSpace)
		{
			case 1 : exportPrefs->SetEPSExColorSpace(IEPSExportPreferences::kExportPSColorSpaceCMYK); break;
			case 2 : exportPrefs->SetEPSExColorSpace(IEPSExportPreferences::kExportPSColorSpaceRGB); break;
			case 3 : exportPrefs->SetEPSExColorSpace(IEPSExportPreferences::kExportPSColorSpaceGray); break;
			case 4 : exportPrefs->SetEPSExColorSpace(IEPSExportPreferences::kExportPSColorSpaceDIC); break;
			case 5 : exportPrefs->SetEPSExColorSpace(IEPSExportPreferences::kExportPSColorSpaceLeaveUnchanged); break;
			default : break;
		}
		
		switch(preview)
		{
			case 1 : exportPrefs->SetEPSExPreview(IEPSExportPreferences::kExportPreviewTIFF); break;
			case 2 : exportPrefs->SetEPSExPreview(IEPSExportPreferences::kExportPreviewNone); break;
			case 3 : exportPrefs->SetEPSExPreview(IEPSExportPreferences::kExportPreviewPICT); break;
			default : break;
		}
		
		switch(includeFonts)
		{
			case 1 : exportPrefs->SetEPSExIncludeFonts(IEPSExportPreferences::kExportIncludeFontsWhole); break;
			case 2 : exportPrefs->SetEPSExIncludeFonts(IEPSExportPreferences::kExportIncludeFontsSubset); break;
			case 3 : exportPrefs->SetEPSExIncludeFonts(IEPSExportPreferences::kExportIncludeFontsNone); break;
			default : break;
		}
		
		if(bmpSampling == 1)
			exportPrefs->SetEPSExBitmapSampling(IEPSExportPreferences::kExportBMSampleNormal);
		else
			exportPrefs->SetEPSExBitmapSampling(IEPSExportPreferences::kExportBMSampleLowRes);
		
		if(bleed)
		{
			exportPrefs->SetEPSExBleedOnOff(IEPSExportPreferences::kExportBleedON);
			exportPrefs->SetEPSExBleedTop(bTop);
			exportPrefs->SetEPSExBleedBottom(bBottom);
			exportPrefs->SetEPSExBleedInside(bInside);
			exportPrefs->SetEPSExBleedOutside(bOutside);	
		}
		else
			exportPrefs->SetEPSExBleedOnOff(IEPSExportPreferences::kExportBleedOFF);
		
		if(opiReplace)
			exportPrefs->SetEPSExOPIReplace(IEPSExportPreferences::kExportOPIReplaceON);
		else
			exportPrefs->SetEPSExOPIReplace(IEPSExportPreferences::kExportOPIReplaceOFF);
		
		if(omitPDF)
			exportPrefs->SetEPSExOmitPDF(IEPSExportPreferences::kExportOmitPDFON);
		else
			exportPrefs->SetEPSExOmitPDF(IEPSExportPreferences::kExportOmitPDFOFF);
		
		
		if(omitEPS)
			exportPrefs->SetEPSExOmitEPS(IEPSExportPreferences::kExportOmitEPSON);
		else
			exportPrefs->SetEPSExOmitEPS(IEPSExportPreferences::kExportOmitEPSOFF);
		
		
		if(omitBmp)
			exportPrefs->SetEPSExOmitBitmapImages(IEPSExportPreferences::kExportOmitBitmapImagesON);
		else
			exportPrefs->SetEPSExOmitBitmapImages(IEPSExportPreferences::kExportOmitBitmapImagesOFF);
		
		if(CmdUtils::ProcessCommand(exportPrefCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}
	
		status = kSuccess;

	}while(false);
	

	if (status != kSuccess)
	{
		throw TCLError(error);
	}		
}


/* Constructor
*/
Func_JV::Func_JV(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_JV::Func_JV -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);

		// Options for image frame justification
		justifyHorizontaly = reader->ExtractBooleen(kTrue);
		justifyVerticaly = reader->ExtractBooleen(kTrue);
		maxHor = reader->ExtractReel('P','+',0,0);
		maxVer = reader->ExtractReel('P','+',0,0);
		leftJustify = reader->ExtractBooleen(kFalse);
		topJustify = reader->ExtractBooleen(kFalse); // this parameter is also used for justifying text frame's top

		// in order JV command not to be blocking
		allowOutOfSpread = reader->ExtractBooleen(kFalse);
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
		IDCall_JV(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_JV::~Func_JV()
{
}


/* IDCall_JV
*/
void Func_JV::IDCall_JV(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	
	PMString error("");	

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IItemTransform> itemTransform (parser->QueryItemTransform());
		if(itemTransform == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		IDataBase * db = parser->getCurDataBase();

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
		
		UID frame =  parser->getItem(refBloc);

		if(frame == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDRef frameRef = UIDRef(db, frame);
		UIDRef justifiedFrame = UIDRef::gNull;

		InterfacePtr<IHierarchy> frameHier (frameRef, UseDefaultIID());
		
		// Is it a text frame ?
		if(Utils<IFrameUtils>()->IsTextFrame(frameHier))
		{
			if(itemTransform->JustifyFrameVertically(frameRef, &justifiedFrame, topJustify, allowOutOfSpread) != kSuccess) 
			{
				error = ErrorUtils::PMGetGlobalErrorString();			
				break;
			}
		}
		else
		{
			if(itemTransform->JustifyImageFrame(frameRef, justifyHorizontaly, justifyVerticaly, maxHor, maxVer, leftJustify, topJustify) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();			
				break;
			}
			justifiedFrame = frameRef;
		}

		// Select the frame which is going to be resized
		if(itemManager->SelectPageItem(justifiedFrame) != kSuccess) 
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if (status != kSuccess)
		throw TCLError(error);
}



/* Constructor
*/
Func_XX::Func_XX(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_XX::Func_XX -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
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
		IDCall_XX(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_XX::~Func_XX()
{
}


/* IDCall_XX
*/
void Func_XX::IDCall_XX(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		IDataBase * db = parser->getCurDataBase();
			
		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
		
		// Select the frame
		if(itemManager->SelectPageItem(UIDRef(db,parser->getItem(refBloc))) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		parser->setCurItem(refBloc);
		
		UIDRef multiColRef;
		
		// Get the muticol ref, if any
		if(parser->getCurBoxUIDRef(multiColRef) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
	
		InterfacePtr<IMultiColumnTextFrame> txtFrame (multiColRef, UseDefaultIID());
		if(txtFrame == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}
	
		InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());
		if(txtModel == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		if(txtModel->GetPrimaryStoryThreadSpan() - 1 == 0)
		{
			status = kSuccess;
			break;
		}
		
		InterfacePtr<ITextModelCmds> txtModelCmds (txtModel, UseDefaultIID());
		if(txtModelCmds == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}
		
		// Create a delete command and process it
		InterfacePtr<ICommand> deleteCmd (txtModelCmds->DeleteCmd(0, txtModel->GetPrimaryStoryThreadSpan() - 1));
		if(deleteCmd == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		if((status = CmdUtils::ProcessCommand(deleteCmd)) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}
		
	}while(false);
	
	if (seq != nil)
	{
		if (status == kSuccess)
			CmdUtils::EndCommandSequence(seq);
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
Func_XC::Func_XC(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_XC::Func_XC -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBlocSrc = reader->ExtractTCLRef(kCurrentTCLRef);
		refBlocDest = reader->ExtractTCLRef(kCurrentTCLRef);
		deleteText = reader->ExtractBooleen(kTrue);
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
		IDCall_XC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_XC::~Func_XC()
{
}


/* IDCall_XC
*/
void Func_XC::IDCall_XC(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		IDataBase * db = parser->getCurDataBase();
			
		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(refBlocSrc == kCurrentTCLRef)
			refBlocSrc = parser->getCurItem();
		
		if(refBlocDest == kCurrentTCLRef)
			refBlocDest = parser->getCurItem();
		
		// Select the destination frame
		if(itemManager->SelectPageItem(UIDRef(db,parser->getItem(refBlocDest))) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		parser->setCurItem(refBlocDest);
		
		UIDRef multiColDestRef;
		
		// Get the destination muticol ref, if any
		if(parser->getCurBoxUIDRef(multiColDestRef) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		InterfacePtr<IMultiColumnTextFrame> txtDestFrame (multiColDestRef, UseDefaultIID());
		if(txtDestFrame == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}
		
		UIDRef destModel = UIDRef(db,txtDestFrame->GetTextModelUID());

		// Select the source frame (it becomes the current frame)
		if(itemManager->SelectPageItem(UIDRef(db,parser->getItem(refBlocSrc))) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		parser->setCurItem(refBlocSrc);
		
		UIDRef multiColSrcRef;
		
		// Get the source muticol ref, if any
		if(parser->getCurBoxUIDRef(multiColSrcRef) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		InterfacePtr<IMultiColumnTextFrame> txtSrcFrame (multiColSrcRef, UseDefaultIID());
		if(txtSrcFrame == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}
		
		UIDRef srcModel = UIDRef(db,txtSrcFrame->GetTextModelUID());

		
		// Create a copy text or a move text command and process it
		if(deleteText)
		{	
			InterfacePtr<ICommand> moveCmd (Utils<ITextUtils>()->QueryMoveStoryFromAllToEndCommand(srcModel,destModel));
			if(moveCmd == nil)
			{		
				error = PMString(kErrNilInterface);
				break;
			}
			
			if(CmdUtils::ProcessCommand(moveCmd) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}
		
		}
		else
		{
			InterfacePtr<ICommand> copyCmd (Utils<ITextUtils>()->QueryCopyStoryFromAllToEndCommand(srcModel,destModel));
			if(copyCmd == nil)
			{		
				error = PMString(kErrNilInterface);
				break;
			}
			
			if(CmdUtils::ProcessCommand(copyCmd) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}
		}
		
		status = kSuccess;
		
	}while(false);
	
	if (seq != nil)
	{
		if (status == kSuccess)
			CmdUtils::EndCommandSequence(seq);
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
Func_AP::Func_AP(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_AP::Func_AP -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		resetMode = reader->ExtractBooleen(kFalse);
		
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

	if(!reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_AP::~Func_AP()
{
}


/* Constructor
*/
Func_AU::Func_AU(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_AU::Func_AU -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		useNewConvert = reader->ExtractBooleen(kFalse);
		
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

	if(!reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_AU::~Func_AU()
{
}

/* Constructor
*/
Func_AL::Func_AL(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_AL::Func_AL -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		modeAlign = reader->ExtractInt(0,0,1);
		
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

	if(!reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_AL::~Func_AL()
{
}

/* Constructor
*/
Func_IC::Func_IC(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_IC::Func_IC -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		imagePath = reader->ExtractString(255);
		
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
		IDCall_IC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* Destructor
*/
Func_IC::~Func_IC()
{
}

/* IDCall_IC
*/
void Func_IC::IDCall_IC(ITCLParser * parser)
{
	parser->setPicturePrefs(kTrue,kTrue,imagePath,kTrue,kTrue,9,100);
}

/* Constructor
*/
Func_EH::Func_EH(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_EH::Func_EH -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc1 = reader->ExtractTCLRef(kDefaultTCLRef);
		refBloc2 = reader->ExtractTCLRef(kDefaultTCLRef);

		if(refBloc1 >= refBloc2)
			throw TCLError(PMString(kErrParamEH));

		mode = reader->ExtractInt(0,0,2);
		offset = reader->ExtractReel('D','+',0,0);
		
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
		IDCall_EH(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* Destructor
*/
Func_EH::~Func_EH()
{
}

/* IDCall_EH
*/
void Func_EH::IDCall_EH(ITCLParser * parser)
{
	ErrorCode status = kSuccess;
	ICommandSequence * seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		IDataBase * db = parser->getCurDataBase();
			
		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}
		
		PMReal heightToSet = 0.0;
		switch(mode)
		{
			case 0 : 
				for(TCLRef i = refBloc1 ; i <= refBloc2 ; ++i)
				{
					InterfacePtr<IGeometry> frameGeo (db, parser->getItem(i), UseDefaultIID());
					if(frameGeo == nil)
					{
						error = PMString(kErrInvalidItem);
						status = kFailure;
						break;
					}
					PMReal frameHeight = frameGeo->GetStrokeBoundingBox().Height();
					if(frameHeight > heightToSet)
						heightToSet = frameHeight;
				}
				break;

			case 1 : 
				heightToSet = kPMMaxReal;
				for(TCLRef i = refBloc1 ; i <= refBloc2 ; ++i)
				{
					InterfacePtr<IGeometry> frameGeo (db, parser->getItem(i), UseDefaultIID());
					if(frameGeo == nil)
					{
						error = PMString(kErrInvalidItem);
						status = kFailure;
						break;
					}
					PMReal frameHeight = frameGeo->GetStrokeBoundingBox().Height();
					if(frameHeight < heightToSet)
						heightToSet = frameHeight;
				}
				break;

			case 2 : 
				PMReal sum = 0.0;
				for(TCLRef i = refBloc1 ; i <= refBloc2 ; ++i)
				{
					InterfacePtr<IGeometry> frameGeo (db, parser->getItem(i), UseDefaultIID());
					if(frameGeo == nil)
					{
						error = PMString(kErrInvalidItem);
						status = kFailure;
						break;
					}
					sum += frameGeo->GetStrokeBoundingBox().Height();
				}
				heightToSet = sum / (refBloc2 - refBloc1 + 1);
				break;
		}
		
		if(status == kFailure)
			break;

		heightToSet += offset;

		InterfacePtr<IItemTransform> itemTransform (parser->QueryItemTransform());
		if(itemTransform == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}

		for(TCLRef i = refBloc1 ; i <= refBloc2 ; ++i)
		{
			InterfacePtr<IGeometry> frameGeo (db, parser->getItem(i), UseDefaultIID());
		
			PMReal diff = heightToSet - frameGeo->GetStrokeBoundingBox().Height();

			if(itemTransform->ResizeFrameCmd(::GetUIDRef(frameGeo),0.0,diff) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				status = kFailure;
				break;
			}
		}	
		
	}while(false);
	
	if (seq != nil)
	{
		if (status == kSuccess)
			CmdUtils::EndCommandSequence(seq);
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
Func_XD::Func_XD(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_XD::Func_XD -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		id = reader->ExtractInt(0,0,kMaxInt32);
		type = reader->ExtractInt(0,0,128);
		allowHoles = reader->ExtractInt(0,0,128);
		invert = reader->ExtractInt(0,0,256);
		index = reader->ExtractInt(0,0,256);
		minSize = reader->ExtractReel('D','+',0,0);
		outTop = reader->ExtractReel('D','+',0,0);
		outLeft = reader->ExtractReel('D','+',0,0);
		outBottom = reader->ExtractReel('D','+',0,0);
		outRight = reader->ExtractReel('D','+',0,0);
		maxError = reader->ExtractReel('D','+',0,0);
		flags = reader->ExtractInt(0,0,256);
		whiteness = reader->ExtractReel('D','+',0,0);
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
		IDCall_XD(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_XD::~Func_XD()
{
}

/* IDCall_XD
*/
void Func_XD::IDCall_XD(ITCLParser * parser)
{
}

/* Constructor
*/
Func_XH::Func_XH(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_XH::Func_XH -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		id = reader->ExtractInt(0,0,kMaxInt32);
		type = reader->ExtractInt(0,0,128);
		allowHoles = reader->ExtractInt(0,0,128);
		invert = reader->ExtractInt(0,0,256);
		index = reader->ExtractInt(0,0,256);
		minSize = reader->ExtractReel('D','+',0,0);
		outTop = reader->ExtractReel('D','+',0,0);
		outLeft = reader->ExtractReel('D','+',0,0);
		outBottom = reader->ExtractReel('D','+',0,0);
		outRight = reader->ExtractReel('D','+',0,0);
		maxError = reader->ExtractReel('D','+',0,0);
		flags = reader->ExtractInt(0,0,256);
		whiteness = reader->ExtractReel('D','+',0,0);
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
		IDCall_XH(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_XH::~Func_XH()
{
}

/* IDCall_XH
*/
void Func_XH::IDCall_XH(ITCLParser * parser)
{
}


/* Constructor
*/
Func_XR::Func_XR(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_XR::Func_XR -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		runaroundsides = reader->ExtractBooleen(kTrue);
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

	if(!reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Constructor
*/
Func_ER::Func_ER(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_ER::Func_ER -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		
		reader->MustEgal();

		bTop = reader->ExtractReel('D','+',0,0);
		bInside = reader->ExtractReel('D','+',0,0);
		bBottom = reader->ExtractReel('D','+',0,0);
		bOutside = reader->ExtractReel('D','+',0,0);
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
		IDCall_ER(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_ER
*/
void Func_ER::IDCall_ER(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDocument * doc = parser->getCurDoc();
		
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<ICommand> exportPrefCmd (CmdUtils::CreateCommand(kPrsSetDocEpsExportPrefsCmdBoss));
		if(exportPrefCmd == nil)
		{
			error = PMString(kErrNilInterface);	
			break;
		}

		exportPrefCmd->SetItemList(UIDList(doc));

		// Change bleed parameters
		InterfacePtr<IEPSExportPreferences> exportPrefsCmdData (exportPrefCmd,UseDefaultIID());
		exportPrefsCmdData->SetEPSExBleedOnOff(IEPSExportPreferences::kExportBleedON);
		exportPrefsCmdData->SetEPSExBleedTop(bTop);
		exportPrefsCmdData->SetEPSExBleedBottom(bBottom);
		exportPrefsCmdData->SetEPSExBleedInside(bInside);
		exportPrefsCmdData->SetEPSExBleedOutside(bOutside);

		// Keep other parameters
		InterfacePtr<IEPSExportPreferences> docEpsExportPref (doc, UseDefaultIID());
		exportPrefsCmdData->SetEPSExPageOption(docEpsExportPref->GetEPSExPageOption());
		exportPrefsCmdData->SetEPSExReaderSpread(docEpsExportPref->GetEPSExReaderSpread());
		exportPrefsCmdData->SetEPSExPSLevel(docEpsExportPref->GetEPSExPSLevel());
		exportPrefsCmdData->SetEPSExColorSpace(docEpsExportPref->GetEPSExColorSpace());
		exportPrefsCmdData->SetEPSExPreview(docEpsExportPref->GetEPSExPreview());
		exportPrefsCmdData->SetEPSExDataFormat(docEpsExportPref->GetEPSExDataFormat());
		exportPrefsCmdData->SetEPSExBitmapSampling(docEpsExportPref->GetEPSExBitmapSampling());
		exportPrefsCmdData->SetEPSExIncludeFonts(docEpsExportPref->GetEPSExIncludeFonts());
		exportPrefsCmdData->SetEPSExOPIReplace(docEpsExportPref->GetEPSExOPIReplace());
		
		exportPrefsCmdData->SetEPSExOmitPDF(docEpsExportPref->GetEPSExOmitPDF());
		exportPrefsCmdData->SetEPSExOmitEPS(docEpsExportPref->GetEPSExOmitEPS());
		exportPrefsCmdData->SetEPSExOmitBitmapImages(docEpsExportPref->GetEPSExOmitBitmapImages());
		exportPrefsCmdData->SetEPSExSpotOverPrint(docEpsExportPref->GetEPSExSpotOverPrint());
		exportPrefsCmdData->SetEPSExIgnoreFlattenerSpreadOverrides(docEpsExportPref->GetEPSExIgnoreFlattenerSpreadOverrides());
		exportPrefsCmdData->SetEPSExFlattenerStyle(docEpsExportPref->GetEPSExFlattenerStyle());
		exportPrefsCmdData->SetEPSExPageRange(docEpsExportPref->GetEPSExPageRange());

		InterfacePtr<IEpsDocumentName> epsDocName (doc, UseDefaultIID());

		InterfacePtr<IEpsDocumentName> docNameCmdData (exportPrefCmd, UseDefaultIID());
		docNameCmdData->setEpsFileName(epsDocName->getEpsFileName());
		docNameCmdData->setEpsFileExt(epsDocName->getEpsFileExt());

		if(CmdUtils::ProcessCommand(exportPrefCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}
	
		status = kSuccess;

	}while(false);
	

	if (status != kSuccess)
	{
		throw TCLError(error);
	}		
}

/* Constructor
*/
Func_RB::Func_RB(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_RB::Func_RB -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		xScale = reader->ExtractPourcent('+',100,0);
		yScale = reader->ExtractPourcent('+',100,0);
		minSize = reader->ExtractReel('D','+',10,0);
		maxSize = reader->ExtractReel('D','+',10,0);
		minLeading = reader->ExtractReel('D','+',10,0);
		maxLeading = reader->ExtractReel('D','+',10,0);
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
		IDCall_RB(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_RB
*/
void Func_RB::IDCall_RB(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		IDocument * doc = parser->getCurDoc();
		
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		if(xScale == 1.0 && yScale == 1.0)
		{
			status = kSuccess;
			break;
		}

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		InterfacePtr<IHierarchy> frameHier (parser->getCurDataBase(),parser->getItem(refBloc),UseDefaultIID());

		if(!Utils<IFrameUtils>()->IsTextFrame(frameHier))
		{
			error=PMString(kErrTextBoxRequired);
			break;
		}
			
		InterfacePtr<IMultiColumnTextFrame> txtFrame (parser->getCurDataBase(), frameHier->GetChildUID(0), UseDefaultIID());
		InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());
	
		
			
		PMReal maxLeadingFound = - kPMMaxReal , minLeadingFound = kPMMaxReal;
		PMReal maxSizeFound = -kPMMaxReal , minSizeFound = kPMMaxReal;
		PMReal coeffSize, coeffLeading;
		if(txtModel->GetPrimaryStoryThreadSpan() > 1)
		{				
			// first adjust horizontal scale
			PMReal xScaleVal = xScale/yScale;

			InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrXGlyphScaleBoss, IID_ITEXTATTRREALNUMBER));
			if(txtAttrReal == nil)
				break;

			txtAttrReal->SetRealNumber(xScaleVal);

			boost::shared_ptr<AttributeBossList> attrList (new AttributeBossList);
			attrList->ApplyAttribute(txtAttrReal);

			InterfacePtr<ITextModelCmds> txtModelCmds (txtModel, UseDefaultIID());
			InterfacePtr<ICommand> applyCmd (txtModelCmds->ApplyCmd(0, txtModel->GetPrimaryStoryThreadSpan() -1, attrList, kCharAttrStrandBoss));
			if(CmdUtils::ProcessCommand(applyCmd) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			if(yScale != 1.0)
			{
				// Update size and leading
				InDesign::TextRange txtRange (txtModel, 0, txtModel->GetPrimaryStoryThreadSpan()-1);

				K2Vector<InDesign::TextRange> textRanges;
				textRanges.push_back(txtRange);

				K2Vector<ClassID> attributeClasses;
				attributeClasses.push_back(kTextAttrLeadBoss);
				attributeClasses.push_back(kTextAttrPointSizeBoss);

				// Construct the iterator.
				TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
				while(runIter)
				{
					InterfacePtr<ITextAttrRealNumber> leadAttr ((ITextAttrRealNumber *) runIter->QueryByClassID(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER));
					if(leadAttr != nil)
					{
						PMReal leadValue = leadAttr->Get();
						if(leadValue > 0)
						{
							if(leadValue < minLeadingFound)
								minLeadingFound = leadValue;
							if(leadValue > maxLeadingFound)
								maxLeadingFound = leadValue;
						}
					}

					InterfacePtr<ITextAttrRealNumber> sizeAttr ((ITextAttrRealNumber *) runIter->QueryByClassID(kTextAttrPointSizeBoss, IID_ITEXTATTRREALNUMBER));
					if(sizeAttr != nil)
					{
						PMReal sizeValue = sizeAttr->Get();
						if(sizeValue < minSizeFound)
							minSizeFound = sizeValue;
						if(sizeValue > maxSizeFound)
							maxSizeFound = sizeValue;
					}

					++runIter;
				}

				if(maxLeadingFound == - kPMMaxReal)
					maxLeadingFound =  - maxLeading;
					
				if(minLeadingFound == kPMMaxReal)
					minLeading = - minLeading;
		
				// Determine coefficients
				coeffSize = yScale;
				coeffLeading = yScale;
				if(yScale > 1.0)
				{
					if((maxSizeFound * yScale) > maxSize)
						coeffSize = maxSize / maxSizeFound;
					
					if((maxLeadingFound * yScale) > maxLeading)
						coeffLeading = maxLeading / maxLeadingFound;										
				}
				else
				{
					if((minSizeFound * yScale) < minSize)
						coeffSize = minSize / minSizeFound;

					if((minLeadingFound * yScale) < minLeading)
						coeffLeading = minLeading / minLeadingFound;
				}

				// Adjust size and leading with the coefficient
				runIter = TextAttributeRunIterator(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
				while(runIter)
				{
					boost::shared_ptr<AttributeBossList> newAttrList (new AttributeBossList);

					InterfacePtr<ITextAttrRealNumber> leadAttr ((ITextAttrRealNumber *) runIter->QueryByClassID(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER));
					if(leadAttr != nil)
					{
						InterfacePtr<ITextAttrRealNumber> newLeadAttr ((ITextAttrRealNumber *) ::CreateObject(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER));
						if(newLeadAttr == nil)
							break;
						if(leadAttr->Get() > 0)
						{
							newLeadAttr->SetRealNumber(leadAttr->Get() * coeffLeading);
							newAttrList->ApplyAttribute(newLeadAttr);
						}
					}

					InterfacePtr<ITextAttrRealNumber> sizeAttr ((ITextAttrRealNumber *) runIter->QueryByClassID(kTextAttrPointSizeBoss, IID_ITEXTATTRREALNUMBER));
					if(sizeAttr != nil)
					{
						InterfacePtr<ITextAttrRealNumber> newSizeAttr ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPointSizeBoss, IID_ITEXTATTRREALNUMBER));
						if(newSizeAttr == nil)
							break;
						newSizeAttr->SetRealNumber(sizeAttr->Get() * coeffSize);
						newAttrList->ApplyAttribute(newSizeAttr);
					}

					runIter.ApplyAttributes(newAttrList);

					++runIter;
				}
			}
		}

		status = kSuccess;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_EB::Func_EB(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_RB::Func_RB -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		
		reader->MustEgal();

		type = reader->ExtractInt(0,0,1);
		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		group = reader->ExtractBooleen(kTrue);
		
		int32 nbFrame = 0; 
		do
		{
			frameList.push_back(reader->ExtractTCLRef(kCurrentTCLRef));
			++nbFrame;
		}
		while(!reader->IsEndOfCommand() && nbFrame <= MAX_BLOC);

		if(nbFrame > MAX_BLOC)
			throw TCLError(PMString(kErrTooManyVariables));

		if(!group && (nbFrame % 2 != 0))
			throw TCLError(PMString(kErrNotRightArgumentNumber));
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
		IDCall_EB(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_EB
*/
void Func_EB::IDCall_EB(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	ICommandSequence * seq = nil;

	do
	{
		IDocument * doc = parser->getCurDoc();
		
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		// Get the items' UID list and check whether they all lay on the same spread
		bool16 errorOccured = kFalse;
		UIDList frameUIDList = Func_BG::GetAndCheckItemsToGroup(parser, group,db, frameList, errorOccured, error);
		if(errorOccured)
			break;

		InterfacePtr<IItemTransform> itemTransform (parser->QueryItemTransform());
		if(itemTransform == nil)
			break;

		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		switch(type)
		{
			case 0 : // Horizontal
				{
					PMReal totalWidth = 0.0;
					for(int32 i = 0 ; i < frameUIDList.Length() ; ++i)
					{
						InterfacePtr<IGeometry> frameGeo (db, frameUIDList[i], UseDefaultIID());
						totalWidth += frameGeo->GetStrokeBoundingBox().Width();
					}
					if(refBloc != 0)
					{
						InterfacePtr<IGeometry> refBlocGeo (db, parser->getItem(refBloc), UseDefaultIID());
						if(refBlocGeo == nil)
						{
							error = PMString(kErrInvalidItem);
							break;
						}
						
						PMReal refWidth = refBlocGeo->GetStrokeBoundingBox().Width();

						PMReal inter = (refWidth - totalWidth) / frameUIDList.Length();
						
						if(inter > 0)
						{
							PMPoint refTopLeft = refBlocGeo->GetStrokeBoundingBox().LeftTop();
							::TransformInnerPointToPasteboard(refBlocGeo, &refTopLeft);

							InterfacePtr<IGeometry> frameGeo (db, frameUIDList[0], UseDefaultIID());
							PMPoint frameTopLeft = frameGeo->GetStrokeBoundingBox().LeftTop();
							::TransformInnerPointToPasteboard(frameGeo, &frameTopLeft);

							PMPoint newTopLeft(refTopLeft.X() + (inter/2.0), frameTopLeft.Y());
							if((status = itemTransform->MoveCmd(UIDRef(db,frameUIDList[0]),newTopLeft)) != kSuccess)
							{
								error = ErrorUtils::PMGetGlobalErrorString();
								break;
							}

							for(int32 i = 1 ; i < frameUIDList.Length() ; ++i)
							{
								PMPoint previousFrameRightTop = frameGeo->GetStrokeBoundingBox().RightTop();
								::TransformInnerPointToPasteboard(frameGeo, &previousFrameRightTop);

								frameGeo = InterfacePtr<IGeometry>(db, frameUIDList[i], UseDefaultIID());
								frameTopLeft = frameGeo->GetStrokeBoundingBox().LeftTop();
								::TransformInnerPointToPasteboard(frameGeo, &frameTopLeft);

								newTopLeft = PMPoint(previousFrameRightTop.X() + inter, frameTopLeft.Y());
								if((status = itemTransform->MoveCmd(UIDRef(db,frameUIDList[i]),newTopLeft)) != kSuccess)
								{
									error = ErrorUtils::PMGetGlobalErrorString();
									break;
								}
							}
						}
						else
							status = kSuccess;
					}
					else
						status = kSuccess;
				}
				break;

			case 1 :
				{
					PMReal totalHeight = 0.0;
					for(int32 i = 0 ; i < frameUIDList.Length() ; ++i)
					{
						InterfacePtr<IGeometry> frameGeo (db, frameUIDList[i], UseDefaultIID());
						totalHeight += frameGeo->GetStrokeBoundingBox().Height();
					}
					if(refBloc != 0)
					{
						InterfacePtr<IGeometry> refBlocGeo (db, parser->getItem(refBloc), UseDefaultIID());
						if(refBlocGeo == nil)
						{
							error = PMString(kErrInvalidItem);
							break;
						}
						
						PMReal refHeight = refBlocGeo->GetStrokeBoundingBox().Height();

						PMReal inter = (refHeight - totalHeight) / frameUIDList.Length();
						
						if(inter > 0)
						{
							PMPoint refTopLeft = refBlocGeo->GetStrokeBoundingBox().LeftTop();
							::TransformInnerPointToPasteboard(refBlocGeo, &refTopLeft);

							InterfacePtr<IGeometry> frameGeo (db, frameUIDList[0], UseDefaultIID());
							PMPoint frameTopLeft = frameGeo->GetStrokeBoundingBox().LeftTop();
							::TransformInnerPointToPasteboard(frameGeo, &frameTopLeft);

							PMPoint newTopLeft(frameTopLeft.X(), refTopLeft.Y() + (inter/2.0));
							if((status = itemTransform->MoveCmd(UIDRef(db,frameUIDList[0]),newTopLeft)) != kSuccess)
							{
								error = ErrorUtils::PMGetGlobalErrorString();
								break;
							}

							for(int32 i = 1 ; i < frameUIDList.Length() ; ++i)
							{
								PMPoint previousFrameLeftBottom = frameGeo->GetStrokeBoundingBox().LeftBottom();
								::TransformInnerPointToPasteboard(frameGeo, &previousFrameLeftBottom);

								frameGeo = InterfacePtr<IGeometry>(db, frameUIDList[i], UseDefaultIID());
								frameTopLeft = frameGeo->GetStrokeBoundingBox().LeftTop();
								::TransformInnerPointToPasteboard(frameGeo, &frameTopLeft);

								newTopLeft = PMPoint(frameTopLeft.X(), previousFrameLeftBottom.Y() + inter);
								if((status = itemTransform->MoveCmd(UIDRef(db,frameUIDList[i]),newTopLeft)) != kSuccess)
								{
									error = ErrorUtils::PMGetGlobalErrorString();
									break;
								}
							}
						}
						else
							status = kSuccess;
					}
					else 
						status = kSuccess;
				}	
				break;
		}

		

	}while(false);
	
	if (seq != nil)
	{
		if (status == kSuccess)
			CmdUtils::EndCommandSequence(seq);
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
Func_IP::Func_IP(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		type = reader->ExtractInt(1,1,3);
		xScale = reader->ExtractPourcent('+',100,0);
		yScale = reader->ExtractPourcent('+',100,0);		
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
		IDCall_IP(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_IP
*/
void Func_IP::IDCall_IP(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
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
		
		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
			break;

		InterfacePtr<IItemTransform> itemTransform (parser->QueryItemTransform());
		if(itemTransform == nil)
			break;
		
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		UID itemUID = parser->getItem(refBloc);
		if(itemUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IGeometry> frameGeo (db, itemUID, UseDefaultIID());

		if(!Utils<IPageItemTypeUtils>()->IsGraphicFrame(frameGeo))
		{
			//error = PMString(kErrImageBoxRequired);
			status = kSuccess;
			break;
		}

		PMRect bbox = frameGeo->GetPathBoundingBox();

		InterfacePtr<IHierarchy> frameHier (frameGeo, UseDefaultIID());
		
		InterfacePtr<IHierarchy> imageHier (frameHier->QueryChild(0));
		if(imageHier == nil)
		{
			//error = PMString(kErrImageBoxRequired);
			status = kSuccess;
			break;
		}

		InterfacePtr<IGeometry> imageGeo (imageHier, UseDefaultIID());
		PMRect imagebbox = imageGeo->GetPathBoundingBox();

		PMReal deltaX = 0, deltaY = 0;
		PMPoint refPoint = imagebbox.LeftTop();
		switch(type)
		{
			case 1 : // bottom left
				{							
					PMPoint leftBottom = imagebbox.LeftBottom();
					PMPoint leftTop = imagebbox.LeftTop();
					::TransformInnerPointToPasteboard(imageGeo,&leftBottom);
					::TransformInnerPointToPasteboard(imageGeo,&leftTop);

					deltaX = 0;
					deltaY = bbox.Height() - (leftBottom.Y()- leftTop.Y()); //imagebbox.Height() is not the real image's height if its scale is not 100 %
					refPoint = imagebbox.LeftBottom();
				}
				break;

			case 2 : // top right
				{
					PMPoint rightTop = imagebbox.RightTop();
					PMPoint leftTop = imagebbox.LeftTop();
					::TransformInnerPointToPasteboard(imageGeo,&rightTop);
					::TransformInnerPointToPasteboard(imageGeo,&leftTop);

					deltaX = bbox.Width() - (rightTop.X() - leftTop.X()); //imagebbox.Width() is not the real image's height if its scale is not 100 %
					deltaY = 0;
					refPoint = imagebbox.RightTop();
				}
				break;

			case 3 : // bottom right
				{
					PMPoint leftBottom = imagebbox.LeftBottom();
					PMPoint leftTop = imagebbox.LeftTop();
					PMPoint rightTop = imagebbox.RightTop();
					::TransformInnerPointToPasteboard(imageGeo,&leftBottom);
					::TransformInnerPointToPasteboard(imageGeo,&leftTop);
					::TransformInnerPointToPasteboard(imageGeo,&rightTop);

					deltaX = bbox.Width() - (rightTop.X() - leftTop.X());
					deltaY = bbox.Height() - (leftBottom.Y()- leftTop.Y());
					refPoint = imagebbox.RightBottom();
				}
				break;

			default :
				break;
		}

		// Calculate the new position of the left top corner, then move the image
		PMPoint frameLeftTop (bbox.LeftTop());
		::TransformInnerPointToPasteboard(frameGeo,&frameLeftTop);

		PMPoint newLeftTop (frameLeftTop.X() + deltaX, frameLeftTop.Y() + deltaY);
		
		if(itemTransform->MoveCmd(::GetUIDRef(imageGeo),newLeftTop) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(itemTransform->Scale(::GetUIDRef(imageGeo),xScale,yScale,refPoint) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(itemManager->SelectPageItem(UIDRef(db,itemUID)) != kSuccess) 
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurItem(refBloc);
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
Func_SE::Func_SE(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		if(reader->ExtractBooleen(kFalse))
			data.fMode = kDSMBlurred;
		else
			data.fMode = kDSMNone;
		
		data.fBlendMode = (PMBlendingMode) reader->ExtractInt(0,0,15);
		data.fOffsetX = reader->ExtractReel('D','+',0,0);
		data.fOffsetY = reader->ExtractReel('D','+',0,0);
		
		int32 numColor = reader->ExtractInt(5,0,MAX_COLOR);
		data.fColorUID = parser->getColorList(numColor).colorUID;
		data.fOpacity = reader->ExtractPourcent('+',75,0) * 100;
		data.fBlurRadius  = reader->ExtractReel('D','+',5,0);
		data.fSpread = reader->ExtractPourcent('+',0,0) * 100;
		data.fNoise = reader->ExtractPourcent('+',0,0) * 100;

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
		IDCall_SE(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_SE
*/
void Func_SE::IDCall_SE(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
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
		
		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
			break;

		
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID itemUID = parser->getItem(refBloc);
		if(itemUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}
		
		UIDList itemList(db,itemUID);
		K2Vector<IXPAttributeSuite::AttributeType> attrList;
		attrList.push_back(IXPAttributeSuite::kDSMode);
		attrList.push_back(IXPAttributeSuite::kDSOffsetX);
		attrList.push_back(IXPAttributeSuite::kDSOffsetY);
		attrList.push_back(IXPAttributeSuite::kDSBlendMode);
		attrList.push_back(IXPAttributeSuite::kDSOpacity);
		attrList.push_back(IXPAttributeSuite::kDSBlurRadius);
		attrList.push_back(IXPAttributeSuite::kDSColor);
		attrList.push_back(IXPAttributeSuite::kDSSpread);
		attrList.push_back(IXPAttributeSuite::kDSNoise);

		if(Utils<IXPAttributeFacade>()->ApplyDropShadowAttributes(itemList,&data,attrList) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(itemManager->SelectPageItem(UIDRef(db,itemUID)) != kSuccess) 
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurItem(refBloc);
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
Func_AR::Func_AR(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();

		tclFilename = reader->ExtractString(255);	
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
		IDCall_AR(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_AR
*/
void Func_AR::IDCall_AR(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		InterfacePtr<IReverseTCL> reverse ((IReverseTCL *) ::CreateObject(kReverseTCLBoss,IID_IREVERSETCL));
		if (reverse == nil)
			break;

		IDFile tclfile = FileUtils::PMStringToSysFile(tclFilename);
		IDFile parent;		

		FileUtils::GetParentDirectory(tclfile, parent) ;		
		if(FileUtils::DoesFileExist(parent) == kFalse)// Invalid path
		{
       		error = PMString(kErrBadPath); 
            break;
        }

		if(!reverse->SetOutputFile(tclfile))
		{
			error = PMString(kErrCannotWriteFile);
			break;
		}
			
		status = reverse->StartReverse();

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_DZ::Func_DZ(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();

		pdfFilename = reader->ExtractString(255);
		startPage = reader->ExtractInt(1,0,MAX_PAGE);
		endPage = reader->ExtractInt(MAX_PAGE,0,MAX_PAGE);
		eraseExisting = reader->ExtractBooleen(kTrue);	
		exportBook = reader->ExtractBooleen(kFalse);
		if(exportBook)
			bookExportStyle = reader->ExtractString(255);
		else
			bookExportStyle = kNullString;
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
		IDCall_DZ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_DZ
*/
void Func_DZ::IDCall_DZ(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		IDocument * doc = nil;
		UIDRef bookRef = UIDRef::gNull;
		if(exportBook)
		{
			bookRef = parser->getCurBook();
			if(bookRef == UIDRef::gNull)
			{
				error = PMString(kErrNoCurrentBook);
				break;
			}
		}
		else
		{
			doc = parser->getCurDoc();
			
			// Check if there is a document open
			if(doc == nil)
			{
				error = PMString(kErrNeedDoc);		
				break;
			}
			
			if(startPage == 0)
				startPage = parser->getCurPage();
			
			if(endPage == 0)
				endPage = parser->getCurPage();
			else if(endPage == MAX_PAGE)
			{
				InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
				if(pageList == nil)
				{
					error = PMString(kErrNilInterface);	
					break;
				}
				
				endPage = pageList->GetPageCount();
			}
		}

		if((status = ExportUtils::PdfExport(doc,pdfFilename,eraseExisting,kFalse,startPage,endPage, UIDList(), bookRef, bookExportStyle)) != kSuccess) {
			error = ErrorUtils::PMGetGlobalErrorString();
        }
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_DDollar::Func_DDollar(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();	
		pdfStyleName = reader->ExtractString(255);
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
		IDCall_DDollar(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_DDollar
*/
void Func_DDollar::IDCall_DDollar(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	// Code a conserver pour test ++
	/*
	InterfacePtr<IWorkspace> workspace(gSession->QueryWorkspace());
	InterfacePtr<IGenStlEdtListMgr> PDFExptStyleListMgr(workspace, IID_IPDFEXPORTSTYLELISTMGR);
	//int32 nStyleIndex = PDFExptStyleListMgr->GetStyleIndexByName(pdfStyleName);
    for(int32 i =0;i< PDFExptStyleListMgr->GetNumStyles();i++){
		PMString styleName;
		PDFExptStyleListMgr->GetNthStyleName(i,&styleName);
		CAlert::InformationAlert("Style : " + styleName);
	}
	*/
	// Code a conserver pour test --
	do
	{
		IDocument * doc = parser->getCurDoc();
		
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// GD 5.4.5 MODIF MAKEHIA FINALEMENT ANNULEE
		// POUR REJOINDRE CAS GENERAL TOUS CLIENTS : 
		// Pas d'ajout auto de crochets, il faut passer
		// en parametre le nom exact du joboptions.
		// Attention, les accents doivent etre en UTF-8.
		/*
		// GD 5.4.4 ++
		PMString tempstr;
		tempstr = "[" + pdfStyleName + "]";
		tempstr.SetTranslatable(kFalse);
		pdfStyleName = tempstr;
		// GD 5.4.4 --
		*/
		// GD 5.4.5 --
		
		InterfacePtr<ICommand> setPdfStyleCmd (CmdUtils::CreateCommand(kPrsSetDocPdfExportPrefsCmdBoss));
		setPdfStyleCmd->SetItemList(UIDList(doc));

		InterfacePtr<IStringData> styleName (setPdfStyleCmd, IID_IPDFEXPORTPREFS);
		styleName->Set(pdfStyleName);
	
		status = CmdUtils::ProcessCommand(setPdfStyleCmd);

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}


/* Constructor
*/
Func_AG::Func_AG(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();	
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		returnChar = reader->ExtractBooleen(kFalse);
		groupage = reader->ExtractBooleen(kTrue);

		int32 nbFrame = 0; 
		do
		{
			frameList.push_back(reader->ExtractTCLRef(kCurrentTCLRef));
			++nbFrame;
		}
		while(!reader->IsEndOfCommand() && nbFrame <= MAX_BLOC);

		if(nbFrame > MAX_BLOC)
			throw TCLError(PMString(kErrTooManyVariables));

		if(!groupage && (nbFrame % 2 != 0))
			throw TCLError(PMString(kErrNotRightArgumentNumber));
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
		IDCall_AG(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_AG
*/
void Func_AG::IDCall_AG(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		IDocument * doc = parser->getCurDoc();
		IDataBase * db = parser->getCurDataBase();
		
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		// Check whether all frames are correct
		bool16 errorOccured = kFalse;
		UIDList * itemsToGroup = new UIDList(Func_BG::GetAndCheckItemsToGroup(parser, groupage,db, frameList, errorOccured, error));
		if(errorOccured)
		{	
			delete itemsToGroup;
			break;			
		}

		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());

		UIDRef groupItem = UIDRef::gNull;
		if(itemsToGroup->Length() > 1){
			if((status = itemManager->GroupPageItemsCmd(itemsToGroup, groupItem)) != kSuccess){
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}	
		}
		else{
			groupItem = itemsToGroup->GetRef(0);
			delete itemsToGroup;
		}

		// Make an inline from the group, but we need to remove it from its owner spread first
		InterfacePtr<ICommand> removeFromHierCmd (CmdUtils::CreateCommand(kRemoveFromHierarchyCmdBoss));
		removeFromHierCmd->SetItemList(UIDList(groupItem));
		if((status = CmdUtils::ProcessCommand(removeFromHierCmd)) != kSuccess){
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}		

		InterfacePtr<IItemTransform> itemTransform (parser->QueryItemTransform());
		if((status = itemTransform->MakeInlineCmd(groupItem, UIDRef(db, parser->getItem(refBloc)))) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}		

		if(returnChar)
		{
			InterfacePtr<IHierarchy> targetTxtFrame (db, parser->getItem(refBloc), UseDefaultIID());			
			InterfacePtr<ITextFrameHelper>  txtFrameHelper (parser->QueryTextFrameHelper());
			
			WideString carriageReturn;
			carriageReturn.Append(kTextChar_CR);
			if((status = txtFrameHelper->InsertTextAtEndCmd(UIDRef(db,targetTxtFrame->GetChildUID(0)) , carriageReturn, kFalse)) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_OE::Func_OE(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		data.fBlendMode = (PMBlendingMode) reader->ExtractInt(0,0,15);
		data.fOpacity = reader->ExtractPourcent('+',100,0) * 100;
		data.fKnockoutGroup = reader->ExtractBooleen(kFalse);
		data.fIsolationGroup = reader->ExtractBooleen(kFalse);
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
		IDCall_OE(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_OE
*/
void Func_OE::IDCall_OE(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
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
		
		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
			break;

		
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		UID itemUID = parser->getItem(refBloc);
		if(itemUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}
		
		UIDList itemList(db,itemUID);
		K2Vector<IXPAttributeSuite::AttributeType> attrList;
		attrList.push_back(IXPAttributeSuite::kBSBlendMode);
		attrList.push_back(IXPAttributeSuite::kBSOpacity);
		attrList.push_back(IXPAttributeSuite::kBSKnockoutGroup);
		attrList.push_back(IXPAttributeSuite::kBSIsolationGroup);
  

		if(Utils<IXPAttributeFacade>()->ApplyBasicAttributes(itemList,&data,attrList) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(itemManager->SelectPageItem(UIDRef(db,itemUID)) != kSuccess) 
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurItem(refBloc);
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
Func_FA::Func_FA(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		data.fMode = (PMVignetteMode) reader->ExtractInt(3,0,3);
		data.fWidth = reader->ExtractReel('D','+',12,0);
		data.fCorners =	(PMVignetteCorners)reader->ExtractInt(0,0,2); 
		data.fNoise = reader->ExtractPourcent('+',0,0) * 100;
		data.fInnerOpacity = reader->ExtractPourcent('+',100,0) * 100;
		data.fOuterOpacity = reader->ExtractPourcent('+',0,0) * 100;
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
		IDCall_FA(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_FA
*/
void Func_FA::IDCall_FA(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
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
		
		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
			break;

		
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		UID itemUID = parser->getItem(refBloc);
		if(itemUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}
		
		UIDList itemList(db,itemUID);
		K2Vector<IXPAttributeSuite::AttributeType> attrList;
		attrList.push_back(IXPAttributeSuite::kVTMode);
		attrList.push_back(IXPAttributeSuite::kVTWidth);
		attrList.push_back(IXPAttributeSuite::kVTCorners);
		attrList.push_back(IXPAttributeSuite::kVTNoise);
		attrList.push_back(IXPAttributeSuite::kVTInnerOpacity);
		attrList.push_back(IXPAttributeSuite::kVTOuterOpacity);
  
		if(Utils<IXPAttributeFacade>()->ApplyVignetteAttributes(itemList,&data,attrList) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(itemManager->SelectPageItem(UIDRef(db,itemUID)) != kSuccess) 
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurItem(refBloc);
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
Func_GO::Func_GO(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		isFillAttr = reader->ExtractBooleen(kTrue);
		angle = reader->ExtractInt(0,-180,360);

		// Optional
		length = reader->ExtractReel('D','+',0,0);
		center.X() = reader->ExtractReel('D','+',0,0);
		center.Y() = reader->ExtractReel('D','+',0,0);		
		radius = reader->ExtractReel('D','+',0,0);
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
		IDCall_GO(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_GO
*/
void Func_GO::IDCall_GO(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	ICommandSequence * seq = nil;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		IDataBase * db = parser->getCurDataBase();
		
		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
			break;
		
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		UID itemUID = parser->getItem(refBloc);
		if(itemUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		// Convert coordinates : center is given in pasteboard coordinates and expected to be set in inner coordinates
		InterfacePtr<IGeometry> itemGeo (db, itemUID, UseDefaultIID());
		::TransformPasteboardPointToInner(itemGeo, &center);

		InterfacePtr<IGraphicStyleDescriptor> graphicDesc (db, itemUID, UseDefaultIID());
		
		InterfacePtr<ICommand> gfxApplyMultAttributesCmd(CmdUtils::CreateCommand(kGfxApplyMultAttributesCmdBoss));

		InterfacePtr<IApplyMultAttributesCmdData> cmdData(gfxApplyMultAttributesCmd, UseDefaultIID());
		
		InterfacePtr<IGraphicAttrRealNumber> gradientAngle(nil);
		InterfacePtr<IGraphicAttrRealNumber> gradientLength(nil);
		InterfacePtr<IGraphicAttrRealNumber> gradientRadius(nil);
		InterfacePtr<IGraphicAttrPoint> gradientCenter(nil);
		UID renderingObject = kInvalidUID;
		if(isFillAttr)
		{
			 gradientAngle = InterfacePtr<IGraphicAttrRealNumber> (Utils<IGraphicAttributeUtils>()->CreateGradientFillAngleAttribute(angle));	
			 // other attributes can come here...
			 gradientLength = InterfacePtr<IGraphicAttrRealNumber> (Utils<IGraphicAttributeUtils>()->CreateGradientFillLengthAttribute(length));	
			 gradientRadius = InterfacePtr<IGraphicAttrRealNumber> (Utils<IGraphicAttributeUtils>()->CreateGradientFillRadiusAttribute(radius));	
			 gradientCenter = InterfacePtr<IGraphicAttrPoint> (Utils<IGraphicAttributeUtils>()->CreateGradientFillGradCenterAttribute(center));

			 Utils<IGraphicAttributeUtils>()->GetFillRenderingUID(renderingObject, graphicDesc);			 			 
		}
		else
		{
			gradientAngle = InterfacePtr<IGraphicAttrRealNumber> (Utils<IGraphicAttributeUtils>()->CreateGradientStrokeAngleAttribute(angle));
			// other attributes can come here...
			gradientLength = InterfacePtr<IGraphicAttrRealNumber> (Utils<IGraphicAttributeUtils>()->CreateGradientStrokeLengthAttribute(length));	
			gradientRadius = InterfacePtr<IGraphicAttrRealNumber> (Utils<IGraphicAttributeUtils>()->CreateGradientStrokeRadiusAttribute(radius));	
			gradientCenter = InterfacePtr<IGraphicAttrPoint> (Utils<IGraphicAttributeUtils>()->CreateGradientStrokeGradCenterAttribute(center));

			Utils<IGraphicAttributeUtils>()->GetStrokeRenderingUID(renderingObject, graphicDesc);
		}

		cmdData->AddAnAttribute(gradientAngle);
		cmdData->AddAnAttribute(gradientCenter);
		InterfacePtr<IGradientFill> gradientFill (db, renderingObject, UseDefaultIID());
		if(gradientFill)
		{
			if(gradientFill->GetGradientFillType() == kAxial)
				cmdData->AddAnAttribute(gradientLength);
			else
			{
				cmdData->AddAnAttribute(gradientRadius);
			}
		}	
		else
		{
			cmdData->AddAnAttribute(gradientLength);
			cmdData->AddAnAttribute(gradientRadius);
		}
		
		gfxApplyMultAttributesCmd->SetItemList(UIDList(db,itemUID));
		if(CmdUtils::ProcessCommand(gfxApplyMultAttributesCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		if(itemManager->SelectPageItem(UIDRef(db,itemUID)) != kSuccess) 
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurItem(refBloc);
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
Func_WW::Func_WW(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();	
		startPage = reader->ExtractInt(1, 0, MAX_PAGE);	
		refBlocMaster = reader->ExtractTCLRef(kDefaultTCLRef);
		refOverridenBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		autoPlace = reader->ExtractBooleen(kTrue);
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
		IDCall_WW(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

Func_WW::~Func_WW()
{
}

/* IDCall_WW
*/
void Func_WW::IDCall_WW(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	UIDRef overridenItem = UIDRef::gNull;

	do
	{		
		// Check if there is a document open
		IDocument * doc = parser->getCurDoc();
		IDataBase * db = parser->getCurDataBase();
		
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		if(startPage == CURRENT_PAGE)
			startPage = parser->getCurPage();

		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		UID overridePageUID = pageList->GetNthPageUID(startPage-1);
		
		// Get the 2 linked frames on the master
		InterfacePtr<IHierarchy> masterItem (db, parser->getItem(refBlocMaster), UseDefaultIID());
		if(masterItem == nil)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		if(!Utils<ILayoutUtils>()->IsAMaster(Utils<ILayoutUtils>()->GetOwnerPageUID(masterItem), db))
		{
			error = PMString(kErrNotAMasterItem); 
			break;
		}

		// Override the item
		UIDList overrideFrames(db);
		if(autoPlace)
		{
			UIDRef leftTxtFrame = UIDRef::gNull;
			UIDRef rightTxtFrame = UIDRef::gNull;
		
			if(this->GetMasterTextFrames(masterItem, leftTxtFrame, rightTxtFrame, error) != kSuccess)
				break;		

			if(rightTxtFrame == UIDRef::gNull)
			{
				error = PMString(kErrInvalidMasterTxtFrame);
				break;
			}
				
			if(pageList->GetPageType(overridePageUID) == kRightPage)
				overrideFrames.Append(rightTxtFrame);
			else
				overrideFrames.Append(leftTxtFrame);
		}
		else
			overrideFrames.Append(::GetUID(masterItem));


		// Handle shifting if overridden from a single page master in a facing pages spread (will have a shift of +/- (1/2 page width))
		PageType overridePageType = pageList->GetPageType(overridePageUID);
		InterfacePtr<ISpread> masterSpread (db, masterItem->GetSpreadUID(), UseDefaultIID());
		
		PMMatrix shift;
		if(overridePageType != kUnisexPage && masterSpread->GetNumPages() == 1)
		{
			InterfacePtr<IGeometry> pageGeo (db, overridePageUID, UseDefaultIID());
			PMReal pageWidth = pageGeo->GetStrokeBoundingBox().Width();
			if(overridePageType == kLeftPage)
				shift.Translate(-pageWidth/2, 0);
			else
				shift.Translate(pageWidth/2, 0);
		}

		UIDList pageUIDList(db);
		pageUIDList.Append(overridePageUID);

		PMMatrixCollection shiftList;
		shiftList.push_back(shift);

		InterfacePtr<ICommand> overrideItemCmd (CmdUtils::CreateCommand(kOverrideMasterPageItemCmdBoss));

		InterfacePtr<IOverrideMasterPageItemData> cmdData (overrideItemCmd, IID_IOVERRIDEMASTERPAGEITEMDATA );
		cmdData->SetOverrideMasterPageItemData(overrideFrames, pageUIDList, shiftList);

		if((status = CmdUtils::ProcessCommand(overrideItemCmd)) != kSuccess)
			break;		
		
		UIDRef dummy1, dummy2;
		PMMatrix dummyPoint;
		cmdData->GetNthOverride(0, &dummy1, &dummy2, &overridenItem, &dummyPoint);

		// Select the overriden item
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil) break;

		if((status = itemManager->SelectPageItem(overridenItem)) != kSuccess)
			error = ErrorUtils::PMGetGlobalErrorString();

	} while(kFalse);

	if(status == kSuccess)
	{
		parser->setItem(refOverridenBloc,overridenItem.GetUID());
		parser->setCurItem(refOverridenBloc);

		// Set default parameter for FW command
		if(autoPlace)
			Func_FW::refBloc = refOverridenBloc;
	}
	else
		throw TCLError(error);
}

ErrorCode Func_WW::GetMasterTextFrames(IHierarchy * masterItem, UIDRef& leftTxtFrame, UIDRef& rightTxtFrame, PMString& error)
{
	ErrorCode status = kFailure;
	error = PMString(kErrInvalidMasterTxtFrame);

	do
	{
		int32 textFlags;
		if(!Utils<IFrameUtils>()->IsTextFrame(masterItem, &textFlags))
		{
			error = PMString(kErrTextBoxRequired);
			break;
		}
		
		// There must be two linked frame, one per page in the master
		if(!(textFlags & IFrameUtils::kTF_InLink) && !(textFlags & IFrameUtils::kTF_OutLink))
			break;

		leftTxtFrame = rightTxtFrame = UIDRef::gNull;
		
		InterfacePtr<IMultiColumnTextFrame> masterTxtFrame (::GetDataBase(masterItem), masterItem->GetChildUID(0), UseDefaultIID());
		InterfacePtr<IFrameList> masterFrameList (masterTxtFrame->QueryFrameList());

		InterfacePtr<ITextFrameColumn> firstFrame (masterFrameList->QueryNthFrame(0));

		InterfacePtr<IHierarchy> firstSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(firstFrame));

		UIDRef firstTxtFrame = ::GetUIDRef(firstSpline);
		UIDRef secondTxtFrame = UIDRef::gNull;
		UID firstFramePage = Utils<ILayoutUtils>()->GetOwnerPageUID(firstSpline);

		for(int32 i = 1 ; i < masterFrameList->GetFrameCount() ; ++i)
		{
			InterfacePtr<ITextFrameColumn> txtFrame (masterFrameList->QueryNthFrame(i));
			InterfacePtr<IHierarchy> spline (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
			UIDRef splineRef = ::GetUIDRef(spline);

			if(splineRef != firstTxtFrame && splineRef != secondTxtFrame)
			{
				if(secondTxtFrame != UIDRef::gNull)
					break;
				else if(firstFramePage == Utils<ILayoutUtils>()->GetOwnerPageUID(spline))		
					break;
				else
				{
					secondTxtFrame = splineRef;
					
					// Determine page type
					PageType type = Utils<ILayoutUtils>()->GetPageType(UIDRef(firstTxtFrame.GetDataBase(), firstFramePage));
					if(type == kLeftPage)
					{
						leftTxtFrame = firstTxtFrame;
						rightTxtFrame = secondTxtFrame;
					}
					else if(type == kRightPage)
					{
						leftTxtFrame = secondTxtFrame;
						rightTxtFrame = firstTxtFrame;
					}						
					else
						break;

					status = kSuccess;
				}
			}
		}
		
	} while(kFalse);

	return status;
}

/* Constructor
*/
Func_FW::Func_FW(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		if(reader->TestEgal())
			refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
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
		IDCall_FW(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

TCLRef Func_FW::refBloc = kInvalidTCLRef;

void Func_FW::IDCall_FW(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error = PMString(kErrNilInterface);
	ICommandSequence * seq = nil;

	do
	{
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		IDataBase * db = parser->getCurDataBase();
		IDocument * doc = parser->getCurDoc();

		UID itemUID = parser->getItem(refBloc);

		InterfacePtr<IControllingPageItem> controlledPageItem (db, itemUID, UseDefaultIID());
		if(controlledPageItem == nil)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		if(controlledPageItem->GetControllingUID() == kInvalidUID)
		{
			error = PMString(kErrItemNotOverriden); // Item not overriden from master
			break;
		}

		InterfacePtr<IHierarchy> masterItem (db, controlledPageItem->GetControllingUID(), UseDefaultIID());

		InterfacePtr<IHierarchy> refBlocHier (db, parser->getItem(refBloc), UseDefaultIID());
		if(refBlocHier == nil)
		{
			error = PMString(kErrInvalidItem);
			break;
		}		

		if(!Utils<IFrameUtils>()->IsTextFrame(refBlocHier))
		{
			error = PMString(kErrTextBoxRequired);
			break;
		}

		// Get master text frames (2 linked frames)
		UIDRef leftTxtFrame = UIDRef::gNull;
		UIDRef rightTxtFrame = UIDRef::gNull;
	
		if(Func_WW::GetMasterTextFrames(masterItem, leftTxtFrame, rightTxtFrame, error) != kSuccess)
			break;	
		
		// Get page to start autoflow from
		InterfacePtr<IMultiColumnTextFrame> txtFrame (db, refBlocHier->GetChildUID(0), UseDefaultIID());
		InterfacePtr<IFrameList> frameList (txtFrame->QueryFrameList());

		InterfacePtr<ITextFrameColumn> curOverridenFrame (frameList->QueryNthFrame(frameList->GetFrameCount()-1));
		InterfacePtr<IHierarchy> curOverridenItem (Utils<ITextUtils>()->QuerySplineFromTextFrame(curOverridenFrame));
		
		int32 overridenItemPageIndex = -1;
		UID ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(curOverridenItem);
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		overridenItemPageIndex = pageList->GetPageIndex(ownerPageUID);

		if(overridenItemPageIndex == -1)
		{
			error = PMString(kErrItemOutOfSpread);
			break;
		}
		
		// Get master to apply
		InterfacePtr<IMasterPage> itemOwnerPage (db, ownerPageUID, UseDefaultIID());
		UIDRef masterSpreadUIDRef(db, itemOwnerPage->GetMasterSpreadUID());

		InterfacePtr<IFrameListComposer> frameComposer (curOverridenFrame, UseDefaultIID());
		frameComposer->RecomposeThruLastFrame();

		// Begin  a command sequence to be able to rollback
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;
		
		seq->SetUndoability(ICommand::kAutoUndo);

		status = kSuccess;

		// Loop while text is overset : add a new page if required, create a new text frame from master and link it to the previous one
		int32 latestFrameDamaged = -1;
		while(Utils<ITextUtils>()->IsOverset(frameList))
		{
			overridenItemPageIndex++;
			if(overridenItemPageIndex > pageList->GetPageCount()-1)
			{
				InterfacePtr<ISpread> spreadToAddTo (db,curOverridenItem->GetSpreadUID(), UseDefaultIID());

				InterfacePtr<ICommand> newPageCmd (CmdUtils::CreateCommand(kNewPageCmdBoss));
				newPageCmd->SetUndoability(ICommand::kAutoUndo);

				InterfacePtr<IPageCmdData> pageCmdData (newPageCmd, UseDefaultIID());
				pageCmdData->SetNewPageCmdData(::GetUIDRef(spreadToAddTo), 1, spreadToAddTo->GetNumPages());

				InterfacePtr<IApplyMasterCmdData> masterCmdData (newPageCmd, UseDefaultIID());
				masterCmdData->SetApplyMasterCmdData(masterSpreadUIDRef, IApplyMasterCmdData::kKeepCurrentPageSize);

				if((status = CmdUtils::ProcessCommand(newPageCmd)) != kSuccess)
					break;							
			}
			else
			{
				InterfacePtr<ICommand> applyMasterCmd (CmdUtils::CreateCommand(kApplyMasterSpreadCmdBoss ));
				applyMasterCmd->SetUndoability(ICommand::kAutoUndo);
				applyMasterCmd->SetItemList(UIDList(db, pageList->GetNthPageUID(overridenItemPageIndex)));

				InterfacePtr<IApplyMasterCmdData> masterCmdData (applyMasterCmd, UseDefaultIID());
				masterCmdData->SetApplyMasterCmdData(masterSpreadUIDRef, IApplyMasterCmdData::kKeepCurrentPageSize);

				if((status = CmdUtils::ProcessCommand(applyMasterCmd)) != kSuccess)
					break;
			}
			
			UID overridePageUID = pageList->GetNthPageUID(overridenItemPageIndex);
			PageType overridePageType = pageList->GetPageType(overridePageUID);

			UIDList overrideFrames(db);
			if(overridePageType == kRightPage)
				overrideFrames.Append(rightTxtFrame);
			else
				overrideFrames.Append(leftTxtFrame);

			// Handle shifting if overridden from a single page master in a facing pages spread (will have a shift of +/- (1/2 page width))
			InterfacePtr<ISpread> masterSpread (db, masterItem->GetSpreadUID(), UseDefaultIID());

			PMMatrix shift;
			if(overridePageType != kUnisexPage && masterSpread->GetNumPages() == 1)
			{
				InterfacePtr<IGeometry> pageGeo (db, overridePageUID, UseDefaultIID());
				PMReal pageWidth = pageGeo->GetStrokeBoundingBox().Width();
				if(overridePageType == kLeftPage)
					shift.Translate(-pageWidth/2, 0);
				else
					shift.Translate(pageWidth/2, 0);
			}

			// Override text frame
			UIDList pageUIDList(db);
			pageUIDList.Append(overridePageUID);

			PMMatrixCollection shiftList;
			shiftList.push_back(shift);

			InterfacePtr<ICommand> overrideItemCmd (CmdUtils::CreateCommand(kOverrideMasterPageItemCmdBoss));
			overrideItemCmd->SetUndoability(ICommand::kAutoUndo);

			InterfacePtr<IOverrideMasterPageItemData> cmdData (overrideItemCmd, IID_IOVERRIDEMASTERPAGEITEMDATA );
			cmdData->SetOverrideMasterPageItemData(overrideFrames, pageUIDList, shiftList);
			if((status = CmdUtils::ProcessCommand(overrideItemCmd)) != kSuccess)
				break;

			UIDRef overridenItem = UIDRef::gNull, dummy1, dummy2;
			PMMatrix dummyPoint;
			cmdData->GetNthOverride(0, &dummy1, &dummy2, &overridenItem, &dummyPoint);

			InterfacePtr<IHierarchy> overridenItemHier (overridenItem, UseDefaultIID());
			InterfacePtr<IMultiColumnTextFrame> overridentTxtFrame(db, overridenItemHier->GetChildUID(0), UseDefaultIID());

			InterfacePtr<IMultiColumnTextFrame> curOverridenFrame(db,curOverridenItem->GetChildUID(0), UseDefaultIID());
			status = Utils<ITextUtils>()->LinkTextFrames(curOverridenFrame, overridentTxtFrame, kFalse);
			curOverridenItem = overridenItemHier;			

			// Recompose the newly added frame
			int32 firstDamaged = frameList->GetFirstDamagedFrameIndex();			

			if(firstDamaged == latestFrameDamaged){
				// Remove latest added page, because we've gone one loop too far
				InterfacePtr<ICommand> deletePageCmd (CmdUtils::CreateCommand(kDeletePageCmdBoss));
				deletePageCmd->SetUndoability(ICommand::kAutoUndo);
				deletePageCmd->SetItemList(UIDList(UIDRef(db, overridePageUID)));
				status = CmdUtils::ProcessCommand(deletePageCmd);
				break;
			}

			latestFrameDamaged = firstDamaged;
			
			while(firstDamaged != -1){
				InterfacePtr<IFrameListComposer> frameComposer(db, frameList->GetNthFrameUID(firstDamaged), UseDefaultIID());
				frameComposer->RecomposeThruLastFrame();
				firstDamaged = frameList->GetFirstDamagedFrameIndex();
			}			
		}		
		
	} while(kFalse);

	if(seq != nil)
	{
		if(status == kSuccess)
			CmdUtils::EndCommandSequence(seq);
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