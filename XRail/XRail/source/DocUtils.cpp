
#include "VCPlugInHeaders.h"

#include "FileUtils.h"
#include "DocUtils.h"
#include "CAlert.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "Utils.h"
#include "UIDList.h"
#include "SnapshotUtils.h"
#include "StreamUtil.h"
#include "XRLID.h"
#include "XRLUIID.h"
#include "IXRailPrefsData.h"
#include "IXRailPageSlugData.h"
#include "IPageList.h"
#include "IDocument.h"
#include "IWorkspace.h"
#include "ISectionCmdData.h"
#include "IDocumentUtils.h"
#include "ErrorUtils.h"
#include "IDocFileHandler.h"
#include "ISectionList.h"
#include "RsrcSpec.h"
#include "IApplication.h"
#include "IDialogMgr.h"
#include "IDialog.h"
#include "IIntData.h"
#include "ITextModelCmds.h"
#include "IMultiColumnTextFrame.h"
#include "IHierarchy.h"
#include "TransformUtils.h"
#include "INewPageItemCmdData.h"
#include "IGraphicAttributeUtils.h"
#include "SplineID.h"
#include "ISwatchUtils.h"
#include "IPageItemTypeUtils.h"
#include "IDataLinkReference.h"
#include "IDataLink.h"
#include "IDataLinkHelper.h"
//#include "IImportFileCmdData.h"
#include "IPlacePIData.h"
#include "ITextAttrUID.h"
#include "PageItemScrapID.h"
#include "openplaceid.h"
#include "ITextAttrRealNumber.h"
#include "IImportResourceCmdData.h"
#include "URI.h"
#include "IURIUtils.h"
#include "ILockPosition.h"
#include "IS4SPPackage.h"
#include "PackageAndPreflightID.h"
#include "IFrameContentUtils.h"

#include "ILinkManager.h"
#include "ILink.h"
#include "ILinkObject.h"
#include "ILinkResource.h"

// Section APIs (used by SetPageFolio).
#include "ISectionList.h"
#include "ISection.h"
#include "SectionID.h"

// Pagemakeup TCL parser (used by RunPagemakeupString to apply [HS]/[HR]
// roulettes via the existing parser instead of reimplementing the
// tokenizer).
#include "ITCLParser.h"
#include "PrsID.h"
#include <cstdio>
#if WINDOWS
#include <windows.h>
#else
#include <cstdlib>
#include <unistd.h>
#endif

#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "IExportProvider.h"
#include "IPlaceGun.h"
#include "IUIDData.h"

// IDMS template import + frame transform (ad-block templates).
#include "ISnippetImport.h"
#include "IDOMElement.h"
#include "IGeometry.h"
#include "ITransformFacade.h"
#include "IGeometryFacade.h"
#include "TransformTypes.h"
#include "ITextModel.h"
#include "ITextModelCmds.h"
#include "TextIterator.h"
#include "WideString.h"
#include "ISpread.h"
#include "ISetFittingOptionsCmdData.h"  // FillProportionally fitting on template frames
#include "IReferencePointData.h"

ErrorCode DoMakePreview(const UIDRef& documentUIDRef, IDFile& jpgFile, int32 page, int32 scale, int32 resolution)
{
	ErrorCode status = kFailure;

	do {
		InterfacePtr<IDocument> doc(documentUIDRef, UseDefaultIID());
		if (doc == nil)
		{
			ASSERT_FAIL("Could not get IDocument");
			break;
		}

		// Get the page UID
		InterfacePtr<IPageList> pageList(doc, UseDefaultIID());

		UID pageUID = pageList->GetNthPageUID(page);

		if (pageUID == kInvalidUID)
		{
			ASSERT_FAIL("Wrong page index ?");
			break;
		}

		SnapshotUtils* fSnapshotUtils;
		// Create a new snapshot utils instance
		fSnapshotUtils = new SnapshotUtils(UIDRef(documentUIDRef.GetDataBase(), pageUID),
			scale / 100,     // X Scale
			scale / 100,     // Y Scale
			resolution,    // desired resolution of resulting snapshot
			resolution,    // minimum resolution of resulting snapshot
			0,     // Extend the bounds of the area to be drawn by the given amount of bleed
			IShape::kPrinting,// Drawing flags (kPrinting suppresses drawing of margins and guides)
			kTrue,    // kTrue forces images and graphics to draw at full resolution, kFalse to draw proxies
			kFalse,    // kTrue to draw grayscale, kFalse to draw RGB
			kFalse);    // kTrue to add an alpha channel, kFalse no alpha channel


		uint32 mode = kOpenOut | kOpenTrunc;
		OSType fileType = 'JPEG';
		OSType creator = '8BIM';

		// Create a stream to write out
		InterfacePtr<IPMStream> jpegStream(StreamUtil::CreateFileStreamWrite(jpgFile, mode, fileType, creator));
		if (jpegStream == nil)
		{
			ASSERT_FAIL("Could not Create a stream to write, so we can't export");
			break;
		}

		// export to JPEG
		status = fSnapshotUtils->ExportImageToJPEG(jpegStream);
		if (fSnapshotUtils)
			delete fSnapshotUtils;

	} while (false);


	return status;
}

ErrorCode DoMakeIDML(const UIDRef& documentUIDRef, IDFile& idmlFile)
{
	ICommandSequence* seq = nil;
	ErrorCode status = kFailure;

	PMString formatName("InDesignMarkup", PMString::kUnknownEncoding);
	do
	{
		InterfacePtr<IDocument> doc(documentUIDRef, UseDefaultIID());
		if (doc == nil)
		{
			ASSERT_FAIL("Could not get IDocument");
			break;
		}

		InterfacePtr<IK2ServiceRegistry> k2ServiceRegistry(GetExecutionContextSession(), UseDefaultIID());
		if (k2ServiceRegistry == nil)
		{
			break;
		}

		// get number of kExportProviderService providers
		int32 exportProviderCount = k2ServiceRegistry->GetServiceProviderCount(kExportProviderService);

		for (int32 exportProviderIndex = 0; exportProviderIndex < exportProviderCount; exportProviderIndex++)
		{
			// get the service provider boss class
			InterfacePtr<IK2ServiceProvider> k2ServiceProvider(k2ServiceRegistry->QueryNthServiceProvider(kExportProviderService, exportProviderIndex));
			if (k2ServiceProvider == nil)
			{
				break;
			}

			InterfacePtr<IExportProvider> exportProvider(k2ServiceProvider, IID_IEXPORTPROVIDER);
			if (exportProvider == nil)
			{
				break;
			}

			// check to see if the current selection specifier can be exported by this provider
			if (exportProvider->CanExportToFile())
			{
				// check by format name too, as sometimes, CanExportThisFormat ignores the formatName
				int32 formatCount = exportProvider->CountFormats();
				for (int32 formatIndex = 0; formatIndex < formatCount; formatIndex++)
				{
					PMString localFormatName = exportProvider->GetNthFormatName(formatIndex);
					if (localFormatName == formatName)
					{
						// found the right IDML export provider -> start the export process

						exportProviderIndex = exportProviderCount; // to go out of the loop after processing the command

						PMString fileName, fileExt, fileToExport;

						seq = CmdUtils::BeginCommandSequence();
						if (seq == nil)
						{
							ASSERT_FAIL("Func_EX::IDCall_EX -> seq nil");
							break;
						}



						// Do the export!
						exportProvider->ExportToFile(idmlFile, doc, nil, formatName, K2::kSuppressUI);

						// check error
						status = ErrorUtils::PMGetGlobalErrorCode();

						break;
					}
				}
			}
		}

	} while (kFalse);

	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(kFailure);

		CmdUtils::EndCommandSequence(seq);
	}
	else
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return status;
}



