
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IDataExchangeHandler.h"
#include "ICommand.h"
#include "IDocument.h"
#include "ISysFileListData.h"

// General includes:
#include "PMPoint.h"
#include "CActionComponent.h"
#include "CAlert.h"
#include "CmdUtils.h"
#include "CDataExchangeHandlerFor.h"

// Project includes:
#include "XPGUIID.h"


/** XPGUIFormeFileHandler
	is responsible for keeping track of the data during an exchange operation such as drag'n'drop.

	XPGUIFormeFileHandler implements based on the partial implementation of CDataExchangeHandlerFor.
*/
class XPGUIFormeFileHandler : public CDataExchangeHandlerFor
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		XPGUIFormeFileHandler(IPMUnknown* boss) : CDataExchangeHandlerFor(boss) {}
		/**
			Destructor.
		*/
		virtual	~XPGUIFormeFileHandler() {}

		/**
			indicates the type of object this data exchange handler can deal with.
			@return the flavor supported by the handler
		*/
		virtual	PMFlavor GetFlavor() const { return XPageFormeFlavor; }

		/**
			clears the file.
		*/
		virtual void				Clear();
		/**
			reports whether there is a file or not.
			@return kTrue if there is a file , kFalse otherwise.
		*/
		virtual bool16			IsEmpty() const;

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUIFormeFileHandler, kXPGUIFormeFileHandlerImpl)


void XPGUIFormeFileHandler::Clear()
{
	InterfacePtr<ISysFileListData> fileData (this, IID_ISYSFILELISTDATA);
	
	fileData->Clear();
}

bool16 XPGUIFormeFileHandler::IsEmpty() const
{
	InterfacePtr<ISysFileListData> fileData (this, IID_ISYSFILELISTDATA);
	
	return (fileData->Length() > 0);
}

// End, XPGUIFormeFileHandler.cpp.