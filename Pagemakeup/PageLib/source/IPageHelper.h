/*
//	File:	IPageHelper.h
//
//	Author:	Nicolas METAYE
//
//	Date:	28-Oct-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __IPageHelper_h__
#define __IPageHelper_h__

#include "PLBID.h"
#include "IDocument.h"

class IPMUnknown;
class ILayoutControlData;

/** IDocHelper
	Provides method to manage documents
*/
class IPageHelper : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_IPAGEHELPER };

		/** 
			Create a new page with a command
			@param IN OUT spreadToAddTo : spread which takes a new page
			@param IN numToInsert : number of pages that we insert
			@param IN posToInsertAt : position of the insertion
			@param IN masterUID : UID of the spread around the page
			Return ErrorCode 
		*/

		virtual ErrorCode CreateNewPageCmd(const UIDRef& spreadToAddTo,int32 numToInsert, int32 posToInsertAt, UIDRef masterUID, bool8 allowUndo = kTrue) =0;

		/** 
			Create a new page with a command and return a list which contains the pages created
			@param IN OUT spreadToAddTo : spread which takes a new page
			@param IN OUT pageList : return list
			@param IN numToInsert : number of pages that we insert
			@param IN posToInsertAt : position of the insertion
			@param IN masterUID : UID of the spread around the page
			Return ErrorCode 
		*/

		virtual ErrorCode CreateNewPageUIDListCmd(const UIDRef& spreadToAddTo,int32 numToInsert, int32 posToInsertAt, UIDRef masterUID, UIDList * pageList, bool8 allowUndo =kTrue) =0;

		/** 
			Create a new section with a command
			@param IN OUT sectionList : section's list of the document
			@param IN startPageUID : UID of the page where there will be the section
			@param IN startPageNumber : number of the section's start
			@param IN prefixe : prefixe of the section
			@param IN marker : marker of the section
			@param IN pageNumStyle : num's style of the section
			Return the ErrorCode
		*/
		virtual ErrorCode CreateNewSectionCmd(const UIDRef& sectionList, UID startPageUID,int32 startPageNumber, PMString prefixe, PMString marker, int32 pageNumStyle, bool16 autoNumber, bool16 includeSectionPrefix, bool8 allowUndo = kTrue) =0;
		virtual ErrorCode ModifSectionCmd(IDataBase *db, const UIDRef& sectionList, UID sectionID, UID startPageUID, int32 startPageNumber, PMString prefixe, PMString marker, int32 pageNumStyle, bool16 autoNumber, bool16 includeSectionPrefix, bool8 allowUndo = kTrue) = 0;


		/** 
			delete a page with a command
			@param IN OUT pageTotDelete : UIDRef of the page to delete
			Return the ErrorCode
		*/
		virtual ErrorCode DeletePageCmd(const UIDRef& pageToDelete, bool8 allowUndo = kTrue) =0;

		/** 
			select a page with a command
			@param IN spread : UIDRef of page's spread to select
			@param IN doc : UIDRef of doc
			@param IN layoutData :  
			@param IN bSet : 
			@param IN pageGeometry : geometry definition of the page
			Return the ErrorCode
		*/
		virtual ErrorCode SelectPageCmd(UIDRef spread, UIDRef doc, ILayoutControlData* layoutData, bool16 bSet, IGeometry* pageGeometry, bool8 allowUndo = kTrue) =0;
	
		/** 
			Apply a master spread to a range of pages
			@param IN doc : UIDRef of doc
			@param IN masterPrefix 
			@param IN masterName 
			@param IN startPage : start page index
			@param IN endPage : end page index
			Return the ErrorCode
		*/
		virtual ErrorCode ApplyMasterSpreadCmd(UIDRef doc, PMString masterPrefix, PMString masterName, int32 startPage, int32 endPage, bool8 allowUndo = kTrue) =0;
		
		/** 
			Create a new master spread
			@param IN doc : UIDRef of doc the master will be created in
			@param IN masterPrefix 
			@param IN masterName 
			@param IN basedOnPrefix 
			@param IN basedOn 
			@param IN nbPages
			@param OUT newMaster : new master UIDRef (kMasterPagesBoss)
			Return the ErrorCode
		*/
		virtual ErrorCode CreateNewMasterCmd(UIDRef doc, PMString masterPrefix, PMString masterName, PMString basedOnPrefix, PMString basedOn, int32 nbPages, UIDRef& newMaster, bool8 allowUndo = kTrue) =0;
};

#endif // __IPageHelper_h__
