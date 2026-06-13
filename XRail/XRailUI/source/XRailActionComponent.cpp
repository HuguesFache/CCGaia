//========================================================================================
//  
//  $File: //depot/indesign_3.0/gm/source/sdksamples/basicdialog/BscDlgActionComponent.cpp $
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
#include "ISession.h"
#include "IDialog.h"
#include "IBoolData.h"
#include "ILayoutUIUtils.h"
#include "IDocument.h"
#include "IActionStateList.h"
#include "IWebServices.h"
#include "IPageList.h"
#include "IStringData.h"
#include "IOutputPages.h"
#include "IUIFlagData.h"
#include "ISysFileData.h"
#include "IPDFExportPrefs.h"
#include "IPDFExptStyleListMgr.h"
#include "FileUtils.h"
#include "IWorkspace.h"
#include "IWindow.h"
#include "IDocFileHandler.h"
#include "IDocumentUtils.h"
#include "IWorkspace.h"
#include "IXrailPrefsData.h"

// General includes:
#include "CActionComponent.h"
#include "CAlert.h"
#include "Utils.h"
#include "RsrcSpec.h"
#include "DocUtils.h"

// Project includes:
#include "XRLUIID.h"
#include "XRLID.h"

/** Implements IActionComponent based on
	the partial implementation CActionComponent; executes the actions that are executed when the plug-in's
	menu items are selected.

	@ingroup basicdialog
	@author Rodney Cook
*/
class XRailActionComponent : public CActionComponent
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRailActionComponent(IPMUnknown* boss);

	/**
		The action component should do the requested action.
		This is where the menu item's action is taken.
		When a menu item is selected, the action manager determines
		which plug-in is responsible for it, and calls its DoAction
		with the ID for the menu item chosen.

		@param actionID identifies the menu item that was selected.
		@see CActionComponent::DoAction
		*/
	void DoAction(IActiveContext* ac, ActionID actionID, GSysPoint mousePoint = kInvalidMousePoint, IPMUnknown* widget = nil);

	/**
		Called to determine if an action should be enabled.
		This call enables the actions that pop up the search image dialog
		@see CActionComponent::UpdateActionStates
	*/
	void UpdateActionStates(IActiveContext* ac, IActionStateList* listToUpdate, GSysPoint mousePoint = kInvalidMousePoint, IPMUnknown* widget = nil);

private:
	/** Pops this plug-in's about box. */
	void DoAbout();

	/** affichage du dialogue de preference*/
	void DoTraitement();

	/** Affichage du dialogue de connection*/
	void DoConnection();

	/** activation des vignettes */
	void DoEnablePreview();

	/** generation des PDFs */
	void DoGeneratePDF(bool16 externe);

	/** generation des PDFs */
	void DoGeneratePDFMono();

	/** Dialogue de liaison avec XRail */
	void DoLink();

	/** Supprime la liaison d'XRail */
	void DoUnLink();

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XRailActionComponent, kXRLUIActionComponentImpl)

/* XRailActionComponent Constructor
*/
XRailActionComponent::XRailActionComponent(IPMUnknown* boss)
	: CActionComponent(boss)
{
}


/* DoAction
*/
void XRailActionComponent::DoAction(IActiveContext* ac, ActionID actionID, GSysPoint mousePoint, IPMUnknown* widget)
{
	InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
	InterfacePtr<IXRailPrefsData>  xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));

	switch (actionID.Get())
	{
	case kXRLUIAboutActionID:
	{
		this->DoAbout();
		break;
	}

	case kXRLUIPreferencesActionID:
	{
		this->DoTraitement();
		break;
	}

	case kXRLUIConnectionActionID:
	{
		this->DoConnection();
		break;
	}

	case kXRLUIEnablePreviewActionID:
	{
		this->DoEnablePreview();
		break;
	}

	case kXRLUIExternalPDFActionID:
	{
		if (xrailPrefsData->GetPDFMultipages() == 0) {
			this->DoGeneratePDFMono();
		}
		else {
			this->DoGeneratePDF(true);
		}
		break;
	}

	case kXRLUIXRailLinkActionID:
	{
		this->DoLink();
		break;
	}

	case kXRLUIXRailUnLinkActionID:
	{
		this->DoUnLink();
		break;
	}

	default:
	{
		break;
	}
	}
}

