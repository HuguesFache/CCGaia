

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

/** XPGUIXRailInCopyImagesPanelObserver

	@author Trias Developpement
*/

class XPGUIXRailInCopyImagesPanelObserver : public CObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIXRailInCopyImagesPanelObserver( IPMUnknown* boss );
	/** Destructor
	*/
	virtual ~XPGUIXRailInCopyImagesPanelObserver() ;
	
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

	IDocument * curFrontDoc;
	bool16 firstInit;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUIXRailInCopyImagesPanelObserver, kXPGUIXRailInCopyImagesPanelObserverImpl)

/* Constructor
*/
XPGUIXRailInCopyImagesPanelObserver::XPGUIXRailInCopyImagesPanelObserver( IPMUnknown* boss ) :
	CObserver( boss ), curFrontDoc(nil), firstInit(kTrue)
{
	// Get notification when a refresh is required
	InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
	sessionSubject->AttachObserver(this, IID_IREFRESHPROTOCOL);	
}

/* Destructor
*/
XPGUIXRailInCopyImagesPanelObserver::~XPGUIXRailInCopyImagesPanelObserver()
{
}

/* AutoAttach
*/
void XPGUIXRailInCopyImagesPanelObserver::AutoAttach()
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

		if(firstInit){
			firstInit = kFalse;
		}

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
void XPGUIXRailInCopyImagesPanelObserver::AutoDetach()
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

		this->DetachDocList();

	} while(false);

}

/* Update
*/
void XPGUIXRailInCopyImagesPanelObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{	
	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());

	if ((theChange == kSetFrontDocCmdBoss || theChange == kCloseDocCmdBoss) && (protocol == IID_IDOCUMENTLIST))
	{	
		IDocument * frontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(frontDoc != curFrontDoc)
		{
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
void XPGUIXRailInCopyImagesPanelObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
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
void XPGUIXRailInCopyImagesPanelObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
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
void XPGUIXRailInCopyImagesPanelObserver::AttachDocList(void){
	
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
void XPGUIXRailInCopyImagesPanelObserver::DetachDocList(void)
{
	do{
		InterfacePtr<IApplication> app (GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> doclist (app->QueryDocumentList());
		InterfacePtr<ISubject> subject (doclist, UseDefaultIID());
		subject->DetachObserver(this, IID_IDOCUMENTLIST);
	}
	while (false);
}

void XPGUIXRailInCopyImagesPanelObserver::RefreshModel(IPanelControlData * panelControlData)
{
	do{
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IImageDataModel> model (pageMrgAccessor->QueryImageDataModel());

		// Get ID of opened article from its file name
		if(curFrontDoc != nil && ::GetDataBase(curFrontDoc)->GetSysFile() != nil){
			
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
 	
        InterfacePtr<ITreeViewController> treeViewController(treeViewMgr, UseDefaultIID());
		NodeIDList selectedItems;
	    treeViewController->GetSelectedItems(selectedItems);
        if(selectedItems.size()>0)
            treeViewController->Select(selectedItems[0]);

	} while(kFalse);
}