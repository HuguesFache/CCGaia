#include "VCPlugInHeaders.h"

// Interface includes:
#include "ISAXAttributes.h"
#include "ISession.h"
#include "ISAXServices.h"
#include "IXMLServicesError.h"
#include "IXPGPreferences.h"
#include "IKeyValueData.h"


// General includes:
#include "CSAXContentHandler.h"
#include "CPMUnknown.h"
#include "CAlert.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"

class XPGXMLMatchingFileHandler2 : public CSAXContentHandler 
{ 
public: 
	
	XPGXMLMatchingFileHandler2(IPMUnknown *boss); 
	
	virtual ~XPGXMLMatchingFileHandler2(); 
	
	virtual void Register(ISAXServices* saxServices, IPMUnknown *importer);
	virtual bool16 HandlesSubElements() const;

	virtual void Characters(const WideString& chars);
	virtual void StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs);
	virtual void EndElement(const WideString& uri, const WideString& localname, const WideString& qname);

private:
	
	// Tag string
	static const WideString topLevelElement;
	WideString currentTextTag, charStyle;
	K2Vector<IDValuePair> stylesTags;
	int32 depth;
}; 

CREATE_PMINTERFACE(XPGXMLMatchingFileHandler2, kXPGXMLMatchingFileHandler2Impl) 
const WideString XPGXMLMatchingFileHandler2::topLevelElement = WideString("MATCHING");

XPGXMLMatchingFileHandler2::XPGXMLMatchingFileHandler2(IPMUnknown *boss):CSAXContentHandler(boss){ 

	depth = 0;
	currentTextTag = WideString();
	stylesTags.clear();
	
} 

XPGXMLMatchingFileHandler2::~XPGXMLMatchingFileHandler2(){} 
void XPGXMLMatchingFileHandler2::Register(ISAXServices* saxServices, IPMUnknown *importer){
	do{
		if(!saxServices) 
			break;
		saxServices->RegisterElementHandler(WideString(""), WideString(topLevelElement), this);

	}while(kFalse);
}

bool16 XPGXMLMatchingFileHandler2::HandlesSubElements( ) const{
	return kTrue;
}

void XPGXMLMatchingFileHandler2::StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs) 
{ 	
	++depth;
	if(depth == 2){ // Paragraph mappings depth in xml file	
		WideString paraStyle;
		attrs->GetValue(WideString("style"), paraStyle);
		currentTextTag = localname;	
		stylesTags.push_back(KeyValuePair<PMString, PMString>(currentTextTag, paraStyle));
	}
} 

void XPGXMLMatchingFileHandler2::EndElement(const WideString& uri, 
										  const WideString& localname, 
										  const WideString& qname )
{
	charStyle = WideString("");
	InterfacePtr<IKeyValueData> formeMatchingStyleData (this, IID_IFORMEMATCHINGSTYLESLIST);
	formeMatchingStyleData->SetKeyValueList(stylesTags);	
	--depth;
}

void XPGXMLMatchingFileHandler2::Characters(const WideString & chars) 
{ 
	if(depth == 3)
		charStyle += chars;
} 