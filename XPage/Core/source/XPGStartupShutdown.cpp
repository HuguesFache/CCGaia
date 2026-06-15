/*
//
//	Author: Trias Developpement
//
//	Date: 22-Mar-2007
//
*/
#include "VCPluginHeaders.h"

// Interface includes:
#include "IStartupShutdownService.h"
#include "IDataStringUtils.h"
#include "ISerialNumber.h"
#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "ISAXServices.h"
#include "ISAXContentHandler.h"
#include "ISAXEntityResolver.h"
#include "ISAXParserOptions.h"
#include "IPMStream.h"
#include "IIntData.h"
#include "IWorkspace.h"
#include "IKeyValueData.h"

// General includes:
#include "CAlert.h"
#include "FileUtils.h"
#include "Utils.h"
#include "StreamUtil.h"
#include "ErrorUtils.h"
#include "StringUtils.h"
#include "ISysFileData.h"
#include "IObserver.h"
#include <fstream>

// Project includes:
#include "XPGID.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"

using namespace std;

/** Implements IStartupShutdownService; purpose is to install the idle task.
*/
class XPGStartupShutdown : public CPMUnknown<IStartupShutdownService>
{
public:
	/**	Constructor
		@param boss
	 */
	XPGStartupShutdown(IPMUnknown* boss);
	/**	Destructor
	 */
	virtual ~XPGStartupShutdown() {}
	/**	Called by the core when app is starting up
	 */
	virtual void Startup();
	/**	Called by the core when app is shutting down	 */
	virtual void Shutdown();

private:

	// Read configuration file data
	void ReadConfFile();

	ErrorCode ReadXMLExportFile(const IDFile& xmlExportFile);
	ErrorCode ReadXMLListTags(const IDFile& xmlTagsFile);

	PMString serveurPlugin = "0";

};

CREATE_PMINTERFACE(XPGStartupShutdown, kXPGStartupShutdownImpl)
XPGStartupShutdown::XPGStartupShutdown(IPMUnknown* boss) : CPMUnknown<IStartupShutdownService>(boss) {}

/*
*/
void XPGStartupShutdown::Startup()
{
	do {
		if (LocaleSetting::GetLocale().GetProductFS() == kInDesignServerProductFS) {
			// Display startup message in server version only
			CAlert::InformationAlert("Startup XPage plugin...\r\n");
		}

		// Initialize preferences
		ReadConfFile();

		// Init AdronmentShape
		InterfacePtr<IBoolData> isAdronmentShape(GetExecutionContextSession(), IID_IDISPLAYSHAPE);
		isAdronmentShape->Set(kFalse);

		// Init "Afficher les formes" (shared by the XPage + XDA palettes)
		InterfacePtr<IBoolData> isDisplayForme(GetExecutionContextSession(), IID_IDISPLAYFORME);
		isDisplayForme->Set(kFalse);
	} while (kFalse);
}

/* Shutdown
*/
void XPGStartupShutdown::Shutdown()
{
}

