

#include "VCPluginHeaders.h"

// Interface includes
#include "IControlView.h"
#include "IPanelControlData.h"
#include "ITextControlData.h"
#include "ITriStateControlData.h"
#include "ISubject.h"
#include "IApplication.h"
#include "IDocument.h"
#include "IStringData.h"
#include "IXPageMgrAccessor.h"
#include "IFormeDataModel.h"
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

// General includes
#include "SelectionObserver.h"
#include "UIDList.h"
#include "XPageUIUtils.h"
#include "CAlert.h"
#include "ErrorUtils.h"
#include "ILayoutUIUtils.h"
#include "ILayoutControlData.h"
#include "IGeometry.h"
#include "IHierarchy.h"
#include "TransformUtils.h"
#include"IUnitOfMeasure.h"

// Project includes
#include "XPGUIFormeNodeID.h"
#include "XPGUIID.h"
#include "XPGID.h"

/** XPGUIFormesPanelObserver

	@author Trias Developpement
*/

class XPGUIFormesPanelObserver : public ActiveSelectionObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIFormesPanelObserver( IPMUnknown* boss );
	/** Destructor
	*/
	virtual ~XPGUIFormesPanelObserver() ;
	
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
	void CreateForme();
	void PlaceForme();
	void DeleteForme();
	// Reflect the shared session "Afficher les formes" state on this palette's
	// checkbox (no re-fire). Called on attach and on the sibling-palette sync
	// message, so this checkbox always mirrors the XPage palette one.
	void SyncDisplayFormesCheckbox();

	PMString currentNumero;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUIFormesPanelObserver, kXPGUIFormesPanelObserverImpl)

/* Constructor
*/
XPGUIFormesPanelObserver::XPGUIFormesPanelObserver( IPMUnknown* boss ) :
	ActiveSelectionObserver( boss ), currentNumero(kNullString)
{
	InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
	sessionSubject->AttachObserver(this, IID_IREFRESHPROTOCOL);	
}

/* Destructor
*/
XPGUIFormesPanelObserver::~XPGUIFormesPanelObserver()
{
}

/* AutoAttach
*/
void XPGUIFormesPanelObserver::AutoAttach()
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
		AttachWidget(panelControlData, kXPGUIFormesViewWidgetID,		IID_ITREEVIEWCONTROLLER);
		AttachWidget(panelControlData, kXPGUINewFormeWidgetID,			IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUIDeleteFormeWidgetID,		IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUIComboClasseurListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		AttachWidget(panelControlData, kXPGUIPlaceAutoFormeWidgetID,	IID_IBOOLEANCONTROLDATA);
		AttachWidget(panelControlData, kXPGUINewClasseurButtonWidgetID,	IID_ITRISTATECONTROLDATA);
		// "Afficher les formes" checkbox + its refresh button — same widget
		// IDs as the XPage palette so they share the string key and the
		// shared toggle logic. The CheckBoxWidget broadcasts on
		// ITriStateControlData (kTrue/kFalseStateMessage); the refresh
		// RollOverIconButton on ITriStateControlData too.
		AttachWidget(panelControlData, kXPGUIDisplayFormesWidgetID,			IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUIRefreshFormesAdornmentWidgetID,	IID_ITRISTATECONTROLDATA);

		// Initialise widget state.
		this->HandleSelectionChanged(nil);
		this->EnableDeleteButton();
		this->FillClasseurList();
		// Mirror the current shared "Afficher les formes" state.
		this->SyncDisplayFormesCheckbox();

	} while(false);
	
}

/* AutoDetach
*/
void XPGUIFormesPanelObserver::AutoDetach()
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

		DetachWidget(panelControlData, kXPGUIFormesViewWidgetID,		IID_ITREEVIEWCONTROLLER);
		DetachWidget(panelControlData, kXPGUINewFormeWidgetID,			IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUIDeleteFormeWidgetID,		IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUIComboClasseurListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		DetachWidget(panelControlData, kXPGUIPlaceAutoFormeWidgetID,	IID_IBOOLEANCONTROLDATA);
		DetachWidget(panelControlData, kXPGUINewClasseurButtonWidgetID,	IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUIDisplayFormesWidgetID,			IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUIRefreshFormesAdornmentWidgetID,	IID_ITRISTATECONTROLDATA);

	} while(false);
}

