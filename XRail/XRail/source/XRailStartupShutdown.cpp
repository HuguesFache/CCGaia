/*
//
//    Author: GD
//
//    Date: 2004
//
*/

#include "VCPluginHeaders.h"

// Interface includes:
#include "IStartupShutdownService.h"
#include "ISession.h"
#include "IPlugInList.h"
#include "IStringData.h"
#include "IIntData.h"
#include "IWebServices.h"
#include "IPMStream.h"
#include "IDataStringUtils.h"
#include "IXRailPrefsData.h"
#include "IWorkspace.h"
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
#include "XRLID.h"
#include "XRCID.h"


/** Implements IStartupShutdownService; purpose is to install the idle task.
*/
class XRailStartupShutdown : public CPMUnknown<IStartupShutdownService>
{
public:


    /**    Constructor
        @param boss
     */
    XRailStartupShutdown(IPMUnknown* boss);

    /**    Destructor
     */
    virtual ~XRailStartupShutdown() {}

    /**    Called by the core when app is starting up
     */
    virtual void Startup();

    /**    Called by the core when app is shutting down
     */
    virtual void Shutdown();

private:

    void ReadLine(std::string& line, PMString& urlXR, int32& fromServeur, K2Vector<PMString>& listeBases, K2Vector<PMString>& IPBases);
};

CREATE_PMINTERFACE(XRailStartupShutdown, kXRLStartupShutdownImpl)

/*
*/
XRailStartupShutdown::XRailStartupShutdown(IPMUnknown* boss) : CPMUnknown<IStartupShutdownService>(boss)
{
}

/*
*/
void XRailStartupShutdown::Startup()
{
    // Read config file if are running under InDesign Server
    ErrorCode status = kFailure;
    PMString urlXR;
    int32 fromServeur = 0;
    K2Vector<PMString> listeBases;
    K2Vector<PMString> IPBases;

    do {
        if (LocaleSetting::GetLocale().GetProductFS() == kInDesignServerProductFS) {
            // Display startup message in server version only
            CAlert::InformationAlert("Startup XRail plugin...\r\n");
        }

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
            ReadLine(line, urlXR, fromServeur, listeBases, IPBases);
        }

        input->Close();
        status = kSuccess;

    } while (kFalse);

    if (status == kSuccess) {
        PMString oldUrlXR, user, baseName;
        bool16 modeRobot = kFalse;
        K2Vector<PMString> oldlisteBases;
        K2Vector<PMString> oldIPBases;
        GetPrefs(oldUrlXR, user, modeRobot, oldlisteBases, oldIPBases);
        if (LocaleSetting::GetLocale().GetProductFS() == kInDesignServerProductFS)
            modeRobot = kTrue;

        // Set preferences, if they've changed
        SetPrefs(urlXR, user, modeRobot, listeBases, IPBases);

        // Persist the SERVEUR identifier on the session boss so the login
        // dialog can re-fetch server prefs after a successful VerifLogin.
        // Stored on the XRail side (not via IXPGPreferences) so XRail-only
        // installations — where the XPage plugin isn't loaded — still work.
        InterfacePtr<IIntData> fromServeurData(GetExecutionContextSession(), IID_IXRAILFROMSERVEUR);
        if (fromServeurData)
            fromServeurData->Set(fromServeur);

        PMString serverAddress = GetGoodUrlXR(baseName);
        InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

        // Best-effort prefs fetch at startup: if the server is offline (laptop
        // offline, VPN down, maintenance) we keep whatever prefs are already
        // persisted in the workspace and rely on the login dialog to re-fetch
        // once the user signs in. No alert surfaced — it would fire on every
        // cold start when offline.
        int32 prefs_ImportPub = 0, prefs_PubProp = 0, prefs_PDFMultipages = 1, prefs_ExportIDML = 0, prefs_MakePreview = 1;
        PMString prefs_AdTemplateNoVisual, prefs_AdTemplateWithVisual;
        if (xrailHTTP->GetPrefsXRail_v2(serverAddress, fromServeur, prefs_ImportPub, prefs_PubProp, prefs_PDFMultipages, prefs_ExportIDML, prefs_MakePreview, prefs_AdTemplateNoVisual, prefs_AdTemplateWithVisual))
        {
            InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
            InterfacePtr<IXRailPrefsData>  xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));

            xrailPrefsData->SetImportPub(prefs_ImportPub);
            xrailPrefsData->SetPubProp(prefs_PubProp);
            xrailPrefsData->SetPDFMultipages(prefs_PDFMultipages);
            xrailPrefsData->SetExportIDML(prefs_ExportIDML);
            xrailPrefsData->SetMakePreview(prefs_MakePreview);
            xrailPrefsData->SetAdTemplateNoVisual(prefs_AdTemplateNoVisual);
            xrailPrefsData->SetAdTemplateWithVisual(prefs_AdTemplateWithVisual);
        }
    }
    else if (LocaleSetting::GetLocale().GetProductFS() == kInDesignServerProductFS)
        CAlert::ErrorAlert(PMString(kXRLErrorMissingConfigFileKey));
}

/*
*/
void XRailStartupShutdown::Shutdown()
{
}


void XRailStartupShutdown::ReadLine(std::string& line, PMString& urlXR, int32& fromServeur, K2Vector<PMString>& listeBases, K2Vector<PMString>& IPBases)
{
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

                        if (keyFound->Compare(kFalse, "URLXR") == 0)
                            urlXR = *value;
                        else if (keyFound->Compare(kFalse, "SERVEUR") == 0)
                            fromServeur = Utils<IDataStringUtils>()->StringToInt32(WideString(*value));
#if MULTIBASES == 1
                        else if (keyFound->Compare(kFalse, "nomBase") == 0)
                            listeBases.push_back(*value);
                        else if (keyFound->Compare(kFalse, "URLBase") == 0)
                            IPBases.push_back(*value);
#endif
                    }

                    delete value;
                }
                delete keyFound;
            }
        }
    }
}
