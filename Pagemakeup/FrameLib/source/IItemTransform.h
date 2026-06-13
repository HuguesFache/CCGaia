/*
//	File:	IItemTransform.h
//
//  Author: Trias
//
//	Date:	19-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __IItemTransform_h__
#define __IItemTransform_h__

#include "FLBID.h"

#include "IStandOff.h"
#include "IStandOffContourWrapSettings.h"

class IPMUnknown;

/** IItemTransform
	Provides methods to transform page items
*/
class IItemTransform : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_IITEMTRANSFORM };

		enum ReferencePoint { kLeftTop = 0, kRightTop, kLeftBottom, kRightBottom, kCenter };


		/** 
			Rotate a spline item to the angle given in parameter
			@param IN spline : UIDRef of the spline item
			@param IN angle : angle of rotation
			@param IN refPoint : reference point (in Pasteboard coordinate) of the rotation
			@param IN absolute : if kTrue, rotate the item to angle degree, if kFalse, rotate the item angle degree
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode Rotate(UIDRef spline, PMReal angle, PMPoint refPoint, bool16 absolute = kTrue) =0; // refPoint is in inner coordinate

		/**
			Scale a spline item
			@param IN spline : UIDRef of the spline item
			@param IN xScale, yScale : percentage of the current width (xScale) and height (yScale)
			@param IN refPoint : reference point (in Pasteboard coordinate) of the scale
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode Scale(UIDRef spline, PMReal xScale, PMReal yScale, PMPoint refPoint) =0;

		/**
			Move a spline item with a command
			@param IN spline : UIDRef of the spline item
			@param IN newLeftTopCorner : new left top corner location (in Pasteboard coordinate) after the move command
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode MoveCmd(UIDRef spline, PMPoint newLeftTopCorner, bool8 allowUndo = kTrue) =0;

		/**
			Transform a spline item in inline inserted at the end of the story of another spline item
			which must be a text frame (use of a command)
			@param IN spline : UIDRef of the spline item transformed in inline
			@param IN receiver : UIDRef of the spline item which receive the inline, must be a text frame
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode MakeInlineCmd(UIDRef spline, UIDRef receiver, bool8 allowUndo = kTrue) =0;

		/**
			Set the standoffs of a spline item
			@param IN spline : UIDRef of the spline item
			@param IN mode : text wrap mode
			@param IN invert : if true, text wrap mode is inverted
			@params IN left, top, right, bottom : text wrap margins
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode SetStandoffCmd(UIDRef spline,IStandOff::mode mode, bool8 invert, PMReal left, PMReal top, PMReal right, PMReal bottom,
										 IStandOffContourWrapSettings::ContourWrapType contourType, bool8 allowUndo = kTrue) =0;

		/**
			Bring to front an item
			@param IN itemToMove : UIDRef of the item
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode BringToFrontCmd(UIDRef itemToMove,bool8 allowUndo = kTrue) =0;

		/**
			Send to back an item
			@param IN itemToMove : UIDRef of the item
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode SendToBackCmd(UIDRef itemToMove,bool8 allowUndo = kTrue) =0 ;

		
		/** 
			Skew a spline item to the angles (skew and rotation) given in parameter
			@param IN spline : UIDRef of the spline item
			@param IN skewAngle : angle of skew
			@param IN refPoint : reference point (in Pasteboard coordinate) of the rotation
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode Skew(UIDRef spline, PMReal skewAngle, PMPoint refPoint) =0 ;

		/** 
			Center a content in a graphic item
			@param IN content : UIDRef of the content
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode CenterContentCmd(UIDRef content,bool8 allowUndo = kTrue) =0;

		/** 
			Fit a content proportionnaly in a gaphic frame
			@param IN content : UIDRef of the content
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode FitContentPropCmd(UIDRef content,bool8 allowUndo = kTrue) =0 ;

		/** 
			Fit a content to a graphic frame
			@param IN content : UIDRef of the content
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode FitContentToFrameCmd(UIDRef content,bool8 allowUndo = kTrue) =0;
		
		/** 
			Resize a frame
			@param IN content : UIDRef of the spline
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode ResizeFrameCmd(UIDRef spline, PMReal horOffset, PMReal vertOffset, ReferencePoint refPt = kLeftTop, bool8 resizeItemAndChildren = kTrue, bool8 allowUndo = kTrue) =0;

		/** 
			Justify vertically a text frame
			@param IN frameRef :  UIDRef of the text frame to justify, 
			@param OUT justifiedFrame : UIDRef of last linked frame it was linked (it's the one which has been justified actually)
			@param IN justifyTop : TRUE if justication is applied to the frame's top, FALSE if applied to the frame's bottom
			@param IN allowOutOfSpread : TRUE if JV command not blocking when an item get out of spread after resizing, FALSE otherwise
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode JustifyFrameVertically(UIDRef frameRef, UIDRef * justifiedFrame = nil, bool8 justifyTop = kFalse, bool8 allowOutOfSpread = kFalse, bool8 allowUndo = kTrue) =0;

		/** 
			Justify an image frame so that the image is fully visible
			@param IN frameRef :  UIDRef of the image frame to justify, 
			
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode JustifyImageFrame(UIDRef frameRef, bool8 justifyHorizontaly, bool8 justifyVerticaly, PMReal maxHor, PMReal maxVer, bool8 leftJustify, bool8 topJustify, bool8 allowUndo = kTrue) =0;
	
};

#endif // __IItemTransform_h__
