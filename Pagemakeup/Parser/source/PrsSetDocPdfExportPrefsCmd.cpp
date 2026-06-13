/*
//
//	File:	PrsSetDocPdfExportPrefsCmd.cpp
//
//  Author: Trias
//
//	Date:	05-Jan-2006
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
#include "IStringData.h"

// Project includes
#include "PDFID.h"

#include "PrsID.h"

/** PrsSetDocPdfExportPrefsCmd
*/
class PrsSetDocPdfExportPrefsCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsSetDocPdfExportPrefsCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsSetDocPdfExportPrefsCmd();

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


CREATE_PMINTERFACE(PrsSetDocPdfExportPrefsCmd, kPrsSetDocPdfExportPrefsCmdImpl)

/* Constructor
*/
PrsSetDocPdfExportPrefsCmd::PrsSetDocPdfExportPrefsCmd(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
PrsSetDocPdfExportPrefsCmd::~PrsSetDocPdfExportPrefsCmd()
{
}

/* Do
*/
void PrsSetDocPdfExportPrefsCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)
			break;

		// Get the command datas
		InterfacePtr<IStringData> newPDFStyle(this, IID_IPDFEXPORTPREFS);
		if (newPDFStyle == nil)
			break;
	
		InterfacePtr<IStringData> storedPDFStyle(fItemList.GetRef(0), IID_IPDFEXPORTPREFS);
		if (storedPDFStyle == nil)
			break;
	
		// Apply the new info
		storedPDFStyle->Set(newPDFStyle->Get());

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void PrsSetDocPdfExportPrefsCmd::DoNotify()
{
	ErrorCode status = kFailure;
			
	do
	{
		if (fItemList.Length() != 1)
			break;
		else
		{
			InterfacePtr<ISubject> subject ((ISubject *)GetExecutionContextSession()->QueryWorkspace(IID_ISUBJECT));
			if(subject == nil)
			{
				ASSERT_FAIL("PrsSetDocPdfExportPrefsCmd::DoNotify -> subject nil");
				break;
			}

			// Notify the subject of the change
			subject->Change(kPrsSetDocPdfExportPrefsCmdBoss, IID_IPDFEXPORTPREFS, this);

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
PMString * PrsSetDocPdfExportPrefsCmd::CreateName()
{
	PMString * name = new PMString("Save PDF Export Preferences");
	name->SetTranslatable(kFalse);
	return name;
}
