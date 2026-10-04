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


#ifndef __XRLID_h__
#define __XRLID_h__

#include "SDKDef.h"

// Company:
#define kXRLCompanyKey		"Trias Developpement"  // Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
#define kXRLCompanyValue	"Trias Developpement"		// Company name displayed externally.

// Plug-in:
#define kXRLPluginName		"XRail"						// Name of this plug-in.
#define kXRLPrefixNumber	0xe0f00 					// Unique prefix number for this plug-in(*Must* be obtained from Adobe Developer Support).
// Plug-in version string. Pulled from the shared TriasVersion.h header so
// every Trias plug-in (Pagemakeup / XPage / XRail / XRailClient) bumps in
// lock-step. Edit Gaia\TriasVersion.h to change the family-wide version.
#include "../../../Gaia/TriasVersion.h"
#define kXRLVersion			kTriasVersionString						// Version of this plug-in (for the About Box).
#define kXRLAuthor			"Trias"		// Author of this plug-in (for the About Box).
#define kXRLXRailVersion	103

// Plug-in Prefix: (please change kXRLPrefixNumber above to modify the prefix.)
#define kXRLPrefix			RezLong(kXRLPrefixNumber)				// The unique numeric prefix for all object model IDs for this plug-in.
#define kXRLStringPrefix	SDK_DEF_STRINGIZE(kXRLPrefixNumber)	// The string equivalent of the unique prefix number for  this plug-in.

// Missing plug-in: (see ExtraPluginInfo resource)
#define kXRLMissingPluginURLValue		kSDKDefPartnersStandardValue_enUS // URL displayed in Missing Plug-in dialog
#define kXRLMissingPluginAlertValue		kSDKDefMissingPluginAlertValue // Message displayed in Missing Plug-in dialog - provide a string that instructs user how to solve their missing plug-in problem

// Precompilation flag
#define MULTIBASES			0


// PluginID:
DECLARE_PMID(kPlugInIDSpace, kXRLPluginID, kXRLPrefix + 0)

// ClassIDs:
DECLARE_PMID(kClassIDSpace, kXRLStartupShutdownBoss, kXRLPrefix + 0)
DECLARE_PMID(kClassIDSpace, kXRLSetPageSlugDataCmdBoss, kXRLPrefix + 1)
DECLARE_PMID(kClassIDSpace, kXRLSetPrefsCmdBoss, kXRLPrefix + 2)
DECLARE_PMID(kClassIDSpace, kXRLResponderServiceBoss, kXRLPrefix + 3)
DECLARE_PMID(kClassIDSpace, kXRLSetMakePreviewCmdBoss, kXRLPrefix + 4)
DECLARE_PMID(kClassIDSpace, kXRLSetResaPubListCmdBoss, kXRLPrefix + 5)
DECLARE_PMID(kClassIDSpace, kXRLScriptProviderBoss, kXRLPrefix + 6)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 9)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 10)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 11)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 12)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 13)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 14)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 15)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 16)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 17)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 18)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 19)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 20)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 21)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 22)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 23)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 24)
//DECLARE_PMID(kClassIDSpace, kXRLBoss, kXRLPrefix + 25)

// InterfaceIDs:
DECLARE_PMID(kInterfaceIDSpace, IID_IXRAILPREFSDATA, kXRLPrefix + 0)
DECLARE_PMID(kInterfaceIDSpace, IID_ISTRINGDATA_LOGIN, kXRLPrefix + 1)
DECLARE_PMID(kInterfaceIDSpace, IID_PAGESLUGDATA, kXRLPrefix + 2)
DECLARE_PMID(kInterfaceIDSpace, IID_IMAKEPREVIEW, kXRLPrefix + 3)
DECLARE_PMID(kInterfaceIDSpace, IID_IRESAPUBLIST, kXRLPrefix + 4)
DECLARE_PMID(kInterfaceIDSpace, IID_ILOGINSTRING, kXRLPrefix + 5)
DECLARE_PMID(kInterfaceIDSpace, IID_ICREATEPDFBOOL, kXRLPrefix + 6)
DECLARE_PMID(kInterfaceIDSpace, IID_ICREATEPREVIEWBOOL, kXRLPrefix + 7)
DECLARE_PMID(kInterfaceIDSpace, IID_IAUTOCLOSINGDIALOG, kXRLPrefix + 10)
// SERVEUR identifier parsed from Gaia.ini at XRail startup — session-level
// IIntData on kSessionBoss, read by the login dialog to identify this client
// when it refetches server prefs after a successful VerifLogin. Lives in
// XRail so XRail-only installs (no XPage plugin) still work.
DECLARE_PMID(kInterfaceIDSpace, IID_IXRAILFROMSERVEUR, kXRLPrefix + 11)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 11)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 12)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 13)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 14)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 15)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 16)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 17)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 18)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 19)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 20)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 21)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 22)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 23)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 24)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLPrefix + 25)


