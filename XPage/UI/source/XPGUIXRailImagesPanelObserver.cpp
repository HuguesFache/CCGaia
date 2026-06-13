#include "VCPluginHeaders.h"

// Interface includes
#include "IControlView.h"
#include "IPanelControlData.h"
#include "ITextControlData.h"
#include "ISubject.h"
#include "IApplication.h"
#include "ISelectionManager.h"
#include "ISelectionMessages.h"
#include "ITextSelectionSuite.h"
#include "IDocument.h"
#include "IStringData.h"
#include "ILayoutUIUtils.h"
#include "IXPageMgrAccessor.h"
#include "IImageDataModel.h"
#include "ITreeViewMgr.h"
#include "IPMUnknownData.h"
#include "ITriStateControlData.h"
#include "ITreeViewHierarchyAdapter.h"
#include "ITreeViewController.h"
#include "ISysFileData.h"
#include "IDocumentList.h"
#include "IXPageSuite.h"
#include "IAssignmentMgr.h"
#include "IAssignment.h"
#include "IAssignedStory.h"
#include "IAssignmentUtils.h"
#include "ITreeNodeIDData.h"
#include "IStringListControlData.h"
#include "IPlacedArticleData.h"
#include "IXPageUtils.h"
#include "IFormeData.h"
#include "IDropDownListController.h"

// General includes
#include "SelectionObserver.h"
#include "FileUtils.h"
#include "CAlert.h"
#include "ErrorUtils.h"
#include "IStringListData.h"
#include "StringNodeID.h"

// Project includes
#include "XPGUIImageNodeID.h"
#include "XPGUIID.h"
#include "XPGImageDataNode.h"

// Reverse selection sync (graphic frame in layout -> matching photo row).
#include "IHierarchy.h"
#include "ILinkManager.h"
#include "ILink.h"
#include "ILinkResource.h"
#include "IURIUtils.h"
#include "URI.h"
#include "XPGID.h"
#include "XRLID.h"    // IID_ILOGINSTRING
#include "XRLUIID.h"  // kXRLUIOpenLoginDialogCmdBoss
#include "CmdUtils.h"
#include "ICommand.h"
#include "ISpreadList.h"
#include "ISpread.h"
#include "IMultiColumnTextFrame.h"

#if defined(WINDOWS) || defined(_WIN32)
#include <windows.h> // OpenClipboard / SetClipboardData on the copy-button path.
// User32 holds the clipboard symbols. The InDesign plug-in build doesn't
// link User32 by default, so emit an explicit linker directive here rather
// than touching the .vcxproj — keeps the dependency local to the only file
// that needs it.
#pragma comment(lib, "User32.lib")
#endif

/** XPGUIXRailImagesPanelObserver

	@author Trias Developpement
*/

class XPGUIXRailImagesPanelObserver : public ActiveSelectionObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIXRailImagesPanelObserver(IPMUnknown* boss);
	/** Destructor
	*/
	virtual ~XPGUIXRailImagesPanelObserver();

	/**
		Initialises widgets and attaches widget observers.
	*/
	void AutoAttach();

	/**
		Detaches widget observers.
	*/
	void AutoDetach();

	/**
		Update is called for all registered observers, and is
		the method through which changes are broadcast.
		@param theChange this is specified by the agent of change; it can be the class ID of the agent,
		or it may be some specialised message ID.
		@param theSubject this provides a reference to the object which has changed; in this case, the
		widget boss objects that are being observed.
		@param protocol the protocol along which the change occurred.
		@param changedBy this can be used to provide additional information about the change or a reference
		to the boss object that caused the change.
	*/
	void Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy);

protected:
	/*
		This method is called whenever selection changed, update image filter according
		to selected article
	*/
	virtual void HandleSelectionChanged(const ISelectionMessage * message);

private:
	void AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID);
	void DetachWidget(const InterfacePtr<IPanelControlData>&, const WidgetID& widgetID, const PMIID& interfaceID);
	void AttachDocList(void);
	void DetachDocList(void);

	void DoApplyIndex();

	// Action icons next to the Crédit / Légende edit boxes.
	void HandleCopyTextWidget(const WidgetID& sourceWidgetID);
	void HandleSendToFrame(bool16 isCredit);

	// "Voir toutes les photos" mode — collect every article referenced by
	// an assignment in the front document. Returns kFalse when no
	// assignment-backed article was found.
	bool16 GatherAllArticlesInDocument(K2Vector<PMString>& articleIds,
									   K2Vector<PMString>& articleFiles);

	// Login-state-driven visibility
	void UpdateLoginState();
	void OpenLoginDialog();

	IDocument * curFrontDoc;
	bool16 firstInit;
	UID currentFilter;
	// Persists across panel hides/shows: keeps "Voir toutes les photos"
	// ticked between uses without us having to round-trip through
	// IPanelControlData on every AutoAttach.
	bool16 mShowAllImages;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUIXRailImagesPanelObserver, kXPGUIXRailImagesPanelObserverImpl)

/* Constructor
*/
XPGUIXRailImagesPanelObserver::XPGUIXRailImagesPanelObserver(IPMUnknown* boss) :
	ActiveSelectionObserver(boss), curFrontDoc(nil), firstInit(kTrue),
	currentFilter(kInvalidUID), mShowAllImages(kFalse)
{
	// Get notification when a refresh is required
	InterfacePtr<ISubject> sessionSubject(GetExecutionContextSession(), UseDefaultIID());
	sessionSubject->AttachObserver(this, IID_IREFRESHPROTOCOL);
	// Login state changes are broadcast by XRail with its own protocol IID
	// (XRail-owned, no XPage dependency).
	sessionSubject->AttachObserver(this, IID_IXRLUILOGINPROTOCOL);
}

