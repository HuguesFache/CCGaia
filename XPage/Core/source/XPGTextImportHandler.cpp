

#include "VCPlugInHeaders.h"
// Interface includes
#include "IIDXMLElement.h"
#include "IPMUnknownData.h"
#include "ISAXAttributes.h"
#include "ISAXDOMSerializerServices.h"
#include "ITextModel.h"
#include "ITextModelCmds.h"
#include "IXMLImporter.h"
#include "IXMLImportOptionsPool.h"
#include "IXMLServicesError.h"
#include "IXMLTagList.h"
#include "IXMLUtils.h"
#include "IIDXMLDOMNode.h"
#include "IImportXMLData.h"
#include "IXPGTagToStyleMap.h"
#include "ICommand.h"
#include "IStyleNameTable.h"
#include "IDocument.h"
#include "IXPGPreferences.h"
#include "IFontMgr.h"
#include "IAttrReport.h"
#include "IFontFamily.h"
#include "ITextAttrUID.h"
#include "ITextAttrFont.h"
#include "IComposeScanner.h"
#include "IAttributeStrand.h"
#include "IStyleGroupManager.h"
#include "IUIDData.h"
#if !XPAGESERVER
#include "ILayoutUIUtils.h"
#else
#include "IApplication.h"
#include "IDocumentList.h"
#endif 
#include "IStyleInfo.h"
#include "IStyleGroupHierarchy.h"
#include "IXPageUtils.h"

// General includes
#include "CSAXDOMSerializerHandler.h"
#include "UIDList.h"
#include "TextIterator.h"
#include <stack>  // Peut être nécessaire aussi

#include "XPGID.h"
#include "CAlert.h"

#include "XMLDefs.h"

#include "IFootnoteFacade.h"
#include "IFootnoteSettings.h"
#include "IDocument.h"
#include "IOwnedItem.h"
#include "ITextStoryThreadDict.h"
#include "ITextAttrPositionMode.h"
#include "ITextFrameColumn.h"
#include "ITextParcelList.h"
#include "ITextUtils.h"
#include "IStoryList.h"
#include "IStoryService.h"
#include "ITextStoryThread.h"
#include "IComposeScanner.h"
#include "ITextFocus.h"
#include "ISelectionUtils.h"
#include "ITextEditSuite.h"
#include "ITextTarget.h"
#include "ITextModelCmds.h"
#include "IRangeData.h"
#include "IUIDData.h"
#include "IFootnoteNumber.h"
#include "ITextSelectionSuite.h"
#include "IXPageSuite.h"
#include "ITextEditSuite.h"
#include "IXPageIndexSuite.h"
#include "INoteDataUtils.h"
#include "IActiveContext.h"
#include "ITextAttributeSuite.h"

/** Implementation of ISAXDOMSerializerHandler that creates tables based
on DocBook tables.
*/
class XPGTextImportHandler : public CSAXDOMSerializerHandler {

public:

	XPGTextImportHandler(IPMUnknown* boss);
	virtual ~XPGTextImportHandler();

	virtual void Register(ISAXDOMSerializerServices* saxServices, IPMUnknown *importer);
	virtual bool16 HandlesSubElements() const;
	virtual void StartElement(const  WideString& uri, const  WideString& localname, const  WideString& qname, ISAXAttributeList* attrs, IIDXMLDOMNode* currentNode);
	virtual void Characters(const  WideString& chars, IIDXMLDOMNode* currentNode);
	virtual void EndElement(const  WideString& uri, const  WideString& localname, const  WideString& qname, IIDXMLDOMNode* currentNode);

private:

	static const PMString txtElement;
	PMString currentParaStyleTag;
	UID currentParaStyle;
	bool16 inParagraph, tagFound, fontFound;
	bool16 boldFound, italFound, expFound, indFound, endNote, endNoteToClose;
	UIDRef target, storyRef;
	int32 depth, insertPos;
	std::stack<UID> charStyleStack;  

	IFontMgr::FontStyleBits currentFontStyle;

	void		ApplyCurrentFontStyle(int32 startPosition, int32 endPosition);
	ErrorCode	ApplyParaStyleCmd(ITextModel * model, TextIndex insertPos, UID paraStyleUID, bool8 allowUndo);
	ErrorCode	ApplyCharStyleCmd(TextIndex insertPos);

