
#include "VCPluginHeaders.h"

// Interface includes
#include "IControlView.h"
#include "IPanelControlData.h"
#include "ITextControlData.h"
#include "ISubject.h"
#include "ILayoutUIUtils.h"
#include "IHierarchy.h"
#include "IGeometry.h"
#include "IApplication.h"
#include "IDocument.h"
#include "IStringData.h"
#include "IXPageMgrAccessor.h"
#include "IAssemblageDataModel.h"
#include "ITreeViewMgr.h"
#include "ITreeViewHierarchyAdapter.h"
#include "IXPageSuite.h"
#include "ISelectionManager.h"
#include "ITreeViewController.h"
#include "IStringListControlData.h"
#include "IDropDownListController.h"
#include "IXPGPreferences.h"
#include "ISysFileData.h"
#include "DebugClassUtils.h"
#include "IDialogMgr.h"
#include "IApplication.h"
#include "IDialog.h"
#include "RsrcSpec.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "IPointerData.h"
#include "IXPageUtils.h"
#include "IPanelMgr.h"
#include "IFormeData.h"

// General includes
#include "SelectionObserver.h"
#include "UIDList.h"
#include "XPageUIUtils.h"
#include "CAlert.h"
#include "ErrorUtils.h"

// Project includes
#include "XPGUIFormeNodeID.h"
#include "XPGUIID.h"
#include "XPGID.h"

/** XPGUIAssemblagesPanelObserver

	@author Trias
*/

class XPGUIAssemblagesPanelObserver : public ActiveSelectionObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIAssemblagesPanelObserver( IPMUnknown* boss );
	/** Destructor
	*/
	virtual ~XPGUIAssemblagesPanelObserver() ;
	
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

protected:
	/*
		This method is called whenever selection changed, update image filter according 
		to selected article
	*/
	virtual void HandleSelectionChanged(const ISelectionMessage * message);

private:
	void AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID);
	void DetachWidget(const InterfacePtr<IPanelControlData>&, const WidgetID& widgetID, const PMIID& interfaceID);
	
	void RefreshModel(IPanelControlData * panelControlData);
	void EnableDeleteButton();
	void FillClasseurList(const PMString& selectedClasseur = kNullString);
	void CreateAssemblage();
	void DeleteAssemblage();
	void PlaceAssemblage();
	
	PMString currentNumero;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUIAssemblagesPanelObserver, kXPGUIAssemblagesPanelObserverImpl)

/* Constructor
*/
XPGUIAssemblagesPanelObserver::XPGUIAssemblagesPanelObserver( IPMUnknown* boss ) :
	ActiveSelectionObserver( boss ), currentNumero(kNullString)
{
	InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
	sessionSubject->AttachObserver(this, IID_IREFRESHPROTOCOL);
}

/* Destructor
*/
XPGUIAssemblagesPanelObserver::~XPGUIAssemblagesPanelObserver()
{
}

/* AutoAttach
*/
void XPGUIAssemblagesPanelObserver::AutoAttach()
{
	ActiveSelectionObserver::AutoAttach();

	do
	{		
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("AutoAttach() invalid panelControlData");
			break;
		}
 
		// Observe buttons and change of selection in forme list
		AttachWidget(panelControlData, kXPGUIAssemblagesViewWidgetID,			IID_ITREEVIEWCONTROLLER);
		AttachWidget(panelControlData, kXPGUINewAssemblageWidgetID,				IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUIDeleteAssemblageWidgetID,			IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUINewClasseurAssemblageButtonWidgetID, IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUIComboClasseurListWidgetID,			IID_ISTRINGLISTCONTROLDATA);
		AttachWidget(panelControlData, kXPGUIPlaceAutoAssemblageWidgetID,		IID_IBOOLEANCONTROLDATA);

		// Initialise widget state.
		this->HandleSelectionChanged(nil);
		this->EnableDeleteButton();		
		this->FillClasseurList();

	} while(false);
	
}

/* AutoDetach
*/
void XPGUIAssemblagesPanelObserver::AutoDetach()
{
	ActiveSelectionObserver::AutoDetach();

	do
	{
		// Detach widget observers.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("AutoDetach() invalid panelControlData");
			break;
		}	

		DetachWidget(panelControlData, kXPGUIAssemblagesViewWidgetID,			IID_ITREEVIEWCONTROLLER);
		DetachWidget(panelControlData, kXPGUINewAssemblageWidgetID,				IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUIDeleteAssemblageWidgetID,			IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUINewClasseurAssemblageButtonWidgetID, IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUIComboClasseurListWidgetID,			IID_ISTRINGLISTCONTROLDATA);
		DetachWidget(panelControlData, kXPGUIPlaceAutoAssemblageWidgetID,		IID_IBOOLEANCONTROLDATA);
		
	} while(false);
}

