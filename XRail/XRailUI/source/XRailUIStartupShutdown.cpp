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

	virtual int32 GetIllicoFlag();


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
	InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());

	InterfacePtr<IXRailPrefsData>  xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));
	if (GetIllicoFlag() == 0) {
		// Open login dialog so that the user has to connect to XRail on startup directly
		InterfacePtr<ICommand> openLoginDialogCmd(CmdUtils::CreateCommand(kXRLUIOpenLoginDialogCmdBoss));

		CmdUtils::ScheduleCommand(openLoginDialogCmd, ICommand::kLowestPriority);
	}
}

/*
*/
void XRailUIStartupShutdown::Shutdown()
{
}

int32 XRailUIStartupShutdown::GetIllicoFlag()
{
	int32 illicoFlag = 0;

	// Read config file if are running under InDesign Server	
	ErrorCode status = kFailure;
	PMString ip, port;
	int32 fromServeur = 0;
	K2Vector<PMString> listeBases;
	K2Vector<PMString> IPBases;

	do {
		IDFile configFile;

		// GD 28/07/2022 ++
		IDFile approamingdatafolder, parent1, parent2, parent3, parent4, parent5;
		PMString message;
		PMString subFolderName; // on laisse volontairement vide.
		FileUtils::GetAppRoamingDataFolder(&approamingdatafolder, subFolderName);
		FileUtils::GetParentDirectory(approamingdatafolder, parent1);
		FileUtils::GetParentDirectory(parent1, parent2);
		FileUtils::GetParentDirectory(parent2, parent3);
		FileUtils::GetParentDirectory(parent3, parent4);
		FileUtils::GetParentDirectory(parent4, parent5);
		FileUtils::GetParentDirectory(parent5, configFile);
		FileUtils::AppendPath(&configFile, "Gaia.ini");

		if (!FileUtils::DoesFileExist(configFile)) {
			FileUtils::GetAppInstallationFolder(&configFile);
			FileUtils::AppendPath(&configFile, "Gaia.ini");
		}

		InterfacePtr<IPMStream> input(StreamUtil::CreateFileStreamRead(configFile));

		if (!input)
			break;

		uchar c;
		while (input->GetStreamState() == kStreamStateGood) {
			string line;
			while ((input->GetStreamState() == kStreamStateGood) && (input->XferByte(c) != '\n'))
			{
				if (c == '\r')
					break;

				//if(c != '\n')
				line += c;
			}
			PlatformChar equal;
			equal.Set('=');

			PMString str(line.c_str());
			if (!str.IsEmpty())
			{
				if (str[str.NumUTF16TextChars() - 1].IsChar('\r'))
					str.Remove(str.NumUTF16TextChars() - 1);

				int32 pos = str.IndexOfCharacter(equal);
				if (pos != -1)
				{
					PMString* keyFound = str.Substring(0, pos);
					if (keyFound)
					{
						keyFound->StripWhiteSpace();

						PMString* value = str.Substring(pos + 1);
						if (value)
						{
							if (!value->IsEmpty())
							{
								value->StripWhiteSpace(PMString::kLeadingAndTrailingWhiteSpace);

								if (keyFound->Compare(kFalse, "ILLICO") == 0)
									illicoFlag = Utils<IDataStringUtils>()->StringToInt32(WideString(*value));
							}

							delete value;
						}
						delete keyFound;
					}
				}
			}
		}

		input->Close();

	} while (kFalse);



	return illicoFlag;
}