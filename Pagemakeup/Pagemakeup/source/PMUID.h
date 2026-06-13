/*
//	File:	PMUID.h
//
//	Author:	Trias
//
//	Date:	14-Apr-2003
//
//	Defines IDs used by the PMU plug-in.
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//	
//	NOTICE: Adobe permits you to use, modify, and distribute this file in
//	accordance with the terms of the Adobe license agreement accompanying it.
//	If you have received this file from a source other than Adobe, then your
//	use, modification, or distribution of it requires the prior written
//	permission of Adobe.
//
*/

#ifndef __PMUID_h__
#define __PMUID_h__

#include "SDKDef.h"
#if MACOS
#include "GlobalDefs.h"   // GD 8.0.6 NE PAS VIRER CET INCLUDE AU MERGE, MERCI, IL EST INDISPENSABLE ICI !
#endif

// Plug-in:
#if COMPOSER
	#define kPMUPluginName		"Composer"					// Name of this plug-in.
	#define kPMUCompanyKey		"Vio Worldwide Limited"		// Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
	#define kPMUCompanyValue	"Vio Worldwide Limited"		// Company name displayed externally.
#endif
 
#if PAGE_REF
	#define kPMUPluginName		"PageRef"					// Name of this plug-in.
	#define kPMUCompanyKey		"Trias Developpement"		// Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
	#define kPMUCompanyValue	"Trias Developpement"		// Company name displayed externally.
#else
	#define kPMUPluginName		"PageMakeUp"				// Name of this plug-in.
	#define kPMUCompanyKey		"Trias Developpement"		// Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
	#define kPMUCompanyValue	"Trias Developpement"		// Company name displayed externally.
#endif

#define kPMUPrefixNumber		0xdcc00 					// Unique prefix number for this plug-in(*Must* be obtained from Adobe Developer Support).
// Plug-in version string. Pulled from the shared TriasVersion.h header so
// every Trias plug-in (Pagemakeup / XPage / XRail / XRailClient) bumps in
// lock-step. Edit Gaia\TriasVersion.h to change the family-wide version.
#include "../../../Gaia/TriasVersion.h"
#define kPMUVersion				kTriasVersionString					// Version of this plug-in (for the About Box).
#define kPMUAuthor				"Trias"						// Author of this plug-in (for the About Box).

// Plug-in Prefix: (please change kPMUPrefixNumber above to modify the prefix.)
#define kPMUPrefix		RezLong(kPMUPrefixNumber)				// The unique numeric prefix for all object model IDs for this plug-in.
#define kPMUStringPrefix	SDK_DEF_STRINGIZE(kPMUPrefixNumber)	// The string equivalent of the unique prefix number for  this plug-in.

// PluginID:
DECLARE_PMID(kPlugInIDSpace, kPMUPluginID, kPMUPrefix + 0)

// ClassIDs:
DECLARE_PMID(kClassIDSpace, kPMUActionComponentBoss,		kPMUPrefix + 0)
DECLARE_PMID(kClassIDSpace, kPMUPanelWidgetBoss,			kPMUPrefix + 1)
DECLARE_PMID(kClassIDSpace, kPMUPrefsDialogBoss,			kPMUPrefix + 2)
DECLARE_PMID(kClassIDSpace, kPMUDocPathDialogBoss,			kPMUPrefix + 3)
DECLARE_PMID(kClassIDSpace, kPMUImageSettingDialogBoss,		kPMUPrefix + 4)
DECLARE_PMID(kClassIDSpace, kPMUExportPrefsDialogBoss,		kPMUPrefix + 5)
DECLARE_PMID(kClassIDSpace, kPMUTypeRefDialogBoss,			kPMUPrefix + 6)
DECLARE_PMID(kClassIDSpace, kPMUStartupShutdownBoss,		kPMUPrefix + 7)
DECLARE_PMID(kClassIDSpace, kPMUSetUpComposerPaletteCmdBoss,		kPMUPrefix + 8)
// Custom rollover icon button boss that aggregates IID_ITIP so the 3 main
// palette buttons (Exec TCL, Add hidden ref, Identify a block) can show
// tooltips. Derived from kRollOverIconButtonBoss in PMU.fr.
DECLARE_PMID(kClassIDSpace, kPMUButtonWithTipBoss,					kPMUPrefix + 9)

