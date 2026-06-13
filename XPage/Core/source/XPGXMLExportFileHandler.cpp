#include "VCPlugInHeaders.h"

// Interface includes:
#include "ISAXAttributes.h"
#include "ISession.h"
#include "ISAXServices.h"
#include "IXMLServicesError.h"
#include "IDocument.h"
#include "IXPGPreferences.h"
#include "IDataStringUtils.h"

// General includes:
#include "CSAXContentHandler.h"
#include "CPMUnknown.h"
#include "CAlert.h"
#include "TextID.h"
#include "Utils.h"

// Project includes:
#include "XPGID.h"

class XPGXMLExportFileHandler : public CSAXContentHandler 
{ 
public: 
	
	/** Constructor
	*/	
	XPGXMLExportFileHandler(IPMUnknown *boss); 
	
	/** Destructor
	*/
	virtual ~XPGXMLExportFileHandler(); 

	virtual void Register(ISAXServices* saxServices, IPMUnknown *importer);

	virtual bool16 HandlesSubElements() const;

	virtual void StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs);

	virtual void EndElement(const WideString& uri, const WideString& localname, const WideString& qname);

private:

	// Tag string
	static const WideString topLevelElement;
	static const WideString tagElement;

	PMString currentElement;

	TagList list;
	Tag defaultTag;

}; 

CREATE_PMINTERFACE(XPGXMLExportFileHandler, kXPGXMLExportFileHandlerImpl) 

const WideString XPGXMLExportFileHandler::topLevelElement = WideString("EXPORT_XML");
const WideString XPGXMLExportFileHandler::tagElement = WideString("TAG");

XPGXMLExportFileHandler::XPGXMLExportFileHandler(IPMUnknown *boss) : 
CSAXContentHandler(boss), currentElement(kNullString), defaultTag()
{ 
	list.clear();
} 

XPGXMLExportFileHandler::~XPGXMLExportFileHandler() 
{ 
} 

void XPGXMLExportFileHandler::Register(ISAXServices* saxServices, IPMUnknown *importer)
{
	do {
		ASSERT(saxServices);
		if(!saxServices) {
			break;
		}

		saxServices->RegisterElementHandler(WideString(""), WideString(topLevelElement), this);		

	} while(kFalse);
}

bool16 XPGXMLExportFileHandler::HandlesSubElements( ) const
{
	// Yes, we have registered for the top-level element and
	// we will handle all its dependents
	return kTrue;
}

void XPGXMLExportFileHandler::StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs) 
{ 	
	if(localname.compare(tagElement) == 0) // Found tag element
	{
		Tag aTag;

		WideString order, isDefault, style;
		attrs->GetValue(WideString("style"), style);
		attrs->GetValue(WideString("value"), aTag.value);
		attrs->GetValue(WideString("order"), order);
		attrs->GetValue(WideString("default"), isDefault);
		
		aTag.paraStyleName = PMString(style);
		aTag.order = Utils<IDataStringUtils>()->StringToInt32(order);
		list.push_back(aTag);

		// Handle default tag
		if(Utils<IDataStringUtils>()->StringToBool16(isDefault))
			defaultTag = aTag;
	}
} 

void XPGXMLExportFileHandler::EndElement(const WideString& uri, 
										  const WideString& localname, 
										  const WideString& qname )
{
	if(localname.compare(topLevelElement) == 0) // Found tag element
	{
		InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
		xpgPrefs->SetListeTagExport(list, defaultTag);
	}
}