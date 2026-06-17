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
	int32     idPage;        // Gaia page the comment belongs to (0 = no location).
	PMString  commentaire;
	PMString  dateISO;       // raw "YYYY-MM-DDT…Z" — not pre-formatted.
	int32     heureSecs;     // seconds since midnight (server local time).
	PMString  nomUser;
	bool      check;
	bool      hasCoord;      // false when idPage == 0 (no pin to draw).
	PMReal    x;             // % of page width  (normalized to single page).
	PMReal    y;             // % of page height (normalized to single page).
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

	/** Met à jour le flag `check` d'une ligne en mémoire, sans rediffuser.
		Appelé après un toggle utilisateur de la case : la case affiche déjà le
		nouvel état, on garde juste le modèle synchrone pour que le recyclage du
		tree view ne réaffiche pas l'ancienne valeur. No-op si index hors borne.
	*/
	virtual void                   SetCheckAt(int32 index, bool check) = 0;

	virtual void                   Clear() = 0;
};

/** Subject change-message broadcast by SetRows()/Clear(). */
DECLARE_PMID(kMessageIDSpace, kXRLUICommentListChangedMsg, kXRLUIPrefix + 1)

#endif // __IXRLUICommentList__
