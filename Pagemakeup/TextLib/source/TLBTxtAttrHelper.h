/*
//	File:	TLBTxtAttrHelper.h
//
//  Author: Trias
//
//	Date:	18-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __TLBTxtAttrHelper_h__
#define __TLBTxtAttrHelper_h__

// API includes
#include "CPMUnknown.h"

// Project includes
#include "ITxtAttrHelper.h"

/** TLBTxtAttrHelper
	Implements the ITxtAttrHelper interface
*/
class TLBTxtAttrHelper : public CPMUnknown<ITxtAttrHelper> 
{

	public :
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		TLBTxtAttrHelper(IPMUnknown * boss);

		/** 
			Destructor
		*/
		~TLBTxtAttrHelper();

		// --------------------------
		//  Text Attributes creation
		// --------------------------
		virtual ErrorCode AddAutoLeadModeAttribute(AttributeBossList * attrList, PMReal value);
		virtual ErrorCode AddAutoLeadAttribute(AttributeBossList * attrList, PMReal percent);
		virtual ErrorCode AddPairKernAttribute(AttributeBossList * attrList, PMReal value);

		virtual ErrorCode AddColorAttribute(AttributeBossList * attrList, UID colorUID) ;
		virtual ErrorCode AddOverPrintFillAttribute(AttributeBossList * attrList, bool8 overPrint);
		virtual ErrorCode AddFontSizeAttribute(AttributeBossList * attrList, PMReal fontSize) ;
		virtual ErrorCode AddFontStyleAttribute(AttributeBossList * attrList, int16 fontStyle) ;
		virtual ErrorCode AddXScaleAttribute(AttributeBossList * attrList, PMReal percent) ;
		virtual ErrorCode AddYScaleAttribute(AttributeBossList * attrList, PMReal percent) ;
		virtual ErrorCode AddTrackKernAttribute(AttributeBossList * attrList, PMReal value) ;
		virtual ErrorCode AddBLShiftAttribute(AttributeBossList * attrList, PMReal value) ;
		virtual ErrorCode AddTintAttribute(AttributeBossList * attrList, PMReal percent) ;
		virtual ErrorCode AddFontAttribute(AttributeBossList * attrList, UID fontUID) ;
		virtual ErrorCode AddFontStyleAttribute(AttributeBossList * attrList, PMString fontStyle) ;
		virtual ErrorCode AddUnderlineAttribute(AttributeBossList * attrList, bool8 underline) ;
		virtual ErrorCode AddStrikeThruAttribute(AttributeBossList * attrList, bool8 strikeThru) ;
		virtual ErrorCode AddCapitalAttribute(AttributeBossList * attrList, int32 capitalMode) ;
		virtual ErrorCode AddPositionAttribute(AttributeBossList * attrList, int32 positionMode);
		virtual ErrorCode AddNoBreakAttribute(AttributeBossList * attrList, bool8 noBreak) ;
		virtual ErrorCode AddDropCapAttributes(AttributeBossList * attrList, int16 numberOfChars, int16 numberOfLines) ;
		virtual ErrorCode AddSpaceBeforeAttribute(AttributeBossList * attrList, PMReal spaceAfter) ;
		virtual ErrorCode AddSpaceAfterAttribute(AttributeBossList * attrList, PMReal spaceAfter) ;
		virtual ErrorCode AddGridAlignmentAttribute(AttributeBossList * attrList, bool8 gridAlign) ;
		virtual ErrorCode AddKeepWithNextAttribute(AttributeBossList * attrList, int16 numberOfLines);
		virtual ErrorCode AddKeepLinesAttribute(AttributeBossList * attrList, bool8 keepLines);
		virtual ErrorCode AddKeepTogetherAttribute(AttributeBossList * attrList, bool8 keepTogether);
		virtual ErrorCode AddFirstLinesAttribute(AttributeBossList * attrList, int32 firstLines) ;
		virtual ErrorCode AddLastLinesAttribute(AttributeBossList * attrList, int32 lastLines);
		virtual ErrorCode AddAlignAttribute(AttributeBossList * attrList, int32 align) ;
		virtual ErrorCode AddSkewAngleAttribute(AttributeBossList * attrList, PMReal angle);
		virtual ErrorCode AddParaRuleAboveColorAttribute(AttributeBossList * attrList, UID colorUID) ;
		virtual ErrorCode AddParaRuleAboveIndentLAttribute(AttributeBossList * attrList, PMReal indentL);
		virtual ErrorCode AddParaRuleAboveIndentRAttribute(AttributeBossList * attrList, PMReal indentR);
		virtual ErrorCode AddParaRuleAboveModeAttribute(AttributeBossList * attrList, int32 mode);
		virtual ErrorCode AddParaRuleAboveOffsetAttribute(AttributeBossList * attrList, PMReal offset);
		virtual ErrorCode AddParaRuleAboveOnAttribute(AttributeBossList * attrList, bool8 ruleOn);
		virtual ErrorCode AddParaRuleAboveStrokeAttribute(AttributeBossList * attrList, PMReal stroke);
		virtual ErrorCode AddParaRuleAboveTintAttribute(AttributeBossList * attrList, PMReal tint);
		virtual ErrorCode AddParaRuleAboveTypeAttribute(AttributeBossList * attrList, int8 type);
		virtual ErrorCode AddParaRuleBelowColorAttribute(AttributeBossList * attrList, UID colorUID) ;
		virtual ErrorCode AddParaRuleBelowIndentLAttribute(AttributeBossList * attrList, PMReal indentL);
		virtual ErrorCode AddParaRuleBelowIndentRAttribute(AttributeBossList * attrList, PMReal indentR);
		virtual ErrorCode AddParaRuleBelowModeAttribute(AttributeBossList * attrList, int32 mode);
		virtual ErrorCode AddParaRuleBelowOffsetAttribute(AttributeBossList * attrList, PMReal offset);
		virtual ErrorCode AddParaRuleBelowOnAttribute(AttributeBossList * attrList, bool8 ruleOn);
		virtual ErrorCode AddParaRuleBelowStrokeAttribute(AttributeBossList * attrList, PMReal stroke);
		virtual ErrorCode AddParaRuleBelowTintAttribute(AttributeBossList * attrList, PMReal tint);
		virtual ErrorCode AddParaRuleBelowTypeAttribute(AttributeBossList * attrList, int8 type);
		virtual ErrorCode AddIndent1LAttribute(AttributeBossList * attrList, PMReal value);
		virtual ErrorCode AddIndentLAttribute(AttributeBossList * attrList, PMReal value) ;
		virtual ErrorCode AddIndentRAttribute(AttributeBossList * attrList, PMReal value);
		virtual ErrorCode AddTabsAttribute(AttributeBossList * attrList, TabStopTable * tabTable);
		virtual ErrorCode AddLigaturesAttribute(AttributeBossList * attrList, bool8 useLigatures);
		virtual ErrorCode AddPairKernMethodAttribute(AttributeBossList * attrList, int32 method);

