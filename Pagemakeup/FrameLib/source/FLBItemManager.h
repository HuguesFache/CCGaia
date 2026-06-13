/*
//	File:	FLBItemManager.h
//
//  Author: Trias
//
//	Date:	19-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __FLBItemManager_h__
#define __FLBItemManager_h__

// API includes
#include "CPMUnknown.h"

// Project includes
#include "IItemManager.h"
#include "GlobalDefs.h"

/** FLBItemManager
	Implements the IItemManager interface
*/
class FLBItemManager : public CPMUnknown<IItemManager> 
{

	public :
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		FLBItemManager(IPMUnknown * boss);

		/** 
			Destructor
		*/
		~FLBItemManager();

		// ---------------------
		//	Text frame creation
		// ---------------------
		virtual UIDRef CreateTextFrameCmd(IDataBase * db, PMPointList bounds, UID ownerSpreadLayer, int32 numColumns, PMReal gutterWidth, bool8 allowUndo);

		// ------------------------
		//  Coordinates management
		// ------------------------
		virtual UID GetPBCoordAbs(IDocument * doc, int32 numPage, PMPointList & pointsToTransform, IDocumentLayer * docLayer);
		virtual UID GetPBCoordRel(UIDRef spline, PMPointList & pointsToTransform, int8 alignment, IDocumentLayer * docLayer);
		virtual PMPoint GetAbsReferencePoint(UIDRef spline, int8 alignment);
		virtual PMPoint GetRelReferencePoint(UIDRef spline, int8 alignment);
		virtual bool8 CheckContainmentInOwnerSpread(IGeometry * itemGeo);

		// ----------------
		//  Item selection
		// ----------------
		virtual ErrorCode SelectPageItem(UIDRef pageItem);
		
		virtual	ErrorCode SelectPageItems(UIDList itemsToSelect);

		// --------------------------
		//  Graphic frame management
		// --------------------------
		virtual ErrorCode ImportImageCmd(IDataBase * db, UIDRef& imageItem, PMString picturePath, bool8 allowUndo);
		virtual ErrorCode PlaceItemInGraphicFrameCmd(UIDRef graphicFrame, UIDRef item, bool8 allowUndo);
		virtual ErrorCode ImportImageInGraphicFrameCmd(UIDRef graphicFrame, PMString picturePath, UIDRef& imageItem, bool8 allowUndo);
		virtual UID ResolveRGBColorSwatch (const PMString swatchName, const PMReal rRed, const PMReal rGreen, const PMReal rBlue);
		
		// --------------------------
		//  frame action
		// --------------------------
		virtual ErrorCode GroupPageItemsCmd(UIDList * itemsToGroup, UIDRef& groupItem, bool8 allowUndo) ;
		virtual ErrorCode LockPageItemCmd(UIDRef itemToSet, bool8 toLock, bool8 allowUndo) ;
		virtual ErrorCode DeletePageItemsCmd(UIDList itemsToDelete, bool8 allowUndo) ;

		virtual UID GetOwnerSpreadLayer(IDocument * doc, int32 numPage, IDocumentLayer * docLayer) ;

		// --------------------------
		//  Master Creation
		// --------------------------
		virtual void SetMasterInCreation(UIDRef masterSpread);
		virtual void EndMasterCreation();
	
	private :
		UIDRef masterInCreation;
};

#endif // __FLBItemManager_h__