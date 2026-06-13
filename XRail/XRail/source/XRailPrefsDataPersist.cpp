//========================================================================================
//  
//  $File: //depot/indesign_3.0/gm/source/sdksamples/snapshot/SnapPrefsDataPersist.cpp $
//  
//  Owner: Adobe Developer Technologies
//  
//  $Author: pmbuilder $
//  
//  $DateTime: 2003/09/30 15:41:37 $
//  
//  $Revision: #1 $
//  
//  $Change: 223184 $
//  
//  Copyright 1997-2003 Adobe Systems Incorporated. All rights reserved.
//  
//  NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance 
//  with the terms of the Adobe license agreement accompanying it.  If you have received
//  this file from a source other than Adobe, then your use, modification, or 
//  distribution of it requires the prior written permission of Adobe.
//  
//  
//  SnapPrefsDataPersist is the persistent implementation of ISnapPrefsData. 
//  
//========================================================================================

#include "VCPluginHeaders.h"

// Interface includes:
#include "IPMStream.h"

// General includes:
#include "TRACE.h"
#include "CPMUnknown.h"

// Project includes:
#include "XRLID.h"
#include "IXRailPrefsData.h"

/**	SnapPrefsDataPersist

	The persistent implementation of ISnapPrefsData.
	@author	Jane Bond
	@ingroup snapshot
*/
class XRailPrefsDataPersist : public CPMUnknown<IXRailPrefsData>
{
public:
	XRailPrefsDataPersist(IPMUnknown* boss);

	virtual ~XRailPrefsDataPersist();

	virtual void SeturlXR(const PMString& name);
	virtual PMString GeturlXR();

	virtual void SeturlTEC(const PMString& name);
	virtual PMString GeturlTEC();

	virtual void SetLastUser(const PMString& lastUser);
	virtual PMString GetLastUser();

	virtual void SetModeRobot(const bool16& enable);
	virtual bool16 GetModeRobot();

#if MULTIBASES == 1
	virtual void SetListeBases(const K2Vector<PMString>listeBases);
	virtual K2Vector<PMString> GetListeBases();

	virtual void SetIPBases(const K2Vector<PMString>listeBases);
	virtual K2Vector<PMString> GetIPBases();
#endif

	virtual void ReadWrite(IPMStream* s, ImplementationID prop);

	//Preferences globales
	virtual int32 GetImportPub();
	virtual void SetImportPub(const int32& b);

	virtual int32 GetPubProp();
	virtual void SetPubProp(const int32& b);

	virtual int32 GetPDFMultipages();
	virtual void SetPDFMultipages(const int32& b);

	virtual int32 GetExportIDML();
	virtual void SetExportIDML(const int32& b);

	virtual int32 GetMakePreview();
	virtual void SetMakePreview(const int32& b);

	virtual PMString GetAdTemplateNoVisual();
	virtual void SetAdTemplateNoVisual(const PMString& path);

	virtual PMString GetAdTemplateWithVisual();
	virtual void SetAdTemplateWithVisual(const PMString& path);

	virtual bool16 GetRememberMe();
	virtual void SetRememberMe(const bool16& enable);

	virtual PMString GetRememberedPassword();
	virtual void SetRememberedPassword(const PMString& password);

private:

	PMString urlTEC, urlXR;
	PMString lastUser;
	int32 ImportPub, PubProp, PDFMultipages, ExportIDML, MakePreview;
	bool16 modeRobot;

	// IDMS template paths (refreshed from Gaia at login). Not persisted
	// in ReadWrite — same lifecycle as the other server-side prefs above.
	PMString adTemplateNoVisual;
	PMString adTemplateWithVisual;

