//========================================================================================
//  
//  $File: $
//  
//  Owner: Trias Developpement
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


#ifndef __XPGID_h__
#define __XPGID_h__

#include "SDKDef.h"

// Company:
#define kXPGCompanyKey	    "Trias Developpement"			// Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
#define kXPGCompanyValue	"Trias Developpement"			// Company name displayed externally.

// Plug-in:
#define kXPGPluginName		"XPage"							// Name of this plug-in.
#define kXPGPrefixNumber	0xe3e00 						// Unique prefix Number for this plug-in(*Must* be obtained from Adobe Developer Support).
// Plug-in version string. Pulled from the shared TriasVersion.h header so
// every Trias plug-in (Pagemakeup / XPage / XRail / XRailClient) bumps in
// lock-step. Edit Gaia\TriasVersion.h to change the family-wide version.
#include "../../../Gaia/TriasVersion.h"
#define kXPGVersion			kTriasVersionString						// Version of this plug-in (for the About Box).
#define kXPGAuthor			"Trias"			// Author of this plug-in (for the About Box).

// Plug-in Prefix: (please change kXPGPrefixNumber above to modify the prefix.)
#define kXPGPrefix		    RezLong(kXPGPrefixNumber)				// The unique numeric prefix for all object model IDs for this plug-in.
#define kXPGStringPrefix	SDK_DEF_STRINGIZE(kXPGPrefixNumber)		// The string equivalent of the unique prefix Number for  this plug-in.

// Missing plug-in: (see ExtraPluginInfo resource)
#define kXPGMissingPluginURLValue		kSDKDefPartnersStandardValue_enUS	// URL displayed in Missing Plug-in dialog
#define kXPGMissingPluginAlertValue		kSDKDefMissingPluginAlertValue		// Message displayed in Missing Plug-in dialog - provide a string that instructs user how to solve their missing plug-in problem

// OPTIONS
#define XPAGESERVER		0   // 1 pour GPSANTE
#define INCOPY			0

// PluginID:
DECLARE_PMID(kPlugInIDSpace, kXPGPluginID, kXPGPrefix + 0)

