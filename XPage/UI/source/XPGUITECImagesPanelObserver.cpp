

#include "VCPluginHeaders.h"

// Interface includes
#include "IControlView.h"
#include "IPanelControlData.h"
#include "ITextControlData.h"
#include "ISubject.h"
#include "IApplication.h"
#include "ISelectionManager.h"
#include "ISelectionMessages.h"
#include "ITextSelectionSuite.h"
#include "IDocument.h"
#include "IStringData.h"
#include "ILayoutUIUtils.h"

#include "IXPageMgrAccessor.h"
#include "IImageDataModel.h"
#include "ITreeViewMgr.h"
#include "IPMUnknownData.h"
#include "ITriStateControlData.h"
#include "ITreeViewHierarchyAdapter.h"
#include "ITreeViewController.h"
#include "ISysFileData.h"
#include "IDocumentList.h"
#include "IWidgetUtils.h"
#include "IXPageUtils.h"

// General includes
#include "CObserver.h"
#include "FileUtils.h"
#include "CAlert.h"
#include "ErrorUtils.h"

// Project includes
#include "XPGUIImageNodeID.h"
#include "XPGUIID.h"
#include "XPGID.h"

/** XPGUITECImagesPanelObserver

	@author Trias Developpement
*/

class XPGUITECImagesPanelObserver : public CObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUITECImagesPanelObserver( IPMUnknown* boss );
	/** Destructor
	*/
	virtual ~XPGUITECImagesPanelObserver() ;
	
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
	
	void AttachDocList(void);
	void DetachDocList(void);
	
	void RefreshModel(IPanelControlData * panelControlData);	
	void ResetVignette(IPanelControlData * panelControlData, const IDFile& imageFile);
	void HandleImageSelection();
	void HandleNBImageSelection();

	IDocument * curFrontDoc;
	bool16 firstInit;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUITECImagesPanelObserver, kXPGUITECImagesPanelObserverImpl)

/* Constructor
*/
XPGUITECImagesPanelObserver::XPGUITECImagesPanelObserver( IPMUnknown* boss ) :
	CObserver( boss ), curFrontDoc(nil), firstInit(kTrue)
{
	// Get notification when a refresh is required
	InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
	sessionSubject->AttachObserver(this, IID_IREFRESHPROTOCOL);	
}

/* Destructor
*/
XPGUITECImagesPanelObserver::~XPGUITECImagesPanelObserver()
{
}

/* AutoAttach
*/
void XPGUITECImagesPanelObserver::AutoAttach()
{
	// set data model for tree view widget		
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	InterfacePtr<IPMUnknownData> dataModel (panelControlData->FindWidget(kXPGUIImagesViewWidgetID), IID_IDATAMODEL);
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IImageDataModel> imageDataModel (pageMrgAccessor->QueryImageDataModel());
	dataModel->SetPMUnknown(imageDataModel);
		
	CObserver::AutoAttach();

	do
	{
		// Initialise widget state.
		
		if (panelControlData == nil)
		{
			ASSERT_FAIL("AutoAttach() invalid panelControlData");
			break;
		}
		
		AttachWidget(panelControlData, kXPGUIImagesViewWidgetID, IID_ITREEVIEWCONTROLLER);

		// On attache le DocumentList observer, afin de pouvoir detecter un changement de selection de document, et
		// pouvoir reagir (dans le Update de cette classe).
		this->AttachDocList();

		// On initialise le document courant au premier plan
		curFrontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();

		// Update model
		this->RefreshModel(panelControlData);	

	} while(false);

}

/* AutoDetach
*/
void XPGUITECImagesPanelObserver::AutoDetach()
{
	CObserver::AutoDetach();

	do
	{
		// Detach widget observers.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("AutoDetach() invalid panelControlData");
			break;
		}	

		DetachWidget(panelControlData, kXPGUIImagesViewWidgetID, IID_ITREEVIEWCONTROLLER);

		this->DetachDocList();

	} while(false);

}

/* Update
*/
void XPGUITECImagesPanelObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{	
	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());

	if((protocol == IID_ITREEVIEWCONTROLLER) && (theChange == kListSelectionChangedMessage || theChange == kListSelectionChangedByUserMessage))
	{
		if(controlView && controlView->GetWidgetID() == kXPGUIImagesViewWidgetID)
			HandleImageSelection();
	}
	else if ((theChange == kSetFrontDocCmdBoss || theChange == kCloseDocCmdBoss) && (protocol == IID_IDOCUMENTLIST))
	{	
		IDocument * frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(frontDoc){
			curFrontDoc = frontDoc;
			InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
			this->RefreshModel(panelControlData);
		}
	}
	else if(theChange == kXPGRefreshMsg && protocol == IID_IREFRESHPROTOCOL)
	{
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		this->RefreshModel(panelControlData);
	}
}		

