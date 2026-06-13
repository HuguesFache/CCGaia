
#include "VCPluginHeaders.h"

// Interface includes
#include "IDocumentCommands.h"
#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "ISAXContentHandler.h"
#include "ISAXParserOptions.h"
#include "ISAXServices.h"
#include "IPMStream.h"
#include "IXPageMgrAccessor.h"
#include "ISAXEntityResolver.h"
#include "IXPGPreferences.h"
#include "IStringListData.h"
#include "ISnippetExport.h"
#include "IXMLOutStream.h"
#include "IXMLStreamUtils.h"
#include "ICoreFilename.h"
#include "IFormeData.h"
#include "ILinkFacade.h"
#include "ILinkUtils.h"
#include "IAssignmentUtils.h"
#include "IStringData.h"
#include "IBoolData.h"
#include "IAssignmentMgr.h"
#include "PlatformFolderTraverser.h"

// Impl includes
#include <map>

#ifdef WINDOWS
#include <sys\stat.h> // for _S_IWRITE constant
#else

#endif

// Project includes
#include "UIDList.h"
#include "IFormeDataModel.h"
#include "XPGFormeDataNode.h"
#include "FileUtils.h"
#include "IXPageUtils.h"
#include "StreamUtil.h"
#include "ErrorUtils.h"
#include "StringUtils.h"
#include "Utils.h"
#include "SDKFileHelper.h"
#include "ErrorUtils.h"
#include "ICommand.h"
#include "IDataBase.h"
#include "IURIUtils.h"
#include "URI.h"
#include "CAlert.h"
#include "IWebServices.h"



/**  Class representing a folder hierarchy. 
	@ingroup paneltreeview
*/
class XPGFileSystemFormeModel : CPMUnknown<IFormeDataModel>
{
public:
	/** Default constructor */
	XPGFileSystemFormeModel(IPMUnknown* boss);

	/**	Destructor
	 */
	virtual ~XPGFileSystemFormeModel();

	/**		See IFormeDataModel::GetRootID 
	*/
	virtual PMString GetRootID() const;

	/** 	See IFormeDataModel::GetNode
	*/
	virtual XPGFormeDataNode* GetNode(const PMString& id) const;


	/**		See IFormeDataModel::GetParent
	 */
	virtual const XPGFormeDataNode * GetParent(const PMString& id) const;

	/**		See IFormeDataModel::GetChildCount
	 */
	virtual int32 GetChildCount(const PMString& id); 

	/**		See IFormeDataModel::GetNthChild
	 */
	virtual const XPGFormeDataNode * GetNthChild(const PMString& id, int32 nth); 

	/**		See IFormeDataModel::GetChildIndexFor 
	 */
	virtual int32 GetChildIndexFor(const PMString& par, const PMString& kid); 

	/**	See IFormeDataModel::Rebuild
	 */
	virtual void Rebuild(const PMString& classeurName);

	/** See IFormeDataModel::GetClasseurList
	*/
	virtual K2Vector<PMString>  GetClasseurList();

	/** See IFormeDataModel::DoesFormeExist
	*/
	virtual bool16 DoesFormeExist(const PMString& formeName, const PMString& classeurName) const;

	/** See IFormeDataModel::IsPhotoCarton
	*/
	virtual bool16 IsPhotoCarton(const PMString& formeName, const PMString& classeurName);

	/** See IFormeDataModel::CreateForme
	*/
	virtual ErrorCode CreateForme(const PMString& name, const PMString& classeurName, K2Vector<IDValuePair> matchingStyles,
								  const UIDList& textFrames, const UIDList& photoFrames, const UIDList& fixedContentFrames, const UIDRef& storyRef,
								  const UIDList& creditRefs, const UIDList& legendRefs, const UIDList& formeItemsToExport,
								  const PMString posx, const PMString posy, const PMString largeur, const PMString hauteur, const int32 nbsignesestim,
								  const bool16 isPhotoCarton);

	/** See IFormeDataModel::DeleteForme
	*/
	virtual ErrorCode DeleteForme(const PMString& name, const PMString& classeurName);