// ClassIDs:
DECLARE_PMID(kClassIDSpace, kXPageManagerBoss,						kXPGPrefix + 0)
DECLARE_PMID(kClassIDSpace, kXPGXMLParserArticleBoss,				kXPGPrefix + 1)
DECLARE_PMID(kClassIDSpace, kXPGXMLMatchingFileHandlerBoss,			kXPGPrefix + 4)
DECLARE_PMID(kClassIDSpace, kXPGTextImportHandlerBoss,				kXPGPrefix + 5)
DECLARE_PMID(kClassIDSpace, kXPGSetTagToStyleMapCmd,				kXPGPrefix + 6)
DECLARE_PMID(kClassIDSpace, kXGPPostTextImportIterationBoss,		kXPGPrefix + 7)
DECLARE_PMID(kClassIDSpace, kXPGSetArticleIDCmdBoss,				kXPGPrefix + 8)
DECLARE_PMID(kClassIDSpace, kXPGXMLImageDescriptionHandlerBoss,		kXPGPrefix + 9)
DECLARE_PMID(kClassIDSpace, kXPGTagToStyleMapBoss,					kXPGPrefix + 10)
DECLARE_PMID(kClassIDSpace, kXPGXMLDescriptionUpdaterBoss,			kXPGPrefix + 11)
DECLARE_PMID(kClassIDSpace, kXPGXMLLoadTagHandlerBoss,				kXPGPrefix + 12)
DECLARE_PMID(kClassIDSpace, kXPGStartupShutdownBoss,				kXPGPrefix + 13)
DECLARE_PMID(kClassIDSpace, kXPGConversionProvider,					kXPGPrefix + 14)
DECLARE_PMID(kClassIDSpace, kXPGSaveConvertedDocRespBoss,			kXPGPrefix + 15)
DECLARE_PMID(kClassIDSpace, kXPGExportArtRespBoss,					kXPGPrefix + 16)
DECLARE_PMID(kClassIDSpace, kXPGXMLFormeDescriptionHandlerBoss,		kXPGPrefix + 17)
DECLARE_PMID(kClassIDSpace, kXPGSetStringCmdBoss,					kXPGPrefix + 18)
DECLARE_PMID(kClassIDSpace, kXPGScriptProviderBoss,				    kXPGPrefix + 20)
DECLARE_PMID(kClassIDSpace, kXPGSetFormeDataCmdBoss,				kXPGPrefix + 21)
DECLARE_PMID(kClassIDSpace, kXPGErrorStringServiceBoss,				kXPGPrefix + 22)
DECLARE_PMID(kClassIDSpace, kXPGFilterStyleCmdBoss,					kXPGPrefix + 24)
DECLARE_PMID(kClassIDSpace, kXPGSaveAssignmentCmdBoss,				kXPGPrefix + 25)
DECLARE_PMID(kClassIDSpace, kXPGXMLExportFileHandlerBoss,			kXPGPrefix + 26)
DECLARE_PMID(kClassIDSpace, kXPGUnlockStoriesCmdBoss,				kXPGPrefix + 27)
DECLARE_PMID(kClassIDSpace, kXPGResponderServiceBoss,				kXPGPrefix + 28)
DECLARE_PMID(kClassIDSpace, kXPGXMLArticleDescriptionBoss,			kXPGPrefix + 29)
DECLARE_PMID(kClassIDSpace, kXPGIAMServiceProviderBoss,				kXPGPrefix + 34)
DECLARE_PMID(kClassIDSpace, kXPGSetArticleDataCmdBoss,				kXPGPrefix + 36)
DECLARE_PMID(kClassIDSpace, kXPGIAMUIServiceProviderBoss,			kXPGPrefix + 37)
DECLARE_PMID(kClassIDSpace, kXPGSetIndexPreferencesCmdBoss,			kXPGPrefix + 38)
DECLARE_PMID(kClassIDSpace, kXPGXMLMatchingStylesListHandlerBoss,	kXPGPrefix + 41)
DECLARE_PMID(kClassIDSpace, kXPGXMLMatchingFileHandler2Boss,		kXPGPrefix + 42)
DECLARE_PMID(kClassIDSpace, kXPGSetPlacedArticleDataCmdBoss,		kXPGPrefix + 44)
DECLARE_PMID(kClassIDSpace, kXPGXMLPostImportIterationBaseBoss,		kXPGPrefix + 45)
DECLARE_PMID(kClassIDSpace, kXPGArticleSizerCmdBoss,				kXPGPrefix + 47)
DECLARE_PMID(kClassIDSpace, kXPGCheckTextImportHandlerBoss,		    kXPGPrefix + 48)