// ImplementationIDs:
DECLARE_PMID(kImplementationIDSpace, kPMUActionComponentImpl,		kPMUPrefix + 0)
DECLARE_PMID(kImplementationIDSpace, kPMUPanelWidgetObserverImpl,   kPMUPrefix + 1)
DECLARE_PMID(kImplementationIDSpace, kPMUDialogControllerImpl,		kPMUPrefix + 2)
DECLARE_PMID(kImplementationIDSpace, kPMUDialogObserverImpl,		kPMUPrefix + 3)
DECLARE_PMID(kImplementationIDSpace, kPMUDocPathDialogControllerImpl,   kPMUPrefix + 4)
DECLARE_PMID(kImplementationIDSpace, kPMUDocPathDialogObserverImpl,		kPMUPrefix + 5)
DECLARE_PMID(kImplementationIDSpace, kPMUImageSettingDialogControllerImpl,   kPMUPrefix + 6)
DECLARE_PMID(kImplementationIDSpace, kPMUImageSettingDialogObserverImpl,	 kPMUPrefix + 7)
DECLARE_PMID(kImplementationIDSpace, kPMUExportPrefsDialogControllerImpl,    kPMUPrefix + 8)
DECLARE_PMID(kImplementationIDSpace, kPMUTypeRefDialogControllerImpl,        kPMUPrefix + 9)
DECLARE_PMID(kImplementationIDSpace, kPMUSerialNumberDialogControllerImpl,   kPMUPrefix + 10)
DECLARE_PMID(kImplementationIDSpace, kPMUStartupShutdownImpl,   kPMUPrefix + 11)
DECLARE_PMID(kImplementationIDSpace, kPMUSetUpComposerPaletteCmdImpl,   kPMUPrefix + 12)
DECLARE_PMID(kImplementationIDSpace, kPMUButtonTipsImpl,				kPMUPrefix + 13)
//DECLARE_PMID(kImplementationIDSpace, kPMUProgressBarObserverImpl,   kPMUPrefix + 2)

// ActionIDs:
DECLARE_PMID(kActionIDSpace, kPMUAboutActionID,				kPMUPrefix + 0)
DECLARE_PMID(kActionIDSpace, kPMUPanelWidgetActionID,			kPMUPrefix + 1)
DECLARE_PMID(kActionIDSpace, kPMUSeparator1ActionID,			kPMUPrefix + 2)
DECLARE_PMID(kActionIDSpace, kPMUPopupAboutThisActionID,		kPMUPrefix + 3)
DECLARE_PMID(kActionIDSpace, kPMUExecTCLActionID,		kPMUPrefix + 11)
DECLARE_PMID(kActionIDSpace, kPMUReverseTCLActionID,		kPMUPrefix + 12)
DECLARE_PMID(kActionIDSpace, kPMUTCLPrefsActionID,		kPMUPrefix + 13)
DECLARE_PMID(kActionIDSpace, kPMUSeparatorActionID,		kPMUPrefix + 14)
DECLARE_PMID(kActionIDSpace, kPMUExportPermRefsTCLActionID,					kPMUPrefix + 15)
DECLARE_PMID(kActionIDSpace, kPMUDeleteAllPermRefsTCLActionID,				kPMUPrefix + 16)
DECLARE_PMID(kActionIDSpace, kPMUAutoTagDocumentWithPriceTCLActionID,		kPMUPrefix + 17)
DECLARE_PMID(kActionIDSpace, kPMUAutoTagDocumentWithReferenceTCLActionID,	kPMUPrefix + 18)
DECLARE_PMID(kActionIDSpace, kPMUAddReferenceTCLActionID,					kPMUPrefix + 19)
DECLARE_PMID(kActionIDSpace, kPMUSeparator0ActionID,						kPMUPrefix + 20)
DECLARE_PMID(kActionIDSpace, kPMUSeparator4ActionID,						kPMUPrefix + 21)
DECLARE_PMID(kActionIDSpace, kPMUSeparator3ActionID,						kPMUPrefix + 22)
DECLARE_PMID(kActionIDSpace, kPMUSeparator2ActionID,						kPMUPrefix + 23)
DECLARE_PMID(kActionIDSpace, kPMUSeparator5ActionID,						kPMUPrefix + 24)
DECLARE_PMID(kActionIDSpace, kPMUCreateILCRefsActionID,						kPMUPrefix + 25)
DECLARE_PMID(kActionIDSpace, kPMUExportILCRefsActionID,						kPMUPrefix + 26)
// "Identifier un bloc" — same code path as kPMUAddTCLRefWidgetID button on
// the palette (PMUUtils::AddTCLRef). New for the Main:PageMakeUp menu.
DECLARE_PMID(kActionIDSpace, kPMUAddTCLRefActionID,							kPMUPrefix + 27)
// Toggle the "show block IDs" preference (mirrors the palette checkbox).
DECLARE_PMID(kActionIDSpace, kPMUDisplayTCLRefActionID,						kPMUPrefix + 28)
// Toggle the "show hidden references in-text" preference.
DECLARE_PMID(kActionIDSpace, kPMUDisplayPermRefActionID,					kPMUPrefix + 29)
// Separator on the Main:PageMakeUp menu between actions and toggles.
DECLARE_PMID(kActionIDSpace, kPMUMainSeparatorActionID,						kPMUPrefix + 30)
DECLARE_PMID(kActionIDSpace, kPMUExportRefsListDebugActionID,				kPMUPrefix + 31)
DECLARE_PMID(kActionIDSpace, kPMURepairRefsListActionID,					kPMUPrefix + 32)

