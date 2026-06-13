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
#include "IUIDListData.h"
#include "ITriStateControlData.h"
#include "ILayoutSelectionSuite.h"
#include "ISelectionManager.h"
#include "IUIDData.h"
#include "IHierarchy.h"
#include "IMultiColumnTextFrame.h"
#include "IDocument.h"
#include "ILayoutUIUtils.h"
#include "DebugClassUtils.h"
#include "CDialogObserver.h"
#include "IStringData.h"

#include "IXPageSuite.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"

#include "ISelectionUtils.h"
#include "IAssignmentMgr.h"
#include "IUIDData.h"
#include "IXPageUtils.h"
#include "IPlacedArticleData.h"

// General includes
#include "SelectionObserver.h"
#include "FileUtils.h"
#include "CAlert.h"
#include "ErrorUtils.h"

// Project includes
#include "XPGUIID.h"
#include "XPGID.h"

/** XPGUIXRailNewFormeDialogObserver

	@author Trias Developpement
*/

class XPGUIAddToArtDialogObserver : public CDialogObserver
{
public:

	XPGUIAddToArtDialogObserver( IPMUnknown* boss );
	virtual ~XPGUIAddToArtDialogObserver() ;	

	/**
		Respond to a change in an observed subject. 
	*/
	virtual void  Update(const ClassID &theChange, ISubject *theSubject, const PMIID &protocol, void *changedBy); 
	
 	/**
		By default we attach and detach to the ok and cancel buttons. 
	*/
	virtual void  AutoAttach(); 

 	/**
		If the observer knows which subject(s) to detach itself from, it'll do so with this call.
	*/
	virtual void  AutoDetach(); 

	private:

		void AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID);
		void DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID);	 

	protected:
		
		// Selection
		void HandleSelectionChanged(const ISelectionMessage * message);
		
		// Fill stories list
		void FillInCopyStoriesList();
		void FillPlacedStoriesList();		

		K2Vector<UIDRef> itemRefList;	 
};

CREATE_PMINTERFACE(XPGUIAddToArtDialogObserver, kXPGUIAddToArtDialogObserverImpl)

XPGUIAddToArtDialogObserver::XPGUIAddToArtDialogObserver(IPMUnknown* boss ):CDialogObserver(boss){}

XPGUIAddToArtDialogObserver::~XPGUIAddToArtDialogObserver(){}


void XPGUIAddToArtDialogObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID){
	do{
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)
			break;		

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
			break;
	
		if (!subject->IsAttached(this, interfaceID, IID_IOBSERVER)) 
			subject->AttachObserver(this, interfaceID);        

	}while (false);
}

/* DetachWidget
*/
void XPGUIAddToArtDialogObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID){
	
	do{
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)		
			break;	

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)		
			break;
	
		if (!subject->IsAttached(this, interfaceID, IID_IOBSERVER)) 
			subject->DetachObserver(this, interfaceID);        

	}while (false);
}

void XPGUIAddToArtDialogObserver::AutoAttach(){

	CDialogObserver::AutoAttach();
	do{
		// Initialise widget state.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if(panelControlData == nil)
			break;	

		this->AttachWidget(panelControlData, kXPGUIAssignmentListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		this->AttachWidget(panelControlData, kXPGUIRadioInCopyStoryWidgetID, ITriStateControlData::kDefaultIID);
		this->AttachWidget(panelControlData, kXPGUIRadioPlacedStoryWidgetID, ITriStateControlData::kDefaultIID);	
		
		// init uidlist
		itemRefList.clear();

		// First init
		this->FillPlacedStoriesList();
	
	} while(kFalse);	
}

/* AutoDetach
*/
void XPGUIAddToArtDialogObserver::AutoDetach()
{
	CDialogObserver::AutoDetach();
	do{
		// Detach widget observers.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
			break;

		this->DetachWidget(panelControlData, kXPGUIAssignmentListWidgetID, IID_ISTRINGLISTCONTROLDATA);	
		this->DetachWidget(panelControlData, kXPGUIRadioInCopyStoryWidgetID, ITriStateControlData::kDefaultIID);
		this->DetachWidget(panelControlData, kXPGUIRadioPlacedStoryWidgetID, ITriStateControlData::kDefaultIID);	

		itemRefList.clear();

	} while(kFalse);
}


/* Update
*/
void XPGUIAddToArtDialogObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy){	
	
	CDialogObserver::Update(theChange, theSubject, protocol, changedBy);									   
	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());
	do{	
		if(theChange == kTrueStateMessage){
			if(controlView != nil){
				WidgetID widgetID = controlView->GetWidgetID();	
				switch(widgetID.Get()){	

					case kXPGUIRadioInCopyStoryWidgetID:
						FillInCopyStoriesList();
						break;

					case kXPGUIRadioPlacedStoryWidgetID:
						FillPlacedStoriesList();
						break;
					
					default :
						break;
				}
			}
		}
		else if ((protocol == IID_ISTRINGLISTCONTROLDATA) && (theChange == kPopupChangeStateMessage)){
			if(controlView != nil){
				WidgetID widgetID = controlView->GetWidgetID();	
				if(widgetID.Get() == kXPGUIAssignmentListWidgetID){
					this->HandleSelectionChanged(nil);
				}				
			}
		}	
	
	}while (kFalse);
}		