/* Update
*/
void XPGUIFormesPanelObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
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
				case kXPGUINewFormeWidgetID :
					this->CreateForme();
					break;

				case kXPGUIDeleteFormeWidgetID :
					this->DeleteForme();
					break;

				case kXPGUIPlaceAutoFormeWidgetID:
					this->PlaceForme();
					break;

				case kXPGUINewClasseurButtonWidgetID:
					XPageUIUtils::DisplayNewClasseurDialog();
					break;

				case kXPGUIDisplayFormesWidgetID:
					XPageUIUtils::SetDisplayFormes(kTrue);
					break;

				case kXPGUIRefreshFormesAdornmentWidgetID:
				{
					InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
					InterfacePtr<ITriStateControlData> displayFormes(panelControlData->FindWidget(kXPGUIDisplayFormesWidgetID), UseDefaultIID());
					XPageUIUtils::SetDisplayFormes(displayFormes != nil && displayFormes->IsSelected());
					break;
				}

				default:
					break;
			}

		}
	}
	else if (theChange == kFalseStateMessage)
	{
		if (controlView != nil && controlView->GetWidgetID() == kXPGUIDisplayFormesWidgetID)
			XPageUIUtils::SetDisplayFormes(kFalse);
	}
	else if ((protocol == IID_ITREEVIEWCONTROLLER) && (theChange == kListSelectionChangedMessage))
	{
		if(controlView && controlView->GetWidgetID() == kXPGUIFormesViewWidgetID) {
			this->EnableDeleteButton();			
	}
	}
	else if ((protocol == IID_ISTRINGLISTCONTROLDATA) && (theChange == kPopupChangeStateMessage))
	{
		if(controlView && controlView->GetWidgetID() == kXPGUIComboClasseurListWidgetID)
		{
			InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
			this->RefreshModel(panelControlData);
		}		
	}
	else if(theChange == kXPGNewClasseurMsg && protocol == IID_IREFRESHPROTOCOL)
	{
		// Get new classeur name from text data in classeur list widget
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		InterfacePtr<ITextControlData> classeurData (panelControlData->FindWidget(kXPGUIComboClasseurListWidgetID), UseDefaultIID());
		FillClasseurList(classeurData->GetString());
	}
	else if(theChange == kXPGDeleteClasseurMsg && protocol == IID_IREFRESHPROTOCOL){
		FillClasseurList();
	}
	else if(theChange == kXPGRefreshMsg && protocol == IID_IREFRESHPROTOCOL)
	{
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		this->RefreshModel(panelControlData);
	}
	else if(theChange == kXPGDisplayFormesChangedMsg && protocol == IID_IREFRESHPROTOCOL)
	{
		// The sibling XPage palette toggled "Afficher les formes" — re-sync
		// our checkbox to the shared session state (visual only, no re-fire).
		this->SyncDisplayFormesCheckbox();
	}
}


/* AttachWidget
*/
void XPGUIFormesPanelObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
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
void XPGUIFormesPanelObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
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
void XPGUIFormesPanelObserver::HandleSelectionChanged(const ISelectionMessage * message)
{
	bool16 enableNew = kFalse;
	InterfacePtr<IXPageSuite> xpgSuite (fCurrentSelection, IID_IXPAGESUITE);

	if(xpgSuite != nil) // nil means no active selection => don't activate
	{
		UIDList textFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);		
		enableNew = (!textFrames.IsEmpty());
	}	

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
	panelControlData->FindWidget(kXPGUINewFormeWidgetID)->Enable(enableNew && xpgPrefs->GetCreationForme());
}