// InterfaceIDs:
DECLARE_PMID(kInterfaceIDSpace, IID_IPAGEMANAGER,					kXPGPrefix + 0)
DECLARE_PMID(kInterfaceIDSpace, IID_IXPAGEMGRACCESSOR,				kXPGPrefix + 1)
DECLARE_PMID(kInterfaceIDSpace, IID_ICURRENTLEVEL2,					kXPGPrefix + 2)
DECLARE_PMID(kInterfaceIDSpace, IID_ICURRENTLEVEL1,					kXPGPrefix + 3)
DECLARE_PMID(kInterfaceIDSpace, IID_ICURRENTNUMERO,					kXPGPrefix + 4)
DECLARE_PMID(kInterfaceIDSpace, IID_INODEDATA,						kXPGPrefix + 6)
DECLARE_PMID(kInterfaceIDSpace, IID_IXPGTAGTOSTYLEMAP,				kXPGPrefix + 7)
DECLARE_PMID(kInterfaceIDSpace, IID_IXMLELEMENTDATA,				kXPGPrefix + 8)
DECLARE_PMID(kInterfaceIDSpace, IID_ILSIDDATA,						kXPGPrefix + 9)
DECLARE_PMID(kInterfaceIDSpace, IID_IIMAGEDATAMODEL,				kXPGPrefix + 10)
DECLARE_PMID(kInterfaceIDSpace, IID_INUMEROID,						kXPGPrefix + 12)
DECLARE_PMID(kInterfaceIDSpace, IID_IXPAGESUITE,					kXPGPrefix + 13)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLEINFO,					kXPGPrefix + 14)
DECLARE_PMID(kInterfaceIDSpace, IID_ISPOOLERTASK,					kXPGPrefix + 15)
DECLARE_PMID(kInterfaceIDSpace, IID_IFORMEDATAMODEL,				kXPGPrefix + 16)
DECLARE_PMID(kInterfaceIDSpace, IID_IREFRESHPROTOCOL,				kXPGPrefix + 17)
DECLARE_PMID(kInterfaceIDSpace, IID_ILASTIMPORTEDSNIPPET,			kXPGPrefix + 18)
DECLARE_PMID(kInterfaceIDSpace, IID_IIMPORTSNIPPETOBSERVER,			kXPGPrefix + 19)
DECLARE_PMID(kInterfaceIDSpace, IID_IXPGPREFERENCES,				kXPGPrefix + 20)
DECLARE_PMID(kInterfaceIDSpace, IID_ITYPOINFO,						kXPGPrefix + 21)
DECLARE_PMID(kInterfaceIDSpace, IID_IFORMEDATA,						kXPGPrefix + 22)
DECLARE_PMID(kInterfaceIDSpace, IID_IXPAGEINDEXSUITE,				kXPGPrefix + 23)
DECLARE_PMID(kInterfaceIDSpace, IID_ARTICLEOBSERVER,				kXPGPrefix + 24)
DECLARE_PMID(kInterfaceIDSpace, IID_IUNLINKASSIGNMENT,				kXPGPrefix + 25)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLEDATA,					kXPGPrefix + 26)
DECLARE_PMID(kInterfaceIDSpace, IID_IDISPLAYSHAPE,					kXPGPrefix + 27)
DECLARE_PMID(kInterfaceIDSpace, IID_IDISPLAYADORNMRNTTEXT,			kXPGPrefix + 28)
DECLARE_PMID(kInterfaceIDSpace, IID_ITEXTDATA,						kXPGPrefix + 29)
DECLARE_PMID(kInterfaceIDSpace, IID_IXPAGEUTILS,					kXPGPrefix + 30)
DECLARE_PMID(kInterfaceIDSpace, IID_ICURRENTSTORY,					kXPGPrefix + 31)
DECLARE_PMID(kInterfaceIDSpace, IID_ICURRENTGROUP,					kXPGPrefix + 32)
DECLARE_PMID(kInterfaceIDSpace, IID_ICURRENTERECEPIENT,				kXPGPrefix + 33)
DECLARE_PMID(kInterfaceIDSpace, IID_ICURRENTSTATUS,					kXPGPrefix + 34)
DECLARE_PMID(kInterfaceIDSpace, IID_ICURRENTUSER,					kXPGPrefix + 35)
DECLARE_PMID(kInterfaceIDSpace, IID_IXPGUISTATISTICSOBSERVER, 	    kXPGPrefix + 37)
DECLARE_PMID(kInterfaceIDSpace, IID_IXPGINDEXREFERENCE, 			kXPGPrefix + 38)	
DECLARE_PMID(kInterfaceIDSpace, IID_IXPGTEXTMODELOBSERVER, 			kXPGPrefix + 39)	
DECLARE_PMID(kInterfaceIDSpace, IID_IXPGPERSONINDEXTAG, 			kXPGPrefix + 40)	
DECLARE_PMID(kInterfaceIDSpace, IID_IKEYVALUEDATA, 					kXPGPrefix + 41)	
DECLARE_PMID(kInterfaceIDSpace, IID_IPLACEDARTICLEDATA, 			kXPGPrefix + 42)
DECLARE_PMID(kInterfaceIDSpace, IID_IDOCOBSERVER, 					kXPGPrefix + 43)	
DECLARE_PMID(kInterfaceIDSpace, IID_ITEXTEMATCHINGSTYLESLIST, 		kXPGPrefix + 44)
DECLARE_PMID(kInterfaceIDSpace, IID_IASSEMBLAGEDATAMODEL, 			kXPGPrefix + 46)
DECLARE_PMID(kInterfaceIDSpace, IID_IFONTUTILS,						kXPGPrefix + 47)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLEDATAMODEL,				kXPGPrefix + 49)
DECLARE_PMID(kInterfaceIDSpace, IID_XPAGESTORYDATA,					kXPGPrefix + 50)
// Session bool holding the current "Afficher les formes" state, shared by
// the XPage (XRail) palette and the XDA (Formes) palette so both checkboxes
// stay in sync. Mirrors IID_IDISPLAYSHAPE.
DECLARE_PMID(kInterfaceIDSpace, IID_IDISPLAYFORME,					kXPGPrefix + 51)


