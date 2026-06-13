
#include "VCPlugInHeaders.h"

// Interface includes:
#include "CIdleTask.h"
#include "IIntData.h"
#include "IStringData.h"
#include "ICoreFilename.h"
#include "IStringListControlData.h"
#include "IDropDownListController.h"
#include "IPanelControlData.h"
#include "IControlView.h"
#include "ITextControlData.h"
#include "IUIDData.h"
#include "IWebServices.h"
#include "IDocument.h"
#include "ILayoutUIUtils.h"

// General includes:
#include "CAlert.h"
#include "CDialogController.h"
#include "SystemUtils.h"
#include "FileUtils.h"
#include "StreamUtil.h"
#include "DocUtils.h"
#include "WidgetID.h"
#include "PMString.h"
#include "IIntList.h"

// Project includes:
#include "XRLUIID.h"
#include "XRLID.h"
#include "XRCID.h"

/** XPBCLoginDialogController
	Methods allow for the initialization, validation, and application of dialog widget values.

	Implements IDialogController based on the partial implementation CDialogController.

	@author GD
*/


class XRailLinkerDialogController : public CDialogController
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRailLinkerDialogController(IPMUnknown* boss);

	/**
		Initialize each widget in the dialog with its default value.
		Called when the dialog is opened.
	*/
	virtual void InitializeDialogFields(IActiveContext* dlgContext);

	/**
		Validate the values in the widgets.
		By default, the widget with ID kOKButtonWidgetID causes
		ValidateFields to be called. When all widgets are valid,
		ApplyFields will be called.
		@return kDefaultWidgetId if all widget values are valid, WidgetID of the widget to select otherwise.

	*/
	virtual WidgetID ValidateDialogFields(IActiveContext* myContext);

	/**
		Retrieve the values from the widgets and act on them.
		@param widgetId identifies the widget on which to act.
	*/
	virtual void ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);

	PMString baseName, serverAdress;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XRailLinkerDialogController, kXRLUILinkerDialogControllerImpl)

/* Constructor
*/
XRailLinkerDialogController::XRailLinkerDialogController(IPMUnknown* boss) : CDialogController(boss)
{
}