	/** See IFormeDataModel::CreateClasseur
	*/
	virtual ErrorCode CreateClasseur(const PMString& classeurName);

	/** See IFormeDataModel::DeleteClasseur
	*/
	virtual ErrorCode DeleteClasseur(const PMString& classeurName);

	/** See IFormeDataModel::DoesClasseurExist
	*/
	virtual bool16 DoesClasseurExist(const PMString& classeurName);

	/** See IFormeDataModel::GetFormeList
	*/
	virtual K2Vector<PMString> GetFormeList(const PMString& classeurName);

	/** See IFormeDataModel::GetForme
	*/
	virtual bool16 GetForme(const PMString& name, const PMString& classeurName, 
							IDFile& formeFile, IDFile& matchingFile, PMString& posX, PMString& posY);

protected:

	/** Cache the kids of the given node in file system
	*/
	void cacheChildren(const PMString& id);	
	
	/**	Determine if this is a path that we want to put in the data model
		@param p [IN] specifies path of interest
		@return bool16 if the path is a file system path of interest
	 */
	bool16 ValidPath(const PMString& p);

	/**	Destroy the tree represented in this 
	 */
	void deleteTree() ;
	

private:

	ErrorCode parseFormeDescriptif(const IDFile& descFile, PMString& formeName, PMString& classeurName,
								   IDFile& formeFile, IDFile& matchingFile, PMString& posX, PMString& posY,
								   bool16& isPhotoCarton);

	std::map<PMString, XPGFormeDataNode* > fIdNodeMap;
	XPGFormeDataNode* fRootNode;

	PMString formeRoot;
	PMString descDir, typoDir, formeDir, vignetteDir;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGFileSystemFormeModel, kXPGFileSystemFormeModelImpl)


/* Constructor
*/
XPGFileSystemFormeModel::XPGFileSystemFormeModel(IPMUnknown* boss) : 
	CPMUnknown<IFormeDataModel>(boss),
	fRootNode(nil){
	InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
	formeRoot = xpgPrefs->GetCheminFormes();
	descDir = "Descriptif";
	typoDir = "Typographie";
	formeDir = "Forme";
	vignetteDir = "Vignette";
}
	
/* Destructor
*/
XPGFileSystemFormeModel::~XPGFileSystemFormeModel()
{
	deleteTree();
}


/* deleteTree
*/
void XPGFileSystemFormeModel::deleteTree() 
{
	// We've flattened the tree in our hashtable anyway so 
	// use this to delete the nodes.
	std::map<PMString, XPGFormeDataNode* >::iterator iter;
	for(iter = fIdNodeMap.begin(); iter != fIdNodeMap.end(); iter++)
	{
		XPGFormeDataNode* node = iter->second;
		ASSERT(node);
		delete node;
	}
	fIdNodeMap.clear();
	fRootNode = nil;
}



/* Rebuild
*/
void XPGFileSystemFormeModel::Rebuild(const PMString& classeurName)
{
	if(fRootNode != nil || !fIdNodeMap.empty() ) {		
		deleteTree();
	} 
	
	if(classeurName != kNullString)
	{
		IDFile formeRootFile = FileUtils::PMStringToSysFile(formeRoot);
		if(!FileUtils::DoesFileExist(formeRootFile))
			return;		

		FileUtils::AppendPath(&formeRootFile, classeurName);
		PMString classeurFilePath = FileUtils::SysFileToPMString(formeRootFile);

		fRootNode =  new XPGFormeDataNode(XPGFormeDataNode::kClasseurNode);
		fRootNode->SetID(classeurFilePath);
		
		
		fRootNode->SetData(Utils<IXPageUtils>()->TruncatePath(classeurFilePath));
	
		fRootNode->SetParent(nil);
	
		fIdNodeMap.insert( std::pair<PMString, XPGFormeDataNode* >(classeurFilePath, fRootNode));
		this->cacheChildren(classeurFilePath);
		
	}
}


