#include "VCPlugInHeaders.h"

// Interface includes:
#include "ISAXAttributes.h"
#include "ISession.h"
#include "ISAXServices.h"
#include "IXMLServicesError.h"
#include "IXPGPreferences.h"
#include "IDataStringUtils.h"
#include "Utils.h"

// General includes:
#include "CSAXContentHandler.h"
#include "CPMUnknown.h"
#include "CAlert.h"

// Project includes:
#include "XPGID.h"

class XPGXMLMatchingStylesListHandler : public CSAXContentHandler 
{ 
public: 
	
	XPGXMLMatchingStylesListHandler(IPMUnknown *boss); 
	
	virtual ~XPGXMLMatchingStylesListHandler(); 
	virtual void Register(ISAXServices* saxServices, IPMUnknown *importer);
	virtual bool16 HandlesSubElements() const;
	virtual void Characters(const WideString& chars);
	virtual void StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs);
	virtual void EndElement(const WideString& uri, const WideString& localname, const WideString& qname);

private:
	// Tag/attribute string
	static const WideString topLevelElement;
	static const WideString nameAttrValue;
	static const WideString nameAttrOrder;
	PMString tagValue;	
	
	// Current tag
	WideString currentElt ;
	K2Vector<Tag> dataList;

}; 

CREATE_PMINTERFACE(XPGXMLMatchingStylesListHandler, kXPGXMLMatchingStylesListHandlerImpl) 

const WideString XPGXMLMatchingStylesListHandler::topLevelElement = WideString("TAG");
const WideString XPGXMLMatchingStylesListHandler::nameAttrValue = WideString("value");
const WideString XPGXMLMatchingStylesListHandler::nameAttrOrder = WideString("order");



XPGXMLMatchingStylesListHandler::XPGXMLMatchingStylesListHandler(IPMUnknown *boss) : 
CSAXContentHandler(boss){ 

	tagValue = kNullString;
	currentElt = WideString();
	dataList.clear();
} 

XPGXMLMatchingStylesListHandler::~XPGXMLMatchingStylesListHandler(){} 

void XPGXMLMatchingStylesListHandler::Register(ISAXServices* saxServices, IPMUnknown *importer)
{
	do {
		if(!saxServices) 
			break;
		saxServices->RegisterElementHandler(WideString(""), WideString(topLevelElement), this);

	} while(kFalse);
}

bool16 XPGXMLMatchingStylesListHandler::HandlesSubElements( ) const
{
	return kTrue;
}

void XPGXMLMatchingStylesListHandler::StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs) 
{ 
	currentElt = localname;	
	if(localname.compare(topLevelElement) == 0){

		Tag aTag;
		WideString tagOrder, Value;

		if(HasAttribute(attrs, nameAttrValue))	
		{
			Value = GetAttributeString(attrs, nameAttrValue);		
		}
		if(HasAttribute(attrs, nameAttrOrder))	
		{
			tagOrder = GetAttributeString(attrs, nameAttrOrder);		
		}

		aTag.value = Value;
		aTag.order = Utils<IDataStringUtils>()->StringToInt32(tagOrder);
		
		dataList.push_back(aTag);
		
	}
} 

void XPGXMLMatchingStylesListHandler::EndElement(const WideString& uri, const WideString& localname, const WideString& qname)
{
	if(localname.compare(topLevelElement) == 0){	
		// Store data that have been read		
		InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
		xpgPrefs->SetListeMatchingTag(dataList);
	}	
	currentElt.Clear();
}

void XPGXMLMatchingStylesListHandler::Characters(const WideString & chars) 
{
	tagValue += chars;
} 

