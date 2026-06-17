#include "VCPlugInHeaders.h"

// Interface includes
#include "IEventHandler.h"
#include "ITreeNodeIDData.h"
#include "IPanelControlData.h"
#include "IWidgetUtils.h"
#include "IPMUnknownData.h"
#include "IControlView.h"
#include "IDocument.h"
#include "IGeometry.h"
#include "ITransform.h"
#include "ILayoutUIUtils.h"
#include "IPanorama.h"
#include "ISpreadList.h"
#include "ISpread.h"

// General includes
#include "CEventHandler.h"
#include "PersistUtils.h"        // ::GetDataBase
#include "TransformUtils.h"      // ::TransformInnerPointToPasteboard
#include "NodeID.h"              // TreeNodePtr<>
#include "IntNodeID.h"

// Project includes
#include "XRLUIID.h"
#include "IXRLUICommentList.h"
#include "IXRailPageSlugData.h"

/** Event handler aggregated on kXRLUICommentsNodeWidgetBoss. Overrides
	IID_IEVENTHANDLER, but every method except ButtonDblClk just delegates
	to the standard SDK tree-node handler held under IID_IXRLUICOMMENTSSHADOWEH
	on the same boss — so selection / hover / drag continue to work normally.

	On double-click we look up the comment's (X%, Y%) page-relative position
	and scroll the front layout view so it's centred on that point.
*/
class XRLUICommentsNodeEH : public CEventHandler
{
public:
	XRLUICommentsNodeEH(IPMUnknown* boss) : CEventHandler(boss) {}
	virtual ~XRLUICommentsNodeEH() {}

	// Single override — every other event passes through to the shadow.
	virtual bool16 ButtonDblClk(IEvent* e);

	// Boilerplate: each of these forwards to the shadow handler held under
	// IID_IXRLUICOMMENTSSHADOWEH. Mirrors the SDK paneltreeview sample —
	// without these the standard tree-node behaviours (selection,
	// expand/contract, hover hilite, …) would silently break.
	virtual bool16 Activate(IEvent* e)            { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->Activate(e) : kFalse; }
	virtual bool16 Deactivate(IEvent* e)          { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->Deactivate(e) : kFalse; }
	virtual bool16 Suspend(IEvent* e)             { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->Suspend(e) : kFalse; }
	virtual bool16 Resume(IEvent* e)              { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->Resume(e) : kFalse; }
	virtual bool16 MouseMove(IEvent* e)           { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->MouseMove(e) : kFalse; }
	virtual bool16 MouseDrag(IEvent* e)           { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->MouseDrag(e) : kFalse; }
	virtual bool16 LButtonDn(IEvent* e)           { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->LButtonDn(e) : kFalse; }
	virtual bool16 RButtonDn(IEvent* e)           { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->RButtonDn(e) : kFalse; }
	virtual bool16 MButtonDn(IEvent* e)           { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->MButtonDn(e) : kFalse; }
	virtual bool16 LButtonUp(IEvent* e)           { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->LButtonUp(e) : kFalse; }
	virtual bool16 RButtonUp(IEvent* e)           { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->RButtonUp(e) : kFalse; }
	virtual bool16 MButtonUp(IEvent* e)           { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->MButtonUp(e) : kFalse; }
	virtual bool16 ButtonTrplClk(IEvent* e)       { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->ButtonTrplClk(e) : kFalse; }
	virtual bool16 ButtonQuadClk(IEvent* e)       { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->ButtonQuadClk(e) : kFalse; }
	virtual bool16 ButtonQuintClk(IEvent* e)      { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->ButtonQuintClk(e) : kFalse; }
	virtual bool16 MouseWheel(IEvent* e)          { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->MouseWheel(e) : kFalse; }
	virtual bool16 ControlCmd(IEvent* e)          { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->ControlCmd(e) : kFalse; }
	virtual bool16 KeyDown(IEvent* e)             { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->KeyDown(e) : kFalse; }
	virtual bool16 KeyCmd(IEvent* e)              { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->KeyCmd(e) : kFalse; }
	virtual bool16 KeyUp(IEvent* e)               { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->KeyUp(e) : kFalse; }
	virtual void   PreGetKeyFocus()               { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); if (d) d->PreGetKeyFocus(); }
	virtual void   PreGiveUpKeyFocus()            { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); if (d) d->PreGiveUpKeyFocus(); }
	virtual bool16 WillingToGiveUpKeyFocus()      { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->WillingToGiveUpKeyFocus() : kTrue; }
	virtual bool16 SuspendKeyFocus()              { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->SuspendKeyFocus() : kFalse; }
	virtual bool16 ResumeKeyFocus()               { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->ResumeKeyFocus() : kFalse; }
	virtual bool16 CanHaveKeyFocus() const        { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->CanHaveKeyFocus() : kFalse; }
	virtual bool16 WantsTabKeyFocus() const       { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); return d ? d->WantsTabKeyFocus() : kFalse; }
	virtual void   SetView(IControlView* view)    { InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH); if (d) d->SetView(view); }

