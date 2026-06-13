#include "VCPlugInHeaders.h"
#include <vector>

// Interface includes:
#include "ISAXAttributes.h"
#include "ISession.h"
#include "ISAXServices.h"
#include "IXMLServicesError.h"
#include "IPointerData.h"
#include "IStringListData.h"

// General includes:
#include "FileUtils.h"
#include "CSAXContentHandler.h"
#include "CPMUnknown.h"
#include "CAlert.h"
#include "K2Vector.tpp"
#include "XPGImageDataNode.h"

// Project includes:
#include "XPGID.h"


/** XPGXMLImageDescriptionHandler
 *
 * SAX handler for the new <article-id>.OBJRART.xml format used to populate
 * the Photos liées palette. Replaces the legacy per-image PHO.xml parser
 * (one image per file) with a single-pass walk over a single article XML
 * that may contain N <Image> children.
 *
 * Caller protocol: create the boss, set the IPointerData to a
 *     std::vector<XPGImageDataNode::ImageData>*
 * (the destination buffer), then run ParseStream. The handler reads the
 * article folder from the input file path (passed via the data interface
 * before parsing begins — see model/cacheChildren) and appends one
 * ImageData per <Image> child it sees.
*/
class XPGXMLImageDescriptionHandler : public CSAXContentHandler
{
public:

	XPGXMLImageDescriptionHandler(IPMUnknown *boss);
	virtual ~XPGXMLImageDescriptionHandler();

	virtual void Register(ISAXServices* saxServices, IPMUnknown *importer);
	virtual bool16 HandlesSubElements() const;

	virtual void Characters(const WideString& chars);
	virtual void StartElement(const WideString& uri, const WideString& localname, const WideString& qname, ISAXAttributes* attrs);
	virtual void EndElement(const WideString& uri, const WideString& localname, const WideString& qname);

private:

	// Tag names found in OBJRART.
	static const WideString objetElement;
	static const WideString imageElement;
	static const WideString legendeElement;
	static const WideString creditElement;
	static const WideString nomElement;
	static const WideString commentaireElement;
	static const WideString focalXElement;
	static const WideString focalYElement;

	// Attributes on <Objet> and <Image>.
	static const WideString idAttr;
	static const WideString extensionAttr;
	static const WideString cropHAttr;
	static const WideString cropWAttr;
	static const WideString cropXAttr;
	static const WideString cropYAttr;
	static const WideString typeObjetAttr;
	static const WideString etatImageAttr;

	// Parser state.
	PMString fArticleId;            // From <Objet id="...">.
	PMString fArticleFolder;        // Set by the caller via fIDData on the IPointerData target — actually we read it from the buffer.
	PMString fArticleComment;       // Article-level <Commentaire> under <Objet>; copied into each emitted image.
	bool16   fInsideImage;
	WideString fCurrentElt;

	// Accumulators for the currently-parsed <Image>.
	XPGImageDataNode::ImageData fCurrent;

	// Helper: parse a French-decimal-comma string ("159,6") as PMReal.
	static PMReal ParseFrenchReal(const PMString& s);
};


CREATE_PMINTERFACE(XPGXMLImageDescriptionHandler, kXPGXMLImageDescriptionHandlerImpl)


// --- Tag / attribute literals --------------------------------------------

const WideString XPGXMLImageDescriptionHandler::objetElement       = WideString("Objet");
const WideString XPGXMLImageDescriptionHandler::imageElement       = WideString("Image");
const WideString XPGXMLImageDescriptionHandler::legendeElement     = WideString("Legende");
const WideString XPGXMLImageDescriptionHandler::creditElement      = WideString("Credit");
const WideString XPGXMLImageDescriptionHandler::nomElement         = WideString("Nom");
const WideString XPGXMLImageDescriptionHandler::commentaireElement = WideString("Commentaire");
const WideString XPGXMLImageDescriptionHandler::focalXElement      = WideString("focal_x");
const WideString XPGXMLImageDescriptionHandler::focalYElement      = WideString("focal_y");

