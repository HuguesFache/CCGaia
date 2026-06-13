/*
//	File:	ReverseChecker.h
//
//	Date:	25-Juil-2007
//
//  Author : Trias
//
*/


#ifndef __ReverseChecker_h__
#define __ReverseChecker_h__

#include "PMUTypes.h"
#include "IStandOff.h"
#include "IStandOffContourWrapSettings.h"
#include "IAnchoredObjectData.h"
#include "TOPOptionsType.h"

class IPMStream;

/** ReverseChecker
	Check parameter integrity of TCL commands
*/
class ReverseChecker
{
public :

	/**
		Constructor
		@param IN parser : ReverseTCL file being checked
	*/
	ReverseChecker(IDFile ReverseTCLFile);

	/**
		Destructor
	*/
	~ReverseChecker();

	/** Line count handling
	*/
	inline void NewLine() { ++lineCount; }

	/** Write message function
	*/
	void WriteReverseLog(const PMString& msg, const PMString& commandName,int32 numParam);

	/** Log accessors
	*/
	const PMString& GetReverseLog();
	const IDFile& GetReverseLogFile();

	/** Check functions
	*/	
	void checkNewDoc(PMString name, PMReal width, PMReal height, int32 nbCol, PMReal gutter,
							 PMReal left, PMReal top, PMReal right, PMReal bottom, int32 nbPage, int32 pagesPerSpread,
							 bool8 autoLeading, PMReal autoLeadPercent, LanguageID language, bool8 autoKern); // OK
	void checkDE(PMReal pos, PMReal size); // OK
	void checkDH(PMReal pos, PMReal size); // OK
	void checkDK(PMReal size);	// OK						 
	void checkNewStyle(int32 num, PMString name, int32 basedOnNum, int32 nextStyleNum, bool8 isParaStyle); // OK
	void checkStyleAttributes(const AttributeBossList * list);
	void checkNewColor(int32 colorID, PMString colorName, bool8 isSpot, int32 type, K2Vector<PMReal> values); // OK
	void checkNewLayer(PMString layerName);	// OK
	void checkSetCurrentLayer(PMString layerName); // OK	
	void checkNewTextFrame(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, Text::VerticalJustification vj, PMReal offset, int32 numPage, UID color, PMReal tint,
						   int32 nbCol, PMReal gutter, PMReal angle, PMReal vjMax, Text::FirstLineOffsetMetric topAlign, bool8 isInline, TCLRef refOwner,bool8 toLink,TCLRef refLink);	// OK
	void checkNewTextSpline(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList, Text::VerticalJustification vj, PMReal offset, int32 numPage, UID color, PMReal tint,
								int32 nbCol, PMReal gutter, PMReal angle, PMReal vjMax, Text::FirstLineOffsetMetric topAlign, bool8 isInline, TCLRef refOwner,bool8 toLink,TCLRef refLink); // OK
	void checkSetInsets(TCLRef ref, PMReal top, PMReal left, PMReal bottom, PMReal right); // OK
	void checkSkewItem(TCLRef ref, PMReal skewAngle); // OK
	void checkSetTextWrap(TCLRef ref, IStandOff::mode mode, PMReal top, PMReal left, PMReal bottom, PMReal right, IStandOff::form form, IStandOffContourWrapSettings::ContourWrapType);	// OK
	void checkSetStroke(TCLRef ref, PMReal weight, ClassID type, UID color, PMReal tint, ClassID cornerType, PMReal cornerRadius, UID gapColor, PMReal gapTint, int32 strokeAlign);	// OK
	void checkShadowEffect(TCLRef ref, bool16 mode, int8 blendMode, PMReal xOffset, PMReal yOffset, UID color, PMReal opacity, PMReal radius, PMReal spread, PMReal noise); // OK
	void checkNewImageFrame(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, PMString fullName, PMReal xDec, PMReal yDec, PMReal xScale, PMReal yScale,
								int32 numPage, UID color, PMReal tint, PMReal angle, ClassID cornerType, PMReal cornerRadius, bool8 isInline, TCLRef refOwner); // OK
	void checkNewGraphicSpline(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList,  PMString fullName, PMReal xDec, PMReal yDec, PMReal xScale, PMReal yScale,
								int32 numPage, UID color, PMReal tint, PMReal angle, ClassID cornerType, PMReal cornerRadius, bool8 isInline, TCLRef refOwner); // OK
	void checkNewRule(TCLRef ref, PMPoint p1, PMPoint p2, PMReal stroke, int32 strokeAlign, ClassID type, UID color, PMReal tint, int32 numPage, ClassID lineEndStart, ClassID lineEndEnd);	// OK
	void checkSetInlineProperties(TCLRef ref, IAnchoredObjectData::AnchoredPosition pos, PMReal offset);	 // OK
	void checkApplyParaStyle(PMString paraStyleName); // OK
	void checkApplyCharStyle(PMString charStyleName); // OK
	void checkChar(textchar c); // OK
	void checkNewMaster(PMString masterName, PMString basedOn, int32 nbPages);	// OK
	void checkClippingPath(TCLRef ref, int16 clipType, PMReal tolerance, PMReal inset, uint8 threshold, int16 index, bool16 invert, bool16 allowHoles, bool16 limitToFrame, bool16 useHighRes);	 // OK
	void checkTransparencyEffect(TCLRef ref, int8 blendMode, PMReal opacity, bool16 knockoutGroup, bool16 isolationGroup); // OK
	void checkFeatherEffect(TCLRef ref, int8 mode, PMReal width, int8 corner, PMReal noise, PMReal innerOpacity, PMReal outerOpacity);	// OK
	void checkNewGradient(int32 colorID, PMString colorName, int8 type, K2Vector<int32> refColors, K2Vector<PMReal> stopPositions, K2Vector<PMReal> midPoints); // OK
	void checkGradientOptions(TCLRef ref, bool16 isFillAttr, PMReal angle, PMReal length, PMPoint center, PMReal radius); // OK
	void checkNewTintedColor(int32 colorID, int32 basedOn, PMReal tint);
	void checkTOC(TCLRef ref, PMString tocStyle);
	void checkTOCStyle(PMString tocStyleName, PMString tocTitle, int32 tocTitleStyle,
						 bool16 includeHidLayer, bool16 runIn, bool16 includeBookMarks, 
						 K2Vector<KeyValuePair<int32, FormatEntryParam> >& entries);
	void checkHyperlinkTextDest(PMString hlName, bool16 visible);
	void checkHyperlinkPageDest(PMString hlName, int32 pageNumber, bool16 visible);
	void checkHyperlinkURLDest(PMString hlName, PMString url);
	void checkHyperlinkSource(int8 type, PMString hlName, PMString hlDestName, int32 color, int32 hilightType, int32 outlineStyle, 
								  int32 borderWidth, bool16 visible, TextIndex start, TextIndex end);
	void checkBookmark(PMString bkName, PMString bkParent, PMString hlDestName);

