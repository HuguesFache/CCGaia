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


#ifndef __XPGUIID_h__
#define __XPGUIID_h__

#include "SDKDef.h"
 
// Company:
#define kXPGUICompanyKey	"Trias Developpement  "		// Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
#define kXPGUICompanyValue	"Trias Developpement  " 	// Company name displayed externally.

// Plug-in:
#define kXPGUIPluginName	"XPageUI"					// Name of this plug-in.
#define kXPGUIPrefixNumber	0xe3f00 		            // Unique prefix number for this plug-in(*Must* be obtained from Adobe Developer Support).
// Plug-in version string. Pulled from the shared TriasVersion.h header so
// every Trias plug-in bumps in lock-step. Edit Gaia\TriasVersion.h to
// change the family-wide version.
#include "../../../Gaia/TriasVersion.h"
#define kXPGUIVersion		kTriasVersionString					// Version of this plug-in (for the About Box).
#define kXPGUIAuthor		"Trias"						// Author of this plug-in (for the About Box).
 
// Plug-in Prefix: (please change kXPGUIPrefixNumber above to modify the prefix.)
#define kXPGUIPrefix		RezLong(kXPGUIPrefixNumber)				// The unique numeric prefix for all object model IDs for this plug-in.
#define kXPGUIStringPrefix	SDK_DEF_STRINGIZE(kXPGUIPrefixNumber)	// The string equivalent of the unique prefix number for  this plug-in.

// Missing plug-in: (see ExtraPluginInfo resource)
#define kXPGUIMissingPluginURLValue		kSDKDefPartnersStandardValue_enUS // URL displayed in Missing Plug-in dialog
#define kXPGUIMissingPluginAlertValue	kSDKDefMissingPluginAlertValue // Message displayed in Missing Plug-in dialog - provide a string that instructs user how to solve their missing plug-in problem

// PluginID:
DECLARE_PMID(kPlugInIDSpace, kXPGUIPluginID,					kXPGUIPrefix + 0)

// ClassIDs:
DECLARE_PMID(kClassIDSpace, kXPGUIActionComponentBoss,			kXPGUIPrefix + 0)
DECLARE_PMID(kClassIDSpace, kXPGUIDropDownListWidgetBoss,		kXPGUIPrefix + 11)
DECLARE_PMID(kClassIDSpace, kXPGUIXmlFileHandlerBoss,			kXPGUIPrefix + 13)
DECLARE_PMID(kClassIDSpace, kXPGUIXmlDataInPageItemHelperBoss,  kXPGUIPrefix + 14)
DECLARE_PMID(kClassIDSpace, kXPGUIFormeInDocumentHelperBoss,	kXPGUIPrefix + 15)
DECLARE_PMID(kClassIDSpace, kXPGUIImagesPanelWidgetBoss,		kXPGUIPrefix + 16)
DECLARE_PMID(kClassIDSpace, kXPGUIImageNodeWidgetBoss,			kXPGUIPrefix + 17)
DECLARE_PMID(kClassIDSpace, kXPGUIImagesViewWidgetBoss,			kXPGUIPrefix + 18)
DECLARE_PMID(kClassIDSpace, kXPGUIImageInPageItemHelperBoss,	kXPGUIPrefix + 19)
// kXPGUIPrefix + 20 was kXPGUIImageDropTargetImpl (mis-declared in
// kClassIDSpace, dead code — removed with XPGUIImageDropTarget.cpp).
DECLARE_PMID(kClassIDSpace, kXPGUIImageFileHandlerBoss,			kXPGUIPrefix + 21)
DECLARE_PMID(kClassIDSpace, kXPGUIDisplayImagePanelWidgetBoss,	kXPGUIPrefix + 22)
DECLARE_PMID(kClassIDSpace, kXPGUIDisplayImagePanelViewImpl,	kXPGUIPrefix + 23)
DECLARE_PMID(kClassIDSpace, kXPGUIChooseMatchingDialogBoss,		kXPGUIPrefix + 25)
DECLARE_PMID(kClassIDSpace, kXPGUIFormesPanelWidgetBoss,		kXPGUIPrefix + 27)
DECLARE_PMID(kClassIDSpace, kXPGUIFormesViewWidgetBoss,			kXPGUIPrefix + 28)
DECLARE_PMID(kClassIDSpace, kXPGUIFormeNodeWidgetBoss,			kXPGUIPrefix + 29)
DECLARE_PMID(kClassIDSpace, kXPGUIFormeFileHandlerBoss,			kXPGUIPrefix + 30)
DECLARE_PMID(kClassIDSpace, kXPGUIOrderArtDialogBoss,			kXPGUIPrefix + 31)
DECLARE_PMID(kClassIDSpace, kXPGUIArticleWithFormeInDocumentHelperBoss, kXPGUIPrefix + 32) 
DECLARE_PMID(kClassIDSpace, kXPGUINewFormeDialogBoss,				kXPGUIPrefix + 33)
DECLARE_PMID(kClassIDSpace, kXPGUIFormeAdornmentBoss,				kXPGUIPrefix + 35)
DECLARE_PMID(kClassIDSpace, kXPGUIDnDDisplayImagePanelWidgetBoss,	kXPGUIPrefix + 36)
DECLARE_PMID(kClassIDSpace, kXPGUIDisplayImageFormePanelWidgetBoss, kXPGUIPrefix + 37)
DECLARE_PMID(kClassIDSpace, kXPGUIChangeStatusDialogBoss,			kXPGUIPrefix + 38)
DECLARE_PMID(kClassIDSpace, kXPGUIArticleAdornmentBoss,				kXPGUIPrefix + 43)
DECLARE_PMID(kClassIDSpace, kXPGUIArticleAttrReportBoss,			kXPGUIPrefix + 44)
DECLARE_PMID(kClassIDSpace, kXPGUIXRailImagesPanelWidgetBoss,		kXPGUIPrefix + 46)
DECLARE_PMID(kClassIDSpace, kXPGUIAddToArtDialogBoss,				kXPGUIPrefix + 48)
DECLARE_PMID(kClassIDSpace, kXPGUITexteExchangeHandlerBoss,			kXPGUIPrefix + 49)
DECLARE_PMID(kClassIDSpace, kXPGUIFilterLinksListBoxBoss,			kXPGUIPrefix + 51)
DECLARE_PMID(kClassIDSpace, kXPGUIUpdateFormeDialogBoss,			kXPGUIPrefix + 55)
DECLARE_PMID(kClassIDSpace, kXPGUIUpdateStyleMatchingWidgetBoss,	kXPGUIPrefix + 56)
DECLARE_PMID(kClassIDSpace, kXPGUIUpdateStyleMatchingNodeWidgetBoss,kXPGUIPrefix + 57)
DECLARE_PMID(kClassIDSpace, kXPGUILinkArtDialogBoss,				kXPGUIPrefix + 58)
DECLARE_PMID(kClassIDSpace, kXPGUIPlacedFormeHeaderAdornmentBoss,	kXPGUIPrefix + 60)
DECLARE_PMID(kClassIDSpace, kXPGUIPlacedFormeContentAdornmentBoss,	kXPGUIPrefix + 61)
DECLARE_PMID(kClassIDSpace, kXPGUIPlacedFormeFooterAdornmentBoss,	kXPGUIPrefix + 62)
DECLARE_PMID(kClassIDSpace, kXPGUIXmlWithFormeFileHandlerBoss,		kXPGUIPrefix + 63)
DECLARE_PMID(kClassIDSpace, kXPGUIAssemblagesPanelWidgetBoss,		kXPGUIPrefix + 69)
DECLARE_PMID(kClassIDSpace, kXPGUINewAssemblageDialogBoss,			kXPGUIPrefix + 70)
DECLARE_PMID(kClassIDSpace, kXPGUIAssemblagesViewWidgetBoss,		kXPGUIPrefix + 71)
DECLARE_PMID(kClassIDSpace, kXPGUIArticlePanelWidgetBoss,			kXPGUIPrefix + 75)
DECLARE_PMID(kClassIDSpace, kXPGUIArticleViewWidgetBoss,			kXPGUIPrefix + 76)
DECLARE_PMID(kClassIDSpace, kXPGUIArticleNodeWidgetBoss,			kXPGUIPrefix + 77)
DECLARE_PMID(kClassIDSpace, kXPGUINewClasseurDialogBoss,			kXPGUIPrefix + 78)
// Layout-level drop target helper for XPageImageFlavor — opens the
// "Choose photo carton" dialog when an image from the Photos liées palette
// is dropped on empty pasteboard. The on-frame helper for graphic frames
// stays in kXPGUIImageInPageItemHelperBoss.
DECLARE_PMID(kClassIDSpace, kXPGUIImageInLayoutHelperBoss,			kXPGUIPrefix + 79)
// Modal dialog presented by the empty-space drop flow above. Lets the user
// pick a photo carton (a forme tagged isPhotoCarton="1" in its descriptif
// XML) that will be placed at the drop point and have the dragged image
// loaded into each of its photo frames.
DECLARE_PMID(kClassIDSpace, kXPGUIChoosePhotoCartonDialogBoss,		kXPGUIPrefix + 80)
// Drag-enabled variant of the Photos liées image preview: same custom-draw
// view + ISysFileData, plus a drag source (kXPGUIXRailDisplayImageDragDropSourceImpl)
// and the DnD-aware event handler. Lets the user drag from the preview, not
// just from the list rows.
DECLARE_PMID(kClassIDSpace, kXPGUIXRailDnDDisplayImagePanelWidgetBoss,	kXPGUIPrefix + 81)
// Modal dialog that lets the user pick a new EtatImage for a photo of the
// Photos liées palette (opened by clicking the row's colour swatch).
DECLARE_PMID(kClassIDSpace, kXPGUIChooseEtatImageDialogBoss,		kXPGUIPrefix + 82)
// Custom boss for the Photos liées row colour swatch. Inherits the standard
// UIColorSwatchWidget boss and adds an event handler so a click on the
// pastille opens the "change état" dialog. NB: the parent boss constant
// kUIColorSwatchWidgetBoss comes from the SDK widget headers — if it differs
// in this SDK build, adjust the inherit-from id in XPGUI_InDesign.fr.
DECLARE_PMID(kClassIDSpace, kXPGUIImageEtatSwatchWidgetBoss,		kXPGUIPrefix + 83)

