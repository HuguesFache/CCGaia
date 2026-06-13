#include "VCPlugInHeaders.h"

// Interface includes:
#include "ISAXAttributes.h"
#include "ISession.h"
#include "ISAXServices.h"
#include "IXMLServicesError.h"
#include "IPointerData.h"
#include "IDataStringUtils.h"

// General includes:
#include "CSAXContentHandler.h"
#include "CPMUnknown.h"
#include "CAlert.h"
#include "XPGArticleDataNode.h"
#include "FileUtils.h"
#include "Utils.h"
#include "XMLDefs.h"

// Project includes:
#include "XPGID.h"

class XPGXMLParserArticle : public CSAXContentHandler 
{ 
public: 
	
	/** Constructor
	*/	
	XPGXMLParserArticle(IPMUnknown *boss); 
	
	/** Destructor
	*/
	virtual ~XPGXMLParserArticle(); 

	virtual void Register(ISAXServices* saxServices, IPMUnknown *importer);

	virtual bool16 HandlesSubElements() const;

	virtual void Characters(const WideString & chars);

	virtual void StartElement(const WideString & uri, const WideString & localname, const WideString & qname, ISAXAttributes* attrs);

	virtual void EndElement(const WideString & uri, const WideString & localname, const WideString & qname);

private:

	// Directory separator
	static const PMString dirSep;
	
	// Tag/attribute string
	static const WideString topLevelElement;
	static const WideString stateElement;
	static const WideString childIdElement;
	static const WideString parentIdElement;
	static const WideString idAttr;
	static const WideString imageElement;
	static const WideString paragraphElement;	
	static const WideString nbCharsElement;	
	static const WideString typeElement;
	static const WideString folioElement;
	static const WideString dossierCartonElement;
	static const WideString fichierCartonElement;

	// Max characters allowed in text preview
	static const int32 maxChars;
	
	// Current tag
	WideString currentElt;

	// Flag indicating if we are parsing text content
	bool16 isTextContent;

	// Ids
	PMString articleId, childId, parentId;

	// Content variables
	PMString subject, state, preview, folio;
	// Carton par defaut declare dans <Proprietes> (peut rester vide)
	PMString dossierCarton, fichierCarton;
	int32 nbChars, artNbPhotos, artType;

	int32 nbCharsPreview;

	// Associadted images IDs
	K2Vector<PMString> imageIDs;
}; 

CREATE_PMINTERFACE(XPGXMLParserArticle, kXPGXMLParserArticleImpl) 

#ifdef WINDOWS
	const PMString XPGXMLParserArticle::dirSep = "\\";
#else
	const PMString XPGXMLParserArticle::dirSep = "/";
#endif
const WideString XPGXMLParserArticle::topLevelElement = WideString("Objet");
const WideString XPGXMLParserArticle::stateElement = WideString("Etat");
const WideString XPGXMLParserArticle::childIdElement = WideString("ObjetRedactionnel");
const WideString XPGXMLParserArticle::parentIdElement = WideString("ParentRedactionnel");
const WideString XPGXMLParserArticle::idAttr = WideString("id");
const WideString XPGXMLParserArticle::imageElement = WideString("Image");
const WideString XPGXMLParserArticle::paragraphElement = WideString("P");
const WideString XPGXMLParserArticle::nbCharsElement = WideString("NbSignes");
const WideString XPGXMLParserArticle::typeElement = WideString("TypeArticle");
const WideString XPGXMLParserArticle::folioElement = WideString("Folio");
const WideString XPGXMLParserArticle::dossierCartonElement = WideString("DossierCarton");
const WideString XPGXMLParserArticle::fichierCartonElement = WideString("FichierCarton");

const int32 XPGXMLParserArticle::maxChars = 100;

XPGXMLParserArticle::XPGXMLParserArticle(IPMUnknown *boss): 
CSAXContentHandler(boss)
{ 
	folio = subject = state = childId = articleId = parentId = kNullString;
	dossierCarton = fichierCarton = kNullString;
	preview = kNullString;
	nbChars = 0;
	artNbPhotos = 0;
	artType = kMEPJavaStory;
	currentElt = WideString();
	isTextContent = kFalse;
	nbCharsPreview = 0;
	imageIDs.clear();
} 

