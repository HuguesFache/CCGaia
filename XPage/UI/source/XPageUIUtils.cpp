
#include "VCPluginHeaders.h"

// Project includes
#include "XPageUIUtils.h"

// Interface includes
#include "IDialogMgr.h"
#include "IDialog.h"
#include "IApplication.h"
#include "ISysFileData.h"
#include "IStringData.h"
#include "IStringListControlData.h"
#include "IStringListData.h"
#include "IPanelControlData.h"
#include "ITextFrameColumn.h"
#include "IDropDownListController.h"
#include "XMLDefs.h"
// General includes
#include "LocaleSetting.h"
#include "RsrcSpec.h"
#include "CoreResTypes.h"
#include "IClassIDData.h"
#include "UIDList.h"
#include "Utils.h"
#include "IWaxIterator.h"
#include "IAssignmentUtils.h"
#include "ITextStrand.h"
#include "ITextModel.h"
#include "IFrameList.h"
#include "IBoolData.h"
#include "IWaxLine.h"
#include "AppUIID.h"
#include "SpellPanelID.h"
#include "TextEditorModelID.h"
#include "IWaxStrand.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"
#include "FileUtils.h"
#include "CAlert.h"
#include "XPGUIID.h"
#include "XRCID.h"
#include "IPMPointData.h"
#include "IUIDData.h"
#include "IIntData.h"
#include "ITextControlData.h"

// Carton validation
#include "IDocument.h"
#include "IAttributeStrand.h"
#include "IStyleInfo.h"
#include "IStyleGroupManager.h"
#include "AttributeBossList.h"
#include "TextID.h"
#include "GenericID.h"

// Photos liées placement check
#include "ISpreadList.h"
#include "ISpread.h"
#include "IHierarchy.h"
#include "ILinkManager.h"
#include "ILink.h"
#include "ILinkResource.h"
#include "ILayoutUIUtils.h"
#include "URI.h"

// Photos liées per-frame image import (extracted from XPGUIXRailImageDropTarget)
#include "ICommand.h"
#include "CmdUtils.h"
#include "IImportResourceCmdData.h"
#include "IPlacePIData.h"
#include "IGeometry.h"
#include "TransformUtils.h"
#include "IFrameContentFacade.h"
#include "IXMLUtils.h"
#include "IFormeData.h"
#include "IPlacedArticleData.h"
#include "IURIUtils.h"
#include "OpenPlaceID.h"
#include "PageItemScrapID.h"

// Photos liées — choose-photo-carton dialog
#include "IDialog.h"
#include "IDialogMgr.h"
#include "IFormeDataModel.h"
#include "IXPageMgrAccessor.h"

ErrorCode XPageUIUtils::DisplayChooseMatchingDialog(IDFile& matchingFile)
{
	ErrorCode status = kFailure;
	do
	{
		// Get the application interface and the DialogMgr.	
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		if (application == nil)
			break;

		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
		if (dialogMgr == nil)
			break;

		// Load the plug-in's resource.
		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcSpec dialogSpec
			(
			nLocale,					// Locale index from PMLocaleIDs.h. 
			kXPGUIPluginID,				// Plug-in ID
			kViewRsrcType,				// This is the kViewRsrcType.
			kXPGUIChooseMatchingDialogResourceID,	// Resource ID for our dialog.
			kTrue						// Initially visible.
			);

		// CreateNewDialog takes the dialogSpec created above, and also
		// the type of dialog being created (kMovableModal).

		IDialog * dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);
		dialog->Open();
		dialog->WaitForDialog();

		InterfacePtr<ISysFileData> fileData (dialog->GetDialogPanel(), UseDefaultIID());
		IDFile selectedFile = fileData->GetSysFile();

		if(selectedFile == IDFile())
			break;

		matchingFile = selectedFile;

		status = kSuccess;

	} while(kFalse);

	return status;
}

void XPageUIUtils::DisplayNewClasseurDialog(bool16 classeurAssemblage)
{
	do
	{
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		if (application == nil)
			break;

		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
		if (dialogMgr == nil)
			break;

		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcSpec dialogSpec
			(
			nLocale,								// Locale
			kXPGUIPluginID,							// Plug-in ID
			kViewRsrcType,							// Resource type
			kXPGUINewClasseurDialogResourceID,		// Resource ID
			kTrue									// Initially visible
			);

		IDialog * dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);
		if (dialog == nil)
			break;

		// Tell the dialog controller which model (Formes vs Assemblages) it
		// should target. Read back in XPGUINewClasseurDialogController.
		InterfacePtr<IBoolData> assemblageFlag(dialog->GetDialogPanel(), IID_IBOOLDATA);
		if (assemblageFlag)
			assemblageFlag->Set(classeurAssemblage);

		dialog->Open();
		dialog->WaitForDialog();

	} while (kFalse);
}

void XPageUIUtils::DisplayNewFormeDialog(const PMString& classeurName, bool16 assemblageDialog)
{
	do
	{
		// Get the application interface and the DialogMgr.	
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		if (application == nil)
			break;

		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
		if (dialogMgr == nil)
			break;

		// Load the plug-in's resource.
		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcID dialogResourceID = assemblageDialog ? kXPGUINewAssemblageDialogResourceID : kXPGUINewFormeDialogResourceID;
		RsrcSpec dialogSpec
			(
			nLocale,					// Locale index from PMLocaleIDs.h. 
			kXPGUIPluginID,				// Plug-in ID
			kViewRsrcType,				// This is the kViewRsrcType.
			dialogResourceID,	// Resource ID for our dialog.
			kTrue						// Initially visible.
			);

		// CreateNewDialog takes the dialogSpec created above, and also
		// the type of dialog being created (kMovableModal).
		IDialog * dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kModeless);

		// Pass the palette's currently-selected classeur into the dialog so
		// the controller can pre-select it. Done for both forme and assemblage
		// variants — both dialog bosses now aggregate IID_ICLASSEURNAMEDATA.
		InterfacePtr<IStringData> classeurData(dialog->GetDialogPanel(), IID_ICLASSEURNAMEDATA);
		if (classeurData)
			classeurData->Set(classeurName);

		dialog->Open();
		dialog->WaitForDialog();

	} while(kFalse);
}

