/*
//
//	File:	XRailSetResaPubListCmd.cpp
//
//  Author: Trias
//
//	Date:	03/03/2008
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2008 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"
#include "CAlert.h"

// Interface includes
#include "ISubject.h"
#include "IResaPubList.h"

#include "XRLID.h"

/** XRailSetResaPubListCmd
*/
class XRailSetResaPubListCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRailSetResaPubListCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XRailSetResaPubListCmd();

	/**
		@return kFalse don't use this command if memory is low. 
	*/
	bool16 LowMemIsOK() const { return kFalse; }

protected:
	/**
		The Do method is where all the action happens. 
	*/
	void Do();

	void DoNotify();
		
	/**
		Gives the name that shows up under the Edit menu and allows the user to undo or redo the command.

		@return pointer to a PMString allocated using the new operator. Deleted by caller.
	*/
	PMString* CreateName();
	
private:

	K2Vector<ResaPub> oldList;

};


CREATE_PMINTERFACE(XRailSetResaPubListCmd, kXRLSetResaPubListCmdImpl)

/* Constructor
*/
XRailSetResaPubListCmd::XRailSetResaPubListCmd(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
XRailSetResaPubListCmd::~XRailSetResaPubListCmd()
{
}

/* Do
*/
void XRailSetResaPubListCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)// Should have only one kDocBoss in the item list
			break;
		
		InterfacePtr<IResaPubList> cmdData (this, IID_IRESAPUBLIST);
		
		InterfacePtr<IResaPubList> resaPubList (fItemList.GetRef(0),  IID_IRESAPUBLIST);
		
		// Save old data for undo
		oldList = resaPubList->GetResaPubList();
		
		resaPubList->SetResaPubList(cmdData->GetResaPubList());
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}


/* DoNotify
*/
void XRailSetResaPubListCmd::DoNotify()
{
	ErrorCode status = kFailure;

	do
	{
		if (fItemList.Length() != 1)
			break;
		else
		{
			InterfacePtr<ISubject> subject (fItemList.GetRef(0),UseDefaultIID());
			
			// Notify the subject of the change
			subject->Change(kXRLSetResaPubListCmdBoss, IID_IRESAPUBLIST, this);

			status = kSuccess;
		}
		
	} while(false);
}

/* CreateName
*/
PMString * XRailSetResaPubListCmd::CreateName()
{
	PMString * name = new PMString("Set Resa Pub List");
	name->SetTranslatable(kFalse);
	return name;
}



