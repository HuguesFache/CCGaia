/*
//	File:	PrsReverseTCL.cpp
//
//	Author:	Trias
//
//	Date:	19/09/2005
//
*/

#include "VCPlugInHeaders.h"								// For MSVC
#include "GlobalDefs.h"

// Project includes
#include "ITCLReferencesList.h"
#include "StreamUtil.h"
#include "TransformUtils.h"
#include "IPageItemTypeUtils.h"
#include "TextID.h"
#include "ErrorUtils.h"
//#include "K2Vector.h"
//#include "K2Vector.tpp"
//#include "K2VectorBase.h"
#include "KeyValuePair.h"
#include "PreferenceUtils.h"
#include "TCLError.h"
#include "IPageMakeUpPrefs.h"
#include "FileUtils.h"
#include "IUIColorUtils.h"
#include "StringUtils.h"
#include "ILinkObject.h"
#include "ILinkManager.h"
#include "URI.h"
#include <math.h>

// Interface headers
#include "IDocument.h"
#include "IPageList.h"
#include "IMargins.h"
#include "IColumns.h"
#include "IStyleInfo.h"
#include "ITextAttributes.h"
#include "IStyleGroupManager.h"
#include "IStyleGroupHierarchy.h"
#include "ISwatchList.h"
#include "ISwatchUtils.h"
#include "IColorData.h"
#include "IReverseTCL.h"										// Superclass declaration
#include "ITCLWriter.h"
#include "ILayerList.h"
#include "IDocumentLayer.h"
#include "IColorSystemUtils.h"
#include "ISpreadList.h"
#include "ISpread.h"
#include "ISpreadLayer.h"
#include "IHierarchy.h"
#include "IGeometry.h"
#include "IMultiColumnTextFrame.h"
#include "ITextFrameColumn.h"
#include "ITextInset.h"
#include "ITextColumnSizer.h"
#include "ITransform.h"
#include "IStandOffData.h"
#include "IGraphicStyleDescriptor.h"
#include "IGraphicAttributeUtils.h"
#include "IDataLinkReference.h"
#include "IDataLink.h"
#include "IAttributeStrand.h"
#include "ITextStrand.h"
#include "ITextModel.h"
#include "IItemStrand.h"
#include "IFrameUtils.h"
#include "ILayoutUtils.h"
#include "IFrameList.h"
#include "IPathUtils.h"
#include "IPathGeometry.h"
#include "IAnchoredObjectData.h"
#include "IMasterPage.h"
#include "IMasterSpread.h"
#include "IMasterSpreadList.h"
#include "IMasterSpreadUtils.h"
#include "ICompositionStyle.h"
#include "ITextAttrRealNumber.h"
#include "ITextAttrUID.h"
#include "ILanguage.h"
#include "ILanguageList.h"
#include "IGuidePrefs.h"
#include "IStandOffContourWrapSettings.h"
#include "IGraphicFrameData.h"
#include "IXPAttributeUtils.h"
#include "GraphicTypes.h"
#include "ILockPosition.h"
#include "IAttrReport.h"
#include "IClipSettings.h"
#include "IGradientFill.h"
#include "IColorOverrides.h"
#include "ISection.h"
#include "ISectionList.h"
#include "IMainItemTOPData.h"
#include "ITOPOptionsCmdData.h"
#include "IPersistUIDData.h"
#include "ITOCStyleNameTable.h"
#include "ITOCStyleInfo.h"
#include "TOCEntryInfo.h"
#include "IHyperlinkDestination.h"
#include "IHyperlinkMarker.h"
#include "IHyperlinkSource.h"
#include "IHyperlinkAppearanceData.h"
#include "IHyperlinkPageItemSourceData.h"
#include "IHyperlinkTextSourceData.h"
#include "IHyperlinkTable.h"
#include "IOwnedItem.h"
#include "IHyperlinkPageDestinationData.h"
#include "IHyperlink.h"
#include "IBookMarkData.h"
#include "IStringData.h"
#include "IScriptUtils.h"
#include "IScriptRunner.h"
#include "IScriptManager.h"
#include "IPageItemAdornmentList.h"
#include "IClassIDData.h"
#include "AppleScriptID.h"
#include "IURIUtils.h"
#include "ITransformFacade.h"
#include "ITextRangeNode.h"

#if !SERVER
	#include "ILayoutUIUtils.h"
#else
	#include "IApplication.h"
	#include "IDocumentList.h"
#endif

#include "DebugClassUtils.h"

#include "CAlert.h"

class PrsReverseTCL : public CPMUnknown<IReverseTCL>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsReverseTCL(IPMUnknown* iBoss);
	
	/** Destructor. */
	virtual	~PrsReverseTCL(void);

	virtual bool8 SetOutputFile(IDFile file);

	virtual ErrorCode StartReverse();

	virtual void setExportHeaderPrefs(bool16 exportStyles, bool16 exportCJ, bool16 exportColors,
										bool16 exportLayers, bool16 exportMasters);

	virtual void setExportGeometryPrefs(bool16 exportTextFrame, bool16 exportImageFrame, bool16 exportRule);

	virtual void setExportTextPrefs(bool16 exportText, bool16 exportParaCmd, bool16 exportTextCmd, bool16 exportStyleCmd);
	
private :

	/** helper methods */
	void DisplayReverseCheckerLog();
	
	void SaveItemList();

	void CloseFile();

	void GetFrameGeneralAttributes(IHierarchy * itemHier, PMReal& rotationAngle, PMReal& xSkewAngle, PMReal& xScale, PMReal& yScale, PMPoint& leftTopWithoutTransformation, 
								   PMReal& width, PMReal& height, ClassID& lineImpl, ClassID& cornerImpl, UID& strokeUID, UID& fillUID, 
								   PMReal& strokeTint, PMReal& fillTint, PMReal& strokeWeight, int32& strokeAlign, PMReal& cornerRadius, UID& gapUID, PMReal& gapTint, bool16& nonPrint, bool16& locked, PMReal& gradientFillAngle, PMReal& gradientStrokeAngle,
								   PMReal& gradientFillLength, PMReal& gradientFillRadius, PMPoint& gradientFillCenter, PMReal& gradientStrokeLength, PMReal& gradientStrokeRadius, PMPoint& gradientStrokeCenter,  
								   int8& BSblendMode, PMReal& BSopacity, bool16& BSknockoutGroup, bool16& BSisolationGroup, int8& VTmode, PMReal& VTwidth, int8& VTcorner, PMReal& VTnoise, PMReal& VTinnerOpacity, PMReal& VTouterOpacity,
								   bool16& DSmode, int8& DSblendMode, PMReal& DSxOffset, PMReal& DSyOffset, UID& DScolor, PMReal& DSopacity, PMReal& DSradius, PMReal& DSspread, PMReal& DSnoise,IGeometry * curPageGeo);

	void GetSplineAttributes(IPathGeometry * itemPathGeo, K2Vector<KeyValuePair<PMPathPointList, bool16> >& ptsList, PMReal& rotationAngle, PMReal& xSkewAngle, PMReal& xScale, PMReal& yScale, PMPoint& leftTopWithoutTransformation, PMReal& width, PMReal& height, 
							 ClassID& lineImpl, ClassID& cornerImpl, UID& strokeUID, UID& fillUID, PMReal& strokeTint, PMReal& fillTint, PMReal& strokeWeight, int32& strokeAlign, PMReal& cornerRadius, UID& gapUID, PMReal& gapTint, bool16& nonPrint, bool16& locked, 
							 int8& BSblendMode, PMReal& BSopacity, bool16& BSknockoutGroup, bool16& BSisolationGroup, int8& VTmode, PMReal& VTwidth, int8& VTcorner, PMReal& VTnoise, PMReal& VTinnerOpacity, PMReal& VTouterOpacity, 
							 PMReal& gradientFillAngle, PMReal& gradientStrokeAngle, PMReal& gradientFillLength, PMReal& gradientFillRadius, PMPoint& gradientFillCenter, PMReal& gradientStrokeLength, PMReal& gradientStrokeRadius, PMPoint& gradientStrokeCenter,  
							 bool16& DSmode, int8& DSblendMode, PMReal& DSxOffset, PMReal& DSyOffset, UID& DScolor, PMReal& DSopacity, PMReal& DSradius, PMReal& DSspread, PMReal& DSnoise,IGeometry * curPageGeo);

	void GetImageFrameProperties(TCLRef ref, IHierarchy * itemHier, PMString& fileName, PMReal& xScale, PMReal& yScale, PMReal& xDec, PMReal& yDec,
								 int16& clipType, PMReal& tolerance, PMReal& inset, uint8& threshold, int16& index, bool16& invert, 
								 bool16& allowHoles, bool16& limitToFrame, bool16& useHighRes, UID& tiffFillUID, PMReal& tiffFillTint, 
								 PMReal& imageWidth, PMReal& imageHeight, PMReal& imageAngle, PMReal& imageSkew);

	void GetTextFrameProperties(IHierarchy * multiColHier, Text::VerticalJustification& vj, Text::FirstLineOffsetMetric& topAlign,
								PMReal& vjMax, PMReal& offset, PMReal& gutter, int32& nbCol,
								PMReal& leftInset, PMReal& rightInset , PMReal& topInset, PMReal& bottomInset);

	void GetTextWrapAttributes(IHierarchy * itemGeo, IStandOff::mode& mode, IStandOff::form& form, PMReal& topStOff,PMReal& rightStOff,
							   PMReal& leftStOff, PMReal& bottomStOff, IStandOffContourWrapSettings::ContourWrapType& contourType);

	void GetGraphicAttributes(IHierarchy * itemHier, ClassID& lineImpl, ClassID& cornerImpl, UID& strokeUID, UID& fillUID, PMReal& strokeTint, 
							  PMReal& fillTint, PMReal& strokeWeight, int32& strokeAlign, PMReal& cornerRadius,  UID& gapUID, PMReal& gapTint, bool16& nonPrint, PMReal& gradientFillAngle, PMReal& gradientStrokeAngle,
							  PMReal& gradientFillLength, PMReal& gradientFillRadius, PMPoint& gradientFillCenter, PMReal& gradientStrokeLength, PMReal& gradientStrokeRadius, PMPoint& gradientStrokeCenter,  
							  int8& BSblendMode, PMReal& BSopacity, bool16& BSknockoutGroup, bool16& BSisolationGroup, int8& VTmode, PMReal& VTwidth, int8& VTcorner, PMReal& VTnoise, PMReal& VTinnerOpacity, PMReal& VTouterOpacity,
							  bool16& DSmode, int8& DSblendMode, PMReal& DSxOffset, PMReal& DSyOffset, UID& DScolor, PMReal& DSopacity, PMReal& DSradius, PMReal& DSspread, PMReal& DSnoise);

	PMPoint RemoveTransformations(PMPoint toTransform, PMPoint center, PMReal rotationAngle, PMReal xSkewAngle, PMReal yScale, IGeometry * itemGeo);

	void ReverseTextFrameContent(IHierarchy * multiColHier, TCLRef ref);

	TCLRef ReversePageItem(UIDRef pageItem, IGeometry * curPageGeo, int32 curPage, bool8 isInline = kFalse, TCLRef refOwner = kInvalidTCLRef);

	void ReverseLinkedTextFrames(IHierarchy * itemHier, TCLRef ref, bool8 toLink, TCLRef refLink);

	void GetRuleAttributes(IPathGeometry * rulePathGeo, PMPoint& p1, PMPoint& p2, ClassID& lineImpl, PMReal& stroke, int32& strokeAlign, UID& color, 
						   PMReal& tint, ClassID& lineEndStartImpl, ClassID& lineEndEndImpl, bool16& nonPrint, IGeometry * curPageGeo);

	void ReverseInline(UID inlineUID, TCLRef refOwner);

	void ReverseHyperlinkDest(IItemStrand * itemStrand, TextIndex index);
	void ReverseAllHyperlinkSource(IDocument * doc);

	void SortStyleTable(IStyleGroupManager * styleTable, K2Vector<UID>& sortedList, const UID& parent);
	void ExportTOP(IHierarchy * itemHier, TCLRef ref);

	TCLRef getTCLRef(UID pageItem);

	IDataBase * db;
	InterfacePtr<IPageList> pageList;
	InterfacePtr<ISpread> masterSpreadGlobal;
	UIDList * alreadyReversedItem;
	
	ITCLWriter * writer;	
	int32 numStyle, numFrame, numRule;	
	UID * colorList;
	UID currentMaster, currentLayer;
	UID defaultCharStyle, defaultParaStyle, rootParaStyle;

	RefVector docItemList; // List of item's TCL references stored in document
	TCLRef tocFrameRef; // TCL reference of text frame containing a table of content, if any
	PMString tocStyleName;
	
	K2Vector<KeyValuePair<UID,int32> > styles;
	K2Vector<KeyValuePair<UID,PMString> > styleNames;

	bool16 exportStyles, exportCJ, exportColors, exportLayers, exportMasters;
	bool16 exportTextFrame, exportImageFrame, exportRule;
	bool16 exportText, exportParaCmd, exportTextCmd, exportStyleCmd;

	const TCLRef kSkippedTCLRef;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PrsReverseTCL, kPrsReverseTCLImpl)

/* Constructor
*/
PrsReverseTCL::PrsReverseTCL(IPMUnknown* iBoss) : CPMUnknown<IReverseTCL>(iBoss),
#if REF_AS_STRING
	kSkippedTCLRef("                            ")
#else
	kSkippedTCLRef(-2)
#endif
{
	writer = (ITCLWriter *) ::CreateObject(kTCLReadWriteBoss, IID_ITCLWRITER);
	numStyle = 1; // start at numero 1 so that no styles are references as 0 (which is default parameter for basedOn and nextStyle parameters in command [SD])
	numFrame = 32000;
	numRule = 1000;
	currentMaster = currentLayer = kInvalidUID;
	alreadyReversedItem = nil;
	db = nil;
	
	tocFrameRef = kInvalidTCLRef;
	tocStyleName = kNullString;

	exportStyles = exportCJ = exportColors = exportLayers = exportMasters = kTrue;
	exportTextFrame = exportImageFrame = exportRule = kTrue;
	exportText = exportParaCmd = exportTextCmd = exportStyleCmd = kTrue;

	writer->SetExportAttributesPrefs(exportCJ,exportParaCmd,exportTextCmd);

	colorList = new UID[MAX_COLOR + 1];

	// Fill the rest of the array with default value
	for(int i = 10; i < MAX_COLOR + 1 ; i++)
		colorList[i] = kInvalidUID;

	InterfacePtr<IPageMakeUpPrefs> storedPrefs ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
	if (storedPrefs == nil)
    {
		ASSERT_FAIL("PrsReverseTCL::PrsReverseTCL() storedPrefs invalid");
		return;
	}

	bool16 exportStyles, exportCJ, exportColors, exportLayers, exportMasters;
	bool16 exportTextFrame, exportImageFrame, exportRule;
	bool16 exportText, exportParaCmd, exportTextCmd, exportStyleCmd;

	storedPrefs->getExportHeaderPrefs(&exportStyles, &exportCJ, &exportColors, &exportLayers, &exportMasters);
	storedPrefs->getExportGeometryPrefs(&exportTextFrame, &exportImageFrame, &exportRule);					
	storedPrefs->getExportTextPrefs(&exportText, &exportParaCmd, &exportTextCmd, &exportStyleCmd);
								
	this->setExportHeaderPrefs(exportStyles, exportCJ, exportColors, exportLayers, exportMasters);
	this->setExportGeometryPrefs(exportTextFrame, exportImageFrame, exportRule);					
	this->setExportTextPrefs(exportText, exportParaCmd, exportTextCmd, exportStyleCmd);
}



/* Destructor
*/
PrsReverseTCL::~PrsReverseTCL(void)
{
	if(writer != nil)
		writer->Release();
	
	delete[] colorList;

	if(alreadyReversedItem != nil)
		delete alreadyReversedItem;
}

/* SetOutputFile
*/
bool8 PrsReverseTCL::SetOutputFile(IDFile file)
{
	InterfacePtr<IPMStream> writeStream (StreamUtil::CreateFileStreamWrite(file,kOpenOut | kOpenTrunc,kTextType));
	if(writeStream == nil)
	{
		ASSERT_FAIL("PrsReverseTCL::SetOutputFile -> writeStream nil");
		return kFalse;
	}
	
	writer->SetStream(writeStream);

	return kTrue;
}

void PrsReverseTCL::CloseFile()
{
	writer->CloseStream();
}

void PrsReverseTCL::setExportHeaderPrefs(bool16 exportStyles, bool16 exportCJ, bool16 exportColors, bool16 exportLayers, bool16 exportMasters)
{
	
	this->exportStyles = exportStyles;
	this->exportCJ = exportCJ;
	this->exportColors = exportColors;
	this->exportLayers = exportLayers;
	this->exportMasters = exportMasters;

	writer->SetExportAttributesPrefs(exportCJ,exportParaCmd,exportTextCmd);
}


void PrsReverseTCL::setExportGeometryPrefs(bool16 exportTextFrame, bool16 exportImageFrame, bool16 exportRule) 
{
	this->exportTextFrame = exportTextFrame;
	this->exportImageFrame = exportImageFrame;
	this->exportRule = exportRule;
}


