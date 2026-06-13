/*
//	File:	PMUUtils.cpp
//
//	Date:	18-Apr-2003
//
//	Author : Trias
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IOpenFileDialog.h"
#include "IControlView.h"
#include "IPanelControlData.h"
#include "IApplication.h"
#include "IDialogMgr.h"
#include "IDialog.h"

#include "ISelectionUtils.h"
#include "ISelectionManager.h"
#include "ITextValue.h"
#include "IDocument.h"
#include "ILayoutUIUtils.h"
#include "IIntData.h"
#include "IPageItemAdornmentList.h"
#include "IClassIDData.h"
#include "ILayoutSelectionSuite.h"
#include "ITextSelectionSuite.h"
#include "ITextModel.h"
#include "IFrameType.h"
#include "IPathGeometry.h"
#include "IPathUtils.h"
#include "IHierarchy.h"
#include "IFrameList.h"
#include "ISpreadList.h"
#include "ITextFrameColumn.h"
#include "IMultiColumnTextFrame.h"
#include "ITextUtils.h"
#include "IPageItemTypeUtils.h"
#include "ISpread.h"
#include "IGraphicFrameData.h"
#include "IDataLinkReference.h"
#include "IDataLink.h"
#include "IReplaceCmdData.h"
//#include "IImportFileCmdData.h"
#include "IWindowUtils.h"
#include "IGeometry.h"
#include "IWindow.h"
#include "ITextControlData.h"
#include "ITextDataValidation.h"
#include "IItemManager.h"
#include "IStringData.h"
#include "IPMPointData.h"
#include "IItemTransform.h"

// General includes:
#include "CreateObject.h"
#include "SysFileList.h"
#include "NAMEINFO.H"
#include "TransformUtils.h"

// General includes
#include "CAlert.h"
#include "StreamUtil.h"
#include "SDKFileHelper.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"
#include "GlobalDefs.h"
#include "ErrorUtils.h"
#include "FileUtils.h"

// Project includes
#include "PMUID.h"
#include "PMUUtils.h"

// Parser includes
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "IReverseTCL.h"
#include "PrsID.h"
#include "ITCLReferencesList.h"
#include "IParserSuite.h"
#include "IPermRefsSuite.h"
#include "PermRefsUtils.h"

/* OpenFileDialog
*/
bool PMUUtils::OpenFileDialog(IDFile& file)
{
	/*
	SDKFileOpenChooser fileChooser;
	
	// Limit the choice to TCL files
	PMString family("TCL files (*.tcl)");
	PMString extension("tcl");
	family.SetTranslatable(kFalse);
	extension.SetTranslatable(kFalse);
    
    fileChooser.AddFilter( kAllTypes , extension, family);
    fileChooser.ShowDialog();
    if (fileChooser.IsChosen()) 
    {
    	file = fileChooser.GetIDFile();
    	return true;
    }
    else
    	return false;
    */
    bool ok = false;
	do
	{
		SysFileList filesToOpen;
		InterfacePtr<IOpenFileDialog> openFileDialog
		(
			static_cast<IOpenFileDialog*>
			(
				::CreateObject
				(
					kOpenFileDialogBoss,
					IOpenFileDialog::kDefaultIID
				)
			)
		);

		if (openFileDialog == nil)
			break;

		// Limit the choice to TCL files
		PMString family("TCL files (*.tcl)");
		PMString extension("tcl");
		family.SetTranslatable(kFalse);
		extension.SetTranslatable(kFalse);
 

		openFileDialog->AddExtension(&family,&extension) ;

		if (openFileDialog->DoDialog
			(
				nil,		// No default file.
				filesToOpen, 
				kFalse,		// don't allow multiple files to be selected
				nil			// use the default dialog title
			) == kFalse)
			break;

		// Get back the selected file
		file = *filesToOpen.GetNthFile(0);

		ok = true;

	} while (false);
	
	return ok;
}

void PMUUtils::ExecTCL()
{
	IDFile TCLfile;
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do
	{
		if(PMUUtils::OpenFileDialog(TCLfile) == kTrue)
		{							
			
			InterfacePtr<ITCLParser> parser ((ITCLParser *) ::CreateObject(kTCLParserBoss,IID_ITCLPARSER));
			if (parser == nil)
				break;

			
			// Launch the parser
			if(!parser->SetInputFile(TCLfile))
			{
				error = PMString(kErrCannotReadFile);
				break;
			}

			parser->LaunchParser();
		
			status = kSuccess;
		} 
		else
			status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);
}

void PMUUtils::ReverseTCL()
{
	IDFile TCLfile;
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do
	{
		SDKFileSaveChooser saveDialog;
		PMString defaultFileName("Untitled");
		defaultFileName.Translate();
		defaultFileName.Append(".tcl");
		defaultFileName.SetTranslated();
		
		PMString filter("tcl");
		filter.SetTranslatable(kFalse);
		PMString filterName("TCL Files (*.tcl)");
		filterName.SetTranslatable(kFalse);
		
		saveDialog.SetFilename(defaultFileName);
	#ifdef WINDOWS
		saveDialog.AddFilter(0,0,filter, filterName);
	#else
		saveDialog.AddFilter(kAnyCreator,kTextType,filter, filterName);
	#endif
		saveDialog.ShowDialog();
		if(saveDialog.IsChosen())
		{
			InterfacePtr<IReverseTCL> reverse ((IReverseTCL *) ::CreateObject(kReverseTCLBoss,IID_IREVERSETCL));
			if (reverse == nil)
				break;

			if(!reverse->SetOutputFile(saveDialog.GetIDFile()))
			{
				error = PMString(kErrCannotWriteFile);
				break;
			}
			
			if(reverse->StartReverse() != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				ErrorUtils::PMSetGlobalErrorCode(kSuccess);
				break;
			}

			status = kSuccess;
			CAlert::InformationAlert(PMString(kEndReverseTCL));
		}
		else
			status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);					
}

void PMUUtils::ShowTCLPrefs()
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do
	{
		IDialog * dialog = PMUUtils::CreateNewDialog(kSDKDefDialogResourceID);
		if(dialog == nil)
			break;

		// Open the dialog.
		dialog->Open(); 

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);	
}


