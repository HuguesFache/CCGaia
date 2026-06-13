#include "VCPlugInHeaders.h"
// Interface includes:
#include "ISAXAttributes.h"
#include "ISession.h"
#include "ISAXServices.h"
#include "IXMLServicesError.h"
#include "IStringListData.h"
// General includes:
#include "CSAXContentHandler.h"
#include "CPMUnknown.h"
#include "CAlert.h"
#include "XPGFormeDataNode.h"
// Project includes:
#include "XPGID.h"

class XPGXMLArticleDescription : public CSAXContentHandler 
{ 
public: 	
	/** Constructor
	*/	
	XPGXMLArticleDescription(IPMUnknown *boss); 
	
	/** Destructor
	*/
	virtual ~XPGXMLArticleDescription(); 

	virtual void Register(ISAXServices* saxServices, IPMUnknown *importer);
	virtual bool16 HandlesSubElements() const;
	virtual void Characters(const WideString& chars);
	virtual void StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs);
	virtual void EndElement(const WideString& uri, const WideString& localname, const WideString& qname);

private:

	// Tag/attribute string
	static const WideString topLevelElement;
	static const WideString contenuAttr;	
	static const WideString idArticleAttr;	

	// Current tag
	WideString currentElt;

	// Content variables
	PMString  contenuArticle, idArticle;
}; 

CREATE_PMINTERFACE(XPGXMLArticleDescription, kXPGXMLArticleDescriptionImpl)

const WideString XPGXMLArticleDescription::topLevelElement = WideString("Objet");
const WideString XPGXMLArticleDescription::idArticleAttr = WideString("id");
const WideString XPGXMLArticleDescription::contenuAttr = WideString("Contenu");

XPGXMLArticleDescription::XPGXMLArticleDescription(IPMUnknown *boss) : CSAXContentHandler(boss){

	 contenuArticle = kNullString;
	 currentElt = WideString();
} 

XPGXMLArticleDescription::~XPGXMLArticleDescription() {} 

void XPGXMLArticleDescription::Register(ISAXServices* saxServices, IPMUnknown *importer){
	do {
		if(!saxServices) 
			break;	

		saxServices->RegisterElementHandler(WideString(""), WideString(topLevelElement), this);
	} while(kFalse);
}

bool16 XPGXMLArticleDescription::HandlesSubElements( ) const{
	return kTrue;
}

void XPGXMLArticleDescription::StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs) 
{ 
	currentElt = localname;
	if(localname.compare(topLevelElement) == 0)	{			
		if(HasAttribute(attrs, idArticleAttr))	{
			idArticle = GetAttributeString(attrs, idArticleAttr);			
		}
	}
	else if(localname.compare(contenuAttr) == 0)	{			
		contenuArticle = GetAttributeString(attrs, contenuAttr);			
	}
	
} 

void XPGXMLArticleDescription::EndElement(const WideString& uri, 
									      const WideString& localname, 
									      const WideString& qname ){
	
	if(localname.compare(topLevelElement) == 0){

		K2Vector<PMString> dataList;
		dataList.push_back(idArticle);
		dataList.push_back(contenuArticle);
		InterfacePtr<IStringListData> returnData (this, UseDefaultIID());
		returnData->SetStringList(dataList);
	}
	currentElt.Clear();
}

void XPGXMLArticleDescription::Characters(const WideString & chars) {	
	if(currentElt.compare(contenuAttr) == 0)
			contenuArticle += chars;

} 

