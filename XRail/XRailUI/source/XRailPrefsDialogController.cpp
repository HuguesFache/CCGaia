
#include "VCPlugInHeaders.h"

// Interface includes:
#include "ITextControlData.h"
#include "IPanelControlData.h"
#include "ITriStateControlData.h"
#include "IBoolData.h"

// General includes:
#include "CDialogController.h"
#include "SystemUtils.h"

// Project includes:
#include "XRLUIID.h"
#include "DocUtils.h"


/** Implements IDialogController based on the partial implementation CDialogController;
	its methods allow for the initialization, validation, and application of dialog widget values.

	The methods take an additional parameter for 3.0, of type IActiveContext.
	See the design document for an explanation of the rationale for this
	new parameter and the renaming of the methods that CDialogController supports.


	@ingroup basicdialog
	@author Rodney Cook
*/
class XRailDialogController : public CDialogController
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRailDialogController(IPMUnknown* boss) : CDialogController(boss) {}

	/**
		Initializes each widget in the dialog with its default value.
		Called when the dialog is opened.
		@param dlgContext
	*/
	virtual void InitializeDialogFields(IActiveContext* dlgContext);

	/**
		Validate the values in the widgets.
		By default, the widget with ID kOKButtonWidgetID causes
		this method to be called. When all widgets are valid,
		ApplyFields will be called.
		@param myContext
		@return kDefaultWidgetId if all widget values are valid, WidgetID of the widget to select otherwise.

	*/
	virtual WidgetID ValidateDialogFields(IActiveContext* myContext);

	/**
		Retrieve the values from the widgets and act on them.
		@param myContext
		@param widgetId identifies the widget on which to act.
	*/
	virtual void ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XRailDialogController, kXRLUIDialogControllerImpl)

/* ApplyFields
*/
void XRailDialogController::InitializeDialogFields(IActiveContext* dlgContext)
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());

	// Get type of preferences this dialog is intended for
	InterfacePtr<IBoolData> dialogType(this, UseDefaultIID());

	// Get current server address data
	PMString urlXR;

	PMString lastUser;
	bool16 modeRobot = kTrue;
	K2Vector<PMString> listeBases;
	K2Vector<PMString> IPBases;
	GetPrefs(urlXR, lastUser, modeRobot, listeBases, IPBases);

	// Display "mode robot"
	SetTextControlData(kPrefsTypeWidgetID, kXRLUIModeRobotTextKey);

	if (modeRobot)
		this->SetTriStateControlData(kPrefsTypeWidgetID, ITriStateControlData::kSelected);
	else
		this->SetTriStateControlData(kPrefsTypeWidgetID, ITriStateControlData::kUnselected);


	/*ip.SetTranslatable(kFalse);
	port.SetTranslatable(kFalse);

	PMString::ConversionError convError = PMString::kNoError;

	// Get the four ip address' fields
	K2::scoped_ptr<PMString> ipField(nil);
	int32 field = 0;

	ipField.reset(ip.GetItem(".",1));
	if(!ipField) // should not occur
		this->SetTextValue(kIPField1WidgetID, 0);
	else
	{
		field = (*ipField).GetAsNumber(&convError);
		this->SetTextValue(kIPField1WidgetID, field);
	}

	field = 0;
	ipField.reset(ip.GetItem(".",2));
	if(!ipField) // should not occur
		this->SetTextValue(kIPField2WidgetID, 0);
	else
	{
		field = (*ipField).GetAsNumber(&convError);
		this->SetTextValue(kIPField2WidgetID, field);
	}

	field = 0;
	ipField.reset(ip.GetItem(".",3));
	if(!ipField) // should not occur
		this->SetTextValue(kIPField3WidgetID, 0);
	else
	{
		field = (*ipField).GetAsNumber(&convError);
		this->SetTextValue(kIPField3WidgetID, field);
	}

	field = 0;
	ipField.reset(ip.GetItem(".",4));
	if(!ipField) // should not occur
		this->SetTextValue(kIPField4WidgetID, 0);
	else
	{
		field = (*ipField).GetAsNumber(&convError);
		this->SetTextValue(kIPField4WidgetID, field);
	}

	int32 portNumber = port.GetAsNumber(&convError);

	if(convError != PMString::kNoError) // Should not occur
		this->SetTextValue(kPortEditBoxWidgetID, 0);
	else
		this->SetTextValue(kPortEditBoxWidgetID, portNumber);	*/
}

/* ValidateFields
*/
WidgetID XRailDialogController::ValidateDialogFields(IActiveContext* myContext)
{
	WidgetID result = kNoInvalidWidgets;

	// Put code to validate widget values here.

	return result;
}

/* ApplyFields
*/
void XRailDialogController::ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId)
{
	// Get entered IP and port	
	/*PMString ip = this->GetTextControlData(kIPField1WidgetID) + ".";
	ip += this->GetTextControlData(kIPField2WidgetID) + ".";
	ip += this->GetTextControlData(kIPField3WidgetID) + ".";
	ip += this->GetTextControlData(kIPField4WidgetID);

	PMString port = this->GetTextControlData(kPortEditBoxWidgetID);

	// Get type of preferences this dialog is intended for
	InterfacePtr<IBoolData> dialogType (this, UseDefaultIID());

	*/
	PMString urlXR, lastUser;
	bool16 oldMode = kFalse;
	K2Vector<PMString> listeBases;
	K2Vector<PMString> IPBases;
	GetPrefs(urlXR, lastUser, oldMode, listeBases, IPBases);

	bool16 modeRobot = (this->GetTriStateControlData(kPrefsTypeWidgetID) == ITriStateControlData::kSelected);

	SetPrefs(urlXR, lastUser, modeRobot, listeBases, IPBases);
}

// End, XRailDialogController.cpp.



