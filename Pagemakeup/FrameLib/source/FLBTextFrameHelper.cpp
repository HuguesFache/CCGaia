
//	File:	FLBTextFrameHelper.cpp
//
//  Author: Trias
//
//	Date:	19-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//



// Plug-in includes
#include "VCPlugInHeaders.h"

// General includes
#include "CmdUtils.h"
#include "UIDList.h"
#include "PMRect.h"
#include "ErrorUtils.h"
#include "K2SmartPtr.h"
#include "ITextModelCmds.h"
#include "IPageItemUtils.h"

// Interface includes
#include "ICommand.h"
#include "IMultiColumnTextFrame.h"
#include "ITextModel.h"
#include "IFrameList.h"
#include "IFrameListComposer.h"
#include "ITextInset.h"
#include "ITextColumnData.h"
#include "ITextColumnSizer.h"
#include "IHierarchy.h"
#include "ISetBoundingBoxCmdData.h"
#include "IGeometry.h"
#include "IBoundsData.h"
#include "IAttributeStrand.h"
#include "ITextAttrUtils.h"
#include "ITextState.h"

// Project includes
#include "FLBTextFrameHelper.h"

#include "DebugClassUtils.h"

#include "CAlert.h"

CREATE_PMINTERFACE(FLBTextFrameHelper,kFLBTextFrameHelperImpl)

/* Constructor
*/
FLBTextFrameHelper::FLBTextFrameHelper(IPMUnknown * boss)
: CPMUnknown<ITextFrameHelper>(boss)
{
	attrList = new AttributeBossList() ;
	basedOn = nextStyle = 0;
	paraStyleBool = kFalse;
	inCreation = kFalse;
}

/* Destructor
*/
FLBTextFrameHelper::~FLBTextFrameHelper()
{
	
	if (attrList != nil)
		delete attrList ;
}

