/*
//	File:	IItemManager.h
//
//  Author: Trias
//
//	Date:	19-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __IItemManager_h__
#define __IItemManager_h__

#include "FLBID.h"

#include "PMPoint.h"

class IPMUnknown;
class IDataBase;
class IDocument;
class IDocumentLayer;
class IGeometry;

/** IItemManager
	Provides methods to create frames, rules, to select them,
	to convert coordinate, etc...
*/
class IItemManager : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_IITEMMANAGER };

		/**
			Create a text frame
			@param IN db : database in which the text frame will be created
			@param IN bounds : point list that contains the left top corner and the
							   right bottom corner of the new text frame, in pasteboard coordinate
			@param IN ownerSpreadLayer : UID of the spread on which the text frame will lie
			@param IN numColumns : number of columns
			@param IN gutterWidth : width of the gutter between columns
			@param IN allowUndo : command undoability
			Return the UIDRef of the new text frame if succeeded, else a null UIDRef
		*/
		virtual UIDRef CreateTextFrameCmd(IDataBase * db, PMPointList bounds, UID ownerSpreadLayer, int32 numColumns, PMReal gutterWidth, bool8 allowUndo = kTrue) =0;

		/**	
			Transform a list of point in page coordinates system to pasteboard coordinates
			@param IN doc : the document that contains the page
			@param IN numPages : page index
			@param IN OUT pointsToTransform : list of points in the page coordinates system, which will be
											  converted in pasteboard coordinates system
			@param IN docLayer : the document layer that will own the page item described by the previous points
			Return the UID of the spread layer on which these points will lie
		*/
		virtual UID GetPBCoordAbs(IDocument * doc, int32 numPage, PMPointList & pointsToTransform, IDocumentLayer * docLayer) =0;

		/**
			Transform a list of point in a spline item coordinates to pasteboard coordinates
			@param IN spline : UIDRef of the spline item
			@param IN OUT pointsToTransform : list of points in the spline item coordinates system, returned in the pasteboard
							coordinates system
			@param IN alignment : alignment type -> reference point of the spline item coordinates system
			@param IN docLayer : the document layer that will own the page item described by the previous points
			Return the UID of the spread layer on which these points will lie
		*/
		virtual UID GetPBCoordRel(UIDRef spline, PMPointList & pointsToTransform, int8 alignment, IDocumentLayer * docLayer) =0;

		/**
			Return a reference point from a spline item (LeftTop, LeftBottom, or text caret) in absolute coordinates
			@param IN spline : UIDRef of the spline item
			@param IN alignment : alignment type -> reference point of the spline item coordinates system
			Return the reference point in absolute coordinates
		*/
		virtual PMPoint GetAbsReferencePoint(UIDRef spline, int8 alignment) =0;

		/**
			Return a reference point from a spline item (LeftTop, LeftBottom, or text caret) in inner coordinates
			@param IN spline : UIDRef of the spline item
			@param IN alignment : alignment type -> reference point of the spline item coordinates system
			Return the reference point in inner coordinates
		*/
		virtual PMPoint GetRelReferencePoint(UIDRef spline, int8 alignment) =0;

		/** 
			Check if a spline item entirely lie in its owner spread
			@param IN itemGeo : the geometry of the spline item to check
			Return kTrue if the frame entirely lie, kFalse else
		*/
		virtual bool8 CheckContainmentInOwnerSpread(IGeometry * itemGeo) =0;

		/**
			Select a page item with a command
			@param IN pageItem : UIDRef of the page item
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode SelectPageItem(UIDRef pageItem) =0;

		virtual ErrorCode SelectPageItems(UIDList itemsToSelect) = 0 ;

		/**
			Import an image in a kImageItem with a command
			@param IN db : database of the image item
			@param OUT imageItem : UIDRef of the image item the image will be imported in
			@param IN picturePath : absolute file path of the image to import
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode ImportImageCmd(IDataBase * db, UIDRef& imageItem, PMString picturePath, bool8 allowUndo = kTrue) =0;

		/**
			Place a page item into a spline item (that shouldn't be a text frame) with a command
			@param IN graphic frame : UIDRef of the spline item
			@param IN item : UIDRef of the page item
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode PlaceItemInGraphicFrameCmd(UIDRef graphicFrame, UIDRef item, bool8 allowUndo = kTrue) =0;

		/**
			Execute the three previous functions to create a kImageItem, import an image into it and then place
			in a spline item (a graphic frame)
			@param IN graphicFrame : UIDRef of the spline item
			@param IN picturePath : absolute file path of the image to import
			@param OUT imageItem : UIDRef of the image item created
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode ImportImageInGraphicFrameCmd(UIDRef graphicFrame, PMString picturePath, UIDRef& imageItem, bool8 allowUndo = kTrue) =0;


		/**
			Finds or creates a color in the swatch list, returning a UID to the color.		
			@param OUT colorUID : UID of the color		
		*/
		virtual UID ResolveRGBColorSwatch (const PMString swatchName, const PMReal rRed, const PMReal rGreen, const PMReal rBlue) =0;

		/**
			Group the items placed in the UIDList itemsToGroup after having to check if the grouping were possible
			@param IN OUT (pointer) UIDList : UIDList of the items to group
			@param OUT groupItem : UIDRef of the group item which was created
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode GroupPageItemsCmd(UIDList * itemsToGroup, UIDRef& groupItem, bool8 allowUndo = kTrue) = 0 ;

		/**
			Lock the item 
			@param IN itemToSet : UIDRef of the page item
			@param IN toLock : boolean indicating the action to do (lock/unlock )
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode LockPageItemCmd(UIDRef itemToSet, bool8 toLock, bool8 allowUndo = kTrue) = 0 ;

		/**
			delete the items places in the UIDList itemsToDelete 
			@param IN itemsToDelete : UIDList of the page item
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode DeletePageItemsCmd(UIDList itemsToDelete, bool8 allowUndo = kTrue) = 0 ;

		/**	
			return the spreadLayerof the front document
			@param IN doc : the document that contains the page
			@param IN numPages : page index
			@param IN docLayer : the document layer 
			Return the UID of the spread layer on which these points will lie
		*/
		virtual UID GetOwnerSpreadLayer(IDocument * doc, int32 numPage, IDocumentLayer * docLayer) =0 ;

		
		/**	
			@param IN masterSpread : UIDRef of the masterSpread the new item will be added in
		*/
		virtual void SetMasterInCreation(UIDRef masterSpread) =0;

		/**
			End up the creation of a master (this means all new item will be created in the document)
		*/
		virtual void EndMasterCreation() =0;

};

#endif // __IItemManager_h__