// ImplementationIDs:
DECLARE_PMID(kImplementationIDSpace, kXPageMgrAccessorImpl,					kXPGPrefix + 0)
DECLARE_PMID(kImplementationIDSpace, kXPGCurrentLevel2Impl,					kXPGPrefix + 2)
DECLARE_PMID(kImplementationIDSpace, kXPGCurrentLevel1Impl,					kXPGPrefix + 3)
DECLARE_PMID(kImplementationIDSpace, kXPGCurrentNumeroImpl,					kXPGPrefix + 4)
DECLARE_PMID(kImplementationIDSpace, kXPGPointerDataImpl,					kXPGPrefix + 7)
DECLARE_PMID(kImplementationIDSpace, kXPGXMLMatchingFileHandlerImpl,		kXPGPrefix + 8)
DECLARE_PMID(kImplementationIDSpace, kXPGTextImportHandlerImpl,				kXPGPrefix + 9)
DECLARE_PMID(kImplementationIDSpace, kXPGSetTagToStyleMapCmdImpl,			kXPGPrefix + 10)
DECLARE_PMID(kImplementationIDSpace, kXPGTagToStyleMapImpl,					kXPGPrefix + 12)
DECLARE_PMID(kImplementationIDSpace, kXPGLSIDDataPersistImpl,				kXPGPrefix + 13)
// kXPGPrefix + 14 was kXPGFileSystemImageModelImpl (dead code — removed
// with XPGFileSystemImageModel.cpp). The XRail model uses
// kXPGXRailImageModelImpl (kXPGPrefix + 33).
DECLARE_PMID(kImplementationIDSpace, kPersistStringListDataImpl,			kXPGPrefix + 15)
DECLARE_PMID(kImplementationIDSpace, kXPGFileSystemFormeModelImpl,			kXPGPrefix + 16)
DECLARE_PMID(kImplementationIDSpace, kXPGDocNumeroImpl,						kXPGPrefix + 18)
DECLARE_PMID(kImplementationIDSpace, kXPGTextCSBImpl,						kXPGPrefix + 19)
DECLARE_PMID(kImplementationIDSpace, kXPGSuiteASBImpl,						kXPGPrefix + 20)
DECLARE_PMID(kImplementationIDSpace, kXPGLayoutCSBImpl,						kXPGPrefix + 21)
DECLARE_PMID(kImplementationIDSpace, kXPGXMLImageDescriptionHandlerImpl,	kXPGPrefix + 22)
DECLARE_PMID(kImplementationIDSpace, kXPGXMLLoadTagHandlerImpl,				kXPGPrefix + 23)
DECLARE_PMID(kImplementationIDSpace, kXPGXMLParserArticleImpl,		kXPGPrefix + 24)
DECLARE_PMID(kImplementationIDSpace, kXPGStartupShutdownImpl,				kXPGPrefix + 26)
DECLARE_PMID(kImplementationIDSpace, kXPGSetArticleIDCmdImpl,				kXPGPrefix + 27)
DECLARE_PMID(kImplementationIDSpace, kXPGImportSnippetObserverImpl,			kXPGPrefix + 28)
DECLARE_PMID(kImplementationIDSpace, kXPGPreferencesImpl,					kXPGPrefix + 30)
DECLARE_PMID(kImplementationIDSpace, kXPGExportArtRespImpl,					kXPGPrefix + 31)
DECLARE_PMID(kImplementationIDSpace, kXPGXRailImageModelImpl,				kXPGPrefix + 33)
DECLARE_PMID(kImplementationIDSpace, kXPGXMLFormeDescriptionHandlerImpl,	kXPGPrefix + 36)
DECLARE_PMID(kImplementationIDSpace, kXPGSetStringCmdImpl,					kXPGPrefix + 37)
DECLARE_PMID(kImplementationIDSpace, kXPGTypoInfoImpl,						kXPGPrefix + 38)
DECLARE_PMID(kImplementationIDSpace, kXPGUpdateArticleXMLFileCmdImpl,		kXPGPrefix + 40)
DECLARE_PMID(kImplementationIDSpace, kXPGScriptProviderImpl,				kXPGPrefix + 41)
DECLARE_PMID(kImplementationIDSpace, kXPGFormeDataImpl,						kXPGPrefix + 42)
DECLARE_PMID(kImplementationIDSpace, kXPGSetFormeDataCmdImpl,				kXPGPrefix + 43)
DECLARE_PMID(kImplementationIDSpace, kXPGErrorStringServiceImpl,			kXPGPrefix + 44)
DECLARE_PMID(kImplementationIDSpace, kXPGFilterStyleCmdImpl,				kXPGPrefix + 46)
DECLARE_PMID(kImplementationIDSpace, kXPGSaveAssignmentCmdImpl,				kXPGPrefix + 47)
DECLARE_PMID(kImplementationIDSpace, kXPGXMLExportFileHandlerImpl,			kXPGPrefix + 48)
DECLARE_PMID(kImplementationIDSpace, kXPGUnlockStoriesCmdImpl,				kXPGPrefix + 49)
DECLARE_PMID(kImplementationIDSpace, kXPGResponderImpl,						kXPGPrefix + 50)
DECLARE_PMID(kImplementationIDSpace, kXPGServiceProviderImpl,				kXPGPrefix + 51)
DECLARE_PMID(kImplementationIDSpace, kXPGXMLArticleDescriptionImpl,			kXPGPrefix + 52)
DECLARE_PMID(kImplementationIDSpace, kXPGIndexSuiteASBImpl,					kXPGPrefix + 54)
DECLARE_PMID(kImplementationIDSpace, kXPGIndexSuiteTextCSBImpl,				kXPGPrefix + 55)
DECLARE_PMID(kImplementationIDSpace, kXPGIAMServiceProviderImpl,			kXPGPrefix + 59)
DECLARE_PMID(kImplementationIDSpace, kXPGDeleteArticleBoolImpl,				kXPGPrefix + 60)
DECLARE_PMID(kImplementationIDSpace, kXPGCurrentStatusImpl,					kXPGPrefix + 62)
DECLARE_PMID(kImplementationIDSpace, kXPGArticleDataImpl,					kXPGPrefix + 63)
DECLARE_PMID(kImplementationIDSpace, kXPGSetArticleDataCmdImpl,				kXPGPrefix + 64)
DECLARE_PMID(kImplementationIDSpace, kXPGDisplayShapeBoolImpl,				kXPGPrefix + 65)
DECLARE_PMID(kImplementationIDSpace, kXPGDisplayAdornmentTextBoolImpl,		kXPGPrefix + 66)
DECLARE_PMID(kImplementationIDSpace, kXPGIAMServiceImpl,					kXPGPrefix + 67)
DECLARE_PMID(kImplementationIDSpace, kXPGTextDataImpl,						kXPGPrefix + 68)
DECLARE_PMID(kImplementationIDSpace, kXPGUtilsImpl,							kXPGPrefix + 69)
DECLARE_PMID(kImplementationIDSpace, kXPGCurrentStoryImpl,					kXPGPrefix + 70)
DECLARE_PMID(kImplementationIDSpace, kXPGCurrentGroupImpl,					kXPGPrefix + 71)
DECLARE_PMID(kImplementationIDSpace, kXPGCurrentRecepientImpl,				kXPGPrefix + 72)
DECLARE_PMID(kImplementationIDSpace, kXPGCurrentStatusXrailImpl,			kXPGPrefix + 73)
DECLARE_PMID(kImplementationIDSpace, kXPGCurrentUserXrailImpl,				kXPGPrefix + 74)
DECLARE_PMID(kImplementationIDSpace, kXPGIndexPreferenceImpl,				kXPGPrefix + 75)
DECLARE_PMID(kImplementationIDSpace, kXPGSetIndexPreferencesCmdImpl,		kXPGPrefix + 76)
DECLARE_PMID(kImplementationIDSpace, kXPGPersonIndexTagImpl,				kXPGPrefix + 77)
DECLARE_PMID(kImplementationIDSpace, kXPGContentXMLImportPrefsImpl,			kXPGPrefix + 78)
DECLARE_PMID(kImplementationIDSpace, kXPGXMLMatchingStylesListHandlerImpl,	kXPGPrefix + 79)
DECLARE_PMID(kImplementationIDSpace, kXPGKeyValueDataImpl,					kXPGPrefix + 80)
DECLARE_PMID(kImplementationIDSpace, kXPGXMLMatchingFileHandler2Impl,		kXPGPrefix + 81)
DECLARE_PMID(kImplementationIDSpace, kXPGLinkResourceFactoryImpl,			kXPGPrefix + 82)
DECLARE_PMID(kImplementationIDSpace, kXPGPlacedArticleDataImpl,				kXPGPrefix + 83)
DECLARE_PMID(kImplementationIDSpace, kXPGSetPlacedArticleDataCmdImpl,		kXPGPrefix + 84)
DECLARE_PMID(kImplementationIDSpace, kXPGDocObserverImpl,					kXPGPrefix + 85)
DECLARE_PMID(kImplementationIDSpace, kXPGXMLPostImportIterationBaseImpl,	kXPGPrefix + 86)
DECLARE_PMID(kImplementationIDSpace, kXPGCheckTextImportHandlerImpl,		kXPGPrefix + 87)
DECLARE_PMID(kImplementationIDSpace, kXPGArticleSizerCmdImpl,				kXPGPrefix + 88)
DECLARE_PMID(kImplementationIDSpace, kXPGFileSystemAssemblageModelImpl,		kXPGPrefix + 91)
DECLARE_PMID(kImplementationIDSpace, kXPGArticleDataModelImpl,				kXPGPrefix + 96)
DECLARE_PMID(kImplementationIDSpace, kXPGDisplayFormeBoolImpl,				kXPGPrefix + 97)



