
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IStringListData.h"

// General includes:
#include "CmdUtils.h"
#include "Command.h"
#include "ErrorUtils.h"

#include "XPGID.h"

/** Implementation of command to set article id data in kTextStoryBoss
*/
class XPGSetArticleIDCmd : public Command
{
public:
	/** Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.*/
	XPGSetArticleIDCmd(IPMUnknown* boss);

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

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGSetArticleIDCmd, kXPGSetArticleIDCmdImpl)

/* Constructor
*/
XPGSetArticleIDCmd::XPGSetArticleIDCmd(IPMUnknown* boss) : Command(boss)
{
	
}

/* Do
*/
void XPGSetArticleIDCmd::Do()
{
	ErrorCode status = kFailure;
	do                      
	{
		if(fItemList.Length() == 0)
			break;
		
		for(int32 i=0;i<fItemList.Length();++i)
		{
			InterfacePtr<IStringListData> articleInfoData (fItemList.GetRef(i), IID_IARTICLEINFO);			
			if(!articleInfoData)
				break;

			InterfacePtr<IStringListData> cmdData (this, IID_IARTICLEINFO);				
			articleInfoData->SetStringList(cmdData->GetStringList());

		}
		status = kSuccess;

	} while (kFalse);       

	// Handle any errors
	if (status != kSuccess) {
		ErrorUtils::PMSetGlobalErrorCode(status);
	}
}

/* CreateName
*/
PMString* XPGSetArticleIDCmd::CreateName()
{
	return new PMString(kXPGSetArticleIDCmdKey);
}

// End, XPGSetArticleIDCmd.cpp.