IDialog * PMUUtils::CreateNewDialog(RsrcID rsID)
{
	IDialog * dialog = nil;
	do
	{
		// Get the application interface and the DialogMgr.	
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		if (application == nil)
			break;

		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
		if (dialogMgr == nil)
			break;

		// Load the plug-in's resource.
		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcSpec dialogSpec
		(
			nLocale,					// Locale index from PMLocaleIDs.h. 
			kPMUPluginID,				// Plug-in ID
			kViewRsrcType,				// This is the kViewRsrcType.
			rsID,						// Resource ID for our dialog.
			kTrue						// Initially visible.
		);

		// CreateNewDialog takes the dialogSpec created above, and also
		// the type of dialog being created (kMovableModal).
		
		dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);

	} while(kFalse);	

	return dialog;
}

void PMUUtils::AddPermRef()
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		// Test if a document is open
		if(doc != nil)
		{
			// Ckeck for text selection					
			InterfacePtr<ISelectionManager> selectionMgr (Utils<ISelectionUtils>()->QueryActiveSelection ());					
									
			InterfacePtr<IPermRefsSuite> permrefsSuite (selectionMgr, UseDefaultIID());
			if(permrefsSuite == nil) // permrefsSuite == nil means there is no active text selection
			{
				error = PMString(kErrTextSelection);
				break;
			}

			TextIndex start, end;
			UIDRef txtModel;
			permrefsSuite->GetTextSelection(txtModel,start, end);
			if(start == end)
			{
				error = PMString(kErrTextSelection);
				break;
			}

			InterfacePtr<ITextModel> model (txtModel, UseDefaultIID());

			IDialog * dialog = PMUUtils::CreateNewDialog(kPMUTypePermRefDialogResourceID);

			if (dialog == nil)
				break;
			
			InterfacePtr<IPanelControlData> dialogView  (dialog, UseDefaultIID());
			
			IControlView * input = dialogView->FindWidget(kPMURefEditWidgetID);
			
			K2Vector<PermRefStruct> listRef;
			PermRefsUtils::GetPermRefs(model, start, end, listRef);
			
			// Update selection with new start, end
			selectionMgr->DeselectAll(nil);

			InterfacePtr<ITextSelectionSuite> textSelectionSuite (selectionMgr, UseDefaultIID());
			if(textSelectionSuite == nil)
				break;
			
			RangeData rangeToSelect(start,end);
			textSelectionSuite->SetTextSelection(::GetUIDRef(model),rangeToSelect, Selection::kScrollIntoView, &rangeToSelect);
								
			PMString selectedRefs = kNullString;
			PMString selectedParam1 = kNullString;
			PMString selectedParam2 = kNullString;
			PMString selectedParam3 = kNullString;
			PMString selectedParam4 = kNullString;

			if(listRef.size() > 0)
			{	
				selectedRefs.AppendNumber(listRef[0].ref);
				selectedParam1.AppendNumber(listRef[0].param1);
				selectedParam2.AppendNumber(listRef[0].param2);
				selectedParam3.AppendNumber(listRef[0].param3);
				selectedParam4.AppendNumber(listRef[0].param4);

				for(int32 i = 1 ; i < listRef.size() ; ++i)
				{
					selectedRefs += ";";
					selectedRefs.AppendNumber(listRef[i].ref);					
					selectedParam1 += ";";
					selectedParam1.AppendNumber(listRef[i].param1);
					selectedParam2 += ";";
					selectedParam2.AppendNumber(listRef[i].param2);
					selectedParam3 += ";";
					selectedParam3.AppendNumber(listRef[i].param3);
					selectedParam4 += ";";
					selectedParam4.AppendNumber(listRef[i].param4);

				}
			}
			
			selectedRefs.SetTranslatable(kFalse);			
			selectedParam1.SetTranslatable(kFalse);
			selectedParam2.SetTranslatable(kFalse);
			selectedParam3.SetTranslatable(kFalse);
			selectedParam4.SetTranslatable(kFalse);
			
			InterfacePtr<ITextControlData> txtData (input, UseDefaultIID());
			txtData->SetString(selectedRefs);
			txtData = InterfacePtr<ITextControlData>(dialogView->FindWidget(kPMUParam1EditWidgetID), UseDefaultIID());
			txtData->SetString(selectedParam1);
			txtData = InterfacePtr<ITextControlData>(dialogView->FindWidget(kPMUParam2EditWidgetID), UseDefaultIID());
			txtData->SetString(selectedParam2);
			txtData = InterfacePtr<ITextControlData>(dialogView->FindWidget(kPMUParam3EditWidgetID), UseDefaultIID());
			txtData->SetString(selectedParam3);
			txtData = InterfacePtr<ITextControlData>(dialogView->FindWidget(kPMUParam4EditWidgetID), UseDefaultIID());
			txtData->SetString(selectedParam4);
#if COMPOSER
			// Reset title
			InterfacePtr<IWindow> dialogWindow (dialog, UseDefaultIID());
			dialogWindow->SetTitle(PMString(kPMUTypeRefDialogTitleKey));
#endif 

			dialog->Open();
			dialog->WaitForDialog();

			InterfacePtr<ITextValue> inputValue (input,UseDefaultIID());
			bool16 blankEntry;
			int32 ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
			if(!blankEntry)
			{
				// Clean up the dialog
				InterfacePtr<ITextControlData> txtData (inputValue, UseDefaultIID());
				txtData->SetString(kNullString);

				PermRefStruct thePermRef;
				thePermRef.ref = ref;

				inputValue = InterfacePtr<ITextValue>(dialogView->FindWidget(kPMUParam1EditWidgetID), UseDefaultIID());
				ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
				if(!blankEntry) thePermRef.param1 = ref;
				txtData = InterfacePtr<ITextControlData>(inputValue, UseDefaultIID());
				txtData->SetString(kNullString);
				
				inputValue = InterfacePtr<ITextValue>(dialogView->FindWidget(kPMUParam2EditWidgetID), UseDefaultIID());
				ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
				if(!blankEntry) thePermRef.param2 = ref;
				txtData = InterfacePtr<ITextControlData>(inputValue, UseDefaultIID());
				txtData->SetString(kNullString);
				
				inputValue = InterfacePtr<ITextValue>(dialogView->FindWidget(kPMUParam3EditWidgetID), UseDefaultIID());
				ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
				if(!blankEntry) thePermRef.param3 = ref;
				txtData = InterfacePtr<ITextControlData>(inputValue, UseDefaultIID());
				txtData->SetString(kNullString);
				
				inputValue = InterfacePtr<ITextValue>(dialogView->FindWidget(kPMUParam4EditWidgetID), UseDefaultIID());
				ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
				if(!blankEntry) thePermRef.param4 = ref;
				txtData = InterfacePtr<ITextControlData>(inputValue, UseDefaultIID());
				txtData->SetString(kNullString);

				if(PermRefsUtils::ApplyPermRefs(thePermRef, model, start, end) != kSuccess)
				{
					error = PMString(ErrorUtils::PMGetGlobalErrorString());
					break;
				}

				InterfacePtr<ITCLReferencesList> docRefList (doc, UseDefaultIID());
				if(docRefList == nil)
					break;

				PermRefVector storyList = docRefList->GetStoryList();

				// Add ref to the document's list of references
				int32 pos = ::FindLocation(storyList, thePermRef);
				// Check whether ref already exists
				if(pos != -1)
				{
					K2Vector<UID>& storiesUID = storyList[pos].Value();
					
					K2Vector<UID>::iterator iter = ::K2find(storiesUID.begin(), storiesUID.end(),txtModel.GetUID());
					// Check whether story already exists, if not add to the end of the list
					if(iter == storiesUID.end())
						storiesUID.push_back(txtModel.GetUID());					
				}
				else
				{
					// New ref, add it to the list
					K2Vector<UID> storiesUID;
					storiesUID.push_back(txtModel.GetUID());
					::InsertKeyValue(storyList,thePermRef,storiesUID);
				}

				UIDList docToSave(doc);
				InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
				
				saveItemsCmd->SetUndoability(ICommand::kAutoUndo);
				saveItemsCmd->SetItemList(docToSave);

				InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());
				cmdData->SetStoryList(storyList);
				cmdData->SetItemList(docRefList->GetItemList());
				cmdData->SetTableList(docRefList->GetTableList());

				CmdUtils::ProcessCommand(saveItemsCmd);

				status = kSuccess;
			}
			else
				status = kSuccess;
		}
		else
			status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);
}

