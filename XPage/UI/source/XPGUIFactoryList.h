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
REGISTER_PMINTERFACE(XPGUIActionComponent,						kXPGUIActionComponentImpl)
REGISTER_PMINTERFACE(XPGUITextesPanelView,						kXPGUITextesPanelViewImpl)
REGISTER_PMINTERFACE(XPGUITextViewObserver,						kXPGUIImagesViewObserverImpl)
REGISTER_PMINTERFACE(XPGUIImagesFormesViewObserver,				kXPGUIImagesFormesViewObserverImpl)
REGISTER_PMINTERFACE(ActionIDToStringDataImpl,					kActionIDToStringDataImpl)
REGISTER_PMINTERFACE(XPGUIXmlFileHandler,              			kXPGUIXmlFileHandlerImpl)
// Removed (dead code): legacy non-XRail Photos palette stack — its boss
// `kXPGUIImagesPanelWidgetBoss` is no longer in the InDesign PanelList.
// The XRail variants (kXPGUIXRailImagesPanelObserverImpl etc.) drive the
// active palette. InCopy still uses the boss but with different impls.
//   REGISTER_PMINTERFACE(XPGUIImageDragDropSource, kXPGUIImageDragDropSourceImpl)
//   REGISTER_PMINTERFACE(XPGUIImagesPanelObserver, kXPGUIImagesPanelObserverImpl)
//   REGISTER_PMINTERFACE(XPGUIImageDropTarget,     kXPGUIImageDropTargetImpl)
REGISTER_PMINTERFACE(XPGUIImagesTVWidgetMgr,           			kXPGUIImagesTVWidgetMgrImpl)
REGISTER_PMINTERFACE(XPGUIImagesTVHierarchyAdapter,	   			kXPGUIImagesTVHierarchyAdapterImpl)
REGISTER_PMINTERFACE(XPGUIImageFileHandler,			   			kXPGUIImageFileHandlerImpl)
REGISTER_PMINTERFACE(XPGUIDisplayImagePanelView,				kXPGUIDisplayImagePanelViewImpl)
REGISTER_PMINTERFACE(XPGUIDisplayImageFormePanelView,			kXPGUIDisplayImageFormePanelViewImpl)
REGISTER_PMINTERFACE(XPGUIFormesPanelObserver,					kXPGUIFormesPanelObserverImpl)
REGISTER_PMINTERFACE(XPGUIFormesTVHierarchyAdapter,				kXPGUIFormesTVHierarchyAdapterImpl)
REGISTER_PMINTERFACE(XPGUIFormesTVWidgetMgr,					kXPGUIFormesTVWidgetMgrImpl)
REGISTER_PMINTERFACE(XPGUIFormeDragDropSource,					kXPGUIFormeDragDropSourceImpl)
REGISTER_PMINTERFACE(XPGUIFormeFileHandler,						kXPGUIFormeFileHandlerImpl)
REGISTER_PMINTERFACE(XPGUIFormeDropTarget,						kXPGUIFormeDropTargetImpl)
REGISTER_PMINTERFACE(XPGUIOrderArtDialogController, 		    kXPGUIOrderArtDialogControllerImpl)
REGISTER_PMINTERFACE(XPGUIArticleWithFormeDropTarget,			kXPGUIXRailArticleWithFormeDropTargetImpl)
REGISTER_PMINTERFACE(XPGUIXRailXmlDropTarget,					kXPGUIXRailXmlDropTargetImpl)
REGISTER_PMINTERFACE(XPGUIXRailNewFormeDialogController,		kXPGUIXRailNewFormeDialogControllerImpl)
REGISTER_PMINTERFACE(XPGUIXRailTexteDragDropSource,				kXPGUIXRailTexteDragDropSourceImpl)
REGISTER_PMINTERFACE(XPGUIXRailImageDragDropSource,				kXPGUIXRailImageDragDropSourceImpl)
REGISTER_PMINTERFACE(XPGUIXRailDisplayImageDragDropSource,		kXPGUIXRailDisplayImageDragDropSourceImpl)
REGISTER_PMINTERFACE(XPGUIOrderArtDialogObserver,				kXPGUIOrderArtDialogObserverImpl)
REGISTER_PMINTERFACE(XPGUIXRailImagesPanelObserver,				kXPGUIXRailImagesPanelObserverImpl)
REGISTER_PMINTERFACE(XPGUIXRailNewFormeDialogObserver,			kXPGUIXRailNewFormeDialogObserverImpl)
REGISTER_PMINTERFACE(XPGUIXRailImageDropTarget,					kXPGUIXRailImageDropTargetImpl)
REGISTER_PMINTERFACE(XPGUIImageInLayoutDropTarget,				kXPGUIImageInLayoutDropTargetImpl)
REGISTER_PMINTERFACE(XPGUIChoosePhotoCartonDialogController,	kXPGUIChoosePhotoCartonDialogControllerImpl)
REGISTER_PMINTERFACE(XPGUIChoosePhotoCartonDialogObserver,		kXPGUIChoosePhotoCartonDialogObserverImpl)
REGISTER_PMINTERFACE(XPGUIFormeAdornmentShape,					kXPGUIFormeAdornmentShapeImpl)
REGISTER_PMINTERFACE(XPGUIInCopyImageDragDropSource,			kXPGUIInCopyImageDragDropSourceImpl)
REGISTER_PMINTERFACE(XPGUIXRailInCopyImagesPanelObserver,		kXPGUIXRailInCopyImagesPanelObserverImpl)
REGISTER_PMINTERFACE(XPGUIArticleAdornment,						kXPGUIArticleAdornmentImpl)
REGISTER_PMINTERFACE(XPGUIAddToArtDialogController,				kXPGUIAddToArtDialogControllerImpl)
REGISTER_PMINTERFACE(XPGUIMatchingStylesList,					kXPGUIMatchingStylesListImpl)
REGISTER_PMINTERFACE(XPGUIUpdateFormeDialogController,			kXPGUIUpdateFormeDialogControllerImpl)
REGISTER_PMINTERFACE(XPGUIUpdateStyleMatchingHierarchyAdapter,  kXPGUIUpdateStyleMatchingHierarchyAdapterImpl)
REGISTER_PMINTERFACE(XPGUIUpdateStyleMatchingWidgetMgr,			kXPGUIUpdateStyleMatchingWidgetMgrImpl)
REGISTER_PMINTERFACE(XPGUILinkArtDialogObserver,				kXPGUILinkArtDialogObserverImpl)
REGISTER_PMINTERFACE(XPGUILinkArtDialogController,				kXPGUILinkArtDialogControllerImpl)
REGISTER_PMINTERFACE(XPGUITECImagesPanelObserver,				kXPGUITECImagesPanelObserverImpl)
REGISTER_PMINTERFACE(XPGUITECImageDropTarget,					kXPGUITECImageDropTargetImpl)
REGISTER_PMINTERFACE(XPGUITECImageDragDropSource,				kXPGUITECImageDragDropSourceImpl)
REGISTER_PMINTERFACE(XPGUIPlacedFormeHeaderAdornment,			kXPGUIPlacedFormeHeaderAdornmentImpl)
REGISTER_PMINTERFACE(XPGUIPlacedFormeContentAdornment,			kXPGUIPlacedFormeContentAdornmentImpl)
REGISTER_PMINTERFACE(XPGUIPlacedFormeFooterAdornment,			kXPGUIPlacedFormeFooterAdornmentImpl)
REGISTER_PMINTERFACE(XPGUIXmlWithFormeFileHandler,				kXPGUIXmlWithFormeFileHandlerImpl)
REGISTER_PMINTERFACE(XPGUIAddToArtDialogObserver,				kXPGUIAddToArtDialogObserverImpl)
REGISTER_PMINTERFACE(XPGUIAssemblagesPanelObserver,				kXPGUIAssemblagesPanelObserverImpl)
REGISTER_PMINTERFACE(XPGUINewAssemblageDialogController,		kXPGUINewAssemblageDialogControllerImpl)
REGISTER_PMINTERFACE(XPGUINewClasseurDialogController,			kXPGUINewClasseurDialogControllerImpl)
REGISTER_PMINTERFACE(XPGUIAssemblagesTVHierarchyAdapter,		kXPGUIAssemblagesTVHierarchyAdapterImpl)
REGISTER_PMINTERFACE(XPGUIArticleTVHierarchyAdapter,			kXPGUIArticleTVHierarchyAdapterImpl)
REGISTER_PMINTERFACE(XPGUIArticleTVWidgetMgr,					kXPGUIArticleTVWidgetMgrImpl)
REGISTER_PMINTERFACE(XPGUIArticlePanelObserver,					kXPGUIArticlePanelObserverImpl)
REGISTER_PMINTERFACE(XPGUIArticleViewObserver,					kXPGUIArticleViewObserverImpl)
REGISTER_PMINTERFACE(XPGUIImageNodeButtonsObserver,				kXPGUIImageNodeButtonsObserverImpl)
REGISTER_PMINTERFACE(XPGUIChooseEtatImageDialogController,		kXPGUIChooseEtatImageDialogControllerImpl)
REGISTER_PMINTERFACE(XPGUIImageEtatSwatchEH,					kXPGUIImageEtatSwatchEHImpl)


//  Code generated by DollyXs code generator
