/*
//
//	File:	XPGSetStringCmd.cpp
//
//  Author: Trias Developpement
//
//	Date:	28-mars-2008
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2008 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"

// Interface includes
#include "ISubject.h"
#include "IStringData.h"
#include "IIntData.h"

// Project includes
#include "XPGID.h"

/** XPGSetStringCmd
*/
class XPGSetStringCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGSetStringCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XPGSetStringCmd();
	
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

private :

};


CREATE_PMINTERFACE(XPGSetStringCmd, kXPGSetStringCmdImpl)

/* Constructor
*/
XPGSetStringCmd::XPGSetStringCmd(IPMUnknown* boss) 
: Command(boss)
{
	SetUndoability(kAutoUndo);
}

/* Destructor
*/
XPGSetStringCmd::~XPGSetStringCmd()
{
}

/* Do
*/
void XPGSetStringCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		int32 length = fItemList.Length();
		if(length != 1)
			break;

		// Get string data to set
		InterfacePtr<IStringData> cmdData (this, UseDefaultIID());
		PMString theString = cmdData->Get();

		// Get targeted interface's ID to apply
		InterfacePtr<IIntData> interfaceData (this, UseDefaultIID());
		PMIID theInterface = interfaceData->Get();
		
	
		InterfacePtr<IStringData> stringData (fItemList.GetRef(0), theInterface);
		if(stringData == nil)
			break;
		
		// Apply string
		stringData->Set(theString);		

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void XPGSetStringCmd::DoNotify()
{
	ErrorCode status = kFailure;
			
	do
	{

		int32 length = fItemList.Length();
		if(length != 1)
			break;

			// Get targeted interface's ID to apply
		InterfacePtr<IIntData> interfaceData (this, UseDefaultIID());
		PMIID theInterface = interfaceData->Get();
		
		// Notify the subject of the change
		InterfacePtr<ISubject> subject (fItemList.GetRef(0), UseDefaultIID());
		subject->Change(kXPGSetStringCmdBoss, theInterface, this);
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}


/* CreateName
*/
PMString * XPGSetStringCmd::CreateName()
{
	PMString * name = new PMString("Set String Data");
	name->SetTranslatable(kFalse);
	return name;
}
