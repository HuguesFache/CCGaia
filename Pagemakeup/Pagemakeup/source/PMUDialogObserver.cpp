/*
//	File:	PMUDialogObserver.cpp
//
//	Author:	Trias
//
//	Date:	14/10/2005
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IControlView.h"
#include "IPanelControlData.h"
#include "ISubject.h"
#include "IApplication.h"
#include "IDialogMgr.h"
#include "IDialog.h"
#include "IDialogController.h"

// General includes:
#include "CDialogObserver.h"
#include "CAlert.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"

// Project includes:
#include "PMUID.h"

/**	Implements IObserver based on the partial implementation CDialogObserver; 
	allows dynamic processing of preferences dialog widget changes	
*/
class PMUDialogObserver : public CDialogObserver
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PMUDialogObserver(IPMUnknown* boss) : CDialogObserver(boss) {}

		/** Destructor. */
		virtual ~PMUDialogObserver() {}

		/** 
			Called by the application to allow the observer to attach to the subjects 
			to be observed, in this case the dialog's info button widget. If you want 
			to observe other widgets on the dialog you could add them here. 
		*/
		virtual void AutoAttach();

		/** Called by the application to allow the observer to detach from the subjects being observed. */
		virtual void AutoDetach();

		/**
			Called by the host when the observed object changes, in this case when
			the dialog's info button is clicked.
			@param theChange specifies the class ID of the change to the subject. Frequently this is a command ID.
			@param theSubject points to the ISubject interface for the subject that has changed.
			@param protocol specifies the ID of the changed interface on the subject boss.
			@param changedBy points to additional data about the change. 
				Often this pointer indicates the class ID of the command that has caused the change.
		*/
		virtual void Update
		(
			const ClassID& theChange, 
			ISubject* theSubject, 
			const PMIID& protocol, 
			void* changedBy
		);
};


/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its 
 ImplementationID making the C++ code callable by the 
 application.
*/
CREATE_PMINTERFACE(PMUDialogObserver, kPMUDialogObserverImpl)

/* AutoAttach
*/
void PMUDialogObserver::AutoAttach()
{
	// Call the base class AutoAttach() function so that default behavior
	// will still occur (OK and Cancel buttons, etc.).
	CDialogObserver::AutoAttach();

	do
	{
		// Get the IPanelControlData interface for the dialog:
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("BscDlgDialogObserver::AutoAttach() panelControlData invalid");
			break;
		}
		
		AttachToWidget(kPMUDocButtonWidgetID, IID_IBOOLEANCONTROLDATA , panelControlData);
		AttachToWidget(kPMUImportImageButtonWidgetID, IID_IBOOLEANCONTROLDATA , panelControlData);
		AttachToWidget(kPMUExportTCLButtonWidgetID, IID_IBOOLEANCONTROLDATA , panelControlData);

	} while (false);
}

/* AutoDetach
*/
void PMUDialogObserver::AutoDetach()
{
	// Call base class AutoDetach() so that default behavior will occur (OK and Cancel buttons, etc.).
	CDialogObserver::AutoDetach();

	do
	{
		// Get the IPanelControlData interface for the dialog:
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
		{
			ASSERT_FAIL("BscDlgDialogObserver::AutoDetach() panelControlData invalid");
			break;
		}
		
		DetachFromWidget(kPMUDocButtonWidgetID, IID_IBOOLEANCONTROLDATA , panelControlData);
		DetachFromWidget(kPMUImportImageButtonWidgetID, IID_IBOOLEANCONTROLDATA , panelControlData);
		DetachFromWidget(kPMUExportTCLButtonWidgetID, IID_IBOOLEANCONTROLDATA , panelControlData);
		
	} while (false);
}

/* Update
*/
void PMUDialogObserver::Update
(
	const ClassID& theChange, 
	ISubject* theSubject, 
	const PMIID &protocol, 
	void* changedBy
)
{
	// Call the base class Update function so that default behavior will still occur (OK and Cancel buttons, etc.).
	CDialogObserver::Update(theChange, theSubject, protocol, changedBy);

	do
	{
		InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());
		if (controlView == nil)
		{
			ASSERT_FAIL("BscDlgDialogObserver::Update() controlView invalid");
			break;
		}

		// Get the button ID from the view.
		WidgetID theSelectedWidget = controlView->GetWidgetID();

		if (theChange == kTrueStateMessage)
		{
			RsrcID dialogResource = 0;
			switch(theSelectedWidget.Get())
			{
				case kPMUDocButtonWidgetID :
					dialogResource = kPMUDocPathDialogResourceID;
					break;

				case kPMUImportImageButtonWidgetID :
					dialogResource = kPMUImageSettingDialogResourceID;
					break;

				case kPMUExportTCLButtonWidgetID :
					dialogResource = kPMUExportPrefsDialogResourceID;
					break;
			}

			if(dialogResource != 0)
			{
				// Get the application interface and the DialogMgr.	
				InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
				if (application == nil)
					break;
	
				InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
				if (dialogMgr == nil)
					break;

				// Load the plug-in's resource.
				PMLocaleId nLocale = LocaleSetting::GetLocale();
				RsrcSpec dialogSpec
				(
					nLocale,					// Locale index from PMLocaleIDs.h. 
					kPMUPluginID,			// Our Plug-in ID from BasicDialogID.h. 
					kViewRsrcType,				// This is the kViewRsrcType.
					dialogResource,	// Resource ID for our dialog.
					kTrue						// Initially visible.
				);

				// CreateNewDialog takes the dialogSpec created above, and also
				// the type of dialog being created (kMovableModal).
				IDialog* dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);
				if (dialog == nil)
					break;

				// Get owner panel (PageMakeUp panel which stores preferences) interface to pass as a parameter of IDialog::Open method
				InterfacePtr<IDialogController> dialogController (this, UseDefaultIID());
				if(dialogController == nil)
					break;

				dialog->SetDialogFocusingAlgorithm(IDialog::kNoAutoFocus);
				// Open the dialog.
				dialog->Open(dialogController->GetOwner()); 
			}
		}

	} while (false);
}