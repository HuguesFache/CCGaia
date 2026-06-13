
#include "VCPluginHeaders.h"

// Interfaces includes
#include "IXPGPreferences.h"
#include "IArticleDataModel.h"
#include "IK2ServiceRegistry.h"
#include "ISAXEntityResolver.h"
#include "IK2ServiceProvider.h"
#include "ISAXContentHandler.h"
#include "ISAXParserOptions.h"
#include "ISAXServices.h"
#include "StreamUtil.h"
#include "IXMLStreamUtils.h"
#include "IPointerData.h"
#include "StringUtils.h"
#include "CAlert.h"
#include "XMLDefs.h"

// Project includes
#include "FileUtils.h"
#include "XPGArticleDataNode.h"
#include "XPGID.h"
#include "IWebServices.h"
/**  Class representing a folder hierarchy. 
	@ingroup paneltreeview
*/
class XPGArticleDataModel : CPMUnknown<IArticleDataModel>
{
public:
	/** Default constructor */
	XPGArticleDataModel(IPMUnknown* boss);

	/**	Destructor
	 */
	virtual ~XPGArticleDataModel();

	/**		See IArticleDataModel::GetRootID 
	*/
	virtual PMString GetRootID() const;

	/** 	See IArticleDataModel::GetNode
	*/
	virtual XPGArticleDataNode* GetNode(const PMString& id) const;

	/**		See IArticleDataModel::GetParent
	 */
	virtual const XPGArticleDataNode * GetParent(const PMString& id) const;

	/**		See IArticleDataModel::GetChildCount
	 */
	virtual int32 GetChildCount(const PMString& id); 

	/**		See IArticleDataModel::GetNthChild
	 */
	virtual const XPGArticleDataNode * GetNthChild(const PMString& id, int32 nth); 

	/**		See IArticleDataModel::GetChildIndexFor 
	 */
	virtual int32 GetChildIndexFor(const PMString& par, const PMString& kid); 

	/**		See IArticleDataModel::Rebuild
	 */
	 virtual void Rebuild(const PMString& parutionID, const PMString& rubID, const PMString& ssRubID);

protected:

	/** Cache the kids of the given node in file system
	*/
	void cacheChildren(const PMString& parutionID, const PMString& rubID, const PMString& ssRubID);	
 
	/**	Destroy the tree represented in this 
	 */
	void deleteTree() ;
	
	bool16 validPath(const PMString& p);
	bool16 isArticleXMLFile(const PMString& filename);
private:
	
	std::map<PMString, XPGArticleDataNode* > fIdNodeMap;
	XPGArticleDataNode* fRootNode;
 
	PMString articleDir, matchingFileDir, defaultMatchingName, racineArbo;
	PMString statusFilter;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGArticleDataModel, kXPGArticleDataModelImpl)


/* Constructor
*/
XPGArticleDataModel::XPGArticleDataModel(IPMUnknown* boss) : 
	CPMUnknown<IArticleDataModel>(boss), fRootNode(nil)
{
	InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());

	racineArbo = xpgPrefs->GetRacineArbo();
}


/* Destructor
*/
XPGArticleDataModel::~XPGArticleDataModel()
{
	deleteTree();
}

/* deleteTree
*/
void XPGArticleDataModel::deleteTree() 
{
	// We've flattened the tree in our hashtable anyway so 
	// use this to delete the nodes.
	std::map<PMString, XPGArticleDataNode* >::iterator iter;
	for(iter = fIdNodeMap.begin(); iter != fIdNodeMap.end(); iter++)
	{
		XPGArticleDataNode* node = iter->second;
		ASSERT(node);
		delete node;
	}
	fIdNodeMap.clear();
	fRootNode = nil;
}

/* Rebuild
*/
void XPGArticleDataModel::Rebuild(const PMString& parutionID, const PMString& rubID, const PMString& ssRubID)
{
	if(fRootNode != nil || !fIdNodeMap.empty()) 
	{		
		deleteTree();
	} 
 
	fRootNode =  new XPGArticleDataNode(XPGArticleDataNode::kCorpsNode);
		
	fRootNode->SetID(parutionID);				 
	fRootNode->SetParent(nil);
	fRootNode->SetData(parutionID);
	fIdNodeMap.insert( std::pair<PMString, XPGArticleDataNode* >(parutionID, fRootNode));
    this->cacheChildren(parutionID, rubID, ssRubID);
}