void PMUUtils::AddTCLRef()
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		// Test if a document is open
		if(doc != nil)
		{
			// Ckeck for layout selection					
			InterfacePtr<ISelectionManager> selectionMgr (Utils<ISelectionUtils>()->QueryActiveSelection ());					
			
			UIDList itemSelectedList;
			InterfacePtr<IParserSuite> prsSuite (selectionMgr, UseDefaultIID());
			if(prsSuite == nil) // prsSuite == nil means there is no active layout selection
			{
#if COMPOSER
				SearchTCLRef(); // Special behavior for composer
				status = kSuccess;
#else
				error = PMString(kErrLayoutSelection);
#endif
				break;
			}

			prsSuite->GetSelectedItems(itemSelectedList);

			if(itemSelectedList.Length() > 1)
			{
				error = PMString(kPMUTooManySelectedItemsKey);
				break;
			}

			UID selectedItem = itemSelectedList.First();

			InterfacePtr<IFrameType> frameType (::GetDataBase(doc), selectedItem, UseDefaultIID());
			if(frameType == nil)
			{
				error = PMString(kErrTCLItemRequired);
				break;
			}

			// Add ref to the item, then add the adornment, if necessary
			InterfacePtr<ITCLRefData> tclData (::GetDataBase(doc), selectedItem, IID_ITCLREFDATA);
			TCLRef oldRef = tclData->Get();	

			// Test if there is an item list stored in the document
			InterfacePtr<ITCLReferencesList> docRefList(doc,UseDefaultIID());
			
			if(docRefList != nil)
			{		
				RefVector itemList = docRefList->GetItemList();

				/*if(itemList.size() - 1 > MAX_BLOC) // Should never occur	
				{
					error = PMString(PMString(kErrTooManySavedItems));
					break;
				}*/
			
				IDialog * dialog = PMUUtils::CreateNewDialog(kPMUTypeRefDialogResourceID);
				if (dialog == nil)
					break;
				
#if COMPOSER
				// Reset title (it may have been changed on SearchTCLRef call 
				InterfacePtr<IWindow> dialogWindow (dialog, UseDefaultIID());
				dialogWindow->SetTitle(PMString(kPMUTypeRefDialogTitleKey));
#endif

				InterfacePtr<IPanelControlData> dialogView  (dialog, UseDefaultIID());
				
				IControlView * input = dialogView->FindWidget(kPMURefEditWidgetID);
				// Display item's current reference
#if REF_AS_STRING
				
				InterfacePtr<ITextControlData> inputValue (input, UseDefaultIID());
				if(oldRef != kDefaultTCLRefData)
					inputValue->SetString(oldRef);
#else
				InterfacePtr<ITextValue> inputValue (input,UseDefaultIID());				
				if(oldRef != kDefaultTCLRefData)
					inputValue->SetTextFromValue(oldRef);				
#endif

#if COMPOSER && !REF_AS_STRING

				InterfacePtr<ITextDataValidation> dataValidation (input, UseDefaultIID());
				dataValidation->SetLowerLimit(2000.0);
				dataValidation->SetUpperLimit(32000.0);
#endif

				dialog->Open();
				dialog->WaitForDialog();
				
				bool16 blankEntry = kFalse;

#if REF_AS_STRING
				InterfacePtr<ITextValue> dataValidation (input, UseDefaultIID());
				dataValidation->GetTextAsValue(nil, &blankEntry);
				TCLRef ref = inputValue->GetString();
				ref.SetTranslatable(kFalse);
#else
				TCLRef ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
#endif
				if(!blankEntry && ref != oldRef)
				{
					// Clean up the dialog
					InterfacePtr<ITextControlData> txtData (inputValue, UseDefaultIID());
					txtData->SetString(kNullString);				

					UIDList splinesToUpdate(tclData);

					// Check if the new ref is attributed somewhere else
					int32 location = ::FindLocation(itemList, ref);
					if(location >= 0)
					{
						error = PMString(kErrAlreadyUsed);
						break;
					}
					
					if(oldRef != kDefaultTCLRefData)
					{
						// Remove old ref from the list
						int32 location = ::FindLocation(itemList, oldRef);
						if(location >= 0)
							itemList.erase(itemList.begin() + location);	
					}

					// Set the new ref
					IDataBase * db = ::GetDataBase(doc);

					InterfacePtr<ICommand> setTCLRefCmd (CmdUtils::CreateCommand(kPrsSetTCLRefDataCmdBoss));
					setTCLRefCmd->SetUndoability(ICommand::kAutoUndo);
					setTCLRefCmd->SetItemList(UIDList(tclData));
					
					InterfacePtr<ITCLRefData> setTCLRefCmdData (setTCLRefCmd, IID_ITCLREFDATA);
					setTCLRefCmdData->Set(ref);
					CmdUtils::ProcessCommand(setTCLRefCmd);
					
					// Add ref to the document's list of references
					::InsertOrReplaceKeyValue(itemList,ref,selectedItem);

					UIDList docToSave(doc);
					InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
					
					saveItemsCmd->SetUndoability(ICommand::kAutoUndo);
					saveItemsCmd->SetItemList(docToSave);

					InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());
					cmdData->SetItemList(itemList);
					cmdData->SetStoryList(docRefList->GetStoryList());
					cmdData->SetTableList(docRefList->GetTableList());

					CmdUtils::ProcessCommand(saveItemsCmd);
					
					InterfacePtr<IPageItemAdornmentList> adornmentList (tclData, UseDefaultIID());
					if(!adornmentList->HasAdornment(kPrsTCLAdornmentBoss))
					{
						InterfacePtr<ICommand>  addPageItemAdornmentCmd(CmdUtils::CreateCommand(kAddPageItemAdornmentCmdBoss));
						InterfacePtr<IClassIDData>  classIDData(addPageItemAdornmentCmd, UseDefaultIID());

						classIDData->Set(kPrsTCLAdornmentBoss);
						addPageItemAdornmentCmd->SetItemList(UIDList(tclData));
						
						if(CmdUtils::ProcessCommand(addPageItemAdornmentCmd) != kSuccess)
							error = PMString(PMString(kErrTCLAdornment));
					}

					Utils<ILayoutUIUtils>()->InvalidateViews(doc);

					status = kSuccess;
				}
				else
				{
#if COMPOSER && !REF_AS_STRING
					InterfacePtr<ITextDataValidation> dataValidation (input, UseDefaultIID());
					dataValidation->SetLowerLimit(MIN_BLOC);
					dataValidation->SetUpperLimit(MAX_BLOC);
#endif
					status = kSuccess;
				}
			}			
		}
		else
			status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);
}

