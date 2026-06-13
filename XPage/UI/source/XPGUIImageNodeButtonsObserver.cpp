// XPGUIImageNodeButtonsObserver.cpp
//
// Per-row observer aggregated on kXPGUIImageNodeWidgetBoss that listens to
// the two action icons appended to each Photos liées list row:
//   * kXPGUIImageGotoLinkBtnWidgetID    — selects + scrolls to the photo's
//                                         frame in the layout (if it's
//                                         already placed on a spread).
//   * kXPGUIImageEditOriginalBtnWidgetID — opens the high-res image with
//                                          the OS default app (Photoshop,
//                                          Aperçu, …).
//

#include "VCPlugInHeaders.h"

// Interface includes
#include "ISubject.h"
#include "IControlView.h"
#include "IPanelControlData.h"
#include "ITreeNodeIDData.h"
#include "ITriStateControlData.h"
#include "IDocument.h"
#include "ISpreadList.h"
#include "ISpread.h"
#include "ISelectionManager.h"
#include "ISelectionUtils.h"
#include "ILayoutSelectionSuite.h"
#include "ILayoutUIUtils.h"
#include "IFormeData.h"
#include "IPlacedArticleData.h"
#include "IHierarchy.h"
#include "ILinkManager.h"
#include "ILink.h"
#include "ILinkResource.h"
#include "IURIUtils.h"

// General includes
#include "CObserver.h"
#include "FileUtils.h"
#include "Utils.h"
#include "K2Vector.tpp"
#include "CAlert.h"
#include "URI.h"

#include "WidgetID.h"

#include <cstdlib>  // for std::system on the "edit original" path.

// Project includes
#include "XPGUIID.h"
#include "XPGID.h"
#include "XPGUIImageNodeID.h"
#include "XPGImageDataNode.h"


/**
 * Aggregated on kXPGUIImageNodeWidgetBoss with IID_IOBSERVER. AutoAttach
 * subscribes to IID_ITRISTATECONTROLDATA on the two action buttons of the
 * row; Update() routes the kTrueStateMessage to the right action.
 */
class XPGUIImageNodeButtonsObserver : public CObserver
{
public:
	XPGUIImageNodeButtonsObserver(IPMUnknown* boss);
	virtual ~XPGUIImageNodeButtonsObserver();

	virtual void AutoAttach();
	virtual void AutoDetach();
	virtual void Update(const ClassID& theChange, ISubject* theSubject,
	                    const PMIID& protocol, void* changedBy);

private:
	void GotoLinkedFrame(const PMString& articleId, const PMString& imageId);
	void OpenOriginalImage(const PMString& hrFilePath);

	void AttachButton(const InterfacePtr<IPanelControlData>& panelControlData,
	                  const WidgetID& widgetID);
	void DetachButton(const InterfacePtr<IPanelControlData>& panelControlData,
	                  const WidgetID& widgetID);
};

CREATE_PMINTERFACE(XPGUIImageNodeButtonsObserver, kXPGUIImageNodeButtonsObserverImpl)


XPGUIImageNodeButtonsObserver::XPGUIImageNodeButtonsObserver(IPMUnknown* boss)
	: CObserver(boss) {}

XPGUIImageNodeButtonsObserver::~XPGUIImageNodeButtonsObserver() {}


void XPGUIImageNodeButtonsObserver::AutoAttach()
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;
	this->AttachButton(panelControlData, kXPGUIImageGotoLinkBtnWidgetID);
	this->AttachButton(panelControlData, kXPGUIImageEditOriginalBtnWidgetID);
}


void XPGUIImageNodeButtonsObserver::AutoDetach()
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;
	this->DetachButton(panelControlData, kXPGUIImageGotoLinkBtnWidgetID);
	this->DetachButton(panelControlData, kXPGUIImageEditOriginalBtnWidgetID);
}


void XPGUIImageNodeButtonsObserver::AttachButton(
	const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID)
{
	IControlView* widget = panelControlData->FindWidget(widgetID);
	if (widget == nil)
		return;
	InterfacePtr<ISubject> subject(widget, UseDefaultIID());
	if (subject != nil && !subject->IsAttached(this, IID_ITRISTATECONTROLDATA))
		subject->AttachObserver(this, IID_ITRISTATECONTROLDATA);
}


