/*
//
//	File:	PrsSetPreferencesCmd.cpp
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
#include "IPageMakeUpPrefs.h"

/** PrsSetPreferencesCmd
	The implementation of the PrsSetPreferencesCmd, it gets the command data from its sibling interface, 
	a non persistent IPageMakeUpPrefs, then use it to set the persist IPageMakeUpPrefs on a document, passed in the
	item list of the command
*/
class PrsSetPreferencesCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsSetPreferencesCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsSetPreferencesCmd();

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


CREATE_PMINTERFACE(PrsSetPreferencesCmd, kPrsSetPreferencesCmdImpl)

/* Constructor
*/
PrsSetPreferencesCmd::PrsSetPreferencesCmd(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
PrsSetPreferencesCmd::~PrsSetPreferencesCmd()
{
}

/* Do
*/
void PrsSetPreferencesCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)
			break;

		// Get the command datas
		InterfacePtr<IPageMakeUpPrefs> newPreferences(this, UseDefaultIID());
		if (newPreferences == nil)
		{
			ASSERT_FAIL("PrsSetPreferencesCmd::Do -> newPreferences nil");
			break;
		}
	
		InterfacePtr<IPageMakeUpPrefs> storedPreferences(fItemList.GetRef(0), UseDefaultIID());
		if(storedPreferences == nil)
		{
			ASSERT_FAIL("PrsSetPreferencesCmd::Do -> storedPreferences nil");
			break;
		}

		// Change the preferences
		storedPreferences->CopyFrom(newPreferences);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void PrsSetPreferencesCmd::DoNotify()
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
				ASSERT_FAIL("PrsSetPreferencesCmd::DoNotify -> subject nil");
				break;
			}

			// Notify the subject of the change
			subject->Change(kPrsSetPreferencesCmdBoss, IID_IPAGEMAKEUPPREFS, this);

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
PMString * PrsSetPreferencesCmd::CreateName()
{
	PMString * name = new PMString("Save Preferences");
	name->SetTranslatable(kFalse);
	return name;
}