/* GetParent
*/
const XPGArticleDataNode * XPGArticleDataModel::GetParent(const PMString& id) const
{
	// We assume that the parent is cached, because we
	// found its kids
	std::map<PMString, XPGArticleDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end())
	{
		XPGArticleDataNode* node = result->second;
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
int32 XPGArticleDataModel::GetChildCount(const PMString& id) // not const any more
{
	std::map<PMString, XPGArticleDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end()) {
		int32 possChildCount = result->second->ChildCount();
		if(possChildCount > 0) {
			// Don't trust a zero value, it may just be we didn't yet cache the kids
			return possChildCount;
		}
	}
	// So, we may not have cached the kids.
	//this->cacheChildren(id);
	// Don't recurse, as we may have no kids and we only want one try
	std::map<PMString, XPGArticleDataNode* >::const_iterator postCacheResult = fIdNodeMap.find(id);
	if(postCacheResult != fIdNodeMap.end()) {
		return postCacheResult->second->ChildCount();
	}
	return 0;
}

/* GetNthChild
*/
const XPGArticleDataNode * XPGArticleDataModel::GetNthChild(const PMString& id, int32 nth) // not const any more
{
	std::map<PMString, XPGArticleDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end()) {
		int32 possChildCount = result->second->ChildCount();
		if(possChildCount > 0) {
			// Again, distrust a node with zero child count, may be kids not yet cached 
			return &(result->second->GetNthChild(nth));
		}
	}
	// Again, we may not have cached the kids.
	//this->cacheChildren(id);
	// Don't recurse, as we may have no kids and we only want one try
	std::map<PMString, XPGArticleDataNode* >::const_iterator postCacheResult = fIdNodeMap.find(id);
	if(postCacheResult != fIdNodeMap.end()) {
		return &(postCacheResult->second->GetNthChild(nth));
	}

	return nil;
}

