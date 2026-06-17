
#ifndef __IWebServices__
#define __IWebServices__

#include "IPMUnknown.h"
#include "XRCID.h"
#include "K2Vector.tpp"

#include "K2Vector.h"
#include "KeyValuePair.h"
#include <rapidjson/document.h>

using namespace rapidjson;
typedef KeyValuePair<PMString, PMString> IDValuePair;
typedef K2Vector<IDValuePair > IDValueList;

/**    Interface for XRailWebServices.
 */
class IWebServices : public IPMUnknown
{
public:
	enum { kDefaultIID = IID_IWEBSERVICES };

	virtual Value convert_PMStringToUTF8(PMString mystring, Document::AllocatorType& allocator) = 0;

	virtual bool CallWebServiceAndParseJson(const std::string& url, rapidjson::Document& doc) = 0;

	virtual bool VerifLogin_v2(const PMString& address, const PMString& login, const PMString& pwd, PMString& userID, PMString& errorCode, int& VoirPDF, int& VoirVignettes) = 0;

	// adTemplateNoVisual/WithVisual: absolute paths to optional IDMS templates
	// used by XRailResponder::ImportPublicites when inserting ad blocks. Empty
	// strings (or unreachable paths) make the importer fall back to its
	// legacy hard-coded creation path. Server may simply omit the JSON keys.
	virtual bool GetPrefsXRail_v2(const PMString& address, const int32& fromServeur, int32& prefs_ImportPub, int32& prefs_PubProp, int32& prefs_PDFMultipages, int32& prefs_ExportIDML, int32& prefs_MakePreview, PMString& prefs_AdTemplateNoVisual, PMString& prefs_AdTemplateWithVisual) = 0;

	virtual bool GetListeTitres_v2(const PMString& address, const int32& userID, K2Vector<PMString>& nomstitres, K2Vector<int32>& idstitres) = 0;

	virtual bool GetListeParu_v2(const PMString& address, const int32& idtitre, K2Vector<PMString>& nomsparus, K2Vector<int32>& idparus) = 0;

	virtual bool GetListeEdi_v2(const PMString& address, const int32& idparu, K2Vector<PMString>& nomsedis, K2Vector<int32>& idsedis) = 0;

	virtual bool GetListePages_v2(const PMString& address, const int32& idedi, int32 nbPages, K2Vector<PMString>& nomspages, K2Vector<int32>& idspages) = 0;

	virtual bool GetListeRub_v2(const PMString& address, const int32& idtitre, K2Vector<PMString>& nomsrubs, K2Vector<int32>& idrubs) = 0;

	virtual bool GetListeSSRub_v2(const PMString& address, const PMString& idsection, K2Vector<PMString>& nomsssrubs, K2Vector<int32>& idssrubs) = 0;

	virtual bool GetPreviewFolder_v2(const PMString& address, const int32& IDPage, PMString& ClientPictPath, int32& Scale, int32& resolution) = 0;

	// blocPageIDs / blocX / blocY / blocLargeur / blocHauteur : tableaux paralleles
	// decrivant les blocs sans ID Gaia (un element par bloc). Regroupes par page
	// dans le JSON envoye (cle "blocs").
	virtual bool RecupPreviews_v2(const PMString& address, const K2Vector<PMString>& FilePath, const K2Vector<int32>& IDPage,
		const K2Vector<int32>& blocPageIDs, const K2Vector<PMString>& blocX, const K2Vector<PMString>& blocY,
		const K2Vector<PMString>& blocLargeur, const K2Vector<PMString>& blocHauteur) = 0;

	// On success populates listPagesIDS (one IDPage per page in the linked
	// document) and listFolios (one FolioReel per page; same length as
	// listPagesIDS). listRoulettes carries an arbitrary number of TCL
	// snippets (Pagemakeup [HS=…]…[HR] commands) that must be applied to
	// the document after linking. All vectors are cleared on entry; the
	// server may omit any of the JSON keys, in which case the corresponding
	// vector remains empty.
	virtual bool LinkDocument_v2(const PMString& address, const int32& pageID, K2Vector<int32>& listPagesIDS, K2Vector<int32>& listFolios, K2Vector<PMString>& listRoulettes, PMString& docFileName) = 0;

	virtual bool UnlinkDocument_v2(const PMString& address, const int32& IDPage, PMString& docFileName) = 0;