void XPageUIUtils::DisplayLinkArticleDialog(const PMString& articleId, const PMString& articleSnippetFile,
											const PMString& articleSubject, IDFile& matchingFile, PMPoint currentPoint, 
											const PMString& idStatus, const PMString& libelleStatus,
											const PMString& couleurStatus, 
											UIDRef targetSpread, const int32 typeArt, 
											const PMString& articleXMLFile, const PMString& artRub, const PMString& artSubRub)
{
	do
	{
		// Get the application interface and the DialogMgr.	
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		if (application == nil)
			break;

		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
		if (dialogMgr == nil)
			break;

		// Load the plug-in's resource.
		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcSpec dialogSpec
		(
			nLocale,						// Locale index from PMLocaleIDs.h. 
			kXPGUIPluginID,					// Plug-in ID
			kViewRsrcType,					// This is the kViewRsrcType.
			kXPGUILinkArtDialogResourceID,	// Resource ID for our dialog.
			kTrue							// Initially visible.
		);

		// CreateNewDialog takes the dialogSpec created above, and also
		// the type of dialog being created (kMovableModal).		
		IDialog * dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);

		InterfacePtr<IIntData> articleTypeData (dialog->GetDialogPanel(), IID_IARTICLETYPEDATA);
		articleTypeData->Set(typeArt);

		InterfacePtr<IStringData> articleData (dialog->GetDialogPanel(), IID_IARTICLEIDDATA);
		articleData->Set(articleId);

		InterfacePtr<IStringData> articleSubjectData (dialog->GetDialogPanel(), IID_IARTICLETOPICDATA);
		articleSubjectData->Set(articleSubject);

		InterfacePtr<IStringData> articleSnippetFileData (dialog->GetDialogPanel(), IID_IARTICLESNIPPETFILEDATA);
		articleSnippetFileData->Set(articleSnippetFile);

		InterfacePtr<IStringData> articleXMLFileData (dialog->GetDialogPanel(), IID_IARTICLEXMLFILEDATA);
		articleXMLFileData->Set(articleXMLFile);		
		
		InterfacePtr<IStringData> articleIdStatusData (dialog->GetDialogPanel(), IID_IARTICLEIDSTATUS);
        articleIdStatusData->Set(idStatus);
	  
	    InterfacePtr<IStringData> articleLibelleStatusData (dialog->GetDialogPanel(), IID_IARTICLELIBELLESTATUS);
		articleLibelleStatusData->Set(libelleStatus);

		InterfacePtr<IStringData> articleCouleurStatusData (dialog->GetDialogPanel(), IID_IARTICLECOULEURSTATUS);
		articleCouleurStatusData->Set(couleurStatus);

		InterfacePtr<IStringData> articleRubData (dialog->GetDialogPanel(), IID_IARTICLERUBDATA);
		articleRubData->Set(artRub);

		InterfacePtr<IStringData> articleSubRubData (dialog->GetDialogPanel(), IID_IARTICLESUBRUBDATA);
		articleSubRubData->Set(artSubRub);

		InterfacePtr<ISysFileData> matchingData (dialog->GetDialogPanel(), UseDefaultIID());
		matchingData->Set(matchingFile);

		InterfacePtr<IPMPointData>  pmPointData (dialog->GetDialogPanel(), UseDefaultIID());
		pmPointData->Set(currentPoint);

		InterfacePtr<IUIDData> uidData(dialog->GetDialogPanel(), UseDefaultIID());
		uidData->Set(targetSpread);
		
		dialog->Open();
		dialog->WaitForDialog();

	} while(kFalse);
}


void XPageUIUtils::FillDropdownlist(const IPanelControlData * panelCtrlData, const WidgetID& dropdownlist, IDValueList elements, bool16 notify){

	do{
		IControlView * dropdownview = panelCtrlData->FindWidget(dropdownlist);
		if(!dropdownview)
			break;

		InterfacePtr<IStringListControlData> dropDownListData (dropdownview, UseDefaultIID());
		if(!dropDownListData)
			break;

		dropDownListData->Clear();
		InterfacePtr<IStringListData> idListData (dropDownListData, IID_IIDLIST);
		if(!idListData)
			break; 
		
		K2Vector<PMString> idList;
		for(int32 i = 0 ; i < elements.size() ; ++i){
			PMString value = elements[i].Value();
			value.SetTranslatable(kFalse);
			dropDownListData->AddString(value);
			idList.push_back(elements[i].Key());
		}	

		idListData->SetStringList(idList);

		// Select first item in the list, if any
		if(!elements.empty()){
			InterfacePtr<IDropDownListController> dropDownListController (dropDownListData, UseDefaultIID());
			dropDownListController->Select(IDropDownListController::kBeginning, kTrue, notify);
		}

	} while(kFalse);
}

void XPageUIUtils::FillDropdownlist(const IPanelControlData * panelCtrlData, const WidgetID& dropdownlist, K2Vector<PMString> elements, bool16 notify)
{
	do
	{
		IControlView * dropdownview = panelCtrlData->FindWidget(dropdownlist);
		if(!dropdownview)
			break;

		InterfacePtr<IStringListControlData> dropDownListData (dropdownview, UseDefaultIID());
		dropDownListData->Clear();

		for(int32 i = 0 ; i < elements.size() ; ++i)
		{
			PMString value = elements[i];
			value.SetTranslatable(kFalse);
			dropDownListData->AddString(value);
		}			

		// Select first item in the list, if any
		if(!elements.empty())
		{
			InterfacePtr<IDropDownListController> dropDownListController (dropDownListData, UseDefaultIID());
			dropDownListController->Select(IDropDownListController::kBeginning, kTrue, notify);
		}
	} while(kFalse);
}

