//========================================================================================
//  
//  $File: //depot/indesign_3.0/gm/source/sdksamples/docwatch/DocWchResponder.cpp $
//  
//  Owner: Adobe Developer Technologies
//  
//  $Author: pmbuilder $
//  
//  $DateTime: 2003/09/30 15:41:37 $
//  
//  $Revision: #1 $
//  
//  $Change: 223184 $
//  
//  Copyright 1997-2003 Adobe Systems Incorporated. All rights reserved.
//  
//  NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance 
//  with the terms of the Adobe license agreement accompanying it.  If you have received
//  this file from a source other than Adobe, then your use, modification, or 
//  distribution of it requires the prior written permission of Adobe.
//  
//========================================================================================

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IClassIDData.h"
#include "ISignalMgr.h"
#include "IDocumentSignalData.h"
#include "IUIDData.h"
#include "IDialog.h"
#include "CAlert.h"
#include "ISession.h"
#include "IUIDRefListData.h"
#include "IWebServices.h"
#include "IBoolData.h"
#include "IResaPubList.h"
#include "IPageList.h"
#include "INewLayerCmdData.h"
#include "ILayerList.h"
#include "IUnitOfMeasure.h"
#include "IGeometry.h"
#include "ISpread.h"
#include "IHierarchy.h"
#include "IXRailPageSlugData.h"
#include "IDocumentLayer.h"
#include "ISpreadLayer.h"
#include "SDKLayoutHelper.h"
#include "IStoryList.h"
#include "ITextModel.h"
#include "IWaxStrand.h"
#include "IWaxIterator.h"
#include "IWaxGlyphs.h"
#include "ITextStrand.h"
#include "IAttributeStrand.h"
#include "IItemStrand.h"
#include "IStyleInfo.h"
#include "IXMLStreamUtils.h"
#include "IXMLOutStream.h"
#include "ILinkObject.h"
#include "ILinkManager.h"
#include "IURIUtils.h"
#include "URI.h"
#include "ILayerUtils.h"
#include "IIdleTask.h"
#include "IPreflightFacade.h"
#include "IPreflightManager.h"
#include "IXRailPrefsData.h"
#include "IWorkspace.h"
#include "IScriptLabel.h"  // template-block marker (XRAIL_AD_FROM_TEMPLATE)
#include "ISpreadList.h"
#include "IGuidePrefs.h"
#include "ILayoutUtils.h"
#include "IStringListData.h"
#include "IMasterSpreadList.h"
#include "IMasterPage.h"
#include "IPlacedArticleData.h"  // XPage cross-plugin : marqueur d'appartenance article Gaia

// Implementation includes:
#include "CreateObject.h"
#include "CResponder.h"
#include "DocUtils.h"
#include "FileUtils.h"
#include "CoreResTypes.h"
#include "PMString.h"
#include "IStringData.h"
#include "ErrorUtils.h"
#include "TransformUtils.h"
#include "IDataLinkReference.h"
#include "IDataLink.h"
#include "IDocument.h"
#include "Utils.h"
#include "ILayoutUIUtils.h"
#include "IWindow.h"
#include "IDocumentUtils.h"

// Project includes
#include "XRLID.h"
#include "XRLUIID.h"
#include "XRCID.h"
#include "TextID.h"
#include "PageItemScrapID.h"
#include "IInCopyBridgeUtils.h"
#include "IFrameList.h"
#include "UIDList.h"

#include <vector>

/** XRailResponder
Handles signals related to document file actions.  The file action
signals it receives are dictated by the XRailServiceProvider class.

XRailResponder implements IResponder based on
the partial implementation CResponder.


@ingroup docwatch
@author John Hake
*/
class XRailResponder : public CResponder
{
public:

	/**
	Constructor.
	@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRailResponder(IPMUnknown* boss);

	/**
	Respond() handles the file action signals when they
	are dispatched by the signal manager.  This implementation
	simply creates alerts to display each signal.

	@param signalMgr Pointer back to the signal manager to get
	additional information about the signal.
	*/
	virtual void Respond(ISignalMgr* signalMgr);

private:

	bool Checkpreflight(UIDRef doc);

	/**
	Get a list of reserves from XRail server, and place them into the document.
	Removed reserves that may be not associated with the parution anymore.
	*/
	void ImportPublicites(UIDRef doc, const K2Vector<int32>& tabIDPages, PMString baseAdress);

	/**
	Send reserves positions back to XRail server
	*/
	void MAJPublicites(UIDRef doc, PMString serverAddress);

	/**
	 Send preview to XRail server
	 */
	void DoMakePreviewToServer(UIDRef doc);

	/**
	 Remonte tous les blocs de la maquette qui n'ont PAS d'ID Gaia, c'est a dire
	 ceux qui ne sont ni des pubs Gaia (absents de IResaPubList) ni des blocs
	 faisant partie d'un article Gaia (sans IPlacedArticleData ni IID_IARTICLEINFO).
	 Pour chaque bloc retenu on remplit des tableaux paralleles : id de page, X, Y,
	 largeur, hauteur (en millimetres, dans le repere de la page).
	 */
	void GetBlocsSansIDGaia(UIDRef doc, K2Vector<int32>& blocPageIDs, K2Vector<PMString>& blocX,
		K2Vector<PMString>& blocY, K2Vector<PMString>& blocLargeur, K2Vector<PMString>& blocHauteur);

};


/* CREATE_PMINTERFACE
Binds the C++ implementation class onto its ImplementationID
making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XRailResponder, kXRLResponderImpl)

/* XRailResponder Constructor
*/
XRailResponder::XRailResponder(IPMUnknown* boss) :
	CResponder(boss)
{
}

