
#include "VCPluginHeaders.h"

// Interface includes:
#include "IPMStream.h"

// General includes:
#include "CPMUnknown.h"

// Project includes:
#include "XRCID.h"

#include "PMRect.h"

#include "ErrorUtils.h"
#include <string>
#include "CAlert.h"
#include "StringUtils.h"

#include "IWebServices.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif


#include <curl/curl.h>
#include <sstream>
#include <cstdio>
#include <map>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;
using namespace std;

/**    XRailWebServices
*/


// Fonction de rappel pour récupérer la réponse HTTP
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

// Log verbeux curl écrit dans %TEMP%\gaia_curl.log — utile pour diagnostiquer un hang HTTPS.
// Désactiver en mettant kGaiaCurlVerbose à 0 une fois le diagnostic terminé.
static const long kGaiaCurlVerbose = 1;

static int GaiaCurlDebugCallback(CURL* /*handle*/, curl_infotype type, char* data, size_t size, void* /*userptr*/)
{
#ifdef _WIN32
	static FILE* gLog = nullptr;
	if (!gLog) {
		char tempPath[MAX_PATH] = { 0 };
		GetTempPathA(MAX_PATH, tempPath);
		char fullPath[MAX_PATH] = { 0 };
		_snprintf_s(fullPath, _TRUNCATE, "%sgaia_curl.log", tempPath);
		fopen_s(&gLog, fullPath, "a");
	}
	if (gLog) {
		const char* prefix = "?";
		switch (type) {
			case CURLINFO_TEXT: prefix = "*"; break;
			case CURLINFO_HEADER_IN: prefix = "<"; break;
			case CURLINFO_HEADER_OUT: prefix = ">"; break;
			case CURLINFO_DATA_IN: prefix = "{"; break;
			case CURLINFO_DATA_OUT: prefix = "}"; break;
			default: break;
		}
		fprintf(gLog, "%s ", prefix);
		fwrite(data, 1, size, gLog);
		if (size && data[size - 1] != '\n') fputc('\n', gLog);
		fflush(gLog);
	}
#endif
	return 0;
}

// Options communes appliquées à chaque CURL* avant curl_easy_perform.
// Sans timeout, un serveur lent ou un handshake TLS qui patine gèle le main thread d'InDesign à 100% CPU.
// CURLOPT_PROXY="" et CURLSSLOPT_NO_REVOKE : évite les hangs liés à un proxy hérité de l'env utilisateur
// ou à une vérif CRL/OCSP qui patine en session interactive (InDesign Server n'a pas ces problèmes).
static void ApplyCommonCurlOpts(CURL* curl)
{
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(curl, CURLOPT_PROXY, "");
	curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, (long)CURLSSLOPT_NO_REVOKE);
	if (kGaiaCurlVerbose) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, GaiaCurlDebugCallback);
	}
}


class XRailWebServices : public CPMUnknown<IWebServices>
{
public:
	XRailWebServices(IPMUnknown* boss);

	virtual ~XRailWebServices();

	virtual bool VerifLogin_v2(const PMString& address, const PMString& login, const PMString& pwd, PMString& userID, PMString& errorCode, int& VoirPDF, int& VoirVignettes);

	virtual bool GetPrefsXRail_v2(const PMString& address, const int32& fromServeur, int32& prefs_ImportPub, int32& prefs_PubProp, int32& prefs_PDFMultipages, int32& prefs_ExportIDML, int32& prefs_MakePreview, PMString& prefs_AdTemplateNoVisual, PMString& prefs_AdTemplateWithVisual);

	virtual bool GetListeTitres_v2(const PMString& address, const int32& userID, K2Vector<PMString>& nomstitres, K2Vector<int32>& idstitres);

	virtual bool GetListeParu_v2(const PMString& address, const int32& idtitre, K2Vector<PMString>& nomsparus, K2Vector<int32>& idsparus);

	virtual bool GetListeEdi_v2(const PMString& address, const int32& idparu, K2Vector<PMString>& nomsedis, K2Vector<int32>& idsedis);

	virtual bool GetListePages_v2(const PMString& address, const int32& idedi, int32 nbPages, K2Vector<PMString>& nomspages, K2Vector<int32>& idspages);

	virtual bool GetListeRub_v2(const PMString& address, const int32& idtitre, K2Vector<PMString>& nomsrubs, K2Vector<int32>& idsrubs);

	virtual bool GetListeSSRub_v2(const PMString& address, const PMString& idsection, K2Vector<PMString>& nomsrubs, K2Vector<int32>& idsrubs);

	virtual bool GetPreviewFolder_v2(const PMString& address, const int32& IDPage, PMString& ClientPictPath, int32& Scale, int32& resolution);

	virtual bool RecupPreviews_v2(const PMString& address, const K2Vector<PMString>& FilePath, const K2Vector<int32>& IDPage,
		const K2Vector<int32>& blocPageIDs, const K2Vector<PMString>& blocX, const K2Vector<PMString>& blocY,
		const K2Vector<PMString>& blocLargeur, const K2Vector<PMString>& blocHauteur);

	virtual bool LinkDocument_v2(const PMString& address, const int32& pageID, K2Vector<int32>& listPagesIDS, K2Vector<int32>& listFolios, K2Vector<PMString>& listRoulettes, PMString& docFileName);

	virtual bool SetVerrou_v2(const PMString& address, const K2Vector<int32>& IDPage, int lock, const PMString& userID, PMString& status, bool errorInDoc);

	virtual bool GetListeStatus_v2(const PMString& address, const PMString& userID, K2Vector<PMString>& listStatus, K2Vector<bool>& checkError);

	virtual bool SetStatus_v2(const PMString& address, const K2Vector<int32>& tabidspages, const PMString& status, const PMString& userID);

	virtual bool SetImageState_v2(const PMString& address, const int32& idImage, const int32& idEtat);

	virtual bool GetListePubs_v2(const PMString& address, const K2Vector<int32>& IDPages, K2Vector<int32>& TabPagesIDS,
		K2Vector<int32>& TabFolios, K2Vector<PMString>& TabResaIDs, K2Vector<PMString>& TabPubsIDS,
		K2Vector<PMReal>& TabHauteurs, K2Vector<PMReal>& TabLargeurs, K2Vector<PMReal>& TabX,
		K2Vector<PMReal>& TabY, K2Vector<PMString>& TabAnnonceurs, K2Vector<PMString>& TabCheminImage, K2Vector<bool16>& TabVerrouPos);

	virtual bool SetPub_v2(const PMString& address, const PMString& IDPub, const int32& IDPage, const PMString& x, const PMString& y, const PMString& l, const PMString& h, const PMString& path);

	virtual bool UnlinkDocument_v2(const PMString& address, const int32& IDPage, PMString& docFileName);


	virtual bool MovePDFFile_v2(const PMString& address, const int32& IDPage, const PMString& fileName, const int32& IDUser);

	virtual bool GetPDFFileName_v2(const PMString& address, const int32& IDPage, PMString& profilPDF, PMString& pdfFileName);


	virtual bool GetPrefsXPage_v2(const PMString& address, const PMString& fromServeur, int32& prefs_ExportXML, int32& prefs_GestionIDMS, int32& prefs_IDMSMAJIDMS, int32& prefs_IDMSALLBLOCS, int32& prefs_ImportLegendes, int32& prefs_ImportCredits, int32& prefs_ChangePictureState,
		K2Vector<int32>& etatsImages_IDs, K2Vector<int32>& etatsImages_Ordres, K2Vector<PMString>& etatsImages_Noms, K2Vector<PMString>& etatsImages_Couleurs,
		K2Vector<int32>& etatsArticles_IDs, K2Vector<int32>& etatsArticles_Ordres, K2Vector<PMString>& etatsArticles_Noms, K2Vector<PMString>& etatsArticles_CouleursHTML, K2Vector<int32>& etatsArticles_Couleurs, K2Vector<int32>& etatsArticles_Rayures);