private:
	bool   ResolvePageUIDByID(int32 idPage, IDocument*& outDoc, UID& outPageUID) const;
	bool   ResolveRowFromNode(int32 nodeIndex, XRLUIComment& outRow) const;
	bool   ScrollLayoutToPagePoint(IDocument* doc, UID pageUID, PMReal xPct, PMReal yPct) const;
};

CREATE_PMINTERFACE(XRLUICommentsNodeEH, kXRLUICommentsNodeEHImpl)


bool16 XRLUICommentsNodeEH::ButtonDblClk(IEvent* e)
{
	do
	{
		// Row index — the IntNodeID payload our adapter creates.
		InterfacePtr<ITreeNodeIDData> nodeData(this, UseDefaultIID());
		if (nodeData == nil)
			break;
		TreeNodePtr<IntNodeID> nodeID(nodeData->Get());
		if (nodeID == nil || nodeID->Get() < 0)
			break;

		XRLUIComment row;
		if (!this->ResolveRowFromNode(nodeID->Get(), row))
			break;

		// Commentaire sans localisation : rien où naviguer.
		if (!row.hasCoord)
			break;

		IDocument* doc = nil;
		UID pageUID = kInvalidUID;
		if (!this->ResolvePageUIDByID(row.idPage, doc, pageUID))
			break;

		this->ScrollLayoutToPagePoint(doc, pageUID, row.x, row.y);

	} while (kFalse);

	// Forward to the shadow handler so the row also gets selected/hilited.
	InterfacePtr<IEventHandler> d(this, IID_IXRLUICOMMENTSSHADOWEH);
	return d ? d->ButtonDblClk(e) : kFalse;
}


bool XRLUICommentsNodeEH::ResolveRowFromNode(int32 nodeIndex, XRLUIComment& outRow) const
{
	// The data model is hung on the tree view widget (IID_IXRLUIDATAMODEL).
	// Walk up from the row widget to find that slot. Same pattern XPage uses
	// in XPGUIActionComponent for cross-widget look-ups.
	IPMUnknown* dataSlot = Utils<IWidgetUtils>()->QueryRelatedWidget(this, kXRLUICommentsViewWidgetID, IID_IXRLUIDATAMODEL);
	if (dataSlot == nil)
		return false;
	InterfacePtr<IPMUnknownData> dataModel(dataSlot, IID_IXRLUIDATAMODEL);
	if (dataModel == nil)
		return false;
	InterfacePtr<IXRLUICommentList> list(dataModel->GetPMUnknown(), IID_IXRLUICOMMENTLIST);
	if (list == nil || nodeIndex >= list->GetCount())
		return false;

	outRow = list->GetAt(nodeIndex);
	return true;
}


bool XRLUICommentsNodeEH::ResolvePageUIDByID(int32 idPage, IDocument*& outDoc, UID& outPageUID) const
{
	// Cherche la page dont le slug XRail == idPage du commentaire, pour
	// centrer la vue sur la BONNE page (un document peut en contenir plusieurs).
	if (idPage <= 0)
		return false;

	outDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (outDoc == nil)
		return false;

	IDataBase* db = ::GetDataBase(outDoc);
	if (db == nil)
		return false;

	InterfacePtr<ISpreadList> spreadList(outDoc, UseDefaultIID());
	if (spreadList == nil)
		return false;

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
			if (slug != nil && slug->GetID() == idPage)
			{
				outPageUID = pageUID;
				return true;
			}
		}
	}
	return false;
}


bool XRLUICommentsNodeEH::ScrollLayoutToPagePoint(IDocument* doc, UID pageUID, PMReal xPct, PMReal yPct) const
{
	IDataBase* db = ::GetDataBase(doc);
	if (db == nil)
		return false;

	// Compute the target point in the page's inner coordinate system, then
	// transform it to pasteboard so IPanorama can centre on it. The page
	// bounding box gives us width/height; xPct & yPct are in [0,100].
	InterfacePtr<IGeometry> pageGeo(db, pageUID, UseDefaultIID());
	if (pageGeo == nil)
		return false;

	const PMRect pageBounds = pageGeo->GetStrokeBoundingBox();
	const PMReal pw = pageBounds.Right() - pageBounds.Left();
	const PMReal ph = pageBounds.Bottom() - pageBounds.Top();
	if (pw <= 0 || ph <= 0)
		return false;

	PMPoint pbPoint(pageBounds.Left() + (xPct / 100.0) * pw,
	                pageBounds.Top()  + (yPct / 100.0) * ph);
	::TransformInnerPointToPasteboard(pageGeo, &pbPoint);

	IControlView* layoutView = Utils<ILayoutUIUtils>()->QueryFrontView();
	if (layoutView == nil)
		return false;

	InterfacePtr<IPanorama> panorama(layoutView, UseDefaultIID());
	if (panorama == nil)
		return false;

	panorama->ScrollViewCenterTo(pbPoint, kTrue);
	return true;
}