const WideString XPGXMLImageDescriptionHandler::idAttr        = WideString("id");
const WideString XPGXMLImageDescriptionHandler::extensionAttr = WideString("extension");
const WideString XPGXMLImageDescriptionHandler::cropHAttr     = WideString("cropH");
const WideString XPGXMLImageDescriptionHandler::cropWAttr     = WideString("cropW");
const WideString XPGXMLImageDescriptionHandler::cropXAttr     = WideString("cropX");
const WideString XPGXMLImageDescriptionHandler::cropYAttr     = WideString("cropY");
const WideString XPGXMLImageDescriptionHandler::typeObjetAttr = WideString("typeObjet");
const WideString XPGXMLImageDescriptionHandler::etatImageAttr = WideString("etatimage");


XPGXMLImageDescriptionHandler::XPGXMLImageDescriptionHandler(IPMUnknown *boss)
	: CSAXContentHandler(boss),
	  fInsideImage(kFalse)
{
}

XPGXMLImageDescriptionHandler::~XPGXMLImageDescriptionHandler() {}


void XPGXMLImageDescriptionHandler::Register(ISAXServices* saxServices, IPMUnknown *importer)
{
	if (saxServices == nil)
		return;
	// Top-level <Objet> registration — HandlesSubElements returns kTrue
	// so we get every nested element through StartElement/EndElement.
	saxServices->RegisterElementHandler(WideString(""), objetElement, this);
}


bool16 XPGXMLImageDescriptionHandler::HandlesSubElements() const
{
	return kTrue;
}


void XPGXMLImageDescriptionHandler::StartElement(const WideString& uri, const WideString& localname,
                                                 const WideString& qname, ISAXAttributes* attrs)
{
	fCurrentElt = localname;

	if (localname.compare(objetElement) == 0)
	{
		// <Objet id="429024" ...> — root article id, propagated to every
		// image emitted from this file.
		if (HasAttribute(attrs, idAttr))
			fArticleId = GetAttributeString(attrs, idAttr);
		return;
	}

	if (localname.compare(imageElement) == 0)
	{
		// New <Image …> — reset accumulators and capture attributes.
		fInsideImage = kTrue;
		fCurrent = XPGImageDataNode::ImageData();   // zero-init crop / focal.
		fCurrent.articleID = fArticleId;

		if (HasAttribute(attrs, idAttr))
		{
			// We tuck the image's numeric id into hrFile temporarily so
			// EndElement can build the final filename without keeping a
			// dedicated field. (Replaced with the full path below.)
			fCurrent.hrFile = GetAttributeString(attrs, idAttr);
		}
		if (HasAttribute(attrs, extensionAttr))
			fCurrent.brFile = GetAttributeString(attrs, extensionAttr);  // re-used as scratch.
		if (HasAttribute(attrs, typeObjetAttr))
			fCurrent.type = GetAttributeString(attrs, typeObjetAttr);
		if (HasAttribute(attrs, cropHAttr))
			fCurrent.cropH = ParseFrenchReal(GetAttributeString(attrs, cropHAttr));
		if (HasAttribute(attrs, cropWAttr))
			fCurrent.cropW = ParseFrenchReal(GetAttributeString(attrs, cropWAttr));
		if (HasAttribute(attrs, cropXAttr))
			fCurrent.cropX = ParseFrenchReal(GetAttributeString(attrs, cropXAttr));
		if (HasAttribute(attrs, cropYAttr))
			fCurrent.cropY = ParseFrenchReal(GetAttributeString(attrs, cropYAttr));
		if (HasAttribute(attrs, etatImageAttr))
		{
			PMString etatStr(GetAttributeString(attrs, etatImageAttr));
			PMString::ConversionError convErr = PMString::kNoError;
			fCurrent.etatImage = etatStr.GetAsNumber(&convErr);
			if (convErr != PMString::kNoError)
				fCurrent.etatImage = 0;
		}
	}
}


