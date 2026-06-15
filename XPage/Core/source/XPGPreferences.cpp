#include "VCPluginHeaders.h"

// Interface includes:
#include "IXPGPreferences.h"

// General includes:
#include "CAlert.h"

// Project includes:
#include "XPGID.h"


/** Implements IStartupShutdownService; purpose is to install the idle task.
*/
class XPGPreferences : public CPMUnknown<IXPGPreferences>
{
public:

	/**	Constructor
		@param boss
	 */
	XPGPreferences(IPMUnknown* boss);

	/**	Destructor
	 */
	virtual ~XPGPreferences();

	// General section
	virtual PMString GetRacineArbo() const;
	virtual void SetRacineArbo(const PMString& str);

	virtual Tag GetTagExport(const PMString& styleName) const;
	virtual PMString GetStyleName(const PMString& tag) const;
	virtual void SetListeTagExport(const TagList& l, const Tag& aDefaultTag);

	// Formes section
	virtual PMString GetCheminFormes() const;
	virtual void SetCheminFormes(const PMString& str);

	virtual bool16 GetCreationForme() const;
	virtual void SetCreationForme(const bool16& b);

	virtual void SetCheminAssemblageCartons(const PMString& b);
	virtual PMString GetCheminAssemblageCartons();

	// XRail section
	virtual PMString GetXRail_URL() const;
	virtual void SetXRail_URL(const PMString& str);
	virtual PMString GetTEC_URL() const;
	virtual void SetTEC_URL(const PMString& str);


	//Import des credits et des legendes
	virtual bool16 GetImportLegende() const;
	virtual void SetImportLegende(const bool16& b);

	virtual bool16 GetImportCredit() const;
	virtual void SetImportCredit(const bool16& b);

	// Matching Styles Preferences

	virtual void SetListeMatchingTag(K2Vector<Tag> listTags);
	virtual K2Vector<Tag> GetListeMatchingTag();

	//Preferences globales
	virtual int32 GetGestionIDMS() const;
	virtual void SetGestionIDMS(const int32& b);

	virtual int32 GetIDMS_MAJIDMS() const;
	virtual void SetIDMS_MAJIDMS(const int32& b);

	virtual int32 GetIDMSALLBLOCS() const;
	virtual void SetIDMSALLBLOCS(const int32& b);

	virtual int32 GetChangePictureState() const;
	virtual void SetChangePictureState(const int32& b);

	virtual EtatImageList GetEtatsImages() const;
	virtual void SetEtatsImages(const EtatImageList& l);

	virtual EtatArticleList GetEtatsArticles() const;
	virtual void SetEtatsArticles(const EtatArticleList& l);

    virtual PMString GetNoUpdateFile() const;
    virtual void SetNoUpdateFile(const PMString& str);

	virtual PMString GetPluginServerName() const;
	virtual void SetPluginServerName(const PMString& name);

private:

	PMString racineArbo,  noUpdateFile;
	bool16 creationForme;
	bool16 importCredit, importLegende;
	int32 gestionIDMS, majIDMS, idmsAllBlocs, changePictureState;
	PMString status;
    PMString cheminFormes, cheminAssemblageCartons;
	PMString urlXR, urlTEC;
	PMString pluginServerName;

	K2Vector<Tag> matchingTagsList;
	TagList tagList;
	Tag defaultTag;

	EtatImageList etatsImages;
	EtatArticleList etatsArticles;

};

CREATE_PMINTERFACE(XPGPreferences, kXPGPreferencesImpl)


/**	Constructor
 */
	XPGPreferences::XPGPreferences(IPMUnknown* boss) : CPMUnknown<IXPGPreferences>(boss)
{
	racineArbo = kNullString;
	tagList.clear();
	defaultTag = Tag();
	cheminFormes = cheminAssemblageCartons = kNullString;
	creationForme = kFalse;
	urlXR = kNullString;
	urlTEC = kNullString;
	pluginServerName = "0";
	matchingTagsList.clear();
	etatsImages.clear();
	etatsArticles.clear();
	changePictureState = 0; // bloqué par défaut tant que Gaia n'envoie pas 1
}

/**	Destructor
 */
XPGPreferences::~XPGPreferences()
{
}

PMString XPGPreferences::GetRacineArbo() const
{
	return racineArbo;
}

