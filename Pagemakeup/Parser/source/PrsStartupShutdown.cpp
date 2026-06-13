/*
//
//	Author: Trias
//
//	Date: 09-Dec-2005
//
*/

#include "VCPluginHeaders.h"

// Interface includes:
#include "IStartupShutdownService.h"
#include "IRegistration.h"
#include "ISerialNumber.h"
#include "LocaleSetting.h"

// General includes:
#include "CAlert.h"


// Project includes:
#include "PrsID.h"
#include "GlobalDefs.h"

#include "GlobalTime.h"

/** Implements IStartupShutdownService; purpose is to install the idle task.
*/
class PrsStartupShutdown : public CPMUnknown<IStartupShutdownService>
{
public:


	/**	Constructor
		@param boss 
	 */
    PrsStartupShutdown(IPMUnknown* boss );

	/**	Destructor
	 */
	virtual ~PrsStartupShutdown() {}

	/**	Called by the core when app is starting up
	 */
    virtual void Startup();

	/**	Called by the core when app is shutting down
	 */
    virtual void Shutdown();

private:

	/** Check registration information stored in kSessionBoss
		@param OUT askForSerial : true if serial number has never been entered, false otherwise
		@return : true if product has been registered, false if it hasn't or if license is not valid
	*/
	bool16 CheckRegistration(bool16& askForSerial);

	bool16 CheckMaxUsers(int16 maxUsers);

	/**
		In case of a one month demo license, check whether it has expired or not
		@return : true if not expired, false otherwise
	*/
	bool16 TimeBombed();

};

CREATE_PMINTERFACE(PrsStartupShutdown, kPrsStartupShutdownImpl)


/*
*/
PrsStartupShutdown::PrsStartupShutdown(IPMUnknown *boss) : CPMUnknown<IStartupShutdownService>(boss)
{
	
}

/*
*/
void PrsStartupShutdown::Startup()
{	

#if DEMO 
	if(!TimeBombed())
	{
		InterfacePtr<ICommand> QuitCmd (CmdUtils::CreateCommand(kQuitCmdBoss));
		CmdUtils::ScheduleCommand(QuitCmd);
		return;
	}
#elif SERVER

	if(LocaleSetting::GetLocale().GetProductFS() == kInDesignServerProductFS) {
		// Display startup message in server version only
		PMString message;
		message.Append("Startup PageMakeup plugin ");
		message.Append(kPrsVersion);
		message.Append("...\r\n");
		CAlert::InformationAlert(message);
	}
	
	
	/*const PMString serialToCheck = "11551009497975562932";
	
	InterfacePtr<ISerialNumber> idSerial (GetExecutionContextSession(), IID_ISERIALNUMBER );
	CAlert::InformationAlert("Serial InDesign : *" + idSerial->GetSerialNumber() + "*");
	if(idSerial && idSerial->GetSerialNumber().Compare(kTrue, serialToCheck) != 0)
	{
		CAlert::ErrorAlert(PMString (kErrInvalidIDSerial));
		InterfacePtr<ICommand> QuitCmd (CmdUtils::CreateCommand(kQuitCmdBoss));
		CmdUtils::ScheduleCommand(QuitCmd);
		return;
	}*/

#else
/*	
	bool16 askForSerial = kFalse;
	if(!CheckRegistration(askForSerial))
	{
		if(askForSerial)
		{
			InterfacePtr<ICommand> enterSerialCmd (CmdUtils::CreateCommand(kPrsEnterSerialNumberCmdBoss));
			CmdUtils::ScheduleCommand(enterSerialCmd);
		}
		else
		{
			InterfacePtr<ICommand> QuitCmd (CmdUtils::CreateCommand(kQuitCmdBoss));
			CmdUtils::ScheduleCommand(QuitCmd);
		}
	}	
*/
#endif
	
}

void PrsStartupShutdown::Shutdown()
{
}

