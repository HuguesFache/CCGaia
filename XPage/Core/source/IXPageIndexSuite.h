/*
//	File:	IXPageIndexSuite.h
//
//  Author: Trias Developpement
//
//	Date:	23/07/2006
//
*/

#ifndef _IXPageIndexSuite_
#define _IXPageIndexSuite_


#include "XPGID.h"

class IPMUnknown;


/** IXPageIndexSuite
*/
class IXPageIndexSuite : public IPMUnknown
{
public:
//	Data Types
	enum { kDefaultIID = IID_IXPAGEINDEXSUITE };

//	Member functions
public:
	/**
		Get selected text
		@param OUT txt : selected text, if any
        @param OUT multiLineSelection : TRUE if text selection includes more than one paragraph
	*/
	virtual void GetSelectedText(WideString& txt, UIDRef& txtModel, bool16& multiLineSelection, TextIndex& start, TextIndex& end) = 0;
	
};

#endif // _IXPageIndexSuite_


