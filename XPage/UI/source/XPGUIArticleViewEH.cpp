// XPGUIArticleViewEH.cpp
//
// Event handler de la liste des articles (palette XPage) : ajoute la
// navigation clavier (fleches haut/bas, d'article en article, rubriques
// sautees). Tout le reste est delegue au gestionnaire d'origine du widget
// arbre (kTreeViewEventHandlerImpl), agrege sur le meme boss sous
// IID_IXPGUIORIGTREEEVENTHANDLER.
//
//   Haut / Bas            : article precedent / suivant
//   Maj + Haut / Bas      : etend la selection (seulement si DONTASKFORM=1)
//   Echap                 : rend le clavier
//
// La liste prend le clavier quand l'utilisateur selectionne un article
// (XPGUIArticleViewObserver : selection changee bouton souris enfonce).

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IEventHandler.h"
#include "IEvent.h"
#include "IKeyFocusHandler.h"
#include "ITreeViewController.h"
#include "ITreeViewHierarchyAdapter.h"
#include "ITreeViewMgr.h"
#include "IXPGPreferences.h"

// General includes:
#include "KeyboardDefs.h"
#include "K2Vector.tpp" // For NodeIDList to compile

// Project includes:
#include "XPGUIID.h"
#include "XPGUIArticleNodeID.h"


class XPGUIArticleViewEH : public CPMUnknown<IEventHandler>
{
public:
	XPGUIArticleViewEH(IPMUnknown* boss) : CPMUnknown<IEventHandler>(boss) {}
	virtual ~XPGUIArticleViewEH() {}

	virtual bool16 Activate(IEvent* e)			{ return Delegate() ? Delegate()->Activate(e) : kFalse; }
	virtual bool16 Deactivate(IEvent* e)		{ return Delegate() ? Delegate()->Deactivate(e) : kFalse; }
	virtual bool16 Suspend(IEvent* e)			{ return Delegate() ? Delegate()->Suspend(e) : kFalse; }
	virtual bool16 Resume(IEvent* e)			{ return Delegate() ? Delegate()->Resume(e) : kFalse; }
	virtual bool16 MouseMove(IEvent* e)			{ return Delegate() ? Delegate()->MouseMove(e) : kFalse; }
	virtual bool16 MouseExit(IEvent* e)			{ return Delegate() ? Delegate()->MouseExit(e) : kFalse; }
	virtual bool16 MouseDrag(IEvent* e)			{ return Delegate() ? Delegate()->MouseDrag(e) : kFalse; }
	virtual bool16 LButtonDn(IEvent* e)			{ return Delegate() ? Delegate()->LButtonDn(e) : kFalse; }
	virtual bool16 RButtonDn(IEvent* e)			{ return Delegate() ? Delegate()->RButtonDn(e) : kFalse; }
	virtual bool16 MButtonDn(IEvent* e)			{ return Delegate() ? Delegate()->MButtonDn(e) : kFalse; }
	virtual bool16 LButtonUp(IEvent* e)			{ return Delegate() ? Delegate()->LButtonUp(e) : kFalse; }
	virtual bool16 RButtonUp(IEvent* e)			{ return Delegate() ? Delegate()->RButtonUp(e) : kFalse; }
	virtual bool16 MButtonUp(IEvent* e)			{ return Delegate() ? Delegate()->MButtonUp(e) : kFalse; }
	virtual bool16 ButtonDblClk(IEvent* e)		{ return Delegate() ? Delegate()->ButtonDblClk(e) : kFalse; }
	virtual bool16 ButtonTrplClk(IEvent* e)		{ return Delegate() ? Delegate()->ButtonTrplClk(e) : kFalse; }
	virtual bool16 ButtonQuadClk(IEvent* e)		{ return Delegate() ? Delegate()->ButtonQuadClk(e) : kFalse; }
	virtual bool16 ButtonQuintClk(IEvent* e)	{ return Delegate() ? Delegate()->ButtonQuintClk(e) : kFalse; }
	virtual bool16 MouseWheel(IEvent* e)		{ return Delegate() ? Delegate()->MouseWheel(e) : kFalse; }
	virtual bool16 TabletEvent(IEvent* e)		{ return Delegate() ? Delegate()->TabletEvent(e) : kFalse; }
	virtual bool16 GestureEvent(IEvent* e)		{ return Delegate() ? Delegate()->GestureEvent(e) : kFalse; }
	virtual bool16 MultiTouchEvent(IEvent* e)	{ return Delegate() ? Delegate()->MultiTouchEvent(e) : kFalse; }
	virtual bool16 ControlCmd(IEvent* e)		{ return Delegate() ? Delegate()->ControlCmd(e) : kFalse; }
	virtual bool16 KeyCmd(IEvent* e)			{ return Delegate() ? Delegate()->KeyCmd(e) : kFalse; }
	virtual bool16 KeyUp(IEvent* e)				{ return Delegate() ? Delegate()->KeyUp(e) : kFalse; }
	virtual void PreGetKeyFocus()				{ if (Delegate()) Delegate()->PreGetKeyFocus(); }
	virtual void PostGetKeyFocus()				{ if (Delegate()) Delegate()->PostGetKeyFocus(); }
	virtual void PreGiveUpKeyFocus()			{ if (Delegate()) Delegate()->PreGiveUpKeyFocus(); }
	virtual void PostGiveUpKeyFocus()			{ if (Delegate()) Delegate()->PostGiveUpKeyFocus(); }
	virtual bool16 WillingToGiveUpKeyFocus()	{ return kTrue; }
	virtual bool16 SuspendKeyFocus()			{ return Delegate() ? Delegate()->SuspendKeyFocus() : kTrue; }
	virtual bool16 ResumeKeyFocus()				{ return Delegate() ? Delegate()->ResumeKeyFocus() : kTrue; }
	virtual bool16 CanHaveKeyFocus() const		{ return kTrue; }
	virtual bool16 WantsTabKeyFocus() const		{ return kFalse; }
	virtual void SetView(IControlView* view)	{ if (Delegate()) Delegate()->SetView(view); }

