/*
//	File:	ReverseChecker.cpp
//
//	Date:	25-Juil-2007
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"
#include "GlobalDefs.h"

// Interfaces includes
#include "IPMStream.h"
#include "IClipSettings.h"

// General includes
#include "StreamUtil.h"
#include "FileUtils.h"
#include "AttributeBossList.h"
#include "LanguageID.h"
#include "CTextEnum.h"
#include "PMPathPoint.h"
#include "StringUtils.h"
#include "InstStrokeFillID.h"
#include "ScotchRulesID.h"

// Project includes
#include "ReverseChecker.h"
#include "PrsID.h"

/* Constructor
*/
ReverseChecker::ReverseChecker(IDFile ReverseTCLFile) : lineCount(1), log(kNullString)
{	
#if REVERSE_CHECKER
	PMString fileName;		
	FileUtils::GetFileName(ReverseTCLFile, fileName);

	// Remove extension if any
	int32 extPos = fileName.LastIndexOfCharacter(PlatformChar('.'));
	if(extPos >= 0)
		fileName.Remove(extPos, fileName.NumUTF16TextChars() - extPos);

	fileName += ".log";

	logFile = ReverseTCLFile;
	logFile.SetFileName(fileName);
#endif
}

/* Destructor
*/
ReverseChecker::~ReverseChecker()
{
}

void ReverseChecker::WriteReverseLog(const PMString& msg, const PMString& commandName,int32 numParam)
{
#if REVERSE_CHECKER

	// Prepare message header just as usual TCL messages
	PMString ErrorLine(kErrorLineKey);
	ErrorLine.Translate(); ErrorLine.Append(" ");
	ErrorLine.AppendNumber(lineCount);
	ErrorLine += kNewLine;

	PMString CommandName = kNullString;
	if(!commandName.IsEmpty())
	{
		CommandName = PMString(kCommandKey);
		CommandName.Translate(); 
		CommandName += " " + commandName;
		CommandName += kNewLine;
	}

	PMString Parameter = kNullString;
	if(numParam > 0)
	{
		Parameter = PMString(kParameterKey);
		Parameter.Translate(); 
		Parameter += " ";
		Parameter.AppendNumber(numParam);
		Parameter += " : ";
	}
	
	PMString message = msg;
	message.Translate();

	PMString toWrite = ErrorLine + CommandName + Parameter + message + kNewLine + kNewLine;

	// Append to log string
	log += toWrite;

	// Append to log file
	InterfacePtr<IPMStream> logStream (StreamUtil::CreateFileStreamWrite(logFile,kOpenOut | kOpenApp,kTextType,kAnyCreator));
	if(logStream)
	{		
		logStream->XferByte((uchar *) toWrite.GetPlatformString().c_str(), toWrite.GetPlatformString().size()); 
		logStream->Flush();
		logStream->Close();
	}

#endif
}

const PMString& ReverseChecker::GetReverseLog()
{
	return log;
}

const IDFile& ReverseChecker::GetReverseLogFile()
{
	return logFile;
}

PMString ReverseChecker::GetInvalidRangeMsg(PMReal inf, PMReal sup)
{
	PMString msg(kErrInvalidRange);
	msg.Translate();

	PMString infStr;
	infStr.AppendNumber(inf, 2, kTrue, kTrue);

	PMString supStr;
	supStr.AppendNumber(sup, 2, kTrue, kTrue);

	StringUtils::ReplaceStringParameters(&msg, infStr, supStr);
	return msg;
}

void ReverseChecker::checkNewDoc(PMString name, PMReal width, PMReal height, int32 nbCol, PMReal gutter,
							 PMReal left, PMReal top, PMReal right, PMReal bottom, int32 nbPage, int32 pagesPerSpread,
							 bool8 autoLeading, PMReal autoLeadPercent, LanguageID language, bool8 autoKern)
{
#if REVERSE_CHECKER

	if(name.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "DN", 1);

	if(width < MIN_WIDTH)
		WriteReverseLog(PMString(kErrInfMin), "DN", 2);

	if(width > MAX_WIDTH)
		WriteReverseLog(PMString(kErrSupMax), "DN", 2);

	if(height < MIN_HEIGHT)
		WriteReverseLog(PMString(kErrInfMin), "DN", 3);

	if(height > MAX_HEIGHT)
		WriteReverseLog(PMString(kErrSupMax), "DN", 3);

	if(nbCol < 1 || nbCol > MAX_COL)
		WriteReverseLog(GetInvalidRangeMsg(1,MAX_COL), "DN", 4);
#endif
}