ErrorCode DoSaveDoc(UIDRef documentUIDRef, IDFile sysFile)
{
	ErrorCode result = kFailure;
	do {
		// Save the document to another file.

		// Starting from InDesign 3.0, IDocumentUtils has been added to 
		// the utils boss where it is more logically placed. 
		// The version of IDocumentUtils on the session boss is being deprecated. 
		InterfacePtr<IDocFileHandler> docFileHandler(Utils<IDocumentUtils>()->QueryDocFileHandler(documentUIDRef));
		if (!docFileHandler) {
			break;
		}
		//Try to do SaveAs
		if (docFileHandler->CanSaveAs(documentUIDRef)) {
			docFileHandler->SaveAs(documentUIDRef, &sysFile, kSuppressUI);
			result = ErrorUtils::PMGetGlobalErrorCode();
			ASSERT_MSG(result == kSuccess, "IDocFileHandler::SaveAs failed");
			if (result != kSuccess) {
				break;
			}
		}
	} while (false);
	return result;
}

void SetPrefs(PMString urlXR, PMString lastUser, bool16 modeRobot, K2Vector<PMString> listeBases, K2Vector<PMString> IPBases)
{
	do
	{
		InterfacePtr<ICommand> setPrefsCmd(CmdUtils::CreateCommand(kXRLSetPrefsCmdBoss));

		InterfacePtr<IXRailPrefsData> prefsData(setPrefsCmd, UseDefaultIID());
		prefsData->SeturlXR(urlXR);
		prefsData->SetLastUser(lastUser);
		prefsData->SetModeRobot(modeRobot);
#if MULTIBASES == 1
		prefsData->SetListeBases(listeBases);
		prefsData->SetIPBases(IPBases);
#endif
		if (CmdUtils::ProcessCommand(setPrefsCmd) != kSuccess)
		{
			ASSERT_FAIL("command setPrefsCmd failed");
			break;
		}
	} while (false);
}

void SetPageID(UIDRef docUIDRef, int32 indexofpage, int32 id, PMString baseName)
{
	do {

		// On cree la commande
		InterfacePtr<ICommand> writeSlugCmd(CmdUtils::CreateCommand(kXRLSetPageSlugDataCmdBoss));
		writeSlugCmd->SetUndoability(ICommand::kAutoUndo);

		// On specifie les parametres
		InterfacePtr<IXRailPageSlugData> slugdata(writeSlugCmd, IID_PAGESLUGDATA);
		slugdata->SetID(id);
#if MULTIBASES == 1
		slugdata->SetBaseName(baseName);
#endif
		InterfacePtr<IIntData> pageIndexData(writeSlugCmd, UseDefaultIID());
		pageIndexData->Set(indexofpage);

		// On precise la cible de la commande
		UIDList List(docUIDRef);
		writeSlugCmd->SetItemList(List);

		// On lance la commandePriority
		ErrorCode err = CmdUtils::ProcessCommand(writeSlugCmd);

		// Gestion des erreurs
		if (err != kSuccess) {
			ASSERT_FAIL("TLPanelWidgetObserver::WriteStatus -> unable to save data in doc");
		}

	} while (false);
}

int32 GetPageID(UIDRef docUIDRef, int32 index, PMString& baseName)
{
	int32 returnID = 0;
	do
	{
		InterfacePtr<IPageList> pageList(docUIDRef, UseDefaultIID());

		UID pageUID = pageList->GetNthPageUID(index);
		if (pageUID == kInvalidUID) // Wrong page index
			break;

		InterfacePtr<IXRailPageSlugData> readdata(docUIDRef.GetDataBase(), pageUID, IID_PAGESLUGDATA);

		returnID = readdata->GetID();
#if MULTIBASES == 1
		baseName = readdata->GetBaseName();
#endif
	} while (false);

	return returnID;
}

void GetPrefs(PMString& urlXR, PMString& lastUser, bool16& modeRobot, K2Vector<PMString>& listeBases, K2Vector<PMString>& IPBases)
{
	InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());

	InterfacePtr<IXRailPrefsData>  xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));

	urlXR = xrailPrefsData->GeturlXR();
	lastUser = xrailPrefsData->GetLastUser();
	modeRobot = xrailPrefsData->GetModeRobot();
#if MULTIBASES == 1
	listeBases = xrailPrefsData->GetListeBases();
	IPBases = xrailPrefsData->GetIPBases();
#endif
}

void GetRememberedCredentials(bool16& rememberMe, PMString& password)
{
	rememberMe = kFalse;
	password = "";

	InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
	if (workspace == nil)
		return;

	InterfacePtr<IXRailPrefsData> xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));
	if (xrailPrefsData == nil)
		return;

	rememberMe = xrailPrefsData->GetRememberMe();
	password = xrailPrefsData->GetRememberedPassword();
}

void SetRememberedCredentials(bool16 rememberMe, const PMString& password)
{
	InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
	if (workspace == nil)
		return;

	InterfacePtr<IXRailPrefsData> xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));
	if (xrailPrefsData == nil)
		return;

	xrailPrefsData->SetRememberMe(rememberMe);
	// Only store the password when "remember me" is on; clear it otherwise so
	// we never leave a stale password in the preferences file.
	xrailPrefsData->SetRememberedPassword(rememberMe ? password : PMString(""));
}

PMString GetGoodUrlXR(PMString baseName)
{
	int32 i = 0;
	PMString urlXR, temp;
	bool16 modeRobot = kTrue;
	bool16 isReadOnly = kFalse;
	K2Vector<PMString> listeBases;
	K2Vector<PMString> IPBases;
	GetPrefs(urlXR, temp, modeRobot, listeBases, IPBases);

	PMString serverAddress = urlXR;
#if MULTIBASES == 1
	CAlert::InformationAlert(baseName);
	//si le nom de la base est renseigne
	if (baseName != "") {
		for (i = 0; i < listeBases.size(); i++) {
			if (listeBases[i] == baseName) {
				serverAddress = IPBases[i];

				CAlert::InformationAlert(serverAddress);
				break;
			}
		}
	}
#endif
	return serverAddress;
}