// MessageIDs
DECLARE_PMID(kMessageIDSpace,		 kXPGNumeroChangeMsg,					kXPGPrefix + 0)
DECLARE_PMID(kMessageIDSpace,		 kXPGRefreshMsg,						kXPGPrefix + 1)
DECLARE_PMID(kMessageIDSpace,		 kXPGNewClasseurMsg,					kXPGPrefix + 2)
DECLARE_PMID(kMessageIDSpace,		 kXPGDeleteClasseurMsg,					kXPGPrefix + 3)
DECLARE_PMID(kMessageIDSpace,		 kXPGRefreshAssemblageMsg,				kXPGPrefix + 4)
DECLARE_PMID(kMessageIDSpace,		 kXPGNewClasseurAssemblageMsg,			kXPGPrefix + 5)
DECLARE_PMID(kMessageIDSpace,		 kXPGDeleteClasseurAssemblageMsg,		kXPGPrefix + 6)
// Broadcast on the session subject (IID_IREFRESHPROTOCOL) when the
// "Afficher les formes" state changes on either palette, so the sibling
// palette's checkbox re-syncs from IID_IDISPLAYFORME.
DECLARE_PMID(kMessageIDSpace,		 kXPGDisplayFormesChangedMsg,			kXPGPrefix + 7)

