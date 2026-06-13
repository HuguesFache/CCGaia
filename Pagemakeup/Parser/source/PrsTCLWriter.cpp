/*
//	File:	PrsTCLWriter.cpp
//
//	Author:	Trias
//
//	Date:	19/09/2005
//
*/

#include "VCPlugInHeaders.h"								// For MSVC

// Project headers
#include "AttributeBossList.h"
#include "InstStrokeFillID.h"
#include "ScotchRulesID.h"
#include "GlobalDefs.h"
#include "TCLError.h"
#include "PMRect.h"
#include "FontMgrID.h"
#include "HZID.h"

#include "StringUtils.h"

// Interface headers
#include "ITextAttrUID.h"
#include "ITextAttrRealNumber.h"
#include "ITextAttrFont.h"
#include "ITextAttrUnderlineMode.h"
#include "ITextAttrBoolean.h"
#include "ITextAttrCapitalMode.h"
#include "ITextAttrPositionMode.h"
#include "IFontFamily.h"
#include "IDrawingStyle.h"
#include "ITextAttrGridAlignment.h"
#include "ITextAttrInt16.h"
#include "ITextAttrAlign.h"
#include "ICompositionStyle.h"
#include "ITextAttrParaRuleMode.h"
#include "ITextAttrTabSettings.h"
#include "ITextAttrHyphenMode.h"
#include "IHyphenationStyle.h"
#include "ITextAttrStrokeType.h"
#include "IDashedAttributeValues.h"	
#include "ITextAttributes.h"
#include "ITextAttrBreakBeforeMode.h"
#include "IAnchoredObjectData.h"	
#include "ITextAttrClassID.h"
#include "ILanguage.h"
#include "IUnitOfMeasure.h"
#include "IStyleInfo.h"
#include "IFileStreamData.h"
#include "IPermRefsTag.h"
#include "IPMStream.h"
#include "ITCLWriter.h"
#include "ReverseChecker.h"

#include "FileUtils.h"
#include "CAlert.h"
#include "DebugClassUtils.h"

class PrsTCLWriter : public CPMUnknown<ITCLWriter>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsTCLWriter(IPMUnknown* iBoss);
	
	/** Destructor. */
	virtual	~PrsTCLWriter(void);

	virtual void SetStream(IPMStream * TCLStream);

	virtual void CloseStream();
	
	virtual void SetColorList(UID * list);

	virtual void SetExportAttributesPrefs(bool16 exportCJ, bool16 exportParaCmd, bool16 exportTextCmd);

	virtual ReverseChecker * GetReverseChecker();
	
	virtual void writeNewDoc(PMString name, PMReal width, PMReal height, int32 nbCol, PMReal gutter,
							 PMReal left, PMReal top, PMReal right, PMReal bottom, int32 nbPage, int32 pagesPerSpread, bool16 recto,
							 bool8 autoLeading, PMReal autoLeadPercent, LanguageID language, bool8 autoKern);

	virtual void writeDE(PMReal pos, PMReal size);

	virtual void writeDH(PMReal pos, PMReal size);

	virtual void writeDK(PMReal size);
							 
	virtual void writeNewStyle(int32 num, PMString name, int32 basedOnNum, int32 nextStyleNum, bool8 isParaStyle);
	
	virtual void writeEndStyle();
	
	virtual void writeStyleAttributes(const AttributeBossList * list);
	
	virtual PMString writeAttribute(ClassID attrID, const IPMUnknown * inter);

	virtual void writeApplyStylesOrOverrides(const AttributeBossList * list, const AttributeBossList * list2, PMString paraStyleName, PMString charStyleName);
	
	virtual void writeApplyMaster(PMString name, int32 startPage, int32 endPage);
	
	virtual void writeNewColor(int32 colorID, PMString colorName, bool8 isSpot, int32 type, K2Vector<PMReal> values);
	
	virtual void SetDataBase(IDataBase * db);
	
	virtual void writeNewLayer(PMString layerName);
	
	virtual void writeSetCurrentLayer(PMString layerName);
	
	virtual void writeNewTextFrame(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, Text::VerticalJustification vj, PMReal offset, int32 numPage, UID color, PMReal tint,
								   int32 nbCol, PMReal gutter, PMReal angle, PMReal vjMax, Text::FirstLineOffsetMetric topAlign, bool8 isInline, TCLRef refOwner,bool8 toLink,TCLRef refLink);
					
	virtual void writeNewTextSpline(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList, Text::VerticalJustification vj, PMReal offset, int32 numPage, UID color, PMReal tint,
										int32 nbCol, PMReal gutter, PMReal angle, PMReal vjMax, Text::FirstLineOffsetMetric topAlign, bool8 isInline, TCLRef refOwner,bool8 toLink=kFalse,TCLRef refLink=kInvalidTCLRef);

	virtual void writeSetInsets(TCLRef ref, PMReal top, PMReal left, PMReal bottom, PMReal right);

	virtual void writeNonPrint(TCLRef ref, bool8 nonPrint);

	virtual void writeIsLocked(TCLRef ref, bool8 locked);

	virtual void writeFlipItem(TCLRef ref, bool8 flipH, bool8 flipV);

	virtual void writeSkewItem(TCLRef ref, PMReal skewAngle);
	
	virtual void writeSetTextWrap(TCLRef ref, IStandOff::mode mode, PMReal top, PMReal left, PMReal bottom, PMReal right, IStandOff::form form, IStandOffContourWrapSettings::ContourWrapType);
	
	virtual void writeSetStroke(TCLRef ref, PMReal weight, ClassID type, UID color, PMReal tint, ClassID cornerType, PMReal cornerRadius, UID gapColor, PMReal gapTint, int32 strokeAlign);
	
	virtual void writeShadowEffect(TCLRef ref, bool16 mode, int8 blendMode, PMReal xOffset, PMReal yOffset, UID color, PMReal opacity, PMReal radius, PMReal spread, PMReal noise);

	virtual void writeNewImageFrame(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, PMString fullName, PMReal xDec, PMReal yDec, PMReal xScale, PMReal yScale,
										int32 numPage, UID color, PMReal tint, PMReal angle, ClassID cornerType, PMReal cornerRadius, bool8 isInline, TCLRef refOwner);

	virtual void writeNewGraphicSpline(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList,  PMString fullName, PMReal xDec, PMReal yDec, PMReal xScale, PMReal yScale,
										int32 numPage, UID color, PMReal tint, PMReal angle, ClassID cornerType, PMReal cornerRadius, bool8 isInline=kFalse, TCLRef refOwner = kInvalidTCLRef);

	virtual void writeNewRule(TCLRef ref, PMPoint p1, PMPoint p2, PMReal stroke, int32 strokeAlign, ClassID type, UID color, PMReal tint, int32 numPage, ClassID lineEndStart, ClassID lineEndEnd);
	
	virtual void writeSetInlineProperties(TCLRef ref, IAnchoredObjectData::AnchoredPosition pos, PMReal offset);
	
	virtual void writeCreateGroup(K2Vector<TCLRef> itemRefList);

	virtual void writeCreateInlineGroup(TCLRef refOwner, K2Vector<TCLRef> itemRefList);

	virtual void setCurrentParaStyle(UID currentParaStyle);

	virtual void setCurrentCharStyle(UID currentCharStyle);

	virtual void writeApplyParaStyle(PMString paraStyleName);

	virtual void writeApplyCharStyle(PMString charStyleName);
	
	virtual void writeChar(textchar c, bool16& inlineFound, bool16& zeroBreakFound);

	virtual void writeSelectItem(TCLRef ref, bool16 selectTOP);

	virtual void writeNewMaster(PMString masterName, PMString basedOn, int32 nbPages);

	virtual void writeEndMaster();

	virtual void EndText();

	virtual void writeClippingPath(TCLRef ref, int16 clipType, PMReal tolerance, PMReal inset, uint8 threshold, int16 index, bool16 invert, bool16 allowHoles, bool16 limitToFrame, bool16 useHighRes);
	
	virtual void writeTransparencyEffect(TCLRef ref, int8 blendMode, PMReal opacity, bool16 knockoutGroup, bool16 isolationGroup);

	virtual void writeFeatherEffect(TCLRef ref, int8 mode, PMReal width, int8 corner, PMReal noise, PMReal innerOpacity, PMReal outerOpacity);
	
	virtual void writeNewGradient(int32 colorID, PMString colorName, int8 type, K2Vector<int32> refColors, K2Vector<PMReal> stopPositions, K2Vector<PMReal> midPoints); 

	virtual void writeGradientOptions(TCLRef ref, bool16 isFillAttr, PMReal angle, PMReal length, PMPoint center, PMReal radius);

	virtual void writeNewTintedColor(int32 colorID, int32 basedOn, PMReal tint);

	virtual void writeSection(int32 startPage, int32 folio, PMString prefix, int8 type, bool16 autoNumber, bool16 includeSectionPrefix, PMString marker);

	virtual void writeTOP(TCLRef ref, PMReal startPos, PMReal endPos, const TextAlignType textAlign, const PathAlignType pathAlign, const EffectType typeEffect, const int16 overlapOffset, const bool16 flip);
	
	virtual void writeTOC(TCLRef ref, PMString tocStyle);

	virtual void writeTOCStyle(PMString tocStyleName, PMString tocTitle, int32 tocTitleStyle,
								 bool16 includeHidLayer, bool16 runIn, bool16 includeBookMarks, 
								 K2Vector<KeyValuePair<int32, FormatEntryParam> >& entries);

	virtual void writeTiffOptions(TCLRef ref, UID color, PMReal tint);

	virtual void writeHyperlinkTextDest(PMString hlName, bool16 visible);
	virtual void writeHyperlinkPageDest(PMString hlName, int32 pageNumber, bool16 visible);
	virtual void writeHyperlinkURLDest(PMString hlName, PMString url);

	virtual void writeHyperlinkSource(int8 type, PMString hlName, PMString hlDestName, int32 color, int32 hilightType, int32 outlineStyle, 
										  int32 borderWidth, bool16 visible, TextIndex start, TextIndex end);

	virtual void writeBookmark(PMString bkName, PMString bkParent, PMString hlDestName);

	virtual void writeImageInfo(TCLRef ref, PMReal imageWidth, PMReal imageHeight);

	virtual void writeImageProperties(TCLRef ref, PMReal imageAngle, PMReal imageSkew, PMReal xOffset, PMReal yOffset);

private :

	// Private functions
	void WriteTCLRef(PMString& command, TCLRef ref);

	void WriteUTF8(const PMString& command);

	int32 getColorRef(UID colorUID, PMString commandName, int32 numParam);

	int32 getLanguageRef(UID languageUID, PMString commandName, int32 numParam);

	void ResetValues();

	PMString writeDefaultAttribute(ClassID attrID,bool8 styleHasChanged);

	bool8 HandleBreakCharacter(ClassID attrID, const IPMUnknown * breakAttr);

	void writeTG_TJCommands();
	void writeMultiAttributesCommands();

	void RemoveOldOverrides();
	void ApplyOverrides(bool8 styleHasChanged);
	void Flush();
	
	// Private members
	IPMStream * TCLStream;
	IDataBase * curDB;
	UID * colorList;
	bool16 exportCJ, exportParaCmd, exportTextCmd;

	WideString text;

	ITextAttributes * currentParaStyle;
	ITextAttributes * currentCharStyle;
	
	AttributeBossList * currentOverrides, * newOverrides;

	Text::StartBreakLocation breakCharacter;

	const PMString separator;
	const PMString trueS;
	const PMString falseS;
	
	int32 valTJ, valTG;
	bool8 attrTJ, attrTG;
	
	int16 nbChar, nbLine;
	bool8 attrPL;
	
	PMReal spaceBef, spaceAft;
	bool8 attrPE;
	
	bool8 keepLines, keepAll;
	int16 firstLines, lastLines;
	bool8 attrPH;
	
	PMReal strokePA, lIndentPA, rIndentPA, offsetPA, tintPA;
	int32 typePA, colorPA;
	bool8 modePA, PAon;
	bool8 attrPA;
	
	PMReal strokePP, lIndentPP, rIndentPP, offsetPP, tintPP;
	int32 typePP, colorPP;
	bool8 modePP, PPon;
	bool8 attrPP;
	
	PMReal indentFirst, indentLeft, indentRight;
	bool8 attrPR;
	
	const PMString cjName;
	bool8 hyphenMode, capitalized, lastWord;
	int16 minWord, afterFirst, beforeLast, limit;
	PMReal hyphenZone, wordMin, wordMax, wordDes, letterMin, letterMax, letterDes;
	bool8 attrCJ;

	bool8 permrefFound; // To handle bug regarding null permrefs

	/** Added for mediagerance : check command's parameters consistency */
	ReverseChecker * checker;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PrsTCLWriter, kPrsTCLWriterImpl)

/* Constructor
*/
PrsTCLWriter::PrsTCLWriter(IPMUnknown* iBoss) :
	CPMUnknown<ITCLWriter>(iBoss), separator(";"), trueS("TRUE"), falseS("FALSE"), cjName("ReverseTCL_CJ_Rule"), permrefFound(kFalse)
{
	currentOverrides = new AttributeBossList();
	newOverrides = new AttributeBossList();
	currentParaStyle = nil;
	currentCharStyle = nil;
	TCLStream = nil;

	exportCJ = exportParaCmd = exportTextCmd = kTrue;

	breakCharacter = Text::kNoForcedBreak;

	checker = nil;
}

/* Destructor
*/
PrsTCLWriter::~PrsTCLWriter(void)
{
	if(TCLStream != nil)
	{
		if(TCLStream->GetStreamState() != kStreamStateClosed)
			TCLStream->Close();
		
		TCLStream->Release();
	}
		
	delete currentOverrides;
	delete newOverrides;

	if(currentParaStyle != nil)
		currentParaStyle->Release();

	if(currentCharStyle != nil)
		currentCharStyle->Release();

	if(checker)
		delete checker;
}

/* SetStream
*/
void PrsTCLWriter::SetStream(IPMStream * TCLStream)
{
	TCLStream->AddRef();
	this->TCLStream = TCLStream;

	InterfacePtr<IFileStreamData> TCLStreamData (TCLStream, IID_IFILESTREAMDATA);
	IDFile reverseTCLFile = TCLStreamData->GetSysFile();

	checker = new ReverseChecker(reverseTCLFile);
}

/* CloseStream
*/
void PrsTCLWriter::CloseStream()
{
	if(TCLStream != nil)
		TCLStream->Close();
}

/* SetColorList
*/
void PrsTCLWriter::SetColorList(UID * list)
{
	this->colorList = list;
}

/* SetDataBase
*/
void PrsTCLWriter::SetDataBase(IDataBase * db)
{
	this->curDB = db;
}

/* SetExportAttributesPrefs
*/
void PrsTCLWriter::SetExportAttributesPrefs(bool16 exportCJ, bool16 exportParaCmd, bool16 exportTextCmd)
{
	this->exportCJ = exportCJ;
	this->exportParaCmd = exportParaCmd;
	this->exportTextCmd = exportTextCmd;
}

ReverseChecker * PrsTCLWriter::GetReverseChecker()
{
	return this->checker;
}

#if REF_AS_STRING

inline void PrsTCLWriter::WriteTCLRef(PMString& command, TCLRef ref) { command += "\"" + ref + "\""; }

#else

inline void PrsTCLWriter::WriteTCLRef(PMString& command, TCLRef ref) { command.AppendNumber(ref); }

#endif

/* getColorRef
*/
int32 PrsTCLWriter::getColorRef(UID colorUID, PMString commandName, int32 numParam)
{
	if(colorUID == kInvalidUID)
		return -1; // default if it's not a color

	for(int32 ref = 0 ; ref < MAX_COLOR + 1 ; ++ref)
	{
		if(colorList[ref] == colorUID)
			return ref;
	}
	
	if(commandName != kNullString)
		checker->WriteReverseLog(PMString(kErrUnknownColor), commandName, numParam);

	return 9; // None color index if not found
}

int32 PrsTCLWriter::getLanguageRef(UID languageUID, PMString commandName, int32 numParam)
{
	int8 ref = 0;
	
	InterfacePtr<ILanguage> lang (curDB, languageUID,UseDefaultIID());
	if(lang)
	{
		LanguageID langID = lang->GetLanguageID(); 
		switch(GetPrimaryLanguageID(langID))
		{
			case kLanguageEnglish :
				if(GetSubLanguageID(langID) == kSubLanguageEnglishUS)
					ref = 0;
				else if(GetSubLanguageID(langID) == kSubLanguageEnglishUK)
					ref = 2;
				else if(GetSubLanguageID(langID) == kSubLanguageEnglishAustralian)
					ref = 15;
				else if(GetSubLanguageID(langID) == kSubLanguageEnglishEire)
					ref = 50;
				else if(GetSubLanguageID(langID) == kSubLanguageEnglishCanadian)
					ref = 82;
				else
				{
					ref = 0;
					checker->WriteReverseLog(PMString(kErrUnknownLanguage), commandName, numParam);
				}
				break;

			case kLanguageFrench : 
				if(GetSubLanguageID(langID) == kSubLanguageFrench)
					ref = 1;
				else if(GetSubLanguageID(langID) == kSubLanguageFrenchCanadian)
					ref = 11;
				else if(GetSubLanguageID(langID) == kSubLanguageFrenchSwiss)
					ref = 18; 
				else if(GetSubLanguageID(langID) == kLanguageFrench)
					ref = 98; 
				else
				{
					ref = 1;
					checker->WriteReverseLog(PMString(kErrUnknownLanguage), commandName, numParam);
				}
				break;
			case kLanguageGerman : 
				if(GetSubLanguageID(langID) == kSubLanguageGerman)
					ref = 3;
				else if(GetSubLanguageID(langID) == kSubLanguageGermanSwiss)
					ref = 19;
				else if(GetSubLanguageID(langID) == kSubLanguageGermanAustrian)
					ref = 92;
				else
				{
					ref = 3;
					checker->WriteReverseLog(PMString(kErrUnknownLanguage), commandName, numParam);
				}
				break;
			case kLanguageItalian : 
				if(GetSubLanguageID(langID) == kSubLanguageItalian)
					ref = 4; 
				else if(GetSubLanguageID(langID) == kSubLanguageItalianSwiss)
					ref = 36;
				else
					ref = 4;
				break;
			case kLanguageDutch : ref = 5; break;
			case kLanguageSwedish : ref = 7; break;
			case kLanguageSpanish : ref = 8; break;
			case kLanguageDanish : ref = 9; break;
			case kLanguagePortuguese : 
				if(GetSubLanguageID(langID) == kSubLanguagePortuguese)
					ref = 10;
				else if(GetSubLanguageID(langID) == kSubLanguagePortugueseBrazilian)
					ref = 71;
				else
					ref = 10; 
				break;
			case kLanguageNorwegian : 
				if(GetSubLanguageID(langID) == kSubLanguageNorwegianBokmal)
					ref = 12;
				else if(GetSubLanguageID(langID) == kSubLanguageNorwegianNynorsk)
					ref = 101;
				else
					ref = 12; 
				break;
			case kLanguageHebrew : ref = 13; break;
			case kLanguageJapanese : ref = 14; break;
			case kLanguageArabic : ref = 16; break;
			case kLanguageFinnish : ref = 17; break;
			case kLanguageGreek : ref = 20; break;
			case kLanguageIcelandic : ref = 21; break;
			case kLanguageMaltese : ref = 22; break;
			case kLanguageTurkish : ref = 24; break;
			case kLanguageCroatian : ref = 25; break;
			case kLanguageHindi : ref = 33; break;
			case kLanguageUrdu : 
				if(GetSubLanguageID(langID) == kSubLanguageUrduPakistan)
					ref = 34;
				else if(GetSubLanguageID(langID) == kSubLanguageUrduIndia)
					ref = 96;
				else				
					ref = 34;			
				break;
			case kLanguageRomanian : ref = 39; break;
			case kLanguageLithuanian : ref = 41; break;
			case kLanguagePolish : ref = 42; break;
			case kLanguageHungarian : ref = 43; break;
			case kLanguageEstonian : ref = 44; break;
			case kLanguageLatvian : ref = 45; break;
			case kLanguageSami : ref = 46; break;
			case kLanguageFarsi : ref = 48; break;
			case kLanguageRussian : ref = 49; break;
			case kLanguageKorean : ref =51; break;
			case kLanguageChinese : 
				if(GetSubLanguageID(langID) == kSubLanguageChineseSimplified)
					ref = 52;
				else if(GetSubLanguageID(langID) == kSubLanguageChineseTraditional)
					ref = 53;
				else if(GetSubLanguageID(langID) == kSubLanguageChineseSingapore)
					ref = 100;
				else
				{
					ref = 52;
					checker->WriteReverseLog(PMString(kErrUnknownLanguage), commandName, numParam);
				}
				break;
			case kLanguageThai : ref = 54; break;
			case kLanguageCzech : ref = 56; break;
			case kLanguageSlovak : ref = 57; break;
			case kLanguageBengali : ref = 60; break;
			case kLanguageByeloRussian : ref = 61; break;
			case kLanguageUkrainian : ref = 62; break;
			case kLanguageSerbian : ref = 65; break;
			case kLanguageSlovenian : ref = 66; break;
			case kLanguageMacedonian : ref = 67; break;
			case kLanguageBulgarian  : ref = 72; break;
			case kLanguageCatalan : ref = 73; break;
			case kLanguageGaelic : 
				if(GetSubLanguageID(langID) == kSubLanguageGaelicScotland)
					ref = 75;
				else if(GetSubLanguageID(langID) == kSubLanguageGaelicIreland)
					ref = 81;
				else
					ref = 75; 
				break;
			case kLanguageArmenian : ref = 84; break;
			case kLanguageGeorgian : ref = 85; break;
			case kLanguageGujarati : ref = 94; break;
			case kLanguagePunjabi : ref = 95; break;
			case kLanguageVietnamese : ref = 97; break;
			
			default : 
				checker->WriteReverseLog(PMString(kErrUnknownLanguage), commandName, numParam);			
				break;
		}		
	}
	return ref;
}