// InterfaceIDs:
DECLARE_PMID(kInterfaceIDSpace, IID_ITEXTFRAMELIST,					kXPGUIPrefix + 0)
DECLARE_PMID(kInterfaceIDSpace, IID_IIDDATA,						kXPGUIPrefix + 1)
DECLARE_PMID(kInterfaceIDSpace, IID_ISHADOWEVENTHANDLER,			kXPGUIPrefix + 2)
DECLARE_PMID(kInterfaceIDSpace, IID_IIDLIST,						kXPGUIPrefix + 3)
DECLARE_PMID(kInterfaceIDSpace, IID_IACTIONIDTOLEVEL1,				kXPGUIPrefix + 4)
DECLARE_PMID(kInterfaceIDSpace, IID_IACTIONIDTOLEVEL2,				kXPGUIPrefix + 5)
DECLARE_PMID(kInterfaceIDSpace, IID_IACTIONIDTONUMEROS,				kXPGUIPrefix + 6)
DECLARE_PMID(kInterfaceIDSpace, IID_IDATAMODEL,						kXPGUIPrefix + 7)
DECLARE_PMID(kInterfaceIDSpace, IID_IRUBRIQUEIDDATA,				kXPGUIPrefix + 8)
DECLARE_PMID(kInterfaceIDSpace, IID_ICLASSEURNAMEDATA,				kXPGUIPrefix + 9)
DECLARE_PMID(kInterfaceIDSpace, IID_ISTORYDATA,						kXPGUIPrefix + 10)
DECLARE_PMID(kInterfaceIDSpace, IID_IPHOTOLIST,						kXPGUIPrefix + 11)
DECLARE_PMID(kInterfaceIDSpace, IID_ICREDITLIST,					kXPGUIPrefix + 12)
DECLARE_PMID(kInterfaceIDSpace, IID_ILEGENDLIST,					kXPGUIPrefix + 13)
DECLARE_PMID(kInterfaceIDSpace, IID_INEWFORMEOBSERVER,				kXPGUIPrefix + 14)
DECLARE_PMID(kInterfaceIDSpace, IID_ICREDITPHOTO,					kXPGUIPrefix + 15)
DECLARE_PMID(kInterfaceIDSpace, IID_ILEGENDPHOTO,					kXPGUIPrefix + 16)
// Carries the article id alongside the dragged image — lets the drop
// target detect a drop onto a frame that doesn't belong to the dragged
// photo's article and propagate the article identifiers to it.
DECLARE_PMID(kInterfaceIDSpace, IID_IPHOTOARTICLEID,				kXPGUIPrefix + 223)
// Carries the crop rectangle (packed "cropX|cropY|cropW|cropH", BR pixels)
// alongside the dragged image so the drop target can apply it after placement.
DECLARE_PMID(kInterfaceIDSpace, IID_IPHOTOCROPDATA,					kXPGUIPrefix + 225)
DECLARE_PMID(kInterfaceIDSpace, IID_IFORMEITEMLIST,					kXPGUIPrefix + 17)
DECLARE_PMID(kInterfaceIDSpace, IID_IFORMESTYLESLIST,				kXPGUIPrefix + 22)
DECLARE_PMID(kInterfaceIDSpace, IID_IMATCHINGSTYLESLIST,			kXPGUIPrefix + 23)
DECLARE_PMID(kInterfaceIDSpace, IID_IFORMEMATCHINGSTYLESLIST,		kXPGUIPrefix + 24)
DECLARE_PMID(kInterfaceIDSpace, IID_IFORMENAMEDATA,		            kXPGUIPrefix + 25)
DECLARE_PMID(kInterfaceIDSpace, IID_IMATCHINGSTYLESWITHTAGLIST,		kXPGUIPrefix + 26)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLEIDDATA,					kXPGUIPrefix + 27)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLESNIPPETFILEDATA,		kXPGUIPrefix + 28)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLETOPICDATA,				kXPGUIPrefix + 29)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLEIDSTATUS,				kXPGUIPrefix + 30)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLELIBELLESTATUS,			kXPGUIPrefix + 31)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLECOULEURSTATUS,			kXPGUIPrefix + 32)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLETYPEDATA,    			kXPGUIPrefix + 34)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLEXMLFILEDATA,    		kXPGUIPrefix + 35)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLERUBDATA,    			kXPGUIPrefix + 36)
DECLARE_PMID(kInterfaceIDSpace, IID_IARTICLESUBRUBDATA,    			kXPGUIPrefix + 37)
DECLARE_PMID(kInterfaceIDSpace, IID_IFIXEDCONTENTFRAMELIST,         kXPGUIPrefix + 39)
// In/out int carried on kXPGUIChooseEtatImageDialogBoss: the EH sets it to the
// photo's current etatImage id before opening the dialog (for preselection);
// the dialog controller overwrites it with the chosen etat id on OK.
DECLARE_PMID(kInterfaceIDSpace, IID_IXPGETATCHOICE,                 kXPGUIPrefix + 40)