/* RecomposeWholeStory
*/
ErrorCode FLBTextFrameHelper::RecomposeWholeStory(IMultiColumnTextFrame * textFrame)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(textFrame == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::RecomposeWholeStory -> textFrame nil");
			error = PMString(kFlbNilParameter);
			break;
		}

		// Get the frame composer
		InterfacePtr<IFrameList> frList (textFrame->QueryFrameList());
		if(frList == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::RecomposeWholeStory -> frList nil");
			break;
		}

		InterfacePtr<IFrameListComposer> frComposer (frList,UseDefaultIID());
		if(frComposer == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::RecomposeWholeStory -> frComposer nil");
			break;
		}

		// Recompose all the frames of the story
		// CS5 : Porting  frComposer->RecomposeThruNthFrame(frList->GetFrameCount() - 1);
		frComposer->RecomposeThruLastFrame();
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* LinkFramesCmd
*/
ErrorCode FLBTextFrameHelper::LinkFramesCmd(UIDRef firstMultiCol, UIDRef secondMultiCol, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(firstMultiCol.GetDataBase() == nil || firstMultiCol.GetDataBase() != secondMultiCol.GetDataBase()
		   || firstMultiCol.GetUID() == kInvalidUID || secondMultiCol.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBTextFrameHelper::LinkFramesCmd -> invalid(s) multicolumn(s) item(s)");
			error = PMString(kFlbNilParameter);
			break;
		}

		InterfacePtr<IMultiColumnTextFrame> testFirstIsTextFrame (firstMultiCol,UseDefaultIID());
		InterfacePtr<IMultiColumnTextFrame> testSecondIsTextFrame (secondMultiCol,UseDefaultIID());
		if(testFirstIsTextFrame == nil || testSecondIsTextFrame == nil) // not a text frame, cannot insert an inline
		{
			ASSERT_FAIL("FLBTextFrameHelper::LinkFramesCmd -> frameAnc nil");
			error = PMString(kErrNotTextFrame);
			break;
		}

		InterfacePtr<ICommand> textLinkCmd (CmdUtils::CreateCommand(kTextLinkCmdBoss));
		if(textLinkCmd == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::LinkFramesCmd -> textLinkCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			textLinkCmd->SetUndoability(ICommand::kAutoUndo);

		// Fill a list with the frames to link
		UIDList framesToLink(firstMultiCol);
		framesToLink.Append(secondMultiCol.GetUID());

		textLinkCmd->SetItemList(framesToLink);

		status = CmdUtils::ProcessCommand(textLinkCmd);
		if(status != kSuccess)
			error = PMString(kTextLinkCmdFailed);
			
	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* UnLinkFramesCmd
*/
ErrorCode FLBTextFrameHelper::UnLinkFramesCmd(UIDRef multiCol, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(multiCol.GetDataBase() == nil || multiCol.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBTextFrameHelper::UnLinkFramesCmd -> invalid multicolumn item");
			error = PMString(kFlbNilParameter);
			break;
		}

		InterfacePtr<IMultiColumnTextFrame> testIsTextFrame (multiCol,UseDefaultIID());
		if(testIsTextFrame == nil) // not a text frame, cannot insert an inline
		{
			ASSERT_FAIL("FLBTextFrameHelper::UnLinkFramesCmd -> frameAnc nil");
			error = PMString(kErrNotTextFrame);
			break;
		}

		InterfacePtr<ICommand> textUnlinkCmd(CmdUtils::CreateCommand(kTextUnlinkCmdBoss));	
		if(textUnlinkCmd == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::UnLinkFramesCmd -> textUnlinkCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			textUnlinkCmd->SetUndoability(ICommand::kAutoUndo);

		UIDList frameList(multiCol);

		textUnlinkCmd->SetItemList(frameList);
		status = CmdUtils::ProcessCommand(textUnlinkCmd);
		if(status != kSuccess)
			error = PMString(kTextUnlinkCmdFailed);

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}




/* ApplyParaStyleCmd
*/
ErrorCode FLBTextFrameHelper::ApplyParaStyleCmd(ITextModel * model, TextIndex insertPos, UID paraStyleUID, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);
	ICommandSequence * seq ;

	do
	{
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("FLBTextFrameHelper::ApplyParaStyleCmd -> seq nil");
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);


		//apply the paragraph style
		if (paraStyleUID!=kInvalidUID)
		{
			InterfacePtr<ITextModelCmds> modelCmds(model, UseDefaultIID()); 		
			
			/** BUG FIXING  : we must insert a character to be able to delete character overrides which are not deleted
							  in some cases when paragraph style is applied */
			InterfacePtr<IAttributeStrand> charStrand (static_cast<IAttributeStrand* >(model->QueryStrand(kCharAttrStrandBoss, IID_IATTRIBUTESTRAND)));
			DataWrapper<AttributeBossList> localCharOverrides = charStrand->GetLocalOverrides(insertPos, nil);
			if(localCharOverrides.get()->CountBosses() > 0)
			{
				boost::shared_ptr<WideString> dummyChar (new WideString());
				dummyChar->push_back(kTextChar_Space);
				InterfacePtr<ICommand> insertCharCmd (modelCmds->InsertCmd(insertPos, dummyChar));
				CmdUtils::ProcessCommand(insertCharCmd);

				boost::shared_ptr<AttributeBossList> overridesToDelete (new AttributeBossList(*localCharOverrides));
				InterfacePtr<ICommand> clearOverridesCmd (modelCmds->ClearOverridesCmd (insertPos, 1, overridesToDelete, kCharAttrStrandBoss));
				status = CmdUtils::ProcessCommand(clearOverridesCmd);
				if (status != kSuccess)
				{
					error = PMString(kApplyCmdFailed);
					break;
				}							

				InterfacePtr<ICommand> deleteCmd (modelCmds->DeleteCmd(insertPos, 1));
				CmdUtils::ProcessCommand(deleteCmd);
			}					
					
			// Create an ApplyTextStyleCmd						
			InterfacePtr<ICommand> applyCmd(modelCmds->ApplyStyleCmd(insertPos,0,paraStyleUID,kParaAttrStrandBoss, kTrue, kFalse, kFalse));
			if (applyCmd == nil)
			{
				ASSERT_FAIL("FLBTextFrameHelper::ApplyParaStyleCmd -> applyCmd nil");
				break;
			}

			// Process the ApplyTextStyleCmd: 
			status = CmdUtils::ProcessCommand(applyCmd);
			if (status != kSuccess)
				error = PMString(kApplyCmdFailed);
		
		}

		status=kSuccess;
			
	} while(false);
	
	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(status);

		CmdUtils::EndCommandSequence(seq);	
	}
	else
		ErrorUtils::PMSetGlobalErrorCode(status);
		

	return status;
}



