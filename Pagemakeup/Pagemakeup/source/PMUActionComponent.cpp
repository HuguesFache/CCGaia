/*
//	File:	PMUActionComponent.cpp
//
//	Date:	14-Apr-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//	
//	NOTICE: Adobe permits you to use, modify, and distribute this file in
//	accordance with the terms of the Adobe license agreement accompanying it.
//	If you have received this file from a source other than Adobe, then your
//	use, modification, or distribution of it requires the prior written
//	permission of Adobe.
//
*/

#include "VCPlugInHeaders.h"


// General includes:
#include "CActionComponent.h"
#include "CAlert.h"
#include "CmdUtils.h"
#include "ILayoutUIUtils.h"

// Interfaces includes
#include "IDialog.h"
#include "IActionStateList.h"
#include "IApplication.h"
#include "IDocumentList.h"
#include "ICommand.h"
#include "IPageMakeUpPrefs.h"
#include "IPanelMgr.h"
#include "IPanelControlData.h"
#include "IControlView.h"
#include "ITriStateControlData.h"
#include "ISelectionManager.h"
#include "ISelectionUtils.h"
#include "IPermRefsSuite.h"
#include "IParserSuite.h"
#include "UIDList.h"
#include "TextID.h"

// Project includes:
#include "PMUID.h"
#include "PMUUtils.h"
#include "prsID.h"


/** PMUActionComponent
	implements the actions that are executed when the plug-in's
	menu items are selected.

	PMUActionComponent implements IActionComponent based on
	the partial implementation CActionComponent.


	@author Trias
*/
class PMUActionComponent : public CActionComponent
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PMUActionComponent(IPMUnknown* boss);

		/**
			The action component should do the requested action.
			This is where the menu item's action is taken.
			When a menu item is selected, the Menu Manager determines
			which plug-in is responsible for it, and calls its DoAction
			with the ID for the menu item chosen.

			@param actionID identifies the menu item that was selected.
			*/
		virtual void DoAction(IActiveContext* ac, ActionID actionID, GSysPoint mousePoint, IPMUnknown* widget);

		/** Set the checkmark on the display-toggle menu items so each
			reflects the current preference state.
		*/
		virtual void UpdateActionStates(IActiveContext* ac, IActionStateList* listToUpdate, GSysPoint mousePoint, IPMUnknown* widget);

	private:
		/** Encapsulates functionality for the about menu item. */
		void DoAbout();

		/** Flip a display preference (block IDs or hidden references) and
			invalidate all open documents so the change shows immediately.
			@param toggleBlockIDs kTrue to flip the block-ID preference,
				kFalse to flip the hidden-references preference.
		*/
		void ToggleDisplayPref(bool16 toggleBlockIDs);
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(PMUActionComponent, kPMUActionComponentImpl)

/* PMUActionComponent Constructor
*/
PMUActionComponent::PMUActionComponent(IPMUnknown* boss)
				   : CActionComponent(boss){}

/* DoAction
*/

void PMUActionComponent::DoAction(IActiveContext* ac, ActionID actionID, GSysPoint mousePoint, IPMUnknown* widget)
{
	switch (actionID.Get())
	{
		case kPMUPopupAboutThisActionID:
		case kPMUAboutActionID:
		{
			this->DoAbout();
			break;
		}

		case kPMUExecTCLActionID:
		{
			PMUUtils::ExecTCL();		
			break;
		}
		
		case kPMUReverseTCLActionID:
		{
			PMUUtils::ReverseTCL();
			break;
		}

		case kPMUExportPermRefsTCLActionID:
		{
			PMUUtils::ExportPermRefsWithContent();
			break;
		}

		case kPMUTCLPrefsActionID:
		{
			PMUUtils::ShowTCLPrefs();
			break;
		}

		case kPMUAddReferenceTCLActionID:
		{
			PMUUtils::AddPermRef();
			break;
		}

		case kPMUAddTCLRefActionID:
		{
			// "Identifier un bloc" — same code path as the palette button
			// kPMUAddTCLRefWidgetID.
			PMUUtils::AddTCLRef();
			break;
		}

		case kPMUDisplayTCLRefActionID:
		{
			// Toggle "show block IDs" preference.
			this->ToggleDisplayPref(kTrue);
			break;
		}

		case kPMUDisplayPermRefActionID:
		{
			// Toggle "show hidden references in-text" preference.
			this->ToggleDisplayPref(kFalse);
			break;
		}

		case kPMUDeleteAllPermRefsTCLActionID:
		{
			PMUUtils::DeleteAllPermsRefsTCL();
			break;
		}

		case kPMUExportRefsListDebugActionID:
		{
			PMUUtils::ExportRefsListDebug();
			break;
		}

		case kPMURepairRefsListActionID:
		{
			PMUUtils::RepairRefsList();
			break;
		}

#ifdef AUTO_TAGGING

		case kPMUAutoTagDocumentWithPriceTCLActionID:
		{
			PMUUtils::AutoTagDocumentPermsRefsTCL();
			break;
		}

		case kPMUAutoTagDocumentWithReferenceTCLActionID:
		{
			PMUUtils::AutoTagDocumentWithReferencePermsRefsTCL();
			break;
		}

#endif

#if ILC
		case kPMUCreateILCRefsActionID:
		{
			PMUUtils::ILC_AutoTagDocumentWithReferencePermsRefsTCL();
			break;
		}
			
		case kPMUExportILCRefsActionID:
		{
			PMUUtils::ILC_ExportPermRefs();
			break;
		}
#endif

		default:
		{
			break;
		}
	}
}