    ErrorCode    ApplyCharStyleOnEndnote();

	ErrorCode	PrepaEndnote(const boost::shared_ptr<WideString>& endnoteString, bool16 create, bool16 insert, bool16 close);
	ErrorCode	InsertEndnote(ITextEditSuite* iTextEditSuite, const boost::shared_ptr<WideString>& endnoteText, TextIndex position, ITextModel* iTextModel, bool16 create, bool16 insert, bool16 close);
	bool16		CanInsertEndnote(ITextModel *iTextModel, TextIndex position);
	bool16		IsEndnoteStory(ITextModel * iTextModel);

};


CREATE_PMINTERFACE(XPGTextImportHandler, kXPGTextImportHandlerImpl)

const PMString XPGTextImportHandler::txtElement = txtTag;

/*
*/
XPGTextImportHandler::XPGTextImportHandler(IPMUnknown* boss) :CSAXDOMSerializerHandler(boss),
depth(0),
target(UIDRef::gNull),
storyRef(UIDRef::gNull),
insertPos(0),
currentParaStyleTag(kNullString),
inParagraph(kFalse), tagFound(kFalse),
currentFontStyle(IFontMgr::kNormal) {}

/*
*/
XPGTextImportHandler::~XPGTextImportHandler() {}


/*
*/
void XPGTextImportHandler::Register(ISAXDOMSerializerServices* saxServices, IPMUnknown *xmlImporter) {

	do {
		// Get back mapping tables and text frame targer ref
		InterfacePtr<IPMUnknownData> importData(xmlImporter, UseDefaultIID());
		InterfacePtr<IImportXMLData> importXMLData(importData->GetPMUnknown(), UseDefaultIID());

		InterfacePtr<IXMLImportOptionsPool> importDataOptions(importXMLData, UseDefaultIID());

		int32 val = 0;
		importDataOptions->GetValue(WideString(kXPGXMLImportTargetKey), val);
		target = UIDRef(importXMLData->GetDataBase(), UID((uint32)val));

		if (target.GetUID() == kInvalidUID) // It's not an xml importation done using XPage, so don't register
			break;

		// Not in namespace
		saxServices->RegisterElementHandler(WideString(""), WideString(txtElement), this);

		storyRef = target;
		Utils<IXMLUtils>()->GetActualContent(storyRef);
        // Must be a story UIDRef since we are only dealing with text frames
        
		// Delete previous existing text
		InterfacePtr<ITextModel> txtModel(storyRef, UseDefaultIID());
		if (txtModel == nil)
			break;
		InterfacePtr<ITextModelCmds> txtModelCmds(txtModel, UseDefaultIID());
		InterfacePtr<ICommand> deleteCmd(txtModelCmds->DeleteCmd(0, txtModel->GetPrimaryStoryThreadSpan() - 1));
		CmdUtils::ProcessCommand(deleteCmd);

	} while (kFalse);
}

bool16 XPGTextImportHandler::HandlesSubElements() const
{
	return kTrue;
}

void XPGTextImportHandler::Characters(const  WideString& chars, IIDXMLDOMNode* currentNode)
{
	boost::shared_ptr<WideString> data(new WideString(chars));
	InterfacePtr<ITextModelCmds> txtModelCmds(storyRef, UseDefaultIID());
	if (tagFound && inParagraph) {
		// GD 16.09.2022 ++
		if (endNote) {
			if (endNoteToClose == kFalse) {
				PrepaEndnote(data, true, true, false);
			this->ApplyCharStyleCmd(insertPos);
				endNoteToClose = kTrue;
		}
		else {
				//on doit inserer du texte dans la note actuelle
				PrepaEndnote(data, false, true, false);
				this->ApplyCharStyleCmd(insertPos);
			}

		}
		else {
			if (endNoteToClose) {
				PrepaEndnote(data, false, false, true);
				this->ApplyCharStyleCmd(insertPos);
				endNoteToClose = kFalse;
			}
			// GD 16.09.2022 --
            
			InterfacePtr<ICommand> insertCmd(txtModelCmds->InsertCmd(insertPos, data));
			CmdUtils::ProcessCommand(insertCmd);
			insertPos += chars.NumUTF16TextChars();

			// GD 16.09.2022 ++
			InterfacePtr<ITextSelectionSuite> textSelectionSuite(Utils<ISelectionUtils>()->QueryActiveTextSelectionSuite());
			if (textSelectionSuite != nil) {
				RangeData rangeToSelect(insertPos, RangeData::kLeanForward);
				textSelectionSuite->SetTextSelection(storyRef, rangeToSelect, Selection::kDontScrollSelection, &rangeToSelect);
			}
			// GD 16.09.2022 --
		}
	}
}