/* UpdateActionStates
*/
void XRailActionComponent::UpdateActionStates(IActiveContext* ac, IActionStateList* listToUpdate, GSysPoint mousePoint, IPMUnknown* widget)
{
	if (ac == nil)
		return;

	// Check droit des utilisateurs 
	InterfacePtr<IBoolData> createPDFBool(GetExecutionContextSession(), IID_ICREATEPDFBOOL);
	InterfacePtr<IBoolData> createPreviewBool(GetExecutionContextSession(), IID_ICREATEPREVIEWBOOL);
	InterfacePtr<IStringData> loginID(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);

	for (int32 iter = 0; iter < listToUpdate->Length(); iter++)
	{
		ActionID actionID = listToUpdate->GetNthAction(iter);
		if (loginID->GetString() == "") {
			if (actionID != kXRLUIAboutActionID && actionID != kXRLUIPreferencesActionID && actionID != kXRLUIConnectionActionID) {
				listToUpdate->SetNthActionState(iter, kFalse);
			}
		}
		else {
			if (actionID == kXRLUIExternalPDFActionID) {
				listToUpdate->SetNthActionState(iter, kEnabledAction && createPDFBool->Get());
			}
			else if (actionID == kXRLUIXRailLinkActionID) {
				listToUpdate->SetNthActionState(iter, kEnabledAction && createPreviewBool->Get());
			}
			else if (actionID == kXRLUIXRailUnLinkActionID) {
				listToUpdate->SetNthActionState(iter, kEnabledAction && createPreviewBool->Get());
			}
			else if (actionID == kXRLUIEnablePreviewActionID) {
				IDocument* frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
				if (frontDoc == nil)
					continue;
				if (createPreviewBool->Get()) {
					InterfacePtr<IBoolData>  makePreview(frontDoc, IID_IMAKEPREVIEW);
					if (makePreview->Get())
						listToUpdate->SetNthActionState(iter, kSelectedAction | kEnabledAction);
					else
						listToUpdate->SetNthActionState(iter, kEnabledAction);
				}
				else
					listToUpdate->SetNthActionState(iter, kEnabledAction);
			}
		}
	}
}

/* DoAbout
*/
void XRailActionComponent::DoAbout()
{

	InterfacePtr<IBoolData> createPDFBool(GetExecutionContextSession(), IID_ICREATEPDFBOOL);
	InterfacePtr<IBoolData> createPreviewBool(GetExecutionContextSession(), IID_ICREATEPREVIEWBOOL);
	if (createPDFBool->Get())
		CAlert::InformationAlert("Creation des pdfs active");
	else
		CAlert::InformationAlert("Creation des pdfs inactive");
	if (createPreviewBool->Get())
		CAlert::InformationAlert("Creation des vignettes active");
	else
		CAlert::InformationAlert("Creation des vignettes inactive");


	CAlert::ModalAlert
	(
		kXRLUIAboutBoxStringKey,		// Alert string
		kOKString, 						// OK button
		kNullString, 					// No second button
		kNullString, 					// No third button
		1,								// Set OK button to default
		CAlert::eInformationIcon		// Information icon.
	);
}


void XRailActionComponent::DoTraitement()
{
	IDialog* dialog = CreateNewDialog(kXRLUIPluginID, kSDKDefDialogResourceID);
	if (dialog)
	{
		InterfacePtr<IWindow> dialogWindow(dialog, UseDefaultIID());
		dialogWindow->SetTitle(kXRLUIPrefsDialogTitleKey);

		dialog->Open();
	}
}

