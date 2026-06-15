#include "VCPluginHeaders.h"

// Interface includes
#include "ISubject.h"
#include "IPanelControlData.h"
#include "IDropDownListController.h"
#include "IStringListControlData.h"
#include "IStringListData.h"
#include "ITextControlData.h"
#include "ITriStateControlData.h"
#include "ISysFileData.h"
#include "ILayoutUIUtils.h"
#include "ISelectionUtils.h"
#include "IDatabase.h"
#include "IAssignmentMgr.h"
#include "IAssignment.h"
#include "IAssignmentUtils.h"
#include "IAssignedDocument.h"
#include "IDFile.h"
#include "IHierarchy.h"
#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "ISAXServices.h"
#include "ISAXContentHandler.h"
#include "ISAXEntityResolver.h"
#include "ISAXParserOptions.h"
#include "IPMStream.h"
#include "StreamUtil.h"
#include "StringUtils.h" 
#include "IInCopyStoryList.h"
#include "IInCopyWorkFlowUtils.h"
#include "IGeometry.h"
#include "TransformTypes.h"
#include "TransformUtils.h"
#include "IPageList.h"
#include "ISpread.h"
#include "ISpreadList.h"
#include "ISpreadLayer.h"
#include "IGroupCmdData.h"
#include "CmdUtils.h"
#include "IAssignmentUIUtils.h"
#include "IFrameType.h"
#include "PageItemScrapID.h"
#include "IClassIDData.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"
#include "CoreResTypes.h"
#include "IDialog.h"
#include "IDialogMgr.h"
#include "IApplication.h"
#include "IStringData.h"
#include "IAttrReport.h"
#include "IActiveContext.h"
#include "IPageItemTypeUtils.h"
#include "IDrawingStyle.h" 
#include "IUnitOfMeasure.h"
#include "IDocFileHandler.h"
#include "IDocumentUtils.h"
#include "IStrand.h"
#include "IPMUnknownData.h"
#include "ITextData.h"
#include "ITreeViewHierarchyAdapter.h"
#include "ILayerList.h"
#include "IGuidePrefs.h"
#include "IRangeData.h"
#include "TextRange.h"
#include "IDocumentList.h"
#include "ISnippetExport.h"
#include "ITreeViewController.h"
#include "IToolBoxUtils.h"
#include "ITool.h"
#include "Utils.h"
#include "IPointerData.h"
#include "IUIDData.h"
#include "ILiveEditFacade.h"
#include "ILinkFacade.h"
#include "IGraphicFrameData.h"
#include "IWidgetParent.h"
#include "XPGArticleDataNode.h"
#include "IArticleData.h"
#include "DataWrapper.h"
#include "ITextStrand.h"
#include "IAttributeStrand.h"
#include "IItemStrand.h"
#include "IStyleInfo.h"
#include "IStyleGroupManager.h"
#include "IDocumentCommands.h"

// General includes
#include "SelectionObserver.h"
#include "FileUtils.h"
#include "CAlert.h"
#include "ErrorUtils.h"
#include "IFrameList.h"
#include "ITextAttrUtils.h"
#include "ITextModelCmds.h"
#include "ITextFrameColumn.h"
#include "ITextAttrBoolean.h"
#include "TextAttributeRunIterator.h"
#include "IScrapItem.h"
#include "IPageItemAdornmentList.h"
#include "IStoryUtils.h"
#include "IXPageMgrAccessor.h"
#include "IXPageIndexSuite.h"
#include "IXPageUtils.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"
#include "IXPageSuite.h"
#include "IFormeData.h"
#include "IPlacedArticleData.h"

// Project includes
#include "XPGUIArticleNodeID.h"
#include "XPageUIUtils.h"
#include "XMLDefs.h"
#include "XPGUIID.h"
#include "XPGID.h"
#include "XRLUIID.h"  // For kXRLUIOpenLoginDialogCmdBoss (cross-plugin)

class XPGUIXRailPanelObserver : public ActiveSelectionObserver {

public:

	XPGUIXRailPanelObserver(IPMUnknown* boss);
	virtual ~XPGUIXRailPanelObserver();
	void AutoAttach();
	void AutoDetach();
	void Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy);

private:

	void AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID);
	void DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID);
	void AttachDocList(void);
	void DetachDocList(void);

	// Gestion des articles
	void UpdateSelectedStories();
	void UpdateIDMS();
	void CreateArticle();
	void UnlinkAssignment();
	void AddToAssignment();
	void EditArticle();
	void ResetStoryInfosWidgets();

	ErrorCode UpdatePlacedStory(const UIDRef& itemRef, int32& idPage);
	ErrorCode UpdatePlacedStory2(const UIDRef& itemRef, int32& idPage);
	ErrorCode UpdateInCopyStory(const UIDRef& itemRef, int32& idPage);

	// Info users
	void DisplayInfoPage(const PMString& titreLib, const PMString& parutionLib);
	void DisplayCurrentUser();

	// Adornments
	void DisplayAdornmentShape(bool16 visible);
	void DisplayAdornmentText(bool16 visible);
	void DeleteAdonments(const UIDList& itemList);
	void RefreshAdornment();
	void ChangeStyles();
	void RefreshAdornmentForme();
	// Reflect the shared session "Afficher les formes" state on our checkbox
	// (no re-fire). Called on attach and on the sibling-palette sync message.
	void SyncDisplayFormesCheckbox();

	// Selection
	void HandleSelectionChanged(const ISelectionMessage * message);

	// Section & SsSection
	void FillSectionList(int32 titreID);
	void FillSsSectionList();

	// Parutions
	void FillParutionList();
	void HandleParutionSelection();

	// Fill All Texts
	void FillParutionStories();

	ErrorCode DeleteAssignment(IAssignment* assignment);

	void HandleItem(IHierarchy * itemHier, K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > >& exportableStories,
		K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > >& itemsByArticle, UIDList& alreadyAddedStories);

	int32 CanUpdateStory(const PMString& idArticle);

	// Show/hide the panel content depending on the session login state and
	// open the XRail login dialog when the user clicks the "Sign in" button.
	void UpdateLoginState();
	void OpenLoginDialog();

	// True iff there is a front document AND the active page has a non-zero
	// XRail page ID. Used to gate the whole palette: when false, all
	// interactive widgets are greyed out.
	bool16 IsPaletteUsable() const;

	bool16 firstInit;
	int32 lastPageID;
	UID ownerPageUID;

	// UID of the document for which we last reloaded parutions/rubriques.
	// Compared in HandleSelectionChanged so we reload only on doc switch
	// or palette display — page navigation within the same doc keeps the
	// already-fetched lists (titre/rubriques are doc-scoped, not page-scoped).
	UID fLastReloadedDocUID;

};
CREATE_PMINTERFACE(XPGUIXRailPanelObserver, kXPGUIArticlePanelObserverImpl)

XPGUIXRailPanelObserver::XPGUIXRailPanelObserver(IPMUnknown* boss)
	:ActiveSelectionObserver(boss),
	firstInit(kTrue),
	lastPageID(-1),
	fLastReloadedDocUID(kInvalidUID)
{
	// Get notification when a refresh is required
	InterfacePtr<ISubject> sessionSubject(GetExecutionContextSession(), UseDefaultIID());
	sessionSubject->AttachObserver(this, IID_IREFRESHPROTOCOL);
	// Login state changes are broadcast by XRail with its own protocol IID
	// (XRail-owned, no XPage dependency).
	sessionSubject->AttachObserver(this, IID_IXRLUILOGINPROTOCOL);
}

XPGUIXRailPanelObserver::~XPGUIXRailPanelObserver() {}

