/*
//
//	File:	PrsLastPremRefCmd.cpp
//
//  Author: Wilfried LEFEVRE
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
#include "ILastPermRefsTag.h"

/** PrsLastPremRefCmd
	The implementation of the PrsLastPremRefCmd, it gets the command data from its sibling interface, 
	a non persistent IPageMakeUpPrefs, then use it to set the persist IPageMakeUpPrefs on a document, passed in the
	item list of the command
*/
class PrsLastPremRefCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsLastPremRefCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsLastPremRefCmd();

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


CREATE_PMINTERFACE(PrsLastPremRefCmd, kPrsLastPremRefCmdImpl)

/* Constructor
*/
PrsLastPremRefCmd::PrsLastPremRefCmd(IPMUnknown* boss): Command(boss)
{
}

/* Destructor
*/
PrsLastPremRefCmd::~PrsLastPremRefCmd()
{
}

/* Do
*/
void PrsLastPremRefCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		// Get the command datas
		InterfacePtr<ILastPermRefsTag> newPermRefTag(this, UseDefaultIID());
		if (newPermRefTag == nil)
			break;
	
		InterfacePtr<ILastPermRefsTag> storedPermRefTag((ILastPermRefsTag *)GetExecutionContextSession()->QueryWorkspace(IID_ILASTPERMREFSTAG));
		if(storedPermRefTag == nil)
			break;

		// Apply the new info
		storedPermRefTag->setLastReference(newPermRefTag->getLastReference());
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void PrsLastPremRefCmd::DoNotify()
{
	ErrorCode status = kFailure;
			
	do{
		InterfacePtr<ISubject> subject ((ISubject *)GetExecutionContextSession()->QueryWorkspace(IID_ISUBJECT));
		if(subject == nil){
			break;
		}
		// Notify the subject of the change
		subject->Change(kPrsLastPremRefCmdBoss, IID_ILASTPERMREFSTAG, this);

		status = kSuccess;
	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}


/* CreateName
*/
PMString * PrsLastPremRefCmd::CreateName()
{
	PMString * name = new PMString("Save Last PermRef");
	name->SetTranslatable(kFalse);
	return name;
}

/*
	SetUpTarget
*/
void PrsLastPremRefCmd::SetUpTarget(){
	SetTarget(Command::kSessionWorkspaceTarget);
}

