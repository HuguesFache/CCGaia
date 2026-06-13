/*
//
//	File:	PMUSetUpComposerPaletteCmd.cpp
//
//  Author: Trias
//
//	Date:	19-oct-2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// Interface includes
#include "IControlView.h"
#include "IApplication.h"
#include "IPanelMgr.h"
#include "IActionManager.h"
#include "IMenuManager.h"
#include "ITextControlData.h"
#include "ISession.h"
#include "IPersistUIDData.h"
#include "IWindow.h"
#include "IRegistration.h"
#include "IPanelControlData.h"
#include "IPalettePanelUtils.h"

// General includes:
#include "ResourceEnabler.h"
#include "LocaleSetting.h"
#include "CoreResTypes.h" // kViewRsrcType
#include "ActionDefs.h"	// kDontTranslateChar
#include "AdobeMenuPositions.h"
#include "CPMUnknown.h"
#include "RsrcSpec.h"
#include "PaletteRefUtils.h"
#include "Command.h"
#include "ErrorUtils.h"

// Project includes
#include "PMUID.h"
#include "PrsID.h"

/** PMUSetUpComposerPaletteCmd
	The implementation of the PMUSetUpComposerPaletteCmd, it gets the command data from its sibling interface, 
	a non persistent IPageMakeUpPrefs, then use it to set the persist IPageMakeUpPrefs on a document, passed in the
	item list of the command
*/
class PMUSetUpComposerPaletteCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PMUSetUpComposerPaletteCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PMUSetUpComposerPaletteCmd();

	/**
		@return kFalse don't use this command if memory is low. 
	*/
	bool16 LowMemIsOK() const { return kFalse; }

protected:
	/**
		The Do method is where all the action happens. 
	*/
	void Do();

	/**
		Undoes the command, by simply setting the data interfaces back to  the original values.
	*/
	void Undo();

	/**
		Call Do function
	*/
	void Redo();

	/**
		Notify interested observer about the change we have done in this command.
	*/
	void DoNotify();

	/**
		Gives the name that shows up under the Edit menu and allows the user to undo or redo the command.

		@return pointer to a PMString allocated using the new operator. Deleted by caller.
	*/
	PMString* CreateName();

	/**
		Set up target to session
	*/
	void SetUpTarget();

};


CREATE_PMINTERFACE(PMUSetUpComposerPaletteCmd, kPMUSetUpComposerPaletteCmdImpl)

/* Constructor
*/
PMUSetUpComposerPaletteCmd::PMUSetUpComposerPaletteCmd(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
PMUSetUpComposerPaletteCmd::~PMUSetUpComposerPaletteCmd()
{
}

/* Do
*/
void PMUSetUpComposerPaletteCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		// Check registration to figure out which palette we've got to instantiate
		InterfacePtr<IRegistration> registrationInfo((IRegistration *) GetExecutionContextSession()->QueryWorkspace(IID_IREGISTRATION));
		bool16 composerTool = registrationInfo->getComposerTools(); 

#if !DEMO
		if(registrationInfo->getStatus() != ID_REGISTERED)
		{
			// Composer is not registered yet, no need to create palette
			status = kSuccess;
			break;
		}		
#endif
		ResourceEnabler en;
		InterfacePtr<IApplication> app(GetExecutionContextSession()->QueryApplication());
		
		InterfacePtr<IPanelMgr> panelMgr(app->QueryPanelManager());

		// Check whether palette has already been created
		WidgetID panelToCheck = 0, otherPanel = 0;
		if(composerTool)
		{
			panelToCheck = kComposerToolPanelWidgetID;
			otherPanel = kPMUPanelWidgetID;			
		}
		else
		{
			panelToCheck = kPMUPanelWidgetID;
			otherPanel = kComposerToolPanelWidgetID;
		}

		InterfacePtr<IActionManager> actionMgr(app->QueryActionManager());
		InterfacePtr<IMenuManager> menuMgr(actionMgr, UseDefaultIID());

		if(panelMgr->DoesPanelExist(panelToCheck))
		{
			status = kSuccess;
			break;
		}
		else
		{
			IControlView * existingPanel = panelMgr->GetPanelFromWidgetID(otherPanel);
			if(existingPanel)
			{
				// User may have changed license option, delete old palette
				menuMgr->RemoveAllMenusForAction(kPMUSeparatorActionID);			
				
				InterfacePtr<ICommand> destroyCmd (Utils<IPalettePanelUtils>()->CreateDestroyPanelCmd(::GetUID(existingPanel)));
				CmdUtils::ProcessCommand(destroyCmd);
			}
		}

		IControlView* newPanel = nil;
		PMLocaleId localeId = LocaleSetting::GetLocale();
		
		// Create the panel			
		PMString panelName;
		RsrcID paletteID = 0;
		WidgetID panelID = 0;
		if(composerTool)
		{			
			panelName = PMString(kComposerToolPanelTitleKey, PMString::kUnknownEncoding);				
			paletteID = kComposerToolPanelResourceID;
			panelID = kComposerToolPanelWidgetID;
		}
		else
		{				
			panelName = PMString(kPMUPanelTitleKey, PMString::kUnknownEncoding); // PageMakeUp and Composer Full version shared the same main palette
			paletteID = kPMUPanelResourceID;
			panelID = kPMUPanelWidgetID;
		}

		PMString menuName("");
		menuName.Append( kDontTranslateChar ) ;	// do this so the filename won't be translated by the menumgr.
		menuName.Append(panelName);
					
		// Make the new panel
		IDataBase *panelDB = ::GetDataBase(panelMgr);
		InterfacePtr<IControlView> panel((IControlView*) ::CreateObject(panelDB,
						RsrcSpec(localeId, kPMUPluginID, kViewRsrcType, paletteID), IID_ICONTROLVIEW));

		panel->SetWidgetID(panelID);

		// Change the name of the panel
		InterfacePtr<ITextControlData> panelTextData(panel, UseDefaultIID());
		ASSERT(panelTextData);
		if(!panelTextData) {
			break;
		}
		panelTextData->SetString( panelName, kTrue, kTrue );

		// Add menu item at the end of Window menu, just after a separator			
		menuMgr->AddMenuItem(kPMUSeparatorActionID, 
					"Main:&Window:-",
					kWindowLastPanelMenuPosition + 1,
					kFalse);

		// Give the panel to the panel manager and display
		menuName.Insert("Main:&Window:");
	
		panelMgr->RegisterPanel( kTabPanelContainerType,panel, 
								kFalse, "", 
								kPMUPanelWidgetActionID, 
								menuName,kWindowLastPanelMenuPosition + 2,
								c_Panel,kPMUPluginID,0) ;
		
		// Make the Palette and Panel visible
		panelMgr->ShowPanelByWidgetID(panelID);

		PaletteRefUtils::CenterPaletteOnScreen(panelMgr->GetPaletteRefContainingPanel(panel));

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* Redo
*/
void PMUSetUpComposerPaletteCmd::Redo()
{
	this->Do();
}

/* Undo
*/
void PMUSetUpComposerPaletteCmd::Undo()
{
}

/* DoNotify
*/
void PMUSetUpComposerPaletteCmd::DoNotify()
{
}


/* CreateName
*/
PMString * PMUSetUpComposerPaletteCmd::CreateName()
{
	PMString * name = new PMString("Set Up Palette");
	return name;
}

/*
	SetUpTarget
*/
void PMUSetUpComposerPaletteCmd::SetUpTarget()
{
	SetTarget(Command::kSessionTarget);
}