void PrsTCLWriter::writeNewDoc(PMString name, PMReal width, PMReal height, int32 nbCol, PMReal gutter,
							   PMReal left, PMReal top, PMReal right, PMReal bottom, int32 nbPage, int32 pagesPerSpread, bool16 recto,
							   bool8 autoLeading, PMReal autoLeadPercent, LanguageID language, bool8 autoKern)
{
	// Convert values to millimeters
	InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));
	
	width = uom->PointsToUnits(width);
	height = uom->PointsToUnits(height);
	gutter = uom->PointsToUnits(gutter);
	left = uom->PointsToUnits(left);
	top = uom->PointsToUnits(top);
	right = uom->PointsToUnits(right);
	bottom = uom->PointsToUnits(bottom);

	PMString command("[DN=");
	command += "\"" + name + "\"" + separator;
	
	PMString widthS("N"); widthS.AppendNumber(width,3,kTrue,kTrue);
	PMString heightS("N"); heightS.AppendNumber(height,3,kTrue,kTrue);
	PMString nbColS; nbColS.AppendNumber(nbCol);
	PMString gutterS("N"); gutterS.AppendNumber(gutter,3,kTrue,kTrue);
	
	PMString leftS("N"); leftS.AppendNumber(left,3,kTrue,kTrue);
	PMString topS("N"); topS.AppendNumber(top,3,kTrue,kTrue);
	PMString rightS("N"); rightS.AppendNumber(right,3,kTrue,kTrue);
	PMString bottomS("N"); bottomS.AppendNumber(bottom,3,kTrue,kTrue); 
	PMString nbPageS; nbPageS.AppendNumber(nbPage); 
	
	command += widthS + ";";
	command += heightS + ";";
	command += nbColS + ";";
	command += gutterS + ";";
	
	command += leftS + ";";
	command += rightS + ";";
	command += topS + ";";
	command += bottomS + ";";
	command += nbPageS + ";";
	
	if(recto)
		command += "TRUE;";
	else
		command += "FALSE;";

	if(autoLeading)
		command += "TRUE;";
	else
		command += "FALSE;";

	command.AppendNumber(autoLeadPercent,3,kTrue,kTrue);
	command += ";";

	if(GetPrimaryLanguageID(language) == kLanguageFrench)
		command += "0;";
	else if(GetPrimaryLanguageID(language) == kLanguageEnglish)
		command += "1;";
	else if(GetPrimaryLanguageID(language) == kLanguageGerman)
		command += "2;";
	else
	{
		command += "0;";
		checker->WriteReverseLog(PMString(kErrUnknownLanguage), "DN", 14);
	}

	if(autoKern)
		command += "TRUE;";
	else
		command += "FALSE;";
	
	command.AppendNumber(pagesPerSpread);

	command += "]"; 
	command += kNewLine;
	//command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkNewDoc(name,width,height,nbCol,gutter,left,top,right,bottom,nbPage,pagesPerSpread,autoLeading,autoLeadPercent,language,autoKern);
	checker->NewLine();
}

