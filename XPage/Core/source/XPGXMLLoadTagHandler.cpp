#include "VCPlugInHeaders.h"

// Interface includes:
#include "ISAXAttributes.h"
#include "ISession.h"
#include "ISAXServices.h"
#include "IXMLServicesError.h"
#include "IXMLUtils.h"
#include "IDocument.h"
#include "IXMLTagList.h"
#include "IUIDData.h"

// General includes:
#include "CSAXContentHandler.h"
#include "CPMUnknown.h"
#include "CAlert.h"
#include "TextID.h"

// Project includes:
#include "XPGID.h"
#include "XMLDefs.h"

class XPGXMLLoadTagHandler : public CSAXContentHandler 
{ 
public: 
	
	/** Constructor
	*/	
	XPGXMLLoadTagHandler(IPMUnknown *boss); 
	
	/** Destructor
	*/
	virtual ~XPGXMLLoadTagHandler(); 

	virtual void Register(ISAXServices* saxServices, IPMUnknown *importer);
	virtual bool16 HandlesSubElements() const;
	virtual void StartElement(const WideString & uri, const WideString & localname, const WideString & qname, ISAXAttributes* attrs);

private:

	static const PMString txtElement;
	InterfacePtr<IXMLTagList> tagList;
	IDataBase * db;

}; 

CREATE_PMINTERFACE(XPGXMLLoadTagHandler, kXPGXMLLoadTagHandlerImpl) 

const PMString XPGXMLLoadTagHandler::txtElement = txtTag;

XPGXMLLoadTagHandler::XPGXMLLoadTagHandler(IPMUnknown *boss) : 
CSAXContentHandler(boss), tagList(nil), db(nil)
{ 
	
} 

XPGXMLLoadTagHandler::~XPGXMLLoadTagHandler() 
{ 
} 

void XPGXMLLoadTagHandler::Register(ISAXServices* saxServices, IPMUnknown *importer)
{
	do {
		ASSERT(saxServices);
		if(!saxServices) {
			break;
		}

		saxServices->RegisterElementHandler(WideString(""), WideString(txtElement), this);		
		
		/* Get document to load tag into
		*/
		InterfacePtr<IUIDData> docData (this, UseDefaultIID());
		UIDRef docRef = docData->GetRef();
		if(docRef != UIDRef::gNull)
		{
			db = docRef.GetDataBase();
			InterfacePtr<IDocument> doc (docRef, UseDefaultIID());
			tagList = InterfacePtr<IXMLTagList> (doc->GetDocWorkSpace(), UseDefaultIID());
		}

	} while(kFalse);
}

bool16 XPGXMLLoadTagHandler::HandlesSubElements( ) const
{
	// Yes, we have registered for the top-level element and
	// we will handle all its dependents
	return kTrue;
}

void XPGXMLLoadTagHandler::StartElement(const WideString & uri, const WideString & localname, const WideString & qname, ISAXAttributes* attrs) 
{ 	
	// Has the tag already been added ?
	UID theTag = tagList->GetTag(localname);
	if(theTag == kInvalidUID)
	{
		UID newTag = kInvalidUID;
		Utils<IXMLUtils>()->AssignTagUID(localname, db, kInvalidUID, newTag);
	}
} 
