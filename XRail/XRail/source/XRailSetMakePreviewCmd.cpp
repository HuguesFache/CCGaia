/*
//
//	File:	XRailSetMakePreviewCmd.cpp
//
//  Author: Trias
//
//	Date:	05/06/2007
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2006 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"
#include "CAlert.h"

// Interface includes
#include "ISubject.h"
#include "IBoolData.h"

#include "XRLID.h"

/** XRailSetMakePreviewCmd
*/
class XRailSetMakePreviewCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRailSetMakePreviewCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XRailSetMakePreviewCmd();

	/**
		@return kFalse don't use this command if memory is low. 
	*/
	bool16 LowMemIsOK() const { return kFalse; }

protected:
	/**
		The Do method is where all the action happens. 
	*/
	void Do();

	void Undo();
		
	void Redo();
	
	void DoNotify();
		
	/**
		Gives the name that shows up under the Edit menu and allows the user to undo or redo the command.

		@return pointer to a PMString allocated using the new operator. Deleted by caller.
	*/
	PMString* CreateName();
	
private:

	bool16 oldMkPreview;

};


CREATE_PMINTERFACE(XRailSetMakePreviewCmd, kXRLSetMakePreviewCmdImpl)

/* Constructor
*/
XRailSetMakePreviewCmd::XRailSetMakePreviewCmd(IPMUnknown* boss) 
: Command(boss)
{
	oldMkPreview = kFalse;
}

/* Destructor
*/
XRailSetMakePreviewCmd::~XRailSetMakePreviewCmd()
{
}

/* Do
*/
void XRailSetMakePreviewCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)// Should have only one kDocBoss in the item list
			break;
		
		InterfacePtr<IBoolData> cmdData (this, IID_IBOOLDATA);
		
		InterfacePtr<IBoolData> mkPreview (fItemList.GetRef(0),  IID_IMAKEPREVIEW);
		
		// Save old data for undo
		oldMkPreview = mkPreview->Get();
		
		mkPreview->Set(cmdData->Get());
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* Undo
*/
void XRailSetMakePreviewCmd::Undo()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)// Should have only one kDocBoss in the item list
			break;
	
		InterfacePtr<IBoolData> mkPreview (fItemList.GetRef(0),  IID_IMAKEPREVIEW);
		mkPreview->Set(oldMkPreview);
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);
}


/* Redo
*/
void XRailSetMakePreviewCmd::Redo()
{
	Do();
}


/* DoNotify
*/
void XRailSetMakePreviewCmd::DoNotify()
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
			subject->Change(kXRLSetMakePreviewCmdBoss, IID_IMAKEPREVIEW, this);

			status = kSuccess;
		}
		
	} while(false);
}

/* CreateName
*/
PMString * XRailSetMakePreviewCmd::CreateName()
{
	PMString * name = new PMString("Set Make Preview");
	name->SetTranslatable(kFalse);
	return name;
}