void XPGStartupShutdown::ReadConfFile() {

	do {

		InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
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

		K2Vector<KeyValuePair<PMString, PMString> >  sectionValues;

		InterfacePtr<IPMStream> input(StreamUtil::CreateFileStreamRead(configFile));
		if (!input)
			break;

		uchar c;
		PMString exportFileName = kNullString;
		PMString currentSection = kNullString;

		xpgPrefs->SetImportLegende(true);
		xpgPrefs->SetImportCredit(true);

		xpgPrefs->SetGestionIDMS(1);
		xpgPrefs->SetIDMS_MAJIDMS(1);
		xpgPrefs->SetIDMSALLBLOCS(0);
		xpgPrefs->SetChangePictureState(0); // bloqué tant que Gaia n'autorise pas

		while (input->GetStreamState() == kStreamStateGood) {
			string line;
			while ((input->GetStreamState() == kStreamStateGood) && (input->XferByte(c) != '\r')) {
				if (c != '\n')
					line += c;

				// GD 7.5.4 ++
				// Lecture ligne Win ET Mac (-> retour chariot '\n' uniquement, pas '\r\n')
				if (c == '\n')
					break;
				// GD 7.5.4 --
			}

			PlatformChar equal;
			equal.Set('=');
			PMString str(line.c_str());

			if (!str.IsEmpty()) {

				if (str[str.NumUTF16TextChars() - 1].IsChar('\r'))
					str.Remove(str.NumUTF16TextChars() - 1);

				str.StripWhiteSpace(PMString::kLeadingAndTrailingWhiteSpace);
				if (str.IsEmpty())
					continue;

				if (str[0] == '[' && str[str.NumUTF16TextChars() - 1] == ']') {
					currentSection = str;
				}
				else {
					int32 equalPos = str.IndexOfCharacter(equal);
					if (equalPos >= 0) {
						K2::scoped_ptr<PMString> key(str.Substring(0, equalPos));
						if (key && !key->IsEmpty()) {
							K2::scoped_ptr<PMString> value(str.Substring(equalPos + 1));
							// Value is a path
							if (value && !value->IsEmpty()) {
#ifdef MACINTOSH
								PMString valueBis = FileUtils::DecomposeUnicode(*value);
#else
								PMString valueBis = *value;
#endif														
								if ((key->Compare(kFalse, "RacineRedactions") == 0) || (key->Compare(kFalse, "RacineArbo") == 0))
									xpgPrefs->SetRacineArbo(valueBis);
								else if (key->Compare(kFalse, "Chemin") == 0 /*&& currentSection.Compare(kFalse, "[Formes]") == 0*/)
									xpgPrefs->SetCheminFormes(valueBis);
								else if (key->Compare(kFalse, "CheminAssemblage") == 0 /*&& currentSection.Compare(kFalse, "[Formes]") == 0*/)
									xpgPrefs->SetCheminAssemblageCartons(valueBis);
								else if (key->Compare(kFalse, "URLXR") == 0) {
									xpgPrefs->SetXRail_URL(valueBis);
								}
								else if (key->Compare(kFalse, "URLTEC") == 0) {
									xpgPrefs->SetTEC_URL(valueBis);
								}
								else if (key->Compare(kFalse, "NoUpdate") == 0)
									xpgPrefs->SetNoUpdateFile(valueBis);
								else if (key->Compare(kFalse, "SERVEUR") == 0)
									serveurPlugin = valueBis;
								else if (key->Compare(kFalse, "CreationForme") == 0)
									xpgPrefs->SetCreationForme(Utils<IDataStringUtils>()->StringToBool(WideString(valueBis)));

							}
						}
					}
				}
			}
		}

		// Persist the SERVEUR identifier so the XRail login dialog can reuse
		// it to re-fetch these preferences after a successful connection.
		xpgPrefs->SetPluginServerName(serveurPlugin);

		// Best-effort prefs fetch at startup: the server may be offline
		// (laptop offline, VPN down, maintenance), in which case we keep the
		// hard-coded defaults applied above and rely on the XRail login
		// dialog to re-fetch once the user successfully signs in. No alert
		// is surfaced here — it would fire on every cold start when offline.
		InterfacePtr<IWebServices> baseHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
		int32 prefs_ExportXML, prefs_GestionIDMS, prefs_IDMSMAJIDMS, prefs_IDMSALLBLOCS, prefs_ImportLegendes, prefs_ImportCredits, prefs_ChangePictureState = 0;
		K2Vector<int32> ei_IDs, ei_Ordres;
		K2Vector<PMString> ei_Noms, ei_Couleurs;
		K2Vector<int32> ea_IDs, ea_Ordres, ea_Couleurs, ea_Rayures;
		K2Vector<PMString> ea_Noms, ea_CouleursHTML;
		if (baseHTTP->GetPrefsXPage_v2(xpgPrefs->GetTEC_URL(), serveurPlugin, prefs_ExportXML, prefs_GestionIDMS, prefs_IDMSMAJIDMS, prefs_IDMSALLBLOCS, prefs_ImportLegendes, prefs_ImportCredits, prefs_ChangePictureState,
			ei_IDs, ei_Ordres, ei_Noms, ei_Couleurs,
			ea_IDs, ea_Ordres, ea_Noms, ea_CouleursHTML, ea_Couleurs, ea_Rayures))
		{
			xpgPrefs->SetGestionIDMS(prefs_GestionIDMS);
			xpgPrefs->SetIDMS_MAJIDMS(prefs_IDMSMAJIDMS);
			xpgPrefs->SetIDMSALLBLOCS(prefs_IDMSALLBLOCS);
			xpgPrefs->SetImportLegende(prefs_ImportLegendes);
			xpgPrefs->SetImportCredit(prefs_ImportCredits);
			xpgPrefs->SetChangePictureState(prefs_ChangePictureState);

			EtatImageList etatsImages;
			for (int32 i = 0; i < ei_IDs.size(); ++i) {
				EtatImage e;
				e.ID = ei_IDs[i];
				e.Ordre = ei_Ordres[i];
				e.Nom = ei_Noms[i];
				e.Couleur = ei_Couleurs[i];
				etatsImages.push_back(e);
			}
			xpgPrefs->SetEtatsImages(etatsImages);

			EtatArticleList etatsArticles;
			for (int32 i = 0; i < ea_IDs.size(); ++i) {
				EtatArticle e;
				e.ID = ea_IDs[i];
				e.Ordre = ea_Ordres[i];
				e.Nom = ea_Noms[i];
				e.CouleurHTML = ea_CouleursHTML[i];
				e.Couleur = ea_Couleurs[i];
				e.Rayures = ea_Rayures[i];
				etatsArticles.push_back(e);
			}
			xpgPrefs->SetEtatsArticles(etatsArticles);
		}


		// Close File
		input->Close();
	} while (kFalse);
}