/* Update
*/
void XPGUIAssemblagesPanelObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{	
	ActiveSelectionObserver::Update(theChange, theSubject, protocol, changedBy);

	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());
	
	if (theChange == kTrueStateMessage)
	{
		if (controlView != nil)
		{
			WidgetID widgetID = controlView->GetWidgetID();			
			switch (widgetID.Get())
			{
				case kXPGUINewAssemblageWidgetID :
					this->CreateAssemblage();
					break;

				case kXPGUIDeleteAssemblageWidgetID :
					this->DeleteAssemblage();
					break;

				case kXPGUINewClasseurAssemblageButtonWidgetID :
					// Mirrors the XDA palette's "New classeur" button.
					XPageUIUtils::DisplayNewClasseurDialog(kTrue);
					break;

				case kXPGUIPlaceAutoAssemblageWidgetID:
					this->PlaceAssemblage();
					break;

				default:
					break;
			}
			
		}
	}
	else if ((protocol == IID_ITREEVIEWCONTROLLER) && (theChange == kListSelectionChangedMessage))
	{
		if(controlView && controlView->GetWidgetID() == kXPGUIAssemblagesViewWidgetID)
			this->EnableDeleteButton();			
	}
	else if ((protocol == IID_ISTRINGLISTCONTROLDATA) && (theChange == kPopupChangeStateMessage))
	{
		if(controlView && controlView->GetWidgetID() == kXPGUIComboClasseurListWidgetID)
		{
			InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
			this->RefreshModel(panelControlData);
		}		
	}
	else if(theChange == kXPGNewClasseurAssemblageMsg && protocol == IID_IREFRESHPROTOCOL)
	{
		// Get new classeur name from text data in classeur list widget
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		InterfacePtr<ITextControlData> classeurData (panelControlData->FindWidget(kXPGUIComboClasseurListWidgetID), UseDefaultIID());
		FillClasseurList(classeurData->GetString());
	}
	else if(theChange == kXPGDeleteClasseurAssemblageMsg && protocol == IID_IREFRESHPROTOCOL){
		FillClasseurList();
	}
	else if(theChange == kXPGRefreshAssemblageMsg && protocol == IID_IREFRESHPROTOCOL)
	{
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		this->RefreshModel(panelControlData);	
	}
}		


/* AttachWidget
*/
void XPGUIAssemblagesPanelObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
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
void XPGUIAssemblagesPanelObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
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

/* HandleSelectionChanged
*/
void XPGUIAssemblagesPanelObserver::HandleSelectionChanged(const ISelectionMessage * message)
{
	bool16 enableNew = kFalse;
	InterfacePtr<IXPageSuite> xpgSuite (fCurrentSelection, IID_IXPAGESUITE);

	if(xpgSuite != nil) // nil means no active selection => don't activate
	{
		UIDList textFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);		
		
		for(int32 i = 0 ; i < selectedItems.Length() ; ++i)
		{
			InterfacePtr<IFormeData> formeData (textFrames.GetRef(i), UseDefaultIID());
			if(formeData && formeData->GetUniqueName() != kNullString)
			{
				enableNew = kTrue;
				break;
			}
		}
	}	

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
	panelControlData->FindWidget(kXPGUINewAssemblageWidgetID)->Enable(enableNew && xpgPrefs->GetCreationForme());
}

/* EnableDeleteButton
*/
void XPGUIAssemblagesPanelObserver::EnableDeleteButton()
{
	bool16 enableDelete = kFalse;
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

	// Get selected element
	InterfacePtr<ITreeViewController> assemblageListController (panelControlData->FindWidget(kXPGUIAssemblagesViewWidgetID), UseDefaultIID());
	NodeIDList selectedItems;
	assemblageListController->GetSelectedItems(selectedItems);

	// We've got single selection only
	if(selectedItems.size()>0)
	{
		TreeNodePtr<XPGUIFormeNodeID> nodeID(selectedItems[0]);
		enableDelete = (nodeID->GetType() == XPGFormeDataNode::kFormeNode);
	}

	InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
	panelControlData->FindWidget(kXPGUIDeleteAssemblageWidgetID)->Enable(enableDelete && xpgPrefs->GetCreationForme());
}

/* RefreshModel
*/
void XPGUIAssemblagesPanelObserver::RefreshModel(IPanelControlData * panelControlData)
{
	// Get current selected classeur name
	IControlView * imageFormePanelView = panelControlData->FindWidget(kXPGUIDisplayImageFormePanelWidgetID);
	InterfacePtr<ISysFileData> imageFormePanelFileData (imageFormePanelView, UseDefaultIID());
	imageFormePanelFileData->Set(IDFile());
	imageFormePanelView->Invalidate();

	PMString classeurName = kNullString;
	InterfacePtr<IDropDownListController> classeurListWidgetController (panelControlData->FindWidget(kXPGUIComboClasseurListWidgetID), UseDefaultIID());
	int32 selectedIndex = classeurListWidgetController->GetSelected();
	if(selectedIndex != IDropDownListController::kNoSelection)
	{
		InterfacePtr<IStringListControlData> classeurListWidgetData (classeurListWidgetController, UseDefaultIID());
		classeurName = classeurListWidgetData->GetString(selectedIndex);
	}
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());	
	InterfacePtr<IAssemblageDataModel> model (pageMrgAccessor->QueryAssemblageDataModel());
	InterfacePtr<ITreeViewMgr> treeViewMgr(panelControlData->FindWidget(kXPGUIAssemblagesViewWidgetID), UseDefaultIID());
	
	model->Rebuild(classeurName);
	
	treeViewMgr->ClearTree();
	treeViewMgr->ChangeRoot(kTrue);

	InterfacePtr<ITreeViewHierarchyAdapter> assemblagesHierAdapter (treeViewMgr, UseDefaultIID());
	NodeID formeRootNode = assemblagesHierAdapter->GetRootNode();
	if(assemblagesHierAdapter->GetNumChildren(formeRootNode) >  0){
		InterfacePtr<ITreeViewController> assemblagesListController (assemblagesHierAdapter, UseDefaultIID());
		assemblagesListController->Select(assemblagesHierAdapter->GetNthChild(assemblagesHierAdapter->GetRootNode(), 0));		
	}
}