/* ApplyCharStyleCmd
*/
ErrorCode FLBTextFrameHelper::ApplyCharStyleCmd(ITextModel * model, TextIndex insertPos, UID charStyleUID, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);
	ICommandSequence * seq ;

	do
	{
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("FLBTextFrameHelper::ApplyCharStyleCmd -> seq nil");
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);


		//apply the character style

		if (charStyleUID!=kInvalidUID)
		{
			// Create an ApplyTextStyleCmd through the ITextModel:
			InterfacePtr<ITextModelCmds> modelCmds(model, UseDefaultIID()); 
			if(modelCmds == nil) break;
			InterfacePtr<ICommand> applyCmd(modelCmds->ApplyStyleCmd(insertPos,0,charStyleUID,kCharAttrStrandBoss));
			if (applyCmd == nil)
			{
				ASSERT_FAIL("FLBTextFrameHelper::ApplyCharStyleCmd -> applyCmd nil");
				break;
			}

			// Process the ApplyTextStyleCmd: 
			status = CmdUtils::ProcessCommand(applyCmd);
			if (status != kSuccess)
				error = PMString(kApplyCmdFailed);
		}

		status=kSuccess;
			
	} while(false);
	
	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(status);

		CmdUtils::EndCommandSequence(seq);
	}
	else
		ErrorUtils::PMSetGlobalErrorCode(status);
		

	return status;
}




/* ApplyAttributeParaCmd
*/
ErrorCode FLBTextFrameHelper::ApplyAttributeParaCmd(ITextModel * model, TextIndex insertPos, AttributeBossList * attrList2, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);
	ICommandSequence * seq ;

	do
	{
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("FLBTextFrameHelper::ApplyAttributeParaCmd -> seq nil");
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);


		if (attrList2 != nil )
		{
			boost::shared_ptr<AttributeBossList> attrListPtr (new AttributeBossList(*attrList2)) ;

			// Create an ApplyTextStyleCmd
			InterfacePtr<ITextModelCmds> modelCmds(model, UseDefaultIID()); 
			if(modelCmds == nil) break;
			InterfacePtr<ICommand> applyCmd(modelCmds->ApplyCmd(insertPos,0,attrListPtr,kParaAttrStrandBoss));
			if (applyCmd == nil)
			{
				ASSERT_FAIL("FLBTextFrameHelper::ApplyAttributeParaCmd -> applyCmd nil");
				break;
			}

			// Process the ApplyTextStyleCmd: 
			if (CmdUtils::ProcessCommand(applyCmd) != kSuccess)
			{
				error = PMString(kApplyCmdFailed);
				break;
			}

			//we don't delete the attributeBossList Pointer because
			//the list was a parameter of an specific indesign function
			//(here applyCmd)
			//indeed indesign wants to delete himself structures which were
			//parameters of certains functions!! it's incredible but true (explication found in the indesign's forum)
			//if we delete normally the pointer, indesign crash...

		}

		status=kSuccess;
			
	} while(false);
	
	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(status);

		CmdUtils::EndCommandSequence(seq);
	}
	else
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}





/* ApplyAttributeCharCmd
*/
ErrorCode FLBTextFrameHelper::ApplyAttributeCharCmd(ITextModel * model, TextIndex insertPos, AttributeBossList * attrList2, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);
	ICommandSequence * seq ;

	do
	{
		
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("FLBTextFrameHelper::ApplyAttributeCharCmd -> seq nil");
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);


		if (attrList2 != nil )
		{
			boost::shared_ptr<AttributeBossList> attrListPtr (new AttributeBossList(*attrList2)) ;

			// Create an ApplyTextStyleCmd
			InterfacePtr<ITextModelCmds> modelCmds(model, UseDefaultIID()); 
			if(modelCmds == nil) break;
			InterfacePtr<ICommand> applyCmd(modelCmds->ApplyCmd(insertPos,0,attrListPtr,kCharAttrStrandBoss));
			if (applyCmd == nil)
			{
				ASSERT_FAIL("FLBTextFrameHelper::ApplyAttributeCharCmd -> applyCmd nil");
				break;
			}

			// Process the ApplyTextStyleCmd: 
			if (CmdUtils::ProcessCommand(applyCmd) != kSuccess)
			{
				error = PMString(kApplyCmdFailed);
				break;
			}

			//we don't delete the attributeBossList Pointer because
			//the list was a parameter of an specific indesign function
			//(here applyCmd)
			//indeed indesign wants to delete himself structures which were
			//parameters of certains functions!! it's incredible but true (explication found in the indesign's forum)
			//if we delete normally the pointer, indesign crash...

		}

		status=kSuccess;
			
	} while(false);
	
		if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(status);

		CmdUtils::EndCommandSequence(seq);
	}

	return status;
}