	virtual bool GetListeIDMS_v2(const PMString& address, const K2Vector<int32>& IDPages, K2Vector<int32>& TabIDStories, K2Vector<int32>& TabPagesIDS,
		K2Vector<PMReal>& TabX, K2Vector<PMReal>& TabY, K2Vector<PMString>& TabIDMSpaths, K2Vector<PMString>& TabXMLStorypaths, K2Vector<PMString>& TabSubjects);

	virtual bool GetPageInfos_v2(const PMString& address, const int32 pageID,
		int32& titreID,
		PMString& titreLib,
		PMString& parutionLib);

	virtual bool UpdateStory_v2(const PMString& address, const PMString& storyID, const PMString& userLogin, int32& canUpdate);

	virtual bool GetStories_v2(const PMString& address, const int32 idParutionMEP,
		const PMString& idSection,
		const PMString& idSubSection,
		K2Vector<PMString>& IDStories,
		K2Vector<PMString>& ListSujets,
		K2Vector<PMString>& CheminArticles,
		K2Vector<PMString>& CouleurStatus,
		K2Vector<PMString>& LibStatus);

	virtual bool SaveStories_v2(const PMString& address, const K2Vector<int32>& tabidspages, const PMString from, K2Vector<int32>& ListePages, K2Vector<int32>& TabIDStories, K2Vector<int32>& TabTypes,
		K2Vector<int32>& TabNbSignes,
		K2Vector<int32>& IDStoriesByBloc, K2Vector<PMString>& TabX, K2Vector<PMString>& TabY,
		K2Vector<PMString>& TabLargeur, K2Vector<PMString>& TabHauteur, K2Vector<int32>& TabIDPage,
		K2Vector<PMString>& ListeImages, K2Vector<WideString>& ListeLegendes, K2Vector<WideString>& ListeCredits, PMString& result);

	virtual bool GetStoryInfo_v2(const PMString& address, const PMString& idStory, PMString& colorStatus, PMString& sujet);


	virtual bool GetUsersAffectations_v2(const PMString& address, const int32& idPage, K2Vector<PMString>& usersid, K2Vector<PMString>& usersnames);

	virtual bool CreateAffectation_v2(const PMString& address, const PMString& idUserDest,
		const PMString& subject,
		const PMString& idSection,
		const PMString& idSousSection,
		const int32 nbPhotos,
		const PMString& nbSignesEstim,
		PMString& idStory,
		PMString& storyFullPath);

	virtual bool StoryLockChanged(const PMString& idArt, const PMString& flagLock);

	virtual bool GetStoryStates(const PMString& idArt,
		const PMString& user,
		IDValueList& StatusValuesList,
		K2Vector<PMString>& ColorsList,
		K2Vector<PMString>& StatusOrderList,
		K2Vector<PMString>& StatusRoleList,
		K2Vector<PMString>& StatusRegleList,
		K2Vector<PMString>& CommentaireList,
		PMString& result);

	virtual bool GetGroupsStatusList(const PMString& IDStory,
		const PMString& author,
		const PMString& status,
		IDValueList& StatusValuesList,
		PMString& result);

	virtual bool GetUsersStatusList(const PMString& idArt,
		const PMString& author,
		const PMString& status,
		const PMString& group,
		IDValueList& usersValuesList,
		PMString& result);

	virtual bool StoryStatusChanged(const PMString& storyID,
		const PMString& statusID,
		const PMString& userID,
		const PMString& groupID,
		PMString& result);

	virtual bool GetCommentaires_v2(const PMString& address, const K2Vector<int32>& IDPages,
		K2Vector<int32>& IDsComm,
		K2Vector<int32>& IDsPage,
		K2Vector<PMString>& Commentaires,
		K2Vector<PMString>& DatesISO,
		K2Vector<int32>& HeuresSecs,
		K2Vector<PMString>& NomsUser,
		K2Vector<bool>& Checks,
		K2Vector<PMReal>& Xs,
		K2Vector<PMReal>& Ys,
		bool& fromPhp);

	virtual bool CommValid(const PMString& address, const int32& IDComm);

	virtual Value convert_PMStringToUTF8(PMString mystring, Document::AllocatorType& allocator);
	virtual bool CallWebServiceAndParseJson(const std::string& url, rapidjson::Document& doc);

private:
	int32 onwindows;
};

/* CREATE_PMINTERFACE
Binds the C++ implementation class onto its
ImplementationID making the C++ code callable by the
application.
*/
CREATE_PMINTERFACE(XRailWebServices, kXRCWebServicesImpl)

/* Constructor
*/
XRailWebServices::XRailWebServices(IPMUnknown* boss) :
	CPMUnknown<IWebServices>(boss)
{
#if WINDOWS
	onwindows = 1;
#else
	onwindows = 0;
#endif
}

/* Destructor.
*/
XRailWebServices::~XRailWebServices()
{
}

//----------------------------------------------------------------------------------------------------------------
// Pour Incopy, verrouille/deverrouille un article
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::StoryLockChanged(const PMString& idArt, const PMString& flagLock)
{

	return true;

}

//----------------------------------------------------------------------------------------------------------------
// Pour Incopy, renvoie la liste des etats lors du changement d'etat d'un article. A VERIFIER
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::GetStoryStates(const PMString& idArt,
	const PMString& user,
	IDValueList& StatusValuesList,
	K2Vector<PMString>& ColorsList,
	K2Vector<PMString>& StatusOrderList,
	K2Vector<PMString>& StatusRoleList,
	K2Vector<PMString>& StatusRegleList,
	K2Vector<PMString>& CommentaireList,
	PMString& result
)
{
	/*PMString StatusID = kNullString,
	StatusName = kNullString;

	struct ns1__WS_USCOREGetStoryStatusListResponse response;
	bool success = (soap_call_ns1__WS_USCOREGetStoryStatusList(&soap, serverAddress.GetPlatformString().c_str(), NULL,
	idArt.GetPlatformString().c_str(), user.GetPlatformString().c_str(),
	response) == SOAP_OK);
	if (!success) {
	HandleWebServiceError("GetStoryStates");
	}
	else {
	for (int32 i = 0; i < response.StatusList->__size; ++i) {

	StatusID.Append(convert_UTF8_to_PMString(response.StatusIDList->__ptr[i].c_str()));
	StatusID.SetTranslatable(kFalse);
	StatusName.Append(convert_UTF8_to_PMString(response.StatusList->__ptr[i].c_str()));
	StatusName.SetTranslatable(kFalse);
	StatusValuesList.push_back(IDValuePair(StatusID, StatusName));

	StatusID.clear();
	StatusName.clear();

	ColorsList.push_back(convert_UTF8_to_PMString(response.ColorsList->__ptr[i].c_str()));
	StatusOrderList.push_back(convert_UTF8_to_PMString(response.StatusOrderList->__ptr[i].c_str()));
	StatusRoleList.push_back(convert_UTF8_to_PMString(response.StatusRoleList->__ptr[i].c_str()));
	StatusRegleList.push_back(convert_UTF8_to_PMString(response.StatusRegleList->__ptr[i].c_str()));
	CommentaireList.push_back(convert_UTF8_to_PMString(response.CommentaireList->__ptr[i].c_str()));
	}
	}
	return success;*/
	return true;

}

//----------------------------------------------------------------------------------------------------------------
// Pour Incopy, renvoie la liste des groupes lors du changement d'etat d'un article. A VERIFIER
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::GetGroupsStatusList(const PMString& idArt,
	const PMString& author,
	const PMString& status,
	IDValueList& groupsValuesList,
	PMString& result)
{
	/*
	PMString groupID, groupName;

	struct ns1__WS_USCOREGetGroupsStatusListResponse response;
	bool success = (soap_call_ns1__WS_USCOREGetGroupsStatusList(&soap, serverAddress.GetPlatformString().c_str(), NULL,
	author.GetPlatformString().c_str(), idArt.GetPlatformString().c_str(),
	status.GetPlatformString().c_str(), response) == SOAP_OK);

	if (!success) {
	HandleWebServiceError("GetGroupsStatusList");
	}
	else {
	for (int32 i = 0; i < response.GroupDataList->__size; ++i) {

	groupID.Append(convert_UTF8_to_PMString(response.GroupDataList->__ptr[i].c_str()));
	groupID.SetTranslatable(kFalse);

	groupName.Append(convert_UTF8_to_PMString(response.GroupLabelList->__ptr[i].c_str()));
	groupName.SetTranslatable(kFalse);

	groupsValuesList.push_back(IDValuePair(groupID, groupName));

	groupID.clear();
	groupName.clear();
	}
	}
	return success;*/
	return true;
}

