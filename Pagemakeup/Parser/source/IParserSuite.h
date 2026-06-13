/*
//	File:	IParserSuite.h
//
//  Author: Trias
//
//	Date:	19/07/2205
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef _IParserSuite_
#define _IParserSuite_


#include "PrsID.h"

class IPMUnknown;


/** IParserSuite
*/
class IParserSuite : public IPMUnknown
{
//	Data Types
public:
	enum { kDefaultIID = IID_IPARSERSUITE };

//	Member functions
public:
	/**
		Get the list of all selected objects
		@param OUT items : UIDlist of selected objects
	*/
	virtual void GetSelectedItems(UIDList& items) = 0;
};

#endif // _IParserSuite_