// ImplementationIDs:
DECLARE_PMID(kImplementationIDSpace, kXPGUIActionComponentImpl,					kXPGUIPrefix + 0)
DECLARE_PMID(kImplementationIDSpace, kXPGUITextesPanelViewImpl,					kXPGUIPrefix + 4)
DECLARE_PMID(kImplementationIDSpace, kXPGUIImagesViewObserverImpl,				kXPGUIPrefix + 8)
DECLARE_PMID(kImplementationIDSpace, kActionIDToStringDataImpl,					kXPGUIPrefix + 12)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXmlFileHandlerImpl,					kXPGUIPrefix + 16)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormeFileHandlerImpl,				kXPGUIPrefix + 18)
// kXPGUIPrefix + 19 was kXPGUIImageDragDropSourceImpl (dead code — removed with XPGUIImageDragDropSource.cpp).
DECLARE_PMID(kImplementationIDSpace, kXPGUIImagesTVHierarchyAdapterImpl,		kXPGUIPrefix + 21)
// kXPGUIPrefix + 22 was kXPGUIImagesPanelObserverImpl (dead code — removed with XPGUIIImagesPanelObserver.cpp).
DECLARE_PMID(kImplementationIDSpace, kXPGUIImagesTVWidgetMgrImpl,				kXPGUIPrefix + 24)
DECLARE_PMID(kImplementationIDSpace, kXPGUIImageFileHandlerImpl,				kXPGUIPrefix + 25)
DECLARE_PMID(kImplementationIDSpace, kXPGUIRubriqueIdDataImpl,					kXPGUIPrefix + 28)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormesTVWidgetMgrImpl,				kXPGUIPrefix + 31)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormesTVHierarchyAdapterImpl,		kXPGUIPrefix + 32)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormeDragDropSourceImpl,				kXPGUIPrefix + 33)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormeDropTargetImpl,					kXPGUIPrefix + 34)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormesPanelObserverImpl,				kXPGUIPrefix + 35)
DECLARE_PMID(kImplementationIDSpace, kXPGUIOrderArtDialogControllerImpl,		kXPGUIPrefix + 36)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXRailArticleWithFormeDropTargetImpl,	kXPGUIPrefix + 37)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXRailXmlDropTargetImpl,				kXPGUIPrefix + 38)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXRailNewFormeDialogControllerImpl,	kXPGUIPrefix + 41)
DECLARE_PMID(kImplementationIDSpace, kXPGUIClasseurNameDataImpl,				kXPGUIPrefix + 42)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXRailTexteDragDropSourceImpl,		kXPGUIPrefix + 44)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXRailImageDragDropSourceImpl,		kXPGUIPrefix + 45)
DECLARE_PMID(kImplementationIDSpace, kXPGUIOrderArtDialogObserverImpl,			kXPGUIPrefix + 46)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXRailImagesPanelObserverImpl,		kXPGUIPrefix + 47)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXRailNewFormeDialogObserverImpl,		kXPGUIPrefix + 50)
DECLARE_PMID(kImplementationIDSpace, kXPGUITextFrameListImpl,					kXPGUIPrefix + 51)
DECLARE_PMID(kImplementationIDSpace, kXPGUIPhotoListImpl,						kXPGUIPrefix + 52)
DECLARE_PMID(kImplementationIDSpace, kXPGUICreditListImpl,						kXPGUIPrefix + 53)
DECLARE_PMID(kImplementationIDSpace, kXPGUILegendListImpl,						kXPGUIPrefix + 54)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXRailImageDropTargetImpl,			kXPGUIPrefix + 55)
DECLARE_PMID(kImplementationIDSpace, kXPGUICreditPhotoImpl,						kXPGUIPrefix + 56)
DECLARE_PMID(kImplementationIDSpace, kXPGUILegendPhotoImpl,						kXPGUIPrefix + 57)
DECLARE_PMID(kImplementationIDSpace, kXPGUIPhotoArticleIdImpl,					kXPGUIPrefix + 224)
DECLARE_PMID(kImplementationIDSpace, kXPGUIPhotoCropDataImpl,					kXPGUIPrefix + 226)
DECLARE_PMID(kImplementationIDSpace, kXPGUIImageNodeButtonsObserverImpl,		kXPGUIPrefix + 227)
DECLARE_PMID(kImplementationIDSpace, kXPGUIImageInLayoutDropTargetImpl,			kXPGUIPrefix + 228)
DECLARE_PMID(kImplementationIDSpace, kXPGUIChoosePhotoCartonDialogControllerImpl, kXPGUIPrefix + 229)
DECLARE_PMID(kImplementationIDSpace, kXPGUIChoosePhotoCartonDialogObserverImpl,	kXPGUIPrefix + 230)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormeAdornmentShapeImpl,				kXPGUIPrefix + 58)
DECLARE_PMID(kImplementationIDSpace, kXPGUIInCopyImageDragDropSourceImpl,		kXPGUIPrefix + 59)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormeItemListImpl,					kXPGUIPrefix + 60)
DECLARE_PMID(kImplementationIDSpace, kXPGUIDisplayImageFormePanelViewImpl,		kXPGUIPrefix + 61)
DECLARE_PMID(kImplementationIDSpace, kXPGUIImagesFormesViewObserverImpl,		kXPGUIPrefix + 62)
DECLARE_PMID(kImplementationIDSpace, kXPGUIChangeStatusDialogObserverImpl,		kXPGUIPrefix + 64)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXRailInCopyImagesPanelObserverImpl,	kXPGUIPrefix + 65)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleAttrReportImpl,				kXPGUIPrefix + 70)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleAdornmentImpl,				kXPGUIPrefix + 71)
DECLARE_PMID(kImplementationIDSpace, kXPGUIAddToArtDialogControllerImpl,		kXPGUIPrefix + 78)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormeStylesListImpl,					kXPGUIPrefix + 80)
DECLARE_PMID(kImplementationIDSpace, kXPGUIMatchingStylesListImpl,				kXPGUIPrefix + 85)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormeMatchingStylesListImpl,			kXPGUIPrefix + 86)
DECLARE_PMID(kImplementationIDSpace, kXPGUIUpdateFormeDialogControllerImpl,		kXPGUIPrefix + 87)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFormeNameDataImpl,					kXPGUIPrefix + 88)
DECLARE_PMID(kImplementationIDSpace, kXPGUIUpdateStyleMatchingWidgetMgrImpl,	kXPGUIPrefix + 91)
DECLARE_PMID(kImplementationIDSpace, kXPGUIUpdateStyleMatchingHierarchyAdapterImpl,	kXPGUIPrefix + 92)
DECLARE_PMID(kImplementationIDSpace, kXPGUIMatchingStylesWithTagListImpl,		kXPGUIPrefix + 93)
DECLARE_PMID(kImplementationIDSpace, kXPGUILinkArtDialogControllerImpl,			kXPGUIPrefix + 94)
DECLARE_PMID(kImplementationIDSpace, kXPGUILinkArtDialogObserverImpl,			kXPGUIPrefix + 95)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleIdImpl,						kXPGUIPrefix + 96)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleSnippetDataImpl,				kXPGUIPrefix + 97)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleTopicDataImpl,				kXPGUIPrefix + 98)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleIdStatusDataImpl,			    kXPGUIPrefix + 99)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleLibelleStatusDataImpl,		kXPGUIPrefix + 100)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleCouleurStatusDataImpl,		kXPGUIPrefix + 101)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleTypeDataImpl,		        	kXPGUIPrefix + 103)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleXMLDataImpl,		        	kXPGUIPrefix + 104)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleRubDataImpl,		        	kXPGUIPrefix + 106)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleSubRubDataImpl,				kXPGUIPrefix + 107)
DECLARE_PMID(kImplementationIDSpace, kXPGUITECImagesPanelObserverImpl,			kXPGUIPrefix + 108)
DECLARE_PMID(kImplementationIDSpace, kXPGUITECImageDropTargetImpl,				kXPGUIPrefix + 109)
DECLARE_PMID(kImplementationIDSpace, kXPGUITECImageDragDropSourceImpl,			kXPGUIPrefix + 110)
DECLARE_PMID(kImplementationIDSpace, kXPGUIPlacedFormeHeaderAdornmentImpl,		kXPGUIPrefix + 111)
DECLARE_PMID(kImplementationIDSpace, kXPGUIPlacedFormeContentAdornmentImpl,		kXPGUIPrefix + 112)
DECLARE_PMID(kImplementationIDSpace, kXPGUIPlacedFormeFooterAdornmentImpl,		kXPGUIPrefix + 113)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXmlWithFormeFileHandlerImpl,			kXPGUIPrefix + 114)
DECLARE_PMID(kImplementationIDSpace, kXPGUIAddToArtDialogObserverImpl,			kXPGUIPrefix + 115)
DECLARE_PMID(kImplementationIDSpace, kXPGUIAssemblagesTVHierarchyAdapterImpl,	kXPGUIPrefix + 117)
DECLARE_PMID(kImplementationIDSpace, kXPGUIStartupShutdownImpl,					kXPGUIPrefix + 119)
DECLARE_PMID(kImplementationIDSpace, kXPGUIAssemblagesPanelObserverImpl,		kXPGUIPrefix + 124)
DECLARE_PMID(kImplementationIDSpace, kXPGUINewAssemblageDialogControllerImpl,	kXPGUIPrefix + 125)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleTVWidgetMgrImpl,				kXPGUIPrefix + 130)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleTVHierarchyAdapterImpl,		kXPGUIPrefix + 131)
DECLARE_PMID(kImplementationIDSpace, kXPGUIXArticlePanelObserverImpl,			kXPGUIPrefix + 142)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticleViewObserverImpl,				kXPGUIPrefix + 143)
DECLARE_PMID(kImplementationIDSpace, kXPGUIArticlePanelObserverImpl,		    kXPGUIPrefix + 144)
DECLARE_PMID(kImplementationIDSpace, kXPGUIFixedContentFrameListImpl,            kXPGUIPrefix + 145)
DECLARE_PMID(kImplementationIDSpace, kXPGUINewClasseurDialogControllerImpl,	kXPGUIPrefix + 146)
// Drag source aggregated on the Photos liées image preview — reads the
// row selected in kXPGUIImagesViewWidgetID and builds the same drag payload
// as the per-row source (kXPGUIXRailImageDragDropSourceImpl).
DECLARE_PMID(kImplementationIDSpace, kXPGUIXRailDisplayImageDragDropSourceImpl,	kXPGUIPrefix + 147)
// "Change état image" feature (Photos liées palette).
DECLARE_PMID(kImplementationIDSpace, kXPGUIChooseEtatImageDialogControllerImpl,	kXPGUIPrefix + 148)
DECLARE_PMID(kImplementationIDSpace, kXPGUIImageEtatSwatchEHImpl,				kXPGUIPrefix + 149)
// Alias → kIntDataImpl (declared in ImplementationAlias). Carries IID_IXPGETATCHOICE.
DECLARE_PMID(kImplementationIDSpace, kXPGUIEtatChoiceDataImpl,					kXPGUIPrefix + 150)

// ActionIDs:
DECLARE_PMID(kActionIDSpace, kXPGUITextesPanelWidgetActionID,					kXPGUIPrefix + 5)
DECLARE_PMID(kActionIDSpace, kXPGUISeparatorActionID,							kXPGUIPrefix + 12)
DECLARE_PMID(kActionIDSpace, kXPGUIImagesPanelWidgetActionID,					kXPGUIPrefix + 13)
DECLARE_PMID(kActionIDSpace, kXPGUIFormesPanelWidgetActionID,					kXPGUIPrefix + 15)
DECLARE_PMID(kActionIDSpace, kXPGUISeparator2ActionID,							kXPGUIPrefix + 17)
DECLARE_PMID(kActionIDSpace, kXPGUINewFormeActionID,							kXPGUIPrefix + 18)
DECLARE_PMID(kActionIDSpace, kXPGUIDeleteFormeActionID,							kXPGUIPrefix + 19)
DECLARE_PMID(kActionIDSpace, kXPGUINewClasseurActionID,							kXPGUIPrefix + 20)
DECLARE_PMID(kActionIDSpace, kXPGUIDeleteClasseurActionID,						kXPGUIPrefix + 21)
DECLARE_PMID(kActionIDSpace, kXPGUIArticlePanelWidgetActionID,					kXPGUIPrefix + 22)
DECLARE_PMID(kActionIDSpace, kXPGUISectionLabelWidgetID,						kXPGUIPrefix + 23)
DECLARE_PMID(kActionIDSpace, kXPGUIssSectionLabelWidgetID,						kXPGUIPrefix + 24)
DECLARE_PMID(kActionIDSpace, kXPGUIFirstLevel1ActionID,							kXPGUIPrefix + 25)
DECLARE_PMID(kActionIDSpace, kXPGUIFirstLevel2ActionID,							kXPGUIPrefix + 26)
DECLARE_PMID(kActionIDSpace, kXPGUIFirstNumeroActionID,							kXPGUIPrefix + 27)
DECLARE_PMID(kActionIDSpace, kXPGUISeparator3ActionID,							kXPGUIPrefix + 30)
DECLARE_PMID(kActionIDSpace, kXPGUIArticlesPanelWidgetActionID,					kXPGUIPrefix + 34)
DECLARE_PMID(kActionIDSpace, kXPGUIAssemblagesPanelActionID,					kXPGUIPrefix + 35)
DECLARE_PMID(kActionIDSpace, kXPGUINewAssemblageActionID,						kXPGUIPrefix + 36)
DECLARE_PMID(kActionIDSpace, kXPGUINewClasseurAssemblageActionID,				kXPGUIPrefix + 37)
DECLARE_PMID(kActionIDSpace, kXPGUIDeleteAssemblageActionID,					kXPGUIPrefix + 38)
DECLARE_PMID(kActionIDSpace, kXPGUIDeleteClasseurAssemblageActionID,			kXPGUIPrefix + 39)
DECLARE_PMID(kActionIDSpace, kXPGUIRefreshAssemblageActionID,					kXPGUIPrefix + 40)
DECLARE_PMID(kActionIDSpace, kXPGUISeparator4ActionID,							kXPGUIPrefix + 41)
DECLARE_PMID(kActionIDSpace, kXPGUISeparator5ActionID,							kXPGUIPrefix + 42)