/* InsertTextCmd
*/
ErrorCode FLBTextFrameHelper::InsertTextAtEndCmd(UIDRef multiCol, WideString text, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);
    ICommandSequence * seq  = nil;
    

	do
	{
		// Test if it is a text frame and get the text model
		InterfacePtr<IMultiColumnTextFrame> txtFrame (multiCol,UseDefaultIID());
		if(txtFrame == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::InsertTextAtEndCmd -> txtFrame nil");
			error = PMString(kErrNotTextFrame);
			break;
		}

		InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());
		if(txtModel == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::InsertTextAtEndCmd -> txtModel nil");
			break;
		}

		// Insertion will happen at the end of the story
		TextIndex position =  txtModel->GetPrimaryStoryThreadSpan() - 1;	

		// Create the insert command
		InterfacePtr<ITextModelCmds> modelCmds (txtModel, UseDefaultIID()); 
		boost::shared_ptr<WideString> data(new WideString(text)); 
		InterfacePtr<ICommand> insertCmd (modelCmds->InsertCmd(position, data));
		if(insertCmd == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::InsertTextAtEndCmd -> insertCmd nil");
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("FLBTextFrameHelper::InsertTextAtEndCmd -> seq nil");
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);
		
		status = CmdUtils::ProcessCommand(insertCmd);
		if(status != kSuccess)
		{
			error = PMString(kInsertCmdFailed);
			break ;
		}


		//apply the paragraph style

		/*if (paraStyleUID!=kInvalidUID)
		{
			// Create an ApplyTextStyleCmd through the ITextModel:
			InterfacePtr<ICommand> applyCmd(txtModel->ApplyStyleCmd(position,data->Length(),paraStyleUID,kParaAttrStrandBoss));
			if (applyCmd == nil)
			{
				ASSERT_FAIL("FLBTextFrameHelper::InsertTextAtEndCmd -> applyCmd nil");
				break;
			}

			// Process the ApplyTextStyleCmd: 
			if (status = CmdUtils::ProcessCommand(applyCmd) != kSuccess)
				error = PMString(kApplyCmdFailed);
		}

		//apply the character style

		if (charStyleUID!=kInvalidUID)
		{
			// Create an ApplyTextStyleCmd through the ITextModel:
			InterfacePtr<ICommand> applyCmd(txtModel->ApplyStyleCmd(position,data->Length(),charStyleUID,kCharAttrStrandBoss));
			if (applyCmd == nil)
			{
				ASSERT_FAIL("FLBTextFrameHelper::InsertTextAtEndCmd -> applyCmd nil");
				break;
			}

			// Process the ApplyTextStyleCmd: 
			if (status = CmdUtils::ProcessCommand(applyCmd) != kSuccess)
				error = PMString(kApplyCmdFailed);
		}


		//apply the additionnal attributes
		if (attrList != nil && GetAttrList().CountBosses()!=0)
		{
			AttributeBossList * attrListPtr ;
			attrListPtr = new AttributeBossList(GetAttrList()) ;

			if(attrListPtr == nil)
			{		
				ASSERT_FAIL("FLBTextFrameHelper::InsertTextAtEndCmd -> attrListPtr nil");
				error = PMString(kApplyCmdFailed);
				break;
			}


			// Create an ApplyTextStyleCmd through the ITextModel:
			InterfacePtr<ICommand> applyCmd(txtModel->ApplyCmd(position,data->Length(),attrListPtr,kCharAttrStrandBoss));
			if (applyCmd == nil)
			{
				ASSERT_FAIL("FLBTextFrameHelper::InsertTextAtEndCmd -> applyCmd nil");
				break;
			}

			// Process the ApplyTextStyleCmd: 
			if (CmdUtils::ProcessCommand(applyCmd) != kSuccess)
			{
				error = PMString(kApplyCmdFailed);
				break;
			}

			//we don't delete the attributeBossList Pointer because
			//the list was a parameter of an specific indesign function
			//(here applyCmd)
			//indeed indesign wants to delete himself structures which were
			//parameters of certains functions!! it's incredible but true (explication found in the indesign's forum)
			//if we delete normally the pointer, indesign crash...

		}


		if (attrList2 != nil && GetAttrList2().CountBosses()!=0)
		{
			AttributeBossList * attrListPtr2 ;
			attrListPtr2 = new AttributeBossList(GetAttrList2()) ;

			if(attrListPtr2 == nil)
			{		
				ASSERT_FAIL("FLBTextFrameHelper::InsertTextAtEndCmd -> attrListPtr2 nil");
				error = PMString(kApplyCmdFailed);
				break;
			}

			// Create an ApplyTextStyleCmd through the ITextModel:
			//apply additionnal paragraph's attributes
			InterfacePtr<ICommand> applyCmd(txtModel->ApplyCmd(position,data->Length(),attrListPtr2,kParaAttrStrandBoss));
			if (applyCmd == nil)
			{
				ASSERT_FAIL("FLBTextFrameHelper::InsertTextAtEndCmd -> applyCmd nil");
				break;
			}

			// Process the ApplyTextStyleCmd: 
			if (CmdUtils::ProcessCommand(applyCmd) != kSuccess)
			{
				error = PMString(kApplyCmdFailed);
				break;
			}
			
			//we don't delete the attributeBossList Pointer because
			//the list was a parameter of an specific indesign function
			//(here applyCmd)
			//indeed indesign wants to delete himself structures which were
			//parameters of certains functions!! it's incredible but true (explication found in the indesign's forum)
			//if we delete normally the pointer, indesign crash...

		}*/

		status=kSuccess;
			
	} while(false);
	
	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(status);

		CmdUtils::EndCommandSequence(seq);
	}		

	return status;
}

