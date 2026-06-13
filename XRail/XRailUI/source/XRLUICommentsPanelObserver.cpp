#include "VCPlugInHeaders.h"
#include <vector>

// Interface includes
#include "IControlView.h"
#include "IPanelControlData.h"
#include "ISubject.h"
#include "IApplication.h"
#include "IDocument.h"
#include "IDocumentList.h"
#include "IStringData.h"
#include "ILayoutUIUtils.h"
#include "IPMUnknownData.h"
#include "ITreeViewMgr.h"
#include "ITriStateControlData.h"
#include "IBooleanControlData.h"
#include "ICommand.h"
#include "ISpreadList.h"
#include "ISpread.h"

// General includes
#include "CObserver.h"
#include "CmdUtils.h"
#include "PersistUtils.h"      // ::GetDataBase

// Project includes (XRail)
#include "XRLUIID.h"
#include "XRLID.h"             // IID_ILOGINSTRING
#include "IXRLUICommentList.h"
#include "IWebServices.h"
#include "XRCID.h"             // kXRCXRailClientBoss
#include "IXRailPageSlugData.h"
#include "DocUtils.h"          // GetGoodUrlXR

/** Comments palette observer — drives the Commentaires palette under
	Window > Gaia > Commentaires. Refreshes the list when:
	  - the front document changes (user switches to another doc),
	  - the user clicks the in-palette refresh button,
	  - the user logs in/out (XRail login broadcast).

	Aggregated on kXRLUICommentsPanelWidgetBoss. The same boss aggregates the
	IXRLUICommentList data; the tree view widget's IID_IDATAMODEL points at
	the panel boss so the adapter + widget mgr can read the same data.
*/
class XRLUICommentsPanelObserver : public CObserver
{
public:
	XRLUICommentsPanelObserver(IPMUnknown* boss);
	virtual ~XRLUICommentsPanelObserver();

	void AutoAttach();
	void AutoDetach();
	void Update(const ClassID& theChange, ISubject* theSubject, const PMIID& protocol, void* changedBy);

private:
	void AttachDocList();
	void DetachDocList();
	void AttachWidget(const InterfacePtr<IPanelControlData>& panelData, const WidgetID& wid, const PMIID& iid);
	void DetachWidget(const InterfacePtr<IPanelControlData>& panelData, const WidgetID& wid, const PMIID& iid);

	void RefreshList();
	int32 ResolveCurrentGaiaPageID() const;
	PMString GetXRailServerURL() const;

	// Login + no-Gaia-page overlay management. Same gating logic as the
	// Photos liées palette in XPage UI (XPGUIXRailImagesPanelObserver).
	void UpdateLoginState();
	void OpenLoginDialog();

	bool fAttached;
};

CREATE_PMINTERFACE(XRLUICommentsPanelObserver, kXRLUICommentsPanelObserverImpl)

XRLUICommentsPanelObserver::XRLUICommentsPanelObserver(IPMUnknown* boss)
	: CObserver(boss), fAttached(false)
{
	// Login state changes are broadcast by XRail with its own protocol IID.
	InterfacePtr<ISubject> sessionSubject(GetExecutionContextSession(), UseDefaultIID());
	if (sessionSubject != nil)
		sessionSubject->AttachObserver(this, IID_IXRLUILOGINPROTOCOL);
}

XRLUICommentsPanelObserver::~XRLUICommentsPanelObserver()
{
	InterfacePtr<ISubject> sessionSubject(GetExecutionContextSession(), UseDefaultIID());
	if (sessionSubject != nil)
		sessionSubject->DetachObserver(this, IID_IXRLUILOGINPROTOCOL);
}

void XRLUICommentsPanelObserver::AutoAttach()
{
	if (fAttached)
		return;
	fAttached = true;

	InterfacePtr<IPanelControlData> panelData(this, UseDefaultIID());
	if (panelData == nil)
		return;

	// Wire the tree view's data-model slot to the panel boss itself — the
	// panel boss aggregates IXRLUICommentList, and the adapter + widget mgr
	// fetch the data through this pointer.
	InterfacePtr<IPMUnknownData> dataModel(panelData->FindWidget(kXRLUICommentsViewWidgetID), IID_IXRLUIDATAMODEL);
	if (dataModel != nil)
		dataModel->SetPMUnknown(this);

	this->AttachDocList();
	this->AttachWidget(panelData, kXRLUICommentsRefreshButtonWidgetID, IID_ITRISTATECONTROLDATA);
	// Login button broadcasts on IID_IBOOLEANCONTROLDATA — same protocol the
	// XPage palettes use for their identical "Sign in" prompt button.
	this->AttachWidget(panelData, kXRLUICommentsLoginPromptButtonWidgetID, IID_IBOOLEANCONTROLDATA);
	// "Show all comments" checkbox — broadcasts kTrueStateMessage on
	// IID_ITRISTATECONTROLDATA the same way as the refresh button.
	this->AttachWidget(panelData, kXRLUICommentsShowAllCheckWidgetID, IID_ITRISTATECONTROLDATA);

	// Initial fetch + visibility state on every show.
	this->RefreshList();
	this->UpdateLoginState();
}