// WidgetIDs:
DECLARE_PMID(kWidgetIDSpace, kXPGUITabDialogWidgetID,							kXPGUIPrefix + 0)
DECLARE_PMID(kWidgetIDSpace, kXPGUITextesPanelWidgetID,							kXPGUIPrefix + 1)
DECLARE_PMID(kWidgetIDSpace, kXPGUIArticleViewWidgetID,							kXPGUIPrefix + 4)
DECLARE_PMID(kWidgetIDSpace, kXPGUIArticleNodeWidgetId,							kXPGUIPrefix + 5)
DECLARE_PMID(kWidgetIDSpace, kXPGUITextSubjectWidgetID,							kXPGUIPrefix + 19)
DECLARE_PMID(kWidgetIDSpace, kXPGUICurrentNumeroWidgetID,						kXPGUIPrefix + 20)
DECLARE_PMID(kWidgetIDSpace, kXPGUINewNameWidgetID,								kXPGUIPrefix + 22)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRubriqueNodeWidgetId,						kXPGUIPrefix + 27)
DECLARE_PMID(kWidgetIDSpace, kXPGUITextStateWidgetID,							kXPGUIPrefix + 28)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRubriqueNameWidgetID,						kXPGUIPrefix + 29)
DECLARE_PMID(kWidgetIDSpace, kXPGUIImagesPanelWidgetID,							kXPGUIPrefix + 30)
DECLARE_PMID(kWidgetIDSpace, kXPGUIImagesViewWidgetID,							kXPGUIPrefix + 31)
DECLARE_PMID(kWidgetIDSpace, kXPGUIImageNodeWidgetId,							kXPGUIPrefix + 32)
DECLARE_PMID(kWidgetIDSpace, kXPGUIImageNameWidgetID,							kXPGUIPrefix + 33)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDisplayImagePanelWidgetID,					kXPGUIPrefix + 35)
DECLARE_PMID(kWidgetIDSpace, kXPGUIChooseMatchingDialogWidgetID,				kXPGUIPrefix + 36)
DECLARE_PMID(kWidgetIDSpace, kXPGUIFilterImageWidgetID,							kXPGUIPrefix + 38)
DECLARE_PMID(kWidgetIDSpace, kXPGUIFormesPanelWidgetID,							kXPGUIPrefix + 39)
DECLARE_PMID(kWidgetIDSpace, kXPGUIFormesViewWidgetID,							kXPGUIPrefix + 40)
DECLARE_PMID(kWidgetIDSpace, kXPGUIFormeNodeWidgetId,							kXPGUIPrefix + 41)
DECLARE_PMID(kWidgetIDSpace, kXPGUIFormeNameWidgetID,							kXPGUIPrefix + 42)
DECLARE_PMID(kWidgetIDSpace, kXPGUIOrderArtDialogWidgetID,						kXPGUIPrefix + 44)
DECLARE_PMID(kWidgetIDSpace, kXPGUITopicWidgetID,								kXPGUIPrefix + 45)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRecipientListWidgetID,						kXPGUIPrefix + 46)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRubriqueListWidgetID,						kXPGUIPrefix + 47)
DECLARE_PMID(kWidgetIDSpace, kXPGUISsRubriqueListWidgetID,						kXPGUIPrefix + 48)
DECLARE_PMID(kWidgetIDSpace, kXPGUINewFormeWidgetID,							kXPGUIPrefix + 49)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDeleteFormeWidgetID,							kXPGUIPrefix + 50)
DECLARE_PMID(kWidgetIDSpace, kXPGUIClasseurListWidgetID,						kXPGUIPrefix + 51)
DECLARE_PMID(kWidgetIDSpace, kXPGUINewFormeDialogWidgetID,						kXPGUIPrefix + 52)
DECLARE_PMID(kWidgetIDSpace, kXPGUIArticlePanelWidgetID,						kXPGUIPrefix + 53)
DECLARE_PMID(kWidgetIDSpace, kXPGUIFilterTypeWidgetID,							kXPGUIPrefix + 54)
DECLARE_PMID(kWidgetIDSpace, kXPGUIToggleFilterWidgetID,						kXPGUIPrefix + 55)
DECLARE_PMID(kWidgetIDSpace, kXPGUIFilterPanelWidgetID,							kXPGUIPrefix + 56)
DECLARE_PMID(kWidgetIDSpace, kXPGUIFirstFilterWidgetID,							kXPGUIPrefix + 57)
DECLARE_PMID(kWidgetIDSpace, kXPGUISecondFilterWidgetID,						kXPGUIPrefix + 58)
DECLARE_PMID(kWidgetIDSpace, kXPGUIThirdFilterWidgetID,							kXPGUIPrefix + 59)
DECLARE_PMID(kWidgetIDSpace, kXPGUIPlacedFilterClusterWidgetID,					kXPGUIPrefix + 60)
DECLARE_PMID(kWidgetIDSpace, kXPGUIMAJArtButtonWidgetID,						kXPGUIPrefix + 64)
DECLARE_PMID(kWidgetIDSpace, kXPGUIArticlePreviewWidgetID,						kXPGUIPrefix + 65)
DECLARE_PMID(kWidgetIDSpace, kXPGUICartonListWidgetID,							kXPGUIPrefix + 68)
DECLARE_PMID(kWidgetIDSpace, kXPGUICreditPhotoWidgetID,							kXPGUIPrefix + 71)
DECLARE_PMID(kWidgetIDSpace, kXPGUILegendPhotoWidgetID,							kXPGUIPrefix + 74)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDefineCreditPhotoWidgetID,					kXPGUIPrefix + 75)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDefineLegendPhotoWidgetID,					kXPGUIPrefix + 76)
DECLARE_PMID(kWidgetIDSpace, kXPGUIStatusCreditPhotoWidgetID,					kXPGUIPrefix + 77)
DECLARE_PMID(kWidgetIDSpace, kXPGUIStatusLegendPhotoWidgetID,					kXPGUIPrefix + 78) 
DECLARE_PMID(kWidgetIDSpace, kXPGUIStatusArticleWidgetID,						kXPGUIPrefix + 80)
DECLARE_PMID(kWidgetIDSpace, kXPGUIPhotoListWidgetID,							kXPGUIPrefix + 81)
DECLARE_PMID(kWidgetIDSpace, kXPGUIComboClasseurListWidgetID,					kXPGUIPrefix + 82)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDisplayImageFormePanelWidgetID,				kXPGUIPrefix + 85)
DECLARE_PMID(kWidgetIDSpace, kXPGUIChangeStatusDialogWidgetID,					kXPGUIPrefix + 86) 
DECLARE_PMID(kWidgetIDSpace, kXPGUIArticlesListWidgetID,						kXPGUIPrefix + 88) 
DECLARE_PMID(kWidgetIDSpace, kXPGUIRecipientStatusListWidgetID,					kXPGUIPrefix + 89) 
DECLARE_PMID(kWidgetIDSpace, kXPGUIStatsListWidgetID,							kXPGUIPrefix + 90) 
DECLARE_PMID(kWidgetIDSpace, kXPGUIXRailImagesPanelWidgetID,					kXPGUIPrefix + 92)
DECLARE_PMID(kWidgetIDSpace, kXPGUICreditTextWidgetID,							kXPGUIPrefix + 97) 
DECLARE_PMID(kWidgetIDSpace, kXPGUILegendeTextWidgetID,							kXPGUIPrefix + 98) 
DECLARE_PMID(kWidgetIDSpace, kXPGUICreditLabelWidgetID,							kXPGUIPrefix + 99)
DECLARE_PMID(kWidgetIDSpace, kXPGUILegendeLabelWidgetID,						kXPGUIPrefix + 100)
DECLARE_PMID(kWidgetIDSpace, kXPGUICreateIncopyWidgetID,						kXPGUIPrefix + 101)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDeleteAssignmentWidgetID,					kXPGUIPrefix + 102)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDisplayStoryStatesWidgetID,					kXPGUIPrefix + 103)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRefreshAdornmentWidgetID,					kXPGUIPrefix + 104)
DECLARE_PMID(kWidgetIDSpace, kXPGUINbImagesStaticTextWidgetID,					kXPGUIPrefix + 105)
DECLARE_PMID(kWidgetIDSpace, kXPGUINbImagesTextWidgetId,						kXPGUIPrefix + 106)
DECLARE_PMID(kWidgetIDSpace, kXPGUINbImagesNudgeWidgetId,						kXPGUIPrefix + 107)
DECLARE_PMID(kWidgetIDSpace, kXPGUINewTopicWidgetID,							kXPGUIPrefix + 112)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRecepientsListWidgetID,						kXPGUIPrefix + 113)
DECLARE_PMID(kWidgetIDSpace, kXPGUIGroupsListWidgetID,							kXPGUIPrefix + 114)
DECLARE_PMID(kWidgetIDSpace, kXPGUIChooseRecepientWidgetID,						kXPGUIPrefix + 117)
DECLARE_PMID(kWidgetIDSpace, kXPGUIGroupsStatusTextWidgetID,					kXPGUIPrefix + 118)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRecipientStatusTextWidgetID,					kXPGUIPrefix + 119)
DECLARE_PMID(kWidgetIDSpace, kXPGUIGroupsStatusListWidgetID,					kXPGUIPrefix + 120)
DECLARE_PMID(kWidgetIDSpace, kXPGUIStatusTextWidgetID,							kXPGUIPrefix + 121)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDisplayIndexesWidgetID,						kXPGUIPrefix + 131)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDispalyIndexWidgetID,						kXPGUIPrefix + 132)
DECLARE_PMID(kWidgetIDSpace, kXPGUIAddToAssignmentButtonWidgetID,				kXPGUIPrefix + 133)
DECLARE_PMID(kWidgetIDSpace, kXPGUIAddToArtDialogWidget,						kXPGUIPrefix + 135)
DECLARE_PMID(kWidgetIDSpace, kXPGUIAssignmentListWidgetID,						kXPGUIPrefix + 136)
DECLARE_PMID(kWidgetIDSpace, kXPGUISectionListWidgetID,							kXPGUIPrefix + 137)
DECLARE_PMID(kWidgetIDSpace, kXPGUISsSectionListWidgetID,						kXPGUIPrefix + 138)
DECLARE_PMID(kWidgetIDSpace, kXPGUITextStatusWidgetID,						    kXPGUIPrefix + 147)
DECLARE_PMID(kWidgetIDSpace, kXPGUIParutionListWidgetID,						kXPGUIPrefix + 148)
DECLARE_PMID(kWidgetIDSpace, kXPGUITextPictureIconWidgetID,						kXPGUIPrefix + 149)
DECLARE_PMID(kWidgetIDSpace, kXPGUITextPictureCountWidgetID,					kXPGUIPrefix + 150)
DECLARE_PMID(kWidgetIDSpace, kXPGUIUpdateStyleMatchingViewWidgetID,				kXPGUIPrefix + 151)
DECLARE_PMID(kWidgetIDSpace, kXPGUIUpdateStyleMatchingWidgetID,					kXPGUIPrefix + 152)
DECLARE_PMID(kWidgetIDSpace, kXPGUIUpdateStyleNameWidgetID,					    kXPGUIPrefix + 153)
DECLARE_PMID(kWidgetIDSpace, kXPGUIUpdateStyleTagListWidgetID,					kXPGUIPrefix + 154)
DECLARE_PMID(kWidgetIDSpace, kXPGUILinkArtDialogWidgetID,					    kXPGUIPrefix + 156)
DECLARE_PMID(kWidgetIDSpace, kXPGUIChangeFormeStatesWidgetID,					kXPGUIPrefix + 157)
DECLARE_PMID(kWidgetIDSpace, kXPGUIChangeFormeBorderWidgetID,					kXPGUIPrefix + 158)
DECLARE_PMID(kWidgetIDSpace, kXPGUIArticleNbSignesWidgetID,						kXPGUIPrefix + 162)
DECLARE_PMID(kWidgetIDSpace, kXPGUIUpdateFormeDialogWidgetID,					kXPGUIPrefix + 163)
DECLARE_PMID(kWidgetIDSpace, kXPGUICalibrageStaticTextWidgetID,					kXPGUIPrefix + 164)
DECLARE_PMID(kWidgetIDSpace, kXPGUICalibrageTextWidgetId,						kXPGUIPrefix + 165)
DECLARE_PMID(kWidgetIDSpace, kXPGUICurrentUserWidgetID,							kXPGUIPrefix + 166)
DECLARE_PMID(kWidgetIDSpace, kXPGUIInfoPageWidgetID,							kXPGUIPrefix + 167)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDisplayFormesWidgetID,						kXPGUIPrefix + 168)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRefreshFormesAdornmentWidgetID,				kXPGUIPrefix + 169)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRadioInCopyStoryWidgetID,					kXPGUIPrefix + 170)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRadioPlacedStoryWidgetID,					kXPGUIPrefix + 171)
DECLARE_PMID(kWidgetIDSpace, kXPGUIComputeCalibrageWidgetID,					kXPGUIPrefix + 177)
DECLARE_PMID(kWidgetIDSpace, kXPGUIArticlesPanelWidgetID,						kXPGUIPrefix + 178)
DECLARE_PMID(kWidgetIDSpace, kXPGUINewAssemblageWidgetID,						kXPGUIPrefix + 179)
DECLARE_PMID(kWidgetIDSpace, kXPGUIDeleteAssemblageWidgetID,					kXPGUIPrefix + 180)
DECLARE_PMID(kWidgetIDSpace, kXPGUIUpdateAssemblageWidgetID,					kXPGUIPrefix + 181)
DECLARE_PMID(kWidgetIDSpace, kXPGUIAssemblagesViewWidgetID,						kXPGUIPrefix + 182)
DECLARE_PMID(kWidgetIDSpace, kXPGUIAssemblagesPanelWidgetID,					kXPGUIPrefix + 183)
DECLARE_PMID(kWidgetIDSpace, kXPGUINewAssemblageDialogWidgetID,					kXPGUIPrefix + 184)
DECLARE_PMID(kWidgetIDSpace, kXPGUIAssemblageNameWidgetID,						kXPGUIPrefix + 185)
DECLARE_PMID(kWidgetIDSpace, kXPGUIPlaceAutoFormeWidgetID,						kXPGUIPrefix + 187)
DECLARE_PMID(kWidgetIDSpace, kXPGUIPlaceAutoAssemblageWidgetID,					kXPGUIPrefix + 188)
DECLARE_PMID(kWidgetIDSpace, kXPGUIGoToStoryCheckBoxWidgetID,					kXPGUIPrefix + 203)
DECLARE_PMID(kWidgetIDSpace, kXPGUIGotoStoryButtonWidgetID,					    kXPGUIPrefix + 204)
DECLARE_PMID(kWidgetIDSpace, kXPGUIChangeStylesWidgetID,                        kXPGUIPrefix + 206)
DECLARE_PMID(kWidgetIDSpace, kXPGUIFixedContentWidgetID,                        kXPGUIPrefix + 207)
DECLARE_PMID(kWidgetIDSpace, kXPGUIImageIndexWidgetID, kXPGUIPrefix + 208)
DECLARE_PMID(kWidgetIDSpace, kXPGUIImageChangeIndexWidgetID, kXPGUIPrefix + 209)
DECLARE_PMID(kWidgetIDSpace, kXPGUIImageChangeTypeWidgetID, kXPGUIPrefix + 210)
DECLARE_PMID(kWidgetIDSpace, kXPGUIImageChangeIndexBtnWidgetID, kXPGUIPrefix + 211)
DECLARE_PMID(kWidgetIDSpace, kXPGUINewClasseurButtonWidgetID,	kXPGUIPrefix + 212)
DECLARE_PMID(kWidgetIDSpace, kXPGUINewClasseurDialogWidgetID,	kXPGUIPrefix + 213)
DECLARE_PMID(kWidgetIDSpace, kXPGUINewClasseurNameWidgetID,		kXPGUIPrefix + 214)
DECLARE_PMID(kWidgetIDSpace, kXPGUILoginPromptButtonWidgetID,	kXPGUIPrefix + 215)
DECLARE_PMID(kWidgetIDSpace, kXPGUILoginPromptTextWidgetID,		kXPGUIPrefix + 216)
// Second-level overlay shown when the user is logged in but the active page
// has no XRail/Gaia ID. Static text only (no button) — the user just has to
// open or focus a Gaia-linked page. Used by both palettes.
DECLARE_PMID(kWidgetIDSpace, kXPGUINoGaiaPageTextWidgetID,		kXPGUIPrefix + 218)
// "New classeur" button on the Assemblages palette (mirrors the same button
// on the Formes/XDA palette). Opens XPGUINewClasseurDialog with the
// classeurAssemblage flag set.
DECLARE_PMID(kWidgetIDSpace, kXPGUINewClasseurAssemblageButtonWidgetID, kXPGUIPrefix + 217)
// Read-only "Commentaire" field on the Photos liées palette (under the
// Légende / Crédit zone). Sourced from the OBJRART.xml <Commentaire>
// node. Label widget + text widget pair.
DECLARE_PMID(kWidgetIDSpace, kXPGUICommentLabelWidgetID,		kXPGUIPrefix + 219)
DECLARE_PMID(kWidgetIDSpace, kXPGUICommentTextWidgetID,			kXPGUIPrefix + 220)
// Action icons appended to each row of the Photos liées list:
//   * "Goto link"      → select & scroll to the photo's frame in the layout.
//   * "Edit original"  → open the high-res image with the OS default app.
DECLARE_PMID(kWidgetIDSpace, kXPGUIImageGotoLinkBtnWidgetID,	kXPGUIPrefix + 225)
DECLARE_PMID(kWidgetIDSpace, kXPGUIImageEditOriginalBtnWidgetID, kXPGUIPrefix + 226)
// Color swatch shown in the leftmost column of each Photos liées row.
// Filled at runtime with the EtatImage colour matching the row's etatimage
// attribute (parsed from OBJRART.xml), looked up against the etatsimages
// list cached in IXPGPreferences.
DECLARE_PMID(kWidgetIDSpace, kXPGUIImageEtatSwatchWidgetID,		kXPGUIPrefix + 228)
// Green check icon shown when the row's image is placed in a frame of the
// front document. Hidden otherwise.
DECLARE_PMID(kWidgetIDSpace, kXPGUIImagePlacedIconWidgetID,		kXPGUIPrefix + 229)
// "Carton photo" checkbox in the New Forme dialog. Tags the carton being
// created as a photo-carton (a carton whose role is to add images to other
// cartons later). Persisted as the `isPhotoCarton` attribute on the FORME
// element of the descriptif XML.
DECLARE_PMID(kWidgetIDSpace, kXPGUICartonPhotoWidgetID,			kXPGUIPrefix + 230)
// "Choose photo carton" dialog widgets (root + classeur dropdown +
// carton dropdown). Shown when an image from the Photos liées palette is
// dropped on empty pasteboard.
DECLARE_PMID(kWidgetIDSpace, kXPGUIChoosePhotoCartonDialogWidgetID,	kXPGUIPrefix + 231)
DECLARE_PMID(kWidgetIDSpace, kXPGUIPhotoCartonClasseurListWidgetID,	kXPGUIPrefix + 232)
DECLARE_PMID(kWidgetIDSpace, kXPGUIPhotoCartonListWidgetID,			kXPGUIPrefix + 233)
// Multi-line static text in the New Forme dialog that surfaces the
// pre-flight style-validation report (XPageUIUtils::ValidateSelectionForCarton).
// Hidden on success — InitializeDialogFields then shrinks the dialog back to
// its compact height so the empty area doesn't waste space.
DECLARE_PMID(kWidgetIDSpace, kXPGUIValidationMessageWidgetID,		kXPGUIPrefix + 234)
// Action icons shown next to the Crédit / Légende edit boxes in the
// Photos liées panel. "Copy" puts the field's text on the OS clipboard;
// "Send" pushes it into the matching kCredit / kLegend frame on the
// page (located by the selected image's id + photoIndex).
DECLARE_PMID(kWidgetIDSpace, kXPGUICopyCreditBtnWidgetID,			kXPGUIPrefix + 235)
DECLARE_PMID(kWidgetIDSpace, kXPGUISendCreditBtnWidgetID,			kXPGUIPrefix + 236)
DECLARE_PMID(kWidgetIDSpace, kXPGUICopyLegendBtnWidgetID,			kXPGUIPrefix + 237)
DECLARE_PMID(kWidgetIDSpace, kXPGUISendLegendBtnWidgetID,			kXPGUIPrefix + 238)
// "Voir toutes les photos" toggle in the Photos liées panel. When checked,
// HandleSelectionChanged stops filtering on the selected frame's article
// and instead loads images from every OBJRART referenced by an assignment
// in the front document. Refresh button next to it forces a re-scan
// without toggling the state (useful after server-side updates).
DECLARE_PMID(kWidgetIDSpace, kXPGUIShowAllImagesCheckboxWidgetID,	kXPGUIPrefix + 239)
DECLARE_PMID(kWidgetIDSpace, kXPGUIRefreshImagesBtnWidgetID,		kXPGUIPrefix + 240)
// Reorder buttons next to the photo dropdown in the New Forme dialog.
// (DEPRECATED — replaced by kXPGUIPhotoPositionListWidgetID. Declarations
// kept so existing offsets don't shift.)
DECLARE_PMID(kWidgetIDSpace, kXPGUIPhotoUpButtonWidgetID,		kXPGUIPrefix + 241)
DECLARE_PMID(kWidgetIDSpace, kXPGUIPhotoDownButtonWidgetID,		kXPGUIPrefix + 242)
// Position dropdown in the New Forme dialog : shows the current slot of
// the photo selected in kXPGUIPhotoListWidgetID. Picking a different slot
// triggers a move-with-shift (insert semantic) on photoList / creditList
// / legendList, then rewrites IFormeData::photoIndex on every block
// impacted so the adornment numbering tracks the dropdown.
DECLARE_PMID(kWidgetIDSpace, kXPGUIPhotoPositionListWidgetID,	kXPGUIPrefix + 243)
// kXPGUIPrefix + 244 (ex-kXPGUIRenumberUnitButtonWidgetID) — bouton Renum
// manuel supprimé de la palette XDA. La renumérotation reste auto-déclenchée
// par XPGDocObserver via Utils<IXPageUtils>()->RenumberUnit.
// "Choose état image" dialog : root widget + état dropdown.
DECLARE_PMID(kWidgetIDSpace, kXPGUIChooseEtatImageDialogWidgetID,	kXPGUIPrefix + 245)
DECLARE_PMID(kWidgetIDSpace, kXPGUIChooseEtatImageListWidgetID,		kXPGUIPrefix + 246)

