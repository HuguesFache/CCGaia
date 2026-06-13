
#ifndef __IXRailPrefsData__
#define __IXRailPrefsData__

#include "IPMUnknown.h"
#include "XRLID.h"

/**	Interface for XRail preferences data.
 */
class IXRailPrefsData : public IPMUnknown
{
public:
	enum { kDefaultIID = IID_IXRAILPREFSDATA };

	/** 
	*/
	virtual void SeturlXR(const PMString& name) = 0;
	virtual void SeturlTEC(const PMString& name) = 0;
	virtual void SetLastUser(const PMString& lastUser) = 0;
	virtual void SetModeRobot(const bool16& enable) =0;
#if MULTIBASES == 1
	virtual void SetListeBases(const K2Vector<PMString>liste) = 0;
	virtual void SetIPBases(const K2Vector<PMString>liste) = 0;
#endif
	/**	

	*/
	virtual PMString GeturlXR() = 0;
	virtual PMString GetLastUser() = 0;
	virtual bool16 GetModeRobot() =0;
#if MULTIBASES == 1
	virtual K2Vector<PMString> GetListeBases() =0;
	virtual K2Vector<PMString> GetIPBases() =0;
#endif

	//Preferences globales
	virtual int32 GetImportPub() = 0;
	virtual void SetImportPub(const int32& b) = 0;

	virtual int32 GetPubProp() = 0;
	virtual void SetPubProp(const int32& b) = 0;

	virtual int32 GetPDFMultipages() = 0;
	virtual void SetPDFMultipages(const int32& b) = 0;

	virtual int32 GetExportIDML() = 0;
	virtual void SetExportIDML(const int32& b) = 0;

	virtual int32 GetMakePreview() = 0;
	virtual void SetMakePreview(const int32& b) = 0;

	// IDMS templates used by XRailResponder::ImportPublicites when
	// inserting an ad block. Empty strings (or unreachable files) make
	// the importer fall back to the legacy hard-coded creation path.
	// Refreshed from Gaia at every successful login (GetPrefsXRail_v2).
	virtual PMString GetAdTemplateNoVisual() = 0;
	virtual void SetAdTemplateNoVisual(const PMString& path) = 0;

	virtual PMString GetAdTemplateWithVisual() = 0;
	virtual void SetAdTemplateWithVisual(const PMString& path) = 0;

	// "Se souvenir de moi" on the login dialog. When rememberMe is kTrue, the
	// rememberedPassword is stored and pre-filled next time the dialog opens.
	// NOTE: the password is currently stored without encryption in the
	// InDesign workspace preferences file. This is sufficient for casual
	// protection (the file lives in the user's account) but not for real
	// secrecy — consider adding encryption if this becomes a concern.
	virtual bool16 GetRememberMe() = 0;
	virtual void SetRememberMe(const bool16& enable) = 0;

	virtual PMString GetRememberedPassword() = 0;
	virtual void SetRememberedPassword(const PMString& password) = 0;
};

#endif // __ISnapPrefsData__



