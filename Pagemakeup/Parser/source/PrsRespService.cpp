/*
//
//	File:	PrsRespService.cpp
//
//  Author: Trias
//
//	Date:	28-Oct-2005
//
*/

#include "VCPlugInHeaders.h"

#include "GlobalDefs.h"

// General includes
#include "Utils.h"
#include "FileUtils.h"
// Project includes
#include "PrsID.h"
#include "TextID.h"

// Interface includes:
#include "ISignalMgr.h"
#include "ICommand.h"
#include "IDocumentSignalData.h"
#include "ITCLReferencesList.h"
#include "IObserver.h"
#include "IDocumentCommands.h"
#include "IDocument.h"
#include "IUIFlagData.h"
#include "ISysFileData.h"
#include "ISpreadList.h"
#include "ISpread.h"
#include "IScriptLabel.h"
#include "PMUTypes.h"

// Implementation includes:
#include "CResponder.h"
#include "UIDList.h"

#include "CAlert.h"


/* IDML-survival re-hydration helpers
 *
 * Pagemakeup's TCLRef impl is a plain IStringData/IIntData alias whose
 * binary persistence does NOT survive an IDML round-trip (or opening
 * the doc with the plugin absent). PrsSetTCLRefDataCmd::Do mirrors each
 * write into the page item's IScriptLabel under "PMU_TCLRef" — that
 * label DOES survive. On open we walk the doc and reconcile the impl
 * with the label per-item.
*/
static const char* const kPrsTCLRefLabelKey = "PMU_TCLRef";

static void SerializeTCLRefToString(const TCLRef& ref, PMString& out)
{
#if REF_AS_STRING
	out = ref;
#else
	out.Clear();
	out.AppendNumber(ref);
#endif
}

static bool16 ParseTCLRefFromString(const PMString& str, TCLRef& out)
{
#if REF_AS_STRING
	out = str;
	return !str.IsEmpty();
#else
	if (str.IsEmpty())
		return kFalse;
	PMString::ConversionError err = PMString::kNoError;
	const int32 v = str.GetAsNumber(&err);
	if (err != PMString::kNoError)
		return kFalse;
	out = v;
	return kTrue;
#endif
}

/* ReconcileTCLRefLabels
 *
 * Walk every page item in the document, then for each item that
 * aggregates ITCLRefData reconcile its impl value with the IScriptLabel
 * under "PMU_TCLRef". Four cases:
 *   (a) impl set + label absent     -> migration (doc was written by an
 *                                      older Pagemakeup): stamp the label
 *                                      from the impl so the next save
 *                                      carries it through IDML.
 *   (b) impl invalid + label set    -> rehydrate: write the impl from
 *                                      the label. This is the IDML
 *                                      round-trip recovery path.
 *   (c) both set + agree            -> nothing.
 *   (d) both set + disagree         -> impl wins (resync label). The
 *                                      binary impl is the source of
 *                                      truth at open time; a stale
 *                                      label could come from a copy/paste
 *                                      between docs.
 *
 * The impl Set is direct (no command, no undo): we are repairing state
 * silently during open, the user shouldn't see this in their undo stack.
*/
static void ReconcileTCLRefLabels(const UIDRef& docRef)
{
	InterfacePtr<IDocument> doc(docRef, UseDefaultIID());
	if (doc == nil)
		return;

	IDataBase* db = docRef.GetDataBase();
	if (db == nil)
		return;

	InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
	if (spreadList == nil)
		return;

	PMString labelKey(kPrsTCLRefLabelKey, PMString::kUnknownEncoding);
	labelKey.SetTranslatable(kFalse);

	const int32 nbSpreads = spreadList->GetSpreadCount();
	for (int32 s = 0; s < nbSpreads; ++s)
	{
		InterfacePtr<ISpread> spread(db, spreadList->GetNthSpreadUID(s), UseDefaultIID());
		if (spread == nil)
			continue;

		UIDList itemsOnSpread(db);
		const int32 nbPages = spread->GetNumPages();
		for (int32 p = 0; p < nbPages; ++p)
			spread->GetItemsOnPage(p, &itemsOnSpread, kFalse, kTrue);

		const int32 nbItems = itemsOnSpread.Length();
		for (int32 i = 0; i < nbItems; ++i)
		{
			UIDRef itemRef = itemsOnSpread.GetRef(i);

			InterfacePtr<ITCLRefData> tclRef(itemRef, IID_ITCLREFDATA);
			if (tclRef == nil)
				continue; // item type doesn't carry a TCLRef.

			InterfacePtr<IScriptLabel> labelData(itemRef, IID_ISCRIPTLABEL);
			if (labelData == nil)
				continue;

			const TCLRef implValue = tclRef->Get();
			const bool16 implHasValue = (implValue != kInvalidTCLRef);

			const PMString labelValue = labelData->GetTag(labelKey);
			const bool16 labelHasValue = !labelValue.IsEmpty();

			if (!implHasValue && labelHasValue)
			{
				// (b) rehydrate impl from label
				TCLRef parsed;
				if (ParseTCLRefFromString(labelValue, parsed))
					tclRef->Set(parsed);
			}
			else if (implHasValue && !labelHasValue)
			{
				// (a) migrate: stamp label from impl
				PMString str;
				SerializeTCLRefToString(implValue, str);
				str.SetTranslatable(kFalse);
				labelData->SetTag(labelKey, str);
			}
			else if (implHasValue && labelHasValue)
			{
				// (c) or (d): check agreement
				PMString implStr;
				SerializeTCLRefToString(implValue, implStr);
				if (implStr != labelValue)
				{
					// (d) impl wins, resync label
					implStr.SetTranslatable(kFalse);
					labelData->SetTag(labelKey, implStr);
				}
			}
			// else: both empty — nothing to do.
		}
	}
}