	virtual bool16 KeyDown(IEvent* e);

private:
	IEventHandler* Delegate() const;
	void CollectArticles(ITreeViewHierarchyAdapter* adapter, ITreeViewMgr* mgr, const NodeID& parent, NodeIDList& articles) const;
};

CREATE_PMINTERFACE(XPGUIArticleViewEH, kXPGUIArticleViewEHImpl)


/* Gestionnaire d'origine du widget arbre (pointeur non possede : meme boss). */
IEventHandler* XPGUIArticleViewEH::Delegate() const
{
	InterfacePtr<IEventHandler> orig(this, IID_IXPGUIORIGTREEEVENTHANDLER);
	return orig.get(); // l'objet vit autant que le boss
}


/* Articles visibles, dans l'ordre d'affichage (rubriques sautees, rubriques
   repliees non parcourues). Un article = un noeud avec un chemin XML. */
void XPGUIArticleViewEH::CollectArticles(ITreeViewHierarchyAdapter* adapter, ITreeViewMgr* mgr, const NodeID& parent, NodeIDList& articles) const
{
	const int32 nb = adapter->GetNumChildren(parent);
	for (int32 i = 0; i < nb; ++i) {
		NodeID child = adapter->GetNthChild(parent, i);
		TreeNodePtr<XPGUIArticleNodeID> nodeID(child);
		if (nodeID && nodeID->GetArticleData() && !nodeID->GetArticleData()->artPath.IsEmpty())
			articles.push_back(child);
		if (adapter->GetNumChildren(child) > 0 && mgr->IsNodeExpanded(child))
			CollectArticles(adapter, mgr, child, articles);
	}
}


bool16 XPGUIArticleViewEH::KeyDown(IEvent* e)
{
	const VirtualKey key = e->GetVirtualKey();

	if (key == kVirtualEscapeKey) {
		// Rend le clavier (retour a la mise en page).
		IKeyFocusHandler* focusHandler = ::QueryKeyFocusHandler(this);
		if (focusHandler) {
			focusHandler->SetCurrentTargetEventHandler(nil);
			focusHandler->Release();
		}
		return kTrue;
	}

	if (key != kVirtualUpArrowKey && key != kVirtualDownArrowKey)
		return Delegate() ? Delegate()->KeyDown(e) : kFalse;

	do {
		InterfacePtr<ITreeViewController> controller(this, UseDefaultIID());
		InterfacePtr<ITreeViewHierarchyAdapter> adapter(this, UseDefaultIID());
		InterfacePtr<ITreeViewMgr> mgr(this, UseDefaultIID());
		if (!controller || !adapter || !mgr)
			break;

		NodeIDList articles;
		CollectArticles(adapter, mgr, adapter->GetRootNode(), articles);
		if (articles.empty())
			break;

		const bool16 down = (key == kVirtualDownArrowKey);

		// Position courante : dernier article selectionne (bas) / premier (haut).
		int32 first = -1, last = -1;
		for (int32 i = 0; i < articles.size(); ++i) {
			if (controller->IsSelected(articles[i])) {
				if (first < 0) first = i;
				last = i;
			}
		}

		int32 target;
		if (first < 0)
			target = down ? 0 : articles.size() - 1;
		else
			target = down ? last + 1 : first - 1;
		if (target < 0 || target >= articles.size())
			break; // deja en bout de liste

		InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
		const bool16 extend = e->ShiftKeyDown() && xpgPrefs && xpgPrefs->GetDontAskForm() && first >= 0;

		if (!extend)
			controller->DeselectAll(kFalse, kTrue);
		controller->Select(articles[target], kTrue, kTrue);
		mgr->ScrollToNode(articles[target], ITreeViewMgr::eScrollIntoView);
	} while (kFalse);

	return kTrue; // fleches toujours consommees quand la liste a le clavier
}

// End, XPGUIArticleViewEH.cpp.
