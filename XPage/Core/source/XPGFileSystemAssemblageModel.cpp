
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
#include "PlatformFolderTraverser.h"

// Impl includes
#include <map>

// Project includes
#include "UIDList.h"
#include "IAssemblageDataModel.h"
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
class XPGFileSystemAssemblageModel : CPMUnknown<IAssemblageDataModel>
{
public:
	/** Default constructor */
	XPGFileSystemAssemblageModel(IPMUnknown* boss);

	/**	Destructor
	 */
	virtual ~XPGFileSystemAssemblageModel();

	/**		See IAssemblageDataModel::GetRootID 
	*/
	virtual PMString GetRootID() const;

	/** 	See IAssemblageDataModel::GetNode
	*/
	virtual XPGFormeDataNode* GetNode(const PMString& id) const;


	/**		See IAssemblageDataModel::GetParent
	 */
	virtual const XPGFormeDataNode * GetParent(const PMString& id) const;

	/**		See IAssemblageDataModel::GetChildCount
	 */
	virtual int32 GetChildCount(const PMString& id); 

	/**		See IAssemblageDataModel::GetNthChild
	 */
	virtual const XPGFormeDataNode * GetNthChild(const PMString& id, int32 nth); 

	/**		See IAssemblageDataModel::GetChildIndexFor 
	 */
	virtual int32 GetChildIndexFor(const PMString& par, const PMString& kid); 

	/**	See IAssemblageDataModel::Rebuild
	 */
	virtual void Rebuild(const PMString& classeurName);

	/** See IAssemblageDataModel::GetClasseurList
	*/
	virtual K2Vector<PMString>  GetClasseurList();

	/** See IAssemblageDataModel::DoesAssemblageExist
	*/
	virtual bool16 DoesAssemblageExist(const PMString& assemblageName, const PMString& classeurName) const;

	/** See IAssemblageDataModel::CreateAssemblage
	*/
	virtual ErrorCode CreateAssemblage(const PMString& name, const PMString& classeurName, const UIDList& assemblageItemsToExport);

	/** See IAssemblageDataModel::DeleteAssemblage
	*/
	virtual ErrorCode DeleteAssemblage(const PMString& name, const PMString& classeurName);
	virtual IDFile GetAssemblageRoot();

	/** See IAssemblageDataModel::CreateClasseur
	*/
	virtual ErrorCode CreateClasseur(const PMString& classeurName);

	/** See IAssemblageDataModel::DeleteClasseur
	*/
	virtual ErrorCode DeleteClasseur(const PMString& classeurName);

	/** See IAssemblageDataModel::DoesClasseurExist
	*/
	virtual bool16 DoesClasseurExist(const PMString& classeurName);

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
	
	std::map<PMString, XPGFormeDataNode* > fIdNodeMap;
	XPGFormeDataNode* fRootNode;

	PMString assemblageRoot;
	PMString assemblageDir, vignetteDir;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGFileSystemAssemblageModel, kXPGFileSystemAssemblageModelImpl)


/* Constructor
*/
XPGFileSystemAssemblageModel::XPGFileSystemAssemblageModel(IPMUnknown* boss) : 
	CPMUnknown<IAssemblageDataModel>(boss),
	fRootNode(nil){
	
	InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
	assemblageRoot = xpgPrefs->GetCheminAssemblageCartons();
	
	assemblageDir = "Forme";
	vignetteDir = "Vignette";

}
	
/* Destructor
*/
XPGFileSystemAssemblageModel::~XPGFileSystemAssemblageModel()
{
	deleteTree();
}


