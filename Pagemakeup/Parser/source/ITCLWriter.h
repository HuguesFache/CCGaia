/*
//	File:	ITCLWriter.h
//
//  Author: Trias
//
//	Date:	16-Sep-2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __ITCLWriter_h__
#define __ITCLWriter_h__

#include "PrsID.h"

#include "ITCLParser.h"

#include "IStandOff.h"
#include "LanguageID.h"
#include "IStandOffContourWrapSettings.h"
#include "PMPathPoint.h"
#include "TOPOptionsType.h"
#include "ReverseChecker.h"

class IPMUnknown;
class AttributeBossList;

/** ITCLWriter
	Provides method to write TCL files.
*/
class ITCLWriter : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_ITCLWRITER };

		/**
			Accessor methods
		*/
		virtual void SetStream(IPMStream * TCLStream) =0;

		virtual void CloseStream() =0;
		
		virtual void SetColorList(UID * list) =0;
		
		virtual void SetDataBase(IDataBase * db) =0;

		virtual void SetExportAttributesPrefs(bool16 exportCJ, bool16 exportParaCmd, bool16 exportTextCmd) =0;

		virtual ReverseChecker * GetReverseChecker() =0;
		
		/* Write TCL methods */
		virtual void writeNewDoc(PMString name, PMReal width, PMReal height, int32 nbCol, PMReal gutter,
								 PMReal left, PMReal top, PMReal right, PMReal bottom, int32 nbPage, int32 pagesPerSpread, bool16 recto, 
								 bool8 autoLeading, PMReal autoLeadPercent, LanguageID language, bool8 autoKern) =0;

		virtual void writeDE(PMReal pos, PMReal size) =0;

		virtual void writeDH(PMReal pos, PMReal size) =0;

		virtual void writeDK(PMReal size) =0;
		
		virtual void writeNewStyle(int32 num, PMString name, int32 basedOnNum, int32 nextStyleNum, bool8 isParaStyle = kFalse) =0;
		
		virtual void writeEndStyle() =0;
		
		virtual void writeStyleAttributes(const AttributeBossList * list) =0;

		virtual PMString writeAttribute(ClassID attrID, const IPMUnknown * inter) =0;

		virtual void writeApplyStylesOrOverrides(const AttributeBossList * list, const AttributeBossList * list2, PMString paraStyleName, PMString charStyleName) =0;
		
		virtual void writeApplyMaster(PMString name, int32 startPage, int32 endPage) =0;
		
		virtual void writeNewColor(int32 colorID, PMString colorName, bool8 isSpot, int32 type, K2Vector<PMReal> values) =0;
		
		virtual void writeNewLayer(PMString layerName) =0;
		
		virtual void writeSetCurrentLayer(PMString layerName) =0;
		
		virtual void writeNewTextFrame(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, Text::VerticalJustification vj, PMReal offset, int32 numPage, UID color, PMReal tint,
										int32 nbCol, PMReal gutter, PMReal angle, PMReal vjMax, Text::FirstLineOffsetMetric topAlign, bool8 isInline, TCLRef refOwner,bool8 toLink=kFalse,TCLRef refLink=kInvalidTCLRef) =0;

		virtual void writeNewTextSpline(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList, Text::VerticalJustification vj, PMReal offset, int32 numPage, UID color, PMReal tint,
										int32 nbCol, PMReal gutter, PMReal angle, PMReal vjMax, Text::FirstLineOffsetMetric topAlign, bool8 isInline, TCLRef refOwner,bool8 toLink=kFalse,TCLRef refLink=kInvalidTCLRef) =0;
										
		virtual void writeSetInsets(TCLRef ref, PMReal top, PMReal left, PMReal bottom, PMReal right) =0;

		virtual void writeNonPrint(TCLRef ref, bool8 nonPrint) =0;

		virtual void writeIsLocked(TCLRef ref, bool8 locked) =0;
		
		virtual void writeFlipItem(TCLRef ref, bool8 flipH, bool8 flipV) =0;

		virtual void writeSkewItem(TCLRef ref, PMReal skewAngle) =0;

		virtual void writeSetTextWrap(TCLRef ref, IStandOff::mode mode, PMReal top, PMReal left, PMReal bottom, PMReal right, IStandOff::form form, IStandOffContourWrapSettings::ContourWrapType contourType) =0;
		
		virtual void writeSetStroke(TCLRef ref, PMReal weight, ClassID type, UID color, PMReal tint, ClassID cornerType, PMReal cornerRadius, UID gapColor, PMReal gapTint, int32 strokeAlign) =0;

		virtual void writeShadowEffect(TCLRef ref, bool16 mode, int8 blendMode, PMReal xOffset, PMReal yOffset, UID color, PMReal opacity, PMReal radius, PMReal spread, PMReal noise) =0;
		
		virtual void writeNewImageFrame(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, PMString fullName, PMReal xDec, PMReal yDec, PMReal xScale, PMReal yScale,
										int32 numPage, UID color, PMReal tint, PMReal angle, ClassID cornerType, PMReal cornerRadius, bool8 isInline, TCLRef refOwner) =0;
		
		virtual void writeNewRule(TCLRef ref, PMPoint p1, PMPoint p2, PMReal stroke, int32 strokeAlign, ClassID type, UID color, PMReal tint, int32 numPage, ClassID lineEndStart, ClassID lineEndEnd) =0;
		
		virtual void writeNewGraphicSpline(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList,  PMString fullName, PMReal xDec, PMReal yDec, PMReal xScale, PMReal yScale,
										int32 numPage, UID color, PMReal tint, PMReal angle, ClassID cornerType, PMReal cornerRadius, bool8 isInline, TCLRef refOwner) =0;

		virtual void writeSetInlineProperties(TCLRef ref, IAnchoredObjectData::AnchoredPosition pos, PMReal offset) =0;
		
		virtual void writeCreateGroup(K2Vector<TCLRef> itemRefList) =0;

		virtual void writeCreateInlineGroup(TCLRef refOwner, K2Vector<TCLRef> itemRefList) =0;

		virtual void setCurrentParaStyle(UID currentParaStyle)=0;

		virtual void setCurrentCharStyle(UID currentCharStyle)=0;

		virtual void writeApplyParaStyle(PMString paraStyleName) =0;

		virtual void writeApplyCharStyle(PMString charStyleName) =0;

		virtual void writeChar(textchar c, bool16& inlineFound, bool16& zeroBreakFound) =0;

		virtual void writeSelectItem(TCLRef ref, bool16 selectTOP = kFalse) =0;

		virtual void writeNewMaster(PMString masterName, PMString basedOn, int32 nbPages) =0;

		virtual void writeEndMaster() =0;

		virtual void EndText() =0;

		virtual void writeClippingPath(TCLRef ref, int16 clipType, PMReal tolerance, PMReal inset, uint8 threshold, int16 index, bool16 invert, bool16 allowHoles, bool16 limitToFrame, bool16 useHighRes) =0;
		
		virtual void writeTransparencyEffect(TCLRef ref, int8 blendMode, PMReal opacity, bool16 knockoutGroup, bool16 isolationGroup) =0;

		virtual void writeFeatherEffect(TCLRef ref, int8 mode, PMReal width, int8 corner, PMReal noise, PMReal innerOpacity, PMReal outerOpacity) =0;

		virtual void writeNewGradient(int32 colorID, PMString colorName, int8 type, K2Vector<int32> refColors, K2Vector<PMReal> stopPositions, K2Vector<PMReal> midPoints) =0;

		virtual void writeGradientOptions(TCLRef ref, bool16 isFillAttr, PMReal angle, PMReal length, PMPoint center, PMReal radius) =0;

		virtual void writeNewTintedColor(int32 colorID, int32 basedOn, PMReal tint) =0;

		virtual void writeSection(int32 startPage, int32 folio, PMString prefix, int8 type, bool16 autoNumber, bool16 includeSectionPrefix, PMString marker) =0;

		virtual void writeTOP(TCLRef ref, PMReal startPos, PMReal endPos, const TextAlignType textAlign, const PathAlignType pathAlign, const EffectType typeEffect, const int16 overlapOffset, const bool16 flip) =0;

		virtual void writeTOC(TCLRef ref, PMString tocStyle) =0;

		virtual void writeTOCStyle(PMString tocStyleName, PMString tocTitle, int32 tocTitleStyle,
								 bool16 includeHidLayer, bool16 runIn, bool16 includeBookMarks, 
								 K2Vector<KeyValuePair<int32, FormatEntryParam> >& entries) =0;

		virtual void writeTiffOptions(TCLRef ref, UID color, PMReal tint) =0;

		virtual void writeHyperlinkTextDest(PMString hlName, bool16 visible) =0;
		virtual void writeHyperlinkPageDest(PMString hlName, int32 pageNumber, bool16 visible) =0;
		virtual void writeHyperlinkURLDest(PMString hlName, PMString url) =0;

		virtual void writeHyperlinkSource(int8 type, PMString hlName, PMString hlDestName, int32 color, int32 hilightType, int32 outlineStyle, 
										  int32 borderWidth, bool16 visible, TextIndex start = kInvalidTextIndex, TextIndex end = kInvalidTextIndex) =0;

		virtual void writeBookmark(PMString bkName, PMString bkParent, PMString hlDestName) =0;

		virtual void writeImageInfo(TCLRef ref, PMReal imageWidth, PMReal imageHeight) =0;

		virtual void writeImageProperties(TCLRef ref, PMReal imageAngle, PMReal imageSkew, PMReal xOffset, PMReal yOffset) =0;
};																											
																											
#endif // __ITCLWriter_h__