/* ApplyFields
*/
void XRailLinkerDialogController::InitializeDialogFields(IActiveContext* dlgContext)
{
	//on recupere l'utilisateur
	InterfacePtr<IStringData> sessionID(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);
	PMString userStr = sessionID->GetString();
	PMString::ConversionError convError = PMString::kNoError;
	int32 userID = userStr.GetAsNumber(&convError);

	do {
		// Initialize status list, getting it from XRail server
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

		//on recupere la premiere liste
		IControlView* controlView = panelControlData->FindWidget(kXRLUILevel1ListBoxWidgetID);

		InterfacePtr<IStringListControlData> stringListControlData(controlView, UseDefaultIID());
		stringListControlData->Clear(kFalse, kFalse);

		IDocument* theDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();

		int32 idPage = GetPageID(::GetUIDRef(theDoc), 0, baseName);
		serverAdress = GetGoodUrlXR(baseName);

		InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

		K2Vector<PMString> nomstitres;
		K2Vector<int32> idstitres;

		bool connexionOK = xrailHTTP->GetListeTitres_v2(serverAdress, userID, nomstitres, idstitres);
		if (!connexionOK)
		{
			CAlert::ErrorAlert(kXRLUIErrorWSGetLevelKey);
			break;
		}

		// Get page status
		InterfacePtr<IStringData> pageStatus(this, UseDefaultIID());

		// Prepend a placeholder ("Choisissez un titre") so the dialog opens
		// without an actual title pre-selected — the auto-Select(0) below
		// doesn't broadcast kPopupChangeStateMessage, so a real title at
		// position 0 would just sit there with empty cascade lists. The
		// placeholder makes that state explicit and forces the user to
		// pick a real title (which DOES fire the cascade).
		PMString placeholder(kXRLUIChooseTitlePromptKey, PMString::kTranslateDuringCall);
		placeholder.SetTranslatable(kFalse);
		stringListControlData->AddString(placeholder,
			IStringListControlData::kEnd,
			kFalse,
			kFalse);
		idstitres.insert(idstitres.begin(), -1);  // sentinel ID matching the placeholder slot

		const int32 selectedIndex = IDropDownListController::kBeginning;
		for (int32 i = 0; i < nomstitres.size(); ++i)
		{
			PMString titrename = nomstitres[i];

			stringListControlData->AddString(titrename,
				IStringListControlData::kEnd,
				kFalse, 	// Invalidate?
				kFalse);	// Notify of change?
		}

		InterfacePtr<IIntList> pageListID(controlView, IID_IINTLIST);
		pageListID->SetIntList(idstitres);

		InterfacePtr<IDropDownListController> dropDownListController(controlView, UseDefaultIID());

		if (stringListControlData->Length() > 0)
			dropDownListController->Select(selectedIndex);

		//on vide les listes 2, 3 et 4
		IControlView* controlView2 = panelControlData->FindWidget(kXRLUILevel2ListBoxWidgetID);
		InterfacePtr<IStringListControlData> stringListControlData2(controlView2, UseDefaultIID());
		stringListControlData2->Clear(kFalse, kFalse);
		InterfacePtr<IIntList> pageListID2(controlView2, IID_IINTLIST);
		pageListID2->Clear();

		IControlView* controlView3 = panelControlData->FindWidget(kXRLUILevel3ListBoxWidgetID);
		InterfacePtr<IStringListControlData> stringListControlData3(controlView3, UseDefaultIID());
		stringListControlData3->Clear(kFalse, kFalse);
		InterfacePtr<IIntList> pageListID3(controlView3, IID_IINTLIST);
		pageListID3->Clear();

		IControlView* controlView4 = panelControlData->FindWidget(kXRLUILevel4ListBoxWidgetID);
		InterfacePtr<IStringListControlData> stringListControlData4(controlView4, UseDefaultIID());
		stringListControlData4->Clear(kFalse, kFalse);
		InterfacePtr<IIntList> pageListID4(controlView4, IID_IINTLIST);
		pageListID4->Clear();

	} while (false);
}

/* ValidateFields
*/
WidgetID XRailLinkerDialogController::ValidateDialogFields(IActiveContext* dlgContext)
{
	WidgetID 		badWidgetID = kInvalidWidgetID;

	do {
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		IControlView* controlView1 = panelControlData->FindWidget(kXRLUILevel1ListBoxWidgetID);
		InterfacePtr<IStringListControlData> stringListControlData1(controlView1, UseDefaultIID());
		if (stringListControlData1->Length() == 0)
		{
			badWidgetID = kXRLUILevel1ListBoxWidgetID;
			break;
		}

		// The first entry is the "Choisissez un titre" placeholder (id = -1).
		// Refuse to apply if the user hasn't picked a real title yet —
		// otherwise levels 2/3/4 are empty and the next checks would fail
		// with a less informative widget focus.
		{
			InterfacePtr<IDropDownListController> dropCtl1(controlView1, UseDefaultIID());
			InterfacePtr<IIntList> ids1(controlView1, IID_IINTLIST);
			const int32 sel = dropCtl1 ? dropCtl1->GetSelected() : IDropDownListController::kNoSelection;
			K2Vector<int32> idVec = ids1 ? ids1->GetIntList() : K2Vector<int32>();
			if (sel == IDropDownListController::kNoSelection ||
			    sel < 0 || sel >= idVec.size() || idVec[sel] < 0)
			{
				badWidgetID = kXRLUILevel1ListBoxWidgetID;
				break;
			}
		}

		IControlView* controlView2 = panelControlData->FindWidget(kXRLUILevel2ListBoxWidgetID);
		InterfacePtr<IStringListControlData> stringListControlData2(controlView2, UseDefaultIID());
		if (stringListControlData2->Length() == 0)
		{
			badWidgetID = kXRLUILevel2ListBoxWidgetID;
			break;
		}

		IControlView* controlView3 = panelControlData->FindWidget(kXRLUILevel3ListBoxWidgetID);
		InterfacePtr<IStringListControlData> stringListControlData3(controlView3, UseDefaultIID());
		if (stringListControlData3->Length() == 0)
		{
			badWidgetID = kXRLUILevel3ListBoxWidgetID;
			break;
		}

		IControlView* controlView4 = panelControlData->FindWidget(kXRLUILevel4ListBoxWidgetID);
		InterfacePtr<IStringListControlData> stringListControlData4(controlView4, UseDefaultIID());
		if (stringListControlData4->Length() == 0)
		{
			badWidgetID = kXRLUILevel4ListBoxWidgetID;
			break;
		}

	} while (false);
	return badWidgetID;
}

