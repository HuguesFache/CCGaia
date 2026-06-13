/*
//
//	File:	XPGExportArtResp.cpp
//
//  Author: Trias Developpement
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "Utils.h"
#include "FileUtils.h"
#include "StringUtils.h"
// Project includes
#include "XPGID.h"
//#include "TESID.h"

// Interface includes:
#include "ISignalMgr.h"
#include "IDocument.h"
#include "IAssignmentMgr.h"
#include "IAssignedDocument.h"
#include "IAssignment.h"
#include "IAssignmentUtils.h"
#include "IInCopyWorkFlowUtils.h"
#include "InCopyImportID.h"
#include "IWebServices.h"
#include "ILayoutUtils.h"
#include "IHierarchy.h"
#include "ITextModel.h"
#include "IDocumentCommands.h"
#include "IDocumentSignalData.h"
#include "IGuidePrefs.h"
#include "ISpreadLayer.h"
#include "ILayerList.h"
#include "ISpreadList.h"
#include "IStringListData.h"
#include "IPageItemTypeUtils.h"
#include "IUIFlagData.h"
#include "IDialog.h"
#include "IDialogMgr.h"
#include "IApplication.h"
#include "RsrcSpec.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "IUnitOfMeasure.h"
#include "StreamUtil.h"
#include "ISnippetExport.h"
#include "IFrameType.h"

// General includes:
#include "CDialogController.h"
#include "FileUtils.h"
#include "ErrorUtils.h"
#include "CAlert.h"
#include "K2Pair.h"
#include "CResponder.h"
#include "UIDList.h"
#include "ISelectionUtils.h"
#include "ISelectionManager.h"
#include "IDocumentUtils.h"
#include "IGeometry.h"
#include "ISpread.h"
#include "TransformUtils.h"
#include "ITransformFacade.h"
#include "IStringData.h"
#include "ILayoutUtils.h"
#include "CAlert.h"
#include "IStoryUtils.h"
#include "IDataLink.h"
#include "ITextFrameColumn.h"
#include "IXPageUtils.h"
#include "IXPGPreferences.h"
#include "IXPageSuite.h"
#include "IXRailPageSlugData.h"
#include "ITextStoryThread.h"
#include "IFrameUtils.h"
#include "ILinkObject.h"
#include "ILinkManager.h"
#include "IURIUtils.h"
#include "URI.h"
#include "ITextStrand.h"
#include "IPlacedArticleData.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "XRLID.h"
#include "IFormeData.h"

class XPGExportArtResp : public CResponder
{
public:
	/**
	Constructor.
	@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGExportArtResp(IPMUnknown* boss);

	/**
	Respond() handles the file action signals when they
	are dispatched by the signal manager.  This implementation
	simply creates alerts to display each signal.

	@param signalMgr Pointer back to the signal manager to get
	additional information about the signal.
	*/
	virtual void Respond(ISignalMgr* signalMgr);

private:

	/**
	Used internally during export
	*/
	void HandleItem(IHierarchy* itemHier, K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > >& exportableStories,
		K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > >& itemsByArticle, UIDList& alreadyAddedStories);


	void ExportLegendesCredits(IDataBase* db, IDocument* theDoc);
	void ReorderPictures(IDataBase* db, IDocument* theDoc);
	void ExportInCopyStories(IDataBase* db, IDocument* theDoc, ISignalMgr* signalMgr);
	void ExportPlacedStories(IDataBase* db, IDocument* theDoc);
	void DisplayChangeStatusDialog();
	void GetNbSignes();
	UIDList RecupCoordBlocs(IDataBase* db, PMString artID, UIDList& articleItems);
	bool16 CheckResourceInCopy(UIDList& articleItems);
	bool16 DoExport(PMString artID, PMString artPath, UIDList& articleItems);
	PMString GetPictureFileName(IDataBase* db, UID curPict);
	WideString GetBlocContent(IDataBase* db, UID storyRef);

	//liste de tous les tableaux qui doivent être remplis par les differentes methodes lors de la sauvegarde
	K2Vector<int32>TabIDStories;
	K2Vector<int32>TabTypes;
	K2Vector<int32>ListPages;
	K2Vector<int32>TabNbSignes;
	K2Vector<int32>IDStoriesByBloc;
	K2Vector<PMString>TabX;
	K2Vector<PMString>TabY;
	K2Vector<PMString>TabLargeur;
	K2Vector<PMString>TabHauteur;
	K2Vector<int32>TabIDPage;
	K2Vector<PMString>ListeImages;
	K2Vector<WideString>ListeLegendes;
	K2Vector<WideString>ListeCredits;
	K2Vector<WideString>ListeLegendesSecours;
	K2Vector<WideString>ListeCreditsSecours;

};


/* CREATE_PMINTERFACE
Binds the C++ implementation class onto its ImplementationID
making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGExportArtResp, kXPGExportArtRespImpl)

/* DocWchActionComponent Constructor
*/
XPGExportArtResp::XPGExportArtResp(IPMUnknown* boss) :
	CResponder(boss)
{}

