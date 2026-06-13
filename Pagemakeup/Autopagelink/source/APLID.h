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


#ifndef __APLID_h__
#define __APLID_h__

#include "SDKDef.h"

// Company:
#define kAPLCompanyKey		kSDKDefPlugInCompanyKey		// Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
#define kAPLCompanyValue	kSDKDefPlugInCompanyValue	// Company name displayed externally.

// Plug-in:
#define kAPLPluginName		"AutoPageLink"			// Name of this plug-in.
#define kAPLPrefixNumber	0xdd600 		// Unique prefix number for this plug-in(*Must* be obtained from Adobe Developer Support).
// Plug-in version string. Pulled from the shared TriasVersion.h header so
// every Trias plug-in bumps in lock-step. Edit Gaia\TriasVersion.h to
// change the family-wide version.
#include "../../../Gaia/TriasVersion.h"
#define kAPLVersion			kTriasVersionString					// Version of this plug-in (for the About Box).
#define kAPLAuthor			"Trias"					// Author of this plug-in (for the About Box).

// Plug-in Prefix: (please change kAPLPrefixNumber above to modify the prefix.)
#define kAPLPrefix			RezLong(kAPLPrefixNumber)				// The unique numeric prefix for all object model IDs for this plug-in.
#define kAPLStringPrefix	SDK_DEF_STRINGIZE(kAPLPrefixNumber)	// The string equivalent of the unique prefix number for  this plug-in.

// Missing plug-in: (see ExtraPluginInfo resource)
#define kAPLMissingPluginURLValue		kSDKDefPartnersStandardValue_enUS // URL displayed in Missing Plug-in dialog
#define kAPLMissingPluginAlertValue	kSDKDefMissingPluginAlertValue // Message displayed in Missing Plug-in dialog - provide a string that instructs user how to solve their missing plug-in problem

// PluginID:
DECLARE_PMID(kPlugInIDSpace, kAPLPluginID, kAPLPrefix + 0)

// ClassIDs:
DECLARE_PMID(kClassIDSpace, kAPLStartupShutdownBoss, kAPLPrefix + 3)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 4)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 5)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 6)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 7)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 8)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 9)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 10)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 11)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 12)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 13)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 14)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 15)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 16)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 17)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 18)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 19)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 20)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 21)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 22)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 23)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 24)
//DECLARE_PMID(kClassIDSpace, kAPLBoss, kAPLPrefix + 25)


// InterfaceIDs:
DECLARE_PMID(kInterfaceIDSpace, IID_AUTOPAGELINKTASK, kAPLPrefix + 0)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 1)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 2)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 3)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 4)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 5)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 6)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 7)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 8)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 9)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 10)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 11)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 12)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 13)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 14)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 15)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 16)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 17)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 18)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 19)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 20)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 21)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 22)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 23)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 24)
//DECLARE_PMID(kInterfaceIDSpace, IID_IAPLINTERFACE, kAPLPrefix + 25)


// ImplementationIDs:
DECLARE_PMID(kImplementationIDSpace, kAPLStartupShutdownImpl, kAPLPrefix + 0)
DECLARE_PMID(kImplementationIDSpace, kAPLIdleTaskImpl, kAPLPrefix + 1)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 2)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 3)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 4)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 5)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 6)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 7)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 8)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 9)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 10)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 11)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 12)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 13)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 14)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 15)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 16)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 17)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 18)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 19)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 20)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 21)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 22)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 23)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 24)
//DECLARE_PMID(kImplementationIDSpace, kAPLImpl, kAPLPrefix + 25)


// ActionIDs:
DECLARE_PMID(kActionIDSpace, kAPLAboutActionID, kAPLPrefix + 0)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 5)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 6)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 7)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 8)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 9)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 10)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 11)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 12)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 13)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 14)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 15)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 16)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 17)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 18)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 19)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 20)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 21)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 22)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 23)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 24)
//DECLARE_PMID(kActionIDSpace, kAPLActionID, kAPLPrefix + 25)


// WidgetIDs:
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 2)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 3)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 4)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 5)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 6)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 7)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 8)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 9)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 10)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 11)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 12)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 13)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 14)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 15)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 16)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 17)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 18)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 19)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 20)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 21)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 22)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 23)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 24)
//DECLARE_PMID(kWidgetIDSpace, kAPLWidgetID, kAPLPrefix + 25)


// "About Plug-ins" sub-menu:
#define kAPLAboutMenuKey			kAPLStringPrefix "kAPLAboutMenuKey"
#define kAPLAboutMenuPath		kSDKDefStandardAboutMenuPath kAPLCompanyKey

// "Plug-ins" sub-menu:
#define kAPLPluginsMenuKey 		kAPLStringPrefix "kAPLPluginsMenuKey"
#define kAPLPluginsMenuPath		kSDKDefPlugInsStandardMenuPath kAPLCompanyKey kSDKDefDelimitMenuPath kAPLPluginsMenuKey

// Menu item keys:

// Other StringKeys:
#define kAPLAboutBoxStringKey	kAPLStringPrefix "kAPLAboutBoxStringKey"
#define kAPLTargetMenuPath kAPLPluginsMenuPath
#define kAPLErrorInFolderStringKey	kAPLStringPrefix "kAPLErrorInFolderStringKey"
#define kAPLErrorOpenTCLStringKey	kAPLStringPrefix "kAPLErrorOpenTCLStringKey"

// Task key
#define kAPLIdleTaskKey	kAPLStringPrefix "kAPLIdleTaskKey"

// Menu item positions:


// Initial data format version numbers
#define kAPLFirstMajorFormatNumber  RezLong(1)
#define kAPLFirstMinorFormatNumber  RezLong(0)

// Data format version numbers for the PluginVersion resource 
#define kAPLCurrentMajorFormatNumber kAPLFirstMajorFormatNumber
#define kAPLCurrentMinorFormatNumber kAPLFirstMinorFormatNumber

#endif // __APLID_h__

//  Code generated by DollyXs code generator
