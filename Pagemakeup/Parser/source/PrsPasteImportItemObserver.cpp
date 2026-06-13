/*
//	File:	PrsPasteImportItemObserver.cpp
//
//	Date:	23-Nov-2006
//
*/

#include "VCPluginHeaders.h"

// Interface includes
#include "ISubject.h"
#include "IImportResourceCmdData.h"
#include "ITCLReferencesList.h"
#include "IPageItemAdornmentList.h"
#include "IClassIDData.h"
#include "IPMUnknownData.h"
#include "IUIDListData.h"
#include "IHierarchy.h"
#include "IMultiColumnTextFrame.h"
#include "ITextModel.h"
#include "IPageItemTypeUtils.h"
#include "PermRefsUtils.h"
#include "URI.h"
#include "IURIUtils.h"

// General includes
#include "CObserver.h"
#include "GenericID.h"
#include "OpenPlaceID.h"
#include "PageItemScrapID.h"
#include "FileUtils.h"
#include "UIDList.h"
#include "IDocument.h"

// Project includes
#include "PrsID.h"
#include "PMUTypes.h"


#include "DebugClassUtils.h"
#include "CAlert.h"

/** PrsPasteImportItemObserver
	Observes paste and import snippet actions in order to register items if they have a TCL reference

	@author Trias
*/
class PrsPasteImportItemObserver : public CObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsPasteImportItemObserver( IPMUnknown* boss );
	/** Destructor
	*/
	virtual ~PrsPasteImportItemObserver() {}

	/** 
		Attaches this observer to the document it's aggregated onto
	*/
	void AutoAttach();

	/** 
		Detaches this observer to the document it's aggregated onto
	*/
	void AutoDetach();

	/**
		Update is called for all registered observers, and is
		the method through which changes are broadcast. 
		@param theChange this is specified by the agent of change; it can be the class ID of the agent,
		or it may be some specialised message ID.
		@param theSubject this provides a reference to the object which has changed; in this case, the
		widget boss objects that are being observed.
		@param protocol the protocol along which the change occurred.
		@param changedBy this can be used to provide additional information about the change or a reference
		to the boss object that caused the change.
	*/
	void Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy);

private:

	void  AttachDocument (IDocument *document); // Attaches this observer to a document. 
	void  DetachDocument (IDocument *document); //Detaches this observer from a document. 

	K2Vector<UID> itemsFromSnippet; // Gathers UIDs of items created from a snippet
	IDataBase * db; // Database of the document this observer is aggregated onto

	/**
		Add item's TCL reference to the document's reference list and add an adornment to the item
		Add PermRefs to the document's reference list, if any
	*/
	void RegisterItem(UID item);

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(PrsPasteImportItemObserver, kPrsPasteImportItemObserverImpl)

/* Constructor
*/
PrsPasteImportItemObserver::PrsPasteImportItemObserver( IPMUnknown* boss ) :CObserver( boss ){
	db = ::GetDataBase(this);
}

void PrsPasteImportItemObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{
	if(theChange == kNewPageItemCmdBoss) // Handle pasted page items (add their TCL reference, if any, to the document's reference list)
	{
		ICommand * newPageItemCmd = static_cast<ICommand *>(changedBy);		

		if(::GetClass(newPageItemCmd) == kCopyPageItemCmdBoss)
		{
			if(newPageItemCmd->GetCommandState() == ICommand::kDone)
			{
				const UIDList * itemList = newPageItemCmd->GetItemList();
				for(int32 i = 0 ; i < itemList->Length() ; ++i)
					this->RegisterItem(itemList->At(i));
			}			
		}
	}
	else if(theChange == kImportAndPlaceCmdBoss)
	{
		ICommand * importFileCmd = static_cast<ICommand *>(changedBy);

		InterfacePtr<IImportResourceCmdData> cmdData (importFileCmd, IID_IIMPORTRESOURCECMDDATA);
		URI importedURI = cmdData->GetURI();

		IDFile importedFile;
		Utils<IURIUtils>()->URIToIDFile(importedURI, importedFile);

		PMString ext;
		FileUtils::GetExtension(importedFile, ext);

		if(ext.Compare(kFalse, "inds") == 0) // We only cares about snippet imports
		{
			if(importFileCmd->GetCommandState() == ICommand::kNotDone) // means a snippet is going to be imported manually, look for created frames
			{
				InterfacePtr<ISubject> theDocSubject (this, UseDefaultIID());
				theDocSubject->AttachObserver(this, IID_IHIERARCHY_DOCUMENT, IID_PASTEIMPORTITEMOBSERVER); 

				// Clean up imported item list
				InterfacePtr<IUIDListData> importedItems (this, IID_IPRSLASTIMPORTEDSNIPPET);
				if(importedItems->GetRef())
					importedItems->Set(nil);
			}
			else if(importFileCmd->GetCommandState() == ICommand::kDone) // means a snippet has been imported manually, stop looking for created frames
			{	
				// Detach document hierarchy observer
				InterfacePtr<ISubject> theDocSubject (this, UseDefaultIID()); 
				theDocSubject->DetachObserver(this, IID_IHIERARCHY_DOCUMENT, IID_PASTEIMPORTITEMOBSERVER); 

				// Register each newly created items 				
				InterfacePtr<IUIDListData> importedItems (this, IID_IPRSLASTIMPORTEDSNIPPET);
				const UIDList * itemsFromSnippet = importedItems->GetRef();
				
				for(int32 i = 0 ; i < itemsFromSnippet->Length() ; ++i)				
					this->RegisterItem(itemsFromSnippet->At(i));	

				// Clean up imported item list
				importedItems->Set(nil);
			}
		}
	}
	else if(theChange == kAddToHierarchyCmdBoss) // called each time an item is created from a snippet (manually or automatically via command IE)
	{
		const UIDList * itemList = (static_cast<ICommand *>(changedBy))->GetItemList();
		if(itemList)
		{
			InterfacePtr<IUIDListData> importedItems (this, IID_IPRSLASTIMPORTEDSNIPPET);
			UIDList * newList = new UIDList(db);

			if(importedItems->GetRef())
				newList->Append(*(importedItems->GetRef()));

			for(int32 i = 0 ; i < itemList->Length() ; ++i) // Can't get TCLRef here, it has not been set yet
			{
				InterfacePtr<ITCLRefData> tclRefData (itemList->GetRef(i), IID_ITCLREFDATA);
				if(tclRefData) // We only cares about items which aggregate IID_ITCLREFDATA (that is to say kSplineItemBoss);
					newList->Append(itemList->At(i));
			}

			importedItems->Set(newList);
		}
	}
}

void PrsPasteImportItemObserver::AutoAttach(){
     CObserver::AutoAttach();
     InterfacePtr<IDocument> document(this, UseDefaultIID());
     if (document != nil)
         this->AttachDocument(document);
}

void PrsPasteImportItemObserver::AutoDetach() {
     CObserver::AutoDetach(); 
     InterfacePtr<IDocument> document(this, UseDefaultIID());
     if (document != nil)
         this->DetachDocument(document);
}

void PrsPasteImportItemObserver::DetachDocument(IDocument *  document){
     do{
         InterfacePtr<ISubject> docSubject(document, UseDefaultIID());
         ASSERT(docSubject);
         if(!docSubject)
             break;

         if (docSubject->IsAttached(ISubject::kRegularAttachment,this, IID_IHIERARCHY_DOCUMENT, IID_PASTEIMPORTITEMOBSERVER)) {
             docSubject->DetachObserver(ISubject::kRegularAttachment,this, IID_IHIERARCHY_DOCUMENT, IID_PASTEIMPORTITEMOBSERVER);
         }
     } while (kFalse);
}