// WidgetIDs:
DECLARE_PMID(kWidgetIDSpace, kPMUPanelWidgetID,				kPMUPrefix + 0) 
DECLARE_PMID(kWidgetIDSpace, kPMUExecTCLWidgetID,			kPMUPrefix + 1) 
DECLARE_PMID(kWidgetIDSpace, kPMUReverseTCLWidgetID,			kPMUPrefix + 2) 
DECLARE_PMID(kWidgetIDSpace, kPMUTCLPrefsWidgetID,			kPMUPrefix + 3) 
DECLARE_PMID(kWidgetIDSpace, kPMUPrefsDialogWidgetID,			kPMUPrefix + 4) 
DECLARE_PMID(kWidgetIDSpace, kPMUExportWidgetID,			kPMUPrefix + 5) 
DECLARE_PMID(kWidgetIDSpace, kPMUDisplayTCLRefWidgetID,			kPMUPrefix + 6) 
DECLARE_PMID(kWidgetIDSpace, kPMUStopOnErrorWidgetID,			kPMUPrefix + 7) 
DECLARE_PMID(kWidgetIDSpace, kPMUDocButtonWidgetID,			kPMUPrefix + 8) 
DECLARE_PMID(kWidgetIDSpace, kPMUImportImageButtonWidgetID,			kPMUPrefix + 9) 
DECLARE_PMID(kWidgetIDSpace, kPMUExportTCLButtonWidgetID,			kPMUPrefix + 10) 


