 /*
//
//	File:	PrsSetImageInfosCmd.cpp
//
//  Author: Trias
//
//	Date:	01-fev-2006
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2006 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"

// Interface includes
#include "ISubject.h"
#include "IPMPointData.h"
#include "IStringData.h"

// Project includes
#include "PrsID.h"

/** PrsSetImageInfosCmd
	The implementation of the PrsSetImageInfosCmd, it gets the command data from its sibling interface, 
	a non persistent IPageMakeUpPrefs, then use it to set the persist IPageMakeUpPrefs on a document, passed in the
	item list of the command
*/
class PrsSetImageInfosCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsSetImageInfosCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsSetImageInfosCmd();

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

private:

};

CREATE_PMINTERFACE(PrsSetImageInfosCmd, kPrsSetImageInfosCmdImpl)

/* Constructor
*/
PrsSetImageInfosCmd::PrsSetImageInfosCmd(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
PrsSetImageInfosCmd::~PrsSetImageInfosCmd()
{
}

/* Do
*/
void PrsSetImageInfosCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)
			break;

		InterfacePtr<IPMPointData> storedScale(fItemList.GetRef(0), IID_IIMAGESCALE);
		InterfacePtr<IPMPointData> storedShift(fItemList.GetRef(0), IID_IIMAGESHIFT);
		InterfacePtr<IStringData> storedPath(fItemList.GetRef(0), IID_IIMAGEPATH);

		// Get the command datas
		InterfacePtr<IPMPointData> newScale(this, IID_IIMAGESCALE);
		InterfacePtr<IPMPointData> newShift(this, IID_IIMAGESHIFT);
		InterfacePtr<IStringData> newPath(this, IID_IIMAGEPATH);
		
		storedScale->Set(newScale->GetPMPoint());
		storedShift->Set(newShift->GetPMPoint());
		storedPath->Set(newPath->GetString());

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void PrsSetImageInfosCmd::DoNotify()
{
	ErrorCode status = kFailure;
			
	do
	{
		if (fItemList.Length() != 1)
			break;
		else
		{
			InterfacePtr<ISubject> subject (fItemList.GetRef(0),UseDefaultIID());
			if(subject == nil)
			{
				ASSERT_FAIL("PrsSetImageInfosCmd::DoNotify -> subject nil");
				break;
			}

			// Notify the subject of the change
			subject->Change(kPrsSetImageInfosCmdBoss, IID_IIMAGESCALE, this);
			subject->Change(kPrsSetImageInfosCmdBoss, IID_IIMAGESHIFT, this);
			subject->Change(kPrsSetImageInfosCmdBoss, IID_IIMAGEPATH, this);

			status = kSuccess;
		}
	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}


/* CreateName
*/
PMString * PrsSetImageInfosCmd::CreateName()
{
	PMString * name = new PMString("Save Image Info");
	name->SetTranslatable(kFalse);
	return name;
}