/* Respond
*/
void XRailResponder::Respond(ISignalMgr* signalMgr)
{
	// Get the service ID from the signal manager
	ServiceID serviceTrigger = signalMgr->GetServiceID();

	// Get a UIDRef for the document.  It will be an invalid UIDRef
	// for BeforeNewDoc, BeforeOpenDoc, AfterSaveACopy, and AfterCloseDoc because
	// the document doesn't exist at that point.
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	if (docData == nil)
	{
		ASSERT_FAIL("Invalid IDocumentSignalData* - XRailResponder::Respond");
		return;
	}
	UIDRef doc = docData->GetDocument();

	// Get userID to interact with XRail server
	InterfacePtr<IStringData> sessionID(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);
	PMString userID = sessionID->GetString();
	bool16 isReadOnly = kFalse;
	PMString urlXR, lastUser;
	bool16 modeRobot;
	K2Vector<PMString> listeBases;
	K2Vector<PMString> IPBases;
	PMString baseAdress;
	PMString serverAddress;

	GetPrefs(urlXR, lastUser, modeRobot, listeBases, IPBases);
	InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
	InterfacePtr<IXRailPrefsData>  xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));


	if (!modeRobot) {
		// Get userID to interact with XRail
		userID = sessionID->GetString();
		//si pas de login, on ne fait rien (cas des gens qui ne se sont pas loggué XRail au demarrage d'InDesign)
		if (userID == "") {
			return;
		}
	}

	InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

	// Take action based on the service ID
	switch (serviceTrigger.Get())
	{
	case kDuringOpenDocSignalResponderService: // Lock pages so that no other user can edit them at the same time			
	{
		InterfacePtr<IDocument> theDoc(doc, UseDefaultIID());
		if (theDoc->IsReadOnly()) {
			isReadOnly = kTrue;
			if (!modeRobot) {
				CAlert::InformationAlert(kXRLDocumentReadOnly);
			}
		}
		else {
			int32 nbPages = GetNbPages(doc);
			K2Vector<int32> tabIDPages;
			for (int16 i = 0; i < nbPages; i++) {
				int32 idPage = GetPageID(doc, i, baseAdress);
				if (idPage > 0) {
					tabIDPages.push_back(idPage);
				}
			}

			serverAddress = GetGoodUrlXR(baseAdress);


			if (!modeRobot) {
				if (tabIDPages.size() > 0) {
					PMString pageStatus;
					bool connexionOK = xrailHTTP->SetVerrou_v2(serverAddress, tabIDPages, kTrue, userID, pageStatus, kFalse);
					if (!connexionOK) {
						CAlert::ErrorAlert(PMString(kXRLErrorWSLockPageKey));
						break;
					}
				}
			}
			if (xrailPrefsData->GetImportPub() == 1) {
				if (tabIDPages.size() == nbPages) // we only handle pub if all page IDs are set correctly 
					ImportPublicites(doc, tabIDPages, serverAddress);
			}
		}
		break;
	}

	case kAfterSaveAsDocSignalResponderService:
	case kAfterSaveDocSignalResponderService:
	{
		this->DoMakePreviewToServer(doc);
		break;
	}

	case kBeforeSaveDocSignalResponderService:
	{

		if (xrailPrefsData->GetImportPub() == 1) {
			int32 nbPages = GetNbPages(doc);
			int32 nbXRailPages = 0;
			for (int16 i = 0; i < nbPages; i++) {
				if (GetPageID(doc, i, baseAdress) > 0)
					++nbXRailPages;
			}
			serverAddress = GetGoodUrlXR(baseAdress);

			if (nbXRailPages == nbPages) // we only handle pub if all page IDs are set correctly 
				MAJPublicites(doc, serverAddress); // We sent back to the server the reserves coordinates
		}
		break;
	}

	case kBeforeCloseDocSignalResponderService:
	{
		// First, unlock pages
		int32 nbPages = GetNbPages(doc);
		bool16 showDlg = kFalse;
		PMString pageStatus;
		K2Vector<int32> listPagesIDS;
		bool16 erreurssurledoc = kFalse;
		if (!modeRobot) {
			erreurssurledoc = Checkpreflight(doc);
			for (int32 i = 0; i < nbPages; ++i) {
				int32 idPage = GetPageID(doc, i, baseAdress);
				if (idPage > 0) {
					//on remplit une liste avec tous les ids de chaque page
					listPagesIDS.push_back(idPage);

				}
			}
			serverAddress = GetGoodUrlXR(baseAdress);

			if (listPagesIDS.size() > 0) {
				bool connexionOK = xrailHTTP->SetVerrou_v2(serverAddress, listPagesIDS, kFalse, userID, pageStatus, erreurssurledoc);
				if (!connexionOK) {
					CAlert::ErrorAlert(PMString(kXRLErrorWSLockPageKey));
				}
				else {
					showDlg = kTrue;
				}
			}
		}

		if (showDlg && !modeRobot && !isReadOnly) {
			//dans tous les cas, en mode normal (non robot) et si il y a des IDS, on demande a l'utilisateur de changer l'etat des pages
			IDialog* dialog = CreateNewDialog(kXRLUIPluginID, kXRLUIChangeStateDialogResourceID);
			if (!dialog)
				break;

			// Get doc Name 
			//IDocument * frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
			PMString docName;
			InterfacePtr<IDocument> theDoc(docData->GetDocument(), UseDefaultIID());
			if (theDoc == nil) {
				break;
			}
			theDoc->GetName(docName);

			// Set Title Window
			InterfacePtr<IWindow> window(dialog, UseDefaultIID());
			PMString titleWindow(kXRLUIChangeStateDialogTitleKey, PMString::kTranslateDuringCall);
			titleWindow.Append(docName);
			titleWindow.SetTranslatable(kFalse);
			window->SetTitle(titleWindow);

			// Set dialog parameter : UIDRef of the document and page status
			InterfacePtr<IUIDData> dialogParameter1(dialog->GetDialogPanel(), UseDefaultIID());
			dialogParameter1->Set(doc);

			InterfacePtr<IStringData> dialogParameter2(dialog->GetDialogPanel(), UseDefaultIID());
			dialogParameter2->Set(pageStatus);

			InterfacePtr<IBoolData> dialogParameter3(dialog->GetDialogPanel(), UseDefaultIID());
			dialogParameter3->Set(erreurssurledoc);

			InterfacePtr<IIdleTask> closeDialogTask(GetExecutionContextSession(), IID_IAUTOCLOSINGDIALOG);
			if (closeDialogTask) {
				int32 execInterval = 15 * 1000;
				closeDialogTask->InstallTask(execInterval);
			}
			// Open the dialog.
			dialog->Open(nil, false);
			dialog->WaitForDialog(kTrue);
			if (closeDialogTask) {
				closeDialogTask->UninstallTask();
			}
		}
	}
	break;
	default:
		break;
	}
}