//ScriptIDs
DECLARE_PMID(kScriptInfoIDSpace,			kXPGFormeDataPropScriptElement,			kXPGPrefix + 0)
DECLARE_PMID(kScriptInfoIDSpace,			kXPGPlacedArticleDataPropScriptElement,	kXPGPrefix + 1)
DECLARE_PMID(kScriptInfoIDSpace,			kXPGPArticleDataPropScriptElement,		kXPGPrefix + 2)

// ErrorCodes
DECLARE_PMID(kErrorIDSpace,					kXPGFailedToLoadFormeDataErrorCode,		kXPGPrefix + 0)

// "About Plug-ins" sub-menu:
#define kXPGAboutMenuKey					kXPGStringPrefix						"kXPGAboutMenuKey"
#define kXPGAboutMenuPath					kSDKDefStandardAboutMenuPath			kXPGCompanyKey

// "Plug-ins" sub-menu:
#define kXPGPluginsMenuKey 					kXPGStringPrefix						"kXPGPluginsMenuKey"
#define kXPGPluginsMenuPath					kSDKDefPlugInsStandardMenuPath			kXPGCompanyKey kSDKDefDelimitMenuPath kXPGPluginsMenuKey
	
// Menu item keys:

// Other StringKeys:
#define kXPGTargetMenuPath					kXPGPluginsMenuPath
#define kXPGAboutBoxStringKey				kXPGStringPrefix        "kXPGAboutBoxStringKey"