void XPGUIXRailPanelObserver::AutoAttach() {

	// set data model for tree view widget		
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	InterfacePtr<IPMUnknownData> dataModel(panelControlData->FindWidget(kXPGUIArticleViewWidgetID), IID_IDATAMODEL);
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor(GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IArticleDataModel> textDataModel(pageMrgAccessor->QueryArticleDataModel());
	dataModel->SetPMUnknown(textDataModel);

	ActiveSelectionObserver::AutoAttach();
	do {
		// Initialise widget state.
		if (panelControlData == nil)
			break;

		// Observes widgets — note that the login prompt is a ButtonWidget,
		// which broadcasts state changes on IID_IBOOLEANCONTROLDATA (not
		// ITriStateControlData like icon buttons). Subscribing with the
		// wrong protocol means the observer never gets the click.
		AttachWidget(panelControlData, kXPGUILoginPromptButtonWidgetID, IID_IBOOLEANCONTROLDATA);
		AttachWidget(panelControlData, kXPGUIArticleViewWidgetID, IID_ITREEVIEWCONTROLLER);
		AttachWidget(panelControlData, kXPGUIMAJArtButtonWidgetID, ITriStateControlData::kDefaultIID);
		AttachWidget(panelControlData, kXPGUICreateIncopyWidgetID, ITriStateControlData::kDefaultIID);
		AttachWidget(panelControlData, kXPGUIDeleteAssignmentWidgetID, ITriStateControlData::kDefaultIID);
		AttachWidget(panelControlData, kXPGUIRefreshAdornmentWidgetID, ITriStateControlData::kDefaultIID);
		AttachWidget(panelControlData, kXPGUIDisplayStoryStatesWidgetID, ITriStateControlData::kDefaultIID);
		AttachWidget(panelControlData, kXPGUIAddToAssignmentButtonWidgetID, ITriStateControlData::kDefaultIID);
		AttachWidget(panelControlData, kXPGUIDisplayFormesWidgetID, ITriStateControlData::kDefaultIID);
		AttachWidget(panelControlData, kXPGUISectionListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		AttachWidget(panelControlData, kXPGUISsSectionListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		AttachWidget(panelControlData, kXPGUIParutionListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		AttachWidget(panelControlData, kXPGUIRefreshFormesAdornmentWidgetID, ITriStateControlData::kDefaultIID);
		AttachWidget(panelControlData, kXPGUIChangeStylesWidgetID, IID_IBOOLEANCONTROLDATA);

		// Sync the checkbox visual to the shared session state (set by the
		// sibling XDA palette, if any). Don't re-apply adornments here — they
		// are (re)applied on an actual toggle/refresh.
		this->SyncDisplayFormesCheckbox();

		InterfacePtr<ITriStateControlData> displayStoryStatesControlData(panelControlData->FindWidget(kXPGUIDisplayStoryStatesWidgetID), UseDefaultIID());
		if (displayStoryStatesControlData)
			this->DisplayAdornmentShape(displayStoryStatesControlData->GetState());


		// Observes front document changes
		this->AttachDocList();
		if (firstInit) {
			// Fill parution list
			this->FillParutionList();
			this->HandleParutionSelection();
			firstInit = kFalse;
		}

		// Reflect the current login state on the panel widgets.
		this->UpdateLoginState();

		// Apply the palette-usable gate + selection-driven button rules
		// right away so the user doesn't see freshly-Enable()d buttons
		// before the first selection-change broadcast arrives.
		this->HandleSelectionChanged(nil);

	} while (false);
}


void XPGUIXRailPanelObserver::AutoDetach() {

	ActiveSelectionObserver::AutoDetach();
	do {
		// Detach widget observers.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
			break;

		DetachWidget(panelControlData, kXPGUILoginPromptButtonWidgetID, IID_IBOOLEANCONTROLDATA);
		DetachWidget(panelControlData, kXPGUIArticleViewWidgetID, IID_ITREEVIEWCONTROLLER);
		DetachWidget(panelControlData, kXPGUIMAJArtButtonWidgetID, ITriStateControlData::kDefaultIID);
		DetachWidget(panelControlData, kXPGUIDeleteAssignmentWidgetID, ITriStateControlData::kDefaultIID);
		DetachWidget(panelControlData, kXPGUICreateIncopyWidgetID, ITriStateControlData::kDefaultIID);
		DetachWidget(panelControlData, kXPGUIRefreshAdornmentWidgetID, ITriStateControlData::kDefaultIID);
		DetachWidget(panelControlData, kXPGUIDisplayStoryStatesWidgetID, ITriStateControlData::kDefaultIID);
		DetachWidget(panelControlData, kXPGUIAddToAssignmentButtonWidgetID, ITriStateControlData::kDefaultIID);
		DetachWidget(panelControlData, kXPGUIDisplayFormesWidgetID, ITriStateControlData::kDefaultIID);
		DetachWidget(panelControlData, kXPGUIRefreshFormesAdornmentWidgetID, ITriStateControlData::kDefaultIID);
		DetachWidget(panelControlData, kXPGUISectionListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		DetachWidget(panelControlData, kXPGUISsSectionListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		DetachWidget(panelControlData, kXPGUIParutionListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		DetachWidget(panelControlData, kXPGUIChangeStylesWidgetID, IID_IBOOLEANCONTROLDATA);

		this->DetachDocList();

	} while (false);
}

/* Update
 */
void XPGUIXRailPanelObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy) {

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());

	ActiveSelectionObserver::Update(theChange, theSubject, protocol, changedBy);
	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());

	if ((protocol == IID_ISTRINGLISTCONTROLDATA)) {
		if (controlView != nil) {
			WidgetID widgetID = controlView->GetWidgetID();
			switch (widgetID.Get()) {

			case kXPGUISectionListWidgetID:
				this->FillSsSectionList();
				break;

			case kXPGUIParutionListWidgetID:
				this->HandleParutionSelection();
				break;

			case kXPGUISsSectionListWidgetID:
				this->HandleParutionSelection();
				break;

			default:
				break;
			}
		}
	}
	else if (theChange == kTrueStateMessage) {
		if (controlView != nil) {
			WidgetID widgetID = controlView->GetWidgetID();
			switch (widgetID.Get()) {

				// Gestion des articles
			case kXPGUIMAJArtButtonWidgetID:
				
				if (xpgPrefs->GetIDMS_MAJIDMS() == 1) {
					UpdateIDMS();
					//UpdateSelectedStories();
				}
				else {
					UpdateSelectedStories();
				}
				break;

			case kXPGUICreateIncopyWidgetID:
				CreateArticle();
				break;

			case kXPGUIDeleteAssignmentWidgetID:
				UnlinkAssignment();
				break;

			case kXPGUIAddToAssignmentButtonWidgetID:
				this->AddToAssignment();
				break;

				// Adornments
			case kXPGUIDisplayFormesWidgetID:
				XPageUIUtils::SetDisplayFormes(kTrue);
				break;

			case kXPGUIDisplayStoryStatesWidgetID:
				this->DisplayAdornmentShape(kTrue);
				break;

			case kXPGUIRefreshAdornmentWidgetID:
				this->RefreshAdornment();
				break;

			case kXPGUIRefreshFormesAdornmentWidgetID:
				this->RefreshAdornmentForme();
				break;

			case kXPGUIChangeStylesWidgetID:
				this->ChangeStyles();
				break;

			case kXPGUILoginPromptButtonWidgetID:
				this->OpenLoginDialog();
				break;

			default:
				break;
			}
		}
	}

	else if (theChange == kFalseStateMessage) {
		if (controlView != nil) {
			WidgetID widgetID = controlView->GetWidgetID();
			switch (widgetID.Get()) {

			case kXPGUIDisplayFormesWidgetID:
				XPageUIUtils::SetDisplayFormes(kFalse);
				break;

			case kXPGUIDisplayStoryStatesWidgetID:
				DisplayAdornmentShape(kFalse);
				break;

			}
		}
	}

	else if ((theChange == kOpenDocCmdBoss || theChange == kSetFrontDocCmdBoss || theChange == kCloseDocCmdBoss) && (protocol == IID_IDOCUMENTLIST)) {

		// Do NOT reload unconditionally here: InDesign re-fires
		// kSetFrontDocCmdBoss on app re-activation (e.g. switching to the
		// browser and back) with the SAME front document. Reloading then
		// would wipe the user's current parution/rubrique selection and reset
		// the parution dropdown to its default.
		//
		// HandleSelectionChanged's Step 4 already performs the reload *guarded
		// by the front-doc UID*: it invalidates lastPageID and calls
		// FillParutionList only when curDocUID != fLastReloadedDocUID, i.e. on
		// a genuine doc switch/open/close — not on a same-doc re-activation.
		this->HandleSelectionChanged(nil);
	}

	else if (/*(theChange == kSaveDocCmdBoss) && */(protocol == IID_IDOCUMENT)) {
		// Update the parution List
		this->FillParutionStories();
	}

	else if (theChange == kListSelectionChangedMessage) {
		this->HandleSelectionChanged(nil);
	}

	else if (theChange == kXPGRefreshMsg && protocol == IID_IREFRESHPROTOCOL) {
		this->HandleParutionSelection();
	}

	else if (theChange == kXPGDisplayFormesChangedMsg && protocol == IID_IREFRESHPROTOCOL) {
		// The sibling palette toggled "Afficher les formes" — re-sync our
		// checkbox to the shared session state (visual only, no re-fire).
		this->SyncDisplayFormesCheckbox();
	}

	else if (theChange == kXRLUILoginChangedMsg && protocol == IID_IXRLUILOGINPROTOCOL) {
		// Login state changed (sign-in success or dialog cancel). Refresh
		// the server-side XPage prefs first so the rest of the UI shows
		// fresh values, then toggle the "Sign in" prompt visibility.
		XPageUIUtils::RefreshXPagePrefsFromServer();
		this->UpdateLoginState();
	}
}

/* UpdateLoginState
 *
 * Three-state overlay for the palette:
 *   1. Not logged in           -> show login prompt (text + "Se connecter").
 *   2. Logged in, no Gaia page -> show "Open a Gaia page" message.
 *   3. Logged in + Gaia page   -> show normal panel content; hide overlays.
 *
 * Login state takes priority — the user has to authenticate before we even
 * bother checking for an XRail page.
*/
void XPGUIXRailPanelObserver::UpdateLoginState()
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	InterfacePtr<IStringData> gLogin(GetExecutionContextSession(), IID_ILOGINSTRING);
	const bool16 loggedIn = (gLogin != nil && !gLogin->Get().IsEmpty());

	// "Has a Gaia page" only matters when logged in. We compute it eagerly
	// but it's only consulted in the loop below if loggedIn == kTrue.
	const bool16 hasGaiaPage = this->IsPaletteUsable();

	const int32 n = panelControlData->Length();
	for (int32 i = 0; i < n; ++i)
	{
		IControlView* child = panelControlData->GetWidget(i);
		if (child == nil)
			continue;

		const WidgetID widgetID = child->GetWidgetID();

		bool16 shouldShow = kFalse;
		if (widgetID == kXPGUILoginPromptButtonWidgetID ||
		    widgetID == kXPGUILoginPromptTextWidgetID)
		{
			shouldShow = !loggedIn;
		}
		else if (widgetID == kXPGUINoGaiaPageTextWidgetID)
		{
			shouldShow = (loggedIn && !hasGaiaPage);
		}
		else
		{
			// Normal panel content shows only when we've made it past
			// both gates.
			shouldShow = (loggedIn && hasGaiaPage);
		}

		if (shouldShow) child->ShowView();
		else            child->HideView();
	}
}

/* OpenLoginDialog
 *
 * Trigger the XRail plugin's OpenLoginDialog command. After a successful
 * login the XRail dialog broadcasts kXRLUILoginChangedMsg which drives
 * UpdateLoginState().
 */
void XPGUIXRailPanelObserver::OpenLoginDialog()
{
	InterfacePtr<ICommand> cmd(CmdUtils::CreateCommand(kXRLUIOpenLoginDialogCmdBoss));
	if (cmd == nil)
		return;
	CmdUtils::ProcessCommand(cmd);
}
void XPGUIXRailPanelObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
{
	do {
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)
			break;

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
			break;

		subject->AttachObserver(this, interfaceID);
	} while (false);
}

/* DetachWidget
 */
void XPGUIXRailPanelObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
{
	do {
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)
			break;

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
			break;

		subject->DetachObserver(this, interfaceID);
	} while (false);
}

/* AttachDocList
 */
void XPGUIXRailPanelObserver::AttachDocList(void)
{
	do
	{
		InterfacePtr<IApplication> app(GetExecutionContextSession()->QueryApplication());
		if (app == nil)
			break;

		InterfacePtr<IDocumentList> doclist(app->QueryDocumentList());
		if (doclist == nil)
			break;

		InterfacePtr<ISubject> subject(doclist, UseDefaultIID());
		if (subject == nil)
			break;

		subject->AttachObserver(this, IID_IDOCUMENTLIST);

		IDocument *doc;
		doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if (doc == nil)
			break;
		InterfacePtr<ISubject> acSubject(doc, UseDefaultIID());
		if (acSubject && !acSubject->IsAttached(this, IID_IDOCUMENT)) {
			acSubject->AttachObserver(this, IID_IDOCUMENT);
		}
	} while (false);
}

/* DetachDocList
 */
void XPGUIXRailPanelObserver::DetachDocList(void) {
	do {
		InterfacePtr<IApplication> app(GetExecutionContextSession()->QueryApplication());
		if (app == nil)
			break;
		InterfacePtr<IDocumentList> doclist(app->QueryDocumentList());
		if (app == nil)
			break;
		InterfacePtr<ISubject> subject(doclist, UseDefaultIID());
		if (app == nil)
			break;
		subject->DetachObserver(this, IID_IDOCUMENTLIST);

		IDocument *doc;
		doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if (doc == nil)
			break;
		InterfacePtr<ISubject> acSubject(doc, UseDefaultIID());
		if (acSubject && !acSubject->IsAttached(this, IID_IDOCUMENT)) {

			acSubject->DetachObserver(this, IID_IDOCUMENT);
		}

	} while (false);
}

void XPGUIXRailPanelObserver::RefreshAdornment() {

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	IControlView* displayShapeCtrlView = panelControlData->FindWidget(kXPGUIDisplayStoryStatesWidgetID);
	InterfacePtr<ITriStateControlData> displayShapeValue(displayShapeCtrlView, UseDefaultIID());

	if (displayShapeValue->IsSelected())
		this->DisplayAdornmentShape(kTrue);
	else
		this->DisplayAdornmentShape(kFalse);
}


void XPGUIXRailPanelObserver::CreateArticle() {
	do {

		InterfacePtr<ITool> activeTool(Utils<IToolBoxUtils>()->QueryActiveTool());
		if (activeTool && activeTool->IsTextTool())
			break;

		InterfacePtr<ISelectionManager> selectionMgr(Utils<ISelectionUtils>()->QueryActiveSelection());
		InterfacePtr<IXPageSuite> xpgSuite(selectionMgr, IID_IXPAGESUITE);
		if (xpgSuite == nil) // xpgSuite == nil means there is no active selection (neither layout nor text)
			break;

		UIDList txtFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(txtFrames, graphicFrames, selectedItems);

		UIDList exportableItems(txtFrames);
		exportableItems.Append(graphicFrames);

		bool16 alreadyAssigned = kFalse;
		InterfacePtr<IAssignmentMgr> inCopyMgr(GetExecutionContextSession(), UseDefaultIID());
		for (int32 i = 0; i < exportableItems.Length(); ++i) {
			if (inCopyMgr->IsAssigned(exportableItems.GetRef(i))) {
				alreadyAssigned = kTrue;
				break;
			}
		}

		if (alreadyAssigned) {
			CAlert::InformationAlert(kXPGUISelectionAlreadyAssignedKey);
			break;
		}
		// Get the application interface and the DialogMgr.	
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());

		// Load the plug-in's resource.
		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcSpec dialogSpec(
			nLocale,						// Locale index from PMLocaleIDs.h. 
			kXPGUIPluginID,					// Our Plug-in ID  
			kViewRsrcType,					// This is the kViewRsrcType.
			kXPGUIOrderArtDialogResourceID,	// Resource ID for our dialog.
			kTrue							// Initially visible.
		);
		// CreateNewDialog takes the dialogSpec created above, and also
		// the type of dialog being created (kMovableModal).
		IDialog* dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);

		// Open the dialog.
		dialog->Open();
	} while (kFalse);
}