void XRailActionComponent::DoLink()
{
	IDialog* dialog = CreateNewDialog(kXRLUIPluginID, kXRLUILinkerDialogResourceID);
	if (dialog)
		dialog->Open();

}
// Cette fonction supprime tous les ids de toutes les pages, afin que le document ne soit plus lie a XRail
void XRailActionComponent::DoUnLink()
{
	IDocument* frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (frontDoc)
	{
		int32 nbPages = GetNbPages(::GetUIDRef(frontDoc));
		PMString baseName;

		// GD 1.9.5 ++
		// On recupere l'id de la premiere page du document
		UIDRef docRef = ::GetUIDRef(frontDoc);
		int32 idPage = GetPageID(docRef, 0, baseName);
		// GD 1.9.5 --

		for (int16 i = 0; i < nbPages; i++) {
			SetPageID(::GetUIDRef(frontDoc), i, 0, "");
		}

		// GD 1.9.5 ++
		// Sauvegarde du doc delie avec nouveau chemin/nom (donne par XRail)
		// Quand ce webservice est appele, XRail deverrouille aussi la page originale
		PMString newDocName, serverAdress;
		serverAdress = GetGoodUrlXR(baseName);

		// interface Webservice
		InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

		do {
			bool connexionOK = xrailHTTP->UnlinkDocument_v2(serverAdress, idPage, newDocName);
			if (!connexionOK)
			{
				CAlert::ErrorAlert(kXRLUIErrorWSLinkPagesKey);
				break;
			}
			if (newDocName.Compare(kFalse, "") != 0) {
				SaveAsDoc(docRef, newDocName);
			}
		} while (false);
	}

}

void XRailActionComponent::DoConnection()
{
	IDialog* dialog = CreateNewDialog(kXRLUIPluginID, kXRLUILoginDialogResourceID);
	if (dialog)
		dialog->Open();
}

void XRailActionComponent::DoEnablePreview()
{
	do
	{
		IDocument* frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if (frontDoc == nil)
			break;

		InterfacePtr<IBoolData>  makePreview(frontDoc, IID_IMAKEPREVIEW);

		InterfacePtr<ICommand> setMakePreviewCmd(CmdUtils::CreateCommand(kXRLSetMakePreviewCmdBoss));
		setMakePreviewCmd->SetItemList(UIDList(frontDoc));
		setMakePreviewCmd->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<IBoolData> cmdData(setMakePreviewCmd, IID_IBOOLDATA);
		cmdData->Set(!makePreview->Get());

		CmdUtils::ProcessCommand(setMakePreviewCmd);

	} while (kFalse);

}