void XRLUICommentsPanelObserver::AutoDetach()
{
	if (!fAttached)
		return;
	fAttached = false;

	InterfacePtr<IPanelControlData> panelData(this, UseDefaultIID());
	if (panelData == nil)
		return;

	this->DetachDocList();
	this->DetachWidget(panelData, kXRLUICommentsRefreshButtonWidgetID, IID_ITRISTATECONTROLDATA);
	this->DetachWidget(panelData, kXRLUICommentsLoginPromptButtonWidgetID, IID_IBOOLEANCONTROLDATA);
	this->DetachWidget(panelData, kXRLUICommentsShowAllCheckWidgetID, IID_ITRISTATECONTROLDATA);
}

void XRLUICommentsPanelObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID& protocol, void* /*changedBy*/)
{
	if (protocol == IID_IDOCUMENTLIST &&
	    (theChange == kSetFrontDocCmdBoss || theChange == kCloseDocCmdBoss))
	{
		this->RefreshList();
		this->UpdateLoginState();
		return;
	}

	if (protocol == IID_IXRLUILOGINPROTOCOL && theChange == kXRLUILoginChangedMsg)
	{
		this->RefreshList();
		this->UpdateLoginState();
		return;
	}

	if (theChange == kTrueStateMessage || theChange == kFalseStateMessage)
	{
		InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());
		if (controlView == nil)
			return;
		const WidgetID wid = controlView->GetWidgetID();
		if (wid == kXRLUICommentsRefreshButtonWidgetID && theChange == kTrueStateMessage)
		{
			this->RefreshList();
			return;
		}
		if (wid == kXRLUICommentsLoginPromptButtonWidgetID && theChange == kTrueStateMessage)
		{
			this->OpenLoginDialog();
			return;
		}
		if (wid == kXRLUICommentsShowAllCheckWidgetID)
		{
			// On toggle: invalidate the layout views so any future spread
			// adornment that paints the markers picks up the new state.
			// The actual marker rendering is deferred — see the TODO comment
			// next to the checkbox declaration in XRLUI.fr.
			IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
			if (doc != nil)
				Utils<ILayoutUIUtils>()->InvalidateViews(doc);
			return;
		}
	}
}

void XRLUICommentsPanelObserver::AttachDocList()
{
	InterfacePtr<IApplication> app(GetExecutionContextSession()->QueryApplication());
	if (app == nil) return;
	InterfacePtr<IDocumentList> docList(app->QueryDocumentList());
	if (docList == nil) return;
	InterfacePtr<ISubject> subject(docList, UseDefaultIID());
	if (subject != nil)
		subject->AttachObserver(this, IID_IDOCUMENTLIST);
}

void XRLUICommentsPanelObserver::DetachDocList()
{
	InterfacePtr<IApplication> app(GetExecutionContextSession()->QueryApplication());
	if (app == nil) return;
	InterfacePtr<IDocumentList> docList(app->QueryDocumentList());
	if (docList == nil) return;
	InterfacePtr<ISubject> subject(docList, UseDefaultIID());
	if (subject != nil)
		subject->DetachObserver(this, IID_IDOCUMENTLIST);
}

void XRLUICommentsPanelObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelData, const WidgetID& wid, const PMIID& iid)
{
	IControlView* view = panelData->FindWidget(wid);
	if (view == nil) return;
	InterfacePtr<ISubject> subject(view, UseDefaultIID());
	if (subject != nil)
		subject->AttachObserver(this, iid);
}

void XRLUICommentsPanelObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelData, const WidgetID& wid, const PMIID& iid)
{
	IControlView* view = panelData->FindWidget(wid);
	if (view == nil) return;
	InterfacePtr<ISubject> subject(view, UseDefaultIID());
	if (subject != nil)
		subject->DetachObserver(this, iid);
}

PMString XRLUICommentsPanelObserver::GetXRailServerURL() const
{
	// Same helper every other XRailUI dialog uses to resolve the active
	// XRail server URL — empty baseName picks the default (single-base
	// install). For multi-base installs the active doc's base is what
	// GetGoodUrlXR resolves internally.
	PMString baseName;
	return GetGoodUrlXR(baseName);
}