bool XRailResponder::Checkpreflight(UIDRef docUIDRef)
{
	if (Utils<Facade::IPreflightFacade>()->IsPreflightingOn(docUIDRef.GetDataBase())) {
		bool dataAvail = Utils<Facade::IPreflightFacade>()->ArePreflightResultsAvailable(docUIDRef.GetDataBase());
		if (dataAvail) {
			ScriptListData results;
			Utils<Facade::IPreflightFacade>()->GetPreflightResults(docUIDRef.GetDataBase(), results);
			if (results.size() > 0) {
				ScriptListData listData;
				ScriptData data = results[2];
				data.GetList(listData);
				if (listData.size() > 0) {
					return kTrue;
				}
			}
		}
	}

	return kFalse;
}

void XRailResponder::ImportPublicites(UIDRef doc, const K2Vector<int32>& IDPages, PMString baseAdress) {

	do {
		InterfacePtr<IDocument> frontDoc(doc, UseDefaultIID());

		// Get latest list of resa pub registered in the document
		InterfacePtr<IResaPubList> resaPubListData(doc, IID_IRESAPUBLIST);
		if (resaPubListData == nil) {
			break;
		}
		K2Vector<ResaPub> currentResaPubList = resaPubListData->GetResaPubList();
		IDataBase* db = doc.GetDataBase();
		if (db == nil) {
			break;
		}
		InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
		if (xrailHTTP == nil) {
			break;
		}

		// Get list of pubs from xrail
		PMString baseName, serverAdress;
		int32 idPage = GetPageID(doc, 0, baseName);
		serverAdress = GetGoodUrlXR(baseName);


		K2Vector<int32> TabFolios, TabPageID;
		K2Vector<PMString> TabResaIDS, TabPubsIDS, TabAnnonceurs, TabCheminImage;
		K2Vector<PMReal> TabHauteurs, TabLargeurs, TabX, TabY;

		K2Vector<bool16> TabVerrouPos;
		bool connexionOK = xrailHTTP->GetListePubs_v2(serverAdress, IDPages, TabPageID, TabFolios, TabResaIDS, TabPubsIDS, TabHauteurs, TabLargeurs, TabX, TabY, TabAnnonceurs, TabCheminImage, TabVerrouPos);
		if (!connexionOK) {
			//CAlert::ErrorAlert(PMString(kXRLErrorWebServiceKey));
			break;
		}
		int32 nbResaPub = TabPubsIDS.size();
		// Get the list of pages
		InterfacePtr<IPageList> pageList(doc, UseDefaultIID());
		int32 nbPages = pageList->GetPageCount();
		// Create converter (resa dimensions are sent in millimeters)
		InterfacePtr<IUnitOfMeasure> uom((IUnitOfMeasure*)::CreateObject(kMillimetersBoss, IID_IUNITOFMEASURE));
		if (uom == nil)
		{
			break;
		}
		// First step, we create resa that XRail sent
		K2Vector<ResaPub> newResaPubList;
		UIDList pubItemsToDelete(db);
		for (int32 i = 0; i < nbResaPub; ++i) {
			ResaPub aPub;
			aPub.resaID = TabResaIDS[i];

			PMString resaAnnonceur = TabAnnonceurs[i];

			// First, check whether pub is already created in the document or not
			bool16 placed = kTrue;
			K2Vector<ResaPub>::iterator iter = ::K2find(currentResaPubList.begin(), currentResaPubList.end(), aPub);
			if (iter != currentResaPubList.end())
			{
				// Pub is already placed, check whether it's owner page corresponds to XRail data
				InterfacePtr<IHierarchy> pubHier(db, iter->resaUID, UseDefaultIID());
				if (!pubHier) // Pub has been deleted, re-place it
					placed = kFalse;
				else
				{
					InterfacePtr<IXRailPageSlugData> pageSlug(db, Utils<ILayoutUtils>()->GetOwnerPageUID(pubHier), IID_PAGESLUGDATA);
					if (!pageSlug || pageSlug->GetID() != TabPageID[i]) // Pub's page doesn't match XRail data, delete it, then recreate it at the right place
					{
						placed = kFalse;
						pubItemsToDelete.Append(iter->resaUID);
					}
					else
					{
						aPub = *iter;
					}
				}
				currentResaPubList.erase(iter);
			}
			else
				placed = kFalse;

			// Update pub ID, since it may have changed in the meantime
			aPub.pubID = TabPubsIDS[i];

			// Set when an IDMS template was successfully imported for this
			// pub during the !placed branch below — the post-creation
			// ConvertToResaPub / ImportPubFile section then needs to skip
			// any block that would overwrite the template's styling.
			//
			// Persisted across sessions via a script label
			// "XRAIL_AD_FROM_TEMPLATE" written on the page item: at every
			// reopen we read the label so a block created from a template
			// in a previous session is also recognised (otherwise
			// ConvertToResaPub would happily overwrite it on the second
			// run when placed=true).
			//
			// Label values:
			//   "1"          -> no-visual template
			//   "withVisual" -> with-visual template (image still placed
			//                   in-frame by ImportPubFile)
			bool16 templateUsed         = kFalse;
			bool16 templateIsWithVisual = kFalse;

			static const char* const kAdTemplateLabelKey = "XRAIL_AD_FROM_TEMPLATE";

			if (placed && aPub.resaUID != kInvalidUID)
			{
				InterfacePtr<IScriptLabel> labelData(UIDRef(db, aPub.resaUID), IID_ISCRIPTLABEL);
				if (labelData != nil)
				{
					PMString labelKey(kAdTemplateLabelKey, PMString::kUnknownEncoding);
					labelKey.SetTranslatable(kFalse);
					const PMString labelValue = labelData->GetTag(labelKey);
					if (!labelValue.IsEmpty())
					{
						templateUsed = kTrue;
						templateIsWithVisual = (labelValue == "withVisual");
					}
				}
			}

			if (!placed) // Pub not placed yet, create it
			{
				// Get the page where the item for the resa must be created
				UID pageUID = kInvalidUID;
				for (int16 j = 0; j < nbPages; j++)
				{
					aPub.pageID = GetPageID(doc, j, baseName);
					if (aPub.pageID == TabPageID[i])
					{
						pageUID = pageList->GetNthPageUID(j);
						break;
					}
				}

				if (pageUID == kInvalidUID) // page doesn't exist in this document
				{
					CAlert::ErrorAlert(kXRLErrorInvalidResaPageKey);
					continue;
				}

				// Compute resa coordinates in pasteboard coordinates			
				PMPoint leftTop(uom->UnitsToPoints(TabX[i]), uom->UnitsToPoints(TabY[i]));
				InterfacePtr<IGeometry> ownerPageGeo(UIDRef(db, pageUID), UseDefaultIID());
				::TransformInnerPointToPasteboard(ownerPageGeo, &leftTop);

				// Get owner spread layer
				InterfacePtr<IHierarchy> pageHier(ownerPageGeo, UseDefaultIID());
				InterfacePtr<ISpread> ownerSpread(db, pageHier->GetSpreadUID(), UseDefaultIID());
				InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(frontDoc));
				InterfacePtr<ISpreadLayer> ownerSpreadLayer(ownerSpread->QueryLayer(activeLayer));
				//InterfacePtr<ISpreadLayer> ownerSpreadLayer (ownerSpread->QueryLayer(ownerLayer));

				// Try template-driven creation first. The two IDMS template
				// paths come from Gaia prefs (refreshed at login). When either
				// or both are configured we use the matching template instead
				// of the hard-coded creation path:
				//   * pub has a visual reachable on disk + with-visual template
				//     present  -> import the with-visual template
				//   * otherwise (no visual, or visual unreachable, or
				//     with-visual template missing)                  -> import the
				//     no-visual template (with placeholder substitution)
				// Any failure (template missing, file unreadable, import
				// produces 0 or >1 top-level frames, geometry op fails) falls
				// through to the legacy CreerResaPub call below.
				{
					// `workspace` is declared in Respond() but not in this
					// function (ImportPublicites is called by Respond), so
					// re-resolve it here.
					InterfacePtr<IWorkspace> wsForTmpl(GetExecutionContextSession()->QueryWorkspace());
					InterfacePtr<IXRailPrefsData> xrailPrefsData2(wsForTmpl, IID_IXRAILPREFSDATA);
					PMString tmplNoVisual  = xrailPrefsData2 ? xrailPrefsData2->GetAdTemplateNoVisual()  : PMString();
					PMString tmplWithVisual = xrailPrefsData2 ? xrailPrefsData2->GetAdTemplateWithVisual() : PMString();

					const PMString& pubFilePathRef = TabCheminImage[i];
					IDFile pubImageFile = (pubFilePathRef != kNullString) ? FileUtils::PMStringToSysFile(pubFilePathRef) : IDFile();
					const bool16 visualReachable = (pubFilePathRef != kNullString)
						&& FileUtils::DoesFileExist(pubImageFile)
						&& !FileUtils::IsDirectory(pubImageFile);

					PMString chosenTmpl = (visualReachable && !tmplWithVisual.IsEmpty()) ? tmplWithVisual : tmplNoVisual;

					if (!chosenTmpl.IsEmpty())
					{
						IDFile tmplFile = FileUtils::PMStringToSysFile(chosenTmpl);
						if (FileUtils::DoesFileExist(tmplFile) && !FileUtils::IsDirectory(tmplFile))
						{
							UID newUID = kInvalidUID;
							if (ImportAdTemplateBlock(::GetUIDRef(ownerSpreadLayer), tmplFile, leftTop,
								uom->UnitsToPoints(TabLargeurs[i]), uom->UnitsToPoints(TabHauteurs[i]),
								newUID) == kSuccess)
							{
								aPub.resaUID = newUID;

								// Substitute {ORDRE_ID} / {ORDRE_ANNONCEUR}
								// only on the no-visual template (the
								// with-visual template is a graphic frame
								// with no story to walk).
								if (chosenTmpl == tmplNoVisual)
								{
									K2Vector<PMString> keys, vals;
									keys.push_back(PMString("{ORDRE_ID}", PMString::kUnknownEncoding));
									vals.push_back(aPub.resaID);
									keys.push_back(PMString("{ORDRE_ANNONCEUR}", PMString::kUnknownEncoding));
									vals.push_back(TabAnnonceurs[i]);
									ReplaceTextInFrame(UIDRef(db, aPub.resaUID), keys, vals);
								}

								if (TabVerrouPos[i])
									LockPageItemCmd(UIDRef(db, aPub.resaUID), kTrue, kTrue);

								templateUsed = kTrue;
								templateIsWithVisual = (chosenTmpl == tmplWithVisual && !tmplWithVisual.IsEmpty());

								// Persist the marker so that on the next
								// document reopen we can tell this block was
								// created from a template (placed=true path
								// otherwise has no way to know, and would
								// happily call ConvertToResaPub on it).
								InterfacePtr<IScriptLabel> labelData(UIDRef(db, aPub.resaUID), IID_ISCRIPTLABEL);
								if (labelData != nil)
								{
									PMString labelKey(kAdTemplateLabelKey, PMString::kUnknownEncoding);
									labelKey.SetTranslatable(kFalse);
									PMString labelValue(templateIsWithVisual ? "withVisual" : "1", PMString::kUnknownEncoding);
									labelValue.SetTranslatable(kFalse);
									labelData->SetTag(labelKey, labelValue);
								}
							}
						}
					}
				}

				if (!templateUsed)
				{
					if (CreerResaPub(::GetUIDRef(ownerSpreadLayer), aPub.resaID, TabAnnonceurs[i], leftTop,
						uom->UnitsToPoints(TabLargeurs[i]), uom->UnitsToPoints(TabHauteurs[i]), aPub.resaUID, TabVerrouPos[i]) != kSuccess)
					{
						CAlert::ErrorAlert(kXRLErrorCreateResaPubKey);
						continue;
					}
				}
			}

			// Add it the new list of resa pub
			newResaPubList.push_back(aPub);
			// Check whether pub file is available for import
			PMString pubFilePath = TabCheminImage[i];

			// Post-creation handling. The block has just been created
			// (or refreshed at re-open) — now place the actual image if
			// the visual is available, or fall back to the no-visual
			// state otherwise.
			//
			// Three branches:
			//   * templateUsed = false (legacy / fallback) — original code
			//     path: ConvertToResaPub on no visual, ImportPubFile on
			//     visual.
			//   * templateUsed = true + visual available — ImportPubFile
			//     places the image. ImportPubFile auto-converts the frame
			//     from text to graphic if needed (template "no-visual"
			//     was a text frame, but kPlaceItemInGraphicFrameCmd needs
			//     a graphic target).
			//   * templateUsed = true + no visual — keep the template
			//     intact (placeholders already substituted at creation).
			if (!templateUsed)
			{
				if (pubFilePath == kNullString) // Pub file is not terminated yet, or has been cancelled
				{
					// Convert back to an empty resa, if necessary
					ConvertToResaPub(UIDRef(db, aPub.resaUID), aPub.resaID, resaAnnonceur);
				}
				else
				{
					IDFile pubFile = FileUtils::PMStringToSysFile(pubFilePath);
					if (FileUtils::DoesFileExist(pubFile) && !FileUtils::IsDirectory(pubFile)) {
						ImportPubFile(UIDRef(db, aPub.resaUID), pubFile, pubFilePath);

					}
					else
					{
						// Pub file does not exist anymore (it may occur if removed manually...)
						// So we convert back to an empty resa, if necessary
						ConvertToResaPub(UIDRef(db, aPub.resaUID), aPub.resaID, resaAnnonceur);
					}
				}
			}
			else if (pubFilePath != kNullString)
			{
				// Template was used AND we have a visual: place it. Works
				// for both with-visual templates (already a graphic frame
				// — clean place inside) and no-visual templates (text
				// frame — ImportPubFile converts to graphic frame first).
				// templateIsWithVisual is currently informational; once the
				// fitting-options phase 2 lands it'll gate which fitting
				// command runs.
				(void)templateIsWithVisual;
				IDFile pubFile = FileUtils::PMStringToSysFile(pubFilePath);
				if (FileUtils::DoesFileExist(pubFile) && !FileUtils::IsDirectory(pubFile))
					ImportPubFile(UIDRef(db, aPub.resaUID), pubFile, pubFilePath);
			}
		}


		// At the end of the loop, currentResaPubList only contains pub that are not placed anymore, delete them
		for (int32 k = 0; k < currentResaPubList.size(); k++) {

			InterfacePtr<IHierarchy> pubHierToDelete(db, currentResaPubList[k].resaUID, UseDefaultIID());
			if (pubHierToDelete) {
				pubItemsToDelete.Append(currentResaPubList[k].resaUID);
			}
		}

		// Delete removed pubs
		if (pubItemsToDelete.Length() > 0)
		{
			InterfacePtr<ICommand> deleteCmd(CmdUtils::CreateCommand(kDeleteCmdBoss));
			deleteCmd->SetUndoability(ICommand::kAutoUndo);
			deleteCmd->SetItemList(pubItemsToDelete);
			CmdUtils::ProcessCommand(deleteCmd);
		}

		// Store new list of resa pub
		InterfacePtr<ICommand> setResaPubListCmd(CmdUtils::CreateCommand(kXRLSetResaPubListCmdBoss));
		setResaPubListCmd->SetUndoability(ICommand::kAutoUndo);
		setResaPubListCmd->SetItemList(UIDList(doc));
		InterfacePtr<IResaPubList> cmdData(setResaPubListCmd, IID_IRESAPUBLIST);
		cmdData->SetResaPubList(newResaPubList);
		CmdUtils::ProcessCommand(setResaPubListCmd);

	} while (false);
}

