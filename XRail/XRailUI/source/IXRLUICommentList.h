#ifndef __IXRLUICommentList__
#define __IXRLUICommentList__

#include "IPMUnknown.h"
#include <vector>
#include "PMString.h"
#include "K2Vector.h"
#include "XRLUIID.h"

/** One comment row, as fetched from Plugin_Comm_List. Field names mirror
	the JSON. X/Y are page-relative percentages.
*/
struct XRLUIComment
{
	int32     idComm;
	PMString  commentaire;
	PMString  dateISO;       // raw "YYYY-MM-DDT…Z" — not pre-formatted.
	int32     heureSecs;     // seconds since midnight (server local time).
	PMString  nomUser;
	bool      check;
	PMReal    x;             // % of page width
	PMReal    y;             // % of page height
};

/** Data model for the Commentaires palette. Aggregated on the panel widget
	boss; the observer fills it on refresh, the tree view's adapter + widget
	mgr read from it via the IID_IDATAMODEL pointer hung on the tree view.
*/
class IXRLUICommentList : public IPMUnknown
{
public:
	enum { kDefaultIID = IID_IXRLUICOMMENTLIST };

	virtual int32                  GetCount() const = 0;
	virtual const XRLUIComment&    GetAt(int32 index) const = 0;

	/** Replace the contents with `newRows`, sorted by date+time descending
		(newest first). Caller hands ownership of the data — implementation
		copies. Broadcasts kCommentListChangedMsg on the panel widget's
		ISubject so observers (the tree view) refresh.
	*/
	virtual void                   SetRows(const std::vector<XRLUIComment>& newRows) = 0;

	virtual void                   Clear() = 0;
};

/** Subject change-message broadcast by SetRows()/Clear(). */
DECLARE_PMID(kMessageIDSpace, kXRLUICommentListChangedMsg, kXRLUIPrefix + 1)

#endif // __IXRLUICommentList__
