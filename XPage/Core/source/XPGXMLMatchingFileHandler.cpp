#include "VCPlugInHeaders.h"

// Interface includes:
#include "ISAXAttributes.h"
#include "ISession.h"
#include "ISAXServices.h"
#include "IXMLServicesError.h"
#include "IXMLMappingCommands.h"
#include "IDocument.h"
#include "ISysFileData.h"
#include "IXMLTagList.h"
#include "IStyleGroupManager.h"
#include "IXPGTagToStyleMap.h"
#include "IXMLParsingContext.h"
#include "IUIDData.h"
#include "IStyleGroupHierarchy.h"
#include "IStyleInfo.h"
#include "IXPageUtils.h"
#include "IStyleGroupHierarchy.h"

// General includes:
#include "CSAXContentHandler.h"
#include "CPMUnknown.h"
#include "CAlert.h"
#include "TextID.h"

// Project includes:
#include "XPGID.h"

class XPGXMLMatchingFileHandler : public CSAXContentHandler 
{ 
public: 
	
	/** Constructor
	*/	
	XPGXMLMatchingFileHandler(IPMUnknown *boss); 
	
	/** Destructor
	*/
	virtual ~XPGXMLMatchingFileHandler(); 

	virtual void Register(ISAXServices* saxServices, IPMUnknown *importer);
	virtual bool16 HandlesSubElements() const;
	virtual void Characters(const WideString& chars);
	virtual void StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs);
	virtual void EndElement(const WideString& uri, const WideString& localname, const WideString& qname);

private:

	// Tag string
	static const WideString topLevelElement;

	int32 depth;

	// Current text tag (depth == 2)
	WideString currentTextTag;

	// Content variable
	WideString charStyle;

	UIDRef docWS;

}; 

CREATE_PMINTERFACE(XPGXMLMatchingFileHandler, kXPGXMLMatchingFileHandlerImpl) 

const WideString XPGXMLMatchingFileHandler::topLevelElement = WideString("MATCHING");

XPGXMLMatchingFileHandler::XPGXMLMatchingFileHandler(IPMUnknown *boss):CSAXContentHandler(boss)
{ 
	depth = 0;
	currentTextTag = WideString();
	docWS = UIDRef::gNull;
} 

XPGXMLMatchingFileHandler::~XPGXMLMatchingFileHandler() 
{ 
} 

void XPGXMLMatchingFileHandler::Register(ISAXServices* saxServices, IPMUnknown *importer)
{
	do {
		if(!saxServices) 
			break;

		saxServices->RegisterElementHandler(WideString(""), topLevelElement, this);	
		InterfacePtr<IXPGTagToStyleMap> tag2styleData (this, UseDefaultIID());
		tag2styleData->ClearMap();
		
		// Get document to read registered tag from
		
		InterfacePtr<IUIDData> docData (this, UseDefaultIID());
		InterfacePtr<IDocument> doc (docData->GetRef(), UseDefaultIID());
		if(doc)
			docWS = doc->GetDocWorkSpace();
		

	} while(kFalse);
}

bool16 XPGXMLMatchingFileHandler::HandlesSubElements( ) const
{
	// Yes, we have registered for the top-level element and
	// we will handle all its dependents
	return kTrue;
}

void XPGXMLMatchingFileHandler::StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs) 
{ 	
	++depth;
	if(depth == 2){ // Paragraph mappings depth in xml file
		WideString paraStyle;
		attrs->GetValue(WideString("style"), paraStyle);		
		if(!paraStyle.IsNull()){
			
			// First find the tag
			InterfacePtr<IXMLTagList> tagList (docWS, UseDefaultIID());
			UID theTag = tagList->GetTag(localname);
			if(theTag != kInvalidUID){ // TRUE if the tag appears in the xml file to be imported (tag were loaded before the matching got parsed)
				// Then find the paragraph style
				InterfacePtr<IStyleGroupManager> paraStyleTable (docWS, IID_IPARASTYLEGROUPMANAGER);
				IStyleGroupHierarchy *paraStylesHierarchy = paraStyleTable->GetRootHierarchy();
				if (paraStylesHierarchy != nil)
				{
					UID paraStyleUID = Utils<IXPageUtils>()->TriasFindStyleByName(paraStylesHierarchy, paraStyle);
					if (paraStyleUID != kInvalidUID) {
						InterfacePtr<IXPGTagToStyleMap> tag2styleData (this, UseDefaultIID());
						tag2styleData->AddTagToParaStyleMapping(PMString(localname), paraStyleUID);
					}
				}
			}
		}

		// Set current text tag
		currentTextTag = localname;
	}
} 

void XPGXMLMatchingFileHandler::EndElement(const WideString& uri, 
										  const WideString& localname, 
										  const WideString& qname )
{
	--depth;
}

void XPGXMLMatchingFileHandler::Characters(const WideString & chars) 
{ 
	if(depth == 3) {
		charStyle += chars;
} 
} 