void PMUUtils::SearchTCLRef()
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		// Test if a document is open
		if(doc != nil)
		{
			InterfacePtr<ITCLReferencesList> docRefList(doc,UseDefaultIID());
			
			if(docRefList != nil)
			{
				IDialog * dialog = PMUUtils::CreateNewDialog(kPMUTypeRefDialogResourceID);
				if (dialog == nil)
					break;
				
				InterfacePtr<IPanelControlData> dialogView  (dialog, UseDefaultIID());
				
				IControlView * input = dialogView->FindWidget(kPMURefEditWidgetID);
				
#if COMPOSER
				// Change title
				InterfacePtr<IWindow> dialogWindow (dialog, UseDefaultIID());
				dialogWindow->SetTitle(PMString(kPMUSearchRefDialogTitleKey));
#if !REF_AS_STRING

				InterfacePtr<ITextDataValidation> dataValidation (input, UseDefaultIID());
				dataValidation->SetLowerLimit(2000.0);
				dataValidation->SetUpperLimit(32000.0);
#endif
#endif

				dialog->Open();
				dialog->WaitForDialog();
				
				bool16 blankEntry = kFalse;
#if REF_AS_STRING
				InterfacePtr<ITextValue> dataValidation (input, UseDefaultIID());
				dataValidation->GetTextAsValue(nil, &blankEntry);

				InterfacePtr<ITextControlData> inputValue (input, UseDefaultIID());
				TCLRef ref = inputValue->GetString();
#else
				InterfacePtr<ITextValue> inputValue (input,UseDefaultIID());
				TCLRef ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
#endif				
				if(!blankEntry)
				{
					// Clean up the dialog
					InterfacePtr<ITextControlData> txtData (input, UseDefaultIID());
					txtData->SetString(kNullString);

					int32 location = ::FindLocation(docRefList->GetItemList(), ref);
					if(location >= 0)
					{
						UID itemToSelect = docRefList->GetItemList()[location].Value();

						// Deselect any previous selection
						InterfacePtr<ISelectionManager> selectionMgr (Utils<ISelectionUtils>()->QueryActiveSelection ());			
						selectionMgr->DeselectAll(nil);						

						InterfacePtr<ILayoutSelectionSuite> layoutSelectionSuite (Utils<ISelectionUtils>()->QueryActiveLayoutSelectionSuite());
						if(layoutSelectionSuite == nil)
							break;

						layoutSelectionSuite->SelectPageItems(UIDList(::GetDataBase(doc),itemToSelect), Selection::kReplace, Selection::kAlwaysCenterInView);

						status = kSuccess;

					}
					else
						error = PMString(PMString(kErrInvalidItem));
				}
				else
				{
#if COMPOSER && !REF_AS_STRING
					InterfacePtr<ITextDataValidation> dataValidation (input, UseDefaultIID());
					dataValidation->SetLowerLimit(MIN_BLOC);
					dataValidation->SetUpperLimit(MAX_BLOC);
#endif
					status = kSuccess;
				}
					
			}
		}
		else
			status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);
}