/* EnableDeleteButton
*/
void XPGUIFormesPanelObserver::EnableDeleteButton()
{
    bool16 enableDelete = kFalse;
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

    // Get selected element
	InterfacePtr<ITreeViewController> formeListController (panelControlData->FindWidget(kXPGUIFormesViewWidgetID), UseDefaultIID());
	NodeIDList selectedItems;
	formeListController->GetSelectedItems(selectedItems);

    // We've got single selection only
	if(selectedItems.size()>0)
	{
    	TreeNodePtr<XPGUIFormeNodeID> nodeID(selectedItems[0]);
		enableDelete = (nodeID->GetType() == XPGFormeDataNode::kFormeNode);
	}

    InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
    panelControlData->FindWidget(kXPGUIDeleteFormeWidgetID)->Enable(enableDelete && xpgPrefs->GetCreationForme());
    }

/* RefreshModel
*/
void XPGUIFormesPanelObserver::RefreshModel(IPanelControlData * panelControlData)
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
	InterfacePtr<IFormeDataModel> model (pageMrgAccessor->QueryFormeDataModel());
	InterfacePtr<ITreeViewMgr> treeViewMgr(panelControlData->FindWidget(kXPGUIFormesViewWidgetID), UseDefaultIID());
	
	model->Rebuild(classeurName);
	
	treeViewMgr->ClearTree();
	treeViewMgr->ChangeRoot(kTrue);

	InterfacePtr<ITreeViewHierarchyAdapter> formesHierAdapter (treeViewMgr, UseDefaultIID());
	NodeID formeRootNode = formesHierAdapter->GetRootNode();
	if(formesHierAdapter->GetNumChildren(formeRootNode) >  0){
			InterfacePtr<ITreeViewController> formesListController (formesHierAdapter, UseDefaultIID());
			NodeID firstChild = formesHierAdapter->GetNthChild(formesHierAdapter->GetRootNode(), 0);
			formesListController->Select(firstChild);
			// Ensure the selected node is visible and the tree view actually
			// draws the selection highlight. Without this, the first post-doc-
			// open refresh sometimes leaves the selection invisible until the
			// user interacts with the dropdown.
			treeViewMgr->ScrollToNode(firstChild, ITreeViewMgr::eScrollIntoView);
			treeViewMgr->NodeChanged(firstChild, kFalse, 0);
	}

	// Refresh delete button state: clearing/rebuilding the tree does not
	// necessarily fire kListSelectionChangedMessage, so ensure the state is
	// recomputed after every model rebuild.
	this->EnableDeleteButton();
}

/* FillClasseurList
*/
void XPGUIFormesPanelObserver::FillClasseurList(const PMString& selectedClasseur)
{
	// Access forme model to get classeur list
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IFormeDataModel> model (pageMrgAccessor->QueryFormeDataModel());
	K2Vector<PMString> classeurList = model->GetClasseurList();

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

	XPageUIUtils::FillDropdownlistAndSelect(panelControlData, kXPGUIComboClasseurListWidgetID, classeurList, selectedClasseur, kTrue);
}

void XPGUIFormesPanelObserver::CreateForme()
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

	// Pre-flight style validation now runs inside the New Forme dialog
	// (XPGUIXRailNewFormeDialogController::InitializeDialogFields) and is
	// rendered inline above the OK/Cancel buttons rather than as a CAlert,
	// so this observer just opens the dialog.

	// Get selected classeur
	InterfacePtr<IDropDownListController> classeurListWidgetController (panelControlData->FindWidget(kXPGUIComboClasseurListWidgetID), UseDefaultIID());
	int32 selectedIndex = classeurListWidgetController->GetSelected();
	if(selectedIndex != IDropDownListController::kNoSelection)
	{
		InterfacePtr<IStringListControlData> classeurListWidgetData (classeurListWidgetController, UseDefaultIID());
		XPageUIUtils::DisplayNewFormeDialog(classeurListWidgetData->GetString(selectedIndex));

		// Refresh model
		RefreshModel(panelControlData);
	}
}