/* GetParent
*/
const XPGFormeDataNode * XPGFileSystemFormeModel::GetParent(const PMString& id) const
{
	// We assume that the parent is cached, because we
	// found its kids
	std::map<PMString, XPGFormeDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end())
	{
		XPGFormeDataNode* node = result->second;
		ASSERT(node);
		ASSERT(fRootNode);
		// Use the comparison we've defined on the node class
		if((*node) == (*fRootNode))
		{
			return nil;
		}
		return node->GetParent();
	}
	return nil;
}


/* GetChildCount
*/
int32 XPGFileSystemFormeModel::GetChildCount(const PMString& id) // not const any more
{
	std::map<PMString, XPGFormeDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end()) {
		int32 possChildCount = result->second->ChildCount();
		if(possChildCount > 0) {
			// Don't trust a zero value, it may just be we didn't yet cache the kids
			return possChildCount;
		}
	}
	// So, we may not have cached the kids.
	this->cacheChildren(id);
	// Don't recurse, as we may have no kids and we only want one try
	std::map<PMString, XPGFormeDataNode* >::const_iterator postCacheResult = fIdNodeMap.find(id);
	if(postCacheResult != fIdNodeMap.end()) {
		return postCacheResult->second->ChildCount();
	}
	return 0;
}

/* GetNthChild
*/
const XPGFormeDataNode * XPGFileSystemFormeModel::GetNthChild(
	const PMString& id, int32 nth) // not const any more
{
	std::map<PMString, XPGFormeDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end()) {
		int32 possChildCount = result->second->ChildCount();
		if(possChildCount > 0) {
			// Again, distrust a node with zero child count, may be kids not yet cached 
			return &(result->second->GetNthChild(nth));
		}
	}
	// Again, we may not have cached the kids.
	this->cacheChildren(id);
	// Don't recurse, as we may have no kids and we only want one try
	std::map<PMString, XPGFormeDataNode* >::const_iterator postCacheResult = fIdNodeMap.find(id);
	if(postCacheResult != fIdNodeMap.end()) {
		return &(postCacheResult->second->GetNthChild(nth));
	}

	return nil;
}

/* GetChildIndexFor
*/
int32  XPGFileSystemFormeModel::GetChildIndexFor(
	const PMString& par, const PMString& kid) //const
{
	// Can we be called here without the kids being already cached?
	int32 retval=(-1);
	std::map<PMString, XPGFormeDataNode* >::const_iterator result = fIdNodeMap.find(par);
	if(result != fIdNodeMap.end()) {
		XPGFormeDataNode* node = result->second;
		if(node->ChildCount() == 0) {
			// Try to cache the kids, we may not have done so
			this->cacheChildren(par);
		}
		for(int32 i=0; i < node->ChildCount(); i++){
			PMString kidId = node->GetNthChild(i).GetID();
			if(kid == kidId) {
				retval=i;
				break;
			}
		}
	}
	
	return retval;
}


/* GetRootID
*/
PMString XPGFileSystemFormeModel::GetRootID() const
{
	if(fRootNode != nil)
	{
		return fRootNode->GetID();
	}
	return PMString();
}

XPGFormeDataNode* XPGFileSystemFormeModel::GetNode(const PMString& id) const
{
	XPGFormeDataNode* retval = nil;

	std::map<PMString, XPGFormeDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end()) {
		retval = result->second;
	}
	return retval;
}

/* validPath
*/
bool16 XPGFileSystemFormeModel::ValidPath(const PMString& p)
{
	const PMString thisDir(".");
	const PMString parentDir("..");
	return p != thisDir && p != parentDir;
}