	// "Remember me" on the login dialog.
	bool16 rememberMe;
	PMString rememberedPassword;

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
CREATE_PERSIST_PMINTERFACE(XRailPrefsDataPersist, kXRLPrefsDataPersistImpl)

/* Constructor
*/
XRailPrefsDataPersist::XRailPrefsDataPersist(IPMUnknown* boss) :
	CPMUnknown<IXRailPrefsData>(boss), urlXR(""), urlTEC(""), lastUser(""), modeRobot(kFalse),
	adTemplateNoVisual(""), adTemplateWithVisual(""),
	rememberMe(kFalse), rememberedPassword("")
{
}

/* Destructor.
*/
XRailPrefsDataPersist::~XRailPrefsDataPersist()
{
}


/* SetName
*/
void XRailPrefsDataPersist::SeturlXR(const PMString& name)
{
	if (urlXR != name)
	{
		PreDirty();
		urlXR = name;
	}
}

/* GetName
*/
PMString XRailPrefsDataPersist::GeturlXR()
{
	return urlXR;
}


void XRailPrefsDataPersist::SeturlTEC(const PMString& name)
{
	if (urlTEC != name)
	{
		PreDirty();
		urlTEC = name;
	}
}

/* GetName
*/
PMString XRailPrefsDataPersist::GeturlTEC()
{
	return urlTEC;
}

/* SetName
*/
void XRailPrefsDataPersist::SetLastUser(const PMString& name)
{
	if (lastUser != name)
	{
		PreDirty();
		lastUser = name;
	}
}

/* GetName
*/
PMString XRailPrefsDataPersist::GetLastUser()
{
	return lastUser;
}

/* SetModeRobot
*/
void XRailPrefsDataPersist::SetModeRobot(const bool16& enable)
{
	PreDirty();
	modeRobot = enable;
}

/* GetModeRobot
*/
bool16 XRailPrefsDataPersist::GetModeRobot()
{
	return modeRobot;
}

/* SetListeBases
*/
#if MULTIBASES == 1
void XRailPrefsDataPersist::SetListeBases(const K2Vector<PMString>liste)
{
	PreDirty();
	listeBases = liste;
}

/* GetListeBases
*/
K2Vector<PMString> XRailPrefsDataPersist::GetListeBases()
{
	return listeBases;
}

/* SetIPBases
*/

void XRailPrefsDataPersist::SetIPBases(const K2Vector<PMString>ip)
{
	PreDirty();
	IPBases = ip;
}

/* GetListeBases
*/
K2Vector<PMString> XRailPrefsDataPersist::GetIPBases()
{
	return IPBases;
}
#endif

void XRailPrefsDataPersist::SetImportPub(const int32& value)
{
	PreDirty();
	ImportPub = value;
}

int32 XRailPrefsDataPersist::GetImportPub()
{
	return ImportPub;
}

void XRailPrefsDataPersist::SetPubProp(const int32& value)
{
	PreDirty();
	PubProp = value;
}

int32 XRailPrefsDataPersist::GetPubProp()
{
	return PubProp;
}

void XRailPrefsDataPersist::SetExportIDML(const int32& value)
{
	PreDirty();
	ExportIDML = value;
}

int32 XRailPrefsDataPersist::GetExportIDML()
{
	return ExportIDML;
}

void XRailPrefsDataPersist::SetMakePreview(const int32& value)
{
	PreDirty();
	MakePreview = value;
}

int32 XRailPrefsDataPersist::GetMakePreview()
{
	return MakePreview;
}

void XRailPrefsDataPersist::SetPDFMultipages(const int32& value)
{
	PreDirty();
	PDFMultipages = value;
}

int32 XRailPrefsDataPersist::GetPDFMultipages()
{
	return PDFMultipages;
}

void XRailPrefsDataPersist::SetAdTemplateNoVisual(const PMString& path)
{
	if (adTemplateNoVisual != path)
	{
		PreDirty();
		adTemplateNoVisual = path;
	}
}

PMString XRailPrefsDataPersist::GetAdTemplateNoVisual()
{
	return adTemplateNoVisual;
}

void XRailPrefsDataPersist::SetAdTemplateWithVisual(const PMString& path)
{
	if (adTemplateWithVisual != path)
	{
		PreDirty();
		adTemplateWithVisual = path;
	}
}

PMString XRailPrefsDataPersist::GetAdTemplateWithVisual()
{
	return adTemplateWithVisual;
}

/* SetRememberMe
*/
void XRailPrefsDataPersist::SetRememberMe(const bool16& enable)
{
	if (rememberMe != enable)
	{
		PreDirty();
		rememberMe = enable;
	}
}

/* GetRememberMe
*/
bool16 XRailPrefsDataPersist::GetRememberMe()
{
	return rememberMe;
}

/* SetRememberedPassword
*/
void XRailPrefsDataPersist::SetRememberedPassword(const PMString& password)
{
	if (rememberedPassword != password)
	{
		PreDirty();
		rememberedPassword = password;
	}
}

/* GetRememberedPassword
*/
PMString XRailPrefsDataPersist::GetRememberedPassword()
{
	return rememberedPassword;
}


/* ReadWrite
*/
void XRailPrefsDataPersist::ReadWrite(IPMStream* s, ImplementationID prop)
{
	int32 numOptions = 0;
	urlXR.ReadWrite(s);
	urlTEC.ReadWrite(s);
	lastUser.ReadWrite(s);
	s->XferBool(modeRobot);

#if MULTIBASES == 1
	numOptions = listeBases.size();
	for (int32 i = 0; i < numOptions; i++)
	{
		listeBases[i].ReadWrite(s);
		IPBases[i].ReadWrite(s);
	}
#endif

	// "Remember me" data. Appended at the end so that older preference files
	// without these fields still load.
	//
	// On a un sentinel int32 = 0xCAFE en tête : si on le lit comme tel, on sait
	// que le bloc remember-me est présent. Sur un ancien fichier, on lit du
	// garbage / on dépasse EOF, GetStreamState devient bad, on garde les
	// défauts. Sans sentinel, le check post-EOF est fragile : XferBool/ReadWrite
	// lisent souvent quelques octets garbage AVANT de marquer le stream bad,
	// produisant un rememberedPassword corrompu de taille arbitraire.
	const int32 kRememberMeSentinel = (int32)0xCAFE;
	int32 sentinel = kRememberMeSentinel;
	if (s->IsReading())
	{
		s->XferInt32(sentinel);
		if (s->GetStreamState() != kStreamStateGood || sentinel != kRememberMeSentinel)
		{
			// Ancien fichier, ou stream épuisé : on garde les défauts du ctor.
			rememberMe = kFalse;
			rememberedPassword = PMString("");
			return;
		}
		s->XferBool(rememberMe);
		rememberedPassword.ReadWrite(s);
		// Garde-fou : si la lecture renvoie une PMString anormale, on rejette.
		if (rememberedPassword.NumUTF16TextChars() > 256)
		{
			rememberMe = kFalse;
			rememberedPassword = PMString("");
		}
	}
	else
	{
		s->XferInt32(sentinel);
		s->XferBool(rememberMe);
		rememberedPassword.ReadWrite(s);
	}
}

// End, SnapPrefsDataPersist.cpp.