void XPageUIUtils::FillDropdownlistAndSelect(const IPanelControlData * panelCtrlData, const WidgetID& dropdownlist, K2Vector<PMString> elements, const PMString& elementToSelect, bool16 notify)
{
	do
	{
		IControlView * dropdownview = panelCtrlData->FindWidget(dropdownlist);
		if(!dropdownview)
			break;

		// Get current selected index
		int32 selectedIndex = IDropDownListController::kNoSelection;
		InterfacePtr<IDropDownListController> dropDownListController (dropdownview, UseDefaultIID());
		if(elementToSelect == kNullString) // No element to select passed in parameter, keep current selection
		{		
			selectedIndex = dropDownListController->GetSelected();
			if(selectedIndex == IDropDownListController::kNoSelection || selectedIndex >= elements.size())
				selectedIndex = IDropDownListController::kBeginning;
		}

		// Populate list widget
		InterfacePtr<IStringListControlData> dropDownListData (dropdownview, UseDefaultIID());
		dropDownListData->Clear();

		for(int32 i = 0 ; i < elements.size() ; ++i)
		{
			PMString value = elements[i];
			value.SetTranslatable(kFalse);
			dropDownListData->AddString(value);
		}	

		// Select the element, if any
		if(elementToSelect != kNullString)
			selectedIndex = dropDownListData->GetIndex(elementToSelect);

		if(selectedIndex != IDropDownListController::kNoSelection && !elements.empty())
		{
			dropDownListController->Select(selectedIndex, kTrue, notify);

			InterfacePtr<ITextControlData> listTxtControlData (dropDownListController, UseDefaultIID());
			listTxtControlData->SetString(elements[selectedIndex], kTrue, kFalse);
		}

	}while(kFalse);
}

bool16 XPageUIUtils::IsValidName(const PMString& fileName)
{
	// Note on '_': the underscore is rejected because it is used as the
	// internal delimiter in placed-carton unique names
	// ({classeur}_{carton}_{YYYYMMDDHHMMSS}). Allowing '_' inside a classeur
	// or carton name breaks the reverse lookup required to find the right
	// typographie file at import time. Names created before this rule was
	// enforced continue to work (SplitFormeDescription does a
	// model-backed reverse lookup instead of a naive split).
	char invalidChars[10] = { '/', '\\', ':', '*', '?', '"', '<', '>', '|', '_' };
	bool16 isValid = kTrue;
	for(int32 i = 0 ; i < 10 ; ++i)
	{
		if(fileName.IndexOfCharacter(invalidChars[i]) >= 0)
		{
			isValid = kFalse;
			break;
		}
	}

	return isValid;
}


void XPageUIUtils::RefreshXPagePrefsFromServer()
{
	InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
	if (xpgPrefs == nil)
		return;

	InterfacePtr<IWebServices> baseHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
	if (baseHTTP == nil)
		return;

	int32 prefs_ExportXML = 0, prefs_GestionIDMS = 1, prefs_IDMSMAJIDMS = 1,
	      prefs_IDMSALLBLOCS = 0, prefs_ImportLegendes = 1, prefs_ImportCredits = 1;

	K2Vector<int32> ei_IDs, ei_Ordres;
	K2Vector<PMString> ei_Noms, ei_Couleurs;
	K2Vector<int32> ea_IDs, ea_Ordres, ea_Couleurs, ea_Rayures;
	K2Vector<PMString> ea_Noms, ea_CouleursHTML;

	if (baseHTTP->GetPrefsXPage_v2(xpgPrefs->GetTEC_URL(), xpgPrefs->GetPluginServerName(),
		prefs_ExportXML, prefs_GestionIDMS, prefs_IDMSMAJIDMS, prefs_IDMSALLBLOCS,
		prefs_ImportLegendes, prefs_ImportCredits,
		ei_IDs, ei_Ordres, ei_Noms, ei_Couleurs,
		ea_IDs, ea_Ordres, ea_Noms, ea_CouleursHTML, ea_Couleurs, ea_Rayures))
	{
		xpgPrefs->SetGestionIDMS(prefs_GestionIDMS);
		xpgPrefs->SetIDMS_MAJIDMS(prefs_IDMSMAJIDMS);
		xpgPrefs->SetIDMSALLBLOCS(prefs_IDMSALLBLOCS);
		xpgPrefs->SetImportLegende(prefs_ImportLegendes);
		xpgPrefs->SetImportCredit(prefs_ImportCredits);

		EtatImageList etatsImages;
		for (int32 i = 0; i < ei_IDs.size(); ++i) {
			EtatImage e;
			e.ID = ei_IDs[i];
			e.Ordre = ei_Ordres[i];
			e.Nom = ei_Noms[i];
			e.Couleur = ei_Couleurs[i];
			etatsImages.push_back(e);
		}
		xpgPrefs->SetEtatsImages(etatsImages);

		EtatArticleList etatsArticles;
		for (int32 i = 0; i < ea_IDs.size(); ++i) {
			EtatArticle e;
			e.ID = ea_IDs[i];
			e.Ordre = ea_Ordres[i];
			e.Nom = ea_Noms[i];
			e.CouleurHTML = ea_CouleursHTML[i];
			e.Couleur = ea_Couleurs[i];
			e.Rayures = ea_Rayures[i];
			etatsArticles.push_back(e);
		}
		xpgPrefs->SetEtatsArticles(etatsArticles);
	}
	// Silent on failure: the broadcast that triggered us came right after a
	// successful VerifLogin, so a failure here is unusual but not worth
	// alerting the user about.
}


// ============================================================================
// Photos liées — encodage/decodage du crop transporte par le drag
// ============================================================================

