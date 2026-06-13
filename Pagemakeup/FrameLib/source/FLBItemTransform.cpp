/*
//	File:	FLBItemTransform.cpp
//
//  Author: Trias
//
//	Date:	19-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/


// Plug-in includes
#include "VCPlugInHeaders.h"

// General includes
#include "CmdUtils.h"
#include "UIDList.h"
#include "TransformUtils.h"
#include "WideString.h"
#include "TextChar.h"
#include "ErrorUtils.h"
#include "K2SmartPtr.h"
#include "ITextModelCmds.h"
#include "IPageItemUtils.h"
#include "IFrameList.h"
#include "IOwnedItem.h"
#include "IFrameListComposer.h"
#include "IFrameUtils.h"
#include "ITextParcelListComposer.h"
#include "IIntData.h"
#include "IFrameListComposer.h"
#include "IMargins.h"

// Interface includes
#include "ICommand.h"
#include "IHierarchy.h"
#include "IGeometry.h"
#include "ITransformFacade.h"
#include "IGeometryFacade.h"
#include "IUIDData.h"
#include "IRangeData.h"
#include "ITextModel.h"
#include "IMultiColumnTextFrame.h"
#include "ITextFrameColumn.h"
#include "IStandOffMarginCmdData.h"
#include "IStandOffFormCmdData.h"
#include "IStandOffModeCmdData.h"
#include "IPointListData.h"
#include "ISetBoundingBoxCmdData.h"
#include "IPageItemTypeUtils.h"
#include "IStandOffContourWrapSettings.h"
#include "ITextWrapFacade.h"
#include "IBoundsData.h"
#include "IWaxStrand.h"
#include "IWaxIterator.h"
#include "IWaxLine.h"
#include "IWaxLineShape.h"
#include "IGraphicAttributeUtils.h"
#include "IGraphicStyleDescriptor.h"
#include "ISpread.h"
#include "IFrameContentFacade.h"

#include "CALert.h"

// Project includes
#include "FLBItemTransform.h"
#include "ILayoutUtils.h"
#include "PMLineSeg.h"


CREATE_PMINTERFACE(FLBItemTransform,kFLBItemTransformImpl)

/* Constructor
*/
FLBItemTransform::FLBItemTransform(IPMUnknown * boss)
: CPMUnknown<IItemTransform>(boss)
{
}

/* Destructor
*/
FLBItemTransform::~FLBItemTransform()
{
}