/* Respond
*/
void XPGExportArtResp::Respond(ISignalMgr* signalMgr) {
	// Get the service ID from the signal manager
	ServiceID serviceTrigger = signalMgr->GetServiceID();
	// Notify XRail + Preferences
	InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IWebServices> baseHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef docRef = docData->GetDocument();
	InterfacePtr<IDocument> theDoc(docRef, UseDefaultIID());

	IDataBase* db = ::GetDataBase(theDoc);


	int32 nbPages = GetNbPages(docRef);
	K2Vector<int32> tabIDPages;

	for (int16 i = 0; i < nbPages; i++) {
		int32 idPage = Utils<IXPageUtils>()->GetPageID(docRef, i);
		if (idPage > 0) {
			tabIDPages.push_back(idPage);
		}
	}



	//on cleane les tableaux
	TabIDStories.clear();
	TabTypes.clear();
	ListPages.clear();
	TabNbSignes.clear();
	IDStoriesByBloc.clear();
	TabX.clear();
	TabY.clear();
	TabLargeur.clear();
	TabHauteur.clear();
	TabIDPage.clear();
	ListeImages.clear();
	ListeLegendes.clear();
	ListeCredits.clear();
	ListeLegendesSecours.clear();
	ListeCreditsSecours.clear();

#if INCOPY == 0
	ListPages.push_back(Utils<IXPageUtils>()->GetPageID(docRef, 0L));
#endif	

#if INCOPY == 0
	//export des legendes et des credits
	this->ExportLegendesCredits(db, theDoc);
#endif
	this->ReorderPictures(db, theDoc);

	this->ExportInCopyStories(db, theDoc, signalMgr);
#if INCOPY == 0
	this->ExportPlacedStories(db, theDoc);
#endif	
	PMString from, result;

#if INCOPY
	from = "incopy";
#else
	from = "indesign";
#endif

	if (!baseHTTP->SaveStories_v2(xpgPrefs->GetTEC_URL(), tabIDPages, from, ListPages, TabIDStories, TabTypes, TabNbSignes, IDStoriesByBloc, TabX, TabY, TabLargeur, TabHauteur, TabIDPage, ListeImages, ListeLegendes, ListeCredits, result))
	{
		if (!baseHTTP->SaveStories_v2(xpgPrefs->GetTEC_URL(), tabIDPages, from, ListPages, TabIDStories, TabTypes, TabNbSignes, IDStoriesByBloc, TabX, TabY, TabLargeur, TabHauteur, TabIDPage, ListeImages, ListeLegendesSecours, ListeCreditsSecours, result))
		{
			CAlert::InformationAlert("Impossible de sauvegarder les articles dans gaia");
		}

	}
}

//-----------------------------------------------------------
//Export des articles java
//-----------------------------------------------------------
void XPGExportArtResp::ExportLegendesCredits(IDataBase* db, IDocument* theDoc)
{
	PMString fileName;
	bool16 creditFound, legendFound;
	std::map<PMString, UIDList>::iterator iter;
	std::map<PMString, UIDList> formeDataList = Utils<IXPageUtils>()->GetFormeDataList(theDoc);
	for (iter = formeDataList.begin(); iter != formeDataList.end(); ++iter) {
		UIDList formeItems = iter->second;
		int32 nbItems = formeItems.Length();
		for (int32 j = 0; j < nbItems; ++j) {
			InterfacePtr<IFormeData> itemPictureData(formeItems.GetRef(j), UseDefaultIID());
			if (itemPictureData && itemPictureData->GetType() == IFormeData::kPhoto) {
				//Si on arrive ici, c'est que c'est un bloc image qui fait partie d'une forme
				//on va maintenant verifier si il y a une image dans le bloc
				fileName = this->GetPictureFileName(db, formeItems.GetRef(j).GetUID());
				if (fileName != "") {
					creditFound = kFalse;
					legendFound = kFalse;
					//il y a bien une image dans le bloc
					ListeImages.push_back(fileName);
					//on doit maintenant recuperer le bloc legende et le bloc credit
					//on part du principe que les deux sont sur la meme page que la photo
					for (int32 k = 0; k < nbItems; ++k) {
						InterfacePtr<IFormeData> itemBlocsData(formeItems.GetRef(k), UseDefaultIID());
						if (itemBlocsData && itemBlocsData->GetUniqueName().Compare(kFalse, itemPictureData->GetUniqueName()) == 0) {
							//si c'est un bloc credit ET qu'il a le meme index que la photo ET qu'on a pas deja un credit pour cette photo
							if (itemBlocsData->GetType() == IFormeData::kCredit && itemBlocsData->GetPhotoIndex() == itemPictureData->GetPhotoIndex()) {
								//on recupere le credit
								ListeCredits.push_back(this->GetBlocContent(db, formeItems.GetRef(k).GetUID()));
								ListeCreditsSecours.push_back(WideString("-"));
								creditFound = kTrue;
							}
							//si c'est un bloc legende ET qu'il a le meme index que la photo ET qu'on a pas deja une legende pour cette photo
							else if (itemBlocsData->GetType() == IFormeData::kLegend && itemBlocsData->GetPhotoIndex() == itemPictureData->GetPhotoIndex()) {
								//on recupere la legende
								ListeLegendes.push_back(this->GetBlocContent(db, formeItems.GetRef(k).GetUID()));
								ListeLegendesSecours.push_back(WideString("-"));
								legendFound = kTrue;
							}
						}
					}
					if (!legendFound) {
						ListeLegendes.push_back(WideString("-"));
						ListeLegendesSecours.push_back(WideString("-"));
					}
					if (!creditFound) {
						ListeCredits.push_back(WideString("-"));
						ListeCreditsSecours.push_back(WideString("-"));
					}
				}
			}
		}
	}

}