void PMUUtils::SearchPermRef()
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		// Test if a document is open
		if(doc != nil)
		{
			InterfacePtr<ITCLReferencesList> docRefList(doc,UseDefaultIID());
			
			if(docRefList != nil)
			{
				IDialog * dialog = PMUUtils::CreateNewDialog(kPMUTypePermRefDialogResourceID);

				if (dialog == nil)
					break;
				
				InterfacePtr<IPanelControlData> dialogView  (dialog, UseDefaultIID());
				
				IControlView * input = dialogView->FindWidget(kPMURefEditWidgetID);
				
				// Reset fields
				InterfacePtr<ITextControlData> txtData (input, UseDefaultIID());
				txtData->SetString(kNullString);
				txtData = InterfacePtr<ITextControlData>(dialogView->FindWidget(kPMUParam1EditWidgetID), UseDefaultIID());
				txtData->SetString(kNullString);
				txtData = InterfacePtr<ITextControlData>(dialogView->FindWidget(kPMUParam2EditWidgetID), UseDefaultIID());
				txtData->SetString(kNullString);
				txtData = InterfacePtr<ITextControlData>(dialogView->FindWidget(kPMUParam3EditWidgetID), UseDefaultIID());
				txtData->SetString(kNullString);
				txtData = InterfacePtr<ITextControlData>(dialogView->FindWidget(kPMUParam4EditWidgetID), UseDefaultIID());
				txtData->SetString(kNullString);
#if COMPOSER
				// Change title
				InterfacePtr<IWindow> dialogWindow (dialog, UseDefaultIID());
				dialogWindow->SetTitle(PMString(kPMUSearchRefDialogTitleKey));
#endif 

				dialog->Open();
				dialog->WaitForDialog();

				InterfacePtr<ITextValue> inputValue (input,UseDefaultIID());
				bool16 blankEntry;
				int32 ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
				if(!blankEntry)
				{
					// Clean up the dialog
					InterfacePtr<ITextControlData> txtData (inputValue, UseDefaultIID());
					txtData->SetString(kNullString);

					PermRefStruct thePermRef;
					thePermRef.ref = ref;

					inputValue = InterfacePtr<ITextValue>(dialogView->FindWidget(kPMUParam1EditWidgetID), UseDefaultIID());
					ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
					if(!blankEntry) thePermRef.param1 = ref;
					txtData = InterfacePtr<ITextControlData>(inputValue, UseDefaultIID());
					txtData->SetString(kNullString);
					
					inputValue = InterfacePtr<ITextValue>(dialogView->FindWidget(kPMUParam2EditWidgetID), UseDefaultIID());
					ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
					if(!blankEntry) thePermRef.param2 = ref;
					txtData = InterfacePtr<ITextControlData>(inputValue, UseDefaultIID());
					txtData->SetString(kNullString);
					
					inputValue = InterfacePtr<ITextValue>(dialogView->FindWidget(kPMUParam3EditWidgetID), UseDefaultIID());
					ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
					if(!blankEntry) thePermRef.param3 = ref;
					txtData = InterfacePtr<ITextControlData>(inputValue, UseDefaultIID());
					txtData->SetString(kNullString);
					
					inputValue = InterfacePtr<ITextValue>(dialogView->FindWidget(kPMUParam4EditWidgetID), UseDefaultIID());
					ref = ToInt32(inputValue->GetTextAsValue(nil, &blankEntry));
					if(!blankEntry) thePermRef.param4 = ref;
					txtData = InterfacePtr<ITextControlData>(inputValue, UseDefaultIID());
					txtData->SetString(kNullString);

					int32 location = ::FindLocation(docRefList->GetStoryList(), thePermRef);
					if(location >= 0)
					{
						K2Vector<UID> txtModelUIDs = docRefList->GetStoryList()[location].Value();
						
						TextIndex start, end;
						UID story;
						
						if(PermRefsUtils::LookForPermRefs(thePermRef, ::GetDataBase(doc), txtModelUIDs, story, start, end) != kSuccess)
						{
							error = PMString(ErrorUtils::PMGetGlobalErrorString());	
							ErrorUtils::PMSetGlobalErrorCode(kSuccess);
							break;
						}

						// Deselect any previous selection
						InterfacePtr<ISelectionManager> selectionMgr (Utils<ISelectionUtils>()->QueryActiveSelection ());			
						selectionMgr->DeselectAll(nil);

						InterfacePtr<ITextSelectionSuite> textSelectionSuite (Utils<ISelectionUtils>()->QueryActiveTextSelectionSuite());
						if(textSelectionSuite == nil)
							break;
						
						RangeData rangeToSelect(start,end);
						textSelectionSuite->SetTextSelection(UIDRef(::GetDataBase(doc),story),rangeToSelect, Selection::kScrollIntoView, &rangeToSelect);

						status = kSuccess;
					}
					else
						error = PMString(kErrPermRefsInvalidRef);
				}
				else
					status = kSuccess;
			}
		}
		else
			status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);
}

void PMUUtils::SearchOversetBox()
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc != nil)
		{
			IDataBase * db = ::GetDataBase(doc);
			
			// Ckeck for layout selection					
			InterfacePtr<ISelectionManager> selectionMgr (Utils<ISelectionUtils>()->QueryActiveSelection ());					
			
			int32 startSpreadIndex = 0;
			UID firstSpreadUID = kInvalidUID;

			UIDList itemSelectedList;
			InterfacePtr<IParserSuite> prsSuite (selectionMgr, UseDefaultIID());
			if(prsSuite != nil) // prsSuite == nil means there is no active layout selection
			{
				prsSuite->GetSelectedItems(itemSelectedList);
				InterfacePtr<IHierarchy> itemHier (db, itemSelectedList.First(), UseDefaultIID());

				firstSpreadUID = itemHier->GetSpreadUID();
			}

			InterfacePtr<ISpreadList> spreadList (doc, UseDefaultIID());
			if(spreadList == nil)
				break;
			
			if(firstSpreadUID != kInvalidUID)
				startSpreadIndex = spreadList->GetSpreadIndex(firstSpreadUID);

			int32 nbSpreads = spreadList->GetSpreadCount();
			for(int32 i = startSpreadIndex ; i < nbSpreads && status != kSuccess ; ++i)
			{
				InterfacePtr<ISpread> spread (db, spreadList->GetNthSpreadUID(i), UseDefaultIID());
				if(spread == nil)
					break;
				
				int32 countPages = spread->GetNumPages();

				// Iterator through spread layers (1st z-order level)
				for(int32 j = 0 ; j < countPages && status != kSuccess; ++j)
				{
					UIDList itemsOnPage(db);
					spread->GetItemsOnPage(j,&itemsOnPage, kFalse, kTrue);

					int32 nbItems = itemsOnPage.Length();
					
					// Iterate through childs (the page items) of the spread layer (2nd z-order level)
					for(int32 itemIndex = 0 ; itemIndex < nbItems && status != kSuccess; ++itemIndex)
					{
						InterfacePtr<IHierarchy> itemHier (db, itemsOnPage[itemIndex], UseDefaultIID());
						if(itemHier == nil)
							break;

						if(Utils<IPageItemTypeUtils>()->IsTextFrame(itemHier))
						{
							InterfacePtr<IMultiColumnTextFrame> txtFrame (db, itemHier->GetChildUID(0), UseDefaultIID());

							InterfacePtr<IFrameList> frameList (txtFrame->QueryFrameList());

							// Overset frame list found, select last box of this list
							if(Utils<ITextUtils>()->IsOverset(frameList))
							{
								InterfacePtr<ITextFrameColumn> lastTextFrame (frameList->QueryNthFrame(frameList->GetFrameCount()-1));
								InterfacePtr<IHierarchy> oversetSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(lastTextFrame));
							
								selectionMgr->DeselectAll(nil);						

								InterfacePtr<ILayoutSelectionSuite> layoutSelectionSuite (Utils<ISelectionUtils>()->QueryActiveLayoutSelectionSuite());
								if(layoutSelectionSuite == nil)
									break;

								layoutSelectionSuite->SelectPageItems(UIDList(oversetSpline), Selection::kReplace, Selection::kAlwaysCenterInView);

								status = kSuccess;
							}
						}
					}
				}
			}

			if(status != kSuccess) // No overset box 			
				error = PMString(kErrNoOversetBox);
		}
		else
			status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
	{
		error.Translate();	
		CAlert::InformationAlert(error);
	}
}

