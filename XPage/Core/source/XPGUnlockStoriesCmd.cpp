
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IDocument.h"
#include "IAssignmentMgr.h"
#include "IAssignmentUtils.h"
#include "IItemLockData.h"
#include "IUIFlagData.h"
#include "ILiveEditFacade.h"
#include "FileUtils.h"
// General includes:
#include "CmdUtils.h"
#include "Command.h"
#include "ErrorUtils.h"
#include "XPGID.h"
#include "CAlert.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"

/** Implementation of command to set tag to style map in spline item
*/
class XPGUnlockStoriesCmd : public Command
{
public:
	/** Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.*/
	XPGUnlockStoriesCmd(IPMUnknown* boss);

	/** Destructor
	*/
	~XPGUnlockStoriesCmd();

protected:

	/** Do
		@see Command::Do
	*/
	virtual void Do();

	/** CreateName
		@see Command::CreateName
	*/
	virtual PMString* CreateName();

private:
	// Check if we can check out the assignment
	bool16 canCheckOutAssignment(const PMString& idArticle);
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUnlockStoriesCmd, kXPGUnlockStoriesCmdImpl)

/* Constructor
*/
XPGUnlockStoriesCmd::XPGUnlockStoriesCmd(IPMUnknown* boss) : Command(boss)
{
	SetUndoability(ICommand::kAutoUndo);
}

/** Destructor
*/
XPGUnlockStoriesCmd::~XPGUnlockStoriesCmd()
{
}

/* Do
*/
void XPGUnlockStoriesCmd::Do()
{
	do
	{
		InterfacePtr<IAssignmentMgr> assignmentMgr(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IDocument> theDoc(fItemList.GetRef(0), IID_IDOCUMENT);

		// Check whether it's is a saved assignment
		IDataBase* db = fItemList.GetDataBase();
		if (!db->GetSysFile())
			break;

		IDFile assignFile = *db->GetSysFile();

		IAssignedDocument* assignDoc = assignmentMgr->GetAssignedDocument(db);
		if (!assignDoc)
			break;

		// Get all assigned stories in the document
		UIDList storyList(db), unlockList(db);
		assignmentMgr->GetStoryListOnDoc(assignDoc, storyList);

		// Go through stories looking for locked ones
		for (int32 i = 0; i < storyList.size(); ++i) {
			InterfacePtr<IItemLockData> lockData(storyList.GetRef(i), UseDefaultIID());
			if (lockData && lockData->GetInsertLock())
				unlockList.Append(storyList.At(i));
		}
		//Check Out the selected assignment after update
		if (!unlockList.IsEmpty())
			Utils<Facade::ILiveEditFacade>()->CheckOut(storyList, ICommand::kRegularUndo, nil, kFalse, kFullUI);

	} while (kFalse);
}



/* CreateName
*/
PMString* XPGUnlockStoriesCmd::CreateName()
{
	return new PMString("Unlock all stories");
}

// End, XPGUnlockStoriesCmd.cpp.


bool16 XPGUnlockStoriesCmd::canCheckOutAssignment(const PMString& idArticle)
{
	bool16 canUpdateArticle = kFalse;
	do {
		//TODO PLUGINS INCOPY
		/*
		InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IWebServices> xrailConnexion (::CreateObject2<IWebServices>(kXRCXRailClientBoss));
		xrailConnexion->SetServerAddress(xpgPrefs->GetXRail_URL());

		PMString section = kNullString, ssSection = kNullString, colorStatus = kNullString, incopyPath = kNullString,
				 currentStatus = kNullString, sujet = kNullString, proprio = kNullString, result = kNullString;

		if(!xrailConnexion->StoryGetInfo(idArticle, section, ssSection , currentStatus, colorStatus,
										 incopyPath, sujet, proprio, result))
		{
			PMString error = ErrorUtils::PMGetGlobalErrorString();
			ErrorUtils::PMSetGlobalErrorCode(kSuccess);
			CAlert::InformationAlert(error);
			break;
		}


		// On verifie est ce qu'on a le droit d'extraire le bloc
		int i = 1;
		K2::scoped_ptr<PMString> status (xpgPrefs->GetStatus().GetItem(";", i));
		while(status != nil){
			if(currentStatus == *status) {
				 canUpdateArticle = kTrue;
			}
			++i;
			status.reset(xpgPrefs->GetStatus().GetItem(";", i));
		}
		*/
	} while (kFalse);

	return canUpdateArticle;
}