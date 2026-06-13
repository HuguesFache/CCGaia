

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
#include "ILayoutUtils.h"
#include "IDocument.h"

// General includes
#include "CDialogObserver.h"
#include "FileUtils.h"
#include "CAlert.h"
#include "ErrorUtils.h"

// Project includes
#include "XPageUIUtils.h"
#include "XPGUIID.h"
#include "XPGID.h"
#include "XRLID.h"
#include "IXRailPageSlugData.h"


/** XPGUIOrderArtDialogObserver

	@author Trias Developpement
*/

class XPGUIOrderArtDialogObserver : public CDialogObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIOrderArtDialogObserver( IPMUnknown* boss );
	/** Destructor
	*/
	virtual ~XPGUIOrderArtDialogObserver() ;
	
	/** 
		Initialises widgets and attaches widget observers.
	*/
	void AutoAttach();

	/** 
		Detaches widget observers. 
	*/
	void AutoDetach();

	/**
		Update is called for all registered observers, and is
		the method through which changes are broadcast. 
		@param theChange this is specified by the agent of change; it can be the class ID of the agent,
		or it may be some specialised message ID.
		@param theSubject this provides a reference to the object which has changed; in this case, the
		widget boss objects that are being observed.
		@param protocol the protocol along which the change occurred.
		@param changedBy this can be used to provide additional information about the change or a reference
		to the boss object that caused the change.
	*/
	void Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy);

private:
	void AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID);
	void DetachWidget(const InterfacePtr<IPanelControlData>&, const WidgetID& widgetID, const PMIID& interfaceID);

	void FillFirstFilterList();
	void FillSecondFilterList();
	
	UID ownerPageUID ;
	IWebServices * baseHTTP; 
};



CREATE_PMINTERFACE(XPGUIOrderArtDialogObserver, kXPGUIOrderArtDialogObserverImpl)

/* Constructor
*/
XPGUIOrderArtDialogObserver::XPGUIOrderArtDialogObserver( IPMUnknown* boss ) :
	CDialogObserver( boss ), ownerPageUID(kInvalidUID)
{
	InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());

	// Create XRailConnexion 
	baseHTTP = ::CreateObject2<IWebServices>(kXRCXRailClientBoss);

}

/* Destructor
*/
XPGUIOrderArtDialogObserver::~XPGUIOrderArtDialogObserver()
{
}

/* AutoAttach
*/
void XPGUIOrderArtDialogObserver::AutoAttach()
{
	CDialogObserver::AutoAttach();

	do
	{
		// Initialise widget state.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("AutoAttach() invalid panelControlData");
			break;
		}

		FillFirstFilterList();
		
		// Observes widgets		
		AttachWidget(panelControlData, kXPGUIFirstFilterWidgetID, IID_ISTRINGLISTCONTROLDATA);
	
	} while(false);
	
}

/* AutoDetach
*/
void XPGUIOrderArtDialogObserver::AutoDetach()
{
	CDialogObserver::AutoDetach();

	do
	{
		// Detach widget observers.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("AutoDetach() invalid panelControlData");
			break;
		}			

		DetachWidget(panelControlData, kXPGUIFirstFilterWidgetID, IID_ISTRINGLISTCONTROLDATA);		
	
	} while(false);
}

/* Update
*/
void XPGUIOrderArtDialogObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
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
				case kXPGUIFirstFilterWidgetID:					
						FillSecondFilterList();					
					break;				

				default :
					break;
			}
		}
	}	
}		


/* AttachWidget
*/
void XPGUIOrderArtDialogObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
{
	do
	{
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)
		{
			ASSERT_FAIL("controlView invalid");
			break;
		}

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
		{
			ASSERT_FAIL("subject invalid");
			break;
		}
		subject->AttachObserver(this, interfaceID);
	}
	while (false);
}

/* DetachWidget
*/
void XPGUIOrderArtDialogObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
{
	do
	{
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)
		{
			ASSERT_FAIL("controlView invalid.");
			break;
		}

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
		{
			ASSERT_FAIL("subject invalid");
			break;
		}
		subject->DetachObserver(this, interfaceID);
	}
	while (false);
}