void ReverseChecker::checkDE(PMReal pos, PMReal size)
{
#if REVERSE_CHECKER

	if(pos < -500 || size > 500)
		WriteReverseLog(GetInvalidRangeMsg(-500,500), "DE", 1);

	if(size < 1 || size > 200)
		WriteReverseLog(GetInvalidRangeMsg(1,200), "DE", 2);
#endif
}

void ReverseChecker::checkDH(PMReal pos, PMReal size)
{
#if REVERSE_CHECKER

	if(pos < -500 || size > 500)
		WriteReverseLog(GetInvalidRangeMsg(-500,500), "DH", 1);

	if(size < 1 || size > 200)
		WriteReverseLog(GetInvalidRangeMsg(1,200), "DH", 2);
#endif
}

void ReverseChecker::checkDK(PMReal size)
{
#if REVERSE_CHECKER

	if(size < 1 || size > 200)
		WriteReverseLog(GetInvalidRangeMsg(1,200), "DK", 1);

#endif
}

void ReverseChecker::checkNewStyle(int32 num, PMString name, int32 basedOnNum, int32 nextStyleNum, bool8 isParaStyle)
{
#if REVERSE_CHECKER

	if(name.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "SD", 2);

#endif
}

void ReverseChecker::checkNewColor(int32 colorID, PMString colorName, bool8 isSpot, int32 type, K2Vector<PMReal> values)
{
#if REVERSE_CHECKER

	switch(type)
	{
		case CCID_CMYK :
			if(values[0] < 0 || values[0] >  65535)
				WriteReverseLog(GetInvalidRangeMsg(0,65535), "CN", 4);

			if(values[1] < 0 || values[1] >  65535)
				WriteReverseLog(GetInvalidRangeMsg(0,65535), "CN", 5);

			if(values[2] < 0 || values[2] >  65535)
				WriteReverseLog(GetInvalidRangeMsg(0,65535), "CN", 6);

			if(values[3] < 0 || values[3] >  65535)
				WriteReverseLog(GetInvalidRangeMsg(0,65535), "CN", 7);
			
			break;

		case CCID_RGB :
			if(values[0] < 0 || values[0] >  65535)
				WriteReverseLog(GetInvalidRangeMsg(0,65535), "CN", 8);

			if(values[1] < 0 || values[1] >  65535)
				WriteReverseLog(GetInvalidRangeMsg(0,65535), "CN", 9);

			if(values[2] < 0 || values[2] >  65535)
				WriteReverseLog(GetInvalidRangeMsg(0,65535), "CN", 10);

			break;
						
		case CCID_LAB :
			if(values[0] < 0 || values[0] >  65535)
				WriteReverseLog(GetInvalidRangeMsg(0,65535), "CN", 4);

			if(values[1] < 0 || values[1] >  65535)
				WriteReverseLog(GetInvalidRangeMsg(0,65535), "CN", 5);

			if(values[2] < 0 || values[2] >  65535)
				WriteReverseLog(GetInvalidRangeMsg(0,65535), "CN", 6);

			break;

		default :
			
			WriteReverseLog(PMString(kErrInvalidParameter), "CN", 11);
			break;
	}

#endif
}

void ReverseChecker::checkNewLayer(PMString layerName)
{
#if REVERSE_CHECKER

	if(layerName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "LN", 1);
#endif
}

void ReverseChecker::checkSetCurrentLayer(PMString layerName)
{
#if REVERSE_CHECKER

	if(layerName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "LS", 1);

#endif
}

void ReverseChecker::checkNewTextFrame(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, Text::VerticalJustification vj, PMReal offset, int32 numPage, UID color, PMReal tint,
						   int32 nbCol, PMReal gutter, PMReal angle, PMReal vjMax, Text::FirstLineOffsetMetric topAlign, bool8 isInline, TCLRef refOwner,bool8 toLink,TCLRef refLink)
{
#if REVERSE_CHECKER

	if(nbCol < 1 || nbCol > MAX_COL)
		WriteReverseLog(GetInvalidRangeMsg(1,MAX_COL), "BN", 12);

#endif
}

