
#ifndef __IXPGPreferences_h__
#define __IXPGPreferences_h__

#include "IPMUnknown.h"
#include <vector>
#include "XPGID.h"
#include "K2Vector.h"
#include "K2Vector.tpp"
#include "KeyValuePair.h"

//========================================================================================
// CLASS IXPGPreferences
//========================================================================================

typedef struct tag
{
	typedef object_type data_type;

	WideString value;
	PMString paraStyleName;
	int32 order;
	tag() : value("DEFAULT"), paraStyleName(kNullString), order(0) {}

} Tag;



typedef K2Vector<Tag> TagList;


// Image/article states fetched from Gaia (Plugin_GetPrefs response).
// Couleur is stored verbatim from the server: for EtatImage it's a string
// (hex like "#ff6e00" or named like "vert"); for EtatArticle the server
// sends both an HTML hex string and a packed RGB int — we keep both.
typedef struct
{
	int32 ID;
	int32 Ordre;
	PMString Nom;
	PMString Couleur;
} EtatImage;

typedef struct
{
	int32 ID;
	int32 Ordre;
	PMString Nom;
	PMString CouleurHTML;
	int32 Couleur;
	int32 Rayures;
} EtatArticle;

typedef std::vector<EtatImage> EtatImageList;
typedef std::vector<EtatArticle> EtatArticleList;


class IXPGPreferences : public IPMUnknown
{
public:

	enum { kDefaultIID = IID_IXPGPREFERENCES };

	// General section
	virtual PMString GetRacineArbo() const = 0;
	virtual void SetRacineArbo(const PMString& str) = 0;

	virtual Tag GetTagExport(const PMString& styleName) const = 0;
	virtual PMString GetStyleName(const PMString& tag) const = 0;
	virtual void SetListeTagExport(const TagList& l, const Tag& aDefaultTag) = 0;

	// Formes section
	virtual PMString GetCheminFormes() const = 0;
	virtual void SetCheminFormes(const PMString& str) = 0;

	virtual bool16 GetCreationForme() const = 0;
	virtual void SetCreationForme(const bool16& b) = 0;

	virtual void SetCheminAssemblageCartons(const PMString& b) = 0;
	virtual PMString GetCheminAssemblageCartons() = 0;

	// XRail section
	virtual PMString GetXRail_URL() const = 0;
	virtual void SetXRail_URL(const PMString& str) = 0;
	virtual PMString GetTEC_URL() const = 0;
	virtual void SetTEC_URL(const PMString& str) = 0;

		//Import des credits et des legendes
	virtual bool16 GetImportLegende() const = 0;
	virtual void SetImportLegende(const bool16& b) = 0;

	virtual bool16 GetImportCredit() const = 0;
	virtual void SetImportCredit(const bool16& b) = 0;

	// Matching Styles Preferences

	virtual void SetListeMatchingTag(K2Vector<Tag> listTags) = 0;
	virtual K2Vector<Tag> GetListeMatchingTag() = 0;

	//Preferences globales
	virtual int32 GetGestionIDMS() const = 0;
	virtual void SetGestionIDMS(const int32& b) = 0;

	virtual int32 GetIDMS_MAJIDMS() const = 0;
	virtual void SetIDMS_MAJIDMS(const int32& b) = 0;

	virtual int32 GetIDMSALLBLOCS() const = 0;
	virtual void SetIDMSALLBLOCS(const int32& b) = 0;

	// Flag de licence (vient de Gaia, clé JSON "ChangePictureState") : autorise
	// ou non le changement d'état d'une photo depuis la palette Photos liées.
	// Bloqué par défaut (0) : l'option n'est active que si Gaia renvoie 1.
	virtual int32 GetChangePictureState() const = 0;
	virtual void SetChangePictureState(const int32& b) = 0;

	// Etats (image/article statuses) fetched from Gaia at startup and after
	// XRail login. Used by the Photos Liées palette and other XPage UI to
	// render status colors and labels consistently with the server config.
	virtual EtatImageList GetEtatsImages() const = 0;
	virtual void SetEtatsImages(const EtatImageList& l) = 0;

	virtual EtatArticleList GetEtatsArticles() const = 0;
	virtual void SetEtatsArticles(const EtatArticleList& l) = 0;

    // Don't create update file
    virtual PMString GetNoUpdateFile() const =0;
    virtual void SetNoUpdateFile(const PMString& str) =0;

	// "SERVEUR" value from Gaia.ini — identifies this client workstation to
	// the server. Stored as PMString because XPage's web service takes it as
	// a string (XRail's variant takes it as int; callers can convert with
	// GetAsNumber()).
	virtual PMString GetPluginServerName() const = 0;
	virtual void SetPluginServerName(const PMString& name) = 0;
};

#endif // __IXPGPreferences_h__