ErrorCode SaveAsDoc(UIDRef doc, PMString absoluteName)
{
	ErrorCode status = kFailure;
	PMString error;
	do
	{
		// Get an IDocFileHandler interface for the document:
		InterfacePtr<IDocFileHandler> docFileHandler(Utils<IDocumentUtils>()->QueryDocFileHandler(doc));
		if (docFileHandler == nil)
			break;

		// Use IDocFileHandler's SaveAs() command to implement SaveAsDocCmd:
		if (!docFileHandler->CanSaveAs(doc))
		{
			break;
		}

		IDFile file = FileUtils::PMStringToSysFile(absoluteName);
		IDFile parent;

		FileUtils::GetParentDirectory(file, parent);

		if (FileUtils::DoesFileExist(parent) == kFalse)// Invalid path
		{
			break;
		}

		int16 rep = 1;
		if (FileUtils::DoesFileExist(file) == kTrue)// le fichier existe deja, on demande si on sauvegarde
			rep = CAlert::ModalAlert(kXRLUIMessageLinkerKey, kOKString, kCancelString, kNullString, 2, CAlert::eWarningIcon);

		if (rep == 1) {
			// if the user clicked on "OK" button
			DoSaveDoc(doc, file);
			status = ErrorUtils::PMGetGlobalErrorCode();
			if (status != kSuccess)
			{
				ErrorUtils::PMSetGlobalErrorCode(kTrue);
				break;
			}
		}
		else
			status = kSuccess; // Not an error if user cancelled

	} while (false);

	if (status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(kFalse);

	return status;
}

int32 GetNbPages(const UIDRef& documentUIDRef)
{
	int32 nbPages = 0;

	if (documentUIDRef != UIDRef::gNull)
	{
		InterfacePtr<IPageList> pageList(documentUIDRef, UseDefaultIID());
		nbPages = pageList->GetPageCount();
	}
	return nbPages;
}

IDialog* CreateNewDialog(PluginID pluginID, RsrcID dialogID, IDialog::DialogType dialogType)
{
	IDialog* dialog = nil;

	do {
		// Get the application interface and the DialogMgr.	
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		if (!application)
			break;

		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
		if (!dialogMgr)
			break;

		// Load the plug-in's resource.
		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcSpec dialogSpec
		(
			nLocale,					// Locale index from PMLocaleIDs.h. 
			pluginID,			// Our Plug-in ID from BasicDialogID.h. 
			kViewRsrcType,				// This is the kViewRsrcType.
			dialogID,	// Resource ID for our dialog.
			kTrue						// Initially visible.
		);

		// CreateNewDialog takes the dialogSpec Created above, and also
		// the type of dialog being Created (kMovableModal).
		dialog = dialogMgr->CreateNewDialog(dialogSpec, dialogType);

	} while (kFalse);

	return dialog;
}

ErrorCode CreerResaPub(UIDRef parent, const PMString& resaID, const PMString& resaAnnonceur, const PMPoint& leftTop, const PMReal& width, const PMReal& height, UID& resaUID, bool16 VerrouPos)
{
	ErrorCode status = kFailure;
	do
	{
		IDataBase* db = parent.GetDataBase();

		PMPointList resaBounds;
		resaBounds.push_back(leftTop);
		resaBounds.push_back(PMPoint(leftTop.X() + width, leftTop.Y() + height));

		// Create a text frame
		InterfacePtr<ICommand> CreateTextFrameCmd(CmdUtils::CreateCommand(kCreateMultiColumnItemCmdBoss));
		CreateTextFrameCmd->SetUndoability(ICommand::kAutoUndo);
		InterfacePtr<INewPageItemCmdData> data(CreateTextFrameCmd, UseDefaultIID());
		data->Set(db, kSplineItemBoss, parent.GetUID(), resaBounds);

		if (CmdUtils::ProcessCommand(CreateTextFrameCmd) != kSuccess)
			break;
		UIDRef resaRef = CreateTextFrameCmd->GetItemList()->GetRef(0);
		resaUID = resaRef.GetUID(); // Register resa UID

		// GD 7.5.6 ++
		// Ajout de la prise en compte du parametre "verrou position pub"
		if (resaRef != kInvalidUIDRef) {
			if (VerrouPos == kTrue) {
				LockPageItemCmd(resaRef, kTrue, kTrue);
			}
		}
		// GD 7.5.6 --
		InterfacePtr<IHierarchy> resaHier(resaRef, UseDefaultIID());
		InterfacePtr<IMultiColumnTextFrame> txtFrame(db, resaHier->GetChildUID(0), UseDefaultIID());
		InterfacePtr<ITextModelCmds>modelCmds(db, txtFrame->GetTextModelUID(), UseDefaultIID());

		// Set text color to white
		boost::shared_ptr<AttributeBossList> attrList(new AttributeBossList());
		InterfacePtr<ITextAttrUID> textAttrUID_Color(::CreateObject2<ITextAttrUID>(kTextAttrColorBoss));
		textAttrUID_Color->SetUIDData(Utils<ISwatchUtils>()->GetPaperSwatchUID(db));

		// Create a point size, leading and underline attributes.
		InterfacePtr<ITextAttrRealNumber> textAttrPointSize(::CreateObject2<ITextAttrRealNumber>(kTextAttrPointSizeBoss));
		textAttrPointSize->Set(PMReal(20.0));

		// Create a point size, leading and underline attributes.
		InterfacePtr<ITextAttrRealNumber> textAttrLeading(::CreateObject2<ITextAttrRealNumber>(kTextAttrLeadBoss));
		textAttrLeading->Set(PMReal(20.0));

		attrList->ApplyAttribute(textAttrUID_Color);
		attrList->ApplyAttribute(textAttrPointSize);
		attrList->ApplyAttribute(textAttrLeading);
		InterfacePtr<ICommand> applyAttrCmd(modelCmds->ApplyCmd(0, 0, attrList, kCharAttrStrandBoss));
		if (CmdUtils::ProcessCommand(applyAttrCmd) != kSuccess)
			break;
		// Insert ID in resa and Annonceur		
		boost::shared_ptr<WideString> toInsert(new WideString(resaID));
		toInsert->Append(kTextChar_CR);
		toInsert->Append(WideString(resaAnnonceur));

		InterfacePtr<ICommand> insertTextCmd(modelCmds->InsertCmd(0, toInsert));
		if (CmdUtils::ProcessCommand(insertTextCmd) != kSuccess)
			break;
		// Set fill color
		UIDList frameList(resaRef);
		InterfacePtr<ICommand> fillCmd(Utils<IGraphicAttributeUtils>()->CreateFillRenderingCommand(Utils<ISwatchUtils>()->GetBlackSwatchUID(db), &frameList, kTrue, kTrue));
		if (CmdUtils::ProcessCommand(fillCmd) != kSuccess)
			break;

		// Set the tint (other method than this used to set the color : IGraphicAttributeUtils)
		int32 trameFond = 30;
		InterfacePtr<ICommand> fillTintCmd(Utils<IGraphicAttributeUtils>()->CreateFillTintCommand(trameFond, &frameList, kTrue, kTrue));
		if (CmdUtils::ProcessCommand(fillTintCmd) != kSuccess)
			break;
		status = kSuccess;

	} while (kFalse);

	return status;
}

ErrorCode ConvertToResaPub(UIDRef resaFrame, const PMString& resaID, const PMString& resaAnnonceur)
{
	ErrorCode status = kFailure;
	do
	{
		UIDList frameList(resaFrame);
		IDataBase* db = resaFrame.GetDataBase();
		if (db == nil)
			break;

		InterfacePtr<IHierarchy> resaHier(resaFrame, UseDefaultIID());
		if (resaHier == nil)
			break;

		boost::shared_ptr<WideString> toInsert(new WideString(resaID));
		toInsert->Append(kTextChar_CR);
		toInsert->Append(WideString(resaAnnonceur));
		// Check whether it's already a text frame or not
		if (Utils<IPageItemTypeUtils>()->IsTextFrame(resaHier))
		{
			// Reset text in it to pub ID
			InterfacePtr<IMultiColumnTextFrame> txtFrame(db, resaHier->GetChildUID(0), UseDefaultIID());
			if (txtFrame == nil)
				break;

			InterfacePtr<ITextModelCmds> modelCmds(db, txtFrame->GetTextModelUID(), UseDefaultIID());
			if (modelCmds == nil)
				break;

			InterfacePtr<ITextModel> txtModel(modelCmds, UseDefaultIID());
			if (txtModel == nil)
				break;

			ErrorUtils::PMSetGlobalErrorCode(kSuccess);

			InterfacePtr<ICommand> replaceTextCmd(modelCmds->ReplaceCmd(0, txtModel->GetPrimaryStoryThreadSpan() - 1, toInsert));
			if (CmdUtils::ProcessCommand(replaceTextCmd) != kSuccess)
				break;
		}
		else
		{
			// Convert to text frame
			InterfacePtr<ICommand> convertToTextCmd(CmdUtils::CreateCommand(kConvertItemToTextCmdBoss));
			convertToTextCmd->SetUndoability(ICommand::kAutoUndo);
			convertToTextCmd->SetItemList(frameList);
			if (CmdUtils::ProcessCommand(convertToTextCmd) != kSuccess)
				break;

			InterfacePtr<IMultiColumnTextFrame> txtFrame(db, resaHier->GetChildUID(0), UseDefaultIID());
			if (!txtFrame)
				break;

			InterfacePtr<ITextModelCmds> modelCmds(db, txtFrame->GetTextModelUID(), UseDefaultIID());
			if (modelCmds == nil)
				break;

			InterfacePtr<ITextModel> txtModel(modelCmds, UseDefaultIID());
			if (txtFrame == nil)
				break;

			// Set text color to white
			boost::shared_ptr<AttributeBossList> attrList(new AttributeBossList());
			InterfacePtr<ITextAttrUID> textAttrUID_Color(::CreateObject2<ITextAttrUID>(kTextAttrColorBoss));
			textAttrUID_Color->SetUIDData(Utils<ISwatchUtils>()->GetPaperSwatchUID(db));
			attrList->ApplyAttribute(textAttrUID_Color);

			InterfacePtr<ICommand> applyAttrCmd(modelCmds->ApplyCmd(0, 0, attrList, kCharAttrStrandBoss));
			if (CmdUtils::ProcessCommand(applyAttrCmd) != kSuccess)
				break;

			// Insert ID in resa	
			InterfacePtr<ICommand> insertTextCmd(modelCmds->InsertCmd(0, toInsert));
			if (CmdUtils::ProcessCommand(insertTextCmd) != kSuccess)
				break;
		}

		// Set fill color		
		InterfacePtr<ICommand> fillCmd(Utils<IGraphicAttributeUtils>()->CreateFillRenderingCommand(Utils<ISwatchUtils>()->GetBlackSwatchUID(db), &frameList, kTrue, kTrue));
		if (CmdUtils::ProcessCommand(fillCmd) != kSuccess)
			break;

		// Set the tint (other method than this used to set the color : IGraphicAttributeUtils)
		int32 trameFond = 30;
		InterfacePtr<ICommand> fillTintCmd(Utils<IGraphicAttributeUtils>()->CreateFillTintCommand(trameFond, &frameList, kTrue, kTrue));
		if (CmdUtils::ProcessCommand(fillTintCmd) != kSuccess)
			break;

		status = kSuccess;

	} while (kFalse);

	return status;
}

void ImportPubFile(UIDRef resaPub, const IDFile& pubFile, PMString pubFileName, bool16 respectTemplateFitting)
{
	do
	{
		bool16 reimport = kTrue;

		IDataBase* db = resaPub.GetDataBase();
		if (db == nil)
			break;

		InterfacePtr<IHierarchy> currentPubHier(resaPub, UseDefaultIID());
		if (currentPubHier == nil)
			break;

		if (!FileUtils::DoesFileExist(pubFile))
			break;

		// Check whether the frame already contains an image, and if the
		// filename matches — we want to avoid re-importing the same image.
		// Skip this whole block when the frame has no child (case of a
		// fresh empty graphic frame coming from a with-visual IDMS
		// template: ImportFromStream gives us the styled rectangle but
		// nothing's been placed inside yet). Without these guards
		// ::GetDataBase(nil)->GetRootUID() crashes.
		InterfacePtr<IHierarchy> imageHier(currentPubHier->QueryChild(0));
		ILinkManager::QueryResult linkQueryResult;
		if (imageHier != nil)
		{
			IDataBase* iDataBase = ::GetDataBase(imageHier);
			if (iDataBase == nil)
				break;
			InterfacePtr<ILinkManager> iLinkManager(iDataBase, iDataBase->GetRootUID(), UseDefaultIID());
			if (iLinkManager == nil)
				break;

			InterfacePtr<ILinkObject> iLinkObject(imageHier, UseDefaultIID()); // poupee vaudoue...

			if (iLinkManager->QueryLinksByObjectUID(::GetUID(imageHier), linkQueryResult))
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
						PMString fileNameWS, fileNameDoc;
						FileUtils::GetFileName(imageFile, fileNameWS);
						FileUtils::GetFileName(pubFileName, fileNameDoc);
						// on ne compare QUE le nom du fichier, pas le chemin
						if (fileNameWS == fileNameDoc) {
							reimport = kFalse;
						}
					}
				}
			}
		} // end if (imageHier != nil)

		// le fichier pub est le meme, on verifie si il est a jour
		if (reimport == kFalse) {
			// Check whether it has been imported or not
			if (currentPubHier->GetChildCount() == 1)
			{
				// If we're on a placed image we should have a data link to source item
				InterfacePtr<ILinkManager> iLinkManagerPub(db, db->GetRootUID(), UseDefaultIID());
				InterfacePtr<ILinkObject> iLinkObjectPub(db, currentPubHier->GetChildUID(0), UseDefaultIID());
				if (iLinkObjectPub)
				{
					ILinkManager::QueryResult linkQueryResultPub;
					if (iLinkManagerPub && iLinkManagerPub->QueryLinksByObjectUID(currentPubHier->GetChildUID(0), linkQueryResultPub))
					{
						ILinkManager::QueryResult::const_iterator iterLinks = linkQueryResultPub.begin();
						InterfacePtr<ILink> iLink(db, *iterLinks, UseDefaultIID());
						if (iLink)
						{
							// The identifiers are used to get and set the modification state
							ILink::ResourceModificationState state = iLink->GetResourceModificationState();
							if (state == ILink::kResourceUnmodified) {
								reimport = kFalse;
							}
							else {
								reimport = kTrue;
							}
						}
					}
				}
			}
		}
		if (reimport) {
			// Import image
			InterfacePtr<ICommand> importFileCmd(CmdUtils::CreateCommand(kImportResourceCmdBoss));
			if (importFileCmd == nil)
				break;

			InterfacePtr<IImportResourceCmdData> importFileCmdData(importFileCmd, IID_IIMPORTRESOURCECMDDATA);
			if (importFileCmdData == nil)
				break;

			// IDFile to URI
			URI pubFileURI;
			Utils<IURIUtils>()->IDFileToURI(pubFile, pubFileURI);

			// Set the file to import
			importFileCmdData->Set(db, pubFileURI, K2::kSuppressUI);
			if (CmdUtils::ProcessCommand(importFileCmd) != kSuccess)
			{
				break;
			}

			// Get the contents of the place gun
			InterfacePtr<IPlaceGun> placeGun(db, db->GetRootUID(), UseDefaultIID());
			ASSERT(placeGun);
			if (!placeGun)
			{
				ErrorUtils::PMSetGlobalErrorCode(kFailure);
				break;
			}

			InterfacePtr<ICommand> clearPlaceGun(CmdUtils::CreateCommand(kClearPlaceGunCmdBoss));
			ASSERT(clearPlaceGun);
			if (!clearPlaceGun)
			{
				ErrorUtils::PMSetGlobalErrorCode(kFailure);
				break;
			}

			InterfacePtr<IUIDData> uidData(clearPlaceGun, UseDefaultIID());
			ASSERT(uidData);
			if (!uidData)
			{
				ErrorUtils::PMSetGlobalErrorCode(kFailure);
				break;
			}
			uidData->Set(placeGun);
			CmdUtils::ProcessCommand(clearPlaceGun);



			UIDRef imageItem = UIDRef(db, importFileCmd->GetItemList()->First());

			// BUG FIX :  sometimes item resulting from importation
			// isn't directly an image item (kImageItem, kEPSItem, kPlacedPDFItem), but
			// a kSplineItemBoss, father of an image item
			UIDList deleteList(db);
			InterfacePtr<IHierarchy> imageHierPub(imageItem, UseDefaultIID());
			if (imageHierPub && imageHierPub->GetChildCount() == 1)
			{
				UIDRef splineItem = imageItem;
				imageItem = UIDRef(db, imageHierPub->GetChildUID(0));

				// Remove image item from container spline
				InterfacePtr<ICommand> removeFromHierCmd(CmdUtils::CreateCommand(kRemoveFromHierarchyCmdBoss));
				removeFromHierCmd->SetItemList(UIDList(imageItem));
				CmdUtils::ProcessCommand(removeFromHierCmd);

				// Delete useless spline item
				deleteList.Append(splineItem.GetUID());

				// Re-Create datalink cause it was on spline item which has just been deleted
				InterfacePtr<IDataLinkHelper> dlHelper(::CreateObject2<IDataLinkHelper>(kDataLinkHelperBoss));
				InterfacePtr<IDataLink> imageLink(dlHelper->CreateDataLink(pubFile));
				if (imageLink && dlHelper->NewDataLinkUID(db, imageLink) != 0)
				{
					break;
				}
			}

			// Delete existing image item, if any
			if (currentPubHier && currentPubHier->GetChildCount() > 0)
			{
				deleteList.Append(currentPubHier->GetChildUID(0));
			}

			if (!deleteList.IsEmpty())
			{
				InterfacePtr<ICommand> deletePageItem(CmdUtils::CreateCommand(kDeleteCmdBoss));
				deletePageItem->SetItemList(deleteList);
				if (CmdUtils::ProcessCommand(deletePageItem) != kSuccess)
					break;
			}

			// Place image in resapub
			InterfacePtr<ICommand> placeItemCmd(CmdUtils::CreateCommand(kPlaceItemInGraphicFrameCmdBoss));
			placeItemCmd->SetUndoability(ICommand::kAutoUndo);

			InterfacePtr<IPlacePIData> placeData(placeItemCmd, IID_IPLACEPIDATA);
			placeData->Set(resaPub, nil, kFalse);

			placeItemCmd->SetItemList(UIDList(imageItem));
			if (CmdUtils::ProcessCommand(placeItemCmd) != kSuccess)
				break;

			// Resize reserve frame and reset backgroung color
			UIDList frameList(resaPub);
			InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
			InterfacePtr<IXRailPrefsData>  xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));

			// When respectTemplateFitting is set (IDMS-template path), we
			// skip both auto-fit commands: the frame already carries the
			// FrameFittingOption configured by the client in the template
			// (e.g. "Remplir proportionnellement"), and
			// kPlaceItemInGraphicFrameCmdBoss above honoured it when
			// placing the image. Forcing kFitContent / kFitFrame here
			// would override the template's choice and resize the frame
			// to the visual's intrinsic dimensions, which is exactly the
			// behaviour we want to avoid.
			if (!respectTemplateFitting)
			{
				if (xrailPrefsData->GetPubProp() == 1) {
					if (!Utils<IFrameContentUtils>()->DoesContainGraphicFrame(&frameList, kFalse))
						break;

					UIDList* contentList = nil;
					contentList = Utils<IFrameContentUtils>()->CreateListOfContent(frameList);
					UIDRef content = UIDRef(db, contentList->At(0));
					InterfacePtr<ICommand> fitContentToFrameCmd(CmdUtils::CreateCommand(kFitContentToFrameCmdBoss));
					fitContentToFrameCmd->SetUndoability(ICommand::kAutoUndo);
					fitContentToFrameCmd->SetItemList(UIDList(content));

					if (CmdUtils::ProcessCommand(fitContentToFrameCmd) != kSuccess)
						break;
				}
				else {
					InterfacePtr<ICommand> fitFrameToContentCmd(CmdUtils::CreateCommand(kFitFrameToContentCmdBoss));
					fitFrameToContentCmd->SetUndoability(ICommand::kAutoUndo);
					fitFrameToContentCmd->SetItemList(frameList);
					if (CmdUtils::ProcessCommand(fitFrameToContentCmd) != kSuccess)
						break;
				}
			}
			else
			{
				// IDMS-template path: kPlaceItemInGraphicFrameCmd above
				// places the image at 100% even when the frame's
				// FrameFittingOption is "FillProportionally", so we
				// re-apply the fitting explicitly here. The current
				// implementation hardcodes kFillProportionally because
				// that's the option the templates carry in practice; if
				// later templates use kFitProportionally / kFitContents,
				// we'll switch to reading the frame's existing
				// FrameFittingOption attribute instead of forcing one.
				// Reference point = center anchor (Adobe default for
				// proportional fills); no crop adjustments.
				InterfacePtr<ICommand> setFittingCmd(CmdUtils::CreateCommand(kSetFittingOptionsCmdBoss));
				if (setFittingCmd != nil)
				{
					setFittingCmd->SetUndoability(ICommand::kAutoUndo);
					InterfacePtr<ISetFittingOptionsCmdData> fittingData(setFittingCmd, IID_ISETFITTINGOPTIONSCMDDATA);
					if (fittingData != nil)
					{
						fittingData->SetFittingOptionsData(
							IReferencePointData::kCenterReferencePoint,
							PMRect(0, 0, 0, 0),
							ISetFittingOptionsCmdData::kFillProportionally,
							kFalse);
						setFittingCmd->SetItemList(frameList);
						CmdUtils::ProcessCommand(setFittingCmd);
					}
				}
			}

			// Reset fill color
			InterfacePtr<ICommand> fillCmd(Utils<IGraphicAttributeUtils>()->CreateFillRenderingCommand(
				Utils<ISwatchUtils>()->GetNoneSwatchUID(db), &frameList, kTrue, kTrue));
			CmdUtils::ProcessCommand(fillCmd);
		}
	} while (kFalse);
}