/* cacheChildren
*/
void XPGFileSystemFormeModel::cacheChildren(const PMString& parentId)
{

	do
	{	
		XPGFormeDataNode* parentNode = nil;
		std::map<PMString, XPGFormeDataNode* >::const_iterator iterForParent = fIdNodeMap.find(parentId);
	
		if(iterForParent == fIdNodeMap.end()) {
			break;
		}
		parentNode = iterForParent->second;
		ASSERT(parentNode);
	
		if(parentNode->GetType() == XPGFormeDataNode::kClasseurNode)
		{
			IDFile parIDFile = FileUtils::PMStringToSysFile(parentId);
			IDFile descriptifDirFile = parIDFile;
			FileUtils::AppendPath(&descriptifDirFile, descDir);

			PlatformFolderTraverser folderTraverser(descriptifDirFile, kFalse, kFalse, kFalse, kTrue);
            IDFile idFile;
			while (folderTraverser.Next(&idFile))
            {
                
				PMString idFilePath = FileUtils::SysFileToPMString(idFile);
				PMString extension;
				FileUtils::GetExtension(idFile, extension);
                bool isDir = FileUtils::IsDirectory(idFile);
				if(!isDir && extension.Compare(kFalse, "xml") == 0)
				{
					XPGFormeDataNode* newNode = new XPGFormeDataNode(XPGFormeDataNode::kFormeNode);

					newNode->SetID(idFilePath);
					newNode->SetData(idFilePath);
					newNode->SetParent(parentNode);
					
					// Parse xml file to get related forme data
					PMString formeName = kNullString, classeurName = kNullString;
					PMString posx, posy;
					IDFile formeFile, matchingFile;
					bool16 isPhotoCarton = kFalse;

					if(parseFormeDescriptif(idFile, formeName, classeurName, formeFile, matchingFile, posx, posy, isPhotoCarton) != kSuccess)
					{
						// Error occured while parsing xml file, don't add node, clean up memory
						delete newNode;
					}
					else
					{

						// Check whether an item with the same id is there already...if so, don't add current item

						std::map<PMString, XPGFormeDataNode* >::iterator existingIter = fIdNodeMap.find(newNode->GetID());

						if(existingIter != fIdNodeMap.end())
						{
							delete newNode;
						}
						else
						{
							newNode->SetFormeData(formeName, classeurName, matchingFile, formeFile, posx, posy, isPhotoCarton);
							parentNode->AddChild(static_cast<const XPGFormeDataNode& >(*newNode));
							// Add node to the map
							fIdNodeMap.insert( std::pair<PMString, XPGFormeDataNode* > (idFilePath, newNode));
						}
					}
				}
			}
		}
		
	} while(kFalse);

}

/* parseFormeDescriptif
*/
ErrorCode XPGFileSystemFormeModel::parseFormeDescriptif(const IDFile& descFile, PMString& formeName,
														PMString& classeurName,IDFile& formeFile,
														IDFile& matchingFile, PMString& posX, PMString& posY,
														bool16& isPhotoCarton) {
	isPhotoCarton = kFalse;
	ErrorCode status = kSuccess;
	// Parse xml file to get related forme data
	InterfacePtr<IK2ServiceRegistry> serviceRegistry(GetExecutionContextSession(), UseDefaultIID()); 
	InterfacePtr<IK2ServiceProvider> xmlProvider(serviceRegistry->QueryServiceProviderByClassID(kXMLParserService, kXMLParserServiceBoss)); 
	
	InterfacePtr<ISAXServices> saxServices(xmlProvider, UseDefaultIID()); 

	InterfacePtr<ISAXContentHandler> contentHandler(static_cast<ISAXContentHandler*>(::CreateObject(kXPGXMLFormeDescriptionHandlerBoss, ISAXContentHandler::kDefaultIID))); 
	contentHandler->Register(saxServices);

	InterfacePtr<ISAXEntityResolver> entityResolver(static_cast<ISAXEntityResolver*>(::CreateObject(kXMLEmptyEntityResolverBoss, ISAXEntityResolver::kDefaultIID)));
	InterfacePtr<ISAXParserOptions> parserOptions(saxServices, UseDefaultIID());
	
	// These commitments: follow 
	// the existing ImportXMLFileCmd
	parserOptions->SetNamespacesFeature(kFalse);
	parserOptions->SetShowWarningAlert(kFalse);

	InterfacePtr<IPMStream> xmlFileStream(StreamUtil::CreateFileStreamRead(descFile));
	saxServices->ParseStream(xmlFileStream, contentHandler, entityResolver);	
	if(ErrorUtils::PMGetGlobalErrorCode() != kSuccess)
	{
		status = kFailure;
		// Reset Error Code
		ErrorUtils::PMSetGlobalErrorCode(kSuccess);
		PMString error(kXPGErrParseFormeFailed);
		error.Translate();
		StringUtils::ReplaceStringParameters(&error, FileUtils::SysFileToPMString(descFile));
		CAlert::InformationAlert(error);
	}
	else
	{						
		IDFile classeur;
		FileUtils::GetParentDirectory (descFile, classeur); 
		FileUtils::GetParentDirectory (classeur, classeur);

		// Get back forme data
		InterfacePtr<IStringListData> formeData (contentHandler, UseDefaultIID());
		formeName = formeData->GetStringList()[0];
		classeurName = formeData->GetStringList()[1];
		PMString formeFileName = formeData->GetStringList()[2];
		PMString typoFileName = formeData->GetStringList()[3];
		posX = formeData->GetStringList()[4];
		posY = formeData->GetStringList()[5];

		// Index 6 was added with the "Carton photo" feature; older descriptifs
		// won't have it, so guard against a short list.
		if (formeData->GetStringList().size() > 6)
			isPhotoCarton = (formeData->GetStringList()[6].Compare(kFalse, "1") == 0) ? kTrue : kFalse;

		formeFile = classeur;
		FileUtils::AppendPath(&formeFile, formeDir);
		FileUtils::AppendPath(&formeFile, formeFileName);

		matchingFile = classeur;
		FileUtils::AppendPath(&matchingFile, typoDir);
		FileUtils::AppendPath(&matchingFile, typoFileName);
	}

	return status;
}

