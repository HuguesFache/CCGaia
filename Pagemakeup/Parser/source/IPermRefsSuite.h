/*
//	File:	IPermRefsSuite.h
//
//  Author: Trias
//
//	Date:	27/10/2005
//
*/

#ifndef _IPermRefsSuite_
#define _IPermRefsSuite_


#include "PrsID.h"

class IPMUnknown;


/** IPermRefsSuite
*/
class IPermRefsSuite : public IPMUnknown
{
//	Data Types
public:
	enum { kDefaultIID = IID_IPERMREFSSUITE };

//	Member functions
public:
	/**
		Get range of selected text
		@param OUT txtModel : text model of the selection
		@param OUT start : start of the selection
		@param OUT end : end of the selection
	*/
	virtual void GetTextSelection(UIDRef& txtModel, TextIndex& start, TextIndex& end) = 0;
};

#endif // _IPermRefsSuite_


