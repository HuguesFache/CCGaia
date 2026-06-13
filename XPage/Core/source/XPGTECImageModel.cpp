

#include "VCPluginHeaders.h"
#include <vector>

// Interface includes
#include "IXPageMgrAccessor.h"
#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "IImportProvider.h"
#include "IPMStream.h"
#include "IStringData.h"
#include "ISAXServices.h"
#include "ISAXContentHandler.h"
#include "ISAXEntityResolver.h"
#include "ISAXParserOptions.h"
#include "IPointerData.h"
#include "IXPGPreferences.h"
#include "IXPageUtils.h"
#include "IWebServices.h"

// Project includes
#include "IImageDataModel.h"
#include "XPGImageDataNode.h"
#include "XPGImageModelBase.h"
#include "FileUtils.h"
#include "OpenPlaceID.h"
#include "StreamUtil.h"
#include "ImageID.h"
#include "EPSID.h"
#include "PDFID.h"
#include "DCSID.h"
#include "PlatformFolderTraverser.h"
#include "CAlert.h"
#include "ErrorUtils.h"
#include "StringUtils.h"
#include "K2Stack.h"
#include "XMLDefs.h"


/**  Class representing a folder hierarchy.
*/
class XPGXRailImageModel : XPGImageModelBase
{
public:
	/** Default constructor */
	XPGXRailImageModel(IPMUnknown* boss);

	/**	See IImageDataModel::Rebuild
		Note : no need to provide numeroId for XRail implementation, caller must provide a filter before any call to Rebuild
	 */
	virtual void Rebuild(const PMString& numeroId, ImageType typeImage = kPhoto);

	/**	See IImageDataModel::RebuildFromArticles
	 */
	virtual void RebuildFromArticles(const K2Vector<PMString>& articleIds,
									 const K2Vector<PMString>& articleFiles);

	/**	See IImageDataModel::SetFilter
	 */
	virtual void SetFilter(const PMString& filter, const PMString& filterPath);

	/**	See IImageDataModel::GetImageData
	 */
	virtual bool16 GetImageData(const PMString& id, const IDFile& linkedArticleFile, IDFile& brFile,
								const bool16& wantNB, IDFile& hrFile, PMString& legend, PMString& credit);

protected:

	/**	See XPGImageModelBase::cacheChildren
	 */
	virtual void cacheChildren(const PMString& id);

	/** Determine if the file is an image xml description file
	*/
	bool16 isImageXMLFile(const PMString& filename);

	// ParseImageXMLFile (legacy per-image PHO.xml parser) was removed when
	// cacheChildren switched to reading the article-scoped OBJRART.xml in
	// one shot. The new SAX handler is incompatible with the old single-
	// node IPointerData contract, so calling the old method would crash.

private:

	/** Parse one article's OBJRART.xml (located at <articleFolder>/<articleId>.OBJRART.xml)
		and add an XPGImageDataNode under parentNode for every <Image> entry.
		Shared by cacheChildren (single-article mode) and RebuildFromArticles
		(multi-article "Voir toutes les photos" mode). Silent on missing/invalid
		files when reportErrors=kFalse — useful in multi-article mode where
		one stale assignment shouldn't kill the whole batch with an alert.
	*/
	void parseAndAddArticle(const PMString& articleId,
							const IDFile& articleFolder,
							XPGImageDataNode* parentNode,
							bool16 reportErrors);

	PMString currentFilter, currentFilterPath;
	K2Vector<PMString> imageSuffixes;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGXRailImageModel, kXPGXRailImageModelImpl)


/* Constructor
*/
XPGXRailImageModel::XPGXRailImageModel(IPMUnknown* boss) : 
	XPGImageModelBase(boss), currentFilter(kNullString), currentFilterPath(kNullString){
	
    int i = 1;
	K2::scoped_ptr<PMString> suffix (imgSuffixes.GetItem(";", i));

	while(suffix != nil)
	{
		imageSuffixes.push_back(*suffix);
		++i;
		suffix.reset(imgSuffixes.GetItem(";", i));
	} 	
}


/* SetFilter
*/
void XPGXRailImageModel::SetFilter(const PMString& filter, const PMString& filterPath)
{
	currentFilter = filter;
	currentFilterPath = filterPath;
}