// ImplementationIDs:
DECLARE_PMID(kImplementationIDSpace, kXRLPrefsDataPersistImpl,		kXRLPrefix + 0)
DECLARE_PMID(kImplementationIDSpace, kXRLSetPrefsCmdImpl, kXRLPrefix + 1)
DECLARE_PMID(kImplementationIDSpace, kXRLStartupShutdownImpl, kXRLPrefix + 2)
DECLARE_PMID(kImplementationIDSpace, kXRLDataPersistImpl, kXRLPrefix + 3)
DECLARE_PMID(kImplementationIDSpace, kXRLSetPageSlugDataCmdImpl, kXRLPrefix + 4)
DECLARE_PMID(kImplementationIDSpace, kXRLStringDataImpl, kXRLPrefix + 5)
DECLARE_PMID(kImplementationIDSpace, kXRLServiceProviderImpl, kXRLPrefix + 6)
DECLARE_PMID(kImplementationIDSpace, kXRLResponderImpl, kXRLPrefix + 7)
DECLARE_PMID(kImplementationIDSpace, kXRLSetMakePreviewCmdImpl, kXRLPrefix + 8)
DECLARE_PMID(kImplementationIDSpace, kXRLMakePreviewImpl, kXRLPrefix + 9)
DECLARE_PMID(kImplementationIDSpace, kXRLResaPubListImpl, kXRLPrefix + 10)
DECLARE_PMID(kImplementationIDSpace, kXRLSetResaPubListCmdImpl, kXRLPrefix + 11)
DECLARE_PMID(kImplementationIDSpace, kXRLLoginStringDataImpl, kXRLPrefix + 12)
DECLARE_PMID(kImplementationIDSpace, kXRLCreatePreviewBoolDataImpl, kXRLPrefix + 13)
DECLARE_PMID(kImplementationIDSpace, kXRLCreatePDFBoolDataImpl, kXRLPrefix + 14)
DECLARE_PMID(kImplementationIDSpace, kXRLPathStringDataImpl, kXRLPrefix + 15)
DECLARE_PMID(kImplementationIDSpace, kXRLAutoClosingDialogImpl, kXRLPrefix + 17)
DECLARE_PMID(kImplementationIDSpace, kXRLScriptProviderImpl, kXRLPrefix + 18)
//DECLARE_PMID(kImplementationIDSpace, kXRLPageSlugScriptProviderImpl, kXRLPrefix + 20)
//DECLARE_PMID(kImplementationIDSpace, kXRLImpl, kXRLPrefix + 22)
//DECLARE_PMID(kImplementationIDSpace, kXRLImpl, kXRLPrefix + 23)
//DECLARE_PMID(kImplementationIDSpace, kXRLImpl, kXRLPrefix + 24)
//DECLARE_PMID(kImplementationIDSpace, kXRLImpl, kXRLPrefix + 25)


// ActionIDs:
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 0)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 1)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 2)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 8)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 9)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 10)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 11)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 12)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 13)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 14)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 15)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 16)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 17)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 18)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 19)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 20)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 21)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 22)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 23)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 24)
//DECLARE_PMID(kActionIDSpace, kXRLActionID, kXRLPrefix + 25)


// WidgetIDs:
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 0)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 1)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 2)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 4)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 5)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 6)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 7)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 8)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 9)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 10)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 11)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 12)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 13)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 14)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 15)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 16)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 17)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 18)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 19)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 20)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 21)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 22)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 23)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 24)
//DECLARE_PMID(kWidgetIDSpace, kXRLWidgetID, kXRLPrefix + 25)

//Scripts
DECLARE_PMID(kScriptInfoIDSpace, kXRLPageSlugPropScriptElement, kXRLPrefix + 0)
DECLARE_PMID(kScriptInfoIDSpace, kXRLPageDataPropScriptElement,	kXRLPrefix + 1)



// Other StringKeys:
#define kCommandXRailSetPrefsCmdKey 	kXRLStringPrefix "kCommandXRailSetPrefsCmdKey"
#define kCommandXRailSetPageSlugCmdKey 	kXRLStringPrefix "kCommandXRailSetPageSlugCmdKey"

#define kXRLErrorWSLockPageKey kXRLStringPrefix "kXRLErrorWSLockPageKey"
#define kXRLErrorWSGenerateVignetteKey kXRLStringPrefix "kXRLErrorWSGenerateVignetteKey"
#define kXRLErrorUnknownUserKey kXRLStringPrefix "kXRLErrorUnknownUserKey"
#define kXRLErrorBadPasswordKey kXRLStringPrefix "kXRLErrorBadPasswordKey"
#define kXRLErrorPrivilegeKey kXRLStringPrefix "kXRLErrorPrivilegeKey"
#define kXRLErrorSOAPKey kXRLStringPrefix "kXRLErrorSOAPKey"
#define kXRLErrorWebServiceKey kXRLStringPrefix "kXRLErrorWebServiceKey"
#define kXRLErrorMissingConfigFileKey kXRLStringPrefix "kXRLErrorMissingConfigFileKey"
#define kXRLErrorCreateResaPubKey kXRLStringPrefix "kXRLErrorCreateResaPubKey"
#define kXRLErrorInvalidResaPageKey kXRLStringPrefix "kXRLErrorInvalidResaPageKey"
#define kXRLErrorPDFNotCreatedStringKey kXRLStringPrefix "kXRLErrorPDFNotCreatedStringKey"
#define kXRLErrorGetPDFFileNameKey kXRLStringPrefix "kXRLErrorGetPDFFileNameKey"
#define kXRLErrorPDFStyleNotFoundKey kXRLStringPrefix "kXRLErrorPDFStyleNotFoundKey"
#define kXRLErrorMovePDFFileKey kXRLStringPrefix "kXRLErrorMovePDFFileKey"
#define kXRLVersionTextKey kXRLStringPrefix "kXRLVersionTextKey"
#define kXRLDocumentReadOnly kXRLStringPrefix "kXRLDocumentReadOnly"
#define kXRLAutoClosingDialogKey kXRLStringPrefix "kXRLAutoClosingDialogKey"


// Menu item positions:

// Initial data format version numbers
#define kXRLFirstMajorFormatNumber  RezLong(1)
#define kXRLFirstMinorFormatNumber  RezLong(0)

// Data format version numbers for the PluginVersion resource 
#define kXRLCurrentMajorFormatNumber kXRLFirstMajorFormatNumber
#define kXRLCurrentMinorFormatNumber kXRLFirstMinorFormatNumber

#endif // __XRLID_h__

//  Code generated by DollyXs code generator
