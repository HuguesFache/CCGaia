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


#ifndef __XRLUIID_h__
#define __XRLUIID_h__

#include "SDKDef.h"

// Company:
#define kXRLUICompanyKey	"Trias Developpement"		// Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
#define kXRLUICompanyValue	"Trias Developpement"	// Company name displayed externally.

// Plug-in:
#define kXRLUIPluginName	"XRailUI"			// Name of this plug-in.
#define kXRLUIPrefixNumber	0xe1200 		// Unique prefix number for this plug-in(*Must* be obtained from Adobe Developer Support).
// Plug-in version string. Pulled from the shared TriasVersion.h header so
// every Trias plug-in bumps in lock-step. Edit Gaia\TriasVersion.h to
// change the family-wide version.
#include "../../../Gaia/TriasVersion.h"
#define kXRLUIVersion		kTriasVersionString				// Version of this plug-in (for the About Box).
#define kXRLUIAuthor		"Trias"					// Author of this plug-in (for the About Box).

// Plug-in Prefix: (please change kXRLUIPrefixNumber above to modify the prefix.)
#define kXRLUIPrefix		RezLong(kXRLUIPrefixNumber)				// The unique numeric prefix for all object model IDs for this plug-in.
#define kXRLUIStringPrefix	SDK_DEF_STRINGIZE(kXRLUIPrefixNumber)	// The string equivalent of the unique prefix number for  this plug-in.

// Missing plug-in: (see ExtraPluginInfo resource)
#define kXRLUIMissingPluginURLValue		kSDKDefPartnersStandardValue_enUS // URL displayed in Missing Plug-in dialog
#define kXRLUIMissingPluginAlertValue	kSDKDefMissingPluginAlertValue // Message displayed in Missing Plug-in dialog - provide a string that instructs user how to solve their missing plug-in problem

// PluginID:
DECLARE_PMID(kPlugInIDSpace, kXRLUIPluginID, kXRLUIPrefix + 0)

// ClassIDs:
DECLARE_PMID(kClassIDSpace, kXRLUIActionComponentBoss, kXRLUIPrefix + 0)
DECLARE_PMID(kClassIDSpace, kXRLUIOpenLoginDialogCmdBoss, kXRLUIPrefix + 1)
DECLARE_PMID(kClassIDSpace, kXRLUIPrefsDialogBoss, kXRLUIPrefix + 2)
DECLARE_PMID(kClassIDSpace, kXRLUIChangeStateDialogBoss, kXRLUIPrefix + 3)
DECLARE_PMID(kClassIDSpace, kXRLUILoginDialogBoss, kXRLUIPrefix + 4)
DECLARE_PMID(kClassIDSpace, kXRLUIStartupShutdownBoss, kXRLUIPrefix + 5)
DECLARE_PMID(kClassIDSpace, kXRLUILinkerDialogBoss, kXRLUIPrefix + 6)
DECLARE_PMID(kClassIDSpace, kXRLUIIDDropDownListWidgetBoss, kXRLUIPrefix + 7)
// Commentaires palette (Window > Gaia > Commentaires).
DECLARE_PMID(kClassIDSpace, kXRLUICommentsPanelWidgetBoss,	kXRLUIPrefix + 8)
DECLARE_PMID(kClassIDSpace, kXRLUICommentsViewWidgetBoss,	kXRLUIPrefix + 9)
DECLARE_PMID(kClassIDSpace, kXRLUICommentsNodeWidgetBoss,	kXRLUIPrefix + 10)
// Case à cocher de validation d'une ligne — checkbox dérivé qui ajoute son
// propre observer (appelle Plugin_Comm_Valid au toggle).
DECLARE_PMID(kClassIDSpace, kXRLUICommentsCellCheckWidgetBoss,	kXRLUIPrefix + 11)
// Adornment dessinant les pins de commentaires sur les pages Gaia.
DECLARE_PMID(kClassIDSpace, kXRLUICommentsPinAdornmentBoss,		kXRLUIPrefix + 12)

//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 13)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 14)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 15)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 16)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 17)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 18)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 19)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 20)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 21)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 22)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 23)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 24)
//DECLARE_PMID(kClassIDSpace, kXRLUIBoss, kXRLUIPrefix + 25)


// MessageIDs:
// Broadcast on the session subject (IID_IREFRESHPROTOCOL) right after a
// successful VerifLogin in XRailLoginDialogController, and also when the
// login dialog is cancelled (gLogin gets cleared). Subscribers — the XPage
// palettes — react by refreshing their own preferences and toggling
// between "Sign in" prompt and normal content.
DECLARE_PMID(kMessageIDSpace, kXRLUILoginChangedMsg, kXRLUIPrefix + 0)