		virtual ErrorCode AddHyphenCapAttribute(AttributeBossList * attrList, bool8 hyphenCap) ;
		virtual ErrorCode AddHyphenWeightAttribute(AttributeBossList * attrList, int16 hyphenWeight);
		virtual ErrorCode AddHyphenZoneAttribute(AttributeBossList * attrList, PMReal hyphenZone) ;
		virtual ErrorCode AddMinAfterAttribute(AttributeBossList * attrList, int16 minAfter);
		virtual ErrorCode AddMinBeforeAttribute(AttributeBossList * attrList, int16 minBefore) ;
		virtual ErrorCode AddHyphenModeAttribute(AttributeBossList * attrList, bool8 actived) ;
		virtual ErrorCode AddHyphenLastWordAttribute(AttributeBossList * attrList, bool8 lastWord) ;
		virtual ErrorCode AddLetterspaceDesAttribute(AttributeBossList * attrList, PMReal letterspace) ;
		virtual ErrorCode AddLetterspaceMaxAttribute(AttributeBossList * attrList, PMReal letterspace) ;
		virtual ErrorCode AddLetterspaceMinAttribute(AttributeBossList * attrList, PMReal letterspace) ;
		virtual ErrorCode AddWordspaceDesAttribute(AttributeBossList * attrList, PMReal wordspace) ;
		virtual ErrorCode AddWordspaceMaxAttribute(AttributeBossList * attrList, PMReal wordspace) ;
		virtual ErrorCode AddWordspaceMinAttribute(AttributeBossList * attrList, PMReal wordspace) ;
		virtual ErrorCode AddShortestWordAttribute(AttributeBossList * attrList, int16 length) ;
		virtual ErrorCode AddLadderAttribute(AttributeBossList * attrList, int16 limit) ;
		
		virtual ErrorCode AddLanguageAttribute(AttributeBossList * attrList, IDocument * doc, LanguageID lang);
};

#endif // __TLBTxtAttrHelper_h__