DECLARE_PMID(kWidgetIDSpace, kPMUDocPathDialogWidgetID,			kPMUPrefix + 14) 
DECLARE_PMID(kWidgetIDSpace, kPMUUseSavePathWidgetID,			kPMUPrefix + 15) 
DECLARE_PMID(kWidgetIDSpace, kPMUSelectSavePathWidgetID,			kPMUPrefix + 16) 
DECLARE_PMID(kWidgetIDSpace, kPMUSavePathWidgetID,			kPMUPrefix + 17) 
DECLARE_PMID(kWidgetIDSpace, kPMUUseOpenPathWidgetID,			kPMUPrefix + 18) 
DECLARE_PMID(kWidgetIDSpace, kPMUSelectOpenPathWidgetID,			kPMUPrefix + 19) 
DECLARE_PMID(kWidgetIDSpace, kPMUOpenPathWidgetID,			kPMUPrefix + 20) 
DECLARE_PMID(kWidgetIDSpace, kPMUImageSettingDialogWidgetID,			kPMUPrefix + 22) 
DECLARE_PMID(kWidgetIDSpace, kPMUSelectImagePathWidgetID,			kPMUPrefix + 23) 
DECLARE_PMID(kWidgetIDSpace, kPMUUseImagePathWidgetID,			kPMUPrefix + 24)
DECLARE_PMID(kWidgetIDSpace, kPMUImagePathWidgetID,			kPMUPrefix + 25)
DECLARE_PMID(kWidgetIDSpace, kPMUAllowImportImageWidgetID,			kPMUPrefix + 26)
DECLARE_PMID(kWidgetIDSpace, kPMUCreateTxtFrameWidgetID,			kPMUPrefix + 27)
DECLARE_PMID(kWidgetIDSpace, kPMUPrintFrameWidgetID,			kPMUPrefix + 28)
DECLARE_PMID(kWidgetIDSpace, kPMUDropDownColorListWidgetID,			kPMUPrefix + 29)
DECLARE_PMID(kWidgetIDSpace, kPMUColorTintComboBoxWidgetID,			kPMUPrefix + 30)
DECLARE_PMID(kWidgetIDSpace, kPMUColorTintNudgeWidgetID,			kPMUPrefix + 31)
DECLARE_PMID(kWidgetIDSpace, kPMUExportPrefsDialogWidgetID,			kPMUPrefix + 33)
DECLARE_PMID(kWidgetIDSpace, kPMUExportStyleWidgetID,			kPMUPrefix + 34)
DECLARE_PMID(kWidgetIDSpace, kPMUExportCJWidgetID,			kPMUPrefix + 35)
DECLARE_PMID(kWidgetIDSpace, kPMUExportColorsWidgetID,			kPMUPrefix + 36)
DECLARE_PMID(kWidgetIDSpace, kPMUExportImageFrameWidgetID,			kPMUPrefix + 37)
DECLARE_PMID(kWidgetIDSpace, kPMUExportTextFrameWidgetID,			kPMUPrefix + 38)
DECLARE_PMID(kWidgetIDSpace, kPMUExportRuleWidgetID,			kPMUPrefix + 39)
DECLARE_PMID(kWidgetIDSpace, kPMUExportTextWidgetID,			kPMUPrefix + 40)
DECLARE_PMID(kWidgetIDSpace, kPMUExportParaCmdWidgetID,			kPMUPrefix + 41)
DECLARE_PMID(kWidgetIDSpace, kPMUExportTextCmdWidgetID,			kPMUPrefix + 42)
DECLARE_PMID(kWidgetIDSpace, kPMUExportStyleCmdWidgetID,			kPMUPrefix + 43)
DECLARE_PMID(kWidgetIDSpace, kPMUTextOverFlowWidgetID,		kPMUPrefix +44) 
DECLARE_PMID(kWidgetIDSpace, kPMUExportLayersWidgetID,			kPMUPrefix + 45) 
DECLARE_PMID(kWidgetIDSpace, kPMUExportMastersWidgetID,			kPMUPrefix + 46) 
DECLARE_PMID(kWidgetIDSpace, kPMUAddTCLRefWidgetID,			kPMUPrefix + 47)
DECLARE_PMID(kWidgetIDSpace, kPMUAddPermRefWidgetID,			kPMUPrefix + 48) 
DECLARE_PMID(kWidgetIDSpace, kPMUSearchTCLRefWidgetID,			kPMUPrefix + 49) 
DECLARE_PMID(kWidgetIDSpace, kPMUSearchPermRefWidgetID,			kPMUPrefix + 50) 
DECLARE_PMID(kWidgetIDSpace, kPMUTypeRefDialogWidgetID,			kPMUPrefix + 52) 
DECLARE_PMID(kWidgetIDSpace, kPMURefEditWidgetID,			kPMUPrefix + 53) 
DECLARE_PMID(kWidgetIDSpace, kPMUDisplayPermRefWidgetID,		kPMUPrefix + 54) 
DECLARE_PMID(kWidgetIDSpace, kPMUUpdatePictureWidgetID,			kPMUPrefix + 55) 
DECLARE_PMID(kWidgetIDSpace, kPMUPDFExportWidgetID,			kPMUPrefix + 58) 
DECLARE_PMID(kWidgetIDSpace, kPMUEPSExportWidgetID,			kPMUPrefix + 59) 
DECLARE_PMID(kWidgetIDSpace, kPMUExportTypeClusterWidgetID,			kPMUPrefix + 60) 
DECLARE_PMID(kWidgetIDSpace, kComposerToolPanelWidgetID,			kPMUPrefix + 61) 
DECLARE_PMID(kWidgetIDSpace, kPMUParam1EditWidgetID,			kPMUPrefix + 62) 
DECLARE_PMID(kWidgetIDSpace, kPMUParam2EditWidgetID,			kPMUPrefix + 63) 
DECLARE_PMID(kWidgetIDSpace, kPMUParam3EditWidgetID,			kPMUPrefix + 64) 
DECLARE_PMID(kWidgetIDSpace, kPMUParam4EditWidgetID,			kPMUPrefix + 65) 
DECLARE_PMID(kWidgetIDSpace, kPMUExportPermRefsWidgetID,			kPMUPrefix + 66) 