void PrsTCLWriter::writeDE(PMReal pos, PMReal size)
{
	PMString command("[DE=");

	command.AppendNumber(pos,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(size,3,kTrue,kTrue);

	command += "]"; 
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkDE(pos,size);
	checker->NewLine();
}

void PrsTCLWriter::writeDH(PMReal pos, PMReal size)
{
	PMString command("[DH=");

	command.AppendNumber(pos,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(size,3,kTrue,kTrue);

	command += "]"; 
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkDH(pos,size);
	checker->NewLine();
}

void PrsTCLWriter::writeDK(PMReal size)
{
	PMString command("[DK=");

	command.AppendNumber(size,3,kTrue,kTrue);

	command += "]"; 
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkDK(size);
	checker->NewLine();
}		

void PrsTCLWriter::writeNewColor(int32 colorID, PMString colorName, bool8 isSpot, int32 type, K2Vector<PMReal> values)
{
	PMString command("[CN=");
	
	command.AppendNumber(colorID);
	command += ";\"" + colorName + "\";;";
	switch(type)
	{
		case CCID_CMYK :
			command.AppendNumber(values[0] * 65535,0,kTrue,kTrue); command += ";";
			command.AppendNumber(values[1] * 65535,0,kTrue,kTrue); command += ";";
			command.AppendNumber(values[2] * 65535,0,kTrue,kTrue); command += ";";
			command.AppendNumber(values[3] * 65535,0,kTrue,kTrue); command += ";";
			command += ";;;";
			command.AppendNumber(CCID_CMYK);
			break;

		case CCID_RGB :
			command += ";;;;";
			command.AppendNumber(values[0] * 65535,0,kTrue,kTrue); command += ";";
			command.AppendNumber(values[1] * 65535,0,kTrue,kTrue); command += ";";
			command.AppendNumber(values[2] * 65535,0,kTrue,kTrue); command += ";";
			command.AppendNumber(CCID_RGB);
			break;
						
		case CCID_LAB :
			command.AppendNumber(values[0],0,kTrue,kTrue); command += ";";
			command.AppendNumber(values[1] + 128,0,kTrue,kTrue); command += ";";
			command.AppendNumber(values[2] + 128,0,kTrue,kTrue); command += ";";
			command += ";;;;";
			command.AppendNumber(CCID_LAB);
			break;
	}

	command += ";\"\";";

	if(isSpot)
		command += "TRUE";
	else
		command += "FALSE";
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkNewColor(colorID,colorName,isSpot,type,values);
	checker->NewLine();
}

void PrsTCLWriter::writeNewStyle(int32 num, PMString name, int32 basedOnNum, int32 nextStyleNum, bool8 isParaStyle)
{
	//cjName = name; // set cj rule name, in case style contains hyphen rules
	
	PMString command("[SD=");
	
	command.AppendNumber(num);
	command += ";";
	command += "\"" + name + "\";";
	command.AppendNumber(nextStyleNum);
	command += ";";
	command.AppendNumber(basedOnNum);
	if(isParaStyle)
		command += ";TRUE";
	
	command += "]";
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);
	checker->checkNewStyle(num,name,basedOnNum,nextStyleNum,isParaStyle);
}

void PrsTCLWriter::writeEndStyle()
{
	PMString command("[SF]");
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::writeNewLayer(PMString name)
{
	PMString command("[LN=");
	
	command += "\"" + name + "\"";
	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkNewLayer(name);
	checker->NewLine();
}

void PrsTCLWriter::writeApplyMaster(PMString name, int32 startPage, int32 endPage)
{
	PMString command("[MA=");
	
	command += "\"" + name + "\";";
	command.AppendNumber(startPage);
	command += ";";
	command.AppendNumber(endPage);
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::writeSetCurrentLayer(PMString name)
{
	PMString command("[LS=");
	
	command += "\"" + name + "\"";
	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkSetCurrentLayer(name);
	checker->NewLine();
}

void PrsTCLWriter::writeApplyStylesOrOverrides(const AttributeBossList * list, const AttributeBossList * list2, PMString paraStyleName, PMString charStyleName)
{
	Flush(); //Flush text buffer

	attrPL = attrTJ = attrTG = attrPE = attrPH = attrPA = attrPP = attrPR = attrCJ = kFalse;
	valTG = valTJ = 0;	

	// Merge both lists into a single one
	newOverrides->ApplyAttributes(list);
	newOverrides->ApplyAttributes(list2);

	//// -------------------------- DEBUG
	//PMString info("new ovverides \r\n");
	//for(int i = 0 ; i < newOverrides->CountBosses() ; ++i)
	//{
	//	const IPMUnknown * attr = newOverrides->QueryBossN(i, IID_IUNKNOWN);
	//	
	//	DebugClassUtilsBuffer buff;
	//	DebugClassUtils::GetIDName(&buff, ::GetClass(attr));
	//	info.Append(buff); info += "\r\n";
	//	attr->Release();
	//}
	//CAlert::InformationAlert(info); info.Clear();
	//// -------------------------------	

	bool16 styleApplied = kFalse;
	RemoveOldOverrides();
	if(paraStyleName != "")
	{
		styleApplied = kTrue;
		writeApplyParaStyle(paraStyleName);
	}
	if(charStyleName != "")
	{
		styleApplied = kTrue;
		writeApplyCharStyle(charStyleName);
	}
	ApplyOverrides(styleApplied);

	AttributeBossList * tmp = currentOverrides;
	currentOverrides = newOverrides;
	newOverrides = tmp;
	newOverrides->ClearAllOverrides();
}


void PrsTCLWriter::RemoveOldOverrides()
{
	int32 nbBosses = currentOverrides->CountBosses();
	PMString commands;	

	// Check for removed attributes
	for(int i = 0 ; i < nbBosses ; ++i)
	{
		ClassID attrBoss = currentOverrides->GetClassN(i);
		const IPMUnknown * attr = currentOverrides->QueryByClassID(attrBoss, IID_IUNKNOWN);
			
		if(newOverrides->ContainsAttribute(attrBoss) == kFalse) // Attribute isn't applied anymore, remove it
			commands += writeDefaultAttribute(attrBoss, /*styleHasChanged*/ kFalse);

		attr->Release();
	}

	writeTG_TJCommands();

	TCLStream->XferByte((uchar *) commands.GetPlatformString().c_str(), commands.GetPlatformString().size()); 
}


void PrsTCLWriter::ApplyOverrides(bool8 styleHasChanged)
{
	PMString commands;
	// Update overrides and new ones
	int32 nbBosses = newOverrides->CountBosses();
	for(int i = 0 ; i < nbBosses ; ++i)
	{
		ClassID attrBoss = newOverrides->GetClassN(i);
		const IPMUnknown * attr = newOverrides->QueryByClassID(attrBoss, IID_IUNKNOWN);
		
		if(currentOverrides->ContainsExact(curDB,attr) == kFalse || styleHasChanged)
		{
			if(!HandleBreakCharacter(attrBoss, attr))	
				commands += writeAttribute(attrBoss, attr);
		}
		attr->Release();
	}

	if(!exportCJ)
		attrCJ = kFalse;

	if(!exportParaCmd)
	{
		attrPL = attrPE = attrPH = attrPA = attrPP = attrPR = kFalse;
	}
	
	writeTG_TJCommands();
	writeMultiAttributesCommands();

	TCLStream->XferByte((uchar *) commands.GetPlatformString().c_str(), commands.GetPlatformString().size()); 
}

void PrsTCLWriter::writeTG_TJCommands()
{
	PMString commands;
	if(attrTG)
	{
		commands += "[TG=";
		commands.AppendNumber(valTG);
		commands += "]";
		attrTG = kFalse;
		valTG = 0;
	}

	if(attrTJ)
	{
		commands += "[TJ=";
		commands.AppendNumber(valTJ);
		commands += "]";
		attrTJ = kFalse;
		valTJ = 0;
	}
	TCLStream->XferByte((uchar *) commands.GetPlatformString().c_str(), commands.GetPlatformString().size()); 
}

void PrsTCLWriter::writeMultiAttributesCommands()
{
	PMString commands;
		
	if(attrPL)
	{
		commands += "[PL=";
		commands.AppendNumber(nbChar);
		commands += ";";
		commands.AppendNumber(nbLine);
		commands += "]";
		attrPL = kFalse;
	}	
	
	if(attrPE)
	{
		commands += "[PE=D";
		commands.AppendNumber(spaceBef,3,kTrue,kTrue);
		commands += ";D";
		commands.AppendNumber(spaceAft,3,kTrue,kTrue);
		commands += "]";
		attrPE = kFalse;
	}
	
	if(attrPH)
	{
		commands += "[PH=";
		if(keepLines)
			commands += trueS;
		else
			commands += falseS;
		commands += ";";
		if(keepAll)
			commands += trueS;
		else
			commands += falseS;
		commands += ";";
		commands.AppendNumber(firstLines);
		commands += ";";
		commands.AppendNumber(lastLines);
		commands += "]";
		attrPH = kFalse;
	}
	
	if(attrPR)
	{
		commands += "[PR=D";
		commands.AppendNumber(indentLeft,3,kTrue,kTrue);
		commands += ";D";
		commands.AppendNumber(indentRight,3,kTrue,kTrue);
		commands += ";D";
		commands.AppendNumber(indentFirst,3,kTrue,kTrue);
		commands += "]";
		attrPR = kFalse;
	}
	
	if(attrPA)
	{
		commands += "[PA=D";
		commands.AppendNumber(strokePA,3,kTrue,kTrue);
		commands += ";";
		if(modePA)
			commands += trueS;
		else
			commands += falseS;
		commands += ";D";
		commands.AppendNumber(lIndentPA,3,kTrue,kTrue);
		commands += ";D";
		commands.AppendNumber(rIndentPA,3,kTrue,kTrue);
		commands += ";D";
		commands.AppendNumber(offsetPA,3,kTrue,kTrue);
		commands += ";";
		commands.AppendNumber(typePA);
		commands += ";";
		commands.AppendNumber(colorPA);
		commands += ";";
		commands.AppendNumber(tintPA);
		if(!PAon)
			commands += ";FALSE";
		commands += "]";
		attrPA = kFalse;
	}
	
	if(attrPP)
	{
		commands += "[PP=D";
		commands.AppendNumber(strokePP,3,kTrue,kTrue);
		commands += ";";
		if(modePP)
			commands += trueS;
		else
			commands += falseS;
		commands += ";D";
		commands.AppendNumber(lIndentPP,3,kTrue,kTrue);
		commands += ";D";
		commands.AppendNumber(rIndentPP,3,kTrue,kTrue);
		commands += ";D";
		commands.AppendNumber(offsetPP,3,kTrue,kTrue);
		commands += ";";
		commands.AppendNumber(typePP);
		commands += ";";
		commands.AppendNumber(colorPP);
		commands += ";";
		commands.AppendNumber(tintPP);
		if(!PPon)
			commands += ";FALSE";
		commands += "]";
		attrPP = kFalse;
	}
	
	if(attrCJ)
	{
		commands += "[CJ=\"" + cjName + "\";";
		if(hyphenMode)
			commands += trueS;
		else
			commands += falseS;
		commands += ";";
		
		if(capitalized)
			commands += trueS;
		else
			commands += falseS;
		commands += ";";
		
		commands.AppendNumber(minWord);
		commands += ";";
		commands.AppendNumber(afterFirst);
		commands += ";";
		commands.AppendNumber(beforeLast);
		commands += ";";
		commands.AppendNumber(limit);
		commands += ";D";
		commands.AppendNumber(hyphenZone,3,kTrue,kTrue);
		commands += ";";
		
		if(lastWord)
			commands += trueS;
		else
			commands += falseS;
		commands += ";";
		
		commands.AppendNumber(wordMin,1,kTrue,kTrue);
		commands += ";";
		commands.AppendNumber(wordDes,1,kTrue,kTrue);
		commands += ";";
		commands.AppendNumber(wordMax,1,kTrue,kTrue);
		commands += ";";
		commands.AppendNumber(letterMin,1,kTrue,kTrue);
		commands += ";";
		commands.AppendNumber(letterDes,1,kTrue,kTrue);
		commands += ";";
		commands.AppendNumber(letterMax,1,kTrue,kTrue);
		commands += "]";
		// Add hyphen rules to the paragraph style being created
		commands += "[PC=\"" + cjName + "\"]";
		attrCJ = kFalse;
	}	

	TCLStream->XferByte((uchar *) commands.GetPlatformString().c_str(), commands.GetPlatformString().size()); 

	checker->checkDropCaps(nbChar, nbLine);
	checker->checkSpacing(spaceBef, spaceAft);
	checker->checkKeepOptions(keepLines, keepAll, firstLines, lastLines);
	checker->checkIndents(indentLeft, indentRight, indentFirst);
	checker->checkParagraphRule("PA", strokePA, modePA, lIndentPA, rIndentPA, offsetPA, typePA, colorPA, tintPA, PAon);
	checker->checkParagraphRule("PP", strokePP, modePP, lIndentPP, rIndentPP, offsetPP, typePP, colorPP, tintPP, PPon);
	checker->chechHyphenation(cjName, hyphenMode, capitalized, minWord, afterFirst, beforeLast, limit, hyphenZone, lastWord, wordMin, wordDes, wordMax, letterMin, letterDes, letterMax);
}

bool8 PrsTCLWriter::HandleBreakCharacter(ClassID attrID, const IPMUnknown * breakAttr)
{
	bool8 found = kFalse;
	if(attrID == kTextAttrGotoNextXBoss)
	{
		InterfacePtr<ITextAttrBreakBeforeMode> breakBeforAttr(breakAttr, UseDefaultIID()); 
		breakCharacter = breakBeforAttr->Get(); 
		found = kTrue;
	}
	return found;
}

PMString PrsTCLWriter::writeDefaultAttribute(ClassID attrID, bool8 styleHasChanged)
{
	PMString command;
	PMString error(kErrNilInterface);
	ErrorCode status = kFailure;
	
	do
	{
		if(attrID == kTextAttrFontStyleBoss && exportTextCmd)
		{
			ITextAttrFont * txtAttrFontStyle = (ITextAttrFont *) currentCharStyle->QueryByClassID(kTextAttrFontStyleBoss, IID_ITEXTATTRFONT);
			if(txtAttrFontStyle == nil)
				txtAttrFontStyle = (ITextAttrFont *) currentParaStyle->QueryByClassID(kTextAttrFontStyleBoss, IID_ITEXTATTRFONT);
			
			if(txtAttrFontStyle != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrFontStyle->Release();
				break;
			}
			PMString style("Regular");
			if(txtAttrFontStyle != nil)
			{
				style = txtAttrFontStyle->GetFontName();
				txtAttrFontStyle->Release();
			}
			command += "[TM=\"";
			command += style;
			command += "\"]";
		}
		else if(attrID == kTextAttrUnderlineBoss && exportTextCmd)
		{
			ITextAttrUnderlineMode * txtAttrUnder = (ITextAttrUnderlineMode *) currentCharStyle->QueryByClassID(kTextAttrUnderlineBoss, IID_ITEXTATTRUNDERLINEMODE );
			if(txtAttrUnder == nil)
				txtAttrUnder = (ITextAttrUnderlineMode *) currentParaStyle->QueryByClassID(kTextAttrUnderlineBoss, IID_ITEXTATTRUNDERLINEMODE );
			if(txtAttrUnder != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrUnder->Release();
				break;
			}
			IDrawingStyle::UnderlineMode mode = IDrawingStyle::kUnderlineNone;
			if(txtAttrUnder != nil)
			{
				mode = txtAttrUnder->GetMode();
				txtAttrUnder->Release();
			}

			if(mode == IDrawingStyle::kUnderlineSingle)
			{
				valTG += 4;
				attrTG = kTrue;
			}
			else
			{
				valTJ += 4;
				attrTJ = kTrue;
			}
		}
		else if(attrID == kTextAttrStrikethruBoss && exportTextCmd)
		{
			ITextAttrBoolean * txtAttrStrikeThru = (ITextAttrBoolean *) currentCharStyle->QueryByClassID(kTextAttrStrikethruBoss, IID_ITEXTATTRBOOLEAN );
			if(txtAttrStrikeThru == nil)
				txtAttrStrikeThru = (ITextAttrBoolean *) currentParaStyle->QueryByClassID(kTextAttrStrikethruBoss, IID_ITEXTATTRBOOLEAN );
			if(txtAttrStrikeThru != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrStrikeThru->Release();
				break;
			}
			bool8 strikeThru = kFalse;
			if(txtAttrStrikeThru != nil)
			{
				strikeThru = txtAttrStrikeThru->Get();
				txtAttrStrikeThru->Release();
			}

			if(strikeThru)
			{
				valTG += 16;
				attrTG = kTrue;
			}
			else
			{
				valTJ += 16;
				attrTJ = kTrue;
			}
		}
		else if(attrID == kTextAttrCapitalModeBoss && exportTextCmd)
		{
			ITextAttrCapitalMode * txtAttrCapital = (ITextAttrCapitalMode *) currentCharStyle->QueryByClassID(kTextAttrCapitalModeBoss, IID_ITEXTATTRCAPITALMODE );
			if(txtAttrCapital == nil)
				txtAttrCapital = (ITextAttrCapitalMode *) currentParaStyle->QueryByClassID(kTextAttrCapitalModeBoss, IID_ITEXTATTRCAPITALMODE );
			
			if(txtAttrCapital != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrCapital->Release();
				break;
			}
			IDrawingStyle::CapitalizeMode mode = IDrawingStyle::kCapNormal;
			if(txtAttrCapital != nil)
			{
				mode = txtAttrCapital->GetMode();
				txtAttrCapital->Release();
			}

			if(mode == IDrawingStyle::kCapNormal)
			{
				valTJ += 128;
				attrTJ = kTrue;
			}
			else
			{
				if(mode == IDrawingStyle::kCapAll)
					valTG += 128;
				else if(mode == IDrawingStyle::kCapSmallLowercase)
					valTG += 256;
				else
					checker->WriteReverseLog(PMString(kErrUnknownCapMode), "TG", 1);

				attrTG = kTrue;
			}
		}
		else if(attrID == kTextAttrPositionModeBoss && exportTextCmd)
		{
			ITextAttrPositionMode * txtAttrPos = (ITextAttrPositionMode *) currentCharStyle->QueryByClassID(kTextAttrPositionModeBoss, IID_ITEXTATTRPOSITIONMODE );
			if(txtAttrPos == nil)
				txtAttrPos = (ITextAttrPositionMode *) currentParaStyle->QueryByClassID(kTextAttrPositionModeBoss, IID_ITEXTATTRPOSITIONMODE);
			
			if(txtAttrPos != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrPos->Release();
				break;
			}
			IDrawingStyle::PositionMode mode = IDrawingStyle::kPosNormal;
			if(txtAttrPos != nil)
			{
				mode = txtAttrPos->GetMode();
				txtAttrPos->Release();
			}

			if(mode == IDrawingStyle::kPosNormal)
			{
				valTJ += 512;
				attrTJ = kTrue;
			}
			else
			{
				if(mode == IDrawingStyle::kPosSuperscript)
					valTG += 512;
				else if(mode == IDrawingStyle::kPosSubscript)
					valTG += 1024;
				else if(mode == IDrawingStyle::kPosSuperior)
					valTG += 2048;
				else if(mode == IDrawingStyle::kPosInferior)
					valTG += 4096;
				else
					checker->WriteReverseLog(PMString(kErrUnknownTextPosition), "TG", 1);
				attrTG = kTrue;
			}
		}
		else if(attrID == kTextAttrNoBreakBoss && exportTextCmd)
		{
			ITextAttrBoolean * txtAttrNoBreak = (ITextAttrBoolean *) currentCharStyle->QueryByClassID(kTextAttrNoBreakBoss, IID_ITEXTATTRBOOLEAN );
			if(txtAttrNoBreak == nil)
				txtAttrNoBreak = (ITextAttrBoolean *) currentParaStyle->QueryByClassID(kTextAttrNoBreakBoss, IID_ITEXTATTRBOOLEAN );
			
			if(txtAttrNoBreak != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrNoBreak->Release();
				break;
			}
			bool8 noBreak = kFalse;
			if(txtAttrNoBreak != nil)
			{
				noBreak = txtAttrNoBreak->Get();
				txtAttrNoBreak->Release();
			}

			if(noBreak)
			{
				valTG += 8192;
				attrTG = kTrue;
			}
			else
			{
				valTJ += 8192;
				attrTJ = kTrue;
			}
		}
		else if(attrID == kTextAttrLeadBoss && exportParaCmd)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			command += "[PI=D";
			if(txtAttrReal != nil)
			{
				command.AppendNumber(txtAttrReal->GetRealNumber(),3,kTrue,kTrue);
				checker->checkLeading(txtAttrReal->GetRealNumber());
				txtAttrReal->Release();
			}
			else
				command += "0";

			command += "]";
		}
		else if(attrID == kTextAttrPairKernBoss && exportTextCmd)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPairKernBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPairKernBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			command += "[TK=";
			if(txtAttrReal != nil)
			{
				PMReal kerning = txtAttrReal->GetRealNumber();
				if(kerning == kAutoKernMagicNumber)
					command += "0;TRUE";
				else
				{
					command.AppendNumber(kerning * 1000.0,3,kTrue,kTrue);
					checker->checkKerning(kerning * 1000.0);
				}

				txtAttrReal->Release();
			}
			else
				command += "0;TRUE";

			command += "]";
		}
		else if(attrID == kTextAttrColorBoss && exportTextCmd)
		{
			ITextAttrUID * txtAttrUID = (ITextAttrUID *) currentCharStyle->QueryByClassID(kTextAttrColorBoss, IID_ITEXTATTRUID);
			if(txtAttrUID == nil)
				txtAttrUID = (ITextAttrUID *) currentParaStyle->QueryByClassID(kTextAttrColorBoss, IID_ITEXTATTRUID);

			if(txtAttrUID != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrUID->Release();
				break;
			}
			command += "[TD=";
			if(txtAttrUID != nil)
			{
				command.AppendNumber(getColorRef(txtAttrUID->GetUIDData(), "TD", 1));
				txtAttrUID->Release();
			}
			else
				command += "5";
			command += "]";
		}
		else if(attrID == kTextAttrPairKernMethodBoss && exportTextCmd)
		{
			ITextAttrClassID * txtAttrClass = (ITextAttrClassID *) currentCharStyle->QueryByClassID(kTextAttrPairKernMethodBoss, IID_ITEXTATTRCLASSID);
			if(txtAttrClass == nil)
				txtAttrClass = (ITextAttrClassID *) currentParaStyle->QueryByClassID(kTextAttrPairKernMethodBoss, IID_ITEXTATTRCLASSID);

			if(txtAttrClass != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrClass->Release();
				break;
			}
			command += "[TY=";
			if(txtAttrClass != nil)
			{
				ClassID method = txtAttrClass->Get();
				if(method == kDefaultPairKerningBoss)
					command += "0";
				else if(method == kKFPairKerningBoss)
					command += "1";
				else if(method == kPairKerningOffBoss)
					command += "2";
				else
				{
					command += "0";
					checker->WriteReverseLog(PMString(kErrUnknownType), "TY", 1);
				}
				txtAttrClass->Release();
			}
			else
				command += "0";
			command += "]";
			
		}
		
		else if(attrID == kTextAttrPointSizeBoss && exportTextCmd)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPointSizeBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPointSizeBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			command += "[TC=D";
			if(txtAttrReal != nil)
			{
				command.AppendNumber(txtAttrReal->GetRealNumber(),3,kTrue,kTrue);
				checker->checkFontSize(txtAttrReal->GetRealNumber());
				txtAttrReal->Release();
			}
			else
				command += "12.5";
			command += "]";
		}
		//HF ++ 1.7.8
		//la commande TO n'etait pas exportee avec le reverse
		else if (attrID == kTextAttrOverprintBoss && exportTextCmd)
		{
			ITextAttrBoolean * txtAttrBool = (ITextAttrBoolean *) currentCharStyle->QueryByClassID(kTextAttrOverprintBoss, IID_ITEXTATTRBOOLEAN);
			if(txtAttrBool == nil)
				txtAttrBool = (ITextAttrBoolean *) currentParaStyle->QueryByClassID(kTextAttrOverprintBoss, IID_ITEXTATTRBOOLEAN);

			if(txtAttrBool != nil && styleHasChanged)
			{
				status = kSuccess;
				break;
			}
			
			if(txtAttrBool != nil)
			{
				command += "[TO=true]";
			}
		}
		//HF -- 1.7.8
		else if(attrID == kTextAttrXGlyphScaleBoss && exportTextCmd)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrXGlyphScaleBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrXGlyphScaleBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			command += "[TH=TRUE;";
			if(txtAttrReal != nil)
			{
				command.AppendNumber(txtAttrReal->GetRealNumber() * 100.0,3,kTrue,kTrue);
				checker->checkGlyphScale("TH", txtAttrReal->GetRealNumber() * 100.0);
				txtAttrReal->Release();
			}
			else
				command += "100";
			command += "]";
		}
		else if(attrID == kTextAttrYGlyphScaleBoss && exportTextCmd)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrYGlyphScaleBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrYGlyphScaleBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			command += "[TV=TRUE;";
			if(txtAttrReal != nil)
			{
				command.AppendNumber(txtAttrReal->GetRealNumber() * 100.0,3,kTrue,kTrue);
				checker->checkGlyphScale("TY",txtAttrReal->GetRealNumber() * 100.0);
				txtAttrReal->Release();
			}
			else
				command += "100";
			command += "]";
		}
		else if(attrID == kTextAttrTrackKernBoss && exportTextCmd)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrTrackKernBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrTrackKernBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			command += "[TA=";
			if(txtAttrReal != nil)
			{
				command.AppendNumber(txtAttrReal->GetRealNumber() * 1000.0,3,kTrue,kTrue);
				checker->checkTracking(txtAttrReal->GetRealNumber() * 1000.0);
				txtAttrReal->Release();
			}
			else
				command += "0";
			command += "]";
		}
		else if(attrID == kTextAttrBLShiftBoss && exportTextCmd)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrBLShiftBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrBLShiftBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			command += "[TL=D";
			if(txtAttrReal != nil)
			{
				command.AppendNumber(txtAttrReal->GetRealNumber(),3,kTrue,kTrue);
				checker->checkBaselineOffset(txtAttrReal->GetRealNumber());
				txtAttrReal->Release();
			}
			else
				command += "0";
			command += "]";
		}
		else if(attrID == kTextAttrTintBoss && exportTextCmd)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrTintBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrTintBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			command += "[TT=";
			if(txtAttrReal != nil)
			{
				command.AppendNumber(txtAttrReal->GetRealNumber(),3,kTrue,kTrue);
				checker->checkTextTint(txtAttrReal->GetRealNumber());
				txtAttrReal->Release();
			}
			else
				command += "100";
			command += "]";
		}
		else if(attrID == kTextAttrFontUIDBoss && exportTextCmd)
		{
			ITextAttrUID * txtAttrUID = (ITextAttrUID *) currentCharStyle->QueryByClassID(kTextAttrFontUIDBoss, IID_ITEXTATTRUID);
			if(txtAttrUID == nil)
				txtAttrUID = (ITextAttrUID *) currentParaStyle->QueryByClassID(kTextAttrFontUIDBoss, IID_ITEXTATTRUID);

			if(txtAttrUID != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrUID->Release();
				break;
			}
			command += "[TN=\"";
			if(txtAttrUID != nil)
			{
				InterfacePtr<IFontFamily> font (curDB, txtAttrUID->GetUIDData(), UseDefaultIID());
				txtAttrUID->Release();
				command.Append(font->GetDisplayFamilyName());
			}
			else
				command += "Times New Roman";
			command += "\"]";
		}
		else if(attrID == kTextAttrDropCapCharsBoss)
		{
			ITextAttrInt16 * txtAttrChars = (ITextAttrInt16 *) currentCharStyle->QueryByClassID(kTextAttrDropCapCharsBoss, IID_ITEXTATTRINT16);
			if(txtAttrChars == nil)
				txtAttrChars = (ITextAttrInt16 *) currentParaStyle->QueryByClassID(kTextAttrDropCapCharsBoss, IID_ITEXTATTRINT16);

			if(txtAttrChars != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrChars->Release();
				break;
			}
			if(txtAttrChars != nil)
			{
				nbChar = txtAttrChars->Get();
				txtAttrChars->Release();
			}
			else
				nbChar = 0;
			
			attrPL = kTrue;
		}
		else if(attrID == kTextAttrDropCapLinesBoss)
		{
			ITextAttrInt16 * txtAttrLines = (ITextAttrInt16 *) currentCharStyle->QueryByClassID(kTextAttrDropCapLinesBoss, IID_ITEXTATTRINT16);
			if(txtAttrLines == nil)
				txtAttrLines = (ITextAttrInt16 *) currentParaStyle->QueryByClassID(kTextAttrDropCapLinesBoss, IID_ITEXTATTRINT16);

			if(txtAttrLines != nil && styleHasChanged)
			{
				txtAttrLines->Release();
				status = kSuccess;
				break;
			}
			if(txtAttrLines != nil)
			{
				nbLine = txtAttrLines->Get();
				txtAttrLines->Release();
			}
			else
				nbLine = 0;
			
			attrPL = kTrue;
		}
		else if(attrID == kTextAttrSpaceBeforeBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrSpaceBeforeBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrSpaceBeforeBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			if(txtAttrReal != nil)
			{
				spaceBef = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				spaceBef = 0.0;
			attrPE = kTrue;
		}
		else if(attrID == kTextAttrSpaceAfterBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrSpaceAfterBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrSpaceAfterBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			if(txtAttrReal != nil)
			{
				spaceAft = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				spaceAft = 0.0;
			attrPE = kTrue;
		}
		else if(attrID == kTextAttrGridAlignmentBoss && exportParaCmd)
		{
			ITextAttrGridAlignment * txtGridAlign = (ITextAttrGridAlignment *) currentCharStyle->QueryByClassID(kTextAttrGridAlignmentBoss, IID_ITEXTATTRGRIDALIGNMENT);
			if(txtGridAlign == nil)
				txtGridAlign = (ITextAttrGridAlignment *) currentParaStyle->QueryByClassID(kTextAttrGridAlignmentBoss, IID_ITEXTATTRGRIDALIGNMENT);

			if(txtGridAlign != nil && styleHasChanged)
			{
				status = kSuccess;
				txtGridAlign->Release();
				break;
			}
			command += "[PG=";
			if(txtGridAlign != nil && (txtGridAlign->GetGridAlignment() == Text::kGABaseline))
			{
				command += "TRUE";
				txtGridAlign->Release();
			}
			else
			{
				command += "FALSE";		
				if(txtGridAlign)
				{
					if(txtGridAlign->GetGridAlignment() != Text::kGANone)
						checker->WriteReverseLog(PMString(kErrUnknownType), "PG", 1);
				}
			}

			command += "]";
		}
		else if(attrID == kTextAttrKeepWithNextBoss)
		{
			ITextAttrInt16 * txtAttrKeepWithNext = (ITextAttrInt16 *) currentCharStyle->QueryByClassID(kTextAttrKeepWithNextBoss, IID_ITEXTATTRINT16);
			if(txtAttrKeepWithNext == nil)
				txtAttrKeepWithNext = (ITextAttrInt16 *) currentParaStyle->QueryByClassID(kTextAttrKeepWithNextBoss, IID_ITEXTATTRINT16);

			if(txtAttrKeepWithNext != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrKeepWithNext->Release();
				break;
			}
			command += "[PK=";
			if(txtAttrKeepWithNext != nil && (txtAttrKeepWithNext->Get() != 0))
			{
				command += "TRUE";
				if(txtAttrKeepWithNext->Get() > 1)
					checker->WriteReverseLog(PMString(kErrUnknownType), "PK", 1);

				txtAttrKeepWithNext->Release();
			}
			else 
			{
				command += "FALSE";
			}
			command += "]";
		}
		else if(attrID == kTextAttrLanguageBoss && exportTextCmd)
		{
			ITextAttrUID * txtAttrLanguage = (ITextAttrUID *) currentCharStyle->QueryByClassID(kTextAttrLanguageBoss, IID_ITEXTATTRUID);
			if(txtAttrLanguage == nil)
				txtAttrLanguage = (ITextAttrUID *) currentParaStyle->QueryByClassID(kTextAttrLanguageBoss, IID_ITEXTATTRUID);

			if(txtAttrLanguage != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrLanguage->Release();
				break;
			}
			command += "[CL=";
			if(txtAttrLanguage != nil)
			{
				command.AppendNumber(getLanguageRef(txtAttrLanguage->Get(), "CL", 1));
				txtAttrLanguage->Release();
			}
			else
				command += "0";
			command += "]";
		}
		else if(attrID == kTextAttrKeepLinesBoss)
		{
			ITextAttrBoolean * txtAttrKeepLines = (ITextAttrBoolean *) currentCharStyle->QueryByClassID(kTextAttrKeepLinesBoss, IID_ITEXTATTRBOOLEAN);
			if(txtAttrKeepLines == nil)
				txtAttrKeepLines = (ITextAttrBoolean *) currentParaStyle->QueryByClassID(kTextAttrKeepLinesBoss, IID_ITEXTATTRBOOLEAN);

			if(txtAttrKeepLines != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrKeepLines->Release();
				break;
			}
			if(txtAttrKeepLines != nil)
			{
				keepLines = txtAttrKeepLines->Get();
				txtAttrKeepLines->Release();
			}
			else
				keepLines = kFalse;
			attrPH = kTrue;
		}
		else if(attrID == kTextAttrKeepTogetherBoss)
		{
			ITextAttrBoolean * txtAttrKeepAll = (ITextAttrBoolean *) currentCharStyle->QueryByClassID(kTextAttrKeepTogetherBoss, IID_ITEXTATTRBOOLEAN);
			if(txtAttrKeepAll == nil)
				txtAttrKeepAll = (ITextAttrBoolean *) currentParaStyle->QueryByClassID(kTextAttrKeepTogetherBoss, IID_ITEXTATTRBOOLEAN);

			if(txtAttrKeepAll != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrKeepAll->Release();
				break;
			}
			if(txtAttrKeepAll != nil)
			{
				keepAll = txtAttrKeepAll->Get();
				txtAttrKeepAll->Release();
			}
			else
				keepAll = kFalse;
			attrPH = kTrue;
		}
		else if(attrID == kTextAttrKeepFirstNLinesBoss)
		{
			ITextAttrInt16 * txtAttrLines = (ITextAttrInt16 *) currentCharStyle->QueryByClassID(kTextAttrKeepFirstNLinesBoss, IID_ITEXTATTRINT16);
			if(txtAttrLines == nil)
				txtAttrLines = (ITextAttrInt16 *) currentParaStyle->QueryByClassID(kTextAttrKeepFirstNLinesBoss, IID_ITEXTATTRINT16);

			if(txtAttrLines != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrLines->Release();
				break;
			}
			if(txtAttrLines != nil)
			{
				firstLines = txtAttrLines->Get();
				txtAttrLines->Release();
			}
			else
				firstLines = 2;
			attrPH = kTrue;
		}
		else if(attrID == kTextAttrKeepLastNLinesBoss)
		{
			ITextAttrInt16 * txtAttrLines = (ITextAttrInt16 *) currentCharStyle->QueryByClassID(kTextAttrKeepLastNLinesBoss, IID_ITEXTATTRINT16);
			if(txtAttrLines == nil)
				txtAttrLines = (ITextAttrInt16 *) currentParaStyle->QueryByClassID(kTextAttrKeepLastNLinesBoss, IID_ITEXTATTRINT16);

			if(txtAttrLines != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrLines->Release();
				break;
			}
			if(txtAttrLines != nil)
			{
				lastLines = txtAttrLines->Get();
				txtAttrLines->Release();
			}
			else
				lastLines = 2;
			attrPH = kTrue;
		}
		else if(attrID == kTextAttrAlignmentBoss && exportParaCmd)
		{
			ITextAttrAlign * txtAttrAlign = (ITextAttrAlign *) currentCharStyle->QueryByClassID(kTextAttrAlignmentBoss, IID_ITEXTATTRALIGN);
			if(txtAttrAlign == nil)
				txtAttrAlign = (ITextAttrAlign *) currentParaStyle->QueryByClassID(kTextAttrAlignmentBoss, IID_ITEXTATTRALIGN);

			if(txtAttrAlign != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrAlign->Release();
				break;
			}
			command += "[PJ=";
			if(txtAttrAlign != nil)
			{
				if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignLeft)
					command.AppendNumber(0);
				else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignCenter)
					command.AppendNumber(1);
				else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignRight)
					command.AppendNumber(2);
				else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignJustifyFull)
					command.AppendNumber(3);
				else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignJustifyLeft)
					command.AppendNumber(4);
				else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignJustifyCenter)
					command.AppendNumber(5);
				else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignJustifyRight)
					command.AppendNumber(6);
				else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignToBinding)
					command.AppendNumber(7);
				else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignAwayBinding)
					command.AppendNumber(8);
				else
				{
					command.AppendNumber(0);
					checker->WriteReverseLog(PMString(kErrUnknownType), "PJ", 1);
				}

				txtAttrAlign->Release();
			}
			else
				command.AppendNumber(0);
			command += "]";
		}
		else if(attrID == kTextAttrPRAColorBoss)
		{
			ITextAttrUID * txtAttrUID = (ITextAttrUID *) currentCharStyle->QueryByClassID(kTextAttrPRAColorBoss, IID_ITEXTATTRUID);
			if(txtAttrUID == nil)
				txtAttrUID = (ITextAttrUID *) currentParaStyle->QueryByClassID(kTextAttrPRAColorBoss, IID_ITEXTATTRUID);

			if(txtAttrUID != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrUID->Release();
				break;
			}
			if(txtAttrUID != nil)
			{
				colorPA = getColorRef(txtAttrUID->GetUIDData(), "PA", 7);
				txtAttrUID->Release();
			}
			else
				colorPA = -1;	
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAIndentLBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPRAIndentLBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPRAIndentLBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			if(txtAttrReal != nil)
			{
				lIndentPA = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				lIndentPA = 0.0;
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAIndentRBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPRAIndentRBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPRAIndentRBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			if(txtAttrReal != nil)
			{
				rIndentPA = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				rIndentPA = 0.0;
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAModeBoss)
		{
			ITextAttrParaRuleMode * txtAttrMode = (ITextAttrParaRuleMode *) currentCharStyle->QueryByClassID(kTextAttrPRAModeBoss, IID_ITEXTATTRPARARULEMODE);
			if(txtAttrMode == nil)
				txtAttrMode = (ITextAttrParaRuleMode *) currentParaStyle->QueryByClassID(kTextAttrPRAModeBoss, IID_ITEXTATTRPARARULEMODE);

			if(txtAttrMode != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrMode->Release();
				break;
			}
			if(txtAttrMode != nil && (txtAttrMode->Get() != ICompositionStyle::kRule_ColumnWidth))
			{
				modePA = 1;
				txtAttrMode->Release();
			}
			else
				modePA = 0;
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAOffsetBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPRAOffsetBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPRAOffsetBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			if(txtAttrReal != nil)
			{
				offsetPA = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				offsetPA = 0.0;
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRARuleOnBoss)
		{
			ITextAttrBoolean * txtAttrPRAOn = (ITextAttrBoolean *) currentCharStyle->QueryByClassID(kTextAttrPRARuleOnBoss, IID_ITEXTATTRBOOLEAN);
			if(txtAttrPRAOn == nil)
				txtAttrPRAOn = (ITextAttrBoolean *) currentParaStyle->QueryByClassID(kTextAttrPRARuleOnBoss, IID_ITEXTATTRBOOLEAN);

			if(txtAttrPRAOn != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrPRAOn->Release();
				break;
			}
			if(txtAttrPRAOn != nil)
			{
				PAon = txtAttrPRAOn->Get();
				txtAttrPRAOn->Release();
			}
			else
				PAon = kFalse;
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAStrokeBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPRAStrokeBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPRAStrokeBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			if(txtAttrReal != nil)
			{
				strokePA = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				strokePA = 1.0;
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRATintBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPRATintBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPRATintBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}
			if(txtAttrReal != nil)
			{
				tintPA = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				tintPA = 100.0;
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAStrokeTypeBoss)
		{
			ITextAttrStrokeType * txtAttrType = (ITextAttrStrokeType *) currentCharStyle->QueryByClassID(kTextAttrPRAStrokeTypeBoss, IID_ITEXTATTRSTROKETYPE);
			if(txtAttrType == nil)
				txtAttrType = (ITextAttrStrokeType *) currentParaStyle->QueryByClassID(kTextAttrPRAStrokeTypeBoss, IID_ITEXTATTRSTROKETYPE);

			if(txtAttrType != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrType->Release();
				break;
			}
			if(txtAttrType != nil)
			{
				if(txtAttrType->GetClassIDData() == kSolidPathStrokerBoss)
					typePA = 0;
				else if(txtAttrType->GetClassIDData() == kDashedPathStrokerBoss)
					typePA = 1;
				else if(txtAttrType->GetClassIDData() == kThinThinPathStrokerBoss)
					typePA = 2;
				else if(txtAttrType->GetClassIDData() == kThickThickPathStrokerBoss)
					typePA = 3;
				else if(txtAttrType->GetClassIDData() == kDashedPathStrokerBoss)
					typePA = 4;
				else if(txtAttrType->GetClassIDData() == kCannedDotPathStrokerBoss)
					typePA = 5;
				else if(txtAttrType->GetClassIDData() == kThinThickPathStrokerBoss)
					typePA = 6;
				else if(txtAttrType->GetClassIDData() == kThickThinPathStrokerBoss)
					typePA = 7;
				else if(txtAttrType->GetClassIDData() == kThinThickThinPathStrokerBoss)
					typePA = 8;
				else if(txtAttrType->GetClassIDData() == kThickThinThickPathStrokerBoss)
					typePA = 9;
				else if(txtAttrType->GetClassIDData() == kTriplePathStrokerBoss)
					typePA = 10;
				else if(txtAttrType->GetClassIDData() == kCannedDash3x2PathStrokerBoss)
					typePA = 11;
				else if(txtAttrType->GetClassIDData() == kCannedDash4x4PathStrokerBoss)
					typePA = 12;
				else if(txtAttrType->GetClassIDData() == kLeftSlantHashPathStrokerBoss)
					typePA = 13;
				else if(txtAttrType->GetClassIDData() == kRightSlantHashPathStrokerBoss)
					typePA = 14;
				else if(txtAttrType->GetClassIDData() == kStraightHashPathStrokerBoss)
					typePA = 15;
				else if(txtAttrType->GetClassIDData() == kSingleWavyPathStrokerBoss)
					typePA = 16;
				else if(txtAttrType->GetClassIDData() == kWhiteDiamondPathStrokerBoss)
					typePA = 17;
				else if(txtAttrType->GetClassIDData() == kJapaneseDotsPathStrokerBoss)
					typePA = 18;
				else
					checker->WriteReverseLog(PMString(kErrUnknownType), "PA", 6);

				txtAttrType->Release();
			}
			else 
				typePA = 0;
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRBColorBoss)
		{
			ITextAttrUID * txtAttrUID = (ITextAttrUID *) currentCharStyle->QueryByClassID(kTextAttrPRBColorBoss, IID_ITEXTATTRUID);
			if(txtAttrUID == nil)
				txtAttrUID = (ITextAttrUID *) currentParaStyle->QueryByClassID(kTextAttrPRBColorBoss, IID_ITEXTATTRUID);

			if(txtAttrUID != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrUID->Release();
				break;
			}
			if(txtAttrUID != nil)
			{
				colorPP = getColorRef(txtAttrUID->GetUIDData(), "PP", 7); 
				txtAttrUID->Release();
			}
			else
				colorPP = -1;
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBIndentLBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPRBIndentLBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPRBIndentLBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				lIndentPP = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				lIndentPP = 0.0;
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBIndentRBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPRBIndentRBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPRBIndentRBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				rIndentPP = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				rIndentPP = 0.0;
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBModeBoss)
		{
			ITextAttrParaRuleMode * txtAttrMode = (ITextAttrParaRuleMode *) currentCharStyle->QueryByClassID(kTextAttrPRBModeBoss, IID_ITEXTATTRPARARULEMODE);
			if(txtAttrMode == nil)
				txtAttrMode = (ITextAttrParaRuleMode *) currentParaStyle->QueryByClassID(kTextAttrPRBModeBoss, IID_ITEXTATTRPARARULEMODE);

			if(txtAttrMode != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrMode->Release();
				break;
			}

			if(txtAttrMode != nil && (txtAttrMode->Get() != ICompositionStyle::kRule_ColumnWidth))
			{
				modePP = kTrue;
				txtAttrMode->Release();
			}
			else
				modePP = kFalse;	
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBOffsetBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPRBOffsetBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPRBOffsetBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				offsetPP = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				offsetPP = 0.0;
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBRuleOnBoss)
		{
			ITextAttrBoolean * txtAttrPRAOn = (ITextAttrBoolean *) currentCharStyle->QueryByClassID(kTextAttrPRBRuleOnBoss, IID_ITEXTATTRBOOLEAN);
			if(txtAttrPRAOn == nil)
				txtAttrPRAOn = (ITextAttrBoolean *) currentParaStyle->QueryByClassID(kTextAttrPRBRuleOnBoss, IID_ITEXTATTRBOOLEAN);

			if(txtAttrPRAOn != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrPRAOn->Release();
				break;
			}

			if(txtAttrPRAOn != nil)
			{
				PPon = txtAttrPRAOn->Get();
				txtAttrPRAOn->Release();
			}
			else
				PPon = kFalse;
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBStrokeBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPRBStrokeBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPRBStrokeBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				strokePP = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				strokePP = 1.0;
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBTintBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrPRBTintBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrPRBTintBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				tintPP = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				tintPP = 100.0;
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBStrokeTypeBoss)
		{
			ITextAttrStrokeType * txtAttrType = (ITextAttrStrokeType *) currentCharStyle->QueryByClassID(kTextAttrPRBStrokeTypeBoss, IID_ITEXTATTRSTROKETYPE);
			if(txtAttrType == nil)
				txtAttrType = (ITextAttrStrokeType *) currentParaStyle->QueryByClassID(kTextAttrPRBStrokeTypeBoss, IID_ITEXTATTRSTROKETYPE);

			if(txtAttrType != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrType->Release();
				break;
			}

			if(txtAttrType != nil)
			{
				if(txtAttrType->GetClassIDData() == kSolidPathStrokerBoss)
					typePP = 0;
				else if(txtAttrType->GetClassIDData() == kDashedPathStrokerBoss)
					typePP = 1;
				else if(txtAttrType->GetClassIDData() == kThinThinPathStrokerBoss)
					typePP = 2;
				else if(txtAttrType->GetClassIDData() == kThickThickPathStrokerBoss)
					typePP = 3;
				else if(txtAttrType->GetClassIDData() == kDashedPathStrokerBoss)
					typePP = 4;
				else if(txtAttrType->GetClassIDData() == kCannedDotPathStrokerBoss)
					typePP = 5;
				else if(txtAttrType->GetClassIDData() == kThinThickPathStrokerBoss)
					typePP = 6;
				else if(txtAttrType->GetClassIDData() == kThickThinPathStrokerBoss)
					typePP = 7;
				else if(txtAttrType->GetClassIDData() == kThinThickThinPathStrokerBoss)
					typePP = 8;
				else if(txtAttrType->GetClassIDData() == kThickThinThickPathStrokerBoss)
					typePP = 9;
				else if(txtAttrType->GetClassIDData() == kTriplePathStrokerBoss)
					typePP = 10;	
				else if(txtAttrType->GetClassIDData() == kCannedDash3x2PathStrokerBoss)
					typePP = 11;
				else if(txtAttrType->GetClassIDData() == kCannedDash4x4PathStrokerBoss)
					typePP = 12;
				else if(txtAttrType->GetClassIDData() == kLeftSlantHashPathStrokerBoss)
					typePP = 13;
				else if(txtAttrType->GetClassIDData() == kRightSlantHashPathStrokerBoss)
					typePP = 14;
				else if(txtAttrType->GetClassIDData() == kStraightHashPathStrokerBoss)
					typePP = 15;
				else if(txtAttrType->GetClassIDData() == kSingleWavyPathStrokerBoss)
					typePP = 16;
				else if(txtAttrType->GetClassIDData() == kWhiteDiamondPathStrokerBoss)
					typePP = 17;
				else if(txtAttrType->GetClassIDData() == kJapaneseDotsPathStrokerBoss)
					typePP = 18;
				else
					checker->WriteReverseLog(PMString(kErrUnknownType), "PP", 6);

				txtAttrType->Release();
			}
			else 
				typePP = 0;
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrIndent1LBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrIndent1LBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrIndent1LBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				indentFirst = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				indentFirst = 0.0;
			attrPR = kTrue;
		}
		else if(attrID == kTextAttrIndentBLBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrIndentBLBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrIndentBLBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				indentLeft = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				indentLeft = 0.0;
			attrPR = kTrue;
		}
		else if(attrID == kTextAttrIndentBRBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrIndentBRBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrIndentBRBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				indentRight = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				indentRight = 0.0;
			attrPR = kTrue;
		}
		else if(attrID == kTextAttrTabsBoss && exportParaCmd)
		{
			ITextAttrTabSettings * txtAttrTabs = (ITextAttrTabSettings *) currentCharStyle->QueryByClassID(kTextAttrTabsBoss, IID_ITEXTATTRTABS);
			if(txtAttrTabs == nil)
				txtAttrTabs = (ITextAttrTabSettings *) currentParaStyle->QueryByClassID(kTextAttrTabsBoss, IID_ITEXTATTRTABS);

			if(txtAttrTabs != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrTabs->Release();
				break;
			}
			command += "[PT=";
			if(txtAttrTabs != nil)
			{
				TabStopTable tabs = txtAttrTabs->GetTabStopTable();
				
				for(int i = 0 ; i < tabs.size() ; ++i)
				{
					const TabStop tab = tabs[i];
					command += "D";
					command.AppendNumber(tab.GetPosition(),3,kTrue,kTrue);
					checker->checkTabPosition(tab.GetPosition(), 1 + 3*i);

					command += ";";
					if(tab.GetAlignment() == TabStop::kTabAlignLeft)
					{
						command.AppendNumber(0);
						command += ";' ';";
					}
					else if(tab.GetAlignment() == TabStop::kTabAlignCenter)
					{
						command.AppendNumber(1);
						command += ";' ';";
					}
					else if(tab.GetAlignment() == TabStop::kTabAlignRight)
					{
						command.AppendNumber(2);
						command += ";' ';";
					}
					else if(tab.GetAlignment() == TabStop::kTabAlignChar)
					{
						command.AppendNumber(3);
						command += ";'";
						command.AppendW(tab.GetAlignToChar());
						command += "';";
					}
				}
				command.Remove(command.NumUTF16TextChars() - 1,1);
				command += "]";
				txtAttrTabs->Release();
			}
			else
			{
				command += "D-1;0;' ']";
				checker->WriteReverseLog(PMString(kErrUnknownType), "PT", 1);
			}
		}
		else if(attrID == kTextAttrHyphenCapBoss)
		{
			ITextAttrBoolean * txtAttrHyphenCap = (ITextAttrBoolean *) currentCharStyle->QueryByClassID(kTextAttrHyphenCapBoss, IID_ITEXTATTRBOOLEAN);
			if(txtAttrHyphenCap == nil)
				txtAttrHyphenCap = (ITextAttrBoolean *) currentParaStyle->QueryByClassID(kTextAttrHyphenCapBoss, IID_ITEXTATTRBOOLEAN);

			if(txtAttrHyphenCap != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrHyphenCap->Release();
				break;
			}

			if(txtAttrHyphenCap != nil)
			{
				capitalized = txtAttrHyphenCap->Get();
				txtAttrHyphenCap->Release();
			}
			else
				capitalized = kTrue;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrHyphenZoneBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrHyphenZoneBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrHyphenZoneBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				hyphenZone = txtAttrReal->GetRealNumber();
				txtAttrReal->Release();
			}
			else
				hyphenZone = 36.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrShortestWordBoss)
		{
			ITextAttrInt16 * txtAttrShortest  = (ITextAttrInt16 *) currentCharStyle->QueryByClassID(kTextAttrShortestWordBoss, IID_ITEXTATTRINT16);
			if(txtAttrShortest  == nil)
				txtAttrShortest  = (ITextAttrInt16 *) currentParaStyle->QueryByClassID(kTextAttrShortestWordBoss, IID_ITEXTATTRINT16);

			if(txtAttrShortest != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrShortest->Release();
				break;
			}

			if(txtAttrShortest  != nil)
			{
				minWord = txtAttrShortest->Get();
				txtAttrShortest->Release();
			}
			else
				minWord = 5;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrMinAfterBoss)
		{
			ITextAttrInt16 * txtAttrMinAfter = (ITextAttrInt16 *) currentCharStyle->QueryByClassID(kTextAttrMinAfterBoss, IID_ITEXTATTRINT16);
			if(txtAttrMinAfter == nil)
				txtAttrMinAfter = (ITextAttrInt16 *) currentParaStyle->QueryByClassID(kTextAttrMinAfterBoss, IID_ITEXTATTRINT16);

			if(txtAttrMinAfter != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrMinAfter->Release();
				break;
			}

			if(txtAttrMinAfter != nil)
			{
				afterFirst = txtAttrMinAfter->Get();
				txtAttrMinAfter->Release();
			}
			else
				afterFirst = 2;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrMinBeforeBoss)
		{
			ITextAttrInt16 * txtAttrMinBefore = (ITextAttrInt16 *) currentCharStyle->QueryByClassID(kTextAttrMinBeforeBoss, IID_ITEXTATTRINT16);
			if(txtAttrMinBefore == nil)
				txtAttrMinBefore  = (ITextAttrInt16 *) currentParaStyle->QueryByClassID(kTextAttrMinBeforeBoss, IID_ITEXTATTRINT16);

			if(txtAttrMinBefore != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrMinBefore->Release();
				break;
			}

			if(txtAttrMinBefore != nil)
			{
				beforeLast = txtAttrMinBefore->Get();
				txtAttrMinBefore->Release();
			}
			else
				beforeLast = 2;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrHyphenModeBoss)
		{
			ITextAttrBoolean * txtAttrHyphenMode = (ITextAttrBoolean *) currentCharStyle->QueryByClassID(kTextAttrHyphenModeBoss, IID_ITEXTATTRBOOLEAN);
			if(txtAttrHyphenMode == nil)
				txtAttrHyphenMode = (ITextAttrBoolean *) currentParaStyle->QueryByClassID(kTextAttrHyphenModeBoss, IID_ITEXTATTRBOOLEAN);

			if(txtAttrHyphenMode != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrHyphenMode->Release();
				break;
			}

			if(txtAttrHyphenMode != nil)
			{
				hyphenMode = txtAttrHyphenMode->Get();
				txtAttrHyphenMode->Release();
			}
			else
				hyphenMode = kTrue;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrHyphenLastBoss)
		{
			ITextAttrBoolean * txtAttrHyphenLast = (ITextAttrBoolean *) currentCharStyle->QueryByClassID(kTextAttrHyphenLastBoss, IID_ITEXTATTRBOOLEAN);
			if(txtAttrHyphenLast == nil)
				txtAttrHyphenLast = (ITextAttrBoolean *) currentParaStyle->QueryByClassID(kTextAttrHyphenLastBoss, IID_ITEXTATTRBOOLEAN);

			if(txtAttrHyphenLast != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrHyphenLast->Release();
				break;
			}

			if(txtAttrHyphenLast != nil)
			{
				lastWord = txtAttrHyphenLast->Get();
				txtAttrHyphenLast->Release();
			}
			else
				lastWord = kTrue;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrLetterspaceDesBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrLetterspaceDesBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrLetterspaceDesBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				letterDes = txtAttrReal->GetRealNumber() * 100.0;
				txtAttrReal->Release();
			}
			else
				letterDes = 0.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrLetterspaceMaxBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrLetterspaceMaxBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrLetterspaceMaxBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				letterMax = txtAttrReal->GetRealNumber() * 100.0;
				txtAttrReal->Release();
			}
			else
				letterMax = 0.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrLetterspaceMinBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrLetterspaceMinBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrLetterspaceMinBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				letterMin = txtAttrReal->GetRealNumber() * 100.0;
				txtAttrReal->Release();
			}
			else
				letterMin = 0.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrWordspaceDesBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrWordspaceDesBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrWordspaceDesBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				wordDes = txtAttrReal->GetRealNumber() * 100.0;
				txtAttrReal->Release();
			}
			else
				wordDes = 100.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrWordspaceMaxBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrWordspaceMaxBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrWordspaceMaxBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				wordMax = txtAttrReal->GetRealNumber() * 100.0;
				txtAttrReal->Release();
			}
			else
				wordMax = 133.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrWordspaceMinBoss)
		{
			ITextAttrRealNumber * txtAttrReal = (ITextAttrRealNumber *) currentCharStyle->QueryByClassID(kTextAttrWordspaceMinBoss, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				txtAttrReal = (ITextAttrRealNumber *) currentParaStyle->QueryByClassID(kTextAttrWordspaceMinBoss, IID_ITEXTATTRREALNUMBER);

			if(txtAttrReal != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrReal->Release();
				break;
			}

			if(txtAttrReal != nil)
			{
				wordMin = txtAttrReal->GetRealNumber() * 100.0;
				txtAttrReal->Release();
			}
			else
				wordMin = 80.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrHyphenLadderBoss)
		{
			ITextAttrInt16 * txtAttrLimit = (ITextAttrInt16 *) currentCharStyle->QueryByClassID(kTextAttrHyphenLadderBoss, IID_ITEXTATTRINT16);
			if(txtAttrLimit == nil)
				txtAttrLimit  = (ITextAttrInt16 *) currentParaStyle->QueryByClassID(kTextAttrHyphenLadderBoss, IID_ITEXTATTRINT16);

			if(txtAttrLimit != nil && styleHasChanged)
			{
				status = kSuccess;
				txtAttrLimit->Release();
				break;
			}

			if(txtAttrLimit != nil)
			{
				limit = txtAttrLimit->Get();
				txtAttrLimit->Release();
			}
			else
				limit = 3;
			attrCJ = kTrue;
		}
		else if(attrID == kPermRefsAttrBoss && exportTextCmd)
		{
			if(permrefFound)
				command += "[HF]";

			permrefFound = kFalse;
		}
		status = kSuccess;
	
	} while(kFalse);
	
	if(status != kSuccess)
		throw TCLError(error);
	
	return command;
}

