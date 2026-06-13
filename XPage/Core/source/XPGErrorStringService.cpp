/*
//	File:	XPGErrorStringService.h
//
//	Author:	Trias Developpement
//
//	Date:	15/04/2008
//
*/

#include "VCPluginHeaders.h"


// General includes:
#include "CErrorStringService.h"

// Project includes:
#include "XPGID.h"

/** Implements IErrorString via partial implementation CErrorStringService.
*/
class XPGErrorStringService : public CErrorStringService
{
public:
	/** Constructor 
	 * 	This is where we specify the pluginID and the resourceID for the 
	 * 	UserErrorTable resource that is associated with this implementation.
	 */
	XPGErrorStringService(IPMUnknown* boss):
		CErrorStringService(boss, kXPGPluginID, kSDKDefErrorStringResourceID) {}

	/** Destructor 
	 */
	virtual ~XPGErrorStringService(void) {}
};

/* Make the implementation available to the application.
*/
CREATE_PMINTERFACE(XPGErrorStringService, kXPGErrorStringServiceImpl)

// End, SnipRunErrorStringService.cpp.
