/*
//
//	File:	XRailOpenLoginDialogCmd.cpp
//
//  Author: Trias
//
//	Date:	20/04/2006
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2006 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"
#include "DocUtils.h"
#include "FileUtils.h"
#include "StreamUtil.h"
#include "Utils.h"
#include <string>

// Interface includes
#include "ISession.h"
#include "ISubject.h"
#include "IDialog.h"
#include "IWorkspace.h"
#include "IXRailPrefsData.h"
#include "IWebServices.h"
#include "IUserInfoUtils.h"
#include "IStringData.h"
#include "IBoolData.h"
#include "IIntData.h"
#include "IDataStringUtils.h"

#include "XRLUIID.h"
#include "XRLID.h"
#include "XRCID.h"

using namespace std;

/* GetIllicoFlag
   Lit le flag ILLICO dans Gaia.ini. Migre depuis XRailUIStartupShutdown : seul
   ce chemin de connexion au demarrage en a besoin. ILLICO==1 => on n'ouvre
   jamais le dialogue de login au demarrage (il bloquerait le chargement du
   plugin Illico).
*/
static int32 GetIllicoFlag()
{
	int32 illicoFlag = 0;

	do {
		IDFile configFile;

		// GD 28/07/2022 ++
		IDFile approamingdatafolder, parent1, parent2, parent3, parent4, parent5;
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

/* TryAutoLogin
   Connexion silencieuse au demarrage a partir des identifiants memorises
   ("Se souvenir de moi"). Replique le chemin "loginOK" de
   XRailLoginDialogController::ValidateDialogFields, mais sans aucune alerte ni
   dialogue. Renvoie kTrue uniquement si le serveur confirme la connexion.
*/
static bool16 TryAutoLogin()
{
	// Sans "Se souvenir de moi", pas d'autolog.
	bool16 rememberMe = kFalse;
	PMString thePassword("", PMString::kUnknownEncoding);
	GetRememberedCredentials(rememberMe, thePassword);
	if (!rememberMe)
		return kFalse;

	// Meme garde que le dialogue : un mot de passe anormalement long signale du
	// garbage lu en post-EOF dans les prefs. On refuse plutot que d'envoyer un
	// POST gigantesque.
	if (thePassword.NumUTF16TextChars() > 256)
		return kFalse;

	// Dernier utilisateur connu.
	PMString urlXR, theUser;
	bool16 modeRobot = kFalse;
	K2Vector<PMString> listeBases, IPBases;
	GetPrefs(urlXR, theUser, modeRobot, listeBases, IPBases);
	if (theUser.NumUTF16TextChars() == 0 || thePassword.NumUTF16TextChars() == 0)
		return kFalse;

	// Connexion a XRail.
	PMString baseName, result, userID("");
	int previewResult = 0, pdfResult = 0;
	PMString serverAddress = GetGoodUrlXR(baseName);

	InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
	bool16 connexionOK = xrailHTTP->VerifLogin_v2(serverAddress, theUser, thePassword, userID, result, pdfResult, previewResult);

	if (!connexionOK || result != "loginOK")
		return kFalse;

	// --- Connexion confirmee : on peuple la session (cf. dialog controller). ---
	Utils<IUserInfoUtils>()->SetInCopyUsername(theUser);

	InterfacePtr<IStringData> gLogin(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);
	gLogin->Set(userID);

	InterfacePtr<IStringData> gLoginString(GetExecutionContextSession(), IID_ILOGINSTRING);
	gLoginString->Set(theUser);

	InterfacePtr<IBoolData> createPreviewBool(GetExecutionContextSession(), IID_ICREATEPREVIEWBOOL);
	InterfacePtr<IBoolData> createPDFBool(GetExecutionContextSession(), IID_ICREATEPDFBOOL);
	if (previewResult == 0) createPreviewBool->Set(kFalse);
	else if (previewResult == 1) createPreviewBool->Set(kTrue);
	if (pdfResult == 0) createPDFBool->Set(kFalse);
	else if (pdfResult == 1) createPDFBool->Set(kTrue);

	// Rafraichir les prefs serveur (serveur joignable, VerifLogin vient de
	// reussir). Silencieux en cas d'echec.
	{
		int32 prefs_ImportPub = 0, prefs_PubProp = 0, prefs_PDFMultipages = 1, prefs_ExportIDML = 0, prefs_MakePreview = 1;
		PMString prefs_AdTemplateNoVisual, prefs_AdTemplateWithVisual;

		InterfacePtr<IIntData> fromServeurData(GetExecutionContextSession(), IID_IXRAILFROMSERVEUR);
		int32 fromServeurInt = fromServeurData ? fromServeurData->Get() : 0;

		if (xrailHTTP->GetPrefsXRail_v2(serverAddress, fromServeurInt,
			prefs_ImportPub, prefs_PubProp, prefs_PDFMultipages, prefs_ExportIDML, prefs_MakePreview,
			prefs_AdTemplateNoVisual, prefs_AdTemplateWithVisual))
		{
			InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
			InterfacePtr<IXRailPrefsData> xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));
			if (xrailPrefsData)
			{
				xrailPrefsData->SetImportPub(prefs_ImportPub);
				xrailPrefsData->SetPubProp(prefs_PubProp);
				xrailPrefsData->SetPDFMultipages(prefs_PDFMultipages);
				xrailPrefsData->SetExportIDML(prefs_ExportIDML);
				xrailPrefsData->SetMakePreview(prefs_MakePreview);
				xrailPrefsData->SetAdTemplateNoVisual(prefs_AdTemplateNoVisual);
				xrailPrefsData->SetAdTemplateWithVisual(prefs_AdTemplateWithVisual);
			}
		}
	}

	// Notifier les palettes abonnees (XPage Articles + Photos liees) pour
	// qu'elles passent du prompt "Se connecter" a leur contenu normal.
	InterfacePtr<ISubject> sessionSubject(GetExecutionContextSession(), UseDefaultIID());
	if (sessionSubject)
		sessionSubject->Change(kXRLUILoginChangedMsg, IID_IXRLUILOGINPROTOCOL);

	return kTrue;
}