/* Destructor
*/
XPGUIXRailImagesPanelObserver::~XPGUIXRailImagesPanelObserver()
{
}

/* AutoAttach
*/
void XPGUIXRailImagesPanelObserver::AutoAttach()
{
	// set data model for tree view widget		
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	InterfacePtr<IPMUnknownData> dataModel(panelControlData->FindWidget(kXPGUIImagesViewWidgetID), IID_IDATAMODEL);
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor(GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IImageDataModel> imageDataModel(pageMrgAccessor->QueryImageDataModel());
	dataModel->SetPMUnknown(imageDataModel);

	ActiveSelectionObserver::AutoAttach();

	do
	{
		// Initialise widget state.		
		if (panelControlData == nil)
		{
			ASSERT_FAIL("AutoAttach() invalid panelControlData");
			break;
		}

		if (firstInit)
		{
			firstInit = kFalse;
			// The "Type" dropdown that this branch used to seed has been
			// removed from the resource (kXPGUIImageChangeTypeWidgetID).
		}

		// On attache le DocumentList observer, afin de pouvoir detecter un changement de selection de document, et
		// pouvoir reagir (dans le Update de cette classe).
		this->AttachDocList();

		// ButtonWidget fires on IID_IBOOLEANCONTROLDATA, not ITriStateControlData.
		AttachWidget(panelControlData, kXPGUILoginPromptButtonWidgetID, IID_IBOOLEANCONTROLDATA);

		// Copy / Send action icons next to the Crédit / Légende fields.
		// RollOverIconButtonWidget broadcasts kTrueStateMessage on click via
		// IID_ITRISTATECONTROLDATA — same wiring as the per-row action
		// icons (cf. XPGUIImageNodeButtonsObserver).
		AttachWidget(panelControlData, kXPGUICopyCreditBtnWidgetID, IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUISendCreditBtnWidgetID, IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUICopyLegendBtnWidgetID, IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUISendLegendBtnWidgetID, IID_ITRISTATECONTROLDATA);

		// "Voir toutes les photos" checkbox + refresh icon. Checkbox
		// broadcasts kTrueStateMessage when ticked / kFalseStateMessage
		// when unticked, both via IID_ITRISTATECONTROLDATA. The refresh
		// icon broadcasts kTrueStateMessage on click. Restore the
		// checkbox visual to the persisted member, in case the user
		// alt-tabbed away while it was ticked.
		AttachWidget(panelControlData, kXPGUIShowAllImagesCheckboxWidgetID, IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUIRefreshImagesBtnWidgetID,      IID_ITRISTATECONTROLDATA);
		{
			InterfacePtr<ITriStateControlData> showAllChk(
				panelControlData->FindWidget(kXPGUIShowAllImagesCheckboxWidgetID), UseDefaultIID());
			if (showAllChk != nil)
			{
				if (mShowAllImages) showAllChk->Select(kFalse, kFalse);
				else                showAllChk->Deselect(kFalse, kFalse);
			}
		}

		// On initialise le document courant au premier plan
		curFrontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();

		// Force a reload every time the palette is shown — AutoAttach is
		// invoked by the framework on each show. Without this reset the
		// equality check in HandleSelectionChanged would short-circuit if
		// the same frame is still selected as when the palette was last
		// closed, and the list would stay empty.
		currentFilter = kInvalidUID;

		// Update model
		this->HandleSelectionChanged(nil);

		// Reflect the current login state on the panel widgets.
		this->UpdateLoginState();

	} while (false);

}

/* AutoDetach
*/
void XPGUIXRailImagesPanelObserver::AutoDetach()
{
	ActiveSelectionObserver::AutoDetach();
	do
	{
		// Detach widget observers.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("AutoDetach() invalid panelControlData");
			break;
		}

		this->DetachDocList();
		DetachWidget(panelControlData, kXPGUILoginPromptButtonWidgetID, IID_IBOOLEANCONTROLDATA);

		DetachWidget(panelControlData, kXPGUICopyCreditBtnWidgetID, IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUISendCreditBtnWidgetID, IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUICopyLegendBtnWidgetID, IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUISendLegendBtnWidgetID, IID_ITRISTATECONTROLDATA);

		DetachWidget(panelControlData, kXPGUIShowAllImagesCheckboxWidgetID, IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUIRefreshImagesBtnWidgetID,      IID_ITRISTATECONTROLDATA);

	} while (false);

}

