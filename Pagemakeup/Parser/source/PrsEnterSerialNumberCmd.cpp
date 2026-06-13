/*
//
//	File:	PrsEnterSerialNumberCmd.cpp
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
#include "GlobalDefs.h"

#if !SERVER

// General includes
#include "ErrorUtils.h"
#include "Command.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"

// Interface includes
#include "IDialog.h"
#include "IDialogMgr.h"
#include "IApplication.h"

// Project includes
#include "PrsID.h"


/** PrsEnterSerialNumberCmd
	The implementation of the PrsEnterSerialNumberCmd, it gets the command data from its sibling interface, 
	a non persistent IPageMakeUpPrefs, then use it to set the persist IPageMakeUpPrefs on a document, passed in the
	item list of the command
*/
class PrsEnterSerialNumberCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsEnterSerialNumberCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsEnterSerialNumberCmd();

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
		Gives the name that shows up under the Edit menu and allows the user to undo or redo the command.

		@return pointer to a PMString allocated using the new operator. Deleted by caller.
	*/
	PMString* CreateName();

	/**
		Set up target to session
	*/
	void SetUpTarget();

};


CREATE_PMINTERFACE(PrsEnterSerialNumberCmd, kPrsEnterSerialNumberCmdImpl)

/* Constructor
*/
PrsEnterSerialNumberCmd::PrsEnterSerialNumberCmd(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
PrsEnterSerialNumberCmd::~PrsEnterSerialNumberCmd()
{
}

/* Do
*/
void PrsEnterSerialNumberCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		// Get the application interface and the DialogMgr.	
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		if (application == nil)
			break;

		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
		if (dialogMgr == nil)
			break;

		// Load the plug-in's resource.
		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcSpec dialogSpec
		(
			nLocale,					// Locale index from PMLocaleIDs.h. 
			kPrsPluginID,				// Plug-in ID
			kViewRsrcType,				// This is the kViewRsrcType.
			kPrsSerialNumberDialogResourceID,	// Resource ID for our dialog.
			kTrue						// Initially visible.
		);

		// CreateNewDialog takes the dialogSpec created above, and also
		// the type of dialog being created (kMovableModal).
		
		IDialog * dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);
		if(dialog == nil)
			break;

		dialog->Open();

#if COMPOSER
		dialog->WaitForDialog(kTrue);  // in SetUpComposerPaletteCmd to be processed when the dialog is closed
#endif
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* CreateName
*/
PMString * PrsEnterSerialNumberCmd::CreateName()
{
	PMString * name = new PMString(kPrsEnterSerialKey);
	return name;
}

/*
	SetUpTarget
*/
void PrsEnterSerialNumberCmd::SetUpTarget()
{
	SetTarget(Command::kSessionTarget);
	SetUndoability(ICommand::kAutoUndo);
}

#endif
