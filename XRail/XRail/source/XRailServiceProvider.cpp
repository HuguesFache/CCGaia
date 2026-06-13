//========================================================================================
//  
//  $File: //depot/indesign_3.0/gm/source/sdksamples/docwatch/DocWchServiceProvider.cpp $
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
//========================================================================================

#include "VCPlugInHeaders.h"

// Interface includes:

// Implementation includes:
#include "CServiceProvider.h"
#include "K2Vector.h"
#include "DocumentID.h"
#include "XRLID.h"
#include "IInCopyBridgeUtils.h"

// General includes
#include "LocaleSetting.h"


/** DocWchServiceProvider
	registers as providing the service of responding to a group of document
	file action signals.  See the constructor code for a list of the
	signals this service responds to.

	DocWchServiceProvider implements IK2ServiceProvider based on
	the partial implementation CServiceProvider.


	@ingroup docwatch
	@author John Hake
*/
class XRailServiceProvider : public CServiceProvider
{
	public:

		/**
			Constructor initializes a list of service IDs, one for each file action 
			signal that DocWchResponder will handle.
			@param boss interface ptr from boss object on which interface is aggregated.
		*/
		XRailServiceProvider(IPMUnknown* boss);
		
		/**
			Destructor.  
		*/
		virtual	~XRailServiceProvider();
		
		virtual IPlugIn::ThreadingPolicy GetThreadingPolicy() const
	{
		return IPlugIn::kMainThreadOnly;
	}
	 
		/**
			GetName initializes the name of the service.
			@param pName Ptr to PMString to receive the name.
		*/
		virtual void GetName(PMString* pName);

		/**
			GetServiceID returns a single service ID.  This is required, even though
			GetServiceIDs() will return the complete list initialized in the constructor.
			This method just returns the first service ID in the list.
		*/
		virtual ServiceID GetServiceID();

		/**
			IsDefaultServiceProvider tells application this service is not the default service.
		*/
		virtual bool16 IsDefaultServiceProvider();
		
		/**
			GetInstantiationPolicy returns a InstancePerX value to indicate that only
			one instance per session is needed.
		*/
		virtual InstancePerX GetInstantiationPolicy();

		/**
			HasMultipleIDs returns kTrue in order to force a call to GetServiceIDs().
		*/
		virtual bool16 HasMultipleIDs() const;

		/**
			GetServiceIDs returns a list of services provided.
			@param serviceIDs List of IDs describing the services that 
			DocWchServiceProvider registers to handle.
		*/
		virtual void GetServiceIDs(K2Vector<ServiceID>& serviceIDs);

	private:

		K2Vector<ServiceID> fSupportedServiceIDs;
};


/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XRailServiceProvider, kXRLServiceProviderImpl)


/* XRailServiceProvider Constructor
*/
XRailServiceProvider::XRailServiceProvider(IPMUnknown* boss)
	: CServiceProvider(boss)
{	
	fSupportedServiceIDs.clear();

	//if(LocaleSetting::GetLocale().GetProductFS() == kInDesignProductFS) // we don't need to lock pages or to offer status management in server version
	//{
		//	OpenDoc
		fSupportedServiceIDs.push_back(kDuringOpenDocSignalResponderService);
		fSupportedServiceIDs.push_back(kAfterOpenDocSignalResponderService);

		//	CloseDoc
		fSupportedServiceIDs.push_back(kBeforeCloseDocSignalResponderService);
	//}

	//  Before SaveDoc
		fSupportedServiceIDs.push_back(kBeforeSaveDocSignalResponderService);
		fSupportedServiceIDs.push_back(kBeforeSaveAsDocSignalResponderService);

	//	SaveDoc
	fSupportedServiceIDs.push_back(kAfterSaveDocSignalResponderService);

	//	SaveAsDoc
	fSupportedServiceIDs.push_back(kAfterSaveAsDocSignalResponderService);
	
	if (fSupportedServiceIDs.size()<=0)
	{
		ASSERT_FAIL("DocWchServiceProvider must support at least 1 service ID");
		fSupportedServiceIDs.push_back(kInvalidService);
	}

}

/* XRailServiceProvider Dtor
*/
XRailServiceProvider::~XRailServiceProvider()
{
}

/* GetName
*/
void XRailServiceProvider::GetName(PMString* pName)
{
	pName->SetCString("XRail Responder Service");
}

/* GetServiceID
*/
ServiceID XRailServiceProvider::GetServiceID() 
{
	// Should never be called given that HasMultipleIDs() returns kTrue.
	return fSupportedServiceIDs[0];
}

/* IsDefaultServiceProvider
*/
bool16 XRailServiceProvider::IsDefaultServiceProvider()
{
	return kFalse;
}

/* GetInstantiationPolicy
*/
IK2ServiceProvider::InstancePerX XRailServiceProvider::GetInstantiationPolicy()
{
	return IK2ServiceProvider::kInstancePerSession;
}

/* HasMultipleIDs
*/
bool16 XRailServiceProvider::HasMultipleIDs() const
{
	return kTrue;
}

/* GetServiceIDs
*/
void XRailServiceProvider::GetServiceIDs(K2Vector<ServiceID>& serviceIDs)
{
	// push_back a service IDs for each service provided. 
	for (int32 i = 0; i<fSupportedServiceIDs.size(); i++)
		serviceIDs.push_back(fSupportedServiceIDs[i]);

	}


// End, DocWchServiceProvider.cpp.



