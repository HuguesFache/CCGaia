/*
//	File:	PMUDialogController.cpp
//
//	Author:	Trias
//
//	Date:	14/10/2005
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IPageMakeUpPrefs.h"
#include "IControlView.h"
#include "ICommand.h"
//#include "IChangeLocaleCmdData.h"
#include "IPanelControlData.h"
#include "ILayoutUIUtils.h"
#include "IApplication.h"
#include "IDocumentList.h"

// General includes:
#include "CDialogController.h"
#include "SystemUtils.h"
#include "LocaleSetting.h"
#include "CAlert.h"
#include "CMdUtils.h"
#include "UIDList.h"
#include "WorkspaceID.h"

// Project includes:
#include "PMUID.h"

/** 
	Preferences dialog controller
*/
class PMUDialogController : public CDialogController
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PMUDialogController(IPMUnknown* boss) : CDialogController(boss) {}

		/**
			Initializes each widget in the dialog with its default value.
			Called when the dialog is opened.
			@param dlgContext
		*/
		virtual void InitializeDialogFields( IActiveContext* dlgContext);

		/**
			Validate the values in the widgets. 
			By default, the widget with ID kOKButtonWidgetID causes 
			this method to be called. When all widgets are valid, 
			ApplyFields will be called.		
			@param myContext
			@return kDefaultWidgetId if all widget values are valid, WidgetID of the widget to select otherwise.

		*/
		virtual WidgetID ValidateDialogFields( IActiveContext* myContext);

		/**
			Retrieve the values from the widgets and act on them.
			@param myContext
			@param widgetId identifies the widget on which to act.
		*/
		virtual void ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId);

		/**
			Reset all fields to default values
			@param myContext
		*/
		virtual void  ResetDialogFields (IActiveContext * myContext);

	private :

		WidgetID currentLanguage;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its 
 ImplementationID making the C++ code callable by the 
 application.
*/
CREATE_PMINTERFACE(PMUDialogController, kPMUDialogControllerImpl)

/* ApplyFields
*/
void PMUDialogController::InitializeDialogFields( IActiveContext* dlgContext) 
{
	do
	{
		InterfacePtr<IPageMakeUpPrefs> storedPrefs ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (storedPrefs == nil)
		{
			ASSERT_FAIL("PMUDocPathDialogController::InitializeDialogFields() storedPrefs invalid");
			break;
		}

		SetTriStateControlData(kPMUStopOnErrorWidgetID,storedPrefs->getStopOnError());
#if PERMREFS
		SetTriStateControlData(kPMUDisplayPermRefWidgetID,storedPrefs->getDisplayPermRef());
#endif

#if !COMPOSER
		if(storedPrefs->getExportType() == PDF_EXPORT)
			SetSelectedClusterWidget(kPMUExportTypeClusterWidgetID,kPMUPDFExportWidgetID);
		else
			SetSelectedClusterWidget(kPMUExportTypeClusterWidgetID,kPMUEPSExportWidgetID);

		SetTriStateControlData(kPMUPrintTCLRefWidgetID,storedPrefs->getPrintTCLRef());
#endif

	} while(kFalse);
}

/* ValidateFields
*/
WidgetID PMUDialogController::ValidateDialogFields( IActiveContext* myContext) 
{
	WidgetID result = kNoInvalidWidgets;

	// Put code to validate widget values here.

	return result;
}

/* ResetDialogFields
*/
void PMUDialogController::ResetDialogFields( IActiveContext* myContext) 
{
	SetTriStateControlData(kPMUStopOnErrorWidgetID,ITriStateControlData::kSelected);
#if PERMREFS
	SetTriStateControlData(kPMUDisplayPermRefWidgetID,ITriStateControlData::kUnselected);
#endif

#if !COMPOSER
	SetSelectedClusterWidget(kPMUExportTypeClusterWidgetID,kPMUPDFExportWidgetID);
	SetTriStateControlData(kPMUPrintTCLRefWidgetID,ITriStateControlData::kUnselected);
#endif
}