void LockPageItemCmd(UIDRef itemToSet, bool8 toLock, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	do {
		// Get the ILockPosition of the item to lock or unlock in order to verify its status
		InterfacePtr<ILockPosition> lockPosition(itemToSet, IID_ILOCKPOSITION);
		if (lockPosition == nil) {
			ASSERT_FAIL("XRail -> LockPageItemCmd -> lockPosition nil");
			break;
		}

		// Create a SetLockPositionCmd
		InterfacePtr<ICommand> setLPCmd(CmdUtils::CreateCommand(kSetLockPositionCmdBoss));
		if (setLPCmd == nil) {
			ASSERT_FAIL("XRail -> LockPageItemCmd -> setLPCmd nil");
			break;
		}

		// Set the SetLockPositionCmd's ItemList
		setLPCmd->SetItemList(UIDList(itemToSet));

		// Set command undoability
		if (!allowUndo)
			setLPCmd->SetUndoability(ICommand::kAutoUndo);

		// Get an IIntData Interface for the SetLockPositionCmd
		InterfacePtr<IIntData> intData(setLPCmd, IID_IINTDATA);
		if (intData == nil) {
			ASSERT_FAIL("XRail -> LockPageItem -> intData nil");
			break;
		}

		// Set the IIntData Interface's data
		if (toLock)
			intData->Set(ILockPosition::kLock);
		else
			intData->Set(ILockPosition::kUnlock);

		// Process the SetLockPositionCmd
		status = CmdUtils::ProcessCommand(setLPCmd);
		if (status != kSuccess) {
			ASSERT_FAIL("XRail -> LockPageItem -> command SetLockPositionCmd failed");
			break;
		}

	} while (false);
}