/* DoAbout
*/
void PMUActionComponent::DoAbout()
{

#if COMPOSER
	IDialog * dialog = PMUUtils::CreateNewDialog(kComposerAboutDialogResourceID);
	if(dialog != nil)
		dialog->Open();
#else
	CAlert::ModalAlert
	(
		kPMUAboutBoxStringKey,				// Alert string
		kOKString, 						// OK button
		kNullString, 						// No second button
		kNullString, 						// No third button
		1,							// Set OK button to default
		CAlert::eInformationIcon				// Information icon.
	);
#endif
}


/* UpdateActionStates
 *
 * Drives the menu state for actions that aren't always-on:
 *   - kPMUAddReferenceTCLActionID ("Insérer une référence cachée"):
 *       enabled only when a text range is currently selected.
 *   - kPMUAddTCLRefActionID ("Identifier un bloc"):
 *       enabled only when a layout (page-item) selection is present.
 *   - kPMUDisplayTCLRefActionID / kPMUDisplayPermRefActionID:
 *       always enabled, with a checkmark reflecting the current pref.
 *
 * Other actions in the list (e.g. kPMUExecTCLActionID) are left untouched
 * and stay enabled by default — that matches "Exécuter TCL toujours actif".
*/
void PMUActionComponent::UpdateActionStates(IActiveContext* /*ac*/, IActionStateList* listToUpdate, GSysPoint /*mousePoint*/, IPMUnknown* /*widget*/)
{
	if (listToUpdate == nil)
		return;

	InterfacePtr<IPageMakeUpPrefs> prefs((IPageMakeUpPrefs*)
		GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));

	// Resolve the active selection once and probe for both kinds (text /
	// layout). Either probe can fail — that's the "no selection of that
	// kind" signal, same convention PMUUtils::AddPermRef and AddTCLRef use.
	bool16 hasTextSelection   = kFalse;
	bool16 hasLayoutSelection = kFalse;
	{
		InterfacePtr<ISelectionManager> selMgr(Utils<ISelectionUtils>()->QueryActiveSelection());
		if (selMgr != nil)
		{
			InterfacePtr<IPermRefsSuite> permRefsSuite(selMgr, UseDefaultIID());
			if (permRefsSuite != nil)
			{
				TextIndex start = 0, end = 0;
				UIDRef txtModel;
				permRefsSuite->GetTextSelection(txtModel, start, end);
				hasTextSelection = (start != end);
			}

			InterfacePtr<IParserSuite> prsSuite(selMgr, UseDefaultIID());
			if (prsSuite != nil)
			{
				UIDList itemList;
				prsSuite->GetSelectedItems(itemList);
				hasLayoutSelection = !itemList.IsEmpty();
			}
		}
	}

	for (int32 i = 0; i < listToUpdate->Length(); ++i)
	{
		ActionID theAction = listToUpdate->GetNthAction(i);
		switch (theAction.Get())
		{
			case kPMUAddReferenceTCLActionID:
			{
				listToUpdate->SetNthActionState(i,
					hasTextSelection ? kEnabledAction : kDisabled_Unselected);
				break;
			}
			case kPMUAddTCLRefActionID:
			{
				listToUpdate->SetNthActionState(i,
					hasLayoutSelection ? kEnabledAction : kDisabled_Unselected);
				break;
			}
			case kPMUDisplayTCLRefActionID:
			{
				// kSelectedAction is a flag (= "checked"), NOT a state. On
				// its own it would mark the menu greyed-out + checked, so
				// the user couldn't toggle it back off. Combine with
				// kEnabledAction to keep the item clickable when ticked.
				int16 state = (prefs && prefs->getDisplayTCLRef())
				              ? (kEnabledAction | kSelectedAction)
				              : kEnabledAction;
				listToUpdate->SetNthActionState(i, state);
				break;
			}
			case kPMUDisplayPermRefActionID:
			{
				int16 state = (prefs && prefs->getDisplayPermRef())
				              ? (kEnabledAction | kSelectedAction)
				              : kEnabledAction;
				listToUpdate->SetNthActionState(i, state);
				break;
			}
			default:
				break;
		}
	}
}


