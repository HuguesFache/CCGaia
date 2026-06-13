/*
//
//	File: PrsTCLReferencesList.cpp
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

/** PrsTCLReferencesList
	Implement IPrsTCLReferencesList interface, non-persistent implementation only to
	stock data for the save item list command
*/

class PrsTCLReferencesList : public CPMUnknown<ITCLReferencesList>
{
public:
	/**
		Constructor.

		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsTCLReferencesList(IPMUnknown* boss);


	/** Destructor
	*/
	~PrsTCLReferencesList();

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

private:
	
	RefVector itemList;
	PermRefVector storyList;
	TableVector tableList;
};

CREATE_PMINTERFACE(PrsTCLReferencesList, kPrsTCLReferencesListImpl)

/* Constructor
*/
PrsTCLReferencesList::PrsTCLReferencesList(IPMUnknown* boss)
: CPMUnknown<ITCLReferencesList>(boss)
{
}

/* Destructor
*/
PrsTCLReferencesList::~PrsTCLReferencesList()
{
}


/* SetItemList
*/
void PrsTCLReferencesList::SetItemList(RefVector newList)
{
	itemList = newList;
}

/* GetItemList
*/
RefVector PrsTCLReferencesList::GetItemList()
{
	return itemList;
}

/* SetStoryList
*/
void PrsTCLReferencesList::SetStoryList(PermRefVector newList)
{
	storyList = newList;
}

/* GetStoryList
*/
PermRefVector PrsTCLReferencesList::GetStoryList()
{
	return storyList;
}

/* SetTableList
*/
void PrsTCLReferencesList::SetTableList(TableVector newList)
{
	tableList = newList;
}

/* GetTableList
*/
TableVector PrsTCLReferencesList::GetTableList()
{
	return tableList;
}
