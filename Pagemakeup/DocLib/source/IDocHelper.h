/*
//	File:	IDocHelper.h
//
//  Author: Trias
//
//	Date:	18-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __IDocHelper_h__
#define __IDocHelper_h__

#include "DLBID.h"
#include "IColumns.h"

class IPMUnknown;

/** IDocHelper
	Provides method to manage documents
*/
class IDocHelper : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_IDOCHELPER };

		/** 
			Create a new document with a command
			@param IN pageSize : page dimension
			@param IN numPages : numbers of pages in the document
			@param IN numPagesPerSpread : number of pages per spread
			@params IN left, top, right, bottom : margins values
			@param IN allowUndo : command undoability
			Return the UIDRef of the document created or a null ref 
			(UIDRef(nil,kInvalidUID)) if function failed
		*/
		virtual UIDRef CreateNewDocCmd(PMRect pageSize,
									   int32 numPages, int32 numPagesPerSpread,
									   int32 numColumns = 0, PMReal gutter = 0.0, bool8 direction = kFalse,
									   PMReal left = 0.0, PMReal top = 0.0, PMReal right= 0.0, PMReal bottom =0.0, bool8 bWide =kTrue,
									    bool8 allowUndo = kTrue
									  ) =0;

		/** 
			Set the name of a document in the window(s) opened
			@param IN doc : UIDRef of the document
			@param IN numPages : name displayed in the windows
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode SetDocName(UIDRef doc, PMString name) =0;

		/** 
			Set the name of a document in the window(s) opened with a command
			@param IN doc : UIDRef of the document
			@param IN numPages : name displayed in the windows
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode SetDocNameCmd(UIDRef doc, PMString name, bool8 allowUndo = kTrue) =0;

		/** 
			Set the pages' size of a document 
			@param IN doc : UIDRef of the document
			@param IN newPageSize : new pages' size
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode SetPageSizeCmd(UIDRef doc, PMRect newPageSize, bool16 bWide, bool8 allowUndo = kTrue) =0;

		/** 
			Set the margin of a document 
			@param IN doc : UIDRef of the document
			@param IN mg,mh,md,md : margin values
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode SetMarginCmd(UIDList& pageList, PMReal mg,PMReal mh,PMReal md,PMReal mb, bool8 allowUndo =kTrue) =0 ;

		/** 
			Set the margin of a document 
			@param IN doc : UIDRef of the document
			@param IN numColumns : number of columns
			@param IN gutter : gutter value
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode SetColumnCmd(UIDList& pageList,PMRealList& columns, bool8 allowUndo =kTrue) =0 ;

		/** 
			Open a window on a document with a command
			@param IN doc : UIDRef of the document
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode OpenWindowCmd(UIDRef doc, bool8 allowUndo = kTrue) =0;

		/** 
			Close the frontmost window opened on a document with a command
			@param IN doc : UIDRef of the document
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode CloseWindowCmd(UIDRef doc, bool8 allowUndo = kTrue) =0;

		/** 
			Save a document, if unsaved, open a save file dialog if UIFlags allows it
			@param IN doc : UIDRef of the document
			@param IN UIFlags : flag that allow / forbid the save file dialog opening
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode SaveDoc(UIDRef doc, UIFlags flags) =0;

		/** 
			Save a document using a predefined path, 
			if no predefined path, open a save file dialog if UIFlags allows it
			@param IN doc : UIDRef of the document
			@param IN absoluteName : document file path
			@param IN UIFlags : flag that allow / forbid the save file dialog opening
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode SaveAsDoc(UIDRef doc, PMString absoluteName) =0; 

		//conversion of path
		virtual ErrorCode ConvertPath(PMString &path, IDFile& file) =0;
		
};

#endif // __IDocHelper_h__
