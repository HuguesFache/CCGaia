#include "VCPlugInHeaders.h"
#include <vector>
#include <map>

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
#include "IClassIDData.h"
#include "ISpreadList.h"
#include "ISpread.h"
#include "ISpreadLayer.h"
#include "IGeometry.h"
#include "IHierarchy.h"
#include "ILayerUtils.h"
#include "IDocumentLayer.h"
#include "INewPageItemCmdData.h"
#include "IPathUtils.h"
#include "IGraphicAttributeUtils.h"
#include "IDataBase.h"
#include "PMRect.h"
#include "PMPoint.h"
#include "UIDList.h"
#include "IPageItemAdornmentList.h"
#include "SplineID.h"          // kSplineItemBoss
#include "PageItemScrapID.h"   // kDeleteCmdBoss

// General includes
#include "CObserver.h"
#include "CmdUtils.h"
#include "PersistUtils.h"      // ::GetDataBase
#include "GenericID.h"         // kAddPageItemAdornmentCmdBoss / kRemovePageItemAdornmentCmdBoss

// Project includes (XRail)
#include "XRLUIID.h"
#include "XRLID.h"             // IID_ILOGINSTRING
#include "IXRLUICommentList.h"
#include "IWebServices.h"
#include "XRCID.h"             // kXRCXRailClientBoss
#include "IXRailPageSlugData.h"
#include "DocUtils.h"          // GetGoodUrlXR
#include "XRLUICommentPinStore.h"

// Helpers définis plus bas (anonymous namespace). Déclarations anticipées car
// Update()/AutoDetach() les utilisent.
namespace { void SyncPinAdornments(bool attach); void DeletePinAnchors(); }

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
	void ResolveAllGaiaPageIDs(K2Vector<int32>& outIDs) const;
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

	// Palette fermée : on retire les blocs-ancres (le doc est encore valide ici).
	DeletePinAnchors();
}

void XRLUICommentsPanelObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID& protocol, void* /*changedBy*/)
{
	if (protocol == IID_IDOCUMENTLIST &&
	    (theChange == kSetFrontDocCmdBoss || theChange == kCloseDocCmdBoss))
	{
		// Retire les ancres du doc courant pendant qu'il est encore valide,
		// avant que le front ne change. RefreshList les recréera si besoin.
		DeletePinAnchors();
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
			// Affiche/masque les pins. On reflète l'état dans le store, on
			// attache/retire l'adornment sur les pages Gaia, et on rafraîchit.
			const bool show = (theChange == kTrueStateMessage);
			XRLUICommentPinStore::Instance().SetShow(show);
			SyncPinAdornments(show);
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

void XRLUICommentsPanelObserver::ResolveAllGaiaPageIDs(K2Vector<int32>& outIDs) const
{
	// Collecte les IDs de slug XRail de TOUTES les pages du document (pas
	// seulement la première), pour récupérer les commentaires de l'ensemble du
	// document. Même parcours que ResolveCurrentGaiaPageID, sans early-return.
	outIDs.clear();

	IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (doc == nil)
		return;

	IDataBase* db = ::GetDataBase(doc);
	if (db == nil)
		return;

	InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
	if (spreadList == nil)
		return;

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
				outIDs.push_back(id);
		}
	}
}

