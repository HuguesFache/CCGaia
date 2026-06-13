
#include "VCPlugInHeaders.h"

// Interface includes:
#include "CIdleTask.h"
#include "IIntData.h"
#include "IStringData.h"
#include "ICoreFilename.h"
#include "IStringListControlData.h"
#include "IDropDownListController.h"
#include "IPanelControlData.h"
#include "IControlView.h"
#include "ITextControlData.h"
#include "IUIDData.h"
#include "IWebServices.h"
#include "IDocument.h"

// General includes:
#include "CAlert.h"
#include "CDialogController.h"
#include "SystemUtils.h"
#include "FileUtils.h"
#include "StreamUtil.h"
#include "DocUtils.h"
#include "WidgetID.h"
#include "PMString.h"
#include "Utils.h"
#include "ILayoutUIUtils.h"
#include "IActiveContext.h"

// Project includes:
#include "XRLUIID.h"
#include "XRLID.h"
#include "XRCID.h"

/** XPBCLoginDialogController
	Methods allow for the initialization, validation, and application of dialog widget values.

	Implements IDialogController based on the partial implementation CDialogController.

	@author GD
*/


class XRailChangeStateDialogController : public CDialogController
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRailChangeStateDialogController(IPMUnknown* boss);

	/**
		Initialize each widget in the dialog with its default value.
		Called when the dialog is opened.
	*/
	virtual void InitializeDialogFields(IActiveContext* dlgContext);

	/**
		Validate the values in the widgets.
		By default, the widget with ID kOKButtonWidgetID causes
		ValidateFields to be called. When all widgets are valid,
		ApplyFields will be called.
		@return kDefaultWidgetId if all widget values are valid, WidgetID of the widget to select otherwise.

	*/
	virtual WidgetID ValidateDialogFields(IActiveContext* myContext);

	/**
		Retrieve the values from the widgets and act on them.
		@param widgetId identifies the widget on which to act.
	*/
	virtual void ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);

	K2Vector<bool> listStatusCheckError;

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XRailChangeStateDialogController, kXRLUIChangeStateDialogControllerImpl)

/* Constructor
*/
XRailChangeStateDialogController::XRailChangeStateDialogController(IPMUnknown* boss) : CDialogController(boss)
{
}

/* ApplyFields
*/
void XRailChangeStateDialogController::InitializeDialogFields(IActiveContext* dlgContext)
{
	// Get userID to interact with XRail server
	InterfacePtr<IStringData> sessionID(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);
	PMString userID = sessionID->GetString();

	do {
		// Get concerned document and update status of its pages with the choice of the user
		InterfacePtr<IUIDData> theDoc(this, UseDefaultIID());

		PMString baseName, serverAdress;
		int32 idPage = GetPageID(theDoc->GetRef(), 0, baseName);
		serverAdress = GetGoodUrlXR(baseName);

		// Initialize status list, getting it from XRail server
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

		IControlView* controlView = panelControlData->FindWidget(kXRLUIStateListBoxWidgetID);

		InterfacePtr<IStringListControlData> stringListControlData(controlView, UseDefaultIID());
		stringListControlData->Clear(kFalse, kFalse);

		InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

		K2Vector<PMString> listStatus;

		bool connexionOK = xrailHTTP->GetListeStatus_v2(serverAdress, userID, listStatus, listStatusCheckError);
		if (!connexionOK)
		{
			CAlert::ErrorAlert(kXRLUIErrorWSGetStatusKey);
			break;
		}

		// Get page status
		InterfacePtr<IStringData> pageStatus(this, UseDefaultIID());

		int32 selectedIndex = IDropDownListController::kBeginning;
		for (int32 i = 0; i < listStatus.size(); ++i)
		{
			PMString status = listStatus[i];

			if (status.Compare(kFalse, pageStatus->Get()) == 0)
				selectedIndex = i;

			stringListControlData->AddString(status,
				IStringListControlData::kEnd,
				kFalse, 	// Invalidate?					
				kFalse);	// Notify of change?
		}

		InterfacePtr<IDropDownListController> dropDownListController(controlView, UseDefaultIID());

		dropDownListController->Select(selectedIndex,	// ordinal place.
			kFalse,									// Invalidate?
			kFalse									// Notify of change?
		);

	} while (false);
}

/* ValidateFields
*/
WidgetID XRailChangeStateDialogController::ValidateDialogFields(IActiveContext* dlgContext)
{
	WidgetID 		badWidgetID = kNoInvalidWidgets;

	do {
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

		IControlView* controlView = panelControlData->FindWidget(kXRLUIStateListBoxWidgetID);
		InterfacePtr<IDropDownListController> dropDownListController(controlView, UseDefaultIID());
		int32 selectedIndex = dropDownListController->GetSelected();
		InterfacePtr<IBoolData> errorInDoc(this, UseDefaultIID());
		if ((listStatusCheckError[selectedIndex] == true) && (errorInDoc->Get() == kTrue)) {
			CAlert::InformationAlert("Vous ne pouvez pas passer la page dans cet état car elle contient des erreurs");
			badWidgetID = kXRLUIStateListBoxWidgetID;
			break;
		}
	} while (false);
	return badWidgetID;
}

/* ApplyFields
*/
void XRailChangeStateDialogController::ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId)
{
	// Get userID to interact with XRail server
	InterfacePtr<IStringData> sessionID(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);
	PMString userID = sessionID->GetString();

	// Get webservice client boss and set server address for it
	InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

	do {
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

		IControlView* controlView = panelControlData->FindWidget(kXRLUIStateListBoxWidgetID);

		// Get user choice
		InterfacePtr<IDropDownListController> dropDownListController(controlView, UseDefaultIID());
		int32 selectedIndex = dropDownListController->GetSelected();

		InterfacePtr<IStringListControlData> stringListControlData(controlView, UseDefaultIID());
		PMString newStatus = stringListControlData->GetString(selectedIndex);

		// Get concerned document and update status of its pages with the choice of the user
		InterfacePtr<IUIDData> theDoc(this, UseDefaultIID());

		int16 nbPages = GetNbPages(theDoc->GetRef());

		bool16 connexionOK = kTrue;
		int32 idPage = 0;
		PMString baseName, serverAddress;
		K2Vector<int32> listPagesIDS;

		for (int32 i = 0; i < nbPages; ++i) {
			idPage = GetPageID(theDoc->GetRef(), i, baseName);
			if (idPage > 0) {
				//on remplit une liste avec tous les ids de chaque page
				listPagesIDS.push_back(idPage);

			}
		}
		serverAddress = GetGoodUrlXR(baseName);

		if (listPagesIDS.size() > 0) {
			bool connexionOK = xrailHTTP->SetStatus_v2(serverAddress, listPagesIDS, newStatus, userID);
			if (!connexionOK) {
				CAlert::ErrorAlert(PMString(kXRLUIErrorWSSetPageStatusKey));
			}

		}

	} while (false);

}

// End, XPBCLoginDialogController.cpp.