//----------------------------------------------------------------------------------------------------------------
// Pour Incopy, renvoie la liste des etats lors du changement d'etat d'un article. A VERIFIER
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::GetUsersStatusList(const PMString& author,
	const PMString& idArt,
	const PMString& status,
	const PMString& group,
	IDValueList& usersValuesList,
	PMString& result)
{

	/*    PMString userID, userName;

	struct ns1__WS_USCOREGetUsersStatusListResponse response;
	bool success = (soap_call_ns1__WS_USCOREGetUsersStatusList(&soap, serverAddress.GetPlatformString().c_str(), NULL,
	author.GetPlatformString().c_str(), idArt.GetPlatformString().c_str(),
	status.GetPlatformString().c_str(), group.GetPlatformString().c_str(),
	response) == SOAP_OK);

	if (!success) {
	HandleWebServiceError("GetUsersStatusList");
	}
	else {
	for (int32 i = 0; i < response.UserDataList->__size; ++i) {

	userID.Append(convert_UTF8_to_PMString(response.UserDataList->__ptr[i].c_str()));
	userID.SetTranslatable(kFalse);

	userName.Append(convert_UTF8_to_PMString(response.UserLabelList->__ptr[i].c_str()));
	userName.SetTranslatable(kFalse);

	usersValuesList.push_back(IDValuePair(userID, userName));

	userID.clear();
	userName.clear();
	}
	}
	return success;
	*/
	return true;
}


//----------------------------------------------------------------------------------------------------------------
// Pour Incopy, change l'etat d'un article
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::StoryStatusChanged(const PMString& storyID, const PMString& statusID,
	const PMString& userID, const PMString& groupID,
	PMString& result)
{

	return true;
}

//----------------------------------------------------------------------------------------------------------------
// XPage, Recupere les infos d'etat d'un article depuis xrail
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::GetStoryInfo_v2(const PMString& address, const PMString& idStory, PMString& colorStatus, PMString& sujet)
{
	std::string serverURL = address.GetPlatformString().c_str();
	std::string id = idStory.GetPlatformString().c_str();

	// Construire l'URL avec les paramètres en GET
	std::string url = serverURL + "/Plugin_GetInfosStory?idstory=" + id;

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;


	// Récupérer les données du JSON
	if (doc.HasMember("colorStatus") && doc["colorStatus"].IsString()) {
		colorStatus = doc["colorStatus"].GetString();
	}
	if (doc.HasMember("sujet") && doc["sujet"].IsString()) {
		sujet = doc["sujet"].GetString();
	}

	return true;

}


//----------------------------------------------------------------------------------------------------------------
// XPage demande a xrail->tec la liste des articles a mettre en page (+idPage pour gestion edition)
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::GetStories_v2(const PMString& address, const int32 idParutionMEP,
	const PMString& idSection,
	const PMString& idSubSection,
	K2Vector<PMString>& IDStories,
	K2Vector<PMString>& ListSujets,
	K2Vector<PMString>& CheminArticles,
	K2Vector<PMString>& CouleurStatus,
	K2Vector<PMString>& LibStatus)

{
	// Construire l'URL avec les paramètres en GET
	std::string serverURL = address.GetPlatformString().c_str();
	std::string section = idSection.GetPlatformString().c_str();
	std::string sssection = idSubSection.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetStories?idparu=" + std::to_string(idParutionMEP) + "&idSection=" + section + "&idSsSection=" + sssection + "&onwindows=" + std::to_string(onwindows);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;

	// Récupérer les données du JSON
	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("ID") && item["ID"].IsString())
		{
			IDStories.push_back(item["ID"].GetString());
			ListSujets.push_back(item["Sujet"].GetString());
			CheminArticles.push_back(item["Chemin"].GetString());
			CouleurStatus.push_back(item["CouleurStatus"].GetString());
			LibStatus.push_back(item["LibStatus"].GetString());
		}
	}

	return true;

}




Value XRailWebServices::convert_PMStringToUTF8(PMString mystring, Document::AllocatorType& allocator) {
	std::string myStr;
	WideString myWD(mystring);

	StringUtils::ConvertWideStringToUTF8(myWD, myStr);
	Value myValue;
	myValue.SetString(myStr.c_str(), static_cast<SizeType>(myStr.length()), allocator);
	return myValue;
}


bool XRailWebServices::CallWebServiceAndParseJson(const std::string& url, rapidjson::Document& doc)
{
	CURL* curl = curl_easy_init();
    if (!curl) {
      return false;
    }
	std::string responseStr;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return false;
    }

	if (doc.Parse(responseStr.c_str()).HasParseError())
		return false;

	return true;
}


//----------------------------------------------------------------------------------------------------------------
// verification du login et du mot de passe
//----------------------------------------------------------------------------------------------------------------