/*
*/
void XPGTextImportHandler::StartElement(const  WideString& uri, const  WideString& localname, const  WideString& qname, ISAXAttributeList* attrs, IIDXMLDOMNode* currentNode) {

	int32 paraStart;
    bool16 isCharstyleChange = kFalse;
    
	++depth;
    
	if (depth == 2) // Paragraph style tag level
	{
		currentParaStyleTag = localname;
		boldFound = kFalse;
		italFound = kFalse;
		expFound = kFalse;
		indFound = kFalse;
		endNote = kFalse;
		endNoteToClose = kFalse;

		// Get paragraph style to apply
		InterfacePtr<IXPGTagToStyleMap> tag2styleMap(storyRef, UseDefaultIID());
		if (tag2styleMap)
		{
			int32 mapCount = tag2styleMap->GetMappingCount();
			for (int32 i = 0; i < mapCount; i++)
			{
				PMString tagName = tag2styleMap->GetTagAt(i);
				if (currentParaStyleTag == WideString(tag2styleMap->GetTagAt(i)))
				{
					tagFound = kTrue;
					break;
				}
			}
		}

		currentParaStyle = tag2styleMap->GetParaStyleMappedToTag(currentParaStyleTag);
		if (tagFound && currentParaStyle != kInvalidUID)
		{
			// Apply style to current paragraph
			InterfacePtr<ITextModelCmds> txtModelCmds(storyRef, UseDefaultIID());
			InterfacePtr<ITextModel> txtModel(storyRef, UseDefaultIID());
			paraStart = insertPos;
			this->ApplyParaStyleCmd(txtModel, paraStart, currentParaStyle, kTrue);
		}
	}

	else if (depth == 3) // Paragraph tag level
	{
		inParagraph = kTrue;
	}

	else if (depth > 3) { // char style tag level

		WideString charTag = localname;
		charTag.ToLower();

		if (charTag == WideString("b") || charTag == WideString("g")) {
			boldFound = kTrue;
            isCharstyleChange = kTrue;
		}
		if (charTag == WideString("i")) {
			italFound = kTrue;
            isCharstyleChange = kTrue;
		}
		if (charTag == WideString("sup")) {
			expFound = kTrue;
            isCharstyleChange = kTrue;
		}
		if (charTag == WideString("inf")) {
			indFound = kTrue;
            isCharstyleChange = kTrue;
		}
		if (charTag == WideString("ins")) {
			endNote = kTrue;
            isCharstyleChange = kFalse;
		}

        if(isCharstyleChange) {
		paraStart = insertPos;
		if (this->ApplyCharStyleCmd(paraStart) != kSuccess) {
		}
	}
}
}