void PrsPasteImportItemObserver::AttachDocument(IDocument* document){	
	do{
		InterfacePtr<ISubject> docSubject(document, UseDefaultIID());
		if(!docSubject) 
             break;
	
         if(!docSubject->IsAttached(ISubject::kRegularAttachment,this, IID_IHIERARCHY_DOCUMENT, IID_PASTEIMPORTITEMOBSERVER)) {
             docSubject->AttachObserver(ISubject::kRegularAttachment,this, IID_IHIERARCHY_DOCUMENT, IID_PASTEIMPORTITEMOBSERVER);
         }
     } while (kFalse);
}


void PrsPasteImportItemObserver::RegisterItem(UID item){
	do{
		// Check if the new ref is attributed somewhere else in the document
		InterfacePtr<ITCLReferencesList> docRefList(this,UseDefaultIID());			
		RefVector itemList = docRefList->GetItemList();
		PermRefVector storyList = docRefList->GetStoryList();

		// Get item's TCL reference, if any
		InterfacePtr<ITCLRefData> tclRefData (db, item, IID_ITCLREFDATA);
		if(!tclRefData) break;

		bool8 saveRequired = kFalse;

		TCLRef ref = tclRefData->Get();

		if(ref != kDefaultTCLRefData) // this is not tagged, don't register
		{		
			int32 location = ::FindLocation(itemList, ref);
			if(location >= 0) // Already used, don't register item and reset its reference
			{
				InterfacePtr<ICommand> setTCLRefCmd (CmdUtils::CreateCommand(kPrsSetTCLRefDataCmdBoss));
				setTCLRefCmd->SetUndoability(ICommand::kAutoUndo);
				setTCLRefCmd->SetItemList(UIDList(tclRefData));
				
				InterfacePtr<ITCLRefData> cmdData (setTCLRefCmd, IID_ITCLREFDATA);
				cmdData->Set(kDefaultTCLRefData);
				CmdUtils::ProcessCommand(setTCLRefCmd);	
			}
			else
			{								
				// Add ref to the document's list of references
				::InsertOrReplaceKeyValue(itemList,ref,item);
				saveRequired = kTrue;
			}
		}

		// If it'a text frame, look for some permrefs
		if(Utils<IPageItemTypeUtils>()->IsTextFrame(tclRefData))
		{
			InterfacePtr<IHierarchy> itemHier (tclRefData, UseDefaultIID());
			InterfacePtr<IMultiColumnTextFrame> txtFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
			InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());

			if(PermRefsUtils::UpdatePermRefsList(storyList, txtModel, 0, txtModel->TotalLength()-1))
				saveRequired = kTrue;
		}

		if(saveRequired)
		{
			UIDList docToSave(this);
			InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
					
			saveItemsCmd->SetUndoability(ICommand::kAutoUndo);
			saveItemsCmd->SetItemList(docToSave);

			InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());
			cmdData->SetItemList(itemList);
			cmdData->SetStoryList(storyList);
			cmdData->SetTableList(docRefList->GetTableList());

			CmdUtils::ProcessCommand(saveItemsCmd);
					
			// Add adornment to the item			
			InterfacePtr<IPageItemAdornmentList> adornmentList (db, item, UseDefaultIID());
			if(!adornmentList->HasAdornment(kPrsTCLAdornmentBoss))
			{
				InterfacePtr<ICommand>  addPageItemAdornmentCmd(CmdUtils::CreateCommand(kAddPageItemAdornmentCmdBoss));
				InterfacePtr<IClassIDData>  classIDData(addPageItemAdornmentCmd, UseDefaultIID());

				classIDData->Set(kPrsTCLAdornmentBoss);
				addPageItemAdornmentCmd->SetItemList(UIDList(adornmentList));
				
				CmdUtils::ProcessCommand(addPageItemAdornmentCmd);		
			}
		}
		
	} while(kFalse);
}