void PMUUtils::UpdatePicture()
{
	ErrorCode status = kSuccess;
	bool16 allDone = kTrue;
	PMString error(kErrNilInterface);
	UID itemToSelect = kInvalidUID;
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc != nil)
		{
			IDataBase * db = ::GetDataBase(doc);
			
			InterfacePtr<ISpreadList> spreadList (doc, UseDefaultIID());
			if(spreadList == nil)
			{
				status = kFailure;
				break;
			}

			int32 nbSpreads = spreadList->GetSpreadCount();
			for(int32 i = 0 ; i < nbSpreads ; ++i)
			{
				InterfacePtr<ISpread> spread (db, spreadList->GetNthSpreadUID(i), UseDefaultIID());
				if(spread == nil)
				{
					status = kFailure;
					break;
				}
				
				int32 countPages = spread->GetNumPages();

				// Iterator through spread layers (1st z-order level)
				for(int32 j = 0 ; j < countPages; ++j)
				{
					UIDList itemsOnPage(db);
					spread->GetItemsOnPage(j,&itemsOnPage, kFalse, kTrue);

					// Iterate through childs (the page items) of the spread layer (2nd z-order level)
					for(int32 itemIndex = 0 ; itemIndex < itemsOnPage.Length() ; ++itemIndex)
					{
						InterfacePtr<IHierarchy> itemHier (db, itemsOnPage[itemIndex], UseDefaultIID());
						if(itemHier == nil)
							break;

						InterfacePtr<IStringData> missingImagePath(itemHier, IID_IIMAGEPATH);
						if(missingImagePath && !missingImagePath->GetString().IsEmpty())
						{
							allDone = kFalse;

							UIDRef imageFrameRef;
							InterfacePtr<IItemManager> itemManager (::CreateObject2<IItemManager>(kFrameHelperBoss));
							InterfacePtr<IItemTransform> itemTransform (itemManager, UseDefaultIID());
							if((status = itemManager->ImportImageInGraphicFrameCmd(::GetUIDRef(itemHier),missingImagePath->GetString(),imageFrameRef)) == kSuccess)
							{
						
								// Scale and shift image item
								InterfacePtr<IGeometry> imGeo (imageFrameRef,UseDefaultIID());							
								InterfacePtr<IPMPointData> scaleData(itemHier, IID_IIMAGESCALE);
								if((status = itemTransform->Scale(imageFrameRef,scaleData->GetPMPoint().X(),scaleData->GetPMPoint().Y(),PMPoint(imGeo->GetStrokeBoundingBox().LeftTop()))) != kSuccess)
								{
									error = ErrorUtils::PMGetGlobalErrorString();
									break;
								}
								
								// Calculate the new position of the left top corner, then move the image
								InterfacePtr<IGeometry> itemGeo(itemHier, UseDefaultIID());
								InterfacePtr<IPMPointData> shiftData(itemHier, IID_IIMAGESHIFT);
								PMPoint leftTop (itemGeo->GetStrokeBoundingBox().LeftTop());
								PMPoint newLeftTop (leftTop.X() + shiftData->GetPMPoint().X(), leftTop.Y() + shiftData->GetPMPoint().Y());
								::TransformInnerPointToPasteboard(itemGeo,&newLeftTop);

								if((status = itemTransform->MoveCmd(imageFrameRef,newLeftTop)) != kSuccess)
								{
									error = ErrorUtils::PMGetGlobalErrorString();
									break;
								}
								
								// Delete missing file's info
								InterfacePtr<ICommand> setMissingFileInfoCmd (CmdUtils::CreateCommand(kPrsSetImageInfosCmdBoss));
								setMissingFileInfoCmd->SetItemList(UIDList(itemHier));
								
								InterfacePtr<IPMPointData> deleteScaleData (setMissingFileInfoCmd, IID_IIMAGESCALE);
								InterfacePtr<IPMPointData> deleteShiftData (setMissingFileInfoCmd, IID_IIMAGESHIFT);
								InterfacePtr<IStringData> deletePathData (setMissingFileInfoCmd, IID_IIMAGEPATH);

								deleteScaleData->Set(PMPoint(0,0));
								deleteShiftData->Set(PMPoint(0,0));
								deletePathData->Set(kNullString);

								if((status = CmdUtils::ProcessCommand(setMissingFileInfoCmd)) != kSuccess)
								{
									error = ErrorUtils::PMGetGlobalErrorString();
									break;
								}
							}
							else
							{
								ErrorUtils::PMSetGlobalErrorCode(kSuccess);
								status = kSuccess;
							}
						}						
						else if(Utils<IPageItemTypeUtils>()->IsGraphicFrame(itemHier))
						{
							if(itemHier->GetChildCount() == 1)
							{						
								InterfacePtr<IHierarchy> imageHier (itemHier->QueryChild(0));

								InterfacePtr<IDataLinkReference> linkref (imageHier, UseDefaultIID());
								if(linkref != nil && linkref->GetUID() != kInvalidUID)
								{						
									InterfacePtr<IDataLink>  link (db, linkref->GetUID(), UseDefaultIID());
									if(link != nil)
									{
										IDataLink::StateType state = link->GetStoredState(nil,nil);
										if(state == IDataLink::kLinkOutOfDate)
										{					
											allDone = kFalse;

											// Get IDFile of the image
											NameInfo fileInfo;
											if(link->GetNameInfo(&fileInfo, nil, nil) == 0)
											{																	
												UIDRef dummy;
												InterfacePtr<IItemManager> itemManager (::CreateObject2<IItemManager>(kFrameHelperBoss));
												if((status = itemManager->ImportImageInGraphicFrameCmd(::GetUIDRef(itemHier),FileUtils::SysFileToPMString(*(fileInfo.GetFile())),dummy)) != kSuccess)
												{
													error = ErrorUtils::PMGetGlobalErrorString();
													break;
												}												
											}
										}
										else if(state == IDataLink::kLinkMissing)
											allDone = kFalse;
									}
								}
							}
						}
						else if(Utils<IPageItemTypeUtils>()->IsTextFrame(itemHier)) // Collect inlines within this text frame in order to inspect them
						{
							InterfacePtr<IMultiColumnTextFrame> txtFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
							InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());

							OwnedItemDataList ownedItems;
							Utils<ITextUtils>()->CollectOwnedItems(txtModel, 0, txtModel->TotalLength(),&ownedItems, kFalse);

							OwnedItemDataList::iterator iter = ownedItems.begin();
							while (iter != ownedItems.end()) 
							{
								if (iter->fClassID == kInlineBoss)		
								{
									InterfacePtr<IHierarchy> inlineItem (db, iter->fUID, UseDefaultIID());
									for(int32 i = 0 ; i < inlineItem->GetChildCount() ; ++i)
										itemsOnPage.Append(inlineItem->GetChildUID(i));
								}								
								iter++;
							}
						}
						else if(Utils<IPageItemTypeUtils>()->IsGroup(itemHier))
						{
							int32 nbItemsInGroup = itemHier->GetChildCount();
							for(int32 n = 0 ; n < nbItemsInGroup ; ++n)							
								itemsOnPage.Append(itemHier->GetChildUID(n));							
						}
					}
				}
			}
		}
		else
			allDone = kFalse;

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);
	else
	{
		if(allDone)
		{
			PMString msg(kErrAllPictureInPlace);
			msg.Translate();
			CAlert::InformationAlert(msg);
		}	
	}
}

