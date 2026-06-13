
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IAssignmentMgr.h"
#include "IAssignment.h"
#include "IDocument.h"

// General includes:
#include "CAlert.h"
#include "CmdUtils.h"
#include "Command.h"
#include "ErrorUtils.h"
#include "XPGID.h"

/** Implementation of command to save assignment passed in parameters
*/
class XPGSaveAssignmentCmd : public Command
{
public:
	/** Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.*/
	XPGSaveAssignmentCmd(IPMUnknown* boss);

	/** Destructor
	*/
	~XPGSaveAssignmentCmd();

protected:

	/** Do
	 	@see Command::Do
	*/
	virtual void Do();
	
	/** CreateName
	 	@see Command::CreateName
	*/
	virtual PMString* CreateName();


};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGSaveAssignmentCmd, kXPGSaveAssignmentCmdImpl)

/* Constructor
*/
XPGSaveAssignmentCmd::XPGSaveAssignmentCmd(IPMUnknown* boss) : Command(boss)	
{
	SetUndoability(ICommand::kAutoUndo);
}

/** Destructor
*/
XPGSaveAssignmentCmd::~XPGSaveAssignmentCmd()
{
}

/* Do
*/
void XPGSaveAssignmentCmd::Do()
{
	ErrorCode status = kSuccess;
	do                      
	{
		InterfacePtr<IAssignmentMgr> assignmentMgr (GetExecutionContextSession(), UseDefaultIID());		

		// First we dirty each assignment, so that IAssignmentMgr::SaveAssignments(IDocument * doc)
		// can work correctly (if assignment are not dirty, no update is done with this method)
		int32 nbAssignments = fItemList.Length();
		for(int32 i = 0 ; i < nbAssignments ; ++i)
		{
			InterfacePtr<IAssignment> assign (fItemList.GetRef(i), UseDefaultIID());
			assign->SetDirty(kTrue);			
		}

		//DirectChangesAreDone(); // SetDirty causes direct changes to the model, calling DirectChangesAreDone avoid asserts

		// Get document
		InterfacePtr<IDocument> theDoc (fItemList.GetDataBase(), fItemList.GetDataBase()->GetRootUID(), UseDefaultIID());

		// Launch update
		status = assignmentMgr->SaveAssignments(theDoc);
	
	} while (kFalse);       

	// Handle any errors
	if (status != kSuccess) {
		ErrorUtils::PMSetGlobalErrorCode(status);
	}
}

/* CreateName
*/
PMString* XPGSaveAssignmentCmd::CreateName()
{
	return new PMString("Save assignment");
}

// End, XPGSaveAssignmentCmd.cpp.


