
#include "VCPluginHeaders.h"

// Interfaces includes
#include "IXPGPreferences.h"
#include "IImageDataModel.h"

// Project includes
#include "XPGImageModelBase.h"
#include "XPGImageDataNode.h"
#include "XPGID.h"
#include "FileUtils.h"

/* Constructor
*/
XPGImageModelBase::XPGImageModelBase(IPMUnknown* boss) : 
	CPMUnknown<IImageDataModel>(boss), fRootNode(nil)
{
	InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());

	racineArbo = xpgPrefs->GetRacineArbo();
	
#ifdef MACINTOSH
	imageDir = FileUtils::DecomposeUnicode(imageDir);
#endif
	
}

/* Destructor
*/
XPGImageModelBase::~XPGImageModelBase()
{
	deleteTree();
}

/* GetRootID
*/
PMString XPGImageModelBase::GetRootID() const
{
	if(fRootNode != nil)
	{
		return fRootNode->GetID();
	}
	return PMString();
}

/* deleteTree
*/
void XPGImageModelBase::deleteTree() 
{
	// We've flattened the tree in our hashtable anyway so 
	// use this to delete the nodes.
	std::map<PMString, XPGImageDataNode* >::iterator iter;
	for(iter = fIdNodeMap.begin(); iter != fIdNodeMap.end(); iter++)
	{
		XPGImageDataNode* node = iter->second;
		ASSERT(node);
		delete node;
	}
	fIdNodeMap.clear();
	fRootNode = nil;
}

/* GetParent
*/
const XPGImageDataNode * XPGImageModelBase::GetParent(const PMString& id) const
{
	// We assume that the parent is cached, because we
	// found its kids
	std::map<PMString, XPGImageDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end())
	{
		XPGImageDataNode* node = result->second;
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
int32 XPGImageModelBase::GetChildCount(const PMString& id) // not const any more
{
	std::map<PMString, XPGImageDataNode* >::const_iterator result = fIdNodeMap.find(id);
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
	std::map<PMString, XPGImageDataNode* >::const_iterator postCacheResult = fIdNodeMap.find(id);
	if(postCacheResult != fIdNodeMap.end()) {
		return postCacheResult->second->ChildCount();
	}
	return 0;
}

/* GetNthChild
*/
const XPGImageDataNode * XPGImageModelBase::GetNthChild(
	const PMString& id, int32 nth) // not const any more
{		
	std::map<PMString, XPGImageDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end()) {
		int32 possChildCount = result->second->ChildCount();
		if(possChildCount > 0) { // Again, distrust a node with zero child count, may be kids not yet cached 
			return  &(result->second->GetNthChild(nth));
		}
	}
	// Again, we may not have cached the kids.
	this->cacheChildren(id);
	// Don't recurse, as we may have no kids and we only want one try
	std::map<PMString, XPGImageDataNode* >::const_iterator postCacheResult = fIdNodeMap.find(id);
	if(postCacheResult != fIdNodeMap.end()) {
		return &(postCacheResult->second->GetNthChild(nth));
	}
	
	return nil;
}

/* GetChildIndexFor
*/
int32  XPGImageModelBase::GetChildIndexFor(
	const PMString& par, const PMString& kid) //const
{
	// Can we be called here without the kids being already cached?
	int32 retval=(-1);
	std::map<PMString, XPGImageDataNode* >::const_iterator result = fIdNodeMap.find(par);
	if(result != fIdNodeMap.end()) {
		XPGImageDataNode* node = result->second;
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



XPGImageDataNode* XPGImageModelBase::GetNode(const PMString& id) const{
	XPGImageDataNode* retval = nil;

	std::map<PMString, XPGImageDataNode* >::const_iterator result = fIdNodeMap.find(id);
	if(result != fIdNodeMap.end()) {
		retval = result->second;
	}
	return retval;
}

/* validPath
*/
bool16 XPGImageModelBase::validPath(const PMString& p)
{
	const PMString thisDir(".");
	const PMString parentDir("..");
	return p != thisDir && p != parentDir;
}