void PMUUtils::ExportPermRefs()
{
	IDFile TCLfile;
	ErrorCode status = kSuccess;
	PMString error(kErrNilInterface);
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc != nil)
		{
			SDKFileSaveChooser saveDialog;
			PMString defaultFileName("Untitled");
			defaultFileName.Translate();
			defaultFileName.Append(".txt");
			defaultFileName.SetTranslated();
			
			PMString filter("txt");
			filter.SetTranslatable(kFalse);
			PMString filterName("Text Files (*.txt)");
			filterName.SetTranslatable(kFalse);
			
			saveDialog.SetFilename(defaultFileName);
		#ifdef WINDOWS
			saveDialog.AddFilter(0,0,filter, filterName);
		#else
			saveDialog.AddFilter(kAnyCreator,kTextType,filter, filterName);
		#endif
			saveDialog.ShowDialog();
			if(saveDialog.IsChosen())
			{
				status = PermRefsUtils::ExportPermRefs(doc, saveDialog.GetIDFile());	
				if(status != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					CAlert::WarningAlert(error);
				}
				else
					CAlert::InformationAlert(kPMUExportPermRefsDoneKey);
			}		
		}
	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);
}


void PMUUtils::ExportPermRefsWithContent()
{
	IDFile TCLfile;
	ErrorCode status = kSuccess;
	PMString error(kErrNilInterface);
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc != nil)
		{
			SDKFileSaveChooser saveDialog;
			// Nom du document
			PMString docName = kNullString;
			doc->GetName(docName);
			docName.Truncate(5);
			
			// Time
			GlobalTime current;
			current.CurrentTime();
			int32 year=0, month=0, day=0, hour=0, min=0, sec=0;
			current.GetTime(&year, &month, &day, &hour, &min, &sec);
			
			PMString time;
			time.AppendNumber(day);
			time += "-";
			if(month < 10) time += "0";
			time.AppendNumber(month);
			time += "-";
			if(day < 10) time += "0";
			time.AppendNumber(year);			
			
			PMString defaultFileName(docName);
			defaultFileName.Append("_");
			defaultFileName.Append(time);
			defaultFileName.Append("_out");
			defaultFileName.Translate();
			defaultFileName.Append(".tcl");
			defaultFileName.SetTranslated();			
			PMString filter("tcl");
			filter.SetTranslatable(kFalse);
			PMString filterName("TCL Files (*.tcl)");
			filterName.SetTranslatable(kFalse);
			
			saveDialog.SetFilename(defaultFileName);
		
		#ifdef WINDOWS
			saveDialog.AddFilter(0,0,filter, filterName);
		#else
			saveDialog.AddFilter(kAnyCreator, kTextType, filter, filterName);
		#endif
			
			saveDialog.ShowDialog();
			if(saveDialog.IsChosen())
			{
				status = PermRefsUtils::ExportPermRefsWithContent(doc, saveDialog.GetIDFile());	
				if(status != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					CAlert::WarningAlert(error);
				}
				else
					CAlert::InformationAlert(kPMUExportPermRefsDoneKey);
			}		
		}
	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);	
}


void PMUUtils::DeleteAllPermsRefsTCL()
{
	ErrorCode status = kSuccess;
	PMString error(kErrNilInterface);
	do{		 
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc == nil)
			break;
		
		int16 rep = CAlert::ModalAlert(PMString(kPMUDeletePermRefskAlertKey), 
									   kYesString, 
									   kNoString, 
									   kNullString, 
									   2, 
									   CAlert::eWarningIcon) ;
		if(rep == 1){       
			status = PermRefsUtils::DeleteAllPermRefs(doc);	
			if(status != kSuccess){
				error = ErrorUtils::PMGetGlobalErrorString();
				CAlert::WarningAlert(error);
			}
			else
				CAlert::InformationAlert(kPMUDeleteAlltPermRefsDoneKey);
		}

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);	
}