#define kXPGEraseCurrentVersionKey			kXPGStringPrefix		"kXPGEraseCurrentVersionKey"
#define kXPGCannotMoveFileKey				kXPGStringPrefix	    "kXPGCannotMoveFileKey"
#define kXPGFileAlreadyExistsKey			kXPGStringPrefix		"kXPGFileAlreadyExistsKey"
#define kXPGFileInNewStatusAlreadyExistsKey kXPGStringPrefix		"kXPGFileInNewStatusAlreadyExistsKey"
#define kXPGErrDescFileNotUpdatedKey		kXPGStringPrefix		"kXPGErrDescFileNotUpdatedKey"
#define kXPGErrParseMatchingFailed			kXPGStringPrefix		"kXPGErrParseMatchingFailed"
#define kXPGErrImportFailed					kXPGStringPrefix		"kXPGErrImportFailed"
#define kXPGErrParseArticleFailed			kXPGStringPrefix		"kXPGErrParseArticleFailed"
#define kXPGErrParseImageFailed				kXPGStringPrefix		"kXPGErrParseImageFailed"
#define kXPGErrCannotOpenTemplate			kXPGStringPrefix		"kXPGErrCannotOpenTemplate"
#define kXPGErrFolderDontExistKey			kXPGStringPrefix	    "kXPGErrFolderDontExistKey"
#define kXPGErrIdenticalFolderKey			kXPGStringPrefix		"kXPGErrIdenticalFolderKey"
#define kXPGSpoolerTaskKey					kXPGStringPrefix		"kXPGSpoolerTaskKey"
#define kXPGErrMatchingFileNotFound			kXPGStringPrefix        "kXPGErrMatchingFileNotFound"
#define kXPGErrFormeImportFailed			kXPGStringPrefix        "kXPGErrFormeImportFailed"
#define kXPGErrArticleLocked				kXPGStringPrefix        "kXPGErrArticleLocked"
#define KXPGErrCopyDoc						kXPGStringPrefix        "KXPGErrCopyDoc"
#define kXPGErrParseFormeFailed				kXPGStringPrefix        "kXPGErrParseFormeFailed"
#define kXPGErrArtStatusNotUpdatedKey		kXPGStringPrefix		"kXPGErrArtStatusNotUpdatedKey"
#define kXPGErrImportImageFailed			kXPGStringPrefix        "kXPGErrImportImageFailed"
#define kXPGErrFailedToCreateInCopyFiles	kXPGStringPrefix		"kXPGErrFailedToCreateInCopyFiles"
#define kXPGErrInvalidInCopyID				kXPGStringPrefix        "kXPGErrInvalidInCopyID"
#define kXPGUpdateArticleXMLFileCmdKey		kXPGStringPrefix		"kXPGUpdateArticleXMLFileCmdKey"
#define kXPGErrInCopyFileNotExistKey		kXPGStringPrefix		"kXPGErrInCopyFileNotExistKey"
#define kXPGErrCannotOpenInCopyFileKey		kXPGStringPrefix		"kXPGErrCannotOpenInCopyFileKey"
#define kXPGFailedToLoadFormeDataKey		kXPGStringPrefix		"kXPGFailedToLoadFormeDataKey"
#define kXPGAskForStoriesUpdateKey			kXPGStringPrefix		"kXPGAskForStoriesUpdateKey"
#define kXPGFailedToUpdateStoriesKey		kXPGStringPrefix		"kXPGFailedToUpdateStoriesKey"
#define kXPGNoInCopyStoryInSelectionKey		kXPGStringPrefix		"kXPGNoInCopyStoryInSelectionKey"
#define kXPGMissingInCopyStoriesKey			kXPGStringPrefix		"kXPGMissingInCopyStoriesKey"
#define kXPGErrExportXmlFailed				kXPGStringPrefix        "kXPGErrExportXmlFailed"
#define kXPGErrDoSnapShotFailed				kXPGStringPrefix        "kXPGErrDoSnapShotFailed"
#define kXPGErrParseExportXMLFailed			kXPGStringPrefix		"kXPGErrParseExportXMLFailed"
#define kXPGErrFailedToCreateInCopyWS		kXPGStringPrefix		"kXPGErrFailedToCreateInCopyWS"
#define kXPGErrImportXmlFailed              kXPGStringPrefix		"kXPGErrImportXmlFailed"
#define kXPGUnTaggedTextKey					kXPGStringPrefix		"kXPGUnTaggedTextKey"
#define kXPGExtensionInCopyKey				kXPGStringPrefix		"kXPGExtensionInCopyKey"
#define kXPGExtensionWithoutDotInCopyKey	kXPGStringPrefix		"kXPGExtensionWithoutDotInCopyKey"
#define kXPGExtensionAffectationKey			kXPGStringPrefix		"kXPGExtensionAffectationKey"
#define kXPGExtensionAffectationWithoutDotKey				kXPGStringPrefix		"kXPGExtensionAffectationWithoutDotKey"