void XPGUIXRailPanelObserver::AddToAssignment() {

	do {
		InterfacePtr<ISelectionManager> selectionMgr(Utils<ISelectionUtils>()->QueryActiveSelection());
		InterfacePtr<IXPageSuite> xpgSuite(selectionMgr, IID_IXPAGESUITE);
		if (xpgSuite == nil) // xpgSuite == nil means there is no active selection (neither layout nor text)
			break;

		UIDList txtFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(txtFrames, graphicFrames, selectedItems);

		UIDList exportableItems(txtFrames);
		if (exportableItems.Length() == 0)
			break;
		bool16 alreadyAssigned = kFalse, placedStory = kFalse;


		InterfacePtr<IPlacedArticleData> placedArticleData(exportableItems.GetRef(0), UseDefaultIID());
		if (placedArticleData && placedArticleData->GetUniqueId() != kNullString) {
			placedStory = kTrue;
		}
		else {
			InterfacePtr<IAssignmentMgr> inCopyMgr(GetExecutionContextSession(), UseDefaultIID());
			for (int32 i = 0; i < exportableItems.Length(); ++i) {
				if (inCopyMgr->IsAssigned(exportableItems.GetRef(i))) {
					alreadyAssigned = kTrue;
					break;
				}
			}
		}

		if (alreadyAssigned) {
			CAlert::InformationAlert(kXPGUISelectionAlreadyAssignedKey);
			break;
		}

		if (placedStory) {
			CAlert::InformationAlert(kXPGUISelectionAlreadyPlacedKey);
			break;
		}

		// Get the application interface and the DialogMgr.	
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());

		// Load the plug-in's resource.
		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcSpec dialogSpec(
			nLocale,									// Locale index from PMLocaleIDs.h. 
			kXPGUIPluginID,								// Our Plug-in ID  
			kViewRsrcType,								// This is the kViewRsrcType.
			kXPGUIAddToStorySubjectDialogResourceID,	// Resource ID for our dialog.
			kTrue										// Initially visible.
		);
		// CreateNewDialog takes the dialogSpec created above, and also
		// the type of dialog being created (kMovableModal).
		// IDialog* dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);
		IDialog* dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kModeless);

		// Open the dialog.
		dialog->Open();
	} while (kFalse);
}
void XPGUIXRailPanelObserver::UnlinkAssignment() {

	int16 rep = CAlert::ModalAlert(PMString(kXPGUIUnlinkStoryKey), kYesString, kNoString, kNullString, 2, CAlert::eWarningIcon);
	if (rep != 1)
		return;

	do {
		InterfacePtr<ISelectionManager> selectionMgr(Utils<ISelectionUtils>()->QueryActiveSelection());
		InterfacePtr<IXPageSuite> xpgSuite(selectionMgr, IID_IXPAGESUITE);
		if (xpgSuite == nil)
			break;

		UIDList textFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);

		UIDList exportableItems(textFrames);
		exportableItems.Append(graphicFrames);

		IDataBase * db = exportableItems.GetDataBase();
		if (db == nil || exportableItems.IsEmpty())
			break;

		// Collect only the selected frames that carry article info. We do NOT
		// expand to the full article group: only the frames actually selected
		// get unlinked. Carton / forme data (IFormeData) is left untouched.
		UIDList articleItems(db);
		for (int32 i = 0; i < exportableItems.Length(); ++i) {

			UIDRef itemRef = exportableItems.GetRef(i);
			InterfacePtr<IPlacedArticleData> placedArticleData(itemRef, UseDefaultIID());
			if (placedArticleData == nil || placedArticleData->GetUniqueId() == kNullString)
				continue;

			articleItems.Append(exportableItems[i]);
		}

		if (articleItems.IsEmpty())
			break;

		// Clear only the article data (id / folder / topic). The carton stays.
		InterfacePtr<ICommand> deletePlacedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
		InterfacePtr<IPlacedArticleData> deleteArticleData(deletePlacedArticleDataCmd, IID_IPLACEDARTICLEDATA);
		deleteArticleData->SetUniqueId(kNullString);
		deleteArticleData->SetStoryFolder(kNullString);
		deleteArticleData->SetStoryTopic(kNullString);
		deletePlacedArticleDataCmd->SetItemList(articleItems);
		if (CmdUtils::ProcessCommand(deletePlacedArticleDataCmd) != kSuccess)
			break;

		// Remove the article adornment only (kXPGUIArticleAdornmentBoss).
		this->DeleteAdonments(articleItems);

		// Send notification so that texte panel is updated
		InterfacePtr<ISubject> sessionSubject(GetExecutionContextSession(), UseDefaultIID());
		sessionSubject->Change(kXPGRefreshMsg, IID_IREFRESHPROTOCOL);

	} while (kFalse);
}


void XPGUIXRailPanelObserver::UpdateSelectedStories() {

	do {
		IDocument * frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if (!frontDoc)
			break;

		IDataBase * db = ::GetDataBase(frontDoc);
		if (!db)
			break;

		InterfacePtr<ISelectionManager> selectionMgr(Utils<ISelectionUtils>()->QueryActiveSelection());
		InterfacePtr<IXPageSuite> xpgSuite(selectionMgr, IID_IXPAGESUITE);
		if (xpgSuite == nil)
			break;

		UIDList textFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);

		int32 itemCount = selectedItems.Length();
		if (itemCount == 0)
			break;

		//ID de la page
		PMReal leftMost = kMaxInt32;
		UID leftMostSelectedItem = kInvalidUID;
		for (int32 i = 0; i < selectedItems.Length(); ++i) {
			InterfacePtr<IGeometry> itemGeo(selectedItems.GetRef(i), UseDefaultIID());
			PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();
			::TransformInnerPointToPasteboard(itemGeo, &leftTop);
			if (leftTop.X() < leftMost) {
				leftMostSelectedItem = selectedItems.At(i);
				leftMost = leftTop.X();
			}
		}

		InterfacePtr<IHierarchy> leftMostItemHier(db, leftMostSelectedItem, UseDefaultIID());
		ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(leftMostItemHier);

		InterfacePtr<ISpread> isASpread(db, ownerPageUID, UseDefaultIID());
		if (isASpread != nil)
			// Le bloc le plus a gauche de l'article est hors page, on renvoie l'ID de la page la plus a gauche dans la planche
			ownerPageUID = isASpread->GetNthPageUID(0);

		// RÈcuperartion de l'id de la page a partir de Xrail Client
		if (ownerPageUID == kInvalidUID) // Wrong page index
			break;

		InterfacePtr<IXRailPageSlugData> readData(db, ownerPageUID, IID_PAGESLUGDATA);
		if (!readData)
			break;

		int32 idPage = readData->GetID();
		UIDRef storyRef = textFrames.GetRef(0);

		InterfacePtr<IPlacedArticleData> placedArticleData(storyRef, UseDefaultIID());
		if (placedArticleData && placedArticleData->GetUniqueId() != kNullString) {
			if (this->UpdatePlacedStory2(storyRef, idPage) != kSuccess) { // apapap message alert
				break;
			}
		}
		else {

			InterfacePtr<IAssignmentMgr> assignMgr(GetExecutionContextSession(), UseDefaultIID());

			// Get assignment related to this story
			InterfacePtr<IAssignment> assignment(nil);
			InterfacePtr<IAssignedStory> assignedStory(nil);

			assignMgr->QueryAssignmentAndAssignedStory(storyRef, assignment, assignedStory);
			if (assignment == nil || assignedStory == nil)
				continue;

			UIDList storyList;
			assignMgr->GetStoryListOnAssignment(assignment, storyList);

			PMString assignFilePath = assignment->GetFile();
			IDFile assignFile = FileUtils::PMStringToSysFile(assignFilePath);
			PMString idArticle = Utils<IXPageUtils>()->GetStoryIDFromAssignmentFile(assignFile);

			UIDList uidsToUpdate(db);
			// Process update CheckOut stories with commands
			InterfacePtr<ILinkManager> iLinkManager(db, db->GetRootUID(), UseDefaultIID());

			for (int32 i = 0; i < storyList.size(); i++) {

				UIDRef itemRef = storyList.GetRef(i);

				// If we're on a placed image we should have a data link to source item
				//InterfacePtr<ILinkObject> iLinkObject(itemRef, UseDefaultIID());

				// get the link for this object			 
				ILinkManager::QueryResult linkQueryResult;
				if (iLinkManager->QueryLinksByObjectUID(storyList[i], linkQueryResult)) {

					ILinkManager::QueryResult::const_iterator iter = linkQueryResult.begin();
					InterfacePtr<ILink> iLink(db, *iter, UseDefaultIID());
					if (iLink /*&& iLink->GetResourceModificationState() == ILink::kResourceModified*/)
					{
						uidsToUpdate.push_back(::GetUID(iLink));
					}
				}
			}

			if (!uidsToUpdate.IsEmpty()) {
				Utils<Facade::ILinkFacade>()->UpdateLinks(uidsToUpdate, false, kMinimalUI, false);      // link uids, do not force, minimal ui, do not schedule cmd
			}

			Utils<Facade::ILiveEditFacade>()->CheckOut(storyList, ICommand::kRegularUndo, nil, kFalse, kFullUI);

		}

	} while (kFalse);
}

