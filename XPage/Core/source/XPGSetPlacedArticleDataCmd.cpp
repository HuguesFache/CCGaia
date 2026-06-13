#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"

// Interface includes
#include "ISubject.h"
#include "IPlacedArticleData.h"

// Project includes
#include "XPGID.h"
#include "CAlert.h"

/** XPGSetPlacedArticleDataCmd
*/
class XPGSetPlacedArticleDataCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGSetPlacedArticleDataCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XPGSetPlacedArticleDataCmd();
	
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


CREATE_PMINTERFACE(XPGSetPlacedArticleDataCmd, kXPGSetPlacedArticleDataCmdImpl)

/* Constructor
*/
XPGSetPlacedArticleDataCmd::XPGSetPlacedArticleDataCmd(IPMUnknown* boss):Command(boss){
	SetUndoability(kAutoUndo);
}

/* Destructor
*/
XPGSetPlacedArticleDataCmd::~XPGSetPlacedArticleDataCmd(){}

/* Do
*/
void XPGSetPlacedArticleDataCmd::Do(){

	ErrorCode status = kFailure;
	do{
		int32 length = fItemList.Length();
		if(length == 0)
			break;

		// Get Article data to set
		InterfacePtr<IPlacedArticleData> cmdData (this, IID_IPLACEDARTICLEDATA);	
		status = kSuccess;
		for(int32 i = 0 ; i < length ; ++i){

			InterfacePtr<IPlacedArticleData> placedArticleData (fItemList.GetRef(i), IID_IPLACEDARTICLEDATA);
			if(placedArticleData == nil){
				status = kFailure;
				break;
			}
				
			// apply string fields
			placedArticleData->SetUniqueId(cmdData->GetUniqueId());
			placedArticleData->SetStoryFolder(cmdData->GetStoryFolder());
			placedArticleData->SetStoryTopic(cmdData->GetStoryTopic());
		}
	
	} while(false);

	if(status != kSuccess){
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void XPGSetPlacedArticleDataCmd::DoNotify(){

	ErrorCode status = kFailure;			
	do{
		int32 length = fItemList.Length();
		if(length == 0)
			break;

		for(int32 i = 0 ; i < length ; ++i){
			// Notify the subject of the change
			InterfacePtr<ISubject> subject (fItemList.GetRef(i), UseDefaultIID());
			subject->Change(kXPGSetPlacedArticleDataCmdBoss, IID_IPLACEDARTICLEDATA, this);
		}
		
		status = kSuccess;
	} while(false);

	if(status != kSuccess){
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* CreateName
*/
PMString * XPGSetPlacedArticleDataCmd::CreateName()
{
	PMString * name = new PMString("Set Placed Article Data");
	name->SetTranslatable(kFalse);
	return name;
}
