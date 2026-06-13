/*
//
//	File:	PrsCustomServiceProvider.cpp
//
//  Author: Trias
//
//	Date:	28-Oct-2005
//
*/

#include "VCPlugInHeaders.h"

#include "GlobalDefs.h"

// Interface includes:

// Implementation includes:
#include "CServiceProvider.h"
#include "K2Vector.h"
#include "DocumentID.h"
#include "TextID.h"
#include "PrsID.h"

/** PrsCustomServiceProvider
	Responder service provider notified of :
		- story deletion so that we can remove affected PermRefs references  
		- document opening, so that we can update spline's IIntData interface	
*/
class PrsCustomServiceProvider : public CServiceProvider
{
	public:

		/**
			Constructor initializes a list of service IDs
			@param boss interface ptr from boss object on which interface is aggregated.
		*/
		PrsCustomServiceProvider(IPMUnknown* boss);
		
		/**
			Destructor.  
		*/
		virtual	~PrsCustomServiceProvider();

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
			PrsCustomServiceProvider registers to handle.
		*/
		virtual void GetServiceIDs(K2Vector<ServiceID>& serviceIDs);

	private:

		K2Vector<ServiceID> fSupportedServiceIDs;
};


/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PrsCustomServiceProvider, kPrsCustomServiceProviderImpl)


/* PrsCustomServiceProvider Constructor
*/
PrsCustomServiceProvider::PrsCustomServiceProvider(IPMUnknown* boss)
	: CServiceProvider(boss)
{
	// Add the serviceIDs we want the associated responder to handle.
	fSupportedServiceIDs.clear();
	
	// OpenDoc
	fSupportedServiceIDs.push_back(kDuringOpenDocSignalResponderService);

	// AfterOpenDoc
	fSupportedServiceIDs.push_back(kAfterOpenDocSignalResponderService);

	// NewDoc
	fSupportedServiceIDs.push_back(kAfterNewDocSignalResponderService);

	// CloseDoc
	fSupportedServiceIDs.push_back(kBeforeCloseDocSignalResponderService);

	//	Delete story
	fSupportedServiceIDs.push_back(kDeleteStoryRespService);
}

/* PrsCustomServiceProvider Dtor
*/
PrsCustomServiceProvider::~PrsCustomServiceProvider()
{
}

/* GetName
*/
void PrsCustomServiceProvider::GetName(PMString* pName)
{
	pName->SetCString("PageMakeUp Responder Service");
}

/* GetServiceID
*/
ServiceID PrsCustomServiceProvider::GetServiceID() 
{
	// Should never be called given that HasMultipleIDs() returns kTrue.
	return fSupportedServiceIDs[0];
}

/* IsDefaultServiceProvider
*/
bool16 PrsCustomServiceProvider::IsDefaultServiceProvider()
{
	return kFalse;
}

/* GetInstantiationPolicy
*/
IK2ServiceProvider::InstancePerX PrsCustomServiceProvider::GetInstantiationPolicy()
{
	return IK2ServiceProvider::kInstancePerSession;
}

/* HasMultipleIDs
*/
bool16 PrsCustomServiceProvider::HasMultipleIDs() const
{
	return kTrue;
}

/* GetServiceIDs
*/
void PrsCustomServiceProvider::GetServiceIDs(K2Vector<ServiceID>& serviceIDs)
{
	// Append a service IDs for each service provided. 
	for (int32 i = 0; i<fSupportedServiceIDs.size(); i++)
		serviceIDs.push_back(fSupportedServiceIDs[i]);

}
