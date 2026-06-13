/*
//	File:	IStyleHelper.h
//
//  Author: Wilfried LEFEVRE
//
//	Date:	18-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __IStyleHelper_h__
#define __IStyleHelper_h__

#include "TLBID.h"

class IPMUnknown;

/** IStyleHelper
	Provides methods to handle styles
*/
class IStyleHelper : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_ISTYLEHELPER };

		/**
			Create a new paragraph style with a command for a workspace (a document workspace or the global
			workspace)
			@param IN name : name of the paragraph style
			@param IN workspace : workspace UIDRef in which the style will be available
			@param IN overrides : list of text attributes overrides
			@param IN basedOn : paragraph style on which the new one will be based
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode CreateParaStyleCmd(PMString name, UIDRef workspace, const AttributeBossList * overrides, UID basedOn = kInvalidUID, UID nextStyle = kInvalidUID, bool8 allowUndo = kTrue) =0;

		/** 
			Set a default text style applied to a workspace
			@param IN workspace : workspace UIDRef in which the style will be available
			@param IN strand : paragraph strand for a paragraph style, character strand for character style
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode DefaultTextStyleCmd(UIDRef workspace, ClassID strand, UID style, bool8 allowUndo = kTrue) =0;
		
		/**
			Create a new paragraph style with a command for a workspace (a document workspace or the global
			workspace)
			@param IN name : name of the character style
			@param IN workspace : workspace UIDRef in which the style will be available
			@param IN overrides : list of text attributes overrides
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode CreateCharStyleCmd(PMString name, UIDRef workspace,const AttributeBossList * overrides, UID basedOn = kInvalidUID, bool8 allowUndo = kTrue) =0 ;
		
		/**
			Edit a style with some attributes
			workspace)
			@param IN styleToEdit : UID of the style to modify
			@param IN workspace : workspace UIDRef in which the style is available
			@param IN overrides : list of text attributes overrides
			@param IN allowUndo : command undoability
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode EditTextStyleAttributesCmd(UIDRef workspace, UID styleToEdit, const AttributeBossList * attributes, bool8 allowUndo = kTrue) =0; 
};

#endif // __IStyleHelper_h__
