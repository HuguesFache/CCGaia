/*
//
//	File:	PrsDeleteCleanup.cpp
//
//  Author: Trias
//
//	Date:	26-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CPMUnknown.h"
#include "UIDList.h"
#include "ErrorUtils.h"

// Interface includes
#include "IDeleteCleanup.h"
#include "ICommand.h"

// Project includes
#include "ITCLReferencesList.h"


/** PrsDeleteCleanup
	Implement IDeleteCleanup
	Service called by the application after each item deletion
*/
class PrsDeleteCleanup : public CPMUnknown<IDeleteCleanup>
{
	public :
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PrsDeleteCleanup(IPMUnknown * boss);

		/** 
			Destructor
		*/
		~PrsDeleteCleanup();

		/**
			Do nothing
		*/
		virtual ICommand * GetPostDeleteCommand(ICommand * cmd, IScrapItem * scrapItem);

		/**
			Called before items deletion
			Take off from the document's TCL referenced items list the items that have
			been deleted
		*/
		virtual ICommand * GetPreDeleteCommand(ICommand * cmd, IScrapItem * scrapItem);
};


CREATE_PMINTERFACE(PrsDeleteCleanup, kPrsDeleteCleanupImpl)


/** Constructor
*/
PrsDeleteCleanup::PrsDeleteCleanup(IPMUnknown * boss)
: CPMUnknown<IDeleteCleanup>(boss)
{
}

/** Destructor
*/
PrsDeleteCleanup::~PrsDeleteCleanup()
{
}


/** GetPostDeleteCommand
*/
ICommand * PrsDeleteCleanup::GetPostDeleteCommand(ICommand * cmd, IScrapItem * scrapItem)
{
	return nil;
}


/** GetPreDeleteCommand
*/
ICommand * PrsDeleteCleanup::GetPreDeleteCommand(ICommand * cmd, IScrapItem * scrapItem)
{
	ErrorCode status = kFailure;

	do
	{
		const UIDList * list = cmd->GetItemList(); // List of deleted items
		
		IDataBase * db = list->GetDataBase(); 

		// Get the document list of TCL referenced items
		InterfacePtr<ITCLReferencesList> docRefList (db, db->GetRootUID(), UseDefaultIID());

		if(docRefList == nil)
		{		
			ASSERT_FAIL("PrsDeleteCleanup::GetPreDeleteCommand -> docRefList nil");
			//BUG FIX 14/09/07 : when trying to copy items, sometimes docRefList is nil (it may not be asked from a kDocBoss)
			// dont' consider this as an error
			status = kSuccess;
			break;
		}

		RefVector listToUpdate = docRefList->GetItemList();

		K2Vector<TCLRef> itemsToRemove;

		// Check the document items list : remove those that no longer exist
		RefVector::iterator iter;
		for(iter = listToUpdate.begin(); iter < listToUpdate.end(); iter++)
		{
			UID itemUID = iter->Value();

			// Check if itemUID is in the list of deleted items
			if(list->Location(itemUID) >=0)
			{
				// one of the UID doesn't exist anymore,
				// append the item location(in listToUpdate) to the list of items to remove
				itemsToRemove.push_back(iter->Key());
			}
		}

		if(itemsToRemove.size() > 0)
		{
			for(int32 i = 0; i < itemsToRemove.size(); i++)
			{
				listToUpdate.erase(listToUpdate.begin() + ::FindLocation(listToUpdate, itemsToRemove[i]));
			}

			// Update the document items list with a command
			UIDList docToSave(UIDRef(db, db->GetRootUID()));
			InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
			if(saveItemsCmd == nil)
			{		
				ASSERT_FAIL("PrsDeleteCleanup::GetPreDeleteCommand -> saveItemsCmd nil");
				break;
			}

			saveItemsCmd->SetItemList(docToSave);
			saveItemsCmd->SetUndoability(ICommand::kAutoUndo);

			InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());
			if(cmdData == nil)
			{		
				ASSERT_FAIL("PrsDeleteCleanup::GetPreDeleteCommand -> cmdData nil");
				break;
			}

			cmdData->SetItemList(listToUpdate);
			cmdData->SetStoryList(docRefList->GetStoryList());
			cmdData->SetTableList(docRefList->GetTableList());

			status = CmdUtils::ProcessCommand(saveItemsCmd);
		}
		else
			status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}

	return nil;
}