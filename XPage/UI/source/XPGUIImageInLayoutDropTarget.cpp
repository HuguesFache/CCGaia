// XPGUIImageInLayoutDropTarget.cpp
//
// Layout-level (kLayoutDDTargHelperProvider) drop target for the
// XPageImageFlavor flavor produced by the Photos liées palette drag source
// (XPGUIXRailImageDragDropSource).
//
// The on-frame helper (XPGUIXRailImageDropTarget, registered as
// kPageItemDDTargHelperProvider on kXPGUIImageInPageItemHelperBoss) handles
// drops on graphic frames — the original "drop on a photo block to import"
// flow.
//
// This layout-level helper picks up the cases the on-frame helper doesn't:
//   - Drop on empty pasteboard  → open the "Choose photo carton" dialog,
//                                  place the carton at the drop point, then
//                                  import the dragged image into each of
//                                  the carton's photo frames.
//   - Drop on a non-graphic frame (text frame, etc.) → silent no-op. We
//                                  don't want to import anything in this
//                                  case (per spec), so we just bail out.
//
// The hit-test that distinguishes "empty pasteboard" from "non-graphic
// frame underneath" runs in ProcessDragDropCommand, since CouldAcceptTypes
// is called continuously during tracking and we want to keep it cheap +
// flavor-only.

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IPMDataObject.h"
#include "IFlavorData.h"
#include "ISysFileListData.h"
#include "IDataExchangeHandler.h"
#include "IDocument.h"
#include "IControlView.h"
#include "ILayoutUIUtils.h"
#include "ILayoutControlData.h"
#include "IPasteboardUtils.h"
#include "ISpread.h"
#include "IFrameType.h"
#include "IGeometry.h"
#include "IStringData.h"
#include "IHierarchy.h"
#include "ISpreadList.h"

// General includes:
#include "CDragDropTargetFlavorHelper.h"
#include "PMFlavorTypes.h"
#include "DataObjectIterator.h"
#include "FileUtils.h"
#include "TransformUtils.h"
#include "UIDList.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "XPageUIUtils.h"


class XPGUIImageInLayoutDropTarget : public CDragDropTargetFlavorHelper
{
public:
	XPGUIImageInLayoutDropTarget(IPMUnknown* boss);

	DragDrop::TargetResponse CouldAcceptTypes(const IDragDropTarget* target,
		DataObjectIterator* dataIter, const IDragDropSource* fromSource,
		const IDragDropController* controller) const;

	ErrorCode ProcessDragDropCommand(IDragDropTarget* target,
		IDragDropController* controller, DragDrop::eCommandType action);

	virtual bool16 HelperWillDoDragTracking() const { return kTrue; }
	virtual void   DoDragEnter(IDragDropTarget* target);

	DECLARE_HELPER_METHODS()

private:
	// Hit-test: walks the items on `spread` and returns the topmost one
	// whose stroke bounding box contains `pt` (pasteboard coords). Returns
	// UIDRef::gNull on miss.
	static UIDRef FindItemAtPoint(IDataBase* db, ISpread* spread, const PMPoint& pt);
};

CREATE_PMINTERFACE(XPGUIImageInLayoutDropTarget, kXPGUIImageInLayoutDropTargetImpl)
DEFINE_HELPER_METHODS(XPGUIImageInLayoutDropTarget)


XPGUIImageInLayoutDropTarget::XPGUIImageInLayoutDropTarget(IPMUnknown* boss)
	: CDragDropTargetFlavorHelper(boss), HELPER_METHODS_INIT(boss)
{
}


DragDrop::TargetResponse XPGUIImageInLayoutDropTarget::CouldAcceptTypes(
	const IDragDropTarget* /*target*/, DataObjectIterator* dataIter,
	const IDragDropSource* /*fromSource*/, const IDragDropController* /*controller*/) const
{
	DataExchangeResponse response = dataIter->FlavorExistsWithPriorityInAllObjects(XPageImageFlavor);
	if (response.CanDo()) {
		return DragDrop::TargetResponse(response, DragDrop::kDropWillCopy,
			DragDrop::kUseDefaultTrackingFeedback,
			DragDrop::kTargetWillProvideCursorFeedback);
	}
	return DragDrop::kWontAcceptTargetResponse;
}


/*
 * ProcessDragDropCommand
 *
 * Branch on what's under the drop point:
 *   - graphic frame  → no-op (the on-frame helper has already handled it)
 *   - other frame    → no-op (per-spec: no import on non-photo frames)
 *   - empty space    → open the "Choose photo carton" dialog
 */
