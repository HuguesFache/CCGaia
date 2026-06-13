/*
//	File:	PMUTypeRefDialogController.cpp
//
//	Author:	Trias
//
//	Date:	26/10/2005
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IPanelControlData.h"
#include "IControlView.h"

// General includes:
#include "CDialogController.h"

#include "CAlert.h"

// Project includes:
#include "PMUID.h"

/** 
	Preferences dialog controller
*/
class PMUTypeRefDialogController : public CDialogController
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PMUTypeRefDialogController(IPMUnknown* boss) : CDialogController(boss) {}

		/**
			Initializes each widget in the dialog with its default value.
			Called when the dialog is opened.
			@param dlgContext
		*/
		virtual void InitializeDialogFields( IActiveContext* dlgContext);

		/**
			Validate the values in the widgets. 
			By default, the widget with ID kOKButtonWidgetID causes 
			this method to be called. When all widgets are valid, 
			ApplyFields will be called.		
			@param myContext
			@return kDefaultWidgetId if all widget values are valid, WidgetID of the widget to select otherwise.

		*/
		virtual WidgetID ValidateDialogFields( IActiveContext* myContext);

		/**
			Retrieve the values from the widgets and act on them.
			@param myContext
			@param widgetId identifies the widget on which to act.
		*/
		virtual void ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId);


		virtual void UserCancelled();

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its 
 ImplementationID making the C++ code callable by the 
 application.
*/
CREATE_PMINTERFACE(PMUTypeRefDialogController, kPMUTypeRefDialogControllerImpl)

/* ApplyFields
*/
void PMUTypeRefDialogController::InitializeDialogFields( IActiveContext* dlgContext){}

/* ValidateFields
*/
WidgetID PMUTypeRefDialogController::ValidateDialogFields( IActiveContext* myContext) 
{
	WidgetID result = kNoInvalidWidgets;

	if(GetTextControlData(kPMURefEditWidgetID) == kNullString)
		result = kPMURefEditWidgetID;

	return result;
}


/* ApplyFields
*/
void PMUTypeRefDialogController::ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId) 
{ 
}

/* UserCancelled
*/
void PMUTypeRefDialogController::UserCancelled()
{
	SetTextControlData(kPMURefEditWidgetID, kNullString);
}