/* Rotate
*/
ErrorCode FLBItemTransform::Rotate(UIDRef spline, PMReal angle, PMPoint refPoint, bool16 absolute)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(spline.GetDataBase() == nil || spline.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemTransform::Rotate -> invalid spline");
			error = PMString(kFlbNilParameter);
			break;
		}

		InterfacePtr<IGeometry> frameGeo (spline,UseDefaultIID());
		if(frameGeo == nil)
		{
			ASSERT_FAIL("FLBItemTransform::Rotate -> frameGeo nil");
			error = PMString(kErrNotSpline);
			break;
		}

		// Convert the reference point in pasteboard coordinates
		::TransformInnerPointToPasteboard(frameGeo,&refPoint);
		if(absolute)
			status = Utils<Facade::ITransformFacade>()->TransformItems(UIDList(spline), Transform::PasteboardCoordinates(),refPoint, Transform::RotateTo(angle));
		else
			status = Utils<Facade::ITransformFacade>()->TransformItems(UIDList(spline), Transform::PasteboardCoordinates(),refPoint, Transform::RotateBy(angle));

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* Scale
*/
ErrorCode FLBItemTransform::Scale(UIDRef spline, PMReal xScale, PMReal yScale, PMPoint refPoint)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(spline.GetDataBase() == nil || spline.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemTransform::Scale -> invalid spline");
			error = PMString(kFlbNilParameter);
			break;
		}

		InterfacePtr<IGeometry> frameGeo (spline,UseDefaultIID());
		if(frameGeo == nil)
		{
			ASSERT_FAIL("FLBItemTransform::Scale -> frameGeo nil");
			error = PMString(kErrNotSpline);
			break;
		}

		// Convert the reference point in pasteboard coordinates
		::TransformInnerPointToPasteboard(frameGeo,&refPoint);
	
		status = Utils<Facade::ITransformFacade>()->TransformItems( UIDList(spline), Transform::PasteboardCoordinates(), refPoint, Transform::ScaleTo(xScale,yScale));

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* MoveCmd
*/
ErrorCode FLBItemTransform::MoveCmd(UIDRef spline, PMPoint newLeftTopCorner, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(spline.GetDataBase() == nil || spline.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemTransform::MoveCmd -> invalid spline");
			error = PMString(kFlbNilParameter);
			break;
		}

		InterfacePtr<ICommand> moveItemCmd (CmdUtils::CreateCommand(kMoveAbsoluteCmdBoss));
		if(moveItemCmd == nil)
		{
			ASSERT_FAIL("FLBItemTransform::MoveCmd -> moveItemCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			moveItemCmd->SetUndoability(ICommand::kAutoUndo);

		moveItemCmd->SetItemList(UIDList(spline));

		// Set the new position of the left top corner
		PMPointList pointList;
		pointList.push_back(newLeftTopCorner);

		InterfacePtr<IPointListData> iPtListData (moveItemCmd,UseDefaultIID());
		if(iPtListData == nil)
		{
			ASSERT_FAIL("FLBItemTransform::MoveCmd -> iPtListData nil");
			break;
		}

		iPtListData->Set(pointList);
		
		status = CmdUtils::ProcessCommand(moveItemCmd);
		if(status != kSuccess)
			error = PMString(kMoveItemCmdFailed);

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}
/* MakeInline
*/
ErrorCode FLBItemTransform::MakeInlineCmd(UIDRef spline, UIDRef receiver, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(spline.GetDataBase() == nil || spline.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemTransform::Rotate -> invalid spline");
			error = PMString(kFlbNilParameter);
			break;
		}

		// Test if the frame is a text frame and then get its text model
		InterfacePtr<IHierarchy> blocAncHier (receiver, UseDefaultIID()); 
		if(blocAncHier == nil)
		{
			ASSERT_FAIL("FLBItemTransform::MakeInlineCmd -> blocAncHier nil");
			error = PMString(kErrNotSpline);
			break;
		}

		InterfacePtr<IHierarchy> multiCol (blocAncHier->QueryChild(0)); 
		if(multiCol == nil)  // not a text frame, cannot insert an inline
		{
			ASSERT_FAIL("FLBItemTransform::MakeInlineCmd -> multiCol nil");
			error = PMString(kErrNotTextFrame);
			break;
		}

		InterfacePtr<IMultiColumnTextFrame> frameAnc (multiCol,UseDefaultIID());
		if(frameAnc == nil) // not a text frame, cannot insert an inline
		{
			ASSERT_FAIL("FLBItemTransform::MakeInlineCmd -> frameAnc nil");
			error = PMString(kErrNotTextFrame);
			break;
		}
 
		InterfacePtr<ITextModel> txtModel (frameAnc->QueryTextModel());
		if(txtModel == nil)
		{
			ASSERT_FAIL("FLBItemTransform::MakeInlineCmd -> txtModel nil");
			break;
		}

		// Frame will be anchored at the end of the story
		TextIndex frameFinalTextIndex = txtModel->GetPrimaryStoryThreadSpan() - 1;
		
		// Insert character in text flow to anchor the inline.
		InterfacePtr<ITextModelCmds>modelCmds(txtModel, UseDefaultIID());
		boost::shared_ptr<WideString> replaceChar( new WideString());
		replaceChar->Append(kTextChar_ObjectReplacementCharacter); 
		InterfacePtr<ICommand> insertTextCmd(modelCmds->InsertCmd(frameFinalTextIndex, replaceChar));
		if(insertTextCmd == nil)
		{
			ASSERT_FAIL("FLBItemTransform::MakeInlineCmd -> insertTextCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			insertTextCmd->SetUndoability(ICommand::kAutoUndo);	

		if(CmdUtils::ProcessCommand(insertTextCmd) != kSuccess)
		{
			error = PMString(kInsertCmdFailed);
			break;
		}

		// Command to change the page item into an inline
		InterfacePtr<ICommand> changeILGCmd(CmdUtils::CreateCommand(kChangeILGCmdBoss));
		if(changeILGCmd == nil)
		{
			ASSERT_FAIL("FLBItemTransform::MakeInlineCmd -> changeILGCmd nil");
			break;
		}
		
		// Set command undoability
		if(!allowUndo)
			changeILGCmd->SetUndoability(ICommand::kAutoUndo);	

		changeILGCmd->SetItemList(UIDList(txtModel));

		// Set the insertion location
		InterfacePtr<IRangeData> rangeData(changeILGCmd, UseDefaultIID());
		if(rangeData == nil)
		{
			ASSERT_FAIL("FLBItemTransform::MakeInlineCmd -> rangeData nil");
			break;
		}

		rangeData->Set(frameFinalTextIndex, frameFinalTextIndex);
		
		InterfacePtr<IUIDData> ilgUIDData(changeILGCmd, UseDefaultIID());
		if(ilgUIDData == nil)
		{
			ASSERT_FAIL("FLBItemTransform::MakeInlineCmd -> ilgUIDData nil");
			break;
		}

		ilgUIDData->Set(spline);
	
		status = CmdUtils::ProcessCommand(changeILGCmd);
		if(status != kSuccess)
			error = PMString(kMakeInlineCmdFailed);

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* SetStandoffCmd
*/
ErrorCode FLBItemTransform::SetStandoffCmd(UIDRef spline,IStandOff::mode mode, bool8 invert, PMReal left, PMReal top, PMReal right, PMReal bottom, 
										   IStandOffContourWrapSettings::ContourWrapType contourType, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(spline.GetDataBase() == nil || spline.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemTransform::SetStandoffCmd -> invalid spline");
			error = PMString(kFlbNilParameter);
			break;
		}

		UIDList frameList(spline.GetDataBase());
		UIDRef imageItemRef = UIDRef::gNull;
		if(Utils<IPageItemTypeUtils>()->IsGraphicFrame(spline)) // Set text wrap option for image item if any
		{
			InterfacePtr<IHierarchy> splineHier (spline, UseDefaultIID());

			InterfacePtr<IHierarchy> imageHier (splineHier->QueryChild(0));
			if(imageHier)
			{
				if(mode != IStandOff::kManualContour)
				{
					frameList.Append(::GetUID(imageHier));

					// Reset standoff mode to none for image item
					InterfacePtr<ICommand> modeCmd (CmdUtils::CreateCommand(kStandOffModeCmdBoss));
					modeCmd->SetItemList(frameList);

					InterfacePtr<IStandOffModeCmdData> modeCmdData (modeCmd,UseDefaultIID());
					modeCmdData->SetMode(IStandOff::kNone);

					if(!allowUndo)
						modeCmd->SetUndoability(ICommand::kAutoUndo);

					if(CmdUtils::ProcessCommand(modeCmd) != kSuccess)
					{
						error = PMString(kSetStandoffCmdFailed);
						break;
					}

					frameList.Clear();
					frameList.Append(spline.GetUID());
				}
				else
				{
					imageItemRef = ::GetUIDRef(imageHier);
					frameList.Append(::GetUID(imageHier));
				}
			}
			else
				frameList.Append(spline.GetUID());
		}
		else
			frameList.Append(spline.GetUID());

		// Set standoff mode
		InterfacePtr<ICommand> modeCmd (CmdUtils::CreateCommand(kStandOffModeCmdBoss));
		modeCmd->SetItemList(frameList);

		InterfacePtr<IStandOffModeCmdData> modeCmdData (modeCmd,UseDefaultIID());
		modeCmdData->SetMode(mode);

		// Set standoff form (invert or not)
		InterfacePtr<ICommand> formCmd (CmdUtils::CreateCommand(kStandOffFormCmdBoss));
		formCmd->SetItemList(frameList);

		InterfacePtr<IStandOffFormCmdData> formCmdData (formCmd,UseDefaultIID());
		if(invert)
			formCmdData->SetForm(IStandOff::kInvert);
		else
			formCmdData->SetForm(IStandOff::kRegular);

		// Set margins values (can be negative)
		InterfacePtr<ICommand> marginCmd (CmdUtils::CreateCommand(kStandOffMarginCmdBoss));
		marginCmd->SetItemList(frameList);

		InterfacePtr<IStandOffMarginCmdData> marginCmdData (marginCmd,UseDefaultIID());
		marginCmdData->SetMargins(top,left,bottom,right);

		// Set commands undoabilities
		if(!allowUndo)
		{
			modeCmd->SetUndoability(ICommand::kAutoUndo);
			formCmd->SetUndoability(ICommand::kAutoUndo);
			marginCmd->SetUndoability(ICommand::kAutoUndo);
		}

		if(CmdUtils::ProcessCommand(modeCmd) != kSuccess)
		{
			error = PMString(kSetStandoffCmdFailed);
			break;
		}

		if(CmdUtils::ProcessCommand(formCmd) != kSuccess)
		{
			error = PMString(kSetStandoffCmdFailed);
			break;
		}

		if(CmdUtils::ProcessCommand(marginCmd) != kSuccess)
		{
			error = PMString(kSetStandoffCmdFailed);
			break;
		}	

		if(imageItemRef != UIDRef::gNull)
		{
			InterfacePtr<IStandOffContourWrapSettings> contourWrapSettings (imageItemRef, UseDefaultIID());
			if(contourWrapSettings)
			{
				Utils<Facade::ITextWrapFacade>()->SetContourWrapSettings(UIDList(imageItemRef),
					contourType,
					contourWrapSettings->GetEmbeddedPathIndex(),
					contourWrapSettings->GetAlphaIndex(),
					contourWrapSettings->GetTolerance (),
					contourWrapSettings->GetThreshold(),
					contourWrapSettings->GetAllowHoles());
			}
		}

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}





/* BringToFrontCmd
*/
ErrorCode FLBItemTransform::BringToFrontCmd(UIDRef itemToMove,bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		// Create a BringToFrontCmd:
		InterfacePtr<ICommand> frontCmd(CmdUtils::CreateCommand(kBringToFrontCmdBoss));
		if (frontCmd == nil)
		{
			ASSERT_FAIL("FLBItemTransform::BringToFrontCmd -> frontCmd nil");
			break;
		}

		if(!allowUndo)
		{
			frontCmd->SetUndoability(ICommand::kAutoUndo);
		}

		// Set the BringToFrontCmd's ItemList:
		frontCmd->SetItemList(UIDList(itemToMove));

		// Process the BringToFrontCmd:
		status = CmdUtils::ProcessCommand(frontCmd);
		if (status != kSuccess)
			error = PMString(kBringToFrontCmdFailed);


	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* SendToBackCmd
*/
ErrorCode FLBItemTransform::SendToBackCmd(UIDRef itemToMove,bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		// Create a SendToBackCmd:
		InterfacePtr<ICommand>backCmd(CmdUtils::CreateCommand(kSendToBackCmdBoss));
		if (backCmd == nil)
		{
			ASSERT_FAIL("FLBItemTransform::SendToBackCmd -> backCmd nil");
			break;
		}

		if(!allowUndo)
		{
			backCmd->SetUndoability(ICommand::kAutoUndo);
		}

		// Set the SendToBackCmd's ItemList:
		backCmd->SetItemList(UIDList(itemToMove));

		// Process the SendToBackCmd:
		status = CmdUtils::ProcessCommand(backCmd);
		if (status != kSuccess)
			error = PMString(kSendToBackCmdFailed);


	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* Skew
*/
ErrorCode FLBItemTransform::Skew(UIDRef spline, PMReal skewAngle, PMPoint refPoint)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(spline.GetDataBase() == nil || spline.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemTransform::Skew -> invalid spline");
			error = PMString(kFlbNilParameter);
			break;
		}

		InterfacePtr<IGeometry> frameGeo (spline,UseDefaultIID());
		if(frameGeo == nil)
		{
			ASSERT_FAIL("FLBItemTransform::Skew -> frameGeo nil");
			error = PMString(kErrNotSpline);
			break;
		}

		// Convert the reference point in pasteboard coordinates
		::TransformInnerPointToPasteboard(frameGeo,&refPoint);
		
		status =  Utils<Facade::ITransformFacade>()->TransformItems( UIDList(spline), Transform::PasteboardCoordinates(), refPoint, Transform::SkewTo(skewAngle));

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* CenterContentCmd
*/
ErrorCode FLBItemTransform::CenterContentCmd(UIDRef content,bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(content.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemTransform::CentreContentCmd -> invalid content");
			error = PMString(kFlbNilParameter);
			break;
		}

		UIDList items = UIDList(content);
		status = Utils<Facade::IFrameContentFacade>()->CenterContentInFrame(items);

		if (status != kSuccess)
			error = PMString(kCenterContentCmdFailed);
		

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* FitContentPropCmd
*/
ErrorCode FLBItemTransform::FitContentPropCmd(UIDRef content,bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(content.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemTransform::FitContentPropCmd -> invalid content");
			error = PMString(kFlbNilParameter);
			break;
		}

		UIDList items = UIDList(content);
        // GD 01.07.2019 ++
		//status = Utils<Facade::IFrameContentFacade>()->FillFrameProp(items);
        status = Utils<Facade::IFrameContentFacade>()->FitContentProp(items);
		// GD 01.07.2019 --
        
		if (status != kSuccess)
			break;

		this->CenterContentCmd(content, allowUndo);

		if (status != kSuccess)
			error = PMString(kFitContentPropCmdFailed);
		

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* FitContentToFrameCmd
*/
ErrorCode FLBItemTransform::FitContentToFrameCmd(UIDRef content,bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(content.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemTransform::FitContentToFrameCmd -> invalid content");
			error = PMString(kFlbNilParameter);
			break;
		}
        
        // GD 05/03/2021 ++
		/*InterfacePtr<ICommand> fitContentToFrameCmd(CmdUtils::CreateCommand(kFitContentToFrameCmdBoss));
		if (fitContentToFrameCmd == nil)
		{
			ASSERT_FAIL("FLBItemTransform::FitContentToFrameCmd-> backCmd nil");
			break;
		}

		if(!allowUndo)
		{
			fitContentToFrameCmd->SetUndoability(ICommand::kAutoUndo);
		}

		fitContentToFrameCmd->SetItemList(UIDList(content));

		status = CmdUtils::ProcessCommand(fitContentToFrameCmd);
        */
        UIDList items = UIDList(content);
        status = Utils<Facade::IFrameContentFacade>()->FillFrameProp(items);
        if (status != kSuccess)
            break;
        this->CenterContentCmd(content, allowUndo);
        // GD 05/03/2021 --
        
		if (status != kSuccess)
			error = PMString(kFitContentToFrameCmdFailed);
		

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


ErrorCode FLBItemTransform::ResizeFrameCmd(UIDRef spline, PMReal horOffset, PMReal vertOffset, ReferencePoint refPt, bool8 resizeItemAndChildren, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);
	
	do
	{
		if(spline.GetDataBase() == nil || spline.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemTransform::ResizeFrameCmd -> invalid spline");
			error = PMString(kFlbNilParameter);
			break;
		}
		
		InterfacePtr<IGeometry> splineGeom (spline, UseDefaultIID());
		if (splineGeom == nil)
		{
			ASSERT_FAIL("FLBItemTransform::ResizeFrameCmd-> splineGeom nil");
			break;
		}
		
		PMRect bounds = splineGeom->GetStrokeBoundingBox();
		if(bounds.Width() + horOffset <= 0.0 || bounds.Height() + vertOffset <= 0.0)
		{
			error = PMString(kResizeCmdFailed);
			break;
		}

		PMRect pathBounds = splineGeom->GetPathBoundingBox();

		PMReal xScale = (pathBounds.Width() + horOffset) / pathBounds.Width();
		PMReal yScale = (pathBounds.Height() + vertOffset) / pathBounds.Height();

		PMPoint ref = pathBounds.LeftTop();
		switch(refPt)
		{
			case kLeftTop : ref = pathBounds.LeftTop(); break;
			case kRightTop : ref = pathBounds.RightTop(); break;
			case kLeftBottom : ref = pathBounds.LeftBottom(); break;
			case kRightBottom : ref = pathBounds.RightBottom(); break;
			case kCenter : ref = pathBounds.GetCenter(); break;
			default : ref = pathBounds.LeftTop(); break;
		}

		::TransformInnerPointToPasteboard(splineGeom, &ref);
		
		/*
		IGeometry::SetAction action = IGeometry::kResizeItemAndChildren;
		if(!resizeItemAndChildren)
			action = IGeometry::kResizeItemOnly;
		*/
		status =  Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(spline), Transform::PasteboardCoordinates(), Geometry::PathBounds(), ref, Geometry::MultiplyBy(xScale, yScale));

		if (status != kSuccess)
			error = PMString(kResizeCmdFailed);
		
	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

ErrorCode FLBItemTransform::JustifyFrameVertically(UIDRef frameRef, UIDRef * justifiedFrame, bool8 justifyTop, bool8 allowOutOfSpread, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error("");
	ICommandSequence * seq = nil;
	InterfacePtr<ITextFrameColumn> txtFrame (nil);
	Text::VerticalJustification vj = Text::kVJInvalid;
	UIDRef vjFrameRef = UIDRef::gNull;
	PMReal maxHeight = kMaxInt32; // Compute max height allowed to avoid infinite loop in case allowOutOfSpread is set to true
	PMReal originalHeight = kMaxInt32;

	IDataBase * db = frameRef.GetDataBase();	
	do
	{
		
		InterfacePtr<IHierarchy> frameHier (frameRef, UseDefaultIID());
		if(frameHier == nil)
		{
			error = PMString(kFlbNilInterface);
			break;
		}

		UID ownerPage = Utils<ILayoutUtils>()->GetOwnerPageUID(frameHier);
		
		// Is it a text frame ?
		int32 textflags;
		if(Utils<IFrameUtils>()->IsTextFrame(frameHier, &textflags))
		{
			// It's a text frame, so get the frame list
			InterfacePtr<IMultiColumnTextFrame> multiCol (db, frameHier->GetChildUID(0),UseDefaultIID());			
			InterfacePtr<IFrameList> frameList (multiCol->QueryFrameList());			
			InterfacePtr<ITextModel> txtModel (frameList->QueryTextModel());
			
			txtFrame = InterfacePtr<ITextFrameColumn> (frameList->QueryFrameContaining(txtModel->GetPrimaryStoryThreadSpan()-1,nil));
			if(txtFrame == nil)		
				txtFrame = InterfacePtr<ITextFrameColumn> (frameList->QueryNthFrame(frameList->GetFrameCount()-1));								

			// Get the last spline's geometry, in case text frame are linked, to convert refpoint
			InterfacePtr<IHierarchy> lastSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
			if(lastSpline == nil)
				break;

			frameRef = ::GetUIDRef(lastSpline);
			ownerPage = Utils<ILayoutUtils>()->GetOwnerPageUID(lastSpline);

			seq = CmdUtils::BeginCommandSequence();
			if(seq == nil)
			{		
				error = PMString(kFlbNilInterface);
				break;
			}

			if(!allowUndo)
				seq->SetUndoability(ICommand::kAutoUndo);		

			PMReal diff = 0.0; // Used later
			ReferencePoint refPt = kLeftTop;

			// Set vertical justification to top in order the algorithm to work well	
			InterfacePtr<IMultiColumnTextFrame> lastMulticol (db, lastSpline->GetChildUID(0), UseDefaultIID());
			vj = lastMulticol->GetVerticalJustification();
			vjFrameRef = ::GetUIDRef(lastMulticol);

			InterfacePtr<ICommand> vertJustCmd(CmdUtils::CreateCommand(kSetFrameVertJustifyCmdBoss)); 

			InterfacePtr<IHierarchy> txtFrameSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
			vertJustCmd->SetItemList(UIDList(vjFrameRef));

			InterfacePtr<IIntData> vertJustCmdIntData(vertJustCmd, UseDefaultIID()); 
			if(justifyTop)
			{
				vertJustCmdIntData->Set(Text::kVJBottom);
				refPt = kLeftBottom; // all resize commands will be relative to the frame's bottom
			}
			else
				vertJustCmdIntData->Set(Text::kVJTop);
			
			if(CmdUtils::ProcessCommand(vertJustCmd) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			// Recompose whole frame (required after modification of text frame settings)
			InterfacePtr<IFrameListComposer> frameComposer (db, txtFrameSpline->GetChildUID(0), UseDefaultIID());	
			if(frameComposer)
				frameComposer->RecomposeThruLastFrame();					

			InterfacePtr<IGeometry> frameGeo (frameRef, UseDefaultIID());
			if(frameGeo == nil)
			{
				error = PMString(kFlbNilInterface);
				break;
			}			

			originalHeight = frameGeo->GetStrokeBoundingBox().Height();

			if(Utils<ITextUtils>()->IsOverset(frameList))
			{
				// Resize the height till the beginning/end of the owner page or the owner frame (if it's an inline)
				if(Utils<IPageItemTypeUtils>()->IsInline(frameRef))
				{	
					// Get owner frame, then owner spline
					InterfacePtr<IHierarchy> frameHier (frameRef,UseDefaultIID());
					
					InterfacePtr<IOwnedItem> inlineItem (db, frameHier->GetParentUID(), UseDefaultIID()); // kInlineBoss
					if(inlineItem == nil)
					{
						error = PMString(kFlbNilInterface);
						break;
					}

					InterfacePtr<ITextFrameColumn> ownerFrame (inlineItem->QueryFrame());
					if(ownerFrame == nil)
					{
						// owner frame is overset, inline is not displayed, get last text frame by default
						InterfacePtr<ITextModel> ownerTextModel (inlineItem->QueryTextModel());
						InterfacePtr<IFrameList> ownerFrameList (ownerTextModel->QueryFrameList());
						
						ownerFrame = InterfacePtr<ITextFrameColumn> (ownerFrameList->QueryNthFrame(ownerFrameList->GetFrameCount() - 1));
					}

					InterfacePtr<IHierarchy> ownerSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(ownerFrame));
					if(ownerSpline == nil)
					{
						error = PMString(kFlbNilInterface);
						break;
					}

					InterfacePtr<IGeometry> ownerSplineGeo (ownerSpline,UseDefaultIID());

					PMReal ownerHeight = ownerSplineGeo->GetStrokeBoundingBox().Height(); // for an inline text frame, it doesn't matter whether we justify top or bottom
																						 // we resize frame to the same height as its container																				
					PMReal frameHeight = frameGeo->GetStrokeBoundingBox().Height();

					maxHeight = ownerHeight;
					if(frameHeight < ownerHeight)
						diff = ownerHeight - frameHeight;
					else
						diff = 0.0;
				}
				else
				{
					PMPoint limit = kZeroPoint, frameLimit = kZeroPoint;
					InterfacePtr<IGeometry> ownerGeo (nil);
					InterfacePtr<ISpread> isSpread (db, ownerPage, UseDefaultIID());
					if(isSpread)
					{
						ownerGeo = InterfacePtr<IGeometry> (isSpread, UseDefaultIID());		
						maxHeight = ownerGeo->GetStrokeBoundingBox().Height();
					}
					else
					{
						ownerGeo = InterfacePtr<IGeometry> (db, ownerPage, UseDefaultIID());	
						InterfacePtr<IMargins> ownerPageMargins(ownerGeo, UseDefaultIID());
						PMRect marginsRect;
						ownerPageMargins->GetMargins(&marginsRect, kFalse);
						maxHeight = ownerGeo->GetStrokeBoundingBox().Height() - marginsRect.Top() - marginsRect.Bottom();
					}

					if(justifyTop)
					{
						limit = ownerGeo->GetStrokeBoundingBox().LeftTop();
						::TransformInnerPointToPasteboard(ownerGeo, &limit);

						frameLimit = frameGeo->GetStrokeBoundingBox().LeftTop();
						::TransformInnerPointToPasteboard(frameGeo, &frameLimit);

						diff = frameLimit.Y() - limit.Y();
					}
					else
					{
						limit = ownerGeo->GetStrokeBoundingBox().LeftBottom();
						::TransformInnerPointToPasteboard(ownerGeo, &limit);

						frameLimit = frameGeo->GetStrokeBoundingBox().LeftBottom();
						::TransformInnerPointToPasteboard(frameGeo, &frameLimit);
			
						diff = limit.Y() - frameLimit.Y();
					}
				}				

				if(this->ResizeFrameCmd(frameRef,0.0, diff, refPt) != kSuccess)
				{
					error = PMString(kJVCmdFailed);
					break;
				}

				InterfacePtr<IFrameListComposer> frameComposer (frameList, UseDefaultIID());
				frameComposer->RecomposeThruLastFrame();
				
				if(Utils<ITextUtils>()->IsOverset(frameList)) // text is too big to fit in the frame within the page
				{
					if(allowOutOfSpread)
					{
						// Try to resize frame to the max height allowed
						diff = maxHeight - frameGeo->GetStrokeBoundingBox().Height();
						if(this->ResizeFrameCmd(frameRef,0.0, diff, refPt) != kSuccess)
						{
							error = PMString(kJVCmdFailed);
							break;
						}

						frameComposer->RecomposeThruLastFrame();
					
						if(Utils<ITextUtils>()->IsOverset(frameList)) // Frame cannot be resized, not an error, reset to original height and quit
						{
							diff = originalHeight - frameGeo->GetStrokeBoundingBox().Height();
							if(this->ResizeFrameCmd(frameRef,0.0, diff, refPt) != kSuccess)
								error = PMString(kJVCmdFailed);
							else
								status = kSuccess;

							break;
						}
					}
					else
					{
						error = PMString(kErrJVImpossible);
						break;
					}
				}
			}
			else
			{
				if(txtModel->GetPrimaryStoryThreadSpan()- 1 == 0)
				{
					status = kSuccess; // story is empty, no need to justify
					break;
				}
			}

			InterfacePtr<IHierarchy> multiColHier (multiCol, UseDefaultIID());
			if(multiColHier->GetChildCount() > 1)
			{
				// This frame contains several columns, it can't be justified with the position of the last line (fastest solution)
				// Instead, we use a dichotomic algorithm
				PMPoint leftTop = frameGeo->GetStrokeBoundingBox().LeftTop();
				PMPoint bottomLeft = frameGeo->GetStrokeBoundingBox().LeftBottom();
				
				::TransformInnerPointToPasteboard(frameGeo,&leftTop);
				::TransformInnerPointToPasteboard(frameGeo,&bottomLeft);

				diff = bottomLeft.Y() - leftTop.Y();

				while(diff >= 10)
				{
					diff = diff/2;
					if(!Utils<ITextUtils>()->IsOverset(frameList))
						diff = -diff;

					if(this->ResizeFrameCmd(frameRef,0.0, diff, refPt) != kSuccess)
					{
						error = PMString(kJVCmdFailed);
						break;
					}

					frameComposer->RecomposeThruLastFrame();

					diff = ::abs(diff);
				}

				while(Utils<ITextUtils>()->IsOverset(frameList))
				{
					if(this->ResizeFrameCmd(frameRef,0.0, 2, refPt) != kSuccess)
					{
						error = PMString(kJVCmdFailed);
						break;
					}

					frameComposer->RecomposeThruLastFrame();
				}	
			}			
			
			// Stroke weight must be taken into account
			InterfacePtr<IGraphicStyleDescriptor> frameDescr (frameGeo, UseDefaultIID());
			PMReal strokeWeight = 0.0;
			Utils<IGraphicAttributeUtils>()->GetStrokeWeight(strokeWeight, frameDescr);

			InterfacePtr<IWaxStrand> waxStrand (frameList, UseDefaultIID());
			K2::scoped_ptr<IWaxIterator> waxIterator (waxStrand->NewWaxIterator());
			
			if(justifyTop)
			{							
				IWaxLine * firstLine = waxIterator->GetFirstWaxLine(0);

				InterfacePtr<IWaxLineShape> waxLineShape( firstLine, UseDefaultIID());
				PMLineSeg lineSeg;
				waxLineShape->GetSelectionLine(&lineSeg);

				PMPoint lineTopPoint = lineSeg.TopPoint();
				firstLine->GetToPasteboardMatrix().Transform(&lineTopPoint);

				waxIterator->ClearCurrentLine();

				PMPoint leftTop = frameGeo->GetStrokeBoundingBox().LeftTop();
				::TransformInnerPointToPasteboard(frameGeo, &leftTop);		

				diff = leftTop.Y() - lineTopPoint.Y() + strokeWeight;
			}
			else
			{
				PMPoint deepestLastLine (0, -kMaxInt32);
				// Get multicolumn item child (the columns) and adjust frame according to the deepest last line of column's last lines
				for(int32 i = 0 ; i < multiColHier->GetChildCount() ; ++i)
				{
					InterfacePtr<ITextFrameColumn> columnFrame (db, multiColHier->GetChildUID(i), UseDefaultIID());
					IWaxLine * lastLine = waxIterator->GetFirstWaxLine(columnFrame->TextStart() + columnFrame->TextSpan()-1);
					InterfacePtr<IWaxLineShape> waxLineShape( lastLine, UseDefaultIID());

					PMLineSeg lineSeg;
					waxLineShape->GetSelectionLine(&lineSeg);

					PMPoint lineBottomPoint = lineSeg.BottomPoint();
					lastLine->GetToPasteboardMatrix().Transform(&lineBottomPoint);

					if(lineBottomPoint.Y() > deepestLastLine.Y())
						deepestLastLine = lineBottomPoint;

					waxIterator->ClearCurrentLine();
				}						

				PMPoint leftBottom = frameGeo->GetStrokeBoundingBox().LeftBottom();
				::TransformInnerPointToPasteboard(frameGeo, &leftBottom);		

				diff = deepestLastLine.Y() - leftBottom.Y() + strokeWeight;
			}
			
			// Resize frame to the right height
			waxIterator.reset();
										
			status = this->ResizeFrameCmd(frameRef,0.0, diff, refPt);
		}
		else
		{
			error = PMString(kFlbErrTextBoxRequired);
			break;
		}
		
	}while(false);	
	
	if (seq != nil)
	{
		if (status == kSuccess)
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
			if(justifiedFrame) *justifiedFrame = frameRef;
		}
		else			
			ErrorUtils::PMSetGlobalErrorCode(status);	

		CmdUtils::EndCommandSequence(seq);
	}
	else
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

ErrorCode FLBItemTransform::JustifyImageFrame(UIDRef frameRef, bool8 justifyHorizontaly, bool8 justifyVerticaly, PMReal maxHor, PMReal maxVer, bool8 leftJustify, bool8 topJustify, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error("");
	ICommandSequence * seq = nil;

	do
	{
		InterfacePtr<IHierarchy> itemHier (frameRef, UseDefaultIID());

		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			error = PMString(kFlbNilInterface);
			break;
		}

		if(!allowUndo)
			seq->SetUndoability(ICommand::kAutoUndo);	

		if(!Utils<IPageItemTypeUtils>()->IsTextFrame(itemHier))
		{
			if(itemHier->GetChildCount() == 1)
			{
				InterfacePtr<IGeometry> frameGeo (frameRef, UseDefaultIID());
				PMRect frameBbox = frameGeo->GetStrokeBoundingBox();
				::TransformInnerRectToPasteboard(frameGeo, &frameBbox);

				InterfacePtr<IGeometry> imageGeo (frameRef.GetDataBase(), itemHier->GetChildUID(0), UseDefaultIID());
				PMRect imageBbox = imageGeo->GetStrokeBoundingBox();
				::TransformInnerRectToPasteboard(imageGeo, &imageBbox);

				// Stroke weight has to be taken into account
				InterfacePtr<IGraphicStyleDescriptor> frameDescr (frameRef, UseDefaultIID());
				PMReal strokeWeight = 0.0;
				Utils<IGraphicAttributeUtils>()->GetStrokeWeight(strokeWeight, frameDescr);				

				PMReal diff = 0.0;
				if(justifyHorizontaly)
				{
					if(leftJustify)
					{
						diff = imageBbox.Left() - frameBbox.Left();
						if(this->ResizeFrameCmd(frameRef, -diff + strokeWeight, 0.0, kRightTop, kFalse) != kSuccess)
						{
							error = PMString(kResizeCmdFailed);
							break;
						}
					}
					else
					{
						diff = imageBbox.Right() - frameBbox.Right();
						if(this->ResizeFrameCmd(frameRef, diff + strokeWeight, 0.0, kLeftTop, kFalse) != kSuccess)
						{
							error = PMString(kResizeCmdFailed);
							break;
						}
					}
				}

				if(justifyVerticaly)
				{
					if(topJustify)
					{
						diff = imageBbox.Top() - frameBbox.Top();
						if(this->ResizeFrameCmd(frameRef, 0.0, -diff + strokeWeight, kLeftBottom, kFalse) != kSuccess)
						{
							error = PMString(kResizeCmdFailed);
							break;
						}
					}
					else
					{
						diff = imageBbox.Bottom() - frameBbox.Bottom();
						if(this->ResizeFrameCmd(frameRef, 0.0, diff + strokeWeight, kLeftTop, kFalse) != kSuccess)
						{							
							error = PMString(kResizeCmdFailed);
							break;
						}							
					}
				}				

				status = kSuccess;
			}
			else // Image frame is empty, do nothing
				status = kSuccess;
		}
		else
		{
			error = PMString(kFlbErrImageBoxRequired);
			break;
		}
				
	} while(kFalse);

	if (status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	CmdUtils::EndCommandSequence(seq);
	
	return status;
}