void XPGUIXRailPanelObserver::UpdateIDMS() {

	do {
		IDocument * frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if (!frontDoc)
			break;

		IDataBase * db = ::GetDataBase(frontDoc);
		if (!db)
			break;

		// Create converter (coordinates/dimensions are sent in millimeters)
		InterfacePtr<IUnitOfMeasure> uom((IUnitOfMeasure *)::CreateObject(kMillimetersBoss, IID_IUNITOFMEASURE));
		if (uom == nil)
			break;

		InterfacePtr<ISelectionManager> selectionMgr(Utils<ISelectionUtils>()->QueryActiveSelection());
		InterfacePtr<IXPageSuite> xpgSuite(selectionMgr, IID_IXPAGESUITE);
		if (xpgSuite == nil)
			break;

		UIDList textFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);

		int32 itemCount = selectedItems.Length();
		if (itemCount == 0)
			break;

		UIDRef storyRef = textFrames.GetRef(0);
        InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());

		InterfacePtr<IPlacedArticleData> placedArticleData(storyRef, UseDefaultIID());
		if (placedArticleData && placedArticleData->GetUniqueId() != kNullString) {
			//alert de l'id de l'article
			PMString artToUpdate = placedArticleData->GetUniqueId();

			if (CanUpdateStory(artToUpdate) == 0) {
				CAlert::InformationAlert("Impossible de mettre à jour, l'article n'est pas dans le bon état");
				break;
			}

			PMString subject = placedArticleData->GetStoryTopic();

			PMPoint leftTopMost(kMaxInt32, kMaxInt32);
			PMString artPath;
			K2Vector<KeyValuePair <PMString, K2Pair <PMString, UIDList> > > placedArticleList = Utils<IXPageUtils>()->GetPlacedStoriesList(frontDoc);
			for (int32 k = 0; k < placedArticleList.size(); k++) {
				if (placedArticleList[k].Key().IsEqual(artToUpdate)) {
					artPath = placedArticleList[k].Value().first;
					UIDList  articleItems = placedArticleList[k].Value().second;
					InterfacePtr<IHierarchy> firstItem(db, articleItems.At(0), UseDefaultIID());
					UIDRef targetSpread(db, firstItem->GetSpreadUID());

					for (int32 l = 0; l < articleItems.Length(); l++)
					{
						InterfacePtr<IHierarchy> itemHier(db, articleItems.At(l), UseDefaultIID());
						if (itemHier) {
							InterfacePtr<IGeometry> itemGeo(itemHier, UseDefaultIID());
							PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();
							::TransformInnerPointToPasteboard(itemGeo, &leftTop);

							if (leftTop.X() < leftTopMost.X())
								leftTopMost.X() = leftTop.X();
							if (leftTop.Y() < leftTopMost.Y())
								leftTopMost.Y() = leftTop.Y();
						}
					}
					// if yes, delete items from document before IDMS import
					InterfacePtr<ICommand> deletePageItemCmd(CmdUtils::CreateCommand(kDeleteCmdBoss));
					deletePageItemCmd->SetItemList(articleItems);
					if (CmdUtils::ProcessCommand(deletePageItemCmd) != kSuccess)
					{
						CAlert::InformationAlert("XPage - Erreur suppression article avant reimport IDMS");
						break;
					}
					IDFile snippetFile;
					UIDList formeItems(db);
					PMString error;

					snippetFile = FileUtils::PMStringToSysFile(artPath);
					FileUtils::AppendPath(&snippetFile, artToUpdate + ".idms");

					UIDRef docRef = ::GetUIDRef(frontDoc);
					// Import snippet
					if (Utils<IXPageUtils>()->ImportForme(docRef, snippetFile, leftTopMost, targetSpread, IDFile(), error, &formeItems, kTrue, kTrue) != kSuccess) {
						CAlert::InformationAlert("XPage - Erreur import IDMS");
						break;
					}


					// Save article data
					InterfacePtr<ICommand> placedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
					InterfacePtr<IPlacedArticleData> placedArticleData(placedArticleDataCmd, IID_IPLACEDARTICLEDATA);
					placedArticleData->SetUniqueId(artToUpdate);
					placedArticleData->SetStoryFolder(artPath);
					placedArticleData->SetStoryTopic(subject);
					placedArticleDataCmd->SetItemList(formeItems);
					if (CmdUtils::ProcessCommand(placedArticleDataCmd) != kSuccess)
						break;
                    
                    if (xpgPrefs->GetNoUpdateFile() != "1") {
					//on cree un fichier pour dire que le idms a ete mis a jour
					IDFile updateFile = FileUtils::PMStringToSysFile(artPath + artToUpdate+".update");
					InterfacePtr<IPMStream> writeStream(StreamUtil::CreateFileStreamWrite(updateFile, kOpenOut | kOpenTrunc, kTextType));
					if (writeStream == nil)
					{
						ASSERT_FAIL("PrsReverseTCL::SetOutputFile -> writeStream nil");
						break;
					}
					break;
                    }

				}
			}



		}

	} while (kFalse);
}


ErrorCode XPGUIXRailPanelObserver::UpdatePlacedStory(const UIDRef& itemRef, int32& idPage) {

	ErrorCode errorCode = kFailure;
	do {
		InterfacePtr<IPlacedArticleData> placedArticleData(itemRef, UseDefaultIID());
		if (placedArticleData && placedArticleData->GetUniqueId() != kNullString) {

			// Get all item of carton from target story.
			IDataBase * db = itemRef.GetDataBase();
			if (!db)
				break;

			UIDList formeItems(db);
			if (Utils<IXPageUtils>()->GetCartonItemsFromTarget(itemRef, formeItems) != kSuccess) // Apapap Message alert
				break;

			PMString idArt = placedArticleData->GetUniqueId();
			PMString topicArt = placedArticleData->GetStoryTopic();

			PMString pathArt = placedArticleData->GetStoryFolder();
			IDFile xmlFileToImport = FileUtils::PMStringToSysFile(pathArt);
			FileUtils::AppendPath(&xmlFileToImport, idArt + ".OBJRART.xml");
			if (!FileUtils::DoesFileExist(xmlFileToImport)) // Apapap Message alert
				break;

			IDFile cheatFile = FileUtils::PMStringToSysFile(pathArt);
			IDFile matchingFile;
			InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
			FileUtils::AppendPath(&cheatFile, "cheatforme.xml");
			if (FileUtils::DoesFileExist(cheatFile)) { // Apapap Message alert
				matchingFile = cheatFile;
			}
			else {
				// Get forme data
				InterfacePtr<IFormeData> formeData(itemRef, UseDefaultIID());
				if (formeData == nil)// Apapap Message alert
					break;

				PMString formeDescription = formeData->GetUniqueName();
				if (formeDescription == kNullString)
					break;

				// Get cartn and classeur from unique carton name
				PMString classeur, carton;
				Utils<IXPageUtils>()->SplitFormeDescription(formeDescription, classeur, carton);

				// Try to use matching file associated with the story if any	
				FileUtils::AppendPath(&matchingFile, xpgPrefs->GetCheminFormes());
				FileUtils::AppendPath(&matchingFile, classeur);
				FileUtils::AppendPath(&matchingFile, "Typographie");
				FileUtils::AppendPath(&matchingFile, carton + ".xml");
				if (!FileUtils::DoesFileExist(matchingFile)) // Apapap Message alert
					break;
			}

			UIDList targetStories(db);
			K2Vector<UIDRef> targetPictures;
			for (int32 i = 0; i < formeItems.size(); ++i) {
                // GD 27.04.2023 ++
                UIDRef parentUID = formeItems.GetRef(i);
                InterfacePtr<IFormeData> formeData(parentUID, UseDefaultIID());
                if (formeData != nil) {
                    if (formeData->GetType() == IFormeData::kFixedContent)
                        continue;
                }
                // GD 27.04.2023 --
				InterfacePtr<IFrameType> frameType(db, formeItems[i], UseDefaultIID());
				if (frameType && frameType->IsTextFrame()) {
					InterfacePtr<IHierarchy> itemHier(frameType, UseDefaultIID());
					InterfacePtr<IMultiColumnTextFrame> txtFrame(db, itemHier->GetChildUID(0), UseDefaultIID());
					targetStories.Append(txtFrame->GetTextModelUID());
				}
				else if (frameType && frameType->IsGraphicFrame()) {
					targetPictures.push_back(::GetUIDRef(frameType));
				}
			}

			PMString errorImport = kNullString;
			if (Utils<IXPageUtils>()->ImportArticle(targetStories, xmlFileToImport, matchingFile, errorImport, xmlFileToImport, idArt) != kSuccess) {
				CAlert::InformationAlert(errorImport);
				break;
			}

			errorImport = kNullString;
			if (Utils<IXPageUtils>()->ImportImages(targetPictures, xmlFileToImport, errorImport) != kSuccess) {
				CAlert::InformationAlert(errorImport);
				break;
			}


			errorCode = kSuccess;
		}
	} while (kFalse);
	return errorCode;
}

ErrorCode XPGUIXRailPanelObserver::UpdatePlacedStory2(const UIDRef& itemRef, int32& idPage) {

	ErrorCode errorCode = kFailure;
	do {
		InterfacePtr<IPlacedArticleData> placedArticleData(itemRef, UseDefaultIID());
		if (placedArticleData && placedArticleData->GetUniqueId() != kNullString) {

			// Get all item of carton from target story.
			IDataBase * db = itemRef.GetDataBase();
			if (!db)
				break;

			UIDList formeItems(db);
			Utils<IXPageUtils>()->GetPlacedArtItemsFromTarget(itemRef, formeItems);
			//if (Utils<IXPageUtils>()->GetCartonItemsFromTarget(itemRef, formeItems) != kSuccess) // Apapap Message alert
			//	break;

			PMString idArt = placedArticleData->GetUniqueId();
			//alert de l'id de l'article

			if (CanUpdateStory(idArt) == 0) {
				CAlert::InformationAlert("Impossible de mettre à jour, l'article n'est pas dans le bon état");
				break;
			}
			PMString topicArt = placedArticleData->GetStoryTopic();

			PMString pathArt = placedArticleData->GetStoryFolder();
			IDFile xmlFileToImport = FileUtils::PMStringToSysFile(pathArt);
			FileUtils::AppendPath(&xmlFileToImport, idArt + ".OBJRART.xml");
			if (!FileUtils::DoesFileExist(xmlFileToImport)) // Apapap Message alert
				break;

			IDFile cheatFile = FileUtils::PMStringToSysFile(pathArt);
			IDFile matchingFile;
			InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
			FileUtils::AppendPath(&cheatFile, "cheatforme.xml");
			if (FileUtils::DoesFileExist(cheatFile)) { // Apapap Message alert
				matchingFile = cheatFile;
			}
			else {
				// Get forme data
				InterfacePtr<IFormeData> formeData(itemRef, UseDefaultIID());
				if (formeData == nil)// Apapap Message alert
					break;

				PMString formeDescription = formeData->GetUniqueName();
				if (formeDescription == kNullString)
					break;

				// Get cartn and classeur from unique carton name
				PMString classeur, carton;
				Utils<IXPageUtils>()->SplitFormeDescription(formeDescription, classeur, carton);

				// Try to use matching file associated with the story if any	
				FileUtils::AppendPath(&matchingFile, xpgPrefs->GetCheminFormes());
				FileUtils::AppendPath(&matchingFile, classeur);
				FileUtils::AppendPath(&matchingFile, "Typographie");
				FileUtils::AppendPath(&matchingFile, carton + ".xml");
				//CAlert::InformationAlert(FileUtils::SysFileToPMString(matchingFile));
				if (!FileUtils::DoesFileExist(matchingFile)) // Apapap Message alert
					break;
			}

			UIDList targetStories(db);
			K2Vector<UIDRef> targetPictures;
			for (int32 i = 0; i < formeItems.size(); ++i) {
                // GD 27.04.2023 ++
                UIDRef parentUID = formeItems.GetRef(i);
                InterfacePtr<IFormeData> formeData(parentUID, UseDefaultIID());
                if (formeData != nil) {
                    if (formeData->GetType() == IFormeData::kFixedContent)
                        continue;
                }
                // GD 27.04.2023 --
				InterfacePtr<IFrameType> frameType(db, formeItems[i], UseDefaultIID());
				if (frameType && frameType->IsTextFrame()) {
					InterfacePtr<IHierarchy> itemHier(frameType, UseDefaultIID());
					InterfacePtr<IMultiColumnTextFrame> txtFrame(db, itemHier->GetChildUID(0), UseDefaultIID());
					targetStories.Append(txtFrame->GetTextModelUID());
				}
				else if (frameType && frameType->IsGraphicFrame()) {
					targetPictures.push_back(::GetUIDRef(frameType));
				}
			}

			PMString errorImport = kNullString;
			if (Utils<IXPageUtils>()->ImportArticle(targetStories, xmlFileToImport, matchingFile, errorImport, xmlFileToImport, idArt) != kSuccess) {
				CAlert::InformationAlert(errorImport);
				break;
			}
			
            errorImport = kNullString;
            if(Utils<IXPageUtils>()->ImportImages(targetPictures, xmlFileToImport, errorImport) != kSuccess){
                CAlert::InformationAlert(errorImport);
                break;
            }	
            
            if (xpgPrefs->GetNoUpdateFile() != "1") {
			//on cree un fichier pour dire que le idms a ete mis a jour
			IDFile updateFile = FileUtils::PMStringToSysFile(pathArt + idArt + ".update");
			InterfacePtr<IPMStream> writeStream(StreamUtil::CreateFileStreamWrite(updateFile, kOpenOut | kOpenTrunc, kTextType));
			if (writeStream == nil)
			{
				ASSERT_FAIL("PrsReverseTCL::SetOutputFile -> writeStream nil");
				break;
			}
            }
			errorCode = kSuccess;
            
		}
	} while (kFalse);
	return errorCode;
}

