
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IDataExchangeHandler.h"
#include "IDocument.h"
#include "IPMDataObject.h"
#include "ISysFileListData.h"
#include "IStringData.h"

// General includes:
#include "PMPoint.h"
#include "UIDList.h"
#include "CActionComponent.h"
#include "CAlert.h"
#include "CmdUtils.h"
#include "CDataExchangeHandlerFor.h"
#include "IStringListData.h"

// Project includes:
#include "XPGUIID.h"


/** XPGUIXmlWithFormeFileHandler
	is responsible for keeping track of the data during an exchange operation such as drag'n'drop.

	XPGUIXmlWithFormeFileHandler implements based on the partial implementation of CDataExchangeHandlerFor.
*/
class XPGUIXmlWithFormeFileHandler : public CDataExchangeHandlerFor
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		XPGUIXmlWithFormeFileHandler(IPMUnknown* boss) : CDataExchangeHandlerFor(boss) {}
		/**
			Destructor.
		*/
		virtual	~XPGUIXmlWithFormeFileHandler() {}

		/**
			indicates the type of object this data exchange handler can deal with.
			@return the flavor supported by the handler
		*/
		virtual	PMFlavor GetFlavor() const { return XPageXMLWithFormeFlavor; }

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
CREATE_PMINTERFACE(XPGUIXmlWithFormeFileHandler, kXPGUIXmlWithFormeFileHandlerImpl)


void XPGUIXmlWithFormeFileHandler::Clear()
{
	InterfacePtr<ISysFileListData> fileData (this, IID_ISYSFILELISTDATA);
	fileData->Clear();

	InterfacePtr<IStringListData> idListData (this, IID_ISTRINGLISTDATA);
	K2Vector<PMString> emptyList;
	idListData->SetStringList(emptyList);

	InterfacePtr<IStringData> creditData(this, IID_ICREDITPHOTO);
	creditData->Set(kNullString);

	InterfacePtr<IStringData> legendData(this, IID_ILEGENDPHOTO);
	legendData->Set(kNullString);
}

bool16 XPGUIXmlWithFormeFileHandler::IsEmpty() const{
	InterfacePtr<ISysFileListData> fileData (this, IID_ISYSFILELISTDATA);	
	return (fileData->Length() > 0);
}

// End, XPGUIXmlWithFormeFileHandler.cpp.