int32 XRLUICommentsPanelObserver::ResolveCurrentGaiaPageID() const
{
	// Walk the front document's pages and return the first non-zero XRail
	// page-slug ID. Most Gaia documents are single-page; if multi-page the
	// "first matching page" rule mirrors what the rest of the plugin family
	// does in single-page contexts. No XPage Core dependency.
	IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (doc == nil)
		return 0;

	IDataBase* db = ::GetDataBase(doc);
	if (db == nil)
		return 0;

	InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
	if (spreadList == nil)
		return 0;

	const int32 nSpreads = spreadList->GetSpreadCount();
	for (int32 s = 0; s < nSpreads; ++s)
	{
		const UID spreadUID = spreadList->GetNthSpreadUID(s);
		InterfacePtr<ISpread> spread(db, spreadUID, UseDefaultIID());
		if (spread == nil)
			continue;

		const int32 nPages = spread->GetNumPages();
		for (int32 p = 0; p < nPages; ++p)
		{
			const UID pageUID = spread->GetNthPageUID(p);
			InterfacePtr<IXRailPageSlugData> slug(db, pageUID, IID_PAGESLUGDATA);
			if (slug == nil)
				continue;
			const int32 id = slug->GetID();
			if (id > 0)
				return id;
		}
	}
	return 0;
}

void XRLUICommentsPanelObserver::RefreshList()
{
	InterfacePtr<IXRLUICommentList> list(this, UseDefaultIID());
	if (list == nil)
		return;

	const int32 pageID = this->ResolveCurrentGaiaPageID();
	if (pageID <= 0)
	{
		// No Gaia page at the front — clear the list so the palette doesn't
		// show stale rows from the previous document.
		list->Clear();
		return;
	}

	const PMString serverURL = this->GetXRailServerURL();
	if (serverURL.IsEmpty())
	{
		list->Clear();
		return;
	}

	K2Vector<int32>     ids;
	K2Vector<PMString>  texts;
	K2Vector<PMString>  dates;
	K2Vector<int32>     hours;
	K2Vector<PMString>  users;
	K2Vector<bool>      checks;
	K2Vector<PMReal>    xs;
	K2Vector<PMReal>    ys;

	InterfacePtr<IWebServices> http(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
	if (http == nil)
	{
		list->Clear();
		return;
	}

	if (!http->GetCommentaires_v2(serverURL, pageID, ids, texts, dates, hours, users, checks, xs, ys))
	{
		// Network/parse error — wipe the cached list rather than showing a
		// stale snapshot. Silent: a popup on every doc switch would be noisy.
		list->Clear();
	}
	else
	{
		std::vector<XRLUIComment> rows;
		for (size_t i = 0; i < ids.size(); ++i)
		{
			XRLUIComment c;
			c.idComm     = ids[i];
			c.commentaire = texts[i];
			c.dateISO    = dates[i];
			c.heureSecs  = hours[i];
			c.nomUser    = users[i];
			c.check      = checks[i];
			c.x          = xs[i];
			c.y          = ys[i];
			rows.push_back(c);
		}
		list->SetRows(rows);
	}

	// Tell the tree view to rebuild against the new comment count. The list
	// changing on its own doesn't trigger the tree to redraw — same pattern
	// as XPGUIXRailImagesPanelObserver::HandleSelectionChanged.
	InterfacePtr<IPanelControlData> panelData(this, UseDefaultIID());
	if (panelData != nil)
	{
		InterfacePtr<ITreeViewMgr> treeMgr(panelData->FindWidget(kXRLUICommentsViewWidgetID), UseDefaultIID());
		if (treeMgr != nil)
		{
			treeMgr->ClearTree();
			treeMgr->ChangeRoot(kTrue);
		}
	}
}

void XRLUICommentsPanelObserver::UpdateLoginState()
{
	// Three-state overlay (mirrors the Photos liées palette):
	//   1. Not logged in           -> show login prompt + "Sign in" button.
	//   2. Logged in, no Gaia page -> show "Open a Gaia page" message.
	//   3. Logged in + Gaia page   -> show normal palette content.
	//
	// Iterates all child widgets of the panel and toggles their visibility
	// by widget id. Widgets the observer doesn't recognise default to "show
	// when content state is active" so future additions don't get hidden.
	InterfacePtr<IPanelControlData> panelData(this, UseDefaultIID());
	if (panelData == nil)
		return;

	InterfacePtr<IStringData> gLogin(GetExecutionContextSession(), IID_ILOGINSTRING);
	const bool16 loggedIn = (gLogin != nil && !gLogin->Get().IsEmpty());

	const bool16 hasGaiaPage = (this->ResolveCurrentGaiaPageID() > 0);

	const int32 n = panelData->Length();
	for (int32 i = 0; i < n; ++i)
	{
		IControlView* child = panelData->GetWidget(i);
		if (child == nil)
			continue;

		const WidgetID wid = child->GetWidgetID();

		bool16 shouldShow = kFalse;
		if (wid == kXRLUICommentsLoginPromptTextWidgetID ||
		    wid == kXRLUICommentsLoginPromptButtonWidgetID)
		{
			shouldShow = !loggedIn;
		}
		else if (wid == kXRLUICommentsNoGaiaPageTextWidgetID)
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

void XRLUICommentsPanelObserver::OpenLoginDialog()
{
	InterfacePtr<ICommand> cmd(CmdUtils::CreateCommand(kXRLUIOpenLoginDialogCmdBoss));
	if (cmd == nil)
		return;
	CmdUtils::ProcessCommand(cmd);
}