/* Update
*/
void XPGUIXRailImagesPanelObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{
	ActiveSelectionObserver::Update(theChange, theSubject, protocol, changedBy);
	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());

	if ((theChange == kSetFrontDocCmdBoss || theChange == kCloseDocCmdBoss) && (protocol == IID_IDOCUMENTLIST))
	{
		// Always reload on front-doc change — even when the same document
		// regains focus (e.g. user alt-tabbed away to Photoshop and back).
		// We invalidate currentFilter so HandleSelectionChanged doesn't
		// short-circuit on the cached UID below.
		curFrontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		currentFilter = kInvalidUID;
		this->HandleSelectionChanged(nil);
	}
	else if (theChange == kXPGRefreshMsg && protocol == IID_IREFRESHPROTOCOL)
	{
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		this->HandleSelectionChanged(nil);
	}
	else if (theChange == kXRLUILoginChangedMsg && protocol == IID_IXRLUILOGINPROTOCOL)
	{
		// XPage prefs are refetched once by the Articles panel observer
		// (XPGUIXRailPanelObserver) in response to the same broadcast — no
		// need to refetch again here. Just toggle the prompt visibility.
		this->UpdateLoginState();
	}
	else if (theChange == kTrueStateMessage)
	{
		if (controlView != nil)
		{
			const WidgetID widget = controlView->GetWidgetID();
			if (widget == kXPGUILoginPromptButtonWidgetID)
			{
				this->OpenLoginDialog();
			}
			else if (widget == kXPGUICopyCreditBtnWidgetID)
			{
				this->HandleCopyTextWidget(kXPGUICreditTextWidgetID);
			}
			else if (widget == kXPGUICopyLegendBtnWidgetID)
			{
				this->HandleCopyTextWidget(kXPGUILegendeTextWidgetID);
			}
			else if (widget == kXPGUISendCreditBtnWidgetID)
			{
				this->HandleSendToFrame(/*isCredit=*/kTrue);
			}
			else if (widget == kXPGUISendLegendBtnWidgetID)
			{
				this->HandleSendToFrame(/*isCredit=*/kFalse);
			}
			else if (widget == kXPGUIShowAllImagesCheckboxWidgetID)
			{
				// Ticked → switch to "all photos" mode and reload.
				mShowAllImages = kTrue;
				currentFilter = kInvalidUID;
				this->HandleSelectionChanged(nil);
			}
			else if (widget == kXPGUIRefreshImagesBtnWidgetID)
			{
				// Force a re-scan in whichever mode we're currently in.
				currentFilter = kInvalidUID;
				this->HandleSelectionChanged(nil);
			}
		}
	}
	else if (theChange == kFalseStateMessage)
	{
		// The checkbox is the only widget on this panel that broadcasts
		// kFalseStateMessage on uncheck. Switch back to single-article
		// mode and reload from the current selection.
		if (controlView != nil && controlView->GetWidgetID() == kXPGUIShowAllImagesCheckboxWidgetID)
		{
			mShowAllImages = kFalse;
			currentFilter = kInvalidUID;
			this->HandleSelectionChanged(nil);
		}
	}

	switch (protocol.Get())
	{
		case IID_IBOOLEANCONTROLDATA:
		{
			// "Apply index" button (kXPGUIImageChangeIndexBtnWidgetID) was
			// removed from the resource. DoApplyIndex() is now dead code,
			// kept for reference but never reached from the UI.
		}
		break;


		default:
			break;
	}
}

/* UpdateLoginState
 *
 * Three-state overlay (mirrors the XPage palette behaviour):
 *   1. Not logged in           -> show login prompt.
 *   2. Logged in, no Gaia page -> show "Open a Gaia page" message.
 *   3. Logged in + Gaia page   -> show normal panel content.
*/
void XPGUIXRailImagesPanelObserver::UpdateLoginState()
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	InterfacePtr<IStringData> gLogin(GetExecutionContextSession(), IID_ILOGINSTRING);
	const bool16 loggedIn = (gLogin != nil && !gLogin->Get().IsEmpty());

	// Has a Gaia page = front doc exists AND active page has a non-zero
	// XRail ID. Same condition as the Articles palette — kept inline here
	// to avoid a cross-file dependency.
	bool16 hasGaiaPage = kFalse;
	{
		IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if (doc != nil)
		{
			const int32 pageID = Utils<IXPageUtils>()->GetPageId();
			hasGaiaPage = (pageID > 0);
		}
	}

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
			shouldShow = (loggedIn && hasGaiaPage);
		}

		if (shouldShow) child->ShowView();
		else            child->HideView();
	}
}

/* OpenLoginDialog
 */
void XPGUIXRailImagesPanelObserver::OpenLoginDialog()
{
	InterfacePtr<ICommand> cmd(CmdUtils::CreateCommand(kXRLUIOpenLoginDialogCmdBoss));
	if (cmd == nil)
		return;
	CmdUtils::ProcessCommand(cmd);
}

/* AttachWidget
*/
void XPGUIXRailImagesPanelObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
{
	do
	{
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)
		{
			ASSERT_FAIL("controlView invalid");
			break;
		}

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
		{
			ASSERT_FAIL("subject invalid");
			break;
		}
		subject->AttachObserver(this, interfaceID);
	} while (false);
}

/* DetachWidget
*/
void XPGUIXRailImagesPanelObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
{
	do
	{
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)
		{
			ASSERT_FAIL("controlView invalid.");
			break;
		}

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
		{
			ASSERT_FAIL("subject invalid");
			break;
		}
		subject->DetachObserver(this, interfaceID);
	} while (false);
}

