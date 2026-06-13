/*
//
//	File:	PrsSetDocEpsExportPrefsCmd.cpp
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
#include "IEPSExportPreferences.h"
#include "IEpsDocumentName.h"

/** PrsSetDocEpsExportPrefsCmd
*/
class PrsSetDocEpsExportPrefsCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsSetDocEpsExportPrefsCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsSetDocEpsExportPrefsCmd();

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


CREATE_PMINTERFACE(PrsSetDocEpsExportPrefsCmd, kPrsSetDocEpsExportPrefsCmdImpl)

/* Constructor
*/
PrsSetDocEpsExportPrefsCmd::PrsSetDocEpsExportPrefsCmd(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
PrsSetDocEpsExportPrefsCmd::~PrsSetDocEpsExportPrefsCmd()
{
}

/* Do
*/
void PrsSetDocEpsExportPrefsCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)
			break;

		// Get the command datas
		InterfacePtr<IEpsDocumentName> newEpsDocName(this, UseDefaultIID());
		if (newEpsDocName == nil)
			break;

		InterfacePtr<IEPSExportPreferences> newEpsExportPrefs(this, UseDefaultIID());
		if (newEpsExportPrefs == nil)
			break;
	
		InterfacePtr<IEpsDocumentName> storedEpsDocName(fItemList.GetRef(0), UseDefaultIID());
		if (storedEpsDocName == nil)
			break;

		InterfacePtr<IEPSExportPreferences> storedEpsExportPrefs(fItemList.GetRef(0), UseDefaultIID());
		if (storedEpsExportPrefs == nil)
			break;

		// Apply the new info
		storedEpsDocName->setEpsFileName(newEpsDocName->getEpsFileName());
		storedEpsDocName->setEpsFileExt(newEpsDocName->getEpsFileExt());
		
		storedEpsExportPrefs->SetEPSExPageOption(newEpsExportPrefs->GetEPSExPageOption());
		storedEpsExportPrefs->SetEPSExReaderSpread(newEpsExportPrefs->GetEPSExReaderSpread());
		storedEpsExportPrefs->SetEPSExPSLevel(newEpsExportPrefs->GetEPSExPSLevel());
		storedEpsExportPrefs->SetEPSExBleedOnOff(newEpsExportPrefs->GetEPSExBleedOnOff());
		storedEpsExportPrefs->SetEPSExColorSpace(newEpsExportPrefs->GetEPSExColorSpace());
		storedEpsExportPrefs->SetEPSExPreview(newEpsExportPrefs->GetEPSExPreview());
		storedEpsExportPrefs->SetEPSExDataFormat(newEpsExportPrefs->GetEPSExDataFormat());
		storedEpsExportPrefs->SetEPSExBitmapSampling(newEpsExportPrefs->GetEPSExBitmapSampling());
		storedEpsExportPrefs->SetEPSExIncludeFonts(newEpsExportPrefs->GetEPSExIncludeFonts());
		storedEpsExportPrefs->SetEPSExOPIReplace(newEpsExportPrefs->GetEPSExOPIReplace());
		
		storedEpsExportPrefs->SetEPSExAllPageMarks(newEpsExportPrefs->GetEPSExAllPageMarks());
		storedEpsExportPrefs->SetEPSExCropMarks(newEpsExportPrefs->GetEPSExCropMarks());
		storedEpsExportPrefs->SetEPSExBleedMarks(newEpsExportPrefs->GetEPSExBleedMarks());
		storedEpsExportPrefs->SetEPSExPageInfo(newEpsExportPrefs->GetEPSExPageInfo());
		storedEpsExportPrefs->SetEPSExRegMarks(newEpsExportPrefs->GetEPSExRegMarks());
		storedEpsExportPrefs->SetEPSExColorBars(newEpsExportPrefs->GetEPSExColorBars());

		storedEpsExportPrefs->SetEPSExOmitPDF(newEpsExportPrefs->GetEPSExOmitPDF());
		storedEpsExportPrefs->SetEPSExOmitEPS(newEpsExportPrefs->GetEPSExOmitEPS());
		storedEpsExportPrefs->SetEPSExOmitBitmapImages(newEpsExportPrefs->GetEPSExOmitBitmapImages());
		storedEpsExportPrefs->SetEPSExSpotOverPrint(newEpsExportPrefs->GetEPSExSpotOverPrint());
		storedEpsExportPrefs->SetEPSExIgnoreFlattenerSpreadOverrides(newEpsExportPrefs->GetEPSExIgnoreFlattenerSpreadOverrides());
		storedEpsExportPrefs->SetEPSExFlattenerStyle(newEpsExportPrefs->GetEPSExFlattenerStyle());
		storedEpsExportPrefs->SetEPSExBleedTop(newEpsExportPrefs->GetEPSExBleedTop());
		storedEpsExportPrefs->SetEPSExBleedBottom(newEpsExportPrefs->GetEPSExBleedBottom());
		storedEpsExportPrefs->SetEPSExBleedInside(newEpsExportPrefs->GetEPSExBleedInside());
		storedEpsExportPrefs->SetEPSExBleedOutside(newEpsExportPrefs->GetEPSExBleedOutside());
		storedEpsExportPrefs->SetEPSExPageRange(newEpsExportPrefs->GetEPSExPageRange());

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void PrsSetDocEpsExportPrefsCmd::DoNotify()
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
				ASSERT_FAIL("PrsSetDocEpsExportPrefsCmd::DoNotify -> subject nil");
				break;
			}

			// Notify the subject of the change
			subject->Change(kPrsSetDocEpsExportPrefsCmdBoss, IID_IEPSDOCUMENTNAME, this);
			subject->Change(kPrsSetDocEpsExportPrefsCmdBoss, IID_IEPSEXPORTPREFERENCES, this);

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
PMString * PrsSetDocEpsExportPrefsCmd::CreateName()
{
	PMString * name = new PMString("Save EPS Export Preferences");
	name->SetTranslatable(kFalse);
	return name;
}
