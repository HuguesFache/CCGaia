/*
//	File:	IEpsDocumentName.h
//
//  Author: Trias
//
//	Date:	27/07/2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef _IEpsDocumentName_
#define _IEpsDocumentName_


#include "PrsID.h"

class IPMUnknown;


/** IEpsDocumentName
*/
class IEpsDocumentName : public IPMUnknown
{
//	Data Types
public:
	enum { kDefaultIID = IID_IEPSDOCUMENTNAME };

//	Member functions
public:
	
	virtual PMString getEpsFileName() = 0;
	
	virtual PMString getEpsFileExt() = 0;
	
	virtual void setEpsFileName(PMString fileName) = 0;
	
	virtual void setEpsFileExt(PMString fileExt) = 0;


};

#endif // _IEpsDocumentName_