void ReverseChecker::checkNewTextSpline(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList, Text::VerticalJustification vj, PMReal offset, int32 numPage, UID color, PMReal tint,
								int32 nbCol, PMReal gutter, PMReal angle, PMReal vjMax, Text::FirstLineOffsetMetric topAlign, bool8 isInline, TCLRef refOwner,bool8 toLink,TCLRef refLink)
{
#if REVERSE_CHECKER

	int numParamCol = 6;
	for(int32 i = 0 ; i < ptsList.Length() ; ++i)
	{
		numParamCol += 2 + ptsList[i].Key().Length() * 6;
	}
	numParamCol += 7;

	if(nbCol < 1 || nbCol > MAX_COL)
		WriteReverseLog(GetInvalidRangeMsg(1,MAX_COL), "B*", numParamCol);

#endif
}

void ReverseChecker::checkSetInsets(TCLRef ref, PMReal top, PMReal left, PMReal bottom, PMReal right)
{
#if REVERSE_CHECKER

	if(top < 0 || top > 8640)
		WriteReverseLog(GetInvalidRangeMsg(0,8640), "BJ", 2);

	if(left < 0 || left > 8640)
		WriteReverseLog(GetInvalidRangeMsg(0,8640), "BJ", 3);

	if(bottom < 0 || bottom > 8640)
		WriteReverseLog(GetInvalidRangeMsg(0,8640), "BJ", 4);

	if(right < 0 || right > 8640)
		WriteReverseLog(GetInvalidRangeMsg(0,8640), "BJ", 5);

#endif
}

void ReverseChecker::checkSkewItem(TCLRef ref, PMReal skewAngle)
{
#if REVERSE_CHECKER

	if(skewAngle < MIN_SKEW || skewAngle > MAX_SKEW)
		WriteReverseLog(GetInvalidRangeMsg(MIN_SKEW,MAX_SKEW), "BE", 2);

#endif
}

void ReverseChecker::checkSetTextWrap(TCLRef ref, IStandOff::mode mode, PMReal top, PMReal left, PMReal bottom, PMReal right, IStandOff::form form, IStandOffContourWrapSettings::ContourWrapType)
{
#if REVERSE_CHECKER

	int32 modeInt = (int32) mode;

	if(modeInt < 0 || modeInt > 11)
		WriteReverseLog(GetInvalidRangeMsg(0,11), "BH", 2);

	if(mode != IStandOff::kNone)
	{
		if(top < -1296 || top > 1296)
			WriteReverseLog(GetInvalidRangeMsg(-1296,1296), "BH", 3);

		if(left < -1296 || left > 1296)
			WriteReverseLog(GetInvalidRangeMsg(-1296,1296), "BH", 4);

		if(bottom < -1296 || bottom > 1296)
			WriteReverseLog(GetInvalidRangeMsg(-1296,1296), "BH", 5);

		if(right < -1296 || right > 1296)
			WriteReverseLog(GetInvalidRangeMsg(-1296,1296), "BH", 6);
	}

#endif
}

void ReverseChecker::checkSetStroke(TCLRef ref, PMReal weight, ClassID type, UID color, PMReal tint, ClassID cornerType, PMReal cornerRadius, UID gapColor, PMReal gapTint, int32 strokeAlign)
{
#if REVERSE_CHECKER

	if(weight < 0 || weight > 800)
		WriteReverseLog(GetInvalidRangeMsg(0,800), "BC", 2);

	if(type != kSolidPathStrokerBoss && type != kDashedPathStrokerBoss && type != kThinThinPathStrokerBoss && type != kThickThickPathStrokerBoss && type != kDashedPathStrokerBoss
		&& type != kCannedDotPathStrokerBoss && type != kThinThickPathStrokerBoss && type != kThickThinPathStrokerBoss && type != kThinThickThinPathStrokerBoss && type != kThickThinThickPathStrokerBoss
		&& type != kTriplePathStrokerBoss && type != kCannedDash3x2PathStrokerBoss && type != kCannedDash4x4PathStrokerBoss && type != kLeftSlantHashPathStrokerBoss && type != kRightSlantHashPathStrokerBoss
		&& type != kStraightHashPathStrokerBoss && type != kSingleWavyPathStrokerBoss && type != kWhiteDiamondPathStrokerBoss && type != kJapaneseDotsPathStrokerBoss)
		WriteReverseLog(PMString(kErrUnknownType), "BC", 3);

	if(cornerType != kInvalidClass && cornerType != kRoundedCornerBoss && cornerType != kInverseRoundedCornerBoss 
		&& cornerType != kBevelCornerBoss && cornerType != kInsetCornerBoss && cornerType != kFancyCornerBoss)
		WriteReverseLog(PMString(kErrUnknownType), "BC", 6);

	if(cornerRadius < 0 || cornerRadius > 800)
		WriteReverseLog(GetInvalidRangeMsg(0,800), "BC", 7);

	if(strokeAlign < 0 || strokeAlign > 2)
		WriteReverseLog(GetInvalidRangeMsg(0,2), "BC", 11);

#endif
}