void XPGPreferences::SetRacineArbo(const PMString& str)
{
	racineArbo = str;
}

Tag XPGPreferences::GetTagExport(const PMString& styleName) const
{
	if (styleName.IsEmpty())
		return defaultTag;

	for (int32 i = 0; i < tagList.size(); ++i)
	{
		if (styleName.Compare(kFalse, tagList[i].paraStyleName) == 0)
		{
			return tagList[i];
		}
	}

	return defaultTag;
}


PMString XPGPreferences::GetStyleName(const PMString& tag) const
{
	if (tag.IsEmpty())
		return kNullString;

	for (int32 i = 0; i < tagList.size(); ++i)
	{
		if (tag.Compare(kFalse, tagList[i].value) == 0)
		{
			return tagList[i].paraStyleName;
		}
	}

	return kNullString;
}


void XPGPreferences::SetListeTagExport(const TagList& l, const Tag& aDefaultTag)
{
	tagList = l;
	defaultTag = aDefaultTag;
}

PMString XPGPreferences::GetCheminFormes() const
{
	return cheminFormes;
}

void XPGPreferences::SetCheminFormes(const PMString& str)
{
	cheminFormes = str;
}

bool16 XPGPreferences::GetCreationForme() const
{
	return creationForme;
}

void XPGPreferences::SetCreationForme(const bool16& b)
{
	creationForme = b;
}

PMString XPGPreferences::GetXRail_URL() const
{
	return urlXR;
}

void XPGPreferences::SetXRail_URL(const PMString& str)
{
	urlXR = str;
}


PMString XPGPreferences::GetTEC_URL() const
{
	return urlTEC;
}

void XPGPreferences::SetTEC_URL(const PMString& str)
{
	urlTEC = str;
}


bool16 XPGPreferences::GetImportLegende() const
{
	return importLegende;
}

void XPGPreferences::SetImportLegende(const bool16& b)
{
	importLegende = b;
}
bool16 XPGPreferences::GetImportCredit() const
{
	return importCredit;
}
void XPGPreferences::SetImportCredit(const bool16& b)
{
	importCredit = b;
}


void XPGPreferences::SetListeMatchingTag(K2Vector<Tag> listTags)
{
	this->matchingTagsList = listTags;
}

K2Vector<Tag> XPGPreferences::GetListeMatchingTag()
{
	return matchingTagsList;
}

void XPGPreferences::SetCheminAssemblageCartons(const PMString& b)
{
	cheminAssemblageCartons = b;
}

PMString XPGPreferences::GetCheminAssemblageCartons()
{
	return cheminAssemblageCartons;
}

//Preferences globales
int32 XPGPreferences::GetGestionIDMS() const
{
	return gestionIDMS;
}

void XPGPreferences::SetGestionIDMS(const int32& b)
{
	gestionIDMS = b;
}
int32 XPGPreferences::GetIDMS_MAJIDMS() const
{
	return majIDMS;
}

void XPGPreferences::SetIDMS_MAJIDMS(const int32& b)
{
	majIDMS = b;
}
int32 XPGPreferences::GetIDMSALLBLOCS() const
{
	return idmsAllBlocs;
}

void XPGPreferences::SetIDMSALLBLOCS(const int32& b)
{
	idmsAllBlocs = b;
}

int32 XPGPreferences::GetChangePictureState() const
{
	return changePictureState;
}

void XPGPreferences::SetChangePictureState(const int32& b)
{
	changePictureState = b;
}

EtatImageList XPGPreferences::GetEtatsImages() const
{
	return etatsImages;
}

void XPGPreferences::SetEtatsImages(const EtatImageList& l)
{
	etatsImages = l;
}

EtatArticleList XPGPreferences::GetEtatsArticles() const
{
	return etatsArticles;
}

void XPGPreferences::SetEtatsArticles(const EtatArticleList& l)
{
	etatsArticles = l;
}

PMString XPGPreferences::GetNoUpdateFile() const
{
    return noUpdateFile;

}
void XPGPreferences::SetNoUpdateFile(const PMString& str)
{
    noUpdateFile = str;
}

PMString XPGPreferences::GetPluginServerName() const
{
	return pluginServerName;
}

void XPGPreferences::SetPluginServerName(const PMString& name)
{
	pluginServerName = name;
}
