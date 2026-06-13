/*
//	File:	PMUExportPrefsDialogController.cpp
//
//	Author:	Trias
//
//	Date:	18/10/2005
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IPanelControlData.h"
#include "IControlView.h"
#include "IPageMakeUpPrefs.h"
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
class PMUExportPrefsDialogController : public CDialogController
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PMUExportPrefsDialogController(IPMUnknown* boss) : CDialogController(boss) {}

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
CREATE_PMINTERFACE(PMUExportPrefsDialogController, kPMUExportPrefsDialogControllerImpl)

/* ApplyFields
*/
void PMUExportPrefsDialogController::InitializeDialogFields( IActiveContext* dlgContext) 
{
	do
	{
		InterfacePtr<IPageMakeUpPrefs> storedPrefs ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (storedPrefs == nil)
		{
			ASSERT_FAIL("PMUExportPrefsDialogController::InitializeDialogFields() storedPrefs invalid");
			break;
		}

		// Get preferences
		bool16 exportStyles, exportCJ, exportColors, exportLayers, exportMasters;
		bool16 exportTextFrame, exportImageFrame, exportRule;
		bool16 exportText, exportParaCmd, exportTextCmd, exportStyleCmd;

		storedPrefs->getExportHeaderPrefs(&exportStyles, &exportCJ, &exportColors,&exportLayers, &exportMasters);
		storedPrefs->getExportGeometryPrefs(&exportTextFrame, &exportImageFrame, &exportRule);
		storedPrefs->getExportTextPrefs(&exportText, &exportParaCmd, &exportTextCmd, &exportStyleCmd);

		SetTriStateControlData(kPMUExportStyleWidgetID,exportStyles);
		SetTriStateControlData(kPMUExportCJWidgetID,exportCJ);
		SetTriStateControlData(kPMUExportColorsWidgetID,exportColors);
		SetTriStateControlData(kPMUExportLayersWidgetID,exportLayers);
		SetTriStateControlData(kPMUExportMastersWidgetID,exportMasters);

		SetTriStateControlData(kPMUExportTextFrameWidgetID,exportTextFrame);
		SetTriStateControlData(kPMUExportImageFrameWidgetID,exportImageFrame);
		SetTriStateControlData(kPMUExportRuleWidgetID,exportRule);

		SetTriStateControlData(kPMUExportTextWidgetID,exportText);
		SetTriStateControlData(kPMUExportParaCmdWidgetID,exportParaCmd);
		SetTriStateControlData(kPMUExportTextCmdWidgetID,exportTextCmd);
		SetTriStateControlData(kPMUExportStyleCmdWidgetID,exportStyleCmd);
		
	} while (false);
}

/* ValidateFields
*/
WidgetID PMUExportPrefsDialogController::ValidateDialogFields( IActiveContext* myContext) 
{
	WidgetID result = kNoInvalidWidgets;

	// Put code to validate widget values here.

	return result;
}

/* ResetDialogFields
*/
void PMUExportPrefsDialogController::ResetDialogFields( IActiveContext* myContext) 
{
	SetTriStateControlData(kPMUExportStyleWidgetID,ITriStateControlData::kSelected);
	SetTriStateControlData(kPMUExportCJWidgetID,ITriStateControlData::kSelected);
	SetTriStateControlData(kPMUExportColorsWidgetID,ITriStateControlData::kSelected);
	SetTriStateControlData(kPMUExportLayersWidgetID,ITriStateControlData::kSelected);
	SetTriStateControlData(kPMUExportMastersWidgetID,ITriStateControlData::kSelected);

	SetTriStateControlData(kPMUExportTextFrameWidgetID,ITriStateControlData::kSelected);
	SetTriStateControlData(kPMUExportImageFrameWidgetID,ITriStateControlData::kSelected);
	SetTriStateControlData(kPMUExportRuleWidgetID,ITriStateControlData::kSelected);

	SetTriStateControlData(kPMUExportTextWidgetID,ITriStateControlData::kSelected);
	SetTriStateControlData(kPMUExportParaCmdWidgetID,ITriStateControlData::kSelected);
	SetTriStateControlData(kPMUExportTextCmdWidgetID,ITriStateControlData::kSelected);
	SetTriStateControlData(kPMUExportStyleCmdWidgetID,ITriStateControlData::kSelected);
}