void ReverseChecker::checkShadowEffect(TCLRef ref, bool16 mode, int8 blendMode, PMReal xOffset, PMReal yOffset, UID color, PMReal opacity, PMReal radius, PMReal spread, PMReal noise)
{
#if REVERSE_CHECKER

	if(blendMode < 0 || blendMode > 15)
		WriteReverseLog(GetInvalidRangeMsg(0,15), "SE", 3);
	
	if(xOffset < -1000 || xOffset > 1000)
		WriteReverseLog(GetInvalidRangeMsg(-1000,1000), "SE", 4);

	if(yOffset < -1000 || yOffset > 1000)
		WriteReverseLog(GetInvalidRangeMsg(-1000,1000), "SE", 5);

	if(opacity < 0 || opacity > 100)
		WriteReverseLog(GetInvalidRangeMsg(0,100), "SE", 7);

	if(radius < 0 || radius > 144)
		WriteReverseLog(GetInvalidRangeMsg(0,100), "SE", 8);

	if(spread < 0 || spread > 100)
		WriteReverseLog(GetInvalidRangeMsg(0,100), "SE", 9);

	if(noise < 0 || noise > 100)
		WriteReverseLog(GetInvalidRangeMsg(0,100), "SE", 10);
#endif
}

void ReverseChecker::checkNewImageFrame(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, PMString fullName, PMReal xDec, PMReal yDec, PMReal xScale, PMReal yScale,
								int32 numPage, UID color, PMReal tint, PMReal angle, ClassID cornerType, PMReal cornerRadius, bool8 isInline, TCLRef refOwner)
{
#if REVERSE_CHECKER
	
	if(cornerRadius < 0 || cornerRadius > 800)
		WriteReverseLog(GetInvalidRangeMsg(0,800), "BI", 14);

	if(cornerType != kInvalidClass && cornerType != kRoundedCornerBoss && cornerType != kInverseRoundedCornerBoss 
		&& cornerType != kBevelCornerBoss && cornerType != kInsetCornerBoss && cornerType != kFancyCornerBoss)
		WriteReverseLog(PMString(kErrUnknownType), "BI", 15);

#endif
}

void ReverseChecker::checkNewGraphicSpline(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList,  PMString fullName, PMReal xDec, PMReal yDec, PMReal xScale, PMReal yScale,
								int32 numPage, UID color, PMReal tint, PMReal angle, ClassID cornerType, PMReal cornerRadius, bool8 isInline, TCLRef refOwner)
{
#if REVERSE_CHECKER
	
	int numParamRadius = 6;
	for(int32 i = 0 ; i < ptsList.Length() ; ++i)
	{
		numParamRadius += 2 + ptsList[i].Key().Length() * 6;
	}
	numParamRadius += 9;

	if(cornerRadius < 0 || cornerRadius > 800)
		WriteReverseLog(GetInvalidRangeMsg(0,800), "B!", numParamRadius);

	if(cornerType != kInvalidClass && cornerType != kRoundedCornerBoss && cornerType != kInverseRoundedCornerBoss 
		&& cornerType != kBevelCornerBoss && cornerType != kInsetCornerBoss && cornerType != kFancyCornerBoss)
		WriteReverseLog(PMString(kErrUnknownType), "B!", numParamRadius + 1);

	
#endif
}