// InterfaceIDs:
DECLARE_PMID(kInterfaceIDSpace, IID_IINTLIST, kXRLUIPrefix + 0)
// Protocol IID for kXRLUILoginChangedMsg broadcasts on the session subject.
// XRail-owned (no dependency on XPage's IID_IREFRESHPROTOCOL) so XRail-only
// installs work. Subscribers — currently the XPage palette observers — must
// AttachObserver(this, IID_IXRLUILOGINPROTOCOL) to receive it.
DECLARE_PMID(kInterfaceIDSpace, IID_IXRLUILOGINPROTOCOL, kXRLUIPrefix + 1)
// Comment list interface aggregated on kXRLUICommentsPanelWidgetBoss; the
// observer fills it on refresh, the tree view's adapter + widget mgr read
// from it via the IID_IDATAMODEL pointer set on the tree view widget.
DECLARE_PMID(kInterfaceIDSpace, IID_IXRLUICOMMENTLIST, kXRLUIPrefix + 2)
// IID under which the comment list pointer is stashed on the tree view
// widget boss (via kPMUnknownDataImpl). Distinct from IID_IXRLUICOMMENTLIST
// — the data-model slot is a plain IPMUnknown* hook, while the COMMENTLIST
// IID identifies the typed accessor interface.
DECLARE_PMID(kInterfaceIDSpace, IID_IXRLUIDATAMODEL, kXRLUIPrefix + 3)
// Shadow IID under which the standard tree-node IEventHandler is aggregated
// on kXRLUICommentsNodeWidgetBoss so XRLUICommentsNodeEH can override
// IID_IEVENTHANDLER (to catch double-click) while still delegating every
// other event to the SDK-provided handler. Same pattern as PnlTrvNodeEH in
// the SDK paneltreeview sample.
DECLARE_PMID(kInterfaceIDSpace, IID_IXRLUICOMMENTSSHADOWEH, kXRLUIPrefix + 4)

//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 6)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 7)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 8)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 9)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 10)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 11)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 12)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 13)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 14)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 15)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 16)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 17)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 18)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 19)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 20)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 21)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 22)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 23)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 24)
//DECLARE_PMID(kInterfaceIDSpace, IID_IXRLINTERFACE, kXRLUIPrefix + 25)


// ImplementationIDs:
DECLARE_PMID(kImplementationIDSpace, kXRLUIActionComponentImpl, kXRLUIPrefix + 0 )
DECLARE_PMID(kImplementationIDSpace, kXRLUIOpenLoginDialogCmdImpl, kXRLUIPrefix + 1)
DECLARE_PMID(kImplementationIDSpace, kXRLUIChangeStateDialogControllerImpl, kXRLUIPrefix + 2)
DECLARE_PMID(kImplementationIDSpace, kXRLUILoginDialogControllerImpl, kXRLUIPrefix + 3)
DECLARE_PMID(kImplementationIDSpace, kXRLUIDialogControllerImpl, kXRLUIPrefix + 4)
DECLARE_PMID(kImplementationIDSpace, kXRLUIUIStartupShutdownImpl, kXRLUIPrefix + 5)
DECLARE_PMID(kImplementationIDSpace, kXRLUILinkerDialogControllerImpl, kXRLUIPrefix + 6)
DECLARE_PMID(kImplementationIDSpace, kXRLUIIntListImpl, kXRLUIPrefix + 7)
DECLARE_PMID(kImplementationIDSpace, kXRLUILinkerDialogObserverImpl, kXRLUIPrefix + 8)
DECLARE_PMID(kImplementationIDSpace, kXRLUICommentsPanelObserverImpl,			kXRLUIPrefix + 9)
DECLARE_PMID(kImplementationIDSpace, kXRLUICommentListImpl,						kXRLUIPrefix + 10)
DECLARE_PMID(kImplementationIDSpace, kXRLUICommentsTVHierarchyAdapterImpl,		kXRLUIPrefix + 11)
DECLARE_PMID(kImplementationIDSpace, kXRLUICommentsTVWidgetMgrImpl,				kXRLUIPrefix + 12)
DECLARE_PMID(kImplementationIDSpace, kXRLUICommentsNodeEHImpl,					kXRLUIPrefix + 13)
DECLARE_PMID(kImplementationIDSpace, kXRLUICommentsCellCheckObserverImpl,		kXRLUIPrefix + 14)
DECLARE_PMID(kImplementationIDSpace, kXRLUICommentsPinAdornmentImpl,				kXRLUIPrefix + 15)
//DECLARE_PMID(kImplementationIDSpace, kXRLUIImpl, kXRLUIPrefix + 17)
//DECLARE_PMID(kImplementationIDSpace, kXRLUIImpl, kXRLUIPrefix + 18)
//DECLARE_PMID(kImplementationIDSpace, kXRLUIImpl, kXRLUIPrefix + 19)
//DECLARE_PMID(kImplementationIDSpace, kXRLUIImpl, kXRLUIPrefix + 20)
//DECLARE_PMID(kImplementationIDSpace, kXRLUIImpl, kXRLUIPrefix + 21)
//DECLARE_PMID(kImplementationIDSpace, kXRLUIImpl, kXRLUIPrefix + 22)
//DECLARE_PMID(kImplementationIDSpace, kXRLUIImpl, kXRLUIPrefix + 23)
//DECLARE_PMID(kImplementationIDSpace, kXRLUIImpl, kXRLUIPrefix + 24)
//DECLARE_PMID(kImplementationIDSpace, kXRLUIImpl, kXRLUIPrefix + 25)