	virtual bool SetVerrou_v2(const PMString& address, const K2Vector<int32>& IDPage, int lock, const PMString& userID, PMString& status, bool errorInDoc) = 0;

	virtual bool GetListeStatus_v2(const PMString& address, const PMString& userID, K2Vector<PMString>& listStatus, K2Vector<bool>& checkError) = 0;

	virtual bool SetStatus_v2(const PMString& address, const K2Vector<int32>& tabidspages, const PMString& status, const PMString& userID) = 0;

	// Change l'etat d'une image (palette Photos liees). POST Plugin_SetImageState
	// avec un corps JSON {"idimage": <id>, "idetat": <id>}. Le serveur repond "ok".
	// Retourne true si la reponse contient "ok".
	virtual bool SetImageState_v2(const PMString& address, const int32& idImage, const int32& idEtat) = 0;

	virtual bool GetListePubs_v2(const PMString& address, const K2Vector<int32>& IDPages, K2Vector<int32>& TabPagesIDS,
		K2Vector<int32>& TabFolios, K2Vector<PMString>& TabResaIDs, K2Vector<PMString>& TabPubsIDS,
		K2Vector<PMReal>& TabHauteurs, K2Vector<PMReal>& TabLargeurs, K2Vector<PMReal>& TabX,
		K2Vector<PMReal>& TabY, K2Vector<PMString>& TabAnnonceurs, K2Vector<PMString>& TabCheminImage, K2Vector<bool16>& TabVerrouPos) = 0;

	virtual bool MovePDFFile_v2(const PMString& address, const int32& IDPage, const PMString& fileName, const int32& IDUser) = 0;

	virtual bool GetPDFFileName_v2(const PMString& address, const int32& IDPage, PMString& profilPDF, PMString& pdfFileName) = 0;

	virtual bool SetPub_v2(const PMString& address, const PMString& IDPub, const int32& IDPage, const PMString& x, const PMString& y, const PMString& l, const PMString& h, const PMString& path) = 0;



	// etatsImages_* and etatsArticles_* are parallel arrays populated from the
	// "etatsimages"/"etatsarticles" JSON arrays returned by Plugin_GetPrefs.
	// They're cleared on entry. If the server omits them the vectors stay empty.
	virtual bool GetPrefsXPage_v2(const PMString& address, const PMString& fromServeur, int32& prefs_ExportXML, int32& prefs_GestionIDMS, int32& prefs_IDMSMAJIDMS, int32& prefs_IDMSALLBLOCS, int32& prefs_ImportLegendes, int32& prefs_ImportCredits, int32& prefs_ChangePictureState,
		K2Vector<int32>& etatsImages_IDs, K2Vector<int32>& etatsImages_Ordres, K2Vector<PMString>& etatsImages_Noms, K2Vector<PMString>& etatsImages_Couleurs,
		K2Vector<int32>& etatsArticles_IDs, K2Vector<int32>& etatsArticles_Ordres, K2Vector<PMString>& etatsArticles_Noms, K2Vector<PMString>& etatsArticles_CouleursHTML, K2Vector<int32>& etatsArticles_Couleurs, K2Vector<int32>& etatsArticles_Rayures) = 0;

	virtual bool GetListeIDMS_v2(const PMString& address, const K2Vector<int32>& IDPages, K2Vector<int32>& TabIDStories, K2Vector<int32>& TabPagesIDS,
		K2Vector<PMReal>& TabX, K2Vector<PMReal>& TabY, K2Vector<PMString>& TabIDMSpaths, K2Vector<PMString>& TabXMLStorypaths, K2Vector<PMString>& TabSubjects) = 0;

	virtual bool GetPageInfos_v2(const PMString& address, const int32 pageID,
		int32& titreID,
		PMString& titreLib,
		PMString& parutionLib) = 0;

	virtual bool UpdateStory_v2(const PMString& address, const PMString& storyID, const PMString& userLogin, int32& canUpdate) = 0;


	virtual bool GetStories_v2(const PMString& address, const int32 idParutionMEP,
		const PMString& idSection,
		const PMString& idSubSection,
		K2Vector<PMString>& IDStories,
		K2Vector<PMString>& ListSujets,
		K2Vector<PMString>& CheminArticles,
		K2Vector<PMString>& CouleurStatus,
		K2Vector<PMString>& LibStatus) = 0;