/*
*/
void XPGTextImportHandler::EndElement(const WideString& uri, const WideString& localname, const WideString& qname, IIDXMLDOMNode* currentNode)
{
	int32 paraStart;
    bool16 isCharstyleChange = kFalse;
    
	if (depth == 2) // Paragraph tag level
	{
		tagFound = kFalse;
	}

	if (tagFound && depth == 3) // Paragraph tag level		
	{
		// Insert carriage return
		boost::shared_ptr<WideString> data(new WideString());
		data->push_back(kTextChar_CR);
		InterfacePtr<ITextModelCmds> txtModelCmds(storyRef, UseDefaultIID());
		InterfacePtr<ICommand> insertCmd(txtModelCmds->InsertCmd(insertPos, data));
		CmdUtils::ProcessCommand(insertCmd);
		++insertPos;
		inParagraph = kFalse;
	}
	else if (depth > 3)
	{
		// No mapping, use font variants then
		WideString charTag = localname;
		charTag.ToLower();

		if (charTag == WideString("b") || charTag == WideString("g")) {
			boldFound = kFalse;
            isCharstyleChange = kTrue;
		}
		if (charTag == WideString("i")) {
			italFound = kFalse;
            isCharstyleChange = kTrue;
		}
		if (charTag == WideString("sup")) {
			expFound = kFalse;
            isCharstyleChange = kTrue;
		}
		if (charTag == WideString("inf")) {
			indFound = kFalse;
            isCharstyleChange = kTrue;
		}
		if (charTag == WideString("ins")) {
            endNote = kFalse;
            isCharstyleChange = kFalse;
		}

        if(isCharstyleChange) {
		paraStart = insertPos;
		if (this->ApplyCharStyleCmd(paraStart) != kSuccess) {
			charStyleStack.pop();
			UID charStyle = (charStyleStack.empty() ? kInvalidUID : charStyleStack.top());
			if (charStyle != kInvalidUID)
			{
				//InterfacePtr<ITextModelCmds> txtModelCmds (storyRef, UseDefaultIID());
				//InterfacePtr<ICommand> applyCharStyleCmd (txtModelCmds->ApplyStyleCmd(insertPos, 0, charStyle, kCharAttrStrandBoss));
				//CmdUtils::ProcessCommand(applyCharStyleCmd);
			}
			else
			{
				if (charTag == WideString("b") || charTag == WideString("g"))
				{
					currentFontStyle = (currentFontStyle == IFontMgr::kBoldItalic) ? IFontMgr::kItalic : IFontMgr::kNormal;
				}
				else if (charTag == WideString("i"))
				{
					currentFontStyle = (currentFontStyle == IFontMgr::kBoldItalic) ? IFontMgr::kBold : IFontMgr::kNormal;
				}
				this->ApplyCurrentFontStyle(insertPos, 0);
				fontFound = kFalse;
			}
		}
	}
	}
	--depth;
}

/*
*/
void XPGTextImportHandler::ApplyCurrentFontStyle(int32 startPosition, int32 endPosition)
{
	// GD DEBUG ++
	// Suspicion plantage InDesign CC7, on desactive cette gestion puisqu'on gere present gras / ital via application
	// des FdS caracteres.
	return;
	// GD DEBUG --
	/*
	// Get current font
	InterfacePtr<IComposeScanner> composeScanner (storyRef, UseDefaultIID());
	InterfacePtr<IAttrReport> fontAttr ((IAttrReport*) composeScanner->QueryAttributeAt(startPosition, endPosition, kTextAttrFontUIDBoss));

	if(fontAttr)
	{
	InterfacePtr<ITextAttrUID> textAttrFontUID (fontAttr, IID_ITEXTATTRUID);

	// Get style name corresponding to current font bits according to font prefs
	InterfacePtr<IFontFamily> fontFamily (storyRef.GetDataBase(), textAttrFontUID->GetUIDData(), UseDefaultIID());
	PMString fontStyleName = Utils<IFontUtils>()->GetFontStyleName(fontFamily->GetFamilyNameNative(), currentFontStyle);

	if(!fontStyleName.empty())
	{
	// Apply font style attribute
	InterfacePtr<ITextAttrFont> fontStyleAttr (::CreateObject2<ITextAttrFont> (kTextAttrFontStyleBoss));
	fontStyleAttr->SetFontName(fontStyleName, kFalse);

	boost::shared_ptr<AttributeBossList> attrList (new AttributeBossList());
	attrList->ApplyAttribute(fontStyleAttr);

	InterfacePtr<ITextModelCmds> txtModelCmds (storyRef, UseDefaultIID());
	InterfacePtr<ICommand> applyCmd (txtModelCmds->ApplyCmd(startPosition, endPosition, attrList, kCharAttrStrandBoss));
	CmdUtils::ProcessCommand(applyCmd);
	}
	}*/
}