//----------------------------------------------------------------------------------------------------------------
// ImportAdTemplateBlock — import an IDMS ad template, then move + resize it
// to the target rectangle. Returns the imported frame's UID via outFrameUID.
//
// Strategy:
//   1. Snapshot the spread layer's children before import (UIDs).
//   2. Stream the IDMS file through ISnippetImport into the layer's DOM.
//   3. Snapshot after — the difference is the imported items (we expect 1).
//   4. Compute current bbox of that frame, translate so its top-left lands
//      on `leftTop`, then resize to (width, height).
//
// Failure modes (return kFailure, outFrameUID = kInvalidUID):
//   - file unreadable / not an IDMS
//   - import yields 0 or >1 top-level items (template doesn't follow the
//     "single frame" contract)
//   - geometry calls fail
// Caller falls back to the legacy CreerResaPub / ImportPubFile path.
//----------------------------------------------------------------------------------------------------------------
ErrorCode ImportAdTemplateBlock(UIDRef ownerSpreadLayer, const IDFile& templateFile,
	const PMPoint& leftTop, const PMReal& width, const PMReal& height,
	UID& outFrameUID)
{
	outFrameUID = kInvalidUID;
	ErrorCode status = kFailure;

	do
	{
		IDataBase* db = ownerSpreadLayer.GetDataBase();
		if (db == nil)
			break;

		// Resolve the parent spread of the target layer. We snapshot the
		// spread's full item list (across all pages, all layers) before
		// and after the import: a snippet whose ItemLayer doesn't match
		// the destination doc layer ends up on a different layer than
		// ownerSpreadLayer, so a per-layer diff would miss it.
		InterfacePtr<IHierarchy> layerHier(ownerSpreadLayer, UseDefaultIID());
		if (layerHier == nil)
			break;
		const UID spreadUID = layerHier->GetSpreadUID();
		InterfacePtr<ISpread> ownerSpread(db, spreadUID, UseDefaultIID());
		if (ownerSpread == nil)
			break;
		const int32 nbPagesInSpread = ownerSpread->GetNumPages();

		UIDList preImport(db);
		for (int32 p = 0; p < nbPagesInSpread; ++p)
			ownerSpread->GetItemsOnPage(p, &preImport, kFalse, kTrue);

		// Open the IDMS file as a stream.
		InterfacePtr<IPMStream> snippetStream(StreamUtil::CreateFileStreamRead(templateFile, kOpenIn));
		if (snippetStream == nil)
			break;

		// Resolve a DOM element host for the snippet importer. The SDK
		// expects the SPREAD (not a spread layer) to satisfy IDOMElement —
		// passing the layer used to silently fail (the InterfacePtr came
		// back nil and the import never ran). Cf. SDK sample
		// SnpImportExportSnippet::ImportToSpreadElement and XPage's
		// XPageUtils::ImportForme, which both root the import on the
		// spread.
		InterfacePtr<IDOMElement> rootElement(ownerSpread, UseDefaultIID());
		if (rootElement == nil)
			break;

		// Import. kSuppressUI keeps the importer silent — we don't want
		// progress dialogs for an automated ad-block insertion at doc open.
		ICommandSequence* seq = CmdUtils::BeginCommandSequence();
		ErrorCode importResult = Utils<ISnippetImport>()->ImportFromStream(
			snippetStream, rootElement, kInvalidClass, kSuppressUI);
		CmdUtils::EndCommandSequence(seq);

		if (importResult != kSuccess)
			break;

		// Diff: anything on the spread now that wasn't there before is
		// from the snippet. We expect exactly one top-level frame per
		// template (single-frame contract documented in IXRailPrefsData).
		UIDList postImport(db);
		for (int32 p = 0; p < nbPagesInSpread; ++p)
			ownerSpread->GetItemsOnPage(p, &postImport, kFalse, kTrue);

		UIDList newItems(db);
		const int32 nbPost = postImport.Length();
		for (int32 i = 0; i < nbPost; ++i)
		{
			const UID childUID = postImport[i];
			if (preImport.Location(childUID) == -1)
				newItems.Append(childUID);
		}

		if (newItems.Length() != 1)
		{
			// Template didn't produce a single top-level frame — bail. The
			// caller falls back to the hard-coded creation path; we don't
			// try to clean up the partial import here (CmdUtils handles
			// command-level rollback if the sequence aborted).
			break;
		}

		UIDRef newItemRef = newItems.GetRef(0);

		// Move + resize the imported frame to (leftTop, width, height).
		InterfacePtr<IGeometry> frameGeo(newItemRef, UseDefaultIID());
		if (frameGeo == nil)
			break;

		// Current top-left of the frame in pasteboard coordinates.
		PMPoint currentTopLeft = frameGeo->GetStrokeBoundingBox().LeftTop();
		::TransformInnerPointToPasteboard(frameGeo, &currentTopLeft);

		// Translate so the frame's top-left lands on `leftTop`.
		const PMReal dx = leftTop.X() - currentTopLeft.X();
		const PMReal dy = leftTop.Y() - currentTopLeft.Y();
		if (dx != 0 || dy != 0)
		{
			ErrorCode moveStatus = Utils<Facade::ITransformFacade>()->TransformItems(
				UIDList(newItemRef),
				Transform::PasteboardCoordinates(),
				kZeroPoint,
				Transform::TranslateBy(dx, dy));
			if (moveStatus != kSuccess)
				break;
		}

		// Resize to the requested width/height, keeping the (now correct)
		// top-left as anchor.
		if (width > 0 && height > 0)
		{
			ErrorCode resizeStatus = Utils<Facade::IGeometryFacade>()->ResizeItems(
				UIDList(newItemRef),
				Transform::PasteboardCoordinates(),
				Geometry::OuterStrokeBounds(),
				leftTop,
				Geometry::ResizeTo(width, height));
			if (resizeStatus != kSuccess)
				break;
		}

		outFrameUID = newItemRef.GetUID();
		status = kSuccess;

	} while (false);

	return status;
}


