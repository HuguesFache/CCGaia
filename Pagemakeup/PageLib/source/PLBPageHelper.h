/*
//	File:	PLBPageHelper.h
//
//	Author:	Nicolas METAYE
//
//	Date:	28-Oct-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __PLBPageHelper_h__
#define __PLBPageHelper_h__

// API includes
#include "CPMUnknown.h"

// Project includes
#include "IPageHelper.h"

/** PLBPageHelper
	Implements the IPageHelper interface.
*/
class PLBPageHelper : public CPMUnknown<IPageHelper> 
{

	public :
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PLBPageHelper(IPMUnknown * boss);

		/** 
			Destructor
		*/
		~PLBPageHelper();


		// -----------------------
		//	Page Creation Command
		// -----------------------
		virtual ErrorCode CreateNewPageCmd(const UIDRef& spreadToAddTo,int32 numToInsert, int32 posToInsertAt, UIDRef masterUID, bool8 allowUndo = kTrue) ;

		virtual ErrorCode CreateNewPageUIDListCmd(const UIDRef& spreadToAddTo,int32 numToInsert, int32 posToInsertAt, UIDRef masterUID, UIDList * pageList, bool8 allowUndo =kTrue) ;

		virtual ErrorCode CreateNewSectionCmd(const UIDRef& sectionList, UID startPageUID,int32 startPageNumber, PMString prefixe, PMString marker, int32 pageNumStyle, bool16 autoNumber, bool16 includeSectionPrefix, bool8 allowUndo = kTrue) ;
		virtual ErrorCode ModifSectionCmd(IDataBase *db, const UIDRef& sectionList, UID SectionID, UID startPageUID, int32 startPageNumber, PMString prefixe, PMString marker, int32 pageNumStyle, bool16 autoNumber, bool16 includeSectionPrefix, bool8 allowUndo = kTrue);

		virtual ErrorCode DeletePageCmd(const UIDRef& pageToDelete, bool8 allowUndo = kTrue) ;

		virtual ErrorCode SelectPageCmd(UIDRef spread, UIDRef doc, ILayoutControlData* layoutData, bool16 bSet, IGeometry* pageGeometry, bool8 allowUndo = kTrue) ;
		
		// -----------------------
		//	Master Pages Command
		// -----------------------
		virtual ErrorCode ApplyMasterSpreadCmd(UIDRef doc, PMString masterPrefix, PMString masterName, int32 startPage, int32 endPage, bool8 allowUndo = kTrue);

		virtual ErrorCode CreateNewMasterCmd(UIDRef doc, PMString masterPrefix, PMString masterName, PMString basedOnPrefix, PMString basedOn, int32 nbPages, UIDRef& newMaster, bool8 allowUndo = kTrue);
};

#endif // __DLBDocHelper_h__