void ReverseChecker::checkNewRule(TCLRef ref, PMPoint p1, PMPoint p2, PMReal stroke, int32 strokeAlign, ClassID type, UID color, PMReal tint, int32 numPage, ClassID lineEndStart, ClassID lineEndEnd)
{
#if REVERSE_CHECKER

	if(stroke < 0 || stroke > 800)
		WriteReverseLog(GetInvalidRangeMsg(0,800), "FD", 6);

	if(type != kSolidPathStrokerBoss && type != kDashedPathStrokerBoss && type != kThinThinPathStrokerBoss && type != kThickThickPathStrokerBoss && type != kDashedPathStrokerBoss
		&& type != kCannedDotPathStrokerBoss && type != kThinThickPathStrokerBoss && type != kThickThinPathStrokerBoss && type != kThinThickThinPathStrokerBoss && type != kThickThinThickPathStrokerBoss
		&& type != kTriplePathStrokerBoss && type != kCannedDash3x2PathStrokerBoss && type != kCannedDash4x4PathStrokerBoss && type != kLeftSlantHashPathStrokerBoss && type != kRightSlantHashPathStrokerBoss
		&& type != kStraightHashPathStrokerBoss && type != kSingleWavyPathStrokerBoss && type != kWhiteDiamondPathStrokerBoss && type != kJapaneseDotsPathStrokerBoss)
		WriteReverseLog(PMString(kErrUnknownType), "FD", 7);

	if(lineEndStart == kInvalidClass)
	{
		if(lineEndEnd != kInvalidClass && lineEndEnd != kCurvedArrowHeadBoss)
			WriteReverseLog(PMString(kErrUnknownType), "FD", 13);
	}
	else if(lineEndStart == kCurvedArrowHeadBoss)
	{
		if(lineEndEnd != kInvalidClass && lineEndEnd != kCurvedArrowHeadBoss && lineEndEnd != kBarArrowHeadBoss)
			WriteReverseLog(PMString(kErrUnknownType), "FD", 13);
	}
	else if(lineEndStart == kBarArrowHeadBoss && lineEndEnd != kCurvedArrowHeadBoss)
		WriteReverseLog(PMString(kErrUnknownType), "FD", 13);
	else
		WriteReverseLog(PMString(kErrUnknownType), "FD", 13);

	if(strokeAlign < 0 || strokeAlign > 2)
		WriteReverseLog(GetInvalidRangeMsg(0,2), "FD", 15);
#endif
}

void ReverseChecker::checkSetInlineProperties(TCLRef ref, IAnchoredObjectData::AnchoredPosition pos, PMReal offset)
{
#if REVERSE_CHECKER
	
	if(pos != IAnchoredObjectData::kAboveLine && pos != IAnchoredObjectData::kStandardInline)
		WriteReverseLog(PMString(kErrUnknownType), "BQ", 2);
	
	if(offset < -10000 || offset > 10000)
		WriteReverseLog(GetInvalidRangeMsg(-10000,10000), "BQ", 6);

#endif
}

void ReverseChecker::checkApplyParaStyle(PMString paraStyleName)
{
#if REVERSE_CHECKER
	
	if(paraStyleName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "SN", 1);

#endif
}

void ReverseChecker::checkApplyCharStyle(PMString charStyleName)
{
#if REVERSE_CHECKER

	if(charStyleName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "SB", 1);

#endif
}

void ReverseChecker::checkChar(textchar c)
{
#if REVERSE_CHECKER

	switch(c)
	{
		case kTextChar_FootnoteMarker  : case kTextChar_Table  : case kTextChar_TableContinued  : 
			WriteReverseLog(PMString(kErrUnknownChar), kNullString, 0); 
			break;

		default : break;
	}

#endif
}

void ReverseChecker::checkNewMaster(PMString masterName, PMString basedOn, int32 nbPages)
{
#if REVERSE_CHECKER

	if(masterName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "MD", 1);

#endif
}

void ReverseChecker::checkClippingPath(TCLRef ref, int16 clipType, PMReal tolerance, PMReal inset, uint8 threshold, int16 index, bool16 invert, bool16 allowHoles, bool16 limitToFrame, bool16 useHighRes)
{
#if REVERSE_CHECKER

	if(clipType == IClipSettings::kClipUserPath)
		WriteReverseLog(PMString(kErrUnknownType), "BZ", 2);

	if(inset < -10000 || inset > 10000)
		WriteReverseLog(GetInvalidRangeMsg(-10000,10000), "BZ", 3);

	if(threshold < 0 || threshold > 255)
		WriteReverseLog(GetInvalidRangeMsg(0,255), "BZ", 12);

	if(tolerance < 0 || tolerance > 10)
		WriteReverseLog(GetInvalidRangeMsg(0,10), "BZ", 13);

#endif
}