//----------------------------------------------------------------------------------------------------------------
// ReplaceTextInFrame — substitute placeholders inside a text frame's story.
//
// Used for ad templates "without visual": the client's IDMS contains a text
// frame with placeholders like "{ORDRE_ID}" / "{ORDRE_ANNONCEUR}" that we
// replace with the actual order data. Walks the story once per placeholder
// using a simple linear scan — story sizes for an ad block are tiny so the
// O(N×M) cost is irrelevant.
//----------------------------------------------------------------------------------------------------------------
ErrorCode ReplaceTextInFrame(UIDRef frameRef,
	const K2Vector<PMString>& keys, const K2Vector<PMString>& values)
{
	if (keys.size() != values.size())
		return kFailure;

	do
	{
		// Resolve the story attached to the text frame's first child column.
		InterfacePtr<IHierarchy> frameHier(frameRef, UseDefaultIID());
		if (frameHier == nil || frameHier->GetChildCount() == 0)
			break;

		IDataBase* db = frameRef.GetDataBase();
		InterfacePtr<IMultiColumnTextFrame> mcTxtFrame(db, frameHier->GetChildUID(0), UseDefaultIID());
		if (mcTxtFrame == nil)
			break;

		InterfacePtr<ITextModel> textModel(db, mcTxtFrame->GetTextModelUID(), UseDefaultIID());
		if (textModel == nil)
			break;

		// Read the full story content via TextIterator (the SDK doesn't
		// expose a one-shot GetWideString on ITextModel — we walk char by
		// char). The SDK's TextIterator is the same pattern XPage uses in
		// XPageIndexSuiteTextCSB.cpp.
		const TextIndex storyLen = textModel->TotalLength();
		if (storyLen <= 0)
			break;

		WideString storyWide;
		{
			TextIterator beginIt(textModel, 0);
			TextIterator endIt(textModel, storyLen);
			for (TextIterator it = beginIt; it != endIt; ++it)
				storyWide.Append((*it).GetValue());
		}

		PMString storyStr(storyWide);

		bool16 anyReplaced = kFalse;
		for (int32 i = 0; i < keys.size(); ++i)
		{
			const PMString& key = keys[i];
			const PMString& value = values[i];
			if (key.IsEmpty())
				continue;

			while (kTrue)
			{
				const int32 idx = storyStr.IndexOfString(key);
				if (idx < 0)
					break;
				storyStr.Remove(idx, key.NumUTF16TextChars());
				if (!value.IsEmpty())
					storyStr.Insert(value, idx);
				anyReplaced = kTrue;
			}
		}

		if (!anyReplaced)
			break; // no placeholder hit — nothing to do.

		// Atomic replace: drop everything except the trailing CR (story
		// invariant) and insert the new content. ReplaceCmd takes a
		// boost::shared_ptr<WideString> for the inserted payload.
		InterfacePtr<ITextModelCmds> txtModelCmds(textModel, UseDefaultIID());
		if (txtModelCmds == nil)
			break;

		const int32 replaceLen = (storyLen > 0) ? storyLen - 1 : 0;
		boost::shared_ptr<WideString> payload(new WideString(storyStr));
		InterfacePtr<ICommand> replaceCmd(txtModelCmds->ReplaceCmd(0, replaceLen, payload));
		if (replaceCmd != nil)
			CmdUtils::ProcessCommand(replaceCmd);

	} while (false);

	return kSuccess;
}