XPGXMLParserArticle::~XPGXMLParserArticle(){} 

void XPGXMLParserArticle::Register(ISAXServices* saxServices, IPMUnknown *importer)
{
	do {
		ASSERT(saxServices);
		if(!saxServices) {
			break;
		}

		saxServices->RegisterElementHandler(WideString(""), topLevelElement, this);

	} while(kFalse);
}

bool16 XPGXMLParserArticle::HandlesSubElements( ) const
{
	// Yes, we have registered for the ktop-level element and
	// we will handle all its dependents
	return kTrue;
}

void XPGXMLParserArticle::StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs) 
{ 
	currentElt = localname;
	
	if(localname.compare(topLevelElement) == 0)
	{
		// Get article id from attribute
		if(HasAttribute(attrs, idAttr))
		{
			articleId = GetAttributeString(attrs, idAttr);
		}
	}
	else if(localname.compare(childIdElement) == 0)
	{
		// Get child id from attribute
		if(HasAttribute(attrs, idAttr))
		{
			childId = GetAttributeString(attrs, idAttr);
		}
	}
	else if(localname.compare(parentIdElement) == 0)
	{
		// Get parent id from attribute
		if(HasAttribute(attrs, idAttr))
		{
			parentId = GetAttributeString(attrs, idAttr);
		}
	}
	else if(localname.compare(imageElement) == 0)
	{
		PMString id;
		if(HasAttribute(attrs, idAttr))
		{
			artNbPhotos ++;
			id = GetAttributeString(attrs, idAttr);
			imageIDs.push_back(id+".PHO.xml");
		}
	}
	else if(localname.compare(paragraphElement) == 0)
	{
		isTextContent = kTrue;
	}
} 

void XPGXMLParserArticle::EndElement(const WideString& uri, 
										  const WideString& localname, 
										  const WideString& qname )
{
	if(localname.compare(topLevelElement) == 0)
	{	
		InterfacePtr<IPointerData> dataNode (this, IID_INODEDATA);
		XPGArticleDataNode * theNode = (XPGArticleDataNode *) dataNode->Get();

		// Set node text data
		theNode->SetID(articleId);

		PMString contentPath = kNullString;
#if INCOPY
		IDFile articleFolder = FileUtils::PMStringToSysFile(theNode->GetData());
		FileUtils::GetParentDirectory(articleFolder, articleFolder);
		IDFile assignmentFile = articleFolder;
		FileUtils::AppendPath(&assignmentFile, articleId + PMString(kXPGExtensionInCopyKey, PMString::kTranslateDuringCall));
		contentPath = FileUtils::SysFileToPMString(assignmentFile);
#else
		contentPath = theNode->GetData();
#endif
		theNode->SetArticleDataFromXML(preview, artNbPhotos, nbChars, artType, folio, imageIDs, dossierCarton, fichierCarton);
	}
	else if(localname.compare(paragraphElement) == 0)
	{
		isTextContent = kFalse;
		preview.Append(kTextChar_CR);
		nbCharsPreview += 1;
	}
	
	currentElt.Clear();
}

void XPGXMLParserArticle::Characters(const WideString & chars) { 

	if(currentElt.compare(typeElement) == 0)
		artType = Utils<IDataStringUtils>()->StringToInt32(chars);
	else if(currentElt.compare(stateElement) == 0)
		state += chars;
	else if(currentElt.compare(nbCharsElement) == 0)
		nbChars = Utils<IDataStringUtils>()->StringToInt32(chars);
	else if(isTextContent && nbCharsPreview < maxChars)	{
		preview += chars;
		nbCharsPreview += chars.NumUTF16TextChars();
	}
	else if(currentElt.compare(folioElement) == 0)
		folio += chars;
	else if(currentElt.compare(dossierCartonElement) == 0)
		dossierCarton += chars;
	else if(currentElt.compare(fichierCartonElement) == 0)
		fichierCarton += chars;
}