DECLARE_PMID(kWidgetIDSpace, kPMUTypePermRefDialogWidgetID,			kPMUPrefix + 68) 
DECLARE_PMID(kWidgetIDSpace, kPMUPrintTCLRefWidgetID,			kPMUPrefix + 69) 


//DECLARE_PMID(kWidgetIDSpace, kPMUProgressBarWidgetID,			kPMUPrefix + 2) 
 
// "About Plug-ins" sub-menu:
#define kPMUAboutMenuKey			kPMUStringPrefix "kPMUAboutMenuKey"
#define kPMUAboutMenuPath			kSDKDefStandardAboutMenuPath kPMUCompanyKey
#define kPMUAboutBoxStringKey 		kPMUStringPrefix "kPMUAboutBoxStringKey"


// "Plug-ins" sub-menu:
#define kPMUPluginsMenuKey 			kPMUStringPrefix "kPMUPluginsMenuKey"
#define kPMUPluginsMenuPath			kSDKDefPlugInsStandardMenuPath kPMUCompanyKey kSDKDefDelimitMenuPath kPMUPluginsMenuKey

// Menu item keys:
#define kPMUExecTCLMenuItemKey		kPMUStringPrefix "kPMUExecTCLMenuItemKey"
#define kPMUReverseTCLMenuItemKey	kPMUStringPrefix "kPMUReverseTCLMenuItemKey"
#define kPMUAboutThisPlugInMenuKey  kPMUStringPrefix "kPMUAboutThisPlugInMenuKey"
#define kPMUTCLPrefsMenuItemKey		kPMUStringPrefix "kPMUTCLPrefsMenuItemKey"
#define kPMUExportPermRefsTCLMenuItemKey			kPMUStringPrefix "kPMUExportPermRefsTCLMenuItemKey"
#define kPMUExportRefsListDebugMenuItemKey			kPMUStringPrefix "kPMUExportRefsListDebugMenuItemKey"
#define kPMURepairRefsListMenuItemKey				kPMUStringPrefix "kPMURepairRefsListMenuItemKey"
#define kPMURepairRefsListAlertKey					kPMUStringPrefix "kPMURepairRefsListAlertKey"
#define kPMURepairRefsListDoneKey					kPMUStringPrefix "kPMURepairRefsListDoneKey"
#define kPMUExportRefsListDebugDoneKey				kPMUStringPrefix "kPMUExportRefsListDebugDoneKey"

// Other StringKeys:
#define kPMUExecTCLStringKey				kPMUStringPrefix	"kPMUExecTCLStringKey"
#define kPMUPanelTitleKey					kPMUStringPrefix	"kPMUPanelTitleKey"
#define kComposerToolPanelTitleKey			kPMUStringPrefix	"kComposerToolPanelTitleKey"
#define kPMUStaticTextKey					kPMUStringPrefix	"kPMUStaticTextKey"
#define kPMUInternalPopupMenuNameKey		kPMUStringPrefix	"kPMUInternalPopupMenuNameKey"
#define kPMUTargetMenuPath					kPMUInternalPopupMenuNameKey
#define kPMUAddRefMenuPath					kPMUStringPrefix	"kPMUAddRefMenuPath"