void XRailResponder::DoMakePreviewToServer(UIDRef doc)
{
	do
	{
		InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
		InterfacePtr<IXRailPrefsData>  xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));

		// Check whether previews are enabled or not0
		InterfacePtr<IBoolData>  makePreview(doc, IID_IMAKEPREVIEW);
		if (!makePreview->Get())
			break;

		int32 nbPages = GetNbPages(doc);
		int32 idPage = 0;
		PMString baseName;
		int32 index = 0;
		do {
			idPage = GetPageID(doc, index, baseName);
			++index;
		} while (index < nbPages && idPage == 0);

		if (index == nbPages && idPage == 0)
			break;

		// First, get vignette repository
		PMString clientPictPath;
		int32 scale, resolution;

		InterfacePtr<IStringData> login(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);

		PMString serverAddress = GetGoodUrlXR(baseName);
		InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
		bool connexionOK = xrailHTTP->GetPreviewFolder_v2(serverAddress, idPage, clientPictPath, scale, resolution);
		if (!connexionOK)
		{
			CAlert::ErrorAlert(PMString(kXRLErrorWSGenerateVignetteKey));
			break;
		}

		// Create preview for each registered page
		K2Vector<int32> idPages;
		K2Vector<PMString> filePaths;

		for (int16 i = index - 1; i < nbPages; ++i)
		{
			idPage = GetPageID(doc, i, baseName);
			if (idPage > 0)
			{
				idPages.push_back(idPage);
				if (xrailPrefsData->GetMakePreview() == 1) {
					// Generate vignette
					PMString jpgName = clientPictPath;
					jpgName.AppendNumber(idPage);
					jpgName += ".jpg";
					IDFile jpgFile = FileUtils::PMStringToSysFile(jpgName);
					DoMakePreview(doc, jpgFile, i, scale, resolution);
					filePaths.push_back(jpgName);
				}
				else {
					filePaths.push_back("");

				}
				if (xrailPrefsData->GetExportIDML() == 1) {
					if (i == 0) {
						PMString idmlName = clientPictPath;
						idmlName.AppendNumber(idPage);
						idmlName += ".idml";

						IDFile idmlFile = FileUtils::PMStringToSysFile(idmlName);
						DoMakeIDML(doc, idmlFile);
					}
				}
			}
		}

		// Remontee des blocs sans ID Gaia (groupes par page cote serveur)
		K2Vector<int32> blocPageIDs;
		K2Vector<PMString> blocX, blocY, blocLargeur, blocHauteur;
		GetBlocsSansIDGaia(doc, blocPageIDs, blocX, blocY, blocLargeur, blocHauteur);

		// Notify server
		connexionOK = xrailHTTP->RecupPreviews_v2(serverAddress, filePaths, idPages,
			blocPageIDs, blocX, blocY, blocLargeur, blocHauteur);
		if (!connexionOK)
		{
			CAlert::ErrorAlert(PMString(kXRLErrorWSGenerateVignetteKey));
			break;
		}

	} while (kFalse);
}

