#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"

// Interface includes
#include "ISubject.h"
#include "IFormeData.h"

// Project includes
#include "XPGID.h"
#include "CAlert.h"

/** XPGSetFormeDataCmd
*/
class XPGSetFormeDataCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGSetFormeDataCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XPGSetFormeDataCmd();
	
		/**
		@return kFalse don't use this command if memory is low. 
	*/
	bool16 LowMemIsOK() const { return kFalse; }

protected:
	/**
		The Do method is where all the action happens. 
	*/
	void Do();

	/**
		Notify interested observer about the change we have done in this command.
	*/
	void DoNotify();

	/**
		Gives the name that shows up under the Edit menu and allows the user to undo or redo the command.

		@return pointer to a PMString allocated using the new operator. Deleted by caller.
	*/
	PMString* CreateName();

private :

};


CREATE_PMINTERFACE(XPGSetFormeDataCmd, kXPGSetFormeDataCmdImpl)

/* Constructor
*/
XPGSetFormeDataCmd::XPGSetFormeDataCmd(IPMUnknown* boss) 
: Command(boss)
{
	SetUndoability(kAutoUndo);
}

/* Destructor
*/
XPGSetFormeDataCmd::~XPGSetFormeDataCmd()
{
}

/* Do
*/
void XPGSetFormeDataCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		int32 length = fItemList.Length();
		if(length == 0)
			break;

		// Get forme data to set
		InterfacePtr<IFormeData> cmdData (this, UseDefaultIID());	
		status = kSuccess;
		for(int32 i = 0 ; i < length ; ++i)
		{
			InterfacePtr<IFormeData> formeData (fItemList.GetRef(i), UseDefaultIID());
			if(formeData == nil){
				status = kFailure;
				break;
			}
				
			// Apply string
			formeData->SetUniqueName(cmdData->GetUniqueName());
			formeData->SetType(cmdData->GetType());
			formeData->SetPhotoIndex(cmdData->GetPhotoIndex());
			formeData->SetMatchingTagsNames(cmdData->GetMatchingTagsNames());
		}
	
	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void XPGSetFormeDataCmd::DoNotify()
{
	ErrorCode status = kFailure;
			
	do{
		int32 length = fItemList.Length();
		if(length == 0)
			break;

		for(int32 i = 0 ; i < length ; ++i){
			// Notify the subject of the change
			InterfacePtr<ISubject> subject (fItemList.GetRef(i), UseDefaultIID());
			subject->Change(kXPGSetFormeDataCmdBoss, IID_IFORMEDATA, this);
		}
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}


/* CreateName
*/
PMString * XPGSetFormeDataCmd::CreateName()
{
	PMString * name = new PMString("Set Forme Data");
	name->SetTranslatable(kFalse);
	return name;
}