//-----------------------------------------------------------
//Réorganise les images
//-----------------------------------------------------------
void XPGExportArtResp::ReorderPictures(IDataBase* db, IDocument* theDoc)
{
	PMString fileName;
	std::map<PMString, UIDRef>::iterator iter;
	std::map<PMString, UIDList> formeDataList = Utils<IXPageUtils>()->GetFormeDataList(theDoc);
	std::map<PMString, UIDRef> formeDataListSorted;

	K2Vector<KeyValuePair <PMString, K2Pair <PMString, UIDList> > > placedStories = Utils<IXPageUtils>()->GetPlacedStoriesList(theDoc);
	/*  for (int32 i = 0; i < placedStories.size(); i++) {
		  PMString artPath = placedStories[i].Value().first;
		  UIDList articleStories = placedStories[i].Value().second;

		  for (int32 j = 0; j < articleStories.size(); ++j) {
			  InterfacePtr<IFormeData> itemPictureData(articleStories.GetRef(j), UseDefaultIID());
			  PMString formeName = itemPictureData->GetUniqueName();
			  PMString  timestamp = formeName;
			  int32 index2 = formeName.LastIndexOfString("_");
			  if (index2 > 0) {
				  timestamp.Remove(0, index2 + 1);
			  }
			  formeDataListSorted.insert(std::make_pair(timestamp, articleStories.GetRef(j)));
		  }
	  }*/


	  //on boucle sur chaque article
	for (int32 i = 0; i < placedStories.size(); i++) {
		int32 compteur = 1000;
		UIDList articleStories = placedStories[i].Value().second;

		//Boucle sur chaque bloc de l'article
		for (int32 j = 0; j < articleStories.size(); j++) {
			InterfacePtr<IFormeData> itemPictureData(articleStories.GetRef(j), UseDefaultIID());
			//Si c'est une image
			if (itemPictureData && itemPictureData->GetType() == IFormeData::kPhoto) {
				//on recupere le nom de la forme + l'index de l'image
				PMString NomFormeImage = itemPictureData->GetUniqueName();
				int32 indexDeLaPhoto = itemPictureData->GetPhotoIndex();
				//on stocke le nouvel index de l'image (1000 et quelques)
				itemPictureData->SetPhotoIndex(compteur);

				//on reboucle pour aller chercher le credit/legende associe a cette image
				for (int32 k = 0; k < articleStories.size(); k++) {
					InterfacePtr<IFormeData> newbloc(articleStories.GetRef(k), UseDefaultIID());
					//Si c'est un credit ou une legende
					if (newbloc && (newbloc->GetType() == IFormeData::kCredit || newbloc->GetType() == IFormeData::kLegend)) {
						int32 indexDuBloc = newbloc->GetPhotoIndex();
						PMString NomFormeBloc = newbloc->GetUniqueName();
						//Si l'index/nom de forme est identique a la photo
						if ((indexDuBloc == indexDeLaPhoto) && (NomFormeBloc.Compare(kFalse, NomFormeImage) == 0)) {
							//on stocke le nouvel index
							newbloc->SetPhotoIndex(compteur);
						}
					}
				}
				compteur++;
			}
		}
	}
	//deuxieme boucle pour remettre les index en partant de 0
	for (int32 i = 0; i < placedStories.size(); i++) {
		UIDList articleStories = placedStories[i].Value().second;

		//Boucle sur chaque bloc de l'article
		for (int32 j = 0; j < articleStories.size(); j++) {
			InterfacePtr<IFormeData> itemData(articleStories.GetRef(j), UseDefaultIID());
			//Si c'est une image, un credit ou une legende
			if (itemData && (itemData->GetType() == IFormeData::kPhoto || itemData->GetType() == IFormeData::kCredit || itemData->GetType() == IFormeData::kLegend)) {
				//on recupere son index
				int32 index = itemData->GetPhotoIndex();
				//Si il est superieur a 1000
				if ((index >= 1000) && (index < 10000)) {
					//on le remet moins haut
					itemData->SetPhotoIndex(index - 1000);
				}
				else if (index < 1000) {
					itemData->SetPhotoIndex(index + 10000);
				}
			}
		}
	}

}