namespace {

// Infos géométriques d'une page Gaia, indexées par son idPage.
struct PageGeo
{
	UID    uid;
	int32  idx;      // index de la page dans son spread (0 = gauche)
	int32  nPages;   // nb de pages du spread
};

// idPage -> infos de page (UID + position dans le spread) pour le document de
// devant. Sert à la normalisation du X 4D et à l'attache de l'adornment.
void BuildPageGeoInfo(std::map<int32, PageGeo>& out)
{
	out.clear();
	IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (doc == nil) return;
	IDataBase* db = ::GetDataBase(doc);
	if (db == nil) return;
	InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
	if (spreadList == nil) return;

	const int32 nSpreads = spreadList->GetSpreadCount();
	for (int32 s = 0; s < nSpreads; ++s)
	{
		InterfacePtr<ISpread> spread(db, spreadList->GetNthSpreadUID(s), UseDefaultIID());
		if (spread == nil) continue;
		const int32 nPages = spread->GetNumPages();
		for (int32 p = 0; p < nPages; ++p)
		{
			const UID pageUID = spread->GetNthPageUID(p);
			InterfacePtr<IXRailPageSlugData> slug(db, pageUID, IID_PAGESLUGDATA);
			if (slug == nil) continue;
			const int32 id = slug->GetID();
			if (id > 0)
			{
				PageGeo g; g.uid = pageUID; g.idx = p; g.nPages = nPages;
				out[id] = g;
			}
		}
	}
}

// X 4D -> X page seule (% page). Le X 4D (cf. zoom.js getCoordClicImg) est :
//   X = posPixels / largeur(#contentzoom) * 100   (+ largeur image gauche pour
//   la page droite). Or #contentzoom est ~r fois plus large qu'une page, donc
//   X est un % du CONTENEUR, pas de la page. On rétablit le % page par
//   X_page = X * r - idx*100, avec r = largeurConteneur / largeurPage.
// kXScale = r = largeur(#contentzoom) / largeur(#imgpageleft), relevé dans la
// console navigateur AVEC le panneau commentaires ouvert (état de capture, car
// le panneau est toujours ouvert pour poser un commentaire -> col-sm-9).
// NB : dépend un peu de la largeur de fenêtre du relecteur ; fix robuste =
// diviser par la largeur de l'image côté zoom.js (option A).
static const PMReal kXScale = 1.0757812542022704;

PMReal NormalizeX4D(const PMReal& xCumul, int32 pageIdx)
{
	return xCumul * kXScale - pageIdx * 100.0;
}

PMString FormatDateFr(const PMString& dateISO)
{
	if (dateISO.NumUTF16TextChars() < 10)
		return dateISO;
	PMString* dayP = dateISO.Substring(8, 2);
	PMString* monP = dateISO.Substring(5, 2);
	PMString out;
	if (dayP != nil && monP != nil) { out = *dayP; out += "/"; out += *monP; }
	else                            { out = dateISO; }
	delete dayP;
	delete monP;
	return out;
}

PMString FormatTimeFr(int32 secs)
{
	if (secs < 0) secs = 0;
	const int32 hh = (secs / 3600) % 24;
	const int32 mm = (secs / 60) % 60;
	PMString out;
	if (hh < 10) out += "0";
	out.AppendNumber(hh);
	out += ":";
	if (mm < 10) out += "0";
	out.AppendNumber(mm);
	return out;
}

// Blocs-ancres créés (splines 1x1, premier plan) qui portent l'adornment des
// pins. Tracés ici pour pouvoir les supprimer. Process / main thread.
static std::vector<UIDRef> gPinAnchors;

// Supprime les blocs-ancres encore présents. Garde IsValidUID pour ne pas
// toucher un item déjà disparu (doc fermé, undo, etc.).
void DeletePinAnchors()
{
	for (size_t i = 0; i < gPinAnchors.size(); ++i)
	{
		IDataBase* adb = gPinAnchors[i].GetDataBase();
		const UID   uid = gPinAnchors[i].GetUID();
		if (adb == nil || uid == kInvalidUID || !adb->IsValidUID(uid))
			continue;

		InterfacePtr<ICommand> del(CmdUtils::CreateCommand(kDeleteCmdBoss));
		if (del == nil)
			continue;
		del->SetUndoability(ICommand::kAutoUndo);
		UIDList l(adb);
		l.Append(uid);
		del->SetItemList(l);
		CmdUtils::ProcessCommand(del);
	}
	gPinAnchors.clear();
}

// Crée un bloc-ancre 1x1 au premier plan sur chaque spread Gaia et y attache
// l'adornment des pins. Le bloc neuf est l'item le plus haut -> kAfterShape
// dessine au-dessus de toute la maquette.
void CreatePinAnchors()
{
	IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (doc == nil) return;
	IDataBase* db = ::GetDataBase(doc);
	if (db == nil) return;
	InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(doc));
	if (activeLayer == nil) return;
	InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
	if (spreadList == nil) return;

	const int32 nSpreads = spreadList->GetSpreadCount();
	for (int32 s = 0; s < nSpreads; ++s)
	{
		const UID spreadUID = spreadList->GetNthSpreadUID(s);
		InterfacePtr<ISpread> spread(db, spreadUID, UseDefaultIID());
		if (spread == nil) continue;

		bool hasGaia = false;
		const int32 nPages = spread->GetNumPages();
		for (int32 p = 0; p < nPages && !hasGaia; ++p)
		{
			InterfacePtr<IXRailPageSlugData> slug(db, spread->GetNthPageUID(p), IID_PAGESLUGDATA);
			if (slug != nil && slug->GetID() > 0) hasGaia = true;
		}
		if (!hasGaia) continue;

		InterfacePtr<ISpreadLayer> spreadLayer(spread->QueryLayer(activeLayer));
		if (spreadLayer == nil) continue;
		const UID ownerSpreadLayer = ::GetUID(spreadLayer);

		// Cadre-ancre 100x100 dans le pasteboard, à gauche de la page de gauche
		// du spread (hors maquette).
		PMReal bx = -110.0, by = 0.0;
		InterfacePtr<IGeometry> spreadGeo(db, spreadUID, UseDefaultIID());
		if (spreadGeo != nil)
		{
			const PMRect sb = spreadGeo->GetStrokeBoundingBox();
			bx = sb.Left() - 110.0;   // 100 (bloc) + 10 d'écart avec la page
			by = sb.Top();
		}
		PMRect anchorBounds(bx, by, bx + 100.0, by + 100.0);

		// Vrai cadre graphique rectangulaire (même helper que Pagemakeup/Blocs).
		const UIDRef anchorRef = Utils<IPathUtils>()->CreateRectangleSpline(
			UIDRef(db, ownerSpreadLayer), anchorBounds, INewPageItemCmdData::kGraphicFrameAttributes);
		if (anchorRef == UIDRef(nil, kInvalidUID)) continue;

		// Attache l'adornment des pins.
		InterfacePtr<ICommand> add(CmdUtils::CreateCommand(kAddPageItemAdornmentCmdBoss));
		if (add == nil) continue;
		add->SetUndoability(ICommand::kAutoUndo);
		InterfacePtr<IClassIDData> cid(add, UseDefaultIID());
		if (cid == nil) continue;
		cid->Set(kXRLUICommentsPinAdornmentBoss);
		UIDList al(db);
		al.Append(anchorRef.GetUID());
		add->SetItemList(al);
		CmdUtils::ProcessCommand(add);

		// Rend le bloc non imprimable (aucun impact impression / export / preflight).
		InterfacePtr<ICommand> nonPrint(Utils<IGraphicAttributeUtils>()->CreateNonPrintCommand(kTrue, &al, kTrue, kTrue));
		if (nonPrint != nil)
		{
			nonPrint->SetUndoability(ICommand::kAutoUndo);
			CmdUtils::ProcessCommand(nonPrint);
		}

		gPinAnchors.push_back(anchorRef);
	}
}