void ReverseChecker::checkTransparencyEffect(TCLRef ref, int8 blendMode, PMReal opacity, bool16 knockoutGroup, bool16 isolationGroup)
{
#if REVERSE_CHECKER

	if(blendMode < 0 || blendMode > 15)
		WriteReverseLog(GetInvalidRangeMsg(0,15), "OE", 2);

	if(opacity < 0 || opacity > 100)
		WriteReverseLog(GetInvalidRangeMsg(0,100), "OE", 3);

#endif
}

void ReverseChecker::checkFeatherEffect(TCLRef ref, int8 mode, PMReal width, int8 corner, PMReal noise, PMReal innerOpacity, PMReal outerOpacity)
{
#if REVERSE_CHECKER

	if(mode < 0 || mode > 3)
		WriteReverseLog(GetInvalidRangeMsg(0,3), "FA", 2);

	if(width < 0 || width > 1000)
		WriteReverseLog(GetInvalidRangeMsg(0,1000), "FA", 3);

	if(corner < 0 || corner > 2)
		WriteReverseLog(GetInvalidRangeMsg(0,2), "FA", 4);

	if(noise < 0 || noise > 100)
		WriteReverseLog(GetInvalidRangeMsg(0,100), "FA", 5);

#endif
}

void ReverseChecker::checkNewGradient(int32 colorID, PMString colorName, int8 type, K2Vector<int32> refColors, K2Vector<PMReal> stopPositions, K2Vector<PMReal> midPoints)
{
#if REVERSE_CHECKER

	if(colorName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "GN", 2);

	if(type < 1 || type > 2)
		WriteReverseLog(GetInvalidRangeMsg(1,2), "GN", 3);

	int32 numParam = 4;

	for(int32 i = 0 ; i < refColors.Length() ; ++i)
	{
		if(stopPositions[i] < 0 || stopPositions[i] > 100)
			WriteReverseLog(GetInvalidRangeMsg(0,100), "GN",  numParam + 2);

		if(midPoints[i] < 13 || midPoints[i] > 87)
			WriteReverseLog(GetInvalidRangeMsg(13,87), "GN",  numParam + 3);

		numParam += 3;
	}

#endif
}

void ReverseChecker::checkGradientOptions(TCLRef ref, bool16 isFillAttr, PMReal angle, PMReal length, PMPoint center, PMReal radius)
{
#if REVERSE_CHECKER
	
	if(angle < -180 || angle > 360)
		WriteReverseLog(GetInvalidRangeMsg(-180,360), "GO", 3);

#endif
}

void ReverseChecker::checkNewTintedColor(int32 colorID, int32 basedOn, PMReal tint)
{
#if REVERSE_CHECKER

	if(tint < 0 || tint > 100)
		WriteReverseLog(GetInvalidRangeMsg(0,100), "C2", 3);

#endif
}

void ReverseChecker::checkTOC(TCLRef ref, PMString tocStyle)
{
#if REVERSE_CHECKER

	if(tocStyle.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "MT", 2);

#endif
}

void ReverseChecker::checkTOCStyle(PMString tocStyleName, PMString tocTitle, int32 tocTitleStyle,
						 bool16 includeHidLayer, bool16 runIn, bool16 includeBookMarks, 
						 K2Vector<KeyValuePair<int32, FormatEntryParam> >& entries)
{
#if REVERSE_CHECKER

	if(tocStyleName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "ST", 2);	

#endif
}

void ReverseChecker::checkHyperlinkTextDest(PMString hlName, bool16 visible)
{
#if REVERSE_CHECKER

	if(hlName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "H1", 1);

#endif
}
	
void ReverseChecker::checkHyperlinkPageDest(PMString hlName, int32 pageNumber, bool16 visible)
{
#if REVERSE_CHECKER

	if(hlName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "H2", 1);

#endif
}

void ReverseChecker::checkHyperlinkURLDest(PMString hlName, PMString url)
{
#if REVERSE_CHECKER

	if(hlName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "H3", 1);

#endif
}

void ReverseChecker::checkHyperlinkSource(int8 type, PMString hlName, PMString hlDestName, int32 color, int32 hilightType, int32 outlineStyle, 
								  int32 borderWidth, bool16 visible, TextIndex start, TextIndex end)
{
#if REVERSE_CHECKER

	if(hlName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "HK", 2);

	if(hlDestName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "HK", 3);

	if(hilightType < 0 || hilightType > 3)
		WriteReverseLog(GetInvalidRangeMsg(0,3), "HK", 5);

	if(outlineStyle < 0 || outlineStyle > 1)
		WriteReverseLog(GetInvalidRangeMsg(0,1), "HK", 6);

	if(borderWidth < 1 || borderWidth > 3)
		WriteReverseLog(GetInvalidRangeMsg(1,3), "HK", 7);

#endif
}