ErrorCode XPGUIXRailPanelObserver::UpdateInCopyStory(const UIDRef& itemRef, int32& idPage) {

	ErrorCode errorCode = kFailure;

	do {
		// Get assignment related to this story
		InterfacePtr<IAssignmentMgr> assignMgr(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IAssignment> assignment(nil);
		InterfacePtr<IAssignedStory> assignedStory(nil);

		assignMgr->QueryAssignmentAndAssignedStory(itemRef, assignment, assignedStory);
		if (!assignment || !assignedStory)
		{
			break;
		}

		UIDList storyList;
		assignMgr->GetStoryListOnAssignment(assignment, storyList);
		IDataBase *db = itemRef.GetDataBase();
		if (!db)
		{
			break;
		}

		// Process update CheckOut stories with commands
		if (Utils<Facade::ILiveEditFacade>()->UpdateStoryLink(storyList) != kSuccess)
		{
			break;
		}


		// Check Out the selected assignment after update
		errorCode = Utils<Facade::ILiveEditFacade>()->CheckOut(storyList, ICommand::kRegularUndo, nil, kFalse, kFullUI);

		errorCode = kSuccess;
	} while (kFalse);

	return errorCode;
}

void XPGUIXRailPanelObserver::DisplayAdornmentShape(bool16 visible) {

	do {

		InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IWebServices> baseHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

		IDocument * theDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if (theDoc == nil)
			break;

		InterfacePtr<ILayerList> layerList(theDoc, UseDefaultIID());
		InterfacePtr<IGuidePrefs> guidePrefs(theDoc->GetDocWorkSpace(), UseDefaultIID());
		int32 nbLayers = layerList->GetCount();
		int32 startContentLayer, endContentLayer;

		if (guidePrefs && guidePrefs->GetGuidesInBack()) {
			startContentLayer = nbLayers + 1;
			endContentLayer = (2 * nbLayers) - 1;
		}
		else {
			startContentLayer = 2;
			endContentLayer = nbLayers;
		}

		IDataBase * db = ::GetDataBase(theDoc);
		K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > > exportableStories;
		K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > > itemsByArticle;
		UIDList alreadyAddedStories(db);

		InterfacePtr<ISpreadList> spreadList(theDoc, UseDefaultIID());
		int32 nbSpreads = spreadList->GetSpreadCount();
		for (int32 i = 0; i < nbSpreads; ++i) {
			InterfacePtr<IHierarchy> spreadHier(db, spreadList->GetNthSpreadUID(i), UseDefaultIID());
			for (int32 j = startContentLayer; j <= endContentLayer; ++j) {
				InterfacePtr<IHierarchy> spreadLayerHier(db, spreadHier->GetChildUID(j), UseDefaultIID());
				int32 nbItems = spreadLayerHier->GetChildCount();
				for (int32 itemIndex = 0; itemIndex < nbItems; ++itemIndex) {
					InterfacePtr<IHierarchy> itemHier(db, spreadLayerHier->GetChildUID(itemIndex), UseDefaultIID());
					HandleItem(itemHier, exportableStories, itemsByArticle, alreadyAddedStories);
				}
			}
		}

		// Add placed stories
		K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > > placedStories = Utils<IXPageUtils>()->GetPlacedStoriesList(theDoc);
		for (int32 i = 0; i < placedStories.size(); ++i)
			itemsByArticle.push_back(placedStories[i]);


		for (int32 i = 0; i < itemsByArticle.size(); ++i) {

			UIDList articleItems = itemsByArticle[i].Value().second;
			if (visible) {

				PMString section = kNullString, incopyPath = kNullString, ssSection = kNullString, currentStatus = kNullString,
					colorStatus = kNullString, sujet = kNullString, proprio = kNullString, result = kNullString;

				PMString artID = itemsByArticle[i].Key();
				PMString artPath = itemsByArticle[i].Value().first;

				// si le fichier contient "-", on supprime le versionning 
				if (artID.Contains("-")) {
					int32 extPos = artID.LastIndexOfCharacter(PlatformChar('-'));
					if (extPos != -1)
						artID.Remove(extPos, artID.NumUTF16TextChars() - extPos);
				}
				if (!baseHTTP->GetStoryInfo_v2(xpgPrefs->GetTEC_URL(), artID, colorStatus, sujet)) {
					CAlert::InformationAlert("Impossible de récupérer les informations de l'article");
					continue;
				}

				// Save article data
				/*InterfacePtr<ICommand> placedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
				InterfacePtr<IPlacedArticleData> placedArticleData(placedArticleDataCmd, IID_IPLACEDARTICLEDATA);
				placedArticleData->SetStoryTopic(sujet);
				placedArticleDataCmd->SetItemList(articleItems);
				if (CmdUtils::ProcessCommand(placedArticleDataCmd) != kSuccess) {
					continue;
				}*/

				// Store status of story
				InterfacePtr<ICommand> labelCommand(CmdUtils::CreateCommand(kXPGSetArticleDataCmdBoss));
				labelCommand->SetItemList(articleItems);
				InterfacePtr<IArticleData> labelData(labelCommand, UseDefaultIID());
				labelData->SetStatus(colorStatus);
				CmdUtils::ProcessCommand(labelCommand);

				// Show adornment
				InterfacePtr<ICommand> addPageItemAdornmentCmd(CmdUtils::CreateCommand(kAddPageItemAdornmentCmdBoss));
				InterfacePtr<IClassIDData> classIDData(addPageItemAdornmentCmd, UseDefaultIID());
				classIDData->Set(kXPGUIArticleAdornmentBoss);
				addPageItemAdornmentCmd->SetItemList(articleItems);
				if (CmdUtils::ProcessCommand(addPageItemAdornmentCmd) != kSuccess)
					break;
			}
			else {
				InterfacePtr<ICommand> removePageItemAdornmentCmd(CmdUtils::CreateCommand(kRemovePageItemAdornmentCmdBoss));
				InterfacePtr<IClassIDData>  classIDData(removePageItemAdornmentCmd, UseDefaultIID());
				classIDData->Set(kXPGUIArticleAdornmentBoss);
				removePageItemAdornmentCmd->SetItemList(articleItems);
				if (CmdUtils::ProcessCommand(removePageItemAdornmentCmd) != kSuccess)
					break;
			}
		}
		// Refresh the views of a document
		Utils<ILayoutUIUtils>()->InvalidateViews(theDoc);

		// Notify IID_IDISPLAYSHAPE
		InterfacePtr<IBoolData> isAdronmentShape(GetExecutionContextSession(), IID_IDISPLAYSHAPE);
		isAdronmentShape->Set(visible);

	} while (kFalse);
}

void XPGUIXRailPanelObserver::RefreshAdornmentForme() {

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	IControlView* displayShapeCtrlView = panelControlData->FindWidget(kXPGUIDisplayFormesWidgetID);
	InterfacePtr<ITriStateControlData> displayShapeValue(displayShapeCtrlView, UseDefaultIID());

	// Re-apply the current checkbox state (shared code) and re-sync the
	// sibling palette.
	XPageUIUtils::SetDisplayFormes(displayShapeValue->IsSelected());
}

// Reflect the shared session "Afficher les formes" state on our own checkbox,
// without firing the toggle action again (notifyOfChange = kFalse).
void XPGUIXRailPanelObserver::SyncDisplayFormesCheckbox() {

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	InterfacePtr<ITriStateControlData> displayFormesControlData(panelControlData->FindWidget(kXPGUIDisplayFormesWidgetID), UseDefaultIID());
	if (displayFormesControlData)
		displayFormesControlData->SetState(
			XPageUIUtils::GetDisplayFormeState() ? ITriStateControlData::kSelected : ITriStateControlData::kUnselected,
			kTrue, kFalse);
}


// Per-frame helper: does this page item carry a Gaia identifier?
//   - Text/InCopy frames: IPlacedArticleData::GetUniqueId() non-empty.
//   - Graphic / forme frames: IFormeData::GetUniqueName() non-empty.
// NOT recursive on its own — sCountGaiaIDs handles groups.
static bool16 sHasGaiaIDOnLeaf(const UIDRef& itemRef)
{
	InterfacePtr<IPlacedArticleData> placed(itemRef, UseDefaultIID());
	if (placed != nil && placed->GetUniqueId() != kNullString)
		return kTrue;

	InterfacePtr<IFormeData> forme(itemRef, UseDefaultIID());
	if (forme != nil && forme->GetUniqueName() != kNullString)
		return kTrue;

	return kFalse;
}

// Recursively count leaf items (going through InDesign groups, which
// themselves carry no Gaia ID) and how many of them carry a Gaia ID. The
// surrounding HandleSelectionChanged uses these counts to decide which of
// the four action buttons should be enabled.
static void sCountGaiaIDs(const UIDRef& itemRef, int32& total, int32& withGaia)
{
	InterfacePtr<IHierarchy> hier(itemRef, UseDefaultIID());
	if (hier != nil && Utils<IPageItemTypeUtils>()->IsGroup(hier))
	{
		const int32 nbChildren = hier->GetChildCount();
		for (int32 i = 0; i < nbChildren; ++i)
		{
			InterfacePtr<IHierarchy> childHier(hier->QueryChild(i));
			if (childHier != nil)
				sCountGaiaIDs(::GetUIDRef(childHier), total, withGaia);
		}
		return;
	}

	++total;
	if (sHasGaiaIDOnLeaf(itemRef))
		++withGaia;
}


void XPGUIXRailPanelObserver::HandleSelectionChanged(const ISelectionMessage * message)
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	// === Step 1 — global gate ===
	// The whole palette is only "intervenable" when there is a front
	// document AND its active page has a non-zero XRail ID. When the gate
	// is closed, every interactive widget gets greyed out.
	const bool16 paletteUsable = this->IsPaletteUsable();

	// === Step 2 — selection-driven button rules (only relevant when gate open) ===
	// Ajouter / Ajouter à : enabled iff selection non-empty AND no LEAF item has a Gaia ID.
	// MAJ                 : enabled iff selection non-empty AND every LEAF item has a Gaia ID.
	// Délier              : enabled as soon as selection is non-empty (it only
	//                       clears article info, no-ops on blocks without one).
	// Groups are walked transparently — a group of two ID'd frames behaves
	// the same as the two frames being selected directly.
	bool16 enableNew    = kFalse;
	bool16 enableAddTo  = kFalse;
	bool16 enableUnlink = kFalse;
	bool16 enableUpdate = kFalse;

	if (paletteUsable)
	{
		InterfacePtr<ISelectionManager> selectionMgr(Utils<ISelectionUtils>()->QueryActiveSelection());
		InterfacePtr<IXPageSuite> xpgSuite(selectionMgr, IID_IXPAGESUITE);
		if (xpgSuite != nil)
		{
			UIDList textFrames, graphicFrames, selectedItems;
			xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);

			int32 total = 0, withGaia = 0;
			for (int32 i = 0; i < selectedItems.Length(); ++i)
				sCountGaiaIDs(selectedItems.GetRef(i), total, withGaia);

			if (total > 0)
			{
				enableUnlink = kTrue;	// Délier : actif dès qu'au moins un bloc est sélectionné
				if (withGaia == 0)        { enableNew = kTrue; enableAddTo = kTrue; }
				if (withGaia == total)    { enableUpdate = kTrue; }
			}
		}
	}

	// === Step 3 — apply states ===
	// Action buttons.
	IControlView* btnNew    = panelControlData->FindWidget(kXPGUICreateIncopyWidgetID);
	IControlView* btnAddTo  = panelControlData->FindWidget(kXPGUIAddToAssignmentButtonWidgetID);
	IControlView* btnUnlink = panelControlData->FindWidget(kXPGUIDeleteAssignmentWidgetID);
	IControlView* btnUpdate = panelControlData->FindWidget(kXPGUIMAJArtButtonWidgetID);
	if (btnNew)    { btnNew->Enable(enableNew);       btnNew->Validate(); }
	if (btnAddTo)  { btnAddTo->Enable(enableAddTo);   btnAddTo->Validate(); }
	if (btnUnlink) { btnUnlink->Enable(enableUnlink); btnUnlink->Validate(); }
	if (btnUpdate) { btnUpdate->Enable(enableUpdate); btnUpdate->Validate(); }

	// Dropdowns + the two adornment-toggle buttons (Display formes /
	// Display story states / their refresh siblings) also follow the gate
	// — they're meaningless without an active Gaia page.
	IControlView* ddSection         = panelControlData->FindWidget(kXPGUISectionListWidgetID);
	IControlView* ddSsSection       = panelControlData->FindWidget(kXPGUISsSectionListWidgetID);
	IControlView* ddParution        = panelControlData->FindWidget(kXPGUIParutionListWidgetID);
	IControlView* btnDispFormes     = panelControlData->FindWidget(kXPGUIDisplayFormesWidgetID);
	IControlView* btnDispStates     = panelControlData->FindWidget(kXPGUIDisplayStoryStatesWidgetID);
	IControlView* btnRefreshFormes  = panelControlData->FindWidget(kXPGUIRefreshFormesAdornmentWidgetID);
	IControlView* btnRefreshAdorn   = panelControlData->FindWidget(kXPGUIRefreshAdornmentWidgetID);
	if (ddSection)        ddSection->Enable(paletteUsable);
	if (ddSsSection)      ddSsSection->Enable(paletteUsable);
	if (ddParution)       ddParution->Enable(paletteUsable);
	if (btnDispFormes)    btnDispFormes->Enable(paletteUsable);
	if (btnDispStates)    btnDispStates->Enable(paletteUsable);
	if (btnRefreshFormes) btnRefreshFormes->Enable(paletteUsable);
	if (btnRefreshAdorn)  btnRefreshAdorn->Enable(paletteUsable);

	// === Step 4 — palette-display reload ===
	// If the active document changed since the last reload (palette just
	// became visible after a doc switch, palette opened with a doc already
	// active, etc.), refresh the parutions/rubriques cascade. Page nav
	// inside the same doc keeps fLastReloadedDocUID and is therefore a
	// no-op here, exactly what the user asked for.
	IDocument* curDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	const UID curDocUID = (curDoc != nil) ? ::GetUID(curDoc) : kInvalidUID;
	if (curDocUID != fLastReloadedDocUID)
	{
		fLastReloadedDocUID = curDocUID;
		this->lastPageID = -1;	// invalidate FillParutionList's cache so it reloads
		this->FillParutionList();
	}

	// Refresh the overlay (login / no-Gaia-page / content) since the page
	// XRail-ID gate may have flipped — e.g. user just opened a doc, or
	// switched to a doc whose page has no XRail ID.
	this->UpdateLoginState();
}

