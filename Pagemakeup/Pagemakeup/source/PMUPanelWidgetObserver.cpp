/*
//	File:	PMUPanelWidgetObserver.cpp
//
//	Date:	14-Apr-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//	
//	NOTICE: Adobe permits you to use, modify, and distribute this file in
//	accordance with the terms of the Adobe license agreement accompanying it.
//	If you have received this file from a source other than Adobe, then your
//	use, modification, or distribution of it requires the prior written
//	permission of Adobe.
//
*/

#include "VCPluginHeaders.h"

// Interface includes
#include "IControlView.h"
#include "IPanelControlData.h"
#include "ISubject.h"
#include "ITriStateControlData.h"
#include "IPageMakeUpPrefs.h"
#include "ICommand.h"
#include "ISelectionManager.h"
#include "IParserSuite.h"
#include "ISelectionUtils.h"
#include "IDocument.h"
#include "ILayoutUIUtils.h"
#include "TransformUtils.h"
#include "IHierarchy.h"
#include "IDocumentList.h"
#include "IApplication.h"
#include "IDocumentUtils.h"
#include "IDocFileHandler.h"

// General includes
#include "CObserver.h"
#include "CAlert.h"
#include "ErrorUtils.h"

// Project includes
#include "PMUID.h"
#include "PMUUtils.h"
#include "ExportUtils.h"
#include "GlobalDefs.h"

/** PMUPanelWidgetObserver
	Launch different functions when icons are clicked.

	@author Trias
*/


class PMUPanelWidgetObserver : public CObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PMUPanelWidgetObserver( IPMUnknown* boss );
	/** Destructor
	*/
	virtual ~PMUPanelWidgetObserver() ;

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

	void SetDisplayRef(IControlView * displayRefCtrlView);
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(PMUPanelWidgetObserver, kPMUPanelWidgetObserverImpl)

/* Constructor
*/
PMUPanelWidgetObserver::PMUPanelWidgetObserver( IPMUnknown* boss ) :
	CObserver( boss )
{
}

/* Destructor
*/
PMUPanelWidgetObserver::~PMUPanelWidgetObserver()
{
}

/* AutoAttach
*/
void PMUPanelWidgetObserver::AutoAttach()
{
	do
	{
		// Initialise widget state.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("AutoAttach() invalid panelControlData");
			break;
		}


		// Attach widget observers.
		this->AttachWidget(panelControlData, kPMUExecTCLWidgetID, ITriStateControlData::kDefaultIID);
		this->AttachWidget(panelControlData, kPMUReverseTCLWidgetID, ITriStateControlData::kDefaultIID);
#if !COMPOSER
		this->AttachWidget(panelControlData, kPMUTCLPrefsWidgetID, ITriStateControlData::kDefaultIID);
#endif
		// Initialize "Display references" check box according to PageMakeUp preferences
		IControlView* displayRefCtrlView = panelControlData->FindWidget(kPMUDisplayTCLRefWidgetID);
		InterfacePtr<ITriStateControlData> displayRefValue(displayRefCtrlView, UseDefaultIID());
		
		InterfacePtr<IPageMakeUpPrefs> currentPreferences ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (currentPreferences == nil)
			break;
		
		if(currentPreferences->getDisplayTCLRef())
			displayRefValue->Select();
		else
			displayRefValue->Deselect();
		this->AttachWidget(panelControlData, kPMUDisplayTCLRefWidgetID, ITriStateControlData::kDefaultIID);
#if PERMREFS
		this->AttachWidget(panelControlData, kPMUAddPermRefWidgetID, ITriStateControlData::kDefaultIID);
		this->AttachWidget(panelControlData, kPMUAddTCLRefWidgetID, ITriStateControlData::kDefaultIID);
		this->AttachWidget(panelControlData, kPMUUpdatePictureWidgetID, ITriStateControlData::kDefaultIID);
		this->AttachWidget(panelControlData, kPMUExportPermRefsWidgetID, ITriStateControlData::kDefaultIID);
		this->AttachWidget(panelControlData, kPMUSearchPermRefWidgetID, ITriStateControlData::kDefaultIID);
#if COMPOSER
		this->AttachWidget(panelControlData, kPMUTextOverFlowWidgetID, ITriStateControlData::kDefaultIID);
#else
		this->AttachWidget(panelControlData, kPMUSearchTCLRefWidgetID, ITriStateControlData::kDefaultIID);
		this->AttachWidget(panelControlData, kPMUExportWidgetID, ITriStateControlData::kDefaultIID);
#endif
#endif

	} while(false);

}

/* AutoDetach
*/
void PMUPanelWidgetObserver::AutoDetach()
{
	do
	{
		// Detach widget observers.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("AutoDetach() invalid panelControlData");
			break;
		}
		
		this->DetachWidget(panelControlData, kPMUExecTCLWidgetID, ITriStateControlData::kDefaultIID);
		this->DetachWidget(panelControlData, kPMUReverseTCLWidgetID, ITriStateControlData::kDefaultIID);
#if !COMPOSER
		this->DetachWidget(panelControlData, kPMUTCLPrefsWidgetID, ITriStateControlData::kDefaultIID);
#endif
		this->DetachWidget(panelControlData, kPMUDisplayTCLRefWidgetID, ITriStateControlData::kDefaultIID);
#if PERMREFS
		this->DetachWidget(panelControlData, kPMUAddPermRefWidgetID, ITriStateControlData::kDefaultIID);
		this->DetachWidget(panelControlData, kPMUAddTCLRefWidgetID, ITriStateControlData::kDefaultIID);
		this->DetachWidget(panelControlData, kPMUUpdatePictureWidgetID, ITriStateControlData::kDefaultIID);
		this->DetachWidget(panelControlData, kPMUExportPermRefsWidgetID, ITriStateControlData::kDefaultIID);
		this->DetachWidget(panelControlData, kPMUSearchPermRefWidgetID, ITriStateControlData::kDefaultIID);
#if COMPOSER
		this->DetachWidget(panelControlData, kPMUTextOverFlowWidgetID, ITriStateControlData::kDefaultIID);		
#else
		this->DetachWidget(panelControlData, kPMUSearchTCLRefWidgetID, ITriStateControlData::kDefaultIID);
		this->DetachWidget(panelControlData, kPMUExportWidgetID, ITriStateControlData::kDefaultIID);		
#endif

#endif
		
	} while(false);

}