/* deleteTree
*/
void XPGFileSystemAssemblageModel::deleteTree() 
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
void XPGFileSystemAssemblageModel::Rebuild(const PMString& classeurName)
{
	if(fRootNode != nil || !fIdNodeMap.empty() ) {		
		deleteTree();
	} 
	
	if(classeurName != kNullString)
	{
		IDFile assemblageRootFile = FileUtils::PMStringToSysFile(assemblageRoot);
		if(!FileUtils::DoesFileExist(assemblageRootFile))
			return;		

		FileUtils::AppendPath(&assemblageRootFile, classeurName);
		PMString classeurFilePath = FileUtils::SysFileToPMString(assemblageRootFile);

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
const XPGFormeDataNode * XPGFileSystemAssemblageModel::GetParent(const PMString& id) const
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
int32 XPGFileSystemAssemblageModel::GetChildCount(const PMString& id) // not const any more
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
const XPGFormeDataNode * XPGFileSystemAssemblageModel::GetNthChild(
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
int32  XPGFileSystemAssemblageModel::GetChildIndexFor(
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
PMString XPGFileSystemAssemblageModel::GetRootID() const
{
	if(fRootNode != nil)
	{
		return fRootNode->GetID();
	}
	return PMString();
}

XPGFormeDataNode* XPGFileSystemAssemblageModel::GetNode(const PMString& id) const
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
bool16 XPGFileSystemAssemblageModel::ValidPath(const PMString& p)
{
	const PMString thisDir(".");
	const PMString parentDir("..");
	return p != thisDir && p != parentDir;
}

/* cacheChildren
*/
void XPGFileSystemAssemblageModel::cacheChildren(const PMString& parentId)
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
            IDFile snippetFile = parIDFile;
			FileUtils::AppendPath(&snippetFile, assemblageDir);
            PlatformFolderTraverser folderTraverser(snippetFile, kFalse, kFalse, kFalse, kTrue);
            IDFile idFile;
			while (folderTraverser.Next(&idFile))
            {
                
                PMString assemblageFilePath = FileUtils::SysFileToPMString(idFile);
                PMString extension;
                FileUtils::GetExtension(idFile, extension);
                bool isDir = FileUtils::IsDirectory(idFile);
                
				if(!isDir && extension.Compare(kFalse, "inds") == 0)
				{														
					// Check whether an item with the same id is there already...if so, don't add current item
					std::map<PMString, XPGFormeDataNode* >::iterator existingIter = fIdNodeMap.find(assemblageFilePath);					
					if(existingIter == fIdNodeMap.end()) 
					{	
						
						XPGFormeDataNode* newNode = new XPGFormeDataNode(XPGFormeDataNode::kFormeNode);

						PMString assemblageName = idFile.GetFileName();
						CharCounter pos = assemblageName.LastIndexOfCharacter(PlatformChar('.'));
						if(pos > -1)
							assemblageName.Remove(pos, assemblageName.NumUTF16TextChars() - pos);

						newNode->SetID(assemblageFilePath);
						newNode->SetData(assemblageFilePath);
						newNode->SetParent(parentNode);
						newNode->SetFormeData(assemblageName,parIDFile.GetFileName(), IDFile(), idFile, kNullString, kNullString);
						parentNode->AddChild(static_cast<const XPGFormeDataNode& >(*newNode));					
						// Add node to the map 	
						fIdNodeMap.insert( std::pair<PMString, XPGFormeDataNode* > (assemblageFilePath, newNode));						
					}													
				}

			}
		}
		
	} while(kFalse);

}

/* GetClasseurList
*/
K2Vector<PMString> XPGFileSystemAssemblageModel::GetClasseurList()
{	
	K2Vector<KeyValuePair<PMString, PMString> > subFolderList = Utils<IXPageUtils>()->GetSubDirectories(assemblageRoot);
	
	K2Vector<PMString> classeurList;
	for(int32 i = 0 ; i < subFolderList.size() ; ++i)
	{
		PMString classeurName(subFolderList[i].Value());
		classeurName.SetTranslatable(kFalse);
		classeurList.push_back(classeurName);
	}	
	return classeurList;
}

bool16 XPGFileSystemAssemblageModel::DoesAssemblageExist(const PMString& assemblageName, const PMString& classeurName) const
{
	if(assemblageName == kNullString || classeurName == kNullString)
		return kFalse;

	IDFile assemblageFile = FileUtils::PMStringToSysFile(assemblageRoot);
	FileUtils::AppendPath(&assemblageFile, classeurName);
	FileUtils::AppendPath(&assemblageFile, assemblageName+".inds");

	return FileUtils::DoesFileExist(assemblageFile);
}

ErrorCode XPGFileSystemAssemblageModel::CreateAssemblage(const PMString& name, const PMString& classeurName, const UIDList& assemblageItemsToExport)
{

	ErrorCode success = kFailure;
	do {
			
		IDFile classeur = FileUtils::PMStringToSysFile(assemblageRoot);
		FileUtils::AppendPath(&classeur, classeurName);

		// Create the snippet file		
		IDFile snippetFile = classeur;
		FileUtils::AppendPath(&snippetFile, assemblageDir);
		FileUtils::CreateFolderIfNeeded(snippetFile);
		FileUtils::AppendPath(&snippetFile, name+".inds");

		InterfacePtr<IPMStream> snippetStream (StreamUtil::CreateFileStreamWrite(snippetFile, kOpenOut|kOpenTrunc, 'ICx4', 'InCp'));
		if(Utils<ISnippetExport>()->ExportPageitems(snippetStream, assemblageItemsToExport)!= kSuccess){	
			snippetStream->Close();
			break;
		}				
		snippetStream->Close();
	
		// Create the snapshot
		IDFile assemblageVignette = classeur;
		FileUtils::AppendPath(&assemblageVignette, vignetteDir);
		FileUtils::CreateFolderIfNeeded(assemblageVignette);
		FileUtils::AppendPath(&assemblageVignette, name+".jpg");
		
		// Create the snapshot for the website
		if(Utils<IXPageUtils>()->DoSnapShot(assemblageItemsToExport, assemblageVignette) != kSuccess)
			break;

		success = kSuccess;
	}while(kFalse);

	return success;
}

ErrorCode XPGFileSystemAssemblageModel::DeleteAssemblage(const PMString& name, const PMString& classeurName)
{
	ErrorCode success = kFailure;
	do
	{
		IDFile classeur = FileUtils::PMStringToSysFile(assemblageRoot);
		FileUtils::AppendPath(&classeur, classeurName);
		
		InterfacePtr<ICoreFilename> coreFile (::CreateObject2<ICoreFilename>(kCoreFilenameBoss));		

		// Delete Snippet file
		IDFile snippetFile = classeur;
		FileUtils::AppendPath(&snippetFile, assemblageDir);
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

ErrorCode XPGFileSystemAssemblageModel::CreateClasseur(const PMString& classeurName)
{
	ErrorCode status = kSuccess;

	// Create classeur
	IDFile classeur = FileUtils::PMStringToSysFile(assemblageRoot);
	FileUtils::AppendPath(&classeur, classeurName);
	if(FileUtils::DoesFileExist(classeur))
		status = kFailure;
	else
		FileUtils::CreateFolderIfNeeded(classeur);

	// Create snippet subfolders
	IDFile snippetFolder = classeur;
	FileUtils::AppendPath(&snippetFolder, assemblageDir);
	FileUtils::CreateFolderIfNeeded(snippetFolder);

	// Create vignette subfolders
	IDFile vignetteFolder = classeur;
	FileUtils::AppendPath(&vignetteFolder, vignetteDir);
	FileUtils::CreateFolderIfNeeded(vignetteFolder);

	return status;
}

ErrorCode XPGFileSystemAssemblageModel::DeleteClasseur(const PMString& classeurName)
{
	ErrorCode status = kSuccess;

	IDFile classeur = FileUtils::PMStringToSysFile(assemblageRoot);
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

bool16 XPGFileSystemAssemblageModel::DoesClasseurExist(const PMString& classeurName)
 {
	 if(classeurName == kNullString)
		return kFalse;

	 IDFile classeur = FileUtils::PMStringToSysFile(assemblageRoot);
	 FileUtils::AppendPath(&classeur, classeurName);
	 return FileUtils::DoesFileExist(classeur);		
 }

IDFile XPGFileSystemAssemblageModel::GetAssemblageRoot() {
	return FileUtils::PMStringToSysFile(assemblageRoot);;
}
//	end, File: XPGFileSystemAssemblageModel.cpp