/*
 / In param : itemHier
 Out param : exportableStories, itemsByArticle, alreadyAddedStories
 */

void XPGUIXRailPanelObserver::HandleItem(IHierarchy * itemHier,
	K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > >& exportableStories,
	K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > >& itemsByArticle,
	UIDList& alreadyAddedStories)
{

	if (Utils<IPageItemTypeUtils>()->IsGroup(itemHier)) {
		// If it's a group, handle children item recursively
		for (int32 i = 0; i < itemHier->GetChildCount(); ++i) {
			InterfacePtr<IHierarchy> childHier(itemHier->QueryChild(i));
			HandleItem(childHier, exportableStories, itemsByArticle, alreadyAddedStories);
		}
	}
	else {

		// Check out item if is an InCopy story
		UIDRef itemRef = ::GetUIDRef(itemHier);
		PMString artID = kNullString, artPath = kNullString;
		UIDRef storyRef = itemRef;
		Utils<IAssignmentUtils>()->NormalizeTextModelRef(storyRef);
		if (Utils<IStoryUtils>()->IsAStory(storyRef)) {

			// Figure the related article ID from the Incopy file path
			InterfacePtr<IAssignment> assignment(nil);
			InterfacePtr<IAssignedStory> assignedStory(nil);
			InterfacePtr<IAssignmentMgr> assignMgr(GetExecutionContextSession(), UseDefaultIID());
			assignMgr->QueryAssignmentAndAssignedStory(storyRef, assignment, assignedStory);

			if (assignment) {
				IDFile inCopyFile = FileUtils::PMStringToSysFile(assignment->GetFile());
				artID = inCopyFile.GetFileName();
				artID.Truncate(5); // Remove icma extension	
				artPath = assignment->GetFile();
			}
		}
#if !INCOPY
		else {
			InterfacePtr<IStringListData> selectedArticleID(storyRef, IID_IARTICLEINFO);
			if (selectedArticleID) { // That means the item is a text frame				
				// Check wether an article was imported into this frame
				K2Vector<PMString> articleInfo = selectedArticleID->GetStringList();
				if (!articleInfo.empty()) {
					artID = articleInfo[0]; // article ID is the first stored information 
					artPath = articleInfo[1]; // article path is the second stored information 
				}
			}
		}
#endif

		// Add item to the lists
		//if(!artID.IsEmpty() && alreadyAddedStories.DoesNotContain(storyRef.GetUID()))
		if (!artID.IsEmpty()) {
			alreadyAddedStories.Append(storyRef.GetUID());
			// Add item to the group of items it belongs to
			K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > >::iterator iter = ::K2find(itemsByArticle, artID);
			if (iter == itemsByArticle.end())
				itemsByArticle.push_back(KeyValuePair<PMString, K2Pair<PMString, UIDList> >(artID, K2Pair<PMString, UIDList>(artPath, UIDList(itemRef))));
			else
				iter->Value().second.Append(itemRef);

			// If it's a text frame, add it too to the exportable stories list
			if (Utils<IPageItemTypeUtils>()->IsTextFrame(itemRef)) {
				Utils<IAssignmentUtils>()->NormalizeTextModelRef(itemRef);
				K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > >::iterator iterExportable = ::K2find(exportableStories, artID);
				if (iterExportable == exportableStories.end())
					exportableStories.push_back(KeyValuePair<PMString, K2Pair<PMString, UIDList> >(artID, K2Pair<PMString, UIDList>(artPath, UIDList(itemRef))));
				else
					iterExportable->Value().second.Append(itemRef);
			}
		}
	}
}

void XPGUIXRailPanelObserver::FillSectionList(int32 idtitre)
{
	IDValueList filterList;

	InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IWebServices> baseHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

	do {

		// Snapshot the rubrique displayed before we wipe the list — so we
		// can re-select the same rubrique by name after the reload (the
		// numeric ID may differ across titres but the user-visible name is
		// what they actually expect to keep).
		PMString priorRubricName;
		{
			InterfacePtr<IPanelControlData> panelCtl(this, UseDefaultIID());
			if (panelCtl != nil)
			{
				InterfacePtr<IDropDownListController> rubCtl(
					panelCtl->FindWidget(kXPGUISectionListWidgetID), UseDefaultIID());
				InterfacePtr<IStringListControlData> rubData(rubCtl, UseDefaultIID());
				if (rubCtl != nil && rubData != nil)
				{
					int32 sel = rubCtl->GetSelected();
					if (sel != IDropDownListController::kNoSelection &&
					    sel >= 0 && sel < rubData->Length())
					{
						priorRubricName = rubData->GetString(sel);
					}
				}
			}
		}

		K2Vector<PMString> rubsnames;
		K2Vector<int32> rubsids;
		if (!baseHTTP->GetListeRub_v2(xpgPrefs->GetTEC_URL(), idtitre, rubsnames, rubsids))
		{
			CAlert::InformationAlert("Impossible de récupérer la liste des rubriques");
			break;
		}

		for (int32 i = 0; i < rubsnames.size(); ++i) {
			PMString name = rubsnames[i];
			PMString monID = "";
			monID.AppendNumber(rubsids[i]);
			filterList.push_back(IDValuePair(monID, name));
		}

		// Fill drop down list widget
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
			break;

		XPageUIUtils::FillDropdownlist(panelControlData, kXPGUISectionListWidgetID, filterList, kFalse);

		// Try to restore the previously-selected rubrique by name. If the
		// new list doesn't contain it, FillDropdownlist already pre-selected
		// the first item for us.
		if (!priorRubricName.IsEmpty())
		{
			InterfacePtr<IDropDownListController> rubCtl(
				panelControlData->FindWidget(kXPGUISectionListWidgetID), UseDefaultIID());
			InterfacePtr<IStringListControlData> rubData(rubCtl, UseDefaultIID());
			if (rubCtl != nil && rubData != nil)
			{
				for (int32 i = 0; i < rubData->Length(); ++i)
				{
					if (rubData->GetString(i) == priorRubricName)
					{
						// notify=kFalse so we don't re-cascade — we'll call
						// FillSsSectionList ourselves right below.
						rubCtl->Select(i, kTrue, kFalse);
						break;
					}
				}
			}
		}

		// Fill second filter list (preserves its own selection internally).
		this->FillSsSectionList();

	} while (kFalse);
}