ErrorCode XPGTextImportHandler::ApplyParaStyleCmd(ITextModel * model, TextIndex insertPosition, UID paraStyleUID, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	do
	{
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil)
		{
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);


		//apply the paragraph style
		if (paraStyleUID != kInvalidUID)
		{
			InterfacePtr<ITextModelCmds> modelCmds(model, UseDefaultIID());
			InterfacePtr<IAttributeStrand> charStrand(static_cast<IAttributeStrand* >(model->QueryStrand(kCharAttrStrandBoss, IID_IATTRIBUTESTRAND)));
			DataWrapper<AttributeBossList> localCharOverrides = charStrand->GetLocalOverrides(insertPosition, nil);
			if (localCharOverrides.get()->CountBosses() > 0)
			{
				boost::shared_ptr<WideString> dummyChar(new WideString());
				dummyChar->push_back(kTextChar_Space);
				InterfacePtr<ICommand> insertCharCmd(modelCmds->InsertCmd(insertPosition, dummyChar));
				CmdUtils::ProcessCommand(insertCharCmd);

				boost::shared_ptr<AttributeBossList> overridesToDelete(new AttributeBossList(*localCharOverrides));
				InterfacePtr<ICommand> clearOverridesCmd(modelCmds->ClearOverridesCmd(insertPosition, 1, overridesToDelete, kCharAttrStrandBoss));
				status = CmdUtils::ProcessCommand(clearOverridesCmd);
				if (status != kSuccess)
				{
					break;
				}
				InterfacePtr<ICommand> deleteCmd(modelCmds->DeleteCmd(insertPosition, 1));
				CmdUtils::ProcessCommand(deleteCmd);
			}

			// Create an ApplyTextStyleCmd
			//on recupere l'uid de la feuille de style [none] pour reinit la fds
			IDocument *doc;
#if !XPAGESERVER
			doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
			if (doc == nil) {
				break;
			}

#else
			// In InDesign Server mode, current doc is the latest opened document (before the one which has just been closed)
			InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
			InterfacePtr<IDocumentList> documentList(application->QueryDocumentList());
			if (documentList->GetDocCount() > 0)
				doc = documentList->GetNthDoc(documentList->GetDocCount() - 1);
			if (doc == nil) {
				break;
			}

#endif

			UIDRef docWS = doc->GetDocWorkSpace();
			InterfacePtr<IStyleGroupManager> charNameTable(docWS, IID_ICHARSTYLEGROUPMANAGER);

			UID resetStyleUID = charNameTable->GetRootStyleUID();
			if (resetStyleUID != kInvalidUID) {
				InterfacePtr<ICommand> resetStyleCmd(modelCmds->ApplyStyleCmd(insertPosition, 0, resetStyleUID, kCharAttrStrandBoss));
				if (resetStyleCmd == nil) {
					break;
				}
				// Process the ApplyTextStyleCmd: 
				status = CmdUtils::ProcessCommand(resetStyleCmd);
			}
			// Create an ApplyTextStyleCmd						
			InterfacePtr<ICommand> applyCmd(modelCmds->ApplyStyleCmd(insertPosition, 0, paraStyleUID, kParaAttrStrandBoss, kTrue, kFalse, kFalse));
			if (applyCmd == nil)
			{
				break;
			}
			// Process the ApplyTextStyleCmd: 
			status = CmdUtils::ProcessCommand(applyCmd);
		}

		status = kSuccess;

	} while (false);
	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(kSuccess);

		CmdUtils::EndCommandSequence(seq);
	}
	else
		ErrorUtils::PMSetGlobalErrorCode(status);


	return status;
}