PMString XPageUIUtils::PackCropPayload(const PMReal& cropX, const PMReal& cropY,
                                       const PMReal& cropW, const PMReal& cropH,
                                       const PMString& brPath)
{
	// 4 entiers (pixels BR x1000) separes par '|', puis le chemin BR.
	PMString s;
	s.AppendNumber((int32)(::ToDouble(cropX) * 1000.0 + 0.5)); s.Append("|");
	s.AppendNumber((int32)(::ToDouble(cropY) * 1000.0 + 0.5)); s.Append("|");
	s.AppendNumber((int32)(::ToDouble(cropW) * 1000.0 + 0.5)); s.Append("|");
	s.AppendNumber((int32)(::ToDouble(cropH) * 1000.0 + 0.5)); s.Append("|");
	s.Append(brPath);
	s.SetTranslatable(kFalse);
	return s;
}

void XPageUIUtils::UnpackCropPayload(const PMString& packed, PMReal& cropX, PMReal& cropY,
                                     PMReal& cropW, PMReal& cropH, PMString& brPath)
{
	cropX = 0; cropY = 0; cropW = 0; cropH = 0;
	brPath.Clear();
	if (packed.IsEmpty())
		return;

	PMReal* outs[4] = { &cropX, &cropY, &cropW, &cropH };
	PMString rest(packed);
	PlatformChar sep; sep.Set('|');

	for (int32 i = 0; i < 4; ++i)
	{
		const int32 idx = rest.IndexOfCharacter(sep);
		if (idx < 0)
			return;   // payload malforme -> on laisse le crop a zero

		PMString token(rest);
		token.Truncate(idx);          // garde [0, idx)
		rest.Remove(0, idx + 1);      // enleve le token + le separateur

		PMString::ConversionError err = PMString::kNoError;
		const int32 scaled = token.GetAsNumber(&err);
		if (err != PMString::kNoError)
			return;
		*outs[i] = PMReal(scaled) / PMReal(1000);
	}

	// Le reste est le chemin de la vignette BR ('|' est illegal dans un nom de
	// fichier Windows, donc aucune ambiguite avec les separateurs ci-dessus).
	brPath = rest;
}


// ============================================================================
// Photos liées — per-frame image import (extracted from drop target)
// ============================================================================

