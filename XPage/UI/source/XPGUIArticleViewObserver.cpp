#include "VCPlugInHeaders.h"

// Interface includes
#include "ISubject.h"
#include "ITreeViewMgr.h"
#include "ITreeNodeIDData.h"
#include "XPGUIArticleNodeID.h"
#include "IWidgetParent.h"
#include "ITextControlData.h"
#include "IPanelControlData.h"
#include "IControlView.h"
#include "CObserver.h"
#include "XPGID.h"
#include "XPGUIID.h"
#include "CAlert.h"
#include "ISysFileData.h"
#include "ITreeViewController.h"
#include "FileUtils.h"
#include "K2Vector.tpp" // For NodeIDList to compile

class XPGUIArticleViewObserver : public CObserver{

	public:

		XPGUIArticleViewObserver(IPMUnknown* boss);
		~XPGUIArticleViewObserver();
		virtual void AutoAttach();
		virtual void AutoDetach();
		virtual void Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy);

	private:
		void handleSelectionChanged();
};

CREATE_PMINTERFACE(XPGUIArticleViewObserver, kXPGUIArticleViewObserverImpl)

XPGUIArticleViewObserver::XPGUIArticleViewObserver(IPMUnknown* boss):CObserver(boss){}

XPGUIArticleViewObserver::~XPGUIArticleViewObserver(){}

void XPGUIArticleViewObserver::AutoAttach(){
	InterfacePtr<ISubject> subj(this, UseDefaultIID()); 
	subj->AttachObserver(this, IID_ITREEVIEWCONTROLLER);
	this->handleSelectionChanged();
}

/* AutoDetach
*/
void XPGUIArticleViewObserver::AutoDetach(){
	InterfacePtr<ISubject> subj(this, UseDefaultIID()); 
	subj->DetachObserver(this, IID_ITREEVIEWCONTROLLER);
}


/* Update
*/
void XPGUIArticleViewObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{
	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());
	if(theChange == kListSelectionChangedMessage)
	{
		this->handleSelectionChanged();
	}	
}


/* handleChange
*/
void XPGUIArticleViewObserver::handleSelectionChanged()
{

	do{
		InterfacePtr<ITreeViewController> controller(this, UseDefaultIID());
	
		NodeIDList selectedItems;
		controller->GetSelectedItems(selectedItems);	
		PMString previewArt = kNullString, folioArt = kNullString, sheatsArt = kNullString;
		
		// We've got single selection only		
		if(selectedItems.size()>0){
			TreeNodePtr<XPGUIArticleNodeID> nodeID(selectedItems[0]);
			if(!nodeID)
				break;

			previewArt = nodeID->GetArticleData()->artPreview;		
			sheatsArt.AppendNumber(nodeID->GetArticleData()->artNbSignes);
		}			

		InterfacePtr<IWidgetParent> widgetParent (this, UseDefaultIID());

		InterfacePtr<IPanelControlData> hilitePanelCtrlData ((IPanelControlData *)widgetParent->QueryParentFor(IID_IPANELCONTROLDATA));
		InterfacePtr<IWidgetParent> hilitePanelParent (hilitePanelCtrlData, UseDefaultIID());
		InterfacePtr<IPanelControlData> panelCtrlData ((IPanelControlData *)hilitePanelParent->QueryParentFor(IID_IPANELCONTROLDATA));
		
		// Show article preview	
		InterfacePtr<ITextControlData> articlePreviewData (panelCtrlData->FindWidget(kXPGUIArticlePreviewWidgetID), UseDefaultIID());
		previewArt.SetTranslatable(kFalse);
		articlePreviewData->SetString(previewArt);

		// Show nb sheats
	 	InterfacePtr<ITextControlData> articleSheatsData (panelCtrlData->FindWidget(kXPGUIArticleNbSignesWidgetID), UseDefaultIID());
		sheatsArt.SetTranslatable(kFalse);
		articleSheatsData->SetString(sheatsArt);	
		
	} while(kFalse);
}