#define kComposerToolInternalPopupMenuNameKey		kPMUStringPrefix	"kComposerToolInternalPopupMenuNameKey"
#define kComposerToolTargetMenuPath					kComposerToolInternalPopupMenuNameKey
#define kPMUPrefsDialogTitleKey						kPMUStringPrefix "kPMUPrefsDialogTitleKey"
#define kPMUStaticTextKey							kPMUStringPrefix "kPMUStaticTextKey"
#define kPMUDisplayTCLRefTextKey			kPMUStringPrefix "kPMUDisplayTCLRefTextKey"
#define kPMUStopOnErrorTextKey				kPMUStringPrefix "kPMUStopOnErrorTextKey"
#define kPMUDocButtonTextKey				kPMUStringPrefix "kPMUDocButtonTextKey"
#define kPMUImportImageButtonTextKey		kPMUStringPrefix "kPMUImportImageButtonTextKey"
#define kPMUExportTCLButtonTextKey			kPMUStringPrefix "kPMUExportTCLButtonTextKey"
#define kPMUEnglishRadioTextKey				kPMUStringPrefix "kPMUEnglishRadioTextKey"
#define kPMUFrenchRadioTextKey				kPMUStringPrefix "kPMUFrenchRadioTextKey"
#define kPMUDocPathDialogTitleKey			kPMUStringPrefix "kPMUDocPathDialogTitleKey"
#define kPMUUsePathTextKey kPMUStringPrefix "kPMUUsePathTextKey"
#define kPMUStaticTextKey2 kPMUStringPrefix "kPMUStaticTextKey2"
#define kPMUStaticTextKey3 kPMUStringPrefix "kPMUStaticTextKey3"
#define kPMUStaticTextKey4 kPMUStringPrefix "kPMUStaticTextKey4"
#define kPMUImageSettingDialogTitleKey kPMUStringPrefix "kPMUImageSettingDialogTitleKey"
#define kPMUAllowImportImageTextKey kPMUStringPrefix "kPMUAllowImportImageTextKey"
#define kPMUPrintFrameTextKey kPMUStringPrefix "kPMUPrintFrameTextKey"
#define kPMUCreateTxtFrameTextKey kPMUStringPrefix "kPMUCreateTxtFrameTextKey"
#define kPMUStaticTextKey5 kPMUStringPrefix "kPMUStaticTextKey5"
#define kPMUWhiteKey kPMUStringPrefix "kPMUWhiteKey"
#define kPMUBlueKey kPMUStringPrefix "kPMUBlueKey"
#define kPMUCyanKey kPMUStringPrefix "kPMUCyanKey"
#define kPMUYellowKey kPMUStringPrefix "kPMUYellowKey"
#define kPMUMagentaKey kPMUStringPrefix "kPMUMagentaKey"
#define kPMUBlackKey kPMUStringPrefix "kPMUBlackKey"
#define kPMURegistrationKey kPMUStringPrefix "kPMURegistrationKey"
#define kPMURedKey kPMUStringPrefix "kPMURedKey"
#define kPMUGreenKey kPMUStringPrefix "kPMUGreenKey"
#define kPMUNoneKey kPMUStringPrefix "kPMUNoneKey"
#define kPMUPanelHeaderKey kPMUStringPrefix "kPMUPanelHeaderKey"
#define kPMUPanelGeometryKey kPMUStringPrefix "kPMUPanelGeometryKey"
#define kPMUPanelTextKey kPMUStringPrefix "kPMUPanelTextKey"
#define kPMUExportStyleKey kPMUStringPrefix "kPMUExportStyleKey"
#define kPMUExportCJKey kPMUStringPrefix "kPMUExportCJKey"
#define kPMUExportColorsKey kPMUStringPrefix "kPMUExportColorsKey"
#define kPMUExportTextFrameKey kPMUStringPrefix "kPMUExportTextFrameKey"
#define kPMUExportImageFrameKey kPMUStringPrefix "kPMUExportImageFrameKey"
#define kPMUExportRuleKey kPMUStringPrefix "kPMUExportRuleKey"
#define kPMUExportTextKey kPMUStringPrefix "kPMUExportTextKey"
#define kPMUExportParaCmdKey kPMUStringPrefix "kPMUExportParaCmdKey"
#define kPMUExportTextCmdKey kPMUStringPrefix "kPMUExportTextCmdKey"
#define kPMUExportStyleCmdKey kPMUStringPrefix "kPMUExportStyleCmdKey"
#define kPMUExportMastersKey kPMUStringPrefix "kPMUExportMastersKey"
#define kPMUExportLayersKey kPMUStringPrefix "kPMUExportLayersKey"
#define kPMUExportPrefsDialogTitleKey kPMUStringPrefix "kPMUExportPrefsDialogTitleKey"
#define kPMUSavePrefsPb kPMUStringPrefix "kPMUSavePrefsPb"
#define kPMUTypeRefDialogTitleKey kPMUStringPrefix "kPMUTypeRefDialogTitleKey"
#define kPMUStaticTextKey6 kPMUStringPrefix "kPMUStaticTextKey6"
#define kPMUTooManySelectedItemsKey kPMUStringPrefix "kPMUTooManySelectedItemsKey"
#define kPMUDisplayPermRefTextKey kPMUStringPrefix "kPMUDisplayPermRefTextKey"
#define kErrTextSelection kPMUStringPrefix "kErrTextSelection"
#define kErrLayoutSelection kPMUStringPrefix "kErrLayoutSelection"
#define kErrNoOversetBox kPMUStringPrefix "kErrNoOversetBox"
#define kErrAllPictureInPlace kPMUStringPrefix "kErrAllPictureInPlace"
#define kPMUTypeExportTextKey kPMUStringPrefix "kPMUTypeExportTextKey"
#define kPMUPDFTextKey kPMUStringPrefix "kPMUPDFTextKey"
#define kPMUEPSTextKey kPMUStringPrefix "kPMUEPSTextKey"
#define kPMUStaticTextParam1 kPMUStringPrefix "kPMUStaticTextParam1"
#define kPMUStaticTextParam2 kPMUStringPrefix "kPMUStaticTextParam2"
#define kPMUStaticTextParam3 kPMUStringPrefix "kPMUStaticTextParam3"
#define kPMUStaticTextParam4 kPMUStringPrefix "kPMUStaticTextParam4"
#define kPMUVersionKey kPMUStringPrefix "kPMUVersionKey"
#define kPMUExportPermRefsDoneKey kPMUStringPrefix "kPMUExportPermRefsDoneKey" 
#define kPMUSearchRefDialogTitleKey kPMUStringPrefix "kPMUSearchRefDialogTitleKey" 
#define kPMUPrintTCLRefTextKey kPMUStringPrefix "kPMUPrintTCLRefTextKey" 
#define kPMUExportTableFrameKey						kPMUStringPrefix "kPMUExportTableFrameKey" 
#define kPMUAutoTagDocument1TCLMenuItemKey			kPMUStringPrefix "kPMUAutoTagDocument1TCLMenuItemKey"	
#define kPMUAutoTagDocument2TCLMenuItemKey			kPMUStringPrefix "kPMUAutoTagDocument2TCLMenuItemKey"	 
#define kPMUDeleteAllPermRefsTCLMenuItemKey			kPMUStringPrefix "kPMUDeleteAllPermRefsTCLMenuItemKey"
#define kPMUAddReferenceMenuKey						kPMUStringPrefix "kPMUAddReferenceMenuKey"
#define kPMUDeleteAlltPermRefsDoneKey				kPMUStringPrefix "kPMUDeleteAlltPermRefsDoneKey"
#define kPMUDeletePermRefskAlertKey                 kPMUStringPrefix "kPMUDeletePermRefskAlertKey"
#define kPMUAutoTagDocumentPermRefsDoneKey          kPMUStringPrefix "kPMUAutoTagDocumentPermRefsDoneKey"
#define kPMUCreateILCRefsMenuItemKey				kPMUStringPrefix "kPMUCreateILCRefsMenuItemKey"
#define kPMUExportILCRefsMenuItemKey				kPMUStringPrefix "kPMUExportILCRefsMenuItemKey"