void XPGUIAddToArtDialogObserver::HandleSelectionChanged(const ISelectionMessage * message){

	do{
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if(!panelControlData)
			break;	
	
		IControlView* controlViewList = panelControlData->FindWidget(kXPGUIAssignmentListWidgetID);	
		if(!controlViewList)
			break;

		InterfacePtr<IDropDownListController> controlViewListController(controlViewList, UseDefaultIID());
		if(!controlViewListController)
			break;

		int32 selectedIndex = controlViewListController->GetSelected();
		if(selectedIndex == IDropDownListController::kNoSelection)
			break;

		if(itemRefList.size() == 0)
			break;
		
		InterfacePtr<IUIDData> uiDataTarget (this, UseDefaultIID());
		if(!uiDataTarget) 
			break;	
	
		// Get selected item if any
		InterfacePtr<ITriStateControlData> triStatePlacedStoryControlData (panelControlData->FindWidget(kXPGUIRadioPlacedStoryWidgetID), UseDefaultIID());
		if(triStatePlacedStoryControlData->GetState() == ITriStateControlData::kSelected){			
			UIDRef storyTargetRef = this->itemRefList[selectedIndex];
			uiDataTarget->Set(storyTargetRef);
		}else{
			UIDRef assignementTargetRef = this->itemRefList[selectedIndex];
			uiDataTarget->Set(assignementTargetRef);
		}

	}while(kFalse);

}
/**
 * @desc fill with assignement on document
*/
void XPGUIAddToArtDialogObserver::FillInCopyStoriesList(){
	
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	
	do{
		IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(doc == nil)
			break;

		InterfacePtr<IAssignedDocument> assignedDoc(doc, UseDefaultIID());
		if(assignedDoc == nil)
			break;

		InterfacePtr<IStringListControlData> assignmentListData (panelControlData->FindWidget(kXPGUIAssignmentListWidgetID), UseDefaultIID());
		if(assignmentListData) 
			assignmentListData->Clear();	

		// Clear global list
		itemRefList.clear();

		UIDList assignmentsList = assignedDoc->GetAssignments(); 

		for(int32 i = 0; i < assignmentsList.size(); i++){

			InterfacePtr<IAssignment> assignment(assignmentsList.GetRef(i), UseDefaultIID());
			if(assignment == nil)
				continue;
		 
			IDFile assignmentFile = FileUtils::PMStringToSysFile(assignment->GetFile());
			if(!FileUtils::DoesFileExist(assignmentFile))
				continue;

			PMString assignmentName = assignment->GetName();
			assignmentName.SetTranslatable(kFalse);
			
			// Insert element ui
			assignmentListData->AddString(assignment->GetName(), IStringListControlData::kEnd);

			// Add uid of assignment
			itemRefList.push_back(::GetUIDRef(assignment));			
		}
	 
		// Select first if any
		if(assignmentListData->Length() > 0){
			InterfacePtr<IDropDownListController> assignmentListController (assignmentListData, UseDefaultIID());
			assignmentListController->Select(IDropDownListController::kBeginning);		 
		}
	}while(kFalse);
}

void  XPGUIAddToArtDialogObserver::FillPlacedStoriesList(){
	
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	do{
		IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(!doc)
			break;

		// Clear list 
		InterfacePtr<IStringListControlData> assignmentListData (panelControlData->FindWidget(kXPGUIAssignmentListWidgetID), UseDefaultIID());
		if(!assignmentListData) 
			break;
		assignmentListData->Clear();

		// Clear global list
		itemRefList.clear();
	 
		K2Vector<KeyValuePair<PMString, K2Pair<PMString, UIDList> > > placedStoriesList = Utils<IXPageUtils>()->GetPlacedStoriesList(doc);

		for(int32 i = 0; i < placedStoriesList.size(); i++){

			UIDList items = placedStoriesList[i].Value().second;
			if(items.size() == 0)
				continue;

			InterfacePtr<IPlacedArticleData> placedArticleData(items.GetRef(0), UseDefaultIID());
			if(placedArticleData && placedArticleData->GetUniqueId() != kNullString){
				
				// Insert element at end position
				PMString storyName = placedArticleData->GetStoryTopic();
				storyName.SetTranslatable(kFalse);
				assignmentListData->AddString(storyName, IStringListControlData::kEnd);

				// Append element
				itemRefList.push_back(::GetUIDRef(placedArticleData));	
			}	
		}
		
		// Select first if any
		if(assignmentListData->Length() > 0){
			InterfacePtr<IDropDownListController> assignmentListController (assignmentListData, UseDefaultIID());
			assignmentListController->Select(IDropDownListController::kBeginning);		 
		}

	}while(kFalse);
}
