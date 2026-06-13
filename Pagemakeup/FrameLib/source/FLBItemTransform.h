/*
//	File:	FLBItemTransform.h
//
//  Author: Trias
//
//	Date:	19-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __FLBItemTransform_h__
#define __FLBItemTransform_h__

// API includes
#include "CPMUnknown.h"

// Project includes
#include "IItemTransform.h"

/** FLBItemTransform
	Implements the IItemTransform interface
*/
class FLBItemTransform : public CPMUnknown<IItemTransform> 
{

	public :
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		FLBItemTransform(IPMUnknown * boss);

		/** 
			Destructor
		*/
		~FLBItemTransform();
		
		// ----------------------------
		//  Spline item transformation
		// ----------------------------
		virtual ErrorCode Rotate(UIDRef spline, PMReal angle, PMPoint refPoint, bool16 absolute = kTrue); // refPoint is in inner coordinate
		virtual ErrorCode Scale(UIDRef spline, PMReal xScale, PMReal yScale, PMPoint refPoint);
		virtual ErrorCode MoveCmd(UIDRef spline, PMPoint newLeftTopCorner, bool8 allowUndo);
		virtual ErrorCode Skew(UIDRef spline, PMReal skewAngle, PMPoint refPoint);
		virtual ErrorCode ResizeFrameCmd(UIDRef spline, PMReal horOffset, PMReal vertOffset, ReferencePoint refPt = kLeftTop, bool8 resizeItemAndChildren = kTrue, bool8 allowUndo = kTrue);

		// ---------
		//  Inlines
		// ---------
		virtual ErrorCode MakeInlineCmd(UIDRef spline, UIDRef receiver, bool8 allowUndo);

		// -----------
		//  Standoffs
		// -----------
		virtual ErrorCode SetStandoffCmd(UIDRef spline, IStandOff::mode mode, bool8 invert, PMReal left, PMReal top, PMReal right, PMReal bottom,
										 IStandOffContourWrapSettings::ContourWrapType contourType, bool8 allowUndo);

		//-------------
		//Front & Back
		//-------------
		virtual ErrorCode BringToFrontCmd(UIDRef itemToMove,bool8 allowUndo = kTrue);
		virtual ErrorCode SendToBackCmd(UIDRef itemToMove,bool8 allowUndo = kTrue);

		//------------------------------------
		//fit and center a content in a frame
		//------------------------------------
		virtual ErrorCode CenterContentCmd(UIDRef content,bool8 allowUndo = kTrue) ;
		virtual ErrorCode FitContentPropCmd(UIDRef content,bool8 allowUndo = kTrue) ;
		virtual ErrorCode FitContentToFrameCmd(UIDRef content,bool8 allowUndo = kTrue) ;

		//------------------------------------
		//justify frame
		//------------------------------------
		virtual ErrorCode JustifyFrameVertically(UIDRef frameRef, UIDRef * justifiedFrame, bool8 justifyTop, bool8 allowOutOfSpread, bool8 allowUndo = kTrue);
		virtual ErrorCode JustifyImageFrame(UIDRef frameRef, bool8 justifyHorizontaly, bool8 justifyVerticaly, PMReal maxHor, PMReal maxVer, bool8 leftJustify, bool8 topJustify, bool8 allowUndo = kTrue);
};

#endif // __FLBItemTransform_h__