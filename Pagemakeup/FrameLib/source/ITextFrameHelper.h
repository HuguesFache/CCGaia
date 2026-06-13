/*
//	File:	ITextFrameHelper.h
//
//  Author: Trias
//
//	Date:	19-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __ITextFrameHelper_h__
#define __ITextFrameHelper_h__

#include "FLBID.h"

class IPMUnknown;
class ITextModel;
class IMultiColumnTextFrame;

/** ITextFrameHelper
	Provides methods to handle text frame
*/
class ITextFrameHelper : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_ITEXTFRAMEHELPER };

		/**
			Recompose all the frames of a story
			@param IN textFrame : one of the frames  that hold a part of the story
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode RecomposeWholeStory(IMultiColumnTextFrame * textFrame) =0;

		/** 
			Link two frames (two stories become a single story) with a command
			@param IN firstMultiCol : UIDRef of the first text frame (UIDRef of a kMultiColumnItemBoss)
			@param IN secondMultiCol : UIDRef of the second text frame that will be linked after the first,
									   (UIDRef of a kMultiColumnItemBoss)
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode LinkFramesCmd(UIDRef firstMultiCol, UIDRef secondMultiCol, bool8 allowUndo = kTrue) =0;

		/** 
			Unlink two frame with a command
			@param IN firstMultiCol : UIDRef of the first text frame (UIDRef of a kMultiColumnItemBoss)
			@param IN secondMultiCol : UIDRef of the second text frame that will be unlinked from the first,
									   (UIDRef of a kMultiColumnItemBoss)
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode UnLinkFramesCmd(UIDRef multiCol, bool8 allowUndo = kTrue) =0;

		/** 
			Insert text with a command at the end of the story held (in pat or not) by a text frame 
			@param IN multiCol : UIDRef of the text frame that hold the story (must be a kMultiColumnItemBoss)
			@param text : text to insert
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode InsertTextAtEndCmd(UIDRef multiCol, WideString text, bool8 allowUndo = kTrue) =0;

		/** 
			Set the insets of a text frame
			@param IN spline : UIDRef (kSplineItemBoss) of the text frame
			@param IN top, right, left, bottom : insets
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode SetInset(UIDRef spline, PMReal left, PMReal top, PMReal right, PMReal bottom) =0;

		/** 
			Set the insets of a text frame with a command
			@param IN spline : UIDRef (kSplineItemBoss) of the text frame
			@param IN top, right, left, bottom : insets
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/	
		virtual ErrorCode SetInsetCmd(UIDRef spline, PMReal left, PMReal top, PMReal right, PMReal bottom,bool8 allowUndo = kTrue) =0;

		/** 
			Apply a paragraph style in a bloc
			@param IN model : text model to apply the style to
			@param IN insertPos : text index where the style is applied
			@param IN paraStyleUID : UID of the style
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual	ErrorCode ApplyParaStyleCmd(ITextModel * model, TextIndex insertPos, UID paraStyleUID, bool8 allowUndo=kTrue) =0 ;

		/** 
			Apply a character style in a bloc
			@param IN model : text model to apply the style to
			@param IN insertPos : text index where the style is applied
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode ApplyCharStyleCmd(ITextModel * model, TextIndex insertPos, UID charStyleUID, bool8 allowUndo=kTrue)=0 ;
		
		/** 
			Apply a paragraph attribute boss list in a bloc
			@param IN model : text model to apply the attributes to
			@param IN insertPos : text index where the attributes are applied
			@param IN attrList : attribute list
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode ApplyAttributeParaCmd(ITextModel * model, TextIndex insertPos, AttributeBossList * attrList, bool8 allowUndo=kTrue)=0 ;
		
		/** 
			Apply a character attribute boss list in a bloc
			@param IN model : text model to apply the attributes to
			@param IN insertPos : text index where the attributes are applied
			@param IN attrList : attribute list
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed
		*/
		virtual ErrorCode ApplyAttributeCharCmd(ITextModel * model, TextIndex insertPos, AttributeBossList * attrList, bool8 allowUndo=kTrue)=0 ;

		// -------------
		//  Accessors
		// -------------
		virtual AttributeBossList GetAttrList() =0;
		virtual void SetAttrList(AttributeBossList AttrBossList) =0;

		virtual int32 GetNextStyleNum() =0;
		virtual void SetNextStyleNum(int32 numStyle) =0;

		virtual int32 GetBasedOnNum() =0;
		virtual void SetBasedOnNum(int32 numStyle) =0;

		virtual PMString GetStyleName() =0;
		virtual void SetStyleName(PMString StyleName) =0;
		virtual bool8 IsParaStyle() =0;
		virtual void SetIsParaStyle(bool8 isParaStyle) =0;

		virtual void SetCreation(bool8 creation) =0;
		virtual bool8 GetCreation() =0;

		virtual void Clean() =0;

		virtual AttributeBossList * GetAttrListPtr() =0 ;

};

#endif // __ITextFrameHelper_h__