/* GetChildIndexFor
*/
int32  XPGArticleDataModel::GetChildIndexFor(const PMString& par, const PMString& kid) //const
{
	// Can we be called here without the kids being already cached?
	int32 retval=(-1);
	std::map<PMString, XPGArticleDataNode* >::const_iterator result = fIdNodeMap.find(par);
	if(result != fIdNodeMap.end()) {
		XPGArticleDataNode* node = result->second;
		if(node->ChildCount() == 0) {
			// Try to cache the kids, we may not have done so
			//this->cacheChildren(par);
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
PMString XPGArticleDataModel::GetRootID() const
{
	if(fRootNode != nil)
	{
		return fRootNode->GetID();
	}
	return PMString();
}

XPGArticleDataNode* XPGArticleDataModel::GetNode(const PMString& id) const
{
	XPGArticleDataNode* retval = nil;

	std::map<PMString, XPGArticleDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end()) {
		retval = result->second;
	}
	return retval;
}


/* validPath
*/
bool16 XPGArticleDataModel::validPath(const PMString& p)
{
	const PMString thisDir(".");
	const PMString parentDir("..");
	return p != thisDir && p != parentDir;
}

void XPGArticleDataModel::cacheChildren(const PMString& parentId, const PMString& rubID, const PMString& ssRubID)
{
	do{	
		XPGArticleDataNode* parentNode = nil;
        std::map<PMString, XPGArticleDataNode* >::const_iterator iterForParent = fIdNodeMap.find(parentId);
        if(iterForParent == fIdNodeMap.end()) 
		{
            break;
        }
        parentNode = iterForParent->second;
		ASSERT(parentNode);
		
		InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());	
		InterfacePtr<IWebServices> baseHTTP (::CreateObject2<IWebServices>(kXRCXRailClientBoss));

		// on recupere la liste des articles
		int32 idPage = Utils<IXPageUtils>()->GetPageId();
		if (idPage == -1)
			break;
		K2Vector<PMString> IDStoriesMEP, ListSujetsMEP, CheminArticlesMEP, CouleurStatusMEP, LibStatusMEP;		
		if(!baseHTTP->GetStories_v2(xpgPrefs->GetTEC_URL(), parentId.GetAsNumber(), rubID, ssRubID, IDStoriesMEP, ListSujetsMEP,
											CheminArticlesMEP, CouleurStatusMEP, LibStatusMEP))
		{
			CAlert::InformationAlert("Impossible de récupérer la liste des articles");
			break;
		}

		for(int32 i = 0; i< IDStoriesMEP.size(); i++)
		{
			std::map<PMString, XPGArticleDataNode* >::iterator existingIter = fIdNodeMap.find(IDStoriesMEP[i]);
			if(existingIter == fIdNodeMap.end()) 
			{        
				XPGArticleDataNode * newNode =  new XPGArticleDataNode(XPGArticleDataNode::kCorpsNode);				 			
				newNode->SetID(IDStoriesMEP[i]);
				newNode->SetParent(parentNode);

				PMString storyPath = CheminArticlesMEP[i];
				storyPath.Append(IDStoriesMEP[i]);
				storyPath.Append(articleSuffix);			
				
				PMString storySnippet = CheminArticlesMEP[i];
				storySnippet.Append(IDStoriesMEP[i]);
				storySnippet.Append(".inds");	
				
				//on va maintenant lire le xml pour recuperer les infos manquantes
                const IDFile artXML = FileUtils::PMStringToSysFile(storyPath);
				//si le fichier existe, on fait le traitement, sinon, on ne l'ajoute meme pas a la liste
				if(FileUtils::DoesFileExist(artXML)) {

					// Parse xml file to get related text data
					InterfacePtr<IK2ServiceRegistry> serviceRegistry(GetExecutionContextSession(), UseDefaultIID()); 
					InterfacePtr<IK2ServiceProvider> xmlProvider(serviceRegistry->QueryServiceProviderByClassID(kXMLParserService, kXMLParserServiceBoss)); 
		
					InterfacePtr<ISAXServices> saxServices(xmlProvider, UseDefaultIID()); 

					InterfacePtr<ISAXContentHandler> contentHandler( ::CreateObject2<ISAXContentHandler>(kXPGXMLParserArticleBoss,  ISAXContentHandler::kDefaultIID)); 
					InterfacePtr<IPointerData> nodeData (contentHandler, IID_INODEDATA); // Pass node to the handler which will deal with text data initialization from xml data
					nodeData->Set(newNode);
		
					contentHandler->Register(saxServices);
					InterfacePtr<ISAXEntityResolver> entityResolver(::CreateObject2<ISAXEntityResolver>(kXMLEntityResolverBoss, ISAXEntityResolver::kDefaultIID));
					InterfacePtr<ISAXParserOptions> parserOptions(saxServices, UseDefaultIID());
		
					// These commitments: follow 
					// the existing ImportXMLFileCmd
					parserOptions->SetNamespacesFeature(kFalse);
					parserOptions->SetShowWarningAlert(kFalse);	
		
					InterfacePtr<IPMStream> xmlFileStream(StreamUtil::CreateFileStreamRead(artXML));
					saxServices->ParseStream(xmlFileStream, contentHandler, entityResolver);	

					xmlFileStream->Close();

					if(ErrorUtils::PMGetGlobalErrorCode() == kSuccess){
						
						K2Vector<PMString> images;
						PMString empty;
						newNode->SetArticleData(CheminArticlesMEP[i], IDStoriesMEP[i],
										   ListSujetsMEP[i], storyPath, storySnippet, CouleurStatusMEP[i], 
										   LibStatusMEP[i], empty, empty, empty);
				
						fRootNode->AddChild(static_cast<const XPGArticleDataNode& >(*newNode));
						fIdNodeMap.insert(std::pair<PMString, XPGArticleDataNode* >(IDStoriesMEP[i], newNode));
					}
                    else {
                        PMString errorString = PMString("Impossible de lire le fichier ");
                        errorString.Append(storyPath);
                        CAlert::InformationAlert(errorString);
                    }
				}
			}
		}

	} while(kFalse);	
}

//	end, File: XPGXRailTextModel.cpp