void PrsReverseTCL::setExportTextPrefs(bool16 exportText, bool16 exportParaCmd, bool16 exportTextCmd, bool16 exportStyleCmd)
{
	this->exportText = exportText;
	this->exportParaCmd = exportParaCmd;
	this->exportTextCmd = exportTextCmd;
	this->exportStyleCmd = exportStyleCmd;

	writer->SetExportAttributesPrefs(exportCJ,exportParaCmd,exportTextCmd);
}

void PrsReverseTCL::SortStyleTable(IStyleGroupManager * styleTable, K2Vector<UID>& sortedList, const UID& parent)
{
	UIDList allStyles (::GetDataBase(styleTable));
	styleTable->GetRootHierarchy()->GetDescendents(&allStyles,IID_ISTYLEINFO);

	// Iterate all styles looking for the style hierarchy
	sortedList.push_back(parent);

	allStyles.Remove(allStyles.Location(parent));
	while(!allStyles.IsEmpty()){
		for(int32 currentStyle = 0 ; (currentStyle < sortedList.size()) && (!allStyles.IsEmpty()); ++currentStyle){
			for(int32 i = 0; i < allStyles.Length(); ){
				InterfacePtr<IStyleInfo> styleInfo (db, allStyles[i], UseDefaultIID());
				if(styleInfo->GetBasedOn() == sortedList[currentStyle]){
					sortedList.push_back(allStyles[i]);			
					allStyles.Remove(i);
				}
				else
					++i;
			}			
		}		
	}
}

/* StartReverse
*/
ErrorCode PrsReverseTCL::StartReverse()
{
	ErrorCode status = kFailure;
	PMString error = PMString(kErrNilInterface);
	try
	{
		/*************************/
		/*	 Document settings   */
		/*************************/
#if !SERVER
		IDocument *curDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
#else
		// In InDesign Server mode, current doc is the latest opened document, if any
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> documentList(application->QueryDocumentList());
		IDocument * curDoc = nil;
		if(documentList->GetDocCount() > 0)
			curDoc = documentList->GetNthDoc(documentList->GetDocCount()-1);
#endif
		if(curDoc == nil)
		{
			error = PMString(kErrNeedDoc);
			throw TCLError(error);	
		}
		
		db = ::GetDataBase(curDoc);
		alreadyReversedItem = new UIDList(db);

		writer->SetDataBase(db);
		UIDRef docWS = curDoc->GetDocWorkSpace();

		// Get doc item list
		InterfacePtr<ITCLReferencesList> docTCLRefs (curDoc,UseDefaultIID());
		if(docTCLRefs != nil)
		{		
			docItemList = docTCLRefs->GetItemList();

			/*if(docItemList.size() - 1 > MAX_BLOC) // Should never occur
			{
				error = PMString(kErrTooManySavedItems);
				throw TCLError(error);
			}*/
		}
		
		// Doc name
		PMString docName;
		curDoc->GetName(docName);
		
		// Number of pages
		InterfacePtr<IPageList> pageListTmp (curDoc, UseDefaultIID());
		if(pageListTmp == nil)
			throw TCLError(error);
		
		pageList = pageListTmp;

		int32 nbPage = pageList->GetPageCount(); 
		
		// Dimensions (based on the first page's dimensions)
		UID firstPage = pageList->GetNthPageUID(0);
		
		InterfacePtr<IHierarchy> firstPageHier (db, firstPage, UseDefaultIID());

		PageType pageType = Utils<ILayoutUtils>()->GetPageType(UIDRef(db,firstPage));

		InterfacePtr<ISpread> firstSpread (db, firstPageHier->GetSpreadUID(), UseDefaultIID());
		int32 pagesPerSpread = firstSpread->GetNumPages();

		if(pageType != kUnisexPage)
		{
			if(pagesPerSpread == 1)
				pagesPerSpread = 2;
		}

		bool16 recto = ( (pagesPerSpread == 2) && (firstSpread->GetNumPages() == 1) );	// Not a recto/verso document if first spread has 2 pages or more
		
		PMReal left, top, right, bottom;
		InterfacePtr<IMargins> margins (pageList->QueryPageMargins(firstPage));
		if(margins == nil)
			throw TCLError(error);

		margins->GetMargins(&left,&top,&right,&bottom);
		
		InterfacePtr<IColumns> columns (pageList->Query__PageColumns(firstPage));
		if(columns == nil)
			throw TCLError(error);
		
		int32 nbCol = columns->GetNumberColumns_();
		PMReal gutter = columns->GetGutter_();
		
		InterfacePtr<IGeometry> pageGeo (columns, UseDefaultIID());
		if(pageGeo == nil)
			throw TCLError(error);
		
		PMRect bbox = pageGeo->GetStrokeBoundingBox();
		PMReal height = bbox.Height();
		PMReal width = bbox.Width();
		
		// Get document attributes in default paragraph style
		bool8 autoLeading = kTrue, autoKern = kTrue;
		PMReal autoLeadPercent = 20;
		LanguageID language;

		// Get default language
		InterfacePtr<ILanguageList> docLangList ((ILanguageList *) ::QueryPreferences(IID_ILANGUAGELIST,curDoc));
		if(docLangList == nil)
			throw TCLError(error);
		
		InterfacePtr<ILanguage> defaultLanguage (docLangList->QueryDefaultLanguage());
		if(defaultLanguage == nil)
			throw TCLError(error);

		language = defaultLanguage->GetLanguageID();

		InterfacePtr<IStyleGroupManager> paraStyles (docWS, IID_IPARASTYLEGROUPMANAGER);
		if(paraStyles == nil)
			throw TCLError(error);

		PMString defaultStyleName("NormalParagraphStyle",PMString::kUnknownEncoding);
		defaultParaStyle = paraStyles->FindByName(defaultStyleName);
		if (defaultParaStyle == kInvalidUID)
				defaultParaStyle = paraStyles->GetDefaultStyleUID();

		rootParaStyle = paraStyles->GetRootStyleUID();

		InterfacePtr<ITextAttributes> defaultAttributes (db, defaultParaStyle, UseDefaultIID());
		if(defaultParaStyle == kInvalidUID)
			throw TCLError(error);

		InterfacePtr<ITextAttrRealNumber> kernAttr ((ITextAttrRealNumber *) defaultAttributes->QueryByClassID(kTextAttrPairKernBoss, IID_ITEXTATTRREALNUMBER));
		if(kernAttr != nil)
		{
			if(kernAttr->GetRealNumber() != kAutoKernMagicNumber)
				autoKern = kFalse;
		}

		InterfacePtr<ITextAttrRealNumber> autoLeadPercentAttr ((ITextAttrRealNumber *) defaultAttributes->QueryByClassID(kTextAttrAutoLeadBoss, IID_ITEXTATTRREALNUMBER));
		if(autoLeadPercentAttr != nil)
			autoLeadPercent = (autoLeadPercentAttr->GetRealNumber() - 1.0) * 100.0;

		InterfacePtr<ITextAttrRealNumber> autoLeadtAttr ((ITextAttrRealNumber *) defaultAttributes->QueryByClassID(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER));
		if(autoLeadtAttr != nil)
		{
			if(autoLeadtAttr->GetRealNumber() != -1)
				autoLeading = kFalse;
		}

		InterfacePtr<ITextAttrUID> langAttr ((ITextAttrUID *) defaultAttributes->QueryByClassID(kTextAttrLanguageBoss, IID_ITEXTATTRUID));
		if(langAttr != nil)
		{
			UID langUID = langAttr->Get();
			InterfacePtr<ILanguage> lang (db, langUID, UseDefaultIID());
			if(lang == nil)
				throw TCLError(error);

			language = lang->GetLanguageID(); 
		}

		writer->writeNewDoc(docName, width, height, nbCol, gutter, left, top, right, bottom, nbPage, pagesPerSpread,
							recto, autoLeading, autoLeadPercent, language, autoKern);

		InterfacePtr<ITextOptions> docTxtOptions (docWS, IID_ITEXTOPTIONS);
		writer->writeDE(docTxtOptions->GetSuperScriptPos(), docTxtOptions->GetSuperScriptSize());
		writer->writeDH(docTxtOptions->GetSubScriptPos(), docTxtOptions->GetSubScriptSize());
		writer->writeDK(docTxtOptions->GetSmallCapSize());
		
		/**************************/
		/*	 Getting section list   */
		/**************************/
		InterfacePtr<ISectionList> sectionList (curDoc, UseDefaultIID());
		if(sectionList == nil)
			throw TCLError(error);
		
		UIDList * orderedSectionList = sectionList->QueryOrderedList();

		int32 nbSection = orderedSectionList->Length();
		for(int32 i = 0 ; i < nbSection ; ++i)
		{
			InterfacePtr<ISection> section (db, orderedSectionList->At(i), UseDefaultIID());
			PMString prefix = kNullString, marker = kNullString;
			ClassID styleClass = kInvalidClass;
			UID startPageUID = section->GetStartPageUID();
			int32 startPageNumber = section->GetStartPageNumber();
			bool16 autoNumber = section->GetContinueFromPrevSection();
			section->GetName(&prefix);			
			section->GetStyleInfo(&styleClass);
			bool16 includeSectionPrefix = section->GetPageNumberIncludeSectionPrefix();
			section->GetMarker(&marker);
			
			int8 style = 0;
			if(styleClass == kArabicPageNumberingStyleServiceBoss)
				style = 1;
			else if(styleClass == kRomanUpperPageNumberingStyleServiceBoss)
				style = 2;
			else if(styleClass == kRomanLowerPageNumberingStyleServiceBoss)
				style = 3;
			else if(styleClass == kLettersUpperPageNumberingStyleServiceBoss)
				style = 4;
			else if(styleClass == kLettersLowerPageNumberingStyleServiceBoss)
				style = 5;
			
			int32 startPageIndex = pageList->GetPageIndex(startPageUID) + 1;

			writer->writeSection(startPageIndex, startPageNumber, prefix, style, autoNumber, includeSectionPrefix, marker);
		}
		
		// Clean up section list pointer
		delete orderedSectionList;

		/**************************/
		/*	 Getting layer list   */
		/**************************/
		InterfacePtr<ILayerList> layerList (curDoc, UseDefaultIID());
		if(layerList == nil)
			throw TCLError(error);

		if(exportLayers)
		{			
			int32 nbLayers = layerList->GetCount();
			for(int32 i = 1 ; i < nbLayers ; ++i) // skip first : pages layer
			{
				InterfacePtr<IDocumentLayer> layer (layerList->QueryLayer(i));
				writer->writeNewLayer(layer->GetName()); 
			}
		}

		/**************************/
		/*	   Getting Colors     */
		/**************************/
		
		// Recognizing default colors
		InterfacePtr<ISwatchList> swatchList (docWS, UseDefaultIID());
		if(swatchList == nil)
			throw TCLError(error);
				
		ColorArray colorArray;
		UID tmpColor;
		colorList[0] = swatchList->GetPaperSwatchUID();

		colorArray.push_back(1);
		colorArray.push_back(0.9);
		colorArray.push_back(0.1);
		colorArray.push_back(0);
		tmpColor = swatchList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
		colorList[1] = (Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,tmpColor) ? tmpColor : kInvalidUID);			
		colorArray.clear();

		colorArray.push_back(1);
		colorArray.push_back(0);
		colorArray.push_back(0);
		colorArray.push_back(0);
		tmpColor = swatchList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
		colorList[2] = (Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,tmpColor) ? tmpColor : kInvalidUID);	
		colorArray.clear();

		colorArray.push_back(0);
		colorArray.push_back(0);
		colorArray.push_back(1);
		colorArray.push_back(0);
		tmpColor = swatchList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
		colorList[3] = (Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,tmpColor) ? tmpColor : kInvalidUID);	
		colorArray.clear();

		colorArray.push_back(0);
		colorArray.push_back(1);
		colorArray.push_back(0);
		colorArray.push_back(0);
		tmpColor = swatchList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
		colorList[4] = (Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,tmpColor) ? tmpColor : kInvalidUID);	
		colorArray.clear();

		colorList[5] = swatchList->GetBlackSwatchUID();
		colorList[6] = swatchList->GetRegistrationSwatchUID();

		colorArray.push_back(0.15);
		colorArray.push_back(1);
		colorArray.push_back(1);
		colorArray.push_back(0);
		tmpColor = swatchList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
		colorList[7] = (Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,tmpColor) ? tmpColor : kInvalidUID);	
		colorArray.clear();

		colorArray.push_back(0.75);
		colorArray.push_back(0.05);
		colorArray.push_back(1);
		colorArray.push_back(0);
		tmpColor = swatchList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
		colorList[8] = (Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,tmpColor) ? tmpColor : kInvalidUID);	
		colorArray.clear();

		colorList[9] = swatchList->GetNoneSwatchUID();
		
		if(exportColors)
		{
			// Get referenced colors
			UIDList used(db), unused(db);
			Utils<ISwatchUtils>()->GetSwatchReferences(db,used,unused,kTrue,kTrue,ISwatchUtils::kAllObjectReferences);
			
			// Remove default colors from used colors' list
			for(int i = 0 ; i < 10 ; ++i)
			{
				int32 pos = used.Location(colorList[i]);
				if(pos != -1)
					used.Remove(pos);	
			}
					
			int colorRef = 10;
			UIDList gradients(db), tinted(db);
			for(int i = 0 ; i < used.Length() ; ++i)
			{
				UID swatchUID = used[i];
				ISwatchUtils::SwatchType colorType = Utils<ISwatchUtils>()->GetSwatchType(UIDRef(db,swatchUID));
				if(colorType == ISwatchUtils::kGradient)
					gradients.Append(swatchUID);			
				else if(colorType == ISwatchUtils::kTintColor)
					tinted.Append(swatchUID);
				else	
				{					
					bool8 isSpot = Utils<ISwatchUtils>()->SwatchIsSpotColorSwatch(UIDRef(db,swatchUID));
					PMString colorName = Utils<ISwatchUtils>()->GetSwatchName(db,swatchUID);

					InterfacePtr<IColorData> color (db, swatchUID, UseDefaultIID());
					if(color == nil)
						throw TCLError(error);

					colorList[colorRef] = swatchUID;
					
					int32 colorSpace = color->GetColorSpace();
					ColorArray colorData = color->GetColorData(); 
					K2Vector<PMReal> values;
					switch(colorSpace)
					{
						case kPMCsCalCMYK :
							writer->writeNewColor(colorRef++, colorName, isSpot, CCID_CMYK, colorData);
							break;
					
						case kPMCsCalRGB :
							writer->writeNewColor(colorRef++, colorName, isSpot, CCID_RGB, colorData);
							break;
							
						case kPMCsLab :
							writer->writeNewColor(colorRef++, colorName, isSpot, CCID_LAB, colorData);
							break;

						default : 
							PMString invalidColor(kErrInvalidColorType);
							invalidColor.Translate();
							StringUtils::ReplaceStringParameters(&invalidColor, colorName);
							writer->GetReverseChecker()->WriteReverseLog(invalidColor, "", 0);
							colorList[colorRef] = kInvalidUID; 
							break;
					}
				}
			}

			for(int32 i = 0 ; i < tinted.Length() ; ++i)
			{
				UID swatchUID = tinted[i];
				InterfacePtr<IColorOverrides> colorOverrides (db, swatchUID, UseDefaultIID());
				PMReal tint = colorOverrides->GetTint();

				UID baseColorUID = kInvalidUID;
				Utils<ISwatchUtils>()->SwatchIsTintSwatch(UIDRef(db, swatchUID), &baseColorUID);

				if(baseColorUID == kInvalidUID)
					break;
			
				for(int32 j = 0 ; j < colorRef ; ++j)
				{
					if(colorList[j] == baseColorUID)
					{
						colorList[colorRef] = swatchUID;
						writer->writeNewTintedColor(colorRef++, j, tint);
						break;
					}
				}				
			}

			for(int32 i = 0 ; i < gradients.Length() ; ++i)
			{
				UID swatchUID = gradients[i];
				colorList[colorRef] = swatchUID;

				PMString colorName = Utils<ISwatchUtils>()->GetSwatchName(db,swatchUID);
				
				InterfacePtr<IGradientFill> gradientFill (db, swatchUID, UseDefaultIID());
				int8 type = (int8) gradientFill->GetGradientFillType();
				int16 nbStop = gradientFill->GetNumberOfStop();

				K2Vector<PMReal> stopPositions, midPoints;
				K2Vector<int32> colorRefs;
				for(int32 i = 0 ; i < nbStop ; ++i)
				{
					stopPositions.push_back(gradientFill->GetNthStopPosition(i)*100);
					midPoints.push_back(gradientFill->GetNthMidPointRelativeLocation(i)*100);
					
					UID colorStopUID = gradientFill->GetNthGradientColorUID(i);
					bool16 found = kFalse;
					for(int32 j = 0 ; !found && j < colorRef ; ++j)
					{
						if(colorList[j] == colorStopUID)
						{
							colorRefs.push_back(j);
							found = kTrue;
						}
					}
				}
				writer->writeNewGradient(colorRef++, colorName, type, colorRefs, stopPositions, midPoints);
			}
		}

		writer->SetColorList(colorList);
		
		/**************************/
		/*	   Getting Styles     */
		/**************************/
		InterfacePtr<IStyleGroupManager> charStyles (docWS, IID_ICHARSTYLEGROUPMANAGER);
		if(charStyles == nil)
			throw TCLError(error);

		defaultCharStyle = charStyles->GetRootStyleUID();