	void checkDropCaps(int32 nbChars, int32 nbLines);
	void checkSpacing(PMReal spaceBefore, PMReal spaceAfter);
	void checkKeepOptions(bool8 keepLines, bool8 keepAll, int32 firstLines, int32 lastLines);
	void checkIndents(PMReal indentLeft, PMReal indentRight, PMReal indentFirst);
	void checkParagraphRule(PMString commandName, PMReal stroke, bool8 mode, PMReal lIndent, PMReal rIndent, PMReal offset, int32 type, int32 color, PMReal tint, bool8 on);
	void chechHyphenation(PMString cjName, bool8 mode, bool8 capitalized, int16 minWord, int16 afterFirst, int16 beforeLast, int16 limit, PMReal zone,
						  bool8 lastWord, PMReal wordMin, PMReal wordDes, PMReal wordMax, PMReal letterMin, PMReal letterDes, PMReal letterMax);
	void checkLeading(PMReal leading);
	void checkKerning(PMReal kerning);
	void checkFontSize(PMReal size);
	void checkGlyphScale(PMString commandName, PMReal scale);
	void checkTracking(PMReal tracking);
	void checkBaselineOffset(PMReal offset);
	void checkTextTint(PMReal tint);
	void checkTabPosition(PMReal pos, int32 numParam);

private :

	IDFile logFile;
	int32 lineCount;
	PMString log;
	
	PMString GetInvalidRangeMsg(PMReal inf, PMReal sup);
};

#endif // __ReverseChecker_h__