void XRailActionComponent::DoGeneratePDF(bool16 externe)
{
	PMString error = kNullString;

	//on recupere l'utilisateur
	InterfacePtr<IStringData> sessionID(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);
	PMString userStr = sessionID->GetString();
	PMString::ConversionError convError = PMString::kNoError;
	int32 userID = userStr.GetAsNumber(&convError);

	//interface Webservice
	InterfacePtr<IWebServices> XRailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

	ErrorCode status = kFailure;

	// Create a PDF Export Command
	do
	{
		PMString pdfStyleName = kNullString;
		PMString docName = kNullString;
		PMString baseName = kNullString, serverAdress;

		// recuperer front doc
		IDocument* frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if (!frontDoc)
		{
			status = kSuccess;
			break;
		}

		//on recupere l'id de la premiere page du document
		int32 idPage = GetPageID(::GetUIDRef(frontDoc), 0, baseName);
		serverAdress = GetGoodUrlXR(baseName);
		//on commence par recuperer le nom du pdf et du style
		PMString pdfFileName, profilPDF;

		bool connexionOK = XRailHTTP->GetPDFFileName_v2(serverAdress, idPage, profilPDF, pdfFileName);
		if (!connexionOK) {
			error = PMString(kXRLErrorGetPDFFileNameKey);
			break;
		}

		InterfacePtr<ICommand> PDFExportCmd(CmdUtils::CreateCommand(kPDFExportCmdBoss));
		// Get the pages to be exported and set them on the command's output pages interface
		InterfacePtr<IPageList> pageList(frontDoc, UseDefaultIID());
		int32 startPage = 1;
		int32 endPage = pageList->GetPageCount();

		UIDList pageUIDList(::GetDataBase(frontDoc));
		for (int32 i = startPage - 1; i < endPage; i++)
		{
			UID uidPage = pageList->GetNthPageUID(i);
			pageUIDList.Append(uidPage);
		}
		InterfacePtr<IOutputPages> PDFOutputPages(PDFExportCmd, UseDefaultIID());
		PDFOutputPages->InitializeFrom(pageUIDList, kFalse);


		// variable pour le style d'export : pdfStyleName				

		// Find the pdf export style UIDRef
		InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
		InterfacePtr<IPDFExptStyleListMgr> PDFExptStyleListMgr(workspace, IID_IPDFEXPORTSTYLELISTMGR);
		int32 nStyleIndex = PDFExptStyleListMgr->GetStyleIndexByName(profilPDF);

		if (nStyleIndex == -1)
		{
			error = PMString(kXRLErrorPDFStyleNotFoundKey);
			break;
		}

		UIDRef styleRef = PDFExptStyleListMgr->GetNthStyleRef(nStyleIndex);

		InterfacePtr<IPDFExportPrefs> PDFExportCmdPrefs(PDFExportCmd, UseDefaultIID());

		// Get the export preferences interface for our style and copy it into the command's interface
		InterfacePtr<IPDFExportPrefs> PDFStylePrefs(styleRef, UseDefaultIID());
		PDFExportCmdPrefs->CopyPrefs(PDFStylePrefs);

		// outputFile is the file to which the PDF will be exported - You have to set its value
		IDFile outputFile = FileUtils::PMStringToSysFile(pdfFileName);

		InterfacePtr<ISysFileData> PDFFileData(PDFExportCmd, UseDefaultIID());
		PDFFileData->Set(outputFile);

		// No UI display
		InterfacePtr<IUIFlagData> uiFlagCmdData(PDFExportCmd, IID_IUIFLAGDATA);
		uiFlagCmdData->Set(kSuppressUI);

		bool16 showAlerts = CAlert::GetShowAlerts();
		CAlert::SetShowAlerts(kFalse);

		// Process the command
		status = CmdUtils::ProcessCommand(PDFExportCmd);

		CAlert::SetShowAlerts(kTrue);

		if (status != kSuccess || !FileUtils::DoesFileExist(outputFile))
		{
			status = kFailure;
			error = PMString(kXRLErrorPDFNotCreatedStringKey);
			break;
		}

		//le traitement est termine, on previent XRail
		connexionOK = XRailHTTP->MovePDFFile_v2(serverAdress, idPage, pdfFileName, userID);
		if (!connexionOK) {
			status = kFailure;
			error = PMString(kXRLErrorMovePDFFileKey);
			break;
		}

	} while (kFalse);

	if (status != kSuccess)
	{
		CAlert::ErrorAlert(error);
	}
}