// ActionIDs:
DECLARE_PMID(kActionIDSpace, kXRLUIAboutActionID, kXRLUIPrefix + 0)
DECLARE_PMID(kActionIDSpace, kXRLUIPreferencesActionID, kXRLUIPrefix + 1)
DECLARE_PMID(kActionIDSpace, kXRLUIConnectionActionID, kXRLUIPrefix + 2)
DECLARE_PMID(kActionIDSpace, kXRLUIEnablePreviewActionID, kXRLUIPrefix + 7)
DECLARE_PMID(kActionIDSpace, kXRLUIExternalPDFActionID, kXRLUIPrefix + 9)
DECLARE_PMID(kActionIDSpace, kXRLUIXRailLinkActionID, kXRLUIPrefix + 10)
DECLARE_PMID(kActionIDSpace, kXRLUISep1ActionID, kXRLUIPrefix + 11)
DECLARE_PMID(kActionIDSpace, kXRLUISep2ActionID, kXRLUIPrefix + 12)
DECLARE_PMID(kActionIDSpace, kXRLUIXRailUnLinkActionID, kXRLUIPrefix + 15)
DECLARE_PMID(kActionIDSpace, kXRLUISep3ActionID, kXRLUIPrefix + 17)
// Show/hide the Commentaires palette (PanelList field 4).
DECLARE_PMID(kActionIDSpace, kXRLUICommentsPanelWidgetActionID, kXRLUIPrefix + 18)
//DECLARE_PMID(kActionIDSpace, kXRLUIActionID, kXRLUIPrefix + 19)
//DECLARE_PMID(kActionIDSpace, kXRLUIActionID, kXRLUIPrefix + 19)
//DECLARE_PMID(kActionIDSpace, kXRLUIActionID, kXRLUIPrefix + 20)
//DECLARE_PMID(kActionIDSpace, kXRLUIActionID, kXRLUIPrefix + 21)
//DECLARE_PMID(kActionIDSpace, kXRLUIActionID, kXRLUIPrefix + 22)
//DECLARE_PMID(kActionIDSpace, kXRLUIActionID, kXRLUIPrefix + 23)
//DECLARE_PMID(kActionIDSpace, kXRLUIActionID, kXRLUIPrefix + 24)
//DECLARE_PMID(kActionIDSpace, kXRLUIActionID, kXRLUIPrefix + 25)


