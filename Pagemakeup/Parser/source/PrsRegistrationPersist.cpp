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
#include "IPMStream.h"

// General includes:
#include "CAlert.h"
#include "GlobalDefs.h"

class PrsRegistrationPersist : public CPMUnknown<IRegistration>
{
public:


	/**	Constructor
		@param boss 
	 */
    PrsRegistrationPersist(IPMUnknown* boss );

	/**	Destructor
	 */
	virtual ~PrsRegistrationPersist() {}

	
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

	/**
		@param stream contains the stream to be written or read.
		@param implementation the implementation ID.
	*/
	void ReadWrite(IPMStream* stream, ImplementationID implementation);

private :
	int32 status, maxUsers, serial;
	PMString IDSerialNumber;
	uint32 version;
	bool16 composer, reverse, permrefs, pagelink, timebombed;
};

CREATE_PERSIST_PMINTERFACE(PrsRegistrationPersist, kPrsRegistrationPersistImpl)


/* Constructor
*/
PrsRegistrationPersist::PrsRegistrationPersist(IPMUnknown *boss) : CPMUnknown<IRegistration>(boss)
{
	status = ID_UNREGISTERED;
	serial = 0;
	maxUsers = 1;
	IDSerialNumber = "";
	version = ID_CURRENTVERSION;
	composer = kFalse; 
	reverse = kFalse;
	permrefs = kFalse;
	pagelink = kFalse;
	timebombed = kFalse;
}


int32 PrsRegistrationPersist::getStatus()
{
	return status;
}

void PrsRegistrationPersist::setStatus(int32 status)
{
	PreDirty();
	this->status = status;
}

uint32 PrsRegistrationPersist::getVersion()
{
	return version;
}

void PrsRegistrationPersist::setVersion(uint32 version)
{
	PreDirty();
	this->version = version;
}

int32 PrsRegistrationPersist::getMaxUsers()
{
	return maxUsers;
}

void PrsRegistrationPersist::setMaxUsers(int32 maxusers)
{
	PreDirty();
	this->maxUsers = maxusers;
}

PMString PrsRegistrationPersist::getIDSerialNumber()
{
	return IDSerialNumber;
}

void PrsRegistrationPersist::setIDSerialNumber(PMString idSerial)
{
	PreDirty();
	this->IDSerialNumber = idSerial;
}

int32 PrsRegistrationPersist::getSerialNumber()
{
	return serial;
}

void PrsRegistrationPersist::setSerialNumber(int32 serial)
{
	PreDirty();
	this->serial = serial;
}

bool16 PrsRegistrationPersist::getComposerTools()
{
	return composer;
}

void PrsRegistrationPersist::setComposerTools(bool16 composer)
{
	PreDirty();
	this->composer = composer;
}

bool16 PrsRegistrationPersist::getReverse()
{
	return reverse;
}

void PrsRegistrationPersist::setReverse(bool16 reverse)
{
	PreDirty();
	this->reverse = reverse;
}

bool16 PrsRegistrationPersist::getPermRefs()
{
	return permrefs;
}

void PrsRegistrationPersist::setPermRefs(bool16 permrefs)
{
	PreDirty();
	this->permrefs = permrefs;
}

bool16 PrsRegistrationPersist::getPageLink()
{
	return pagelink;
}

void PrsRegistrationPersist::setPageLink(bool16 pagelink)
{
	PreDirty();
	this->pagelink = pagelink;
}

bool16 PrsRegistrationPersist::getTimeBombed()
{
	return timebombed;
}

void PrsRegistrationPersist::setTimeBombed(bool16 timebombed)
{
	PreDirty();
	this->timebombed = timebombed;
}


void PrsRegistrationPersist::ReadWrite(IPMStream* stream, ImplementationID implementation)
{
	stream->XferInt32((int32&) this->version);
	stream->XferInt32(this->status);
	stream->XferInt32(this->maxUsers);
	stream->XferInt32(this->serial);	
	this->IDSerialNumber.ReadWrite(stream);
	stream->XferBool(this->composer);
	stream->XferBool(this->reverse);
	stream->XferBool(this->permrefs);
	stream->XferBool(this->pagelink);
	stream->XferBool(this->timebombed);	
}
