/*
//	File:	DLBDocHelper.h
//
//  Author: Trias
//
//	Date:	18-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __DLBDocHelper_h__
#define __DLBDocHelper_h__

// API includes
#include "CPMUnknown.h"

// Project includes
#include "IDocHelper.h"

/** DLBDocHelper
	Implements the IDocHelper interface.
*/
class DLBDocHelper : public CPMUnknown<IDocHelper> 
{

	public :
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		DLBDocHelper(IPMUnknown * boss);

		/** 
			Destructor
		*/
		~DLBDocHelper();

		// ------------------
		//	Document opening
		// ------------------
		virtual UIDRef CreateNewDocCmd(PMRect pageSize,
									   int32 numPages, int32 numPagesPerSpread,
									   int32 numColumns, PMReal gutter, bool8 direction,
									   PMReal left, PMReal top, PMReal right, PMReal bottom, bool8 bWide,
									   bool8 allowUndo
									  );

		// -----------------------
		//	Document name setting
		// -----------------------
		virtual ErrorCode SetDocName(UIDRef doc, PMString name);
		virtual ErrorCode SetDocNameCmd(UIDRef doc, PMString name, bool8 allowUndo);
		virtual ErrorCode SetPageSizeCmd(UIDRef doc, PMRect newPageSize, bool16 bWide, bool8 allowUndo);
		virtual ErrorCode SetMarginCmd(UIDList& pageList, PMReal mg,PMReal mh,PMReal md,PMReal mb, bool8 allowUndo) ;
		virtual ErrorCode SetColumnCmd(UIDList& pageList,PMRealList& columns, bool8 allowUndo) ;
		
		// ----------------------
		//	Windows open / close
		// ----------------------
		virtual ErrorCode OpenWindowCmd(UIDRef doc, bool8 allowUndo);
		virtual ErrorCode CloseWindowCmd(UIDRef doc, bool8 allowUndo);

		// -----------------
		//	Document saving
		// -----------------
		virtual ErrorCode SaveDoc(UIDRef doc, UIFlags flags);
		virtual ErrorCode SaveAsDoc(UIDRef doc, PMString absoluteName); 

		//conversion of path
		virtual ErrorCode ConvertPath(PMString &path, IDFile& file) ;
};

#endif // __DLBDocHelper_h__