ErrorCode XPGTextImportHandler::ApplyCharStyleCmd(TextIndex insertPosition)
{
	ErrorCode status = kFailure;
	ErrorCode result = kFailure;
	ICommandSequence * seq = nil;

	do
	{
		//on a pas de style paragraphe, donc pas la peine d'aller plus loin
		if (currentParaStyle == kInvalidUID)
			break;

		//on recupere le document
		IDocument *doc;
#if !XPAGESERVER
		doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
#else
		// In InDesign Server mode, current doc is the latest opened document (before the one which has just been closed)
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> documentList(application->QueryDocumentList());
		if (documentList->GetDocCount() > 0)
			doc = documentList->GetNthDoc(documentList->GetDocCount() - 1);
#endif
		if (doc == nil) {
			break;
		}
		//on recupere la database
		IDataBase * db = ::GetDataBase(doc);
		if (db == nil) {
			break;
		}

		UIDRef docWS = doc->GetDocWorkSpace();

		//on recup la liste des feuilles de styles characteres
		InterfacePtr<IStyleGroupManager> charNameTable(docWS, IID_ICHARSTYLEGROUPMANAGER);
		if (charNameTable == nil)
		{
			ASSERT_FAIL("XPGTextImportHandler::ApplyCharStyleCmd -> charNameTable invalid");
			break;
		}

        InterfacePtr<ITextModelCmds> txtModelCmds(storyRef, UseDefaultIID());
        InterfacePtr<ITextModel> txtModel(storyRef, UseDefaultIID());

		//on recupere le nom de la feuille de style paragraphe appliquée
		PMString paraStyleName;
		PMString charStyleName;

		InterfacePtr<IStyleInfo> styleInfo(db, currentParaStyle, UseDefaultIID());
		if (styleInfo == nil) {
			break;
		}
		paraStyleName = styleInfo->GetName();
		charStyleName = paraStyleName;
		//on complete le nom de la feuille de style
		if (boldFound) {
			charStyleName.Append("Bold");
		}
		if (italFound) {
			charStyleName.Append("Ital");
		}
		if (expFound) {
			charStyleName.Append("Exp");
		}
		if (indFound) {
			charStyleName.Append("Ind");
		}

		// HF-GD 8.0.4 ++
		IStyleGroupHierarchy *charStylesHierarchy = charNameTable->GetRootHierarchy();
		if (charStylesHierarchy == nil)
		{
			ASSERT_FAIL("XPGTextImportHandler::ApplyCharStyleCmd -> charStylesHierarchy invalid");
			break;
		}
       
		UID charStyleUID = Utils<IXPageUtils>()->TriasFindStyleByName(charStylesHierarchy, charStyleName);
		if (charStyleUID == kInvalidUID)
			charStyleUID = charNameTable->GetRootStyleUID();
		// HF-GD 8.0.4 --

		//si la FdS existe, on l'applique
		if (charStyleUID != kInvalidUID)
		{
			// Create a command sequence
			seq = CmdUtils::BeginCommandSequence();
			if (seq == nil)
			{
				break;
			}
			seq->SetUndoability(ICommand::kAutoUndo);

			InterfacePtr<ITextModelCmds> modelCmds(txtModel, UseDefaultIID());
            
			InterfacePtr<IAttributeStrand> charStrand(static_cast<IAttributeStrand* >(txtModel->QueryStrand(kCharAttrStrandBoss, IID_IATTRIBUTESTRAND)));
            DataWrapper<AttributeBossList> localCharOverrides = charStrand->GetLocalOverrides(insertPosition, nil);
			if (localCharOverrides.get()->CountBosses() > 0)
			{
				boost::shared_ptr<WideString> dummyChar(new WideString());
				dummyChar->push_back(kTextChar_Space);
                InterfacePtr<ICommand> insertCharCmd(modelCmds->InsertCmd(insertPosition, dummyChar));
				CmdUtils::ProcessCommand(insertCharCmd);

				boost::shared_ptr<AttributeBossList> overridesToDelete(new AttributeBossList(*localCharOverrides));
				InterfacePtr<ICommand> clearOverridesCmd(modelCmds->ClearOverridesCmd(insertPos, 1, overridesToDelete, kCharAttrStrandBoss));
				status = CmdUtils::ProcessCommand(clearOverridesCmd);
				if (status != kSuccess)
				{
					break;
				}

				InterfacePtr<ICommand> deleteCmd(modelCmds->DeleteCmd(insertPos, 1));
				CmdUtils::ProcessCommand(deleteCmd);
			}

			// Create an ApplyTextStyleCmd						
            InterfacePtr<ICommand> applyCmd(modelCmds->ApplyStyleCmd(insertPos, 0, charStyleUID, kCharAttrStrandBoss));
			if (applyCmd == nil)
			{
				break;
			}
			// Process the ApplyTextStyleCmd: 
			status = CmdUtils::ProcessCommand(applyCmd);

			if (seq != nil)
			{
				CmdUtils::EndCommandSequence(seq);
			}

			ErrorUtils::PMSetGlobalErrorCode(kSuccess);
		}

	} while (false);
	status = kSuccess;
	result = status;
	return result;
}