ErrorCode XPageUIUtils::ImportPhotoIntoFrame(const UIDRef& photoFrameRef,
                                              const IDFile& imageFile,
                                              const PMString& creditText,
                                              const PMString& legendText,
                                              const PMString& draggedArticleId,
                                              const PMString& cropData)
{
	if (photoFrameRef == UIDRef::gNull)
		return kFailure;

	IDataBase* db = photoFrameRef.GetDataBase();
	if (db == nil)
		return kFailure;

	ErrorCode returnCode = kFailure;
	do {
		// 1. Import image as a resource.
		InterfacePtr<ICommand> importFileCmd(CmdUtils::CreateCommand(kImportResourceCmdBoss));
		InterfacePtr<IImportResourceCmdData> data(importFileCmd, IID_IIMPORTRESOURCECMDDATA);

		URI imageURI;
		Utils<IURIUtils>()->IDFileToURI(imageFile, imageURI);
		data->Set(db, imageURI, K2::kSuppressUI);
		if (CmdUtils::ProcessCommand(importFileCmd) != kSuccess)
			break;

		UIDRef imageItem = UIDRef(db, importFileCmd->GetItemList()->First());

		// 2. Delete existing child of target (replace).
		InterfacePtr<IHierarchy> containerFrame(photoFrameRef, UseDefaultIID());
		if (containerFrame && containerFrame->GetChildCount() > 0) {
			InterfacePtr<IHierarchy> frameToDelete(containerFrame->QueryChild(0));
			InterfacePtr<ICommand> deletePageItem(CmdUtils::CreateCommand(kDeleteCmdBoss));
			deletePageItem->SetItemList(UIDList(frameToDelete));
			if (CmdUtils::ProcessCommand(deletePageItem) != kSuccess)
				break;
		}

		// 3. Compute target bounding box and place imageItem inside.
		InterfacePtr<IGeometry> itemGeo(photoFrameRef, UseDefaultIID());
		if (itemGeo == nil)
			break;

		PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();
		PMPoint rightBottom = itemGeo->GetStrokeBoundingBox().RightBottom();
		::TransformInnerPointToPasteboard(itemGeo, &leftTop);
		::TransformInnerPointToPasteboard(itemGeo, &rightBottom);

		PMPointList pointList;
		pointList.push_back(leftTop);
		pointList.push_back(rightBottom);

		InterfacePtr<ICommand> placeItemCmd(CmdUtils::CreateCommand(kPlaceItemInGraphicFrameCmdBoss));
		placeItemCmd->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<IPlacePIData> placeData(placeItemCmd, IID_IPLACEPIDATA);
		placeData->Set(photoFrameRef, &pointList, kFalse);
		placeItemCmd->SetItemList(UIDList(imageItem));
		returnCode = CmdUtils::ProcessCommand(placeItemCmd);

		// 4. Crop (si renseigne dans le payload du drag), sinon ajustement
		//    proportionnel comme avant.
		PMReal cropX, cropY, cropW, cropH;
		PMString brPathStr;
		UnpackCropPayload(cropData, cropX, cropY, cropW, cropH, brPathStr);
		IDFile brFile = FileUtils::PMStringToSysFile(brPathStr);

		if (!(cropW > 0 && cropH > 0 &&
			Utils<IXPageUtils>()->ApplyCropToImage(photoFrameRef, imageItem,
				cropX, cropY, cropW, cropH, brFile) == kSuccess))
		{
			UIDList imageList = UIDList(imageItem);
			Utils<Facade::IFrameContentFacade>()->FitContentProp(imageList);
		}

		// 5. Article tagging + credit/legend — same as the on-frame drop target.
		InterfacePtr<IFormeData> photoFormeData(photoFrameRef, UseDefaultIID());
		if (photoFormeData == nil)
			break;

		InterfacePtr<ISpread> imageSpread(db, containerFrame->GetSpreadUID(), UseDefaultIID());
		if (imageSpread == nil)
			break;

		PMString formeName  = photoFormeData->GetUniqueName();
		int16    photoIndex = photoFormeData->GetPhotoIndex();

		// Article-graft block (cf. XPGUIXRailImageDropTarget for full
		// rationale): if the target frame doesn't already belong to the
		// dragged photo's article, copy article tags from a sibling frame
		// already on the spread that does, and re-tag linked credit/legend
		// frames so they stay paired.
		if (!draggedArticleId.IsEmpty())
		{
			InterfacePtr<IPlacedArticleData> targetPAD(photoFrameRef, UseDefaultIID());
			const bool16 inArticle = (targetPAD != nil &&
				targetPAD->GetUniqueId() == draggedArticleId);

			if (!inArticle)
			{
				PMString refFormeName, refStoryFolder, refStoryTopic;
				int16    maxPhotoIndex = -1;
				bool16   foundRef      = kFalse;

				UIDList itemsOnSpread(db);
				imageSpread->GetItemsOnPage(0, &itemsOnSpread, kFalse, kTrue);
				const int32 nbAll = itemsOnSpread.Length();
				for (int32 i = 0; i < nbAll; ++i)
				{
					UIDRef itemRef = itemsOnSpread.GetRef(i);
					InterfacePtr<IPlacedArticleData> itemPAD(itemRef, UseDefaultIID());
					if (itemPAD == nil)
						continue;
					if (itemPAD->GetUniqueId() != draggedArticleId)
						continue;

					InterfacePtr<IFormeData> itemFD(itemRef, UseDefaultIID());
					if (itemFD == nil)
						continue;

					if (!foundRef)
					{
						refFormeName   = itemFD->GetUniqueName();
						refStoryFolder = itemPAD->GetStoryFolder();
						refStoryTopic  = itemPAD->GetStoryTopic();
						foundRef       = kTrue;
					}

					if (itemFD->GetType() == IFormeData::kPhoto)
					{
						const int16 idx = itemFD->GetPhotoIndex();
						if (idx > maxPhotoIndex)
							maxPhotoIndex = idx;
					}
				}

				if (foundRef)
				{
					const PMString oldFormeName  = formeName;
					const int16    oldPhotoIndex = photoIndex;

					formeName  = refFormeName;
					photoIndex = static_cast<int16>(maxPhotoIndex + 1);

					InterfacePtr<ICommand> padCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
					if (padCmd != nil)
					{
						InterfacePtr<IPlacedArticleData> padCmdData(padCmd, IID_IPLACEDARTICLEDATA);
						if (padCmdData != nil)
						{
							padCmdData->SetUniqueId(draggedArticleId);
							padCmdData->SetStoryFolder(refStoryFolder);
							padCmdData->SetStoryTopic(refStoryTopic);
							padCmd->SetItemList(UIDList(photoFrameRef));
							CmdUtils::ProcessCommand(padCmd);
						}
					}

					Utils<IXPageUtils>()->SetFormeData(photoFrameRef, formeName,
						IFormeData::kPhoto, photoIndex, kNullString, kNullString);

					if (!oldFormeName.IsEmpty())
					{
						UIDList oldFormeItems(db);
						Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(oldFormeName, imageSpread, oldFormeItems);
						const int32 nbOld = oldFormeItems.Length();
						for (int32 i = 0; i < nbOld; ++i)
						{
							UIDRef linkedRef = oldFormeItems.GetRef(i);
							if (linkedRef == photoFrameRef)
								continue;

							InterfacePtr<IFormeData> linkedFD(linkedRef, UseDefaultIID());
							if (linkedFD == nil)
								continue;

							const int16 linkedType = linkedFD->GetType();
							const int16 linkedIdx  = linkedFD->GetPhotoIndex();
							if ((linkedType != IFormeData::kCredit && linkedType != IFormeData::kLegend) ||
								linkedIdx != oldPhotoIndex)
								continue;

							Utils<IXPageUtils>()->SetFormeData(linkedRef, formeName,
								linkedType, photoIndex, kNullString, kNullString);

							InterfacePtr<ICommand> linkedPadCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
							if (linkedPadCmd != nil)
							{
								InterfacePtr<IPlacedArticleData> linkedPadCmdData(linkedPadCmd, IID_IPLACEDARTICLEDATA);
								if (linkedPadCmdData != nil)
								{
									linkedPadCmdData->SetUniqueId(draggedArticleId);
									linkedPadCmdData->SetStoryFolder(refStoryFolder);
									linkedPadCmdData->SetStoryTopic(refStoryTopic);
									linkedPadCmd->SetItemList(UIDList(linkedRef));
									CmdUtils::ProcessCommand(linkedPadCmd);
								}
							}
						}
					}
				}
			}
		}

		UIDList allFormeItems(db);
		Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(formeName, imageSpread, allFormeItems);
		const int32 nbItems = allFormeItems.Length();
		for (int32 i = 0; i < nbItems; ++i) {
			InterfacePtr<IFormeData> itemFormeData(allFormeItems.GetRef(i), UseDefaultIID());
			if (itemFormeData == nil)
				continue;
			const int16 currentPhotoIndex = itemFormeData->GetPhotoIndex();
			if (itemFormeData->GetType() == IFormeData::kCredit && currentPhotoIndex == photoIndex) {
				UIDRef targetStory = ::GetUIDRef(itemFormeData);
				Utils<IXMLUtils>()->GetActualContent(targetStory);
				Utils<IXPageUtils>()->ImportCreditOrLegend(targetStory, creditText, kTrue);
			}
			else if (itemFormeData->GetType() == IFormeData::kLegend && currentPhotoIndex == photoIndex) {
				UIDRef targetStory = ::GetUIDRef(itemFormeData);
				Utils<IXMLUtils>()->GetActualContent(targetStory);
				Utils<IXPageUtils>()->ImportCreditOrLegend(targetStory, legendText, kFalse);
			}
		}

	} while (kFalse);

	return returnCode;
}


// ============================================================================
// Photos liées — "Choose photo carton" dialog launcher
// ============================================================================