/* ApplyFields
*/
void XRailLinkerDialogController::ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId)
{
	// Get webservice client boss and set server address for it
	InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));

	do {
		//on commence par recuperer l'ID de la page choisie
		K2Vector<int32> listLevelID;
		int32 selectedLine, pageID;

		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		IControlView* controlView = panelControlData->FindWidget(kXRLUILevel4ListBoxWidgetID);
		InterfacePtr<IIntList> pageListID(controlView, IID_IINTLIST);
		InterfacePtr<IDropDownListController> dropDownListController(controlView, UseDefaultIID());
		selectedLine = dropDownListController->GetSelected();
		listLevelID = pageListID->GetIntList();
		pageID = listLevelID[selectedLine];

		//on recupere le document qui est au premier plan
		IDocument* frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		//on recupere le nb de pages de ce document
		int16 	nbPages;
		UIDRef myRef;
		if (!frontDoc)
		{
			// Get concerned document and update status of its pages with the choice of the user
			InterfacePtr<IUIDData> theDoc(this, UseDefaultIID());
			myRef = theDoc->GetRef();
		}
		else
			myRef = ::GetUIDRef(frontDoc);

		nbPages = GetNbPages(myRef);

		//appel du webservice, on recupere la liste de tous les ID
		K2Vector<int32> newIDS;
		K2Vector<int32> newFolios;
		K2Vector<PMString> newRoulettes;
		serverAdress = GetGoodUrlXR(baseName);
		PMString newDocName;
		bool connexionOK = xrailHTTP->LinkDocument_v2(serverAdress, pageID, newIDS, newFolios, newRoulettes, newDocName);
		if (!connexionOK)
		{
			CAlert::ErrorAlert(kXRLUIErrorWSLinkPagesKey);
			break;
		}
		
		// Stamp each page with its IDPage and folio. idpages/folios are parallel
		// to the document's pages — same length expected, but the server response
		// is parsed from a fragile JSON-encoded string (cf. LinkDocument_v2), so a
		// short/empty list must NOT be indexed past its end. newIDS[i] was read
		// unguarded here: one ID short of nbPages = out-of-bounds read = crash
		// before SaveAsDoc, leaving the previous (unlink) file on disk. Guard both.
		for (int16 i = 0; i < nbPages; i++)
		{
			if (i < newIDS.size())
				SetPageID(myRef, i, newIDS[i], baseName);
			if (i < newFolios.size())
				SetPageFolio(myRef, i, newFolios[i]);
		}

		// IMPORTANT : on sauvegarde AVANT d'appliquer les roulettes.
		// RunPagemakeupString lance le parser TCL Pagemakeup, qui laisse ensuite
		// le document dans un etat ou IDocFileHandler::CanSaveAs renvoie kFalse :
		// un SaveAs effectue apres les roulettes echoue silencieusement et le
		// document reste sous son ancien nom (fichier "Unlink_..."). On fait donc
		// le SaveAs d'abord (comme le chemin unlink, on ne tente le SaveAs que si
		// le serveur a bien renvoye un chemin).
		if (newDocName.Compare(kFalse, "") != 0) {
			SaveAsDoc(myRef, newDocName);
		}
		else
			CAlert::ErrorAlert(kXRLUIErrorWSLinkPagesKey);

		// Roulettes are Pagemakeup TCL snippets ([HS=…]…[HR]) the server
		// attaches to the link payload.
		for (int32 i = 0; i < newRoulettes.size(); ++i)
		{
			RunPagemakeupString(newRoulettes[i]);
		}

	} while (false);
}

// End, XPBCLoginDialogController.cpp.