//-----------------------------------------------------------
//Export des articles InCopy
//-----------------------------------------------------------
void XPGExportArtResp::ExportInCopyStories(IDataBase* db, IDocument* theDoc, ISignalMgr* signalMgr) {
	do {
		UIDList exportableItems(db);
		// Determine the range of content spread layer within each spread's children 				
		InterfacePtr<ILayerList> layerList(theDoc, UseDefaultIID());
		InterfacePtr<IGuidePrefs> guidePrefs(theDoc->GetDocWorkSpace(), UseDefaultIID());
		int32 nbLayers = layerList->GetCount();
		int32 startContentLayer, endContentLayer;
		InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());

		if (guidePrefs && guidePrefs->GetGuidesInBack()) {
			startContentLayer = nbLayers + 1;
			endContentLayer = (2 * nbLayers) - 1;
		}
		else {
			startContentLayer = 2;
			endContentLayer = nbLayers;
		}
		// On one hand we gather all items grouped by their article ID
		// On the other hand we gather all stories to export grouped by their article ID
		K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > > exportableStories;
		K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > > itemsByArticle;
		UIDList alreadyAddedStories(db); // Needed because in InCopy only, text stories appears twice (??)

		// Go through each spline item in the document						
		InterfacePtr<ISpreadList> spreadList(theDoc, UseDefaultIID());
		int32 nbSpreads = spreadList->GetSpreadCount();

		for (int32 i = 0; i < nbSpreads; ++i) {
			InterfacePtr<IHierarchy> spreadHier(db, spreadList->GetNthSpreadUID(i), UseDefaultIID());
			if (spreadHier == nil)
				continue;
			// Iterator through spread layers (1st z-order level)		
			for (int32 j = startContentLayer; j <= endContentLayer; ++j) {
				InterfacePtr<IHierarchy> spreadLayerHier(db, spreadHier->GetChildUID(j), UseDefaultIID());
				if (spreadLayerHier == nil)
					continue;
				int32 nbItems = spreadLayerHier->GetChildCount();
				// Iterate through childs (the page items) of the spread layer (2nd z-order level)
				for (int32 itemIndex = 0; itemIndex < nbItems; ++itemIndex) {
					InterfacePtr<IHierarchy> itemHier(db, spreadLayerHier->GetChildUID(itemIndex), UseDefaultIID());
					HandleItem(itemHier, exportableStories, itemsByArticle, alreadyAddedStories);
					if (itemHier) {

						UIDRef itemRef = ::GetUIDRef(itemHier);
						exportableItems.Append(itemRef);
					}
				}
			}
		}

		PMString::ConversionError cError = PMString::kNoError;

		// Export each article in XML
		for (int32 i = 0; i < exportableStories.size(); ++i) {
			PMString artID = exportableStories[i].Key();
			PMString artPath = exportableStories[i].Value().first;
			UIDList articleStories = exportableStories[i].Value().second;

			//on ajoute l'id d'article au tableau
			TabIDStories.push_back(artID.GetAsNumber(&cError));
			//c'est un article incopy
			TabTypes.push_back(1);

			// Si l'article n'est pas a jour par rapport a InCopy, 
			// on ne genere ni la vignette ni l'XML asscies a l'article

#if !INCOPY
			if (this->CheckResourceInCopy(articleStories)) {
				continue;
			}
#endif

			//export des legendes et des credits
			IDFile tempFile;
			FileUtils::GetParentDirectory(FileUtils::PMStringToSysFile(artPath), tempFile);
			artPath = FileUtils::SysFileToPMString(tempFile);
			if (!this->DoExport(artID, artPath, articleStories)) {
				continue;
			}

		}
		// Create snippet for each article
		for (int32 i = 0; i < itemsByArticle.size(); ++i) {

			PMString artID = itemsByArticle[i].Key();
			PMString artPath = itemsByArticle[i].Value().first;
			UIDList  articleItems = itemsByArticle[i].Value().second;

#if !INCOPY
			/*if (this->CheckResourceInCopy(articleItems)) {
				TabNbSignes.push_back(-1);
				continue;
			}*/
#endif

			// si le fichier contient "-", on supprime le versionning 
			if (artID.Contains("-"))
			{
				int32 extPos = artID.LastIndexOfCharacter(PlatformChar('-'));
				if (extPos != -1)
					artID.Remove(extPos, artID.NumUTF16TextChars() - extPos);
			}

			UIDList spreadList2(db);
			for (int32 k = 0; k < articleItems.Length(); ++k)
			{
				InterfacePtr<IHierarchy> itemHier(db, articleItems.At(k), UseDefaultIID());
				if (itemHier)
				{
					InterfacePtr<ISpread> spread(db, itemHier->GetSpreadUID(), UseDefaultIID());
					if (spread && spreadList2.DoesNotContain(itemHier->GetSpreadUID()))
					{
						spreadList2.Append(itemHier->GetSpreadUID());
					}
				}
			}

			if (spreadList2.size() <= 1)
			{
				IDFile snippetFile;
				FileUtils::GetParentDirectory(FileUtils::PMStringToSysFile(artPath), snippetFile);
				//if (FileUtils::IsDirectory(snippetFile)) {
					FileUtils::AppendPath(&snippetFile, artID + "fromind.idms");
                    if (FileUtils::DoesFileExist(snippetFile)) {
                        snippetFile.Delete();
                    }

					InterfacePtr<IPMStream> snippetStream(StreamUtil::CreateFileStreamWrite(snippetFile, kOpenOut | kOpenTrunc, 'ICx4', 'InCp'));

				//}
                /*else {
                    if (FileUtils::DoesFileExist(snippetFile)) {
                        snippetFile.Delete();
                    }
                    InterfacePtr<IPMStream> snippetStream(StreamUtil::CreateFileStreamWrite(snippetFile, kOpenOut | kOpenTrunc, 'ICx4', 'InCp'));
                }*/
				if (xpgPrefs->GetIDMSALLBLOCS() == 1) {
					if (Utils<ISnippetExport>()->ExportPageitems(snippetStream, exportableItems) != kSuccess) {
						snippetStream->Close();
					}
				}
				else {
					if (Utils<ISnippetExport>()->ExportPageitems(snippetStream, articleItems) != kSuccess) {
						snippetStream->Close();
					}
				}
			}
#if !INCOPY
			RecupCoordBlocs(db, artID, articleItems);
#endif
			//recuperation du nombre de signes de l'article
			int32 nbSignes = Utils<IXPageUtils>()->GetNbSignes(articleItems, db);
			TabNbSignes.push_back(nbSignes);
		}
	} while (kFalse);

}


