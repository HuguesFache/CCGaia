#include "VCPlugInHeaders.h"
#include <vector>

#include "CPMUnknown.h"
#include "ISubject.h"

#include "IXRLUICommentList.h"
#include "XRLUIID.h"

#include <algorithm>

/** Aggregated on kXRLUICommentsPanelWidgetBoss. Holds the comment rows the
	panel observer fetched. SetRows sorts descending by (date, heure) so the
	newest comments come first — matches the screenshot the user provided.
*/
class XRLUICommentList : public CPMUnknown<IXRLUICommentList>
{
public:
	XRLUICommentList(IPMUnknown* boss) : CPMUnknown<IXRLUICommentList>(boss) {}
	virtual ~XRLUICommentList() {}

	virtual int32 GetCount() const                        { return static_cast<int32>(fRows.size()); }
	virtual const XRLUIComment& GetAt(int32 index) const  { return fRows[index]; }

	virtual void SetRows(const std::vector<XRLUIComment>& newRows);
	virtual void Clear();

private:
	std::vector<XRLUIComment> fRows;

	void Broadcast();
};

CREATE_PMINTERFACE(XRLUICommentList, kXRLUICommentListImpl)

void XRLUICommentList::SetRows(const std::vector<XRLUIComment>& newRows)
{
	fRows = newRows;

	// Sort newest first. Compare as (dateISO, heureSecs) lexicographic on
	// the ISO date — that ordering matches calendar order even across years.
	std::sort(fRows.begin(), fRows.end(), [](const XRLUIComment& a, const XRLUIComment& b) {
		const int cmp = a.dateISO.Compare(kFalse, b.dateISO);
		if (cmp != 0)
			return cmp > 0; // descending date
		return a.heureSecs > b.heureSecs;
	});

	this->Broadcast();
}

void XRLUICommentList::Clear()
{
	fRows.clear();
	this->Broadcast();
}

void XRLUICommentList::Broadcast()
{
	InterfacePtr<ISubject> subject(this, UseDefaultIID());
	if (subject != nil)
		subject->Change(kXRLUICommentListChangedMsg, IID_IXRLUICOMMENTLIST, nil);
}