bool XRailWebServices::VerifLogin_v2(const PMString& address, const PMString& login, const PMString& pwd, PMString& userID, PMString& errorCode, int& VoirPDF, int& VoirVignettes)
{
	std::string loginStr = login.GetPlatformString().c_str();
	std::string pwdStr = pwd.GetPlatformString().c_str();
	std::string serverURL = address.GetPlatformString().c_str();

	// Construire l'URL avec les paramètres en GET
	std::string url = serverURL + "/Plugin_Connection?login=" + loginStr + "&password=" + pwdStr;

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;


	// Récupérer les données du JSON
	if (doc.HasMember("retour") && doc["retour"].IsString()) {
		errorCode = doc["retour"].GetString();
	}

	if (doc.HasMember("iduser") && doc["iduser"].IsInt()) {
		userID.AppendNumber(doc["iduser"].GetInt());
	}

	if (doc.HasMember("VoirPDF") && doc["VoirPDF"].IsInt()) {
		VoirPDF = doc["VoirPDF"].GetInt();
	}

	if (doc.HasMember("VoirVignettes") && doc["VoirVignettes"].IsInt()) {
		VoirVignettes = doc["VoirVignettes"].GetInt();
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------

bool XRailWebServices::GetPrefsXRail_v2(const PMString& address, const int32& fromServeur, int32& prefs_ImportPub, int32& prefs_PubProp, int32& prefs_PDFMultipages, int32& prefs_ExportIDML, int32& prefs_MakePreview, PMString& prefs_AdTemplateNoVisual, PMString& prefs_AdTemplateWithVisual)
{
	// Construire l'URL avec les paramètres en GET
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetPrefs?serveur=" + std::to_string(fromServeur);

	Document doc;
    if (!CallWebServiceAndParseJson(url, doc)) {
        PMString tempPMString(url.c_str());
        return false;
    }

	// Récupérer les données du JSON
	if (doc.HasMember("ImportPub") && doc["ImportPub"].IsInt()) {
		prefs_ImportPub = doc["ImportPub"].GetInt();
	}
	if (doc.HasMember("PubProp") && doc["PubProp"].IsInt()) {
		prefs_PubProp = doc["PubProp"].GetInt();
	}
	if (doc.HasMember("PDFMultipages") && doc["PDFMultipages"].IsInt()) {
		prefs_PDFMultipages = doc["PDFMultipages"].GetInt();
	}
	if (doc.HasMember("ExportIDML") && doc["ExportIDML"].IsInt()) {
		prefs_ExportIDML = doc["ExportIDML"].GetInt();
	}
	if (doc.HasMember("MakePreview") && doc["MakePreview"].IsInt()) {
		prefs_MakePreview = doc["MakePreview"].GetInt();
	}

	// IDMS template paths for ad blocks. Both keys are optional — when
	// absent or empty the importer falls back to the hard-coded creation
	// path. We don't reset the out-params on missing keys so the caller
	// can pass them already initialised to "" without surprises.
	if (doc.HasMember("adTemplateNoVisual") && doc["adTemplateNoVisual"].IsString()) {
		prefs_AdTemplateNoVisual = PMString(doc["adTemplateNoVisual"].GetString());
	}
	if (doc.HasMember("adTemplateWithVisual") && doc["adTemplateWithVisual"].IsString()) {
		prefs_AdTemplateWithVisual = PMString(doc["adTemplateWithVisual"].GetString());
	}

	return true;
}



//----------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------

// Resolve a JSON value that's expected to be an array of objects. Handles the
// proper form (already an array of objects) AND the broken form Gaia emits
// today, where the array is wrapped as a single JSON string inside a
// one-element array. `fallback` owns the re-parsed Document when we have to
// unwrap, so the caller must keep it alive while using the returned Value*.
// Returns nil if the value doesn't resolve to a usable array.
static const Value* ResolveArrayDoc(const Value& v, Document& fallback)
{
	if (v.IsArray() && (v.Size() == 0 || v[0].IsObject()))
		return &v;
	if (v.IsArray() && v.Size() == 1 && v[0].IsString()) {
		fallback.Parse(v[0].GetString());
		if (!fallback.HasParseError() && fallback.IsArray())
			return &fallback;
	}
	if (v.IsString()) {
		fallback.Parse(v.GetString());
		if (!fallback.HasParseError() && fallback.IsArray())
			return &fallback;
	}
	return nil;
}

bool XRailWebServices::GetPrefsXPage_v2(const PMString& address, const PMString& fromServeur, int32& prefs_ExportXML, int32& prefs_GestionIDMS, int32& prefs_IDMSMAJIDMS, int32& prefs_IDMSALLBLOCS, int32& prefs_ImportLegendes, int32& prefs_ImportCredits, int32& prefs_ChangePictureState,
	K2Vector<int32>& etatsImages_IDs, K2Vector<int32>& etatsImages_Ordres, K2Vector<PMString>& etatsImages_Noms, K2Vector<PMString>& etatsImages_Couleurs,
	K2Vector<int32>& etatsArticles_IDs, K2Vector<int32>& etatsArticles_Ordres, K2Vector<PMString>& etatsArticles_Noms, K2Vector<PMString>& etatsArticles_CouleursHTML, K2Vector<int32>& etatsArticles_Couleurs, K2Vector<int32>& etatsArticles_Rayures)
{
	// Construire l'URL avec les paramètres en GET
	std::string serverURL = address.GetPlatformString().c_str();
	std::string fromServeurStr = fromServeur.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetPrefs?serveur=" + fromServeurStr;

	etatsImages_IDs.clear();
	etatsImages_Ordres.clear();
	etatsImages_Noms.clear();
	etatsImages_Couleurs.clear();
	etatsArticles_IDs.clear();
	etatsArticles_Ordres.clear();
	etatsArticles_Noms.clear();
	etatsArticles_CouleursHTML.clear();
	etatsArticles_Couleurs.clear();
	etatsArticles_Rayures.clear();

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;

	// Récupérer les données du JSON
	if (doc.HasMember("ExportXML") && doc["ExportXML"].IsInt()) {
		prefs_ExportXML = doc["ExportXML"].GetInt();
	}

	if (doc.HasMember("GestionIDMS") && doc["GestionIDMS"].IsInt()) {
		prefs_GestionIDMS = doc["GestionIDMS"].GetInt();
	}
	if (doc.HasMember("IDMSMAJIDMS") && doc["IDMSMAJIDMS"].IsInt()) {
		prefs_IDMSMAJIDMS = doc["IDMSMAJIDMS"].GetInt();
	}
	if (doc.HasMember("IDMSALLBLOCS") && doc["IDMSALLBLOCS"].IsInt()) {
		prefs_IDMSALLBLOCS = doc["IDMSALLBLOCS"].GetInt();
	}
	if (doc.HasMember("ImportLegendes") && doc["ImportLegendes"].IsInt()) {
		prefs_ImportLegendes = doc["ImportLegendes"].GetInt();
	}
	if (doc.HasMember("ImportCredits") && doc["ImportCredits"].IsInt()) {
		prefs_ImportCredits = doc["ImportCredits"].GetInt();
	}
	// Flag de licence : autorise le changement d'état depuis la palette Photos
	// liées. Absent => on laisse la valeur passée par l'appelant (bloqué par
	// défaut), donc l'option n'est active que si Gaia l'envoie explicitement.
	if (doc.HasMember("ChangePictureState") && doc["ChangePictureState"].IsInt()) {
		prefs_ChangePictureState = doc["ChangePictureState"].GetInt();
	}

	// Defensive parsing: Gaia currently double-wraps these arrays as a single
	// JSON string inside a one-element array (e.g.
	//   "etatsimages":["[{\"ID\":1,...}]"]
	// instead of the proper
	//   "etatsimages":[{"ID":1,...}]).
	// Try the direct form first, then fall back to unwrapping the string and
	// re-parsing it. Once the server emits a real array we can drop the
	// fallback. (See ResolveArrayDoc free function above.)

	if (doc.HasMember("etatsimages")) {
		Document fallback;
		const Value* arrPtr = ResolveArrayDoc(doc["etatsimages"], fallback);
		if (arrPtr != nil) {
			const Value& arr = *arrPtr;
			for (SizeType i = 0; i < arr.Size(); i++) {
				const Value& item = arr[i];
				if (!item.IsObject()) continue;
				int32 id = (item.HasMember("ID") && item["ID"].IsInt()) ? item["ID"].GetInt() : 0;
				int32 ordre = (item.HasMember("Ordre") && item["Ordre"].IsInt()) ? item["Ordre"].GetInt() : 0;
				PMString nom = (item.HasMember("Nom") && item["Nom"].IsString()) ? PMString(item["Nom"].GetString()) : PMString();
				PMString couleur = (item.HasMember("Couleur") && item["Couleur"].IsString()) ? PMString(item["Couleur"].GetString()) : PMString();
				etatsImages_IDs.push_back(id);
				etatsImages_Ordres.push_back(ordre);
				etatsImages_Noms.push_back(nom);
				etatsImages_Couleurs.push_back(couleur);
			}
		}
	}

	if (doc.HasMember("etatsarticles")) {
		Document fallback;
		const Value* arrPtr = ResolveArrayDoc(doc["etatsarticles"], fallback);
		if (arrPtr != nil) {
			const Value& arr = *arrPtr;
			for (SizeType i = 0; i < arr.Size(); i++) {
				const Value& item = arr[i];
				if (!item.IsObject()) continue;
				int32 id = (item.HasMember("ID") && item["ID"].IsInt()) ? item["ID"].GetInt() : 0;
				int32 ordre = (item.HasMember("Ordre") && item["Ordre"].IsInt()) ? item["Ordre"].GetInt() : 0;
				PMString nom = (item.HasMember("Nom") && item["Nom"].IsString()) ? PMString(item["Nom"].GetString()) : PMString();
				PMString couleurHTML = (item.HasMember("CouleurHTML") && item["CouleurHTML"].IsString()) ? PMString(item["CouleurHTML"].GetString()) : PMString();
				int32 couleur = (item.HasMember("Couleur") && item["Couleur"].IsInt()) ? item["Couleur"].GetInt() : 0;
				int32 rayures = (item.HasMember("Rayures") && item["Rayures"].IsInt()) ? item["Rayures"].GetInt() : 0;
				etatsArticles_IDs.push_back(id);
				etatsArticles_Ordres.push_back(ordre);
				etatsArticles_Noms.push_back(nom);
				etatsArticles_CouleursHTML.push_back(couleurHTML);
				etatsArticles_Couleurs.push_back(couleur);
				etatsArticles_Rayures.push_back(rayures);
			}
		}
	}

	return true;
}

bool XRailWebServices::GetListeTitres_v2(const PMString& address, const int32& userID, K2Vector<PMString>& nomstitres, K2Vector<int32>& idstitres)
{
	// Construire l'URL avec les paramètres en GET
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetListeTitres?userid=" + std::to_string(userID);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;
	if (!doc.IsArray())
		return false;

	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("ID") && item["ID"].IsInt() &&
			item.HasMember("NomTitre") && item["NomTitre"].IsString())
		{
			idstitres.push_back(item["ID"].GetInt());
			nomstitres.push_back(item["NomTitre"].GetString());
		}
	}

	return true;
}
bool XRailWebServices::GetListeParu_v2(const PMString& address, const int32& idtitre, K2Vector<PMString>& nomsparus, K2Vector<int32>& idsparus)
{
	// Construire l'URL avec les paramètres en GET
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetListeParus?idtitre=" + std::to_string(idtitre);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;

	// Récupérer les données du JSON
	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("ID") && item["ID"].IsInt() &&
			item.HasMember("NomParution") && item["NomParution"].IsString())
		{
			idsparus.push_back(item["ID"].GetInt());
			nomsparus.push_back(item["NomParution"].GetString());
		}
	}

	return true;
}