/* Rebuild
*/
void XPGXRailImageModel::Rebuild(const PMString& numeroId, ImageType typeImage)
{
	if(fRootNode != nil) {
		deleteTree();
	}	
	if(currentFilter.IsEmpty() == kFalse) {
				
		// Set image root path to article file's parent directory
		IDFile imageRootFile = FileUtils::PMStringToSysFile(currentFilterPath);	
		FileUtils::GetParentDirectory(imageRootFile,imageRootFile);

		PMString imageRootFilePath = FileUtils::SysFileToPMString(imageRootFile);
		fRootNode =  new XPGImageDataNode(XPGImageDataNode::kRubriqueNode);
		fRootNode->SetID(imageRootFilePath);
		fRootNode->SetData(Utils<IXPageUtils>()->TruncatePath(imageRootFilePath));
		fRootNode->SetParent(nil);
							
		fIdNodeMap.insert( std::pair<PMString, XPGImageDataNode* >(imageRootFilePath, fRootNode));
		this->cacheChildren(imageRootFilePath);
	}
}
/* cacheChildren
 *
 * Single-article entry point. Resolves the parent node and dispatches to
 * parseAndAddArticle, which does the SAX parse + node creation. The
 * "Voir toutes les photos" multi-article path uses the same helper to
 * avoid drift between the two codepaths.
*/
void XPGXRailImageModel::cacheChildren(const PMString& parentId)
{
	do
	{
		if (currentFilter == kNullString)
			break;

		XPGImageDataNode* parentNode = nil;
		std::map<PMString, XPGImageDataNode*>::const_iterator iterForParent = fIdNodeMap.find(parentId);
		if (iterForParent == fIdNodeMap.end())
			break;
		parentNode = iterForParent->second;
		ASSERT(parentNode);

		// Only the root (article folder) carries an OBJRART.xml. The tree
		// view also calls cacheChildren when an image node is expanded,
		// passing the image id as parentId — there's nothing to load on a
		// leaf, so bail out silently. (Without this guard the code below
		// would try to read "<imageId>/<articleId>.OBJRART.xml" and surface
		// a misleading "Fichier article introuvable" alert per photo.)
		if (parentNode->GetType() != XPGImageDataNode::kRubriqueNode)
			break;

		IDFile articleFolder = FileUtils::PMStringToSysFile(parentId);
		this->parseAndAddArticle(currentFilter, articleFolder, parentNode, /*reportErrors=*/kTrue);

	} while (kFalse);
}