void PMUUtils::ExportRefsListDebug()
{
	ErrorCode status = kSuccess;
	PMString error(kErrNilInterface);
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc == nil)
			break;

		SDKFileSaveChooser saveDialog;

		PMString docName = kNullString;
		doc->GetName(docName);
		docName.Truncate(5); // retire l'extension .indd

		PMString defaultFileName(docName);
		defaultFileName.Append("_refs_debug");
		defaultFileName.Translate();
		defaultFileName.Append(".txt");
		defaultFileName.SetTranslated();

		PMString filter("txt");
		filter.SetTranslatable(kFalse);
		PMString filterName("Text Files (*.txt)");
		filterName.SetTranslatable(kFalse);

		saveDialog.SetFilename(defaultFileName);

	#ifdef WINDOWS
		saveDialog.AddFilter(0,0,filter, filterName);
	#else
		saveDialog.AddFilter(kAnyCreator, kTextType, filter, filterName);
	#endif

		saveDialog.ShowDialog();
		if(saveDialog.IsChosen())
		{
			status = PermRefsUtils::ExportTCLReferencesListDebug(doc, saveDialog.GetIDFile());
			if(status != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				CAlert::WarningAlert(error);
			}
			else
				CAlert::InformationAlert(kPMUExportRefsListDebugDoneKey);
		}

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);
}

void PMUUtils::RepairRefsList()
{
	ErrorCode status = kSuccess;
	PMString error(kErrNilInterface);
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc == nil)
			break;

		int16 rep = CAlert::ModalAlert(PMString(kPMURepairRefsListAlertKey),
									   kYesString,
									   kNoString,
									   kNullString,
									   2,
									   CAlert::eWarningIcon);
		if(rep != 1)
			break; // l'utilisateur a annule

		PMString report;
		status = PermRefsUtils::RepairTCLReferencesList(doc, report);
		if(status != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			CAlert::WarningAlert(error);
		}
		else
		{
			PMString msg(kPMURepairRefsListDoneKey);
			msg.Translate();
			msg.Append(report);
			msg.SetTranslatable(kFalse);
			CAlert::InformationAlert(msg);
		}

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);
}

void PMUUtils::AutoTagDocumentPermsRefsTCL() {
	
	ErrorCode status = kSuccess;
	PMString error(kErrNilInterface);
	do{		 
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc == nil)
			break;
		
		// Process Auto tagging 
		status = PermRefsUtils::AutoTaggingDocumentPermRefs(doc);

		if(status != kSuccess){
			error = ErrorUtils::PMGetGlobalErrorString();
			CAlert::WarningAlert(error);
		}
		else
			CAlert::InformationAlert(kPMUAutoTagDocumentPermRefsDoneKey);

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);	

}


void  PMUUtils::AutoTagDocumentWithReferencePermsRefsTCL() {
	
	ErrorCode status = kSuccess;
	PMString error(kErrNilInterface);
	do{		 
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc == nil)
			break;
		
		status = PermRefsUtils::AutoTaggingDocumentWithReferencePermRefs(doc);

		if(status != kSuccess){
			error = ErrorUtils::PMGetGlobalErrorString();
			CAlert::WarningAlert(error);
		}
		else
			CAlert::InformationAlert(kPMUAutoTagDocumentPermRefsDoneKey);

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(error);	

}
void PMUUtils::ILC_AutoTagDocumentWithReferencePermsRefsTCL() {
	
	ErrorCode status = kSuccess;
	PMString error(kErrNilInterface);
	do{		 
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc == nil)
			break;
		
		status = PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefs(doc);
		
		if(status != kSuccess){
			error = ErrorUtils::PMGetGlobalErrorString();
			CAlert::WarningAlert(error);
		}
		else
			CAlert::InformationAlert(kPMUAutoTagDocumentPermRefsDoneKey);
		
	} while(kFalse);
	
	if(status != kSuccess)
		CAlert::InformationAlert(error);	
	
}

void PMUUtils::ILC_ExportPermRefs()
{
	IDFile TCLfile;
	ErrorCode status = kSuccess;
	PMString error(kErrNilInterface);
	do
	{
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc != nil)
		{
			SDKFileSaveChooser saveDialog;
			// Nom du document
			PMString docName = kNullString;
			doc->GetName(docName);
			docName.Truncate(5);
			
			// Time
			GlobalTime current;
			current.CurrentTime();
			int32 year=0, month=0, day=0, hour=0, min=0, sec=0;
			current.GetTime(&year, &month, &day, &hour, &min, &sec);
			
			PMString time;
			time.AppendNumber(day);
			time += "-";
			if(month < 10) time += "0";
			time.AppendNumber(month);
			time += "-";
			if(day < 10) time += "0";
			time.AppendNumber(year);			
			
			PMString defaultFileName(docName);
			defaultFileName.Append("_");
			defaultFileName.Append(time);
			defaultFileName.Append("_out");
			defaultFileName.Translate();
			defaultFileName.Append(".tcl");
			defaultFileName.SetTranslated();			
			PMString filter("tcl");
			filter.SetTranslatable(kFalse);
			PMString filterName("TCL Files (*.tcl)");
			filterName.SetTranslatable(kFalse);
			
			saveDialog.SetFilename(defaultFileName);
			
#ifdef WINDOWS
			saveDialog.AddFilter(0,0,filter, filterName);
#else
			saveDialog.AddFilter(kAnyCreator, kTextType, filter, filterName);
#endif
			
			saveDialog.ShowDialog();
			if(saveDialog.IsChosen())
			{
				status = PermRefsUtils::ILC_ExportPermRefs(doc, saveDialog.GetIDFile());	
				if(status != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					CAlert::WarningAlert(error);
				}
				else
					CAlert::InformationAlert(kPMUExportPermRefsDoneKey);
			}		
		}
	} while(kFalse);
	
	if(status != kSuccess)
		CAlert::InformationAlert(error);	
}
