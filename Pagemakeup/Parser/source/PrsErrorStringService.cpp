/*
//	File:	PrsErrorStringService.h
//
//	Author:	Trias
//
//	Date:	23/11/2005
//
*/

#include "VCPluginHeaders.h"


// General includes:
#include "CErrorStringService.h"

// Project includes:
#include "PrsID.h"

/** Implements IErrorString via partial implementation CErrorStringService.
*/
class PrsErrorStringService : public CErrorStringService
{
public:
	/** Constructor 
	 * 	This is where we specify the pluginID and the resourceID for the 
	 * 	UserErrorTable resource that is associated with this implementation.
	 */
	PrsErrorStringService(IPMUnknown* boss):
		CErrorStringService(boss, kPrsPluginID, kSDKDefErrorStringResourceID) {}

	/** Destructor 
	 */
	virtual ~PrsErrorStringService(void) {}
};

/* Make the implementation available to the application.
*/
CREATE_PMINTERFACE(PrsErrorStringService, kPrsErrorStringServiceImpl)

// End, SnipRunErrorStringService.cpp.
