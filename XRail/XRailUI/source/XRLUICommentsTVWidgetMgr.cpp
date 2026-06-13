#include "VCPlugInHeaders.h"

#include "CTreeViewWidgetMgr.h"
#include "CreateObject.h"    // ::CreateObject(...)
#include "IPanelControlData.h"
#include "ITextControlData.h"
#include "ITriStateControlData.h"
#include "IControlView.h"
#include "IPMUnknownData.h"
#include "NodeID.h"          // TreeNodePtr<>
#include "IntNodeID.h"
#include "PersistUtils.h"    // ::GetDataBase
#include "RsrcSpec.h"
#include "CoreResTypes.h"    // kViewRsrcType
#include "LocaleSetting.h"   // LocaleSetting::GetLocale

#include "XRLUIID.h"
#include "IXRLUICommentList.h"

/** Cell renderer for the Commentaires palette tree view. The tree is flat —
	root is a sentinel, every visible row is a leaf created from
	IntNodeID(rowIndex) by the hierarchy adapter. ApplyNodeIDToWidget pulls
	the matching XRLUIComment record from the IXRLUICommentList held on the
	panel boss and writes it into the cell widgets defined in
	XRLUICommentsNodeWidget.
*/
class XRLUICommentsTVWidgetMgr : public CTreeViewWidgetMgr
{
public:
	XRLUICommentsTVWidgetMgr(IPMUnknown* boss) : CTreeViewWidgetMgr(boss) {}
	virtual ~XRLUICommentsTVWidgetMgr() {}

	virtual WidgetID      GetWidgetTypeForNode(const NodeID& node) const;
	virtual IControlView* CreateWidgetForNode(const NodeID& node) const;
	virtual bool16        ApplyNodeIDToWidget(const NodeID& node, IControlView* widget, int32 message = 0) const;

private:
	IPMUnknown* QueryListModel() const;
	static void SetCellText(IPanelControlData* row, WidgetID wid, const PMString& text);
	static PMString FormatDate(const PMString& dateISO);   // "YYYY-MM-DD…" -> "DD/MM"
	static PMString FormatTime(int32 secsSinceMidnight);   // 82335 -> "HH:MM"
};

CREATE_PMINTERFACE(XRLUICommentsTVWidgetMgr, kXRLUICommentsTVWidgetMgrImpl)

IPMUnknown* XRLUICommentsTVWidgetMgr::QueryListModel() const
{
	InterfacePtr<IPMUnknownData> dataModel(this, IID_IXRLUIDATAMODEL);
	if (dataModel == nil)
		return nil;
	return dataModel->GetPMUnknown();
}

WidgetID XRLUICommentsTVWidgetMgr::GetWidgetTypeForNode(const NodeID& /*node*/) const
{
	// Identifies the per-row template among the children of the panel widget
	// — used by ApplyNodeIDToWidget consumers in some code paths. The actual
	// instantiation goes through CreateWidgetForNode below, which loads the
	// resource directly so the framework's RegisterStyles flow (default
	// would point at Adobe's standard tree node, not ours) is bypassed.
	return kXRLUICommentsNodeWidgetTemplateID;
}

IControlView* XRLUICommentsTVWidgetMgr::CreateWidgetForNode(const NodeID& /*node*/) const
{
	// Instantiate our custom node widget by (pluginID, resourceID). The
	// default CTreeViewWidgetMgr::CreateWidgetForNode uses an internal style
	// rsrc spec set by RegisterStyles, which defaults to Adobe's standard
	// tree node — that's why GetNodeWidgetWidth was crashing trying to load
	// a stock resource we don't ship. Same pattern XPGUIImagesTVWidgetMgr
	// uses to load its own per-row template.
	IControlView* view = (IControlView*) ::CreateObject(
		::GetDataBase(this),
		RsrcSpec(LocaleSetting::GetLocale(),
			kXRLUIPluginID,
			kViewRsrcType,
			kXRLUICommentsNodeWidgetResID),
		IID_ICONTROLVIEW);
	ASSERT(view);
	return view;
}

bool16 XRLUICommentsTVWidgetMgr::ApplyNodeIDToWidget(const NodeID& node, IControlView* widget, int32 message) const
{
	CTreeViewWidgetMgr::ApplyNodeIDToWidget(node, widget, message);

	TreeNodePtr<IntNodeID> id(node);
	if (id == nil || id->Get() < 0)
		return kFalse;

	InterfacePtr<IXRLUICommentList> list(this->QueryListModel(), IID_IXRLUICOMMENTLIST);
	if (list == nil)
		return kFalse;
	const int32 idx = id->Get();
	if (idx >= list->GetCount())
		return kFalse;
	const XRLUIComment& row = list->GetAt(idx);

	InterfacePtr<IPanelControlData> rowData(widget, UseDefaultIID());
	if (rowData == nil)
		return kFalse;

	// Date/time: ISO date down to "DD/MM", heureSecs to "HH:MM". A two-line
	// cell ("05/05\nà 22:52") would match the screenshot more closely but
	// requires a multi-line widget; single-line "05/05 à 22:52" is good
	// enough for v1 and keeps the row height minimal.
	PMString dateText = FormatDate(row.dateISO);
	dateText += " à ";
	dateText += FormatTime(row.heureSecs);

	SetCellText(rowData, kXRLUICommentsCellDateWidgetID, dateText);
	SetCellText(rowData, kXRLUICommentsCellTextWidgetID, row.commentaire);
	SetCellText(rowData, kXRLUICommentsCellUserWidgetID, row.nomUser);

	IControlView* checkView = rowData->FindWidget(kXRLUICommentsCellCheckWidgetID);
	if (checkView != nil)
	{
		InterfacePtr<ITriStateControlData> tri(checkView, UseDefaultIID());
		if (tri != nil)
			tri->SetState(row.check ? ITriStateControlData::kSelected : ITriStateControlData::kUnselected, kFalse, kFalse);
	}

	return kTrue;
}

void XRLUICommentsTVWidgetMgr::SetCellText(IPanelControlData* row, WidgetID wid, const PMString& text)
{
	if (row == nil)
		return;
	IControlView* view = row->FindWidget(wid);
	if (view == nil)
		return;
	InterfacePtr<ITextControlData> tc(view, UseDefaultIID());
	if (tc != nil)
		tc->SetString(text);
}

PMString XRLUICommentsTVWidgetMgr::FormatDate(const PMString& dateISO)
{
	// Expected shape: "YYYY-MM-DDT…" (ISO 8601, 10+ chars). Render as DD/MM.
	// Bail out on anything shorter — show the raw string so the user can
	// see what the server actually returned.
	if (dateISO.NumUTF16TextChars() < 10)
		return dateISO;
	K2::scoped_ptr<PMString> month(dateISO.Substring(5, 2));
	K2::scoped_ptr<PMString> day(dateISO.Substring(8, 2));
	if (month == nil || day == nil)
		return dateISO;
	PMString out = *day;
	out += "/";
	out += *month;
	return out;
}

PMString XRLUICommentsTVWidgetMgr::FormatTime(int32 secsSinceMidnight)
{
	if (secsSinceMidnight < 0)
		secsSinceMidnight = 0;
	const int32 hh = (secsSinceMidnight / 3600) % 24;
	const int32 mm = (secsSinceMidnight / 60) % 60;
	PMString out;
	if (hh < 10) out += "0";
	out.AppendNumber(hh);
	out += ":";
	if (mm < 10) out += "0";
	out.AppendNumber(mm);
	return out;
}
