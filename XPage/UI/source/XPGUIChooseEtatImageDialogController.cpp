// XPGUIChooseEtatImageDialogController.cpp
//
// Controller for the modal "Changer l'état de l'image" dialog opened from the
// Photos liées palette when the user clicks a row's colour swatch
// (kXPGUIImageEtatSwatchEHImpl).
//
// Pure picker: a single dropdown lists the etatsimages cached on
// IXPGPreferences. The current état id is passed in via IID_IXPGETATCHOICE
// (IIntData) so we can preselect it; on OK we write the chosen état id back to
// the same interface. The caller (the swatch event handler) compares old vs new
// and performs the Plugin_SetImageState web service call + row refresh.

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IActiveContext.h"
#include "IControlView.h"
#include "IPanelControlData.h"
#include "IDropDownListController.h"
#include "IStringListControlData.h"
#include "IIntData.h"
#include "IXPGPreferences.h"

// General includes:
#include "CDialogController.h"
#include "K2Vector.tpp"

// Project includes:
#include "XPGUIID.h"
#include "XPageUIUtils.h"


class XPGUIChooseEtatImageDialogController : public CDialogController
{
public:
	XPGUIChooseEtatImageDialogController(IPMUnknown* boss) : CDialogController(boss) {}
	virtual ~XPGUIChooseEtatImageDialogController() {}

	virtual void     InitializeDialogFields(IActiveContext* dlgContext);
	virtual void     ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);
};

CREATE_PMINTERFACE(XPGUIChooseEtatImageDialogController, kXPGUIChooseEtatImageDialogControllerImpl)


void XPGUIChooseEtatImageDialogController::InitializeDialogFields(IActiveContext* /*dlgContext*/)
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	// Current état id passed in by the caller (0 = none yet).
	int32 currentEtat = 0;
	InterfacePtr<IIntData> choiceData(this, IID_IXPGETATCHOICE);
	if (choiceData != nil)
		currentEtat = choiceData->Get();

	InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
	if (xpgPrefs == nil)
		return;

	EtatImageList etats = xpgPrefs->GetEtatsImages();

	K2Vector<PMString> names;
	PMString currentName = kNullString;
	for (int32 i = 0; i < etats.size(); ++i)
	{
		names.push_back(etats[i].Nom);
		if (etats[i].ID == currentEtat)
			currentName = etats[i].Nom;
	}

	// Fill + preselect the current état (no-op selection when currentName is
	// empty, e.g. the photo has no state yet).
	XPageUIUtils::FillDropdownlistAndSelect(panelControlData,
		kXPGUIChooseEtatImageListWidgetID, names, currentName, kFalse);
}


void XPGUIChooseEtatImageDialogController::ApplyDialogFields(IActiveContext* /*myContext*/, const WidgetID& /*widgetId*/)
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	InterfacePtr<IDropDownListController> listCtrl(
		panelControlData->FindWidget(kXPGUIChooseEtatImageListWidgetID), UseDefaultIID());
	if (listCtrl == nil)
		return;

	const int32 selected = listCtrl->GetSelected();
	if (selected == IDropDownListController::kNoSelection)
		return;

	// The dropdown was filled in etatsimages order, so the selected index maps
	// straight back to that list. Re-fetch it (stable, cached on prefs).
	InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
	if (xpgPrefs == nil)
		return;
	EtatImageList etats = xpgPrefs->GetEtatsImages();
	if (selected < 0 || selected >= etats.size())
		return;

	const int32 chosenEtat = etats[selected].ID;

	InterfacePtr<IIntData> choiceData(this, IID_IXPGETATCHOICE);
	if (choiceData != nil)
		choiceData->Set(chosenEtat);
}


// End, XPGUIChooseEtatImageDialogController.cpp.