// Resource IDs of the InDesign Links panel's per-row "Atteindre le lien"
// and "Modifier l'original" icons. The values come from LinksUIResDefs.h
// (in the SDK's open/components/linksui sources, which isn't on our ODFRC
// include path) — we duplicate the constants here so the .fr can reference
// them without pulling that header in. Used with kLinksUIPluginID (declared
// in LinksUIID.h, which is on the include path).
#define kIDLinksUIGotoLinkButtonIconResID		160
#define kIDLinksUIEditOriginalButtonIconResID	180
// Same provenance as the two above — value of kLinksUIUpdateLinkButtonIconRsrcID
// (140) from LinksUIResDefs.h. Used as the "refresh / re-scan all OBJRARTs"
// icon next to the "Voir toutes les photos" checkbox in the Photos liées panel.
#define kIDLinksUIUpdateLinkButtonIconRsrcID	140

// Message IDs
DECLARE_PMID(kMessageIDSpace, kXPGUIRefreshPageListMsg,							kXPGUIPrefix + 0)
DECLARE_PMID(kMessageIDSpace, kXPGUICloseFormeDialogMsg,						kXPGUIPrefix + 1)
// kXRLUILoginChangedMsg now lives in XRail's XRLUIID.h — the login event
// is owned by XRail since it controls the login dialog. XPage observers
// subscribe to it via #include "XRLUIID.h".