/* parseAndAddArticle
 *
 * Replaces the per-folder PHO.xml scan with a single OBJRART read:
 *
 *   1. Build the path <articleFolder>/<articleId>.OBJRART.xml.
 *   2. Stream-parse it via the OBJRART SAX handler. Each <Image> child
 *      inside the <Images> block becomes one ImageData entry.
 *   3. Convert each ImageData into a child node attached to parentNode.
 *
 * Cheaper than the old approach (1 file vs N) and aligns with the way
 * Gaia structures article data on disk. When reportErrors is kTrue and
 * the OBJRART is missing/unreadable we surface a single alert; in batch
 * (multi-article) mode the caller passes kFalse to keep stale entries
 * from spamming the user.
*/
void XPGXRailImageModel::parseAndAddArticle(const PMString& articleId,
											const IDFile& articleFolder,
											XPGImageDataNode* parentNode,
											bool16 reportErrors)
{
	if (parentNode == nil || articleId.IsEmpty())
		return;

	// Build <folder>/<articleID>.OBJRART.xml
	IDFile objrartFile = articleFolder;
	PMString objrartFilename(articleId);
	objrartFilename.Append(".OBJRART.xml");
	objrartFilename.SetTranslatable(kFalse);
	FileUtils::AppendPath(&objrartFile, objrartFilename);

	if (!FileUtils::DoesFileExist(objrartFile))
	{
		if (reportErrors)
		{
			PMString msg("Fichier article introuvable : ", PMString::kUnknownEncoding);
			msg.Append(FileUtils::SysFileToPMString(objrartFile));
			msg.SetTranslatable(kFalse);
			CAlert::InformationAlert(msg);
		}
		return;
	}

	// SAX parse — handler appends one ImageData per <Image> to outImages.
	std::vector<XPGImageDataNode::ImageData> outImages;

	InterfacePtr<IK2ServiceRegistry> serviceRegistry(GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IK2ServiceProvider> xmlProvider(serviceRegistry->QueryServiceProviderByClassID(kXMLParserService, kXMLParserServiceBoss));
	InterfacePtr<ISAXServices> saxServices(xmlProvider, UseDefaultIID());

	InterfacePtr<ISAXContentHandler> contentHandler(
		::CreateObject2<ISAXContentHandler>(kXPGXMLImageDescriptionHandlerBoss, ISAXContentHandler::kDefaultIID));

	InterfacePtr<IPointerData> nodeData(contentHandler, IID_INODEDATA);
	nodeData->Set(&outImages);

	contentHandler->Register(saxServices);

	InterfacePtr<ISAXEntityResolver> entityResolver(
		::CreateObject2<ISAXEntityResolver>(kXMLEmptyEntityResolverBoss, ISAXEntityResolver::kDefaultIID));

	InterfacePtr<ISAXParserOptions> parserOptions(saxServices, UseDefaultIID());
	parserOptions->SetNamespacesFeature(kFalse);
	parserOptions->SetShowWarningAlert(kFalse);

	InterfacePtr<IPMStream> xmlFileStream(StreamUtil::CreateFileStreamRead(objrartFile));
	saxServices->ParseStream(xmlFileStream, contentHandler);

	if (ErrorUtils::PMGetGlobalErrorCode() != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kSuccess);
		if (reportErrors)
		{
			PMString msg("Lecture impossible du fichier article : ", PMString::kUnknownEncoding);
			msg.Append(FileUtils::SysFileToPMString(objrartFile));
			msg.SetTranslatable(kFalse);
			CAlert::InformationAlert(msg);
		}
		return;
	}

	// For each <Image>, finalise the path fields (the handler stowed
	// the numeric id in hrFile and the extension in brFile during
	// parsing — see XPGXMLImageDescriptionHandler::EndElement) and
	// hand the data off to a new XPGImageDataNode child.
	for (int32 i = 0; i < outImages.size(); ++i)
	{
		const PMString imgId = outImages[i].hrFile;          // id stowed by handler
		const PMString imgExt = outImages[i].brFile;         // extension stowed by handler
		if (imgId.IsEmpty())
			continue;

		// HR  : <folder>/<id>.HR<extension>   (e.g. .HR.jpeg)
		// BR  : <folder>/<id>.BR.jpg          (always .jpg thumbnail)
		IDFile hrPath = articleFolder;
		PMString hrName(imgId);
		hrName.Append(".HR");
		hrName.Append(imgExt);
		hrName.SetTranslatable(kFalse);
		FileUtils::AppendPath(&hrPath, hrName);

		IDFile brPath = articleFolder;
		PMString brName(imgId);
		brName.Append(".BR.jpg");
		brName.SetTranslatable(kFalse);
		FileUtils::AppendPath(&brPath, brName);

		XPGImageDataNode::ImageData img = outImages[i];
		img.hrFile = FileUtils::SysFileToPMString(hrPath);
		img.brFile = FileUtils::SysFileToPMString(brPath);
		// Legacy field (kept for backwards-compat on any downstream
		// code that reads it) — point at the same HR file since the
		// N&B variant is no longer used.
		img.hrFileNB = img.hrFile;

		std::map<PMString, XPGImageDataNode*>::iterator existing = fIdNodeMap.find(imgId);
		if (existing != fIdNodeMap.end())
		{
			// Already loaded (e.g. same image referenced by two articles
			// in multi-article mode, or rebuild called on a still-loaded
			// model). Skip — the first occurrence wins.
			continue;
		}

		XPGImageDataNode* newNode = new XPGImageDataNode(XPGImageDataNode::kImageNode);
		newNode->SetID(imgId);                    // unique node ID = image id
		newNode->SetData(img.hrFile);             // legacy: kImage node "data" is the HR path
		newNode->SetImageData(img);

		newNode->SetParent(parentNode);
		parentNode->AddChild(static_cast<const XPGImageDataNode&>(*newNode));
		fIdNodeMap.insert(std::pair<PMString, XPGImageDataNode*>(imgId, newNode));
	}
}

