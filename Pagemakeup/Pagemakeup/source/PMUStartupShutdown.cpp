/*
//
//	Author: Trias
//
//	Date: 09-Fev-2006
//
*/

#include "VCPluginHeaders.h"

// Interface includes:
#include "IStartupShutdownService.h"
#include "ICommand.h"

// General includes:
#include "CAlert.h"


// Project includes:
#include "PMUID.h"
#include "GlobalDefs.h"

/** Implements IStartupShutdownService; purpose is to install the idle task.
*/
class PMUStartupShutdown : public CPMUnknown<IStartupShutdownService>
{
public:


	/**	Constructor
		@param boss 
	 */
    PMUStartupShutdown(IPMUnknown* boss );

	/**	Destructor
	 */
	virtual ~PMUStartupShutdown() {}

	/**	Called by the core when app is starting up
	 */
    virtual void Startup();

	/**	Called by the core when app is shutting down
	 */
    virtual void Shutdown();

};

CREATE_PMINTERFACE(PMUStartupShutdown, kPMUStartupShutdownImpl)


/*
*/
PMUStartupShutdown::PMUStartupShutdown(IPMUnknown *boss) : CPMUnknown<IStartupShutdownService>(boss)
{
	
}

/* Startup
*/
void PMUStartupShutdown::Startup()
{	
	InterfacePtr<ICommand> setUpComposerPaletteCmd (CmdUtils::CreateCommand(kPMUSetUpComposerPaletteCmdBoss));
	CmdUtils::ScheduleCommand(setUpComposerPaletteCmd, ICommand::kLowestPriority);
}

/* Shutdown
*/
void PMUStartupShutdown::Shutdown()
{
}

