

#include "VCPlugInHeaders.h"

// Interface includes
#include "ISubject.h"
#include "ISession.h"
#include "IWorkspace.h"
#include "IPageList.h"
#include "IIntData.h"
#include "CAlert.h"

// General includes
#include "ErrorUtils.h"
#include "PreferenceUtils.h"
#include "Command.h"
#include "K2Vector.h"

// XRailSetDocDataCmd includes
#include "IXRailPageSlugData.h"
#include "XRLID.h"

/**	XRailSetDocDataCmd
*/
class XRailSetDocDataCmd : public Command
{
	public:
		/**
			Constructor.
		*/
		XRailSetDocDataCmd(IPMUnknown* boss);

	protected:
		void Do();
		void Undo();
		void Redo();
		void DoNotify();
		PMString* CreateName();

	private:
		
		int32 pageID;
#if MULTIBASES == 1
    PMString baseName;
#endif
	
};


/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its 
 ImplementationID making the C++ code callable by the 
 application.
*/
CREATE_PMINTERFACE(XRailSetDocDataCmd, kXRLSetPageSlugDataCmdImpl)


/* Constructor.
*/
XRailSetDocDataCmd::XRailSetDocDataCmd(IPMUnknown* boss) :
	Command(boss)
{
}

/* Do
*/
void XRailSetDocDataCmd::Do()
{
	ErrorCode status = kFailure;
	do
	{
		if(fItemList.Length() != 1)// Should have only one kDocBoss in the item list
			break;

		// Get the command datas
		InterfacePtr<IXRailPageSlugData> newdata (this, UseDefaultIID());
		InterfacePtr<IIntData> pageIndex (this, UseDefaultIID());

		InterfacePtr<IPageList> pageList (fItemList.GetRef(0), UseDefaultIID());
		
		UID pageUID = pageList->GetNthPageUID(pageIndex->Get());
		if(pageUID == kInvalidUID) // Wrong page index
			break;
	
		InterfacePtr<IXRailPageSlugData> xrailslugdata (fItemList.GetDataBase(), pageUID, UseDefaultIID());
		pageID = xrailslugdata->GetID(); // Save for Undo
#if MULTIBASES == 1
		baseName = xrailslugdata->GetBaseName(); // Save for Undo
#endif
		xrailslugdata->SetID(newdata->GetID());
#if MULTIBASES == 1
		xrailslugdata->SetBaseName(newdata->GetBaseName());
#endif
		status = kSuccess;
		   		
	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);
}


/* Undo
*/
void XRailSetDocDataCmd::Undo()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)// Should have only one kDocBoss in the item list
			break;
	
		InterfacePtr<IIntData> pageIndex (this, UseDefaultIID());

		InterfacePtr<IPageList> pageList (fItemList.GetRef(0), UseDefaultIID());
		
		UID pageUID = pageList->GetNthPageUID(pageIndex->Get());
		if(pageUID == kInvalidUID) // Wrong page index
			break;

		InterfacePtr<IXRailPageSlugData> xrailslugdata (fItemList.GetDataBase(), pageUID, UseDefaultIID());
		xrailslugdata->SetID(pageID);
#if MULTIBASES == 1
		xrailslugdata->SetBaseName(baseName);
#endif	
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);
}


/* Redo
*/
void XRailSetDocDataCmd::Redo()
{
	Do();
}


/* DoNotify
*/
void XRailSetDocDataCmd::DoNotify()
{
	ErrorCode status = kFailure;

	do
	{
		if (fItemList.Length() != 1)
			break;
		else
		{
			InterfacePtr<ISubject> subject (fItemList.GetRef(0),UseDefaultIID());
			
			// Notify the subject of the change
			subject->Change(kXRLSetPageSlugDataCmdBoss, IID_PAGESLUGDATA, this);

			status = kSuccess;
		}
	} while(false);
}


/* CreateName
*/
PMString* XRailSetDocDataCmd::CreateName()
{
	PMString* str = new PMString(kCommandXRailSetPageSlugCmdKey);
	return str;
}


