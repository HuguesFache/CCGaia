/*
//	File:	IXPageSuite.h
//
//  Author: Trias Developpement
//
//	Date:	23/07/2006
//
*/

#ifndef _IXPageSuite_
#define _IXPageSuite_


#include "XPGID.h"

class IPMUnknown;


/** IXPageSuite
*/
class IXPageSuite : public IPMUnknown
{
public:
//	Data Types
	enum { kDefaultIID = IID_IXPAGESUITE };

//	Member functions
public:
	/**
		Get story reference of selected text frame, if any
		@param OUT txtFrameRef : UIDRef of selected text frame
	*/
	virtual void GetTextFrameStoryRef(UIDRef& txtFrameStoryRef) = 0;
	
	/**
		Get selected text frame, if any
		@param OUT txtFrameRef : UIDRef of selected text frame
	*/
	virtual void GetTextFrameRef(UIDRef& txtFrameRef) = 0;

	/** Get selected items (breaking down between text frames and graphic frames)
	*/
	virtual void GetSelectedItems(UIDList& textFrames, UIDList& graphicFrames, UIDList& selectedItems) =0;

	/** Get selected forme, if all selected items belongs to one and only one forme)
		Then forme items are broken down between text frames and graphic frames
 	*/
	virtual void GetSelectedForme(PMString& idForme, UIDList& textFrames, UIDList& graphicFrames, UIDList& formeItems) =0;
};

#endif // _IXPageSuite_