#if COMPOSER
		const AttributeBossList * allAttributes = nil;
		AttributeBossList * tmp = nil;
#endif
		if(exportStyles)
		{
			K2Vector<UID> sortedStyleList;
			this->SortStyleTable(paraStyles, sortedStyleList, paraStyles->GetRootStyleUID());

			for(int32 i = 0 ; i < sortedStyleList.size() ; ++i)
			{
				UID styleUID = sortedStyleList[i];
				PMString info; info.AppendNumber(i); info += " ";
				info.AppendNumber(styleUID.Get());
				//CAlert::InformationAlert(info);				
				
				InterfacePtr<IStyleInfo> styleInfo (db, styleUID, UseDefaultIID());
				if(styleInfo == nil)
					throw TCLError(error);

				if(styleUID == paraStyles->GetRootStyleUID()) 
				{
#if COMPOSER
					InterfacePtr<ITextAttributes> styleAttr (db, styleUID, UseDefaultIID());
					allAttributes = styleAttr->GetBossList(); // Root paragraph style contains definitions of all attributes
#endif
					continue;
				}

				UID basedOn = styleInfo->GetBasedOn();
				UID nextStyle = styleInfo->GetNextStyle();

				int32 basedOnNum = 0, nextStyleNum = 0;
				int32 pos = ::FindLocation(styles,basedOn);
				if(pos != -1)
					basedOnNum = styles[pos].Value();
				else
					basedOnNum = -1; // based on root paragraph style

				pos = ::FindLocation(styles,nextStyle);
				if(pos != -1)
					nextStyleNum = styles[pos].Value();
				else
				{
					if(nextStyle == paraStyles->GetRootStyleUID())
						nextStyleNum = -1;
				}

				PMString styleName = styleInfo->GetName();
				if(styleName != defaultStyleName)
					styleName.Translate();

				// Add style to map between references and UIDs
				styles.push_back(KeyValuePair<UID,int32>(styleUID,numStyle));
				styleNames.push_back(KeyValuePair<UID,PMString>(styleUID,styleName));
				
				writer->writeNewStyle(numStyle++, styleName, basedOnNum, nextStyleNum, kTrue);
				
				// Get text attributes
				InterfacePtr<ITextAttributes> styleAttr (db, styleUID, UseDefaultIID());
				if(styleAttr == nil)
					throw TCLError(error);
					
#if COMPOSER
				tmp = new AttributeBossList(*allAttributes);
				K2Vector<UID> parents;
				InterfacePtr<IStyleInfo> styleInfoTmp = styleInfo;
				while(styleInfoTmp->GetBasedOn() != paraStyles->GetRootStyleUID())
				{
					parents.push_back(styleInfoTmp->GetBasedOn());
					styleInfoTmp = InterfacePtr<IStyleInfo> (db, styleInfoTmp->GetBasedOn(), UseDefaultIID());
				}

				for(int32 i = parents.Length() - 1 ; i >= 0 ; --i)
				{
					InterfacePtr<ITextAttributes> styleAttrParent (db, parents[i], UseDefaultIID());
					tmp->ApplyAttributes(styleAttrParent->GetBossList());
				}

				tmp->ApplyAttributes(styleAttr->GetBossList());
				writer->writeStyleAttributes(tmp);
				delete tmp;
#else
				writer->writeStyleAttributes(styleAttr->GetBossList());
#endif
				writer->writeEndStyle();
			}
			
			sortedStyleList.clear();
			this->SortStyleTable(charStyles, sortedStyleList, charStyles->GetRootStyleUID());

			for(int32 i = 0 ; i < sortedStyleList.size() ; ++i)
			{
				UID styleUID = sortedStyleList[i];
				
				InterfacePtr<IStyleInfo> styleInfo (db, styleUID, UseDefaultIID());
				if(styleInfo == nil)
					throw TCLError(error);

				if(styleUID == charStyles->GetRootStyleUID()) 
				{
					//CAlert::InformationAlert(styleInfo->GetName());
					continue;
				}
				
				UID basedOn = styleInfo->GetBasedOn();
				
				int32 basedOnNum = 0;
				int32 pos = ::FindLocation(styles,basedOn);
				if(pos != -1)
					basedOnNum = styles[pos].Value();

				PMString styleName = styleInfo->GetName();
				styleName.Translate();

				// Add style to map between references and UIDs
				styles.push_back(KeyValuePair<UID,int32>(styleUID,numStyle));
				styleNames.push_back(KeyValuePair<UID,PMString>(styleUID,styleName));

				writer->writeNewStyle(numStyle++, styleName, basedOnNum, 0, kFalse);

				// Get text attributes
				InterfacePtr<ITextAttributes> styleAttr (db, styleUID, UseDefaultIID());
				if(styleAttr == nil)
					throw TCLError(error);

				writer->writeStyleAttributes(styleAttr->GetBossList());

				writer->writeEndStyle();
			}
		}
		
		/***************************************/
		/*	 Getting Table of Content Styles   */
		/***************************************/

		InterfacePtr<ITOCStyleNameTable> TOCStyleList (docWS, IID_ITOCSTYLENAMETABLE);
		int32 numTOCStyles = TOCStyleList->GetNumStyles();
		for(int32 i = 0 ; i < numTOCStyles ; ++i)
		{
			InterfacePtr<ITOCStyleInfo> tocStyleInfo (db, TOCStyleList->GetNthStyle(i), UseDefaultIID());

			int32 pos = -1, titleFormatStyle = -1;
			pos = ::FindLocation(styles,tocStyleInfo->GetTOCTitleFormatStyle());
			if(pos == -1)
				titleFormatStyle = -1;
			else
				titleFormatStyle = styles[pos].Value();
			
			FormatEntriesInfo entriesMap = tocStyleInfo->GetTOCEntryMap();
			K2Vector<KeyValuePair<int32, FormatEntryParam> > entries;

			int32 numEntries = entriesMap.size();
			for(int32 i = 0 ; i < numEntries ; ++i)
			{
				FormatEntryParam entryParam;
				TOCFormatEntryInfo formatEntryInfo = entriesMap[i].Value();

				pos = ::FindLocationByValue(styleNames,entriesMap[i].Key());
				int32 entryStyle = -1;
				if(pos != -1)
				{
					UID paraStyleEntry = styleNames[pos].Key();
					pos = ::FindLocation(styles, paraStyleEntry);
					entryStyle = styles[pos].Value();
				}
				
				if(formatEntryInfo.fParaFormatStyle != kInvalidUID)
				{
					pos = ::FindLocation(styles,formatEntryInfo.fParaFormatStyle);
					if(pos == -1)
						entryParam.formatEntryStyle = -1;
					else
						entryParam.formatEntryStyle = styles[pos].Value();
				}
				else
					entryParam.formatEntryStyle = -2;
				
				if(formatEntryInfo.fPageNumFormatStyle != kInvalidUID)
				{
					pos = ::FindLocation(styles,formatEntryInfo.fPageNumFormatStyle);
					entryParam.formatPageStyle = styles[pos].Value();
				}
				else
					entryParam.formatPageStyle = -1;

				entryParam.level = formatEntryInfo.fLevel;
				entryParam.levelSort = formatEntryInfo.fLevelSort;
				entryParam.pageNumPos = formatEntryInfo.fPosition;

				entryParam.separator = PMString(formatEntryInfo.fSeparator);

				if(formatEntryInfo.fSeparatorStyle != kInvalidUID)
				{
					pos = ::FindLocation(styles,formatEntryInfo.fSeparatorStyle);
					entryParam.formatSeparatorStyle = styles[pos].Value();
				}
				else
					entryParam.formatSeparatorStyle = -1;

				entries.push_back(KeyValuePair<int32,FormatEntryParam>(entryStyle, entryParam));
			}

			if(numEntries > 0) // BUG FIX : default style's name isn't always "DefaultTOCStyleName", but never has any entries
			{
				PMString styleName = tocStyleInfo->GetName();
				styleName.SetTranslatable(kFalse);
				writer->writeTOCStyle(styleName, tocStyleInfo->GetTOCTitle(), titleFormatStyle,
									tocStyleInfo->GetIncludeHidderLayerFlag(), tocStyleInfo->GetRunInFlag(), 
									tocStyleInfo->GetIncludeBookmarks(),entries);
			}
		}

		/*************************************/
		/*	   Getting Frames in masters     */
		/*************************************/

		// Determine the range of content spread layer within each spread's children 
		int32 nbLayers = layerList->GetCount();
		int32 startContentLayer, endContentLayer;
		
		InterfacePtr<IGuidePrefs> guidePrefs (docWS, UseDefaultIID());
		if(guidePrefs && guidePrefs->GetGuidesInBack())
		{
			startContentLayer = nbLayers + 1;
			endContentLayer = (2 * nbLayers) - 1;
		}
		else
		{
			startContentLayer = 2;
			endContentLayer = nbLayers;
		}

		if(exportMasters)
		{
			InterfacePtr<IMasterSpreadList> masterSpreadList (curDoc, UseDefaultIID());
			if(masterSpreadList == nil)
				throw TCLError(error);
			
			UIDList sortedMasterList(db);
			Utils<IMasterSpreadUtils>()->TopoSortMasterSpread(masterSpreadList, &sortedMasterList);

			int32 nbMasters = masterSpreadList->GetMasterSpreadCount();
			for(int32 i = nbMasters - 1 ; i >= 0 ; --i)
			{
				UID masterUID = sortedMasterList[i];
				InterfacePtr<IMasterSpread> master (db, masterUID, UseDefaultIID());
				if(master == nil)
					throw TCLError(error);

				InterfacePtr<ISpread> spread (master,UseDefaultIID());
				if(spread == nil)
					throw TCLError(error);

				masterSpreadGlobal = spread;

				// Get master spread info
				PMString masterName, basedOn;
				master->GetName(&masterName);

				UIDList ancestors(db);
				Utils<IMasterSpreadUtils>()->GetAncestorsOf(masterSpreadList,masterUID,&ancestors);
				
				if(ancestors.Length() <= 1) // Direct ancestor, if any, is at index 1
					basedOn = kNullString;
				else
				{
					InterfacePtr<IMasterSpread> basedOnMaster (db, ancestors[1], UseDefaultIID());
					if(basedOnMaster == nil)
						throw TCLError(error);

					basedOnMaster->GetName(&basedOn);
				}
			
				int32 nbPagesOfSpread = spread->GetNumPages();

				writer->writeNewMaster(masterName,basedOn,nbPagesOfSpread);
							
				// Iterator through spread layers (1st z-order level)
				InterfacePtr<IHierarchy> spreadHier (spread, UseDefaultIID());
				if(spreadHier == nil)
					throw TCLError(error);
			
				int32 childCount = spreadHier->GetChildCount();
			
				for(int32 j = startContentLayer ; j <= endContentLayer ; ++j)
				{
					InterfacePtr<IHierarchy> spreadLayerHier (db, spreadHier->GetChildUID(j), UseDefaultIID());
					if(spreadLayerHier == nil)
						break;

					int32 nbItems = spreadLayerHier->GetChildCount();
					
					// Iterate through childs (the page items) of the spread layer (2nd z-order level)
					for(int32 itemIndex = 0 ; itemIndex < nbItems ; ++itemIndex)
					{
						InterfacePtr<IHierarchy> itemHier (db, spreadLayerHier->GetChildUID(itemIndex), UseDefaultIID());
						if(itemHier == nil)
							break;

						UID ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);
						int32 curPage = spread->GetPageIndex(ownerPageUID) + 1;

						if(curPage == 0) // means that the item is outside pages, in this case we use the first page as reference
							curPage = 1;

						InterfacePtr<IGeometry> curPageGeo (db, spread->GetNthPageUID(curPage - 1), UseDefaultIID());
						if(curPageGeo == nil)
							throw TCLError(error);

						int32 alreadyReversedIndex =  alreadyReversedItem->Location(::GetUID(itemHier));
						if(alreadyReversedIndex == -1)
							ReversePageItem(::GetUIDRef(itemHier), curPageGeo, curPage);
						else
							alreadyReversedItem->Remove(alreadyReversedIndex);
					}
				}
				
				writer->writeEndMaster();
			}
		}

		/*************************************/
		/*	   Getting Frames in document    */
		/*************************************/
		InterfacePtr<ISpreadList> spreadList (curDoc, UseDefaultIID());
		if(spreadList == nil)
			throw TCLError(error);
		
		int32 nbSpreads = spreadList->GetSpreadCount();
		int32 firstPageOfSpread = 1;
		
		// Get master applied to page one
		InterfacePtr<IMasterPage> pageOne (db, firstPage, UseDefaultIID());
		if(pageOne == nil)
			throw TCLError(error);

		currentMaster = pageOne->GetMasterSpreadUID();
		int32 masterStartPage = 1;
		for(int32 i = 0 ; i < nbSpreads ; ++i)
		{
			InterfacePtr<ISpread> spread (db, spreadList->GetNthSpreadUID(i), UseDefaultIID());
			if(spread == nil)
				throw TCLError(error);
			
			InterfacePtr<IHierarchy> spreadHier (spread, UseDefaultIID());
			if(spreadHier == nil)
				throw TCLError(error);

			// Iterator through spread layers (1st z-order level)
			int32 childCount = spreadHier->GetChildCount();
			
			for(int32 j = startContentLayer ; j <= endContentLayer ; ++j)
			{
				InterfacePtr<IHierarchy> spreadLayerHier (db, spreadHier->GetChildUID(j), UseDefaultIID());
				if(spreadLayerHier == nil)
					break;

				int32 nbItems = spreadLayerHier->GetChildCount();
				
				// Iterate through childs (the page items) of the spread layer (2nd z-order level)
				for(int32 itemIndex = 0 ; itemIndex < nbItems ; ++itemIndex)
				{
					InterfacePtr<IHierarchy> itemHier (db, spreadLayerHier->GetChildUID(itemIndex), UseDefaultIID());
					if(itemHier == nil)
						break;

					UID ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);
					int32 pageIndexWithinSpread = spread->GetPageIndex(ownerPageUID);

					if(pageIndexWithinSpread == -1) // means that the item is outside pages, in this case we use the first page as reference
						pageIndexWithinSpread = 0;

					InterfacePtr<IGeometry> curPageGeo (db, spread->GetNthPageUID(pageIndexWithinSpread), UseDefaultIID());
					if(curPageGeo == nil)
						throw TCLError(error);

					int32 alreadyReversedIndex =  alreadyReversedItem->Location(::GetUID(itemHier));
					if(alreadyReversedIndex == -1)
						ReversePageItem(::GetUIDRef(itemHier), curPageGeo, firstPageOfSpread + pageIndexWithinSpread);
					else
						alreadyReversedItem->Remove(alreadyReversedIndex);
				}
			}

			int32 nbPagesOfSpread = spread->GetNumPages();
			if(exportMasters)
			{
				for(int32 j = 0 ; j < nbPagesOfSpread ; ++j)
				{
					// Check for an applied master
					InterfacePtr<IMasterPage> currentPage (db, spread->GetNthPageUID(j), UseDefaultIID());
					if(currentPage == nil)
						throw TCLError(error);

					UID masterUID = currentPage->GetMasterSpreadUID();
					if(currentMaster != masterUID) // another master is applied
					{
						InterfacePtr<IMasterSpread> master (db, currentMaster, UseDefaultIID());
						PMString masterName;
						if(master == nil)
							masterName = kNullString;
						else
							master->GetName(&masterName);

						writer->writeApplyMaster(masterName,masterStartPage, firstPageOfSpread - 1);
						
						currentMaster = masterUID;
						masterStartPage = firstPageOfSpread;
					}
					++firstPageOfSpread;
				}
			}
			else
				firstPageOfSpread += nbPagesOfSpread;
		}

		if(exportMasters)
		{
			// Write last applied master
			InterfacePtr<IMasterSpread> master (db, currentMaster, UseDefaultIID());
			PMString masterName;
			if(master == nil) // "None" master is applied
				masterName = kNullString;
			else
				master->GetName(&masterName);

			writer->writeApplyMaster(masterName,masterStartPage, firstPageOfSpread - 1);
		}

		// Export table of content, if any
		if(tocFrameRef != kInvalidTCLRef)
			writer->writeTOC(tocFrameRef, tocStyleName);
		
#if HYPERLINKS
		ReverseAllHyperlinkSource(curDoc);
#endif

		// Save newly assigned TCL references in the document
		SaveItemList();

		status = kSuccess;
	}
	catch(TCLError e) 
	{
		PMString error = e.getMessage();
		ErrorUtils::PMSetGlobalErrorCode(kFailure, &error);
	}

	this->CloseFile();