void XPGXMLImageDescriptionHandler::EndElement(const WideString& uri, const WideString& localname,
                                               const WideString& qname)
{
	if (localname.compare(imageElement) == 0)
	{
		// Wrap up the current <Image> and push it to the destination
		// vector held by the caller via IPointerData.
		InterfacePtr<IPointerData> dataPtr(this, IID_INODEDATA);
		if (dataPtr != nil)
		{
			std::vector<XPGImageDataNode::ImageData>* outVec =
				static_cast<std::vector<XPGImageDataNode::ImageData>*>(dataPtr->Get());
			if (outVec != nil)
			{
				// The OBJRART format actually carries <Commentaire> per-image
				// (inside each <Image>), so the gating in Characters() picks
				// it up directly. We still keep an article-level capture as a
				// belt-and-braces fallback in case a future variant of the
				// XML moves the field up to <Objet> level. The per-image
				// value always wins when present.
				if (fCurrent.comment.IsEmpty() && !fArticleComment.IsEmpty())
					fCurrent.comment = fArticleComment;

				// hrFile currently holds the image numeric id, brFile the
				// extension string. Caller (the model) finishes the path
				// by prepending the article folder — the handler doesn't
				// know it (no SDK API to fetch the source URI from inside
				// SAX). The model rewrites these fields right after we
				// emit, so they're really just (id, extension) carriers
				// at this point.
				outVec->push_back(fCurrent);
			}
		}
		fInsideImage = kFalse;
	}

	fCurrentElt.Clear();
}


void XPGXMLImageDescriptionHandler::Characters(const WideString& chars)
{
	// Article-level <Commentaire> (sibling of <Images>, not nested inside an
	// <Image>) — capture it once so EndElement can fold it into each emitted
	// ImageData. We gate on the tag name to avoid swallowing whitespace or
	// other article-level text.
	if (!fInsideImage)
	{
		if (fCurrentElt.compare(commentaireElement) == 0)
			fArticleComment.Append(PMString(chars));
		return;
	}

	// Inner text elements of the current <Image>. Article-level <Nom>,
	// <Legende>, etc. (the fields outside <Images>) are intentionally
	// ignored — fInsideImage gates them out (except <Commentaire>, handled
	// above).
	if (fCurrentElt.compare(legendeElement) == 0)
		fCurrent.legend.Append(PMString(chars));
	else if (fCurrentElt.compare(creditElement) == 0)
		fCurrent.credit.Append(PMString(chars));
	else if (fCurrentElt.compare(nomElement) == 0)
		fCurrent.name.Append(PMString(chars));
	else if (fCurrentElt.compare(commentaireElement) == 0)
		fCurrent.comment.Append(PMString(chars));
	else if (fCurrentElt.compare(focalXElement) == 0)
	{
		PMString tmp(chars);
		fCurrent.focalX = ParseFrenchReal(tmp);
	}
	else if (fCurrentElt.compare(focalYElement) == 0)
	{
		PMString tmp(chars);
		fCurrent.focalY = ParseFrenchReal(tmp);
	}
}


/* ParseFrenchReal
 *
 * The OBJRART server output uses a French decimal comma ("159,6"). Convert
 * to a dot before delegating to PMString::GetAsDouble. Robust to ASCII
 * digits / minus sign — anything else returns 0.
*/
PMReal XPGXMLImageDescriptionHandler::ParseFrenchReal(const PMString& s)
{
	PMString normalised(s);
	normalised.StripWhiteSpace(PMString::kLeadingAndTrailingWhiteSpace);
	if (normalised.IsEmpty())
		return PMReal(0);

	PlatformChar comma; comma.Set(',');
	const int32 commaIdx = normalised.IndexOfCharacter(comma);
	if (commaIdx >= 0)
	{
		normalised.Remove(commaIdx, 1);
		normalised.Insert(PMString(".", PMString::kUnknownEncoding), commaIdx);
	}

	PMString::ConversionError err = PMString::kNoError;
	double v = normalised.GetAsDouble(&err);
	if (err != PMString::kNoError)
		return PMReal(0);
	return PMReal(v);
}