bool XRailWebServices::GetListeRub_v2(const PMString& address, const int32& idtitre, K2Vector<PMString>& nomsrubs, K2Vector<int32>& idsrubs)
{
	// Construire l'URL avec les paramètres en GET
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetListeRubs?idtitre=" + std::to_string(idtitre);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;

	// Récupérer les données du JSON
	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("ID") && item["ID"].IsInt() &&
			item.HasMember("Rubrique") && item["Rubrique"].IsString())
		{
			idsrubs.push_back(item["ID"].GetInt());
			nomsrubs.push_back(item["Rubrique"].GetString());
		}
	}

	return true;
}

bool XRailWebServices::GetListeSSRub_v2(const PMString& address, const PMString& idsection, K2Vector<PMString>& nomsrubs, K2Vector<int32>& idsrubs)
{
	// Construire l'URL avec les paramètres en GET
	std::string serverURL = address.GetPlatformString().c_str();
	std::string section = idsection.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetListeSSRubs?idsection=" + section;

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;

	// Récupérer les données du JSON
	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("ID") && item["ID"].IsInt() &&
			item.HasMember("Nom") && item["Nom"].IsString())
		{
			idsrubs.push_back(item["ID"].GetInt());
			nomsrubs.push_back(item["Nom"].GetString());
		}
	}

	return true;
}

bool XRailWebServices::GetListeEdi_v2(const PMString& address, const int32& idparu, K2Vector<PMString>& nomsedis, K2Vector<int32>& idsedis)
{
	// Construire l'URL avec les paramètres en GET
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetListeEdis?idparu=" + std::to_string(idparu);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;

	// Récupérer les données du JSON
	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("ID") && item["ID"].IsInt() &&
			item.HasMember("NomEdition") && item["NomEdition"].IsString())
		{
			idsedis.push_back(item["ID"].GetInt());
			nomsedis.push_back(item["NomEdition"].GetString());
		}
	}

	return true;
}
bool XRailWebServices::GetListePages_v2(const PMString& address, const int32& idedi, int32 nbPages, K2Vector<PMString>& nomspages, K2Vector<int32>& idspages)
{
	// Construire l'URL avec les paramètres en GET
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetListePages?idedi=" + std::to_string(idedi) + "&nbpages=" + std::to_string(nbPages);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;

	// Récupérer les données du JSON
	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("ID") && item["ID"].IsInt() &&
			item.HasMember("NomPage") && item["NomPage"].IsString())
		{
			idspages.push_back(item["ID"].GetInt());
			nomspages.push_back(item["NomPage"].GetString());
		}
	}

	return true;
}

bool XRailWebServices::GetPreviewFolder_v2(const PMString& address, const int32& IDPage, PMString& ClientPictPath, int32& Scale, int32& resolution)
{
	std::string serverURL = address.GetPlatformString().c_str();

	// Construire l'URL avec les paramètres en GET
	std::string url = serverURL + "/Plugin_GetPreviewFolder?idpage=" + std::to_string(IDPage) + "&onwindows=" + std::to_string(onwindows);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;


	// Récupérer les données du JSON
	if (doc.HasMember("scale") && doc["scale"].IsInt()) {
		Scale = doc["scale"].GetInt();
	}

	if (doc.HasMember("folder") && doc["folder"].IsString()) {
		ClientPictPath = doc["folder"].GetString();
	}
	if (doc.HasMember("resolution") && doc["resolution"].IsInt()) {
		resolution = doc["resolution"].GetInt();
	}


	return true;
}