class PrsRespService : public CResponder
{
	public:
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PrsRespService(IPMUnknown* boss);

		/**
			Respond() handles the file action signals when they
			are dispatched by the signal manager.  This implementation
			simply creates alerts to display each signal.

			@param signalMgr Pointer back to the signal manager to get
			additional information about the signal.
		*/
		virtual void Respond(ISignalMgr* signalMgr);

};


/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PrsRespService, kPrsRespServiceImpl)

/* DocWchActionComponent Constructor
*/
PrsRespService::PrsRespService(IPMUnknown* boss) :
	CResponder(boss)
{
}

/* Respond
*/
void PrsRespService::Respond(ISignalMgr* signalMgr)
{
	// Get the service ID from the signal manager
	ServiceID serviceTrigger = signalMgr->GetServiceID();

	if(serviceTrigger.Get() == kDeleteStoryRespService)
	{
		InterfacePtr<ICommand> theDeleteCommand(signalMgr, UseDefaultIID()); 

		const UIDList& deletedStories = theDeleteCommand->GetItemListReference(); 

		UIDRef doc = UIDRef(deletedStories.GetDataBase(), deletedStories.GetDataBase()->GetRootUID());
		InterfacePtr<ITCLReferencesList> docRefList (doc, UseDefaultIID());
		if(!docRefList)
			return;

		PermRefVector listToUpdate = docRefList->GetStoryList();

		for(int32 i = 0 ; i < deletedStories.Length() ; ++i)
		{
			UID story = deletedStories[i];
			
			// Check if this story is referenced
			PermRefVector::iterator iter = listToUpdate.begin();
			while(iter < listToUpdate.end())
			{
				K2Vector<UID>& storiesUID = iter->Value();
				
				K2Vector<UID>::iterator iter2 = ::K2find(storiesUID.begin(), storiesUID.end(),story);
				if(iter2 != storiesUID.end())
					storiesUID.erase(iter2);

				if(storiesUID.empty()) // PermRef has disappeared
					iter = listToUpdate.erase(iter);
				else
					iter++;
			}
		}
		
		// Update document item list
		InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
	
		saveItemsCmd->SetItemList(UIDList(doc));
		saveItemsCmd->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());

		cmdData->SetStoryList(listToUpdate);
		cmdData->SetItemList(docRefList->GetItemList());
		cmdData->SetTableList(docRefList->GetTableList());

		CmdUtils::ProcessCommand(saveItemsCmd);
	}
	else if(serviceTrigger.Get() == kDuringOpenDocSignalResponderService)
	{
		InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());

		// Attach paste / import snippet observer
		InterfacePtr<IObserver> docObserver (docData->GetDocument(), IID_PASTEIMPORTITEMOBSERVER);
		docObserver->AutoAttach();

		// Clean up table list, tables may have been removed manually
		UIDRef doc = docData->GetDocument();
		InterfacePtr<ITCLReferencesList> docItemList (doc, UseDefaultIID());

		TableVector tableList = docItemList->GetTableList();

		IDataBase * db = doc.GetDataBase();
		TableVector::iterator iter = tableList.begin();
		while(iter != tableList.end())
		{
			if(!db->IsValidUID(iter->Value()))
			{
				iter = tableList.erase(iter);
			}
			else
				iter++;
		}

		UIDList docToSave(doc);
		InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
		saveItemsCmd->SetUndoability(ICommand::kAutoUndo);
		saveItemsCmd->SetItemList(docToSave);

		InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());
		cmdData->SetItemList(docItemList->GetItemList());
		cmdData->SetStoryList(docItemList->GetStoryList());
		cmdData->SetTableList(tableList);
		
		CmdUtils::ScheduleCommand(saveItemsCmd);

		// IDML round-trip survival: reconcile each item's TCLRef impl
		// with its IScriptLabel mirror. Recovers TCLRefs for docs that
		// went through IDML / a snippet / an InDesign without Pagemakeup
		// loaded; also stamps the label on existing docs where the
		// migration hasn't run yet.
		ReconcileTCLRefLabels(doc);