void XPGUIXRailPanelObserver::FillSsSectionList()
{
	IDValueList filterList;
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

	// Snapshot the previously-selected sous-rubrique by name, so we can
	// restore it after the reload (same rationale as in FillSectionList).
	PMString priorSubRubricName;
	if (panelControlData != nil)
	{
		InterfacePtr<IDropDownListController> ssCtl(
			panelControlData->FindWidget(kXPGUISsSectionListWidgetID), UseDefaultIID());
		InterfacePtr<IStringListControlData> ssData(ssCtl, UseDefaultIID());
		if (ssCtl != nil && ssData != nil)
		{
			int32 sel = ssCtl->GetSelected();
			if (sel != IDropDownListController::kNoSelection &&
			    sel >= 0 && sel < ssData->Length())
			{
				priorSubRubricName = ssData->GetString(sel);
			}
		}
	}

	do
	{
		InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IWebServices> baseHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

		// Get select first filter element
		InterfacePtr<IDropDownListController> firstFilterListController(panelControlData->FindWidget(kXPGUISectionListWidgetID), UseDefaultIID());
		if (firstFilterListController == nil)
			break;

		InterfacePtr<IStringListData> firstFilterIDList(firstFilterListController, IID_IIDLIST);
		if (firstFilterIDList == nil)
			break;

		int32 selectedFilterIndex = firstFilterListController->GetSelected();
		if (selectedFilterIndex == IDropDownListController::kNoSelection)
			break;


		K2Vector<PMString> ssrubsnames;
		K2Vector<int32> ssrubsids;
		bool connexionOK = baseHTTP->GetListeSSRub_v2(xpgPrefs->GetTEC_URL(), firstFilterIDList->GetStringList()[selectedFilterIndex], ssrubsnames, ssrubsids);
		if (!connexionOK)
		{
			CAlert::InformationAlert("Impossible de récupérer la liste des sous rubriques");
			break;
		}

		for (int32 i = 0; i < ssrubsnames.size(); ++i) {
			PMString name = ssrubsnames[i];
			PMString monID = "";
			monID.AppendNumber(ssrubsids[i]);
			filterList.push_back(IDValuePair(monID, name));
		}


		// Add "none" item at first position
		filterList.insert(filterList.begin(), IDValuePair(kNullString, PMString(kXPGUINoneTextKey, PMString::kTranslateDuringCall)));

	} while (kFalse);

	// Fill drop down list widget
	XPageUIUtils::FillDropdownlist(panelControlData, kXPGUISsSectionListWidgetID, filterList, kFalse);

	// Restore the previously-selected sous-rubrique by name if it still
	// exists in the new list. Otherwise FillDropdownlist's first-item
	// pre-selection wins.
	if (panelControlData != nil && !priorSubRubricName.IsEmpty())
	{
		InterfacePtr<IDropDownListController> ssCtl(
			panelControlData->FindWidget(kXPGUISsSectionListWidgetID), UseDefaultIID());
		InterfacePtr<IStringListControlData> ssData(ssCtl, UseDefaultIID());
		if (ssCtl != nil && ssData != nil)
		{
			for (int32 i = 0; i < ssData->Length(); ++i)
			{
				if (ssData->GetString(i) == priorSubRubricName)
				{
					ssCtl->Select(i, kTrue, kFalse);
					break;
				}
			}
		}
	}

	// Fill Tree
	this->HandleParutionSelection();
}


void XPGUIXRailPanelObserver::FillParutionList() {

	IDValueList filterList;

	InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IWebServices> baseHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
	do {
		// Fill drop down list widget
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
			break;

		int32 idPage = Utils<IXPageUtils>()->GetPageId();

		// Charger la liste des parution en cas de changement
		if (lastPageID == idPage) {
			this->DisplayInfoPage("", "");
			break;
		}
		
		// Save last parution id
		this->lastPageID = idPage;
		if (idPage == -1) {
			this->DisplayInfoPage("", "");
			// Display info page
			break;
		}

		int32 titreID;
		PMString titreLib = kNullString, editionLib = kNullString, parutionLib = kNullString;

		// Recuperer l'id titre en fonction de l'id de la page
		if (!baseHTTP->GetPageInfos_v2(xpgPrefs->GetTEC_URL(), idPage, titreID, titreLib, parutionLib)) {
			CAlert::InformationAlert("Impossible de récupérer les informations de la page");
			break;
		}
		// Display info page
		this->DisplayInfoPage(titreLib, parutionLib);

		// Display user name
		this->DisplayCurrentUser();

		
		K2Vector<PMString> parusnames;
		K2Vector<int32> parusids;
		bool connexionOK = baseHTTP->GetListeParu_v2(xpgPrefs->GetTEC_URL(), titreID, parusnames, parusids);
		if (!connexionOK)
		{
			CAlert::InformationAlert("Impossible de récupérer la liste des parutions");
			break;
		}

		for (int32 i = 0; i < parusnames.size(); ++i) {
			PMString paruname = parusnames[i];
			PMString parutionID = "";
			parutionID.AppendNumber(parusids[i]);
			filterList.push_back(IDValuePair(parutionID, paruname));
		}

		XPageUIUtils::FillDropdownlist(panelControlData, kXPGUIParutionListWidgetID, filterList, kFalse);
		this->FillSectionList(titreID);

	} while (kFalse);
}


void XPGUIXRailPanelObserver::HandleParutionSelection() {
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	do {
		this->FillParutionStories();
	} while (kFalse);
}

void  XPGUIXRailPanelObserver::FillParutionStories() {

	do {
		// Section et sous Section
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		InterfacePtr<IDropDownListController> firstFilter(panelControlData->FindWidget(kXPGUISectionListWidgetID), UseDefaultIID());
		InterfacePtr<IDropDownListController> secondFilter(panelControlData->FindWidget(kXPGUISsSectionListWidgetID), UseDefaultIID());
		InterfacePtr<IDropDownListController> parutionFilter(panelControlData->FindWidget(kXPGUIParutionListWidgetID), UseDefaultIID());

		InterfacePtr<IStringListData> firstFilterIDs(firstFilter, IID_IIDLIST);
		InterfacePtr<IStringListData> secondFilterIDs(secondFilter, IID_IIDLIST);
		InterfacePtr<IStringListData> parutionFilterIDs(parutionFilter, IID_IIDLIST);

		PMString rubID = kNullString, ssrubID = kNullString, parutionID = kNullString;
		// Get selected section item if any
		if (firstFilterIDs->GetStringList().size() > 0) {
			int32 selectedIndex = firstFilter->GetSelected();
			if (selectedIndex != IDropDownListController::kNoSelection) {
				rubID = firstFilterIDs->GetStringList()[selectedIndex];
			}
		}
		// Get selected sub section item if any	
		if (secondFilterIDs->GetStringList().size() > 0) {
			int32 selectedIndex = secondFilter->GetSelected();
			if (selectedIndex != IDropDownListController::kNoSelection) {
				ssrubID = secondFilterIDs->GetStringList()[selectedIndex];
			}
		}

		// Get selected parution section item if any	
		if (parutionFilterIDs->GetStringList().size() > 0) {
			int32 selectedIndex = parutionFilter->GetSelected();
			if (selectedIndex != IDropDownListController::kNoSelection) {
				parutionID = parutionFilterIDs->GetStringList()[selectedIndex];
			}
		}

		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IArticleDataModel> model(pageMrgAccessor->QueryArticleDataModel());
		model->Rebuild(parutionID, rubID, ssrubID);

		InterfacePtr<ITreeViewMgr> treeViewMgr(panelControlData->FindWidget(kXPGUIArticleViewWidgetID), UseDefaultIID());
		treeViewMgr->ClearTree();
		treeViewMgr->ChangeRoot(kTrue);

		InterfacePtr<ITreeViewHierarchyAdapter> articleHierAdapter(treeViewMgr, UseDefaultIID());
		NodeID articleRootNode = articleHierAdapter->GetRootNode();
		if (articleHierAdapter->GetNumChildren(articleRootNode) > 0)
		{
			InterfacePtr<ITreeViewController> articleListController(articleHierAdapter, UseDefaultIID());
			articleListController->Select(articleHierAdapter->GetNthChild(articleHierAdapter->GetRootNode(), 0));
		}
		else
		{
			this->ResetStoryInfosWidgets();
		}

	} while (kFalse);
}

/**
 / Display current user
 */
void XPGUIXRailPanelObserver::DisplayCurrentUser() {

	do {
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
			break;

		InterfacePtr<ITextControlData> currentUserTextCtrlData(panelControlData->FindWidget(kXPGUICurrentUserWidgetID), UseDefaultIID());
		if (currentUserTextCtrlData == nil)
			break;

		InterfacePtr<IStringData> currentUserData(GetExecutionContextSession(), IID_ILOGINSTRING);
		if (currentUserData == nil)
			break;

		PMString currentUser = currentUserData->GetString();
		currentUser.SetTranslatable(kFalse);
		currentUserTextCtrlData->SetString(currentUser);

	} while (kFalse);
}

/**
 / Display info about current page
 */
void XPGUIXRailPanelObserver::DisplayInfoPage(const PMString& titreLib, const PMString& parutionLib) {

	do {
		PMString infoPage = kNullString;
		infoPage.Append(titreLib);
		infoPage.Append(kTextChar_LF);  // Separator
		infoPage.Append(parutionLib);
		infoPage.SetTranslatable(kFalse);

		// Display info page
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
			break;

		InterfacePtr<ITextControlData> infoPageCtrlData(panelControlData->FindWidget(kXPGUIInfoPageWidgetID), UseDefaultIID());
		if (infoPageCtrlData == nil)
			break;

		infoPageCtrlData->SetString(infoPage);

	} while (kFalse);
}


ErrorCode XPGUIXRailPanelObserver::DeleteAssignment(IAssignment* assignment) {

	ErrorCode result = kFailure;
	do {
		if (!assignment)
			break;

		IDataBase *db = ::GetDataBase(assignment);
		PMString fromAssignPath = assignment->GetFile();
		IAssignedDocumentPtr assignDoc(db, db->GetRootUID(), UseDefaultIID());

		// remove assignment from document
		InterfacePtr<ICommand> unassignDocCmd(CmdUtils::CreateCommand(kUnassignDocCmdBoss));
		UIDList uidl(::GetUIDRef(assignDoc));
		unassignDocCmd->SetItemList(uidl);
		InterfacePtr<IStringData> sd(unassignDocCmd, UseDefaultIID());
		sd->Set(fromAssignPath);
		result = CmdUtils::ProcessCommand(unassignDocCmd);

		InterfacePtr<IAssignmentMgr> assMgr(GetExecutionContextSession(), UseDefaultIID());
		assMgr->NotifyPanel();

	} while (false);
	return result;
}