#if REVERSE_CHECKER && !SERVER
		// if reverse checker has detected any problem, display log file in system text editor
		PMString reverseLog = writer->GetReverseChecker()->GetReverseLog();
		if(reverseLog != kNullString)
		{
			this->DisplayReverseCheckerLog();
		}
#endif
	
	return status;
}

TCLRef PrsReverseTCL::getTCLRef(UID pageItem)
{
	TCLRef ref = kInvalidTCLRef;
	PMString info;
	// Check for existing TCL reference in document
	int32 pos = ::FindLocationByValue(docItemList, pageItem);
	if(pos == -1) // no reference found
	{
		do
		{
#if REF_AS_STRING
			ref.AsNumber(numFrame);
			--numFrame;
#else
			ref = numFrame--;
#endif
		} while(::FindLocation(docItemList, ref) != -1);

#if COMPOSER
		if(numFrame-1 < 2000)
			throw TCLError(PMString(kErrReverseNotEnoughRef));
#endif

		docItemList.push_back(TCLRefItem(ref, pageItem)); // register reference assigned automatically to items
	}
	else
		ref = docItemList[pos].Key();

	return ref;
}

void PrsReverseTCL::GetTextFrameProperties(IHierarchy * multicolHier, 
										   Text::VerticalJustification& vj, 
										   Text::FirstLineOffsetMetric& topAlign,
										   PMReal& vjMax,     PMReal& offset,      
										   PMReal& gutter,    int32& nbCol,
										   PMReal& leftInset, PMReal& rightInset , 
										   PMReal& topInset,  PMReal& bottomInset)
{
	PMString error(kErrNilInterface);
	
	InterfacePtr<IMultiColumnTextFrame> multiColSettings (multicolHier, UseDefaultIID());
	if(multiColSettings == nil)
		throw TCLError(error);
	
	vj = multiColSettings->GetVerticalJustification();
	topAlign = multiColSettings->GetFirstLineOffsetMetric();
	vjMax = multiColSettings->GetMaxVJInterParaSpace();
	offset = multiColSettings->GetMinFirstLineOffset();
	
	InterfacePtr<ITextColumnSizer> multiCol (multicolHier, UseDefaultIID());
	if(multiCol == nil)
		throw TCLError(error);
		
	nbCol = multiCol->GetNumberOfColumns();
	gutter = multiCol->GetGutterWidth();
	
	// Get insets values
	InterfacePtr<IHierarchy> textSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(multiColSettings));
	if (textSpline == nil) 
		throw TCLError(error);			
		
	InterfacePtr<ITextInset> textInset(textSpline, UseDefaultIID());
	if (textInset == nil) 
		throw TCLError(error);

	PMRect insetGeoCoord = textInset->GetRectInset();

	leftInset = insetGeoCoord.Left();
	topInset = insetGeoCoord.Top();
    rightInset = insetGeoCoord.Right();
    bottomInset = insetGeoCoord.Bottom();
}

void PrsReverseTCL::GetImageFrameProperties(TCLRef ref, IHierarchy * itemHier, PMString& fileName, PMReal& xScale, PMReal& yScale, PMReal& xDec, PMReal& yDec,
											int16& clipType, PMReal& tolerance, PMReal& inset, uint8& threshold, int16& index, bool16& invert, 
											bool16& allowHoles, bool16& limitToFrame, bool16& useHighRes, UID& tiffFillUID, PMReal& tiffFillTint, 
											PMReal& imageWidth, PMReal& imageHeight, PMReal& imageAngle, PMReal& imageSkew)
{
	PMString error(kErrNilInterface);

	PMString fullName = kNullString;
	InterfacePtr<IHierarchy> imageHier (itemHier->QueryChild(0));

	// If we're on a placed image we should have a data link to source item
	InterfacePtr<ILinkObject> iLinkObject(imageHier, UseDefaultIID());

	// get the link for this object
	IDataBase* iDataBase = ::GetDataBase(imageHier);
	InterfacePtr<ILinkManager> iLinkManager(iDataBase, iDataBase->GetRootUID(), UseDefaultIID());
	ILinkManager::QueryResult linkQueryResult;

	if (iLinkManager->QueryLinksByObjectUID(::GetUID(imageHier), linkQueryResult)){

		ILinkManager::QueryResult::const_iterator iter = linkQueryResult.begin();
		InterfacePtr<ILink> iLink (iDataBase, *iter, UseDefaultIID());
		if (iLink != nil){
			InterfacePtr<ILinkResource> iLinkResource(iLinkManager->QueryResourceByUID(iLink->GetResource()));
			if(iLinkResource != nil){
				
				URI linkURI = iLinkResource->GetId();
				IDFile linkFile;
				Utils<IURIUtils>()->URIToIDFile(linkURI, linkFile);
				PMString datalinkPath = FileUtils::SysFileToPMString(linkFile);
				fullName = datalinkPath;

			}
			else{

				PMString inconsistentLink(kErrInconsistentLink);
				inconsistentLink.Translate();
				PMString tclRefStr;
#if REF_AS_STRING
				tclRefStr = ref;
#else
				tclRefStr.AppendNumber(ref);
#endif
				StringUtils::ReplaceStringParameters(&inconsistentLink, tclRefStr);
				writer->GetReverseChecker()->WriteReverseLog(inconsistentLink, "", 0);
			}
		}
	}

	InterfacePtr<ITransform> imageTransform (imageHier, UseDefaultIID());
	if(imageTransform == nil)
		throw TCLError(error);
	
	xScale = imageTransform->GetItemScaleX() * 100.0;
	yScale = imageTransform->GetItemScaleY() * 100.0;
	
	InterfacePtr<IGeometry> imageGeo (imageHier, UseDefaultIID());
	if(imageGeo == nil)
		throw TCLError(error);
	
	InterfacePtr<IGeometry> itemGeo (itemHier, UseDefaultIID());
	if(itemGeo == nil)
		throw TCLError(error);

	// GD 1.1.7 / 8.0.5 ++
	//PMRect innerItemCoord = itemGeo->GetPathBoundingBox();
	//PMPoint imageLeftTop = imageGeo->GetPathBoundingBox(imageTransform->GetInnerToParentMatrix()).LeftTop(); // Transform to owner frame coords
	//InterfacePtr<ITransform> itemTransform (itemHier, UseDefaultIID());
	
	InterfacePtr<ITransform> itemTransform (itemHier, UseDefaultIID());
	PMRect innerItemCoord;
	PMPoint imageLeftTop;
	
	if(itemTransform->GetItemRotationAngle() == 0) {
		innerItemCoord = itemGeo->GetPathBoundingBox();
	::TransformInnerRectToPasteboard(itemGeo, &innerItemCoord);

		imageLeftTop = imageGeo->GetPathBoundingBox().LeftTop();
 	::TransformInnerPointToPasteboard(imageGeo, &imageLeftTop);
	} else {
		innerItemCoord = itemGeo->GetPathBoundingBox();
		imageLeftTop = imageGeo->GetPathBoundingBox(imageTransform->GetInnerToParentMatrix()).LeftTop(); // Transform to owner frame coords
	}
	// GD 1.1.7 / 8.0.5 --
	
	xDec = (imageLeftTop.X() - innerItemCoord.LeftTop().X()) * itemTransform->GetItemScaleX();
	yDec = (imageLeftTop.Y() - innerItemCoord.LeftTop().Y()) * itemTransform->GetItemScaleY();

	// Get image real width and height
	imageWidth = imageGeo->GetPathBoundingBox().Width();
	imageHeight = imageGeo->GetPathBoundingBox().Height();

	// Get image rotation angle and skew angle
	PMReal itemAngle = 360 - itemTransform->GetItemRotationAngle();
	if(itemAngle == 360)
		itemAngle = 0;

	// GD 7.5.7 ++
	//imageAngle = ::ToInt32((360.0 - imageTransform->GetItemRotationAngle()) + itemAngle) % 360;
	//imageAngle = 360.0 - imageTransform->GetItemRotationAngle();
	imageAngle = (360.0 - imageTransform->GetItemRotationAngle()) + itemAngle; // <- pas encore ça, il manque une translation que InDesign gere en auto sur rotation manuelle.
	// GD 7.5.7 --
	imageSkew = imageTransform->GetItemSkewAngle() * (-1);

	if(fullName != kNullString)
	{
		fileName = fullName;

		// Check whether it's a TIFF image and if so, get fill and tint property
		PMString ext;
		FileUtils::GetExtension(FileUtils::PMStringToSysFile(fileName), ext);

		if(ext.Compare(kFalse, PMString("tif")) == 0 || ext.Compare(kFalse, PMString("tiff")) == 0)
		{
			InterfacePtr<IGraphicStyleDescriptor> graphicDesc (imageHier, UseDefaultIID());
			Utils<IGraphicAttributeUtils>()->GetFillRenderingUID(tiffFillUID, graphicDesc);
			Utils<IGraphicAttributeUtils>()->GetFillTint(tiffFillTint, graphicDesc);
			if(tiffFillTint == kInvalidTintPercent)
				tiffFillTint = 100.0;
		}
	}
	else
		fileName = kNullString;

	// Clipping path settings
	InterfacePtr<IClipSettings> clipSettings (imageHier, UseDefaultIID());
	if(clipSettings)
	{
		clipType = clipSettings->GetClipType();
		tolerance = clipSettings->GetTolerance();
		inset = clipSettings->GetInset();
		threshold = clipSettings->GetThreshold();
		if(clipType == IClipSettings::kClipAlpha)
			index = clipSettings->GetAlphaIndex();
		else
			index = clipSettings->GetEmbeddedPathIndex();
		invert = clipSettings->GetInvert();
		allowHoles = clipSettings->GetAllowHoles();
		limitToFrame = clipSettings->GetRestrictToFrame();
		useHighRes = clipSettings->GetUseHighRes();
	}
	else
	{
		PMString errClippingPath(kErrClippingPath);
		errClippingPath.Translate();
		PMString tclRefStr;
#if REF_AS_STRING
		tclRefStr = ref;
#else
		tclRefStr.AppendNumber(ref);
#endif
		StringUtils::ReplaceStringParameters(&errClippingPath, tclRefStr);
		writer->GetReverseChecker()->WriteReverseLog(errClippingPath, "", 0);
	}
}

/*
	Remove rotation, skewing and flipping effects
*/
PMPoint PrsReverseTCL::RemoveTransformations(PMPoint toTransform, PMPoint center, PMReal rotationAngle, PMReal xSkewAngle, PMReal yScale, IGeometry * itemGeo)
{
	PMPoint transformedPt = toTransform;

	if(rotationAngle != 0)
	{
		// Compute point before rotation		
		toTransform.X() -= center.X();
		toTransform.Y() -= center.Y();			
		
		double angleRadian = ToDouble(((-1) * rotationAngle * kPMPi)/180.0);

		transformedPt.X() = (toTransform.X()  * cos(angleRadian)) + (toTransform.Y() * sin(angleRadian));
		transformedPt.Y() = (toTransform.X() * sin(angleRadian) * (-1)) + (toTransform.Y() * cos(angleRadian));

		if(yScale < 0)
			transformedPt.Y() = -transformedPt.Y();

		transformedPt.X() += center.X();
		transformedPt.Y() += center.Y();
	}
	else
	{
		if(yScale < 0)
			transformedPt.Y() = transformedPt.Y() + 2 * (center.Y() - transformedPt.Y());
	}

	if(xSkewAngle != 0)
	{
		double angleRadian = ToDouble((-xSkewAngle * kPMPi)/180.0);

		if(yScale < 0)
			angleRadian = -angleRadian;

		PMReal diffHeight = center.Y() - transformedPt.Y(); 
		PMReal decSkewing = tan(angleRadian) * diffHeight;
		
		transformedPt.X() += decSkewing;	
	}
	return transformedPt;
}

void PrsReverseTCL::GetFrameGeneralAttributes(IHierarchy * itemHier, PMReal& rotationAngle, PMReal& xSkewAngle, PMReal& xScale, PMReal& yScale, PMPoint& leftTopWithoutTransformation, 
											  PMReal& width, PMReal& height, ClassID& lineImpl, ClassID& cornerImpl, UID& strokeUID, UID& fillUID, PMReal& strokeTint, PMReal& fillTint, PMReal& strokeWeight, int32& strokeAlign, PMReal& cornerRadius, UID& gapUID, PMReal& gapTint, bool16& nonPrint, bool16& locked, PMReal& gradientFillAngle, PMReal& gradientStrokeAngle, PMReal& gradientFillLength, PMReal& gradientFillRadius, PMPoint& gradientFillCenter, PMReal& gradientStrokeLength, PMReal& gradientStrokeRadius, PMPoint& gradientStrokeCenter, int8& BSblendMode, PMReal& BSopacity, bool16& BSknockoutGroup, bool16& BSisolationGroup, int8& VTmode, PMReal& VTwidth, int8& VTcorner, PMReal& VTnoise, PMReal& VTinnerOpacity, PMReal& VTouterOpacity, bool16& DSmode, int8& DSblendMode, PMReal& DSxOffset, PMReal& DSyOffset, UID& DScolor, PMReal& DSopacity, PMReal& DSradius, PMReal& DSspread, PMReal& DSnoise,IGeometry * curPageGeo)
{
	PMString error(kErrNilInterface);

	// Get Transformations
	InterfacePtr<ITransform> itemTransform (itemHier, UseDefaultIID());
	if(itemTransform == nil)
		throw TCLError(error);
	
	rotationAngle = 360.0 - itemTransform->GetItemRotationAngle();
	xSkewAngle = itemTransform->GetItemSkewAngle() * (-1);
	xScale = itemTransform->GetItemScaleX();
	yScale = itemTransform->GetItemScaleY();
	
	// Get item coordinates
	InterfacePtr<IGeometry> itemGeo (itemHier, UseDefaultIID());
	if(itemGeo == nil)
		throw TCLError(error);

	PMRect innerItemCoord = itemGeo->GetPathBoundingBox();
	PMPoint leftTop = innerItemCoord.LeftTop();

	PMPoint center = innerItemCoord.GetCenter();

	// Compute width and height including scaling

	/* OLD WAY to do it : problems with scale which is not 100 % everytime and has a strange value if item belongs to a group scaled itself
	width = innerItemCoord.Width() * ::abs(xScale); 
	height = innerItemCoord.Height() * ::abs(yScale);
	*/

	PMPoint leftTopTmp = innerItemCoord.LeftTop(), rightTopTmp = innerItemCoord.RightTop(), leftBottom = innerItemCoord.LeftBottom();

	// Remove skew effect
	PMMatrix invertMatrix;
	if(yScale > 0)
		invertMatrix.SkewTo(itemTransform->GetItemSkewAngle() * (-1));
	else
		invertMatrix.SkewTo(itemTransform->GetItemSkewAngle());
	invertMatrix.Transform(&leftTopTmp);
	invertMatrix.Transform(&rightTopTmp);
	invertMatrix.Transform(&leftBottom);

	::TransformInnerPointToPasteboard(itemGeo, &leftTopTmp);
	::TransformInnerPointToPasteboard(itemGeo, &rightTopTmp);
	::TransformInnerPointToPasteboard(itemGeo, &leftBottom);

	width = sqrt(pow(::ToDouble(rightTopTmp.X() - leftTopTmp.X()),2) + pow(::ToDouble(rightTopTmp.Y() - leftTopTmp.Y()),2));
	height = sqrt(pow(::ToDouble(leftTopTmp.X() - leftBottom.X()),2) + pow(::ToDouble(leftTopTmp.Y() - leftBottom.Y()),2));

	if(rotationAngle == 360)
		rotationAngle = 0;

	// Compute left top coordinates before transformations (rotation and skewing)
	if(curPageGeo != nil)
	{
		// First transform coordinates from item inner space to pasteboard space
		::TransformInnerPointToPasteboard(itemGeo, &leftTop);
		::TransformInnerPointToPasteboard(itemGeo, &center);
		// Then transform coordinates from pasteboard space to page space
		::TransformPasteboardPointToInner(curPageGeo, &leftTop);	
		::TransformPasteboardPointToInner(curPageGeo, &center);	

		leftTopWithoutTransformation = this->RemoveTransformations(leftTop, center, rotationAngle, xSkewAngle, yScale, itemGeo);
	} 
	else  // it's an inline, no need to convert coordinates
	{
		leftTopWithoutTransformation = itemGeo->GetStrokeBoundingBox(itemTransform->GetInnerToParentMatrix()).LeftTop();

	}

	// Get graphic attributes
	GetGraphicAttributes(itemHier, lineImpl, cornerImpl, strokeUID, fillUID, strokeTint, fillTint, strokeWeight, strokeAlign, cornerRadius, gapUID, gapTint, nonPrint, gradientFillAngle, gradientStrokeAngle,
						 gradientFillLength, gradientFillRadius, gradientFillCenter, gradientStrokeLength, gradientStrokeRadius, gradientStrokeCenter, 
						 BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity,
						 DSmode, DSblendMode, DSxOffset, DSyOffset, DScolor, DSopacity, DSradius, DSspread, DSnoise);

	// Transform gradient center attribut to pasteboard coordinate
	::TransformInnerPointToPasteboard(itemGeo, &gradientFillCenter);
	::TransformInnerPointToPasteboard(itemGeo, &gradientStrokeCenter);

	// Is position locked
	InterfacePtr<ILockPosition> iLockPos (itemHier, UseDefaultIID());
	locked = iLockPos->IsLocked();
}