/* ApplyFields
*/
void PMUDialogController::ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId) 
{
	ErrorCode status = kFailure;
	do
	{
		InterfacePtr<IPageMakeUpPrefs> currentPreferences ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (currentPreferences == nil)
		{
			ASSERT_FAIL("PMUDialogController::InitializeDialogFields() storedPrefs invalid");
			break;
		}

		// Update the preferences with a command
		UIDList workspace(::GetUIDRef(currentPreferences));
		InterfacePtr<ICommand> setPrefsCmd (CmdUtils::CreateCommand(kPrsSetPreferencesCmdBoss));
		if(setPrefsCmd == nil)
		{		
			ASSERT_FAIL("PMUDialogController::ApplyDialogFields -> setPrefsCmd nil");
			break;
		}

		setPrefsCmd->SetItemList(workspace);

		InterfacePtr<IPageMakeUpPrefs> cmdData (setPrefsCmd, UseDefaultIID());
		if(cmdData == nil)
		{		
			ASSERT_FAIL("PMUDialogController::ApplyDialogFields -> cmdData nil");
			break;
		}

		cmdData->CopyFrom(currentPreferences);

		// Get new preferences from widget values
		bool16 stopOnError = (GetTriStateControlData(kPMUStopOnErrorWidgetID) == ITriStateControlData::kSelected);
#if PERMREFS
		bool16 displayPermRef = (GetTriStateControlData(kPMUDisplayPermRefWidgetID) == ITriStateControlData::kSelected);
		cmdData->setDisplayPermRef(displayPermRef);
#endif

#if !COMPOSER
		int16 exportType;
		if(GetSelectedClusterWidget(kPMUExportTypeClusterWidgetID) == kPMUPDFExportWidgetID)
			exportType = PDF_EXPORT;
		else
			exportType = EPS_EXPORT;
		cmdData->setExportType(exportType);

		bool16 print = (GetTriStateControlData(kPMUPrintTCLRefWidgetID) == ITriStateControlData::kSelected);
		cmdData->setPrintTCLRef(print);
#endif

		cmdData->setStopOnError(stopOnError);

		if(CmdUtils::ProcessCommand(setPrefsCmd) != kSuccess)
			break;

		// Update views of all opened documents
		InterfacePtr<IApplication> app (GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> docList (app->QueryDocumentList());

		int32 nbDocs = docList->GetDocCount();
		for(int32 i = 0 ; i < nbDocs ; ++i)
			Utils<ILayoutUIUtils>()->InvalidateViews(docList->GetNthDoc(i)); 

		//// Change locale, if necessary ==> DOESN'T WORK !!!
		//WidgetID selectedLanguage = GetSelectedClusterWidget(kPMUClusterLangWidgetID);
		//if(selectedLanguage != currentLanguage)
		//{
		//	currentLanguage = selectedLanguage;
		//	InterfacePtr<ICommand> changeLocaleCmd (CmdUtils::CreateCommand(kChangeLocaleCmdBoss));
		//	if(changeLocaleCmd == nil)
		//	{		
		//		ASSERT_FAIL("PMUDialogController::ApplyDialogFields -> changeLocaleCmd nil");
		//		break;
		//	}

		//	InterfacePtr<IChangeLocaleCmdData> cmdData (this, IID_ICHANGELOCALECMDDATA);
		//	if(cmdData == nil)
		//		break;

		//	if(selectedLanguage == kPMUEnglishRadioWidgetID)
		//		cmdData->Init(PMLocaleId(LocaleSetting::GetLocale().GetLanguageFS(),k_enUS));
		//	else
		//		cmdData->Init(PMLocaleId(LocaleSetting::GetLocale().GetLanguageFS(),k_frFR));

		//	if(CmdUtils::ProcessCommand(changeLocaleCmd) != kSuccess)
		//		break;
		//	
		//}

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(kPMUSavePrefsPb);
}




