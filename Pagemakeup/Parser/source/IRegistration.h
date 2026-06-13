/*
//	File:	IRegistration.h
//
//  Author: Trias
//
//	Date:	12-Dec-2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2005 Trias Developpement. All rights reserved.
//
*/

#ifndef __IRegistration_h__
#define __IRegistration_h__

#include "PrsID.h"

#define ID_REGISTERED 0
#define ID_UNREGISTERED 1

class IPMUnknown;


/** IRegistration
	Provides method to store registration information
*/
class IRegistration : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_IREGISTRATION };

		virtual int32 getStatus() =0;
		virtual void setStatus(int32 status) =0;

		virtual uint32 getVersion() =0;
		virtual void setVersion(uint32 version) =0;

		virtual int32 getMaxUsers() =0;
		virtual void setMaxUsers(int32 maxusers) =0;

		virtual PMString getIDSerialNumber() =0;
		virtual void setIDSerialNumber(PMString idSerial) =0;

		virtual int32 getSerialNumber() =0;
		virtual void setSerialNumber(int32 serial) =0;

		virtual bool16 getComposerTools() =0;
		virtual void setComposerTools(bool16 composer) =0;

		virtual bool16 getReverse() =0;
		virtual void setReverse(bool16 reverse) =0;

		virtual bool16 getPermRefs() =0;
		virtual void setPermRefs(bool16 permrefs) =0;

		virtual bool16 getPageLink() =0;
		virtual void setPageLink(bool16 pagelink) =0;

		virtual bool16 getTimeBombed() =0;
		virtual void setTimeBombed(bool16 timebombed) =0;
};

#endif // __IRegistration_h__

		