/** XRailOpenLoginDialogCmd
*/
class XRailOpenLoginDialogCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRailOpenLoginDialogCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XRailOpenLoginDialogCmd();

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
		Set up target to session
	*/
	void SetUpTarget();

	/**
		Gives the name that shows up under the Edit menu and allows the user to undo or redo the command.

		@return pointer to a PMString allocated using the new operator. Deleted by caller.
	*/
	PMString* CreateName();

};


CREATE_PMINTERFACE(XRailOpenLoginDialogCmd, kXRLUIOpenLoginDialogCmdImpl)

/* Constructor
*/
XRailOpenLoginDialogCmd::XRailOpenLoginDialogCmd(IPMUnknown* boss) 
: Command(boss)
{
	SetUndoability(ICommand::kAutoUndo);
}

/* Destructor
*/
XRailOpenLoginDialogCmd::~XRailOpenLoginDialogCmd()
{
}

/* Do
*/
void XRailOpenLoginDialogCmd::Do()
{
	ErrorCode status = kSuccess;

	do
	{
		// 1. Connexion silencieuse via "Se souvenir de moi".
		if (TryAutoLogin())
			break;

		// 2. Echec (ou pas d'identifiants memorises) : on n'ouvre le dialogue
		//    QUE si ILLICO==0. En ILLICO==1, le dialogue modal bloquerait le
		//    chargement du plugin Illico : on reste non connecte silencieusement.
		if (GetIllicoFlag() != 0)
			break;

		IDialog * dialog = CreateNewDialog(kXRLUIPluginID, kXRLUILoginDialogResourceID);
		if(!dialog)
		{
			status = kFailure;
			break;
		}

		dialog->Open();

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* CreateName
*/
PMString * XRailOpenLoginDialogCmd::CreateName()
{
	PMString * name = new PMString("Open login dialog");
	name->SetTranslatable(kFalse);
	return name;
}

/*
	SetUpTarget
*/
void XRailOpenLoginDialogCmd::SetUpTarget()
{
	SetTarget(Command::kSessionTarget);
}