// Miscellaneous
#define kSetTagToStyleMapCmdKey				kXPGStringPrefix       "kSetTagToStyleMapCmdKey"
#define kXPGXMLImportTargetKey				kXPGStringPrefix	   "kXPGXMLImportTargetKey"
#define kXPGXMLDescFileKey					kXPGStringPrefix       "kXPGXMLDescFileKey"
#define kXPGXMLArticleIdKey					kXPGStringPrefix       "kXPGXMLArticleIdKey"
#define kXPGSetArticleIDCmdKey				kXPGStringPrefix       "kXPGSetArticleIDCmdKey"
#define kXPGNoNumeroTextKey					kXPGStringPrefix       "kXPGNoNumeroTextKey"
#define kXPGNumeroTextKey					kXPGStringPrefix       "kXPGNumeroTextKey"
#define kXPGAlertFormatImageKey				kXPGStringPrefix       "kXPGAlertFormatImageKey"
#define kXPGVersionTextKey					kXPGStringPrefix       "kXPGVersionTextKey"
#define kXPGXMLIndexTargetKey				kXPGStringPrefix	   "kXPGXMLIndexTargetKey"
#define kXPGXMLCountTargetKey				kXPGStringPrefix	   "kXPGXMLCountTargetKey"

// Menu item positions:

// Initial data format version Numbers
#define kXPGFirstMajorFormatNumber		RezLong(1)
#define kXPGFirstMinorFormatNumber		RezLong(0)

#define kXPGSecondMinorFormatNumber		kXPGFirstMinorFormatNumber + 1
#define kXPGThirdMinorFormatNumber		kXPGFirstMinorFormatNumber + 2
#define kXPGFourthMinorFormatNumber		kXPGFirstMinorFormatNumber + 3

// Data format version Numbers for the PluginVersion resource 
#define kXPGCurrentMajorFormatNumber	kXPGFirstMajorFormatNumber
#define kXPGCurrentMinorFormatNumber	kXPGThirdMinorFormatNumber

#endif // __XPGID_h__

//  Code generated by DollyXs code generator