//----------------------------------------------------------------------------------------------------------------
// SetPageFolio — set the section start number for the page at indexofpage.
//
// If a section already starts on that page, its start number is updated via
// kModifySectionSettingsCmdBoss; otherwise a new section is created via
// kNewSectionCmdBoss. We always use Arabic numbering with no prefix/marker
// and the section is independent (continueFromPrevSection = kFalse). If the
// page is page 0 of the doc, InDesign forbids deleting the implicit first
// section, but ModifySection on it is fine.
//
// Mirrors the dual create/modify logic from Pages.cpp (Pagemakeup parser
// command [NS]); we keep it inline here so DocUtils doesn't take a runtime
// dependency on the IPageHelper boss in the PageLib plugin.
//----------------------------------------------------------------------------------------------------------------
void SetPageFolio(UIDRef docUIDRef, int32 indexofpage, int32 folio)
{
	do {
		InterfacePtr<IPageList> pageList(docUIDRef, UseDefaultIID());
		if (pageList == nil)
			break;

		UID pageUID = pageList->GetNthPageUID(indexofpage);
		if (pageUID == kInvalidUID)
			break;

		InterfacePtr<ISectionList> sectionList(docUIDRef, UseDefaultIID());
		if (sectionList == nil)
			break;

		UIDRef sectionListRef = ::GetUIDRef(sectionList);
		IDataBase* db = docUIDRef.GetDataBase();

		// Look for an existing section that already starts on this page.
		bool16 modified = kFalse;
		const int32 nbSections = sectionList->GetSectionCount();
		for (int32 i = 0; i < nbSections; ++i)
		{
			InterfacePtr<ISection> sec(sectionList->QueryNthSection(i));
			if (sec == nil)
				continue;

			if (sec->GetStartPageUID() != pageUID)
				continue;

			UID sectionUID = sectionList->GetNthSectionUID(i);

			InterfacePtr<ICommand> modifyCmd(CmdUtils::CreateCommand(kModifySectionSettingsCmdBoss));
			if (modifyCmd == nil)
				break;

			InterfacePtr<ISectionCmdData> data(modifyCmd, IID_ISECTIONCMDDATA);
			if (data == nil)
				break;

			PMString prefix(""); prefix.SetTranslatable(kFalse);
			PMString marker(""); marker.SetTranslatable(kFalse);
			data->Set(sectionListRef, pageUID, folio, &prefix, &marker,
				kArabicPageNumberingStyleServiceBoss, kFalse);
			data->SetPageNumberIncludeSectionPrefix(kFalse);

			modifyCmd->SetItemList(UIDList(db, sectionUID));
			modifyCmd->SetUndoability(ICommand::kAutoUndo);

			if (CmdUtils::ProcessCommand(modifyCmd) == kSuccess)
				modified = kTrue;
			break;
		}

		if (modified)
			break;

		// No existing section on this page → create one.
		InterfacePtr<ICommand> newCmd(CmdUtils::CreateCommand(kNewSectionCmdBoss));
		if (newCmd == nil)
			break;

		newCmd->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<ISectionCmdData> data(newCmd, IID_ISECTIONCMDDATA);
		if (data == nil)
			break;

		PMString prefix(""); prefix.SetTranslatable(kFalse);
		PMString marker(""); marker.SetTranslatable(kFalse);
		data->Set(sectionListRef, pageUID, folio, &prefix, &marker,
			kArabicPageNumberingStyleServiceBoss, kFalse);
		data->SetPageNumberIncludeSectionPrefix(kFalse);

		ErrorCode err = CmdUtils::ProcessCommand(newCmd);
		if (err != kSuccess) {
			ASSERT_FAIL("SetPageFolio -> NewSectionCmd failed");
		}

	} while (false);
}