void XPGUIImageNodeButtonsObserver::DetachButton(
	const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID)
{
	IControlView* widget = panelControlData->FindWidget(widgetID);
	if (widget == nil)
		return;
	InterfacePtr<ISubject> subject(widget, UseDefaultIID());
	if (subject != nil && subject->IsAttached(this, IID_ITRISTATECONTROLDATA))
		subject->DetachObserver(this, IID_ITRISTATECONTROLDATA);
}


void XPGUIImageNodeButtonsObserver::Update(const ClassID& theChange, ISubject* theSubject,
                                            const PMIID& protocol, void* changedBy)
{
	// IconButtonWidget broadcasts kTrueStateMessage via IID_ITRISTATECONTROLDATA
	// on click; ignore everything else.
	if (theChange != kTrueStateMessage)
		return;

	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());
	if (controlView == nil)
		return;

	const WidgetID widget = controlView->GetWidgetID();
	if (widget != kXPGUIImageGotoLinkBtnWidgetID &&
	    widget != kXPGUIImageEditOriginalBtnWidgetID)
		return;

	// Look up the tree node we're sitting on.
	InterfacePtr<ITreeNodeIDData> nodeData(this, UseDefaultIID());
	if (nodeData == nil)
		return;
	TreeNodePtr<XPGUIImageNodeID> nodeID(nodeData->Get());
	if (!nodeID)
		return;
	if (nodeID->GetType() != XPGImageDataNode::kImageNode)
		return; // root rubrique row — no action.

	const XPGImageDataNode::ImageData* img = nodeID->GetImageData();
	if (img == nil)
		return;

	if (widget == kXPGUIImageGotoLinkBtnWidgetID)
	{
		// Match the placed frame by Gaia image id (e.g. "51226"). The
		// image id is the node's primary key — set by the model when it
		// pushes the photo into the tree (cf. XPGTECImageModel::cacheChildren).
		// Matching by id is more robust than matching by hrFile basename:
		// hrFile carries an absolute path that can differ between machines
		// and the basename comparison was case-sensitive.
		this->GotoLinkedFrame(img->articleID, nodeID->GetID());
	}
	else
	{
		this->OpenOriginalImage(img->hrFile);
	}
}


/* GotoLinkedFrame
 *
 * Walks every spread of the front document looking for a graphic frame
 * whose placed-image link contains the photo's image id (e.g. "51226").
 *
 * Matching is done by substring on the link's URI: Gaia HR/BR filenames
 * have the form "<imageId>.HR.<ext>" / "<imageId>.BR.jpg", so testing
 * "linkPath contains <imageId>" is sufficient and robust against:
 *   - case differences (Windows is case-insensitive on filenames)
 *   - absolute path differences (machine A vs machine B)
 *   - HR vs BR vs other variants
 *   - URL-encoded characters in the URI form returned by ILinkResource
 *
 * The compare is case-insensitive and brackets the id with a dot to avoid
 * a numeric match inside an unrelated path segment (e.g. id "51226" inside
 * "51226456" would not match since "51226." won't appear there).
 *
 * The first match is selected via the layout selection suite, which
 * scrolls the layout to bring it into view (kAlwaysCenterInView).
 */
