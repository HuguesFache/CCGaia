#include "VCPlugInHeaders.h"

// interface includes
#include "IApplication.h"
#include "IIdleTaskMgr.h"
#include "CIdleTask.h"
#include "GlobalTime.h"
#include "IDialogMgr.h"
#include "RsrcSpec.h"
#include "LocaleSetting.h"
#include "CoreResTypes.h"
#include "CDialogController.h"
#include "CDialogObserver.h"
#include "ICommand.h"
#include "IBoolData.h"
#include "ICloseWinCmdData.h"
#include "IIntData.h"
#include "IWindow.h"
#include "ICloseWinCmdData.h"
#include "IPanelControlData.h"
#include "ISubject.h"

// Project includes
#include "XRLID.h"
#include "CAlert.h"

class XRLAutoClosingDialog : public CIdleTask {
    
    public:

        XRLAutoClosingDialog(IPMUnknown* boss);
	    virtual ~XRLAutoClosingDialog() ;
	    virtual uint32 RunTask(uint32 appFlags, IdleTimer* timeCheck);
	    virtual const char* TaskName();	     

	private: 
		int32 kXRLExecInterval;
};


CREATE_PMINTERFACE(XRLAutoClosingDialog, kXRLAutoClosingDialogImpl)

XRLAutoClosingDialog::XRLAutoClosingDialog(IPMUnknown *boss):CIdleTask(boss){}

XRLAutoClosingDialog::~XRLAutoClosingDialog(){}

uint32 XRLAutoClosingDialog::RunTask(uint32 appFlags, IdleTimer* timeCheck) 
{
	do{
		if(	appFlags & (IIdleTaskMgr::kMouseTracking | 
			            IIdleTaskMgr::kUserActive | 
						IIdleTaskMgr::kMenuUp))
			return kOnFlagChange;
		
		InterfacePtr<IApplication> application ( GetExecutionContextSession()->QueryApplication() );
		InterfacePtr<IDialogMgr> dialogMgr ( application, UseDefaultIID() );
		InterfacePtr<IWindow> window(dialogMgr->GetFrontmostDialogWindow(), UseDefaultIID());
		InterfacePtr<IPanelControlData> panelCtrlData(window,UseDefaultIID());
		IControlView *iCtrlView = panelCtrlData->FindWidget(kCancelButton_WidgetID);
		InterfacePtr<ISubject> iiSubject(iCtrlView, IID_ISUBJECT);
		if(iiSubject)
			iiSubject->Change(kTrueStateMessage,IID_IBOOLEANCONTROLDATA);

		
	}while(kFalse);
    return -1;
}

const char* XRLAutoClosingDialog::TaskName(){
	return kXRLAutoClosingDialogKey;
}