bool XRailWebServices::LinkDocument_v2(const PMString& address, const int32& pageID, K2Vector<int32>& listPagesIDS, K2Vector<int32>& listFolios, K2Vector<PMString>& listRoulettes, PMString& docFileName)
{
	std::string serverURL = address.GetPlatformString().c_str();

	// Construire l'URL avec les paramètres en GET
	std::string url = serverURL + "/Plugin_LinkDocument?idpage=" + std::to_string(pageID) + "&onwindows=" + std::to_string(onwindows);

	listPagesIDS.clear();
	listFolios.clear();
	listRoulettes.clear();

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;


	// Récupérer les données du JSON
	if (doc.HasMember("docFileName") && doc["docFileName"].IsString()) {
		docFileName = doc["docFileName"].GetString();
	}

	// Server returns idpages/folios/roulettes as JSON-encoded strings (a
	// single string holding a nested array), not as plain arrays — hence the
	// double Parse() pattern below.
	if (doc.HasMember("idpages") && doc["idpages"].IsString()) {
		const char* idpagesRaw = doc["idpages"].GetString();

		Document idpagesDoc;
		if (!idpagesDoc.Parse(idpagesRaw).HasParseError() && idpagesDoc.IsArray()) {
			for (auto& item : idpagesDoc.GetArray()) {
				if (item.HasMember("IDPage") && item["IDPage"].IsInt()) {
					listPagesIDS.push_back(item["IDPage"].GetInt());
				}
			}
		}
	}

	if (doc.HasMember("folios") && doc["folios"].IsString()) {
		const char* foliosRaw = doc["folios"].GetString();

		Document foliosDoc;
		if (!foliosDoc.Parse(foliosRaw).HasParseError() && foliosDoc.IsArray()) {
			for (auto& item : foliosDoc.GetArray()) {
				if (item.HasMember("FolioReel") && item["FolioReel"].IsInt()) {
					listFolios.push_back(item["FolioReel"].GetInt());
				}
			}
		}
	}

	if (doc.HasMember("roulettes") && doc["roulettes"].IsString()) {
		const char* roulettesRaw = doc["roulettes"].GetString();

		Document roulettesDoc;
		if (!roulettesDoc.Parse(roulettesRaw).HasParseError() && roulettesDoc.IsArray()) {
			for (auto& item : roulettesDoc.GetArray()) {
				if (item.IsString()) {
					PMString roulette(item.GetString());
					roulette.SetTranslatable(kFalse);
					listRoulettes.push_back(roulette);
				}
			}
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------
// Dans XRail, recupere le nom du fichier a enregistrer si on fait un Unlink
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::UnlinkDocument_v2(const PMString& address, const int32& pageID, PMString& docFileName)
{
	std::string serverURL = address.GetPlatformString().c_str();

	// Construire l'URL avec les paramètres en GET
	std::string url = serverURL + "/Plugin_UnlinkDocument?idpage=" + std::to_string(pageID) + "&onwindows=" + std::to_string(onwindows);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;


	// Récupérer les données du JSON
	if (doc.HasMember("docFileName") && doc["docFileName"].IsString()) {
		docFileName = doc["docFileName"].GetString();
	}

	return true;
}

bool XRailWebServices::RecupPreviews_v2(const PMString& address, const K2Vector<PMString>& tabfilesnames, const K2Vector<int32>& tabidspages,
	const K2Vector<int32>& blocPageIDs, const K2Vector<PMString>& blocX, const K2Vector<PMString>& blocY,
	const K2Vector<PMString>& blocLargeur, const K2Vector<PMString>& blocHauteur)
{

	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();

	Value myarray(kArrayType);

	for (int i = 0; i < tabfilesnames.size(); ++i) {
		Value obj(kObjectType);
		obj.AddMember("id", tabidspages[i], allocator);
		obj.AddMember("nomfichier", Value().SetString(tabfilesnames[i].GetPlatformString().c_str(), allocator), allocator);
		myarray.PushBack(obj, allocator);
	}

	doc.AddMember("liste", myarray, allocator);

	// Blocs sans ID Gaia, regroupes par page : un objet { "id": idPage, "blocs": [ {x,y,largeur,hauteur}, ... ] } par page
	Value blocsByPage(kArrayType);
	std::map<int32, SizeType> pageIndex;     // idPage -> position dans blocsByPage
	for (int i = 0; i < blocPageIDs.size(); ++i) {
		int32 idPage = blocPageIDs[i];

		Value bloc(kObjectType);
		bloc.AddMember("x", Value().SetString(blocX[i].GetPlatformString().c_str(), allocator), allocator);
		bloc.AddMember("y", Value().SetString(blocY[i].GetPlatformString().c_str(), allocator), allocator);
		bloc.AddMember("largeur", Value().SetString(blocLargeur[i].GetPlatformString().c_str(), allocator), allocator);
		bloc.AddMember("hauteur", Value().SetString(blocHauteur[i].GetPlatformString().c_str(), allocator), allocator);

		std::map<int32, SizeType>::iterator it = pageIndex.find(idPage);
		if (it == pageIndex.end()) {
			Value pageObj(kObjectType);
			pageObj.AddMember("id", idPage, allocator);
			Value blocsArray(kArrayType);
			blocsArray.PushBack(bloc, allocator);
			pageObj.AddMember("blocs", blocsArray, allocator);
			pageIndex[idPage] = blocsByPage.Size();
			blocsByPage.PushBack(pageObj, allocator);
		}
		else {
			blocsByPage[it->second]["blocs"].PushBack(bloc, allocator);
		}
	}
	doc.AddMember("blocs", blocsByPage, allocator);

	doc.AddMember("onwindows", onwindows, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	// 2. Préparation de l'URL
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_RecupPreviews";

	// 3. cURL init
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;

	return true;
}


//----------------------------------------------------------------------------------------------------------------
// verrouille ou deverrouille la page dans xrail, ouverture ou fermeture du doc
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::SetVerrou_v2(const PMString& address, const K2Vector<int32>& tabidspages, int lock, const PMString& userID, PMString& status, bool errorInDoc)
{

	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();

	Value myarray(kArrayType);

	for (int i = 0; i < tabidspages.size(); ++i) {
		Value obj(kObjectType);
		obj.AddMember("id", tabidspages[i], allocator);
		myarray.PushBack(obj, allocator);
	}

	doc.AddMember("lock", lock, allocator);
	doc.AddMember("listepages", myarray, allocator);
	doc.AddMember("userID", Value().SetString(userID.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("errorInDoc", errorInDoc, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	// 2. Préparation de l'URL
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_SetVerrou";

	// 3. cURL init
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;

	if (doc.Parse(responseStr.c_str()).HasParseError())
		return false;

	if (doc.HasMember("status") && doc["status"].IsString()) {
		status = doc["status"].GetString();
	}
	return true;
}


//----------------------------------------------------------------------------------------------------------------
// renvoie la liste des etats de pages autorises pour cet utilisateur (dlg de changement d'etat XRail)
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::GetListeStatus_v2(const PMString& address, const PMString& userID, K2Vector<PMString>& listStatus, K2Vector<bool>& checkError)
{
	std::string serverURL = address.GetPlatformString().c_str();
	std::string userStr = userID.GetPlatformString().c_str();

	// Construire l'URL avec les paramètres en GET
	std::string url = serverURL + "/Plugin_GetListeStatus?iduser=" + userStr;

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;



	// Récupérer les données du JSON
	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("checkError") && item["checkError"].IsBool() &&
			item.HasMember("NomEtat") && item["NomEtat"].IsString())
		{
			checkError.push_back(item["checkError"].GetBool());
			listStatus.push_back(item["NomEtat"].GetString());
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------
// Applique le changement d'etat d'une page, xrail, lors de la fermeture
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::SetStatus_v2(const PMString& address, const K2Vector<int32>& tabidspages, const PMString& status, const PMString& userID)
{
	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();

	Value myarray(kArrayType);

	for (int i = 0; i < tabidspages.size(); ++i) {
		Value obj(kObjectType);
		obj.AddMember("id", tabidspages[i], allocator);
		myarray.PushBack(obj, allocator);
	}

	Value statusValue = convert_PMStringToUTF8(status, allocator);
	doc.AddMember("listepages", myarray, allocator);
	doc.AddMember("status", statusValue, allocator);
	doc.AddMember("userID", Value().SetString(userID.GetPlatformString().c_str(), allocator), allocator);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	// 2. Préparation de l'URL
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_SetStatus";

	// 3. cURL init
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;


	return true;
}


//----------------------------------------------------------------------------------------------------------------
// Change l'etat d'une image (palette Photos liees). Corps JSON {idimage, idetat}, le serveur repond "ok".
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::SetImageState_v2(const PMString& address, const int32& idImage, const int32& idEtat)
{
	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember("idimage", idImage, allocator);
	doc.AddMember("idetat", idEtat, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	// 2. Preparation de l'URL
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_SetImageState";

	// 3. cURL init
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;

	// Le serveur renvoie juste "ok" (texte brut ou eventuellement entoure de
	// guillemets/espaces). On considere l'appel reussi si "ok" apparait.
	return responseStr.find("ok") != std::string::npos;
}


//----------------------------------------------------------------------------------------------------------------
// idem qu'au dessus, avec l'option de rigueur en plus. Le WS au dessus sera supprime a terme
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::GetListePubs_v2(const PMString& address, const K2Vector<int32>& tabidspages, K2Vector<int32>& TabPagesIDS,
	K2Vector<int32>& TabFolios, K2Vector<PMString>& TabResaIDs, K2Vector<PMString>& TabPubsIDS,
	K2Vector<PMReal>& TabHauteurs, K2Vector<PMReal>& TabLargeurs, K2Vector<PMReal>& TabX, K2Vector<PMReal>& TabY,
	K2Vector<PMString>& TabAnnonceurs, K2Vector<PMString>& TabCheminImage, K2Vector<bool16>& TabVerrouPos)
{

	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();

	Value myarray(kArrayType);

	for (int i = 0; i < tabidspages.size(); ++i) {
		Value obj(kObjectType);
		obj.AddMember("id", tabidspages[i], allocator);
		myarray.PushBack(obj, allocator);
	}

	doc.AddMember("listepages", myarray, allocator);
	doc.AddMember("onwindows", onwindows, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	// 2. Préparation de l'URL
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetListePubs";

	// 3. cURL init
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;

	if (doc.Parse(responseStr.c_str()).HasParseError())
		return false;


	// Récupérer les données du JSON
	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("resaid") && item["resaid"].IsString())
		{

			TabResaIDs.push_back(item["resaid"].GetString());
			TabPubsIDS.push_back(item["pubid"].GetString());
			TabPagesIDS.push_back(item["pageid"].GetInt());
			TabFolios.push_back(item["folio"].GetInt());
			TabHauteurs.push_back(PMReal(strtod(item["hauteur"].GetString(), NULL)));
			TabLargeurs.push_back(PMReal(strtod(item["largeur"].GetString(), NULL)));
			TabX.push_back(PMReal(strtod(item["x"].GetString(), NULL)));
			TabY.push_back(PMReal(strtod(item["y"].GetString(), NULL)));
			TabAnnonceurs.push_back(item["annonceur"].GetString());
			TabCheminImage.push_back(item["chemin"].GetString());
			TabVerrouPos.push_back(item["rigueur"].GetBool());

		}
	}
	return true;


}

//----------------------------------------------------------------------------------------------------------------
// demande a xrail de recuperer le pdf qui vient d'etre genere
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::MovePDFFile_v2(const PMString& address, const int32& pageID, const PMString& fileName, const int32& IDUser)
{
	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember("onwindows", onwindows, allocator);
	doc.AddMember("idpage", pageID, allocator);
	doc.AddMember("userID", IDUser, allocator);
	doc.AddMember("fileName", Value().SetString(fileName.GetPlatformString().c_str(), allocator), allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	// 2. Préparation de l'URL
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_PDF_RecupFile";

	// 3. cURL init
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;

	return true;
}

//----------------------------------------------------------------------------------------------------------------
// demande a xrail le nom, l'endroit et le profil pdf pour generer un pdf via le menu
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::GetPDFFileName_v2(const PMString& address, const int32& pageID, PMString& profilPDF, PMString& pdfFileName)
{
	std::string serverURL = address.GetPlatformString().c_str();

	// Construire l'URL avec les paramètres en GET
	std::string url = serverURL + "/Plugin_PDF_GetFileName?idpage=" + std::to_string(pageID) + "&onwindows=" + std::to_string(onwindows);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;


	// Récupérer les données du JSON
	if (doc.HasMember("profilpdf") && doc["profilpdf"].IsString()) {
		profilPDF = doc["profilpdf"].GetString();
	}
	if (doc.HasMember("nomfichier") && doc["nomfichier"].IsString()) {
		pdfFileName = doc["nomfichier"].GetString();
	}

	return true;
}


//----------------------------------------------------------------------------------------------------------------
// met a jour dans xrail les infos d'une pub (coordonnees + path)
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::SetPub_v2(const PMString& address, const PMString& IDPub, const int32& IDPage, const PMString& x, const PMString& y, const PMString& l, const PMString& h, const PMString& path)
{

	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember("onwindows", onwindows, allocator);
	doc.AddMember("idpage", IDPage, allocator);
	doc.AddMember("IDPub", Value().SetString(IDPub.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("x", Value().SetString(x.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("y", Value().SetString(y.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("largeur", Value().SetString(l.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("hauteur", Value().SetString(h.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("filepath", Value().SetString(path.GetPlatformString().c_str(), allocator), allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	// 2. Préparation de l'URL
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_SetPub";

	// 3. cURL init
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;

	return true;
}

//----------------------------------------------------------------------------------------------------------------
// Sauvegarde de tous les articles d'une page
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::SaveStories_v2(const PMString& address, const K2Vector<int32>& tabidspages, const PMString from, K2Vector<int32>& ListePages, K2Vector<int32>& TabIDStories, K2Vector<int32>& TabTypes,
	K2Vector<int32>& TabNbSignes,
	K2Vector<int32>& IDStoriesByBloc, K2Vector<PMString>& TabX, K2Vector<PMString>& TabY,
	K2Vector<PMString>& TabLargeur, K2Vector<PMString>& TabHauteur, K2Vector<int32>& TabIDPage,
	K2Vector<PMString>& ListeImages, K2Vector<WideString>& ListeLegendes, K2Vector<WideString>& ListeCredits, PMString& result)
{

	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();
	Value tabStories(kArrayType);
	Value tabBlocs(kArrayType);
	Value tabImages(kArrayType);
	Value tabPages(kArrayType);

	for (int i = 0; i < tabidspages.size(); ++i) {
		Value obj(kObjectType);
		obj.AddMember("id", tabidspages[i], allocator);
		tabPages.PushBack(obj, allocator);
	}


	for (int i = 0; i < TabIDStories.size(); ++i) {
		Value obj(kObjectType);
		obj.AddMember("idstory", TabIDStories[i], allocator);
		obj.AddMember("type", TabTypes[i], allocator);
		obj.AddMember("nbsignes", TabNbSignes[i], allocator);
		tabStories.PushBack(obj, allocator);
	}

	for (int i = 0; i < IDStoriesByBloc.size(); ++i) {
		Value obj(kObjectType);
		obj.AddMember("idstory", IDStoriesByBloc[i], allocator);
		obj.AddMember("x", Value().SetString(TabX[i].GetPlatformString().c_str(), allocator), allocator);
		obj.AddMember("y", Value().SetString(TabY[i].GetPlatformString().c_str(), allocator), allocator);
		obj.AddMember("largeur", Value().SetString(TabLargeur[i].GetPlatformString().c_str(), allocator), allocator);
		obj.AddMember("hauteur", Value().SetString(TabHauteur[i].GetPlatformString().c_str(), allocator), allocator);
		obj.AddMember("idpage", TabIDPage[i], allocator);
		tabBlocs.PushBack(obj, allocator);
	}

	for (int i = 0; i < ListeImages.size(); ++i) {
		Value obj(kObjectType);
		obj.AddMember("idimage", Value().SetString(ListeImages[i].GetPlatformString().c_str(), allocator), allocator);
		PMString legPM(ListeLegendes[i]);
		obj.AddMember("legende", Value().SetString(legPM.GetPlatformString().c_str(), allocator), allocator);
		PMString crPM(ListeCredits[i]);
		obj.AddMember("credit", Value().SetString(crPM.GetPlatformString().c_str(), allocator), allocator);
		tabImages.PushBack(obj, allocator);
	}

	doc.AddMember("from", Value().SetString(from.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("pages", tabPages, allocator);
	doc.AddMember("stories", tabStories, allocator);
	doc.AddMember("blocs", tabBlocs, allocator);
	//doc.AddMember("images", tabImages, allocator);
	doc.AddMember("onwindows", onwindows, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	// 2. Préparation de l'URL
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_SaveStories";

	// 3. cURL init
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;

	if (doc.Parse(responseStr.c_str()).HasParseError())
		return false;


	return true;

}



//----------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------
bool XRailWebServices::GetListeIDMS_v2(const PMString& address, const K2Vector<int32>& tabidspages, K2Vector<int32>& TabIDStories, K2Vector<int32>& TabPagesIDS,
	K2Vector<PMReal>& TabX, K2Vector<PMReal>& TabY, K2Vector<PMString>& TabIDMSpaths, K2Vector<PMString>& TabXMLStorypaths, K2Vector<PMString>& TabSubjects)
{

	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();
	Value myarray(kArrayType);

	for (int i = 0; i < tabidspages.size(); ++i) {
		Value obj(kObjectType);
		obj.AddMember("id", tabidspages[i], allocator);
		myarray.PushBack(obj, allocator);
	}

	doc.AddMember("listepages", myarray, allocator);
	doc.AddMember("onwindows", onwindows, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	// 2. Préparation de l'URL
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetListeIDMS";

	// 3. cURL init
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;

	if (doc.Parse(responseStr.c_str()).HasParseError())
		return false;


	// Récupérer les données du JSON
	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("idstory") && item["idstory"].IsInt())
		{
			TabIDStories.push_back(item["idstory"].GetInt());
			TabPagesIDS.push_back(item["idpage"].GetInt());
			TabX.push_back(PMReal(strtod(item["x"].GetString(), NULL)));
			TabY.push_back(PMReal(strtod(item["y"].GetString(), NULL)));
			TabIDMSpaths.push_back(item["idmspath"].GetString());
			TabXMLStorypaths.push_back(item["storypath"].GetString());
			TabSubjects.push_back(item["subject"].GetString());
		}
	}
	return true;
}

bool XRailWebServices::GetPageInfos_v2(const PMString& address, const int32 pageID,
	int32& titreID,
	PMString& titreLib,
	PMString& parutionLib) {


	std::string serverURL = address.GetPlatformString().c_str();

	// Construire l'URL avec les paramètres en GET
	std::string url = serverURL + "/Plugin_GetInfosPages?idpage=" + std::to_string(pageID);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;


	// Récupérer les données du JSON
	if (doc.HasMember("nomparu") && doc["nomparu"].IsString()) {
		parutionLib = doc["nomparu"].GetString();
	}
	if (doc.HasMember("nomtitre") && doc["nomtitre"].IsString()) {
		titreLib = doc["nomtitre"].GetString();
	}
	if (doc.HasMember("idtitre") && doc["idtitre"].IsInt()) {
		titreID = doc["idtitre"].GetInt();
	}

	return true;

}


bool XRailWebServices::UpdateStory_v2(const PMString& address, const PMString& storyID, const PMString& userLogin, int32& canUpdate) {


	std::string serverURL = address.GetPlatformString().c_str();
	std::string login = userLogin.GetPlatformString().c_str();
	std::string id = storyID.GetPlatformString().c_str();

	// Construire l'URL avec les paramètres en GET
	std::string url = serverURL + "/Plugin_UpdateStory?idstory=" + id + "&login=" + login;

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;


	// Récupérer les données du JSON
	if (doc.HasMember("canupdate") && doc["canupdate"].IsInt()) {
		canUpdate = doc["canupdate"].GetInt();
	}

	return true;

}

bool XRailWebServices::GetUsersAffectations_v2(const PMString& address, const int32& idPage, K2Vector<PMString>& usersid, K2Vector<PMString>& usersnames)
{
	// Construire l'URL avec les paramètres en GET
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_GetAffectationsUsers?idpage=" + std::to_string(idPage);

	Document doc;
	if (!CallWebServiceAndParseJson(url, doc))
		return false;

	// Récupérer les données du JSON
	for (SizeType i = 0; i < doc.Size(); i++) {
		const Value& item = doc[i];

		if (item.HasMember("id") && item["id"].IsString() &&
			item.HasMember("user") && item["user"].IsString())
		{
			usersid.push_back(item["id"].GetString());
			usersnames.push_back(item["user"].GetString());
		}
	}

	return true;
}


bool XRailWebServices::CreateAffectation_v2(const PMString& address, const PMString& idUserDest,
	const PMString& subject,
	const PMString& idSection,
	const PMString& idSousSection,
	const int32 nbPhotos,
	const PMString& nbSignesEstim,
	PMString& idStory,
	PMString& storyFullPath) {

	std::string iduser = idUserDest.GetPlatformString().c_str();

	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();
	
	doc.AddMember("onwindows", onwindows, allocator);
	doc.AddMember("idUserDest", Value().SetString(idUserDest.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("sujet", Value().SetString(subject.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("nbsignes", Value().SetString(idSousSection.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("section",  Value().SetString(nbSignesEstim.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("soussection", Value().SetString(idSection.GetPlatformString().c_str(), allocator), allocator);
	doc.AddMember("nbphotos", nbPhotos, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	// 2. Préparation de l'URL
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_CreateAffectation";

	// 3. cURL init
	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;

	if (doc.Parse(responseStr.c_str()).HasParseError())
		return false;


	
	// Récupérer les données du JSON
	if (doc.HasMember("idStory") && doc["idStory"].IsString()) {
		idStory = doc["idStory"].GetString();
	}
	if (doc.HasMember("fullpath") && doc["fullpath"].IsString()) {
		storyFullPath = doc["fullpath"].GetString();
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------
// Plugin_Comm_List — POST JSON { listepages:[{id},...], onwindows } pour toutes
// les pages du document ; réponse = tableau JSON plat, un élément par commentaire.
//----------------------------------------------------------------------------------------------------------------

bool XRailWebServices::GetCommentaires_v2(const PMString& address, const K2Vector<int32>& IDPages,
	K2Vector<int32>& IDsComm,
	K2Vector<int32>& IDsPage,
	K2Vector<PMString>& Commentaires,
	K2Vector<PMString>& DatesISO,
	K2Vector<int32>& HeuresSecs,
	K2Vector<PMString>& NomsUser,
	K2Vector<bool>& Checks,
	K2Vector<PMReal>& Xs,
	K2Vector<PMReal>& Ys,
	bool& fromPhp)
{
	IDsComm.clear();
	IDsPage.clear();
	Commentaires.clear();
	DatesISO.clear();
	HeuresSecs.clear();
	NomsUser.clear();
	Checks.clear();
	Xs.clear();
	Ys.clear();
	fromPhp = false;

	if (IDPages.empty())
		return true;

	// Liste des pages envoyée en POST JSON, même format que Plugin_GetListePubs :
	// { "listepages": [ {"id": 1}, {"id": 2} ], "onwindows": 0/1 }.
	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();

	Value myarray(kArrayType);
	for (int i = 0; i < IDPages.size(); ++i) {
		Value obj(kObjectType);
		obj.AddMember("id", IDPages[i], allocator);
		myarray.PushBack(obj, allocator);
	}
	doc.AddMember("listepages", myarray, allocator);
	doc.AddMember("onwindows", onwindows, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string jsonBody = buffer.GetString();

	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_Comm_List";

	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonBody.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
		return false;

	// Réutilise doc pour parser la réponse : objet { fromPhp, comments:[...] }.
	if (doc.Parse(responseStr.c_str()).HasParseError())
		return false;

	if (!doc.IsObject())
		return false;

	// Source du jeu de données complet : true = PHP, false = 4D.
	if (doc.HasMember("fromPhp") && doc["fromPhp"].IsBool())
		fromPhp = doc["fromPhp"].GetBool();

	if (!doc.HasMember("comments") || !doc["comments"].IsArray())
		return true;	// pas de commentaires : fromPhp reste renseigné.

	const Value& comments = doc["comments"];
	for (SizeType i = 0; i < comments.Size(); ++i)
	{
		const Value& row = comments[i];
		if (!row.IsObject())
			continue;

		int32 idComm = 0;
		int32 idPage = 0;
		PMString commentaire;
		PMString dateISO;
		int32 heureSecs = 0;
		PMString nomUser;
		bool check = false;
		PMReal x = 0.0;
		PMReal y = 0.0;

		if (row.HasMember("IDComm") && row["IDComm"].IsInt())
			idComm = row["IDComm"].GetInt();
		if (row.HasMember("IDPage") && row["IDPage"].IsInt())
			idPage = row["IDPage"].GetInt();
		if (row.HasMember("Commentaire") && row["Commentaire"].IsString())
			commentaire = PMString(row["Commentaire"].GetString());
		if (row.HasMember("DateCommentaire") && row["DateCommentaire"].IsString())
			dateISO = PMString(row["DateCommentaire"].GetString());
		if (row.HasMember("HeureCommentaire") && row["HeureCommentaire"].IsInt())
			heureSecs = row["HeureCommentaire"].GetInt();
		if (row.HasMember("NomUser") && row["NomUser"].IsString())
			nomUser = PMString(row["NomUser"].GetString());
		if (row.HasMember("Check") && row["Check"].IsBool())
			check = row["Check"].GetBool();

		// Champs coord selon la source : PHP => coord_x/coord_y (déjà % page),
		// 4D => X/Y (Y % page, X % spread). Stockés bruts ; la normalisation du
		// X 4D se fait côté UI (besoin de la géométrie du document).
		if (fromPhp)
		{
			if (row.HasMember("coord_x") && row["coord_x"].IsNumber())
				x = PMReal(row["coord_x"].GetDouble());
			if (row.HasMember("coord_y") && row["coord_y"].IsNumber())
				y = PMReal(row["coord_y"].GetDouble());
		}
		else
		{
			if (row.HasMember("X") && row["X"].IsNumber())
				x = PMReal(row["X"].GetDouble());
			if (row.HasMember("Y") && row["Y"].IsNumber())
				y = PMReal(row["Y"].GetDouble());
		}

		IDsComm.push_back(idComm);
		IDsPage.push_back(idPage);
		Commentaires.push_back(commentaire);
		DatesISO.push_back(dateISO);
		HeuresSecs.push_back(heureSecs);
		NomsUser.push_back(nomUser);
		Checks.push_back(check);
		Xs.push_back(x);
		Ys.push_back(y);
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------
// Plugin_Comm_Valid — toggle de l'état "validé" d'un commentaire. Le serveur
// renvoie un corps vide : on fait un simple GET et on ignore la réponse.
//----------------------------------------------------------------------------------------------------------------

bool XRailWebServices::CommValid(const PMString& address, const int32& IDComm)
{
	std::string serverURL = address.GetPlatformString().c_str();
	std::string url = serverURL + "/Plugin_Comm_Valid?idcomm=" + std::to_string(IDComm);

	CURL* curl = curl_easy_init();
	if (!curl)
		return false;

	std::string responseStr;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
	ApplyCommonCurlOpts(curl);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	return (res == CURLE_OK);
}