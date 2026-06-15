/*
//
//	Author: Trias
//
//	Date: 2006
//
*/

#include "VCPluginHeaders.h"

// Interface includes:
#include "IStartupShutdownService.h"
#include "ISession.h"
#include "IIdleTask.h"
#include "IXRailPrefsData.h"
#include "IWorkspace.h"
#include "IStringData.h"
#include "IDataStringUtils.h"

// General includes:
#include "CAlert.h"
#include "LocaleSetting.h"
#include "FileUtils.h"
#include "DocUtils.h"
#include "StreamUtil.h"
#include "Utils.h"
#include "IIdleTask.h"
#include <string>
#include <fstream>

using namespace std;

// Project includes:
#include "XRLUIID.h"
#include "XRLID.h"

/** Implements IStartupShutdownService; purpose is to install the idle task.
*/
class XRailUIStartupShutdown : public CPMUnknown<IStartupShutdownService>
{
public:


	/**	Constructor
		@param boss
	 */
	XRailUIStartupShutdown(IPMUnknown* boss);

	/**	Destructor
	 */
	virtual ~XRailUIStartupShutdown() {}

	/**	Called by the core when app is starting up
	 */
	virtual void Startup();

	/**	Called by the core when app is shutting down
	 */
	virtual void Shutdown();

};

CREATE_PMINTERFACE(XRailUIStartupShutdown, kXRLUIUIStartupShutdownImpl)

/*
*/
XRailUIStartupShutdown::XRailUIStartupShutdown(IPMUnknown* boss) : CPMUnknown<IStartupShutdownService>(boss)
{

}

/*
*/
void XRailUIStartupShutdown::Startup()
{
	// Sequence de connexion au demarrage, differee hors du chemin de chargement :
	// l'autolog "Se souvenir de moi" fait un appel reseau, et l'executer inline
	// ici bloquerait le demarrage / le chargement du plugin Illico. La commande
	// tente d'abord l'autolog silencieux, puis n'ouvre le dialogue de login
	// qu'en cas d'echec ET si ILLICO==0 (en ILLICO==1, jamais de modal au
	// demarrage). Voir XRailOpenLoginDialogCmd::Do.
	InterfacePtr<ICommand> openLoginDialogCmd(CmdUtils::CreateCommand(kXRLUIOpenLoginDialogCmdBoss));
	CmdUtils::ScheduleCommand(openLoginDialogCmd, ICommand::kLowestPriority);
}

/*
*/
void XRailUIStartupShutdown::Shutdown()
{
}