// WidgetIDs:
DECLARE_PMID(kWidgetIDSpace, kXRLUIPrefsDialogWidgetID, kXRLUIPrefix + 0)
DECLARE_PMID(kWidgetIDSpace, kIPField1WidgetID, kXRLUIPrefix + 1)
DECLARE_PMID(kWidgetIDSpace, kPortEditBoxWidgetID, kXRLUIPrefix + 2)
DECLARE_PMID(kWidgetIDSpace, kXRLUILoginDialogWidgetID, kXRLUIPrefix + 4)
DECLARE_PMID(kWidgetIDSpace, kXRLUILoginUserWidgetID, kXRLUIPrefix + 6)
DECLARE_PMID(kWidgetIDSpace, kXRLUIPasswordUserWidgetID, kXRLUIPrefix + 7)
DECLARE_PMID(kWidgetIDSpace, kXRLUIRememberMeWidgetID, kXRLUIPrefix + 200)
DECLARE_PMID(kWidgetIDSpace, kXRLUIChangeStateDialogWidgetID, kXRLUIPrefix + 8)
DECLARE_PMID(kWidgetIDSpace, kXRLUIStateListBoxWidgetID, kXRLUIPrefix + 9)
DECLARE_PMID(kWidgetIDSpace, kXRLUIListParentWidgetId, kXRLUIPrefix + 10)
DECLARE_PMID(kWidgetIDSpace, kXRLUIListCol1WidgetID, kXRLUIPrefix + 11)
DECLARE_PMID(kWidgetIDSpace, kIPField2WidgetID, kXRLUIPrefix + 12)
DECLARE_PMID(kWidgetIDSpace, kIPField3WidgetID, kXRLUIPrefix + 13)
DECLARE_PMID(kWidgetIDSpace, kIPField4WidgetID, kXRLUIPrefix + 14)
DECLARE_PMID(kWidgetIDSpace, kPrefsTypeWidgetID, kXRLUIPrefix + 16)
DECLARE_PMID(kWidgetIDSpace, kXRLUILinkerDialogWidgetID, kXRLUIPrefix + 17)
DECLARE_PMID(kWidgetIDSpace, kXRLUILevel1ListBoxWidgetID, kXRLUIPrefix + 19)
DECLARE_PMID(kWidgetIDSpace, kXRLUILevel2ListBoxWidgetID, kXRLUIPrefix + 20)
DECLARE_PMID(kWidgetIDSpace, kXRLUILevel3ListBoxWidgetID, kXRLUIPrefix + 21)
DECLARE_PMID(kWidgetIDSpace, kXRLUILevel4ListBoxWidgetID, kXRLUIPrefix + 22)
DECLARE_PMID(kWidgetIDSpace, kXRLUIRefreshWidgetID, kXRLUIPrefix + 34)
DECLARE_PMID(kWidgetIDSpace, kXRLUIDocumentNameWidgetID, kXRLUIPrefix + 35)
DECLARE_PMID(kWidgetIDSpace, kXRLUIMultilineVScrollBarWidgetID, kXRLUIPrefix + 36)
DECLARE_PMID(kWidgetIDSpace, kXRLUIMessageWidgetID, kXRLUIPrefix + 37)
// Commentaires palette widgets.
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsPanelWidgetID,			kXRLUIPrefix + 38)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsViewWidgetID,			kXRLUIPrefix + 39)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsRefreshButtonWidgetID,	kXRLUIPrefix + 40)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsEmptyTextWidgetID,		kXRLUIPrefix + 41)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsHeaderRowWidgetID,		kXRLUIPrefix + 42)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsHeaderCheckWidgetID,		kXRLUIPrefix + 43)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsHeaderDateWidgetID,		kXRLUIPrefix + 44)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsHeaderTextWidgetID,		kXRLUIPrefix + 45)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsHeaderUserWidgetID,		kXRLUIPrefix + 46)
// Per-row cells in the tree view node widget.
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsCellCheckWidgetID,		kXRLUIPrefix + 47)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsCellDateWidgetID,		kXRLUIPrefix + 48)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsCellTextWidgetID,		kXRLUIPrefix + 49)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsCellUserWidgetID,		kXRLUIPrefix + 50)
// Overlay placeholders shown by UpdateLoginState() when the palette is not
// usable (no XRail login or no Gaia page in the front document).
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsLoginPromptTextWidgetID,		kXRLUIPrefix + 51)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsLoginPromptButtonWidgetID,	kXRLUIPrefix + 52)
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsNoGaiaPageTextWidgetID,		kXRLUIPrefix + 53)
// WidgetID stamped on the per-row node widget resource — returned by the
// tree view's IID_ITREEVIEWWIDGETMGR::GetWidgetTypeForNode so the framework
// knows which template to clone when materialising rows. NOT a resource ID.
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsNodeWidgetTemplateID,			kXRLUIPrefix + 54)
// "Show all comments" checkbox in the panel footer — toggles the spread
// adornment that paints a marker per comment at its (X%, Y%) position.
DECLARE_PMID(kWidgetIDSpace, kXRLUICommentsShowAllCheckWidgetID,			kXRLUIPrefix + 55)

