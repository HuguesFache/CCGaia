/*
//
//	File: PrsTCLReferencesListPersist.cpp
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

// API includes
#include "CPMUnknown.h"

// Project includes:
#include "ITCLReferencesList.h"
#include "IPMStream.h"

#include "CAlert.h"
#include "GlobalDefs.h"

/** PrsTCLReferencesListPersist
	Persistent implementation of ITCLReferencesList, added to the kDocBoss
*/

class PrsTCLReferencesListPersist : public CPMUnknown<ITCLReferencesList>
{
public:
	/**
		Constructor.

		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsTCLReferencesListPersist(IPMUnknown* boss);


	/** Destructor
	*/
	~PrsTCLReferencesListPersist();

	/**
		Accessor methods for item list
	*/	
	virtual RefVector GetItemList();
	virtual void SetItemList(RefVector newList);

	/** 
		Acessor methods for story list
	*/
	virtual void SetStoryList(PermRefVector newList);
	virtual PermRefVector GetStoryList();

	/** 
		Acessor methods for table list
	*/
	virtual void SetTableList(TableVector newList);
	virtual TableVector GetTableList();

	/**
		Because this is a persistent interface, it must support the ReadWrite method. This method is used for 
		both writing information out to the database and reading information in from the database.

		Note: the first thing we read/write here is a version number.  It is used to identify which plug-in
		is used to write out this item list.  Since this is the first version of the Parser plug-in, thus the version
		number is 1, and then followed by the item list.  Later if there is a newer version of this plug-in which
		will write out an extra field of data, then we can just come to this routine, and check the version number
		of the doc, and then read in the data accordingly.  this will make sure the newer plug-in is backward
		compatible with an older version of document.  Lastly, since the version number should be the first thing
		to read in for this interface, all the new data should be append at the bottom.

		@param stream contains the stream to be written or read.
		@param implementation the implementation ID.
	*/
	void ReadWrite(IPMStream* stream, ImplementationID implementation);

private:
	
	RefVector itemList;
	PermRefVector storyList;
	TableVector tableList;
};

CREATE_PERSIST_PMINTERFACE(PrsTCLReferencesListPersist, kPrsTCLReferencesListPersistImpl)

/* Constructor
*/
PrsTCLReferencesListPersist::PrsTCLReferencesListPersist(IPMUnknown* boss)
: CPMUnknown<ITCLReferencesList>(boss)
{
}

/* Destructor
*/
PrsTCLReferencesListPersist::~PrsTCLReferencesListPersist()
{
}


/* SetItemList
*/
void PrsTCLReferencesListPersist::SetItemList(RefVector newList)
{
	PreDirty(); // Mark the object as dirty, it means it should be saved
	itemList = newList;	
}

/* GetItemList
*/
RefVector PrsTCLReferencesListPersist::GetItemList()
{
	return itemList;
}

/* SetStoryList
*/
void PrsTCLReferencesListPersist::SetStoryList(PermRefVector newList)
{
	PreDirty();
	storyList = newList;
}

/* GetStoryList
*/
PermRefVector PrsTCLReferencesListPersist::GetStoryList()
{
	return storyList;
}


void PrsTCLReferencesListPersist::SetTableList(TableVector newList)
{
	PreDirty();
	tableList = newList;
}

TableVector PrsTCLReferencesListPersist::GetTableList()
{
	return tableList;
}