void ReverseChecker::checkBookmark(PMString bkName, PMString bkParent, PMString hlDestName)
{
#if REVERSE_CHECKER

	if(bkName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "SI", 2);

	if(hlDestName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "SI", 4);

#endif
}

void ReverseChecker::checkDropCaps(int32 nbChars, int32 nbLines)
{
#if REVERSE_CHECKER

	if(nbChars < 0 || nbChars > 150)
		WriteReverseLog(GetInvalidRangeMsg(0,150), "PL", 1);

	if(nbLines < 0 || nbLines > 25)
		WriteReverseLog(GetInvalidRangeMsg(0,25), "PL", 2);

#endif
}

void ReverseChecker::checkSpacing(PMReal spaceBefore, PMReal spaceAfter)
{
#if REVERSE_CHECKER

	if(spaceBefore < 0 || spaceBefore > 8640)
		WriteReverseLog(GetInvalidRangeMsg(0,8640), "PE", 1);

	if(spaceAfter < 0 || spaceAfter > 8640)
		WriteReverseLog(GetInvalidRangeMsg(0,8640), "PE", 2);

#endif
}

void ReverseChecker::checkKeepOptions(bool8 keepLines, bool8 keepAll, int32 firstLines, int32 lastLines)
{
#if REVERSE_CHECKER

	if(firstLines < MIN_LINE || firstLines > MAX_LINE)
		WriteReverseLog(GetInvalidRangeMsg(MIN_LINE,MAX_LINE), "PH", 3);

	if(lastLines < MIN_LINE || lastLines > MAX_LINE)
		WriteReverseLog(GetInvalidRangeMsg(MIN_LINE,MAX_LINE), "PH", 4);
	
#endif
}

void ReverseChecker::checkIndents(PMReal indentLeft, PMReal indentRight, PMReal indentFirst)
{
#if REVERSE_CHECKER

	if(indentLeft < 0 || indentLeft > MAX_INDENT)
		WriteReverseLog(GetInvalidRangeMsg(0,MAX_INDENT), "PR", 1);

	if(indentRight < 0 || indentRight > MAX_INDENT)
		WriteReverseLog(GetInvalidRangeMsg(0,MAX_INDENT), "PR", 2);

	if(indentFirst < MIN_INDENT || indentFirst > MAX_INDENT)
		WriteReverseLog(GetInvalidRangeMsg(MIN_INDENT,MAX_INDENT), "PR", 3);
#endif
}

void ReverseChecker::checkParagraphRule(PMString commandName, PMReal stroke, bool8 mode, PMReal lIndent, PMReal rIndent, PMReal offset, int32 type, int32 color, PMReal tint, bool8 on)
{
#if REVERSE_CHECKER

	if(stroke < MIN_PARARULE_STROKE || stroke > MAX_PARARULE_STROKE)
		WriteReverseLog(GetInvalidRangeMsg(MIN_PARARULE_STROKE,MAX_PARARULE_STROKE), commandName, 1);

	if(lIndent < MIN_INDENT || lIndent > MAX_INDENT)
		WriteReverseLog(GetInvalidRangeMsg(MIN_INDENT,MAX_INDENT), commandName, 3);

	if(rIndent < MIN_INDENT || rIndent > MAX_INDENT)
		WriteReverseLog(GetInvalidRangeMsg(MIN_INDENT,MAX_INDENT), commandName, 4);

	if(offset < MIN_OFFSET || offset > MAX_OFFSET)
		WriteReverseLog(GetInvalidRangeMsg(MIN_OFFSET,MAX_OFFSET), commandName, 5);

	if(tint < MIN_TINT || tint > MAX_TINT)
		WriteReverseLog(GetInvalidRangeMsg(MIN_TINT,MAX_TINT), commandName, 8);

#endif
}