// "Plug-ins" sub-menu:
#define kXPGUIPluginMenuKey						kXPGUIStringPrefix		"kXPGUIPluginMenuKey"
#define kXPGUIPluginMenuPath					"Main:&Window"

#define kXPGUIInternalPopupMenuNameKey			kXPGUIStringPrefix		"kXPGUIInternalPopupMenuNameKey"
#define kXPGUIFormePopupMenuNameKey				kXPGUIStringPrefix		"kXPGUIFormePopupMenuNameKey"
#define kXPGUIFormePopupMenuPath				kXPGUIFormePopupMenuNameKey

#define kXPGUIAssemblagePopupMenuNameKey		kXPGUIStringPrefix		"kXPGUIAssemblagePopupMenuNameKey"
#define kXPGUIAssemblagePopupMenuPath			kXPGUIAssemblagePopupMenuNameKey

// Menu item keys:

// Other StringKeys:
#define kXPGUITextesPanelTitleKey				kXPGUIStringPrefix		"kXPGUITextesPanelTitleKey"
#define kXPGUINoneTextKey						kXPGUIStringPrefix		"kXPGUINoneTextKey"
#define kXPGUINoneUserTextKey					kXPGUIStringPrefix		"kXPGUINoneUserTextKey"
#define kXPGUINewNameKey						kXPGUIStringPrefix		"kXPGUINewNameKey"
#define kXPGUIRefreshTextKey					kXPGUIStringPrefix		"kXPGUIRefreshTextKey"
#define kXPGUIImagesPanelTitleKey				kXPGUIStringPrefix		"kXPGUIImagesPanelTitleKey"
#define kXPGUIChooseMatchingDialogTitleKey		kXPGUIStringPrefix		"kXPGUIChooseMatchingDialogTitleKey"
#define kXPGUIFormesPanelTitleKey				kXPGUIStringPrefix		"kXPGUIFormesPanelTitleKey"
#define kXPGUINumeroTextKey						kXPGUIStringPrefix		"kXPGUINumeroTextKey"
#define kXPGUITextesPanelMenuKey				kXPGUIStringPrefix		"kXPGUITextesPanelMenuKey"
#define kXPGUIImagesPanelMenuKey				kXPGUIStringPrefix		"kXPGUIImagesPanelMenuKey"
#define kXPGUIOrderArtDialogTitleKey			kXPGUIStringPrefix		"kXPGUIOrderArtDialogTitleKey"
#define kXPGUITopicTextKey						kXPGUIStringPrefix		"kXPGUITopicTextKey"
#define kXPGUIRecipientTextKey					kXPGUIStringPrefix		"kXPGUIRecipientTextKey"
#define kXPGUICreateurTextKey					kXPGUIStringPrefix		"kXPGUICreateurTextKey"
#define kXPGUIRubriqueTextKey					kXPGUIStringPrefix		"kXPGUIRubriqueTextKey"
#define kXPGUISsRubriqueTextKey					kXPGUIStringPrefix		"kXPGUISsRubriqueTextKey"
#define kXPGUIClasseurTextKey					kXPGUIStringPrefix		"kXPGUIClasseurTextKey"
#define kXPGUINewFormeDialogTitleKey			kXPGUIStringPrefix		"kXPGUINewFormeDialogTitleKey"
#define kXPGUIFormeNameKey						kXPGUIStringPrefix		"kXPGUIFormeNameKey"
#define kXPGUIReplaceFormeKey					kXPGUIStringPrefix		"kXPGUIReplaceFormeKey"
#define kXPGUIFailedToCreateFormeKey			kXPGUIStringPrefix		"kXPGUIFailedToCreateFormeKey"
#define kErrJVImpossible						kXPGUIStringPrefix		"kErrJVImpossible"
#define kResizeCmdFailed						kXPGUIStringPrefix		"kResizeCmdFailed"
#define kXPGUIFilterImageKey					kXPGUIStringPrefix		"kXPGUIFilterImageKey"
#define kXPGUIOrderInCopyArtTextKey				kXPGUIStringPrefix		"kXPGUIOrderInCopyArtTextKey"
#define kXPGUINewFormeTextKey					kXPGUIStringPrefix		"kXPGUINewFormeTextKey"
#define kXPGUIDeleteFormeTextKey				kXPGUIStringPrefix		"kXPGUIDeleteFormeTextKey"
#define kXPGUINewClasseurTextKey				kXPGUIStringPrefix		"kXPGUINewClasseurTextKey"
#define kXPGUIDeleteClasseurTextKey				kXPGUIStringPrefix		"kXPGUIDeleteClasseurTextKey"
#define kXPGUIConfirmDeleteFormeTextKey			kXPGUIStringPrefix		"kXPGUIConfirmDeleteFormeTextKey"
#define kXPGUIConfirmDeleteClasseurKey			kXPGUIStringPrefix		"kXPGUIConfirmDeleteClasseurKey"
#define kXPGUIArticlePanelTitleKey				kXPGUIStringPrefix		"kXPGUIArticlePanelTitleKey"
#define kXPGUIArticlePanelMenuKey				kXPGUIStringPrefix		"kXPGUIArticlePanelMenuKey"
#define kXPGUIRubriqueModeKey					kXPGUIStringPrefix		"kXPGUIRubriqueModeKey"
#define kXPGUILocaliteModeKey					kXPGUIStringPrefix		"kXPGUILocaliteModeKey"
#define kXPGUIMAJArtButtonKey					kXPGUIStringPrefix		"kXPGUIMAJArtButtonKey"
#define kXPGUIAllArticlesTextKey				kXPGUIStringPrefix		"kXPGUIAllArticlesTextKey"
#define kXPGUINotPlacedArticlesTextKey			kXPGUIStringPrefix		"kXPGUINotPlacedArticlesTextKey"
#define kXPGUIPlacedArticlesTextKey				kXPGUIStringPrefix		"kXPGUIPlacedArticlesTextKey"
#define kXPGUINbCharsTextKey					kXPGUIStringPrefix		"kXPGUINbCharsTextKey"
#define kXPGUICartonTextKey						kXPGUIStringPrefix		"kXPGUICartonTextKey"
#define kXPGUIArticleTextKey					kXPGUIStringPrefix		"kXPGUIArticleTextKey"
#define kXPGUIPhotoTextKey						kXPGUIStringPrefix		"kXPGUIPhotoTextKey"
#define kXPGUICreditKey							kXPGUIStringPrefix		"kXPGUICreditKey"
#define kXPGUILegendKey							kXPGUIStringPrefix		"kXPGUILegendKey"
#define kXPGUICommentLabelKey					kXPGUIStringPrefix		"kXPGUICommentLabelKey"
#define kXPGUIShowAllImagesKey					kXPGUIStringPrefix		"kXPGUIShowAllImagesKey"
#define kXPGUIDefineKey							kXPGUIStringPrefix		"kXPGUIDefineKey"
#define kXPGUIPhotoUpKey						kXPGUIStringPrefix		"kXPGUIPhotoUpKey"
#define kXPGUIPhotoDownKey						kXPGUIStringPrefix		"kXPGUIPhotoDownKey"
#define kXPGUIPhotoPositionTextKey				kXPGUIStringPrefix		"kXPGUIPhotoPositionTextKey"
#define kXPGUINoTextFrameSelectedKey			kXPGUIStringPrefix		"kXPGUINoTextFrameSelectedKey"
#define kXPGUIPlacementAutoKey					kXPGUIStringPrefix		"kXPGUIPlacementAutoKey"
#define kXPGUISelectionDontBelongToFormeKey		kXPGUIStringPrefix		"kXPGUISelectionDontBelongToFormeKey"
#define kXPGUITextFrameAlreadyDefinedKey		kXPGUIStringPrefix		"kXPGUITextFrameAlreadyDefinedKey"
#define kXPGUIInvalidLinkedFrameSelectionKey	kXPGUIStringPrefix		"kXPGUIInvalidLinkedFrameSelectionKey"
#define kXPGUITextFrameAlreadyDefined2Key		kXPGUIStringPrefix		"kXPGUITextFrameAlreadyDefined2Key"
#define kXPGUIArticleTypeKey					kXPGUIStringPrefix		"kXPGUIArticleTypeKey"
#define kXPGUIPhotoTypeKey						kXPGUIStringPrefix		"kXPGUIPhotoTypeKey"
#define kXPGUIInvalidFormeNameKey				kXPGUIStringPrefix		"kXPGUIInvalidFormeNameKey"
#define kXPGUIFormeClosedKey					kXPGUIStringPrefix		"kXPGUIFormeClosedKey"
#define kXPGUISelectionAlreadyAssignedKey		kXPGUIStringPrefix		"kXPGUISelectionAlreadyAssignedKey"
#define kXPGUISelectionAlreadyPlacedKey         kXPGUIStringPrefix		"kXPGUISelectionAlreadyPlacedKey" 
#define kXPGUIFormesPanelDATitleKey				kXPGUIStringPrefix		"kXPGUIFormesPanelDATitleKey"
#define kXPGUIChangeStatusDialogTitleKey		kXPGUIStringPrefix		"kXPGUIChangeStatusDialogTitleKey"
#define kXPGUIArticleTitleKey					kXPGUIStringPrefix		"kXPGUIArticleTitleKey"
#define kXPGUIStatusListTextKey					kXPGUIStringPrefix		"kXPGUIStatusListTextKey"
#define kXPGUIAddArticleIncopyKey				kXPGUIStringPrefix		"kXPGUIAddArticleIncopyKey"
#define kXPGUIDeleteAffectationKey				kXPGUIStringPrefix		"kXPGUIDeleteAffectationKey"
#define kXPGUIUnlinkStoryKey					kXPGUIStringPrefix		"kXPGUIUnlinkStoryKey"
#define kXPGUIRetourChariotTextKey				kXPGUIStringPrefix		"kXPGUIRetourChariotTextKey"
#define kXPGUIDisplayStoryStatesKey				kXPGUIStringPrefix		"kXPGUIDisplayStoryStatesKey"
#define kXPGUICreateIncopyTipKey				kXPGUIStringPrefix		"kXPGUICreateIncopyTipKey"
#define kXPGUIDeleteAssignmentTipKey			kXPGUIStringPrefix		"kXPGUIDeleteAssignmentTipKey"
#define kXPGUIMAJArtButtonTipKey				kXPGUIStringPrefix		"kXPGUIMAJArtButtonTipKey"
#define kXPGUINbImagesKey						kXPGUIStringPrefix		"kXPGUINbImagesKey"
#define kXPGUIEditAssignmentTipKey				kXPGUIStringPrefix		"kXPGUIEditAssignmentTipKey"
#define kXPGUIGroupsTextKey						kXPGUIStringPrefix		"kXPGUIGroupsTextKey"
#define kXPGUIStoryStatusListTextKey			kXPGUIStringPrefix		"kXPGUIStoryStatusListTextKey"
#define kXPGUIChooseRecepientTextKey			kXPGUIStringPrefix		"kXPGUIChooseRecepientTextKey"
#define kXPGUIGroupsListTextKey					kXPGUIStringPrefix		"kXPGUIGroupsListTextKey"
#define kXPGUIRecipientListTextKey				kXPGUIStringPrefix		"kXLGRecipientListTextKey"
#define kXPGUIEveryBodyTextKey					kXPGUIStringPrefix		"kXPGUIEveryBodyTextKey " 
#define kXPGUIEditArtDialogTitleKey	    	    kXPGUIStringPrefix		"kXPGUIEditArtDialogTitleKey "
#define kXPGUIUpdateLabelKey					kXPGUIStringPrefix		"kXPGUIUpdateLabelKey"
#define kXPGUIDeleteLabelTextKey				kXPGUIStringPrefix		"kXPGUIDeleteLabelTextKey"
#define kXPGUIAddLabelKey					    kXPGUIStringPrefix		"kXPGUIAddLabelKey"
#define kXPGUIAddToArtDialogTitleKey			kXPGUIStringPrefix		"kXPGUIAddToArtDialogTitleKey"
#define kXPGUIListAssignmentsTextKey			kXPGUIStringPrefix		"kXPGUIListAssignmentsTextKey" 
#define kXPGUIAddToAssignmentKey				kXPGUIStringPrefix		"kXPGUIAddToAssignmentKey" 
#define kXPGUIAddToAssignmentButtonTipKey		kXPGUIStringPrefix		"kXPGUIAddToAssignmentButtonTipKey"
#define kXPGUIDefineMatchingErrorKey    		kXPGUIStringPrefix		"kXPGUIDefineMatchingErrorKey"
#define kXPGUIUpdateFormeDialogTitleKey 	   	kXPGUIStringPrefix		"kXPGUIUpdateFormeDialogTitleKey"
#define kXPGUIRelinkArtDialogTitleKey			kXPGUIStringPrefix		"kXPGUIRelinkArtDialogTitleKey"
#define kXPGUICannotDeleteAssignmentKey			kXPGUIStringPrefix		"kXPGUICannotDeleteAssignmentKey"
#define kXPGUIChangeFormeStatesKey			    kXPGUIStringPrefix		"kXPGUIChangeFormeStatesKey"
#define kXPGUIDisplayFormesKey					kXPGUIStringPrefix		"kXPGUIDisplayFormesKey"
#define kXPGUINewClasseurDialogTitleKey			kXPGUIStringPrefix		"kXPGUINewClasseurDialogTitleKey"
#define kXPGUINewClasseurPromptKey				kXPGUIStringPrefix		"kXPGUINewClasseurPromptKey"
#define kXPGUIEmptyClasseurNameKey				kXPGUIStringPrefix		"kXPGUIEmptyClasseurNameKey"
#define kXPGUIClasseurAlreadyExistsKey			kXPGUIStringPrefix		"kXPGUIClasseurAlreadyExistsKey"
#define kXPGUINewClasseurButtonTipKey			kXPGUIStringPrefix		"kXPGUINewClasseurButtonTipKey"
#define kXPGUINewClasseurAssemblageButtonTipKey	kXPGUIStringPrefix		"kXPGUINewClasseurAssemblageButtonTipKey"
#define kXPGUILoginPromptButtonKey				kXPGUIStringPrefix		"kXPGUILoginPromptButtonKey"
#define kXPGUILoginPromptTextKey				kXPGUIStringPrefix		"kXPGUILoginPromptTextKey"
#define kXPGUINoGaiaPageTextKey					kXPGUIStringPrefix		"kXPGUINoGaiaPageTextKey"
// "Créer carton" pre-flight validation alert (XPageUIUtils::ValidateSelectionForCarton).
#define kXPGUICartonValidationHeaderKey			kXPGUIStringPrefix		"kXPGUICartonValidationHeaderKey"
#define kXPGUICartonValidationOverridesKey		kXPGUIStringPrefix		"kXPGUICartonValidationOverridesKey"
#define kXPGUICartonValidationCrossStoriesKey	kXPGUIStringPrefix		"kXPGUICartonValidationCrossStoriesKey"
#define kXPGUICartonValidationNoStyleKey		kXPGUIStringPrefix		"kXPGUICartonValidationNoStyleKey"
#define kXPGUIStoryLinkErrorKey					kXPGUIStringPrefix		"kXPGUIStoryLinkErrorKey"
#define kXPGUICalibrageKey						kXPGUIStringPrefix		"kXPGUICalibrageKey"
#define kXPGUINbSignesLabelKey					kXPGUIStringPrefix		"kXPGUINbSignesLabelKey"
#define kXPGUINbSignesTitleKey					kXPGUIStringPrefix		"kXPGUINbSignesTitleKey"
#define kXPGUIStoryTypeTextKey					kXPGUIStringPrefix		"kXPGUIStoryTypeTextKey"
#define kXPGUIPlacedStoryTypeKey				kXPGUIStringPrefix		"kXPGUIPlacedStoryTypeKey"
#define kXPGUIInCopyStoryTypeKey				kXPGUIStringPrefix		"kXPGUIInCopyStoryTypeKey"
#define kXPGUITextFolioKey						kXPGUIStringPrefix		"kXPGUITextFolioKey"
#define	kXPGUITextLinkSameFolioErrorKey			kXPGUIStringPrefix 	    "kXPGUITextLinkSameFolioErrorKey"	
#define	kXPGUITextLinkDestErrorKey				kXPGUIStringPrefix 	    "kXPGUITextLinkDestErrorKey"	
#define	kXPGUIArticlesPanelTitleKey				kXPGUIStringPrefix 	    "kXPGUIArticlesPanelTitleKey"
#define	kXPGUIArticlesPanelMenuKey				kXPGUIStringPrefix 	    "kXPGUIArticlesPanelMenuKey"
#define	kXPGUIAssemblagesPanelTitleKey			kXPGUIStringPrefix 	    "kXPGUIAssemblagesPanelTitleKey"
#define kXPGUINewAssemblageTextKey				kXPGUIStringPrefix 	    "kXPGUINewAssemblageTextKey"
#define kXPGUIDeleteAssemblageTextKey			kXPGUIStringPrefix 	    "kXPGUIDeleteAssemblageTextKey"
#define kXPGUINewAssemblageDialogTitleKey		kXPGUIStringPrefix 	    "kXPGUINewAssemblageDialogTitleKey"
#define kXPGUIInvalidAssemblageNameKey			kXPGUIStringPrefix 	    "kXPGUIInvalidAssemblageNameKey"
#define kXPGUIReplaceAssemblageKey				kXPGUIStringPrefix 	    "kXPGUIReplaceAssemblageKey"
#define kXPGUIFailedToCreateAssemblageKey		kXPGUIStringPrefix 	    "kXPGUIFailedToCreateAssemblageKey"
#define kXPGUIAssemblageNameKey					kXPGUIStringPrefix 	    "kXPGUIAssemblageNameKey"
#define kXPGUIInvalidAssemblySelectionKey		kXPGUIStringPrefix 	    "kXPGUIInvalidAssemblySelectionKey"
#define kXPGUIGoToStoryKey						kXPGUIStringPrefix		"kXPGUIGoToStoryKey"
#define kXPGUIChangeStylesResaKey				kXPGUIStringPrefix		"kXPGUIChangeStylesResaKey"
#define kXPGUIkFixedContentKey                  kXPGUIStringPrefix      "kXPGUIkFixedContentKey"
#define kXPGUICartonPhotoKey                    kXPGUIStringPrefix      "kXPGUICartonPhotoKey"
#define kXPGUIChoosePhotoCartonDialogTitleKey   kXPGUIStringPrefix      "kXPGUIChoosePhotoCartonDialogTitleKey"
#define kXPGUIPhotoCartonClasseurKey            kXPGUIStringPrefix      "kXPGUIPhotoCartonClasseurKey"
#define kXPGUIPhotoCartonKey                    kXPGUIStringPrefix      "kXPGUIPhotoCartonKey"
#define kXPGUINoPhotoCartonAvailableKey         kXPGUIStringPrefix      "kXPGUINoPhotoCartonAvailableKey"
#define kXPGUIChooseEtatImageDialogTitleKey     kXPGUIStringPrefix      "kXPGUIChooseEtatImageDialogTitleKey"
#define kXPGUIEtatImageLabelKey                 kXPGUIStringPrefix      "kXPGUIEtatImageLabelKey"
#define kXPGUIImagesIndexKey                  kXPGUIStringPrefix      "kXPGUIImagesIndexKey"
#define kXPGUIkValiderKey                  kXPGUIStringPrefix      "kXPGUIkValiderKey"