/* GetClasseurList
*/
K2Vector<PMString> XPGFileSystemFormeModel::GetClasseurList()
{	
	K2Vector<KeyValuePair<PMString, PMString> > subFolderList = Utils<IXPageUtils>()->GetSubDirectories(formeRoot);
	
	K2Vector<PMString> classeurList;
	for(int32 i = 0 ; i < subFolderList.size() ; ++i)
	{
		PMString classeurName(subFolderList[i].Value());
		classeurName.SetTranslatable(kFalse);
		classeurList.push_back(classeurName);
	}	
	return classeurList;
}

bool16 XPGFileSystemFormeModel::DoesFormeExist(const PMString& formeName, const PMString& classeurName) const
{
	if(formeName == kNullString || classeurName == kNullString)
		return kFalse;

	IDFile formeFile = FileUtils::PMStringToSysFile(formeRoot);
	FileUtils::AppendPath(&formeFile, classeurName);
	FileUtils::AppendPath(&formeFile, descDir);
	FileUtils::AppendPath(&formeFile, formeName+".xml");

	return FileUtils::DoesFileExist(formeFile);
}



ErrorCode XPGFileSystemFormeModel::CreateForme(const PMString& name,
											   const PMString& classeurName,
											   K2Vector<IDValuePair> matchingStyles,
											   const UIDList& textFrames,
											   const UIDList& photoFrames,
                                               const UIDList& fixedContentFrames,
											   const UIDRef& storyRef,
											   const UIDList& creditRefs,
											   const UIDList& legendRefs,
											   const UIDList& formeItemsToExport,
											   const PMString posx, const PMString posy, const PMString largeur, const PMString hauteur, const int32 nbsignesestim,
											   const bool16 isPhotoCarton)
{

	ErrorCode success = kFailure;
	do {

		for(int32 i = 0 ; i < textFrames.Length() ; ++i){
			UIDRef itemRef = textFrames.GetRef(i);
			if(itemRef != UIDRef::gNull){
				Utils<IXPageUtils>()->SetFormeData(itemRef, classeurName + "_" + name, IFormeData::kArticle, -1);
			}
		}
	
		// Set photos, credits and legends		
		for(int32 i = 0 ; i < photoFrames.Length() ; ++i){
			Utils<IXPageUtils>()->SetFormeData(photoFrames.GetRef(i), classeurName + "_" + name, IFormeData::kPhoto, i);
			
			if(creditRefs.At(i) != kInvalidUID)
				Utils<IXPageUtils>()->SetFormeDataOnStory(creditRefs.GetRef(i), classeurName + "_" + name, IFormeData::kCredit, matchingStyles, i);

			if(legendRefs.At(i) != kInvalidUID)
				Utils<IXPageUtils>()->SetFormeDataOnStory(legendRefs.GetRef(i), classeurName + "_" + name, IFormeData::kLegend, matchingStyles, i);
		}
        
        // GD 27.04.2023 ++
        // Ici (important que ce soit apres le set textFrames et photos/credits/legendes) on set les blocs a contenu fixe
        for(int32 i = 0 ; i < fixedContentFrames.Length() ; ++i){
            UIDRef itemRef = fixedContentFrames.GetRef(i);
            if(itemRef != UIDRef::gNull){
                Utils<IXPageUtils>()->SetFormeData(itemRef, classeurName + "_" + name, IFormeData::kFixedContent, -1);
            }
        }
        // GD 27.04.2023 --
        
		// Create the snippet file		
		IDFile classeur = FileUtils::PMStringToSysFile(formeRoot);
		FileUtils::AppendPath(&classeur, classeurName);		
			
		// Create the matching file from given one
		IDFile formeMatchingFile = classeur;
		FileUtils::AppendPath(&formeMatchingFile, typoDir);
		FileUtils::CreateFolderIfNeeded(formeMatchingFile);
		FileUtils::AppendPath(&formeMatchingFile, name + ".xml");

		InterfacePtr<IXMLOutStream> typoStream(Utils<IXMLStreamUtils>()->CreateXMLOutFileStream(formeMatchingFile, IXMLOutStream::kUTF8));
		if (!typoStream)
			break;

		typoStream->Push(WideString("MATCHING"));
		typoStream->Pop(kTrue);
		typoStream->Pop(kTrue);
		typoStream->Flush();
		typoStream->Close();
	
		// Create the descriptif file
		IDFile descFile = classeur;
		FileUtils::AppendPath(&descFile, descDir);
		FileUtils::CreateFolderIfNeeded(descFile);
		FileUtils::AppendPath(&descFile, name+".xml");

		InterfacePtr<IXMLOutStream> descStream (Utils<IXMLStreamUtils>()->CreateXMLOutFileStream(descFile, IXMLOutStream::kUTF8));
		if(!descStream)
			break;

		descStream->DocHeader(WideString("1.0"), kTrue);
		IXMLOutStream::AttributeList attrs;
		attrs.push_back(IXMLOutStream::Attribute(WideString("name"), WideString(name)));
		attrs.push_back(IXMLOutStream::Attribute(WideString("classeur"), WideString(classeurName)));
		attrs.push_back(IXMLOutStream::Attribute(WideString("largeur"), WideString(largeur)));
		attrs.push_back(IXMLOutStream::Attribute(WideString("hauteur"), WideString(hauteur)));
		attrs.push_back(IXMLOutStream::Attribute(WideString("posx"), WideString(posx)));
		attrs.push_back(IXMLOutStream::Attribute(WideString("posy"), WideString(posy)));
		// Tag photo-cartons explicitly. Older descriptif files don't have this
		// attribute — readers default to "0"/false, so existing cartons are
		// untouched. We only emit it when true to keep the XML compact.
		if (isPhotoCarton)
			attrs.push_back(IXMLOutStream::Attribute(WideString("isPhotoCarton"), WideString("1")));
		descStream->PushWithAttributes(WideString("FORME"), attrs, kTrue);
		descStream->Push(WideString("FICHIERFORME"));
		descStream->Write(WideString(name+".inds"));
		descStream->Pop(kFalse);	
		descStream->Push(WideString("FICHIERTYPO"));
		descStream->Write(WideString(name+".xml"));
		descStream->Pop(kTrue);		
		descStream->Push(WideString("FICHIERVIGNETTE"));
		descStream->Write(WideString(name+".jpg"));
		descStream->Pop(kTrue);		
		descStream->Pop(kTrue);
		descStream->Flush();
		descStream->Close();
	
		IDFile snippetFile = classeur;
		FileUtils::AppendPath(&snippetFile, formeDir);
		FileUtils::CreateFolderIfNeeded(snippetFile);
		FileUtils::AppendPath(&snippetFile, name+".inds");
		
		// Export snippet
		InterfacePtr<IPMStream> snippetStream (StreamUtil::CreateFileStreamWrite(snippetFile, kOpenOut|kOpenTrunc, 'ICx4', 'InCp'));
		if(Utils<ISnippetExport>()->ExportPageitems(snippetStream, formeItemsToExport)!= kSuccess){	
			snippetStream->Close();
			break;
		}				
		snippetStream->Close();
	
		// Create the snapshot forme
		IDFile formeVignette = classeur;
		FileUtils::AppendPath(&formeVignette, vignetteDir);
		FileUtils::CreateFolderIfNeeded(formeVignette);
		FileUtils::AppendPath(&formeVignette, name+".jpg");
		
		// Create the snapshot for the website
		if(Utils<IXPageUtils>()->DoSnapShot(formeItemsToExport, formeVignette) != kSuccess)
			break;

		
		success = kSuccess;
		
	}while(kFalse);

	return success;
}