void XPGUIFormesPanelObserver::PlaceForme(){
	
	do{
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		
		// Get selected forme
		InterfacePtr<ITreeViewController> formeListController (panelControlData->FindWidget(kXPGUIFormesViewWidgetID), UseDefaultIID());
		NodeIDList selectedItems;
		formeListController->GetSelectedItems(selectedItems);
		
		if(selectedItems.size() == 0)
			break;
		
		TreeNodePtr<XPGUIFormeNodeID> nodeID(selectedItems[0]);
		//on recupere le X et le Y
		PMString posX = nodeID->GetFormeData()->posX, posY = nodeID->GetFormeData()->posY;
		if (posX == PMString("") || posY == PMString("")) {
			CAlert::InformationAlert("Ce carton n'a pas de coordonnées");
			break;
		}
		PMPoint insertPos (posX.GetAsDouble(), posY.GetAsDouble());
		
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
		if(!doc)
			break;
		
		IDataBase * db = ::GetDataBase(doc);
        if(!db)
			break;
		
		UIDList formeItems(db);
		ILayoutControlData* frontLayout = Utils<ILayoutUIUtils>()->QueryFrontLayoutData();
		if(!frontLayout)
			break;
		
		InterfacePtr<IGeometry> currentPageGeo (db, frontLayout->GetPage(), UseDefaultIID());
		InterfacePtr<IHierarchy> currentPageHier (currentPageGeo, UseDefaultIID());
		UIDRef targetSpread (db, currentPageHier->GetSpreadUID());
		PMString error = kNullString;
		IDFile formeFileToImport = nodeID->GetFormeData()->formeFile;	
		IDFile matchingFile = nodeID->GetFormeData()->matchingFile;	
		
		//::TransformInnerPointToPasteboard (currentPageGeo, &insertPos);
		
		// Process the import
		if(Utils<IXPageUtils>()->ImportForme(::GetUIDRef(doc), formeFileToImport, insertPos, targetSpread, matchingFile, error, &formeItems, kFalse, kTrue) != kSuccess)
		{
			break;
		}

		
	}while(kFalse);
	
	
}

void XPGUIFormesPanelObserver::DeleteForme()
{
	do
	{
		int16 rep = CAlert::ModalAlert(kXPGUIConfirmDeleteFormeTextKey, PMString(kSDKDefOKButtonApplicationKey),PMString(kSDKDefCancelButtonApplicationKey),kNullString,1,CAlert::eWarningIcon) ;
		
		if (rep == 2) // if the user clicked on Cancel button
		{
			break;
		}
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

		// Get selected classeur
		InterfacePtr<IDropDownListController> classeurListWidgetController (panelControlData->FindWidget(kXPGUIComboClasseurListWidgetID), UseDefaultIID());
		int32 selectedIndex = classeurListWidgetController->GetSelected();
		if(selectedIndex == IDropDownListController::kNoSelection)
			break;
		
		InterfacePtr<IStringListControlData> classeurListWidgetData (classeurListWidgetController, UseDefaultIID());

		// Get selected forme
		InterfacePtr<ITreeViewController> formeListController (panelControlData->FindWidget(kXPGUIFormesViewWidgetID), UseDefaultIID());
		NodeIDList selectedItems;
		formeListController->GetSelectedItems(selectedItems);

		if(selectedItems.size() == 0)
			break;

		TreeNodePtr<XPGUIFormeNodeID> nodeID(selectedItems[0]);

		// Delete forme
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IFormeDataModel> model (pageMrgAccessor->QueryFormeDataModel());
		model->DeleteForme(nodeID->GetFormeData()->name,classeurListWidgetData->GetString(selectedIndex));

		// Refresh model
		RefreshModel(panelControlData);

	}while(kFalse);
}

/* SyncDisplayFormesCheckbox
   Reflect the shared session "Afficher les formes" state on this palette's
   checkbox, without firing the toggle action again (notifyOfChange = kFalse).
*/
void XPGUIFormesPanelObserver::SyncDisplayFormesCheckbox() {

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	if (panelControlData == nil)
		return;

	InterfacePtr<ITriStateControlData> displayFormesControlData(panelControlData->FindWidget(kXPGUIDisplayFormesWidgetID), UseDefaultIID());
	if (displayFormesControlData)
		displayFormesControlData->SetState(
			XPageUIUtils::GetDisplayFormeState() ? ITriStateControlData::kSelected : ITriStateControlData::kUnselected,
			kTrue, kFalse);
}