namespace {

// Returns kTrue if at least one photo carton (forme tagged isPhotoCarton="1"
// in its descriptif XML) exists across all classeurs known to the model.
// Used by DisplayChoosePhotoCartonDialog to surface the "no photo carton
// available" alert before opening the dialog.
bool16 AnyPhotoCartonExists(IFormeDataModel* model)
{
	if (model == nil)
		return kFalse;
	K2Vector<PMString> classeurs = model->GetClasseurList();
	for (int32 i = 0; i < classeurs.size(); ++i) {
		K2Vector<PMString> formes = model->GetFormeList(classeurs[i]);
		for (int32 j = 0; j < formes.size(); ++j) {
			if (model->IsPhotoCarton(formes[j], classeurs[i]))
				return kTrue;
		}
	}
	return kFalse;
}

} // namespace

void XPageUIUtils::DisplayChoosePhotoCartonDialog(const IDFile& imageFile,
                                                   const PMString& creditText,
                                                   const PMString& legendText,
                                                   const PMString& draggedArticleId,
                                                   const PMPoint& dropPoint,
                                                   const UIDRef& targetSpread,
                                                   const PMString& cropData)
{
	// Pre-flight: warn the user when nothing is available, but still open
	// the dialog (option B in the spec) so the dropdowns are visible-but-empty.
	{
		InterfacePtr<IXPageMgrAccessor> pageMgrAccessor(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IFormeDataModel>   model(pageMgrAccessor->QueryFormeDataModel());
		if (!AnyPhotoCartonExists(model)) {
			PMString warning(kXPGUINoPhotoCartonAvailableKey);
			warning.Translate();
			CAlert::InformationAlert(warning);
		}
	}

	do {
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		if (application == nil)
			break;
		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
		if (dialogMgr == nil)
			break;

		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcSpec dialogSpec(nLocale, kXPGUIPluginID, kViewRsrcType,
			kXPGUIChoosePhotoCartonDialogResourceID, kTrue);

		IDialog* dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);
		if (dialog == nil)
			break;

		IControlView* panel = dialog->GetDialogPanel();

		// Stash drag payload onto the dialog boss. Same IIDs as the drag
		// source uses (see XPGUIXRailImageDragDropSource).
		InterfacePtr<ISysFileData> imageFileData(panel, UseDefaultIID());
		if (imageFileData) imageFileData->Set(imageFile);

		InterfacePtr<IStringData> creditDataPtr(panel, IID_ICREDITPHOTO);
		if (creditDataPtr) creditDataPtr->Set(creditText);

		InterfacePtr<IStringData> legendDataPtr(panel, IID_ILEGENDPHOTO);
		if (legendDataPtr) legendDataPtr->Set(legendText);

		InterfacePtr<IStringData> articleIdDataPtr(panel, IID_IPHOTOARTICLEID);
		if (articleIdDataPtr) articleIdDataPtr->Set(draggedArticleId);

		InterfacePtr<IStringData> cropDataPtr(panel, IID_IPHOTOCROPDATA);
		if (cropDataPtr) cropDataPtr->Set(cropData);

		InterfacePtr<IPMPointData> pointData(panel, UseDefaultIID());
		if (pointData) pointData->Set(dropPoint);

		InterfacePtr<IUIDData> spreadData(panel, UseDefaultIID());
		if (spreadData) spreadData->Set(targetSpread);

		dialog->Open();
		dialog->WaitForDialog();

	} while (kFalse);
}


// ============================================================================
// Photos liées — placement check
// ============================================================================

bool16 XPageUIUtils::IsImagePlacedInFrontDoc(const PMString& imageId)
{
	if (imageId.IsEmpty())
		return kFalse;

	IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (doc == nil)
		return kFalse;

	IDataBase* db = ::GetDataBase(doc);
	if (db == nil)
		return kFalse;

	InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
	if (spreadList == nil)
		return kFalse;

	InterfacePtr<ILinkManager> linkMgr(db, db->GetRootUID(), UseDefaultIID());
	if (linkMgr == nil)
		return kFalse;

	// Bracket the id with a trailing dot so "51226" doesn't match "512260"
	// in an unrelated path. Lowercased for case-insensitive comparison
	// (Windows filenames are case-insensitive).
	PMString needle(imageId);
	needle.Append(".");
	needle.ToLower();

	const int32 nbSpreads = spreadList->GetSpreadCount();
	for (int32 s = 0; s < nbSpreads; ++s)
	{
		InterfacePtr<ISpread> spread(db, spreadList->GetNthSpreadUID(s), UseDefaultIID());
		if (spread == nil)
			continue;

		UIDList itemsOnSpread(db);
		const int32 nbPages = spread->GetNumPages();
		for (int32 p = 0; p < nbPages; ++p)
			spread->GetItemsOnPage(p, &itemsOnSpread, kFalse, kTrue);

		// Inline group recursion via IHierarchy. GetItemsOnPage only
		// returns top-level page items — without this, a photo nested
		// inside a group never reaches the link query and the image
		// shows as "not placed" even though it is. Worklist-style
		// traversal: appending to itemsOnSpread mid-loop is safe because
		// UIDList::At reads each iteration's index fresh.
		for (int32 i = 0; i < itemsOnSpread.Length(); ++i)
		{
			UIDRef itemRef = itemsOnSpread.GetRef(i);
			InterfacePtr<IHierarchy> hier(itemRef, UseDefaultIID());
			if (hier == nil || hier->GetChildCount() == 0)
				continue;

			ILinkManager::QueryResult linkQuery;
			if (!linkMgr->QueryLinksByObjectUID(hier->GetChildUID(0), linkQuery))
			{
				// No link on child(0) — could be a group, descend.
				const int32 nbChildren = hier->GetChildCount();
				for (int32 c = 0; c < nbChildren; ++c)
					itemsOnSpread.Append(hier->GetChildUID(c));
				continue;
			}

			for (ILinkManager::QueryResult::const_iterator it = linkQuery.begin();
				 it != linkQuery.end(); ++it)
			{
				InterfacePtr<ILink> link(db, *it, UseDefaultIID());
				if (link == nil)
					continue;
				InterfacePtr<ILinkResource> linkRes(db, link->GetResource(), UseDefaultIID());
				if (linkRes == nil)
					continue;

				URI linkURI = linkRes->GetId();
				PMString linkPath(linkURI.GetURI().c_str(), PMString::kUnknownEncoding);
				linkPath.ToLower();
				if (linkPath.IndexOfString(needle) >= 0)
					return kTrue;
			}
		}
	}

	return kFalse;
}


