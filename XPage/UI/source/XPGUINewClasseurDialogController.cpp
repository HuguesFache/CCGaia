
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IActiveContext.h"
#include "IApplication.h"
#include "IControlView.h"
#include "IPanelControlData.h"
#include "IPanelMgr.h"
#include "IStringData.h"
#include "IBoolData.h"
#include "ITextControlData.h"
#include "IXPageMgrAccessor.h"
#include "IFormeDataModel.h"
#include "IAssemblageDataModel.h"

// General includes:
#include "CDialogController.h"
#include "CAlert.h"
#include "Utils.h"
#include "ISubject.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "XPageUIUtils.h"

/**
	Simple dialog controller that prompts the user for a classeur name
	and creates it in the Forme filesystem model.
*/
class XPGUINewClasseurDialogController : public CDialogController
{
public:

	XPGUINewClasseurDialogController(IPMUnknown* boss) : CDialogController(boss) {}

	virtual void InitializeDialogFields(IActiveContext* dlgContext);

	virtual WidgetID ValidateDialogFields(IActiveContext* myContext);

	virtual void ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);
};

CREATE_PMINTERFACE(XPGUINewClasseurDialogController, kXPGUINewClasseurDialogControllerImpl)

/* InitializeDialogFields
*/
void XPGUINewClasseurDialogController::InitializeDialogFields(IActiveContext* /*dlgContext*/)
{
	// Clear any previously entered name.
	SetTextControlData(kXPGUINewClasseurNameWidgetID, kNullString);
}

/* ValidateDialogFields
*/
WidgetID XPGUINewClasseurDialogController::ValidateDialogFields(IActiveContext* /*myContext*/)
{
	PMString classeurName = GetTextControlData(kXPGUINewClasseurNameWidgetID);
	classeurName.StripWhiteSpace(PMString::kLeadingAndTrailingWhiteSpace);

	if (classeurName == kNullString)
	{
		CAlert::InformationAlert(PMString(kXPGUIEmptyClasseurNameKey));
		return kXPGUINewClasseurNameWidgetID;
	}

	if (!XPageUIUtils::IsValidName(classeurName))
	{
		CAlert::InformationAlert(PMString(kXPGUIInvalidFormeNameKey));
		return kXPGUINewClasseurNameWidgetID;
	}

	// Branch on the assemblage flag set by the caller (XPageUIUtils::
	// DisplayNewClasseurDialog) so existence is checked against the right
	// model: Forme tree for XDA, Assemblage tree for the Assemblages palette.
	InterfacePtr<IBoolData> assemblageFlag(this, IID_IBOOLDATA);
	const bool16 isAssemblage = (assemblageFlag != nil && assemblageFlag->Get());

	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor(GetExecutionContextSession(), UseDefaultIID());
	bool16 alreadyExists = kFalse;
	if (isAssemblage)
	{
		InterfacePtr<IAssemblageDataModel> model(pageMrgAccessor->QueryAssemblageDataModel());
		alreadyExists = (model && model->DoesClasseurExist(classeurName));
	}
	else
	{
		InterfacePtr<IFormeDataModel> model(pageMrgAccessor->QueryFormeDataModel());
		alreadyExists = (model && model->DoesClasseurExist(classeurName));
	}

	if (alreadyExists)
	{
		CAlert::InformationAlert(PMString(kXPGUIClasseurAlreadyExistsKey));
		return kXPGUINewClasseurNameWidgetID;
	}

	return kNoInvalidWidgets;
}

/* ApplyDialogFields
*/
void XPGUINewClasseurDialogController::ApplyDialogFields(IActiveContext* /*myContext*/, const WidgetID& /*widgetId*/)
{
	PMString classeurName = GetTextControlData(kXPGUINewClasseurNameWidgetID);
	classeurName.StripWhiteSpace(PMString::kLeadingAndTrailingWhiteSpace);

	// Remember the chosen name on the dialog boss so the caller can read it back.
	InterfacePtr<IStringData> classeurData(this, IID_ICLASSEURNAMEDATA);
	if (classeurData)
		classeurData->Set(classeurName);

	InterfacePtr<IBoolData> assemblageFlag(this, IID_IBOOLDATA);
	const bool16 isAssemblage = (assemblageFlag != nil && assemblageFlag->Get());

	// Create in the matching model.
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor(GetExecutionContextSession(), UseDefaultIID());
	ErrorCode status = kFailure;
	if (isAssemblage)
	{
		InterfacePtr<IAssemblageDataModel> model(pageMrgAccessor->QueryAssemblageDataModel());
		if (model == nil)
			return;
		status = model->CreateClasseur(classeurName);
	}
	else
	{
		InterfacePtr<IFormeDataModel> model(pageMrgAccessor->QueryFormeDataModel());
		if (model == nil)
			return;
		status = model->CreateClasseur(classeurName);
	}
	if (status != kSuccess)
		return;

	// Auto-select the new classeur in the matching palette. The panel observer
	// handler for the broadcast below reads the current text from the classeur
	// combo widget and uses it as the initial selection when repopulating the
	// dropdown — so we write the new name there first, then broadcast.
	const WidgetID targetPanelID =
		isAssemblage ? kXPGUIAssemblagesPanelWidgetID : kXPGUIFormesPanelWidgetID;
	do
	{
		InterfacePtr<IApplication> app(GetExecutionContextSession()->QueryApplication());
		if (app == nil)
			break;

		InterfacePtr<IPanelMgr> panelMgr(app->QueryPanelManager());
		if (panelMgr == nil)
			break;

		IControlView* panelView = panelMgr->GetVisiblePanel(targetPanelID);
		if (panelView == nil)
			break;

		InterfacePtr<IPanelControlData> panelData(panelView, UseDefaultIID());
		if (panelData == nil)
			break;

		InterfacePtr<ITextControlData> classeurTxt(
			panelData->FindWidget(kXPGUIComboClasseurListWidgetID), UseDefaultIID());
		if (classeurTxt)
			classeurTxt->SetString(classeurName);
	} while (kFalse);

	// Notify the matching palette so it refreshes its classeur list.
	InterfacePtr<ISubject> sessionSubject(GetExecutionContextSession(), UseDefaultIID());
	if (sessionSubject)
	{
		const ClassID msg = isAssemblage ? kXPGNewClasseurAssemblageMsg : kXPGNewClasseurMsg;
		sessionSubject->Change(msg, IID_IREFRESHPROTOCOL);
	}
}