// Menu item positions:
#define kXPGUILevel1MenuItemPosition    		1.0
#define kXPGUILevel2MenuItemPosition    		2.0
#define kXPGUINumerosMenuItemPosition   		3.0
#define kXPGUIFirsDynMenuItemPosition   		1.0  

#define	kXPGUIRefreshMenuItemPosition		    11.0

#define kXPGUINewFormeMenuItemPosition          1.0
#define kXPGUIDeleteFormeMenuItemPosition       2.0
#define kXPGUIUpdateFormeMenuItemPosition	    3.0

#define	kXPGUISeparator1MenuItemPosition		10.0
#define kXPGUINewClasseurMenuItemPosition       11.0
#define kXPGUIDeleteClasseurMenuItemPosition    12.0
#define	kXPGUISeparator2MenuItemPosition	    13.0
#define	kXPGUISeparator3MenuItemPosition	    16.0

#define kXPGUINewAssemblageMenuItemPosition     1.0
#define kXPGUIDeleteAssemblageMenuItemPosition  2.0
#define	kXPGUISeparator4MenuItemPosition				  10.0
#define kXPGUINewClasseurAssemblageMenuItemPosition       11.0
#define kXPGUIDeleteClasseurAssemblageMenuItemPosition    12.0
#define	kXPGUISeparator5MenuItemPosition				  13.0
#define	kXPGUIRefreshAssemblageMenuItemPosition			  14.0