/* FillFirstFilterList
*/
void XPGUIOrderArtDialogObserver::FillFirstFilterList()
{
	IDValueList filterList;
	do{		

		// Get list of filter according to filter type		
		bool16 connexionOK = kFalse;
        
		// Get selected items and the folio of the page they belong to
		InterfacePtr<ISelectionManager> selectionMgr (Utils<ISelectionUtils>()->QueryActiveSelection ());					
		InterfacePtr<IXPageSuite> xpgSuite (selectionMgr, IID_IXPAGESUITE);
		if(xpgSuite == nil) // xpgSuite == nil means there is no active selection (neither layout nor text)
			break;		
	
		UIDList txtFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(txtFrames, graphicFrames, selectedItems);
		int32 itemCount = selectedItems.Length();	
		if(itemCount == 0)
			break;      

		IDataBase * db = selectedItems.GetDataBase();
        
        // Determination de l'index de la page de l'article		
		PMReal leftMost = kMaxInt32;
		UID leftMostSelectedItem = kInvalidUID;
		for(int32 i = 0 ; i < selectedItems.Length() ; ++i) 
		{			
			InterfacePtr<IGeometry> itemGeo ( selectedItems.GetRef(i), UseDefaultIID());
			PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();			
			::TransformInnerPointToPasteboard (itemGeo, &leftTop);
	
			if(leftTop.X() < leftMost)
			{
				leftMostSelectedItem = selectedItems.At(i);
				leftMost = leftTop.X();
			}
		}

		InterfacePtr<IHierarchy> leftMostItemHier (db, leftMostSelectedItem, UseDefaultIID());
		ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(leftMostItemHier);

		InterfacePtr<ISpread> isASpread (db, ownerPageUID, UseDefaultIID());
		if(isASpread != nil){
			// Le bloc le plus a gauche de l'article est hors page, on renvoie l'ID de la page la plus a gauche dans la planche
			ownerPageUID = isASpread->GetNthPageUID(0);
		}

		// Recuperartion de l'id de la page a partir de Xrail Client
		if(ownerPageUID == kInvalidUID) // Wrong page index
			break;

		InterfacePtr<IXRailPageSlugData> readData (db, ownerPageUID , IID_PAGESLUGDATA);		
		if(readData == nil)
			break;
        int32 idPage = readData->GetID();
        
		int32 titreID;
		PMString titreLib = kNullString, editionLib = kNullString, parutionLib = kNullString;

		// Recuperer l'id titre en fonction de l'id de la page
		InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());

		if (!baseHTTP->GetPageInfos_v2(xpgPrefs->GetTEC_URL(), idPage, titreID, titreLib, parutionLib)) {
			CAlert::InformationAlert("Impossible de récupérer les infos de la page");
			break;
		}

		K2Vector<PMString> rubsnames;
		K2Vector<int32> rubsids;
		if (!baseHTTP->GetListeRub_v2(xpgPrefs->GetTEC_URL(), titreID, rubsnames, rubsids))
		{
			CAlert::InformationAlert("Impossible de récupérer la liste des rubriques");
			break;
		}

		for (int32 i = 0; i < rubsnames.size(); ++i) {
			PMString name = rubsnames[i];
			PMString monID = "";
			monID.AppendNumber(rubsids[i]);
			filterList.push_back(IDValuePair(monID, name));
		}

	} while(kFalse);

	// Fill drop down list widget
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	XPageUIUtils::FillDropdownlist(panelControlData, kXPGUIFirstFilterWidgetID, filterList, kFalse);

	// Fill second filter list
	FillSecondFilterList();
}


/* FillSecondFilterList
*/
void XPGUIOrderArtDialogObserver::FillSecondFilterList()
{
	IDValueList filterList;
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	do
	{		
		// Get select first filter element
		InterfacePtr<IDropDownListController> firstFilterListController(panelControlData->FindWidget(kXPGUIFirstFilterWidgetID), UseDefaultIID());
		InterfacePtr<IStringListData> firstFilterIDList (firstFilterListController, IID_IIDLIST);
		
		int32 selectedFilterIndex = firstFilterListController->GetSelected();
		if(selectedFilterIndex == IDropDownListController::kNoSelection)
			break;
		
		InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());

		K2Vector<PMString> ssrubsnames;
		K2Vector<int32> ssrubsids;
		bool connexionOK = baseHTTP->GetListeSSRub_v2(xpgPrefs->GetTEC_URL(), firstFilterIDList->GetStringList()[selectedFilterIndex], ssrubsnames, ssrubsids);
		if (!connexionOK)
		{
			break;
		}

		for (int32 i = 0; i < ssrubsnames.size(); ++i) {
			PMString name = ssrubsnames[i];
			PMString monID = "";
			monID.AppendNumber(ssrubsids[i]);
			filterList.push_back(IDValuePair(monID, name));
		}

		// Add "none" item at first position
		filterList.insert(filterList.begin(),IDValuePair(kNullString, PMString(kXPGUINoneTextKey, PMString::kTranslateDuringCall)));

	} while(kFalse);

	// Fill drop down list widget		
	XPageUIUtils::FillDropdownlist(panelControlData,kXPGUISecondFilterWidgetID,filterList, kFalse);
}