ErrorCode XPGTextImportHandler::PrepaEndnote(const boost::shared_ptr<WideString>& endnoteString, bool16 create, bool16 insert, bool16 close) {
	ErrorCode result = kFailure;
	do {
		InterfacePtr<ISelectionManager> iSelectionManager(Utils<ISelectionUtils>()->QueryActiveSelection());
		if (iSelectionManager == nil) {
			CAlert::InformationAlert("XPGTextImportHandler::InsertEndnote - iSelectionManager = nil");
			break;
		}

		InterfacePtr<ITextEditSuite> iTextEditSuite(iSelectionManager, UseDefaultIID());
		if (iTextEditSuite == nil) {
			CAlert::InformationAlert("XPGTextImportHandler::InsertEndnote - iTextEditSuite = nil");
			break;
		}

		InterfacePtr<ITextModel> txtModel(storyRef, UseDefaultIID());
		if (txtModel == nil) {
			CAlert::InformationAlert("XPGTextImportHandler::InsertEndnote - txtModel = nil");
			break;
		}

		result = this->InsertEndnote(iTextEditSuite, endnoteString, insertPos, txtModel, create, insert, close);
	} while (false);
	return result;
}

ErrorCode XPGTextImportHandler::InsertEndnote(ITextEditSuite* iTextEditSuite, const boost::shared_ptr<WideString>& endnoteText, TextIndex position, ITextModel* iTextModel, bool16 create, bool16 insert, bool16 close) {
	ErrorCode result = kFailure;
	do {

		if (create) {
		if (this->CanInsertEndnote(iTextModel, position) == kFalse)
		{
			CAlert::InformationAlert("XPGTextImportHandler::InsertEndnote - Cannot insert endnote at this focus");
			break;
		}

		// Step 1: Insert the endnote marker and create a blank endnote range. Focus will be at the endnote range at this point.
		result = iTextEditSuite->InsertEndnote();
		if (result != kSuccess)
			return result;
            
            ++insertPos; // indispensable pour notre gestion du curPos d'insertion de texte/ application de style
		}
		if (insert) {
					 // Step 2: Insert the actual endnote text
            
            // GD 19.02.2024 ++
            IActiveContext* ac = GetExecutionContextSession()->GetActiveContext();
            InterfacePtr<ITextAttributeSuite> textAttributeSuite(ac->GetContextSelection(), UseDefaultIID());
            InterfacePtr<IStyleGroupManager> styleGroupMgr(ac->GetContextWorkspace(), IID_ICHARSTYLEGROUPMANAGER);
            PMString theStyleName("");
            if (italFound) {
                theStyleName.Append("NotesItal");
            } else {
                theStyleName.Append("Notes");
            }
            theStyleName.SetTranslatable(kFalse);
            UID styleUID = styleGroupMgr->FindByName(theStyleName);
            if (styleUID != kInvalidUID)
                textAttributeSuite->ApplyStyle(styleUID);
            // GD 19.02.2024 --
            
		result = iTextEditSuite->InsertText(*endnoteText.get());
		if (result != kSuccess)
			return result;
		}
		if (close) {
		// Step 3: Return to original selection
		InterfacePtr<ITextSelectionSuite> selectionSuite(iTextEditSuite, UseDefaultIID());

		UIDRef sourceStoryRef(::GetDataBase(iTextModel), ::GetUID(iTextModel));
		RangeData range(position, RangeData::kLeanBack);

		selectionSuite->SetTextSelection(sourceStoryRef, range, Selection::kScrollIntoView, nil);
		}
		result = kSuccess;
	} while (false);

	return result;
}

bool16 XPGTextImportHandler::CanInsertEndnote(ITextModel *iTextModel, TextIndex position)
{
	bool16 canInsert = kFalse;
	do
	{
		// we cannot insert endnotes into an endnote story
		if (this->IsEndnoteStory(iTextModel))
		{
			break;
		}
		// is this operation possible?
		InterfacePtr<ITextStoryThread>	iTextStoryThread(iTextModel->QueryStoryThread(position));
		if (iTextStoryThread == nil)
		{
			ASSERT(iTextStoryThread);
			break;
		}

		InterfacePtr<IOwnedItem> ownedItem(::GetDataBase(iTextModel), iTextStoryThread->GetDictUID(), UseDefaultIID());
		if (ownedItem)
		{
			//Cannot insert in Footnote text or note text 
			if (::GetClass(ownedItem) == kFootnoteReferenceBoss || Utils<INoteDataUtils>()->IsNoteOwnedItem(::GetClass(ownedItem)))
				break;
		}
		canInsert = kTrue;
	} while (kFalse);
	return canInsert;
}

bool16 XPGTextImportHandler::IsEndnoteStory(ITextModel * iTextModel)
{
	if (::GetClass(iTextModel) == kEndnoteStoryBoss)
		return kTrue;
	else
		return kFalse;
}