// Tooltip keys (in-resource via kPMUButtonWithTipBoss + PMUButtonTips).
#define kPMUExecTCLTipKey							kPMUStringPrefix "kPMUExecTCLTipKey"
#define kPMUAddPermRefTipKey						kPMUStringPrefix "kPMUAddPermRefTipKey"
#define kPMUAddTCLRefTipKey							kPMUStringPrefix "kPMUAddTCLRefTipKey"

// Main:PageMakeUp menu items (3 actions + 2 display toggles).
#define kPMUAddTCLRefMenuItemKey					kPMUStringPrefix "kPMUAddTCLRefMenuItemKey"
#define kPMUDisplayTCLRefMenuItemKey				kPMUStringPrefix "kPMUDisplayTCLRefMenuItemKey"
#define kPMUDisplayPermRefMenuItemKey				kPMUStringPrefix "kPMUDisplayPermRefMenuItemKey"

// Menu item positions:
#define kPMUExecTCLMenuItemPosition					1.0
#define	kPMUSeparator0MenuItemPosition				2.0
#define kPMUReverseTCLMenuItemPosition				3.0
#define kPMUExportContentMenuItemPosition			4.0
#define	kPMUSeparator1MenuItemPosition				5.0
#define kPMUDeleteAllPermRefsMenuItemPosition		6.0
#define kPMUExportRefsListDebugMenuItemPosition		6.3
#define kPMURepairRefsListMenuItemPosition			6.6
#define	kPMUSeparator2MenuItemPosition				7.0
#define kPMUAutoTagDocumentMenuItemPosition			8.0
#define	kPMUSeparator5MenuItemPosition				8.4
#define kPMUCreateILCRefsMenuItemPosition			8.5
#define kPMUExportILCRefsMenuItemPosition			8.6
#define	kPMUSeparator3MenuItemPosition				9.0
#define kPMUTCLPrefsMenuItemPosition				10.0
#define	kPMUSeparator4MenuItemPosition				11.0
#define kPMUAboutThisMenuItemPosition				12.0

