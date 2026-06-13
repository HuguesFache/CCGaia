/*
//	File:	ParserPlugIn.cpp
//
//  Author: Wilfried LEFEVRE
//
//	Date:	2-July-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#include "VCPlugInHeaders.h"

// API includes
#include "PlugIn.h"
#include "CmdUtils.h"
#include "UIDList.h"
#include "ErrorUtils.h"

// Interfaces includes
#include "IDocument.h"
#include "ICommand.h"

// Project includes
#include "ParserPlugIn.h"
#include "ITCLReferencesList.h"

#include "CAlert.h"

/* Destructor
*/
ParserPlugIn::~ParserPlugIn()
{
}


/* FixUpData
*/
void ParserPlugIn::FixUpData(IDocument* doc)
{
	ErrorCode status = kFailure;

	do
	{
		if(doc == nil)
			break;

		IDataBase * db = ::GetDataBase(doc);

		// Get the saved reference list of the document
		InterfacePtr<ITCLReferencesList> docRefList (doc,UseDefaultIID());
		if(docRefList == nil)
		{		
			ASSERT_FAIL("ParserPlugIn::FixUpData -> docRefList nil");
			break;
		}

		RefVector itemListToUpdate = docRefList->GetItemList();

		// Check the items list : remove those that no longer exist
		RefVector::iterator iter = itemListToUpdate.begin();
		while(iter < itemListToUpdate.end())
		{
			UID storedUID = iter->Value();

			// Check if UIDs exist in database
			if(!db->IsValidUID(storedUID)) 
			{
				// UID doesn't exist anymore,
				// append the item location(in listToUpdate) to the list of items to remove
				iter = itemListToUpdate.erase(iter);
			}
			else
				iter++;
		}

		// Fix up stories now
		PermRefVector storyListToUpdate = docRefList->GetStoryList();
		
		PermRefVector::iterator iter2 = storyListToUpdate.begin();
		while(iter2 < storyListToUpdate.end())
		{
			K2Vector<UID>& storiesUID = iter2->Value();
			
			K2Vector<UID>::iterator iter3 = storiesUID.begin();			
			while(iter3 < storiesUID.end())
			{
				if(!db->IsValidUID(*iter3))
					iter3 = storiesUID.erase(iter3);
				else
					iter3++;
			}

			if(storiesUID.empty()) // PermRef has disappeared
					iter2 = storyListToUpdate.erase(iter2);
				else
					iter2++;
		}

		// Fix up tables
		TableVector tableListToUpdate = docRefList->GetTableList();
		
		TableVector::iterator iter4 = tableListToUpdate.begin();
		while(iter4 < tableListToUpdate.end())
		{
			UID tableUID = iter4->Value();

			// Check if UIDs exist in database
			if(!db->IsValidUID(tableUID)) 
			{
				// UID doesn't exist anymore,
				// append the item location(in listToUpdate) to the list of items to remove
				iter4 = tableListToUpdate.erase(iter4);
			}
			else
				iter4++;
		}


		// Update the document references list with a command
		UIDList docToSave(::GetUIDRef(doc));
		InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
		if(saveItemsCmd == nil)
		{		
			ASSERT_FAIL("ParserPlugIn::FixUpData -> saveItemsCmd nil");
			break;
		}

		saveItemsCmd->SetItemList(docToSave);

		InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());
		if(cmdData == nil)
		{		
			ASSERT_FAIL("ParserPlugIn::FixUpData -> cmdData nil");
			break;
		}

		cmdData->SetItemList(itemListToUpdate);
		cmdData->SetStoryList(storyListToUpdate);
		cmdData->SetTableList(tableListToUpdate);

		status = CmdUtils::ProcessCommand(saveItemsCmd);

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}