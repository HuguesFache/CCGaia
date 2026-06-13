/*
//
//	Author: Trias
//
//	Date: 01-Mar-2006
//
*/

#include "VCPluginHeaders.h"

// Interface includes:
#include "IStartupShutdownService.h"
#include "IIdleTask.h"
#include "IBoolData.h"

// General includes:
#include "CAlert.h"
#include "LocaleSetting.h"

// Project includes:
#include "APLID.h"
#include "PrsID.h"

/** Implements IStartupShutdownService; purpose is to install the idle task.
*/
class APLStartupShutdown : public CPMUnknown<IStartupShutdownService>
{
public:


	/**	Constructor
		@param boss 
	 */
    APLStartupShutdown(IPMUnknown* boss );

	/**	Destructor
	 */
	virtual ~APLStartupShutdown() {}

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

CREATE_PMINTERFACE(APLStartupShutdown, kAPLStartupShutdownImpl)


/*
*/
APLStartupShutdown::APLStartupShutdown(IPMUnknown *boss) : CPMUnknown<IStartupShutdownService>(boss)
{
	
}

/*
*/
void APLStartupShutdown::Startup()
{	
	do 
	{
		if(LocaleSetting::GetLocale().GetProductFS() == kInDesignServerProductFS){
			// Display startup message in server version only
			CAlert::InformationAlert("Startup AutoPageLink plugin...\r\n");
		}

		/* TEMPORARY VERSION 1.2 */
		InterfacePtr<IBoolData> parserThreadRunning (GetExecutionContextSession(), IID_ITCLPARSER);
		if(!parserThreadRunning)
		{
			break;
		}
		parserThreadRunning->Set(kFalse);
	
		/* TEMPORARY VERSION 1.2 */
		InterfacePtr<IIdleTask> 
			task(GetExecutionContextSession(), IID_AUTOPAGELINKTASK);
		ASSERT(task);
		if(!task)
		{
			break;
		}
		task->InstallTask(0);
	} while(kFalse);
}

/* Shutdown
*/
void APLStartupShutdown::Shutdown()
{
	do 
	{
		InterfacePtr<IIdleTask> 
			task(GetExecutionContextSession(), IID_AUTOPAGELINKTASK);
		ASSERT(task);
		if(!task)
		{
			break;
		}
		task->UninstallTask();
	} while(kFalse);
}