#if !REF_AS_STRING // Implementation of IStringData interface aggregated by kSplineItemBoss seems to be persistent ! thus, no need to set again the values
		InterfacePtr<IDocument> theDoc (docData->GetDocument(), UseDefaultIID());
		if(theDoc && theDoc->IsConverted())	
		{
			// Reset TCL data on spline items when doc is converted, because format has changed
			RefVector list = docItemList->GetItemList();
			for(RefVector::iterator iter = list.begin(); iter < list.end(); iter++)
			{				
				TCLRef ref = iter->Key();
				
				InterfacePtr<ITCLRefData> refData (doc.GetDataBase(), iter->Value(), IID_ITCLREFDATA);
				if(refData != nil)
				{
					TCLRef tmp = refData->Get();
					if(tmp != ref)
					{
						InterfacePtr<ICommand> setTCLRefCmd (CmdUtils::CreateCommand(kPrsSetTCLRefDataCmdBoss));
						setTCLRefCmd->SetUndoability(ICommand::kAutoUndo);
						setTCLRefCmd->SetItemList(UIDList(refData));
						
						InterfacePtr<ITCLRefData> cmdData (setTCLRefCmd, IID_ITCLREFDATA);
						cmdData->Set(ref);
						CmdUtils::ProcessCommand(setTCLRefCmd);
					}
				}
			}
		}
#endif		
	}
	else if(serviceTrigger.Get() == kAfterNewDocSignalResponderService)
	{
		InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());

		// Attach paste / import snippet observer
		InterfacePtr<IObserver> docObserver (docData->GetDocument(), IID_PASTEIMPORTITEMOBSERVER);
		docObserver->AutoAttach();
	}
	else if(serviceTrigger.Get() == kBeforeCloseDocSignalResponderService)
	{
		InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());

		// Detach paste / import snippet observer
		InterfacePtr<IObserver> docObserver (docData->GetDocument(), IID_PASTEIMPORTITEMOBSERVER);
		docObserver->AutoDetach();
	}
	else if(serviceTrigger.Get() == kAfterOpenDocSignalResponderService) // Force converted document to be saved if CONVERT_AUTOSAVE flag is enabled
	{
		InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
		InterfacePtr<IDocument> theDoc (docData->GetDocument(), UseDefaultIID());

		if(theDoc && theDoc->IsConverted())
		{
#if CONVERT_AUTOSAVE
			
			IDFile srcFile = docData->GetFile();

			InterfacePtr<ICommand> saveCmd (Utils<IDocumentCommands>()->CreateSaveAsCommand());
			InterfacePtr<ISysFileData> fileData (saveCmd, UseDefaultIID());
			fileData->Set(srcFile);

			InterfacePtr<IUIFlagData> uiflags (saveCmd, IID_IUIFLAGDATA);
			uiflags->Set(kSuppressUI);

			saveCmd->SetItemList(UIDList(theDoc));

			theDoc.reset(nil);
			CmdUtils::ProcessCommand(saveCmd);
#endif
		}
	}
}