//----------------------------------------------------------------------------------------------------------------
// Helpers pour GetBlocsSansIDGaia
//----------------------------------------------------------------------------------------------------------------

// Renvoie kTrue si le bloc possede deja un ID Gaia : pub (present dans resaPubUIDs)
// ou bloc d'article (IPlacedArticleData non vide, ou IID_IARTICLEINFO non vide).
static bool16 BlocADejaUnIDGaia(const UIDRef& itemRef, UID itemUID, const UIDList& resaPubUIDs)
{
	// Pub Gaia
	if (resaPubUIDs.Contains(itemUID))
		return kTrue;

	// Article place / image : IPlacedArticleData avec un id non vide
	InterfacePtr<IPlacedArticleData> placedArticleData(itemRef, IID_IPLACEDARTICLEDATA);
	if (placedArticleData != nil && !placedArticleData->GetUniqueId().IsEmpty())
		return kTrue;

	// Bloc texte d'article : IID_IARTICLEINFO non vide
	InterfacePtr<IStringListData> articleInfo(itemRef, IID_IARTICLEINFO);
	if (articleInfo != nil && !articleInfo->GetStringList().empty())
		return kTrue;

	return kFalse;
}

// Calcule la position (X,Y dans le repere de pageGeo) et les dimensions du bloc en
// millimetres, puis ajoute le tout aux tableaux paralleles pour la page idPage.
static void AjouterBloc(IGeometry* itemGeo, IGeometry* pageGeo, IUnitOfMeasure* uom, int32 idPage,
	K2Vector<int32>& blocPageIDs, K2Vector<PMString>& blocX, K2Vector<PMString>& blocY,
	K2Vector<PMString>& blocLargeur, K2Vector<PMString>& blocHauteur)
{
	if (itemGeo == nil || pageGeo == nil || uom == nil)
		return;

	PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();
	PMPoint rightBottom = itemGeo->GetStrokeBoundingBox().RightBottom();
	::TransformInnerPointToPasteboard(itemGeo, &leftTop);
	::TransformInnerPointToPasteboard(itemGeo, &rightBottom);

	// Coin haut-gauche VISUEL de la page (document OU gabarit) en coordonnees pasteboard.
	// On se repere par rapport a ce coin, et non via TransformPasteboardPointToInner :
	// une page-gabarit n'a pas la meme origine interne (notamment en Y) qu'une page
	// document, ce qui faussait la position Y des blocs de gabarit (tetieres). La
	// position relative au coin haut-gauche est, elle, identique sur le gabarit et sur
	// chaque page document qui l'utilise.
	PMPoint pageTopLeft = pageGeo->GetStrokeBoundingBox().LeftTop();
	::TransformInnerPointToPasteboard(pageGeo, &pageTopLeft);

	PMString largeur, hauteur, posX, posY;
	largeur.AppendNumber(uom->PointsToUnits(rightBottom.X() - leftTop.X()), 3, kTrue, kTrue);
	hauteur.AppendNumber(uom->PointsToUnits(rightBottom.Y() - leftTop.Y()), 3, kTrue, kTrue);
	posX.AppendNumber(uom->PointsToUnits(leftTop.X() - pageTopLeft.X()), 3, kTrue, kTrue);
	posY.AppendNumber(uom->PointsToUnits(leftTop.Y() - pageTopLeft.Y()), 3, kTrue, kTrue);

	blocPageIDs.push_back(idPage);
	blocX.push_back(posX);
	blocY.push_back(posY);
	blocLargeur.push_back(largeur);
	blocHauteur.push_back(hauteur);
}