ErrorCode XPGUIImageInLayoutDropTarget::ProcessDragDropCommand(
	IDragDropTarget* target, IDragDropController* controller, DragDrop::eCommandType action)
{
	if (action != DragDrop::kDropCommand)
		return kFailure;

	if (controller->InternalizeDrag(XPageImageFlavor, XPageImageFlavor) != kSuccess)
		return kFailure;

	InterfacePtr<IDataExchangeHandler> handler(controller->QuerySourceHandler());
	if (handler == nil)
		return kFailure;

	InterfacePtr<ISysFileListData> fileData(handler, IID_ISYSFILELISTDATA);
	if (fileData == nil || fileData->Length() == 0)
		return kFailure;
	const IDFile imageFile = fileData->GetSysFileItem(0);

	// Drag payload: credit/legend text + article id (carried by drag source).
	InterfacePtr<IStringData> creditData(handler, IID_ICREDITPHOTO);
	InterfacePtr<IStringData> legendData(handler, IID_ILEGENDPHOTO);
	InterfacePtr<IStringData> articleIdData(handler, IID_IPHOTOARTICLEID);
	InterfacePtr<IStringData> cropDataData(handler, IID_IPHOTOCROPDATA);
	const PMString creditText      = (creditData != nil)    ? creditData->Get()    : PMString();
	const PMString legendText      = (legendData != nil)    ? legendData->Get()    : PMString();
	const PMString draggedArticleId = (articleIdData != nil) ? articleIdData->Get() : PMString();
	const PMString cropData        = (cropDataData != nil)  ? cropDataData->Get()  : PMString();

	// Resolve drop point in pasteboard coordinates.
	InterfacePtr<IControlView> layoutView(target, UseDefaultIID());
	if (layoutView == nil)
		return kFailure;
	GSysPoint where = controller->GetDragMouseLocation();
	const PMPoint dropPoint = Utils<ILayoutUIUtils>()->GlobalToPasteboard(layoutView, where);

	InterfacePtr<ISpread> targetSpread(Utils<IPasteboardUtils>()->QueryNearestSpread(layoutView, dropPoint));
	if (targetSpread == nil)
		return kFailure;

	InterfacePtr<ILayoutControlData> layoutCtrlData(layoutView, UseDefaultIID());
	IDocument* doc = layoutCtrlData ? layoutCtrlData->GetDocument() : nil;
	if (doc == nil)
		return kFailure;
	IDataBase* db = ::GetDataBase(doc);
	if (db == nil)
		return kFailure;

	// Hit-test: is there a page item under the drop point?
	const UIDRef hitItem = FindItemAtPoint(db, targetSpread, dropPoint);
	if (hitItem != UIDRef::gNull) {
		// On graphic frame: the page-item helper already handled the drop,
		// don't double-process. On any other frame: per spec, do nothing.
		// Either way, silent return.
		return kSuccess;
	}

	// Empty pasteboard — open the dialog.
	XPageUIUtils::DisplayChoosePhotoCartonDialog(imageFile, creditText, legendText,
		draggedArticleId, dropPoint, ::GetUIDRef(targetSpread), cropData);

	return kSuccess;
}


void XPGUIImageInLayoutDropTarget::DoDragEnter(IDragDropTarget* /*target*/)
{
	InterfacePtr<IDragDropController> controller(GetExecutionContextSession(), UseDefaultIID());
	controller->SetTrackingCursorFeedback(CursorSpec(kCrsrItemCreation));
}


UIDRef XPGUIImageInLayoutDropTarget::FindItemAtPoint(IDataBase* db, ISpread* spread, const PMPoint& pt)
{
	if (db == nil || spread == nil)
		return UIDRef::gNull;

	UIDList items(db);
	const int32 nbPages = spread->GetNumPages();
	for (int32 p = 0; p < nbPages; ++p)
		spread->GetItemsOnPage(p, &items, kFalse, kTrue);

	// Iterate top-down (most recently added items are typically rendered on
	// top in InDesign's drawing order — k2 lists keep insertion order). The
	// loop returns the first containing item it finds; for our purposes that's
	// good enough since we only branch "is anything under cursor or not".
	const int32 n = items.Length();
	for (int32 i = n - 1; i >= 0; --i) {
		UIDRef itemRef = items.GetRef(i);
		InterfacePtr<IGeometry> geo(itemRef, UseDefaultIID());
		if (geo == nil)
			continue;
		PMRect bbox = geo->GetStrokeBoundingBox();
		PMPoint topLeft = bbox.LeftTop();
		PMPoint botRight = bbox.RightBottom();
		::TransformInnerPointToPasteboard(geo, &topLeft);
		::TransformInnerPointToPasteboard(geo, &botRight);
		// PMRect has no Contains() — inline the bounds check. Pasteboard
		// coords share the document's Y-down orientation, so topLeft has
		// the smaller Y and botRight the larger.
		if (pt.X() >= topLeft.X() && pt.X() <= botRight.X() &&
		    pt.Y() >= topLeft.Y() && pt.Y() <= botRight.Y())
			return itemRef;
	}
	return UIDRef::gNull;
}


// End, XPGUIImageInLayoutDropTarget.cpp.