ErrorCode XPGFileSystemFormeModel::DeleteForme(const PMString& name, const PMString& classeurName)
{
	ErrorCode success = kFailure;
	do
	{
		IDFile classeur = FileUtils::PMStringToSysFile(formeRoot);
		FileUtils::AppendPath(&classeur, classeurName);
		
		InterfacePtr<ICoreFilename> coreFile (::CreateObject2<ICoreFilename>(kCoreFilenameBoss));		

		// Delete description file
		IDFile descFile = classeur;
		FileUtils::AppendPath(&descFile, descDir);
		FileUtils::AppendPath(&descFile, name+".xml");

		coreFile->Initialize(&descFile);
		coreFile->Delete();

		// Delete Matching file
		IDFile formeMatchingFile = classeur;
		FileUtils::AppendPath(&formeMatchingFile, typoDir);
		FileUtils::AppendPath(&formeMatchingFile, name+".xml");

		coreFile->Initialize(&formeMatchingFile);
		coreFile->Delete();

		// Delete Snippet file
		IDFile snippetFile = classeur;
		FileUtils::AppendPath(&snippetFile, formeDir);
		FileUtils::AppendPath(&snippetFile, name+".inds");

		coreFile->Initialize(&snippetFile);
		coreFile->Delete();

		// Delete Vignette file
		IDFile vignetteFile = classeur;
		FileUtils::AppendPath(&vignetteFile, vignetteDir);
		FileUtils::AppendPath(&vignetteFile, name+".jpg");

		coreFile->Initialize(&vignetteFile);
		coreFile->Delete();

		success = kSuccess;

	} while(kFalse);

	return success;
}

