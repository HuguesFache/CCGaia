// XPGUIChoosePhotoCartonDialogController.cpp
//
// Controller for the modal "Choose photo carton" dialog opened by the
// layout-level image drop target (XPGUIImageInLayoutDropTarget) when an
// image from the Photos liées palette is dropped on empty pasteboard.
//
// On OK we:
//   1. Look up the user-selected (classeur, carton) pair via IFormeDataModel
//   2. Place the carton at the captured drop point with IXPageUtils::ImportForme
//   3. Walk the placed carton's items, picking out every IFormeData::kPhoto
//      frame, and pump the dragged image into each via the shared helper
//      XPageUIUtils::ImportPhotoIntoFrame (same code path as on-frame drop —
//      includes article tagging + credit/legend import).

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IActiveContext.h"
#include "IControlView.h"
#include "IPanelControlData.h"
#include "IDropDownListController.h"
#include "IStringListControlData.h"
#include "IStringData.h"
#include "ISysFileData.h"
#include "IPMPointData.h"
#include "IUIDData.h"
#include "IDocument.h"
#include "IXPageMgrAccessor.h"
#include "IFormeDataModel.h"
#include "IFormeData.h"
#include "IXPageUtils.h"
#include "IHierarchy.h"

// General includes:
#include "CDialogController.h"
#include "CAlert.h"
#include "FileUtils.h"
#include "Utils.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "XPageUIUtils.h"


class XPGUIChoosePhotoCartonDialogController : public CDialogController
{
public:
	XPGUIChoosePhotoCartonDialogController(IPMUnknown* boss) : CDialogController(boss) {}
	virtual ~XPGUIChoosePhotoCartonDialogController() {}

	virtual void     InitializeDialogFields(IActiveContext* dlgContext);
	virtual WidgetID ValidateDialogFields(IActiveContext* myContext);
	virtual void     ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);
};

CREATE_PMINTERFACE(XPGUIChoosePhotoCartonDialogController, kXPGUIChoosePhotoCartonDialogControllerImpl)


void XPGUIChoosePhotoCartonDialogController::InitializeDialogFields(IActiveContext* /*dlgContext*/)
{
	// Dropdown population is handled in the observer's AutoAttach (first
	// call only) — same pattern as XPGUILinkArtDialog.
}


WidgetID XPGUIChoosePhotoCartonDialogController::ValidateDialogFields(IActiveContext* myContext)
{
	return CDialogController::ValidateDialogFields(myContext);
}


void XPGUIChoosePhotoCartonDialogController::ApplyDialogFields(IActiveContext* myContext, const WidgetID& /*widgetId*/)
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	do {
		// 1. Pull selected (classeur, carton) from the dropdowns.
		InterfacePtr<IDropDownListController> classeurCtrl(
			panelControlData->FindWidget(kXPGUIPhotoCartonClasseurListWidgetID), UseDefaultIID());
		InterfacePtr<IDropDownListController> cartonCtrl(
			panelControlData->FindWidget(kXPGUIPhotoCartonListWidgetID), UseDefaultIID());
		if (classeurCtrl == nil || cartonCtrl == nil)
			break;

		const int32 idxC = classeurCtrl->GetSelected();
		const int32 idxK = cartonCtrl->GetSelected();
		if (idxC == IDropDownListController::kNoSelection ||
			idxK == IDropDownListController::kNoSelection)
			break;

		InterfacePtr<IStringListControlData> classeurListData(classeurCtrl, UseDefaultIID());
		InterfacePtr<IStringListControlData> cartonListData(cartonCtrl, UseDefaultIID());
		const PMString classeur = classeurListData->GetString(idxC);
		const PMString carton   = cartonListData->GetString(idxK);

		// 2. Resolve the snippet + matching files via the model.
		InterfacePtr<IXPageMgrAccessor> pageMgrAccessor(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IFormeDataModel>   formeModel(pageMgrAccessor->QueryFormeDataModel());
		if (formeModel == nil)
			break;

		IDFile formeFile, matchingFile;
		PMString posX, posY;
		if (!formeModel->GetForme(carton, classeur, formeFile, matchingFile, posX, posY))
			break;
		if (!FileUtils::DoesFileExist(formeFile) || !FileUtils::DoesFileExist(matchingFile))
			break;

		// 3. Replay the drag payload captured by the drop target.
		InterfacePtr<ISysFileData> imageFileData(this, UseDefaultIID());
		const IDFile imageFile = imageFileData->GetSysFile();

		InterfacePtr<IStringData> creditDataPtr(this, IID_ICREDITPHOTO);
		InterfacePtr<IStringData> legendDataPtr(this, IID_ILEGENDPHOTO);
		InterfacePtr<IStringData> articleIdDataPtr(this, IID_IPHOTOARTICLEID);
		InterfacePtr<IStringData> cropDataPtr(this, IID_IPHOTOCROPDATA);
		const PMString creditText = (creditDataPtr != nil)    ? creditDataPtr->Get()    : PMString();
		const PMString legendText = (legendDataPtr != nil)    ? legendDataPtr->Get()    : PMString();
		const PMString articleId  = (articleIdDataPtr != nil) ? articleIdDataPtr->Get() : PMString();
		const PMString cropData   = (cropDataPtr != nil)      ? cropDataPtr->Get()      : PMString();

		InterfacePtr<IPMPointData> pointData(this, UseDefaultIID());
		const PMPoint dropPoint = pointData->GetPMPoint();

		InterfacePtr<IUIDData> spreadData(this, UseDefaultIID());
		const UIDRef targetSpread = spreadData->GetRef();

		IDocument* doc = myContext->GetContextDocument();
		if (doc == nil)
			break;
		IDataBase* db = ::GetDataBase(doc);
		if (db == nil)
			break;

		// 4. Place the carton at the drop point. ImportForme returns the
		// list of placed items via its out-param; we'll iterate that to
		// find every kPhoto frame.
		UIDList placedItems(db);
		PMString error = kNullString;
		if (Utils<IXPageUtils>()->ImportForme(::GetUIDRef(doc), formeFile, dropPoint,
			targetSpread, matchingFile, error, &placedItems, kTrue, kTrue) != kSuccess)
		{
			CAlert::InformationAlert(error);
			break;
		}

		// 5. Pour the dragged image into every photo frame of the placed
		// carton. Multiple kPhoto frames duplicate the same image — that
		// matches the spec ("import into all photo blocks").
		const int32 nbPlaced = placedItems.Length();
		for (int32 i = 0; i < nbPlaced; ++i) {
			UIDRef itemRef = placedItems.GetRef(i);
			InterfacePtr<IFormeData> fd(itemRef, UseDefaultIID());
			if (fd == nil)
				continue;
			if (fd->GetType() != IFormeData::kPhoto)
				continue;
			XPageUIUtils::ImportPhotoIntoFrame(itemRef, imageFile, creditText, legendText, articleId, cropData);
		}

	} while (kFalse);
}


// End, XPGUIChoosePhotoCartonDialogController.cpp.