namespace {

// Resolve an EtatImage "Couleur" string (e.g. "#ff6e00" or "vert") to an
// RGB triple in [0,1]. Returns kFalse if the string couldn't be parsed.
// The named-colour list mirrors the small set Gaia emits today.
bool16 ResolveCouleurString(const PMString& couleur, PMReal& r, PMReal& g, PMReal& b)
{
	PMString s(couleur);
	s.StripWhiteSpace(PMString::kLeadingAndTrailingWhiteSpace);
	if (s.IsEmpty())
		return kFalse;

	// Hex form "#RRGGBB". Manual nibble decode since PMString::GetAsNumber
	// only handles base 10.
	const std::string raw = s.GetPlatformString().c_str();
	if (raw.size() == 7 && raw[0] == '#')
	{
		int32 packed = 0;
		bool ok = true;
		for (int i = 1; i < 7 && ok; ++i)
		{
			const char c = raw[i];
			int nibble;
			if      (c >= '0' && c <= '9') nibble = c - '0';
			else if (c >= 'a' && c <= 'f') nibble = 10 + (c - 'a');
			else if (c >= 'A' && c <= 'F') nibble = 10 + (c - 'A');
			else { ok = false; break; }
			packed = (packed << 4) | nibble;
		}
		if (ok)
		{
			r = ((packed >> 16) & 0xFF) / 255.0;
			g = ((packed >> 8)  & 0xFF) / 255.0;
			b = (packed         & 0xFF) / 255.0;
			return kTrue;
		}
	}

	// Named (French) colours used by Gaia in etatsimages.
	struct Named { const char* name; int32 rgb; };
	static const Named kNames[] = {
		{"vert",       0x00CC00},
		{"rouge",      0xFF0000},
		{"bleu",       0x0033FF},
		{"jaune",      0xFFFF00},
		{"orange",     0xFF9900},
		{"noir",       0x000000},
		{"blanc",      0xFFFFFF},
		{"gris",       0x808080},
		{"violet",     0x9933FF},
		{"rose",       0xFF3399},
		{"marron",     0x8B4513},
		{"cyan",       0x00FFFF},
		{"magenta",    0xFF00FF},
	};
	for (size_t i = 0; i < sizeof(kNames) / sizeof(kNames[0]); ++i)
	{
		if (s.Compare(kFalse, kNames[i].name) == 0)
		{
			r = ((kNames[i].rgb >> 16) & 0xFF) / 255.0;
			g = ((kNames[i].rgb >> 8)  & 0xFF) / 255.0;
			b = (kNames[i].rgb         & 0xFF) / 255.0;
			return kTrue;
		}
	}
	return kFalse;
}

} // namespace


bool16 XPageUIUtils::GetEtatImageColor(int32 etatId, PMReal& r, PMReal& g, PMReal& b)
{
	if (etatId <= 0)
		return kFalse;

	InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
	if (xpgPrefs == nil)
		return kFalse;

	EtatImageList etats = xpgPrefs->GetEtatsImages();
	for (int32 i = 0; i < etats.size(); ++i)
	{
		if (etats[i].ID == etatId)
			return ResolveCouleurString(etats[i].Couleur, r, g, b);
	}
	return kFalse;
}


// ============================================================================
// Carton creation pre-flight validation
// ============================================================================

namespace {

// Add `uid` to `vec` if it isn't already there. Linear scan, but the vectors
// stay tiny in practice (one entry per distinct paragraph style or story in
// the selection).
void AppendIfMissing(K2Vector<UID>& vec, UID uid)
{
	for (int32 i = 0; i < vec.size(); ++i) {
		if (vec[i] == uid)
			return;
	}
	vec.push_back(uid);
}

// Resolve UIDs to style names, sort alphabetically, and append one per line
// (indented with a bullet) to `out`. Names are marked non-translatable so
// the localizer doesn't try to translate user style names.
void AppendStyleNames(IDataBase* db, const K2Vector<UID>& styles, PMString& out)
{
	K2Vector<PMString> names;
	for (int32 i = 0; i < styles.size(); ++i) {
		InterfacePtr<IStyleInfo> info(db, styles[i], UseDefaultIID());
		if (info == nil)
			continue;
		PMString n = info->GetName();
		n.SetTranslatable(kFalse);
		names.push_back(n);
	}
	// Insertion sort — vectors stay small (one entry per offending style).
	for (int32 i = 1; i < names.size(); ++i) {
		PMString key = names[i];
		int32 j = i - 1;
		while (j >= 0 && names[j].Compare(kFalse, key) > 0) {
			names[j + 1] = names[j];
			--j;
		}
		names[j + 1] = key;
	}
	for (int32 i = 0; i < names.size(); ++i) {
		PMString line("    - ", PMString::kUnknownEncoding);
		line.SetTranslatable(kFalse);
		line.Append(names[i]);
		line.Append("\r\n");
		out.Append(line);
	}
}

} // anonymous namespace