/* AttachDocList
*/
void XPGUIXRailImagesPanelObserver::AttachDocList(void)
{
	do
	{
		InterfacePtr<IApplication> app(GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> doclist(app->QueryDocumentList());
		InterfacePtr<ISubject> subject(doclist, UseDefaultIID());
		subject->AttachObserver(this, IID_IDOCUMENTLIST);
	} while (false);
}

/* DetachDocList
*/
void XPGUIXRailImagesPanelObserver::DetachDocList(void)
{
	do
	{
		InterfacePtr<IApplication> app(GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> doclist(app->QueryDocumentList());
		InterfacePtr<ISubject> subject(doclist, UseDefaultIID());
		subject->DetachObserver(this, IID_IDOCUMENTLIST);
	} while (false);
}

/* HandleSelectionChanged
*/
void XPGUIXRailImagesPanelObserver::HandleSelectionChanged(const ISelectionMessage * message)
{
	// Refresh the overlay (login / no-Gaia-page / content) — the active
	// document or its current page may have changed, which can flip the
	// "has Gaia page" gate on/off.
	this->UpdateLoginState();

	PMString idArticle = kNullString;
	IDFile assignFile;
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	do {

		IControlView * imagePanelView = panelControlData->FindWidget(kXPGUIDisplayImagePanelWidgetID);
		InterfacePtr<ITreeViewMgr> treeViewMgr(panelControlData->FindWidget(kXPGUIImagesViewWidgetID), UseDefaultIID());

		InterfacePtr<ISysFileData> imagePanelFileData(imagePanelView, UseDefaultIID());
		imagePanelFileData->Set(IDFile());
		imagePanelView->Invalidate();

		IControlView* displayCreditView = panelControlData->FindWidget(kXPGUICreditTextWidgetID);
		InterfacePtr<ITextControlData> creditPanelView(displayCreditView, UseDefaultIID());
		if (creditPanelView) creditPanelView->SetString(kNullString);

		IControlView* displaylegendView = panelControlData->FindWidget(kXPGUILegendeTextWidgetID);
		InterfacePtr<ITextControlData> legendPanelView(displaylegendView, UseDefaultIID());
		if (legendPanelView) legendPanelView->SetString(kNullString);

		// Reset the new Commentaire field too — populated below if a
		// matching node is found in the model.
		IControlView* displayCommentView = panelControlData->FindWidget(kXPGUICommentTextWidgetID);
		InterfacePtr<ITextControlData> commentPanelView(displayCommentView, UseDefaultIID());
		if (commentPanelView) commentPanelView->SetString(kNullString);

		PMString contentIndex = "", indexString = "-1";
		int32 indexitem = 0, typeitem = 0;
		
		// Helper for the "no selection" branches below: wipe the model so
		// the previous article's images don't linger, clear the tree view,
		// and reset the cached UID so re-selecting the same frame later
		// triggers a real reload. (ChangeRoot alone re-asks the adapter,
		// which would just hand back the still-loaded model nodes.)
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IImageDataModel> model(pageMrgAccessor->QueryImageDataModel());

		// "Voir toutes les photos" mode: ignore the current frame selection
		// and load images from every assignment-backed article in the front
		// document. Keep this branch above the xpgSuite check so the panel
		// stays populated even when the user clicks outside any frame.
		if (mShowAllImages)
		{
			K2Vector<PMString> articleIds, articleFiles;
			const bool16 anyFound = this->GatherAllArticlesInDocument(articleIds, articleFiles);
			if (model != nil)
			{
				if (anyFound)
				{
					model->RebuildFromArticles(articleIds, articleFiles);
				}
				else
				{
					model->SetFilter(kNullString, kNullString);
					model->Rebuild(kNullString);
				}
			}
			treeViewMgr->ClearTree();
			treeViewMgr->ChangeRoot(kTrue);
			currentFilter = kInvalidUID; // sentinel — selection cache unused in this mode
			break;
		}

		InterfacePtr<IXPageSuite> xpgSuite(fCurrentSelection, IID_IXPAGESUITE);
		if (xpgSuite == nil) {
			// No XPage selection suite — empty selection or non-layout context.
			if (model != nil) {
				model->SetFilter(kNullString, kNullString);
				model->Rebuild(kNullString);
			}
			treeViewMgr->ClearTree();
			treeViewMgr->ChangeRoot(kTrue);
			currentFilter = kInvalidUID;
			break;
		}

		UIDList textFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);
		if (selectedItems.size() == 0) {
			// User clicked outside any frame.
			if (model != nil) {
				model->SetFilter(kNullString, kNullString);
				model->Rebuild(kNullString);
			}
			treeViewMgr->ClearTree();
			treeViewMgr->ChangeRoot(kTrue);
			currentFilter = kInvalidUID;
			break;
		}
		if (currentFilter == selectedItems.At(0)) {
			// Same frame as last fired load — nothing changed, no reload.
			// (Don't ClearTree here: if we did, alt-tabbing into InDesign
			// would wipe the list when the selection didn't actually
			// change.)
			break;
		}
		currentFilter = selectedItems.At(0);
		// Gather all InCopy assignments
		InterfacePtr<IAssignmentMgr> assignMgr(GetExecutionContextSession(), UseDefaultIID());
		UIDList storyFrames(textFrames);
		storyFrames.Append(graphicFrames);
		K2Vector<PMString> assignFilePaths;
		for (int32 i = 0; i < storyFrames.Length(); ++i)
		{
			UIDRef storyRef = storyFrames.GetRef(i);
				InterfacePtr<IFormeData> currentData(storyRef, UseDefaultIID());
				indexitem = currentData->GetPhotoIndex();
				typeitem = currentData->GetType();
				if (currentData) {
					switch (typeitem) {
						case IFormeData::kPhoto:
							contentIndex = "Image ";
							contentIndex.AppendNumber(currentData->GetPhotoIndex());
							break;
						case IFormeData::kCredit:
							contentIndex = "Credit ";
							contentIndex.AppendNumber(currentData->GetPhotoIndex());
							break;
						case IFormeData::kLegend:
							contentIndex = "Legende ";
							contentIndex.AppendNumber(currentData->GetPhotoIndex());
							break;
					default:
						break;
					}
				}
			Utils<IAssignmentUtils>()->NormalizeTextModelRef(storyRef);

			// Get assignment related to this story
			InterfacePtr<IAssignment> assignment(nil);
			InterfacePtr<IAssignedStory> assignedStory(nil);
			assignMgr->QueryAssignmentAndAssignedStory(storyRef, assignment, assignedStory);

			if (assignment == nil || assignedStory == nil)
				continue;

			PMString assignFilePath = assignment->GetFile();
			if (::K2find(assignFilePaths, assignFilePath) == assignFilePaths.end())
				assignFilePaths.push_back(assignFilePath);
		}

		if (assignFilePaths.size() == 1)
		{
			assignFile = FileUtils::PMStringToSysFile(assignFilePaths[0]);
			idArticle = Utils<IXPageUtils>()->GetStoryIDFromAssignmentFile(assignFile);
		}
		// Try with placed story
		if (assignFilePaths.size() == 0)
		{
			InterfacePtr<IPlacedArticleData> placedArticleData(storyFrames.GetRef(0), UseDefaultIID());
			if (placedArticleData)
			{
				if (placedArticleData->GetUniqueId() != kNullString)
				{
					idArticle = placedArticleData->GetUniqueId();
					assignFile = FileUtils::PMStringToSysFile(placedArticleData->GetStoryFolder());
					FileUtils::AppendPath(&assignFile, idArticle + ".OBJRART.xml");
					//CAlert::InformationAlert(FileUtils::SysFileToPMString(assignFile));
				}
			}
		}

		// The "index / type / apply" widgets below Légende/Crédit were
		// removed; nothing to update on those. (contentIndex, indexitem,
		// typeitem are now unused.) The model rebuild + tree refresh below
		// still needs to happen. (model / pageMrgAccessor were already
		// resolved above for the empty-selection branches.)

		if (idArticle == kNullString)
			model->SetFilter(kNullString, kNullString); // Reset filters

		else if (idArticle.Contains("-"))
		{
			int32 extPos = idArticle.LastIndexOfCharacter(PlatformChar('-'));
			if (extPos != -1)
				idArticle.Remove(extPos, idArticle.NumUTF16TextChars() - extPos);
			model->SetFilter(idArticle, FileUtils::SysFileToPMString(assignFile));
		}
		else
			model->SetFilter(idArticle, FileUtils::SysFileToPMString(assignFile));
		model->Rebuild(kNullString);

		treeViewMgr->ClearTree();
		treeViewMgr->ChangeRoot(kTrue);
		InterfacePtr<ITreeViewHierarchyAdapter> imagesHierAdapter(treeViewMgr, UseDefaultIID());
		NodeID imageRootNode = imagesHierAdapter->GetRootNode();
		const int32 nbPhotoNodes = imagesHierAdapter->GetNumChildren(imageRootNode);
		if (nbPhotoNodes > 0)
		{
			InterfacePtr<ITreeViewController> imagesListController(imagesHierAdapter, UseDefaultIID());

			// Reverse selection sync: if the user has a single graphic
			// frame selected and that frame contains a placed image, try
			// to match the image's link basename against the photo nodes
			// we just loaded — and select that node instead of the
			// default first one. This is the inverse of the "atteindre
			// le lien" button on each row.
			NodeID nodeToSelect = imagesHierAdapter->GetNthChild(imageRootNode, 0);

			InterfacePtr<IXPageSuite> xpgSuiteForSync(fCurrentSelection, IID_IXPAGESUITE);
			if (xpgSuiteForSync != nil)
			{
				UIDList syncTextFrames, syncGraphicFrames, syncSelected;
				xpgSuiteForSync->GetSelectedItems(syncTextFrames, syncGraphicFrames, syncSelected);

				if (syncGraphicFrames.Length() == 1)
				{
					UIDRef gfRef = syncGraphicFrames.GetRef(0);
					IDataBase* db = gfRef.GetDataBase();

					InterfacePtr<IHierarchy> gfHier(gfRef, UseDefaultIID());
					if (gfHier != nil && gfHier->GetChildCount() > 0)
					{
						InterfacePtr<ILinkManager> linkMgr(db, db->GetRootUID(), UseDefaultIID());
						if (linkMgr != nil)
						{
							// Read the placed image's link URI as a flat
							// lowercased string. We then test each photo
							// node id's "<id>." pattern against it — same
							// matching strategy as XPGUIImageNodeButtonsObserver
							// uses for the goto-link button. More robust
							// than basename equality (case differences,
							// path encoding, HR/BR variants).
							PMString linkPath;
							ILinkManager::QueryResult linkQuery;
							if (linkMgr->QueryLinksByObjectUID(gfHier->GetChildUID(0), linkQuery))
							{
								for (ILinkManager::QueryResult::const_iterator it = linkQuery.begin();
								     it != linkQuery.end() && linkPath.IsEmpty(); ++it)
								{
									InterfacePtr<ILink> link(db, *it, UseDefaultIID());
									if (link == nil)
										continue;
									InterfacePtr<ILinkResource> linkRes(db, link->GetResource(), UseDefaultIID());
									if (linkRes == nil)
										continue;
									URI linkURI = linkRes->GetId();
									// URI::GetURI() returns std::string — wrap.
									linkPath = PMString(linkURI.GetURI().c_str(), PMString::kUnknownEncoding);
								}
							}

							if (!linkPath.IsEmpty())
							{
								linkPath.ToLower();
								for (int32 ci = 0; ci < nbPhotoNodes; ++ci)
								{
									NodeID candidate = imagesHierAdapter->GetNthChild(imageRootNode, ci);
									TreeNodePtr<XPGUIImageNodeID> photoNodeID(candidate);
									if (!photoNodeID)
										continue;
									if (photoNodeID->GetType() != XPGImageDataNode::kImageNode)
										continue;
									PMString needle = photoNodeID->GetID();
									if (needle.IsEmpty())
										continue;
									needle.Append(".");
									needle.ToLower();
									if (linkPath.IndexOfString(needle) >= 0)
									{
										nodeToSelect = candidate;
										break;
									}
								}
							}
						}
					}
				}
			}

			// Force a full reset of the tree view selection before selecting
			// the target node. Without the explicit DeselectAll, switching
			// directly between two photo blocks of the same article would
			// fail to update the palette's highlighted row: the
			// ChangeRoot(kTrue) above keeps the previous selection state,
			// and Select() on a fresh node is a no-op when the controller
			// thinks the selection hasn't really changed.
			imagesListController->DeselectAll(kFalse, kFalse);
			imagesListController->Select(nodeToSelect, kTrue, kTrue);
		}
	} while (kFalse);

}



/* DoApplyResa
*/
void XPGUIXRailImagesPanelObserver::DoApplyIndex()
{
	do
	{
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		int32 nouvelIndex = -1;

		InterfacePtr<ITextControlData> indexRollOverBloc(panelControlData->FindWidget(kXPGUIImageChangeIndexWidgetID), UseDefaultIID());
		if (indexRollOverBloc) {
			PMString::ConversionError cError = PMString::kNoError;
			nouvelIndex = indexRollOverBloc->GetString().GetAsNumber(&cError);
		}
		// Find dropdown list menu control view from panel data.
		IControlView* pDropDownListControlView = panelControlData->FindWidget(kXPGUIImageChangeTypeWidgetID);
		if (pDropDownListControlView == nil)
		{
			break;
		}

		// Get IDropDownListController interface pointer.
		InterfacePtr<IDropDownListController> pDropDownListController(pDropDownListControlView, UseDefaultIID());
		if (pDropDownListController == nil)
		{
			break;
		}
		int32 selectedIndex = pDropDownListController->GetSelected();

		InterfacePtr<IXPageSuite> xpgSuite(fCurrentSelection, IID_IXPAGESUITE);
		if (xpgSuite == nil) {
			return;
		}
		else {


			UIDList textFrames, graphicFrames, selectedItems;
			xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);
			if (selectedItems.size() > 0) {

				UIDList storyFrames(textFrames);
				storyFrames.Append(graphicFrames);
				for (int32 i = 0; i < storyFrames.Length(); ++i)
				{
					UIDRef storyRef = storyFrames.GetRef(i);
					InterfacePtr<IFormeData> currentData(storyRef, UseDefaultIID());
					if (currentData) {
						currentData->SetPhotoIndex(nouvelIndex);
						currentData->SetType(selectedIndex);

					}

				}

			}

		}


	} while (kFalse);
}