void PrsTCLWriter::ResetValues()
{
	ErrorCode status = kFailure;
	do 
	{
		// PL attributes
		nbChar = nbLine = 0;
		InterfacePtr<ITextAttrInt16> txtAttrChars ((ITextAttrInt16 *)currentParaStyle->QueryByClassID(kTextAttrDropCapCharsBoss, IID_ITEXTATTRINT16));
		InterfacePtr<ITextAttrInt16> txtAttrLines ((ITextAttrInt16 *)currentParaStyle->QueryByClassID(kTextAttrDropCapLinesBoss, IID_ITEXTATTRINT16));
		if(txtAttrChars != nil) 
			nbChar = txtAttrChars->Get();
		if(txtAttrLines != nil) 
			nbLine = txtAttrLines->Get();
		
		// PH attributes
		firstLines = lastLines = 2;
		keepAll = keepLines = kFalse;
		InterfacePtr<ITextAttrBoolean> txtAttrKeepLines ((ITextAttrBoolean *)currentParaStyle->QueryByClassID(kTextAttrKeepLinesBoss, IID_ITEXTATTRBOOLEAN));
		InterfacePtr<ITextAttrBoolean> txtAttrKeepTogether ((ITextAttrBoolean *)currentParaStyle->QueryByClassID(kTextAttrKeepTogetherBoss, IID_ITEXTATTRBOOLEAN));
		InterfacePtr<ITextAttrInt16> txtAttrFirstLines ((ITextAttrInt16 *)currentParaStyle->QueryByClassID(kTextAttrKeepFirstNLinesBoss, IID_ITEXTATTRINT16));
		InterfacePtr<ITextAttrInt16> txtAttrLastLines ((ITextAttrInt16 *)currentParaStyle->QueryByClassID(kTextAttrKeepLastNLinesBoss, IID_ITEXTATTRINT16));

		if(txtAttrKeepLines != nil)
			keepLines = txtAttrKeepLines->Get();
		if(txtAttrKeepTogether != nil)
			keepAll = txtAttrKeepTogether->Get(); 
		if(txtAttrFirstLines != nil)
			firstLines = txtAttrFirstLines->Get();
		if(txtAttrLastLines != nil)
			lastLines = txtAttrLastLines->Get(); 
	
		// PE attributes
		spaceBef = spaceAft = 0.0;
		InterfacePtr<ITextAttrRealNumber> txtAttrSpaceBef ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrSpaceBeforeBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrSpaceAfter ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrSpaceAfterBoss, IID_ITEXTATTRREALNUMBER));

		if(txtAttrSpaceBef != nil)
			spaceBef = txtAttrSpaceBef->GetRealNumber();
		if(txtAttrSpaceAfter != nil)
			spaceAft = txtAttrSpaceAfter->GetRealNumber(); 
		
		// PR attributes
		indentFirst = indentLeft = indentRight = 0.0;
		InterfacePtr<ITextAttrRealNumber> txtAttrIndentFirst ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrIndent1LBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrIndentLeft((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrIndentBLBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrIndentRight ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrIndentBRBoss, IID_ITEXTATTRREALNUMBER));

		if(txtAttrIndentFirst != nil)
			indentFirst = txtAttrIndentFirst->GetRealNumber();
		if(txtAttrIndentLeft != nil)
			indentLeft = txtAttrIndentLeft->GetRealNumber(); 
		if(txtAttrIndentRight != nil)
			indentRight = txtAttrIndentRight->GetRealNumber();

		// PA attributes
		strokePA = 1.0;
		tintPA = 100.0;
		lIndentPA = rIndentPA = offsetPA = 0.0;
		typePA = 0;
		colorPA = -1;
		modePA = kFalse;
		PAon = kFalse;

		InterfacePtr<ITextAttrUID> txtAttrPAColorUID ((ITextAttrUID *)currentParaStyle->QueryByClassID(kTextAttrPRAColorBoss, IID_ITEXTATTRUID));
		InterfacePtr<ITextAttrRealNumber> txtAttrLIndentPA ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrPRAIndentLBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrRIndentPA ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrPRAIndentRBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrParaRuleMode> txtAttrModePA ((ITextAttrParaRuleMode *)currentParaStyle->QueryByClassID(kTextAttrPRAModeBoss, IID_ITEXTATTRPARARULEMODE));
		InterfacePtr<ITextAttrRealNumber> txtAttrOffsetPA ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrPRAOffsetBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrBoolean> txtAttrRuleOnPA ((ITextAttrBoolean *)currentParaStyle->QueryByClassID(kTextAttrPRARuleOnBoss, IID_ITEXTATTRBOOLEAN));
		InterfacePtr<ITextAttrRealNumber> txtAttrStrokePA ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrPRAStrokeBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrTintPA ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrPRATintBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrStrokeType> txtAttrTypePA ((ITextAttrStrokeType *)currentParaStyle->QueryByClassID(kTextAttrPRAStrokeTypeBoss, IID_ITEXTATTRSTROKETYPE));

		if(txtAttrPAColorUID != nil)
			colorPA = getColorRef(txtAttrPAColorUID->GetUIDData(), kNullString, 0); 
		if(txtAttrLIndentPA != nil)
			lIndentPA = txtAttrLIndentPA->GetRealNumber();
		if(txtAttrRIndentPA != nil)
			rIndentPA = txtAttrRIndentPA->GetRealNumber(); 
		if(txtAttrModePA != nil)
		{
			if(txtAttrModePA->Get() != ICompositionStyle::kRule_ColumnWidth)
				modePA = kTrue;
		}
		if(txtAttrOffsetPA != nil)
			offsetPA = txtAttrOffsetPA->GetRealNumber(); 
		if(txtAttrRuleOnPA != nil)
			PAon = txtAttrRuleOnPA->Get(); 
		if(txtAttrStrokePA != nil)
			strokePA = txtAttrStrokePA->GetRealNumber(); 
		if(txtAttrTintPA != nil)
			tintPA = txtAttrTintPA->GetRealNumber(); 
		if(txtAttrTypePA != nil)
		{
			if(txtAttrTypePA->GetClassIDData() == kSolidPathStrokerBoss)
				typePA = 0;
			else if(txtAttrTypePA->GetClassIDData() == kDashedPathStrokerBoss)
				typePA = 1;
			else if(txtAttrTypePA->GetClassIDData() == kThinThinPathStrokerBoss)
				typePA = 2;
			else if(txtAttrTypePA->GetClassIDData() == kThickThickPathStrokerBoss)
				typePA = 3;
			else if(txtAttrTypePA->GetClassIDData() == kDashedPathStrokerBoss)
				typePA = 4;
			else if(txtAttrTypePA->GetClassIDData() == kCannedDotPathStrokerBoss)
				typePA = 5;
			else if(txtAttrTypePA->GetClassIDData() == kThinThickPathStrokerBoss)
				typePA = 6;
			else if(txtAttrTypePA->GetClassIDData() == kThickThinPathStrokerBoss)
				typePA = 7;
			else if(txtAttrTypePA->GetClassIDData() == kThinThickThinPathStrokerBoss)
				typePA = 8;
			else if(txtAttrTypePA->GetClassIDData() == kThickThinThickPathStrokerBoss)
				typePA = 9;
			else if(txtAttrTypePA->GetClassIDData() == kTriplePathStrokerBoss)
				typePA = 10;
			else if(txtAttrTypePA->GetClassIDData() == kCannedDash3x2PathStrokerBoss)
				typePA = 11;
			else if(txtAttrTypePA->GetClassIDData() == kCannedDash4x4PathStrokerBoss)
				typePA = 12;
			else if(txtAttrTypePA->GetClassIDData() == kLeftSlantHashPathStrokerBoss)
				typePA = 13;
			else if(txtAttrTypePA->GetClassIDData() == kRightSlantHashPathStrokerBoss)
				typePA = 14;
			else if(txtAttrTypePA->GetClassIDData() == kStraightHashPathStrokerBoss)
				typePA = 15;
			else if(txtAttrTypePA->GetClassIDData() == kSingleWavyPathStrokerBoss)
				typePA = 16;
			else if(txtAttrTypePA->GetClassIDData() == kWhiteDiamondPathStrokerBoss)
				typePA = 17;
			else if(txtAttrTypePA->GetClassIDData() == kJapaneseDotsPathStrokerBoss)
				typePA = 18;
		}
					
		// PR attributes
		strokePP = 1.0;
		tintPP = 100.0;
		lIndentPP = rIndentPP = offsetPP = 0.0;
		typePP = 0;
		colorPP = -1;
		modePP = kFalse;
		PPon = kFalse;

		InterfacePtr<ITextAttrUID> txtAttrPPColorUID ((ITextAttrUID *)currentParaStyle->QueryByClassID(kTextAttrPRBColorBoss, IID_ITEXTATTRUID));
		InterfacePtr<ITextAttrRealNumber> txtAttrLIndentPP ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrPRBIndentLBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrRIndentPP ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrPRBIndentRBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrParaRuleMode> txtAttrModePP ((ITextAttrParaRuleMode *)currentParaStyle->QueryByClassID(kTextAttrPRBModeBoss, IID_ITEXTATTRPARARULEMODE));
		InterfacePtr<ITextAttrRealNumber> txtAttrOffsetPP ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrPRBOffsetBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrBoolean> txtAttrRuleOnPP ((ITextAttrBoolean *)currentParaStyle->QueryByClassID(kTextAttrPRBRuleOnBoss, IID_ITEXTATTRBOOLEAN));
		InterfacePtr<ITextAttrRealNumber> txtAttrStrokePP ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrPRBStrokeBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrTintPP ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrPRBTintBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrStrokeType> txtAttrTypePP ((ITextAttrStrokeType *)currentParaStyle->QueryByClassID(kTextAttrPRBStrokeTypeBoss, IID_ITEXTATTRSTROKETYPE));

		if(txtAttrPPColorUID != nil)
			colorPP = getColorRef(txtAttrPPColorUID->GetUIDData(), kNullString, 0); 
		if(txtAttrLIndentPP != nil)
			lIndentPP = txtAttrLIndentPP->GetRealNumber();
		if(txtAttrRIndentPP != nil)
			rIndentPP = txtAttrRIndentPP->GetRealNumber(); 
		if(txtAttrModePP != nil)
		{
			if(txtAttrModePP->Get() != ICompositionStyle::kRule_ColumnWidth)
				modePP = kTrue;
		}
		if(txtAttrOffsetPP != nil)
			offsetPP = txtAttrOffsetPP->GetRealNumber(); 
		if(txtAttrRuleOnPP != nil)
			PPon = txtAttrRuleOnPP->Get(); 
		if(txtAttrStrokePP != nil)
			strokePP = txtAttrStrokePP->GetRealNumber(); 
		if(txtAttrTintPP != nil)
			tintPP = txtAttrTintPP->GetRealNumber(); 
		if(txtAttrTypePP != nil)
		{
			if(txtAttrTypePP->GetClassIDData() == kSolidPathStrokerBoss)
				typePP = 0;
			else if(txtAttrTypePP->GetClassIDData() == kDashedPathStrokerBoss)
				typePP = 1;
			else if(txtAttrTypePP->GetClassIDData() == kThinThinPathStrokerBoss)
				typePP = 2;
			else if(txtAttrTypePP->GetClassIDData() == kThickThickPathStrokerBoss)
				typePP = 3;
			else if(txtAttrTypePP->GetClassIDData() == kDashedPathStrokerBoss)
				typePP = 4;
			else if(txtAttrTypePP->GetClassIDData() == kCannedDotPathStrokerBoss)
				typePP = 5;
			else if(txtAttrTypePP->GetClassIDData() == kThinThickPathStrokerBoss)
				typePP = 6;
			else if(txtAttrTypePP->GetClassIDData() == kThickThinPathStrokerBoss)
				typePP = 7;
			else if(txtAttrTypePP->GetClassIDData() == kThinThickThinPathStrokerBoss)
				typePP = 8;
			else if(txtAttrTypePP->GetClassIDData() == kThickThinThickPathStrokerBoss)
				typePP = 9;
			else if(txtAttrTypePP->GetClassIDData() == kTriplePathStrokerBoss)
				typePP = 10;
			else if(txtAttrTypePP->GetClassIDData() == kCannedDash3x2PathStrokerBoss)
				typePP = 11;
			else if(txtAttrTypePP->GetClassIDData() == kCannedDash4x4PathStrokerBoss)
				typePP = 12;
			else if(txtAttrTypePP->GetClassIDData() == kLeftSlantHashPathStrokerBoss)
				typePP = 13;
			else if(txtAttrTypePP->GetClassIDData() == kRightSlantHashPathStrokerBoss)
				typePP = 14;
			else if(txtAttrTypePP->GetClassIDData() == kStraightHashPathStrokerBoss)
				typePP = 15;
			else if(txtAttrTypePP->GetClassIDData() == kSingleWavyPathStrokerBoss)
				typePP = 16;
			else if(txtAttrTypePP->GetClassIDData() == kWhiteDiamondPathStrokerBoss)
				typePP = 17;
			else if(txtAttrTypePP->GetClassIDData() == kJapaneseDotsPathStrokerBoss)
				typePP = 18;
		}

		// CS attributes
		hyphenMode = capitalized = lastWord = kTrue;
		minWord = 5;
		afterFirst = beforeLast = 2;
		limit = 3;
		hyphenZone = 36.0; 
		wordMin = 80;
		wordMax = 133;
		wordDes = 100; 
		letterMin = letterMax = letterDes = 0.0;

		InterfacePtr<ITextAttrBoolean> txtAttrHyphenCap ((ITextAttrBoolean *)currentParaStyle->QueryByClassID(kTextAttrHyphenCapBoss, IID_ITEXTATTRBOOLEAN));
		InterfacePtr<ITextAttrRealNumber> txtAttrHyphenZone ((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrHyphenZoneBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrInt16> txtAttrShortest ((ITextAttrInt16 *)currentParaStyle->QueryByClassID(kTextAttrShortestWordBoss, IID_ITEXTATTRINT16));
		InterfacePtr<ITextAttrInt16> txtAttrMinAfter ((ITextAttrInt16 *)currentParaStyle->QueryByClassID(kTextAttrMinAfterBoss, IID_ITEXTATTRINT16));
		InterfacePtr<ITextAttrInt16> txtAttrMinBefore ((ITextAttrInt16 *)currentParaStyle->QueryByClassID(kTextAttrMinBeforeBoss, IID_ITEXTATTRINT16));
		InterfacePtr<ITextAttrHyphenMode> txtAttrHyphenMode ((ITextAttrHyphenMode *)currentParaStyle->QueryByClassID(kTextAttrHyphenModeBoss, IID_ITEXTATTRHYPHENMODE));
		InterfacePtr<ITextAttrBoolean> txtAttrHyphenLast ((ITextAttrBoolean *)currentParaStyle->QueryByClassID(kTextAttrHyphenLastBoss, IID_ITEXTATTRBOOLEAN));
		InterfacePtr<ITextAttrRealNumber> txtAttrLetterDes((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrLetterspaceDesBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrLetterMax((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrLetterspaceMaxBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrLetterMin((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrLetterspaceMinBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrWSDes((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrWordspaceDesBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrWSMax((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrWordspaceMaxBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrRealNumber> txtAttrWSMin((ITextAttrRealNumber *)currentParaStyle->QueryByClassID(kTextAttrWordspaceMinBoss, IID_ITEXTATTRREALNUMBER));
		InterfacePtr<ITextAttrInt16> txtAttrLimit((ITextAttrInt16 *)currentParaStyle->QueryByClassID(kTextAttrHyphenLadderBoss, IID_ITEXTATTRINT16));

		if(txtAttrHyphenCap != nil)
			capitalized = txtAttrHyphenCap->Get();
		if(txtAttrHyphenZone != nil)
			hyphenZone = txtAttrHyphenZone->GetRealNumber();
		if(txtAttrShortest != nil)
			minWord = txtAttrShortest->Get(); 
		if(txtAttrMinAfter != nil)
			afterFirst = txtAttrMinAfter->Get();
		if(txtAttrMinBefore != nil)
			beforeLast = txtAttrMinBefore->Get();
		if(txtAttrHyphenMode != nil)
		{
			if(txtAttrHyphenMode->GetMethod() == IHyphenationStyle::kHyphensOff)
				hyphenMode = kFalse;
		}
		if(txtAttrHyphenLast != nil)
			lastWord = txtAttrHyphenLast->Get();
		if(txtAttrLetterDes != nil)
			letterDes = txtAttrLetterDes->GetRealNumber() * 100.0;
		if(txtAttrLetterMax != nil)
			letterMax = txtAttrLetterMax->GetRealNumber() * 100.0;
		if(txtAttrLetterMin != nil)
			letterMin = txtAttrLetterMin->GetRealNumber() * 100.0;
		if(txtAttrWSDes != nil)
			wordDes = txtAttrWSDes->GetRealNumber() * 100.0;
		if(txtAttrWSMax != nil)
			wordMax = txtAttrWSMax->GetRealNumber() * 100.0;
		if(txtAttrWSMin != nil)
			wordMin = txtAttrWSMin->GetRealNumber() * 100.0;
		if(txtAttrLimit != nil)
			limit = txtAttrLimit->Get();
		
		status = kSuccess;

	} while(kFalse);

	
}

void PrsTCLWriter::writeStyleAttributes(const AttributeBossList * list)
{
	int32 nbBosses = list->CountBosses();
	PMString commands;

	// Attributes are set to default values
	valTG = valTJ = nbChar = nbLine = 0;
	firstLines = lastLines = 2;
	keepAll = keepLines = kFalse;
	
	spaceBef = spaceAft = indentFirst = indentLeft = indentRight = 0.0;
	
	strokePA = 1.0;
	tintPA = 100.0;
	lIndentPA = rIndentPA = offsetPA = 0.0;
	typePA = 0;
	colorPA = -1;
	modePA = kFalse;
	PAon = kFalse;
	
	strokePP = 1.0;
	tintPP = 100.0;
	lIndentPP = rIndentPP = offsetPP = 0.0;
	typePP = 0;
	colorPP = -1;
	modePP = kFalse;
	PPon = kFalse;
	
	hyphenMode = capitalized = lastWord = kTrue;
	minWord = 5;
	afterFirst = beforeLast = 2;
	limit = 3;
	hyphenZone = 36.0; 
	wordMin = 80;
	wordMax = 133;
	wordDes = 100; 
	letterMin = letterMax = letterDes = 0.0;
	
	attrPL = attrTG = attrTJ = attrPE = attrPH = attrPA = attrPP = attrPR = attrCJ = kFalse;

	for(int32 i = 0 ; i < nbBosses ; ++i)
	{
		const IPMUnknown * attr = list->QueryBossN(i, IID_IUNKNOWN);
		commands += writeAttribute(list->GetClassN(i), attr);
		attr->Release();
	}

	if(!exportCJ)
		attrCJ = kFalse;

	if(!exportParaCmd)
	{
		attrPL = attrPE = attrPH = attrPA = attrPP = attrPR = kFalse;
	}

	writeTG_TJCommands();
	writeMultiAttributesCommands();
	
	TCLStream->XferByte((uchar *) commands.GetPlatformString().c_str(), commands.GetPlatformString().size()); 
}

PMString PrsTCLWriter::writeAttribute(ClassID attrID, const IPMUnknown * inter)
{
	PMString command;
	PMString error(kErrNilInterface);
	ErrorCode status = kFailure;
	do
	{
		if(attrID == kTextAttrLeadBoss && exportParaCmd)
		{
			command += "[PI=D";
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			command.AppendNumber(txtAttrReal->GetRealNumber(),3,kTrue,kTrue);
			command += "]";
			checker->checkLeading(txtAttrReal->GetRealNumber());
		}
		else if(attrID == kTextAttrPairKernBoss && exportTextCmd)
		{
			command += "[TK=";
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			PMReal kerning = txtAttrReal->GetRealNumber();
			if(kerning == kAutoKernMagicNumber)
				command += "0;TRUE";
			else
			{
				command.AppendNumber(kerning * 1000.0,3,kTrue,kTrue);
				checker->checkKerning(kerning * 1000.0);
			}
			command += "]";
		}
		else if(attrID == kTextAttrColorBoss && exportTextCmd)
		{
			command += "[TD=";
			InterfacePtr<ITextAttrUID> txtAttrUID (inter, IID_ITEXTATTRUID);
			if(txtAttrUID == nil)
				break;
			
			command.AppendNumber(getColorRef(txtAttrUID->GetUIDData(), "TD", 1));
			command += "]";
		}
		else if(attrID == kTextAttrPointSizeBoss && exportTextCmd)
		{
			command += "[TC=D";
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			command.AppendNumber(txtAttrReal->GetRealNumber(),3,kTrue,kTrue);
			command += "]";
			checker->checkFontSize(txtAttrReal->GetRealNumber());
		}
		else if(attrID == kTextAttrPairKernMethodBoss && exportTextCmd)
		{
			command += "[TY=";
			InterfacePtr<ITextAttrClassID> txtAttrClass (inter, IID_ITEXTATTRCLASSID);
			if(txtAttrClass == nil)
				break;
			
			ClassID method = txtAttrClass->Get();
				if(method == kDefaultPairKerningBoss)
					command += "0";
				else if(method == kKFPairKerningBoss)
					command += "1";
				else if(method == kPairKerningOffBoss)
					command += "2";
				else
				{
					command += "0";
					checker->WriteReverseLog(PMString(kErrUnknownType), "TY", 1);
				}
			command += "]";
		}
		else if(attrID == kTextAttrFontStyleBoss && exportTextCmd)
		{
			InterfacePtr<ITextAttrFont> txtAttrFontStyle (inter, IID_ITEXTATTRFONT);
			if(txtAttrFontStyle == nil)
				break;
			
			PMString fontStyle = txtAttrFontStyle->GetFontName();
			command += "[TM=\"";
			command += fontStyle;
			command += "\"]";
		}
		else if(attrID == kTextAttrXGlyphScaleBoss && exportTextCmd)
		{
			command += "[TH=TRUE;";
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			command.AppendNumber(txtAttrReal->GetRealNumber() * 100.0,3,kTrue,kTrue);
			command += "]";
			checker->checkGlyphScale("TH", txtAttrReal->GetRealNumber() * 100.0);
		}
		else if(attrID == kTextAttrYGlyphScaleBoss && exportTextCmd)
		{
			command += "[TV=TRUE;";
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			command.AppendNumber(txtAttrReal->GetRealNumber() * 100.0,3,kTrue,kTrue);
			command += "]";
			checker->checkGlyphScale("TV", txtAttrReal->GetRealNumber() * 100.0);
		}
		else if(attrID == kTextAttrTrackKernBoss && exportTextCmd)
		{
			command += "[TA=";
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			command.AppendNumber(txtAttrReal->GetRealNumber() * 1000.0,3,kTrue,kTrue);
			command += "]";
			checker->checkTracking(txtAttrReal->GetRealNumber() * 1000.0);
		}
		else if(attrID == kTextAttrBLShiftBoss && exportTextCmd)
		{
			command += "[TL=D";
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			command.AppendNumber(txtAttrReal->GetRealNumber(),3,kTrue,kTrue);
			command += "]";
			checker->checkBaselineOffset(txtAttrReal->GetRealNumber());
		}
		else if(attrID == kTextAttrTintBoss && exportTextCmd)
		{	
			command += "[TT=";
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			command.AppendNumber(txtAttrReal->GetRealNumber(),3,kTrue,kTrue);
			command += "]";
			checker->checkTextTint(txtAttrReal->GetRealNumber());
		}
		else if(attrID == kTextAttrOverprintBoss && exportTextCmd)
		{     
			command += "[TO=";
			InterfacePtr<ITextAttrBoolean> txtAttrBool (inter, IID_ITEXTATTRBOOLEAN);
			if(txtAttrBool == nil)
				break;
			
			if(txtAttrBool->Get() == kTrue)
				command.Append("TRUE");
			else
				command.Append("FALSE");
			command += "]";
		}
		else if(attrID == kTextAttrFontUIDBoss && exportTextCmd)
		{
			command += "[TN=\"";
			InterfacePtr<ITextAttrUID> txtAttrUID (inter, IID_ITEXTATTRUID);
			if(txtAttrUID == nil)
				break;
			
			InterfacePtr<IFontFamily> font (curDB, txtAttrUID->GetUIDData(), UseDefaultIID());
			if(font == nil)
				break;
			
			command.Append(font->GetDisplayFamilyName());
			command += "\"]";
		}
		else if(attrID == kTextAttrUnderlineBoss && exportTextCmd)
		{
			InterfacePtr<ITextAttrUnderlineMode> txtAttrUnderline(inter, IID_ITEXTATTRUNDERLINEMODE);
			if (txtAttrUnderline == nil) 
				break;
				
			if(txtAttrUnderline->GetMode() == IDrawingStyle::kUnderlineSingle)
			{
				valTG += 4;
				attrTG = kTrue;
			}
			else
			{
				valTJ += 4;
				attrTJ = kTrue;
			}
		}
		else if(attrID == kTextAttrStrikethruBoss && exportTextCmd)
		{
			InterfacePtr<ITextAttrBoolean> txtAttrStrikeThru(inter, IID_ITEXTATTRBOOLEAN);
			if(txtAttrStrikeThru == nil)
				break;
			
			if(txtAttrStrikeThru->GetFlag() == kTrue)
			{
				valTG += 16;
				attrTG = kTrue;
			}
			else
			{
				valTJ += 16;
				attrTJ = kTrue;
			}	
		}
		else if(attrID == kTextAttrCapitalModeBoss && exportTextCmd)
		{
			InterfacePtr<ITextAttrCapitalMode> txtAttrCapital(inter, IID_ITEXTATTRCAPITALMODE);
			if(txtAttrCapital == nil)
				break;
			
			if(txtAttrCapital->GetMode() == IDrawingStyle::kCapNormal)
			{
				valTJ += 128;
				attrTJ = kTrue;
			}
			else
			{
				attrTG = kTrue;
				if(txtAttrCapital->GetMode() == IDrawingStyle::kCapSmallLowercase)
					valTG += 256;
				else if(txtAttrCapital->GetMode() == IDrawingStyle::kCapAll)
					valTG += 128;
				else
					checker->WriteReverseLog(PMString(kErrUnknownCapMode), "TG", 1);
			}
		}
		else if(attrID == kTextAttrPositionModeBoss && exportTextCmd)
		{
			InterfacePtr<ITextAttrPositionMode> txtAttrPos(inter, IID_ITEXTATTRPOSITIONMODE);
			if(txtAttrPos == nil)
				break;
			
			if(txtAttrPos->GetMode() == IDrawingStyle::kPosNormal)
			{
				valTJ += 512;
				attrTJ = kTrue;
			}
			else
			{
				bool8 applyTG = kTrue;
				attrTG = kTrue;
				if(txtAttrPos->GetMode() == IDrawingStyle::kPosSuperscript)
					valTG += 512;
				else if(txtAttrPos->GetMode() == IDrawingStyle::kPosSubscript)
					valTG += 1024;
				else if(txtAttrPos->GetMode() == IDrawingStyle::kPosSuperior)
					valTG += 2048;
				else if(txtAttrPos->GetMode() == IDrawingStyle::kPosInferior)
					valTG += 4096;
				else
					checker->WriteReverseLog(PMString(kErrUnknownTextPosition), "TG", 1);
			}
		}
		else if(attrID == kTextAttrNoBreakBoss && exportTextCmd)
		{
			InterfacePtr<ITextAttrBoolean> txtAttrNoBreak(inter, IID_ITEXTATTRBOOLEAN);
			if(txtAttrNoBreak == nil)
				break;
			
			if(txtAttrNoBreak->GetFlag() == kTrue)
			{
				valTG += 8192;
				attrTG = kTrue;
			}
			else
			{
				valTJ += 8192;
				attrTJ = kTrue;
			}
		}
		else if(attrID == kTextAttrDropCapCharsBoss)
		{
			InterfacePtr<ITextAttrInt16> txtAttrChars (inter, IID_ITEXTATTRINT16);
			if(txtAttrChars == nil)
				break;
			
			nbChar = txtAttrChars->Get();
			attrPL = kTrue;
		}
		else if(attrID == kTextAttrDropCapLinesBoss)
		{
			InterfacePtr<ITextAttrInt16> txtAttrChars (inter, IID_ITEXTATTRINT16);
			if(txtAttrChars == nil)
				break;
			
			nbLine = txtAttrChars->Get();
			attrPL = kTrue;
		}
		else if(attrID == kTextAttrSpaceBeforeBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			spaceBef = txtAttrReal->GetRealNumber();
			attrPE = kTrue;
		}
		else if(attrID == kTextAttrSpaceAfterBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			spaceAft = txtAttrReal->GetRealNumber();
			attrPE = kTrue;
		}
		else if(attrID == kTextAttrGridAlignmentBoss && exportParaCmd)
		{
			command += "[PG=";
			InterfacePtr<ITextAttrGridAlignment> txtGridAlign (inter, IID_ITEXTATTRGRIDALIGNMENT);
			if(txtGridAlign == nil)
				break;
			
			if(txtGridAlign->GetGridAlignment() == Text::kGABaseline)
				command += trueS;
			else 
			{
				command += falseS;
				if(txtGridAlign->GetGridAlignment() != Text::kGANone)
					checker->WriteReverseLog(PMString(kErrUnknownType), "PG", 1);
			}
			
			command += "]";
		}
		else if(attrID == kTextAttrKeepWithNextBoss && exportParaCmd)
		{
			command += "[PK=";
			InterfacePtr<ITextAttrInt16> txtAttrKeepWithNext (inter, IID_ITEXTATTRINT16);
			if(txtAttrKeepWithNext == nil)
				break;
			
			if(txtAttrKeepWithNext->Get() == 0)
				command += falseS;
			else
			{
				command += trueS;
				if(txtAttrKeepWithNext->Get() > 1)
					checker->WriteReverseLog(PMString(kErrUnknownType), "PK", 1);
			}
			
			command += "]";
		}
		else if(attrID == kTextAttrLanguageBoss && exportTextCmd)
		{
			command += "[CL=";

			InterfacePtr<ITextAttrUID> txtAttrLanguage (inter, IID_ITEXTATTRUID);
			if(txtAttrLanguage == nil)
				break;
							
			command.AppendNumber(getLanguageRef(txtAttrLanguage->Get(), "CL", 1));
				
			command += "]";
		}
		else if(attrID == kTextAttrKeepLinesBoss)
		{
			InterfacePtr<ITextAttrBoolean> txtAttrKeepLines(inter, IID_ITEXTATTRBOOLEAN);
			if(txtAttrKeepLines == nil)
				break;
			
			keepLines = txtAttrKeepLines->Get();
			attrPH = kTrue;
		}
		else if(attrID == kTextAttrKeepTogetherBoss)
		{
			InterfacePtr<ITextAttrBoolean> txtAttrKeepTogether(inter, IID_ITEXTATTRBOOLEAN);
			if(txtAttrKeepTogether == nil)
				break;
			
			keepAll = txtAttrKeepTogether->Get(); 
			attrPH = kTrue;
		}
		else if(attrID == kTextAttrKeepFirstNLinesBoss)
		{
			InterfacePtr<ITextAttrInt16> txtAttrFirstLines (inter, IID_ITEXTATTRINT16);
			if(txtAttrFirstLines == nil)
				break;
			
			firstLines = txtAttrFirstLines->Get();
			attrPH = kTrue;
		}
		else if(attrID == kTextAttrKeepLastNLinesBoss)
		{
			InterfacePtr<ITextAttrInt16> txtAttrLastLines (inter, IID_ITEXTATTRINT16);
			if(txtAttrLastLines == nil)
				break;
			
			lastLines = txtAttrLastLines->Get();
			attrPH = kTrue;
		}
		else if(attrID == kTextAttrAlignmentBoss && exportParaCmd)
		{
			command += "[PJ=";
			InterfacePtr<ITextAttrAlign> txtAttrAlign(inter, IID_ITEXTATTRALIGN);
			if(txtAttrAlign == nil)
				break;
			
			if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignLeft)
				command.AppendNumber(0);
			else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignCenter)
				command.AppendNumber(1);
			else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignRight)
				command.AppendNumber(2);
			else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignJustifyFull)
				command.AppendNumber(3);
			else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignJustifyLeft)
				command.AppendNumber(4);
			else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignJustifyCenter)
				command.AppendNumber(5);
			else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignJustifyRight)
				command.AppendNumber(6);
			else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignToBinding)
				command.AppendNumber(7);
			else if(txtAttrAlign->GetAlignment() == ICompositionStyle::kTextAlignAwayBinding)
				command.AppendNumber(8);
			else
			{
				command.AppendNumber(0);
				checker->WriteReverseLog(PMString(kErrUnknownType), "PJ", 1);
			}
			
			command += "]";
		}
		else if(attrID == kTextAttrPRAColorBoss)
		{
			InterfacePtr<ITextAttrUID> txtAttrUID (inter, IID_ITEXTATTRUID);
			if(txtAttrUID == nil)
				break;
			
			colorPA = getColorRef(txtAttrUID->GetUIDData(), "PA", 7); 
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAIndentLBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			lIndentPA = txtAttrReal->GetRealNumber();
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAIndentRBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			rIndentPA = txtAttrReal->GetRealNumber();
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAModeBoss)
		{
			InterfacePtr<ITextAttrParaRuleMode> txtAttrMode (inter, IID_ITEXTATTRPARARULEMODE);
			if(txtAttrMode == nil)
				break;
			
			if(txtAttrMode->Get() == ICompositionStyle::kRule_ColumnWidth)
				modePA = 0;
			else
				modePA = 1;
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAOffsetBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			offsetPA = txtAttrReal->GetRealNumber();
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRARuleOnBoss)
		{
			InterfacePtr<ITextAttrBoolean> txtAttrPRAOn(inter, IID_ITEXTATTRBOOLEAN);
			if(txtAttrPRAOn == nil)
				break;
			
			PAon = txtAttrPRAOn->Get();
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAStrokeBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			strokePA = txtAttrReal->GetRealNumber();
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRATintBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			tintPA = txtAttrReal->GetRealNumber();
			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRAStrokeTypeBoss)
		{
			InterfacePtr<ITextAttrStrokeType> txtAttrType (inter, IID_ITEXTATTRSTROKETYPE);
			if(txtAttrType == nil)
				break;
			
			if(txtAttrType->GetClassIDData() == kSolidPathStrokerBoss)
				typePA = 0;
			else if(txtAttrType->GetClassIDData() == kDashedPathStrokerBoss)
				typePA = 1;
			else if(txtAttrType->GetClassIDData() == kThinThinPathStrokerBoss)
				typePA = 2;
			else if(txtAttrType->GetClassIDData() == kThickThickPathStrokerBoss)
				typePA = 3;
			else if(txtAttrType->GetClassIDData() == kDashedPathStrokerBoss)
				typePA = 4;
			else if(txtAttrType->GetClassIDData() == kCannedDotPathStrokerBoss)
				typePA = 5;
			else if(txtAttrType->GetClassIDData() == kThinThickPathStrokerBoss)
				typePA = 6;
			else if(txtAttrType->GetClassIDData() == kThickThinPathStrokerBoss)
				typePA = 7;
			else if(txtAttrType->GetClassIDData() == kThinThickThinPathStrokerBoss)
				typePA = 8;
			else if(txtAttrType->GetClassIDData() == kThickThinThickPathStrokerBoss)
				typePA = 9;
			else if(txtAttrType->GetClassIDData() == kTriplePathStrokerBoss)
				typePA = 10;
			else if(txtAttrType->GetClassIDData() == kCannedDash3x2PathStrokerBoss)
				typePA = 11;
			else if(txtAttrType->GetClassIDData() == kCannedDash4x4PathStrokerBoss)
				typePA = 12;
			else if(txtAttrType->GetClassIDData() == kLeftSlantHashPathStrokerBoss)
				typePA = 13;
			else if(txtAttrType->GetClassIDData() == kRightSlantHashPathStrokerBoss)
				typePA = 14;
			else if(txtAttrType->GetClassIDData() == kStraightHashPathStrokerBoss)
				typePA = 15;
			else if(txtAttrType->GetClassIDData() == kSingleWavyPathStrokerBoss)
				typePA = 16;
			else if(txtAttrType->GetClassIDData() == kWhiteDiamondPathStrokerBoss)
				typePA = 17;
			else if(txtAttrType->GetClassIDData() == kJapaneseDotsPathStrokerBoss)
				typePA = 18;
			else
				checker->WriteReverseLog(PMString(kErrUnknownType), "PA", 6);

			attrPA = kTrue;
		}
		else if(attrID == kTextAttrPRBColorBoss)
		{
			InterfacePtr<ITextAttrUID> txtAttrUID (inter, IID_ITEXTATTRUID);
			if(txtAttrUID == nil)
				break;
			
			colorPP = getColorRef(txtAttrUID->GetUIDData(), "PP", 7); 
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBIndentLBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			lIndentPP = txtAttrReal->GetRealNumber();
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBIndentRBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			rIndentPP = txtAttrReal->GetRealNumber();
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBModeBoss)
		{
			InterfacePtr<ITextAttrParaRuleMode> txtAttrMode (inter, IID_ITEXTATTRPARARULEMODE);
			if(txtAttrMode == nil)
				break;
			
			if(txtAttrMode->Get() == ICompositionStyle::kRule_ColumnWidth)
				modePP = kFalse;
			else
				modePP = kTrue;
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBOffsetBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			offsetPP = txtAttrReal->GetRealNumber();
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBRuleOnBoss)
		{
			InterfacePtr<ITextAttrBoolean> txtAttrPRBOn(inter, IID_ITEXTATTRBOOLEAN);
			if(txtAttrPRBOn == nil)
				break;
			
			PPon = txtAttrPRBOn->Get();
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBStrokeBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			strokePP = txtAttrReal->GetRealNumber();
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBTintBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			tintPP = txtAttrReal->GetRealNumber();
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrPRBStrokeTypeBoss)
		{
			InterfacePtr<ITextAttrStrokeType> txtAttrType (inter, IID_ITEXTATTRSTROKETYPE);
			if(txtAttrType == nil)
				break;
			
			if(txtAttrType->GetClassIDData() == kSolidPathStrokerBoss)
				typePP = 0;
			else if(txtAttrType->GetClassIDData() == kDashedPathStrokerBoss)
				typePP = 1;
			else if(txtAttrType->GetClassIDData() == kThinThinPathStrokerBoss)
				typePP = 2;
			else if(txtAttrType->GetClassIDData() == kThickThickPathStrokerBoss)
				typePP = 3;
			else if(txtAttrType->GetClassIDData() == kDashedPathStrokerBoss)
				typePP = 4;
			else if(txtAttrType->GetClassIDData() == kCannedDotPathStrokerBoss)
				typePP = 5;
			else if(txtAttrType->GetClassIDData() == kThinThickPathStrokerBoss)
				typePP = 6;
			else if(txtAttrType->GetClassIDData() == kThickThinPathStrokerBoss)
				typePP = 7;
			else if(txtAttrType->GetClassIDData() == kThinThickThinPathStrokerBoss)
				typePP = 8;
			else if(txtAttrType->GetClassIDData() == kThickThinThickPathStrokerBoss)
				typePP = 9;
			else if(txtAttrType->GetClassIDData() == kTriplePathStrokerBoss)
				typePP = 10;
			else if(txtAttrType->GetClassIDData() == kCannedDash3x2PathStrokerBoss)
				typePP = 11;
			else if(txtAttrType->GetClassIDData() == kCannedDash4x4PathStrokerBoss)
				typePP = 12;
			else if(txtAttrType->GetClassIDData() == kLeftSlantHashPathStrokerBoss)
				typePP = 13;
			else if(txtAttrType->GetClassIDData() == kRightSlantHashPathStrokerBoss)
				typePP = 14;
			else if(txtAttrType->GetClassIDData() == kStraightHashPathStrokerBoss)
				typePP = 15;
			else if(txtAttrType->GetClassIDData() == kSingleWavyPathStrokerBoss)
				typePP = 16;
			else if(txtAttrType->GetClassIDData() == kWhiteDiamondPathStrokerBoss)
				typePP = 17;
			else if(txtAttrType->GetClassIDData() == kJapaneseDotsPathStrokerBoss)
				typePP = 18;
			else
				checker->WriteReverseLog(PMString(kErrUnknownType), "PP", 6);
			attrPP = kTrue;
		}
		else if(attrID == kTextAttrIndent1LBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			indentFirst = txtAttrReal->GetRealNumber();
			attrPR = kTrue;
		}
		else if(attrID == kTextAttrIndentBLBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			indentLeft = txtAttrReal->GetRealNumber();
			attrPR = kTrue;
		}
		else if(attrID == kTextAttrIndentBRBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			indentRight = txtAttrReal->GetRealNumber();
			attrPR = kTrue;
		}
		else if(attrID == kTextAttrTabsBoss && exportParaCmd)
		{
			command += "[PT=";
			InterfacePtr<ITextAttrTabSettings> txtAttrTabs (inter, IID_ITEXTATTRTABS);
			if(txtAttrTabs == nil)
				break;
			
			TabStopTable tabs = txtAttrTabs->GetTabStopTable();
			if(tabs.size() == 0)
				command += "D-1;0;' ']";
			else
			{
				for(int i = 0 ; i < tabs.size() ; ++i)
				{
					const TabStop tab = tabs[i];
					command += "D";
					command.AppendNumber(tab.GetPosition(),3,kTrue,kTrue);
					checker->checkTabPosition(tab.GetPosition(), 1 + i * 3);

					command += ";";
					if(tab.GetAlignment() == TabStop::kTabAlignLeft)
					{
						command.AppendNumber(0);
						command += ";' ';";
					}
					else if(tab.GetAlignment() == TabStop::kTabAlignCenter)
					{
						command.AppendNumber(1);
						command += ";' ';";
					}
					else if(tab.GetAlignment() == TabStop::kTabAlignRight)
					{
						command.AppendNumber(2);
						command += ";' ';";
					}
					else if(tab.GetAlignment() == TabStop::kTabAlignChar)
					{
						command.AppendNumber(3);
						command += ";'";
						command.AppendW(tab.GetAlignToChar());
						command += "';";
					}
				}
			}
			command.Remove(command.NumUTF16TextChars() - 1,1);
			command += "]";						
		}
		else if(attrID == kTextAttrHyphenCapBoss)
		{
			InterfacePtr<ITextAttrBoolean> txtAttrHyphenCap (inter, IID_ITEXTATTRBOOLEAN);
			if(txtAttrHyphenCap == nil)
				break;
			
			capitalized = txtAttrHyphenCap->Get();
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrHyphenZoneBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			hyphenZone = txtAttrReal->GetRealNumber();
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrShortestWordBoss)
		{
			InterfacePtr<ITextAttrInt16> txtAttrShortest (inter, IID_ITEXTATTRINT16);
			if(txtAttrShortest == nil)
				break;
			
			minWord = txtAttrShortest->Get();
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrMinAfterBoss)
		{
			InterfacePtr<ITextAttrInt16> txtAttrMinAfter (inter, IID_ITEXTATTRINT16);
			if(txtAttrMinAfter == nil)
				break;
			
			afterFirst = txtAttrMinAfter->Get();
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrMinBeforeBoss)
		{
			InterfacePtr<ITextAttrInt16> txtAttrMinBefore (inter, IID_ITEXTATTRINT16);
			if(txtAttrMinBefore == nil)
				break;
			
			beforeLast = txtAttrMinBefore->Get();
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrHyphenModeBoss)
		{
			InterfacePtr<ITextAttrHyphenMode> txtAttrHyphenMode (inter, IID_ITEXTATTRHYPHENMODE);
			if(txtAttrHyphenMode == nil)
				break;
			
			if(txtAttrHyphenMode->GetMethod() == IHyphenationStyle::kHyphensOff)
				hyphenMode = kFalse;
			else
				hyphenMode = kTrue;
		
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrHyphenLastBoss)
		{
			InterfacePtr<ITextAttrBoolean> txtAttrHyphenLast (inter, IID_ITEXTATTRBOOLEAN);
			if(txtAttrHyphenLast == nil)
				break;
			
			lastWord = txtAttrHyphenLast->Get();
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrLetterspaceDesBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			letterDes = txtAttrReal->GetRealNumber() * 100.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrLetterspaceMaxBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			letterMax = txtAttrReal->GetRealNumber() * 100.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrLetterspaceMinBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			letterMin = txtAttrReal->GetRealNumber() * 100.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrWordspaceDesBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			wordDes = txtAttrReal->GetRealNumber() * 100.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrWordspaceMaxBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			wordMax = txtAttrReal->GetRealNumber() * 100.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrWordspaceMinBoss)
		{
			InterfacePtr<ITextAttrRealNumber> txtAttrReal (inter, IID_ITEXTATTRREALNUMBER);
			if(txtAttrReal == nil)
				break;
			
			wordMin = txtAttrReal->GetRealNumber() * 100.0;
			attrCJ = kTrue;
		}
		else if(attrID == kTextAttrHyphenLadderBoss)
		{
			InterfacePtr<ITextAttrInt16> txtAttrLimit (inter, IID_ITEXTATTRINT16);
			if(txtAttrLimit == nil)
				break;
					
			limit = txtAttrLimit->Get();
			attrCJ = kTrue;
		}
		else if(attrID == kPermRefsAttrBoss && exportTextCmd)
		{
			InterfacePtr<IPermRefsTag> txtAttrPermRef (inter, IID_IPERMREFSTAG);	
			if(txtAttrPermRef == nil)
				break;

			PermRefStruct permref = txtAttrPermRef->getReference();
			if(permref != kNullPermRef
			&& permref.ref > 0) // Bug fix (from converted documents, this value can negative or equal to zero sometimes in the stylesheets)
			{
				if(permrefFound) // this permref is just after an other one
					command += "[HF]";
				else
					permrefFound = kTrue;
				command += "[HD=";	
				command.AppendNumber(permref.ref);
				command += ";";
				command.AppendNumber(permref.param1);
				command += ";";
				command.AppendNumber(permref.param2);
				command += ";";
				command.AppendNumber(permref.param3);
				command += ";";
				command.AppendNumber(permref.param4);
				command += "]";
			}
		}
		command.SetTranslatable(kFalse);
	
		status = kSuccess;
	
	} while(kFalse);
	
	if(status != kSuccess)
		throw TCLError(error);
	
	return command;
}	

void PrsTCLWriter::writeNewTextFrame(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, Text::VerticalJustification vj, PMReal offset, int32 numPage, UID color, PMReal tint,
					   int32 nbCol, PMReal gutter, PMReal angle, PMReal vjMax, Text::FirstLineOffsetMetric topAlign, bool8 isInline, TCLRef refOwner,bool8 toLink,TCLRef refLink)
{
	// Convert values to millimeters
	InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));
	
	leftTop.X() = uom->PointsToUnits(leftTop.X());
	leftTop.Y() = uom->PointsToUnits(leftTop.Y());
	width = uom->PointsToUnits(width);
	height = uom->PointsToUnits(height);
	offset = uom->PointsToUnits(offset);
	gutter = uom->PointsToUnits(gutter);
	vjMax = uom->PointsToUnits(vjMax);
	
	PMString command("[BN=");
	
	WriteTCLRef(command,ref);
	command += ";N";
	command .AppendNumber(leftTop.X(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(leftTop.Y(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(width,3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(height,3,kTrue,kTrue);
	command += ";";
	
	if(vj == Text::kVJTop)
		command += "0;";
	else if(vj == Text::kVJJustify)
		command += "1;" ;
	else if(vj == Text::kVJCenter)
		command += "2;" ;
	else if(vj == Text::kVJBottom)
		command += "3;" ;
	else
		command += "0;";
		
	command += "N";
	command.AppendNumber(offset,3,kTrue,kTrue);
	command += ";";	
	command.AppendNumber(getColorRef(color, "BN", 8));
	command += ";";
	command.AppendNumber(tint);
	command += ";";
	
	if(toLink)
		command += "TRUE;"; 
	else
		command += "FALSE;";

	WriteTCLRef(command,refLink);
	command += ";";
	command.AppendNumber(nbCol);
	command += ";N";
	command.AppendNumber(gutter,3,kTrue,kTrue);
	command += ";N0;";
	command.AppendNumber(angle,0,kTrue,kTrue);
	command += ";";
	
	if(isInline)
		command += "TRUE;"; 
	else
		command += "FALSE;";
	
	WriteTCLRef(command,refOwner);
	command += ";";
	command += "FALSE;";
	
	command.AppendNumber(numPage);
	command += ";0;N";
	
	command.AppendNumber(vjMax);
	command += ";";
	
	if(topAlign == Text::kFLOAscent)
		command += "0";
	else if(topAlign == Text::kFLOxHeight)
		command += "1";
	else if(topAlign == Text::kFLOCapHeight)
		command += "2";
	else if(topAlign == Text::kFLOLeading)
		command += "3";
	else if(topAlign == Text::kFLOFixedHeight)
		command += "4";
	else
		command += "0";
	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkNewTextFrame(ref,leftTop,width,height,vj,offset,numPage,color,tint,
								nbCol,gutter,angle,vjMax,topAlign,isInline,refOwner,toLink,refLink);
	checker->NewLine();
}

void PrsTCLWriter::writeNewTextSpline(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList, Text::VerticalJustification vj, PMReal offset, int32 numPage, UID color, PMReal tint,
										int32 nbCol, PMReal gutter, PMReal angle, PMReal vjMax, Text::FirstLineOffsetMetric topAlign, bool8 isInline, TCLRef refOwner,bool8 toLink,TCLRef refLink)
{
	// Convert values to millimeters
	int32 numParam = 1;

	InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));
	
	leftTop.X() = uom->PointsToUnits(leftTop.X());
	leftTop.Y() = uom->PointsToUnits(leftTop.Y());
	width = uom->PointsToUnits(width);
	height = uom->PointsToUnits(height);
	offset = uom->PointsToUnits(offset);
	gutter = uom->PointsToUnits(gutter);
	vjMax = uom->PointsToUnits(vjMax);

	PMString command("[B*=");
	
	WriteTCLRef(command,ref);
	command += ";N";
	command.AppendNumber(leftTop.X(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(leftTop.Y(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(width,3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(height,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(ptsList.size());
	numParam = 6;
	for(int32 i = 0 ; i < ptsList.size() ; ++i)
	{
		command += ";";
		command.AppendNumber(ptsList[i].Key().size());
		command += ";";
		if(ptsList[i].Value())
			command += "TRUE";
		else
			command += "FALSE";

		numParam += 2;
		for(int32 j = 0 ; j < ptsList[i].Key().size() ; ++j)
		{
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetLeftDirPoint().X()),3,kTrue,kTrue);
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetLeftDirPoint().Y()),3,kTrue,kTrue);
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetAnchorPoint().X()),3,kTrue,kTrue);
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetAnchorPoint().Y()),3,kTrue,kTrue);
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetRightDirPoint().X()),3,kTrue,kTrue);
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetRightDirPoint().Y()),3,kTrue,kTrue);
			numParam += 6;
		}		
	}
	command += ";";
	if(vj == Text::kVJTop)
		command += "0;";
	else if(vj == Text::kVJJustify)
		command += "1;" ;
	else if(vj == Text::kVJCenter)
		command += "2;" ;
	else if(vj == Text::kVJBottom)
		command += "3;" ;
	else
		command += "0;";
		
	command += "N";
	command.AppendNumber(offset,3,kTrue,kTrue);
	command += ";";	
	command.AppendNumber(getColorRef(color, "B*", numParam + 3));
	command += ";";
	command.AppendNumber(tint);
	command += ";";
	
	if(toLink)
		command += "TRUE;"; 
	else
		command += "FALSE;";

	WriteTCLRef(command,refLink);
	command += ";";
	command.AppendNumber(nbCol);
	command += ";N";
	command.AppendNumber(gutter,3,kTrue,kTrue);
	command += ";N0;";
	command.AppendNumber(angle,0,kTrue,kTrue);
	command += ";";
	
	if(isInline)
		command += "TRUE;"; 		
	else
		command += "FALSE;";

	WriteTCLRef(command,refOwner);
	command += ";FALSE;"; // Relative = false
	command.AppendNumber(numPage);
	command += ";0;N";
	
	command.AppendNumber(vjMax);
	command += ";";
	
	if(topAlign == Text::kFLOAscent)
		command += "0";
	else if(topAlign == Text::kFLOxHeight)
		command += "1";
	else if(topAlign == Text::kFLOCapHeight)
		command += "2";
	else if(topAlign == Text::kFLOLeading)
		command += "3";
	else if(topAlign == Text::kFLOFixedHeight)
		command += "4";
	else
		command += "0";
	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkNewTextSpline(ref,leftTop,width,height,ptsList,vj,offset,numPage,color,tint,
								nbCol,gutter,angle,vjMax,topAlign,isInline,refOwner,toLink,refLink);
	checker->NewLine();
}

