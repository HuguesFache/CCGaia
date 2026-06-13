
#include "VCPlugInHeaders.h"

// Interface includes:
#include "CIdleTask.h"
#include "IIntData.h"
#include "IStringData.h"
#include "IBoolData.h"
#include "ICoreFilename.h"
#include "IWebServices.h"
#include "IUserInfoUtils.h"
#include "IWorkspace.h"
#include "ISubject.h"
#include "IXRailPrefsData.h"
#include "Utils.h"

// General includes:
#include "CAlert.h"
#include "CDialogController.h"
#include "SystemUtils.h"
#include "StreamUtil.h"
#include "DocUtils.h"
#include "SDKFileHelper.h"

// Project includes:
#include "XRLUIID.h"
#include "XRLID.h"
#include "XRCID.h"


/** XPBCLoginDialogController
	Methods allow for the initialization, validation, and application of dialog widget values.

	Implements IDialogController based on the partial implementation CDialogController.

	@author GD
*/


class XRailLoginDialogController : public CDialogController
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRailLoginDialogController(IPMUnknown* boss) : CDialogController(boss), userID(kNullString), theUser(kNullString) {}

	/**
		Initialize each widget in the dialog with its default value.
		Called when the dialog is opened.
	*/
	virtual void InitializeDialogFields(IActiveContext* dlgContext);

	/**
		Validate the values in the widgets.
		By default, the widget with ID kOKButtonWidgetID causes
		ValidateFields to be called. When all widgets are valid,
		ApplyFields will be called.
		@return kDefaultWidgetId if all widget values are valid, WidgetID of the widget to select otherwise.

	*/
	virtual WidgetID ValidateDialogFields(IActiveContext* myContext);

	/**
		Retrieve the values from the widgets and act on them.
		@param widgetId identifies the widget on which to act.
	*/
	virtual void ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);

	virtual void UserCancelled();

private:
	PMString userID, theUser;

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XRailLoginDialogController, kXRLUILoginDialogControllerImpl)

/* ApplyFields
*/
void XRailLoginDialogController::InitializeDialogFields(IActiveContext* dlgContext)
{
	PMString		urlXR, lastUser;
	bool16 modeRobot = kTrue;
	K2Vector<PMString> listeBases;
	K2Vector<PMString> IPBases;

	CDialogController::InitializeDialogFields(dlgContext);

	// Put code to initialize widget values here.
	do {
		GetPrefs(urlXR, lastUser, modeRobot, listeBases, IPBases);
		lastUser.SetTranslatable(kFalse);
		this->SetTextControlData(kXRLUILoginUserWidgetID, lastUser);

		// "Remember me": if the user checked the box last time, pre-fill the
		// password and tick the checkbox; otherwise leave the password empty.
		bool16 rememberMe = kFalse;
		PMString rememberedPassword("", PMString::kUnknownEncoding);
		GetRememberedCredentials(rememberMe, rememberedPassword);
		rememberedPassword.SetTranslatable(kFalse);

		// DIAG: si le password sauvegardé est anormalement long, c'est qu'on lit
		// du garbage en post-EOF dans XRailPrefsDataPersist::ReadWrite. On clamp
		// à 256 chars (largement assez pour un vrai mdp) et on force le checkbox
		// à off pour empêcher tout envoi d'un POST gigantesque.
		if (rememberedPassword.NumUTF16TextChars() > 256) {
			rememberedPassword = "";
			rememberMe = kFalse;
		}

		this->SetTextControlData(kXRLUIPasswordUserWidgetID, rememberedPassword);
		this->SetTriStateControlData(kXRLUIRememberMeWidgetID,
		                             rememberMe ? ITriStateControlData::kSelected
		                                        : ITriStateControlData::kUnselected);
	} while (kFalse);
}

