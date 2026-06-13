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

class XPGXMLFormeDescriptionHandler : public CSAXContentHandler 
{ 
public: 
	
	/** Constructor
	*/	
	XPGXMLFormeDescriptionHandler(IPMUnknown *boss); 
	
	/** Destructor
	*/
	virtual ~XPGXMLFormeDescriptionHandler(); 
	virtual void Register(ISAXServices* saxServices, IPMUnknown *importer);
	virtual bool16 HandlesSubElements() const;
	virtual void Characters(const WideString& chars);
	virtual void StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs);
	virtual void EndElement(const WideString& uri, const WideString& localname, const WideString& qname);

private:

	// Tag/attribute string
	static const WideString topLevelElement;
	static const WideString formeFileElement;
	static const WideString typoFileElement;
	static const WideString posxAttr, posyAttr;
	static const WideString nameAttr, classeurNameAttr;
	static const WideString isPhotoCartonAttr;

	// Current tag
	WideString currentElt ;

	// Content variables
	PMString formeName, classeurName, formeFileName, typoFileName, posX, posY;
	PMString isPhotoCarton; // "0" or "1" — index 6 in the emitted StringList.
}; 

CREATE_PMINTERFACE(XPGXMLFormeDescriptionHandler, kXPGXMLFormeDescriptionHandlerImpl) 

const WideString XPGXMLFormeDescriptionHandler::topLevelElement = WideString("FORME");
const WideString XPGXMLFormeDescriptionHandler::formeFileElement = WideString("FICHIERFORME");
const WideString XPGXMLFormeDescriptionHandler::typoFileElement = WideString("FICHIERTYPO");
const WideString XPGXMLFormeDescriptionHandler::posxAttr = WideString("posx");
const WideString XPGXMLFormeDescriptionHandler::posyAttr = WideString("posy");
const WideString XPGXMLFormeDescriptionHandler::nameAttr = WideString("name");
const WideString XPGXMLFormeDescriptionHandler::classeurNameAttr = WideString("classeur");
const WideString XPGXMLFormeDescriptionHandler::isPhotoCartonAttr = WideString("isPhotoCarton");


XPGXMLFormeDescriptionHandler::XPGXMLFormeDescriptionHandler(IPMUnknown *boss) :
CSAXContentHandler(boss)
{
	formeName = classeurName = formeFileName = typoFileName = kNullString;
	isPhotoCarton = "0"; // default for legacy descriptifs without the attribute
	currentElt = WideString();
}

XPGXMLFormeDescriptionHandler::~XPGXMLFormeDescriptionHandler() 
{ 
} 

void XPGXMLFormeDescriptionHandler::Register(ISAXServices* saxServices, IPMUnknown *importer)
{
	do {
		ASSERT(saxServices);
		if(!saxServices) {
			break;
		}

		saxServices->RegisterElementHandler(WideString(""), WideString(topLevelElement), this);

	} while(kFalse);
}

bool16 XPGXMLFormeDescriptionHandler::HandlesSubElements( ) const
{
	// Yes, we have registered for the ktop-level element and
	// we will handle all its dependents
	return kTrue;
}

void XPGXMLFormeDescriptionHandler::StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs) 
{ 

	currentElt = localname;
	
	if(localname.compare(topLevelElement) == 0)
	{
	
		if(HasAttribute(attrs, nameAttr))
		{
			formeName = GetAttributeString(attrs, nameAttr);
			
		}

		if(HasAttribute(attrs, classeurNameAttr))
		{
			classeurName = GetAttributeString(attrs, classeurNameAttr);
		}
		
		if(HasAttribute(attrs, posxAttr))
		{
			posX = GetAttributeString(attrs, posxAttr);
		}
		
		if(HasAttribute(attrs, posyAttr))
		{
			posY = GetAttributeString(attrs, posyAttr);
		}

		if(HasAttribute(attrs, isPhotoCartonAttr))
		{
			isPhotoCarton = GetAttributeString(attrs, isPhotoCartonAttr);
		}
	}
}

void XPGXMLFormeDescriptionHandler::EndElement(const WideString& uri, 
									        	  const WideString& localname, 
									        	  const WideString& qname )
{
	if(localname.compare(topLevelElement) == 0)
	{	
		// Store data that have been read
	
		
		K2Vector<PMString> dataList;
		dataList.push_back(formeName);
		dataList.push_back(classeurName);
		dataList.push_back(formeFileName);
		dataList.push_back(typoFileName);
		dataList.push_back(posX);
		dataList.push_back(posY);
		// Index 6: "1" for photo cartons, "0" otherwise. Always present.
		dataList.push_back(isPhotoCarton);
		InterfacePtr<IStringListData> returnData (this, UseDefaultIID());
		returnData->SetStringList(dataList);

	}
	
	currentElt.Clear();
}

void XPGXMLFormeDescriptionHandler::Characters(const WideString & chars) 
{ 
	if(currentElt.compare(formeFileElement) == 0)
		formeFileName += chars;
	else if(currentElt.compare(typoFileElement) == 0)
		typoFileName += chars;
} 

