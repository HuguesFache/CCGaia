/*
//
//	Author: Trias
//
//	Date: 12-Dec-2005
//
*/

#include "VCPluginHeaders.h"

// Interface includes:
#include "IRegistration.h"

// General includes:
#include "CAlert.h"
#include "GlobalDefs.h"


class PrsRegistration : public CPMUnknown<IRegistration>
{
public:


	/**	Constructor
		@param boss 
	 */
    PrsRegistration(IPMUnknown* boss );

	/**	Destructor
	 */
	virtual ~PrsRegistration() {}

	
	virtual int32 getStatus();
	virtual void setStatus(int32 status);

	virtual uint32 getVersion();
	virtual void setVersion(uint32 version);

	virtual int32 getMaxUsers();
	virtual void setMaxUsers(int32 maxusers);

	virtual PMString getIDSerialNumber();
	virtual void setIDSerialNumber(PMString idSerial);

	virtual int32 getSerialNumber();
	virtual void setSerialNumber(int32 serial);

	virtual bool16 getComposerTools();
	virtual void setComposerTools(bool16 composer);

	virtual bool16 getReverse();
	virtual void setReverse(bool16 reverse);

	virtual bool16 getPermRefs();
	virtual void setPermRefs(bool16 permrefs);

	virtual bool16 getPageLink();
	virtual void setPageLink(bool16 pagelink);

	virtual bool16 getTimeBombed();
	virtual void setTimeBombed(bool16 timebombed);

private :
	int32 status, maxUsers, serial;
	PMString IDSerialNumber;
	uint32 version;
	bool16 composer, reverse, permrefs, pagelink, timebombed;
};

CREATE_PMINTERFACE(PrsRegistration, kPrsRegistrationImpl)


/* Constructor
*/
PrsRegistration::PrsRegistration(IPMUnknown *boss) : CPMUnknown<IRegistration>(boss)
{
	status = ID_UNREGISTERED;
	maxUsers = 1;
	serial = 0;
	IDSerialNumber = "";
	version = ID_CURRENTVERSION;
	composer = kFalse; 
	reverse = kFalse;
	permrefs = kFalse;
	pagelink = kFalse;
	timebombed = kFalse;
}


int32 PrsRegistration::getStatus()
{
	return status;
}

void PrsRegistration::setStatus(int32 status)
{
	this->status = status;
	if(status == ID_UNREGISTERED) 
	{
		// Not registered, every info must be reentered,
		// so each field is reset
		maxUsers = 1;
		serial = 0;
		IDSerialNumber = "";
		version = ID_CURRENTVERSION;
		composer = kFalse; 
		reverse = kFalse;
		permrefs = kFalse;
		pagelink = kFalse;
		timebombed = kFalse;
	}
}

uint32 PrsRegistration::getVersion()
{
	return version;
}

void PrsRegistration::setVersion(uint32 version)
{
	this->version = version;
}

int32 PrsRegistration::getMaxUsers()
{
	return maxUsers;
}

void PrsRegistration::setMaxUsers(int32 maxusers)
{
	this->maxUsers = maxusers;
}

PMString PrsRegistration::getIDSerialNumber()
{
	return IDSerialNumber;
}

void PrsRegistration::setIDSerialNumber(PMString idSerial)
{
	this->IDSerialNumber = idSerial;
}

int32 PrsRegistration::getSerialNumber()
{
	return serial;
}

void PrsRegistration::setSerialNumber(int32 serial)
{
	this->serial = serial;
}

bool16 PrsRegistration::getComposerTools()
{
	return composer;
}

void PrsRegistration::setComposerTools(bool16 composer)
{
	this->composer = composer;
}

bool16 PrsRegistration::getReverse()
{
	return reverse;
}

void PrsRegistration::setReverse(bool16 reverse)
{
	this->reverse = reverse;
}

bool16 PrsRegistration::getPermRefs()
{
	return permrefs;
}

void PrsRegistration::setPermRefs(bool16 permrefs)
{
	this->permrefs = permrefs;
}

bool16 PrsRegistration::getPageLink()
{
	return pagelink;
}

void PrsRegistration::setPageLink(bool16 pagelink)
{
	this->pagelink = pagelink;
}

bool16 PrsRegistration::getTimeBombed()
{
	return timebombed;
}

void PrsRegistration::setTimeBombed(bool16 timebombed)
{
	this->timebombed = timebombed;
}