void ReverseChecker::chechHyphenation(PMString cjName, bool8 mode, bool8 capitalized, int16 minWord, int16 afterFirst, int16 beforeLast, int16 limit, PMReal zone,
						  bool8 lastWord, PMReal wordMin, PMReal wordDes, PMReal wordMax, PMReal letterMin, PMReal letterDes, PMReal letterMax)
{
#if REVERSE_CHECKER

	if(cjName.IsEmpty())
		WriteReverseLog(PMString(kErrNoString), "CJ", 1);

	if(minWord < MIN_HYPHWORD || minWord > MAX_HYPHWORD)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHWORD,MAX_HYPHWORD), "CJ", 4);

	if(beforeLast < MIN_HYPHCHAR || beforeLast > MAX_HYPHCHAR)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHCHAR,MAX_HYPHCHAR), "CJ", 5);

	if(afterFirst < MIN_HYPHCHAR || afterFirst > MAX_HYPHCHAR)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHCHAR,MAX_HYPHCHAR), "CJ", 6);

	if(limit < MIN_HYPHLIMIT || limit > MAX_HYPHLIMIT)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHLIMIT,MAX_HYPHLIMIT), "CJ", 7);

	if(zone < MIN_HYPHZONE || zone > MAX_HYPHZONE)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHZONE,MAX_HYPHZONE), "CJ", 8);

	if(wordMin < MIN_HYPHWD || wordMin > MAX_HYPHWD)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHWD,MAX_HYPHWD), "CJ", 10);

	if(wordDes < MIN_HYPHWD || wordDes > MAX_HYPHWD)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHWD,MAX_HYPHWD), "CJ", 11);

	if(wordMax < MIN_HYPHWD || wordMax > MAX_HYPHWD)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHWD,MAX_HYPHWD), "CJ", 12);

	if(letterMin < MIN_HYPHCH || letterMin > MAX_HYPHCH)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHCH,MAX_HYPHCH), "CJ", 13);

	if(letterDes < MIN_HYPHCH || letterDes > MAX_HYPHCH)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHCH,MAX_HYPHCH), "CJ", 14);

	if(letterMax < MIN_HYPHCH || letterMax > MAX_HYPHCH)
		WriteReverseLog(GetInvalidRangeMsg(MIN_HYPHCH,MAX_HYPHCH), "CJ", 15);

#endif
}

void ReverseChecker::checkLeading(PMReal leading)
{
#if REVERSE_CHECKER

	if(leading < MIN_LEAD || leading > MAX_LEAD)
		WriteReverseLog(GetInvalidRangeMsg(MIN_LEAD,MAX_LEAD), "PI", 1);

#endif
}

void ReverseChecker::checkKerning(PMReal kerning)
{
#if REVERSE_CHECKER

	if(kerning < MIN_TRACKING || kerning > MAX_TRACKING)
		WriteReverseLog(GetInvalidRangeMsg(MIN_TRACKING,MAX_TRACKING), "TK", 1);

#endif
}

void ReverseChecker::checkFontSize(PMReal size)
{
#if REVERSE_CHECKER

	if(size < MIN_FONTSIZE || size > MAX_FONTSIZE)
		WriteReverseLog(GetInvalidRangeMsg(MIN_FONTSIZE,MAX_FONTSIZE), "TC", 1);

#endif
}

void ReverseChecker::checkGlyphScale(PMString commandName, PMReal scale)
{
#if REVERSE_CHECKER

	if(scale < MIN_XSCALE || scale > MAX_XSCALE)
		WriteReverseLog(GetInvalidRangeMsg(MIN_XSCALE,MAX_XSCALE), commandName, 2);

#endif
}

void ReverseChecker::checkTracking(PMReal tracking)
{
#if REVERSE_CHECKER

	if(tracking < MIN_TRACKING || tracking > MAX_TRACKING)
		WriteReverseLog(GetInvalidRangeMsg(MIN_TRACKING,MAX_TRACKING), "TA", 1);

#endif
}

void ReverseChecker::checkBaselineOffset(PMReal offset)
{
#if REVERSE_CHECKER

	if(offset < MIN_BL || offset > MAX_BL)
		WriteReverseLog(GetInvalidRangeMsg(MIN_BL,MAX_BL), "BL", 1);

#endif
}

void ReverseChecker::checkTextTint(PMReal tint)
{
#if REVERSE_CHECKER

	if(tint < MIN_TINT || tint > MAX_TINT)
		WriteReverseLog(GetInvalidRangeMsg(MIN_TINT,MAX_TINT), "TT", 1);

#endif
}

void ReverseChecker::checkTabPosition(PMReal pos, int32 numParam)
{
#if REVERSE_CHECKER

	if(pos < 0 || pos > 8640)
		WriteReverseLog(GetInvalidRangeMsg(0,8640), "PT", numParam);

#endif
}