//----------------------------------------------------------------------------------------------------------------
// Remonte tous les blocs de la maquette qui n'ont pas d'ID Gaia (ni pub, ni article),
// y compris les blocs poses sur les gabarits (ex : tetieres), mappes sur chaque page
// document qui utilise le gabarit.
//----------------------------------------------------------------------------------------------------------------
void XRailResponder::GetBlocsSansIDGaia(UIDRef doc, K2Vector<int32>& blocPageIDs, K2Vector<PMString>& blocX,
	K2Vector<PMString>& blocY, K2Vector<PMString>& blocLargeur, K2Vector<PMString>& blocHauteur)
{
	do
	{
		IDataBase* db = doc.GetDataBase();
		if (db == nil)
			break;

		InterfacePtr<IDocument> theDoc(doc, UseDefaultIID());
		if (theDoc == nil)
			break;

		// Liste des UID des pubs Gaia : on les exclura
		UIDList resaPubUIDs(db);
		InterfacePtr<IResaPubList> resaPubListData(theDoc, IID_IRESAPUBLIST);
		if (resaPubListData != nil)
		{
			K2Vector<ResaPub> resaPubList = resaPubListData->GetResaPubList();
			for (int32 i = 0; i < resaPubList.size(); ++i)
			{
				if (db->IsValidUID(resaPubList[i].resaUID) && resaPubUIDs.DoesNotContain(resaPubList[i].resaUID))
					resaPubUIDs.Append(resaPubList[i].resaUID);
			}
		}

		// Convertisseur en millimetres
		InterfacePtr<IUnitOfMeasure> uom((IUnitOfMeasure*)::CreateObject(kMillimetersBoss, IID_IUNITOFMEASURE));
		if (uom == nil)
			break;

		// Determination de la plage de calques "contenu" (on ignore les calques de reperes)
		InterfacePtr<ILayerList> layerList(theDoc, UseDefaultIID());
		InterfacePtr<IGuidePrefs> guidePrefs(theDoc->GetDocWorkSpace(), UseDefaultIID());
		int32 nbLayers = layerList->GetCount();
		int32 startContentLayer, endContentLayer;
		if (guidePrefs && guidePrefs->GetGuidesInBack())
		{
			startContentLayer = nbLayers + 1;
			endContentLayer = (2 * nbLayers) - 1;
		}
		else
		{
			startContentLayer = 2;
			endContentLayer = nbLayers;
		}

		//==========================================================================
		// PASSE 1 : blocs poses directement sur les pages document
		//==========================================================================
		InterfacePtr<ISpreadList> spreadList(theDoc, UseDefaultIID());
		int32 nbSpreads = spreadList->GetSpreadCount();
		for (int32 s = 0; s < nbSpreads; ++s)
		{
			InterfacePtr<IHierarchy> spreadHier(db, spreadList->GetNthSpreadUID(s), UseDefaultIID());
			if (spreadHier == nil)
				continue;

			for (int32 l = startContentLayer; l <= endContentLayer; ++l)
			{
				InterfacePtr<IHierarchy> spreadLayerHier(db, spreadHier->GetChildUID(l), UseDefaultIID());
				if (spreadLayerHier == nil)
					continue;

				int32 nbItems = spreadLayerHier->GetChildCount();
				for (int32 itemIndex = 0; itemIndex < nbItems; ++itemIndex)
				{
					UID itemUID = spreadLayerHier->GetChildUID(itemIndex);
					InterfacePtr<IHierarchy> itemHier(db, itemUID, UseDefaultIID());
					if (itemHier == nil)
						continue;

					UIDRef itemRef(db, itemUID);

					// Exclure les blocs qui ont deja un ID Gaia (pub ou article)
					if (BlocADejaUnIDGaia(itemRef, itemUID, resaPubUIDs))
						continue;

					InterfacePtr<IGeometry> itemGeo(itemRef, UseDefaultIID());
					if (itemGeo == nil)
						continue;

					// Page proprietaire ; si c'est un spread, le bloc est sur la table de montage -> ignore
					UID ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);
					if (ownerPageUID == kInvalidUID)
						continue;
					InterfacePtr<ISpread> isASpread(db, ownerPageUID, UseDefaultIID());
					if (isASpread != nil)
						continue;

					InterfacePtr<IXRailPageSlugData> pageSlug(db, ownerPageUID, IID_PAGESLUGDATA);
					if (pageSlug == nil)
						continue;
					int32 idPage = pageSlug->GetID();
					if (idPage <= 0)
						continue;

					InterfacePtr<IGeometry> ownerPageGeo(db, ownerPageUID, UseDefaultIID());
					AjouterBloc(itemGeo, ownerPageGeo, uom, idPage, blocPageIDs, blocX, blocY, blocLargeur, blocHauteur);
				}
			}
		}

		//==========================================================================
		// PASSE 2 : blocs poses sur les gabarits (tetieres, etc.)
		//==========================================================================

		// Correspondance page-gabarit -> ID Gaia des pages document qui l'utilisent
		// (UIDList + tableau parallele, recherche par UIDList::Location ; peu d'entrees)
		UIDList gabaritPageUIDs(db);
		std::vector<std::vector<int32> > gabaritGaiaIDs;
		InterfacePtr<IPageList> pageList(theDoc, UseDefaultIID());
		if (pageList != nil)
		{
			int32 nbPages = pageList->GetPageCount();
			for (int32 p = 0; p < nbPages; ++p)
			{
				UID pageUID = pageList->GetNthPageUID(p);
				if (pageUID == kInvalidUID)
					continue;

				InterfacePtr<IXRailPageSlugData> pageSlug(db, pageUID, IID_PAGESLUGDATA);
				if (pageSlug == nil)
					continue;
				int32 idPage = pageSlug->GetID();
				if (idPage <= 0)
					continue;

				// Gabarit applique a cette page document
				InterfacePtr<IMasterPage> masterPageData(db, pageUID, IID_IMASTERPAGE);
				if (masterPageData == nil)
					continue;
				UID masterSpreadUID = masterPageData->GetMasterSpreadUID();
				if (masterSpreadUID == kInvalidUID)
					continue; // pas de gabarit (Aucun)

				// Page-gabarit correspondante = page de meme index dans le spread-gabarit
				InterfacePtr<IHierarchy> pageHier(db, pageUID, UseDefaultIID());
				if (pageHier == nil)
					continue;
				InterfacePtr<ISpread> docSpread(db, pageHier->GetSpreadUID(), UseDefaultIID());
				if (docSpread == nil)
					continue;
				int32 idxInSpread = docSpread->GetPageIndex(pageUID);

				InterfacePtr<ISpread> masterSpread(db, masterSpreadUID, UseDefaultIID());
				if (masterSpread == nil)
					continue;
				if (idxInSpread < 0 || idxInSpread >= masterSpread->GetNumPages())
					idxInSpread = 0;
				UID masterPageUID = masterSpread->GetNthPageUID(idxInSpread);
				if (masterPageUID == kInvalidUID)
					continue;

				// Ajout dans les tableaux paralleles
				int32 found = gabaritPageUIDs.Location(masterPageUID);
				if (found < 0)
				{
					gabaritPageUIDs.Append(masterPageUID);
					std::vector<int32> ids;
					ids.push_back(idPage);
					gabaritGaiaIDs.push_back(ids);
				}
				else
				{
					gabaritGaiaIDs[found].push_back(idPage);
				}
			}
		}

		if (!gabaritPageUIDs.IsEmpty())
		{
			InterfacePtr<IMasterSpreadList> masterSpreadList(theDoc, UseDefaultIID());
			int32 nbMasters = (masterSpreadList != nil) ? masterSpreadList->GetMasterSpreadCount() : 0;
			for (int32 m = 0; m < nbMasters; ++m)
			{
				InterfacePtr<IHierarchy> spreadHier(db, masterSpreadList->GetNthMasterSpreadUID(m), UseDefaultIID());
				if (spreadHier == nil)
					continue;

				for (int32 l = startContentLayer; l <= endContentLayer; ++l)
				{
					InterfacePtr<IHierarchy> spreadLayerHier(db, spreadHier->GetChildUID(l), UseDefaultIID());
					if (spreadLayerHier == nil)
						continue;

					int32 nbItems = spreadLayerHier->GetChildCount();
					for (int32 itemIndex = 0; itemIndex < nbItems; ++itemIndex)
					{
						UID itemUID = spreadLayerHier->GetChildUID(itemIndex);
						InterfacePtr<IHierarchy> itemHier(db, itemUID, UseDefaultIID());
						if (itemHier == nil)
							continue;

						UIDRef itemRef(db, itemUID);

						if (BlocADejaUnIDGaia(itemRef, itemUID, resaPubUIDs))
							continue;

						InterfacePtr<IGeometry> itemGeo(itemRef, UseDefaultIID());
						if (itemGeo == nil)
							continue;

						// Page-gabarit proprietaire du bloc
						UID masterPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);
						if (masterPageUID == kInvalidUID)
							continue;

						int32 found = gabaritPageUIDs.Location(masterPageUID);
						if (found < 0)
							continue; // gabarit non utilise par une page Gaia

						InterfacePtr<IGeometry> masterPageGeo(db, masterPageUID, UseDefaultIID());
						if (masterPageGeo == nil)
							continue;

						// Le bloc-gabarit apparait sur chaque page document utilisant ce gabarit
						const std::vector<int32>& gaiaIDs = gabaritGaiaIDs[found];
						for (size_t g = 0; g < gaiaIDs.size(); ++g)
						{
							AjouterBloc(itemGeo, masterPageGeo, uom, gaiaIDs[g],
								blocPageIDs, blocX, blocY, blocLargeur, blocHauteur);
						}
					}
				}
			}
		}
	} while (kFalse);
}