/* HandleCopyTextWidget
 *
 * Reads the current contents of a text-edit widget on the panel and pushes
 * it onto the OS clipboard so the user can paste it anywhere. Backs the
 * "copy crédit" / "copy légende" icons.
 *
 * Win32 only for now (the current build target). The Mac path stays a
 * no-op until Cocoa NSPasteboard wiring is added — call sites already
 * tolerate failure silently.
 */
void XPGUIXRailImagesPanelObserver::HandleCopyTextWidget(const WidgetID& sourceWidgetID)
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	IControlView* sourceView = panelControlData->FindWidget(sourceWidgetID);
	if (sourceView == nil)
		return;

	InterfacePtr<ITextControlData> textData(sourceView, UseDefaultIID());
	if (textData == nil)
		return;
	const PMString text = textData->GetString();
	if (text.IsEmpty())
		return;

#if defined(WINDOWS) || defined(_WIN32)
	if (!::OpenClipboard(NULL))
		return;
	::EmptyClipboard();

	int32 numChars = 0;
	const UTF16TextChar* buf = text.GrabUTF16Buffer(&numChars);
	if (buf != nil && numChars >= 0)
	{
		const SIZE_T bytes = (static_cast<SIZE_T>(numChars) + 1) * sizeof(WCHAR);
		HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, bytes);
		if (hMem != NULL)
		{
			void* dst = ::GlobalLock(hMem);
			if (dst != nil)
			{
				::memcpy(dst, buf, static_cast<size_t>(numChars) * sizeof(WCHAR));
				static_cast<WCHAR*>(dst)[numChars] = L'\0';
				::GlobalUnlock(hMem);
				if (::SetClipboardData(CF_UNICODETEXT, hMem) == NULL)
					::GlobalFree(hMem);
			}
			else
			{
				::GlobalFree(hMem);
			}
		}
	}
	::CloseClipboard();