/* AttachWidget
*/
void XPGUITECImagesPanelObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
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
void XPGUITECImagesPanelObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
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

/* AttachDocList
*/
void XPGUITECImagesPanelObserver::AttachDocList(void){
	do{
		InterfacePtr<IApplication> app (GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> doclist (app->QueryDocumentList());
		InterfacePtr<ISubject> subject (doclist, UseDefaultIID());
		subject->AttachObserver(this, IID_IDOCUMENTLIST);
	}
	while (false);
}

/* DetachDocList
*/
void XPGUITECImagesPanelObserver::DetachDocList(void){
	do{
		InterfacePtr<IApplication> app (GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> doclist (app->QueryDocumentList());
		InterfacePtr<ISubject> subject (doclist, UseDefaultIID());
		subject->DetachObserver(this, IID_IDOCUMENTLIST);
	}
	while (false);
}

void XPGUITECImagesPanelObserver::HandleNBImageSelection(){
	
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	do{
		IDFile associatedNBImagePreview;

		// Get selected image
		InterfacePtr<ITreeViewController> imageListController (panelControlData->FindWidget(kXPGUIImagesViewWidgetID), UseDefaultIID());
		NodeIDList selectedItems;
		imageListController->GetSelectedItems(selectedItems);

		if(!selectedItems.empty()){

			TreeNodePtr<XPGUIImageNodeID> nodeID(selectedItems[0]);
			// B&W checkbox was removed: always use the colour preview.
			associatedNBImagePreview = FileUtils::PMStringToSysFile(nodeID->GetImageData()->brFile);
		}

		IControlView * imagePanelView = panelControlData->FindWidget(kXPGUIDisplayImagePanelWidgetID);
		InterfacePtr<ISysFileData> imagePanelFileData (imagePanelView, UseDefaultIID());
		imagePanelFileData->Set(associatedNBImagePreview);
		imagePanelView->Invalidate();

	} while(kFalse);
}

/* HandleImageSelection
*/
void XPGUITECImagesPanelObserver::HandleImageSelection()
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

	do
	{
		IDFile associatedImagePreview;

		// Get selected image
		InterfacePtr<ITreeViewController> imageListController (panelControlData->FindWidget(kXPGUIImagesViewWidgetID), UseDefaultIID());
		NodeIDList selectedItems;
		imageListController->GetSelectedItems(selectedItems);

		if(!selectedItems.empty())
		{
			TreeNodePtr<XPGUIImageNodeID> nodeID(selectedItems[0]);
			// B&W checkbox was removed: always use the colour preview.
			associatedImagePreview = FileUtils::PMStringToSysFile(nodeID->GetImageData()->brFile);
		}

		IControlView * imagePanelView = panelControlData->FindWidget(kXPGUIDisplayImagePanelWidgetID);
		InterfacePtr<ISysFileData> imagePanelFileData (imagePanelView, UseDefaultIID());
		imagePanelFileData->Set(associatedImagePreview);

		imagePanelView->Invalidate();

	} while(kFalse);
}

void XPGUITECImagesPanelObserver::RefreshModel(IPanelControlData * panelControlData)
{
	do
	{
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IImageDataModel> model (pageMrgAccessor->QueryImageDataModel());

		// Get ID of opened article from its file name
		if(curFrontDoc != nil && ::GetDataBase(curFrontDoc)->GetSysFile() != nil)
		{
			IDataBase * db = ::GetDataBase(curFrontDoc);
			
			PMString idArticle = Utils<IXPageUtils>()->GetStoryIDFromAssignmentFile(*(db->GetSysFile()));

			model->SetFilter(idArticle, FileUtils::SysFileToPMString(*(db->GetSysFile())));
		}
		else
			model->SetFilter(kNullString, kNullString);

		model->Rebuild(kNullString);

		InterfacePtr<ITreeViewMgr> treeViewMgr(panelControlData->FindWidget(kXPGUIImagesViewWidgetID), UseDefaultIID());
		treeViewMgr->ClearTree();
		treeViewMgr->ChangeRoot(kTrue);

		// To refresh vignette
		InterfacePtr<ITreeViewHierarchyAdapter> imagesHierAdapter (treeViewMgr, UseDefaultIID());
		NodeID imageRootNode = imagesHierAdapter->GetRootNode();
		if(imagesHierAdapter->GetNumChildren(imageRootNode) > 0){
			InterfacePtr<ITreeViewController> imagesListController (imagesHierAdapter, UseDefaultIID());
			imagesListController->Select(imagesHierAdapter->GetNthChild(imagesHierAdapter->GetRootNode(), 0));		
		}

	} while(kFalse);
}