/* ToggleDisplayPref
 *
 * Flip one of the two display preferences (block IDs or hidden references)
 * via the same kPrsSetPreferencesCmdBoss command path that the palette
 * checkbox uses, then invalidate every open document so the on-canvas
 * adornments redraw immediately.
*/
void PMUActionComponent::ToggleDisplayPref(bool16 toggleBlockIDs)
{
	do
	{
		InterfacePtr<IPageMakeUpPrefs> currentPreferences((IPageMakeUpPrefs*)
			GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (currentPreferences == nil)
			break;

		UIDList workspace(::GetUIDRef(currentPreferences));
		InterfacePtr<ICommand> setPrefsCmd(CmdUtils::CreateCommand(kPrsSetPreferencesCmdBoss));
		if (setPrefsCmd == nil)
			break;

		setPrefsCmd->SetItemList(workspace);

		InterfacePtr<IPageMakeUpPrefs> cmdData(setPrefsCmd, UseDefaultIID());
		if (cmdData == nil)
			break;

		cmdData->CopyFrom(currentPreferences);

		const bool16 newValue = toggleBlockIDs
		                        ? !currentPreferences->getDisplayTCLRef()
		                        : !currentPreferences->getDisplayPermRef();

		if (toggleBlockIDs)
			cmdData->setDisplayTCLRef(newValue);
		else
			cmdData->setDisplayPermRef(newValue);

		if (CmdUtils::ProcessCommand(setPrefsCmd) != kSuccess)
			break;

		// Keep the palette checkbox in sync with the pref we just toggled.
		// The checkbox is initialised from the pref on palette open
		// (PMUPanelWidgetObserver::AutoAttach), but if the palette is
		// already open we have to update the visual state ourselves.
		if (toggleBlockIDs)
		{
			InterfacePtr<IApplication> appForPanel(GetExecutionContextSession()->QueryApplication());
			InterfacePtr<IPanelMgr> panelMgr(appForPanel ? appForPanel->QueryPanelManager() : nil);
			IControlView* panelView = panelMgr ? panelMgr->GetVisiblePanel(kPMUPanelWidgetID) : nil;
			InterfacePtr<IPanelControlData> panelData(panelView, UseDefaultIID());
			IControlView* checkboxView = panelData ? panelData->FindWidget(kPMUDisplayTCLRefWidgetID) : nil;
			InterfacePtr<ITriStateControlData> checkbox(checkboxView, UseDefaultIID());
			if (checkbox)
			{
				// invalidate=kTrue so the widget redraws; notifyOfChange=kFalse
				// so we don't re-enter SetDisplayRef (which would read the
				// widget back and overwrite the pref we just set).
				if (newValue) checkbox->Select(kTrue, kFalse);
				else          checkbox->Deselect(kTrue, kFalse);
			}
		}

		// Force a redraw of every open document.
		InterfacePtr<IApplication> app(GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> docList(app->QueryDocumentList());
		const int32 nbDocs = docList->GetDocCount();
		for (int32 i = 0; i < nbDocs; ++i)
			Utils<ILayoutUIUtils>()->InvalidateViews(docList->GetNthDoc(i));

	} while (kFalse);
}

//  Generated by Dolly build 17: template "IfPanelMenu".
// End, PMUActionComponent.cpp.




