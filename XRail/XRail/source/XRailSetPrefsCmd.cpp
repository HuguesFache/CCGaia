
#include "VCPlugInHeaders.h"

// Interface includes
#include "ISubject.h"
#include "ISession.h"
#include "IWorkspace.h"

// General includes
#include "ErrorUtils.h"
#include "PreferenceUtils.h"
#include "Command.h"

// SnapSetPrefsCmd includes
#include "IXRailPrefsData.h"
#include "XRLID.h"

/**	SnapSetPrefsCmd
*/
class XRailSetPrefsCmd : public Command
{
	public:
		/**
			Constructor.
		*/
		XRailSetPrefsCmd(IPMUnknown* boss);

	protected:
		void Do();
		PMString* CreateName();

	private:
		
	PMString urlXR, urlTEC;
		PMString lastUser;
		bool16 modeRobot;
#if MULTIBASES == 1
		K2Vector<PMString> listeBases;
		K2Vector<PMString> IPBases;
#endif
};


/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its 
 ImplementationID making the C++ code callable by the 
 application.
*/
CREATE_PMINTERFACE(XRailSetPrefsCmd, kXRLSetPrefsCmdImpl)


/* Constructor.
*/
XRailSetPrefsCmd::XRailSetPrefsCmd(IPMUnknown* boss) :Command(boss)
{
	SetTarget(kSessionWorkspaceTarget);
	SetUndoability(ICommand::kAutoUndo);
}

/* Do
*/
void XRailSetPrefsCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		// Acquire the new preference data
		InterfacePtr<IXRailPrefsData> newData(this, UseDefaultIID());
		
		// Acquire the current preference data and save its state	
		InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
		
		InterfacePtr<IXRailPrefsData> xrailPrefsData(workspace, UseDefaultIID());
		if (xrailPrefsData == nil)
		
		// Save for undo
		urlXR = xrailPrefsData->GeturlXR();
		lastUser = xrailPrefsData->GetLastUser();
		modeRobot = xrailPrefsData->GetModeRobot();
#if MULTIBASES == 1
		listeBases = xrailPrefsData->GetListeBases();
		IPBases = xrailPrefsData->GetIPBases();
#endif
		
   		// Update state
		xrailPrefsData->SeturlXR(newData->GeturlXR());
   		xrailPrefsData->SetLastUser(newData->GetLastUser());
   		xrailPrefsData->SetModeRobot(newData->GetModeRobot());
#if MULTIBASES == 1
		xrailPrefsData->SetListeBases(newData->GetListeBases());
		xrailPrefsData->SetIPBases(newData->GetIPBases());
#endif
		
   		status = kSuccess;
   	
	}	while (false);

	// Handle any errors
	if ( status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(status);
	}
}


/* CreateName
*/
PMString* XRailSetPrefsCmd::CreateName()
{
	PMString* str = new PMString(kCommandXRailSetPrefsCmdKey);
	return str;
}


