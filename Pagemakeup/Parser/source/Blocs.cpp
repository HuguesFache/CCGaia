/*
//	File:	Blocs.cpp
//
//	Date:	23-Apr-2003
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"
#include "TransformUtils.h"
#include "InstStrokeFillID.h"
#include "ScotchRulesID.h"
#include "SplineID.h"
#include "SDKUtilities.h"
#include "ILayoutUtils.h"
#include "ErrorUtils.h"
#include "UIDList.h"
#include "IFrameUtils.h"
#include "FileUtils.h"
#include "SDKFileHelper.h"
#include "StreamUtil.h"
#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "TaggedTextFiltersID.h"

// Interface includes
#include "IDocument.h"
#include "IDocumentLayer.h"
#include "IGeometry.h"
#include "IHierarchy.h"
#include "IGraphicAttributeUtils.h"
#include "IApplyMultAttributesCmdData.h"
#include "IPersistUIDData.h"
#include "IGraphicAttrClassID.h"
#include "IGraphicAttrRealNumber.h"
#include "IPMUnknownData.h"
#include "ISwatchUtils.h"
#include "IMultiColumnTextFrame.h"
#include "IDashedAttributeValues.h"
#include "IPathUtils.h"
#include "IFrameContentUtils.h"
#include "ITextColumnSizer.h"
#include "ILayerUtils.h"
#include "IPathGeometry.h"
#include "IFrameType.h"
#include "ITextModelCmds.h"
#include "IAnchoredObjectData.h"
#include "IIntData.h"
#include "IResizeItemsCmdData.h"
#include "IPageItemTypeUtils.h"
#include "IFrameListComposer.h"
#include "IStandOffContourWrapSettings.h"
#include "IGraphicStyleDescriptor.h"
#include "IGraphicFrameData.h"
#include "ISpread.h"
#include "ITextInset.h"
#include "ITextColumnData.h"
#include "ISetBoundingBoxCmdData.h"
#include "IBoundsData.h"
#include "IPageItemUtils.h"
#include "IUIDData.h"
#include "IStringData.h"
#include "IPMPointData.h"
#include "IRangeData.h"
#include "ISetClipValuesCmdData.h"
#include "IClippingPathSuite.h"
#include "IBoolData.h"
#include "ISelectionUtils.h"
#include "IDataLink.h"
#include "IDataLinkReference.h"
#include "IGraphicAttrInt32.h"
#include "IGeometryFacade.h"
#include "ITransformFacade.h"
#include "ILinkManager.h"
#include "IURIUtils.h"


// Project includes
#include "Blocs.h"
#include "Divers.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "IItemManager.h"
#include "IItemTransform.h"
#include "ITextFrameHelper.h"
#include "ITxtAttrHelper.h"

/* Constructor
*/
Func_BN::Func_BN(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BN::Func_BN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		largeur = reader->ExtractReel('P','+',0,0);
		hauteur = reader->ExtractReel('P','+',0,0);
		alignVert = reader->ExtractInt(0,0,3);
		dec = reader->ExtractReel('P','+',0,0);
		coulFond = reader->ExtractInt(0,0,MAX_COLOR);
		trameFond = reader->ExtractInt(0,0,100);
		chainage = reader->ExtractBooleen(kFalse);
		blocChaine = reader->ExtractTCLRef(kCurrentTCLRef);
		nbCol = reader->ExtractInt(1,1,MAX_COL);
		gout = reader->ExtractReel('D','+',3,0);
		retrait = reader->ExtractReel('D','+',0,0);
		angle = reader->ExtractInt(0,0,360);
		ancrage = reader->ExtractBooleen(kFalse);
		blocAncrage = reader->ExtractTCLRef(kCurrentTCLRef);
		relative = reader->ExtractBooleen(kFalse);

		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(0,0,MAX_PAGE);

		alignBloc = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);
		vJustMax = reader->ExtractReel('D','+',0,0);
		topAlign = reader->ExtractInt(0,0,4);
		
		reader->ExtractReel('D','+',0,0); // dummy parameter to ensure backward compatibility

		coulageAuto = reader->ExtractBooleen(kFalse);
		offset = reader->ExtractReel('P','+',0,0);
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
		IDCall_BN(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BN::~Func_BN()
{
}

/* IDCall_BN
*/
void Func_BN::IDCall_BN(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error("");
	UID newFrame;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Get helper interfaces
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BN::IDCall_BN -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);
		

		IDocument * doc = parser->getCurDoc();
		IDataBase * db = parser->getCurDataBase();

		// Define the frame bounding box
		PMPoint startPoint(hPos,vPos);	
		PMPoint endPoint(hPos + largeur,vPos + hauteur);	

		PMPointList points;
		points.push_back(startPoint);
		points.push_back(endPoint);
		
		UID ownerSpreadLayer = kInvalidUID;
		
		InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(doc));
		if(activeLayer == nil)
		{
			ASSERT_FAIL("Func_BN::IDCall_BN -> activeLayer nil");
			error = PMString(kErrNilInterface);		
			break;
		}
		
		if(!ancrage) // If the frame will be an inline -> no owner spread
		{
			if(relative == kFalse)
			{
				if(page == 0)
					page = parser->getCurPage();

#if COULAGE_AUTO
				if(coulageAuto)
				{
					points.clear();
					startPoint = kZeroPoint;
					Func_CDiese::GetPositionRelativeToColumn(doc, page, 0, offset, startPoint, largeur, ownerSpreadLayer);
					
					points.push_back(startPoint);
					points.push_back(PMPoint(startPoint.X()+largeur, startPoint.Y()+hauteur));
				}
				else
				{
#endif
				ownerSpreadLayer = itemManager->GetPBCoordAbs(doc,page,points,activeLayer);			
#if COULAGE_AUTO
				}
#endif
				if(ownerSpreadLayer == kInvalidUID)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
			else
			{
				if(ref == kCurrentTCLRef)
					ref = parser->getCurItem();

				UID refUID = parser->getItem(ref);
				
#if COULAGE_AUTO
				if(coulageAuto)
				{
					points.clear();
					startPoint = kZeroPoint;

					if(refUID == kInvalidUID)
					{
						page = parser->getCurPage();
						Func_CDiese::GetPositionRelativeToColumn(doc, page, 0, 0, startPoint, largeur, ownerSpreadLayer); // no offset
					}
					else
						Func_CDiese::GetPositionRelativeToItem(doc, refUID, offset, kFalse, startPoint, largeur, ownerSpreadLayer);
					
					points.push_back(startPoint);
					points.push_back(PMPoint(startPoint.X()+largeur, startPoint.Y()+hauteur));
				}
				else
				{
#endif
				if(refUID == kInvalidUID)
				{
					error = PMString(kErrInvalidItem);
					break;
				}

				ownerSpreadLayer = itemManager->GetPBCoordRel(UIDRef(db,refUID),points,alignBloc, activeLayer);

#if COULAGE_AUTO
				}
#endif
				if(ownerSpreadLayer == kInvalidUID)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
		}


		UIDRef txtFrameRef = itemManager->CreateTextFrameCmd(db, points, ownerSpreadLayer, nbCol, gout);
		if(txtFrameRef == UIDRef(nil,kInvalidUID))
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(this->ApplyTextFrameAttr(parser, txtFrameRef, error) != kSuccess)
			break;
		
#if !COMPOSER
		// check if the frame don't go out of spread
		//if(!ancrage && itemManager->CheckContainmentInOwnerSpread(frameGeo) != kSuccess)
		//{
		//	error = PMString(kErrItemOutOfSpread);
		//	break;
		//}
#endif
		// Select the bloc
		if((status = itemManager->SelectPageItem(txtFrameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		newFrame = txtFrameRef.GetUID();

	} while(false);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setItem(refBloc,newFrame);
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

ErrorCode Func_BN::ApplyTextFrameAttr(ITCLParser * parser, UIDRef& txtFrameRef, PMString& error)
{
	ErrorCode status = kFailure;
	do
	{
		IDataBase * db = parser->getCurDataBase();

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{					
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{					
			error = PMString(kErrNilInterface);
			break;
		}

		//Clean the attributeList and the values which concern style
		txtFrameHelper->Clean();

		InterfacePtr<IHierarchy> txtFrameHier (txtFrameRef,UseDefaultIID());
		if(txtFrameHier == nil)
		{
			ASSERT_FAIL("Func_BN::IDCall_BN -> txtFrameHier nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		// Set vertical justication and first line offset options, 
		InterfacePtr<IHierarchy> multiColumnItem (txtFrameHier->QueryChild(0)); //kMultiColumnItemBoss
		if(multiColumnItem == nil)
		{
			ASSERT_FAIL("Func_BN::IDCall_BN -> multiColumnItem nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		InterfacePtr<IMultiColumnTextFrame> txtFrameSetting (multiColumnItem,UseDefaultIID());
		if(txtFrameSetting == nil)
		{
			ASSERT_FAIL("Func_BN::IDCall_BN -> txtFrameSetting nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		Text::VerticalJustification vj;
		switch(alignVert)
		{
			case 0 : vj = Text::kVJTop; break;
			case 1 : vj = Text::kVJJustify; break;
			case 2 : vj = Text::kVJCenter; break;
			case 3 : vj = Text::kVJBottom; break;
			default : vj = Text::kVJInvalid; break;
		}
		
		Text::FirstLineOffsetMetric FLOm;
		switch (topAlign)
		{
			case 0 : FLOm = Text::kFLOAscent; break;
			case 1 : FLOm = Text::kFLOxHeight; break;
			case 2 : FLOm = Text::kFLOCapHeight; break;
			case 3 : FLOm = Text::kFLOLeading; break;
			case 4 : FLOm = Text::kFLOFixedHeight; break;
			default : FLOm = Text::kFLOInvalid; break;
		}

		txtFrameSetting->SetVerticalJustification(vj);
		txtFrameSetting->SetMaxVJInterParaSpace(vJustMax);
		txtFrameSetting->SetFirstLineOffsetMetric(FLOm);
		txtFrameSetting->SetMinFirstLineOffset(dec);
		
		// Set the frame inset
		if(txtFrameHelper->SetInsetCmd(txtFrameRef,retrait,retrait,retrait,retrait) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Set the color
		UID colorUID = parser->getColorList(coulFond).colorUID;
		if(colorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			colorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
		}

		InterfacePtr<IPersistUIDData> fillRenderAttr ((IPersistUIDData *)::CreateObject(kGraphicStyleFillRenderingAttrBoss,IID_IPERSISTUIDDATA));
		if(fillRenderAttr == nil)
		{
			ASSERT_FAIL("Func_BN::IDCall_BN -> fillRenderAttr nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		fillRenderAttr->SetUID(colorUID);

		InterfacePtr<ICommand> gfxApplyCmd(CmdUtils::CreateCommand(kGfxApplyAttrOverrideCmdBoss));
		if(gfxApplyCmd == nil)
		{
			ASSERT_FAIL("Func_BN::IDCall_BN -> gfxApplyCmd nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		gfxApplyCmd->SetItemList(UIDList(txtFrameRef));
		
		InterfacePtr<IPMUnknownData> iUnknown(gfxApplyCmd,UseDefaultIID());
		if(iUnknown == nil)
		{
			ASSERT_FAIL("Func_BN::IDCall_BN -> iUnknown nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		iUnknown->SetPMUnknown(fillRenderAttr);

		if(CmdUtils::ProcessCommand(gfxApplyCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		// Set the tint (other method than this used to set the color : IGraphicAttributeUtils)
		UIDList frameList(txtFrameRef);
		InterfacePtr<ICommand> fillTintCmd (Utils<IGraphicAttributeUtils>()->CreateFillTintCommand(trameFond,&frameList,kTrue,kTrue));
		if(fillTintCmd == nil)
		{
			ASSERT_FAIL("Func_BN::IDCall_BN -> fillTintCmd nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		if(CmdUtils::ProcessCommand(fillTintCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		if(ancrage)
		{
			if(blocAncrage == kCurrentTCLRef)
				blocAncrage = parser->getCurItem();

			UID blocAncrageUID = parser->getItem(blocAncrage);
			if(blocAncrageUID == kInvalidUID)
			{
				error = PMString(kErrInvalidItem);
				break;
			}
			
			if(itemTransform->MakeInlineCmd(txtFrameRef,UIDRef(db,blocAncrageUID)) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			// Recompose the frame that receive the inline
			InterfacePtr<IHierarchy> blocAncrageHier (UIDRef(db,blocAncrageUID),UseDefaultIID());
			if(blocAncrageHier == nil)
			{
				error = PMString(kErrNilInterface);		
				break;
			}

			InterfacePtr<IMultiColumnTextFrame> multiColAncr (db, blocAncrageHier->GetChildUID(0), UseDefaultIID());
			if(multiColAncr == nil)
			{
				error = PMString(kErrNilInterface);		
				break;
			}

			txtFrameHelper->RecomposeWholeStory(multiColAncr);
			
		}
		else // Can't link inlines
		{
			if(chainage) // v￩rifier si erreur command qd un des blocs n'est pas texte
			{
				if(blocChaine == kCurrentTCLRef)
					blocChaine = parser->getCurItem();

				InterfacePtr<IHierarchy> firstFrameHier (db,parser->getItem(blocChaine),UseDefaultIID());
				if(firstFrameHier == nil)
				{
					ASSERT_FAIL("Func_BN::IDCall_BN -> firstFrameHier nil");
					error = PMString(kErrInvalidItem);	
					break;
				}

				InterfacePtr<IHierarchy> firstMultiCol (firstFrameHier->QueryChild(0));
				if(firstMultiCol == nil) // Not a text frame
				{
					ASSERT_FAIL("Func_BN::IDCall_BN -> firstMultiCol nil");
					error = PMString(kErrTextBoxRequired);		
					break;
				}
			
				if(txtFrameHelper->LinkFramesCmd(::GetUIDRef(firstMultiCol),::GetUIDRef(multiColumnItem)) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}

				// Recompose the story
				InterfacePtr<IMultiColumnTextFrame> txtFrame (firstMultiCol,UseDefaultIID());
				if(txtFrame == nil)
				{
					ASSERT_FAIL("Func_BN::IDCall_BN -> txtFrame nil");
					error = PMString(kErrNilInterface);		
					break;
				}
			}
		}

		InterfacePtr<IGeometry> frameGeo (txtFrameRef,UseDefaultIID());
		if(frameGeo == nil)
		{
			ASSERT_FAIL("Func_BN::IDCall_BN -> frameGeo nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		if(angle  != 0)
		{
			// Rotate the frame
			if(itemTransform->Rotate(txtFrameRef,PMReal(angle),frameGeo->GetStrokeBoundingBox().GetCenter(), !ancrage) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		status = kSuccess;

	} while(kFalse);

	return status;
}

/* Constructor
*/
Func_BC::Func_BC(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BC::Func_BC -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		graisse = reader->ExtractReel('D','+',1,0);
		type = reader->ExtractInt(0,0,18);
		coulFilet = reader->ExtractInt(5,0,MAX_COLOR);
		trameFilet = reader->ExtractPourcent('+',100,0);
		forme = reader->ExtractInt(0,0,5);
		taille = reader->ExtractReel('D','+',12,0);

		// dummy parameters to ensure backward compatibility
		reader->ExtractInt(0,0,255);
		coulGap = reader->ExtractInt(9,0,MAX_COLOR);
		trameGap = reader->ExtractPourcent('+',100,0);
		strokeAlignment = reader->ExtractInt(0, 0, 2);

	/*	if(taille < 0.0 || taille > 800.0)
			throw TCLError(...);
	*/
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
		IDCall_BC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BC::~Func_BC()
{
}


/* IDCall_BC
*/
void Func_BC::IDCall_BC(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Get helper interfaces
		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_BC::IDCall_BC -> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BC::IDCall_BC -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{
			ASSERT_FAIL("Func_BC::IDCall_BC -> seq nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();
		
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);

		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDList frameList(db,refUID);
	
		InterfacePtr<ICommand> gfxApplyMultAttributesCmd(CmdUtils::CreateCommand(kGfxApplyMultAttributesCmdBoss));
		if(gfxApplyMultAttributesCmd == nil)
		{
			ASSERT_FAIL("Func_BC::IDCall_BC -> gfxApplyMultAttributesCmd nil");
			error = PMString(kErrNilInterface);		
			break;
		}
		
		InterfacePtr<IApplyMultAttributesCmdData> cmdData(gfxApplyMultAttributesCmd, UseDefaultIID());
		if(cmdData == nil)
		{
			ASSERT_FAIL("Func_BC::IDCall_BC -> cmdData nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		// Set the type of the stroke
		ClassID lineImpl = kInvalidClass;
		switch(type)
		{
			case 0 : lineImpl = kSolidPathStrokerBoss; break; 
			case 1 : lineImpl = kDashedPathStrokerBoss; break; 
			case 2 : lineImpl = kThinThinPathStrokerBoss; break; 
			case 3 : lineImpl = kThickThickPathStrokerBoss; break; 
			case 4 : lineImpl = kDashedPathStrokerBoss; //
					 // Set dashs and gaps values to so simulate line-dash type	
					 {
						InterfacePtr<IDashedAttributeValues> dashAttr ((IDashedAttributeValues *)::CreateObject(kDashedAttributeValuesBoss,IID_IDASHEDATTRIBUTEVALUES));
						if(dashAttr == nil)
						{
							ASSERT_FAIL("Func_BC::IDCall_BC -> dashAttr nil");
							error = PMString(kErrNilInterface);		
							break;
						}

						dashAttr->SetValue(0,12.0); // dash 0
						dashAttr->SetValue(1,3.0); // gap 0
						dashAttr->SetValue(2,3.0); // dash 1
						dashAttr->SetValue(3,3.0); // gap 1

						cmdData->AddAnAttribute(dashAttr);
					 }
					 break;

			case 5 : lineImpl = kCannedDotPathStrokerBoss; break; 
			case 6 : lineImpl = kThinThickPathStrokerBoss; break;
			case 7 : lineImpl = kThickThinPathStrokerBoss; break; 
			case 8 : lineImpl = kThinThickThinPathStrokerBoss; break; 
			case 9 : lineImpl = kThickThinThickPathStrokerBoss; break; 
			case 10 : lineImpl = kTriplePathStrokerBoss; break; 
			case 11 : lineImpl = kCannedDash3x2PathStrokerBoss; break;
			case 12 : lineImpl = kCannedDash4x4PathStrokerBoss; break;
			case 13 : lineImpl = kLeftSlantHashPathStrokerBoss; break;
			case 14 : lineImpl = kRightSlantHashPathStrokerBoss; break;
			case 15 : lineImpl = kStraightHashPathStrokerBoss; break;
			case 16 : lineImpl = kSingleWavyPathStrokerBoss; break;
			case 17 : lineImpl = kWhiteDiamondPathStrokerBoss; break;
			case 18 : lineImpl = kJapaneseDotsPathStrokerBoss; break;				
			default : break;
		}

		InterfacePtr<IGraphicAttrClassID> lineImplAttr (Utils<IGraphicAttributeUtils>()->CreateLineImplementationAttribute(lineImpl,kInvalidUID));
		if(lineImplAttr == nil)
		{
			ASSERT_FAIL("Func_BC::IDCall_BC -> lineImplAttr nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		cmdData->AddAnAttribute(lineImplAttr);

		// Set the stroke tint
		InterfacePtr<IGraphicAttrRealNumber> strokeTintAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeTintAttribute(trameFilet*100));
		if(strokeTintAttr == nil)
		{
			ASSERT_FAIL("Func_BC::IDCall_BC -> strokeTintAttr nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		cmdData->AddAnAttribute(strokeTintAttr);

		// Set the stroke weight
		InterfacePtr<IGraphicAttrRealNumber> strokeWeightAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeWeightAttribute(graisse));
		if(strokeWeightAttr == nil)
		{
			ASSERT_FAIL("Func_BC::IDCall_BC -> strokeWeightAttr nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		cmdData->AddAnAttribute(strokeWeightAttr);

		// Set the corner implementation
		ClassID cornerImpl = kInvalidClass;
		switch(forme)
		{
			case 1 : cornerImpl = kRoundedCornerBoss; break;
			case 2 : cornerImpl = kInverseRoundedCornerBoss; break; 
			case 3 : cornerImpl = kBevelCornerBoss; break;
			case 4 : cornerImpl = kInsetCornerBoss; break;
			case 5 : cornerImpl = kFancyCornerBoss; break;
			default : break;
		}

        // GD 28.06.2019 ++
        /*
		InterfacePtr<IGraphicAttrClassID> cornerImplAttr (Utils<IGraphicAttributeUtils>()->CreateCornerImplementationAttribute(cornerImpl));
		if(cornerImplAttr == nil)
		{
			ASSERT_FAIL("Func_BC::IDCall_BC -> cornerImplAttr nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		cmdData->AddAnAttribute(cornerImplAttr);

		// Set the corner radius
		InterfacePtr<IGraphicAttrRealNumber> cornerRadiusAttr (Utils<IGraphicAttributeUtils>()->CreateCornerRadiusAttribute(taille));
		if(cornerRadiusAttr == nil)
		{
			ASSERT_FAIL("Func_BC::IDCall_BC -> cornerRadiusAttr nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		cmdData->AddAnAttribute(cornerRadiusAttr);
        */
        InterfacePtr<IGraphicAttrClassID> cornerImplAttrTopLeft (Utils<IGraphicAttributeUtils>()->CreateCornerImplementationAttribute(cornerImpl, IGraphicAttributeUtils::kLiveTopLeftCorner));
        if(cornerImplAttrTopLeft == nil)
        {
            ASSERT_FAIL("Func_BC::IDCall_BC -> cornerImplAttrTopLeft nil");
            error = PMString(kErrNilInterface);
            break;
        }
        cmdData->AddAnAttribute(cornerImplAttrTopLeft);
        
        InterfacePtr<IGraphicAttrRealNumber> cornerRadiusAttrTopLeft (Utils<IGraphicAttributeUtils>()->CreateCornerRadiusAttribute(taille, IGraphicAttributeUtils::kLiveTopLeftCorner));
        if(cornerRadiusAttrTopLeft == nil)
        {
            ASSERT_FAIL("Func_BC::IDCall_BC -> cornerRadiusAttrTopLeft nil");
            error = PMString(kErrNilInterface);
            break;
        }
        cmdData->AddAnAttribute(cornerRadiusAttrTopLeft);
        
        InterfacePtr<IGraphicAttrClassID> cornerImplAttrTopRight (Utils<IGraphicAttributeUtils>()->CreateCornerImplementationAttribute(cornerImpl, IGraphicAttributeUtils::kLiveTopRightCorner));
        if(cornerImplAttrTopRight == nil)
        {
            ASSERT_FAIL("Func_BC::IDCall_BC -> cornerImplAttrTopRight nil");
            error = PMString(kErrNilInterface);
            break;
        }
        cmdData->AddAnAttribute(cornerImplAttrTopRight);
        
        InterfacePtr<IGraphicAttrRealNumber> cornerRadiusAttrTopRight (Utils<IGraphicAttributeUtils>()->CreateCornerRadiusAttribute(taille, IGraphicAttributeUtils::kLiveTopRightCorner));
        if(cornerRadiusAttrTopRight == nil)
        {
            ASSERT_FAIL("Func_BC::IDCall_BC -> cornerRadiusAttrTopRight nil");
            error = PMString(kErrNilInterface);
            break;
        }
        cmdData->AddAnAttribute(cornerRadiusAttrTopRight);
        
        InterfacePtr<IGraphicAttrClassID> cornerImplAttrBotLeft (Utils<IGraphicAttributeUtils>()->CreateCornerImplementationAttribute(cornerImpl, IGraphicAttributeUtils::kLiveBottomLeftCorner));
        if(cornerImplAttrBotLeft == nil)
        {
            ASSERT_FAIL("Func_BC::IDCall_BC -> cornerImplAttrBotLeft nil");
            error = PMString(kErrNilInterface);
            break;
        }
        cmdData->AddAnAttribute(cornerImplAttrBotLeft);
        
        InterfacePtr<IGraphicAttrRealNumber> cornerRadiusAttrBotLeft (Utils<IGraphicAttributeUtils>()->CreateCornerRadiusAttribute(taille, IGraphicAttributeUtils::kLiveBottomLeftCorner));
        if(cornerRadiusAttrBotLeft == nil)
        {
            ASSERT_FAIL("Func_BC::IDCall_BC -> cornerRadiusAttrBotLeft nil");
            error = PMString(kErrNilInterface);
            break;
        }
        cmdData->AddAnAttribute(cornerRadiusAttrBotLeft);
        
        InterfacePtr<IGraphicAttrClassID> cornerImplAttrBotRight (Utils<IGraphicAttributeUtils>()->CreateCornerImplementationAttribute(cornerImpl, IGraphicAttributeUtils::kLiveBottomRightCorner));
        if(cornerImplAttrBotRight == nil)
        {
            ASSERT_FAIL("Func_BC::IDCall_BC -> cornerImplAttrBotRight nil");
            error = PMString(kErrNilInterface);
            break;
        }
        cmdData->AddAnAttribute(cornerImplAttrBotRight);
        
        InterfacePtr<IGraphicAttrRealNumber> cornerRadiusAttrBotRight (Utils<IGraphicAttributeUtils>()->CreateCornerRadiusAttribute(taille, IGraphicAttributeUtils::kLiveBottomRightCorner));
        if(cornerRadiusAttrBotRight == nil)
        {
            ASSERT_FAIL("Func_BC::IDCall_BC -> cornerRadiusAttrBotRight nil");
            error = PMString(kErrNilInterface);
            break;
        }
        cmdData->AddAnAttribute(cornerRadiusAttrBotRight);
        // GD 28.06.2019 --

		// Gap color & tint
		UID gapColorUID = parser->getColorList(coulGap).colorUID;
		if(gapColorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			gapColorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
		}

		InterfacePtr<IPersistUIDData> gapColorAttr (Utils<IGraphicAttributeUtils>()->CreateGapRenderingAttribute(gapColorUID));
		cmdData->AddAnAttribute(gapColorAttr);

		InterfacePtr<IGraphicAttrRealNumber> gapTintAttr (Utils<IGraphicAttributeUtils>()->CreateGapTintAttribute(trameGap*100));
		cmdData->AddAnAttribute(gapTintAttr);		 
		
        // Set itemList and process main command
		gfxApplyMultAttributesCmd->SetItemList(frameList);
		if(CmdUtils::ProcessCommand(gfxApplyMultAttributesCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		// Set the stroke color
        UID colorUID;
    if (graisse > 0) { // GD 28.06.2019
		colorUID = parser->getColorList(coulFilet).colorUID;
		if(colorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			colorUID = Utils<ISwatchUtils>()->GetBlackSwatchUID(db); // color doesn't exist, replaced with black
		}
     // GD 28.06.2019 ++
    } else {
        colorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db);
    }
    // GD 28.06.2019 --

		InterfacePtr<ICommand> iStrokeRenderCmd (Utils<IGraphicAttributeUtils>()->CreateStrokeRenderingCommand(colorUID,&frameList,kTrue,kTrue));
		if(iStrokeRenderCmd == nil)
		{
			ASSERT_FAIL("Func_BC::IDCall_BC -> iStrokeRenderCmd nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		if(CmdUtils::ProcessCommand(iStrokeRenderCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		// Set stroke alignment
		InterfacePtr<ICommand> strokeAlignCmd (Utils<IGraphicAttributeUtils>()->CreateStrokeAlignmentCommand(strokeAlignment, &frameList, kTrue, kTrue));		
		if(CmdUtils::ProcessCommand(strokeAlignCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		// Select the item
		if((status = itemManager->SelectPageItem(UIDRef(db,refUID))) != kSuccess)
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
Func_BH::Func_BH(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BH::Func_BH -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		type = reader->ExtractInt(0,0,11);

		if(type != 1)
		{
			haut = reader->ExtractReel('D','+',0,0);
			gauche = reader->ExtractReel('D','+',0,0);
			bas = reader->ExtractReel('D','+',0,0);
			droit = reader->ExtractReel('D','+',0,0);
			invert = reader->ExtractBooleen(kFalse);
		}
		else
		{
			invert = kFalse;
			haut = gauche = bas = droit = 0.0;
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
		IDCall_BH(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_BH::~Func_BH()
{
}


/* IDCall_BH
*/
void Func_BH::IDCall_BH(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Get helper interfaces
		InterfacePtr<IItemTransform> itemTransform (parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_BH::IDCall_BH -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BH::IDCall_BH -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BH::IDCall_BH -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();
		
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		IStandOff::mode mode = IStandOff::kBothSides;
		IStandOffContourWrapSettings::ContourWrapType contourType = IStandOffContourWrapSettings::kInvalid;
		switch(type)
		{
			case 0 : mode = IStandOff::kBothSides; break; // Wrap around bounding box
			case 1 : mode = IStandOff::kNone; break; // No Text Wrap			
			case 2 : // Wrap around object shape (image shape if it's an image frame)
				mode = IStandOff::kManualContour;
				contourType = IStandOffContourWrapSettings::kGraphicBounds;
				break; 
			case 3 : 
				mode = IStandOff::kManualContour; 
				contourType = IStandOffContourWrapSettings::kEmbeddedPath;
				break;
			case 4 :
				mode = IStandOff::kManualContour; 
				contourType = IStandOffContourWrapSettings::kAlpha;
				break;
			case 5 :
			case 8 :
				mode = IStandOff::kManualContour; 
				contourType = IStandOffContourWrapSettings::kEdgeDetection;
				break;
			
			case 6 : 
			case 7 :
			case 9 :
				mode = IStandOff::kManualContour; 
				contourType = IStandOffContourWrapSettings::kSameAsClip;
				break;

			case 10 : mode = IStandOff::kBand; break; // Jump Object
			case 11 : mode = IStandOff::kNextFrame; break; // Jump to next column
			default : break;
		}

		if(itemTransform->SetStandoffCmd(UIDRef(db,parser->getItem(refBloc)),
											   mode, invert, gauche, haut, droit, bas, contourType) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if((status = itemManager->SelectPageItem(UIDRef(db,parser->getItem(refBloc)))) != kSuccess)
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
Func_BJ::Func_BJ(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BJ::Func_BJ -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
    {
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		haut = reader->ExtractReel('D','+',0,0);
		gauche = reader->ExtractReel('D','+',0,0);
		bas = reader->ExtractReel('D','+',0,0);
		droit = reader->ExtractReel('D','+',0,0);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error){
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
		IDCall_BJ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_BJ::~Func_BJ()
{
}


/* IDCall_BJ
*/
void Func_BJ::IDCall_BJ(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Get helper interfaces
		InterfacePtr<ITextFrameHelper> txtFrameHelper (parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_BJ::IDCall_BJ -> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BJ::IDCall_BJ -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BJ::IDCall_BJ -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);
		IDataBase * db = parser->getCurDataBase();

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
		
		InterfacePtr<IHierarchy> hier (db,parser->getItem(refBloc),UseDefaultIID());
		if(hier == nil)
		{		
			ASSERT_FAIL("Func_BJ::IDCall_BJ -> hier nil");
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IHierarchy> multi (hier->QueryChild(0));
		if(multi == nil) // not a text frame
		{		
			error = PMString(kErrTextBoxRequired);
			break;
		}

		InterfacePtr<IMultiColumnTextFrame> txtFrame (multi,UseDefaultIID());

		if(txtFrame == nil) // not a text frame
		{		
			error = PMString(kErrTextBoxRequired);
			break;
		}

		if(txtFrameHelper->SetInsetCmd(UIDRef(db,parser->getItem(refBloc)),gauche, haut, droit, bas) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		// Recompose the story
		/*if(txtFrameHelper->RecomposeWholeStory(txtFrame) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}*/

		// Select the text frame
		if((status = itemManager->SelectPageItem(UIDRef(db,parser->getItem(refBloc)))) != kSuccess)
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
Func_FH::Func_FH(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_FH::Func_FH -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		graisse = reader->ExtractReel('D','+',0,0);
		largeur = reader->ExtractReel('P','+',0,0);
		typeFilet = reader->ExtractInt(0,0,18);
		coulFilet = reader->ExtractInt(5,0,MAX_COLOR);
		trameFilet = reader->ExtractInt(100,0,100);
		relative = reader->ExtractBooleen(kFalse);
		
		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(0,0,MAX_PAGE);

		align = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);

		typeFleche = reader->ExtractInt(0,0,5);
		strokeAlign = reader->ExtractInt(0,0,2);
		coulageAuto = reader->ExtractBooleen(kFalse);
		offset = reader->ExtractReel('P','+',0,0);
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
		IDCall_FH(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_FH::~Func_FH()
{
}


/* IDCall_FH
*/
void Func_FH::IDCall_FH(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");
	UID newRule;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Get helper interface
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();

		// Define the frame bounding box
		PMPoint startPoint(hPos,vPos);	
		PMPoint endPoint(hPos + largeur,vPos);	

		PMPointList points;
		points.push_back(startPoint);
		points.push_back(endPoint);
		
		UID ownerSpreadLayer = kInvalidUID;
		
		InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(parser->getCurDoc()));
		if(activeLayer == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> activeLayer nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if(relative == kFalse)
		{
			if(page == 0)
				page = parser->getCurPage();

#if COULAGE_AUTO
				if(coulageAuto)
				{
					points.clear();
					startPoint = kZeroPoint;
					Func_CDiese::GetPositionRelativeToColumn(parser->getCurDoc(), page, 0, offset, startPoint, largeur, ownerSpreadLayer);
					
					points.push_back(startPoint);
					points.push_back(PMPoint(startPoint.X()+largeur, startPoint.Y()));
				}
				else
				{
#endif
				ownerSpreadLayer = itemManager->GetPBCoordAbs(parser->getCurDoc(),page,points,activeLayer);		
#if COULAGE_AUTO
				}
#endif
			
			if(ownerSpreadLayer == kInvalidUID)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			if(ref == kCurrentTCLRef)
				ref = parser->getCurItem();

			UID refUID = parser->getItem(ref);			

#if COULAGE_AUTO
				if(coulageAuto)
				{
					points.clear();
					startPoint = kZeroPoint;

					if(refUID == kInvalidUID)
					{
						page = parser->getCurPage();
						Func_CDiese::GetPositionRelativeToColumn(parser->getCurDoc(), page, 0, 0, startPoint, largeur, ownerSpreadLayer);
					}
					else
						Func_CDiese::GetPositionRelativeToItem(parser->getCurDoc(), refUID, offset, kFalse, startPoint, largeur, ownerSpreadLayer);
					
					points.push_back(startPoint);
					points.push_back(PMPoint(startPoint.X()+largeur, startPoint.Y()));
				}
				else
				{
#endif
				if(refUID == kInvalidUID)
				{
					error = PMString(kErrInvalidItem);
					break;
				}

				ownerSpreadLayer = itemManager->GetPBCoordRel(UIDRef(db,refUID),points,align,activeLayer);
#if COULAGE_AUTO
				}
#endif

			if(ownerSpreadLayer == kInvalidUID)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		UIDRef ruleRef = Utils<IPathUtils>()->CreateLineSpline(UIDRef(db,ownerSpreadLayer), points, INewPageItemCmdData::kDefaultGraphicAttributes);

		if(ruleRef == UIDRef(nil,kInvalidUID))
			break;

		InterfacePtr<IHierarchy> ruleHier (ruleRef,UseDefaultIID());
		if(ruleHier == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> ruleHier nil");
			error = PMString(kErrNilInterface);
			break;
		}

		newRule = ruleRef.GetUID();

		UIDList ruleList(ruleRef);

		InterfacePtr<ICommand> gfxApplyMultAttributesCmd(CmdUtils::CreateCommand(kGfxApplyMultAttributesCmdBoss));
		if(gfxApplyMultAttributesCmd == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> gfxApplyMultAttributesCmd nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		InterfacePtr<IApplyMultAttributesCmdData> cmdData(gfxApplyMultAttributesCmd, UseDefaultIID());
		if(cmdData == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> cmdData nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Set the stroke color
		UID colorUID = parser->getColorList(coulFilet).colorUID;
		if(colorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			colorUID = Utils<ISwatchUtils>()->GetBlackSwatchUID(db); // color doesn't exist, replaced with black
		}

		InterfacePtr<ICommand> iStrokeRenderCmd (Utils<IGraphicAttributeUtils>()->CreateStrokeRenderingCommand(colorUID,&ruleList,kTrue,kTrue));
		if(iStrokeRenderCmd == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> iStrokeRenderCmd nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if(CmdUtils::ProcessCommand(iStrokeRenderCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}
		
		// Set the type of the stroke
		ClassID lineImpl = kInvalidClass;
		switch(typeFilet)
		{
			case 0 : lineImpl = kSolidPathStrokerBoss; break;
			case 1 : lineImpl = kDashedPathStrokerBoss; break;
			case 2 : lineImpl = kThinThinPathStrokerBoss; break;
			case 3 : lineImpl = kThickThickPathStrokerBoss; break;
			case 4 : lineImpl = kDashedPathStrokerBoss; 
					 // Set dashs and gaps values to so simulate line-dash type	
					 {
						InterfacePtr<IDashedAttributeValues> dashAttr ((IDashedAttributeValues *)::CreateObject(kDashedAttributeValuesBoss,IID_IDASHEDATTRIBUTEVALUES));
						if(dashAttr == nil)
						{		
							ASSERT_FAIL("Func_FH::IDCall_FH -> dashAttr nil");
							error = PMString(kErrNilInterface);
							break;
						}

						dashAttr->SetValue(0,12.0); // dash 0
						dashAttr->SetValue(1,3.0); // gap 0
						dashAttr->SetValue(2,3.0); // dash 1
						dashAttr->SetValue(3,3.0); // gap 1

						cmdData->AddAnAttribute(dashAttr);
					 }
					 break;

			case 5 : lineImpl = kCannedDotPathStrokerBoss; break;
			case 6 : lineImpl = kThinThickPathStrokerBoss; break;
			case 7 : lineImpl = kThickThinPathStrokerBoss; break;
			case 8 : lineImpl = kThinThickThinPathStrokerBoss; break;
			case 9 : lineImpl = kThickThinThickPathStrokerBoss; break;
			case 10 : lineImpl = kTriplePathStrokerBoss; break;
			case 11 : lineImpl = kCannedDash3x2PathStrokerBoss; break;
			case 12 : lineImpl = kCannedDash4x4PathStrokerBoss; break;
			case 13 : lineImpl = kLeftSlantHashPathStrokerBoss; break;
			case 14 : lineImpl = kRightSlantHashPathStrokerBoss; break;
			case 15 : lineImpl = kStraightHashPathStrokerBoss; break;
			case 16 : lineImpl = kSingleWavyPathStrokerBoss; break;
			case 17 : lineImpl = kWhiteDiamondPathStrokerBoss; break;
			case 18 : lineImpl = kJapaneseDotsPathStrokerBoss; break;
			default : break;
		}

		InterfacePtr<IGraphicAttrClassID> lineImplAttr (Utils<IGraphicAttributeUtils>()->CreateLineImplementationAttribute(lineImpl,kInvalidUID));
		if(lineImplAttr == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> lineImplAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(lineImplAttr);

		// Set the stroke tint
		InterfacePtr<IGraphicAttrRealNumber> strokeTintAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeTintAttribute(trameFilet));
		if(strokeTintAttr == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> strokeTintAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(strokeTintAttr);

		// Set the stroke weight
		InterfacePtr<IGraphicAttrRealNumber> strokeWeightAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeWeightAttribute(graisse));
		if(strokeWeightAttr == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> strokeWeightAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(strokeWeightAttr);

		// Set the line end and start
		ClassID lineEndStart = kInvalidClass, lineEndEnd = kInvalidClass;
		switch(typeFleche)
		{
			case 1 : lineEndEnd = kCurvedArrowHeadBoss; break;
			case 2 : lineEndStart = kCurvedArrowHeadBoss; break;
			case 3 : lineEndStart = kBarArrowHeadBoss; lineEndEnd = kCurvedArrowHeadBoss; break;
			case 4 : lineEndStart = kCurvedArrowHeadBoss; lineEndEnd = kBarArrowHeadBoss; break;
			case 5 : lineEndStart = kCurvedArrowHeadBoss; lineEndEnd = kCurvedArrowHeadBoss; break;
			default : break;
		}

		InterfacePtr<IGraphicAttrClassID> lineEndStartImpl(Utils<IGraphicAttributeUtils>()->CreateLineEndStartImplementationAttribute(lineEndStart));
		if(lineEndStartImpl == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> lineEndStartImpl nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(lineEndStartImpl);

		InterfacePtr<IGraphicAttrClassID> lineEndEndImpl(Utils<IGraphicAttributeUtils>()->CreateLineEndEndImplementationAttribute(lineEndEnd));
		if(lineEndEndImpl == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> lineEndEndImpl nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(lineEndEndImpl);

		// Set stroke alignment
		InterfacePtr<IGraphicAttrInt32> strokeAlignAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeAlignmentAttribute(strokeAlign));		
		if(strokeAlignAttr == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(strokeAlignAttr);

		gfxApplyMultAttributesCmd->SetItemList(ruleList);
		if(CmdUtils::ProcessCommand(gfxApplyMultAttributesCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		// check if the rule don't go out of spread
		InterfacePtr<IGeometry> ruleGeo (ruleRef,UseDefaultIID());
		if(ruleGeo == nil)
		{		
			ASSERT_FAIL("Func_FH::IDCall_FH -> ruleGeo nil");
			error = PMString(kErrNilInterface);
			break;
		}
#if !COMPOSER
		//if(itemManager->CheckContainmentInOwnerSpread(ruleGeo) != kSuccess)
		//{
		//	error = PMString(kErrItemOutOfSpread);
		//	break;
		//}
#endif
		// Select the rule
		if((status = itemManager->SelectPageItem(ruleRef)) != kSuccess)
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

			parser->setItem(refBloc,newRule);
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
Func_FV::Func_FV(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_FV::Func_FV -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		graisse = reader->ExtractReel('D','+',0,0);
		hauteur = reader->ExtractReel('P','+',0,0);
		typeFilet = reader->ExtractInt(0,0,18);
		coulFilet = reader->ExtractInt(5,0,MAX_COLOR);
		trameFilet = reader->ExtractInt(100,0,100);
		relative = reader->ExtractBooleen(kFalse);
		
		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(0,0,MAX_PAGE);

		align = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);

		typeFleche = reader->ExtractInt(0,0,5);
		strokeAlign = reader->ExtractInt(0,0,2);
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
		IDCall_FV(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_FV::~Func_FV()
{
}


/* IDCall_FV
*/
void Func_FV::IDCall_FV(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");
	UID newRule;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Get helper interfaces
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();

		// Define the frame bounding box
		PMPoint startPoint(hPos,vPos);	
		PMPoint endPoint(hPos,vPos + hauteur);	

		PMPointList points;
		points.push_back(startPoint);
		points.push_back(endPoint);

		UID ownerSpreadLayer = kInvalidUID;
		
		InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(parser->getCurDoc()));
		if(activeLayer == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> activeLayer nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(relative == kFalse)
		{
			if(page == 0)
				page = parser->getCurPage();

			ownerSpreadLayer = itemManager->GetPBCoordAbs(parser->getCurDoc(),page,points,activeLayer);
			
			if(ownerSpreadLayer == kInvalidUID)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			if(ref == kCurrentTCLRef)
				ref = parser->getCurItem();

			UID refUID = parser->getItem(ref);
			if(refUID == kInvalidUID)
			{
				error = PMString(kErrInvalidItem);
				break;
			}
	
			ownerSpreadLayer = itemManager->GetPBCoordRel(UIDRef(db,refUID),points,align,activeLayer);
			if(ownerSpreadLayer == kInvalidUID)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		UIDRef ruleRef = Utils<IPathUtils>()->CreateLineSpline(UIDRef(db,ownerSpreadLayer), points, INewPageItemCmdData::kDefaultGraphicAttributes);

		if(ruleRef == UIDRef(nil,kInvalidUID))
			break;

		InterfacePtr<IHierarchy> ruleHier (ruleRef,UseDefaultIID());
		if(ruleHier == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> ruleHier nil");
			error = PMString(kErrNilInterface);
			break;
		}

		newRule = ruleRef.GetUID();

		UIDList ruleList(ruleRef);

		InterfacePtr<ICommand> gfxApplyMultAttributesCmd(CmdUtils::CreateCommand(kGfxApplyMultAttributesCmdBoss));
		if(gfxApplyMultAttributesCmd == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> gfxApplyMultAttributesCmd nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		InterfacePtr<IApplyMultAttributesCmdData> cmdData(gfxApplyMultAttributesCmd, UseDefaultIID());
		if(cmdData == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> cmdData nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Set the stroke color
		UID colorUID = parser->getColorList(coulFilet).colorUID;
		if(colorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			colorUID = Utils<ISwatchUtils>()->GetBlackSwatchUID(db); // color doesn't exist, replaced with black
		}

		InterfacePtr<ICommand> iStrokeRenderCmd (Utils<IGraphicAttributeUtils>()->CreateStrokeRenderingCommand(colorUID,&ruleList,kTrue,kTrue));
		if(iStrokeRenderCmd == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> iStrokeRenderCmd nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if(CmdUtils::ProcessCommand(iStrokeRenderCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		// Set the type of the stroke
		ClassID lineImpl = kInvalidClass;
		switch(typeFilet)
		{
			case 0 : lineImpl = kSolidPathStrokerBoss; break;
			case 1 : lineImpl = kDashedPathStrokerBoss; break;
			case 2 : lineImpl = kThinThinPathStrokerBoss; break;
			case 3 : lineImpl = kThickThickPathStrokerBoss; break;
			case 4 : lineImpl = kDashedPathStrokerBoss; 
					 // Set dashs and gaps values to so simulate line-dash type	
					 {
						InterfacePtr<IDashedAttributeValues> dashAttr ((IDashedAttributeValues *)::CreateObject(kDashedAttributeValuesBoss,IID_IDASHEDATTRIBUTEVALUES));
						if(dashAttr == nil)
						{		
							ASSERT_FAIL("Func_FV::IDCall_FV -> dashAttr nil");
							error = PMString(kErrNilInterface);
							break;
						}

						dashAttr->SetValue(0,12.0); // dash 0
						dashAttr->SetValue(1,3.0); // gap 0
						dashAttr->SetValue(2,3.0); // dash 1
						dashAttr->SetValue(3,3.0); // gap 1

						cmdData->AddAnAttribute(dashAttr);
					 }
					 break;

			case 5 : lineImpl = kCannedDotPathStrokerBoss; break;
			case 6 : lineImpl = kThinThickPathStrokerBoss; break;
			case 7 : lineImpl = kThickThinPathStrokerBoss; break;
			case 8 : lineImpl = kThinThickThinPathStrokerBoss; break;
			case 9 : lineImpl = kThickThinThickPathStrokerBoss; break;
			case 10 : lineImpl = kTriplePathStrokerBoss; break;
			case 11 : lineImpl = kCannedDash3x2PathStrokerBoss; break;
			case 12 : lineImpl = kCannedDash4x4PathStrokerBoss; break;
			case 13 : lineImpl = kLeftSlantHashPathStrokerBoss; break;
			case 14 : lineImpl = kRightSlantHashPathStrokerBoss; break;
			case 15 : lineImpl = kStraightHashPathStrokerBoss; break;
			case 16 : lineImpl = kSingleWavyPathStrokerBoss; break;
			case 17 : lineImpl = kWhiteDiamondPathStrokerBoss; break;
			case 18 : lineImpl = kJapaneseDotsPathStrokerBoss; break;
			default : break;
		}

		InterfacePtr<IGraphicAttrClassID> lineImplAttr (Utils<IGraphicAttributeUtils>()->CreateLineImplementationAttribute(lineImpl,kInvalidUID));
		if(lineImplAttr == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> lineImplAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(lineImplAttr);

		// Set the stroke tint
		InterfacePtr<IGraphicAttrRealNumber> strokeTintAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeTintAttribute(trameFilet));
		if(strokeTintAttr == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> strokeTintAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(strokeTintAttr);

		// Set the stroke weight
		InterfacePtr<IGraphicAttrRealNumber> strokeWeightAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeWeightAttribute(graisse));
		if(strokeWeightAttr == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> strokeWeightAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(strokeWeightAttr);

		// Set the line end and start
		ClassID lineEndStart = kInvalidClass, lineEndEnd = kInvalidClass;
		switch(typeFleche)
		{
			case 1 : lineEndEnd = kCurvedArrowHeadBoss; break;
			case 2 : lineEndStart = kCurvedArrowHeadBoss; break;
			case 3 : lineEndStart = kBarArrowHeadBoss; lineEndEnd = kCurvedArrowHeadBoss; break;
			case 4 : lineEndStart = kCurvedArrowHeadBoss; lineEndEnd = kBarArrowHeadBoss; break;
			case 5 : lineEndStart = kCurvedArrowHeadBoss; lineEndEnd = kCurvedArrowHeadBoss; break;
			default : break;
		}

		InterfacePtr<IGraphicAttrClassID> lineEndStartImpl(Utils<IGraphicAttributeUtils>()->CreateLineEndStartImplementationAttribute(lineEndStart));
		if(lineEndStartImpl == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> lineEndStartImpl nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(lineEndStartImpl);

		InterfacePtr<IGraphicAttrClassID> lineEndEndImpl(Utils<IGraphicAttributeUtils>()->CreateLineEndEndImplementationAttribute(lineEndEnd));
		if(lineEndEndImpl == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> lineEndEndImpl nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(lineEndEndImpl);

		// Set stroke alignment
		InterfacePtr<IGraphicAttrInt32> strokeAlignAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeAlignmentAttribute(strokeAlign));		
		if(strokeAlignAttr == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(strokeAlignAttr);

		gfxApplyMultAttributesCmd->SetItemList(ruleList);
		if(CmdUtils::ProcessCommand(gfxApplyMultAttributesCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		// check if the rule don't go out of spread
		InterfacePtr<IGeometry> ruleGeo (ruleRef,UseDefaultIID());
		if(ruleGeo == nil)
		{		
			ASSERT_FAIL("Func_FV::IDCall_FV -> ruleGeo nil");
			error = PMString(kErrNilInterface);
			break;
		}
#if !COMPOSER
		//if(itemManager->CheckContainmentInOwnerSpread(ruleGeo) != kSuccess)
		//{
		//	error = PMString(kErrItemOutOfSpread);
		//	break;
		//}
#endif
		// Select the item
		if((status = itemManager->SelectPageItem(ruleRef)) != kSuccess)
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

			parser->setItem(refBloc,newRule);
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
Func_BI::Func_BI(ITCLParser * parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BI::Func_BI -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		largeur = reader->ExtractReel('P','+',0,0);
		hauteur = reader->ExtractReel('P','+',0,0);
		nomFichier = reader->ExtractString(255);
		decH = reader->ExtractReel('P','+',0,0);
		decV = reader->ExtractReel('P','+',0,0);
		echH = reader->ExtractPourcent('+',100,0);
		echV = reader->ExtractPourcent('+',100,0);
		coulFond = reader->ExtractInt(0,0,MAX_COLOR);
		trameFond = reader->ExtractInt(0,0,100);
		angle = reader->ExtractInt(0,0,360);
		taille = reader->ExtractReel('D','+',12,0);
		type = reader->ExtractInt(0,0,5);
		ancrage = reader->ExtractBooleen(kFalse);
		blocAncrage = reader->ExtractTCLRef(kCurrentTCLRef);
		relative = reader->ExtractBooleen(kFalse);

		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(0,0,MAX_PAGE);

		align = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);

		cellAncrage = reader->ExtractBooleen(kFalse);
		coulageAuto = reader->ExtractBooleen(kFalse);
		offset = reader->ExtractReel('P','+',0,0);
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
		IDCall_BI(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BI::~Func_BI()
{
}


/* IDCall_BI
*/
void Func_BI::IDCall_BI(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error("");
	UID newFrame;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Get helper interfaces
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();

		// Define the frame bounding box
		PMPoint startPoint(hPos,vPos);	
		PMPoint endPoint(hPos + largeur,vPos + hauteur);	

		PMPointList points;
		points.push_back(startPoint);
		points.push_back(endPoint);
		
		UID ownerSpreadLayer = kInvalidUID;
		
		InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(parser->getCurDoc()));
		if(activeLayer == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> activeLayer nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(!ancrage && !cellAncrage) // no owner spread, the frame will be an inline
		{
			if(relative == kFalse)
			{
				if(page == 0)
					page = parser->getCurPage();
				
#if COULAGE_AUTO
				if(coulageAuto)
				{
					points.clear();
					startPoint = kZeroPoint;
					Func_CDiese::GetPositionRelativeToColumn(parser->getCurDoc(), page, 0, offset, startPoint, largeur, ownerSpreadLayer);
					
					points.push_back(startPoint);
					points.push_back(PMPoint(startPoint.X()+largeur, startPoint.Y()+hauteur));
				}
				else
				{
#endif
				ownerSpreadLayer = itemManager->GetPBCoordAbs(parser->getCurDoc(),page,points,activeLayer);		
#if COULAGE_AUTO
				}
#endif
				
				if(ownerSpreadLayer == kInvalidUID)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
			else
			{
				if(ref == kCurrentTCLRef)
					ref = parser->getCurItem();

				UID refUID = parser->getItem(ref);				

#if COULAGE_AUTO
				if(coulageAuto)
				{
					points.clear();
					startPoint = kZeroPoint;

					if(refUID == kInvalidUID)
					{
						page = parser->getCurPage();
						Func_CDiese::GetPositionRelativeToColumn(parser->getCurDoc(), page, 0, 0, startPoint, largeur, ownerSpreadLayer);
					}
					else
						Func_CDiese::GetPositionRelativeToItem(parser->getCurDoc(), refUID, offset, kFalse, startPoint, largeur, ownerSpreadLayer);
					
					points.push_back(startPoint);
					points.push_back(PMPoint(startPoint.X()+largeur, startPoint.Y()+hauteur));
				}
				else
				{
#endif
				if(refUID == kInvalidUID)
				{
					error = PMString(kErrInvalidItem);
					break;
				}

				ownerSpreadLayer = itemManager->GetPBCoordRel(UIDRef(db,refUID),points,align,activeLayer);

#if COULAGE_AUTO
				}
#endif
				if(ownerSpreadLayer == kInvalidUID)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
		}
		

		PMRect boundsRect(points[0],points[1]);
		UIDRef imFrameRef = Utils<IPathUtils>()->CreateRectangleSpline(UIDRef(db,ownerSpreadLayer), boundsRect, INewPageItemCmdData::kGraphicFrameAttributes);

		if(imFrameRef == UIDRef(nil,kInvalidUID))
		{
			error = PMString(kErrCmdFailed);
			break;
		}
		
		// Apply various attributes
		if(this->ApplyImageFrameAttr(parser, imFrameRef, error) != kSuccess)
			break;
		
#if !COMPOSER
		//// check if the frame don't go out of spread
		//if(!ancrage && itemManager->CheckContainmentInOwnerSpread(frameGeo) != kSuccess)
		//{
		//	error = PMString(kErrItemOutOfSpread);
		//	break;
		//}
#endif
		if((status = itemManager->SelectPageItem(imFrameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		newFrame = imFrameRef.GetUID();

	} while(false);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setItem(refBloc,newFrame);
			
			if(!cellAncrage) // we keep current table selection in case of insertion in a cell
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

ErrorCode Func_BI::ApplyImageFrameAttr(ITCLParser * parser, UIDRef& imFrameRef, PMString& error)
{
	ErrorCode status = kFailure;
	do
	{
		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		InterfacePtr<IHierarchy> imFrameHier (imFrameRef,UseDefaultIID());
		if(imFrameHier == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> imFrameHier nil");
			error = PMString(kErrNilInterface);
			break;
		}		

		InterfacePtr<IGeometry> frameGeo (imFrameRef,UseDefaultIID());
		if(frameGeo == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> frameGeo nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Set the color
		UID colorUID = parser->getColorList(coulFond).colorUID;
		if(colorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			colorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
		}

		UIDList frameList (imFrameRef);

		InterfacePtr<ICommand> iFillRenderCmd (Utils<IGraphicAttributeUtils>()->CreateFillRenderingCommand(colorUID,&frameList,kTrue,kTrue));
		if(iFillRenderCmd == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> iFillRenderCmd nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if(CmdUtils::ProcessCommand(iFillRenderCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		InterfacePtr<ICommand> gfxApplyMultAttributesCmd(CmdUtils::CreateCommand(kGfxApplyMultAttributesCmdBoss));
		if(gfxApplyMultAttributesCmd == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> gfxApplyMultAttributesCmd nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IApplyMultAttributesCmdData> cmdData(gfxApplyMultAttributesCmd, UseDefaultIID());
		if(cmdData == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> cmdData nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Set the fill tint 
		InterfacePtr<IGraphicAttrRealNumber> fillTintAttr (Utils<IGraphicAttributeUtils>()->CreateFillTintAttribute(trameFond));
		if(fillTintAttr == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> fillTintAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(fillTintAttr);
		
		// Set the corner implementation
		ClassID cornerImpl = kInvalidClass;
		switch(type)
		{
			case 1 : cornerImpl = kRoundedCornerBoss; break;
			case 2 : cornerImpl = kInverseRoundedCornerBoss; break; 
			case 3 : cornerImpl = kBevelCornerBoss; break;
			case 4 : cornerImpl = kInsetCornerBoss; break;
			case 5 : cornerImpl = kFancyCornerBoss; break;
			default : break;
		}

		InterfacePtr<IGraphicAttrClassID> cornerImplAttr (Utils<IGraphicAttributeUtils>()->CreateCornerImplementationAttribute(cornerImpl));
		if(cornerImplAttr == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> cornerImplAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(cornerImplAttr);

		// Set the corner radius
		InterfacePtr<IGraphicAttrRealNumber> cornerRadiusAttr (Utils<IGraphicAttributeUtils>()->CreateCornerRadiusAttribute(taille));
		if(cornerRadiusAttr == nil)
		{		
			ASSERT_FAIL("Func_BI::IDCall_BI -> cornerRadiusAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(cornerRadiusAttr);
		
		gfxApplyMultAttributesCmd->SetItemList(frameList);
		if(CmdUtils::ProcessCommand(gfxApplyMultAttributesCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}
				
		// Import the image into the new image item
		bool8 pictImport, useImportPath, createTextBox, nonPrinting;
		PMString pictPath;
		int32 colorIndex;
		int32 colorTint;
		UIDRef imageItemRef(nil,kInvalidUID);
		parser->getPicturePrefs(&useImportPath, &pictImport,&pictPath,&createTextBox,&nonPrinting,&colorIndex,&colorTint);

		if(pictImport && !nomFichier.IsEmpty())
		{
			PMString absolutePath;
			
			if(useImportPath)
				absolutePath.Append(pictPath);
			
			absolutePath.Append(nomFichier);

			if(itemManager->ImportImageInGraphicFrameCmd(imFrameRef,absolutePath,imageItemRef) == kSuccess)
			{
				// Scale the image
				InterfacePtr<IGeometry> imGeo (imageItemRef,UseDefaultIID());
				if(imGeo == nil)
				{		
					error = PMString(kErrNilInterface);
					break;
				}		

				// The reference point is the image left top corner in image coordinate system
				if(itemTransform->Scale(imageItemRef,echH,echV,PMPoint(imGeo->GetStrokeBoundingBox().LeftTop())) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
				
				// Calculate the new position of the left top corner, then move the image
				PMPoint leftTop (frameGeo->GetStrokeBoundingBox().LeftTop());
				PMPoint newLeftTop (leftTop.X() + decH, leftTop.Y() + decV);
				::TransformInnerPointToPasteboard(frameGeo,&newLeftTop);

				if(itemTransform->MoveCmd(imageItemRef,newLeftTop) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
			else
			{
				if(!createTextBox)
				{
					//error = ErrorUtils::PMGetGlobalErrorString();					
					//error.Translate();
					//parser->WriteLogFile(error);
					ErrorUtils::PMSetGlobalErrorCode(kSuccess);
					//break;
				}
				else
				{
					ErrorUtils::PMSetGlobalErrorCode(kSuccess); 
			
					// Convert item to text frame
					InterfacePtr<ICommand> convertToTextCmd (CmdUtils::CreateCommand(kConvertItemToTextCmdBoss));
					convertToTextCmd->SetItemList(UIDList(imFrameRef));
					if(CmdUtils::ProcessCommand(convertToTextCmd) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}

					// Insert name of missing file in frame
					InterfacePtr<IHierarchy> txtItemHier (imFrameRef, UseDefaultIID());
					InterfacePtr<IMultiColumnTextFrame> txtFrame (db, txtItemHier->GetChildUID(0), UseDefaultIID());
					InterfacePtr<ITextModelCmds> txtModelCmds (db, txtFrame->GetTextModelUID(), UseDefaultIID());
					boost::shared_ptr<WideString> toInsert (new WideString(absolutePath));
					InterfacePtr<ICommand> insertCmd (txtModelCmds->InsertCmd(0, toInsert));
					if(CmdUtils::ProcessCommand(insertCmd) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}

					if(nonPrinting)
					{
						UIDList itemList(imFrameRef);
						InterfacePtr<ICommand> nonPrintCommand (Utils<IGraphicAttributeUtils>()->CreateNonPrintCommand(kTrue,&itemList,kTrue,kTrue));
						if(CmdUtils::ProcessCommand(nonPrintCommand) != kSuccess)
							break;
					}
					coulFond = colorIndex;
					trameFond = colorTint;

					// Reset frame geometry
					InterfacePtr<IGeometry> newFrameGeo (imFrameRef, UseDefaultIID());
					if(imFrameRef == nil)
						break;

					frameGeo = newFrameGeo;
				}

				// Store missing file's info
				InterfacePtr<ICommand> setMissingFileInfoCmd (CmdUtils::CreateCommand(kPrsSetImageInfosCmdBoss));
				setMissingFileInfoCmd->SetItemList(UIDList(imFrameRef));
				
				InterfacePtr<IPMPointData> scaleData (setMissingFileInfoCmd, IID_IIMAGESCALE);
				InterfacePtr<IPMPointData> shiftData (setMissingFileInfoCmd, IID_IIMAGESHIFT);
				InterfacePtr<IStringData> pathData (setMissingFileInfoCmd, IID_IIMAGEPATH);

				scaleData->Set(PMPoint(echH,echV));
				shiftData->Set(PMPoint(decH,decV));
				pathData->Set(absolutePath);

				if(CmdUtils::ProcessCommand(setMissingFileInfoCmd) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
		}

		if(ancrage)
		{
			if(blocAncrage == kCurrentTCLRef)
				blocAncrage = parser->getCurItem();

			UID blocAncrageUID = parser->getItem(blocAncrage);
			if(blocAncrageUID == kInvalidUID)
			{
				error = PMString(kErrInvalidItem);
				break;
			}
			
			if(itemTransform->MakeInlineCmd(imFrameRef,UIDRef(db,blocAncrageUID)) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			// Recompose the frame that receive the inline
			InterfacePtr<IHierarchy> blocAncrageHier (UIDRef(db,blocAncrageUID),UseDefaultIID());
			if(blocAncrageHier == nil)
			{
				error = PMString(kErrNilInterface);		
				break;
			}

			InterfacePtr<IMultiColumnTextFrame> multiColAncr (db, blocAncrageHier->GetChildUID(0), UseDefaultIID());
			if(multiColAncr == nil)
			{
				error = PMString(kErrNilInterface);		
				break;
			}

			InterfacePtr<ITextFrameHelper> txtFrameHelper (parser->QueryTextFrameHelper());
			txtFrameHelper->RecomposeWholeStory(multiColAncr);
		}
		else if(cellAncrage)
		{
			if(parser->getCurCells() == kZeroGridArea)
			{
				error = PMString(kErrNoCurrentCells);
				break;
			}

			// Get receiver cell's story thread
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			
			// Insert character in text flow to anchor the inline.
			InterfacePtr<ITextModelCmds> modelCmds (txtModel, UseDefaultIID());
			boost::shared_ptr<WideString> replaceChar( new WideString());
			replaceChar->Append(kTextChar_ObjectReplacementCharacter); 
			InterfacePtr<ICommand> insertTextCmd(modelCmds->InsertCmd(insertPos, replaceChar));
			
			if((status = CmdUtils::ProcessCommand(insertTextCmd)) != kSuccess)
			{
				error = PMString();
				break;
			}

			// Command to change the page item into an inline
			InterfacePtr<ICommand> changeILGCmd(CmdUtils::CreateCommand(kChangeILGCmdBoss));
			changeILGCmd->SetUndoability(ICommand::kAutoUndo);	

			changeILGCmd->SetItemList(UIDList(txtModel));

			// Set the insertion location
			InterfacePtr<IRangeData> rangeData(changeILGCmd, UseDefaultIID());
			rangeData->Set(insertPos, insertPos);
		
			InterfacePtr<IUIDData> ilgUIDData(changeILGCmd, UseDefaultIID());
			ilgUIDData->Set(imFrameRef);

			if((status = CmdUtils::ProcessCommand(changeILGCmd)) != kSuccess)
			{
				error = PMString();
				break;
			}
		}
		
		if(angle != 0)
		{
			// Rotate the frame
			if(itemTransform->Rotate(imFrameRef,PMReal(angle), frameGeo->GetStrokeBoundingBox().GetCenter(), !ancrage) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		status = kSuccess;

	} while(kFalse);

	return status;
}

/* Constructor
*/
Func_BS::Func_BS(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BS::Func_BS -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		selectTOP = reader->ExtractBooleen(kFalse);
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
		IDCall_BS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_BS::~Func_BS()
{
}


/* IDCall_BS
*/
void Func_BS::IDCall_BS(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BS::IDCall_BS -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BS::IDCall_BS -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();
		
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID itemUID = parser->getItem(refBloc);

		if(itemUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

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
			parser->setCurItem(refBloc, selectTOP);
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
Func_BG::Func_BG(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BG::Func_BG -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		groupage = reader->ExtractBooleen(kFalse) ;

		TCLRef param ;

		while (!reader->IsEndOfCommand())
		{	
			param = reader->ExtractTCLRef(kCurrentTCLRef);
			refBlocList.insert(refBlocList.end(), param) ;
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

	if(!reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));
	else
	{
		if (refBlocList.size()<2)
			throw TCLError(PMString(kErrNotEnoughArgument));
		else
			IDCall_BG(parser);
	}
		
}


/* Destructor
*/
Func_BG::~Func_BG()
{
}

UIDList Func_BG::GetAndCheckItemsToGroup(ITCLParser * parser, bool16 groupage, IDataBase * db, const K2Vector<TCLRef>& refList, bool16& errorOccured, PMString& error)
{
	errorOccured = kFalse;
	UIDList uidList(db);
	do
	{
		TCLRef firstFrame = refList[0];
		UID itemUID = parser->getItem(firstFrame);

		InterfacePtr<IHierarchy> itemHier (db, itemUID, UseDefaultIID());
		if(itemHier == nil)
		{
			error = PMString(kErrInvalidItem);
			errorOccured = kTrue;
			break;
		}

		UID ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);

		UID ownerSpreadUID = kInvalidUID;

		// Check whether item is on a page or on a spread
		InterfacePtr<ISpread> isSpread (db,ownerPageUID, UseDefaultIID());
		if(!isSpread)
		{
			// It's a page, get its owner spread
			UIDList spread(db);
			itemHier->GetAncestors(&spread, IID_ISPREAD);
			ownerSpreadUID = spread[0];
		} 		
		else
			ownerSpreadUID = ownerPageUID;

		if(groupage)
		{			
			for(int32 i = 0 ; i < refList.size() && !errorOccured  ; ++i)
			{
				TCLRef refBloc = refList[i];
				if(refBloc == kCurrentTCLRef) refBloc = parser->getCurItem();
				itemUID = parser->getItem(refBloc);
				InterfacePtr<IHierarchy> itemHier (db, itemUID, UseDefaultIID());
				if(itemHier == nil)
				{
					error = PMString(kErrInvalidItem);
					errorOccured = kTrue;
					break;
				}
				UID itemOwnerSpreadUID = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);
				if(ownerSpreadUID !=  itemOwnerSpreadUID)
				{
					// Item lies on	a page, check if it's same spread
					UIDList spread(db);
					itemHier->GetAncestors(&spread, IID_ISPREAD);
					if(spread[0] != ownerSpreadUID)
					{
						error = PMString(kErrItemNotOnSameSpread);
						errorOccured = kTrue;
						break;
					}
				}

				uidList.Append(itemUID);
			}
		}
		else
		{
			TCLRef startBox, endBox;			
			for(int32 i = 0 ; i < refList.size() && !errorOccured ; i = i + 2)
			{
				startBox = refList[i];
				endBox = refList[i+1];
				TCLRef refBloc = refList[i];
				if(startBox == kCurrentTCLRef) refBloc = parser->getCurItem();
				if(endBox == kCurrentTCLRef) refBloc = parser->getCurItem();

				if(startBox > endBox)
				{
					error = PMString(kErrInvalidGroupArgument);
					errorOccured = kTrue;
					break;
				}

				while(startBox <= endBox)
				{
					itemUID = parser->getItem(startBox);
					InterfacePtr<IHierarchy> itemHier (db, itemUID, UseDefaultIID());
					if(itemHier == nil)
					{
						//error = PMString(kErrInvalidItem);
						//errorOccured = kTrue;
						//break;
						++startBox;
						continue;
					}

					UID itemOwnerSpreadUID = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);
					if(ownerSpreadUID !=  itemOwnerSpreadUID)
					{
						// Item lies on	a page, check if it's same spread
						UIDList spread(db);
						itemHier->GetAncestors(&spread, IID_ISPREAD);
						if(spread[0] != ownerSpreadUID)
						{
							error = PMString(kErrItemNotOnSameSpread);
							errorOccured = kTrue;
							break;
						}
					}

					uidList.Append(itemUID);
					++startBox;
				}
			}
		}
	} while(kFalse);

	return uidList; 
}


/* IDCall_BG
*/
void Func_BG::IDCall_BG(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BG::IDCall_BG -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BG::IDCall_BG -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();

		// Can these items be grouped ?
		bool16 errorOccured = kFalse;
		UIDList * itemsToGroup = new UIDList(this->GetAndCheckItemsToGroup(parser, groupage, db, refBlocList, errorOccured, error)); //allocate memory dynamically in order to avoid underflow and overflow problems
		if(errorOccured)
		{
			delete itemsToGroup;
			break;
		}

		UIDRef groupItem = UIDRef::gNull;
		if((status = itemManager->GroupPageItemsCmd(itemsToGroup, groupItem)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
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
Func_BV::Func_BV(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BV::Func_BV -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		verrouillage = reader->ExtractBooleen(kFalse) ;

		do
		{
			listRef.push_back(reader->ExtractTCLRef(kCurrentTCLRef));
		}
		while(!reader->IsEndOfCommand());

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
		IDCall_BV(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
		
}


/* Destructor
*/
Func_BV::~Func_BV()
{
	listRef.clear();
}


/* IDCall_BV
*/
void Func_BV::IDCall_BV(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BV::IDCall_BV -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BV::IDCall_BV -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();
		
		for(int32 i = 0 ; i < listRef.size() ; ++i)
		{
			UID itemUID = kInvalidUID;
			if(listRef[i] == kCurrentTCLRef)
				itemUID = parser->getItem(parser->getCurItem());
			else
				itemUID = parser->getItem(listRef[i]);

			if(itemUID == kInvalidUID)
			{
				error = PMString(kErrInvalidItem);
				status = kFailure;
				break;
			}
			
			if((status = itemManager->LockPageItemCmd(UIDRef(db,itemUID), verrouillage)) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
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
Func_BD::Func_BD(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BD::Func_BD -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try{
		reader->MustEgal();
		TCLRef param;
		while (!reader->IsEndOfCommand())
		{
			param = reader->ExtractTCLRef(kCurrentTCLRef);
			refBlocList.insert(refBlocList.end(), param);
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

	if(!reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));
	else
	{
		if (refBlocList.size()<1)
			throw TCLError(PMString(kErrNotEnoughArgument));
		else
			IDCall_BD(parser);
	}
		
}


/* Destructor
*/
Func_BD::~Func_BD()
{
}


/* IDCall_BD
*/
void Func_BD::IDCall_BD(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BD::IDCall_BD -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BD::IDCall_BD -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();

		if(db == nil)
            break;
 		//allocate memory dynamically in order to avoid underflow and overflow problems
		//UIDList * itemsToGroup = new UIDList(db) ;

		UIDList itemsToGroup(db) ;

		K2Vector<TCLRef>::iterator iterV;
		for (iterV = refBlocList.begin(); iterV < refBlocList.end(); iterV++)
		{
			if (*iterV == kCurrentTCLRef)
				*iterV = parser->getCurItem();
			itemsToGroup.Append(parser->getItem(*iterV)) ;
		}

   		if(itemsToGroup.size()==0){
            status = kSuccess;
            break;
        }

		if((status = itemManager->DeletePageItemsCmd(itemsToGroup)) != kSuccess)
		{
			status = kSuccess;
			//error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//delete References in the persisting list there have been deletion
		for (iterV = refBlocList.begin(); iterV < refBlocList.end(); iterV++){
			parser->deleteItem(*iterV) ;
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
Func_B1::Func_B1(ITCLParser * parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_B1::Func_B1 -> reader nil");
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
	{
		IDCall_B1(parser);
	}
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_B1::~Func_B1()
{
}


/* IDCall_B1
*/
void Func_B1::IDCall_B1(ITCLParser * parser)
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

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BN::IDCall_BN -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_B1::IDCall_B1 -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase* db = parser->getCurDataBase() ;

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDRef frameRef = UIDRef(db,refUID) ;

		if(itemTransform->BringToFrontCmd(frameRef) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if((status = itemManager->SelectPageItem(frameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}


	} while(false);

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
Func_B2::Func_B2(ITCLParser * parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_B2::Func_B2 -> reader nil");
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
	{
		IDCall_B2(parser);
	}
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_B2::~Func_B2()
{
}


/* IDCall_B2
*/
void Func_B2::IDCall_B2(ITCLParser * parser)
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

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BN::IDCall_BN -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_B2::IDCall_B2 -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase* db = parser->getCurDataBase();

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDRef frameRef = UIDRef(db,refUID) ;

		if(itemTransform->SendToBackCmd(frameRef) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if((status = itemManager->SelectPageItem(frameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}


	} while(false);

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
Func_BE::Func_BE(ITCLParser * parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BE::Func_BE -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		angle = reader->ExtractReel2('+',0,0,PMReal(MIN_SKEW),PMReal(MAX_SKEW));
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
	{
		IDCall_BE(parser);
	}
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BE::~Func_BE()
{
}


/* IDCall_BE
*/
void Func_BE::IDCall_BE(ITCLParser * parser)
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

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_BE::IDCall_BE -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BE::IDCall_BE -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase* db = parser->getCurDataBase() ;

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDRef frameRef = UIDRef(db,refUID) ;

		if(itemTransform->Skew(frameRef,angle,PMPoint(0.0, 0.0)) != kSuccess)
			error = ErrorUtils::PMGetGlobalErrorString();

		if((status = itemManager->SelectPageItem(frameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

	} while(false);

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
Func_BP::Func_BP(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BP::Func_BP -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		rotationAngle = reader->ExtractReel2('+',0,0,0,360);
		skewAngle = reader->ExtractReel2('+',0,0,PMReal(MIN_SKEW),PMReal(MAX_SKEW));
		horOffset = reader->ExtractReel('D','+',0,0);
		verOffset = reader->ExtractReel('D','+',0,0);
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
	{
		IDCall_BP(parser);
	}
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BP::~Func_BP()
{
}


/* IDCall_BP
*/
void Func_BP::IDCall_BP(ITCLParser * parser)
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

		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BP::IDCall_BP -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		// Get the document's database
		IDataBase* db = parser->getCurDataBase() ;

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDRef frameRef = UIDRef(db,refUID) ;

		InterfacePtr<IPathGeometry> framePathGeo (frameRef, UseDefaultIID());
		if(framePathGeo == nil)
			break;

		if(Utils<IPathUtils>()->IsLine(framePathGeo))
		{
			error = PMString(kErrBoxRequired);
			break;
		}

		InterfacePtr<IFrameType> frameType (frameRef, UseDefaultIID());

		if(frameType->IsGraphicFrame())
		{
			// Move the image, if any
			if(!frameType->IsEmpty())
			{
				InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());		

				// Get image frame geometry				
				InterfacePtr<IHierarchy> frameHier (frameRef, UseDefaultIID());
				InterfacePtr<IGeometry> imageFrameGeo (db, frameHier->GetChildUID(0), UseDefaultIID());

				UIDRef imageFrameRef = ::GetUIDRef(imageFrameGeo);

				if(itemTransform->Rotate(imageFrameRef,rotationAngle,imageFrameGeo->GetStrokeBoundingBox().GetCenter()) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}

				if(itemTransform->Skew(imageFrameRef,skewAngle,imageFrameGeo->GetStrokeBoundingBox().GetCenter()) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			
				InterfacePtr<IGeometry> frameGeo (frameRef, UseDefaultIID());

				PMPoint leftTop (frameGeo->GetStrokeBoundingBox().LeftTop());
				PMPoint newLeftTop (leftTop.X() + horOffset, leftTop.Y() + verOffset);
				::TransformInnerPointToPasteboard(frameGeo,&newLeftTop);

				if(itemTransform->MoveCmd(imageFrameRef,newLeftTop) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}			
			}
			else
			{
				InterfacePtr<ICommand> setMissingFileInfoCmd (CmdUtils::CreateCommand(kPrsSetImageInfosCmdBoss));
				setMissingFileInfoCmd->SetItemList(UIDList(frameRef));
				
				InterfacePtr<IPMPointData> scaleCmdData (setMissingFileInfoCmd, IID_IIMAGESCALE);
				InterfacePtr<IPMPointData> shiftCmdData (setMissingFileInfoCmd, IID_IIMAGESHIFT);
				InterfacePtr<IStringData> pathCmdData (setMissingFileInfoCmd, IID_IIMAGEPATH);

				// We only modify offset data
				InterfacePtr<IPMPointData> imageScaleData(frameRef, IID_IIMAGESCALE);
				InterfacePtr<IStringData> imagePathData(frameRef, IID_IIMAGEPATH);

				scaleCmdData->Set(imageScaleData->GetPMPoint());				
				pathCmdData->Set(imagePathData->Get());
				shiftCmdData->Set(PMPoint(horOffset,verOffset));

				if(CmdUtils::ProcessCommand(setMissingFileInfoCmd) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
		}
		else if(frameType->IsTextFrame())
		{
			UID multiColUID = Utils<IFrameUtils>()->GetTextFrameUID(framePathGeo);
			if(multiColUID == kInvalidUID)
				break;	
		
			InterfacePtr<IMultiColumnTextFrame> multiCol (parser->getCurDataBase(), multiColUID, UseDefaultIID());
			if(multiCol == nil)
				break;

			// Determine text range in selected frame
			InterfacePtr<IFrameListComposer> frameComposer (multiCol,UseDefaultIID());
			if(frameComposer == nil)
				break;

			frameComposer->RecomposeThruLastFrame();

			TextIndex startIndex = multiCol->TextStart();
			int32 length = multiCol->TextSpan();

			InterfacePtr<ITxtAttrHelper> txtAttrHelper(parser->QueryTxtAttrHelper());
			if(txtAttrHelper == nil)
			{		
				ASSERT_FAIL("Func_BP::IDCall_BP -> itemTransform nil");
				error = PMString(kErrNilInterface);
				break;
			}
			
			boost::shared_ptr<AttributeBossList> attrList(new AttributeBossList);

			txtAttrHelper->AddSkewAngleAttribute(attrList.get(), skewAngle);

			// Create an ApplyTextStyleCmd
			InterfacePtr<ITextModel> txtModel (multiCol->QueryTextModel());
			if(txtModel == nil)
				break;
			
			InterfacePtr<ITextModelCmds> modelCmds(txtModel, UseDefaultIID()); 
			if(modelCmds == nil)
				break;

			InterfacePtr<ICommand> applyCmd(modelCmds->ApplyCmd(startIndex,length,attrList,kCharAttrStrandBoss));
			if (applyCmd == nil)
				break;
			
			applyCmd->SetUndoability(ICommand::kAutoUndo);

			if(CmdUtils::ProcessCommand(applyCmd) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			error = PMString(kErrBoxRequired);
			break;
		}
	
		if((status = itemManager->SelectPageItem(frameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

	} while(false);

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
Func_FD::Func_FD(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_FD::Func_FD -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		hPos1 = reader->ExtractReel('P','+',0,0);
		vPos1 = reader->ExtractReel('P','+',0,0);
		hPos2 = reader->ExtractReel('P','+',0,0);
		vPos2 = reader->ExtractReel('P','+',0,0);
		graisse = reader->ExtractReel('D','+',0,0);
		typeFilet = reader->ExtractInt(0,0,18);
		coulFilet = reader->ExtractInt(5,0,MAX_COLOR);
		trameFilet = reader->ExtractInt(100,0,100);
		relative = reader->ExtractBooleen(kFalse);
		
		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(0,0,MAX_PAGE);

		align = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);

		typeFleche = reader->ExtractInt(0,0,5);

		reader->ExtractReel('D','+',0,0); // angle parameter to ensure backward compatibility
		strokeAlign = reader->ExtractInt(0,0,2);

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
		IDCall_FD(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_FD::~Func_FD()
{
}


/* IDCall_FD
*/
void Func_FD::IDCall_FD(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");
	UID newRule;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Get helper interfaces
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();

		// Define the frame bounding box
		PMPoint startPoint(hPos1,vPos1);	
		PMPoint endPoint(hPos2,vPos2);	

		PMPointList points;
		points.push_back(startPoint);
		points.push_back(endPoint);

		UID ownerSpreadLayer = kInvalidUID;
		
		InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(parser->getCurDoc()));
		if(activeLayer == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> activeLayer nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if(relative == kFalse)
		{
			if(page == 0)
				page = parser->getCurPage();
			
			ownerSpreadLayer = itemManager->GetPBCoordAbs(parser->getCurDoc(),page,points,activeLayer);
			
			if(ownerSpreadLayer == kInvalidUID)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			if(ref == kCurrentTCLRef)
				ref = parser->getCurItem();

			UID refUID = parser->getItem(ref);
			if(refUID == kInvalidUID)
			{
				error = PMString(kErrInvalidItem);
				break;
			}
	
			ownerSpreadLayer = itemManager->GetPBCoordRel(UIDRef(db,refUID),points,align,activeLayer);
			if(ownerSpreadLayer == kInvalidUID)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		UIDRef ruleRef = Utils<IPathUtils>()->CreateLineSpline(UIDRef(db,ownerSpreadLayer), points, INewPageItemCmdData::kDefaultGraphicAttributes);

		if(ruleRef == UIDRef(nil,kInvalidUID))
			break;

		InterfacePtr<IHierarchy> ruleHier (ruleRef,UseDefaultIID());
		if(ruleHier == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> ruleHier nil");
			error = PMString(kErrNilInterface);
			break;
		}

		newRule = ruleRef.GetUID();

		UIDList ruleList(ruleRef);

		InterfacePtr<ICommand> gfxApplyMultAttributesCmd(CmdUtils::CreateCommand(kGfxApplyMultAttributesCmdBoss));
		if(gfxApplyMultAttributesCmd == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> gfxApplyMultAttributesCmd nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		InterfacePtr<IApplyMultAttributesCmdData> cmdData(gfxApplyMultAttributesCmd, UseDefaultIID());
		if(cmdData == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> cmdData nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Set the stroke color
		UID colorUID = parser->getColorList(coulFilet).colorUID;
		if(colorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			colorUID = Utils<ISwatchUtils>()->GetBlackSwatchUID(db); // color doesn't exist, replaced with black
		}

		InterfacePtr<ICommand> iStrokeRenderCmd (Utils<IGraphicAttributeUtils>()->CreateStrokeRenderingCommand(colorUID,&ruleList,kTrue,kTrue));
		if(iStrokeRenderCmd == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> iStrokeRenderCmd nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if(CmdUtils::ProcessCommand(iStrokeRenderCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		// Set the type of the stroke
		ClassID lineImpl = kInvalidClass;
		switch(typeFilet)
		{
			case 0 : lineImpl = kSolidPathStrokerBoss; break;
			case 1 : lineImpl = kDashedPathStrokerBoss; break;
			case 2 : lineImpl = kThinThinPathStrokerBoss; break;
			case 3 : lineImpl = kThickThickPathStrokerBoss; break;
			case 4 : lineImpl = kDashedPathStrokerBoss; 
					 // Set dashs and gaps values to so simulate line-dash type	
					 {
						InterfacePtr<IDashedAttributeValues> dashAttr ((IDashedAttributeValues *)::CreateObject(kDashedAttributeValuesBoss,IID_IDASHEDATTRIBUTEVALUES));
						if(dashAttr == nil)
						{		
							ASSERT_FAIL("Func_FD::IDCall_FD -> dashAttr nil");
							error = PMString(kErrNilInterface);
							break;
						}

						dashAttr->SetValue(0,12.0); // dash 0
						dashAttr->SetValue(1,3.0); // gap 0
						dashAttr->SetValue(2,3.0); // dash 1
						dashAttr->SetValue(3,3.0); // gap 1

						cmdData->AddAnAttribute(dashAttr);
					 }
					 break;

			case 5 : lineImpl = kCannedDotPathStrokerBoss; break;
			case 6 : lineImpl = kThinThickPathStrokerBoss; break;
			case 7 : lineImpl = kThickThinPathStrokerBoss; break;
			case 8 : lineImpl = kThinThickThinPathStrokerBoss; break;
			case 9 : lineImpl = kThickThinThickPathStrokerBoss; break;
			case 10 : lineImpl = kTriplePathStrokerBoss; break;
			case 11 : lineImpl = kCannedDash3x2PathStrokerBoss; break;
			case 12 : lineImpl = kCannedDash4x4PathStrokerBoss; break;
			case 13 : lineImpl = kLeftSlantHashPathStrokerBoss; break;
			case 14 : lineImpl = kRightSlantHashPathStrokerBoss; break;
			case 15 : lineImpl = kStraightHashPathStrokerBoss; break;
			case 16 : lineImpl = kSingleWavyPathStrokerBoss; break;
			case 17 : lineImpl = kWhiteDiamondPathStrokerBoss; break;
			case 18 : lineImpl = kJapaneseDotsPathStrokerBoss; break;
			default : break;
		}

		InterfacePtr<IGraphicAttrClassID> lineImplAttr (Utils<IGraphicAttributeUtils>()->CreateLineImplementationAttribute(lineImpl,kInvalidUID));
		if(lineImplAttr == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> lineImplAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(lineImplAttr);

		// Set the stroke tint
		InterfacePtr<IGraphicAttrRealNumber> strokeTintAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeTintAttribute(trameFilet));
		if(strokeTintAttr == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> strokeTintAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(strokeTintAttr);

		// Set the stroke weight
		InterfacePtr<IGraphicAttrRealNumber> strokeWeightAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeWeightAttribute(graisse));
		if(strokeWeightAttr == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> strokeWeightAttr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(strokeWeightAttr);

		// Set the line end and start
		ClassID lineEndStart = kInvalidClass, lineEndEnd = kInvalidClass;
		switch(typeFleche)
		{
			case 1 : lineEndEnd = kCurvedArrowHeadBoss; break;
			case 2 : lineEndStart = kCurvedArrowHeadBoss; break;
			case 3 : lineEndStart = kBarArrowHeadBoss; lineEndEnd = kCurvedArrowHeadBoss; break;
			case 4 : lineEndStart = kCurvedArrowHeadBoss; lineEndEnd = kBarArrowHeadBoss; break;
			case 5 : lineEndStart = kCurvedArrowHeadBoss; lineEndEnd = kCurvedArrowHeadBoss; break;
			default : break;
		}

		InterfacePtr<IGraphicAttrClassID> lineEndStartImpl(Utils<IGraphicAttributeUtils>()->CreateLineEndStartImplementationAttribute(lineEndStart));
		if(lineEndStartImpl == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> lineEndStartImpl nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(lineEndStartImpl);

		InterfacePtr<IGraphicAttrClassID> lineEndEndImpl(Utils<IGraphicAttributeUtils>()->CreateLineEndEndImplementationAttribute(lineEndEnd));
		if(lineEndEndImpl == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> lineEndEndImpl nil");
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(lineEndEndImpl);

		// Set stroke alignment
		InterfacePtr<IGraphicAttrInt32> strokeAlignAttr (Utils<IGraphicAttributeUtils>()->CreateStrokeAlignmentAttribute(strokeAlign));		
		if(strokeAlignAttr == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->AddAnAttribute(strokeAlignAttr);

		gfxApplyMultAttributesCmd->SetItemList(ruleList);
		if(CmdUtils::ProcessCommand(gfxApplyMultAttributesCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		// check if the rule don't go out of spread
		InterfacePtr<IGeometry> ruleGeo (ruleRef,UseDefaultIID());
		if(ruleGeo == nil)
		{		
			ASSERT_FAIL("Func_FD::IDCall_FD -> ruleGeo nil");
			error = PMString(kErrNilInterface);
			break;
		}		
		
#if !COMPOSER
		//if(itemManager->CheckContainmentInOwnerSpread(ruleGeo) != kSuccess)
		//{
		//	error = PMString(kErrItemOutOfSpread);
		//	break;
		//}
#endif
		// Select the item
		if((status = itemManager->SelectPageItem(ruleRef)) != kSuccess)
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

			parser->setItem(refBloc,newRule);
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
Func_BPLUS::Func_BPLUS(ITCLParser * parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BPLUS::Func_BPLUS -> reader nil");
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
	{
		IDCall_BPLUS(parser);
	}
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BPLUS::~Func_BPLUS()
{
}


/* IDCall_BPLUS
*/
void Func_BPLUS::IDCall_BPLUS(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error("");
	TCLRef refBloc ;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BN::IDCall_BN -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_BPLUS::IDCall_BPLUS -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase* db = parser->getCurDataBase();

		refBloc = parser->getCurItem();
		++refBloc;

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDRef frameRef = UIDRef(db,refUID) ;

		if((status = itemManager->SelectPageItem(frameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}


	} while(false);

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
Func_BMOINS::Func_BMOINS(ITCLParser * parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BMOINS::Func_BMOINS -> reader nil");
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
	{
		IDCall_BMOINS(parser);
	}
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BMOINS::~Func_BMOINS()
{
}


/* IDCall_BMOINS
*/
void Func_BMOINS::IDCall_BMOINS(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error("");
	TCLRef refBloc ;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BN::IDCall_BN -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_BMOINS::IDCall_BMOINS -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase* db = parser->getCurDataBase();

		refBloc = parser->getCurItem();
		--refBloc;

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDRef frameRef = UIDRef(db,refUID) ;

		if((status = itemManager->SelectPageItem(frameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}


	} while(false);

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
Func_BF::Func_BF(ITCLParser * parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BF::Func_BF -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);

		center = reader->ExtractBooleen(kFalse);

		scale = reader->ExtractBooleen(kFalse);

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
		IDCall_BF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BF::~Func_BF()
{
}


/* IDCall_BF
*/
void Func_BF::IDCall_BF(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error("");
	
	UIDList * contentList = nil;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_BF::IDCall_BF -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase* db = parser->getCurDataBase();	

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDRef frameRef = UIDRef(db,refUID) ;
		UIDList frameList(frameRef);
		
		//we won't delete the UIDList because it will be done by inDesign (if we delete manually it causes an shutdown error)

		if(! Utils<IFrameContentUtils>()->DoesContainGraphicFrame(&frameList,kFalse))
		{
			//error = PMString(kErrImageBoxRequired);
			status = kSuccess;
			break;
		}

		contentList = Utils<IFrameContentUtils>()->CreateListOfContent(frameList) ;
		
		if (contentList->Length() < 1)
		{
			//error = PMString(kErrImageBoxRequired);
			status = kSuccess;
			break;
		}
			
		if (center)
		{
			if(itemTransform->CenterContentCmd(UIDRef(db,contentList->At(0))) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			if (scale)
			{
				if(itemTransform->FitContentPropCmd(UIDRef(db,contentList->At(0))) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
			else
			{
				if(itemTransform->FitContentToFrameCmd(UIDRef(db,contentList->At(0))) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
		}
		
		status = kSuccess ;

	} while(false);

	if (seq != nil)
	{
		if(contentList != nil)
			delete contentList;
		
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
Func_BM::Func_BM(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BM::Func_BM -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		largeur = reader->ExtractReel('P','+',0,0);
		hauteur = reader->ExtractReel('P','+',0,0);
		alignVert = reader->ExtractInt(0,-1,3);
		dec = reader->ExtractReel('P','+',0,0);
		coulFond = reader->ExtractInt(0,-1,MAX_COLOR);
		trameFond = reader->ExtractInt(0,-1,100);
		nbCol = reader->ExtractInt(1,-1,MAX_COL);
		gout = reader->ExtractReel('D','+',3,0);
		retrait = reader->ExtractReel('D','+',0,0);
		angle = reader->ExtractInt(0,-1,360);
		relative = reader->ExtractBooleen(kFalse);

		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(0,0,MAX_PAGE);

		alignBloc = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);
		vJustMax = reader->ExtractReel('D','+',0,0);
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
		IDCall_BM(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BM::~Func_BM()
{
}

/* IDCall_BM
*/
void Func_BM::IDCall_BM(ITCLParser * parser)
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

		// Get helper interfaces
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BM::IDCall_BM -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_BM::IDCall_BM -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_BM::IDCall_BM -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BM::IDCall_BM -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();
		IDocument * doc = parser->getCurDoc();

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDRef txtFrameRef = UIDRef(db,refUID);
		
		if(!Utils<IPageItemTypeUtils>()->IsTextFrame(txtFrameRef))
		{
			error = PMString(kErrTextBoxRequired);
			break;
		}		

		InterfacePtr<IHierarchy> txtFrameHier (txtFrameRef,UseDefaultIID());
		if(txtFrameHier == nil)
		{
			error = PMString(kErrNilInterface);		
			break;
		}

		bool16 isInline = Utils<IPageItemTypeUtils>()->IsInline(txtFrameRef);
		if(!isInline)
		{
			// Get current owner spread
			UID currentOwnerSpread = txtFrameHier->GetSpreadUID(); 

			InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(doc));
			if(activeLayer == nil)
			{
				error = PMString(kErrNilInterface);		
				break;
			}

			PMPoint newLeftTop;
			PMPointList points;
			points.push_back(PMPoint(hPos,vPos));

			UID newOwnerSpreadLayer = kInvalidUID;

			// Calculate the new leftTop point
			if(relative == kFalse)
			{
				if(page == 0)
					page = parser->getCurPage();

				newOwnerSpreadLayer = itemManager->GetPBCoordAbs(doc,page,points,activeLayer);

				if(newOwnerSpreadLayer == kInvalidUID)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}	
			}
			else
			{
				if(ref == kCurrentTCLRef)
					ref = parser->getCurItem();

				UID refUID = parser->getItem(ref);
				if(refUID == kInvalidUID)
				{
					error = PMString(kErrInvalidItem);
					break;
				}

				newOwnerSpreadLayer = itemManager->GetPBCoordRel(UIDRef(db,refUID),points,alignBloc, activeLayer);
				if(newOwnerSpreadLayer == kInvalidUID)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}

			newLeftTop = points[0];		

			InterfacePtr<IHierarchy> spreadLayer (db,newOwnerSpreadLayer,UseDefaultIID());

			// Check before moving if the text frame has to change spread
			if(currentOwnerSpread != spreadLayer->GetSpreadUID())
			{
				InterfacePtr<ICommand> changeSpreadCmd (CmdUtils::CreateCommand(kMoveToSpreadCmdBoss));
				changeSpreadCmd->SetItemList(UIDList(txtFrameRef));

				InterfacePtr<IUIDData> spreadUID (changeSpreadCmd, UseDefaultIID());
				spreadUID->Set(UIDRef(db,spreadLayer->GetSpreadUID()));
				
				if(CmdUtils::ProcessCommand(changeSpreadCmd) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}			
			}

			// Move item
			if(itemTransform->MoveCmd(txtFrameRef,newLeftTop) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			// Resize item
			ErrorCode resizeCmd =  Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(txtFrameRef), Transform::PasteboardCoordinates(), 
					Geometry::OuterStrokeBounds(), newLeftTop, Geometry::ResizeTo(largeur, hauteur));
			
			if(resizeCmd != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			// It's an inline so we just resize it
			InterfacePtr<IGeometry> frameGeo (txtFrameRef, UseDefaultIID());

			PMPoint refPoint = frameGeo->GetStrokeBoundingBox().LeftTop();
			
			ErrorCode resizeCmd =  Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(txtFrameRef), Transform::InnerCoordinates(), 
					Geometry::OuterStrokeBounds(), refPoint, Geometry::ResizeTo(largeur, hauteur));
			
			if(resizeCmd != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}		

		// Set vertical justication and first line offset options, 
		InterfacePtr<IHierarchy> multiColumnItem (txtFrameHier->QueryChild(0)); //kMultiColumnItemBoss
		if(multiColumnItem == nil)
		{
			ASSERT_FAIL("Func_BM::IDCall_BM -> multiColumnItem nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		InterfacePtr<IMultiColumnTextFrame> txtFrameSetting (multiColumnItem,UseDefaultIID());
		if(txtFrameSetting == nil)
		{
			ASSERT_FAIL("Func_BM::IDCall_BM -> txtFrameSetting nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		Text::VerticalJustification vj;
		switch(alignVert)
		{
			case 0 : vj = Text::kVJTop; break;
			case 1 : vj = Text::kVJJustify; break;
			case 2 : vj = Text::kVJCenter; break;
			case 3 : vj = Text::kVJBottom; break;
			default : vj = Text::kVJInvalid; break;
		}

		if(alignVert != -1)
			txtFrameSetting->SetVerticalJustification(vj);

		if(vJustMax != -1)
			txtFrameSetting->SetMaxVJInterParaSpace(vJustMax);

		if(dec != -1)
			txtFrameSetting->SetMinFirstLineOffset(dec);
		
		if(retrait != -1)
		{
			// Set the frame inset
			if(txtFrameHelper->SetInsetCmd(txtFrameRef,retrait,retrait,retrait,retrait) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		//Set the columns and gutter
		InterfacePtr<ITextColumnSizer> txtFrameColumn (multiColumnItem,UseDefaultIID());
		if(txtFrameColumn == nil)
		{
			ASSERT_FAIL("Func_BM::IDCall_BM -> txtFrameColumn nil");
			error = PMString(kErrNilInterface);		
			break;
		}
		
		
/*		InterfacePtr<ICommand> recalculateColumnsCmd  (CmdUtils::CreateCommand(kRecalculateColumnsCmdBoss ));
		recalculateColumnsCmd->SetItemList(UIDList(txtFrameRef));

		if(CmdUtils::ProcessCommand(recalculateColumnsCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
*/
		InterfacePtr<ICommand> changeColumnsCmd  (CmdUtils::CreateCommand(kChangeNumberOfColumnsCmdBoss ));
		changeColumnsCmd->SetItemList(UIDList(txtFrameRef));

		InterfacePtr<ITextColumnData> columnData (changeColumnsCmd, UseDefaultIID());
		columnData->SetNumberOfColumns(nbCol);
		columnData->SetGutterWidth(gout);
		columnData->UseFixedColumnSizing (-1);
		columnData->SetFixedWidth(-1);

		if(CmdUtils::ProcessCommand(changeColumnsCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		InterfacePtr<IGeometry> frameGeo (txtFrameRef,UseDefaultIID());
		if(frameGeo == nil)
		{
			ASSERT_FAIL("Func_BM::IDCall_BM -> frameGeo nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		if(angle != -1)
		{
			// Rotate the frame
			if(itemTransform->Rotate(txtFrameRef,PMReal(angle),frameGeo->GetStrokeBoundingBox().GetCenter()) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		if(coulFond != -1)
		{
			// Set the color
			UID colorUID = parser->getColorList(coulFond).colorUID;
			if(colorUID == kInvalidUID)
			{
				error = PMString(kErrNoSuchColor);
				colorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
			}

			InterfacePtr<IPersistUIDData> fillRenderAttr ((IPersistUIDData *)::CreateObject(kGraphicStyleFillRenderingAttrBoss,IID_IPERSISTUIDDATA));
			if(fillRenderAttr == nil)
			{
				ASSERT_FAIL("Func_BM::IDCall_BM -> fillRenderAttr nil");
				error = PMString(kErrNilInterface);		
				break;
			}

			fillRenderAttr->SetUID(colorUID);

			InterfacePtr<ICommand> gfxApplyCmd(CmdUtils::CreateCommand(kGfxApplyAttrOverrideCmdBoss));
			if(gfxApplyCmd == nil)
			{
				ASSERT_FAIL("Func_BM::IDCall_BM -> gfxApplyCmd nil");
				error = PMString(kErrNilInterface);		
				break;
			}

			gfxApplyCmd->SetItemList(UIDList(txtFrameRef));
			
			InterfacePtr<IPMUnknownData> iUnknown(gfxApplyCmd,UseDefaultIID());
			if(iUnknown == nil)
			{
				ASSERT_FAIL("Func_BM::IDCall_BM -> iUnknown nil");
				error = PMString(kErrNilInterface);		
				break;
			}

			iUnknown->SetPMUnknown(fillRenderAttr);

			if(CmdUtils::ProcessCommand(gfxApplyCmd) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}
		}

		if(trameFond != -1)
		{
			// Set the tint (other method than this used to set the color : IGraphicAttributeUtils)
			UIDList frameList(txtFrameRef);
			InterfacePtr<ICommand> fillTintCmd (Utils<IGraphicAttributeUtils>()->CreateFillTintCommand(trameFond,&frameList,kTrue,kTrue));
			if(fillTintCmd == nil)
			{
				ASSERT_FAIL("Func_BM::IDCall_BM -> fillTintCmd nil");
				error = PMString(kErrNilInterface);		
				break;
			}

			if(CmdUtils::ProcessCommand(fillTintCmd) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}
		}
#if !COMPOSER
		// check if the frame don't go out of spread
		//if(!isInline && itemManager->CheckContainmentInOwnerSpread(frameGeo) != kSuccess)
		//{
		//	error = PMString(kErrItemOutOfSpread);
		//	break;
		//}
#endif
		// Select the bloc
		if((status = itemManager->SelectPageItem(txtFrameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

	} while(false);

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
Func_AV::Func_AV(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BM::Func_BM -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		ref = reader->ExtractTCLRef(kCurrentTCLRef);
		alignBloc = reader->ExtractInt(0,0,2);

		// $BN$ Inverser le parmetre alignBloc 
		// Correspondance avec la documentation PageMakeUp

		if(alignBloc == 0)
			alignBloc =1;
		else if(alignBloc == 1)
			alignBloc = 0;


		offset = reader->ExtractReel('P','+',0,0);
		
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
		IDCall_AV(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_AV::~Func_AV()
{
}

void Func_AV::IDCall_AV(ITCLParser * parser)
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
		
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_AV::IDCall_AV -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_AV::IDCall_AV -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);
		
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
		else if(ref == kCurrentTCLRef)
			ref = parser->getCurItem();

		UID refBlocUID = parser->getItem(refBloc);
		if(refBlocUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}
		
		UID refUID = parser->getItem(ref);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}
		
		// Set the reference point according to the alignment
		PMPoint refPoint = itemManager->GetAbsReferencePoint(UIDRef(parser->getCurDataBase(), refUID),alignBloc);

		if(ErrorUtils::PMGetGlobalErrorCode() != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		UIDRef refBlocUIDRef = UIDRef(parser->getCurDataBase(), refBlocUID) ;
		
		InterfacePtr<IFrameType> refBlocType (refBlocUIDRef,UseDefaultIID());
		if(refBlocType == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(refBlocType->IsNotAFrame())
		{
			InterfacePtr<IPathGeometry> pathGeo (refBlocType, UseDefaultIID());
			if(pathGeo == nil)
			{
				error = PMString(kErrNilInterface);
				break;
			}
			
			if(Utils<IPathUtils>()->IsLine(pathGeo))
			{
				
				PMPoint startPoint = pathGeo->GetNthPoint(0,0).GetPoint( kAnchorPt);
				PMPoint endPoint = pathGeo->GetNthPoint(0,1).GetPoint( kAnchorPt);
				
				if(startPoint.Y() == endPoint.Y()) // Horizontal rule
				{
					if(startPoint.Y() < refPoint.Y())
					{	
						PMPoint left, right;
						if(startPoint.X() > endPoint.X())
						{
							left = endPoint;
							right = startPoint;
						}
						else
						{
							left = startPoint;
							right = endPoint;
						}
						
						InterfacePtr<IGeometry> refRuleGeo (pathGeo,UseDefaultIID());
						if(refRuleGeo == nil)
						{
							error = PMString(kErrNilInterface);
							break;
						}
						
						PMReal ruleHeight = refRuleGeo->GetStrokeBoundingBox().Height();
				
						if(itemTransform->MoveCmd(refBlocUIDRef, PMPoint(left.X(),refPoint.Y() - ruleHeight)) != kSuccess)
						{
							error = ErrorUtils::PMGetGlobalErrorString();
							break;
						}
					}
				}
				else if(startPoint.X() == endPoint.X()) // Vertical rule
				{
					PMPoint bottom, top;
					if(startPoint.Y() < endPoint.Y())
					{
						bottom = endPoint;
						top = startPoint;
					}
					else
					{
						bottom = startPoint;
						top = endPoint;
					}
			
					if(bottom.Y() < refPoint.Y())
					{		
						InterfacePtr<ICommand> resizeCmd (CmdUtils::CreateCommand(kResizeItemsCmdBoss));
						if(resizeCmd == nil)
						{
							error = PMString(kErrNilInterface);
							break;
						}
						
						InterfacePtr<IResizeItemsCmdData> cmdData (resizeCmd, UseDefaultIID());
						if(resizeCmd == nil)
						{
							error = PMString(kErrNilInterface);
							break;
						}
						
						PMReal vertRedim = ((bottom.Y() - top.Y()) + (refPoint.Y() + offset - bottom.Y())) / (bottom.Y() - top.Y());												 

						cmdData->SetResizeData (Transform::PasteboardCoordinates(), Geometry::PathBounds(),
									startPoint, Geometry::MultiplyBy(Geometry::KeepCurrentValue(), vertRedim));
				
						resizeCmd->SetItemList(UIDList(refBlocUIDRef));
						status = CmdUtils::ProcessCommand(resizeCmd);
						if (status != kSuccess)
						{
							error = ErrorUtils::PMGetGlobalErrorString();
							break;
						}
						
					}
				}
			}
		}
		else
		{
			InterfacePtr<IGeometry> refBlocGeo (refBlocUIDRef,UseDefaultIID());
			if(refBlocGeo == nil)
			{
				error = PMString(kErrNilInterface);
				break;
			}			
			
			PMPoint blocTop = refBlocGeo->GetStrokeBoundingBox().LeftTop();
			::TransformInnerPointToPasteboard(refBlocGeo,&blocTop); 	
			
			if(blocTop.Y() < refPoint.Y())
			{
				PMPoint blocBottom = refBlocGeo->GetStrokeBoundingBox().LeftBottom();
				::TransformInnerPointToPasteboard(refBlocGeo,&blocBottom); 		
				
				PMReal vertRedim = refPoint.Y() + offset - blocBottom.Y();
		 
				if(itemTransform->ResizeFrameCmd(refBlocUIDRef, 0.0, vertRedim, IItemTransform::kLeftTop, kFalse) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
		}
		
		if((status = itemManager->SelectPageItem(refBlocUIDRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		

	} while(false);

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

Func_BA::Func_BA(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BA::Func_BA -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		supprImpr = reader->ExtractBooleen(kFalse);
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
		IDCall_BA(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BA::~Func_BA()
{
}

void Func_BA::IDCall_BA(ITCLParser * parser)
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

		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
			break;
		
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
		
		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDList itemList(UIDRef(parser->getCurDataBase(), refUID));
		InterfacePtr<ICommand> nonPrintCommand (Utils<IGraphicAttributeUtils>()->CreateNonPrintCommand(supprImpr,&itemList,kTrue,kTrue));
		if(CmdUtils::ProcessCommand(nonPrintCommand) != kSuccess)
			break;

		if((status = itemManager->SelectPageItem(UIDRef(parser->getCurDataBase(), refUID))) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
		}
		
	} while(kFalse);
	
	
	if (status == kSuccess)
	{
		parser->setCurItem(refBloc);
	}
	else
	{
		throw TCLError(error);
	}
}

Func_BU::Func_BU(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BU::Func_BU -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
	
		flipH = reader->ExtractBooleen(kFalse); 
		flipV = reader->ExtractBooleen(kFalse); 
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
		IDCall_BU(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BU::~Func_BU()
{
}

void Func_BU::IDCall_BU(ITCLParser * parser)
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
		
		InterfacePtr<IItemTransform> itemTransform (parser->QueryItemTransform());
		if(itemTransform == nil)
		{		
			ASSERT_FAIL("Func_BU::IDCall_BU -> itemTransform nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BU::IDCall_BU -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BU::IDCall_BU -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
		
		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		PMReal xScale = 1.0, yScale = 1.0;

		if(flipH)
			xScale = -1.0;

		if(flipV)
			yScale = -1.0;

		if(itemTransform->Scale(UIDRef(parser->getCurDataBase(), refUID),xScale,yScale,PMPoint(0.0,0.0)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if((status = itemManager->SelectPageItem(UIDRef(parser->getCurDataBase(), refUID))) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
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

Func_BQ::Func_BQ(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BQ::Func_BQ -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		positionType = reader->ExtractBooleen(kTrue);
		yOffset = reader->ExtractReel('P','+',0,0); 	
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
		IDCall_BQ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_BQ::~Func_BQ()
{
}

void Func_BQ::IDCall_BQ(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");
	do
	{
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BQ::IDCall_BQ -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}
				
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
		
		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}
				
		// Test whether it's an inline or not	
		UIDRef blocUIDRef = UIDRef(parser->getCurDataBase(),refUID);
		if(!Utils<IFrameUtils>()->IsPageItemAnchoredObject(blocUIDRef))
		{
			error= PMString(kErrNotAnInline);
			break;
		}
		
		// Get the related kInlineBoss Object
		InterfacePtr<IHierarchy> blocHier (blocUIDRef,UseDefaultIID());
		if(blocHier == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		InterfacePtr<IHierarchy> inlineHier (blocHier->QueryParent()); // An inline KSplineItemBoss is a child of a kInlineBoss
		if(inlineHier == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		// Create commands to modify properties
		InterfacePtr<ICommand> changePositionCmd (CmdUtils::CreateCommand(kChangeAnchoredObjectDataCmdBoss));
		if(changePositionCmd == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		InterfacePtr<IAnchoredObjectData> inlinePos (changePositionCmd, UseDefaultIID());
		if(inlinePos == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		InterfacePtr<IIntData> cmdSelector (inlinePos, UseDefaultIID());
		if(cmdSelector == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		cmdSelector->Set(IAnchoredObjectData::kCmdSelector_Position);
		
		if(positionType == kTrue)
			inlinePos->SetPosition(IAnchoredObjectData::kAboveLine);
		else
			inlinePos->SetPosition(IAnchoredObjectData::kStandardInline);
		
		changePositionCmd->SetItemList(UIDList(::GetUIDRef(inlineHier)));
		
		if(CmdUtils::ProcessCommand(changePositionCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}
		
		InterfacePtr<ICommand> changeOffsetCmd (CmdUtils::CreateCommand(kChangeAnchoredObjectDataCmdBoss));
		if(changeOffsetCmd == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		InterfacePtr<IAnchoredObjectData> inlineOffset (changeOffsetCmd, UseDefaultIID());
		if(inlineOffset == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		InterfacePtr<IIntData> cmdSelector2 (inlineOffset, UseDefaultIID());
		if(cmdSelector2 == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		cmdSelector2->Set(IAnchoredObjectData::kCmdSelector_YOffset);
		
		inlineOffset->SetYOffset(yOffset);
		
		changeOffsetCmd->SetItemList(UIDList(::GetUIDRef(inlineHier)));
		
		if(CmdUtils::ProcessCommand(changeOffsetCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
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


Func_FF::Func_FF(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_FF::Func_FF -> reader nil");
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
		IDCall_FF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_FF::~Func_FF()
{
}

void Func_FF::IDCall_FF(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");
	do
	{
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}
		
		TCLRef previousItemRef = parser->getPreviousItem();
		
		UID previousItemUID = parser->getItem(previousItemRef);
		if(previousItemUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}
				
		// Select the bloc
		if((status = itemManager->SelectPageItem(UIDRef(parser->getCurDataBase(), previousItemUID))) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
	} while(kFalse);
	
	if(status == kSuccess)
		parser->setCurItem(parser->getPreviousItem());
	else
		throw TCLError(error);
}

Func_BO::Func_BO(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BO::Func_BO -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		coulFond = reader->ExtractInt(0,0,MAX_COLOR);
		trameFond = reader->ExtractInt(0,0,100);

		reader->ExtractBooleen(kFalse); // dummy parameter to ensure backward compatibility
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
		IDCall_BO(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_BO
*/
void Func_BO::IDCall_BO(ITCLParser * parser)
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

		// Get helper interface
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{
			ASSERT_FAIL("Func_BO::IDCall_BO -> seq nil");
			error = PMString(kErrNilInterface);		
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();
		
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);

		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		// Check whether the item is an image item of a TIFF file
		InterfacePtr<IHierarchy> itemHier (db, refUID, UseDefaultIID());
		if(itemHier->GetChildCount() != 1)
		{
			error = PMString(kErrTiffImageRequired);
			break;
		}
		
		InterfacePtr<IHierarchy> imageHier (itemHier->QueryChild(0));
		InterfacePtr<IDataLinkReference> imageRef (db, itemHier->GetChildUID(0), UseDefaultIID());
		if(!imageRef){
			break;
		}
		
				// Apply fill color and tint attributes to the image item
		UIDList itemList(db, ::GetUID(imageRef));

	
		UID colorUID = parser->getColorList(coulFond).colorUID;
		if(colorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			colorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
		}
		
		InterfacePtr<ICommand> fillColorCmd (Utils<IGraphicAttributeUtils>()->CreateFillRenderingCommand(colorUID, &itemList, kTrue, kTrue));
		if(CmdUtils::ProcessCommand(fillColorCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
			
		InterfacePtr<ICommand> fillTintCmd (Utils<IGraphicAttributeUtils>()->CreateFillTintCommand(trameFond, &itemList, kTrue, kTrue));
		if(CmdUtils::ProcessCommand(fillTintCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Select the bloc
		if((status = itemManager->SelectPageItem(UIDRef(db, refUID))) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
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

/* Func_BR
*/
Func_BR::Func_BR(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BR::Func_BR -> reader nil");
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
		IDCall_BR(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));

}

/* IDCall_BR
*/
void Func_BR::IDCall_BR(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do
	{
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
		
		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IHierarchy> itemHier (parser->getCurDataBase(), refUID, UseDefaultIID());
		if(itemHier == nil)		
			break;

		InterfacePtr<IGraphicFrameData> graphicFrameData (itemHier, UseDefaultIID());		
		
		PMString itemInfo("[BR=");
#if REF_AS_STRING
		itemInfo += "\"" + refBloc + "\";D";
#else
		itemInfo.AppendNumber(refBloc);
		itemInfo += ";D";
#endif
		if(graphicFrameData && (graphicFrameData->IsGraphicFrame() || graphicFrameData->GetTextContentUID() != kInvalidUID) )
		{
			InterfacePtr<IGeometry> frameGeo (graphicFrameData, UseDefaultIID());
			PMRect bbox = frameGeo->GetPathBoundingBox();

			PMReal width = bbox.Width();
			PMReal height = bbox.Height();
			PMReal absBottom = bbox.Bottom();

			::TransformInnerRectToPasteboard(frameGeo, &bbox);

			if(Utils<IPageItemTypeUtils>()->IsInline(itemHier))
				itemInfo += "0;D0;D";
			else
			{
				UID ownerUID  = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);
				InterfacePtr<ISpread> isSpread (parser->getCurDataBase(),ownerUID, UseDefaultIID());
				if(isSpread)
				{
					error = PMString(kErrItemOutOfSpread);
					break;
				}

				InterfacePtr<IGeometry> ownerPageGeo (parser->getCurDataBase(),ownerUID, UseDefaultIID());

				::TransformPasteboardRectToInner(ownerPageGeo, &bbox);
				itemInfo.AppendNumber(bbox.LeftTop().X(),3,kFalse,kTrue);
				itemInfo += ";D";
				itemInfo.AppendNumber(bbox.LeftTop().Y(),3,kFalse,kTrue);
				itemInfo += ";D";
			}
			
			itemInfo.AppendNumber(width,3,kFalse,kTrue);
			itemInfo += ";D";
			itemInfo.AppendNumber(height,3,kFalse,kTrue);
	
			int32 textflags = 0;
			if(Utils<IFrameUtils>()->IsTextFrame(frameGeo,&textflags))
			{
				itemInfo += ";D";
				if(textflags & IFrameUtils::kTF_Overset)
					itemInfo += "-1";
				else if(Utils<IFrameContentUtils>()->IsEmptyTextFrame(graphicFrameData)) 
					itemInfo.AppendNumber(height,3,kFalse,kTrue);
				else // Compute remaining height
				{
					InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());
					if(itemManager == nil)
						break;

					PMPoint cursorPos = itemManager->GetRelReferencePoint(::GetUIDRef(frameGeo), TEXT_ALIGNEMENT);

					itemInfo.AppendNumber(absBottom - cursorPos.Y(),3,kFalse,kTrue);
				}
			}
		}
		else
		{
			InterfacePtr<IPathGeometry> itemPathGeo (itemHier, UseDefaultIID());

			if(Utils<IPathUtils>()->IsLine(itemPathGeo))
			{
				PMRect bbox = itemPathGeo->GetPathBoundingBox();

				PMPoint p1 = itemPathGeo->GetNthPoint(0,0).GetAnchorPoint();
				PMPoint p2 = itemPathGeo->GetNthPoint(0,1).GetAnchorPoint();

				InterfacePtr<IGeometry> ruleGeo (itemPathGeo, UseDefaultIID());

				::TransformInnerPointToPasteboard(ruleGeo, &p1);
				::TransformInnerPointToPasteboard(ruleGeo, &p2);

				UID ownerUID  = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);
				InterfacePtr<ISpread> isSpread (parser->getCurDataBase(),ownerUID, UseDefaultIID());
				if(isSpread)
				{
					error = PMString(kErrItemOutOfSpread);
					break;
				}

				InterfacePtr<IGeometry> ownerPageGeo (parser->getCurDataBase(),ownerUID, UseDefaultIID());

				::TransformPasteboardPointToInner(ownerPageGeo, &p1);
				::TransformPasteboardPointToInner(ownerPageGeo, &p2);

				// Get Stroke
				InterfacePtr<IGraphicStyleDescriptor> graphicDesc (ruleGeo, UseDefaultIID());
				if(graphicDesc == nil)
					break;

				PMReal stroke;
				Utils<IGraphicAttributeUtils>()->GetStrokeWeight(stroke,graphicDesc);

				if(p1.X() == p2.X()) // vertical rule
				{
					PMPoint top = p1;
					PMReal height = p2.Y() - p1.Y();
					if(p1.Y() > p2.Y())
					{					
						top = p2;
						height = -height;
					}
					
					itemInfo.AppendNumber(top.X(),3,kFalse,kTrue);
					itemInfo += ";D";
					itemInfo.AppendNumber(top.Y(),3,kFalse,kTrue);
					itemInfo += ";D";
					itemInfo.AppendNumber(stroke, 3 , kFalse, kTrue);
					itemInfo += ";D";
					itemInfo.AppendNumber(height, 3 , kFalse, kTrue);
				}
				else if(p1.Y() == p2.Y()) // horizontal rule
				{
					PMPoint left = p1;
					PMReal width = p2.X() - p1.X();
					if(p1.X() > p2.X())
					{					
						left = p2;
						width = -width;
					}
					
					itemInfo.AppendNumber(left.X(),3,kFalse,kTrue);
					itemInfo += ";D";
					itemInfo.AppendNumber(left.Y(),3,kFalse,kTrue);
					itemInfo += ";D";
					itemInfo.AppendNumber(stroke, 3 , kFalse, kTrue);
					itemInfo += ";D";
					itemInfo.AppendNumber(width, 3 , kFalse, kTrue);
				}
				else // diagonal rule
				{
					itemInfo.AppendNumber(p1.X(),3,kFalse,kTrue);
					itemInfo += ";D";
					itemInfo.AppendNumber(p1.Y(),3,kFalse,kTrue);
					itemInfo += ";D";
					itemInfo.AppendNumber(p2.X(),3,kFalse,kTrue);
					itemInfo += ";D";
					itemInfo.AppendNumber(p2.Y(),3,kFalse,kTrue);
					itemInfo += ";D";
					itemInfo.AppendNumber(stroke, 3 , kFalse, kTrue);
				}	

			}
			else
			{
				error = PMString(kErrTCLItemRequired);
				break;
			}
		}

		itemInfo += "]";
		itemInfo.SetTranslatable(kFalse);
		parser->WriteLogFile(itemInfo);

		status = kSuccess;
				
	} while(kFalse);
	
	if(status != kSuccess)
		throw TCLError(error);
}


Func_BDollar::Func_BDollar(ITCLParser * parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BDollar::Func_BDollar -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		largeur = reader->ExtractReel('P','+',0,0);
		hauteur = reader->ExtractReel('P','+',0,0);
		int32 nbPathPoints = reader->ExtractInt(3,3,1000);
		
		PMPathPointList tmp;
		for(int32 i = 0 ; i < nbPathPoints ; ++i)
		{
			PMReal x1 = 0, x2 = 0, x3 = 0, y1 = 0, y2 = 0, y3 = 0;

			x1 = reader->ExtractReel('D','+',0,0);
			y1 = reader->ExtractReel('D','+',0,0);
			x2 = reader->ExtractReel('D','+',0,0);
			y2= reader->ExtractReel('D','+',0,0);
			x3 = reader->ExtractReel('D','+',0,0);
			y3 = reader->ExtractReel('D','+',0,0);

			// Make path points relative to the page
			tmp.push_back(PMPathPoint(PMPoint(x1 + hPos, y1 + vPos),
									   PMPoint(x2 + hPos, y2 + vPos),
									   PMPoint(x3 + hPos, y3 + vPos)));
		}

		ptsList.push_back(tmp);
		isClosedList.push_back(kTrue);

		nomFichier = reader->ExtractString(255);
		decH = reader->ExtractReel('P','+',0,0);
		decV = reader->ExtractReel('P','+',0,0);
		echH = reader->ExtractPourcent('+',100,0);
		echV = reader->ExtractPourcent('+',100,0);
		coulFond = reader->ExtractInt(0,0,MAX_COLOR);
		trameFond = reader->ExtractInt(0,0,100);
		angle = reader->ExtractInt(0,0,360);
		taille = reader->ExtractReel('D','+',12,0);
		type = reader->ExtractInt(0,0,5);
		ancrage = reader->ExtractBooleen(kFalse);
		blocAncrage = reader->ExtractTCLRef(kCurrentTCLRef);
		relative = reader->ExtractBooleen(kFalse);

		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(0,0,MAX_PAGE);

		align = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);

		cellAncrage = reader->ExtractBooleen(FALSE);
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
		IDCall_BPExcl(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));

}


Func_BPCent::Func_BPCent(ITCLParser * parser)
{

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		largeur = reader->ExtractReel('P','+',0,0);
		hauteur = reader->ExtractReel('P','+',0,0);
		int32 nbPathPoints = reader->ExtractInt(3,3,1000);
		
		PMPathPointList tmp;
		for(int32 i = 0 ; i < nbPathPoints ; ++i)
		{
			PMReal x1 = 0, x2 = 0, x3 = 0, y1 = 0, y2 = 0, y3 = 0;

			x1 = reader->ExtractReel('D','+',0,0);
			y1 = reader->ExtractReel('D','+',0,0);
			x2 = reader->ExtractReel('D','+',0,0);
			y2= reader->ExtractReel('D','+',0,0);
			x3 = reader->ExtractReel('D','+',0,0);
			y3 = reader->ExtractReel('D','+',0,0);

			// Make path points relative to the page
			tmp.push_back(PMPathPoint(PMPoint(x1 + hPos, y1 + vPos),
									   PMPoint(x2 + hPos, y2 + vPos),
									   PMPoint(x3 + hPos, y3 + vPos)));
		}

		ptsList.push_back(tmp);
		isClosedList.push_back(kTrue);

		alignVert = reader->ExtractInt(0,0,3);
		dec = reader->ExtractReel('P','+',0,0);
		coulFond = reader->ExtractInt(0,0,MAX_COLOR);
		trameFond = reader->ExtractInt(0,0,100);
		chainage = reader->ExtractBooleen(kFalse);
		blocChaine = reader->ExtractTCLRef(kCurrentTCLRef);
		nbCol = reader->ExtractInt(1,1,MAX_COL);
		gout = reader->ExtractReel('D','+',3,0);
		retrait = reader->ExtractReel('D','+',0,0);
		angle = reader->ExtractInt(0,0,360);
		ancrage = reader->ExtractBooleen(kFalse);
		blocAncrage = reader->ExtractTCLRef(kCurrentTCLRef);
		relative = reader->ExtractBooleen(kFalse);

		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(0,0,MAX_PAGE);

		alignBloc = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);
		vJustMax = reader->ExtractReel('D','+',0,0);
		topAlign = reader->ExtractInt(0,0,4);
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
		IDCall_BPStar(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}


/* Constructor
*/
Func_BZ::Func_BZ(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BZ::Func_BZ -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		type = reader->ExtractInt(0,0,4);
		
		inset = reader->ExtractReel('D','+',0,0);

		// Not used in ID version, same inset value for all sides
		reader->ExtractReel('D','+',0,0); 
		reader->ExtractReel('D','+',0,0);
		reader->ExtractReel('D','+',0,0);

		invert = reader->ExtractBooleen(kFalse);
		allowHoles = reader->ExtractBooleen(kFalse);

		index = reader->ExtractInt(0,-1,50);

		limitBloc = reader->ExtractBooleen(kFalse);
		useHighRes = reader->ExtractBooleen(kTrue);

		threshold = reader->ExtractInt(25,0,255);
		tolerance = reader->ExtractReel3('+','D',2,0,0,10);
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
		IDCall_BZ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}


void Func_BZ::IDCall_BZ(ITCLParser * parser)
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
		
		if(refBloc == kInvalidTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);

		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IFrameType> frameType (db, refUID, UseDefaultIID());
		if(!frameType->IsGraphicFrame())
		{
			InterfacePtr<IStringData> missingImagePath(frameType, IID_IIMAGEPATH);
			if(missingImagePath && !missingImagePath->GetString().IsEmpty()) // It's a text frame, but it should have been an image frame(image is missing) => not an error
				status = kSuccess;
			else
				error = PMString(kErrImageBoxRequired);
			break;
		}

		InterfacePtr<IItemManager> itemManager (parser->QueryItemManager());

		if(itemManager->SelectPageItem(UIDRef(db,refUID)) != kSuccess) 
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(!frameType->IsEmpty())
		{
			/*	
			// Get Image item
			InterfacePtr<IHierarchy> itemHier (frameType, UseDefaultIID());

			InterfacePtr<ICommand> setClippingPathCmd (CmdUtils::CreateCommand(kSetClipValuesCmdBoss));
			PMString dummyName("Set Clipping Path", -1, PMString::kNoTranslate);
			setClippingPathCmd->SetName(dummyName);
			setClippingPathCmd->SetUndoability(ICommand::kAutoUndo);

			InterfacePtr<ISetClipValuesCmdData> cmdData (setClippingPathCmd, IID_ISETCLIPVALUESCMDDATA);

			// Set target image
			cmdData->SetImageItem(db, itemHier->GetChildUID(0));

			// Set clipping path data
			cmdData->Set((IClipSettings::ClipType)type, index, tolerance, inset, threshold, invert, useHighRes, allowHoles, limitBloc);

			if(CmdUtils::ProcessCommand(setClippingPathCmd) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			*/
					
			InterfacePtr<IClippingPathSuite> iClipPathSuite ((IClippingPathSuite *)Utils<ISelectionUtils>()->QuerySuite(IID_ICLIPPINGPATHSUITE));
			if (!iClipPathSuite)
			{
#if SERVER
				status = kSuccess; // Not an error under ID Server, selection manager is not available
#endif
				break;
			}

			if(type == 0) // None
				status = iClipPathSuite->RemoveClipping();
			else
			{
				 uint32 flags = 0;
				 if(useHighRes)
					flags |= IClippingPathSuite::kUseHighResFlag;
				 if(allowHoles)
					flags |= IClippingPathSuite::kAllowHolesFlag;
				 if (invert)
					flags |= IClippingPathSuite::kInvertFlag;
				 if (limitBloc)
					flags |= IClippingPathSuite::kRestrictToFrameFlag;

				 switch(type)
				 {
					case 1 :  status = iClipPathSuite->DoDetectEdges(tolerance, 2*tolerance, inset, flags, threshold); break;
					
					case 2 : 
						if (iClipPathSuite->HasAlphaChannels())										
							status = iClipPathSuite->ApplyAlphaChannel(index, tolerance, 2*tolerance, inset, flags, threshold);
						else
							status = kFailure;
						break;

					case 3 :
						if (iClipPathSuite->HasPhotoshopPaths())
							status = iClipPathSuite->ApplyEmbeddedPath(index, nil, tolerance, 2*tolerance, inset,flags);
						else
							status = kFailure;
						break;
	            }

			}

		}
		else
			status = kSuccess;

	} while(kFalse);


	if(status != kSuccess)
		throw TCLError(error);
}


Func_BExcl::Func_BExcl(ITCLParser * parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BExcl::Func_BExcl -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		largeur = reader->ExtractReel('P','+',0,0);
		hauteur = reader->ExtractReel('P','+',0,0);
		int32 nbPaths = reader->ExtractInt(1,1,1000);

		for(int32 path = 0 ; path < nbPaths ; ++path)
		{
			int32 nbPathPoints = reader->ExtractInt(1,1,1000);
			isClosedList.push_back(reader->ExtractBooleen(kFalse));

			PMPathPointList pathPoints;

			for(int32 i = 0 ; i < nbPathPoints ; ++i)
			{
				PMReal x1 = 0, x2 = 0, x3 = 0, y1 = 0, y2 = 0, y3 = 0;

				x1 = reader->ExtractReel('D','+',0,0);
				y1 = reader->ExtractReel('D','+',0,0);
				x2 = reader->ExtractReel('D','+',0,0);
				y2= reader->ExtractReel('D','+',0,0);
				x3 = reader->ExtractReel('D','+',0,0);
				y3 = reader->ExtractReel('D','+',0,0);

				// Make path points relative to the page
				pathPoints.push_back(PMPathPoint(PMPoint(x1 + hPos, y1 + vPos),
										   PMPoint(x2 + hPos, y2 + vPos),
										   PMPoint(x3 + hPos, y3 + vPos)));
			}
			ptsList.push_back(pathPoints);
		}

		nomFichier = reader->ExtractString(255);
		decH = reader->ExtractReel('P','+',0,0);
		decV = reader->ExtractReel('P','+',0,0);
		echH = reader->ExtractPourcent('+',100,0);
		echV = reader->ExtractPourcent('+',100,0);
		coulFond = reader->ExtractInt(0,0,MAX_COLOR);
		trameFond = reader->ExtractInt(0,0,100);
		angle = reader->ExtractInt(0,0,360);
		taille = reader->ExtractReel('D','+',12,0);
		type = reader->ExtractInt(0,0,5);
		ancrage = reader->ExtractBooleen(kFalse);
		blocAncrage = reader->ExtractTCLRef(kCurrentTCLRef);
		relative = reader->ExtractBooleen(kFalse);

		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(0,0,MAX_PAGE);

		align = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);

		cellAncrage = reader->ExtractBooleen(FALSE);
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
		IDCall_BPExcl(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}


/* IDCall_BPExcl
*/
void Func_BExcl::IDCall_BPExcl(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error("");
	UID newFrame;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Get helper interfaces
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();

		// Define the frame path
		K2Vector<PMPointList> points;
		for(int32 i = 0 ; i < ptsList.size() ; ++i)
		{
			PMPointList tmp;
			for(int32 j = 0 ; j < ptsList[i].size() ; ++j)
			{
				PMPathPoint p = ptsList[i][j];
				tmp.push_back(p.GetLeftDirPoint());
				tmp.push_back(p.GetAnchorPoint());
				tmp.push_back(p.GetRightDirPoint());
			}
			points.push_back(tmp);
		}
		
		UID ownerSpreadLayer = kInvalidUID;
		
		InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(parser->getCurDoc()));
		if(activeLayer == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(!ancrage) // no owner spread, the frame will be an inline
		{
			if(page == 0)
				page = parser->getCurPage();
				
			for(int32 i = 0 ; i < points.size() ; ++i)
				ownerSpreadLayer = itemManager->GetPBCoordAbs(parser->getCurDoc(),page,points[i],activeLayer);
			
			if(ownerSpreadLayer == kInvalidUID)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Recreate path point list with modified points	
		PMPathPointList tmp;
		for(int32 j = 0 ; j < points[0].size() ; j += 3)
			tmp.push_back(PMPathPoint(points[0][j],points[0][j+1],points[0][j+2]));	

		UIDRef imFrameRef = Utils<IPathUtils>()->CreateSpline(UIDRef(db,ownerSpreadLayer), tmp,INewPageItemCmdData::kGraphicFrameAttributes, kTrue, isClosedList[0]);		
		
			
		for(int32 i = 1 ; i < points.size() ; ++i)
		{
			tmp.clear();
			for(int32 j = 0 ; j < points[i].size() ; j += 3)
				tmp.push_back(PMPathPoint(points[i][j],points[i][j+1],points[i][j+2]));
				
			UIDList itemList(imFrameRef);	
			itemList.Append(Utils<IPathUtils>()->CreateSpline(UIDRef(db,ownerSpreadLayer), tmp,INewPageItemCmdData::kGraphicFrameAttributes, kTrue, isClosedList[i]));			
			InterfacePtr<ICommand> makeCmd ( CmdUtils::CreateCommand( kMakeCompoundPathCmdBoss ) );
			InterfacePtr<IBoolData> iBoolData (makeCmd, IID_IBOOLDATA);
			iBoolData->Set (kFalse); // Tell the command NOT to reverse every other path
			makeCmd->SetItemList( itemList ) ;
			CmdUtils::ProcessCommand( makeCmd ) ;

			imFrameRef = makeCmd->GetItemList()->GetRef(0);
		}
		
		if(imFrameRef == UIDRef(nil,kInvalidUID))
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		if(this->ApplyImageFrameAttr(parser, imFrameRef, error) != kSuccess)
			break;
		
#if !COMPOSER
		// check if the frame don't go out of spread
		//if(!ancrage && itemManager->CheckContainmentInOwnerSpread(frameGeo) != kSuccess)
		//{
		//	error = PMString(kErrItemOutOfSpread);
		//	break;
		//}
#endif
		if((status = itemManager->SelectPageItem(imFrameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		newFrame = imFrameRef.GetUID();

	} while(false);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setItem(refBloc,newFrame);

			if(!cellAncrage) // we keep current table selection in case of insertion in a cell
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

Func_BStar::Func_BStar(ITCLParser * parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BDollar::Func_BDollar -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		largeur = reader->ExtractReel('P','+',0,0);
		hauteur = reader->ExtractReel('P','+',0,0);
		int32 nbPaths = reader->ExtractInt(1,1,1000);

		for(int32 path = 0 ; path < nbPaths ; ++path)
		{
			int32 nbPathPoints = reader->ExtractInt(1,1,1000);
			isClosedList.push_back(reader->ExtractBooleen(kFalse));

			PMPathPointList pathPoints;

			for(int32 i = 0 ; i < nbPathPoints ; ++i)
			{
				PMReal x1 = 0, x2 = 0, x3 = 0, y1 = 0, y2 = 0, y3 = 0;

				x1 = reader->ExtractReel('D','+',0,0);
				y1 = reader->ExtractReel('D','+',0,0);
				x2 = reader->ExtractReel('D','+',0,0);
				y2= reader->ExtractReel('D','+',0,0);
				x3 = reader->ExtractReel('D','+',0,0);
				y3 = reader->ExtractReel('D','+',0,0);

				// Make path points relative to the page
				pathPoints.push_back(PMPathPoint(PMPoint(x1 + hPos, y1 + vPos),
										   PMPoint(x2 + hPos, y2 + vPos),
										   PMPoint(x3 + hPos, y3 + vPos)));
			}
			ptsList.push_back(pathPoints);
		}

		alignVert = reader->ExtractInt(0,0,3);
		dec = reader->ExtractReel('P','+',0,0);
		coulFond = reader->ExtractInt(0,0,MAX_COLOR);
		trameFond = reader->ExtractInt(0,0,100);
		chainage = reader->ExtractBooleen(kFalse);
		blocChaine = reader->ExtractTCLRef(kCurrentTCLRef);
		nbCol = reader->ExtractInt(1,1,MAX_COL);
		gout = reader->ExtractReel('D','+',3,0);
		retrait = reader->ExtractReel('D','+',0,0);
		angle = reader->ExtractInt(0,0,360);
		ancrage = reader->ExtractBooleen(kFalse);
		blocAncrage = reader->ExtractTCLRef(kCurrentTCLRef);
		relative = reader->ExtractBooleen(kFalse);

		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(0,0,MAX_PAGE);

		alignBloc = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);
		vJustMax = reader->ExtractReel('D','+',0,0);
		topAlign = reader->ExtractInt(0,0,4);
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
		IDCall_BPStar(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}


/* IDCall_BPStar
*/
void Func_BStar::IDCall_BPStar(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error("");
	UID newFrame;

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Get helper interfaces
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();

		// Define the frame path
		K2Vector<PMPointList> points;
		for(int32 i = 0 ; i < ptsList.size() ; ++i)
		{
			PMPointList tmp;
			for(int32 j = 0 ; j < ptsList[i].size() ; ++j)
			{
				PMPathPoint p = ptsList[i][j];
				tmp.push_back(p.GetLeftDirPoint());
				tmp.push_back(p.GetAnchorPoint());
				tmp.push_back(p.GetRightDirPoint());
			}
			points.push_back(tmp);
		}
		
		UID ownerSpreadLayer = kInvalidUID;
		
		InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(parser->getCurDoc()));
		if(activeLayer == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(!ancrage) // no owner spread, the frame will be an inline
		{
			if(page == 0)
				page = parser->getCurPage();
				
			for(int32 i = 0 ; i < points.size() ; ++i)
				ownerSpreadLayer = itemManager->GetPBCoordAbs(parser->getCurDoc(),page,points[i],activeLayer);
			
			if(ownerSpreadLayer == kInvalidUID)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Recreate path point list with modified points	
		PMPathPointList tmp;
		for(int32 j = 0 ; j < points[0].size() ; j += 3)
			tmp.push_back(PMPathPoint(points[0][j],points[0][j+1],points[0][j+2]));	

		UIDRef txtFrameRef = Utils<IPathUtils>()->CreateSpline(UIDRef(db,ownerSpreadLayer), tmp,INewPageItemCmdData::kGraphicFrameAttributes, kTrue, isClosedList[0]);		
		
		for(int32 i = 1 ; i < points.size() ; ++i)
		{
			tmp.clear();
			for(int32 j = 0 ; j < points[i].size() ; j += 3)
				tmp.push_back(PMPathPoint(points[i][j],points[i][j+1],points[i][j+2]));
				
			UIDList itemList(txtFrameRef);	
			itemList.Append(Utils<IPathUtils>()->CreateSpline(UIDRef(db,ownerSpreadLayer), tmp,INewPageItemCmdData::kGraphicFrameAttributes, kTrue, isClosedList[i]));			
			InterfacePtr<ICommand> makeCmd ( CmdUtils::CreateCommand( kMakeCompoundPathCmdBoss ) );
			InterfacePtr<IBoolData> iBoolData (makeCmd, IID_IBOOLDATA);
			iBoolData->Set (kFalse); // Tell the command NOT to reverse every other path
			makeCmd->SetItemList( itemList ) ;
			CmdUtils::ProcessCommand( makeCmd ) ;

			txtFrameRef = makeCmd->GetItemList()->GetRef(0);
		}

		if(txtFrameRef == UIDRef(nil,kInvalidUID))
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		// Convert the frame to a text frame
		InterfacePtr<ICommand> convertToTextCmd (CmdUtils::CreateCommand(kConvertItemToTextCmdBoss));
		convertToTextCmd->SetItemList(UIDList(txtFrameRef));

		if(CmdUtils::ProcessCommand(convertToTextCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Set columns properties
		/*InterfacePtr<ICommand> recalculateColumnsCmd  (CmdUtils::CreateCommand(kRecalculateColumnsCmdBoss));
		recalculateColumnsCmd->SetItemList(UIDList(txtFrameRef));

		if(CmdUtils::ProcessCommand(recalculateColumnsCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}*/

		InterfacePtr<ICommand> changeColumnsCmd  (CmdUtils::CreateCommand(kChangeNumberOfColumnsCmdBoss));
		changeColumnsCmd->SetItemList(UIDList(txtFrameRef));

		InterfacePtr<ITextColumnData> columnData (changeColumnsCmd, UseDefaultIID());
		columnData->SetNumberOfColumns(nbCol);
		columnData->SetGutterWidth(gout);
		columnData->UseFixedColumnSizing (-1);
		columnData->SetFixedWidth(-1);

		if(CmdUtils::ProcessCommand(changeColumnsCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		if(this->ApplyTextFrameAttr(parser, txtFrameRef, error) != kSuccess)
			break;
		
#if !COMPOSER
		// check if the frame don't go out of spread
		//if(!ancrage && itemManager->CheckContainmentInOwnerSpread(frameGeo) != kSuccess)
		//{
		//	error = PMString(kErrItemOutOfSpread);
		//	break;
		//}
#endif
		if((status = itemManager->SelectPageItem(txtFrameRef)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		newFrame = txtFrameRef.GetUID();

	} while(false);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setItem(refBloc,newFrame);
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

/* Func_BL
*/
Func_BL::Func_BL(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BL::Func_BL -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		largeur = reader->ExtractReel('P','+',0,0);
		hauteur = reader->ExtractReel('P','+',0,0);
		nomFichier = reader->ExtractString(255);
		decH = reader->ExtractReel('P','+',0,0);
		decV = reader->ExtractReel('P','+',0,0);
		echH = reader->ExtractPourcent('+',100,0);
		echV = reader->ExtractPourcent('+',100,0);
		coulFond = reader->ExtractInt(-1,-1,MAX_COLOR);
		trameFond = reader->ExtractPourcent('+',0,0);
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
		IDCall_BL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_BL
*/
void Func_BL::IDCall_BL(ITCLParser * parser)
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

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		UIDRef frameRef = UIDRef(db,refUID);

		InterfacePtr<IGeometry> frameGeo (frameRef, UseDefaultIID());
		if(Utils<IFrameUtils>()->IsTextFrame(frameGeo))
		{
			error = PMString(kErrImageBoxRequired);
			break;
		}

		// Get helper interfaces
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<IHierarchy> frameHier (frameRef, UseDefaultIID());

		bool16 isInline = Utils<IPageItemTypeUtils>()->IsInline(frameRef);
		if(!isInline)
		{
			// Get current owner spread			
			UID ownerSpread = frameHier->GetSpreadUID(); 

			// Move frame, hPos and vPos are offsets
			ErrorCode moveItemCmd = Utils<Facade::ITransformFacade>()->TransformItems(UIDList(frameRef), Transform::PasteboardCoordinates(), kZeroPoint, 
					 Transform::TranslateBy(hPos, vPos));
		
			if(moveItemCmd != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		if(largeur != 0.0 || hauteur != 0.0) // Resize frame if required
		{
			PMPoint refPoint = frameGeo->GetStrokeBoundingBox().LeftTop();
			::TransformInnerPointToPasteboard(frameGeo, &(refPoint));

			if(largeur == 0.0)
				largeur = frameGeo->GetStrokeBoundingBox().Width();

			if(hauteur == 0.0)
				hauteur = frameGeo->GetStrokeBoundingBox().Height();

			ErrorCode resizeCmd =  Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(frameRef), Transform::PasteboardCoordinates(), 
					Geometry::OuterStrokeBounds(), refPoint, Geometry::ResizeTo(largeur, hauteur));
			
			if(resizeCmd != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Re-import the image into the item
		bool8 pictImport, useImportPath, createTextBox, nonPrinting;
		PMString pictPath;
		int32 colorIndex;
		int32 colorTint;
		UIDRef imageItemRef(nil,kInvalidUID);
		parser->getPicturePrefs(&useImportPath, &pictImport,&pictPath,&createTextBox,&nonPrinting,&colorIndex,&colorTint);

		if(pictImport && !nomFichier.IsEmpty())
		{
			PMString absolutePath;
			
			if(useImportPath)
				absolutePath.Append(pictPath);
			
			absolutePath.Append(nomFichier);

			// Test whether there is already an image within the frame
			// If so and if echH = echV = 0, keep scale and offset of the existing image
			InterfacePtr<IHierarchy> frameHier (frameRef, UseDefaultIID());
			if(echH == 0 && echV == 0 && frameHier->GetChildCount() == 1)
			{
				InterfacePtr<IGeometry> imageFrameGeo (db, frameHier->GetChildUID(0), UseDefaultIID());
				
				PMPoint imageLeftTop = imageFrameGeo->GetStrokeBoundingBox().LeftTop();
				PMPoint leftTop = frameGeo->GetStrokeBoundingBox().LeftTop();

				::TransformInnerPointToPasteboard(frameGeo, &leftTop);
				::TransformInnerPointToPasteboard(imageFrameGeo, &imageLeftTop);

				decH = imageLeftTop.X() - leftTop.X();
				decV = imageLeftTop.Y() - leftTop.Y();

				InterfacePtr<ITransform> imageTransform (imageFrameGeo, UseDefaultIID());
				echH = imageTransform->GetItemScaleX();
				echV = imageTransform->GetItemScaleY();
			}

			if(itemManager->ImportImageInGraphicFrameCmd(frameRef,absolutePath,imageItemRef) == kSuccess)
			{
				// Scale the image
				InterfacePtr<IGeometry> imGeo (imageItemRef,UseDefaultIID());		

				// The reference point is the image left top corner in image coordinate system
				if(itemTransform->Scale(imageItemRef,echH,echV,PMPoint(imGeo->GetStrokeBoundingBox().LeftTop())) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
				
				// Calculate the new position of the left top corner, then move the image
				PMPoint leftTop (frameGeo->GetStrokeBoundingBox().LeftTop());
				PMPoint newLeftTop (leftTop.X() + decH, leftTop.Y() + decV);
				::TransformInnerPointToPasteboard(frameGeo,&newLeftTop);

				if(itemTransform->MoveCmd(imageItemRef,newLeftTop) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
			else
			{
				if(!createTextBox)
					ErrorUtils::PMSetGlobalErrorCode(kSuccess);
				else
				{
					ErrorUtils::PMSetGlobalErrorCode(kSuccess); 
			
					// Convert item to text frame
					InterfacePtr<ICommand> convertToTextCmd (CmdUtils::CreateCommand(kConvertItemToTextCmdBoss));
					convertToTextCmd->SetItemList(UIDList(frameRef));
					if(CmdUtils::ProcessCommand(convertToTextCmd) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}

					// Insert name of missing file in frame
					InterfacePtr<IHierarchy> txtItemHier (frameRef, UseDefaultIID());
					InterfacePtr<IMultiColumnTextFrame> txtFrame (db, txtItemHier->GetChildUID(0), UseDefaultIID());
					InterfacePtr<ITextModelCmds> txtModelCmds (db, txtFrame->GetTextModelUID(), UseDefaultIID());
					boost::shared_ptr<WideString> toInsert (new WideString(absolutePath));
					InterfacePtr<ICommand> insertCmd (txtModelCmds->InsertCmd(0, toInsert));
					if(CmdUtils::ProcessCommand(insertCmd) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}

					if(nonPrinting)
					{
						UIDList itemList(frameRef);
						InterfacePtr<ICommand> nonPrintCommand (Utils<IGraphicAttributeUtils>()->CreateNonPrintCommand(kTrue,&itemList,kTrue,kTrue));
						if(CmdUtils::ProcessCommand(nonPrintCommand) != kSuccess)
							break;
					}
					coulFond = colorIndex;
					trameFond = colorTint/100.0;
				}

				// Store missing file's info
				InterfacePtr<ICommand> setMissingFileInfoCmd (CmdUtils::CreateCommand(kPrsSetImageInfosCmdBoss));
				setMissingFileInfoCmd->SetItemList(UIDList(frameRef));
				
				InterfacePtr<IPMPointData> scaleData (setMissingFileInfoCmd, IID_IIMAGESCALE);
				InterfacePtr<IPMPointData> shiftData (setMissingFileInfoCmd, IID_IIMAGESHIFT);
				InterfacePtr<IStringData> pathData (setMissingFileInfoCmd, IID_IIMAGEPATH);

				scaleData->Set(PMPoint(echH,echV));
				shiftData->Set(PMPoint(decH,decV));
				pathData->Set(absolutePath);

				if(CmdUtils::ProcessCommand(setMissingFileInfoCmd) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
		}
		UIDList frameList (frameRef);

		if(coulFond != -1) // Set the fill color
		{
			UID colorUID = parser->getColorList(coulFond).colorUID;
			if(colorUID == kInvalidUID)
			{
				error = PMString(kErrNoSuchColor);
				colorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
			}

			InterfacePtr<ICommand> fillRenderAttrCmd (Utils<IGraphicAttributeUtils>()->CreateFillRenderingCommand(colorUID, &frameList, kTrue, kTrue));
			if(CmdUtils::ProcessCommand(fillRenderAttrCmd) != kSuccess) 
				break;
		}

		if(trameFond != 0.0) // Set the fill tint 
		{
			InterfacePtr<ICommand> fillTintAttrCmd (Utils<IGraphicAttributeUtils>()->CreateFillTintCommand(trameFond*100.0, &frameList, kTrue, kTrue));
			if(CmdUtils::ProcessCommand(fillTintAttrCmd) != kSuccess) 
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

/* Func_IT
  // GD 29.05.2017 pour Foucauld Perotin
 */
Func_IT::Func_IT(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{
		ASSERT_FAIL("Func_IT::Func_IT -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}
    
	try
	{
		reader->MustEgal();
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		nomFichier = reader->ExtractString(255);
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
		IDCall_IT(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_IT
  // GD 29.05.2017 pour Foucauld Perotin
 */
void Func_IT::IDCall_IT(ITCLParser * parser)
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
        
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
        
		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}
        
		UIDRef frameRef = UIDRef(db,refUID);
        
		InterfacePtr<IGeometry> frameGeo (frameRef, UseDefaultIID());
		if(Utils<IFrameUtils>()->IsTextFrame(frameGeo) != kTrue)
		{
			error = PMString(kErrTextBoxRequired);
			break;
		}
        
		// Get helper interfaces
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
        
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil)
			break;
        
		seq->SetUndoability(ICommand::kAutoUndo);
        
		if(!nomFichier.IsEmpty())
		{
			PMString absolutePath;
			absolutePath.Append(nomFichier);
            
			// TEST
            /*TextIndex insertPos = kInvalidTextIndex;
            InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
            if (txtModel == nil)
            {
                error = ErrorUtils::PMGetGlobalErrorString();
                break;
            }
			InterfacePtr<IHierarchy> txtItemHier (frameRef, UseDefaultIID());
			InterfacePtr<IMultiColumnTextFrame> txtFrame (db, txtItemHier->GetChildUID(0), UseDefaultIID());
			InterfacePtr<ITextModelCmds> txtModelCmds (db, txtFrame->GetTextModelUID(), UseDefaultIID());
			boost::shared_ptr<WideString> toInsert (new WideString(absolutePath));
			InterfacePtr<ICommand> insertCmd (txtModelCmds->InsertCmd(insertPos, toInsert));
			if(CmdUtils::ProcessCommand(insertCmd) != kSuccess)
			{
                error = ErrorUtils::PMGetGlobalErrorString();
                break;
            }
             */
            SDKFileHelper fileHelper(absolutePath);
            if(!fileHelper.IsExisting())
            {
                error = PMString(kErrFileNotExist);
                break;
            }
            IDFile txtFileToImport = fileHelper.GetIDFile();
            
            //CAlert::InformationAlert("DEBUG - fichier : " + FileUtils::SysFileToPMString(txtFileToImport));
            
            InterfacePtr<IK2ServiceRegistry> services(GetExecutionContextSession(), UseDefaultIID());
            if (services == nil)
            {
                error = PMString(kErrNilInterface);
                break;
            }
            InterfacePtr<IK2ServiceProvider> service (services->QueryServiceProviderByClassID(kImportProviderService, kTaggedTextImportFilterBoss));
            InterfacePtr<IImportProvider> provider (service, IID_IIMPORTPROVIDER);
            InterfacePtr<IPMStream> stream (StreamUtil::CreateFileStreamRead(txtFileToImport));
            if (provider->CanImportThisStream(stream) == IImportProvider::kFullImport)
            {
                db->BeginTransaction();
                UIDRef result = UIDRef::gNull;
                provider->ImportThis(db, stream, kSuppressUI, &result);
                if (result != nil)
                {
                    InterfacePtr<IHierarchy> txtItemHier (frameRef, UseDefaultIID());
                    InterfacePtr<IMultiColumnTextFrame> txtFrame (db, txtItemHier->GetChildUID(0), UseDefaultIID());
                    UIDRef txtFrameUIDRef = UIDRef(db, txtFrame->GetTextModelUID());
                    Utils<ITextUtils> textUtils;
                    InterfacePtr<ICommand> command (textUtils->QueryMoveStoryFromAllToAllCommand(result, txtFrameUIDRef));
                    CmdUtils::ProcessCommand(command);
                }
                db->EndTransaction();
            }
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