/* ReadWrite
*/
void PrsTCLReferencesListPersist::ReadWrite(IPMStream* stream, ImplementationID implementation)
{
	// These methods (Xfer...) will read if the stream is a 'read-stream'
	// and write if the stream is a write stream:
	int16 version = 0x04;

	/** format versions :
		0x01 : first version, stores frame and page reference (never released)
		0x02 : second version, only stores frame refence (never released)
		0x03 : add permref storage
		0x04 : add table reference storage
	*/

	/** NOTE 27/02/2007
		There is no need handle version number here anymore, due to the Schema resource added to Prs.fr for kPrsTCLReferencesListPersistImpl
		Version number will no longer evolve, even if following code change. Write a new Schema resource instead.
	*/

	stream->XferInt16(version);
	
	if(stream->IsReading()) // Fill the vector with the saved value
	{
		int32 length = 0;
		stream->XferInt32(length);
	
		for(int32 i = 0; i < length; i++)
		{
			TCLRef itemRef = kInvalidTCLRef;
			UID itemUID = kInvalidUID;

#if REF_AS_STRING		
			itemRef.ReadWrite(stream);
#else
			stream->XferInt32(itemRef);
#endif
			stream->XferObject(itemUID);
			itemList.push_back(TCLRefItem(itemRef,itemUID));
		}
	
		if(version >= 0x03)
		{
			length = 0;
			stream->XferInt32(length);

			for(int32 i = 0; i < length; i++)
			{	
				PermRefStruct PermRef = kNullPermRef;
				int32 nbStories = 0;
				
				stream->XferInt32((int32&)PermRef.ref);
				stream->XferInt32((int32&)PermRef.param1);
				stream->XferInt32((int32&)PermRef.param2);
				stream->XferInt32((int32&)PermRef.param3);
				stream->XferInt32((int32&)PermRef.param4);

				stream->XferInt32(nbStories);
				
				K2Vector<UID> storiesUID;
				for(int32 j = 0; j < nbStories; ++j)
				{
					UID story = kInvalidUID;
					stream->XferObject(story);
					storiesUID.push_back(story);
				}
				storyList.push_back(PermRefStories(PermRef,storiesUID));
			}

			if(version == 0x04)
			{
				stream->XferInt32(length);

				for(int32 i = 0; i < length; i++)
				{
					PMString tableRef;
					UID itemUID = kInvalidUID;

					tableRef.ReadWrite(stream);
					stream->XferObject(itemUID);

					tableList.push_back(TCLRefTable(tableRef,itemUID));
				}
			}
		}
	}
	else // Save the item in the vector
	{
		// BUG FIX : les items references a 0 sont ignores dans la boucle d'ecriture
		// ci-dessous. Le compteur ecrit dans l'entete DOIT donc refleter le nombre
		// d'items REELLEMENT ecrits, sinon la relecture lit trop d'enregistrements
		// et desynchronise tout le flux (storyList/tableList corrompus -> taille du
		// document qui explose a chaque sauvegarde).
		int32 length = 0;
		for (RefVector::iterator iterCount = itemList.begin(); iterCount < itemList.end(); iterCount++)
			if(iterCount->Key() != kZeroTCLRef)
				length++;
		stream->XferInt32(length);

		RefVector::iterator iter;
		for (iter = itemList.begin(); iter < itemList.end(); iter++)
		{
			TCLRef itemRef = iter->Key();
			if(itemRef != kZeroTCLRef) // Don't save the item referenced as 0
			{
				UID itemUID = iter->Value();
#if REF_AS_STRING		
				itemRef.ReadWrite(stream);
#else
				stream->XferInt32(itemRef);
#endif
				stream->XferObject(itemUID);
			}
		}

		length = storyList.size();
		stream->XferInt32(length);
		PermRefVector::iterator iter2;
		for (iter2 = storyList.begin(); iter2 < storyList.end(); iter2++)
		{
			PermRefStruct PermRef = iter2->Key();
			stream->XferInt32((int32&)PermRef.ref);
			stream->XferInt32((int32&)PermRef.param1);
			stream->XferInt32((int32&)PermRef.param2);
			stream->XferInt32((int32&)PermRef.param3);
			stream->XferInt32((int32&)PermRef.param4);
				
			K2Vector<UID> storiesUID = iter2->Value();
			int32 nbStories = storiesUID.size();
			stream->XferInt32(nbStories);
			for(int32 i = 0 ; i < nbStories ; ++i)
				stream->XferObject(storiesUID[i]);
		}

		length = tableList.size();
		stream->XferInt32(length);

		TableVector::iterator iter3;
		for (iter3 = tableList.begin(); iter3 < tableList.end(); iter3++)
		{
			PMString tableRef = iter3->Key();
			tableRef.ReadWrite(stream);
			UID tableUID = iter3->Value();
			stream->XferObject(tableUID);				
		}

	}
}

