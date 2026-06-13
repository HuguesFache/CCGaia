/*
//	File:	ParserPlugIn.h
//
//  Author: Trias
//
//	Date:	2-July-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __ParserPlugIn_h__
#define __ParserPlugIn_h__


#include "PlugIn.h"

class IDocument;

/** ParserPlugIn
	Derive InDesign IPlugIn implementation (class PlugIn)
	to override FixUpData method to purge the parser item list
	if it was modified without the PageMakeUp PlugIn
*/
class ParserPlugIn : public PlugIn
{
	public :
	
		/** Destructor
		*/
		~ParserPlugIn();


		/**
			Called by the application on document opening
			Update the item list stocked in the document by PageMakeUp,
			delete in this list the items that have been deleted in the document
			@param IN doc : document to check
		*/
		void FixUpData(IDocument* doc);
};

#endif // __ParserPlugIn_h__