/* ApplyFields
*/
void PMUExportPrefsDialogController::ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId) 
{
	ErrorCode status = kFailure;
	do
	{
		InterfacePtr<IPageMakeUpPrefs> currentPreferences ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (currentPreferences == nil)
		{
			ASSERT_FAIL("PMUExportPrefsDialogController::InitializeDialogFields() storedPrefs invalid");
			break;
		}

		// Update the preferences with a command
		UIDList workspace(::GetUIDRef(currentPreferences));
		InterfacePtr<ICommand> setPrefsCmd (CmdUtils::CreateCommand(kPrsSetPreferencesCmdBoss));
		if(setPrefsCmd == nil)
		{		
			ASSERT_FAIL("PMUExportPrefsDialogController::ApplyDialogFields -> setPrefsCmd nil");
			break;
		}

		setPrefsCmd->SetItemList(workspace);

		InterfacePtr<IPageMakeUpPrefs> cmdData (setPrefsCmd, UseDefaultIID());
		if(cmdData == nil)
		{		
			ASSERT_FAIL("PMUExportPrefsDialogController::ApplyDialogFields -> cmdData nil");
			break;
		}

		cmdData->CopyFrom(currentPreferences);

		bool16 useSavePath = (GetTriStateControlData(kPMUUseSavePathWidgetID) == ITriStateControlData::kSelected);
		bool16 useOpenPath = (GetTriStateControlData(kPMUUseOpenPathWidgetID) == ITriStateControlData::kSelected);

		// Get new preferences from widget values
		bool16 exportStyles = (GetTriStateControlData(kPMUExportStyleWidgetID) == ITriStateControlData::kSelected);
		bool16 exportCJ = (GetTriStateControlData(kPMUExportCJWidgetID) == ITriStateControlData::kSelected);
		bool16 exportColors = (GetTriStateControlData(kPMUExportColorsWidgetID) == ITriStateControlData::kSelected);
		bool16 exportLayers = (GetTriStateControlData(kPMUExportLayersWidgetID) == ITriStateControlData::kSelected);
		bool16 exportMasters = (GetTriStateControlData(kPMUExportMastersWidgetID) == ITriStateControlData::kSelected);
		bool16 exportTextFrame = (GetTriStateControlData(kPMUExportTextFrameWidgetID) == ITriStateControlData::kSelected);
		bool16 exportImageFrame = (GetTriStateControlData(kPMUExportImageFrameWidgetID) == ITriStateControlData::kSelected);
		bool16 exportRule = (GetTriStateControlData(kPMUExportRuleWidgetID) == ITriStateControlData::kSelected);
		bool16 exportText = (GetTriStateControlData(kPMUExportTextWidgetID) == ITriStateControlData::kSelected);
		bool16 exportParaCmd = (GetTriStateControlData(kPMUExportParaCmdWidgetID) == ITriStateControlData::kSelected);
		bool16 exportTextCmd = (GetTriStateControlData(kPMUExportTextCmdWidgetID) == ITriStateControlData::kSelected);
		bool16 exportStyleCmd = (GetTriStateControlData(kPMUExportStyleCmdWidgetID) == ITriStateControlData::kSelected);

		cmdData->setExportHeaderPrefs(exportStyles, exportCJ, exportColors,exportLayers, exportMasters);
		cmdData->setExportGeometryPrefs(exportTextFrame, exportImageFrame, exportRule);
		cmdData->setExportTextPrefs(exportText, exportParaCmd, exportTextCmd, exportStyleCmd);

		status = CmdUtils::ProcessCommand(setPrefsCmd);

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(kPMUSavePrefsPb);
}




