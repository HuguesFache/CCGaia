

#include "VCPluginHeaders.h"

// Interface includes
#include "IControlView.h"
#include "ISubject.h"
#include "IPanelControlData.h"
#include "IDropDownListController.h"
#include "IStringListControlData.h"
#include "IXPageMgrAccessor.h"
#include "IArticleDataModel.h"
#include "IStringListData.h"
#include "IStringData.h"
#include "ITextControlData.h"
#include "IWebServices.h"
#include "IXPGPreferences.h"
#include "ITriStateControlData.h"
#include "ISysFileData.h"
#include "ITreeViewMgr.h"
#include "ITreeViewController.h"
#include "IIntData.h"
#include "ISelectionManager.h"
#include "DebugClassUtils.h"
#include "IHierarchy.h"
#include "ISpread.h"
#include "IXPageUtils.h"
#include "IActiveContext.h"
#include "ISelectionUtils.h"
#include "IXPageSuite.h"
#include "IGeometry.h"
#include "TransformTypes.h"
#include "TransformUtils.h"
#include "IDataBase.h"
#include "ILayoutUIUtils.h"
#include "IDocument.h"
#include "ISpreadList.h"
#include "IXRailPageSlugData.h"

// General includes
#include "CDialogObserver.h"
#include "Utils.h"
#include "FileUtils.h"
#include "CAlert.h"
#include "ErrorUtils.h"
#include "XMLDefs.h"

// Project includes
#include "XPageUIUtils.h"
#include "XPGUIID.h"
#include "XPGID.h"
#include "XRLID.h"

class XPGUILinkArtDialogObserver : public CDialogObserver{

public:

	XPGUILinkArtDialogObserver( IPMUnknown* boss );
	virtual ~XPGUILinkArtDialogObserver() ;	
	
	void AutoAttach();	
	void AutoDetach();	
	void Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy);

private:

	void AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID);
	void DetachWidget(const InterfacePtr<IPanelControlData>&, const WidgetID& widgetID, const PMIID& interfaceID);

	void HandleFormeSelection();
	
	// Gestion des cartons
	void FillClasseurList();
	void FillCartonList();

	bool8 firstInit;

	// Carton par defaut issu du XML de l'article (DossierCarton -> classeur,
	// FichierCarton -> carton). applyDefaults n'est vrai que pour le premier
	// remplissage des listes ; les changements de classeur ulterieurs faits par
	// l'utilisateur retombent sur la selection du premier element.
	PMString defaultClasseur, defaultCarton;
	bool8 applyDefaults;
};

CREATE_PMINTERFACE(XPGUILinkArtDialogObserver, kXPGUILinkArtDialogObserverImpl)

/* Constructor
*/
XPGUILinkArtDialogObserver::XPGUILinkArtDialogObserver( IPMUnknown* boss ) :
							CDialogObserver(boss), firstInit(kTrue), applyDefaults(kFalse)
{
}

/* Destructor
*/
XPGUILinkArtDialogObserver::~XPGUILinkArtDialogObserver()
{
}

/* AutoAttach
*/
void XPGUILinkArtDialogObserver::AutoAttach()
{
	CDialogObserver::AutoAttach();

	do
	{
		// Initialise widget state.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
			break;
	
		// Observes widgets		
		this->AttachWidget(panelControlData, kXPGUIClasseurListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		this->AttachWidget(panelControlData, kXPGUICartonListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		
		// Lecture des eventuelles valeurs de carton par defaut dans le XML de
		// l'article (elements <DossierCarton> / <FichierCarton>), posees sur le
		// boss du dialogue par DisplayLinkArticleDialog. A relire a CHAQUE
		// ouverture : le boss de l'observateur est reutilise d'un glisser a
		// l'autre, donc se limiter au premier appel figerait le choix.
		defaultClasseur = kNullString;
		defaultCarton = kNullString;
		InterfacePtr<IStringData> articleXMLFileData (this, IID_IARTICLEXMLFILEDATA);
		if(articleXMLFileData != nil){
			PMString xmlPath = articleXMLFileData->GetString();
			if(xmlPath != kNullString){
				IDFile articleFile = FileUtils::PMStringToSysFile(xmlPath);
				Utils<IXPageUtils>()->GetArticleCartonDefaults(articleFile, defaultClasseur, defaultCarton);
			}
		}
		applyDefaults = (defaultClasseur != kNullString || defaultCarton != kNullString);

		FillClasseurList();
		firstInit = kFalse;

			
	} while(false);
	
}

/* AutoDetach
*/
void XPGUILinkArtDialogObserver::AutoDetach()
{
	CDialogObserver::AutoDetach();

	do
	{
		// Detach widget observers.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
			break;	

		this->DetachWidget(panelControlData, kXPGUIClasseurListWidgetID,	IID_ISTRINGLISTCONTROLDATA);
		this->DetachWidget(panelControlData, kXPGUICartonListWidgetID,	IID_ISTRINGLISTCONTROLDATA);
		
	} while(false);
}

/* Update
*/
void XPGUILinkArtDialogObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{	
	CDialogObserver::Update(theChange, theSubject, protocol, changedBy);

	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());
	
	if ((protocol == IID_ISTRINGLISTCONTROLDATA) && (theChange == kPopupChangeStateMessage))
	{
		if(controlView != nil)
		{
			WidgetID widgetID = controlView->GetWidgetID();	
			switch(widgetID.Get())
			{
				case kXPGUIClasseurListWidgetID:
					{	
						FillCartonList();
						HandleFormeSelection();			
					}
					break;

				case kXPGUICartonListWidgetID:						
					this->HandleFormeSelection();				
					break;


				default :
					break;
			}
		}
	}		
}		


/* AttachWidget
*/
void XPGUILinkArtDialogObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
{
	do
	{
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)
			break;	

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
			break;		
		subject->AttachObserver(this, interfaceID);
	}
	while (false);
}