// "About Plug-ins" sub-menu:
#define kXRLUIAboutMenuKey			kXRLUIStringPrefix "kXRLUIAboutMenuKey"
#define kXRLUIAboutMenuPath			kSDKDefStandardAboutMenuPath kXRLUICompanyKey

// "Plug-ins" sub-menu:
#define kXRLUIPluginsMenuKey 		kXRLUIStringPrefix "kXRLUIPluginsMenuKey"
#define kXRLUIPluginsMenuPath		"Main" kSDKDefDelimitMenuPath kXRLUIPluginsMenuKey

// Menu item keys:
#define kXRLUIMenu					kXRLUIStringPrefix "kXRLUIMenu"
#define kXRLUISousMenuPreferences	kXRLUIStringPrefix "kXRLUISousMenuPreferences"
#define kXRLUISousMenuConnection	kXRLUIStringPrefix "kXRLUISousMenuConnection"
#define kXRLUISousMenuDeconnection	kXRLUIStringPrefix "kXRLUISousMenuDeconnection"
#define kXRLUIPrefsIPLabel			kXRLUIStringPrefix "kXRLUIPrefsIPLabel"
#define kXRLUIPrefsPortLabel	    kXRLUIStringPrefix "kXRLUIPrefsPortLabel"

// "Plug-ins" sub-menu item keys:
#define kXRLUIDialogMenuItemKey		kXRLUIStringPrefix "kXRLUIDialogMenuItemKey"

// "Plug-ins" sub-menu item positions:
#define kXRLUIAboutMenuItemPosition				1.0
#define kXRLUIConnectionMenuItemPosition		2.0
#define kXRLUIPreferencesMenuItemPosition		3.0
#define kXRLUISep1MenuItemPosition				11.0
#define kXRLUIExternalPDFMenuItemPosition		20.0
#define kXRLUISep2MenuItemPosition				30.0
#define kXRLUIEnablePreviewMenuItemPosition		31.0
#define kXRLUIXRailLinkMenuItemPosition			32.0
#define kXRLUIXRailUnLinkMenuItemPosition		33.0
#define kXRLUISep3MenuItemPosition				34.0
#define kXRLUISendToServerMenuItemPosition		35.1

// Other string keys
#define kXRLUITargetMenuPath			kXRLUIPluginsMenuPath
#define kXRLUIPrefsDialogTitleKey		kXRLUIStringPrefix "kXRLUIPrefsDialogTitleKey"
#define kXRLUILoginDialogTitleKey		kXRLUIStringPrefix "kXRLUILoginDialogTitleKey"
#define kXRLUIChangeStateDialogTitleKey	kXRLUIStringPrefix "kXRLUIChangeStateDialogTitleKey"
#define kXRLUILinkerDialogTitleKey		kXRLUIStringPrefix "kXRLUILinkerDialogTitleKey"
#define kXRLUIAboutBoxStringKey			kXRLUIStringPrefix "kXRLUIAboutBoxStringKey"
#define kXRLUIErrorWSGetStatusKey 		kXRLUIStringPrefix "kXRLUIErrorWSGetStatusKey"
#define kXRLUIErrorWSSetPageStatusKey 	kXRLUIStringPrefix "kXRLUIErrorWSSetPageStatusKey"
#define kXRLUIErrorWSGetSavePathKey 	kXRLUIStringPrefix "kXRLUIErrorWSGetSavePathKey"
#define kXRLUIErrorBlankLoginKey 		kXRLUIStringPrefix "kXRLUIErrorBlankLoginKey"
#define kXRLUIErrorBlankPasswordKey		kXRLUIStringPrefix "kXRLUIErrorBlankPasswordKey"
#define kXRLUIErrorWSGetLevelKey		kXRLUIStringPrefix "kXRLUIErrorWSGetLevelKey"
#define kXRLUIErrorWSLinkPagesKey		kXRLUIStringPrefix "kXRLUIErrorWSLinkPagesKey"
#define kXRLUILoginTextKey 				kXRLUIStringPrefix "kXRLUILoginTextKey"
#define kXRLUIPasswordTextKey 			kXRLUIStringPrefix "kXRLUIPasswordTextKey"
#define kXRLUIRememberMeTextKey			kXRLUIStringPrefix "kXRLUIRememberMeTextKey"
#define kXRLUIEnablePreviewMenuKey 		kXRLUIStringPrefix "kXRLUIEnablePreviewMenuKey"
#define kXRLUIExternalPDFMenuKey 		kXRLUIStringPrefix "kXRLUIExternalPDFMenuKey"
#define kXRLUIXRailLinkMenuKey	 		kXRLUIStringPrefix "kXRLUIXRailLinkMenuKey"
#define kXRLUIXRailUnLinkMenuKey	 	kXRLUIStringPrefix "kXRLUIXRailUnLinkMenuKey"
#define kXRLUIModeRobotTextKey 			kXRLUIStringPrefix "kXRLUIModeRobotTextKey"
#define kXRLUIMessageLinkerKey 			kXRLUIStringPrefix "kXRLUIMessageLinkerKey"
// Placeholder shown at the top of the Linker dialog's "Titre" dropdown so
// the user can't think a real title is already selected at open time.
#define kXRLUIChooseTitlePromptKey 		kXRLUIStringPrefix "kXRLUIChooseTitlePromptKey"
#define kXRLUIAnnonceurKey 		        kXRLUIStringPrefix "kXRLUIAnnonceurKey"
#define kXRLUIProduitKey 		        kXRLUIStringPrefix "kXRLUIProduitKey"
#define kXRLUITypeSurfaceKey 		    kXRLUIStringPrefix "kXRLUITypeSurfaceKey"
#define kXRLUIEmplacementKey 		    kXRLUIStringPrefix "kXRLUIEmplacementKey"
#define kXRLUIInternalPopupMenuNameKey 	kXRLUIStringPrefix "kXRLUIInternalPopupMenuNameKey"
#define kXRLUISendToServerMenuKey       kXRLUIStringPrefix "kXRLUISendToServerMenuKey"
#define kXRLUIErrorWSKey			    kXRLUIStringPrefix "kXRLUIErrorWSKey"
#define kXRLUIErrorSendToServerKey	    kXRLUIStringPrefix "kXRLUIErrorSendToServerKey"

