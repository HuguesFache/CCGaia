/*
//	File:	PrsSerialNumberDialogController.cpp
//
//	Author:	Trias
//
//	Date:	09/12/2005
//
*/



#include "VCPlugInHeaders.h"
#include "GlobalDefs.h"

#if !SERVER

// Interface includes:
#include "IControlView.h"
#include "IPanelControlData.h"
#include "IRegistration.h"
#include "ISerialNumber.h"

// General includes:
#include "CDialogController.h"
#include "CAlert.h"


// Project includes:
#include "PrsID.h"


/** 
	Serial number dialog controller
*/
class PrsSerialNumberDialogController : public CDialogController
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PrsSerialNumberDialogController(IPMUnknown* boss) : CDialogController(boss) {}

		/**
			Initializes each widget in the dialog with its default value.
			Called when the dialog is opened.
			@param dlgContext
		*/
		virtual void InitializeDialogFields( IActiveContext* dlgContext);

		/**
			Validate the values in the widgets. 
			By default, the widget with ID kOKButtonWidgetID causes 
			this method to be called. When all widgets are valid, 
			ApplyFields will be called.		
			@param myContext
			@return kDefaultWidgetId if all widget values are valid, WidgetID of the widget to select otherwise.

		*/
		virtual WidgetID ValidateDialogFields( IActiveContext* myContext);

		/**
			Retrieve the values from the widgets and act on them.
			@param myContext
			@param widgetId identifies the widget on which to act.
		*/
		virtual void ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId);

		/**
			Reset all fields to default values
			@param myContext
		*/
		virtual void  ResetDialogFields (IActiveContext * myContext);

		/**
			User cancelled registration => quit InDesign
		*/
		virtual void UserCancelled();

	private :

		bool16 CheckLicenseKey();

		bool16 GetSerialNumber(PMString keyCode, int32 * theSerial);

		int32 GetUserCount(PMString keyCode);
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its 
 ImplementationID making the C++ code callable by the 
 application.
*/
CREATE_PMINTERFACE(PrsSerialNumberDialogController, kPrsSerialNumberDialogControllerImpl)

/* ApplyFields
*/
void PrsSerialNumberDialogController::InitializeDialogFields( IActiveContext* dlgContext) 
{
	do
	{
		SetTextControlData(kNumberPart1WidgetID, "");
		SetTextControlData(kNumberPart2WidgetID, "");
		SetTextControlData(kNumberPart3WidgetID, "");
		SetTextControlData(kNumberPart4WidgetID, "");
		SetTextControlData(kNumberPart5WidgetID, "");

	} while(kFalse);
}

/* ValidateFields
*/
WidgetID PrsSerialNumberDialogController::ValidateDialogFields( IActiveContext* myContext) 
{

	WidgetID result = kNoInvalidWidgets;

	if(!CheckLicenseKey())
		result = kNumberPart1WidgetID;

	return result;
}

/* ResetDialogFields
*/
void PrsSerialNumberDialogController::ResetDialogFields( IActiveContext* myContext) 
{
	InitializeDialogFields(myContext);
}

/* ApplyFields
*/
void PrsSerialNumberDialogController::ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId) 
{
	
}

/*
*/
void PrsSerialNumberDialogController::UserCancelled() 
{
	// User cancelled registration => quit InDesign
	InterfacePtr<ICommand> QuitCmd (CmdUtils::CreateCommand(kQuitCmdBoss));
	CmdUtils::ScheduleCommand(QuitCmd);
}

// We check the following characters :

// 0 1 2 3     4 5 6 7     8 9 A B     C D E F     G H I J 
// 0 0 C 1  -  0 A 0 B  -  0 0 0 0  -  0 0 5 M  -  _ _ _ _ 
//
//
// 	0	->		SERIAL_2
//	1	->		SERIAL_1
//	2	->		FIRST_KEY_CODE	always 'C'
//	3	->		SERIAL_5
//  -
//	4	->		SERIAL_3
//	5	->		SECOND_KEY_CODE	always 'Z'
//	6	->		SERIAL_4
//	7	->		PRODUCT_CODE:	'A' Composer 
//								'B' PageMakeUp
//	-
//	8	->		USER_1:			Number of Users (first digit)
//	9	->		Composer Tools  'Y' = Composer Tools
//	A	->		THIRD_KEY_CODE
//	B	->
//  -		
//	C	->		FOURTH_KEY_CODE
//	D	->		USER_2:			Number of Users (second digit)
//	E	->		IDVERSION: 		ID Version '2' = CS 2
//	F	->		MACWINFLAG: 	MAC/WINDOWS Flag. 'M' = Mac, 'W' = Windows

//  G	0x10	->	TIMEBOMBED	'j' Time Bombed 'w' Normal
//	H	0x11	->	PERMREFS	'3'	allow Permrefs
//	I	0x12	->	4DPAGELINK	'C' allow 4DPageLink
//	J	0x13	->	REVERSETCL	'h' allow Reverse TCL