/* ValidateFields
*/
WidgetID XRailLoginDialogController::ValidateDialogFields(IActiveContext* dlgContext)
{

	WidgetID badWidgetID = kInvalidWidgetID;
	PMString result, thePassword;
	do {
		// Recuperation du texte des champs
		theUser = this->GetTextControlData(kXRLUILoginUserWidgetID);
		thePassword = this->GetTextControlData(kXRLUIPasswordUserWidgetID);

		if (theUser.NumUTF16TextChars() == 0) {
			badWidgetID = kXRLUILoginUserWidgetID;
			CAlert::InformationAlert(PMString(kXRLUIErrorBlankLoginKey));
			break;
		}

		// Connexion a XRail
		int previewResult, pdfResult;
		PMString baseName;

		PMString serverAddress = GetGoodUrlXR(baseName);
		InterfacePtr<IWebServices> xrailHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
		userID = "";
		bool16 connexionOK = xrailHTTP->VerifLogin_v2(serverAddress, theUser, thePassword, userID, result, pdfResult, previewResult);

		if (!connexionOK)
		{
			// Server unreachable (database down, network issue, …). Block the
			// dialog from closing — without setting badWidgetID, ValidateDialog
			// returns kInvalidWidgetID, the SDK treats that as success and
			// fires ApplyDialogFields, which used to grant access to the
			// XPage / Photos liées palettes despite the failed login.
			badWidgetID = kXRLUIPasswordUserWidgetID;
			CAlert::ErrorAlert(PMString(kXRLErrorWebServiceKey));
		}
		else {

			if (result == "loginOK") {

				// Store user ID for the session
				Utils<IUserInfoUtils>()->SetInCopyUsername(theUser);

				InterfacePtr<IStringData> gLogin(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);
				gLogin->Set(userID);

				InterfacePtr<IStringData> gLoginString(GetExecutionContextSession(), IID_ILOGINSTRING);
				gLoginString->Set(theUser);

				InterfacePtr<IBoolData> createPreviewBool(GetExecutionContextSession(), IID_ICREATEPREVIEWBOOL);
				InterfacePtr<IBoolData> createPDFBool(GetExecutionContextSession(), IID_ICREATEPDFBOOL);

				if (previewResult == 0)
					createPreviewBool->Set(kFalse);
				else if (previewResult == 1)
					createPreviewBool->Set(kTrue);
				if (pdfResult == 0)
					createPDFBool->Set(kFalse);
				else if (pdfResult == 1)
					createPDFBool->Set(kTrue);

				// Save login for next identification
				PMString oldurlXR, user, baseName;
				bool16 modeRobot = kFalse;
				K2Vector<PMString> oldlisteBases;
				K2Vector<PMString> oldIPBases;
				GetPrefs(oldurlXR, user, modeRobot, oldlisteBases, oldIPBases);

				SetPrefs(oldurlXR, theUser, modeRobot, oldlisteBases, oldIPBases);

				// "Remember me": persist (or clear) the password according to
				// the current checkbox state. Done AFTER a successful login so
				// we never store a bad password.
				bool16 rememberMe =
					(this->GetTriStateControlData(kXRLUIRememberMeWidgetID) == ITriStateControlData::kSelected);
				SetRememberedCredentials(rememberMe, thePassword);

				// ---------------------------------------------------------------
				// Re-fetch XRail server-side prefs now that we know the server
				// is reachable (VerifLogin just succeeded). This covers the
				// common case where the plugin booted offline, got defaults,
				// and the user later signs in once the server is back up.
				//
				// XRail-only: we don't touch IXPGPreferences here so the
				// installation works when the XPage plugin isn't loaded.
				// XPage palettes (when present) refetch their own prefs in
				// response to kXRLUILoginChangedMsg below.
				// ---------------------------------------------------------------
				{
					int32 prefs_ImportPub = 0, prefs_PubProp = 0, prefs_PDFMultipages = 1, prefs_ExportIDML = 0, prefs_MakePreview = 1;
					PMString prefs_AdTemplateNoVisual, prefs_AdTemplateWithVisual;

					// fromServeur was parsed at startup from Gaia.ini and
					// stored on the session by XRailStartupShutdown.
					InterfacePtr<IIntData> fromServeurData(GetExecutionContextSession(), IID_IXRAILFROMSERVEUR);
					int32 fromServeurInt = fromServeurData ? fromServeurData->Get() : 0;

					if (xrailHTTP->GetPrefsXRail_v2(serverAddress, fromServeurInt,
						prefs_ImportPub, prefs_PubProp, prefs_PDFMultipages, prefs_ExportIDML, prefs_MakePreview,
						prefs_AdTemplateNoVisual, prefs_AdTemplateWithVisual))
					{
						InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace());
						InterfacePtr<IXRailPrefsData> xrailPrefsData((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));
						if (xrailPrefsData)
						{
							xrailPrefsData->SetImportPub(prefs_ImportPub);
							xrailPrefsData->SetPubProp(prefs_PubProp);
							xrailPrefsData->SetPDFMultipages(prefs_PDFMultipages);
							xrailPrefsData->SetExportIDML(prefs_ExportIDML);
							xrailPrefsData->SetMakePreview(prefs_MakePreview);
							xrailPrefsData->SetAdTemplateNoVisual(prefs_AdTemplateNoVisual);
							xrailPrefsData->SetAdTemplateWithVisual(prefs_AdTemplateWithVisual);
						}
					}
					// Silent on failure: VerifLogin just worked, so a failure
					// here is unusual but not worth bothering the user with.
				}

				// Notify any subscribed palettes (XPage Articles + Photos
				// liées, when XPage is installed) so they can refresh their
				// own prefs and swap from the "Sign in" prompt to their
				// normal content. No subscribers when XPage is absent — the
				// broadcast is harmless in that case.
				InterfacePtr<ISubject> sessionSubject(GetExecutionContextSession(), UseDefaultIID());
				if (sessionSubject)
					sessionSubject->Change(kXRLUILoginChangedMsg, IID_IXRLUILOGINPROTOCOL);
			}
			else if (result == "badPassword") {
				badWidgetID = kXRLUIPasswordUserWidgetID;
				CAlert::ErrorAlert(PMString(kXRLErrorBadPasswordKey));
			}
			else if (result == "privilegemissing") {
				badWidgetID = kXRLUIPasswordUserWidgetID;
				CAlert::ErrorAlert(PMString(kXRLErrorPrivilegeKey));
			}
			else if (result == "unknownUser") {
				badWidgetID = kXRLUILoginUserWidgetID;
				CAlert::ErrorAlert(PMString(kXRLErrorUnknownUserKey));
			}
			else {
				badWidgetID = kXRLUIPasswordUserWidgetID;
				CAlert::ErrorAlert(PMString(kXRLErrorSOAPKey));
			}
		}

	} while (kFalse);

	return badWidgetID;
}

