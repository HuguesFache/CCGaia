/*
//
//	File:	XRailOpenLoginDialogCmd.cpp
//
//  Author: Trias
//
//	Date:	20/04/2006
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2006 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"
#include "DocUtils.h"

// Interface includes
#include "ISubject.h"
#include "IDialog.h"

#include "XRLUIID.h"

/** XRailOpenLoginDialogCmd
*/
class XRailOpenLoginDialogCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRailOpenLoginDialogCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XRailOpenLoginDialogCmd();

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
		Set up target to session
	*/
	void SetUpTarget();

	/**
		Gives the name that shows up under the Edit menu and allows the user to undo or redo the command.

		@return pointer to a PMString allocated using the new operator. Deleted by caller.
	*/
	PMString* CreateName();

};


CREATE_PMINTERFACE(XRailOpenLoginDialogCmd, kXRLUIOpenLoginDialogCmdImpl)

/* Constructor
*/
XRailOpenLoginDialogCmd::XRailOpenLoginDialogCmd(IPMUnknown* boss) 
: Command(boss)
{
	SetUndoability(ICommand::kAutoUndo);
}

/* Destructor
*/
XRailOpenLoginDialogCmd::~XRailOpenLoginDialogCmd()
{
}

/* Do
*/
void XRailOpenLoginDialogCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		IDialog * dialog = CreateNewDialog(kXRLUIPluginID, kXRLUILoginDialogResourceID);
		if(!dialog)
			break;

		dialog->Open(); 

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* CreateName
*/
PMString * XRailOpenLoginDialogCmd::CreateName()
{
	PMString * name = new PMString("Open login dialog");
	name->SetTranslatable(kFalse);
	return name;
}

/*
	SetUpTarget
*/
void XRailOpenLoginDialogCmd::SetUpTarget()
{
	SetTarget(Command::kSessionTarget);
}