bool16 PrsSerialNumberDialogController::CheckLicenseKey()
{
	// Get number entered by user
	PMString License;
	License += GetTextControlData(kNumberPart1WidgetID);
	License += GetTextControlData(kNumberPart2WidgetID);
	License += GetTextControlData(kNumberPart3WidgetID);
	License += GetTextControlData(kNumberPart4WidgetID);
	License += GetTextControlData(kNumberPart5WidgetID);

	int32 serialNumber = 0;
	if(!GetSerialNumber(License, &serialNumber))
		return kFalse;
	
	// For Composer we have 2 versions.
	bool16 composerTools = kFalse;
	
#if COMPOSER
	if(License[COMPOSERTOOL] == 'Y' || License[COMPOSERTOOL] == 'y')
		composerTools = kTrue;
	else
		composerTools = kFalse;
	
	if(!composerTools)
	{
		if((License[PRODUCT_CODE] != 'A') && (License[PRODUCT_CODE] != 'a'))
		return kFalse;
	}
	
#else
	if((License[PRODUCT_CODE] != 'B') && (License[PRODUCT_CODE] != 'b'))
		return kFalse;
#endif
		
	if((License[FIRST_KEY_CODE] != 'C') && (License[FIRST_KEY_CODE] != 'c'))
		return kFalse;

	if((License[SECOND_KEY_CODE] != 'Z') && (License[SECOND_KEY_CODE] != 'z'))
		return kFalse;
	
	int16 maxusers = GetUserCount(License);
	
	if(maxusers == 0)
		return kFalse;

	if(License[IDVERSION] != '2')
		return kFalse;

#ifdef WINDOWS
	if((License[MACWINFLAG] != 'W') && (License[MACWINFLAG] != 'w'))
		return kFalse;
#endif

#ifdef MACINTOSH
	if((License[MACWINFLAG] != 'M') && (License[MACWINFLAG] != 'm'))
		return kFalse;
#endif

	// Get Options
	bool16 pagelink;
	if((License[PAGELINK] == 'C') || (License[PAGELINK] == 'c')) 
		pagelink = kTrue;
	else
		pagelink = kFalse;
	
	bool16 reverse = kFalse;	
#if REVERSE
	if((License[REVERSETCL] == 'H') || (License[REVERSETCL] == 'h')) 
		reverse = kTrue;
	else
		return kFalse;
#endif	

	bool16 permrefs = kFalse;
#if PERMREFS
	if(License[PERMREFS_OPT] == '3') 
		permrefs = kTrue;
	else
		return kFalse;
#endif
	
	bool16 timebombed;
	if((License[TIMEBOMBED] == 'J') || (License[TIMEBOMBED] == 'j')) 
		timebombed = kTrue;
	else if((License[TIMEBOMBED] == 'W') || (License[TIMEBOMBED] == 'w')) 
		timebombed = kFalse;
	else
		return kFalse;

	// Everything is OK, save registration info
	InterfacePtr<ICommand> setRegistrationInfoCmd (CmdUtils::CreateCommand(kPrsSetRegistrationInfoCmdBoss));

	InterfacePtr<IRegistration> cmdData (setRegistrationInfoCmd, UseDefaultIID());

	// Save options
	cmdData->setPageLink(pagelink);
	cmdData->setReverse(reverse);
	cmdData->setPermRefs(permrefs);
	cmdData->setTimeBombed(timebombed);
	cmdData->setComposerTools(composerTools);

	// Set max users
	cmdData->setMaxUsers(maxusers);

	// Save InDesign SerialNumber
	InterfacePtr<ISerialNumber> idSerial (GetExecutionContextSession(), IID_ISERIALNUMBER);
	cmdData->setIDSerialNumber(idSerial->GetSerialNumber());

	// Update current version
	cmdData->setVersion(ID_CURRENTVERSION);

	cmdData->setSerialNumber(serialNumber);
	cmdData->setStatus(ID_REGISTERED);

	CmdUtils::ProcessCommand(setRegistrationInfoCmd);
	
	return kTrue;	
}

bool16 PrsSerialNumberDialogController::GetSerialNumber(PMString keyCode, int32 *theSerial)
{
	PMString serial;
	
	serial.Append(keyCode[SERIAL_1]);
	serial.Append(keyCode[SERIAL_2]);
	serial.Append(keyCode[SERIAL_3]);
	serial.Append(keyCode[SERIAL_4]);
	serial.Append(keyCode[SERIAL_5]);

	PMString::ConversionError convInfo = PMString::kNoError;
	*theSerial =  serial.GetAsNumber (&convInfo);

	if(convInfo != PMString::kNoError)
		return kFalse;
	
	if( ((*theSerial) % 7) != 0)
		return kFalse;

	return kTrue;
}

// ------------------------------------------------------------------
// Get the maximum allowed users.
// ------------------------------------------------------------------

int32 PrsSerialNumberDialogController::GetUserCount(PMString keyCode)
{
	PMString	numUser;
	int32		users;
	
	numUser.Append(keyCode[USER_1]);
	numUser.Append(keyCode[USER_2]);

	PMString::ConversionError convInfo = PMString::kNoError;
	users =  numUser.GetAsNumber(&convInfo);
	
	if(convInfo != PMString::kNoError)
		users = 0;

	return users;
}


#endif