ErrorCode XPGFileSystemFormeModel::CreateClasseur(const PMString& classeurName)
{
	ErrorCode status = kSuccess;

	// Create classeur
	IDFile classeur = FileUtils::PMStringToSysFile(formeRoot);
	FileUtils::AppendPath(&classeur, classeurName);
	if(FileUtils::DoesFileExist(classeur))
		status = kFailure;
	else
		FileUtils::CreateFolderIfNeeded(classeur);
	
	// Create classeur subfolders
	IDFile typoFolder = classeur;
	FileUtils::AppendPath(&typoFolder, typoDir);
	FileUtils::CreateFolderIfNeeded(typoFolder);

	IDFile formeFolder = classeur;
	FileUtils::AppendPath(&formeFolder, formeDir);
	FileUtils::CreateFolderIfNeeded(formeFolder);
		
	IDFile descFolder = classeur;
	FileUtils::AppendPath(&descFolder, descDir);
	FileUtils::CreateFolderIfNeeded(descFolder);

	IDFile vignetteFolder = classeur;
	FileUtils::AppendPath(&vignetteFolder, vignetteDir);
	FileUtils::CreateFolderIfNeeded(vignetteFolder);

	
	return status;
}

ErrorCode XPGFileSystemFormeModel::DeleteClasseur(const PMString& classeurName)
{
	ErrorCode status = kSuccess;

	IDFile classeur = FileUtils::PMStringToSysFile(formeRoot);
	FileUtils::AppendPath(&classeur, classeurName);

	// Use OS specific function here : ICoreFilename::Delete doesn't work
#if WINDOWS
	if(!Utils<IXPageUtils>()->DeleteDirectory(FileUtils::SysFileToPMString(classeur).GrabTString()))
#else
	if(!Utils<IXPageUtils>()->DeleteDirectory(FileUtils::SysFileToPMString(classeur).GetPlatformString().c_str()))
#endif
		status = kFailure;

	return status;
}