/* ApplyFields
*/
void XRailLoginDialogController::ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId)
{
	// Intentionally empty. The login session strings (IID_ISTRINGDATA_LOGIN
	// + IID_ILOGINSTRING) are populated in the "loginOK" branch of
	// ValidateDialogFields above — only after the server confirms the
	// credentials. Setting them here unconditionally would grant palette
	// access on a connection failure (server unreachable / bad password
	// after the alert was acknowledged).
}


void XRailLoginDialogController::UserCancelled()
{
	InterfacePtr<IStringData> gLogin(GetExecutionContextSession(), IID_ISTRINGDATA_LOGIN);
	gLogin->Set("");

	InterfacePtr<IStringData> gLoginString(GetExecutionContextSession(), IID_ILOGINSTRING);
	gLoginString->Set("");

	// Broadcast so any subscribed palette re-renders its "Sign in" prompt —
	// cancelling the dialog wipes gLogin above, so a palette that was
	// already showing normal content needs to swap back.
	InterfacePtr<ISubject> sessionSubject(GetExecutionContextSession(), UseDefaultIID());
	if (sessionSubject)
		sessionSubject->Change(kXRLUILoginChangedMsg, IID_IXRLUILOGINPROTOCOL);
}

// End, XRailLoginDialogController.cpp.


