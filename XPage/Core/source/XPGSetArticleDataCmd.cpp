#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"

// Interface includes
#include "ISubject.h"
#include "IArticleData.h"

// Project includes
#include "XPGID.h"
#include "CAlert.h"

/** XPGSetArticleDataCmd
*/
class XPGSetArticleDataCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGSetArticleDataCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XPGSetArticleDataCmd();
	
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
};


CREATE_PMINTERFACE(XPGSetArticleDataCmd, kXPGSetArticleDataCmdImpl)

/* Constructor
*/
XPGSetArticleDataCmd::XPGSetArticleDataCmd(IPMUnknown* boss):Command(boss){
	SetUndoability(kAutoUndo);
}

/* Destructor
*/
XPGSetArticleDataCmd::~XPGSetArticleDataCmd(){}

/* Do
*/
void XPGSetArticleDataCmd::Do(){
	ErrorCode status = kFailure;
	do{
		int32 length = fItemList.Length();
		if(length == 0)
			break;

		// Get Article data to set
		InterfacePtr<IArticleData> cmdData (this, UseDefaultIID());	
		status = kSuccess;
		for(int32 i = 0 ; i < length ; ++i){
			InterfacePtr<IArticleData> articleData (fItemList.GetRef(i), UseDefaultIID());
			if(articleData == nil){
				status = kFailure;
				break;
			}				
			// Apply properties
			articleData->SetUniqueName(cmdData->GetUniqueName());
			articleData->SetStatus(cmdData->GetStatus());		
		}	
	} while(false);

	if(status != kSuccess){
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void XPGSetArticleDataCmd::DoNotify(){
	ErrorCode status = kFailure;			
	do{
		int32 length = fItemList.Length();
		if(length == 0)
			break;

		for(int32 i = 0 ; i < length ; ++i)	{
			// Notify the subject of the change
			InterfacePtr<ISubject> subject (fItemList.GetRef(i), UseDefaultIID());
			subject->Change(kXPGSetArticleDataCmdBoss, IID_IARTICLEDATA, this);
		}		
		status = kSuccess;
	} while(false);
	if(status != kSuccess){
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}


/* CreateName
*/
PMString * XPGSetArticleDataCmd::CreateName(){
	PMString * name = new PMString("Set Article Data");
	name->SetTranslatable(kFalse);
	return name;
}