void PrsTCLWriter::writeSetInsets(TCLRef ref, PMReal top, PMReal left, PMReal bottom, PMReal right)
{
	PMString command("[BJ=");
	
	WriteTCLRef(command,ref);
	command += ";D";
	command.AppendNumber(top,3,kTrue,kTrue);
	command += ";D";
	command.AppendNumber(left,3,kTrue,kTrue);
	command += ";D";
	command.AppendNumber(bottom,3,kTrue,kTrue);
	command += ";D";
	command.AppendNumber(right,3,kTrue,kTrue);
	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkSetInsets(ref,top,left,bottom,right);
	checker->NewLine();
}

void PrsTCLWriter::writeNonPrint(TCLRef ref, bool8 nonPrint)
{
	PMString command("[BA=");
	
	WriteTCLRef(command,ref);
	if(nonPrint)
		command += ";TRUE";
	else
		command += ";FALSE";

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::writeIsLocked(TCLRef ref, bool8 locked)
{
	PMString command("[BV=");
	
	if(locked)
		command += "TRUE;";
	else
		command += "FALSE;";

	WriteTCLRef(command,ref);

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::writeFlipItem(TCLRef ref, bool8 flipH, bool8 flipV)
{
	PMString command("[BU=");
	
	WriteTCLRef(command,ref);;
	if(flipH)
		command += ";TRUE;";
	else
		command += ";FALSE;";
	if(flipV)
		command += "TRUE";
	else
		command += "FALSE";
	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::writeSkewItem(TCLRef ref, PMReal skewAngle)
{
	PMString command("[BE=");
	
	WriteTCLRef(command,ref);
	command += ";";
	command.AppendNumber(skewAngle,0,kTrue,kTrue);
	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkSkewItem(ref,skewAngle);
	checker->NewLine();
}

void PrsTCLWriter::writeSetTextWrap(TCLRef ref, IStandOff::mode mode, PMReal top, PMReal left, PMReal bottom, PMReal right, IStandOff::form form, IStandOffContourWrapSettings::ContourWrapType contourType)
{
	PMString command("[BH=");
	
	WriteTCLRef(command,ref);
	command += ";";
	
	if(mode == IStandOff::kNone)
		command += "1";
	else 
	{
		if(mode == IStandOff::kManualContour)
		{
			if(contourType == IStandOffContourWrapSettings::kGraphicBounds)
				command += "2;";
			else if(contourType == IStandOffContourWrapSettings::kEmbeddedPath)
				command += "3;";
			else if(contourType == IStandOffContourWrapSettings::kAlpha)
				command += "4;";
			else if(contourType == IStandOffContourWrapSettings::kEdgeDetection)
				command += "5;";
			else if(contourType == IStandOffContourWrapSettings::kSameAsClip)
				command += "6;";
			else
				command += "6;";
		}
		else if(mode == IStandOff::kBand)
			command += "10;";
		else if(mode == IStandOff::kNextFrame)
			command += "11;";
		else if(mode == IStandOff::kBothSides)
			command += "0;";
		else
			command += "0;";
	
		command += "D";
		command.AppendNumber(top,3,kTrue,kTrue);
		command += ";D";
		command.AppendNumber(left,3,kTrue,kTrue);
		command += ";D";
		command.AppendNumber(bottom,3,kTrue,kTrue);
		command += ";D";
		command.AppendNumber(right,3,kTrue,kTrue);
		command += ";";
		
		if(form == IStandOff::kInvert)
			command += "TRUE";
		else
			command += "FALSE";
	}	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkSetTextWrap(ref,mode,top,left,bottom,right,form,contourType);
	checker->NewLine();
}

void PrsTCLWriter::writeSetStroke(TCLRef ref, PMReal weight, ClassID type, UID color, PMReal tint, ClassID cornerType, PMReal cornerRadius, UID gapColor, PMReal gapTint, int32 strokeAlign)
{
	
	PMString command("[BC=");
	
	WriteTCLRef(command,ref);
	command += ";D";
	command.AppendNumber(weight,3,kTrue,kTrue);
	command += ";";
	
	if(type == kSolidPathStrokerBoss)
		command += "0;";
	else if(type == kDashedPathStrokerBoss)
		command += "1;";
	else if(type == kThinThinPathStrokerBoss)
		command += "2;";
	else if(type == kThickThickPathStrokerBoss)
		command += "3;";
	else if(type == kDashedPathStrokerBoss)
		command += "4;";
	else if(type == kCannedDotPathStrokerBoss)
		command += "5;";
	else if(type == kThinThickPathStrokerBoss)
		command += "6;";
	else if(type == kThickThinPathStrokerBoss)
		command += "7;";
	else if(type == kThinThickThinPathStrokerBoss)
		command += "8;";
	else if(type == kThickThinThickPathStrokerBoss)
		command += "9;";
	else if(type == kTriplePathStrokerBoss)
		command += "10;";
	else if(type == kCannedDash3x2PathStrokerBoss)
		command += "11;";
	else if(type == kCannedDash4x4PathStrokerBoss)
		command += "12;";
	else if(type == kLeftSlantHashPathStrokerBoss)
		command += "13;";
	else if(type == kRightSlantHashPathStrokerBoss)
		command += "14;";
	else if(type == kStraightHashPathStrokerBoss)
		command += "15;";
	else if(type == kSingleWavyPathStrokerBoss)
		command += "16;";
	else if(type == kWhiteDiamondPathStrokerBoss)
		command += "17;";
	else if(type == kJapaneseDotsPathStrokerBoss)
		command += "18;";
	else
		command += "0;";
	
	command.AppendNumber(getColorRef(color, "BC", 4));
	command += ";";
	command.AppendNumber(tint,3,kTrue,kTrue);
	command += ";";
	
	if(cornerType == kRoundedCornerBoss)
		command += "1;D";
	else if(cornerType == kInverseRoundedCornerBoss)
		command += "2;D";
	else if(cornerType == kBevelCornerBoss)
		command += "3;D";
	else if(cornerType == kInsetCornerBoss)
		command += "4;D";
	else if(cornerType == kFancyCornerBoss)
		command += "5;D";
	else
		command += "0;D";
	
	command.AppendNumber(cornerRadius,3,kTrue,kTrue);
	
	command +=";;";
	command.AppendNumber(getColorRef(gapColor, "BC", 9));
	command += ";";
	command.AppendNumber(gapTint,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(strokeAlign,3,kTrue,kTrue);
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkSetStroke(ref,weight,type,color,tint,cornerType,cornerRadius,gapColor,gapTint,strokeAlign);
	checker->NewLine();
}

void PrsTCLWriter::writeShadowEffect(TCLRef ref, bool16 mode, int8 blendMode, PMReal xOffset, PMReal yOffset, UID color, PMReal opacity, PMReal radius, PMReal spread, PMReal noise)
{
	PMString command("[SE=");
	
	WriteTCLRef(command,ref);
	command += ";";
	if(mode)
		command += "TRUE;";
	else
		command += "FALSE;";
	command.AppendNumber(blendMode);
	command += ";";
	command.AppendNumber(xOffset,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(yOffset,3,kTrue,kTrue);
	command += ";";
	if(color == kInvalidUID)
		command += "5"; // Black is default color for drop shadow
	else
		command.AppendNumber(getColorRef(color, "SE", 6));
	command += ";";
	command.AppendNumber(opacity,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(radius,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(spread,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(noise,3,kTrue,kTrue);

	command += "]";
	command += kNewLine;

	WriteUTF8(command);	
	checker->checkShadowEffect(ref,mode,blendMode,xOffset,yOffset,color,opacity,radius,spread,noise);
	checker->NewLine();
}

void PrsTCLWriter::writeNewImageFrame(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, PMString fullName, PMReal xDec, PMReal yDec, PMReal xScale, PMReal yScale,
										int32 numPage, UID color, PMReal tint, PMReal angle, ClassID cornerType, PMReal cornerRadius, bool8 isInline, TCLRef refOwner)
										
{
	// Convert values to millimeters
	InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));

	leftTop.X() = uom->PointsToUnits(leftTop.X());
	leftTop.Y() = uom->PointsToUnits(leftTop.Y());
	width = uom->PointsToUnits(width);
	height = uom->PointsToUnits(height);
	xDec = uom->PointsToUnits(xDec);
	yDec = uom->PointsToUnits(yDec);
	cornerRadius = uom->PointsToUnits(cornerRadius);

	PMString command("[BI=");
	
	WriteTCLRef(command,ref);
	command += ";N";
	command .AppendNumber(leftTop.X(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(leftTop.Y(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(width,3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(height,3,kTrue,kTrue);
	command += ";";

	command += "\"" + fullName + "\"";
	
	command += ";N";
	command .AppendNumber(xDec,3,kTrue,kTrue);
	command += ";N";
	command .AppendNumber(yDec,3,kTrue,kTrue);
	command += ";";
	command .AppendNumber(xScale,3,kTrue,kTrue);
	command += ";";
	command .AppendNumber(yScale,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(getColorRef(color, "BI", 11));
	command += ";";
	command.AppendNumber(tint,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(angle,0,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(cornerRadius,3,kTrue,kTrue);
	command += ";";
	
	if(cornerType == kRoundedCornerBoss)
		command += "1;";
	else if(cornerType == kInverseRoundedCornerBoss)
		command += "2;";
	else if(cornerType == kBevelCornerBoss)
		command += "3;";
	else if(cornerType == kInsetCornerBoss)
		command += "4;";
	else if(cornerType == kFancyCornerBoss)
		command += "5;";
	else
		command += "0;";
	
	if(isInline)
		command += "TRUE;";		
	else
		command += "FALSE;";

	WriteTCLRef(command,refOwner);
	command += ";";
	command += "FALSE;";
	command.AppendNumber(numPage);
		
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkNewImageFrame(ref, leftTop, width, height, fullName, xDec, yDec, xScale, yScale, 
								numPage, color, tint, angle, cornerType, cornerRadius, isInline, refOwner);
	checker->NewLine();
}

void PrsTCLWriter::writeNewGraphicSpline(TCLRef ref, PMPoint leftTop, PMReal width, PMReal height, K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList,PMString fullName, PMReal xDec, PMReal yDec, PMReal xScale, PMReal yScale,
										int32 numPage, UID color, PMReal tint, PMReal angle, ClassID cornerType, PMReal cornerRadius, bool8 isInline, TCLRef refOwner)
{
	int32 numParam = 0;
	// Convert values to millimeters
	InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));

	leftTop.X() = uom->PointsToUnits(leftTop.X());
	leftTop.Y() = uom->PointsToUnits(leftTop.Y());
	width = uom->PointsToUnits(width);
	height = uom->PointsToUnits(height);
	xDec = uom->PointsToUnits(xDec);
	yDec = uom->PointsToUnits(yDec);
	cornerRadius = uom->PointsToUnits(cornerRadius);

	PMString command("[B!=");
	
	WriteTCLRef(command,ref);
	command += ";N";
	command.AppendNumber(leftTop.X(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(leftTop.Y(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(width,3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(height,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(ptsList.size());
	numParam = 6;
	for(int32 i = 0 ; i < ptsList.size() ; ++i)
	{
		command += ";";
		command.AppendNumber(ptsList[i].Key().size());
		command += ";";
		if(ptsList[i].Value())
			command += "TRUE";
		else
			command += "FALSE";
		numParam += 2;
		for(int32 j = 0 ; j < ptsList[i].Key().size() ; ++j)
		{
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetLeftDirPoint().X()),3,kTrue,kTrue);
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetLeftDirPoint().Y()),3,kTrue,kTrue);
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetAnchorPoint().X()),3,kTrue,kTrue);
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetAnchorPoint().Y()),3,kTrue,kTrue);
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetRightDirPoint().X()),3,kTrue,kTrue);
			command += ";N";
			command.AppendNumber(uom->PointsToUnits(ptsList[i].Key()[j].GetRightDirPoint().Y()),3,kTrue,kTrue);
			numParam += 6;
		}		
	}

	command += ";\"" + fullName + "\"";
	
	command += ";N";
	command .AppendNumber(xDec,3,kTrue,kTrue);
	command += ";N";
	command .AppendNumber(yDec,3,kTrue,kTrue);
	command += ";";
	command .AppendNumber(xScale,3,kTrue,kTrue);
	command += ";";
	command .AppendNumber(yScale,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(getColorRef(color, "B!", numParam + 6));
	command += ";";
	command.AppendNumber(tint,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(angle,0,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(cornerRadius,3,kTrue,kTrue);
	command += ";";
	
	if(cornerType == kRoundedCornerBoss)
		command += "1;";
	else if(cornerType == kInverseRoundedCornerBoss)
		command += "2;";
	else if(cornerType == kBevelCornerBoss)
		command += "3;";
	else if(cornerType == kInsetCornerBoss)
		command += "4;";
	else if(cornerType == kFancyCornerBoss)
		command += "5;";
	else
		command += "0;";
	
	if(isInline)
		command += "TRUE;";		
	else
		command += "FALSE;";

	WriteTCLRef(command,refOwner);
	command += ";FALSE;";
	command.AppendNumber(numPage);
		
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkNewGraphicSpline(ref,leftTop,width,height,ptsList,fullName,xDec,yDec,xScale,yScale,
									numPage,color,tint,angle,cornerType,cornerRadius,isInline,refOwner);
	checker->NewLine();
}

void PrsTCLWriter::writeNewRule(TCLRef ref, PMPoint p1, PMPoint p2, PMReal stroke, int32 strokeAlign, ClassID type, UID color, PMReal tint, int32 numPage, ClassID lineEndStart, ClassID lineEndEnd)
{
	// Convert values to millimeters
	InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));
	
	p1.X() = uom->PointsToUnits(p1.X());
	p1.Y() = uom->PointsToUnits(p1.Y());
	p2.X() = uom->PointsToUnits(p2.X());
	p2.Y() = uom->PointsToUnits(p2.Y());
	stroke = uom->PointsToUnits(stroke);

	PMString command("[FD=");

	WriteTCLRef(command,ref);
	command += ";N";
	command .AppendNumber(p1.X(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(p1.Y(),3,kTrue,kTrue);
	command += ";N";
	command .AppendNumber(p2.X(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(p2.Y(),3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(stroke,3,kTrue,kTrue);
	command += ";";
	if(type == kSolidPathStrokerBoss)
		command += "0;";
	else if(type == kDashedPathStrokerBoss)
		command += "1;";
	else if(type == kThinThinPathStrokerBoss)
		command += "2;";
	else if(type == kThickThickPathStrokerBoss)
		command += "3;";
	else if(type == kDashedPathStrokerBoss)
		command += "4;";
	else if(type == kCannedDotPathStrokerBoss)
		command += "5;";
	else if(type == kThinThickPathStrokerBoss)
		command += "6;";
	else if(type == kThickThinPathStrokerBoss)
		command += "7;";
	else if(type == kThinThickThinPathStrokerBoss)
		command += "8;";
	else if(type == kThickThinThickPathStrokerBoss)
		command += "9;";
	else if(type == kTriplePathStrokerBoss)
		command += "10;";
	else if(type == kCannedDash3x2PathStrokerBoss)
		command += "11;";
	else if(type == kCannedDash4x4PathStrokerBoss)
		command += "12;";
	else if(type == kLeftSlantHashPathStrokerBoss)
		command += "13;";
	else if(type == kRightSlantHashPathStrokerBoss)
		command += "14;";
	else if(type == kStraightHashPathStrokerBoss)
		command += "15;";
	else if(type == kSingleWavyPathStrokerBoss)
		command += "16;";
	else if(type == kWhiteDiamondPathStrokerBoss)
		command += "17;";
	else if(type == kJapaneseDotsPathStrokerBoss)
		command += "18;";
	else
		command += "0;";

	command.AppendNumber(getColorRef(color, "FD", 8));
	command += ";";
	command.AppendNumber(tint,3,kTrue,kTrue);
	command += ";";

	command += "FALSE;";
	command.AppendNumber(numPage);
	command += ";0;";
	if(lineEndStart == kInvalidClass)
	{
		if(lineEndEnd == kCurvedArrowHeadBoss)
			command += "1";
		else 
			command += "0";
	}
	else if(lineEndStart == kCurvedArrowHeadBoss)
	{
		if(lineEndEnd == kInvalidClass)
			command += "2";
		else if(lineEndEnd == kCurvedArrowHeadBoss)
			command += "5";
		else if(lineEndEnd == kBarArrowHeadBoss)
			command += "4";
		else
			command += "0";
	}
	else if(lineEndStart == kBarArrowHeadBoss && lineEndEnd == kCurvedArrowHeadBoss)
		command += "3";
	else
		command += "0";

	command += ";;";
	command.AppendNumber(strokeAlign);
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkNewRule(ref,p1,p2,stroke,strokeAlign,type,color,tint,numPage,lineEndStart,lineEndEnd);
	checker->NewLine();
}

void PrsTCLWriter::writeCreateGroup(K2Vector<TCLRef> itemRefList)
{
	PMString command("[BG=TRUE");
	
	for(int32 i = 0 ; i < itemRefList.size() ; ++i)
	{
		command += ";";
		WriteTCLRef(command,itemRefList[i]);
	}
	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::writeCreateInlineGroup(TCLRef refOwner, K2Vector<TCLRef> itemRefList)
{
	PMString command("[AG=");

	WriteTCLRef(command,refOwner);
	command += ";FALSE;TRUE";
	for(int32 i = 0 ; i < itemRefList.size() ; ++i)
	{
		command += ";"; 
		WriteTCLRef(command, itemRefList[i]);
	}
	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::setCurrentParaStyle(UID currentParaStyle)
{
	if(this->currentParaStyle != nil)
		this->currentParaStyle->Release();

	this->currentParaStyle = (ITextAttributes*) curDB->Instantiate(currentParaStyle, IID_ITEXTATTRIBUTES);
}

void PrsTCLWriter::setCurrentCharStyle(UID currentCharStyle)
{
	if(this->currentCharStyle != nil)
		this->currentCharStyle->Release();
	
	this->currentCharStyle = (ITextAttributes*) curDB->Instantiate(currentCharStyle, IID_ITEXTATTRIBUTES);
}

void PrsTCLWriter::writeApplyParaStyle(PMString paraStyleName)
{	
	PMString command("[SN=\"" + paraStyleName + "\"]");
	WriteUTF8(command);
}

void PrsTCLWriter::writeApplyCharStyle(PMString charStyleName)
{
	PMString command("[SB=\"" + charStyleName + "\"]");
	WriteUTF8(command);
}


void PrsTCLWriter::writeChar(textchar c, bool16& inlineFound, bool16& zeroBreakFound)
{
	switch(c)
	{
		case kTextChar_CR : 
			{
				if(breakCharacter == Text::kAtColumn)
				{
					text.Append(WideString("[TF]"));
					breakCharacter = Text::kNoForcedBreak;
				}
				else if(breakCharacter == Text::kAtFrameBox)
				{
					text.Append(WideString("[NB]"));
					breakCharacter = Text::kNoForcedBreak;
				}
				else
					text.Append(WideString("[PF]"));
			}
			text.Append(WideString(kNewLine));
			checker->NewLine();
			break;
		case kTextChar_LF : text.Append(WideString("[LF]")); break;
		case kTextChar_EmSpace : text.Append(WideString("[EC]")); break;
		case kTextChar_EnSpace : text.Append(WideString("[ED]")); break;
		case kTextChar_ThinSpace : text.Append(WideString("[EF]")); break;
		case kTextChar_FigureSpace : text.Append(WideString("[EP]")); break;
		case kTextChar_IndentToHere : text.Append(WideString("[II]")); break;
		case kTextChar_RightAlignedTab : text.Append(WideString("[TE]")); break;
		case kTextChar_PageNumber : text.Append(WideString("[AN]")); break;
		case kTextChar_ObjectReplacementCharacter : inlineFound = kTrue; Flush(); break;
		case kTextChar_ZeroSpaceNoBreak :  zeroBreakFound = kTrue; Flush(); break;
		
		// GD 27.11.08 ++
		case kTextChar_LeftSquareBracket : text.Append(WideString("[CC=91]")); break;
		case kTextChar_RightSquareBracket : text.Append(WideString("[CC=93]")); break;
		// GD 27.11.08 --

		default : 			
			text.Append(c); 
			checker->checkChar(c);
			break;
	}
}

void PrsTCLWriter::writeSetInlineProperties(TCLRef ref, IAnchoredObjectData::AnchoredPosition pos, PMReal offset)
{
	PMString command("[BQ=");

	WriteTCLRef(command,ref);
	command += ";";
		
	if(pos == IAnchoredObjectData::kAboveLine)
		command += "TRUE;D";
	else
		command += "FALSE;D";

	command.AppendNumber(offset,3,kTrue,kTrue);
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);		
	checker->checkSetInlineProperties(ref,pos,offset);
	checker->NewLine();
}

void PrsTCLWriter::writeSelectItem(TCLRef ref, bool16 selectTOP)
{
	PMString command("[BS=");

	WriteTCLRef(command,ref);

	if(selectTOP)	
		command += ";TRUE";

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);	
	checker->NewLine();
}

void PrsTCLWriter::EndText()
{
	if(text.NumUTF16TextChars() > 0)
	{
		// Remove last char kTextChar_CR (added automatically by InDesign when story will be created again)
		PMString uselessLastPF("[PF]");
		int32 pos = text.NumUTF16TextChars() - (uselessLastPF.NumUTF16TextChars() + PMString(kNewLine).NumUTF16TextChars());
		text.Remove(pos, uselessLastPF.NumUTF16TextChars());
	}
	Flush();

	// Reset current ovverides list and values
	currentOverrides->ClearAllOverrides();
	this->ResetValues();
}

void PrsTCLWriter::Flush()
{
	if(!text.empty())
	{			
		//CAlert::InformationAlert(text);
	
		std::string toWrite;
		StringUtils::ConvertWideStringToUTF8(text, toWrite);
		TCLStream->XferByte((uchar *) toWrite.c_str(), toWrite.length());
		text.Clear();
	}
}

void PrsTCLWriter::writeNewMaster(PMString masterName, PMString basedOn, int32 nbPages)
{
	PMString command("[MD=");

	command += "\"" + masterName + "\";";
	command += "\"" + basedOn + "\";";
	command.AppendNumber(nbPages);
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);	
	checker->checkNewMaster(masterName,basedOn,nbPages);
	checker->NewLine();
}

void PrsTCLWriter::writeEndMaster()
{
	PMString command("[MF]");
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);	
	checker->NewLine();
}

void PrsTCLWriter::writeClippingPath(TCLRef ref, int16 clipType, PMReal tolerance, PMReal inset, uint8 threshold, int16 index, bool16 invert, bool16 allowHoles, bool16 limitToFrame, bool16 useHighRes)
{
	PMString command("[BZ=");

	WriteTCLRef(command,ref);
	command += ";";
	command.AppendNumber(clipType);
	command += ";D";
	command.AppendNumber(inset,3,kTrue,kTrue);
	command += ";;;;";
	if(invert)
		command += "TRUE;";
	else
		command += "FALSE;";
	if(allowHoles)
		command += "TRUE;";
	else
		command += "FALSE;";
	command.AppendNumber(index);
	command += ";";
	if(limitToFrame)
		command += "TRUE;";
	else
		command += "FALSE;";
	if(useHighRes)
		command += "TRUE;";
	else
		command += "FALSE;";
	command.AppendNumber(threshold);
	command += ";";
	command.AppendNumber(tolerance,3,kTrue,kTrue);

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);		
	checker->checkClippingPath(ref,clipType,tolerance,inset,threshold,index,invert,allowHoles,limitToFrame,useHighRes);
	checker->NewLine();
}

void PrsTCLWriter::writeTransparencyEffect(TCLRef ref, int8 blendMode, PMReal opacity, bool16 knockoutGroup, bool16 isolationGroup)
{
	PMString command("[OE=");

	WriteTCLRef(command,ref);
	command += ";";
	command.AppendNumber(blendMode);
	command += ";";
	command.AppendNumber(opacity,3,kTrue,kTrue);
	command += ";";
	if(knockoutGroup)
		command += "TRUE;";
	else
		command += "FALSE;";
	if(isolationGroup)
		command += "TRUE";
	else
		command += "FALSE";

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);		
	checker->checkTransparencyEffect(ref,blendMode,opacity,knockoutGroup,isolationGroup);
	checker->NewLine();
}

void PrsTCLWriter::writeFeatherEffect(TCLRef ref, int8 mode, PMReal width, int8 corner, PMReal noise, PMReal innerOpacity, PMReal outerOpacity)
{
	PMString command("[FA=");

	WriteTCLRef(command,ref);
	command += ";";
	command.AppendNumber(mode);
	command += ";D";
	command.AppendNumber(width,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(corner);
	command += ";";
	command.AppendNumber(noise,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(innerOpacity,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(outerOpacity,3,kTrue,kTrue);

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);		
	checker->checkFeatherEffect(ref,mode,width,corner,noise,innerOpacity,outerOpacity);
	checker->NewLine();
}

void PrsTCLWriter::writeNewGradient(int32 colorID, PMString colorName, int8 type, K2Vector<int32> refColors, K2Vector<PMReal> stopPositions, K2Vector<PMReal> midPoints)
{
	PMString command("[GN=");

	command.AppendNumber(colorID);
	command += ";";
	command += "\"" + colorName + "\";";
	command.AppendNumber(type);
	command += ";";
	command.AppendNumber(refColors.size());
	
	for(int32 i = 0 ; i < refColors.size() ; ++i)
	{
		command += ";";
		command.AppendNumber(refColors[i]);
		command += ";";
		command.AppendNumber(stopPositions[i],3,kTrue,kTrue);
		command += ";";
		command.AppendNumber(midPoints[i],3,kTrue,kTrue);
	}

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);	
	checker->checkNewGradient(colorID, colorName, type, refColors, stopPositions, midPoints);
	checker->NewLine();
}

void PrsTCLWriter::writeGradientOptions(TCLRef ref, bool16 isFillAttr, PMReal angle, PMReal length, PMPoint center, PMReal radius)
{
	PMString command("[GO=");

	WriteTCLRef(command,ref);
	command += ";";
	if(isFillAttr)
		command += "TRUE;";
	else
		command += "FALSE;";

	command.AppendNumber(angle,0,kTrue,kTrue);
	command += ";D";
	command.AppendNumber(length, 3, kTrue, kTrue);
	command += ";D";
	command.AppendNumber(center.X(), 3, kTrue, kTrue);
	command += ";D";
	command.AppendNumber(center.Y(), 3, kTrue, kTrue);
	command += ";D";
	command.AppendNumber(radius, 3, kTrue, kTrue);

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);		
	checker->checkGradientOptions(ref, isFillAttr,angle,length,center,radius);
	checker->NewLine();
}

void PrsTCLWriter::writeNewTintedColor(int32 colorID, int32 basedOn, PMReal tint)
{
	PMString command("[C2=");

	command.AppendNumber(colorID);
	command += ";";
	command.AppendNumber(basedOn);
	command += ";";
	command.AppendNumber(tint,0,kTrue,kTrue);

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);		
	checker->checkNewTintedColor(colorID,basedOn,tint);
	checker->NewLine();
}

void PrsTCLWriter::writeSection(int32 startPage, int32 folio, PMString prefix, int8 type, bool16 autoNumber, bool16 includeSectionPrefix, PMString marker)
{
	PMString command("[DD=");

	command.AppendNumber(startPage);
	command += ";";
	command.AppendNumber(folio);
	command += ";";
	command += "\"" + prefix + "\";";
	command.AppendNumber(type);
	command += ";";
	if(autoNumber)
		command += "TRUE;";
	else
		command += "FALSE;";

	if(includeSectionPrefix)
		command += "TRUE;";
	else
		command += "FALSE";
	
#if XRAIL
	command += ";\"" + marker + "\"";
#endif

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::writeTOP(TCLRef ref, PMReal startPos, PMReal endPos, const TextAlignType textAlign, const PathAlignType pathAlign, const EffectType typeEffect, const int16 overlapOffset, const bool16 flip)
{
	PMString command("[T*=");

	WriteTCLRef(command,ref);
	command += ";D";
	command.AppendNumber(startPos,3,kTrue,kTrue);
	command += ";D";
	command.AppendNumber(endPos,3,kTrue,kTrue);	
	command += ";";
	command.AppendNumber(textAlign);
	command += ";";
	command.AppendNumber(pathAlign);
	command += ";";
	command.AppendNumber(typeEffect);
	command += ";";
	command.AppendNumber(overlapOffset);
	if(flip)
		command += ";TRUE";
	else
		command += ";FALSE";

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::writeTOC(TCLRef ref, PMString tocStyle)
{
	PMString command("[MT=");

	WriteTCLRef(command,ref);
	command += ";\"" + tocStyle + "\"]" + kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkTOC(ref, tocStyle);
	checker->NewLine();
}

void PrsTCLWriter::writeTOCStyle(PMString tocStyleName, PMString tocTitle, int32 tocTitleStyle,
								 bool16 includeHidLayer, bool16 runIn, bool16 includeBookMarks, 
								 K2Vector<KeyValuePair<int32, FormatEntryParam> >& entries)
{
	PMString command("[ST=");

	command += "\"" + tocStyleName + "\";\"" + tocTitle + "\";";
	command.AppendNumber(tocTitleStyle);
	if(includeHidLayer)
		command += ";TRUE";
	else
		command += ";FALSE";
	if(runIn)
		command += ";TRUE";
	else
		command += ";FALSE";
	if(includeBookMarks)
		command += ";TRUE";
	else
		command += ";FALSE";
	
	for(int32 i = 0 ; i < entries.size() ; ++i)
	{
		int32 paraStyleEntry = entries[i].Key();
		FormatEntryParam entryParam = entries[i].Value();

		command +=";";
		command.AppendNumber(paraStyleEntry);
		command +=";";
		command.AppendNumber(entryParam.formatEntryStyle);
		command +=";";
		command.AppendNumber(entryParam.formatPageStyle);
		command +=";";
		command.AppendNumber(entryParam.level);
		if(entryParam.levelSort)
			command += ";TRUE;";
		else
			command += ";FALSE;";
		command.AppendNumber(entryParam.pageNumPos);
		command += ";\"" + entryParam.separator + "\";";
		command.AppendNumber(entryParam.formatSeparatorStyle);
	}

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkTOCStyle(tocStyleName,tocTitle,tocTitleStyle,includeHidLayer, runIn, includeBookMarks, entries);
	checker->NewLine();
}

void PrsTCLWriter::writeTiffOptions(TCLRef ref, UID color, PMReal tint)
{
	PMString command("[BO=");

	WriteTCLRef(command,ref);
	command += ";";
	command.AppendNumber(getColorRef(color, "BO", 2));
	command += ";";
	command.AppendNumber(tint);
	
	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::WriteUTF8(const PMString& command)
{
	WideString commandW;
	commandW.BuildFromSystemString(command);
	std::string utf8Command;
	StringUtils::ConvertWideStringToUTF8(commandW, utf8Command);
	TCLStream->XferByte((uchar *) utf8Command.c_str(), utf8Command.length());
}

void PrsTCLWriter::writeHyperlinkTextDest(PMString hlName, bool16 visible)
{
	PMString command("[H1=");

	command += "\"" + hlName + "\";";
	if(visible)
		command += "TRUE]";
	else
		command += "FALSE]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);	
	checker->checkHyperlinkTextDest(hlName,visible);
	checker->NewLine();
}

void PrsTCLWriter::writeHyperlinkSource(int8 type, PMString hlName, PMString hlDestName, int32 color, int32 hilightType, int32 outlineStyle, 
										  int32 borderWidth, bool16 visible, TextIndex start, TextIndex end)
{
	PMString command("[HK=");

	command.AppendNumber(type);
	command += ";\"" + hlName + "\";\"" + hlDestName + "\";";
	command.AppendNumber(color);
	command += ";";
	command.AppendNumber(hilightType);
	command += ";";
	command.AppendNumber(outlineStyle);
	command += ";";
	command.AppendNumber(borderWidth);
	command += ";";
	if(visible)
		command += "TRUE";
	else
		command += "FALSE";
	if(start != kInvalidTextIndex && end != kInvalidTextIndex)
	{
		command += ";";
		command.AppendNumber(start);
		command += ";";
		command.AppendNumber(end);
	}

	command += "]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkHyperlinkSource(type,hlName, hlDestName,color,hilightType,outlineStyle, borderWidth,visible, start, end);
	checker->NewLine();
}

void PrsTCLWriter::writeHyperlinkPageDest(PMString hlName, int32 pageNumber, bool16 visible)
{
	PMString command("[H2=");

	command += "\"" + hlName + "\";";
	command.AppendNumber(pageNumber);
	if(visible)
		command += ";TRUE]";
	else
		command += ";FALSE]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);	
	checker->checkHyperlinkPageDest(hlName,pageNumber,visible);
	checker->NewLine();
}

void PrsTCLWriter::writeHyperlinkURLDest(PMString hlName, PMString url)
{
	PMString command("[H3=");

	command += "\"" + hlName + "\";\"" + url + "\"]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);	
	checker->checkHyperlinkURLDest(hlName, url);
	checker->NewLine();
}

void PrsTCLWriter::writeBookmark(PMString bkName, PMString bkParent, PMString hlDestName)
{
	PMString command("[SI=");

	command += "2;\"" + bkName + "\";\"" + bkParent + "\";\"" + hlDestName + "\"]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);
	checker->checkBookmark(bkName, bkParent, hlDestName);
	checker->NewLine();	
}

void PrsTCLWriter::writeImageInfo(TCLRef ref, PMReal imageWidth, PMReal imageHeight)
{
	// Convert values to millimeters
	InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));
	imageWidth = uom->PointsToUnits(imageWidth);
	imageHeight = uom->PointsToUnits(imageHeight);

	PMString command("[AI=");

	WriteTCLRef(command,ref);
	command += ";N";
	command .AppendNumber(imageWidth,3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(imageHeight,3,kTrue,kTrue);
	command +="]";
	command += kNewLine;
	command.SetTranslatable(kFalse);
	
	WriteUTF8(command);
	checker->NewLine();
}

void PrsTCLWriter::writeImageProperties(TCLRef ref, PMReal imageAngle, PMReal imageSkew, PMReal xOffset, PMReal yOffset)
{
	// Convert values to millimeters
	InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));
	xOffset = uom->PointsToUnits(xOffset);
	yOffset = uom->PointsToUnits(yOffset);

	PMString command("[BP=");

	WriteTCLRef(command,ref);
	command += ";";
	command.AppendNumber(imageAngle,3,kTrue,kTrue);
	command += ";";
	command.AppendNumber(imageSkew,3,kTrue,kTrue);
	command += ";N";
	command .AppendNumber(xOffset,3,kTrue,kTrue);
	command += ";N";
	command.AppendNumber(yOffset,3,kTrue,kTrue);
	command +="]";
	command += kNewLine;
	command.SetTranslatable(kFalse);

	WriteUTF8(command);
	checker->NewLine();
}