void PrsReverseTCL::GetGraphicAttributes(IHierarchy * itemHier, ClassID& lineImpl, ClassID& cornerImpl, UID& strokeUID, UID& fillUID, PMReal& strokeTint, 
										 PMReal& fillTint, PMReal& strokeWeight, int32& strokeAlign, PMReal& cornerRadius,  UID& gapUID, PMReal& gapTint, bool16& nonPrint, PMReal& gradientFillAngle, PMReal& gradientStrokeAngle,
										 PMReal& gradientFillLength, PMReal& gradientFillRadius, PMPoint& gradientFillCenter, PMReal& gradientStrokeLength, PMReal& gradientStrokeRadius, PMPoint& gradientStrokeCenter,  
										 int8& BSblendMode, PMReal& BSopacity, bool16& BSknockoutGroup, bool16& BSisolationGroup, int8& VTmode, PMReal& VTwidth, int8& VTcorner, PMReal& VTnoise, PMReal& VTinnerOpacity, PMReal& VTouterOpacity,
										 bool16& DSmode, int8& DSblendMode, PMReal& DSxOffset, PMReal& DSyOffset, UID& DScolor, PMReal& DSopacity, PMReal& DSradius, PMReal& DSspread, PMReal& DSnoise)
{
	PMString error(kErrNilInterface);

	InterfacePtr<IGraphicStyleDescriptor> graphicDesc (itemHier, UseDefaultIID());
	if(graphicDesc == nil)
		throw TCLError(error);						
	
	UID dummyUID = kInvalidUID;
	Utils<IGraphicAttributeUtils>()->GetFillRenderingUID(fillUID, graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetFillTint(fillTint,graphicDesc);
	if(fillTint == kInvalidTintPercent)
		fillTint = 100.0;
	Utils<IGraphicAttributeUtils>()->GetLineImplementation(lineImpl, dummyUID, graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetStrokeRenderingUID(strokeUID,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetStrokeTint(strokeTint,graphicDesc);
	if(strokeTint == kInvalidTintPercent)
		strokeTint = 100.0;
	Utils<IGraphicAttributeUtils>()->GetStrokeWeight(strokeWeight,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetStrokeAlignment(strokeAlign,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetCornerRadius(cornerRadius,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetCornerImplementation(cornerImpl,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetGapRenderingUID(gapUID,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetGapTint(gapTint,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetNonPrint(nonPrint,graphicDesc);

	// Drop shadow attributes
	PMDropShadowMode mode;
	Utils<IXPAttributeUtils>()->GetDropShadowMode(mode,graphicDesc);
	if(mode == kDSMBlurred)
		DSmode = kTrue;
	else
		DSmode = kFalse;

	// Drop shadow attributes
	PMBlendingMode blendMode;
	Utils<IXPAttributeUtils>()->GetDropShadowBlendMode(blendMode,graphicDesc);
	DSblendMode = blendMode;

	Utils<IXPAttributeUtils>()->GetDropShadowOffsetX(DSxOffset,graphicDesc);
	Utils<IXPAttributeUtils>()->GetDropShadowOffsetY(DSyOffset,graphicDesc);
	Utils<IXPAttributeUtils>()->GetDropShadowOpacity(DSopacity,graphicDesc);
	Utils<IXPAttributeUtils>()->GetDropShadowBlurRadius(DSradius,graphicDesc);
	Utils<IXPAttributeUtils>()->GetDropShadowColorUID(DScolor,graphicDesc);
	Utils<IXPAttributeUtils>()->GetDropShadowSpread(DSspread,graphicDesc);
	Utils<IXPAttributeUtils>()->GetDropShadowNoise(DSnoise,graphicDesc);

	// Transparency attributes
	Utils<IXPAttributeUtils>()->GetBasicBlendMode (blendMode,graphicDesc);
	BSblendMode = blendMode;
	Utils<IXPAttributeUtils>()->GetBasicOpacity (BSopacity,graphicDesc);
	Utils<IXPAttributeUtils>()->GetBasicIsolationGroup (BSisolationGroup,graphicDesc);
	Utils<IXPAttributeUtils>()->GetBasicKnockoutGroup (BSknockoutGroup,graphicDesc);

	// Feather attributes
	PMVignetteMode vignetteMode;
	Utils<IXPAttributeUtils>()->GetVignetteMode(vignetteMode,graphicDesc);
	VTmode = vignetteMode;
	Utils<IXPAttributeUtils>()->GetVignetteWidth(VTwidth,graphicDesc);
	PMVignetteCorners vignetteCorners;
	Utils<IXPAttributeUtils>()->GetVignetteCorners(vignetteCorners,graphicDesc);
	VTcorner = vignetteCorners;
	Utils<IXPAttributeUtils>()->GetVignetteNoise(VTnoise,graphicDesc);
	Utils<IXPAttributeUtils>()->GetVignetteInnerOpacity (VTinnerOpacity,graphicDesc);
	Utils<IXPAttributeUtils>()->GetVignetteOuterOpacity (VTouterOpacity,graphicDesc);

	// Gradient attributes
	Utils<IGraphicAttributeUtils>()->GetGradientFillAngle(gradientFillAngle,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetGradientStrokeAngle(gradientStrokeAngle,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetGradientFillLength(gradientFillLength, graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetGradientStrokeLength(gradientStrokeLength, graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetGradientFillRadius(gradientFillRadius, graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetGradientStrokeRadius(gradientStrokeRadius, graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetGradientFillGradCenter(gradientFillCenter, graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetGradientStrokeGradCenter(gradientStrokeCenter, graphicDesc);
}

void PrsReverseTCL::GetSplineAttributes(IPathGeometry * itemPathGeo, K2Vector<KeyValuePair<PMPathPointList, bool16> >& ptsList, PMReal& rotationAngle, PMReal& xSkewAngle, PMReal& xScale, PMReal& yScale, PMPoint& leftTopWithoutTransformation, PMReal& width, PMReal& height, 
								ClassID& lineImpl, ClassID& cornerImpl, UID& strokeUID, UID& fillUID, PMReal& strokeTint, PMReal& fillTint, PMReal& strokeWeight, int32& strokeAlign, PMReal& cornerRadius, UID& gapUID, PMReal& gapTint, bool16& nonPrint, bool16& locked, 
								int8& BSblendMode, PMReal& BSopacity, bool16& BSknockoutGroup, bool16& BSisolationGroup, int8& VTmode, PMReal& VTwidth, int8& VTcorner, PMReal& VTnoise, PMReal& VTinnerOpacity, PMReal& VTouterOpacity, 
								PMReal& gradientFillAngle, PMReal& gradientStrokeAngle, PMReal& gradientFillLength, PMReal& gradientFillRadius, PMPoint& gradientFillCenter, PMReal& gradientStrokeLength, PMReal& gradientStrokeRadius, PMPoint& gradientStrokeCenter,  
								bool16& DSmode, int8& DSblendMode, PMReal& DSxOffset, PMReal& DSyOffset, UID& DScolor, PMReal& DSopacity, PMReal& DSradius, PMReal& DSspread, PMReal& DSnoise,IGeometry * curPageGeo)
{
	PMString error(kErrNilInterface);

	// Get Transformations
	InterfacePtr<ITransform> itemTransform (itemPathGeo, UseDefaultIID());
	if(itemTransform == nil)
		throw TCLError(error);
	
	rotationAngle = 360.0 - itemTransform->GetItemRotationAngle();
	xSkewAngle = itemTransform->GetItemSkewAngle() * (-1);
	xScale = itemTransform->GetItemScaleX();
	yScale = itemTransform->GetItemScaleY();
	
	if(rotationAngle == 360)
		rotationAngle = 0;

	InterfacePtr<IGeometry> itemGeo (itemPathGeo, UseDefaultIID());
	PMRect innerItemCoord = itemGeo->GetPathBoundingBox();
	PMPoint leftTop = innerItemCoord.LeftTop();
	PMPoint center = innerItemCoord.GetCenter();

	// Compute width and height including scaling
	PMPoint leftTopTmp = innerItemCoord.LeftTop(), rightTopTmp = innerItemCoord.RightTop(), leftBottom = innerItemCoord.LeftBottom();

	// Remove skew effect
	PMMatrix invertMatrix;
	if(yScale > 0)
		invertMatrix.SkewTo(itemTransform->GetItemSkewAngle() * (-1));
	else
		invertMatrix.SkewTo(itemTransform->GetItemSkewAngle());
	invertMatrix.Transform(&leftTopTmp);
	invertMatrix.Transform(&rightTopTmp);
	invertMatrix.Transform(&leftBottom);

	::TransformInnerPointToPasteboard(itemGeo, &leftTopTmp);
	::TransformInnerPointToPasteboard(itemGeo, &rightTopTmp);
	::TransformInnerPointToPasteboard(itemGeo, &leftBottom);

	width = sqrt(pow(::ToDouble(rightTopTmp.X() - leftTopTmp.X()),2) + pow(::ToDouble(rightTopTmp.Y() - leftTopTmp.Y()),2));
	height = sqrt(pow(::ToDouble(leftTopTmp.X() - leftBottom.X()),2) + pow(::ToDouble(leftTopTmp.Y() - leftBottom.Y()),2));

	leftTopWithoutTransformation = leftTop;

	if(curPageGeo != nil)
    {
		// First transform coordinates from item inner space to pasteboard space
		::TransformInnerPointToPasteboard(itemGeo, &leftTop);
		::TransformInnerPointToPasteboard(itemGeo, &center);
		// Then transform coordinates from pasteboard space to page space
		::TransformPasteboardPointToInner(curPageGeo, &leftTop);	
		::TransformPasteboardPointToInner(curPageGeo, &center);

		leftTopWithoutTransformation = this->RemoveTransformations(leftTop, center, rotationAngle, xSkewAngle, yScale, itemGeo);

		int32 numPaths = itemPathGeo->GetNumPaths();
		if(numPaths < 1)
			throw TCLError(error);

		for (int32 index = 0; index < numPaths; index++)
		{	
			PMPathPointList pathPtList;

			int32 nbPoints = itemPathGeo->GetNumPoints(index);
			if(nbPoints < 1)
				throw TCLError(error);

			for(int32 i = 0 ; i < nbPoints; ++i)
			{
				PMPathPoint pt (itemPathGeo->GetNthPoint(index,i));										

				// Transform to pasteboard coordinates
				::TransformInnerPathToPasteboard(itemGeo, &pt);
		
				// Then transform coordinates from pasteboard space to page space
				::TransformPasteboardPathToInner(curPageGeo, &pt);	

				PMPoint leftDir = this->RemoveTransformations(pt.GetLeftDirPoint(), center, rotationAngle, xSkewAngle, yScale, itemGeo);
				PMPoint anchor = this->RemoveTransformations(pt.GetAnchorPoint(), center, rotationAngle, xSkewAngle, yScale, itemGeo);
				PMPoint rightDir = this->RemoveTransformations(pt.GetRightDirPoint(), center, rotationAngle, xSkewAngle, yScale, itemGeo);

				// Make the path points relative to the left top corner
				leftDir -= leftTopWithoutTransformation;
				anchor -= leftTopWithoutTransformation;
				rightDir -= leftTopWithoutTransformation;
				pathPtList.push_back(PMPathPoint(leftDir,anchor,rightDir));
			}		

			ptsList.push_back(KeyValuePair<PMPathPointList,bool16>(pathPtList,itemPathGeo->IsPathClosed(index)));
		}
	}
	else
	{
		int32 numPaths = itemPathGeo->GetNumPaths();
		for (int32 index = 0; index < numPaths; index++)
		{	
			PMPathPointList pathPtList;

			int32 nbPoints = itemPathGeo->GetNumPoints(index);
			for(int32 i = 0 ; i < nbPoints; ++i)
			{
				PMPathPoint pt (itemPathGeo->GetNthPoint(index,i));										

				// Transform to pasteboard coordinates
				::TransformInnerPathToPasteboard(itemGeo, &pt);

				PMPoint leftDir = this->RemoveTransformations(pt.GetLeftDirPoint(), center, rotationAngle, xSkewAngle, yScale, itemGeo);
				PMPoint anchor = this->RemoveTransformations(pt.GetAnchorPoint(), center, rotationAngle, xSkewAngle, yScale, itemGeo);
				PMPoint rightDir = this->RemoveTransformations(pt.GetRightDirPoint(), center, rotationAngle, xSkewAngle, yScale, itemGeo);

				// Make the path points relative to the left top corner
				leftDir -= leftTopWithoutTransformation;
				anchor -= leftTopWithoutTransformation;
				rightDir -= leftTopWithoutTransformation;
				pathPtList.push_back(PMPathPoint(leftDir, anchor, rightDir));
			}									

			ptsList.push_back(KeyValuePair<PMPathPointList,bool16>(pathPtList,itemPathGeo->IsPathClosed(index)));
		}
	}

	InterfacePtr<IHierarchy> itemHier (itemGeo, UseDefaultIID());

	// Get graphic attributes
	GetGraphicAttributes(itemHier, lineImpl, cornerImpl, strokeUID, fillUID, strokeTint, fillTint, strokeWeight, strokeAlign, cornerRadius, gapUID, gapTint, nonPrint, gradientFillAngle, gradientStrokeAngle,
						gradientFillLength, gradientFillRadius, gradientFillCenter, gradientStrokeLength, gradientStrokeRadius, gradientStrokeCenter, 
						BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity,
						DSmode, DSblendMode, DSxOffset, DSyOffset, DScolor, DSopacity, DSradius, DSspread, DSnoise);

	// Transform gradient center attribut to page coordinate
	::TransformInnerPointToPasteboard(itemGeo, &gradientFillCenter);
	::TransformInnerPointToPasteboard(itemGeo, &gradientStrokeCenter);

	// Is position locked
	InterfacePtr<ILockPosition> iLockPos (itemHier, UseDefaultIID());
	locked = iLockPos->IsLocked();
}

void PrsReverseTCL::GetRuleAttributes(IPathGeometry * rulePathGeo, PMPoint& p1, PMPoint& p2, ClassID& lineImpl, PMReal& stroke, int32& strokeAlign, UID& color, 
										   PMReal& tint, ClassID& lineEndStartImpl, ClassID& lineEndEndImpl, bool16& nonPrint, IGeometry * curPageGeo)
{
	PMString error(kErrNilInterface);

	PMRect bbox = rulePathGeo->GetPathBoundingBox();

	p1 = rulePathGeo->GetNthPoint(0,0).GetAnchorPoint();
	p2 = rulePathGeo->GetNthPoint(0,1).GetAnchorPoint();

	InterfacePtr<IGeometry> ruleGeo (rulePathGeo, UseDefaultIID());

	::TransformInnerPointToPasteboard(ruleGeo, &p1);
	::TransformInnerPointToPasteboard(ruleGeo, &p2);

	// Then transform coordinates from pasteboard space to page space
	::TransformPasteboardPointToInner(curPageGeo, &p1);	
	::TransformPasteboardPointToInner(curPageGeo, &p2);	

	// Get graphic attributes
	InterfacePtr<IGraphicStyleDescriptor> graphicDesc (ruleGeo, UseDefaultIID());
	if(graphicDesc == nil)
		throw TCLError(error);						
	
	UID dummyUID = kInvalidUID;
	Utils<IGraphicAttributeUtils>()->GetLineImplementation(lineImpl, dummyUID, graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetStrokeRenderingUID(color,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetStrokeTint(tint,graphicDesc);
	if(tint == kInvalidTintPercent)
		tint = 100.0;
	Utils<IGraphicAttributeUtils>()->GetStrokeWeight(stroke,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetStrokeAlignment(strokeAlign,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetLineEndStartImplementation(lineEndStartImpl,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetLineEndEndImplementation(lineEndEndImpl,graphicDesc);
	Utils<IGraphicAttributeUtils>()->GetNonPrint(nonPrint,graphicDesc);
}

void PrsReverseTCL::GetTextWrapAttributes(IHierarchy * itemHier, IStandOff::mode& mode, IStandOff::form& form, PMReal& topStOff,PMReal& rightStOff,
										  PMReal& leftStOff, PMReal& bottomStOff, IStandOffContourWrapSettings::ContourWrapType& contourType)
{
	PMString error(kErrNilInterface);
		
	InterfacePtr<IStandOffData> standoffData (itemHier, UseDefaultIID());
	if(standoffData == nil)
		throw TCLError(error);
	
	mode = standoffData->GetMode();
	form = standoffData->GetForm();

	topStOff = standoffData->GetMargin(PMRect:: kMiddleTop,kFalse);
	rightStOff = standoffData->GetMargin(PMRect:: kRightMiddle,kFalse);
	leftStOff = standoffData->GetMargin(PMRect:: kLeftMiddle,kFalse); 
	bottomStOff = standoffData->GetMargin(PMRect:: kMiddleBottom,kFalse);

	contourType = IStandOffContourWrapSettings::kInvalid;
	InterfacePtr<IStandOffContourWrapSettings> contourWrapSettings (itemHier, UseDefaultIID());
	if(contourWrapSettings)
		contourType = contourWrapSettings->GetType();				
}

TCLRef PrsReverseTCL::ReversePageItem(UIDRef pageItem, IGeometry * curPageGeo, int32 curPage, bool8 isInline, TCLRef refOwner)
{
	PMString error = PMString(kErrNilInterface);
	TCLRef ref = kInvalidTCLRef;

	InterfacePtr<IHierarchy> itemHier (pageItem, UseDefaultIID());
	if(itemHier == nil)
		throw TCLError(error);
	
	if(!isInline)
	{
		// Get owner layer
		InterfacePtr<ISpreadLayer> ownerSpreadLayer (db, itemHier->GetLayerUID(), UseDefaultIID());
		if(ownerSpreadLayer == nil)
			throw TCLError(error);
		
		InterfacePtr<IDocumentLayer> ownerLayer (ownerSpreadLayer->QueryDocLayer());
		if(ownerLayer == nil)
			error = PMString("No owner document layer");

		if(exportLayers)
		{
			UID layerUID = ::GetUID(ownerLayer);

			if(currentLayer != layerUID)
			{
				currentLayer = layerUID;
				writer->writeSetCurrentLayer(ownerLayer->GetName());
			}
		}
	}
	else
	{
		if(refOwner == kInvalidTCLRef)
			isInline = kFalse; // Special case : item belongs to a group which is an inline, don't consider the item itself as an inline
	}

	PMString info;
	//DebugClassUtilsBuffer buff;
	//DebugClassUtils::GetIDName(&buff, ::GetClass(itemHier));
	//info.Append(buff);
	
	if(Utils<IPageItemTypeUtils>()->IsGroup(itemHier))
	{
		info += " group";
		int32 nbItemInGroup = itemHier->GetChildCount();
		
		K2Vector<TCLRef> itemRefList;
		bool16 createGroup = kTrue;
		for(int32 i = 0 ; i < nbItemInGroup ; ++i)
		{
			TCLRef refItem = ReversePageItem(UIDRef(db,itemHier->GetChildUID(i)), curPageGeo, curPage, isInline, kInvalidTCLRef);
			if(refItem == kInvalidTCLRef) // kInvalidTCLRef is returned by a group item
			{
					// Can't create a group which includes another group with TCL commands
					createGroup = kFalse;
			
			}
			else if(refItem != kSkippedTCLRef)
				itemRefList.push_back(refItem);	
		}

		if(createGroup && itemRefList.size() > 1)
		{
			if(isInline) // Group is inline, use AG command
				writer->writeCreateInlineGroup(refOwner, itemRefList);
			else
				writer->writeCreateGroup(itemRefList);
		}
		else
			writer->GetReverseChecker()->WriteReverseLog(PMString(kErrGroupOfGroup), "", 0);

		ref = kInvalidTCLRef;
	}
	else
	{
		do
		{
			ref = getTCLRef(pageItem.GetUID());

			PMReal rotationAngle = 0.0, xSkewAngle = 0.0, xScale = 100.0, yScale = 100.0, itemWidth = 12.0, itemHeight = 12.0;
			PMReal DSxOffset = 0.0, DSyOffset = 0.0, DSopacity = 75.0, DSradius = 0.0, DSspread = 0.0, DSnoise = 0.0;
			PMReal BSopacity = 100.0, VTwidth = 12.0, VTnoise = 0.0, VTinnerOpacity = 100.0, VTouterOpacity = 0.0;
			PMReal gradientFillAngle = 0.0, gradientStrokeAngle = 0.0, gradientFillLength = 0.0, gradientStrokeLength = 0.0, gradientFillRadius = 0.0, gradientStrokeRadius = 0.0;
			PMPoint leftTop(0.0,0.0), gradientFillCenter(0.0,0.0), gradientStrokeCenter(0.0,0.0);
			ClassID lineImpl, cornerImpl = kInvalidClass;
			UID strokeUID, fillUID, dummyUID = kInvalidUID, DScolorUID, gapUID = kInvalidUID;
			PMReal strokeTint, fillTint, strokeWeight, cornerRadius, gapTint;						
			IStandOff::mode mode;
			IStandOff::form form;		
			PMReal topStOff = 0.0, rightStOff = 0.0,leftStOff = 0.0, bottomStOff = 0.0;
			IStandOffContourWrapSettings::ContourWrapType contourType;
			bool16 nonPrint = kFalse, locked = kFalse;
			bool16 DSmode = kFalse, BSisolationGroup = kFalse, BSknockoutGroup = kFalse;
			int8 DSblendMode = 0, BSblendMode = 0, VTmode = 0, VTcorner = 0;
			int32 strokeAlign = 0;

			InterfacePtr<IPathGeometry> itemPathGeo (itemHier, UseDefaultIID());
			if(itemPathGeo == nil)
			{
				ref = kSkippedTCLRef;
				writer->GetReverseChecker()->WriteReverseLog(PMString(kErrInconsistentItem), "", 0);
				break; //throw TCLError(error); // BUG FIX : sometimes we found kEPSTextItem on corrupted documents, which don't aggregate IPathGeometry
			}
				

			int32 textflags=0;
			if(Utils<IFrameUtils>()->IsTextFrame(itemHier, &textflags))
			{
				if(exportTextFrame)
				{
					// Get MultiColumnItem
					InterfacePtr<IHierarchy> multiColHier (itemHier->QueryChild(0));
					if(multiColHier == nil)
						throw TCLError(error);

					if((textflags & IFrameUtils::kTF_InLink) || (textflags & IFrameUtils::kTF_OutLink))  
					{
						// Iterate through frames
						InterfacePtr<IMultiColumnTextFrame> txtFrame (multiColHier,UseDefaultIID());
						if(txtFrame == nil)
							throw TCLError(error);

						InterfacePtr<IFrameList> frameList (txtFrame->QueryFrameList());
						if(frameList == nil)
							throw TCLError(error);

						int32 nbFrames = frameList->GetFrameCount();
						TCLRef refLink = kInvalidTCLRef; bool8 toLink = kFalse;
						UID latestSpline = kInvalidUID;
						for(int32 i = 0 ; i < nbFrames ; ++i)
						{
							// Get the first frame from the frame list
							InterfacePtr<ITextFrameColumn> frameItem (frameList->QueryNthFrame(i));
							if(frameItem == nil)
								throw TCLError(error);
							// Get the associated spline UIDRef
							InterfacePtr<IHierarchy> textSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(frameItem));
							if(textSpline == nil)
								throw TCLError(error);
							
							UID currentSpline = ::GetUID(textSpline);
							if(currentSpline != latestSpline)
							{
								ref = getTCLRef(currentSpline);
								ReverseLinkedTextFrames(textSpline, ref, toLink, refLink);
								alreadyReversedItem->Append(currentSpline);
								refLink = ref; toLink = kTrue;								
								latestSpline = currentSpline;
							}						
						}
						writer->writeSelectItem(ref);	
						if(exportText)
							ReverseTextFrameContent(multiColHier, ref);

						break;
					}
					else
					{
						Text::VerticalJustification vj;
						Text::FirstLineOffsetMetric topAlign;
						PMReal vjMax = 0.0, offset = 0.0, gutter = 12.0;
						int32 nbCol = 1;
						
						PMReal leftInset = 0.0, rightInset = 0.0, topInset = 0.0,  bottomInset = 0.0;

						GetTextFrameProperties(multiColHier,vj,topAlign,vjMax,offset,gutter,nbCol,leftInset,rightInset,topInset,bottomInset);												

						if(Utils<IPathUtils>()->IsRectangle(itemPathGeo))
						{
							GetFrameGeneralAttributes(itemHier,rotationAngle,xSkewAngle,xScale,yScale,leftTop,itemWidth,itemHeight,lineImpl,cornerImpl,
											strokeUID,fillUID,strokeTint,fillTint,strokeWeight, strokeAlign, cornerRadius, gapUID, gapTint, nonPrint, locked, gradientFillAngle, gradientStrokeAngle,
											gradientFillLength, gradientFillRadius, gradientFillCenter, gradientStrokeLength, gradientStrokeRadius, gradientStrokeCenter, 
											BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity,
											DSmode, DSblendMode, DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise, curPageGeo);
							writer->writeNewTextFrame(ref, leftTop, itemWidth, itemHeight, vj, offset, curPage, fillUID, fillTint,
													nbCol, gutter, rotationAngle, vjMax, topAlign, isInline, refOwner);
						}
						else
						{
							K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList;
							try
							{
								GetSplineAttributes(itemPathGeo, ptsList, rotationAngle,xSkewAngle,xScale,yScale, leftTop,itemWidth,itemHeight, lineImpl,cornerImpl,strokeUID,fillUID,strokeTint,fillTint,strokeWeight,strokeAlign,cornerRadius,
								 gapUID, gapTint, nonPrint,	locked, BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity, gradientFillAngle, gradientStrokeAngle,
								 gradientFillLength, gradientFillRadius, gradientFillCenter, gradientStrokeLength, gradientStrokeRadius, gradientStrokeCenter,  
								 DSmode, DSblendMode, DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise, curPageGeo);
							} catch(...)
							{
								ref = kSkippedTCLRef;
								writer->GetReverseChecker()->WriteReverseLog(PMString(kErrInconsistentItem), "", 0);
								break; // BUG FIX : sometimes we found invalid items (having 0 points on their path) on corrupted documents
							}

							writer->writeNewTextSpline(ref, leftTop,itemWidth,itemHeight, ptsList, vj, offset, curPage, fillUID, fillTint,nbCol, gutter, rotationAngle, vjMax, topAlign, isInline, refOwner);
						}
						GetTextWrapAttributes(itemHier, mode, form, topStOff, rightStOff, leftStOff, bottomStOff, contourType);

						writer->writeSetStroke(ref, strokeWeight, lineImpl, strokeUID, strokeTint, cornerImpl, cornerRadius, gapUID, gapTint, strokeAlign); // BC
						writer->writeSetTextWrap(ref, mode, topStOff, leftStOff, bottomStOff, rightStOff, form, contourType); // BH					
						writer->writeIsLocked(ref, locked);// BV
						writer->writeNonPrint(ref,nonPrint); // BA
						writer->writeSkewItem(ref,xSkewAngle); // BE
						writer->writeSetInsets(ref, topInset, leftInset, bottomInset, rightInset);	// BJ
						if(DSmode)
							writer->writeShadowEffect(ref,DSmode, DSblendMode, DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise); // SE
						if(BSblendMode != kPMBlendNormal || BSopacity != 100.0 || BSknockoutGroup || BSisolationGroup)
							writer->writeTransparencyEffect(ref,BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup); // OE
						if(VTmode)
							writer->writeFeatherEffect(ref, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity); // FA	
						if(gradientFillAngle != 0.0)
							writer->writeGradientOptions(ref, kTrue, gradientFillAngle, gradientFillLength, gradientFillCenter, gradientFillRadius); // GO
						if(gradientStrokeAngle != 0.0)
							writer->writeGradientOptions(ref, kFalse, gradientStrokeAngle, gradientStrokeLength, gradientStrokeCenter, gradientStrokeRadius);
						
						writer->writeFlipItem(ref, (xScale < 0), (yScale < 0)); // BU												
						
						// Export text
						writer->writeSelectItem(ref);						
						if(exportText)
							ReverseTextFrameContent(multiColHier, ref);

						// Export Text On Path if any
						ExportTOP(itemHier,ref);
					}
				}
				else
				{
					ref = kSkippedTCLRef;
					break;
				}
			}
			else 
			{
				if(Utils<IPageItemTypeUtils>()->IsGraphicFrame(itemHier))
				{
					if(exportImageFrame)
					{	
						PMString imageFileName("");
						PMReal imXScale = 100.0, imYScale = 100.0, xDec = 0.0, yDec = 0.0, tolerance = 2.0, inset = 0.0, tiffFillTint = 0.0, imageWidth = 0.0, imageHeight = 0.0;
						PMReal imageAngle = 0, imageSkew = 0;
						int16 clipType = 0;
						uint8 threshold = 25;
						int16 index = -1;
						bool16 invert = kFalse, allowHoles = kFalse, limitToFrame = kFalse, useHighRes = kTrue;
						UID tiffFillUID = kInvalidUID;

						if(itemHier->GetChildCount() == 1)
							GetImageFrameProperties(ref, itemHier, imageFileName,imXScale, imYScale, xDec, yDec,
													clipType, tolerance, inset, threshold, index, invert, 
													allowHoles, limitToFrame, useHighRes, tiffFillUID, tiffFillTint, imageWidth, imageHeight, imageAngle, imageSkew);

						if(Utils<IPathUtils>()->IsRectangle(itemPathGeo))
						{
							GetFrameGeneralAttributes(itemHier,rotationAngle,xSkewAngle,xScale,yScale,leftTop,itemWidth,itemHeight,lineImpl,cornerImpl,
												strokeUID,fillUID,strokeTint,fillTint,strokeWeight,strokeAlign,cornerRadius, gapUID, gapTint, nonPrint, locked, gradientFillAngle, gradientStrokeAngle,
												gradientFillLength, gradientFillRadius, gradientFillCenter, gradientStrokeLength, gradientStrokeRadius, gradientStrokeCenter, 
												BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity,
												DSmode, DSblendMode, DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise, curPageGeo);

							writer->writeNewImageFrame(ref, leftTop, itemWidth, itemHeight, imageFileName, xDec, yDec, imXScale * xScale, imYScale * yScale,
													curPage, fillUID, fillTint, rotationAngle, cornerImpl, cornerRadius, isInline, refOwner);
						}
						else
						{
							K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList;
							try
							{
								GetSplineAttributes(itemPathGeo, ptsList, rotationAngle,xSkewAngle,xScale,yScale, leftTop,itemWidth,itemHeight, lineImpl,cornerImpl,
									strokeUID,fillUID,strokeTint,fillTint,strokeWeight,strokeAlign,cornerRadius, gapUID, gapTint, nonPrint, locked,BSblendMode, BSopacity, BSknockoutGroup, 
									BSisolationGroup, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity, gradientFillAngle, gradientStrokeAngle, 
									gradientFillLength, gradientFillRadius, gradientFillCenter, gradientStrokeLength, gradientStrokeRadius, gradientStrokeCenter, DSmode, DSblendMode,
									DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise, curPageGeo);
							} catch(...)
							{
								ref = kSkippedTCLRef;
								writer->GetReverseChecker()->WriteReverseLog(PMString(kErrInconsistentItem), "", 0);
								break; // BUG FIX : sometimes we found invalid items (having 0 points on their path) on corrupted documents
							}
							writer->writeNewGraphicSpline(ref, leftTop,itemWidth,itemHeight, ptsList,  imageFileName, xDec, yDec, imXScale * xScale, imYScale * yScale,
													curPage, fillUID, fillTint, rotationAngle, cornerImpl, cornerRadius, isInline, refOwner);
						}

						// Text wrap settings
						if(Utils<IPageItemTypeUtils>()->HasStandOff(itemHier))
							GetTextWrapAttributes(itemHier, mode, form, topStOff, rightStOff, leftStOff, bottomStOff, contourType);	
						else
						{						
							if(itemHier->GetChildCount() == 1)
							{
								InterfacePtr<IHierarchy> imageHier (itemHier->QueryChild(0));																											
								GetTextWrapAttributes(imageHier, mode, form, topStOff, rightStOff, leftStOff, bottomStOff, contourType);			
							}
							else
								GetTextWrapAttributes(itemHier, mode, form, topStOff, rightStOff, leftStOff, bottomStOff, contourType);
						}

#if REVERSE_CHECKER
						writer->writeImageInfo(ref, imageWidth, imageHeight);
#endif
						writer->writeSetStroke(ref, strokeWeight, lineImpl, strokeUID, strokeTint, cornerImpl, cornerRadius, gapUID, gapTint, strokeAlign); // BC
						writer->writeSetTextWrap(ref, mode, topStOff, leftStOff, bottomStOff, rightStOff, form, contourType); // BH
						writer->writeIsLocked(ref, locked);// BV
						writer->writeNonPrint(ref,nonPrint); // BA
						writer->writeSkewItem(ref,xSkewAngle); // BE
						if(DSmode)
							writer->writeShadowEffect(ref,DSmode, DSblendMode, DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise); // SE
						if(BSblendMode != kPMBlendNormal || BSopacity != 100.0 || BSknockoutGroup || BSisolationGroup)
							writer->writeTransparencyEffect(ref,BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup); // OE
						if(VTmode)
							writer->writeFeatherEffect(ref, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity); // FA	
						if(clipType != 0)
							writer->writeClippingPath(ref,clipType,tolerance,inset,threshold,index,invert,allowHoles,limitToFrame,useHighRes);
						if(gradientFillAngle != 0.0)
							writer->writeGradientOptions(ref, kTrue, gradientFillAngle, gradientFillLength, gradientFillCenter, gradientFillRadius); // GO
						if(gradientStrokeAngle != 0.0)
							writer->writeGradientOptions(ref, kFalse, gradientStrokeAngle, gradientStrokeLength, gradientStrokeCenter, gradientStrokeRadius);

						writer->writeImageProperties(ref, imageAngle, imageSkew, xDec, yDec); // BP
						writer->writeFlipItem(ref, (xScale < 0), (yScale < 0)); // BU		

						if(tiffFillUID != kInvalidUID)
							writer->writeTiffOptions(ref, tiffFillUID, tiffFillTint);
					}
					else
					{
						ref = kSkippedTCLRef;
						break;
					}
				}
				else
				{
					if(Utils<IPathUtils>()->IsLine(itemPathGeo))
					{
						if(exportRule)
						{
							PMPoint p1, p2;
							ClassID lineEndStartImpl, lineEndEndImpl;
							GetRuleAttributes(itemPathGeo, p1, p2, lineImpl, strokeWeight, strokeAlign, strokeUID, strokeTint, lineEndStartImpl, lineEndEndImpl, nonPrint, curPageGeo);
							
							writer->writeNewRule(ref, p1, p2, strokeWeight, strokeAlign, lineImpl, strokeUID, strokeTint, curPage, lineEndStartImpl, lineEndEndImpl);
							writer->writeNonPrint(ref, nonPrint);

							// Text wrap settings
							if(Utils<IPageItemTypeUtils>()->HasStandOff(itemHier))
							{				
								GetTextWrapAttributes(itemHier, mode, form, topStOff, rightStOff, leftStOff, bottomStOff, contourType);
								writer->writeSetTextWrap(ref, mode, topStOff, leftStOff, bottomStOff, rightStOff, form, contourType);	
							}
						}
						else
						{
							ref = kSkippedTCLRef;
							break;
						}
					}
					else
					{
						if(Utils<IPathUtils>()->IsRectangle(itemPathGeo))
						{
							GetFrameGeneralAttributes(itemHier,rotationAngle,xSkewAngle,xScale,yScale,leftTop,itemWidth,itemHeight,lineImpl,cornerImpl,
												strokeUID,fillUID,strokeTint,fillTint,strokeWeight,strokeAlign,cornerRadius, gapUID, gapTint, nonPrint, locked, gradientFillAngle, gradientStrokeAngle,
												gradientFillLength, gradientFillRadius, gradientFillCenter, gradientStrokeLength, gradientStrokeRadius, gradientStrokeCenter, 
												BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity,
												DSmode, DSblendMode, DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise, curPageGeo);

							writer->writeNewImageFrame(ref, leftTop, itemWidth, itemHeight, "", 0, 0, 100, 100,
													curPage, fillUID, fillTint, rotationAngle, cornerImpl, cornerRadius, isInline, refOwner);
						}
						else
						{
							K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList;
							try
							{
								GetSplineAttributes(itemPathGeo, ptsList, rotationAngle,xSkewAngle,xScale,yScale, leftTop,itemWidth,itemHeight, lineImpl,cornerImpl,
									strokeUID,fillUID,strokeTint,fillTint,strokeWeight,strokeAlign,cornerRadius, gapUID, gapTint, nonPrint, locked, BSblendMode, BSopacity, BSknockoutGroup, 
									BSisolationGroup, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity, gradientFillAngle, gradientStrokeAngle, 
									gradientFillLength, gradientFillRadius, gradientFillCenter, gradientStrokeLength, gradientStrokeRadius, gradientStrokeCenter, 
									DSmode, DSblendMode, DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise, curPageGeo);
							} catch(...)
							{
								ref = kSkippedTCLRef;
								writer->GetReverseChecker()->WriteReverseLog(PMString(kErrInconsistentItem), "", 0);
								break; // BUG FIX : sometimes we found invalid items (having 0 points on their path) on corrupted documents
							}
							writer->writeNewGraphicSpline(ref, leftTop,itemWidth,itemHeight, ptsList,  "", 0, 0, 100, 100,
													curPage, fillUID, fillTint, rotationAngle, cornerImpl, cornerRadius, isInline, refOwner);
						}

						GetTextWrapAttributes(itemHier, mode, form, topStOff, rightStOff, leftStOff, bottomStOff, contourType);
							
						writer->writeSetStroke(ref, strokeWeight, lineImpl, strokeUID, strokeTint, cornerImpl, cornerRadius, gapUID, gapTint, strokeAlign); // BC
						writer->writeSetTextWrap(ref, mode, topStOff, leftStOff, bottomStOff, rightStOff, form, contourType); // BH
						writer->writeIsLocked(ref, locked);// BV
						writer->writeNonPrint(ref,nonPrint); // BA
						writer->writeSkewItem(ref,xSkewAngle); // BE
						if(DSmode)
							writer->writeShadowEffect(ref,DSmode, DSblendMode, DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise); // SE
						if(BSblendMode != kPMBlendNormal || BSopacity != 100.0 || BSknockoutGroup || BSisolationGroup)
							writer->writeTransparencyEffect(ref,BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup); // OE
						if(VTmode)
							writer->writeFeatherEffect(ref, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity); // FA		
						if(gradientFillAngle != 0.0)
							writer->writeGradientOptions(ref, kTrue, gradientFillAngle, gradientFillLength, gradientFillCenter, gradientFillRadius); // GO
						if(gradientStrokeAngle != 0.0)
							writer->writeGradientOptions(ref, kFalse, gradientStrokeAngle, gradientStrokeLength, gradientStrokeCenter, gradientStrokeRadius);
						writer->writeFlipItem(ref, (xScale < 0), (yScale < 0)); // BU				
					}
				}

				// Export Text On Path if any
				ExportTOP(itemHier,ref);
			}							
			
		} while(kFalse);
	}
	return ref;
}


void PrsReverseTCL::ReverseLinkedTextFrames(IHierarchy * itemHier, TCLRef ref, bool8 toLink,TCLRef refLink)
{
	PMString error = PMString(kErrNilInterface);

	PMReal rotationAngle = 0.0, xSkewAngle = 0.0, xScale,yScale, itemWidth, itemHeight;
	PMReal DSxOffset = 0.0, DSyOffset = 0.0, DSopacity = 75.0, DSradius = 0.0, DSspread = 0.0, DSnoise = 0.0;
	PMReal BSopacity = 100.0, VTwidth = 12.0, VTnoise = 0.0, VTinnerOpacity = 100.0, VTouterOpacity = 0.0;
	PMReal gradientFillAngle = 0.0, gradientStrokeAngle = 0.0, gradientFillLength = 0.0, gradientStrokeLength = 0.0, gradientFillRadius = 0.0, gradientStrokeRadius = 0.0;
	PMPoint leftTop(0.0,0.0), gradientFillCenter(0.0,0.0), gradientStrokeCenter(0.0,0.0);
	ClassID lineImpl, cornerImpl = kInvalidClass;
	UID strokeUID, fillUID, dummyUID = kInvalidUID, DScolorUID, gapUID = kInvalidUID;
	PMReal strokeTint, fillTint, strokeWeight, cornerRadius, gapTint;
	bool16 nonPrint = kFalse, locked = kFalse;
	bool16 DSmode = kFalse, BSisolationGroup = kFalse, BSknockoutGroup = kFalse;
	int8 DSblendMode = 0, BSblendMode = 0, VTmode = 0, VTcorner = 0;	 
	int32 strokeAlign = 0;
	IStandOff::mode mode;
	IStandOff::form form;		
	PMReal topStOff = 0.0, rightStOff = 0.0,leftStOff = 0.0, bottomStOff = 0.0;
	IStandOffContourWrapSettings::ContourWrapType contourType;

	UID ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);
	int32 ownerPage = pageList->GetPageIndex(ownerPageUID) + 1;
	if(ownerPage == 0) // frame is on a master spread
		ownerPage = masterSpreadGlobal->GetPageIndex(ownerPageUID) + 1;
	
	InterfacePtr<IGeometry> ownerPageGeo (db, ownerPageUID, UseDefaultIID());
	if(ownerPageGeo == nil)
		throw TCLError(error);

	Text::VerticalJustification vj;
	Text::FirstLineOffsetMetric topAlign;
	PMReal vjMax = 0.0, offset = 0.0, gutter = 12.0;
	int32 nbCol = 1;
	
	PMReal leftInset = 0.0, rightInset = 0.0, topInset = 0.0,  bottomInset = 0.0;
	
	InterfacePtr<IHierarchy> multiColHier (itemHier->QueryChild(0));
	if(multiColHier == nil)
		throw TCLError(error);

	GetTextFrameProperties(multiColHier,vj,topAlign,vjMax,offset,gutter,nbCol,leftInset,rightInset,topInset,bottomInset);

	InterfacePtr<IPathGeometry> itemPathGeo (itemHier, UseDefaultIID());
	if(Utils<IPathUtils>()->IsRectangle(itemPathGeo))
	{
		GetFrameGeneralAttributes(itemHier,rotationAngle,xSkewAngle,xScale,yScale,leftTop,itemWidth,itemHeight,lineImpl,cornerImpl,
						strokeUID,fillUID,strokeTint,fillTint,strokeWeight,strokeAlign,cornerRadius, gapUID, gapTint, nonPrint, locked, gradientFillAngle, gradientStrokeAngle, 
						gradientFillLength, gradientFillRadius, gradientFillCenter, gradientStrokeLength, gradientStrokeRadius, gradientStrokeCenter, 
						BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity,
						DSmode, DSblendMode, DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise, ownerPageGeo);
		writer->writeNewTextFrame(ref, leftTop, itemWidth, itemHeight, vj, offset, ownerPage, fillUID, fillTint,
								nbCol, gutter, rotationAngle, vjMax, topAlign, kFalse,kInvalidTCLRef, toLink, refLink);
	}
	else
	{
		K2Vector<KeyValuePair<PMPathPointList, bool16> > ptsList;
		GetSplineAttributes(itemPathGeo, ptsList, rotationAngle,xSkewAngle,xScale,yScale, leftTop,itemWidth,itemHeight, lineImpl,cornerImpl,
			strokeUID,fillUID,strokeTint,fillTint,strokeWeight,strokeAlign,cornerRadius, gapUID, gapTint, nonPrint, locked, BSblendMode, BSopacity, BSknockoutGroup, 
			BSisolationGroup, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity, gradientFillAngle, gradientStrokeAngle, 
			gradientFillLength, gradientFillRadius, gradientFillCenter, gradientStrokeLength, gradientStrokeRadius, gradientStrokeCenter, DSmode, DSblendMode,
			DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise, ownerPageGeo);

		writer->writeNewTextSpline(ref, leftTop,itemWidth,itemHeight, ptsList, vj, offset, ownerPage, fillUID, fillTint,
									nbCol, gutter, rotationAngle, vjMax, topAlign, kFalse,kInvalidTCLRef, toLink, refLink);
	}

	GetTextWrapAttributes(itemHier, mode, form, topStOff, rightStOff, leftStOff, bottomStOff, contourType);
	
	writer->writeSetStroke(ref, strokeWeight, lineImpl, strokeUID, strokeTint, cornerImpl, cornerRadius, gapUID, gapTint, strokeAlign); // BC
	writer->writeSetTextWrap(ref, mode, topStOff, leftStOff, bottomStOff, rightStOff, form, contourType); // BH					
	writer->writeIsLocked(ref, locked);// BV
	writer->writeNonPrint(ref,nonPrint); // BA
	writer->writeSkewItem(ref,xSkewAngle); // BE
	writer->writeSetInsets(ref, topInset, leftInset, bottomInset, rightInset);	// BJ
	if(DSmode)
		writer->writeShadowEffect(ref,DSmode, DSblendMode, DSxOffset, DSyOffset, DScolorUID, DSopacity, DSradius, DSspread, DSnoise); // SE
	if(BSblendMode != kPMBlendNormal || BSopacity != 100.0 || BSknockoutGroup || BSisolationGroup)
		writer->writeTransparencyEffect(ref,BSblendMode, BSopacity, BSknockoutGroup, BSisolationGroup); // OE
	if(VTmode)
		writer->writeFeatherEffect(ref, VTmode, VTwidth,VTcorner, VTnoise, VTinnerOpacity, VTouterOpacity); // FA	
	if(gradientFillAngle != 0.0)
		writer->writeGradientOptions(ref, kTrue, gradientFillAngle, gradientFillLength, gradientFillCenter, gradientFillRadius); // GO
	if(gradientStrokeAngle != 0.0)
		writer->writeGradientOptions(ref, kFalse, gradientStrokeAngle, gradientStrokeLength, gradientStrokeCenter, gradientStrokeRadius);
	writer->writeFlipItem(ref, (xScale < 0), (yScale < 0)); // BU

	ExportTOP(itemHier,ref);
}

void PrsReverseTCL::ReverseInline(UID inlineUID, TCLRef refOwner)
{
	PMString error = PMString(kErrNilInterface);
	
	InterfacePtr<IHierarchy> inlineItemHier (db, inlineUID, UseDefaultIID());
	if(inlineItemHier == nil)
		throw TCLError(error);

	// Get the associated frame
	InterfacePtr<IHierarchy> frameItem (inlineItemHier->QueryChild(0));
	if(frameItem == nil)
		throw TCLError(error);

	UID frameUID = ::GetUID(frameItem);

	TCLRef refInline = ReversePageItem(UIDRef(db, frameUID), nil, 0, kTrue, refOwner);

	if(refInline != kInvalidTCLRef)
	{
		// Get Inline properties
		InterfacePtr<IAnchoredObjectData> inlineData (inlineItemHier, UseDefaultIID());
		if(inlineData == nil)
			throw TCLError(error);
		
		IAnchoredObjectData::AnchoredPosition pos = inlineData->GetPosition(); 
		if(pos == IAnchoredObjectData::kAnchoredObject)
		{
			error = PMString(kErrReverseInline);
			throw TCLError(error);
		}
		
		PMReal offset = inlineData->GetOffset().Y();

		writer->writeSetInlineProperties(refInline,pos,offset);
	}

	writer->writeSelectItem(refOwner);
}

void PrsReverseTCL::ReverseTextFrameContent(IHierarchy * multiColHier, TCLRef ref)
{
	PMString error = PMString(kErrNilInterface);

	InterfacePtr<IMultiColumnTextFrame> txtFrame (multiColHier, UseDefaultIID());
	if(txtFrame == nil)
		throw TCLError(error);

	InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());
	if(txtModel == nil)
		throw TCLError(error);

	// Check whether this is an automatically generated story (a table of content or an index)
	InterfacePtr<IDataLinkReference> pDataLink(txtModel, IID_IDATALINKREFERENCE );

	// if the IDataLinkReference exists on the story, get the UID of the kGenericDataLinkBoss
	UID datalinkUID = pDataLink->GetUID();

	// Try to get the pointer to the implementation of IID_ITOCSTYLEPERSISTUIDDATA which is of interface type IPersistUIDData
	InterfacePtr<IPersistUIDData> pTOCStyleUIDData(::GetDataBase(txtFrame), datalinkUID, IID_ITOCSTYLEPERSISTUIDDATA);
	if(pTOCStyleUIDData && (pTOCStyleUIDData->GetUID() != kInvalidUID))
	{
		// Look for TOC style name
		InterfacePtr<IDocument> doc (db, db->GetRootUID(), UseDefaultIID());
		InterfacePtr<ITOCStyleNameTable> TOCStyleList (doc->GetDocWorkSpace(), IID_ITOCSTYLENAMETABLE);

		int32 styleCount = TOCStyleList->GetNumStyles();
		for(int32 i = 0 ; i < styleCount ; ++i)
		{
			UID styleUID = TOCStyleList->GetNthStyle(i);
			if(styleUID == pTOCStyleUIDData->GetUID())
			{
				InterfacePtr<ITOCStyleInfo> iStyleInfo(db, styleUID, IID_ITOCSTYLEINFO);
				// Style has been found, but we don't write MT command now, it will be appended at the end of the generated TCL file
				tocStyleName = iStyleInfo->GetName();
				tocFrameRef = ref;

				break;
			}
		}	
	}
	else
	{
	
		// Get text data, character and paragraph strands
		InterfacePtr<IAttributeStrand> charStrand (static_cast<IAttributeStrand* >(txtModel->QueryStrand(kCharAttrStrandBoss, IID_IATTRIBUTESTRAND)));
		if(charStrand == nil)
			throw TCLError(error);

		InterfacePtr<IAttributeStrand> paraStrand (static_cast<IAttributeStrand* >(txtModel->QueryStrand(kParaAttrStrandBoss, IID_IATTRIBUTESTRAND)));
		if(paraStrand == nil)
			throw TCLError(error);

		InterfacePtr<ITextStrand> txtStrand (static_cast<ITextStrand* >(txtModel->QueryStrand(kTextDataStrandBoss, IID_ITEXTSTRAND)));
		if(txtStrand == nil)
			throw TCLError(error);

		InterfacePtr<IItemStrand> itemStrand (static_cast<IItemStrand* >(txtModel->QueryStrand(kOwnedItemStrandBoss, IID_IITEMSTRAND)));
		if(itemStrand == nil)
			throw TCLError(error);

		TextIndex index = 0;
		UID currentParaStyle = kInvalidUID, currentCharStyle = kInvalidUID;
		if(!exportStyleCmd)
		{
			writer->setCurrentCharStyle(defaultCharStyle);
			writer->setCurrentParaStyle(defaultParaStyle);
		}

		while(index < txtModel->GetPrimaryStoryThreadSpan())
		{
			// Get a "chunk" of text (text can be divided in several chunk)
			int32 length;
			DataWrapper<textchar> chunk = txtStrand->FindChunk(index,&length);

			int32 indexChunk = 0;
			int32 endPara = 0;
			int32 endCharRun = 0;
			while(indexChunk < length)
			{
				if(indexChunk == endCharRun || indexChunk == endPara)
				{
					int32 countCharRun,countCharPara;
					UID charStyleApplied = charStrand->GetStyleUID(index + indexChunk,&countCharRun);
					UID paraStyleApplied = paraStrand->GetStyleUID(index + indexChunk,&countCharPara);

					DataWrapper<AttributeBossList> localParaOverrides = paraStrand->GetLocalOverrides(index + indexChunk,nil);
					DataWrapper<AttributeBossList> localCharOverrides = charStrand->GetLocalOverrides(index + indexChunk,nil);

					PMString charStyleName = "", paraStyleName = "";
					if(exportStyleCmd)
					{
						if(paraStyleApplied != currentParaStyle)
						{
							currentParaStyle = paraStyleApplied;
							
							int32 location = ::FindLocation(styleNames, currentParaStyle);
							if(location != -1)	
								paraStyleName  = styleNames[location].Value();	
							else // Should not occur
							{
								paraStyleName = "NormalParagraphStyle";								
							}
							
							if(currentParaStyle == rootParaStyle)
								writer->setCurrentParaStyle(defaultParaStyle);
							else
								writer->setCurrentParaStyle(currentParaStyle);
						}

						if(charStyleApplied != currentCharStyle)
						{
							currentCharStyle = charStyleApplied;
							int32 location = ::FindLocation(styleNames, currentCharStyle);
							if(location != -1)
								charStyleName  = styleNames[location].Value();
							else // must be root style
								charStyleName = "[None]";

							writer->setCurrentCharStyle(currentCharStyle);
						}
					}

					writer->writeApplyStylesOrOverrides(localParaOverrides, localCharOverrides, paraStyleName, charStyleName);

					endPara = indexChunk + countCharPara;
					endCharRun = indexChunk + countCharRun;
				}

				bool16 inlineFound = kFalse, zeroBreakFound = kFalse;
				writer->writeChar(chunk[indexChunk], inlineFound, zeroBreakFound);

				if(inlineFound)
				{					
					UID inlineItem = itemStrand->GetOwnedUID(index + indexChunk, kInlineBoss);
					if(inlineItem != kInvalidUID)
						ReverseInline(inlineItem, ref);
				}
#if HYPERLINKS
				else if(zeroBreakFound)
				{
					ReverseHyperlinkDest(itemStrand, index + indexChunk);			
				}
				else
				{

				}
#endif
				indexChunk++;
			}
			index += length;		
		}
		writer->EndText();
	}
}

void PrsReverseTCL::ReverseHyperlinkDest(IItemStrand * itemStrand, TextIndex index)
{
	// Try to find out if the owned item is an hyperlink text destination marker
	UID hlItem = itemStrand->GetOwnedUID(index,  kHyperlinkTextDestinationMarkerBoss);
						
	if(hlItem != kInvalidUID)
	{
		InterfacePtr<IHyperlinkMarker> marker (db, hlItem, UseDefaultIID());
		if(marker->GetHyperlinkDestinationUID() != kInvalidUID)
		{
			InterfacePtr<IHyperlinkDestination> txtDest (db, marker->GetHyperlinkDestinationUID(), UseDefaultIID());
			PMString hlName = kNullString;
			txtDest->GetName(&hlName);
			writer->writeHyperlinkTextDest(hlName, !txtDest->IsHidden());
		}
	}
}

void PrsReverseTCL::ReverseAllHyperlinkSource(IDocument * doc)
{
	InterfacePtr<IHyperlinkTable> hlTable (doc, UseDefaultIID());
	InterfacePtr<IPageList> pageList (doc, UseDefaultIID());

	// Handle page destination hyperlink
	// GD 29.07.2013 ++
	// Portage CS2 > CS6
	//int32 hlDestCount = hlTable->GetHyperlinkDestinationCount();
	//for(int32 i = 0 ; i < hlDestCount ; ++i)
	//{
	//	InterfacePtr<IHyperlinkDestination> hlDest (db, hlTable->GetNthHyperlink (i), UseDefaultIID());
	UniqueKeyToUIDMap::const_iterator first = hlTable->GetDestinationKeyToUIDMapBeginIter();
	UniqueKeyToUIDMap::const_iterator last = hlTable->GetDestinationKeyToUIDMapEndIter();
	for(; first != last; ++first)
	{
		UID destUID = first->second;
		InterfacePtr<IHyperlinkDestination> hlDest (db, destUID, UseDefaultIID());
	// GD 29.07.2013 --
		
		if(::GetClass(hlDest) == kHyperlinkTextDestinationBoss || ::GetClass(hlDest) == kHyperlinkExternalPageDestinationBoss)
			continue;

		PMString hlName = kNullString;
		hlDest->GetName(&hlName);

		if(hlDest->IsHidden()) // Don't reverse invisible hyperlink, they must have been generated automatically before
			continue;
		
		// Check whether it's page item destination or a URL destination
		InterfacePtr<IHyperlinkPageDestinationData> pageDestData (hlDest, IID_IHYPERLINKPAGEDESTINATIONDATA );
		if(pageDestData)
		{
			int32 pageNumber = pageList->GetPageIndex(pageDestData->GetPageUID()) + 1;
			writer->writeHyperlinkPageDest(hlName, pageNumber, !hlDest->IsHidden());
		}
		else
		{
			InterfacePtr<IStringData> urlData (hlDest, UseDefaultIID());
			PMString url = urlData->Get();
			writer->writeHyperlinkURLDest(hlName, url);
		}
	}

	// Handle hyperlinks source
	int32 hlSrcCount = hlTable->GetHyperlinkSourceCount();
	TCLRef curTCLRef = kInvalidTCLRef;
	for(int32 i = 0 ; i < hlSrcCount ; ++i)
	{
		InterfacePtr<IHyperlinkSource> hlSrc (db, hlTable->GetNthHyperlinkSource(i), UseDefaultIID());

		InterfacePtr<IHyperlink> owningHL (db, hlSrc->GetOwningHyperlink(), UseDefaultIID());
		if(!owningHL)
			continue;

		PMString hlName = kNullString;
		owningHL->GetName(&hlName);
		
		if(owningHL->IsHidden()) // Don't reverse invisible hyperlink, they must have been generated automatically before
			continue;

		InterfacePtr<IHyperlinkDestination> hlDest (db, owningHL->GetDestinationUID(), UseDefaultIID());
		if(!hlDest) // link with no destination defined are not reversed
		{
			writer->GetReverseChecker()->WriteReverseLog(PMString(kErrLinkWithNoDest), "", 0);
			continue;
		}

		PMString hlDestName = kNullString;
		hlDest->GetName(&hlDestName);		

		// Check whether it's page item source or a text source
		int8 type = 1;
		TCLRef splineRef = kInvalidTCLRef;
		TextIndex start = kInvalidTextIndex, end = kInvalidTextIndex;
		InterfacePtr<IHyperlinkPageItemSourceData> pageItemSrcData (hlSrc, UseDefaultIID());
		if(pageItemSrcData)
		{
			splineRef = getTCLRef(pageItemSrcData->GetSourceUID()); // at that point, the spline must have been assigned a reference
		}
		else
		{
			type = 2;
			InterfacePtr<IHyperlinkTextSourceData> txtSrcData (hlSrc, UseDefaultIID());
			
			// GD 29.07.2013 ++
			// Portage CS2 > CS6
			
			//InterfacePtr<IOwnedItem> marker (db, txtSrcData->GetOwningStoryUID(), UseDefaultIID());
			//InterfacePtr<IOwnedItem> startMarker (db, txtSrcData->GetStartMarkerUID(), UseDefaultIID());
			//InterfacePtr<IOwnedItem> endMarker   (db, txtSrcData->GetEndMarkerUID()  , UseDefaultIID());
			//start = startMarker->GetTextIndex();
			//end = endMarker->GetTextIndex();		
			//Utils<IHyperlinkUtils>()->GetRangeContainedHyperlinks(marker->QueryTextModel(), start, end );
			// Look for text spline UID
			//InterfacePtr<ITextFrame> owningTxtFrame (startMarker->QueryFrame());
			//InterfacePtr<ITextFrameColumn> owningTxtFrame (marker->QueryFrame());
			//InterfacePtr<IHierarchy> splineItem (Utils<ITextUtils>()->QuerySplineFromTextFrame(owningTxtFrame));
			//splineRef = getTCLRef(::GetUID(splineItem));
			
			TextRangeReference textRangeRef = txtSrcData->GetHyperlinkRangeReference();
			InterfacePtr<ITextRangeNode> iTextRangeNode (textRangeRef.Instantiate());
			start = iTextRangeNode->GetStartIndex();
			end = iTextRangeNode->GetRangeLength() + start;
			
			UID storyUID = txtSrcData->GetOwningStoryUID();
			UIDRef storyUIDRef = UIDRef(db, storyUID);
			InterfacePtr<ITextModel> txtModel (storyUIDRef, UseDefaultIID());
			InterfacePtr<IFrameList> frameList (txtModel->QueryFrameList());
			InterfacePtr<ITextFrameColumn> txtFrame = InterfacePtr<ITextFrameColumn> (frameList->QueryNthFrame(0));
			InterfacePtr<IHierarchy> splineItem (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
			splineRef = getTCLRef(::GetUID(splineItem));
			// GD 29.07.2013 --
		}

		if(splineRef != curTCLRef)
		{
			writer->writeSelectItem(splineRef);
			curTCLRef = splineRef;
		}
		
		InterfacePtr<IHyperlinkAppearanceData> owningHLData (owningHL, IID_IHYPERLINKAPPEARANCEDATA );		

		writer->writeHyperlinkSource(type, hlName, hlDestName, Utils<IUIColorUtils>()->GetUIColorIndex(owningHLData->GetBorderColorUID(), doc),													     
													owningHLData->GetHyperlinkHilight(),owningHLData->GetHyperlinkOutlineLineStyle(), owningHLData->GetBorderLineWidth(),
													owningHLData->GetVisible(), start, end);	
	}

	// Handle bookmarks
	int32 bookMarkCount =  hlTable->GetBookmarkCount();
	for(int32 i = 0 ; i < bookMarkCount ; ++i)
	{
		InterfacePtr<IBookMarkData> bkData (db, hlTable->GetNthBookmark(i), UseDefaultIID());		
		PMString bkName = bkData->GetName();
	
		PMString bkParent = kNullString;
		UID bkParentUID = bkData->GetContainingBookMarkUID();
		if(bkParentUID != db->GetRootUID())
		{
			InterfacePtr<IBookMarkData> bkParentData (db, hlTable->GetNthBookmark(hlTable->GetBookmarkWithUID(bkParentUID)), UseDefaultIID());	
			bkParent = bkParentData->GetName();
		}	

		InterfacePtr<IHyperlinkDestination> hlDest (db, bkData->GetDestinationUID(), UseDefaultIID());
		if(!hlDest) // link with no destination defined are not reversed
		{
			writer->GetReverseChecker()->WriteReverseLog(PMString(kErrLinkWithNoDest), "", 0);
			continue;
		}

		if(hlDest->IsHidden()) // Don't reverse bookmarhs having invisible hyperlink destinations, they must have been generated automatically before
			continue;
		
		PMString hlDestName = kNullString;
		hlDest->GetName(&hlDestName);		
		
		writer->writeBookmark(bkName, bkParent, hlDestName);
	}
	
}

void PrsReverseTCL::DisplayReverseCheckerLog()
{

	IDFile logFile = writer->GetReverseChecker()->GetReverseLogFile();
	if(FileUtils::DoesFileExist(logFile))
	{
#ifdef MACINTOSH
		InterfacePtr<IScriptManager> appleScriptMgr (Utils<IScriptUtils>()->QueryScriptManager(kAppleScriptMgrBoss));
		InterfacePtr<IScriptRunner> appleScriptRunner(appleScriptMgr, UseDefaultIID());

		PMString openFileScript = "tell application \"TextEdit\"\ractivate\ropen file \"";
		openFileScript += FileUtils::SysFileToPMString(logFile);
		openFileScript += "\"\rend tell";
		
		//if(appleScriptRunner->RunScript(openFileScript, IScriptRunner::) != kSuccess)
		//	CAlert::InformationAlert(kErrFailedOpenTextEdit);
#elif REVERSE_CHECKER && !SERVER
		PMString appl = "notepad.exe";
		PMString op = "open";
		PMString logFileStr = FileUtils::SysFileToPMString(logFile);
		::ShellExecute(NULL, op.GrabTString(), appl.GrabTString(), logFileStr.GrabTString(), nil, SW_SHOWNORMAL);
#endif
	}
}

void PrsReverseTCL::SaveItemList()
{
	InterfacePtr<ITCLReferencesList> docTCLRefs (db, db->GetRootUID(),UseDefaultIID());
	
	// First save item list with newly referenced items
	InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
	saveItemsCmd->SetUndoability(ICommand::kAutoUndo);
	saveItemsCmd->SetItemList(UIDList(docTCLRefs));

	InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());
	cmdData->SetItemList(docItemList);
	cmdData->SetStoryList(docTCLRefs->GetStoryList());
	cmdData->SetTableList(docTCLRefs->GetTableList());
	
	CmdUtils::ProcessCommand(saveItemsCmd);
	
	// Then assign each item an adornment, if it has not yet
	UIDList items(db);
	for(RefVector::iterator iter = docItemList.begin(); iter < docItemList.end() ; iter++)
	{
		InterfacePtr<IPageItemAdornmentList> adornmentList (db, iter->Value(), UseDefaultIID());
		if(adornmentList && !adornmentList->HasAdornment(kPrsTCLAdornmentBoss))
			items.Append(iter->Value());
		
		// Set TCL Ref data stored in item itself
		InterfacePtr<ITCLRefData> tclRefData (db, iter->Value(), IID_ITCLREFDATA);
		if(tclRefData)
		{
			InterfacePtr<ICommand> setTCLRefCmd (CmdUtils::CreateCommand(kPrsSetTCLRefDataCmdBoss));
			setTCLRefCmd->SetUndoability(ICommand::kAutoUndo);
			setTCLRefCmd->SetItemList(UIDList(tclRefData));
			
			InterfacePtr<ITCLRefData> setTCLRefCmdData (setTCLRefCmd, IID_ITCLREFDATA);
			setTCLRefCmdData->Set(iter->Key());
			CmdUtils::ProcessCommand(setTCLRefCmd);	
		}	
	}

	InterfacePtr<ICommand>  addPageItemAdornmentCmd(CmdUtils::CreateCommand(kAddPageItemAdornmentCmdBoss));
	addPageItemAdornmentCmd->SetUndoability(ICommand::kAutoUndo);
	addPageItemAdornmentCmd->SetItemList(items);
		
	InterfacePtr<IClassIDData>  classIDData(addPageItemAdornmentCmd, UseDefaultIID());
	classIDData->Set(kPrsTCLAdornmentBoss);
	
	CmdUtils::ProcessCommand(addPageItemAdornmentCmd);
}

void PrsReverseTCL::ExportTOP(IHierarchy * itemHier, TCLRef ref)
{
	// Check whether the item has a TOP or not
	InterfacePtr<IMainItemTOPData> topData (itemHier, UseDefaultIID());
	
	UID topSplineItem = topData->GetTOPSplineItemUID();
	if(topSplineItem != kInvalidUID)
	{
		// Get text start and end positions
		PMReal txtStart = 0, txtEnd = 0;
		topData->GetAbscissae(&txtStart, &txtEnd);				

		// Get TOP options
		InterfacePtr<ICommand> getTopOptionsCmd (CmdUtils::CreateCommand(kPickUpTOPAttributesCmdBoss));
		getTopOptionsCmd->SetItemList(UIDList(itemHier));
		CmdUtils::ProcessCommand(getTopOptionsCmd);

		InterfacePtr<ITOPOptionsCmdData> topOptions (getTopOptionsCmd, UseDefaultIID());

		writer->writeTOP(ref, txtStart, txtEnd, topOptions->GetTextAlign(), topOptions->GetPathAlign(), 
						 topOptions->GetTypeEffect(), topOptions->GetOverlapOffset(), topOptions->GetFlip());

		if(exportText)
		{
			writer->writeSelectItem(ref, kTrue);	

			InterfacePtr<IHierarchy> topMultiColHier (db, topData->GetTOPMCTextFrameUID(), UseDefaultIID());
			ReverseTextFrameContent(topMultiColHier, ref);
		}
	}
}