//-----------------------------------------------------------
//Export des articles java
//-----------------------------------------------------------
void XPGExportArtResp::ExportPlacedStories(IDataBase* db, IDocument* theDoc) {

	K2Vector<KeyValuePair <PMString, K2Pair <PMString, UIDList> > > placedStories = Utils<IXPageUtils>()->GetPlacedStoriesList(theDoc);
	InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());

	for (int32 i = 0; i < placedStories.size(); i++) {
		PMString::ConversionError cError = PMString::kNoError;
		PMString artID = placedStories[i].Key();
		//on ajoute l'id d'article au tableau
		TabIDStories.push_back(artID.GetAsNumber(&cError));
		//c'est un article web
		TabTypes.push_back(0);

		PMString artPath = placedStories[i].Value().first;
		UIDList articleStories = placedStories[i].Value().second;


		// Export xml
		UIDList textStories(db);
		for (int32 j = 0; j < articleStories.size(); ++j) {
			InterfacePtr<IFrameType> frameType(db, articleStories[j], UseDefaultIID());
			if (frameType && frameType->IsTextFrame()) {
				InterfacePtr<IHierarchy> itemHier(frameType, UseDefaultIID());
				InterfacePtr<IMultiColumnTextFrame> txtFrame(db, itemHier->GetChildUID(0), UseDefaultIID());
				if (textStories.Contains(txtFrame->GetTextModelUID()))
					// To avoid to create several stories for linked frames which actually consist in one story
					continue;
				else
					textStories.Append(txtFrame->GetTextModelUID());
			}
		}

#if !INCOPY
		UIDList articleToExport(db);
		articleToExport = RecupCoordBlocs(db, artID, articleStories);

		IDFile snippetFile = FileUtils::PMStringToSysFile(artPath);
		if (FileUtils::IsDirectory(snippetFile)) {
			FileUtils::AppendPath(&snippetFile, artID + "fromind.idms");
            if (FileUtils::DoesFileExist(snippetFile)) {
                snippetFile.Delete();
            }
			// Export snippet


			UIDList exportableItems(db);
			do {
				if (theDoc == nil)
					break;

				InterfacePtr<ILayerList> layerList(theDoc, UseDefaultIID());
				InterfacePtr<IGuidePrefs> guidePrefs(theDoc->GetDocWorkSpace(), UseDefaultIID());
				int32 nbLayers = layerList->GetCount();
				int32 startContentLayer, endContentLayer;

				if (guidePrefs && guidePrefs->GetGuidesInBack()) {
					startContentLayer = nbLayers + 1;
					endContentLayer = (2 * nbLayers) - 1;
				}
				else {
					startContentLayer = 2;
					endContentLayer = nbLayers;
				}

				InterfacePtr<ISpreadList> spreadList(theDoc, UseDefaultIID());
				int32 nbSpreads = spreadList->GetSpreadCount();
				for (int32 i = 0; i < nbSpreads; ++i) {

					InterfacePtr<IHierarchy> spreadHier(db, spreadList->GetNthSpreadUID(i), UseDefaultIID());
					if (spreadHier == nil)
						continue;

					for (int32 j = startContentLayer; j <= endContentLayer; ++j) {
						InterfacePtr<IHierarchy> spreadLayerHier(db, spreadHier->GetChildUID(j), UseDefaultIID());
						if (spreadLayerHier == nil)
							continue;

						int32 nbItems = spreadLayerHier->GetChildCount();
						for (int32 itemIndex = 0; itemIndex < nbItems; ++itemIndex) {
							InterfacePtr<IHierarchy> itemHier(db, spreadLayerHier->GetChildUID(itemIndex), UseDefaultIID());
							if (itemHier) {

								UIDRef itemRef = ::GetUIDRef(itemHier);
								exportableItems.Append(itemRef);
							}
						}
					}
				}

			} while (kFalse);

			InterfacePtr<IPMStream> snippetStream(StreamUtil::CreateFileStreamWrite(snippetFile, kOpenOut | kOpenTrunc, 'ICx4', 'InCp'));
			if (xpgPrefs->GetIDMSALLBLOCS() == 1) {
				if (Utils<ISnippetExport>()->ExportPageitems(snippetStream, exportableItems) != kSuccess) {
					CAlert::InformationAlert("XPage - Erreur export IDMS");
					ErrorUtils::PMSetGlobalErrorCode(kSuccess);
				}
			}
			else {
				if (Utils<ISnippetExport>()->ExportPageitems(snippetStream, articleToExport) != kSuccess) {
					CAlert::InformationAlert("XPage - Erreur export IDMS");
					ErrorUtils::PMSetGlobalErrorCode(kSuccess);
				}
			}

			snippetStream->Close();
		}
#endif

		//recuperation du nombre de signes de l'article
		int32 nbSignes = Utils<IXPageUtils>()->GetNbSignes(articleStories, db);
		TabNbSignes.push_back(nbSignes);

		//export des legendes et des credits
		/*if (!this->DoExport(artID, artPath, textStories)) {
			continue;
		}*/
	}
}
//-----------------------------------------------------------
//Verifie si l'article est a jour par rapport a incopy
//-----------------------------------------------------------
bool16 XPGExportArtResp::CheckResourceInCopy(UIDList& articleItems)
{
	bool16 ressourceState = kFalse;
	for (int32 j = 0; j < articleItems.Length(); ++j) {
		// Check if the story is out of Date
		UIDRef itemRef = articleItems.GetRef(j);
		if (!ressourceState)
			ressourceState = Utils<IStoryUtils>()->IsLinkResourceOutOfDate(itemRef);
	}
	return ressourceState;
}