void XRailActionComponent::DoGeneratePDFMono()
{
	PMString error = kNullString;

	//on recupere l'utilisateur
	InterfacePtr<IStringData> sessionID(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);
	PMString userStr = sessionID->GetString();
	PMString::ConversionError convError = PMString::kNoError;
	int32 userID = userStr.GetAsNumber(&convError);

	//interface Webservice
	InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

	ErrorCode status = kFailure;

	// Create a PDF Export Command
	do
	{
		PMString pdfStyleName = kNullString;
		PMString docName = kNullString;
		PMString baseName = kNullString, serverAdress;
		serverAdress = GetGoodUrlXR(baseName);


		// recuperer front doc
		IDocument* frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if (!frontDoc)
		{
			status = kSuccess;
			break;
		}
		//on boucle sur chaque page
		int32 nbPages = GetNbPages(::GetUIDRef(frontDoc));
		K2Vector<int32> tabIDPages;
		for (int16 i = 0; i < nbPages; i++) {
			int32 idPage = GetPageID(::GetUIDRef(frontDoc), i, baseName);
			if (idPage > 0) {
				//on commence par recuperer le nom du pdf et du style
				PMString pdfFileName, profilPDF;

				bool connexionOK = xrailHTTP->GetPDFFileName_v2(serverAdress, idPage, profilPDF, pdfFileName);
				if (!connexionOK) {
					error = PMString(kXRLErrorGetPDFFileNameKey);
					break;
				}

				InterfacePtr<ICommand> PDFExportCmd(CmdUtils::CreateCommand(kPDFExportCmdBoss));
				// Get the pages to be exported and set them on the command's output pages interface
				InterfacePtr<IPageList> pageList(frontDoc, UseDefaultIID());
				int32 startPage = 1;
				int32 endPage = pageList->GetPageCount();

				UIDList pageUIDList(::GetDataBase(frontDoc));
				UID uidPage = pageList->GetNthPageUID(i);
				pageUIDList.Append(uidPage);

				InterfacePtr<IOutputPages> PDFOutputPages(PDFExportCmd, UseDefaultIID());
				PDFOutputPages->InitializeFrom(pageUIDList, kFalse);


				// variable pour le style d'export : pdfStyleName

				// Find the pdf export style UIDRef
				InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
				InterfacePtr<IPDFExptStyleListMgr> PDFExptStyleListMgr(workspace, IID_IPDFEXPORTSTYLELISTMGR);
				int32 nStyleIndex = PDFExptStyleListMgr->GetStyleIndexByName(profilPDF);

				if (nStyleIndex == -1)
				{
					error = PMString(kXRLErrorPDFStyleNotFoundKey);
					break;
				}

				UIDRef styleRef = PDFExptStyleListMgr->GetNthStyleRef(nStyleIndex);

				InterfacePtr<IPDFExportPrefs> PDFExportCmdPrefs(PDFExportCmd, UseDefaultIID());

				// Get the export preferences interface for our style and copy it into the command's interface
				InterfacePtr<IPDFExportPrefs> PDFStylePrefs(styleRef, UseDefaultIID());
				PDFExportCmdPrefs->CopyPrefs(PDFStylePrefs);

				// outputFile is the file to which the PDF will be exported - You have to set its value
				IDFile outputFile = FileUtils::PMStringToSysFile(pdfFileName);

				InterfacePtr<ISysFileData> PDFFileData(PDFExportCmd, UseDefaultIID());
				PDFFileData->Set(outputFile);

				// No UI display
				InterfacePtr<IUIFlagData> uiFlagCmdData(PDFExportCmd, IID_IUIFLAGDATA);
				uiFlagCmdData->Set(kSuppressUI);

				bool16 showAlerts = CAlert::GetShowAlerts();
				CAlert::SetShowAlerts(kFalse);

				// Process the command
				status = CmdUtils::ProcessCommand(PDFExportCmd);

				CAlert::SetShowAlerts(kTrue);
				if (status != kSuccess || !FileUtils::DoesFileExist(outputFile))
				{
					status = kFailure;
					error = PMString(kXRLErrorPDFNotCreatedStringKey);
					break;
				}
				//le traitement est termine, on previent XRail
				connexionOK = xrailHTTP->MovePDFFile_v2(serverAdress, idPage, pdfFileName, userID);
				if (!connexionOK) {
					status = kFailure;
					error = PMString(kXRLErrorMovePDFFileKey);
					break;
				}
			}
		}


	} while (kFalse);

	if (status != kSuccess)
	{
		CAlert::ErrorAlert(error);
	}
}



