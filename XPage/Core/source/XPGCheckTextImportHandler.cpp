#include "VCPlugInHeaders.h"

// Interface includes:
#include "ISAXAttributes.h"

#include "ISAXServices.h"
#include "IXMLServicesError.h"


#include "ISysFileData.h"
#include "IXMLTagList.h"


#include "IXMLParsingContext.h"
#include "IUIDData.h"
#include "IStyleGroupHierarchy.h"
#include "IStyleInfo.h"
#include "IStringData.h"

// General includes:
#include "CSAXContentHandler.h"
#include "CPMUnknown.h"
#include "TextID.h"
#include "IUIDListData.h"
#include "K2Stack.h"
#include "ITextModelCmds.h"
#include "CAlert.h"
#include "IXMLUtils.h"
#include "IKeyValueData.h"

// Project includes:
#include "K2Vector.h"
#include "K2Vector.tpp"
#include "KeyValuePair.h"
#include "XMLDefs.h"
#include "XPGID.h"

typedef KeyValuePair<PMString, PMString> IDValuePair;

class XPGCheckTextImportHandler : public CSAXContentHandler
{
public:
    
    XPGCheckTextImportHandler(IPMUnknown *boss);
    virtual ~XPGCheckTextImportHandler();
    
    virtual void Register(ISAXServices* saxServices, IPMUnknown *importer);
    virtual bool16 HandlesSubElements() const;
    virtual void StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs);
    virtual void Characters(const WideString& chars);
    virtual void EndElement(const WideString& uri, const WideString& localname, const WideString& qname);
    virtual void EndDocument() ;
    
private:
    
    // Tag string
    static const WideString txtElement, topLevelElement;
    PMString currentParaStyleTag, currentParaText;
    bool16 inParagraph;
    int32 depth;
    
    // Current tag
    WideString currentElt;
    
    // Flag indicating if we are parsing text content
    bool16 isTextContent;
    
    K2Vector<IDValuePair> dataList;
};

const WideString XPGCheckTextImportHandler::topLevelElement = WideString("Objet");
const WideString XPGCheckTextImportHandler::txtElement = WideString("Contenu");

CREATE_PMINTERFACE(XPGCheckTextImportHandler, kXPGCheckTextImportHandlerImpl)

XPGCheckTextImportHandler::XPGCheckTextImportHandler(IPMUnknown *boss):CSAXContentHandler(boss),
                                                                    currentParaStyleTag(kNullString),
                                                                    depth(0),
                                                                    currentParaText(kNullString),
                                                                    inParagraph(kFalse),
																	   isTextContent(kFalse)
{
    dataList.clear();
}

XPGCheckTextImportHandler::~XPGCheckTextImportHandler() {}

void XPGCheckTextImportHandler::Register(ISAXServices* saxServices, IPMUnknown *importer){
    
    do{
        if(!saxServices)
            break;
        
        saxServices->RegisterElementHandler(WideString(""), topLevelElement, this);
        
    } while(kFalse);
}

bool16 XPGCheckTextImportHandler::HandlesSubElements( ) const {
    return kTrue;
}

void XPGCheckTextImportHandler::StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs){
    
    
    if(localname.compare(txtElement) == 0)
    {
        isTextContent = kTrue;
    }
    
    if(isTextContent){
        
        ++depth;
        if(depth == 2){ // Paragraph style tag level <TI>
            currentParaStyleTag = localname;
        }
        
        else if(depth == 3){  // Paragraph tag level <P>
            inParagraph = kTrue;
        }
    }
}


void XPGCheckTextImportHandler::Characters(const WideString & chars){
    if(isTextContent && inParagraph){
        // Insert current text
        currentParaText.Append(PMString(chars));
    }
}

void XPGCheckTextImportHandler::EndElement(const WideString& uri, const WideString& localname, const WideString& qname){
    
    
    if(localname.compare(txtElement) == 0)
    {
        isTextContent = kFalse;
        
    }
    if(isTextContent){
        if(	depth == 2){ // Paragraph tag level
            dataList.push_back(KeyValuePair<PMString, PMString>(currentParaText, currentParaStyleTag));
            currentParaText.clear();
        }
        else if(depth == 3){
            // Insert carriage return
            currentParaText.Append(kTextChar_CR);
        }
        --depth;
    }
}

void  XPGCheckTextImportHandler::EndDocument () 
{
    InterfacePtr<IKeyValueData> keyValueData(this, IID_ITEXTEMATCHINGSTYLESLIST);		
    keyValueData->SetKeyValueList(dataList);
}