bool16 PrsStartupShutdown::CheckRegistration(bool16& askForSerial)
{
	bool16 result = kFalse;

	InterfacePtr<IRegistration> registrationInfo ((IRegistration *)GetExecutionContextSession()->QueryWorkspace(IID_IREGISTRATION));

	switch(registrationInfo->getStatus()) {
	
		case ID_UNREGISTERED:
			askForSerial = kTrue;
			result = kFalse;
			break;

		case ID_REGISTERED:
			{
				askForSerial = kFalse;
				result = kTrue;
				bool16 resetRegistered = kFalse;
				if(CheckMaxUsers(registrationInfo->getMaxUsers()))
				{
					InterfacePtr<ISerialNumber> idSerial (GetExecutionContextSession(), UseDefaultIID());
					if(idSerial && registrationInfo->getIDSerialNumber().Compare(kTrue,idSerial->GetSerialNumber()) == 0)
						result = kTrue;
					else 
					{
						PMString error(kErrInvalidIDSerial);
						CAlert::InformationAlert(error);
						
						// Set to unregistered
						askForSerial = kFalse;
						result = kFalse;

						resetRegistered = kTrue;
					}
											
					if(registrationInfo->getTimeBombed())
					{
						if(!TimeBombed()) 
						{
							askForSerial = kFalse;
							result = kFalse;
							
							resetRegistered = kTrue;
						}
					}
					if(registrationInfo->getVersion() < ID_CURRENTVERSION) 
					{
						// Big change in version
						PMString error(kErrCorruptedLicense);
						CAlert::InformationAlert(error);
						result = kFalse;
						askForSerial = kFalse;
						
						resetRegistered = kTrue;
					}
						
				} 
				else 
				{
						PMString error(kErrTooManyCopies);
						CAlert::InformationAlert(error);
						result = kFalse;
						askForSerial = kFalse;
						resetRegistered = kTrue;
				}

				if(resetRegistered)
				{
					// Everything is OK, save registration info
					InterfacePtr<ICommand> setRegistrationInfoCmd (CmdUtils::CreateCommand(kPrsSetRegistrationInfoCmdBoss));
					InterfacePtr<IRegistration> cmdData (setRegistrationInfoCmd, UseDefaultIID());
					cmdData->setStatus(ID_UNREGISTERED); // Will reset all other attributes
					CmdUtils::ProcessCommand(setRegistrationInfoCmd);
				}
			}
			break;

		default : 
			result = kFalse;
			askForSerial = kFalse;
			break;
	}

	return result;
}

bool16	PrsStartupShutdown::CheckMaxUsers(int16 maxUsers)
{
	int16		currentUsers = 1;

	if(currentUsers > maxUsers)
		return kFalse;
	
	return kTrue;
}

bool16 PrsStartupShutdown::TimeBombed()
{
	const char dateCompil[12] = __DATE__;
	PMString monthStr;
	monthStr.Append(dateCompil[0]);
	monthStr.Append(dateCompil[1]);
	monthStr.Append(dateCompil[2]);

	PMString dayStr;
	dayStr.Append(dateCompil[4]);
	dayStr.Append(dateCompil[5]);

	PMString yearStr;
	yearStr.Append(dateCompil[7]);
	yearStr.Append(dateCompil[8]);
	yearStr.Append(dateCompil[9]);
	yearStr.Append(dateCompil[10]);

	// Convert string values to number values
	int32 month = 1;
	if(monthStr.Compare(kFalse,"Jan") == 0) month = 1;
	else if(monthStr.Compare(kFalse,"Feb") == 0) month = 2;
	else if(monthStr.Compare(kFalse,"Mar") == 0) month = 3;
	else if(monthStr.Compare(kFalse,"Apr") == 0) month = 4;
	else if(monthStr.Compare(kFalse,"May") == 0) month = 5;
	else if(monthStr.Compare(kFalse,"Jun") == 0) month = 6;
	else if(monthStr.Compare(kFalse,"Jul") == 0) month = 7;
	else if(monthStr.Compare(kFalse,"Aug") == 0) month = 8;
	else if(monthStr.Compare(kFalse,"Sep") == 0) month = 9; 
	else if(monthStr.Compare(kFalse,"Oct") == 0) month = 10;
	else if(monthStr.Compare(kFalse,"Nov") == 0) month = 11;
	else if(monthStr.Compare(kFalse,"Dec") == 0) month = 12;

	int32 day = dayStr.GetAsNumber();
	int32 year = yearStr.GetAsNumber();
	
	month = (month + 2); // 2 months demo;
	if(month > 12) 
	{
		month = month - 12;
		year += 1;
	}

	int8 tmaxj[12] = {31,28,31,30,31,30,31,31,30,31,30,31 };
	if((year%4)==0) tmaxj[1]=29; 

	if(day > tmaxj[month-1])
		day = tmaxj[month-1];

	GlobalTime limit; 
	limit.SetTime(year,month,day,0,0,0);

	GlobalTime current;
	current.CurrentTime();

	bool16 result = kTrue;
	if(current > limit)
	{
		CAlert::InformationAlert(kPrsDemoExpired);		
		result = kFalse;
	}
	else 		
	{
		uint64 diff = (limit.GetTime() - current.GetTime()) / GlobalTime::kOneSecond;
		int32 nbDays = diff / (3600 * 24);

		if(nbDays < 6)
		{
			PMString info(kPrsDemoRemainingDays);
			info.Translate();
			info.AppendNumber(nbDays);
			info.SetTranslatable(kFalse);
			CAlert::InformationAlert(info);
		}
	}

	return result;
}