//-----------------------------------------------------------
//Exporte l'article
//-----------------------------------------------------------
bool16 XPGExportArtResp::DoExport(PMString artID, PMString artPath, UIDList& textStories)
{
	// On supprime le versionning des fichiers InCopy
	if (artID.Contains("-")) {
		int32 extPos = artID.LastIndexOfCharacter(PlatformChar('-'));
		if (extPos != -1)
			artID.Remove(extPos, artID.NumUTF16TextChars() - extPos);
	}

	IDFile xmlFile = FileUtils::PMStringToSysFile(artPath);
	if (Utils<IXPageUtils>()->ExportToXml(textStories, xmlFile, artID) != kSuccess) {
		ErrorUtils::PMSetGlobalErrorCode(kSuccess); // Reset global error code
		PMString error(kXPGErrExportXmlFailed);
		error.Translate();
		StringUtils::ReplaceStringParameters(&error, FileUtils::SysFileToPMString(xmlFile));
		CAlert::InformationAlert(error);
		return kFalse;
	}
	else {
		return kTrue;
	}

}
//-----------------------------------------------------------
//recuperation des coordonnees de tous les blocs de l'article
//-----------------------------------------------------------
UIDList XPGExportArtResp::RecupCoordBlocs(IDataBase* db, PMString artID, UIDList& articleItems)
{
	UID ownerPageUID = kInvalidUID;
	PMString posX = kNullString, posY = kNullString, largeur = kNullString, hauteur = kNullString;
	UIDList blocsItems(db);
#if !INCOPY
	for (int32 k = 0; k < articleItems.Length(); ++k) {
		int32 textflags = 0;
		//on ajoute ce bloc a la liste, quoi qu'il arrive
		blocsItems.Append(articleItems[k]);
		InterfacePtr<IFrameType> frameType(db, articleItems[k], UseDefaultIID());
		InterfacePtr<IHierarchy> itemHier(frameType, UseDefaultIID());

		if (Utils<IFrameUtils>()->IsTextFrame(itemHier, &textflags)) {
			//if(frameType && frameType->IsTextFrame()){
				//si on est sur une chaine
			if ((textflags & IFrameUtils::kTF_InLink) || (textflags & IFrameUtils::kTF_OutLink)) {
				// Get MultiColumnItem
				InterfacePtr<IHierarchy> multiColHier(itemHier->QueryChild(0));
				if (multiColHier == nil)
					continue;

				// Iterate through frames
				InterfacePtr<IMultiColumnTextFrame> txtFrame(multiColHier, UseDefaultIID());
				if (txtFrame == nil)
					continue;

				InterfacePtr<IFrameList> frameList(txtFrame->QueryFrameList());
				if (frameList == nil)
					continue;

				int32 nbFrames = frameList->GetFrameCount();
				UID latestSpline = kInvalidUID;
				for (int32 i = 0; i < nbFrames; ++i)
				{
					// Get the first frame from the frame list
					InterfacePtr<ITextFrameColumn> frameItem(frameList->QueryNthFrame(i));
					if (frameItem == nil)
						continue;
					// Get the associated spline UIDRef
					InterfacePtr<IHierarchy> textSpline(Utils<ITextUtils>()->QuerySplineFromTextFrame(frameItem));
					if (textSpline == nil)
						continue;

					InterfacePtr<IMultiColumnTextFrame> linkedText(frameItem->QueryMultiColumnTextFrame());

					if (blocsItems.Contains(::GetUID(textSpline)))
						// To avoid to create several stories for linked frames which actually consist in one story
						continue;
					else {
						blocsItems.Append(::GetUID(textSpline));
					}
				}
			}
		}
	}

	for (int32 i = 0; i < blocsItems.Length(); ++i)
	{
		// reinit var
		posX = posY = largeur = hauteur = "";

		//Coordonnees du bloc
		InterfacePtr<IGeometry> itemGeo(blocsItems.GetRef(i), UseDefaultIID());
		if (itemGeo == nil)
			continue;
		PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();
		PMPoint rightBottom = itemGeo->GetStrokeBoundingBox().RightBottom();
		::TransformInnerPointToPasteboard(itemGeo, &rightBottom);
		::TransformInnerPointToPasteboard(itemGeo, &leftTop);

		InterfacePtr<IHierarchy> leftMostItemHier(db, blocsItems.At(i), UseDefaultIID());
		ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(leftMostItemHier);
		// Recuperartion de l'id de la page a partir de Xrail Client
		if (ownerPageUID == kInvalidUID) // Wrong page index
			continue;

		InterfacePtr<IXRailPageSlugData> readData(db, ownerPageUID, IID_PAGESLUGDATA);
		if (readData == nil)
			continue;
		int32 idPage = readData->GetID();

		// Calcul en millimètres des dimensions
		InterfacePtr<IUnitOfMeasure> uom((IUnitOfMeasure*)::CreateObject(kMillimetersBoss, IID_IUNITOFMEASURE));
		if (uom == nil)
			continue;
		largeur.AppendNumber(uom->PointsToUnits(rightBottom.X() - leftTop.X()), 3, kTrue, kTrue);
		hauteur.AppendNumber(uom->PointsToUnits(rightBottom.Y() - leftTop.Y()), 3, kTrue, kTrue);

		// Calcul de l'origine dans le repere de la page
		InterfacePtr<IHierarchy> hier(db, blocsItems[i], UseDefaultIID());
		if (hier == nil)
			continue;
		ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(hier);
		InterfacePtr<IGeometry> ownerPageGeo(db, ownerPageUID, UseDefaultIID());
		if (ownerPageGeo == nil)
			continue;

		::TransformPasteboardPointToInner(ownerPageGeo, &leftTop);

		posX.AppendNumber(uom->PointsToUnits(leftTop.X()), 3, kTrue, kTrue);
		posY.AppendNumber(uom->PointsToUnits(leftTop.Y()), 3, kTrue, kTrue);

		PMString::ConversionError cError = PMString::kNoError;

		IDStoriesByBloc.push_back(artID.GetAsNumber(&cError));
		TabIDPage.push_back(idPage);
		TabX.push_back(posX);
		TabY.push_back(posY);
		TabHauteur.push_back(hauteur);
		TabLargeur.push_back(largeur);
	}
#endif
	return blocsItems;
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
void XPGExportArtResp::HandleItem(IHierarchy* itemHier,
	K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > >& exportableStories,
	K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > >& itemsByArticle,
	UIDList& alreadyAddedStories)
{

	// Figure the related article ID from the Incopy file path
	InterfacePtr<IAssignment> assignment(nil);
	InterfacePtr<IAssignedStory> assignedStory(nil);
	InterfacePtr<IAssignmentMgr> assignMgr(GetExecutionContextSession(), UseDefaultIID());

	if (Utils<IPageItemTypeUtils>()->IsGroup(itemHier)) {
		// If it's a group, handle children item recursively
		for (int32 i = 0; i < itemHier->GetChildCount(); ++i) {
			InterfacePtr<IHierarchy> childHier(itemHier->QueryChild(i));
			HandleItem(childHier, exportableStories, itemsByArticle, alreadyAddedStories);
		}
	}
	else {
		// Check out item if is an InCopy story
		UIDRef itemRef = ::GetUIDRef(itemHier);
		UIDRef storyRef = itemRef;
		Utils<IAssignmentUtils>()->NormalizeTextModelRef(storyRef);

		PMString artID = kNullString, artPath = kNullString;

		if (Utils<IStoryUtils>()->IsAStory(storyRef)) {
			assignMgr->QueryAssignmentAndAssignedStory(storyRef, assignment, assignedStory);
			if (assignment != nil) {
				IDFile inCopyFile = FileUtils::PMStringToSysFile(assignment->GetFile());
				artID = inCopyFile.GetFileName();
				artID.Truncate(5); // Remove incopy extension	
				artPath = assignment->GetFile();
			}
		}

#if !INCOPY
		else {

			InterfacePtr<IStringListData> selectedArticleID(storyRef, IID_IARTICLEINFO);
			if (selectedArticleID) { // That means the item is a text frame
				// Check wether an article was imported into this frame
				K2Vector<PMString> articleInfo = selectedArticleID->GetStringList();
				if (!articleInfo.empty()) {
					artID = articleInfo[0]; // article ID is the first stored information 
					artPath = articleInfo[1]; // article path is the second stored information 
				}
			}
		}
#endif

		// Add item to the lists
		if (!artID.IsEmpty() && alreadyAddedStories.DoesNotContain(storyRef.GetUID())) {
			alreadyAddedStories.Append(storyRef.GetUID());

			// Add item to the group of items it belongs to
			K2Vector<KeyValuePair <PMString, K2Pair <PMString, UIDList> > >::iterator iter = ::K2find(itemsByArticle, artID);
			if (iter == itemsByArticle.end())
				itemsByArticle.push_back(KeyValuePair <PMString, K2Pair <PMString, UIDList> >(artID, K2Pair<PMString, UIDList>(artPath, UIDList(itemRef))));
			else
				iter->Value().second.Append(itemRef);

			// If it's a text frame, add it too to the exportable stories list
			if (Utils<IPageItemTypeUtils>()->IsTextFrame(itemRef)) {
				Utils<IAssignmentUtils>()->NormalizeTextModelRef(itemRef);
				K2Vector<KeyValuePair <PMString, K2Pair <PMString, UIDList> > >::iterator iterExportable = ::K2find(exportableStories, artID);

				if (iterExportable == exportableStories.end())
					exportableStories.push_back(KeyValuePair<PMString, K2Pair<PMString, UIDList> >(artID, K2Pair<PMString, UIDList>(artPath, UIDList(itemRef))));
				else
					iterExportable->Value().second.Append(itemRef);
			}
		}
	}
}


//-----------------------------------------------------------
// Renvoi le chemin d'acces de l'image du bloc selectionne
//-----------------------------------------------------------
PMString XPGExportArtResp::GetPictureFileName(IDataBase* db, UID curPict)
{
	PMString result = "";
	// On recupere le ILinkManager
	do {
		InterfacePtr<ILinkManager> iLinkManager(db, db->GetRootUID(), UseDefaultIID());
		if (iLinkManager == nil) {
			break;
		}

		InterfacePtr<IHierarchy> resaHier(db, curPict, UseDefaultIID());
		if (resaHier == nil) {
			break;
		}

		InterfacePtr<ILinkObject> iLinkObject(resaHier, UseDefaultIID());
		if (iLinkObject == nil) {
			break;
		}
		ILinkManager::QueryResult linkQueryResult;
		if (iLinkManager && iLinkManager->QueryLinksByObjectUID(resaHier->GetChildUID(0), linkQueryResult))
		{
			ILinkManager::QueryResult::const_iterator iterLinks = linkQueryResult.begin();
			InterfacePtr<ILink> iLink(db, *iterLinks, UseDefaultIID());
			if (iLink == nil)
				break;
			InterfacePtr<ILinkResource> iLinkResource(db, iLink->GetResource(), UseDefaultIID());
			if (iLinkResource == nil)
				break;
			IDFile imageFile;
			URI imageURI = iLinkResource->GetId();
			Utils<IURIUtils>()->URIToIDFile(imageURI, imageFile);
			if (FileUtils::DoesFileExist(imageFile) && !FileUtils::IsDirectory(imageFile))
			{
				result = FileUtils::SysFileToPMString(imageFile);
			}
		}
	} while (kFalse);
	return result;
}

//-----------------------------------------------------------
// Renvoi le chemin d'acces de l'image du bloc selectionne
//-----------------------------------------------------------
WideString XPGExportArtResp::GetBlocContent(IDataBase* db, UID storyRef)
{
	WideString result;
	do {
		// Read story
		InterfacePtr<IHierarchy> itemHier(db, storyRef, UseDefaultIID());
		if (!itemHier)
			break;

		InterfacePtr<IMultiColumnTextFrame> isTextFrame(db, itemHier->GetChildUID(0), UseDefaultIID());
		if (!isTextFrame)
			break;

		InterfacePtr<ITextModel> txtModel(isTextFrame->QueryTextModel());
		if (txtModel == nil)
			break;

		InterfacePtr<ITextStrand> txtStrand(static_cast<ITextStrand*>(txtModel->QueryStrand(kTextDataStrandBoss, IID_ITEXTSTRAND)));

		TextIndex index = 0;

		while (index < txtModel->GetPrimaryStoryThreadSpan()) {
			int32 length;
			DataWrapper<textchar> chunk = txtStrand->FindChunk(index, &length);
			int32 indexChunk = 0;
			int32 endChar = 0;

			while (indexChunk < length) {
				if (TextChar::IsK2SpecificChar(chunk[indexChunk])) {
					//ce caractere pose probleme, et peut potentiellement afficher une alerte plus loin: 
					//impossible de transcoder ce caractere. 
				}
				if (chunk[indexChunk] == kTextChar_Apostrophe) {
					result.Append(WideString("0x0027"));
				}
				else if (chunk[indexChunk] == kTextChar_LeftSingleQuotationMark) {
					result.Append(WideString("0x2018"));
				}
				else if (chunk[indexChunk] == kTextChar_RightSingleQuotationMark) {
					result.Append(WideString("0x2019"));
				}
				else if (chunk[indexChunk] == kTextChar_LeftDoubleQuotationMark) {
					result.Append(WideString("0x201C"));
				}
				else if (chunk[indexChunk] == kTextChar_RightDoubleQuotationMark) {
					result.Append(WideString("0x201D"));
				}
				else if (chunk[indexChunk] == kTextChar_DoubleLow9QuotationMark) {
					result.Append(WideString("0x201E"));
				}
				else if (chunk[indexChunk] == kTextChar_DoubleHighReversed9QuotationMark) {
					result.Append(WideString("0x201F"));
				}
				else if (chunk[indexChunk] == kTextChar_SingleLow9QuotationMark) {
					result.Append(WideString("0x201A"));
				}
				else if (chunk[indexChunk] == kTextChar_SingleHighReversed9QuotationMark) {
					result.Append(WideString("0x201B"));
				}
				else if (TextChar::IsK2SpecificChar(chunk[indexChunk])) {
					//caractere pose probleme, et peut potentiellement afficher une alerte plus loin: 
					//impossible de transcoder ce caractere. 
				}
				else {
					result.Append(chunk[indexChunk]);
				}
				++indexChunk;
			}
			index += length;
		}
	}

	while (kFalse);
	return result;
}