ErrorCode XPGStartupShutdown::ReadXMLExportFile(const IDFile& xmlExportFile) {

	ErrorCode status = kFailure;
	do {
		InterfacePtr<IK2ServiceRegistry> serviceRegistry(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IK2ServiceProvider> xmlProvider(serviceRegistry->QueryServiceProviderByClassID(kXMLParserService, kXMLParserServiceBoss));
		InterfacePtr<ISAXServices> saxServices(xmlProvider, UseDefaultIID());
		InterfacePtr<ISAXContentHandler> contentHandler(::CreateObject2<ISAXContentHandler>(kXPGXMLExportFileHandlerBoss, ISAXContentHandler::kDefaultIID));
		contentHandler->Register(saxServices);
		InterfacePtr<ISAXEntityResolver> entityResolver(::CreateObject2<ISAXEntityResolver>(kXMLEmptyEntityResolverBoss, ISAXEntityResolver::kDefaultIID));
		InterfacePtr<ISAXParserOptions> parserOptions(saxServices, UseDefaultIID());

		// These commitments: follow 
		// the existing ImportXMLFileCmd
		parserOptions->SetNamespacesFeature(kFalse);
		parserOptions->SetShowWarningAlert(kFalse);

		InterfacePtr<IPMStream> xmlFileStream(StreamUtil::CreateFileStreamRead(xmlExportFile));

		saxServices->ParseStream(xmlFileStream, contentHandler, entityResolver);
		if (ErrorUtils::PMGetGlobalErrorCode() != kSuccess) {
			// Reset error code and quit
			ErrorUtils::PMSetGlobalErrorCode(kSuccess);
			PMString error(kXPGErrParseExportXMLFailed);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, FileUtils::SysFileToPMString(xmlExportFile));
			break;
		}

		status = kSuccess;

	} while (kFalse);

	return status;
}


ErrorCode XPGStartupShutdown::ReadXMLListTags(const IDFile& xmlTagsFile) {

	ErrorCode status = kFailure;
	do {
		InterfacePtr<IK2ServiceRegistry> serviceRegistry(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IK2ServiceProvider> xmlProvider(serviceRegistry->QueryServiceProviderByClassID(kXMLParserService, kXMLParserServiceBoss));

		InterfacePtr<ISAXServices> saxServices(xmlProvider, UseDefaultIID());

		InterfacePtr<ISAXContentHandler> contentHandler(static_cast<ISAXContentHandler*>(::CreateObject(kXPGXMLMatchingStylesListHandlerBoss, ISAXContentHandler::kDefaultIID)));
		contentHandler->Register(saxServices);

		InterfacePtr<ISAXEntityResolver> entityResolver(static_cast<ISAXEntityResolver*>(::CreateObject(kXMLEmptyEntityResolverBoss, ISAXEntityResolver::kDefaultIID)));
		InterfacePtr<ISAXParserOptions> parserOptions(saxServices, UseDefaultIID());

		// These commitments: follow 
		// the existing ImportXMLFileCmd
		parserOptions->SetNamespacesFeature(kFalse);
		parserOptions->SetShowWarningAlert(kFalse);

		InterfacePtr<IPMStream> xmlFileStream(StreamUtil::CreateFileStreamRead(xmlTagsFile));
		saxServices->ParseStream(xmlFileStream, contentHandler, entityResolver);

		if (ErrorUtils::PMGetGlobalErrorCode() != kSuccess) {
			// Reset error code and quit
			ErrorUtils::PMSetGlobalErrorCode(kSuccess);
			break;
		}
		status = kSuccess;

	} while (kFalse);
	return status;
}
