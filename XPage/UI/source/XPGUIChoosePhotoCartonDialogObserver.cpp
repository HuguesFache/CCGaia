// XPGUIChoosePhotoCartonDialogObserver.cpp
//
// Dialog observer for XPGUIChoosePhotoCartonDialog. Drives the cascading
// dropdowns:
//   - Classeur: only those that contain at least one carton tagged
//     isPhotoCarton="1" in its descriptif XML.
//   - Carton:   only those photo cartons within the selected classeur.
//
// Both filters use IFormeDataModel::IsPhotoCarton (added with the
// "Carton photo" feature, see XPGFileSystemFormeModel.cpp).
//
// If no photo carton exists anywhere, the dialog is still shown (so the
// user gets feedback that nothing happened), but the dropdowns stay empty
// — XPageUIUtils::DisplayChoosePhotoCartonDialog also surfaces an alert in
// that case.

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IControlView.h"
#include "ISubject.h"
#include "IPanelControlData.h"
#include "IDropDownListController.h"
#include "IStringListControlData.h"
#include "IXPageMgrAccessor.h"
#include "IFormeDataModel.h"

// General includes:
#include "CDialogObserver.h"

// Project includes:
#include "XPageUIUtils.h"
#include "XPGUIID.h"


class XPGUIChoosePhotoCartonDialogObserver : public CDialogObserver
{
public:
	XPGUIChoosePhotoCartonDialogObserver(IPMUnknown* boss);
	virtual ~XPGUIChoosePhotoCartonDialogObserver() {}

	void AutoAttach();
	void AutoDetach();
	void Update(const ClassID& theChange, ISubject* theSubject,
		const PMIID& protocol, void* changedBy);

private:
	void AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData,
		const WidgetID& widgetID, const PMIID& interfaceID);
	void DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData,
		const WidgetID& widgetID, const PMIID& interfaceID);

	void FillClasseurList();
	void FillCartonList();

	bool8 firstInit;
};

CREATE_PMINTERFACE(XPGUIChoosePhotoCartonDialogObserver, kXPGUIChoosePhotoCartonDialogObserverImpl)


XPGUIChoosePhotoCartonDialogObserver::XPGUIChoosePhotoCartonDialogObserver(IPMUnknown* boss)
	: CDialogObserver(boss), firstInit(kTrue)
{
}


void XPGUIChoosePhotoCartonDialogObserver::AutoAttach()
{
	CDialogObserver::AutoAttach();

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	this->AttachWidget(panelControlData, kXPGUIPhotoCartonClasseurListWidgetID, IID_ISTRINGLISTCONTROLDATA);
	this->AttachWidget(panelControlData, kXPGUIPhotoCartonListWidgetID,         IID_ISTRINGLISTCONTROLDATA);

	if (firstInit) {
		FillClasseurList();
		firstInit = kFalse;
	}
}


void XPGUIChoosePhotoCartonDialogObserver::AutoDetach()
{
	CDialogObserver::AutoDetach();

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	this->DetachWidget(panelControlData, kXPGUIPhotoCartonClasseurListWidgetID, IID_ISTRINGLISTCONTROLDATA);
	this->DetachWidget(panelControlData, kXPGUIPhotoCartonListWidgetID,         IID_ISTRINGLISTCONTROLDATA);
}


void XPGUIChoosePhotoCartonDialogObserver::Update(const ClassID& theChange,
	ISubject* theSubject, const PMIID& protocol, void* changedBy)
{
	CDialogObserver::Update(theChange, theSubject, protocol, changedBy);

	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());

	if ((protocol == IID_ISTRINGLISTCONTROLDATA) && (theChange == kPopupChangeStateMessage)) {
		if (controlView != nil) {
			const WidgetID widgetID = controlView->GetWidgetID();
			if (widgetID == kXPGUIPhotoCartonClasseurListWidgetID)
				FillCartonList();
		}
	}
}


void XPGUIChoosePhotoCartonDialogObserver::AttachWidget(
	const InterfacePtr<IPanelControlData>& panelControlData,
	const WidgetID& widgetID, const PMIID& interfaceID)
{
	IControlView* controlView = panelControlData->FindWidget(widgetID);
	if (controlView == nil)
		return;
	InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
	if (subject != nil)
		subject->AttachObserver(this, interfaceID);
}


void XPGUIChoosePhotoCartonDialogObserver::DetachWidget(
	const InterfacePtr<IPanelControlData>& panelControlData,
	const WidgetID& widgetID, const PMIID& interfaceID)
{
	IControlView* controlView = panelControlData->FindWidget(widgetID);
	if (controlView == nil)
		return;
	InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
	if (subject != nil)
		subject->DetachObserver(this, interfaceID);
}


/*
 * FillClasseurList
 *
 * Lists every classeur that contains at least one photo carton. Walks each
 * classeur's forme list once and asks IsPhotoCarton per forme — bails out
 * of the inner loop on the first match.
 */
void XPGUIChoosePhotoCartonDialogObserver::FillClasseurList()
{
	InterfacePtr<IXPageMgrAccessor> pageMgrAccessor(GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IFormeDataModel>   model(pageMgrAccessor->QueryFormeDataModel());
	if (model == nil)
		return;

	K2Vector<PMString> allClasseurs = model->GetClasseurList();
	K2Vector<PMString> photoClasseurs;
	for (int32 i = 0; i < allClasseurs.size(); ++i) {
		const PMString& cl = allClasseurs[i];
		K2Vector<PMString> formes = model->GetFormeList(cl);
		for (int32 j = 0; j < formes.size(); ++j) {
			if (model->IsPhotoCarton(formes[j], cl)) {
				photoClasseurs.push_back(cl);
				break; // one match is enough — keep this classeur.
			}
		}
	}

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	XPageUIUtils::FillDropdownlist(panelControlData,
		kXPGUIPhotoCartonClasseurListWidgetID, photoClasseurs, kFalse);

	FillCartonList();
}


/*
 * FillCartonList
 *
 * Lists the photo cartons of the currently-selected classeur.
 */
void XPGUIChoosePhotoCartonDialogObserver::FillCartonList()
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

	InterfacePtr<IDropDownListController> classeurCtrl(
		panelControlData->FindWidget(kXPGUIPhotoCartonClasseurListWidgetID), UseDefaultIID());
	const int32 selectedIndex = classeurCtrl->GetSelected();

	K2Vector<PMString> photoCartons;
	if (selectedIndex != IDropDownListController::kNoSelection) {
		InterfacePtr<IStringListControlData> classeurListData(classeurCtrl, UseDefaultIID());
		const PMString selectedClasseur = classeurListData->GetString(selectedIndex);

		InterfacePtr<IXPageMgrAccessor> pageMgrAccessor(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IFormeDataModel>   model(pageMgrAccessor->QueryFormeDataModel());
		if (model != nil) {
			K2Vector<PMString> formes = model->GetFormeList(selectedClasseur);
			for (int32 i = 0; i < formes.size(); ++i) {
				if (model->IsPhotoCarton(formes[i], selectedClasseur))
					photoCartons.push_back(formes[i]);
			}
		}
	}

	XPageUIUtils::FillDropdownlist(panelControlData,
		kXPGUIPhotoCartonListWidgetID, photoCartons, kFalse);
}


// End, XPGUIChoosePhotoCartonDialogObserver.cpp.
