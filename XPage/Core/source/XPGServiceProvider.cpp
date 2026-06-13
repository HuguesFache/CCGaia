#include "VCPlugInHeaders.h"

// Interface includes:
#include "CServiceProvider.h"
#include "K2Vector.h"
#include "DocumentID.h"
#include "XPGID.h"
#include "TextID.h"
//#include "ILinksPanelID.h"
#include "ImageID.h"
#include "ImageImportUIID.h"
#include "EPSUIID.h"


// General includes
#include "LocaleSetting.h"

class XPGServiceProvider : public CServiceProvider
{
	public:

		/**
			Constructor initializes a list of service IDs, one for each file action 
			signal that DocWchResponder will handle.
			@param boss interface ptr from boss object on which interface is aggregated.
		*/
		XPGServiceProvider(IPMUnknown* boss);
		
		/**
			Destructor.  
		*/
		virtual	~XPGServiceProvider();

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
CREATE_PMINTERFACE(XPGServiceProvider, kXPGServiceProviderImpl)


/* XRailServiceProvider Constructor
*/
XPGServiceProvider::XPGServiceProvider(IPMUnknown* boss)
                   :CServiceProvider(boss){	

	fSupportedServiceIDs.clear();
  	fSupportedServiceIDs.push_back(kBeforeCloseDocSignalResponderService);
	fSupportedServiceIDs.push_back(kAfterOpenDocSignalResponderService);
    fSupportedServiceIDs.push_back(kDuringOpenDocSignalResponderService);
    // Required so XPGDocObserver::AutoAttach is called on freshly-created
    // docs as well — without this responder, the doc observer never wakes
    // up on File > New and the page-item-delete hook stays inert.
    fSupportedServiceIDs.push_back(kAfterNewDocSignalResponderService);

    if (fSupportedServiceIDs.size()<=0){
		fSupportedServiceIDs.push_back(kInvalidService);
	}

}

/* XRailServiceProvider Dtor
*/
XPGServiceProvider::~XPGServiceProvider()
{
}

/* GetName
*/
void XPGServiceProvider::GetName(PMString* pName)
{
	pName->SetCString("XPage Responder Service");
}

/* GetServiceID
*/
ServiceID XPGServiceProvider::GetServiceID() 
{
	// Should never be called given that HasMultipleIDs() returns kTrue.
	return fSupportedServiceIDs[0];
}

/* IsDefaultServiceProvider
*/
bool16 XPGServiceProvider::IsDefaultServiceProvider()
{
	return kFalse;
}

/* GetInstantiationPolicy
*/
IK2ServiceProvider::InstancePerX XPGServiceProvider::GetInstantiationPolicy()
{
	return IK2ServiceProvider::kInstancePerSession;
}

/* HasMultipleIDs
*/
bool16 XPGServiceProvider::HasMultipleIDs() const
{
	return kTrue;
}

/* GetServiceIDs
*/
void XPGServiceProvider::GetServiceIDs(K2Vector<ServiceID>& serviceIDs)
{
	// push_back a service IDs for each service provided. 
	for (int32 i = 0; i<fSupportedServiceIDs.size(); i++)
		serviceIDs.push_back(fSupportedServiceIDs[i]);

}


// End, DocWchServiceProvider.cpp.