int32 XPGUIXRailPanelObserver::CanUpdateStory(const PMString& idArticle) {

	int32 canUpdateArticle = 0;
	do {
		InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IWebServices> baseHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
		InterfacePtr<IStringData> gLogin(GetExecutionContextSession(), IID_ILOGINSTRING);

		PMString login = gLogin->Get();
		if (!baseHTTP->UpdateStory_v2(xpgPrefs->GetTEC_URL(), idArticle, login, canUpdateArticle))
		{
			CAlert::InformationAlert("Impossible de mettre à jour l'article");
			break;
		}

	} while (kFalse);
	return canUpdateArticle;
}

void XPGUIXRailPanelObserver::DeleteAdonments(const UIDList &  itemList) {
	// Count the items that have the adornment.

	do {
		UIDList adorneditemList(itemList.GetDataBase());

		for (int32 i = 0; i < itemList.Length(); i++) {
			InterfacePtr<IPageItemAdornmentList> pageItemAdornmentList(itemList.GetRef(i), IID_IPAGEITEMADORNMENTLIST);
			if (pageItemAdornmentList && pageItemAdornmentList->HasAdornment(kXPGUIArticleAdornmentBoss))
				adorneditemList.Append(itemList[i]);
		}

		if (adorneditemList.Length() > 0) {
			// Remove the adornment.
			InterfacePtr<ICommand>  removePageItemAdornmentCmd(CmdUtils::CreateCommand(kRemovePageItemAdornmentCmdBoss));
			if (!removePageItemAdornmentCmd)
				break;

			InterfacePtr<IClassIDData> classIDData(removePageItemAdornmentCmd, UseDefaultIID());
			if (!classIDData)
				break;

			classIDData->Set(kXPGUIArticleAdornmentBoss);
			removePageItemAdornmentCmd->SetItemList(adorneditemList);
			CmdUtils::ProcessCommand(removePageItemAdornmentCmd);
		}
	} while (kFalse);
}

void XPGUIXRailPanelObserver::ResetStoryInfosWidgets() {

	do {
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (!panelControlData)
			break;

		// Reset article preview
		InterfacePtr<ITextControlData> articlePreviewData(panelControlData->FindWidget(kXPGUIArticlePreviewWidgetID), UseDefaultIID());
		if (articlePreviewData)
			articlePreviewData->SetString(kNullString);


		// Reset article sheats
		InterfacePtr<ITextControlData> articleSheatsData(panelControlData->FindWidget(kXPGUIArticleNbSignesWidgetID), UseDefaultIID());
		if (articleSheatsData)
			articleSheatsData->SetString(kNullString);

	} while (kFalse);
}

void XPGUIXRailPanelObserver::ChangeStyles()
{
	do
	{
		InterfacePtr<ISelectionManager> selectionMgr(Utils<ISelectionUtils>()->QueryActiveSelection());
		InterfacePtr<IXPageSuite> xpgSuite(selectionMgr, IID_IXPAGESUITE);
		if (xpgSuite == nil)
			break;

		IDocument *doc;
		doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		UIDRef docWS = doc->GetDocWorkSpace();

		//on recup la liste des feuilles de styles characteres
		InterfacePtr<IStyleGroupManager> charNameTable(docWS, IID_ICHARSTYLEGROUPMANAGER);
		if (charNameTable == nil)
		{
			ASSERT_FAIL("XPGTextImportHandler::ApplyCharStyleCmd -> charNameTable invalid");
			break;
		}

		InterfacePtr<IStyleGroupManager> paraStyleTable(docWS, IID_IPARASTYLEGROUPMANAGER);
		if (paraStyleTable == nil)
		{
			ASSERT_FAIL("XPGTextImportHandler::ApplyCharStyleCmd -> charNameTable invalid");
			break;
		}

		IStyleGroupHierarchy *paraStylesHierarchy = paraStyleTable->GetRootHierarchy();
		if (paraStylesHierarchy == nil)
		{
			ASSERT_FAIL("XPGTextImportHandler::ApplyCharStyleCmd -> charStylesHierarchy invalid");
			break;
		}
		IStyleGroupHierarchy *charStylesHierarchy = charNameTable->GetRootHierarchy();
		if (charStylesHierarchy == nil)
		{
			ASSERT_FAIL("XPGTextImportHandler::ApplyCharStyleCmd -> charStylesHierarchy invalid");
			break;
		}

		IDataBase * db = selectionMgr->GetDataBase();
		if (db == nil)
			break;

		UIDList txtFrames, graphicFrames, selectedItems;
		ICommandSequence * seq = nil;
		ErrorCode status = kFailure;

		xpgSuite->GetSelectedItems(txtFrames, graphicFrames, selectedItems);
		//UIDList txtItems(txtFrames);
		for (int32 i = 0; i < txtFrames.size(); ++i) {
			UIDRef storyRef = txtFrames.GetRef(i);

			Utils<IAssignmentUtils>()->NormalizeTextModelRef(storyRef);
			// Read story
			InterfacePtr<ITextModel> txtModel(storyRef, UseDefaultIID());
			if (txtModel == nil)
				continue;

			// Get text data, character and paragraph strands
			InterfacePtr<IAttributeStrand> charStrand(static_cast<IAttributeStrand*>(txtModel->QueryStrand(kCharAttrStrandBoss, IID_IATTRIBUTESTRAND)));
			if (charStrand == nil)
				continue;

			InterfacePtr<IAttributeStrand> paraStrand(static_cast<IAttributeStrand*>(txtModel->QueryStrand(kParaAttrStrandBoss, IID_IATTRIBUTESTRAND)));
			if (paraStrand == nil)
				continue;

			InterfacePtr<ITextStrand> txtStrand(static_cast<ITextStrand*>(txtModel->QueryStrand(kTextDataStrandBoss, IID_ITEXTSTRAND)));
			if (txtStrand == nil)
				continue;

			InterfacePtr<IItemStrand> itemStrand(static_cast<IItemStrand*>(txtModel->QueryStrand(kOwnedItemStrandBoss, IID_IITEMSTRAND)));
			if (itemStrand == nil)
				continue;

			TextIndex index = 0;
			while (index < txtModel->GetPrimaryStoryThreadSpan())
			{
				// Get a "chunk" of text (text can be divided in several chunk)
				int32 length;
				DataWrapper<textchar> chunk = txtStrand->FindChunk(index, &length);

				int32 indexChunk = 0;
				int32 endPara = 0;
				int32 endCharRun = 0;
				while (indexChunk < length)
				{
					if (indexChunk == endCharRun || indexChunk == endPara)
					{
						int32 countCharRun, countCharPara;
						UID charStyleApplied = charStrand->GetStyleUID(index + indexChunk, &countCharRun);
						UID paraStyleApplied = paraStrand->GetStyleUID(index + indexChunk, &countCharPara);

						PMString charStyleName = "", paraStyleName = "";

						InterfacePtr<IStyleInfo> styleInfo(db, paraStyleApplied, UseDefaultIID());
						if (styleInfo == nil) {
							continue;
						}
						paraStyleName = styleInfo->GetName();

						InterfacePtr<IStyleInfo> styleCharInfo(db, charStyleApplied, UseDefaultIID());
						if (styleInfo == nil) {
							continue;
						}
						charStyleName = styleCharInfo->GetName();
						PMString currentGoodStyleCar;
						PMString newStyle;
						newStyle = "";
						if (charStyleName.IndexOfString("BoldItalExp") > 0) { //Cas 1, c'est une feuille de style BoldItalExp
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("BoldItalExp");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style BoldItalExp, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else if (charStyleName.IndexOfString("BoldItalInd") > 0) { //Cas 1, c'est une feuille de style BoldItal
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("BoldItalInd");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style BoldItal, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else if (charStyleName.IndexOfString("BoldItal") > 0) { //Cas 1, c'est une feuille de style BoldItal
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("BoldItal");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style BoldItal, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else if (charStyleName.IndexOfString("BoldExp") > 0) { //Cas 1, c'est une feuille de style BoldItal
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("BoldExp");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style BoldItal, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else if (charStyleName.IndexOfString("BoldInd") > 0) { //Cas 1, c'est une feuille de style BoldItal
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("BoldInd");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style BoldItal, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else if (charStyleName.IndexOfString("ItalExp") > 0) { //Cas 1, c'est une feuille de style BoldItal
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("ItalExp");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style BoldItal, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else if (charStyleName.IndexOfString("ItalInd") > 0) { //Cas 1, c'est une feuille de style BoldItal
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("ItalInd");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style BoldItal, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else if (charStyleName.IndexOfString("Bold") > 0) {  //Cas 2, c'est une feuille de style Bold
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("Bold");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style Bold, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else if (charStyleName.IndexOfString("Ital") > 0) { //Cas 3, c'est une feuille de style Ital
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("Ital");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style BoldItal, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else if (charStyleName.IndexOfString("Exp") > 0) { //Cas 3, c'est une feuille de style Ital
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("Exp");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style BoldItal, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else if (charStyleName.IndexOfString("Ind") > 0) { //Cas 3, c'est une feuille de style Ital
							currentGoodStyleCar = paraStyleName;
							currentGoodStyleCar.Append("Ind");
							if (currentGoodStyleCar != charStyleName) { //pas la bonne feuille de style BoldItal, on la reapplique donc
								newStyle = currentGoodStyleCar;
							}
						}
						else { //Cas 4, c'est une feuille de style normale, qui a le meme nom que la feuille de style para
							//si on trouve une feuille de style paragraphe avec le meme nom, on considere qu'il faut la remplacer
							UID paraStyleUID = Utils<IXPageUtils>()->TriasFindStyleByName(paraStylesHierarchy, charStyleName);
							if (paraStyleUID != kInvalidUID) {
								newStyle = paraStyleName;

							}
						}

						if (newStyle != "") {
							UID charStyleUID = Utils<IXPageUtils>()->TriasFindStyleByName(charStylesHierarchy, newStyle);

							//si elle existe, on l'applique
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
								// Create an ApplyTextStyleCmd						
								InterfacePtr<ICommand> applyCmd(modelCmds->ApplyStyleCmd(index + indexChunk, countCharRun, charStyleUID, kCharAttrStrandBoss));
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
						}



						endPara = indexChunk + countCharPara;
						endCharRun = indexChunk + countCharRun;
					}


					indexChunk++;
				}
				index += length;
			}

		}
	} while (kFalse);
}


// ============================================================================
// Palette gate: doc + XRail page ID + page-change observation
// ============================================================================

/* IsPaletteUsable
 *
 * The whole palette is "intervenable" (the user's word) only when there's a
 * front document AND its active page carries a non-zero XRail ID. Both
 * conditions short-circuit the rest of HandleSelectionChanged (every
 * action button gets greyed out).
*/
bool16 XPGUIXRailPanelObserver::IsPaletteUsable() const
{
	IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (doc == nil)
		return kFalse;

	const int32 pageID = Utils<IXPageUtils>()->GetPageId();
	return (pageID > 0);
}