// Commentaires palette strings.
#define kXRLUICommentsPanelTitleKey		kXRLUIStringPrefix "kXRLUICommentsPanelTitleKey"
#define kXRLUICommentsColCheckKey		kXRLUIStringPrefix "kXRLUICommentsColCheckKey"
#define kXRLUICommentsColDateKey		kXRLUIStringPrefix "kXRLUICommentsColDateKey"
#define kXRLUICommentsColTextKey		kXRLUIStringPrefix "kXRLUICommentsColTextKey"
#define kXRLUICommentsColUserKey		kXRLUIStringPrefix "kXRLUICommentsColUserKey"
#define kXRLUICommentsEmptyKey			kXRLUIStringPrefix "kXRLUICommentsEmptyKey"
#define kXRLUICommentsRefreshTipKey		kXRLUIStringPrefix "kXRLUICommentsRefreshTipKey"
#define kXRLUICommentsLoginPromptTextKey	kXRLUIStringPrefix "kXRLUICommentsLoginPromptTextKey"
#define kXRLUICommentsLoginPromptButtonKey	kXRLUIStringPrefix "kXRLUICommentsLoginPromptButtonKey"
#define kXRLUICommentsNoGaiaPageTextKey		kXRLUIStringPrefix "kXRLUICommentsNoGaiaPageTextKey"
#define kXRLUICommentsShowAllKey			kXRLUIStringPrefix "kXRLUICommentsShowAllKey"

// Drag & Drop flavor
#define kXRLUIPubDragDropCustomFlavor		PMFlavor('XRUI')

// Initial data format version numbers
#define kXRLUIFirstMajorFormatNumber		RezLong(1)
#define kXRLUIFirstMinorFormatNumber		RezLong(0)

// Ressources ID

#define kXRLUILoginDialogResourceID				kSDKDefPanelResourceID 
#define kXRLUIChangeStateDialogResourceID		kSDKDefPanelResourceID + 2
#define kXRLUILinkerDialogResourceID			kSDKDefPanelResourceID + 3
#define kXRLUIIconRefreshResID					kSDKDefPanelResourceID + 5
// Commentaires palette resources.
#define kXRLUICommentsPanelResourceID			kSDKDefPanelResourceID + 6
#define kXRLUICommentsNodeWidgetResID			kSDKDefPanelResourceID + 7
#define kXRLUICommentsRefreshIconResID			kSDKDefPanelResourceID + 8


// Data format version numbers for the PluginVersion resource 
#define kXRLUICurrentMajorFormatNumber	kXRLUIFirstMajorFormatNumber
#define kXRLUICurrentMinorFormatNumber	kXRLUIFirstMinorFormatNumber

#endif // __XRLUIID_h__

//  Code generated by DollyXs code generator
