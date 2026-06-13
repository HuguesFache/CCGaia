/*
//
//	File:	PrsSetDocTCLReferencesList.cpp
//
//  Author: Trias
//
//	Date:	2-July-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"

// Interface includes
#include "ISubject.h"

// Project includes
#include "ITCLReferencesList.h"
#include "K2Pair.h"

/** PrsSetDocTCLReferencesList
	The implementation of the PrsSetDocTCLReferencesList, it gets the command data from its sibling interface, 
	a non persistent ITCLReferencesList, then use it to set the persist ITCLReferencesList on a document, passed in the
	item list of the command
*/
class PrsSetDocTCLReferencesList : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsSetDocTCLReferencesList(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsSetDocTCLReferencesList();

	/**
		@return kFalse don't use this command if memory is low. 
	*/
	bool16 LowMemIsOK() const { return kFalse; }

protected:
	/**
		The Do method is where all the action happens. In this case we just need to copy the 
		information from the commands data interface to the ITCLReferencesList interface.
	*/
	void Do();

	/**
		Notify interested observer about the change we have done in this command.
	*/
	void DoNotify();

	/**
		Gives the name that shows up under the Edit menu and allows the user to undo or redo the command.

		@return pointer to a PMString allocated using the new operator. Deleted by caller.
	*/
	PMString* CreateName();

private:

};


CREATE_PMINTERFACE(PrsSetDocTCLReferencesList, kPrsSetDocTCLReferencesListImpl)

/* Constructor
*/
PrsSetDocTCLReferencesList::PrsSetDocTCLReferencesList(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
PrsSetDocTCLReferencesList::~PrsSetDocTCLReferencesList()
{
}

/* Do
*/
void PrsSetDocTCLReferencesList::Do()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)// Should have only one kDocBoss in the item list
			break;

		// Get the command datas
		InterfacePtr<ITCLReferencesList> newRefList(this, UseDefaultIID());
		if (newRefList == nil)
		{
			ASSERT_FAIL("PrsSetDocTCLReferencesList::Do -> newRefList nil");
			break;
		}
	
		InterfacePtr<ITCLReferencesList> docRefList(fItemList.GetRef(0), UseDefaultIID());
		if(docRefList == nil)
		{
			ASSERT_FAIL("PrsSetDocTCLReferencesList::Do -> docRefList nil");
			break;
		}

		// Change the item list;
		RefVector newItemList = newRefList->GetItemList();
		PermRefVector newStoryList = newRefList->GetStoryList();
		TableVector newTableList = newRefList->GetTableList();
		
		// Remove the TCL ref 0 item
		int32 location = ::FindLocation(newItemList, kZeroTCLRef);
		if(location >= 0)
			newItemList.erase(newItemList.begin() + location);

		// DEFENSE ANTI-EXPLOSION : tous les writers de l'index passent par cette
		// commande. On collapse ici les entrees storyList en double (meme PermRef)
		// en fusionnant leurs listes de stories (UID uniques). Comme ce nettoyage
		// a lieu a CHAQUE sauvegarde de l'index, les doublons ne peuvent plus
		// s'accumuler d'un cycle a l'autre : la liste reste bornee au nombre de
		// references distinctes, quelle que soit la cause amont des doublons.
		//
		// Garde-fou : sur un document deja corrompu (liste a plusieurs centaines de
		// milliers d'entrees), le dedoublonnage O(n^2) figerait InDesign. Au-dela du
		// seuil on stocke tel quel (pas pire qu'avant) ; ces documents se reparent
		// via le menu "Debug : reparer l'index des references".
		const int32 kDedupGuard = 100000;
		if(newStoryList.size() > 1 && newStoryList.size() <= kDedupGuard)
		{
			PermRefVector dedupStoryList;
			for(PermRefVector::iterator it = newStoryList.begin(); it < newStoryList.end(); ++it)
			{
				int32 loc = ::FindLocation(dedupStoryList, it->Key());
				if(loc < 0)
				{
					::InsertKeyValue(dedupStoryList, it->Key(), it->Value());
				}
				else
				{
					// Fusionne les UID de stories en evitant les doublons.
					K2Vector<UID>& dst = dedupStoryList[loc].Value();
					K2Vector<UID>& src = it->Value();
					for(int32 k = 0 ; k < src.size() ; ++k)
					{
						bool16 present = kFalse;
						for(int32 m = 0 ; m < dst.size() && !present ; ++m)
							if(dst[m] == src[k])
								present = kTrue;
						if(!present)
							dst.push_back(src[k]);
					}
				}
			}
			newStoryList = dedupStoryList;
		}

		docRefList->SetItemList(newItemList);
		docRefList->SetStoryList(newStoryList);
		docRefList->SetTableList(newTableList);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void PrsSetDocTCLReferencesList::DoNotify()
{
	ErrorCode status = kFailure;
			
	do
	{
		if (fItemList.Length() != 1)
			break;
		else
		{
			InterfacePtr<ISubject> subject (fItemList.GetRef(0),UseDefaultIID());
			if(subject == nil)
			{
				ASSERT_FAIL("PrsSetDocTCLReferencesList::DoNotify -> subject nil");
				break;
			}

			// Notify the subject of the change
			subject->Change(kPrsSetDocTCLReferencesListCmdBoss, IID_TCLREFERENCESLIST, this);

			status = kSuccess;
		}
	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}


/* CreateName
*/
PMString * PrsSetDocTCLReferencesList::CreateName()
{
	PMString * name = new PMString("Save TCL References");
	name->SetTranslatable(kFalse);
	return name;
}



