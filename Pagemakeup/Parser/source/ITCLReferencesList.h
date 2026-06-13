/*
//	File:	ITCLReferencesList.h
//
//  Author: Trias
//
//	Date:	26-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __ITCLReferencesList_h__
#define __ITCLReferencesList_h__

#include "PrsID.h"
#include "ITCLParser.h"

class IPMUnknown;

/** Provides get and set methods to handle a list of page items
	(frames and rules) created by PageMakeUp PlugIn
*/
class ITCLReferencesList : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_TCLREFERENCESLIST };


		// Set a new page item list 
		virtual void SetItemList(RefVector newList) =0;

		// Get the page item list
		virtual RefVector GetItemList() =0;

		// Set a new story list (for PermRefs)
		virtual void SetStoryList(PermRefVector newList) =0;

		// Get the story list
		virtual PermRefVector GetStoryList() =0;

		// Set a new table list 
		virtual void SetTableList(TableVector newList) =0;

		// Get the table list
		virtual TableVector GetTableList() =0;
	
};

#endif // __ITCLReferencesList_h__