/* Update
*/
void PMUPanelWidgetObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{
	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());
	if (controlView != nil)
	{
		if (theChange == kTrueStateMessage)
		{
		
			WidgetID widgetID = controlView->GetWidgetID();
			switch (widgetID.Get())
			{
				case kPMUExecTCLWidgetID:
					PMUUtils::ExecTCL();
					break;
				
				case kPMUReverseTCLWidgetID :
					PMUUtils::ReverseTCL();
					break;

				case kPMUTCLPrefsWidgetID :
					PMUUtils::ShowTCLPrefs();
					break;

				case kPMUExportWidgetID :
					{
						IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
						if(doc != nil)
						{
							// First save doc
							UIDRef docUIDRef = ::GetUIDRef(doc);
							InterfacePtr<IDocFileHandler> docFileHandler (Utils<IDocumentUtils>()->QueryDocFileHandler(docUIDRef));
							if(docFileHandler->CanSave(docUIDRef))
							{
								ErrorCode status = kFailure;
								
								docFileHandler->Save(docUIDRef);

								status = ErrorUtils::PMGetGlobalErrorCode();
								if(status != kSuccess)
									break;

								InterfacePtr<IPageMakeUpPrefs> currentPreferences ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
								if (currentPreferences == nil)
									break;
								
								if(currentPreferences->getExportType() == PDF_EXPORT)
									status = ExportUtils::PdfExport(doc,"",kTrue,kTrue);
								else
									status = ExportUtils::EpsExport(doc,kTrue,kTrue);

								if(status == kSuccess)
									docFileHandler->Close(docUIDRef, kSuppressUI, kFalse);
							}														
						}
					}
					break;	

				case kPMUDisplayTCLRefWidgetID :
					SetDisplayRef(controlView);
					break;

#if PERMREFS
				case kPMUAddPermRefWidgetID :
					PMUUtils::AddPermRef();
					break;

				case kPMUAddTCLRefWidgetID :
					PMUUtils::AddTCLRef();
					break;

				case kPMUTextOverFlowWidgetID :
					PMUUtils::SearchOversetBox();
					break;

				case kPMUUpdatePictureWidgetID :
					PMUUtils::UpdatePicture();
					break;
					
				case kPMUExportPermRefsWidgetID :
					PMUUtils::ExportPermRefs();
					break;
					
				case kPMUSearchPermRefWidgetID :
					PMUUtils::SearchPermRef();
					break;	
#if !COMPOSER		 
				case kPMUSearchTCLRefWidgetID :
					PMUUtils::SearchTCLRef();
					break;	
#endif

#endif
				default :
					break;
			}
		}
		
		else if(theChange == kFalseStateMessage)
		{
			WidgetID widgetID = controlView->GetWidgetID();
			if(widgetID.Get() == kPMUDisplayTCLRefWidgetID)
			{
				SetDisplayRef(controlView);
			}
		}
	}
}


/* AttachWidget
*/
void PMUPanelWidgetObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
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
void PMUPanelWidgetObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID)
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

void PMUPanelWidgetObserver::SetDisplayRef(IControlView * displayRefCtrlView)
{
	do
	{
		
		InterfacePtr<IPageMakeUpPrefs> currentPreferences ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (currentPreferences == nil)
			break;

		// Update the preferences with a command
		UIDList workspace(::GetUIDRef(currentPreferences));
		InterfacePtr<ICommand> setPrefsCmd (CmdUtils::CreateCommand(kPrsSetPreferencesCmdBoss));
		if(setPrefsCmd == nil)
			break;

		setPrefsCmd->SetItemList(workspace);

		InterfacePtr<IPageMakeUpPrefs> cmdData (setPrefsCmd, UseDefaultIID());
		if(cmdData == nil)
			break;

		cmdData->CopyFrom(currentPreferences);

		// Get new preferences from widget value
		InterfacePtr<ITriStateControlData> displayRefValue(displayRefCtrlView, UseDefaultIID());

		cmdData->setDisplayTCLRef(displayRefValue->IsSelected());

		if(CmdUtils::ProcessCommand(setPrefsCmd) != kSuccess)
			break;
		
		// Update views of all opened documents
		InterfacePtr<IApplication> app (GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> docList (app->QueryDocumentList());

		int32 nbDocs = docList->GetDocCount();
		for(int32 i = 0 ; i < nbDocs ; ++i)
			Utils<ILayoutUIUtils>()->InvalidateViews(docList->GetNthDoc(i));

	} while(kFalse);
}

// End, PMUPanelWidgetObserver.cpp.