#define kXPGUIArticleNodeWidgetRsrcID			730
#define kXPGUIRubriqueNodeWidgetRsrcID			731
#define kXPGUITextesPanelResourceID				740
#define kXPGUIImagesPanelResourceID				750
#define kXPGUIImageNodeWidgetRsrcID				751
#define kXPGUIStatusListElementRsrcID			770
#define kXPGUITickedCheckBoxIconResourceID		782
#define kXPGUIUntickedCheckBoxIconResourceID	783
#define kXPGUIStatutIconResourceID		        784
#define kXPGUIChooseMatchingDialogResourceID	786
#define kXPGUINewFormeDialogResourceID	        787
#define kXPGUIArticlePanelResourceID			788
#define kXPGUIToggleIconResourceID		        789
#define kXPGUIFormesPanelResourceID 		    791
#define kXPGUIFormeNodeWidgetRsrcID			    792
#define kXPGUINextIconResourceID			    793
#define kXPGUIPreviousIconResourceID		    794
#define kXPGUIOKIconResourceID			        795
#define kXPGUIKOIconResourceID		            796
#define kXPGUIXRailImagesPanelResourceID        797
#define kXPGUITextNewIconResourceID		        800
#define kXPGUITextUpdateIconResourceID		    801
#define kXPGUITextDeleteIconResourceID		    802
#define kXPGUITextAddToAssignmentIconResourceID 807
#define kXPGUIUpdateFormeDialogResourceID		810
#define kXPGUIPictureIconResourceID			    811
#define kXPGUIUpdateStyleMatchingRsrcID         812
#define kXPGUILinkArtDialogResourceID			814
#define kXPGUIOrderArtDialogResourceID          821
#define kXPGUIChangeStatusDialogResourceID      822
#define kXPGUIEditStorySubjectDialogResourceID  823
#define kXPGUIAddToStorySubjectDialogResourceID 824
#define kXPGUIComputeResourceID					827
#define kXPGUIArticlesPanelResourceID			828
#define kXPGUIAssemblagesPanelResourceID		829
#define kXPGUINewAssemblageDialogResourceID		830
#define kXPGUINewClasseurDialogResourceID		850
#define kXPGUIAppliquerIconResourceID			833
#define kXPGUIGotoStoryIconResourceID			834
// Green check icon (placed.png) shown on Photos liées rows when the image
// is already placed in a frame of the front document.
#define kXPGUIPlacedImageIconResourceID			835
// "Choose photo carton" dialog resource (presented when dropping an image
// on empty pasteboard).
#define kXPGUIChoosePhotoCartonDialogResourceID	836
// "Choose état image" dialog (clic sur la pastille d'une ligne Photos liées).
#define kXPGUIChooseEtatImageDialogResourceID	851


// Custom flavor 
#define XPageTextOnlyFlavor						PMFlavor('XPG1')
#define XPageXMLWithFormeFlavor					PMFlavor('XPG2')
#define XPageImageFlavor						PMFlavor('XPG3')
#define XPageFormeFlavor						PMFlavor('XPG4')

// Initial data format version numbers
#define kXPGUIFirstMajorFormatNumber			RezLong(1)
#define kXPGUIFirstMinorFormatNumber			RezLong(0)

// Data format version numbers for the PluginVersion resource 
#define kXPGUICurrentMajorFormatNumber			kXPGUIFirstMajorFormatNumber
#define kXPGUICurrentMinorFormatNumber			kXPGUIFirstMinorFormatNumber

#endif 

// __XPGUIID_h__
//  Code generated by DollyXs code generator