#endif
}


/* HandleSendToFrame
 *
 * Pushes the current Crédit (isCredit=kTrue) or Légende (kFalse) text into
 * the on-page kCredit / kLegend frame that belongs to the photo currently
 * selected in the Photos liées tree. Backs the two "appliquer" icons.
 *
 * Resolution path:
 *   1. selected tree node → image id;
 *   2. walk every spread of the front document, find a graphic frame whose
 *      placed-image link contains the image id (same predicate the per-row
 *      "go to link" icon uses, see XPGUIImageNodeButtonsObserver);
 *   3. read its IFormeData → photoIndex + uniqueName, then walk the same
 *      forme to find the sibling kCredit / kLegend frame at the same index;
 *   4. ImportCreditOrLegend on its story.
 */
void XPGUIXRailImagesPanelObserver::HandleSendToFrame(bool16 isCredit)
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	const WidgetID sourceWidgetID = isCredit ? kXPGUICreditTextWidgetID : kXPGUILegendeTextWidgetID;
	IControlView* sourceView = panelControlData->FindWidget(sourceWidgetID);
	if (sourceView == nil)
		return;
	InterfacePtr<ITextControlData> textData(sourceView, UseDefaultIID());
	if (textData == nil)
		return;
	PMString text = textData->GetString();

	InterfacePtr<ITreeViewController> imageListController(
		panelControlData->FindWidget(kXPGUIImagesViewWidgetID), UseDefaultIID());
	if (imageListController == nil)
		return;
	NodeIDList selected;
	imageListController->GetSelectedItems(selected);
	if (selected.empty())
	{
		CAlert::InformationAlert("Sélectionnez d'abord une image dans la liste");
		return;
	}
	TreeNodePtr<XPGUIImageNodeID> nodeID(selected[0]);
	if (!nodeID || nodeID->GetType() != XPGImageDataNode::kImageNode)
	{
		CAlert::InformationAlert("Sélectionnez d'abord une image dans la liste");
		return;
	}
	const PMString imageId = nodeID->GetID();
	if (imageId.IsEmpty())
		return;

	IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (doc == nil)
		return;
	IDataBase* db = ::GetDataBase(doc);
	if (db == nil)
		return;
	InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
	InterfacePtr<ILinkManager> linkMgr(db, db->GetRootUID(), UseDefaultIID());
	if (spreadList == nil || linkMgr == nil)
		return;

	PMString needle(imageId);
	needle.Append(".");
	needle.ToLower();

	UIDRef photoRef = UIDRef::gNull;
	UID    photoSpreadUID = kInvalidUID;
	const int32 nbSpreads = spreadList->GetSpreadCount();
	for (int32 s = 0; s < nbSpreads && photoRef == UIDRef::gNull; ++s)
	{
		const UID spreadUID = spreadList->GetNthSpreadUID(s);
		InterfacePtr<ISpread> spread(db, spreadUID, UseDefaultIID());
		if (spread == nil)
			continue;
		UIDList itemsOnSpread(db);
		const int32 nbPages = spread->GetNumPages();
		for (int32 p = 0; p < nbPages; ++p)
			spread->GetItemsOnPage(p, &itemsOnSpread, kFalse, kTrue);

		// Worklist-style traversal with inline group recursion. Without
		// it, photos nested inside a group would never reach the link
		// query and "envoyer dans le bloc" would silently fail to find
		// the target frame.
		for (int32 i = 0; i < itemsOnSpread.Length() && photoRef == UIDRef::gNull; ++i)
		{
			UIDRef itemRef = itemsOnSpread.GetRef(i);
			InterfacePtr<IHierarchy> hier(itemRef, UseDefaultIID());
			if (hier == nil || hier->GetChildCount() == 0)
				continue;

			ILinkManager::QueryResult linkQuery;
			if (!linkMgr->QueryLinksByObjectUID(hier->GetChildUID(0), linkQuery))
			{
				// No link on child(0) — could be a group, descend.
				const int32 nbChildren = hier->GetChildCount();
				for (int32 c = 0; c < nbChildren; ++c)
					itemsOnSpread.Append(hier->GetChildUID(c));
				continue;
			}
			for (ILinkManager::QueryResult::const_iterator it = linkQuery.begin();
				it != linkQuery.end(); ++it)
			{
				InterfacePtr<ILink> link(db, *it, UseDefaultIID());
				if (link == nil) continue;
				InterfacePtr<ILinkResource> linkRes(db, link->GetResource(), UseDefaultIID());
				if (linkRes == nil) continue;
				URI linkURI = linkRes->GetId();
				PMString linkPath(linkURI.GetURI().c_str(), PMString::kUnknownEncoding);
				linkPath.ToLower();
				if (linkPath.IndexOfString(needle) >= 0)
				{
					photoRef = itemRef;
					photoSpreadUID = spreadUID;
					break;
				}
			}
		}
	}

	if (photoRef == UIDRef::gNull)
	{
		CAlert::InformationAlert("L'image n'est pas placée dans la page");
		return;
	}

	InterfacePtr<IFormeData> photoFormeData(photoRef, UseDefaultIID());
	if (photoFormeData == nil)
	{
		CAlert::InformationAlert("Le bloc image n'appartient à aucun carton");
		return;
	}
	const PMString formeName = photoFormeData->GetUniqueName();
	const int16 photoIndex = photoFormeData->GetPhotoIndex();
	if (formeName.IsEmpty() || photoIndex < 0)
	{
		CAlert::InformationAlert("Le bloc image n'appartient à aucun carton");
		return;
	}

	InterfacePtr<ISpread> photoSpread(db, photoSpreadUID, UseDefaultIID());
	if (photoSpread == nil)
		return;
	UIDList allFormeItems(db);
	Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(formeName, photoSpread, allFormeItems);

	const int16 wantedType = isCredit ? IFormeData::kCredit : IFormeData::kLegend;
	UID targetSplineUID = kInvalidUID;
	for (int32 i = 0; i < allFormeItems.Length(); ++i)
	{
		InterfacePtr<IFormeData> fd(allFormeItems.GetRef(i), UseDefaultIID());
		if (fd == nil) continue;
		if (fd->GetType() == wantedType && fd->GetPhotoIndex() == photoIndex)
		{
			targetSplineUID = allFormeItems[i];
			break;
		}
	}

	if (targetSplineUID == kInvalidUID)
	{
		CAlert::InformationAlert(isCredit
			? "Aucun bloc crédit n'est associé à cette image dans le carton"
			: "Aucun bloc légende n'est associé à cette image dans le carton");
		return;
	}

	InterfacePtr<IHierarchy> targetHier(db, targetSplineUID, UseDefaultIID());
	if (targetHier == nil || targetHier->GetChildCount() == 0)
		return;
	InterfacePtr<IMultiColumnTextFrame> targetTxtFrame(db, targetHier->GetChildUID(0), UseDefaultIID());
	if (targetTxtFrame == nil)
		return;
	const UID storyUID = targetTxtFrame->GetTextModelUID();
	if (storyUID == kInvalidUID)
		return;

	text.SetTranslatable(kFalse);
	Utils<IXPageUtils>()->ImportCreditOrLegend(UIDRef(db, storyUID), text, isCredit);
}


