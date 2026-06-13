
#include "VCPlugInHeaders.h"

//Interface includes:
#include "IDrwEvtDispatcher.h"
#include "IDrwEvtHandler.h"
#include "ISession.h" // for gSession

// General includes:
#include "CPrintSetupProvider.h"
#include "HelperInterface.h"

// Project includes:
#include "PrsID.h"

/**	Provides a mechanism to selectively enter into the print process.
*/
class PrsPrintSetupProvider : public CPrintSetupProvider
{
public:
	/** Constructor
	 */
	PrsPrintSetupProvider(IPMUnknown* boss);

	/** Destructor
	 */
	~PrsPrintSetupProvider();

	/**	Called after the print UI is shown.
		@param bReturn [out] specifies whether to continue. kFalse means quit.
		@param iPrintData (in) specifies IPrintData containing all print settings used.
	*/
	virtual void AfterPrintUI(bool16& bReturn, IPrintData* iPrintData);


	/**	Called to end printing.
	 */
	virtual void EndPrint(void);

	DECLARE_HELPER_METHODS()
	
	private:
	IDrwEvtHandler* fPSEH;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class to its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PrsPrintSetupProvider, kPrsPrintSetupProviderImpl)

DEFINE_HELPER_METHODS(PrsPrintSetupProvider)


/* Constructor
*/
PrsPrintSetupProvider::PrsPrintSetupProvider(IPMUnknown* boss)
:   HELPER_METHODS_INIT(boss)
{
	fPSEH = nil;
}


/* Destructor
*/
PrsPrintSetupProvider::~PrsPrintSetupProvider()
{
}


/* AfterPrintUI
*/
void PrsPrintSetupProvider::AfterPrintUI(bool16& bReturn, IPrintData* iPrintData)
{
	do
	{
		// check the flag first - if it says "don't return", then skip this whole method
		if (bReturn == kFalse)
		{
			break; // out of do-while
		}

		fPSEH = (IDrwEvtHandler*)::CreateObject(kPrsDrawServicesBoss, IID_IDRWEVTHANDLER);
		if (fPSEH != nil)
		{
			InterfacePtr<IDrwEvtDispatcher> drawEvtDisp(gSession, IID_IDRWEVTDISPATCHER);
			fPSEH->Register(drawEvtDisp);
		}
		// we are ok to continue
		bReturn = kTrue;
			
	} while (kFalse); 
}


/* EndPrint
*/
void PrsPrintSetupProvider::EndPrint(void)
{
	do
	{
		// If registered, unregister our draw event handler.
		if (fPSEH != nil)
		{
			InterfacePtr<IDrwEvtDispatcher> drawEvtDisp(gSession, IID_IDRWEVTDISPATCHER);
			fPSEH->UnRegister(drawEvtDisp);
			fPSEH->Release();
			fPSEH = nil;
		}
	} while (kFalse);
}