void XRailResponder::MAJPublicites(UIDRef doc, PMString serverAdress)
{
	ErrorCode status = kSuccess;

	do {
		IDataBase* db = doc.GetDataBase();
		if (db == nil) {
			break;
		}

		UIDList toDelete(db);

		InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

		// Create converter (resa dimensions are in millimeters)
		InterfacePtr<IUnitOfMeasure> uom((IUnitOfMeasure*)::CreateObject(kMillimetersBoss, IID_IUNITOFMEASURE));
		if (uom == nil) {
			break;
		}

		// Get list of resa pub registered in the document
		InterfacePtr<IResaPubList> resaPubListData(doc, IID_IRESAPUBLIST);
		if (resaPubListData == nil)
		{
			break;
		}

		K2Vector<ResaPub> currentResaPubList = resaPubListData->GetResaPubList();
		K2Vector<ResaPub>::iterator iter = currentResaPubList.begin();
		while (iter < currentResaPubList.end())
		{
			PMString resaID = iter->resaID;
			int32 pageID = 0;
			PMReal x = 0, y = 0, l = 0, h = 0;
			PMString imagePath = kNullString;

			if (!db->IsValidUID(iter->resaUID)) // Resa has been removed from the document
			{
				// Remove pub from document resa pub list
				iter = currentResaPubList.erase(iter);
			}
			else
			{
				InterfacePtr<IHierarchy> resaHier(db, iter->resaUID, UseDefaultIID());
				if (resaHier == nil) {
					continue;
				}

				// If we're on a placed image we should have a data link to source item
				InterfacePtr<ILinkManager> iLinkManager(db, db->GetRootUID(), UseDefaultIID());
				if (iLinkManager == nil) {
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
					if (iLink)
					{
						InterfacePtr<ILinkResource> iLinkResource(db, iLink->GetResource(), UseDefaultIID());
						if (iLinkResource)
						{
							IDFile imageFile;
							URI imageURI = iLinkResource->GetId();
							Utils<IURIUtils>()->URIToIDFile(imageURI, imageFile);
							if (FileUtils::DoesFileExist(imageFile) && !FileUtils::IsDirectory(imageFile))
							{
								imagePath = FileUtils::SysFileToPMString(imageFile);
							}
						}
					}
				}

				UID ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(resaHier);
				InterfacePtr<ISpread> isASpread(db, ownerPageUID, UseDefaultIID());
				if (isASpread)
				{
					// The pub is not placed on a page anymore, delete it
					toDelete.Append(iter->resaUID);

					// Remove pub from document resa pub list
					iter = currentResaPubList.erase(iter);
				}
				else
				{
					// Get page's XRail ID
					InterfacePtr<IXRailPageSlugData> pageSlug(db, ownerPageUID, IID_PAGESLUGDATA);
					if (pageSlug == nil) {
						continue;
					}

					// Update page info
					iter->pageID = pageSlug->GetID();

					// Get top left coordinate of this reserve
					InterfacePtr<IGeometry> resaGeo(resaHier, UseDefaultIID());
					if (resaGeo == nil) {
						continue;
					}

					PMPoint leftTop = resaGeo->GetStrokeBoundingBox().LeftTop();
					PMPoint rigthBottom = resaGeo->GetStrokeBoundingBox().RightBottom();
					::TransformInnerPointToPasteboard(resaGeo, &leftTop);
					::TransformInnerPointToPasteboard(resaGeo, &rigthBottom);

					l = uom->PointsToUnits(rigthBottom.X() - leftTop.X());
					h = uom->PointsToUnits(rigthBottom.Y() - leftTop.Y());
					::TransformPasteboardPointToInner(pageSlug, &leftTop);

					// We'll notify XRail of the position of the reserve
					pageID = iter->pageID;
					x = uom->PointsToUnits(leftTop.X());
					y = uom->PointsToUnits(leftTop.Y());

					++iter;
				}
			}

			PMString stringX, stringY, stringL, stringH;
			stringX.AppendNumber(x, 3, kTrue, kTrue);
			stringY.AppendNumber(y, 3, kTrue, kTrue);
			stringL.AppendNumber(l, 3, kTrue, kTrue);
			stringH.AppendNumber(h, 3, kTrue, kTrue);

			// Send information to XRail		
			bool connexionOK = xrailHTTP->SetPub_v2(serverAdress, resaID, pageID, stringX, stringY, stringL, stringH, imagePath);
			if (!connexionOK)
			{
				//CAlert::ErrorAlert(PMString(kXRLErrorWebServiceKey));
				status = kFailure;
				break;
			}
		}

		if (status == kSuccess)
		{
			// Delete unnecessary pubs here
			if (!toDelete.IsEmpty())
			{
				InterfacePtr<ICommand> deleteCmd(CmdUtils::CreateCommand(kDeleteCmdBoss));
				deleteCmd->SetUndoability(ICommand::kAutoUndo);
				deleteCmd->SetItemList(toDelete);

				// Process the DeleteCmd
				CmdUtils::ProcessCommand(deleteCmd);
			}

			// Update stored resa pub list
			InterfacePtr<ICommand> setResaPubListCmd(CmdUtils::CreateCommand(kXRLSetResaPubListCmdBoss));
			setResaPubListCmd->SetUndoability(ICommand::kAutoUndo);
			setResaPubListCmd->SetItemList(UIDList(doc));

			InterfacePtr<IResaPubList> cmdData(setResaPubListCmd, IID_IRESAPUBLIST);
			cmdData->SetResaPubList(currentResaPubList);

			CmdUtils::ProcessCommand(setResaPubListCmd);
		}
	} while (false);
}

// End, XRailResponder.cpp.