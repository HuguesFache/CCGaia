/*
//
//	File:	PrsSetRegistrationInfoCmd.cpp
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

// General includes
#include "ErrorUtils.h"
#include "Command.h"

// Interface includes
#include "ISubject.h"

// Project includes
#include "IRegistration.h"

/** PrsSetRegistrationInfoCmd
	The implementation of the PrsSetRegistrationInfoCmd, it gets the command data from its sibling interface, 
	a non persistent IPageMakeUpPrefs, then use it to set the persist IPageMakeUpPrefs on a document, passed in the
	item list of the command
*/
class PrsSetRegistrationInfoCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsSetRegistrationInfoCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsSetRegistrationInfoCmd();

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
		Notify interested observer about the change we have done in this command.
	*/
	void DoNotify();

	/**
		Gives the name that shows up under the Edit menu and allows the user to undo or redo the command.

		@return pointer to a PMString allocated using the new operator. Deleted by caller.
	*/
	PMString* CreateName();

	/**
		Set up target to global workspace
	*/
	void SetUpTarget();

private:

};


CREATE_PMINTERFACE(PrsSetRegistrationInfoCmd, kPrsSetRegistrationInfoCmdImpl)

/* Constructor
*/
PrsSetRegistrationInfoCmd::PrsSetRegistrationInfoCmd(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
PrsSetRegistrationInfoCmd::~PrsSetRegistrationInfoCmd()
{
}

/* Do
*/
void PrsSetRegistrationInfoCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		// Get the command datas
		InterfacePtr<IRegistration> newRegistrationInfo(this, UseDefaultIID());
		if (newRegistrationInfo == nil)
			break;
	
		InterfacePtr<IRegistration> storedInfo((IRegistration *)GetExecutionContextSession()->QueryWorkspace(IID_IREGISTRATION));
		if(storedInfo == nil)
			break;

		// Apply the new info
		storedInfo->setStatus(newRegistrationInfo->getStatus());
		storedInfo->setMaxUsers(newRegistrationInfo->getMaxUsers());
		storedInfo->setSerialNumber(newRegistrationInfo->getSerialNumber());
		storedInfo->setIDSerialNumber(newRegistrationInfo->getIDSerialNumber());
		storedInfo->setVersion(newRegistrationInfo->getVersion());
		storedInfo->setComposerTools(newRegistrationInfo->getComposerTools());
		storedInfo->setReverse(newRegistrationInfo->getReverse());
		storedInfo->setPermRefs(newRegistrationInfo->getPermRefs());
		storedInfo->setPageLink(newRegistrationInfo->getPageLink());
		storedInfo->setTimeBombed(newRegistrationInfo->getTimeBombed());

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void PrsSetRegistrationInfoCmd::DoNotify()
{
	ErrorCode status = kFailure;
			
	do
	{
		InterfacePtr<ISubject> subject ((ISubject *)GetExecutionContextSession()->QueryWorkspace(IID_ISUBJECT));
		if(subject == nil)
		{
			ASSERT_FAIL("PrsSetRegistrationInfoCmd::DoNotify -> subject nil");
			break;
		}

		// Notify the subject of the change
		subject->Change(kPrsSetRegistrationInfoCmdBoss, IID_IREGISTRATION, this);

		status = kSuccess;
	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}


/* CreateName
*/
PMString * PrsSetRegistrationInfoCmd::CreateName()
{
	PMString * name = new PMString("Save Preferences");
	name->SetTranslatable(kFalse);
	return name;
}

/*
	SetUpTarget
*/
void PrsSetRegistrationInfoCmd::SetUpTarget()
{
	SetTarget(Command::kSessionWorkspaceTarget);
}