//----------------------------------------------------------------------------------------------------------------
// RunPagemakeupString — apply a Pagemakeup TCL snippet to the front doc.
//
// kTCLParserBoss only knows how to read from a file (PrsTCLParser::SetInputFile
// takes an IDFile and creates a stream from it). To apply an in-memory
// snippet we have to roundtrip through a temp file: write the bytes,
// SetInputFile, LaunchParser, then delete the temp file. The parser uses
// the application's front document by default, which is exactly what the
// linker dialog wants after SaveAsDoc.
//
// Returns kSuccess if the parser reports success (numError == 0), kFailure
// otherwise. Even on failure we still try to delete the temp file. Caller
// is responsible for ensuring the front document is the linked one before
// calling.
//----------------------------------------------------------------------------------------------------------------
ErrorCode RunPagemakeupString(const PMString& tclContent)
{
	ErrorCode status = kFailure;

	// Build a unique temp file path under %TEMP%. GetTempFileNameA creates
	// the file as a side-effect (0-byte), which is fine — we'll overwrite
	// it with the TCL content below.
	char tempPath[1024] = {0};
#if WINDOWS
	char tempDir[MAX_PATH] = {0};
	if (::GetTempPathA(MAX_PATH, tempDir) == 0)
		return kFailure;

	if (::GetTempFileNameA(tempDir, "xrl", 0, tempPath) == 0)
		return kFailure;
#else
	// Mac/POSIX : cree un fichier temporaire unique sous $TMPDIR (defaut /tmp).
	const char* td = ::getenv("TMPDIR");
	if (td == nil || *td == 0) td = "/tmp";
	::snprintf(tempPath, sizeof(tempPath), "%s/xrlXXXXXX", td);
	int fd = ::mkstemp(tempPath);
	if (fd < 0)
		return kFailure;
	::close(fd);
#endif

	PMString tempPathPM(tempPath);
	tempPathPM.SetTranslatable(kFalse);
	IDFile tempFile = FileUtils::PMStringToSysFile(tempPathPM);

	do {
		// Scope the write stream tightly: on Windows the IPMStream impl
		// holds the underlying file handle until the InterfacePtr
		// destructs — Close() alone isn't enough. If the handle is still
		// open with write access when SetInputFile tries to reopen the
		// same path for read, CreateFileStreamRead returns nil and the
		// parser never sees the bytes. Releasing the InterfacePtr in an
		// inner block frees the handle before the parser opens the file.
		{
			InterfacePtr<IPMStream> writeStream(StreamUtil::CreateFileStreamWrite(
				tempFile, kOpenOut | kOpenTrunc, kTextType, kAnyCreator));
			if (writeStream == nil)
				break;

			std::string raw = tclContent.GetPlatformString();
			if (!raw.empty())
				writeStream->XferByte((uchar*)raw.data(), (int32)raw.size());
			writeStream->Close();
		} // writeStream Released, file handle freed

		InterfacePtr<ITCLParser> parser((ITCLParser*)::CreateObject(kTCLParserBoss, IID_ITCLPARSER));
		if (parser == nil)
			break;

		// Suppress the "TCL file processed" success alert and any
		// per-error popups — this runs as a side effect of the link
		// dialog, the user shouldn't see PageMakeUp dialogs.
		parser->SetStopDisplay(kTrue);
		parser->SetStopOnError(kFalse);

		if (!parser->SetInputFile(tempFile))
			break;

		if (parser->LaunchParser() == 0)
			status = kSuccess;

	} while (false);

	// Best-effort cleanup; if the parser kept a handle open, this just
	// silently fails and the OS reaps the file later.
#if WINDOWS
	::DeleteFileA(tempPath);
#else
	::remove(tempPath);
#endif

	return status;
}
