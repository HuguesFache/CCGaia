#include "VCPlugInHeaders.h"

#include "CPMUnknown.h"
#include "ITreeViewHierarchyAdapter.h"
#include "IPMUnknownData.h"
#include "NodeID.h"          // TreeNodePtr<>
#include "IntNodeID.h"

#include "XRLUIID.h"
#include "IXRLUICommentList.h"

/** Flat-list hierarchy adapter for the Commentaires palette.

	Root: IntNodeID(-1) — sentinel; never displayed (the tree view's view-root
	is set to the actual root, but the children of root are the visible rows).
	Leaves: IntNodeID(rowIndex), 0..GetCount()-1 — index into IXRLUICommentList.

	Aggregated on kXRLUICommentsViewWidgetBoss alongside the widget mgr; both
	pull the comment list from IID_IDATAMODEL set by the panel observer.
*/
class XRLUICommentsTVHierarchyAdapter : public CPMUnknown<ITreeViewHierarchyAdapter>
{
public:
	XRLUICommentsTVHierarchyAdapter(IPMUnknown* boss) : CPMUnknown<ITreeViewHierarchyAdapter>(boss) {}
	virtual ~XRLUICommentsTVHierarchyAdapter() {}

	virtual NodeID_rv GetRootNode() const;
	virtual NodeID_rv GetParentNode(const NodeID& node) const;
	virtual int32     GetNumChildren(const NodeID& node) const;
	virtual bool16    ShouldAddNthChild(const NodeID& node, const int32& nth) const;
	virtual NodeID_rv GetNthChild(const NodeID& node, const int32& nth) const;
	virtual int32     GetChildIndex(const NodeID& parent, const NodeID& child) const;
	virtual NodeID_rv GetGenericNodeID() const;
	virtual bool16    GenericNodeIsEqual(const NodeID& node1, const NodeID& node2) const;

private:
	// Resolves the comment list via the tree view's IID_IDATAMODEL slot
	// (set by the panel observer). Caller is expected to wrap the returned
	// IPMUnknown* in an `InterfacePtr<IXRLUICommentList>` with the standard
	// `(IPMUnknown*, PMIID)` constructor — that constructor takes care of
	// the QueryInterface + Release dance.
	IPMUnknown* QueryListModel() const;
};

CREATE_PMINTERFACE(XRLUICommentsTVHierarchyAdapter, kXRLUICommentsTVHierarchyAdapterImpl)

IPMUnknown* XRLUICommentsTVHierarchyAdapter::QueryListModel() const
{
	InterfacePtr<IPMUnknownData> dataModel(this, IID_IXRLUIDATAMODEL);
	if (dataModel == nil)
		return nil;
	return dataModel->GetPMUnknown();
}

NodeID_rv XRLUICommentsTVHierarchyAdapter::GetRootNode() const
{
	return IntNodeID::Create(-1);
}

NodeID_rv XRLUICommentsTVHierarchyAdapter::GetParentNode(const NodeID& node) const
{
	TreeNodePtr<IntNodeID> id(node);
	if (id == nil || id->Get() == -1)
		return kInvalidNodeID;
	return IntNodeID::Create(-1);
}

int32 XRLUICommentsTVHierarchyAdapter::GetNumChildren(const NodeID& node) const
{
	TreeNodePtr<IntNodeID> id(node);
	if (id == nil || id->Get() != -1)
		return 0;

	InterfacePtr<IXRLUICommentList> list(this->QueryListModel(), IID_IXRLUICOMMENTLIST);
	if (list == nil)
		return 0;
	return list->GetCount();
}

bool16 XRLUICommentsTVHierarchyAdapter::ShouldAddNthChild(const NodeID& /*node*/, const int32& /*nth*/) const
{
	// Flat list — every child the model exposes is shown.
	return kTrue;
}

NodeID_rv XRLUICommentsTVHierarchyAdapter::GetNthChild(const NodeID& node, const int32& nth) const
{
	TreeNodePtr<IntNodeID> id(node);
	if (id == nil || id->Get() != -1)
		return kInvalidNodeID;

	InterfacePtr<IXRLUICommentList> list(this->QueryListModel(), IID_IXRLUICOMMENTLIST);
	if (list == nil || nth < 0 || nth >= list->GetCount())
		return kInvalidNodeID;
	return IntNodeID::Create(nth);
}

int32 XRLUICommentsTVHierarchyAdapter::GetChildIndex(const NodeID& parent, const NodeID& child) const
{
	TreeNodePtr<IntNodeID> p(parent);
	TreeNodePtr<IntNodeID> c(child);
	if (p == nil || c == nil || p->Get() != -1)
		return -1;
	const int32 idx = c->Get();
	if (idx < 0)
		return -1;
	return idx;
}

NodeID_rv XRLUICommentsTVHierarchyAdapter::GetGenericNodeID() const
{
	// MUST be a fresh, real NodeID instance — the framework uses it as a
	// template during workspace deserialization (CTreeViewController::
	// ReadWrite calls Read on this instance). Returning kInvalidNodeID
	// crashes in NodeID::SetNodeIDTemplate at InDesign startup.
	return IntNodeID::Create(0);
}

bool16 XRLUICommentsTVHierarchyAdapter::GenericNodeIsEqual(const NodeID& a, const NodeID& b) const
{
	TreeNodePtr<IntNodeID> ia(a);
	TreeNodePtr<IntNodeID> ib(b);
	if (ia == nil || ib == nil)
		return kFalse;
	return ia->Get() == ib->Get();
}
