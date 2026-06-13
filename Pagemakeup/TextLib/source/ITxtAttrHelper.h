/*
//	File:	ITxtAttrHelper.h
//
//  Author: Wilfried LEFEVRE
//
//	Date:	18-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __ITxtAttrHelper_h__
#define __ITxtAttrHelper_h__

#include "TLBID.h"
#include "TabStop.h"
#include "LanguageID.h"

class IPMUnknown;
class IDocument;

/** ITxtAttrHelper
	Provides methods to create text attributes
*/
class ITxtAttrHelper : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_ITXTATTRHELPER };

		/**
			Add an autoleading mode attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN value : autoleading mode -> -1 to set autoleading on
						      else a positive value which will be the leading in points by default
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddAutoLeadModeAttribute(AttributeBossList * attrList, PMReal value) =0;

		/**
			Add a percentage for autoleading attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN value : percentage for autoleading
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddAutoLeadAttribute(AttributeBossList * attrList, PMReal percent) =0;

		/**
			Add a type of kerning attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN value : ? for automatic mode
							  ? for manual mode
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddPairKernAttribute(AttributeBossList * attrList, PMReal value) =0;

		/**
			Add a language attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN doc : document in which languages exist or must be created if they don't
			@param IN lang : language ID
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddLanguageAttribute(AttributeBossList * attrList, IDocument * doc, LanguageID lang) =0;

		/**
			Add a color attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN colorUID : UID of the color
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddColorAttribute(AttributeBossList * attrList, UID colorUID) =0;

		/**
			Add a font size attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN fontSize : value of the font size 
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddFontSizeAttribute(AttributeBossList * attrList, PMReal fontSize) =0;

		/**
			Add a OverPrint Fill attribute
			@param IN OUT attrList : attributes list
			@param IN OverPrint : Boolean Wich Indacte the OverPrint Fill attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddOverPrintFillAttribute(AttributeBossList * attrList, bool8 overPrint) =0;

		/**
			Add a font size attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN fontStyle : value of the font style 
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddFontStyleAttribute(AttributeBossList * attrList, int16 fontStyle) =0;

		/**
			Add a percentage for changing the XScale attribute
			@param IN OUT attrList : attributes list
			@param IN value : percentage 
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddXScaleAttribute(AttributeBossList * attrList, PMReal percent) =0 ; 


		/**
			Add a percentage for changing the YScale attribute
			@param IN OUT attrList : attributes list
			@param IN value : percentage 
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddYScaleAttribute(AttributeBossList * attrList, PMReal percent) =0 ; 

		/**
			Add a TrackKern attribute
			@param IN OUT attrList : attributes list
			@param IN value : value of the TrackKern attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddTrackKernAttribute(AttributeBossList * attrList, PMReal value) =0 ;

		/**
			Add a BLShift attribute
			@param IN OUT attrList : attributes list
			@param IN value : value of the BLShift attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddBLShiftAttribute(AttributeBossList * attrList, PMReal value) =0;

		/**
			Add a Tint attribute
			@param IN OUT attrList : attributes list
			@param IN percent : percentage of Tint attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddTintAttribute(AttributeBossList * attrList, PMReal percent) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN colorUID : UID of the font
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddFontAttribute(AttributeBossList * attrList, UID fontUID) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN fontStyle : Value of the font style
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddFontStyleAttribute(AttributeBossList * attrList, PMString fontStyle) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN underline : boolean which indicates if we have to set the underline mode or not
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddUnderlineAttribute(AttributeBossList * attrList, bool8 underline) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN strikeThru : value of the strikeThru attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddStrikeThruAttribute(AttributeBossList * attrList, bool8 strikeThru) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN capitalMode : value of capitalMode attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddCapitalAttribute(AttributeBossList * attrList, int32 capitalMode) =0;
		
		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN capitalMode : value of positionMode attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddPositionAttribute(AttributeBossList * attrList, int32 positionMode) =0;
		/**
			Add drop cap attributes (number of lines and chars) to an attributes list
			@param IN OUT attrList : attributes list
			@param IN numberOfChars : drop cap chars
			@param IN numberOfLines : drop cap lines
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddDropCapAttributes(AttributeBossList * attrList, int16 numberOfChars, int16 numberOfLines) =0;


		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN noBreak : value of NoBreak attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddNoBreakAttribute(AttributeBossList * attrList, bool8 noBreak) =0;


		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN noBreak : value of spaceBefore attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddSpaceBeforeAttribute(AttributeBossList * attrList, PMReal spaceBefore) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN noBreak : value of spaceAfter attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddSpaceAfterAttribute(AttributeBossList * attrList, PMReal spaceAfter) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN gridAlign : value of gridAlignment attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddGridAlignmentAttribute(AttributeBossList * attrList, bool8 gridAlign) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN numberOfLines : value of KeepWithNextAttribute attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddKeepWithNextAttribute(AttributeBossList * attrList, int16 numberOfLines) =0;
		
		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN keepLines : value of keepLines attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddKeepLinesAttribute(AttributeBossList * attrList, bool8 keepLines) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN keepTogether : value of keepTogether attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddKeepTogetherAttribute(AttributeBossList * attrList, bool8 keepTogether)=0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN firstLines : value of firstLines attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddFirstLinesAttribute(AttributeBossList * attrList, int32 firstLines) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN lastLines : value of lastLines attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddLastLinesAttribute(AttributeBossList * attrList, int32 lastLines) =0;

		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN align : value of align attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddAlignAttribute(AttributeBossList * attrList, int32 align) =0;
		
		/**
			Add a font attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN align : value of skew attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddSkewAngleAttribute(AttributeBossList * attrList, PMReal angle) =0;

		/**
			these fonctions Add a font attribute to an attributes list. these attributes concerns the paragraph rules
			@param IN OUT attrList : attributes list
			@param IN align : value of align attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddParaRuleAboveColorAttribute(AttributeBossList * attrList, UID colorUID) =0;
		virtual ErrorCode AddParaRuleAboveIndentLAttribute(AttributeBossList * attrList, PMReal indentL) =0;
		virtual ErrorCode AddParaRuleAboveIndentRAttribute(AttributeBossList * attrList, PMReal indentR) =0;
		virtual ErrorCode AddParaRuleAboveModeAttribute(AttributeBossList * attrList, int32 mode) =0;
		virtual ErrorCode AddParaRuleAboveOffsetAttribute(AttributeBossList * attrList, PMReal offset) =0;
		virtual ErrorCode AddParaRuleAboveOnAttribute(AttributeBossList * attrList, bool8 ruleOn) =0;
		virtual ErrorCode AddParaRuleAboveStrokeAttribute(AttributeBossList * attrList, PMReal stroke) =0;
		virtual ErrorCode AddParaRuleAboveTintAttribute(AttributeBossList * attrList, PMReal tint) =0;
		virtual ErrorCode AddParaRuleAboveTypeAttribute(AttributeBossList * attrList, int8 type) =0;
		virtual ErrorCode AddParaRuleBelowColorAttribute(AttributeBossList * attrList, UID colorUID) =0;
		virtual ErrorCode AddParaRuleBelowIndentLAttribute(AttributeBossList * attrList, PMReal indentL) =0;
		virtual ErrorCode AddParaRuleBelowIndentRAttribute(AttributeBossList * attrList, PMReal indentR) =0;
		virtual ErrorCode AddParaRuleBelowModeAttribute(AttributeBossList * attrList, int32 mode) =0;
		virtual ErrorCode AddParaRuleBelowOffsetAttribute(AttributeBossList * attrList, PMReal offset) =0;
		virtual ErrorCode AddParaRuleBelowOnAttribute(AttributeBossList * attrList, bool8 ruleOn) =0;
		virtual ErrorCode AddParaRuleBelowStrokeAttribute(AttributeBossList * attrList, PMReal stroke) =0;
		virtual ErrorCode AddParaRuleBelowTintAttribute(AttributeBossList * attrList, PMReal tint) =0;
		virtual ErrorCode AddParaRuleBelowTypeAttribute(AttributeBossList * attrList, int8 type) =0;

		/**
			these fonctions Add a font attribute to an attributes list. these attributes concerns indent left or right
			@param IN OUT attrList : attributes list
			@param IN value : value of indent attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddIndent1LAttribute(AttributeBossList * attrList, PMReal value) =0;
		virtual ErrorCode AddIndentLAttribute(AttributeBossList * attrList, PMReal value) =0;
		virtual ErrorCode AddIndentRAttribute(AttributeBossList * attrList, PMReal value) =0;

		/**
			Add a tab attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN tabSTopTable : value of tabs attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddTabsAttribute(AttributeBossList * attrList, TabStopTable * tabTable) =0;


		/**
			Add a ligatures attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN useLigatures : value of ligatures attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddLigaturesAttribute(AttributeBossList * attrList, bool8 useLigatures) =0;
		
		/**
			Add a ligatures attribute to an attributes list
			@param IN OUT attrList : attributes list
			@param IN method : pair kerning method 
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddPairKernMethodAttribute(AttributeBossList * attrList, int32 method) =0;

		/**
			these fonctions Add a font attribute to an attributes list. 
			these attributes concerns the hyphen's and justification's rules
			@param IN OUT attrList : attributes list
			@param IN value : value of attribute
			Return an ErrorCode to know if function failed 
		*/
		virtual ErrorCode AddHyphenCapAttribute(AttributeBossList * attrList, bool8 hyphenCap) =0;
		virtual ErrorCode AddHyphenWeightAttribute(AttributeBossList * attrList, int16 hyphenWeight)=0;
		virtual ErrorCode AddHyphenZoneAttribute(AttributeBossList * attrList, PMReal hyphenZone) =0;
		virtual ErrorCode AddMinAfterAttribute(AttributeBossList * attrList, int16 minAfter)=0;
		virtual ErrorCode AddMinBeforeAttribute(AttributeBossList * attrList, int16 minBefore) =0;
		virtual ErrorCode AddHyphenModeAttribute(AttributeBossList * attrList, bool8 actived) =0;
		virtual ErrorCode AddHyphenLastWordAttribute(AttributeBossList * attrList, bool8 lastWord) =0;
		virtual ErrorCode AddLetterspaceDesAttribute(AttributeBossList * attrList, PMReal letterspace) =0;
		virtual ErrorCode AddLetterspaceMaxAttribute(AttributeBossList * attrList, PMReal letterspace) =0;
		virtual ErrorCode AddLetterspaceMinAttribute(AttributeBossList * attrList, PMReal letterspace) =0;
		virtual ErrorCode AddWordspaceDesAttribute(AttributeBossList * attrList, PMReal wordspace) =0;
		virtual ErrorCode AddWordspaceMaxAttribute(AttributeBossList * attrList, PMReal wordspace) =0;
		virtual ErrorCode AddWordspaceMinAttribute(AttributeBossList * attrList, PMReal wordspace) =0;
		virtual ErrorCode AddShortestWordAttribute(AttributeBossList * attrList, int16 length) =0;
		virtual ErrorCode AddLadderAttribute(AttributeBossList * attrList, int16 limit) =0;

		

}; 

#endif // __ITxtAttrHelper_h__