bool16 XPGFileSystemFormeModel::DoesClasseurExist(const PMString& classeurName)
 {
	 if(classeurName == kNullString)
		return kFalse;

	 IDFile classeur = FileUtils::PMStringToSysFile(formeRoot);
	 FileUtils::AppendPath(&classeur, classeurName);
	 return FileUtils::DoesFileExist(classeur);		
 }

K2Vector<PMString> XPGFileSystemFormeModel::GetFormeList(const PMString& classeurName)
 {
	 K2Vector<PMString> formeList;

	 IDFile classeur = FileUtils::PMStringToSysFile(formeRoot);
	 FileUtils::AppendPath(&classeur, classeurName);
	 FileUtils::AppendPath(&classeur, descDir);

	 PlatformFolderTraverser folderTraverser(classeur, kFalse, kFalse, kFalse, kTrue);
     IDFile idFile;
	 while (folderTraverser.Next(&idFile))
     {
         PMString extension = kNullString;
		 FileUtils::GetExtension(idFile,extension);
         if(!FileUtils::IsDirectory(idFile) && extension.Compare(kFalse, "xml") == 0)
		 {
			 PMString formeName = idFile.GetFileName();
			 // Remove extension
			 formeName.Remove(formeName.NumUTF16TextChars()-4,4);
			 formeList.push_back(formeName);
		 }

	 }
	 return formeList;
 }

bool16 XPGFileSystemFormeModel::IsPhotoCarton(const PMString& formeName, const PMString& classeurName)
 {
	if (formeName.IsEmpty() || classeurName.IsEmpty())
		return kFalse;

	IDFile descFile = FileUtils::PMStringToSysFile(formeRoot);
	FileUtils::AppendPath(&descFile, classeurName);
	FileUtils::AppendPath(&descFile, descDir);
	FileUtils::AppendPath(&descFile, formeName + ".xml");

	if (!FileUtils::DoesFileExist(descFile))
		return kFalse;

	PMString name, classeur, posX, posY;
	IDFile formeFile, matchingFile;
	bool16 isPhoto = kFalse;
	if (parseFormeDescriptif(descFile, name, classeur, formeFile, matchingFile, posX, posY, isPhoto) != kSuccess)
		return kFalse;
	return isPhoto;
 }

bool16 XPGFileSystemFormeModel::GetForme(const PMString& name, const PMString& classeurName,
										 IDFile& formeFile, IDFile& matchingFile, PMString& posX, PMString& posY)
 {
	bool16 found = kFalse;
	IDFile formeDesc = FileUtils::PMStringToSysFile(formeRoot);
	FileUtils::AppendPath(&formeDesc, classeurName);
	FileUtils::AppendPath(&formeDesc, descDir);
	FileUtils::AppendPath(&formeDesc, name + ".xml");

	PMString formeName = kNullString, dummy = kNullString;
	bool16 dummyIsPhotoCarton = kFalse; // not exposed by GetForme — callers use the model's tree node when they need this flag.

	if(parseFormeDescriptif(formeDesc, formeName, dummy, formeFile, matchingFile, posX, posY, dummyIsPhotoCarton) == kSuccess){

		found = kTrue;
	}
	return found;
 }

//	end, File: XPGFileSystemFormeModel.cpp