	virtual bool SaveStories_v2(const PMString& address, const K2Vector<int32>& tabidspages, const PMString from, K2Vector<int32>& ListePages, K2Vector<int32>& TabIDStories, K2Vector<int32>& TabTypes,
		K2Vector<int32>& TabNbSignes,
		K2Vector<int32>& IDStoriesByBloc, K2Vector<PMString>& TabX, K2Vector<PMString>& TabY,
		K2Vector<PMString>& TabLargeur, K2Vector<PMString>& TabHauteur, K2Vector<int32>& TabIDPage,
		K2Vector<PMString>& ListeImages, K2Vector<WideString>& ListeLegendes, K2Vector<WideString>& ListeCredits, PMString& result) = 0;


	virtual bool GetStoryInfo_v2(const PMString& address, const PMString& idStory, PMString& colorStatus, PMString& sujet) = 0;

	virtual bool GetUsersAffectations_v2(const PMString& address, const int32& idPage, K2Vector<PMString>& usersid, K2Vector<PMString>& usersnames) = 0;

	virtual bool CreateAffectation_v2(const PMString& address, const PMString& idUserDest,
		const PMString& subject,
		const PMString& idSection,
		const PMString& idSousSection,
		const int32 nbPhotos,
		const PMString& nbSignesEstim,
		PMString& idStory,
		PMString& storyFullPath) = 0;


	//INCOPY
	virtual bool StoryLockChanged(const PMString& idArt, const PMString& flagLock) = 0;

	virtual bool GetStoryStates(const PMString& idArt,
		const PMString& user,
		IDValueList& StatusValuesList,
		K2Vector<PMString>& ColorsList,
		K2Vector<PMString>& StatusOrderList,
		K2Vector<PMString>& StatusRoleList,
		K2Vector<PMString>& StatusRegleList,
		K2Vector<PMString>& CommentaireList,
		PMString& result) = 0;


	virtual bool GetGroupsStatusList(const PMString& IDStory,
		const PMString& author,
		const PMString& status,
		IDValueList& StatusValuesList,
		PMString& result) = 0;

	virtual bool GetUsersStatusList(const PMString& author,
		const PMString& idArt,
		const PMString& status,
		const PMString& group,
		IDValueList& usersValuesList,
		PMString& result) = 0;

	virtual bool StoryStatusChanged(const PMString& storyID,
		const PMString& statusID,
		const PMString& userID,
		const PMString& groupID,
		PMString& result) = 0;

	// Plugin_Comm_List — fetches the comment list for all Gaia pages of the
	// document. IDPages is the list of Gaia page IDs, sent as a POST JSON body
	// { listepages:[{id:..},..], onwindows:.. } — same shape as
	// Plugin_GetListePubs. Returns parallel arrays (one entry per comment, all
	// pages merged). Empty arrays on success-with-no-comments. False on
	// HTTP/parse error. Vectors are cleared on entry so callers can reuse the
	// same buffers across calls.
	//
	// fromPhp (out): true if the whole dataset was served by PHP, false if it
	// came from 4D — the caller post-processes differently depending on the
	// source. Read from the top-level "fromPhp" field of the response.
	//
	// IDsPage (out): the Gaia page ID each comment belongs to (0 when the
	// comment has no location). The caller uses it to place the pin on the
	// right page and to normalize the X coordinate (4D X is spread-relative).
	//
	// Coordinate fields depend on the source: PHP sends coord_x / coord_y
	// (already page-relative %), 4D sends X / Y (Y page-relative %, X
	// spread-relative %). Both are stored raw into Xs / Ys; the caller
	// normalizes X when !fromPhp.
	//
	// Server returns a JSON object { "fromPhp": bool, "comments": [ ... ] }
	// where each comment element has: IDComm (int), IDPage (int),
	// Commentaire (string), DateCommentaire (ISO string), HeureCommentaire
	// (int — seconds since midnight), NomUser (string), Check (bool), and the
	// coordinate fields above.
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
		bool& fromPhp) = 0;

	// Plugin_Comm_Valid — bascule l'état "validé" d'un commentaire côté serveur
	// (toggle). Appelé quand l'utilisateur coche/décoche la case d'une ligne de
	// la palette Commentaires. Fire-and-forget : le serveur renvoie un corps
	// vide, on ne parse rien. Retourne false sur erreur HTTP.
	virtual bool CommValid(const PMString& address, const int32& IDComm) = 0;





};

#endif // __IWebServices__