/* RebuildFromArticles
 *
 * "Voir toutes les photos" mode. Builds a flat tree containing the union
 * of every article's photos. Article folder of articleFiles[0] is used as
 * the (visually-hidden) root rubrique label — same convention as Rebuild.
*/
void XPGXRailImageModel::RebuildFromArticles(const K2Vector<PMString>& articleIds,
											 const K2Vector<PMString>& articleFiles)
{
	if (fRootNode != nil)
		deleteTree();

	const int32 n = articleIds.size();
	if (n == 0 || n != articleFiles.size())
		return;

	// Pick the first article's folder as the root id. Multiple articles
	// may live in different folders — the root is just a stable parent
	// node for the tree, the visible content is its children.
	IDFile firstFolder = FileUtils::PMStringToSysFile(articleFiles[0]);
	FileUtils::GetParentDirectory(firstFolder, firstFolder);
	const PMString rootPath = FileUtils::SysFileToPMString(firstFolder);

	fRootNode = new XPGImageDataNode(XPGImageDataNode::kRubriqueNode);
	fRootNode->SetID(rootPath);
	fRootNode->SetData(Utils<IXPageUtils>()->TruncatePath(rootPath));
	fRootNode->SetParent(nil);
	fIdNodeMap.insert(std::pair<PMString, XPGImageDataNode*>(rootPath, fRootNode));

	// Track the active (filter, path) pair: GetImageData and the per-image
	// drag-drop source still consult these. Set to the last article so the
	// values aren't stale, even though they're not used while the tree is
	// flat-aggregated.
	for (int32 i = 0; i < n; ++i)
	{
		IDFile articleFolder = FileUtils::PMStringToSysFile(articleFiles[i]);
		FileUtils::GetParentDirectory(articleFolder, articleFolder);
		this->parseAndAddArticle(articleIds[i], articleFolder, fRootNode, /*reportErrors=*/kFalse);

		currentFilter = articleIds[i];
		currentFilterPath = articleFiles[i];
	}
}

bool16 XPGXRailImageModel::isImageXMLFile(const PMString& filename)
{
	int32 nbSuffixes = imageSuffixes.size();	
	for(int32 i = 0 ; i < nbSuffixes ; ++i)
	{
		int32 posSuff = filename.NumUTF16TextChars() - imageSuffixes[i].NumUTF16TextChars();
		if(posSuff > 0)
		{
			if(filename.IndexOfString(imageSuffixes[i]) == posSuff)
			{
				return kTrue;				
			}				
		}
	}
	
	return kFalse;
}

// ParseImageXMLFile was removed — see header note on cacheChildren.

/** GetImageData
*/
/* GetImageData
 *
 * Used by the drag-drop source when the user drags an image from the
 * palette into the layout. Now that cacheChildren has loaded every image
 * of the active article into fIdNodeMap, we just look up the entry by id
 * — no need to re-parse XML on each drag. linkedArticleFile is ignored
 * (kept for interface compatibility); wantNB has no effect since the B&W
 * variant is no longer surfaced.
*/
bool16 XPGXRailImageModel::GetImageData(const PMString& id, const IDFile& /*linkedArticleFile*/, IDFile& brFile,
									  const bool16& /*wantNB*/, IDFile& hrFile, PMString& legend, PMString& credit)
{
	std::map<PMString, XPGImageDataNode*>::const_iterator iter = fIdNodeMap.find(id);
	if (iter == fIdNodeMap.end() || iter->second == nil)
		return kFalse;

	const XPGImageDataNode::ImageData* data = iter->second->GetImageData();
	if (data == nil)
		return kFalse;

	brFile = FileUtils::PMStringToSysFile(data->brFile);
	hrFile = FileUtils::PMStringToSysFile(data->hrFile.IsEmpty() ? data->hrFileNB : data->hrFile);
	legend = data->legend;
	credit = data->credit;
	return kTrue;
}

//	end, File: XPGXRailImageModel.cpp
