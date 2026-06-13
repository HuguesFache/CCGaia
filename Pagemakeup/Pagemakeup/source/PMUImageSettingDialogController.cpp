/*
//	File:	PMUImageSettingDialogController.cpp
//
//	Author:	Trias
//
//	Date:	17/10/2005
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IPanelControlData.h"
#include "IDropDownListController.h"
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
class PMUImageSettingDialogController : public CDialogController
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PMUImageSettingDialogController(IPMUnknown* boss) : CDialogController(boss) {}

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
CREATE_PMINTERFACE(PMUImageSettingDialogController, kPMUImageSettingDialogControllerImpl)

/* ApplyFields
*/
void PMUImageSettingDialogController::InitializeDialogFields( IActiveContext* dlgContext) 
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
		bool16 useImportPath,  import, textbox, nonPrinting;
		PMString picPath;
		int32 colorIndex; 
		int32 colorTint;
		
		storedPrefs->getPicturePrefs(&useImportPath, &import, &picPath, &textbox, &nonPrinting, &colorIndex, &colorTint);

		// Set controls datas
		SetTriStateControlData(kPMUAllowImportImageWidgetID,import);
		SetTriStateControlData(kPMUUseImagePathWidgetID,useImportPath);
		SetTriStateControlData(kPMUCreateTxtFrameWidgetID,textbox);
		SetTriStateControlData(kPMUPrintFrameWidgetID,nonPrinting);

		SetTextControlData(kPMUImagePathWidgetID, picPath);

		// Get the IPanelControlData interface for the dialog:
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("PMUImageSettingDialogController::InitializeDialogFields() panelControlData invalid");
			break;
		}

		IControlView* colorList = panelControlData->FindWidget(kPMUDropDownColorListWidgetID);
		if (colorList == nil)
		{
			ASSERT_FAIL("colorList invalid");
			break;
		}

		InterfacePtr<IDropDownListController> colorListController (colorList, UseDefaultIID());
		if(colorListController == nil)
		{
			ASSERT_FAIL("colorListController invalid");
			break;
		}

		colorListController->Select(colorIndex);

		SetTextValue(kPMUColorTintComboBoxWidgetID, colorTint);
		
	} while (false);
}

/* ValidateFields
*/
WidgetID PMUImageSettingDialogController::ValidateDialogFields( IActiveContext* myContext) 
{
	WidgetID result = kNoInvalidWidgets;

	// Put code to validate widget values here.

	return result;
}

/* ResetDialogFields
*/
void PMUImageSettingDialogController::ResetDialogFields( IActiveContext* myContext) 
{
	do
	{
		SetTriStateControlData(kPMUAllowImportImageWidgetID,ITriStateControlData::kSelected);
		SetTriStateControlData(kPMUUseImagePathWidgetID,ITriStateControlData::kUnselected);
		SetTriStateControlData(kPMUCreateTxtFrameWidgetID,ITriStateControlData::kSelected);
		SetTriStateControlData(kPMUPrintFrameWidgetID,ITriStateControlData::kSelected);

		SetTextControlData(kPMUImagePathWidgetID, PMString(""));

		// Get the IPanelControlData interface for the dialog:
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("PMUImageSettingDialogController::InitializeDialogFields() panelControlData invalid");
			break;
		}

		IControlView* colorList = panelControlData->FindWidget(kPMUDropDownColorListWidgetID);
		if (colorList == nil)
		{
			ASSERT_FAIL("colorList invalid");
			break;
		}

		InterfacePtr<IDropDownListController> colorListController (colorList, UseDefaultIID());
		if(colorListController == nil)
		{
			ASSERT_FAIL("colorListController invalid");
			break;
		}

		colorListController->Select(9);

		SetTextValue(kPMUColorTintComboBoxWidgetID, 100);

	} while(false);
}

/* ApplyFields
*/
void PMUImageSettingDialogController::ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId) 
{
	ErrorCode status = kFailure;
	do
	{
		InterfacePtr<IPageMakeUpPrefs> currentPreferences ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (currentPreferences == nil)
		{
			ASSERT_FAIL("PMUImageSettingDialogController::InitializeDialogFields() storedPrefs invalid");
			break;
		}

		// Update the preferences with a command
		UIDList workspace(::GetUIDRef(currentPreferences));
		InterfacePtr<ICommand> setPrefsCmd (CmdUtils::CreateCommand(kPrsSetPreferencesCmdBoss));
		if(setPrefsCmd == nil)
		{		
			ASSERT_FAIL("PMUImageSettingDialogController::ApplyDialogFields -> setPrefsCmd nil");
			break;
		}

		setPrefsCmd->SetItemList(workspace);

		InterfacePtr<IPageMakeUpPrefs> cmdData (setPrefsCmd, UseDefaultIID());
		if(cmdData == nil)
		{		
			ASSERT_FAIL("PMUImageSettingDialogController::ApplyDialogFields -> cmdData nil");
			break;
		}

		cmdData->CopyFrom(currentPreferences);

		// Get new preferences from widget values
		bool16 useImportPath = (GetTriStateControlData(kPMUUseImagePathWidgetID) == ITriStateControlData::kSelected);
		bool16 import = (GetTriStateControlData(kPMUAllowImportImageWidgetID) == ITriStateControlData::kSelected);
		bool16 textbox = (GetTriStateControlData(kPMUCreateTxtFrameWidgetID) == ITriStateControlData::kSelected);
		bool16 nonPrinting = (GetTriStateControlData(kPMUPrintFrameWidgetID) == ITriStateControlData::kSelected);
		PMString picPath = GetTextControlData(kPMUImagePathWidgetID);
		int32 colorIndex; 
		PMReal colorTint = GetTextValue(kPMUColorTintComboBoxWidgetID);

		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
			break;

		IControlView* colorList = panelControlData->FindWidget(kPMUDropDownColorListWidgetID);
		if (colorList == nil)
			break;

		InterfacePtr<IDropDownListController> colorListController (colorList, UseDefaultIID());
		if(colorListController == nil)
			break;

		colorIndex = colorListController->GetSelected();

		cmdData->setPicturePrefs(useImportPath, import, picPath, textbox, nonPrinting, colorIndex, ToInt32(colorTint));

		status = CmdUtils::ProcessCommand(setPrefsCmd);

	} while(kFalse);

	if(status != kSuccess)
		CAlert::InformationAlert(kPMUSavePrefsPb); 
}