// Affiche (attach=true) / masque les pins : on (re)crée ou on supprime les
// blocs-ancres. On supprime toujours d'abord pour repartir propre.
void SyncPinAdornments(bool attach)
{
	DeletePinAnchors();
	if (attach)
		CreatePinAnchors();
}

} // namespace

void XRLUICommentsPanelObserver::RefreshList()
{
	InterfacePtr<IXRLUICommentList> list(this, UseDefaultIID());
	if (list == nil)
		return;

	K2Vector<int32> pageIDs;
	this->ResolveAllGaiaPageIDs(pageIDs);
	if (pageIDs.empty())
	{
		// No Gaia page in the document — clear the list so the palette doesn't
		// show stale rows from the previous document.
		list->Clear();
		XRLUICommentPinStore::Instance().Clear();
		return;
	}

	const PMString serverURL = this->GetXRailServerURL();
	if (serverURL.IsEmpty())
	{
		list->Clear();
		XRLUICommentPinStore::Instance().Clear();
		return;
	}

	K2Vector<int32>     ids;
	K2Vector<int32>     pageOf;
	K2Vector<PMString>  texts;
	K2Vector<PMString>  dates;
	K2Vector<int32>     hours;
	K2Vector<PMString>  users;
	K2Vector<bool>      checks;
	K2Vector<PMReal>    xs;
	K2Vector<PMReal>    ys;
	bool                fromPhp = false;	// true = données PHP, false = 4D.

	InterfacePtr<IWebServices> http(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
	if (http == nil)
	{
		list->Clear();
		XRLUICommentPinStore::Instance().Clear();
		return;
	}

	if (!http->GetCommentaires_v2(serverURL, pageIDs, ids, pageOf, texts, dates, hours, users, checks, xs, ys, fromPhp))
	{
		// Network/parse error — wipe the cached list rather than showing a
		// stale snapshot. Silent: a popup on every doc switch would be noisy.
		list->Clear();
		XRLUICommentPinStore::Instance().Clear();
	}
	else
	{
		// Géométrie des pages Gaia (pour normaliser le X 4D selon gauche/droite).
		std::map<int32, PageGeo> pageGeo;
		BuildPageGeoInfo(pageGeo);

		std::vector<XRLUIComment>     rows;
		std::vector<XRLUICommentPin>  pins;
		for (size_t i = 0; i < ids.size(); ++i)
		{
			XRLUIComment c;
			c.idComm     = ids[i];
			c.idPage     = pageOf[i];
			c.commentaire = texts[i];
			c.dateISO    = dates[i];
			c.heureSecs  = hours[i];
			c.nomUser    = users[i];
			c.check      = checks[i];
			c.hasCoord   = false;
			c.x          = 0.0;
			c.y          = 0.0;

			// Pas de coordonnées si idPage vide/0 ou page absente du document.
			std::map<int32, PageGeo>::const_iterator it = (c.idPage > 0) ? pageGeo.find(c.idPage) : pageGeo.end();
			if (it != pageGeo.end())
			{
				c.hasCoord = true;
				c.y = ys[i];	// Y toujours en % page.
				c.x = fromPhp ? xs[i] : NormalizeX4D(xs[i], it->second.idx);

				XRLUICommentPin pin;
				pin.idPage = c.idPage;
				pin.xPct   = c.x;
				pin.yPct   = c.y;
				pin.header = c.nomUser;
				pin.header += " - ";
				pin.header += FormatDateFr(c.dateISO);
				pin.header += " à ";
				pin.header += FormatTimeFr(c.heureSecs);
				pin.text   = c.commentaire;
				pins.push_back(pin);
			}

			rows.push_back(c);
		}
		list->SetRows(rows);

		// Met à jour les pins et (ré)attache l'adornment si l'affichage est actif.
		XRLUICommentPinStore& store = XRLUICommentPinStore::Instance();
		store.SetPins(pins);
		if (store.GetShow())
		{
			SyncPinAdornments(true);
			IDocument* frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
			if (frontDoc != nil)
				Utils<ILayoutUIUtils>()->InvalidateViews(frontDoc);
		}
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