/* DetachWidget
*/
void XPGUILinkArtDialogObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
{
	do
	{
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)
			break;	

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
			break;
		subject->DetachObserver(this, interfaceID);
	}
	while (false);
}

void XPGUILinkArtDialogObserver::FillClasseurList(){

	// Access forme model to get classeur list
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IFormeDataModel> model (pageMrgAccessor->QueryFormeDataModel());
	K2Vector<PMString> classeurList = model->GetClasseurList();

	// Fill drop down list widget
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

	// Si l'article fournit un classeur par defaut present dans la liste, le
	// pre-selectionner ; sinon, selection du premier element (comportement par defaut).
	PMString classeurToSelect = kNullString;
	if(applyDefaults && defaultClasseur != kNullString){
		for(int32 i = 0 ; i < classeurList.size() ; ++i){
			if(classeurList[i] == defaultClasseur){
				classeurToSelect = defaultClasseur;
				break;
			}
		}
	}

	if(classeurToSelect != kNullString)
		XPageUIUtils::FillDropdownlistAndSelect(panelControlData,kXPGUIClasseurListWidgetID,classeurList, classeurToSelect, kFalse);
	else
		XPageUIUtils::FillDropdownlist(panelControlData,kXPGUIClasseurListWidgetID,classeurList, kFalse);

	// Fill carton list
	FillCartonList();
}

void XPGUILinkArtDialogObserver::FillCartonList(){

	// Get selected classeur
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	 
	InterfacePtr<IDropDownListController> classeurListWidgetController (panelControlData->FindWidget(kXPGUIClasseurListWidgetID), UseDefaultIID());		
	int32 selectedIndex = classeurListWidgetController->GetSelected();

	K2Vector<PMString> formeList;
	if(selectedIndex != IDropDownListController::kNoSelection){
		
        InterfacePtr<IStringListControlData> classeurListData (classeurListWidgetController, UseDefaultIID());		
		PMString selectedClasseur = classeurListData->GetString(selectedIndex);     

		// Access forme model to get forme list
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IFormeDataModel> model (pageMrgAccessor->QueryFormeDataModel());
		
		formeList = model->GetFormeList(selectedClasseur);
 	}

	// Pre-selection du carton par defaut, uniquement lors du chargement initial
	// et seulement s'il existe dans la liste du classeur courant.
	PMString cartonToSelect = kNullString;
	if(applyDefaults && defaultCarton != kNullString){
		for(int32 i = 0 ; i < formeList.size() ; ++i){
			if(formeList[i] == defaultCarton){
				cartonToSelect = defaultCarton;
				break;
			}
		}
	}

	// Fill drop down list widget
	if(cartonToSelect != kNullString)
		XPageUIUtils::FillDropdownlistAndSelect(panelControlData,kXPGUICartonListWidgetID,formeList, cartonToSelect, kFalse);
	else
		XPageUIUtils::FillDropdownlist(panelControlData,kXPGUICartonListWidgetID,formeList, kFalse);

	// Les valeurs par defaut ne s'appliquent qu'une seule fois (chargement initial).
	applyDefaults = kFalse;

    HandleFormeSelection();

}


void XPGUILinkArtDialogObserver::HandleFormeSelection(){

	PMString classeur = kNullString, carton = kNullString, posX, posY;
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

	do{

		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IFormeDataModel>   model (pageMrgAccessor->QueryFormeDataModel());		

		InterfacePtr<IDropDownListController> classeurListController (panelControlData->FindWidget(kXPGUIClasseurListWidgetID), UseDefaultIID());		
		InterfacePtr<IDropDownListController> cartonListController (panelControlData->FindWidget(kXPGUICartonListWidgetID), UseDefaultIID());			
		
		int32 indexClasseur = classeurListController->GetSelected();
		int32 indexCarton   = cartonListController->GetSelected();
		
		InterfacePtr<IStringListControlData> classeurListData (classeurListController, UseDefaultIID());
		classeur = classeurListData->GetString (indexClasseur);
			
		InterfacePtr<IStringListControlData> cartonListData (cartonListController, UseDefaultIID());
		carton = cartonListData->GetString (indexCarton);

		if(!model->DoesFormeExist(carton, classeur))
			break;

		IDFile formeFile, matchingFile,parentFileForme, parentFilePreviwe;
		if(model->GetForme(carton, classeur,formeFile, matchingFile, posX, posY)){

			FileUtils::GetParentDirectory(formeFile,parentFileForme);
			FileUtils::GetParentDirectory(parentFileForme,parentFilePreviwe);
		
			PMString pathPreview = FileUtils::SysFileToPMString(parentFilePreviwe);
		   //	pathPreviwe.Append("\\Vignette\\");
			pathPreview.Append(FileUtils::GetDirectorySeperator());
			pathPreview.Append("Vignette");
			pathPreview.Append(FileUtils::GetDirectorySeperator());
			pathPreview.Append(carton);
			pathPreview.Append(".jpg");
				
			// On Affiche la vignette  du Carton
			IDFile preview;
			FileUtils::PMStringToIDFile(pathPreview, preview);
			IControlView * displayImagePanel = panelControlData->FindWidget(kXPGUIDisplayImagePanelWidgetID);
			InterfacePtr<ISysFileData> imagePanelFileData (displayImagePanel, UseDefaultIID());
			imagePanelFileData->Set(preview);
			displayImagePanel->Invalidate();	
			
		}
	} while(kFalse);
}