/* FillClasseurList
*/
void XPGUIAssemblagesPanelObserver::FillClasseurList(const PMString& selectedClasseur)
{
	// Access forme model to get classeur list
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IAssemblageDataModel> model (pageMrgAccessor->QueryAssemblageDataModel());
	K2Vector<PMString> classeurList = model->GetClasseurList();
	
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	XPageUIUtils::FillDropdownlistAndSelect(panelControlData, kXPGUIComboClasseurListWidgetID, classeurList, selectedClasseur, kTrue);
}

void XPGUIAssemblagesPanelObserver::CreateAssemblage()
{	
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());	
	// Get selected classeur
	InterfacePtr<IDropDownListController> classeurListWidgetController (panelControlData->FindWidget(kXPGUIComboClasseurListWidgetID), UseDefaultIID());
	int32 selectedIndex = classeurListWidgetController->GetSelected();
	
	if(selectedIndex != IDropDownListController::kNoSelection)
	{
		InterfacePtr<IStringListControlData> classeurListWidgetData (classeurListWidgetController, UseDefaultIID());
		XPageUIUtils::DisplayNewFormeDialog(classeurListWidgetData->GetString(selectedIndex), kTrue);

		// Refresh model
		RefreshModel(panelControlData);
	}	
}

void XPGUIAssemblagesPanelObserver::DeleteAssemblage()
{
	do
	{
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

		// Get selected classeur
		InterfacePtr<IDropDownListController> classeurListWidgetController (panelControlData->FindWidget(kXPGUIComboClasseurListWidgetID), UseDefaultIID());
		int32 selectedIndex = classeurListWidgetController->GetSelected();
		if(selectedIndex == IDropDownListController::kNoSelection)
			break;
		
		InterfacePtr<IStringListControlData> classeurListWidgetData (classeurListWidgetController, UseDefaultIID());

		// Get selected assemblage
		InterfacePtr<ITreeViewController> assemblageListController (panelControlData->FindWidget(kXPGUIAssemblagesViewWidgetID), UseDefaultIID());
		NodeIDList selectedItems;
		assemblageListController->GetSelectedItems(selectedItems);

		if(selectedItems.size() == 0)
			break;

		
		TreeNodePtr<XPGUIFormeNodeID> nodeID(selectedItems[0]);

		// Delete assemblage
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IAssemblageDataModel> model (pageMrgAccessor->QueryAssemblageDataModel());
		model->DeleteAssemblage(nodeID->GetFormeData()->name,classeurListWidgetData->GetString(selectedIndex));

		// Refresh model
		RefreshModel(panelControlData);
		
	}while(kFalse);
}

void XPGUIAssemblagesPanelObserver::PlaceAssemblage() {

	do {
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

		// Get selected forme
		InterfacePtr<ITreeViewController> assemblageListController(panelControlData->FindWidget(kXPGUIAssemblagesViewWidgetID), UseDefaultIID());
		NodeIDList selectedItems;
		assemblageListController->GetSelectedItems(selectedItems);

		if (selectedItems.size() == 0)
			break;
		PMPoint insertPos(0, 0);

		IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if (!doc)
			break;

		IDataBase* db = ::GetDataBase(doc);
		if (!db)
			break;

		UIDList formeItems(db);
		ILayoutControlData* frontLayout = Utils<ILayoutUIUtils>()->QueryFrontLayoutData();
		if (!frontLayout)
			break;

		TreeNodePtr<XPGUIFormeNodeID> nodeID(selectedItems[0]);

		InterfacePtr<IGeometry> currentPageGeo(db, frontLayout->GetPage(), UseDefaultIID());
		InterfacePtr<IHierarchy> currentPageHier(currentPageGeo, UseDefaultIID());
		UIDRef targetSpread(db, currentPageHier->GetSpreadUID());
		PMString error = kNullString;
		IDFile formeFileToImport = nodeID->GetFormeData()->formeFile;
		IDFile matchingFile = nodeID->GetFormeData()->matchingFile;

		//::TransformInnerPointToPasteboard (currentPageGeo, &insertPos);

		// Process the import
		if (Utils<IXPageUtils>()->ImportForme(::GetUIDRef(doc), formeFileToImport, insertPos, targetSpread, matchingFile, error, &formeItems, kFalse, kFalse) != kSuccess)
		{
			break;
		}


	} while (kFalse);

	
}