void XPGUIImageNodeButtonsObserver::GotoLinkedFrame(const PMString& articleId, const PMString& imageId)
{
	if (imageId.IsEmpty())
		return;
	(void)articleId; // kept for debugging / future use; matching is by id.

	IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (doc == nil)
		return;

	IDataBase* db = ::GetDataBase(doc);
	if (db == nil)
		return;

	InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
	if (spreadList == nil)
		return;

	InterfacePtr<ILinkManager> linkMgr(db, db->GetRootUID(), UseDefaultIID());
	if (linkMgr == nil)
		return;

	// Pattern we look for in link paths — wrap the id with a trailing dot
	// so "51226" doesn't match a path containing "512260". Lowercased
	// for case-insensitive comparison.
	PMString needle(imageId);
	needle.Append(".");
	needle.ToLower();

	UIDRef foundRef = UIDRef::gNull;
	const int32 nbSpreads = spreadList->GetSpreadCount();
	for (int32 s = 0; s < nbSpreads && foundRef == UIDRef::gNull; ++s)
	{
		InterfacePtr<ISpread> spread(db, spreadList->GetNthSpreadUID(s), UseDefaultIID());
		if (spread == nil)
			continue;

		UIDList itemsOnSpread(db);
		const int32 nbPages = spread->GetNumPages();
		for (int32 p = 0; p < nbPages; ++p)
			spread->GetItemsOnPage(p, &itemsOnSpread, kFalse, kTrue);

		// Inline group recursion via IHierarchy. GetItemsOnPage only
		// returns top-level page items — a photo nested inside a group
		// would otherwise be unreachable, leaving "atteindre le lien"
		// silent for grouped articles.
		for (int32 i = 0; i < itemsOnSpread.Length() && foundRef == UIDRef::gNull; ++i)
		{
			UIDRef itemRef = itemsOnSpread.GetRef(i);

			// Walk into the frame to find its placed image (the graphic
			// frame holds an inner page item which carries the link).
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
				if (link == nil)
					continue;
				InterfacePtr<ILinkResource> linkRes(db, link->GetResource(), UseDefaultIID());
				if (linkRes == nil)
					continue;

				URI linkURI = linkRes->GetId();
				// URI::GetURI() returns a std::string — wrap into PMString.
				PMString linkPath(linkURI.GetURI().c_str(), PMString::kUnknownEncoding);
				linkPath.ToLower();
				if (linkPath.IndexOfString(needle) >= 0)
				{
					foundRef = itemRef;
					break;
				}
			}
		}
	}

	if (foundRef == UIDRef::gNull)
		return; // photo isn't placed in this document — silently bail.

	// Replace the current selection with the found frame. The layout
	// selection suite scrolls the document view to make it visible.
	InterfacePtr<ISelectionManager> selMgr(Utils<ISelectionUtils>()->QueryActiveSelection());
	if (selMgr == nil)
		return;

	InterfacePtr<ILayoutSelectionSuite> layoutSelSuite(selMgr, UseDefaultIID());
	if (layoutSelSuite == nil)
		return;

	layoutSelSuite->SelectPageItems(UIDList(foundRef), Selection::kReplace,
		Selection::kAlwaysCenterInView);
}


/* OpenOriginalImage
 *
 * Hands the high-res image off to the OS shell so the user's default
 * picture editor takes over (Photoshop on Windows, Aperçu / Preview on
 * macOS — whatever the system has registered for the file's extension).
 *
 * The SDK doesn't expose a portable "open with default app" API, so we
 * shell out via std::system using the platform-specific wrapper command:
 *   * Windows: `start "" "<path>"`
 *   * macOS:   `open "<path>"`
 */
void XPGUIImageNodeButtonsObserver::OpenOriginalImage(const PMString& hrFilePath)
{
	if (hrFilePath.IsEmpty())
		return;

	IDFile imageFile = FileUtils::PMStringToSysFile(hrFilePath);
	if (!FileUtils::DoesFileExist(imageFile))
	{
		PMString msg("Fichier image introuvable : ", PMString::kUnknownEncoding);
		msg.Append(hrFilePath);
		msg.SetTranslatable(kFalse);
		CAlert::InformationAlert(msg);
		return;
	}

	PMString cmd;
#if defined(WINDOWS) || defined(_WIN32)
	// `start ""` is the idiom that handles paths with spaces; the empty
	// title argument keeps cmd from interpreting the quoted path as a title.
	cmd.SetCString("start \"\" \"", PMString::kUnknownEncoding);
	cmd.Append(hrFilePath);
	cmd.Append("\"");
#else
	cmd.SetCString("open \"", PMString::kUnknownEncoding);
	cmd.Append(hrFilePath);
	cmd.Append("\"");
#endif
	cmd.SetTranslatable(kFalse);
	std::system(cmd.GetPlatformString().c_str());
}


// End, XPGUIImageNodeButtonsObserver.cpp.
