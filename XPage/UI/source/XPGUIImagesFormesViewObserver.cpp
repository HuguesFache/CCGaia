
#include "VCPlugInHeaders.h"

// Interface includes
#include "ISubject.h"
#include "IControlView.h"
#include "ITreeViewMgr.h"
#include "IPanelControlData.h"
#include "ITreeNodeIDData.h"
#include "ITreeViewController.h"
#include "ISysFileData.h"
#include "IPMStream.h"
#include "IWidgetParent.h"
#include "IWorkspace.h"
#include "IArticleDataModel.h"
#include "IXPGPreferences.h"
#include "IActionManager.h"
#include "IActionComponent.h"
#include "IStringData.h"
#include "IApplication.h"

// General includes
#include "CObserver.h"
#include "CAlert.h"
#include "K2Vector.tpp" // For NodeIDList to compile
#include "StreamUtil.h"

// Project includes
#include "XPGUIFormeNodeID.h"
#include "XPGUIID.h"
#include "XPGID.h"
#include "FileUtils.h"

/** Implements IObserver; initialises the tree when an IObserver::AutoAttach message sent
	and listens for when the node selection changes.
	When the selection changes and there's a non-empty selection list, it takes the first item
	and previews the asset if it can do so (for instance, if it's a GIF, JPEG etc).
	It does this by setting a path on a data interface of the custom-view panel widget and
	invalidating the widget, forcing it to redraw to create a rendering of the new asset.

	 The class is derived from CObserver, and overrides the
	AutoAttach(), AutoDetach(), and Update() methods.
	This class implements the IObserver interface using the CObserver helper class,
	and is listening along the IID_ITREEVIEWCONTROLLER protocol for changes in the tree data model.

	@ingroup paneltreeview
	
*/
class XPGUIImagesFormesViewObserver : public CObserver
{
public:
	/**
		Constructor for WLBListBoxObserver class.
		@param interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIImagesFormesViewObserver(IPMUnknown* boss);

	/**
		Destructor 
	*/	
	~XPGUIImagesFormesViewObserver();

	/**
		AutoAttach is only called for registered observers
		of widgets.  This method is called by the application
		core when the widget is shown.
	*/	
	virtual void AutoAttach();

	/**
		AutoDetach is only called for registered observers
		of widgets. Called when widget hidden.
	*/	
	virtual void AutoDetach();

	/**
		Update is called for all registered observers, and is
		the method through which changes are broadcast. 
		@param theChange [IN] this is specified by the agent of change; it can be the class ID of the agent,
		or it may be some specialised message ID.
		@param theSubject [IN] provides a reference to the object which has changed; in this case, the button
		widget boss object that is being observed.
		@param protocol [IN] the protocol along which the change occurred.
		@param changedBy [IN] this can be used to provide additional information about the change or a reference
		to the boss object that caused the change.
	*/	
	virtual void Update(
		const ClassID& theChange, 
		ISubject* theSubject, 
		const PMIID &protocol, 
		void* changedBy);

protected:

	/**	Handles a change in the selection when the end user 
	either clicks on a node or clicks off it
	*/
	void handleSelectionChanged();

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGUIImagesFormesViewObserver, kXPGUIImagesFormesViewObserverImpl)

/* Constructor
*/
XPGUIImagesFormesViewObserver::XPGUIImagesFormesViewObserver(IPMUnknown* boss)
: CObserver(boss)
{
}

/* Destructor
*/
XPGUIImagesFormesViewObserver::~XPGUIImagesFormesViewObserver()
{
}

/* AutoAttach
*/
void XPGUIImagesFormesViewObserver::AutoAttach()
{
	InterfacePtr<ISubject> subj(this, UseDefaultIID()); 
	ASSERT(subj);
	subj->AttachObserver(this, IID_ITREEVIEWCONTROLLER);

	this->handleSelectionChanged();
}

/* AutoDetach
*/
void XPGUIImagesFormesViewObserver::AutoDetach()
{
	InterfacePtr<ISubject> subj(this, UseDefaultIID()); 
	ASSERT(subj);
	subj->DetachObserver(this, IID_ITREEVIEWCONTROLLER);
}

/* Update
*/
void XPGUIImagesFormesViewObserver::Update(
	const ClassID& theChange, 
	ISubject* theSubject, 
	const PMIID &protocol, 
	void* changedBy)
{
	switch(theChange.Get())
	{
		case kListSelectionChangedMessage:
		{
			this->handleSelectionChanged();
			break;
		}
	}
}

/* handleSelectionChanged
*/
void XPGUIImagesFormesViewObserver::handleSelectionChanged()
{
	do
	{
		InterfacePtr<ITreeViewController> controller(this, UseDefaultIID());	
		NodeIDList selectedItems;
		controller->GetSelectedItems(selectedItems);	
			
		IDFile formeFile, parentFormeFile, folderClasseur, previewFile ;			
			// We've got single selection only
		if(selectedItems.size()>0)
		{
			TreeNodePtr<XPGUIFormeNodeID> nodeID(selectedItems[0]);
			ASSERT(nodeID);
			if(!nodeID)	
			{
		
			}

			if(nodeID->GetType() == XPGFormeDataNode::kFormeNode)
			{
				// Set file to display for image panel
				PMString filepath(nodeID->GetData());
				formeFile = FileUtils::PMStringToSysFile(filepath);
				PMString previewName =kNullString;
				FileUtils::GetFileName(formeFile,previewName);
				
				int32 extPos = previewName.LastIndexOfCharacter(PlatformChar('.'));
				if(extPos != -1)
					previewName.Remove(extPos, previewName.NumUTF16TextChars()-extPos); 				

				FileUtils::GetParentDirectory(formeFile, parentFormeFile);
				FileUtils::GetParentDirectory(parentFormeFile, folderClasseur );

				InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
				
				PMString previewPath = FileUtils::SysFileToPMString(folderClasseur);
				previewPath.Append(FileUtils::GetDirectorySeperator());
				previewPath.Append("Vignette");
				previewPath.Append(FileUtils::GetDirectorySeperator());
				previewPath.Append(previewName);
				previewPath.Append(".jpg");
				previewFile = FileUtils::PMStringToSysFile(previewPath);
			}
			else			
				previewFile = IDFile();
		}
						
		InterfacePtr<IWidgetParent> widgetParent (this, UseDefaultIID());
		InterfacePtr<IPanelControlData> hilitePanelCtrlData ((IPanelControlData *)widgetParent->QueryParentFor(IID_IPANELCONTROLDATA));

		InterfacePtr<IWidgetParent> hilitePanelParent (hilitePanelCtrlData, UseDefaultIID());
		InterfacePtr<IPanelControlData> panelCtrlData ((IPanelControlData *)hilitePanelParent->QueryParentFor(IID_IPANELCONTROLDATA));

		IControlView * imageFormePanelView = panelCtrlData->FindWidget(kXPGUIDisplayImageFormePanelWidgetID);
		InterfacePtr<ISysFileData> imageFormePanelFileData (imageFormePanelView, UseDefaultIID());
		imageFormePanelFileData->Set(previewFile);
		imageFormePanelView->Invalidate();


				
	} while(kFalse);
}

//	end, File: XPGUIImagesViewObserver.cpp
