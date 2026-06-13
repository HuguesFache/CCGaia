/*
//	File:	IPermRefs.h
//
//  Author: Trias
//
//	Date:	14/09/2205
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef _IPermRefsTag_
#define _IPermRefsTag_


#include "PrsID.h"
#include "ITCLParser.h"

class IPMUnknown;


/** IPermRefsTag
*/
class IPermRefsTag : public IPMUnknown
{
//	Data Types
public:
	enum { kDefaultIID = IID_IPERMREFSTAG };

//	Member functions
public:
	
	// Getter and setter Reference
	virtual const PermRefStruct& getReference() = 0;	
	virtual void setReference(const PermRefStruct& reference) = 0;
};

#endif // _IPermRefsTag_