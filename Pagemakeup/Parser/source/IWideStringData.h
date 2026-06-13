/*
//	File:	IWideStringData.h
//
//  Author: Trias
//
//	Date:	27/03/2006
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2006 Trias Developpement. All rights reserved.
//
*/

#ifndef _IWideStringData_
#define _IWideStringData_


#include "PrsID.h"

class IPMUnknown;
class WideString;

/** IWideStringData
*/
class IWideStringData : public IPMUnknown
{
//	Data Types
public:
	enum { kDefaultIID = IID_IWIDESTRINGDATA };

//	Member functions
public:

	virtual void Set(const WideString& newString) = 0;
 
    virtual const WideString& Get() const = 0;

};

#endif // _IWideStringData_