// Positions inside the Main:PageMakeUp top-level menu — explicit ordering
// (the existing AddReference entry used kSDKDefAlphabeticPosition before).
#define kPMUMainExecTCLMenuItemPosition				1.0
#define kPMUMainAddPermRefMenuItemPosition			2.0
#define kPMUMainAddTCLRefMenuItemPosition			3.0
#define kPMUMainSeparatorMenuItemPosition			4.0
#define kPMUMainDisplayTCLRefMenuItemPosition		5.0
#define kPMUMainDisplayPermRefMenuItemPosition		6.0

// Icon resource ID
#define kPMUExecTCLPNGIconResourceID			101
#define kPMUPrefsPNGIconResourceID				102
#define kPMUReverseTCLPNGIconResourceID			103
#define kPMUInFolderPNGIconResourceID			104
#define kPMUOutFolderPNGIconResourceID			105
#define kPMUAddTCLRefPNGIconResourceID			106
#define kPMUAddPermRefPNGIconResourceID			107
#define kPMUSearchTCLRefPNGIconResourceID		108
#define kPMUSearchPermRefPNGIconResourceID		109
#define kPMUTextOverFlowPNGIconResourceID		110
#define kPMUUpdatePicturePNGIconResourceID		111
#define kComposerPNGIconResourceID				112
#define kPMUExportPermRefsPNGIconResourceID		114

// Palette resource IDs
#define kPMUPanelResourceID					kSDKDefPanelResourceID 
#define kPMUTypePermRefDialogResourceID		kSDKDefPanelResourceID + 1
#define kComposerToolPanelResourceID		kSDKDefPanelResourceID + 2
#define kPMUDocPathDialogResourceID			kSDKDefPanelResourceID + 3
#define kPMUImageSettingDialogResourceID	kSDKDefPanelResourceID + 4
#define kPMUExportPrefsDialogResourceID		kSDKDefPanelResourceID + 5
#define kPMUTypeRefDialogResourceID			kSDKDefPanelResourceID + 6
#define kComposerAboutDialogResourceID		kSDKDefPanelResourceID + 7
#define kPMUExportResourceID				kSDKDefPanelResourceID + 8


// Initial data format version numbers
#define kPMUFirstMajorFormatNumber  kSDKDef_30_PersistMajorVersionNumber
#define kPMUFirstMinorFormatNumber  kSDKDef_30_PersistMinorVersionNumber

// Data format version numbers for the PluginVersion resource 
#define kPMUCurrentMajorFormatNumber kPMUFirstMajorFormatNumber // most recent major format change
#define kPMUCurrentMinorFormatNumber kPMUFirstMinorFormatNumber // most recent minor format change

#endif // __PMUID_h__
//  Generated by Dolly build 17: template "IfPanelMenu".
// End, PMUID.h.