bool16 XPageUIUtils::ValidateSelectionForCarton(const UIDList& textFrames, PMString& errorMessage)
{
	errorMessage.Clear();
	if (textFrames.IsEmpty())
		return kTrue;

	IDataBase* db = textFrames.GetDataBase();
	if (db == nil)
		return kTrue;

	// Resolve the two style UIDs that mean "no real style" — the root
	// ([No paragraph style]) and the default ([Basic Paragraph]). A paragraph
	// using either of these fails rule 3.
	UID rootStyleUID = kInvalidUID;
	UID defaultStyleUID = kInvalidUID;
	{
		InterfacePtr<IDocument> doc(db, db->GetRootUID(), UseDefaultIID());
		if (doc != nil) {
			InterfacePtr<IStyleGroupManager> paraStyleMgr(
				doc->GetDocWorkSpace(), IID_IPARASTYLEGROUPMANAGER);
			if (paraStyleMgr != nil) {
				rootStyleUID    = paraStyleMgr->GetRootStyleUID();
				defaultStyleUID = paraStyleMgr->GetDefaultStyleUID();
			}
		}
	}

	K2Vector<UID> overriddenStyles;          // styles seen with at least one "+" paragraph
	K2Vector<UID> uniqueStyles;              // every named style encountered
	K2Vector< K2Vector<UID> > storyUIDsPerStyle;  // parallel: stories using each style
	bool16 hasNoStyle = kFalse;

	for (int32 i = 0; i < textFrames.Length(); ++i) {
		UIDRef itemRef = textFrames.GetRef(i);

		// IXPageSuite::GetSelectedItems returns frame UIDRefs, not story
		// UIDRefs. Without this normalization, the next InterfacePtr would
		// be nil and the whole loop would silently skip every paragraph —
		// validation always passes. Same pattern used by GetStatisticsText.
		Utils<IAssignmentUtils>()->NormalizeTextModelRef(itemRef);

		InterfacePtr<ITextModel> txtModel(itemRef, UseDefaultIID());
		if (txtModel == nil)
			continue;

		// Threaded frames share one ITextModel — using the model UID as the
		// "story" key makes rule 2 fall out automatically: same story = OK.
		const UID storyUID = ::GetUID(txtModel);

		InterfacePtr<IAttributeStrand> paraStrand(static_cast<IAttributeStrand*>(
			txtModel->QueryStrand(kParaAttrStrandBoss, IID_IATTRIBUTESTRAND)));
		if (paraStrand == nil)
			continue;
		InterfacePtr<IAttributeStrand> charStrand(static_cast<IAttributeStrand*>(
			txtModel->QueryStrand(kCharAttrStrandBoss, IID_IATTRIBUTESTRAND)));

		const TextIndex span = txtModel->GetPrimaryStoryThreadSpan();
		TextIndex pos = 0;

		while (pos < span) {
			int32 paraStyleRunLen = 0, paraOvLen = 0, charOvLen = 0;
			const UID styleUID = paraStrand->GetStyleUID(pos, &paraStyleRunLen);

			DataWrapper<AttributeBossList> paraOv =
				paraStrand->GetLocalOverrides(pos, &paraOvLen);
			const AttributeBossList* paraOvList = paraOv.get();
			const bool16 hasParaOverride = (paraOvList != nil && paraOvList->CountBosses() > 0);

			bool16 hasCharOverride = kFalse;
			if (charStrand != nil) {
				DataWrapper<AttributeBossList> charOv =
					charStrand->GetLocalOverrides(pos, &charOvLen);
				const AttributeBossList* charOvList = charOv.get();
				hasCharOverride = (charOvList != nil && charOvList->CountBosses() > 0);
			}

			if (styleUID == kInvalidUID || styleUID == rootStyleUID || styleUID == defaultStyleUID) {
				hasNoStyle = kTrue;
			} else {
				// Track style → set of stories
				int32 idx = -1;
				for (int32 j = 0; j < uniqueStyles.size(); ++j) {
					if (uniqueStyles[j] == styleUID) { idx = j; break; }
				}
				if (idx < 0) {
					uniqueStyles.push_back(styleUID);
					K2Vector<UID> v;
					v.push_back(storyUID);
					storyUIDsPerStyle.push_back(v);
				} else {
					AppendIfMissing(storyUIDsPerStyle[idx], storyUID);
				}

				if (hasParaOverride || hasCharOverride)
					AppendIfMissing(overriddenStyles, styleUID);
			}

			// Advance by the smallest run length so the (style, paraOv, charOv)
			// tuple stays consistent within each step.
			int32 advance = paraStyleRunLen;
			if (paraOvLen > 0 && paraOvLen < advance) advance = paraOvLen;
			if (charOvLen > 0 && charOvLen < advance) advance = charOvLen;
			if (advance <= 0) advance = 1;  // safety against pathological strands
			pos += advance;
		}
	}

	// Cross-story duplicates: same style used in 2+ distinct stories.
	K2Vector<UID> crossStoryStyles;
	for (int32 i = 0; i < uniqueStyles.size(); ++i) {
		if (storyUIDsPerStyle[i].size() > 1)
			crossStoryStyles.push_back(uniqueStyles[i]);
	}

	const bool16 valid =
		overriddenStyles.empty() && crossStoryStyles.empty() && !hasNoStyle;
	if (valid)
		return kTrue;

	// Build the alert text. Each section header is a localized key; the
	// dynamic style names are appended verbatim.
	errorMessage = PMString(kXPGUICartonValidationHeaderKey, PMString::kTranslateDuringCall);

	if (!overriddenStyles.empty()) {
		errorMessage.Append(PMString(kXPGUICartonValidationOverridesKey,
		                             PMString::kTranslateDuringCall));
		AppendStyleNames(db, overriddenStyles, errorMessage);
		errorMessage.Append("\r\n");
	}
	if (!crossStoryStyles.empty()) {
		errorMessage.Append(PMString(kXPGUICartonValidationCrossStoriesKey,
		                             PMString::kTranslateDuringCall));
		AppendStyleNames(db, crossStoryStyles, errorMessage);
		errorMessage.Append("\r\n");
	}
	if (hasNoStyle) {
		errorMessage.Append(PMString(kXPGUICartonValidationNoStyleKey,
		                             PMString::kTranslateDuringCall));
	}

	return kFalse;
}