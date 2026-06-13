/*
//	File:	PMUDocPathDialogController.cpp
//
//	Author:	Trias
//
//	Date:	17/10/2005
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IPageMakeUpPrefs.h"
#include "IControlView.h"
#include "ICommand.h"

// General includes:
#include "CDialogController.h"
#include "SystemUtils.h"
#include "CAlert.h"
#include "CMdUtils.h"
#include "UIDList.h"

// Project includes:
#include "PMUID.h"

/** 
	Preferences dialog controller
*/
class PMUDocPathDialogController : public CDialogController
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PMUDocPathDialogController(IPMUnknown* boss) : CDialogController(boss) {}

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
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its 
 ImplementationID making the C++ code callable by the 
 application.
*/
CREATE_PMINTERFACE(PMUDocPathDialogController, kPMUDocPathDialogControllerImpl)

/* ApplyFields
*/
void PMUDocPathDialogController::InitializeDialogFields( IActiveContext* dlgContext) 
{
	do
	{
		InterfacePtr<IPageMakeUpPrefs> storedPrefs ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (storedPrefs == nil)
		{
			ASSERT_FAIL("PMUDocPathDialogController::InitializeDialogFields() storedPrefs invalid");
			break;
		}

		// Get preferences
		bool16 useSavePath, useOpenPath;
		PMString savePath, openPath;
		
		storedPrefs->getSavePrefs(&useSavePath, &savePath);
		storedPrefs->getOpenPrefs(&useOpenPath, &openPath);

		// Set controls datas
		SetTextControlData(kPMUSavePathWidgetID, savePath);
		SetTextControlData(kPMUOpenPathWidgetID, openPath);

		SetTriStateControlData(kPMUUseSavePathWidgetID,useSavePath);
		SetTriStateControlData(kPMUUseOpenPathWidgetID,useOpenPath);

	} while(kFalse);
}

/* ValidateFields
*/
WidgetID PMUDocPathDialogController::ValidateDialogFields( IActiveContext* myContext) 
{
	WidgetID result = kNoInvalidWidgets;

	// Put code to validate widget values here.

	return result;
}

/* ResetDialogFields
*/
void PMUDocPathDialogController::ResetDialogFields( IActiveContext* myContext) 
{
	SetTextControlData(kPMUSavePathWidgetID, PMString(""));
	SetTextControlData(kPMUOpenPathWidgetID, PMString(""));

	SetTriStateControlData(kPMUUseSavePathWidgetID,ITriStateControlData::kUnselected);
	SetTriStateControlData(kPMUUseOpenPathWidgetID,ITriStateControlData::kUnselected);
}

/* ApplyFields
*/
void PMUDocPathDialogController::ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId) 
{
	ErrorCode status = kFailure;
	do
	{
		InterfacePtr<IPageMakeUpPrefs> currentPreferences ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (currentPreferences == nil)
		{
			ASSERT_FAIL("PMUDocPathDialogController::InitializeDialogFields() storedPrefs invalid");
			break;
		}

		// Update the preferences with a command
		UIDList workspace(::GetUIDRef(currentPreferences));
		InterfacePtr<ICommand> setPrefsCmd (CmdUtils::CreateCommand(kPrsSetPreferencesCmdBoss));
		if(setPrefsCmd == nil)
		{		
			ASSERT_FAIL("PMUDocPathDialogController::ApplyDialogFields -> setPrefsCmd nil");
			break;
		}

		setPrefsCmd->SetItemList(workspace);

		InterfacePtr<IPageMakeUpPrefs> cmdData (setPrefsCmd, UseDefaultIID());
		if(cmdData == nil)
		{		
			ASSERT_FAIL("PMUDocPathDialogController::ApplyDialogFields -> cmdData nil");
			break;
		}

		cmdData->CopyFrom(currentPreferences);

		// Get new preferences from widget values
		bool16 useSavePath = (GetTriStateControlData(kPMUUseSavePathWidgetID) == ITriStateControlData::kSelected);
		bool16 useOpenPath = (GetTriStateControlData(kPMUUseOpenPathWidgetID) == ITriStateControlData::kSelected);

		// Get new preferences from widget values
		cmdData->setSavePrefs(useSavePath, GetTextControlData(kPMUSavePathWidgetID));
		cmdData->setOpenPrefs(useOpenPath, GetTextControlData(kPMUOpenPathWidgetID));

		status = CmdUtils::ProcessCommand(setPrefsCmd);

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(kPMUSavePrefsPb);
}