/* GatherAllArticlesInDocument
 *
 * "Voir toutes les photos" feed. Walks every spline in every spread of
 * the front document and harvests (uniqueId, storyFolder/<id>.OBJRART.xml)
 * pairs from IPlacedArticleData — the same payload the single-article
 * fallback in HandleSelectionChanged consumes. Recurses into groups so
 * articles dropped inside a group still surface.
 *
 * Dedupe is by uniqueId: a multi-frame article carries the same
 * IPlacedArticleData on every spline, so we'd otherwise list it once
 * per frame. (We deliberately ignore IAssignedDocument here — the
 * InCopy assignment workflow isn't used in this project.)
 *
 * Returns kFalse when no placed article was found, so the caller can
 * fall back to an empty model.
 */
bool16 XPGUIXRailImagesPanelObserver::GatherAllArticlesInDocument(
	K2Vector<PMString>& articleIds,
	K2Vector<PMString>& articleFiles)
{
	articleIds.clear();
	articleFiles.clear();

	IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (doc == nil)
		return kFalse;
	IDataBase* db = ::GetDataBase(doc);
	if (db == nil)
		return kFalse;
	InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
	if (spreadList == nil)
		return kFalse;

	const int32 nbSpreads = spreadList->GetSpreadCount();
	for (int32 s = 0; s < nbSpreads; ++s)
	{
		InterfacePtr<ISpread> spread(db, spreadList->GetNthSpreadUID(s), UseDefaultIID());
		if (spread == nil)
			continue;

		UIDList itemsOnSpread(db);
		const int32 nbPages = spread->GetNumPages();
		for (int32 p = 0; p < nbPages; ++p)
			spread->GetItemsOnPage(p, &itemsOnSpread, kFalse, kTrue);

		// Inline group recursion via IHierarchy. Worklist-style traversal —
		// avoid std::stack to keep the file's existing dependency surface.
		// Array growth as we expand groups in place is fine: UIDList stores
		// UIDs by value.
		for (int32 i = 0; i < itemsOnSpread.Length(); ++i)
		{
			UIDRef itemRef = itemsOnSpread.GetRef(i);

			// If it's a group, push its children onto the worklist and skip
			// the IPlacedArticleData query — groups don't carry article
			// metadata themselves.
			InterfacePtr<IHierarchy> hier(itemRef, UseDefaultIID());
			if (hier != nil && hier->GetChildCount() > 0)
			{
				// Heuristic: a node that has children AND no IPlacedArticleData
				// is treated as a container; descend. Text frames also have a
				// child (the column), but those are still leaf-level for our
				// purpose because they expose IPlacedArticleData on their own
				// spline. Checking the data first lets us bail before the
				// recursion when it's not needed.
				InterfacePtr<IPlacedArticleData> probe(itemRef, UseDefaultIID());
				const PMString probeId = (probe != nil) ? probe->GetUniqueId() : PMString();
				if (probeId.IsEmpty())
				{
					// No article on this node — descend into its children.
					const int32 nbChildren = hier->GetChildCount();
					for (int32 c = 0; c < nbChildren; ++c)
						itemsOnSpread.Append(hier->GetChildUID(c));
					continue;
				}
			}

			InterfacePtr<IPlacedArticleData> placedData(itemRef, UseDefaultIID());
			if (placedData == nil)
				continue;
			const PMString uniqueId = placedData->GetUniqueId();
			if (uniqueId.IsEmpty())
				continue;

			// Dedupe by article id — a multi-frame story repeats the same
			// IPlacedArticleData on every spline, and a Gaia article with
			// linked photos may show up across multiple page items too.
			if (::K2find(articleIds, uniqueId) != articleIds.end())
				continue;

			const PMString storyFolder = placedData->GetStoryFolder();
			if (storyFolder.IsEmpty())
				continue;

			// OBJRART path = <storyFolder>/<uniqueId>.OBJRART.xml.
			// The model's RebuildFromArticles caller takes parent dir of
			// articleFiles[i] as the article folder, so passing the OBJRART
			// path itself works (parent → storyFolder).
			IDFile objrartFile = FileUtils::PMStringToSysFile(storyFolder);
			PMString objrartFilename(uniqueId);
			objrartFilename.Append(".OBJRART.xml");
			objrartFilename.SetTranslatable(kFalse);
			FileUtils::AppendPath(&objrartFile, objrartFilename);

			articleIds.push_back(uniqueId);
			articleFiles.push_back(FileUtils::SysFileToPMString(objrartFile));
		}
	}

	return !articleIds.empty();
}
