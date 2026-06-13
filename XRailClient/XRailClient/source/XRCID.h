//========================================================================================
//  
//  $File: $
//  
//  Owner: Trias
//  
//  $Author: $
//  
//  $DateTime: $
//  
//  $Revision: $
//  
//  $Change: $
//  
//  Copyright Adobe Systems Incorporated. All rights reserved.
//  
//  NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance 
//  with the terms of the Adobe license agreement accompanying it.  If you have received
//  this file from a source other than Adobe, then your use, modification, or 
//  distribution of it requires the prior written permission of Adobe.
//  
//========================================================================================


#ifndef __XRCID_h__
#define __XRCID_h__

#include "SDKDef.h"

// Company:
#define kXRCCompanyKey				"Trias Developpement"// Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
#define kXRCCompanyValue			"Trias Developpement"// Company name displayed externally.

// Plug-in:
#define kXRCPluginName	"XRailClient"			// Name of this plug-in.
#define kXRCPrefixNumber	0xe0002 		// Unique prefix number for this plug-in(*Must* be obtained from Adobe Developer Support).
// Plug-in version string. Pulled from the shared TriasVersion.h header so
// every Trias plug-in (Pagemakeup / XPage / XRail / XRailClient) bumps in
// lock-step. Edit Gaia\TriasVersion.h to change the family-wide version.
#include "../../../Gaia/TriasVersion.h"
#define kXRCVersion		kTriasVersionString					// Version of this plug-in (for the About Box).
#define kXRCAuthor		"Trias"					// Author of this plug-in (for the About Box).

// Plug-in Prefix: (please change kXRCPrefixNumber above to modify the prefix.)
#define kXRCPrefix		RezLong(kXRCPrefixNumber)				// The unique numeric prefix for all object model IDs for this plug-in.
#define kXRCStringPrefix	SDK_DEF_STRINGIZE(kXRCPrefixNumber)	// The string equivalent of the unique prefix number for  this plug-in.

// Missing plug-in: (see ExtraPluginInfo resource)
#define kXRCMissingPluginURLValue		kSDKDefPartnersStandardValue_enUS // URL displayed in Missing Plug-in dialog
#define kXRCMissingPluginAlertValue	kSDKDefMissingPluginAlertValue // Message displayed in Missing Plug-in dialog - provide a string that instructs user how to solve their missing plug-in problem

// PluginID:
DECLARE_PMID(kPlugInIDSpace, kXRCPluginID, kXRCPrefix + 0)

// ClassIDs:
DECLARE_PMID(kClassIDSpace, kXRCXRailClientBoss, kXRCPrefix + 3)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 4)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 5)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 6)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 7)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 8)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 9)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 10)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 11)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 12)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 13)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 14)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 15)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 16)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 17)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 18)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 19)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 20)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 21)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 22)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 23)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 24)
//DECLARE_PMID(kClassIDSpace, kXRCBoss, kXRCPrefix + 25)


// InterfaceIDs:
DECLARE_PMID(kInterfaceIDSpace, IID_IWEBSERVICES, kXRCPrefix + 0)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 1)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 2)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 3)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 4)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 5)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 6)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 7)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 8)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 9)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 10)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 11)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 12)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 13)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 14)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 15)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 16)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 17)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 18)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 19)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 20)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 21)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 22)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 23)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 24)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRCINTERFACE, kXRCPrefix + 25)


// ImplementationIDs:
DECLARE_PMID(kImplementationIDSpace, kXRCWebServicesImpl, kXRCPrefix + 0)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 1)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 2)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 3)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 4)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 5)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 6)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 7)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 8)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 9)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 10)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 11)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 12)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 13)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 14)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 15)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 16)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 17)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 18)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 19)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 20)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 21)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 22)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 23)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 24)
//DECLARE_PMID(kImplementationIDSpace, kXRCImpl, kXRCPrefix + 25)


// ActionIDs:
DECLARE_PMID(kActionIDSpace, kXRCAboutActionID, kXRCPrefix + 0)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 5)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 6)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 7)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 8)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 9)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 10)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 11)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 12)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 13)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 14)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 15)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 16)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 17)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 18)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 19)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 20)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 21)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 22)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 23)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 24)
//DECLARE_PMID(kActionIDSpace, kXRCActionID, kXRCPrefix + 25)


// WidgetIDs:
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 2)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 3)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 4)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 5)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 6)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 7)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 8)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 9)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 10)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 11)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 12)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 13)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 14)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 15)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 16)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 17)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 18)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 19)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 20)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 21)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 22)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 23)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 24)
//DECLARE_PMID(kWidgetIDSpace, kXRCWidgetID, kXRCPrefix + 25)


// "About Plug-ins" sub-menu:
#define kXRCAboutMenuKey			kXRCStringPrefix "kXRCAboutMenuKey"
#define kXRCAboutMenuPath		kSDKDefStandardAboutMenuPath kXRCCompanyKey

// "Plug-ins" sub-menu:
#define kXRCPluginsMenuKey 		kXRCStringPrefix "kXRCPluginsMenuKey"
#define kXRCPluginsMenuPath		kSDKDefPlugInsStandardMenuPath kXRCCompanyKey kSDKDefDelimitMenuPath kXRCPluginsMenuKey

// Menu item keys:

// Other StringKeys:
#define kXRCAboutBoxStringKey	kXRCStringPrefix "kXRCAboutBoxStringKey"
#define kXRCTargetMenuPath kXRCPluginsMenuPath
#define kErrConnexionFailed	    kXRCStringPrefix "kErrConnexionFailed"

// Menu item positions:


// Initial data format version numbers
#define kXRCFirstMajorFormatNumber  RezLong(1)
#define kXRCFirstMinorFormatNumber  RezLong(0)

// Data format version numbers for the PluginVersion resource 
#define kXRCCurrentMajorFormatNumber kXRCFirstMajorFormatNumber
#define kXRCCurrentMinorFormatNumber kXRCFirstMinorFormatNumber

#endif // __XRCID_h__

//  Code generated by DollyXs code generator