/* SetInset
*/
ErrorCode FLBTextFrameHelper::SetInset(UIDRef spline, PMReal left, PMReal top, PMReal right, PMReal bottom)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(spline.GetDataBase() == nil || spline.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBTextFrameHelper::SetInset -> invalid spline");
			error = PMString(kFlbNilParameter);
			break;
		}

		InterfacePtr<ITextInset> inset(spline, UseDefaultIID());
		if(inset == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::SetInset -> inset nil");
			error = PMString(kErrNotTextFrame);
			break;
		}

		PMRect insets;
		insets.Left(left);
		insets.Top(top);
		insets.Right(right);
		insets.Bottom(bottom);
		inset->SetRectInset(insets);
		inset->UpdateInsetGeometry();

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* SetInsetCmd
*/
ErrorCode FLBTextFrameHelper::SetInsetCmd(UIDRef spline, PMReal left, PMReal top, PMReal right, PMReal bottom, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(spline.GetDataBase() == nil || spline.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBTextFrameHelper::SetInsetCmd -> invalid spline");
			error = PMString(kFlbNilParameter);
			break;
		}

		InterfacePtr<ICommand> textInsetCmd (CmdUtils::CreateCommand(kSetTextInsetCmdBoss));
		if(textInsetCmd == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::SetInsetCmd -> textInsetCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			textInsetCmd->SetUndoability(ICommand::kAutoUndo);

		textInsetCmd->SetItemList(UIDList(spline));

		InterfacePtr<ITextInset> inset(textInsetCmd,UseDefaultIID());
		if(inset == nil)
		{
			ASSERT_FAIL("FLBTextFrameHelper::SetInsetCmd -> inset nil");
			break;
		}
		
		PMRect insets;
		insets.Left(left);
		insets.Top(top);
		insets.Right(right);
		insets.Bottom(bottom);
		inset->SetRectInset(insets);

		inset->SetInset(top);

		if(CmdUtils::ProcessCommand(textInsetCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		
		//InterfacePtr<IHierarchy> splineHier (spline, UseDefaultIID());
		//if(splineHier->GetChildCount() > 0)
		//{

		//	InterfacePtr<ITextColumnSizer> txtColumnSizer (spline.GetDataBase(), splineHier->GetChildUID(0), UseDefaultIID());

		//	InterfacePtr<ICommand> recalculateColumnsCmd  (CmdUtils::CreateCommand(kRecalculateColumnsCmdBoss ));
		//	recalculateColumnsCmd->SetItemList(UIDList(spline));

		//	if(CmdUtils::ProcessCommand(recalculateColumnsCmd) != kSuccess)
		//	{
		//		error = ErrorUtils::PMGetGlobalErrorString();
		//		break;
		//	}

		//	InterfacePtr<ICommand> changeColumnsCmd  (CmdUtils::CreateCommand(kChangeNumberOfColumnsCmdBoss ));
		//	changeColumnsCmd->SetItemList(UIDList(spline));

		//	InterfacePtr<ITextColumnData> columnData (changeColumnsCmd, UseDefaultIID());
		//	columnData->SetNumberOfColumns(-1);
		//	columnData->SetGutterWidth(-1);
		//	columnData->UseFixedColumnSizing (-1);
		//	columnData->SetFixedWidth(-1);

		//	if(CmdUtils::ProcessCommand(changeColumnsCmd) != kSuccess)
		//	{
		//		error = ErrorUtils::PMGetGlobalErrorString();
		//		break;
		//	}

		//	
		//	InterfacePtr<ICommand> setBBoxCmd  (CmdUtils::CreateCommand(kSetBoundingBoxCmdBoss));
		//	setBBoxCmd->SetItemList(UIDList(spline));

		//	InterfacePtr<ISetBoundingBoxCmdData> bboxData (setBBoxCmd, UseDefaultIID());

		//	InterfacePtr<IGeometry> splineGeo (spline, UseDefaultIID());

		//	PMRectCollection rects;
		//	rects.Append(splineGeo->GetPathBoundingBox());
		//	bboxData->SetBoundingBoxData(rects, IGeometry::kInnerCoordinateSpace, IGeometry::kPathBoundsKind ,IGeometry::kResizeItemOnly); 

		//	InterfacePtr<IBoundsData>  boundsData (bboxData, UseDefaultIID());
		//	boundsData->SetBounds(PageItemUtils::GetPasteboardBounds(splineHier));

		//	if(CmdUtils::ProcessCommand(setBBoxCmd) != kSuccess)
		//	{
		//		error = ErrorUtils::PMGetGlobalErrorString();
		//		break;
		//	}
		//}

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
	
}


// -------------
//  Accessors
// -------------

AttributeBossList * FLBTextFrameHelper::GetAttrListPtr()
{
	return attrList ;
}

AttributeBossList FLBTextFrameHelper::GetAttrList()
{
	return * attrList ;
}

void FLBTextFrameHelper::SetAttrList(AttributeBossList AttrBossList)
{
	if (attrList!=nil)
		delete attrList ;
	attrList = new AttributeBossList(AttrBossList) ;
}

int32 FLBTextFrameHelper::GetNextStyleNum()
{
	return nextStyle;
}

void FLBTextFrameHelper::SetNextStyleNum(int32 numStyle)
{
	nextStyle = numStyle;
}

int32 FLBTextFrameHelper::GetBasedOnNum()
{
	return basedOn;
}

void FLBTextFrameHelper::SetBasedOnNum(int32 numStyle)
{
	basedOn = numStyle;
}

PMString FLBTextFrameHelper::GetStyleName()
{
	return styleName;
}

void FLBTextFrameHelper::SetStyleName(PMString StyleName)
{
	styleName=StyleName;
}

bool8 FLBTextFrameHelper::IsParaStyle()
{
	return paraStyleBool;
}

void FLBTextFrameHelper::SetIsParaStyle(bool8 isParaStyle)
{
	paraStyleBool=isParaStyle;
}


void FLBTextFrameHelper::SetCreation(bool8 creation)
{
	inCreation=creation;
}

bool8 FLBTextFrameHelper::GetCreation()
{
	return inCreation;
}


void FLBTextFrameHelper::Clean()
{

	attrList->ClearAllOverrides();

	SetIsParaStyle(kFalse);

	SetCreation(kFalse);

	basedOn = nextStyle = 0;
}

