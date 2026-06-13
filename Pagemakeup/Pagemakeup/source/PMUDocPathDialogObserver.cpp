/*
//	File:	PMUDocPathDialogObserver.cpp
//
//	Author:	Trias
//
//	Date:	17/10/2005
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IControlView.h"
#include "IPanelControlData.h"
#include "ISubject.h"
#include "ITextControlData.h"

// General includes:
#include "CDialogObserver.h"
#include "CAlert.h"
#include "SDKFileHelper.h"
#include "FileUtils.h"

// Project includes:
#include "PMUID.h"

/**	Implements IObserver based on the partial implementation CDialogObserver; 
	allows dynamic processing of preferences dialog widget changes	
*/
class PMUDocPathDialogObserver : public CDialogObserver
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PMUDocPathDialogObserver(IPMUnknown* boss) : CDialogObserver(boss) {}

		/** Destructor. */
		virtual ~PMUDocPathDialogObserver() {}

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
CREATE_PMINTERFACE(PMUDocPathDialogObserver, kPMUDocPathDialogObserverImpl)

/* AutoAttach
*/
void PMUDocPathDialogObserver::AutoAttach()
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
		
		AttachToWidget(kPMUSelectSavePathWidgetID, IID_ITRISTATECONTROLDATA , panelControlData);
		AttachToWidget(kPMUSelectOpenPathWidgetID, IID_ITRISTATECONTROLDATA , panelControlData);

	} while (false);
}

/* AutoDetach
*/
void PMUDocPathDialogObserver::AutoDetach()
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

		DetachFromWidget(kPMUSelectSavePathWidgetID, IID_ITRISTATECONTROLDATA , panelControlData);
		DetachFromWidget(kPMUSelectOpenPathWidgetID, IID_ITRISTATECONTROLDATA , panelControlData);
		
	} while (false);
}

/* Update
*/
void PMUDocPathDialogObserver::Update
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
			SDKFolderChooser folderChooser;
				
			switch(theSelectedWidget.Get())
			{
				case kPMUSelectSavePathWidgetID :
				case kPMUSelectOpenPathWidgetID :
					folderChooser.ShowDialog();
					if (folderChooser.IsChosen())
					{
						PMString path = FileUtils::SysFileToPMString(folderChooser.GetIDFile());
						path.SetTranslatable(kFalse);
						InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
						if (panelControlData == nil)
							break;
					
						IControlView * textWidget = nil;
						if(theSelectedWidget.Get() == kPMUSelectOpenPathWidgetID)
							textWidget = panelControlData->FindWidget(kPMUOpenPathWidgetID);
						else
							textWidget = panelControlData->FindWidget(kPMUSavePathWidgetID);

						if(textWidget != nil)
						{
							InterfacePtr<ITextControlData> txtWidgetData (textWidget, UseDefaultIID());
							if(txtWidgetData != nil)
								txtWidgetData->SetString(path);
						}
					}

					break;

				default :
					break;
			}
		}
		

	} while (false);
}