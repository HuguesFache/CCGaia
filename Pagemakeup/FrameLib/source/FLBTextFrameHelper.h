/*
//	File:	FLBTextFrameHelper.h
//
//  Author: Trias
//
//	Date:	19-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __FLBTextFrameHelper_h__
#define __FLBTextFrameHelper_h__

// API includes
#include "CPMUnknown.h"

// Project includes
#include "ITextFrameHelper.h"

/** FLBTextFrameHelper
	Implements the ITextFrameHelper interface
*/
class FLBTextFrameHelper : public CPMUnknown<ITextFrameHelper> 
{

	public :
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		FLBTextFrameHelper(IPMUnknown * boss);

		/** 
			Destructor
		*/
		~FLBTextFrameHelper();

		// ---------------------
	    //  Story recomposition
		// ---------------------
		virtual ErrorCode RecomposeWholeStory(IMultiColumnTextFrame * textFrame);

		// --------------------------- 
		//  Frame linking / unlinking
		// ---------------------------
		virtual ErrorCode LinkFramesCmd(UIDRef firstMultiCol, UIDRef secondMultiCol, bool8 allowUndo);
		virtual ErrorCode UnLinkFramesCmd(UIDRef multiCol, bool8 allowUndo);

		// ----------------
		//  Text insertion
		// ----------------
		virtual ErrorCode InsertTextAtEndCmd(UIDRef multiCol, WideString text, bool8 allowUndo);

		// -------------
		//  Text Insets
		// -------------
		virtual ErrorCode SetInset(UIDRef spline, PMReal left, PMReal top, PMReal right, PMReal bottom);
		virtual ErrorCode SetInsetCmd(UIDRef spline, PMReal left, PMReal top, PMReal right, PMReal bottom,bool8 allowUndo);

		//Application style methods
		virtual	ErrorCode ApplyParaStyleCmd(ITextModel * model, TextIndex insertPos, UID paraStyleUID, bool8 allowUndo) ;
		virtual ErrorCode ApplyCharStyleCmd(ITextModel * model, TextIndex insertPos, UID charStyleUID, bool8 allowUndo);
		virtual ErrorCode ApplyAttributeParaCmd(ITextModel * model, TextIndex insertPos, AttributeBossList * attrList, bool8 allowUndo);
		virtual ErrorCode ApplyAttributeCharCmd(ITextModel * model, TextIndex insertPos, AttributeBossList * attrList, bool8 allowUndo);


		// -------------
		//  Accessors
		// -------------
		virtual AttributeBossList GetAttrList();
		virtual void SetAttrList(AttributeBossList AttrBossList);

		virtual int32 GetNextStyleNum();
		virtual void SetNextStyleNum(int32 numStyle);

		virtual int32 GetBasedOnNum();
		virtual void SetBasedOnNum(int32 numStyle);

		virtual PMString GetStyleName() ;
		virtual void SetStyleName(PMString StyleName) ;

		virtual bool8 IsParaStyle() ;
		virtual void SetIsParaStyle(bool8 isParaStyle);

		virtual void SetCreation(bool8 creation);
		virtual bool8 GetCreation();

		virtual void Clean();

		virtual AttributeBossList * GetAttrListPtr() ;

private :

	int32 basedOn, nextStyle ; // self explanatory
	
	AttributeBossList * attrList ; //Attribute list - used to create a style and to apply style's attributes of character to the buffer

	PMString styleName ; //Style name used to create a style

	bool8 paraStyleBool; //booleen used to create a style - indicates if the style is a paragraph style or a character style

	bool8 inCreation;

};

#endif // __FLBTextFrameHelper_h__