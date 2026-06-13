#include "VCPlugInHeaders.h"

// Interface includes:
#include "ITreeViewHierarchyAdapter.h"
#include "IWorkspace.h"
#include "ISession.h"
#include "IArticleDataModel.h"

// General includes:
#include "IPMUnknownData.h"
#include "CPMUnknown.h"
#include "CAlert.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIArticleNodeID.h"


class XPGUIArticleTVHierarchyAdapter : public CPMUnknown<ITreeViewHierarchyAdapter>{

	public:

		XPGUIArticleTVHierarchyAdapter(IPMUnknown* boss);
		virtual ~XPGUIArticleTVHierarchyAdapter();
		//virtual int32 GetNumListItems() const;
		NodeID_rv GetRootNode() const;
		//virtual NodeID_rv GetNthListItem(const int32& nth) const;


	/** Acquire a reference to the parent of the given node.
		GetParentNode() should always return kInvalidNodeID for the root.
		@param node (in) specifies node of interest
		@return reference to parent of given node
	*/	
	virtual NodeID_rv GetParentNode(const NodeID& node) const;

	/** 
		Determine the number of child nodes that the given node has.
		@param node (in) specifies node of interest
		@return count of children for given node
	*/	
	virtual int32 GetNumChildren(const NodeID& node) const;

	/**
		Acquire a reference to the nth child. Note: the indexing is from 0 to (numChildren-1).
		@param node (in) identifies the parent node for which we're seeking the nth child
		@param nth (in) get the child at this index in the list of children of the specified node
		@return reference to the nth child
	*/	
	virtual NodeID_rv GetNthChild(const NodeID& node, const int32& nth) const;

	/** Determines the index of given child in the parent's list of children.
		Given the parent and an instance of a child, indicate what the index of
		the given child node should be in the list of children of the parent, if any.
	
		@param parent (in) reference to the parent node
		@param child (in) reference to the child node, whose index we're seeking
		@return index of child in parent's kids, or (-1) if the child doesn't belong to this parent
	*/	
	virtual int32 GetChildIndex(const NodeID& parent, const NodeID& child) const;

	/** Acquire reference to a general-purpose node.
		This should return an instance of a node that can be used anywhere in the hierarchy.
		@return reference to general purpose node
	*/	
	virtual NodeID_rv GetGenericNodeID() const;	

	virtual bool16  ShouldAddNthChild( const NodeID& node, const int32& nth ) const { return kTrue; }

};	

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGUIArticleTVHierarchyAdapter, kXPGUIArticleTVHierarchyAdapterImpl)


/* Destructor
*/
XPGUIArticleTVHierarchyAdapter::~XPGUIArticleTVHierarchyAdapter(){}


XPGUIArticleTVHierarchyAdapter::XPGUIArticleTVHierarchyAdapter(IPMUnknown* boss):CPMUnknown<ITreeViewHierarchyAdapter>(boss){
}


NodeID_rv XPGUIArticleTVHierarchyAdapter::GetRootNode() const
{
	// When we call ITreeViewMgr::ChangeRoot, then this is the first method that gets called
	InterfacePtr<IPMUnknownData> datamodel (this, IID_IDATAMODEL);
	PMString rootID = static_cast<IArticleDataModel *>(datamodel->GetPMUnknown())->GetRootID();
	if(rootID.IsEmpty())
	{
		return XPGUIArticleNodeID::Create();
	}
	else
	{
		XPGArticleDataNode * rootNode = static_cast<IArticleDataModel *>(datamodel->GetPMUnknown())->GetNode(rootID);
		return XPGUIArticleNodeID::Create(rootID, rootNode->GetData(), rootNode->GetType(), rootNode->GetArticleData());		 
	}

}
/* GetParentNode
*/
NodeID_rv XPGUIArticleTVHierarchyAdapter::GetParentNode(const NodeID& node) const
{
	// The root has no parent: this will be a general result	
	do
	{		
		TreeNodePtr<XPGUIArticleNodeID> nodeID(node);
		if (nodeID == nil)
		{
			break; 
		}
		
		InterfacePtr<IPMUnknownData> datamodel (this, IID_IDATAMODEL);		
		PMString id = nodeID->GetID();
		if(id == static_cast<IArticleDataModel *>(datamodel->GetPMUnknown())->GetRootID())
		{
			break;
		} 

		const XPGArticleDataNode * parentNode = static_cast<IArticleDataModel *>(datamodel->GetPMUnknown())->GetParent(id);
		if(parentNode)
		{
			return XPGUIArticleNodeID::Create(parentNode->GetID(), parentNode->GetData(), parentNode->GetType(), parentNode->GetArticleData());
		}
		else
		{
			return kInvalidNodeID;
		}
	} while(kFalse);
	
	return kInvalidNodeID;	
}


/*
*/
int32 XPGUIArticleTVHierarchyAdapter::GetNumChildren(const NodeID& node) const
{
	int32 retval=0;
	do
	{
		TreeNodePtr<XPGUIArticleNodeID> nodeID(node);
		if (nodeID == nil)
		{
			break;
		}		
		
		InterfacePtr<IPMUnknownData> datamodel (this, IID_IDATAMODEL);
		if (datamodel == nil) {
			break;
		}
		retval = static_cast<IArticleDataModel *>(datamodel->GetPMUnknown())->GetChildCount(nodeID->GetID());
	} while(kFalse);
	
	return retval;
}

/* GetNthChild
*/
NodeID_rv XPGUIArticleTVHierarchyAdapter::GetNthChild(const NodeID& node, const int32& nth) const
{
	do
	{
		TreeNodePtr<XPGUIArticleNodeID> nodeID(node);
		if (nodeID == nil)
		{
			break;
		}
	
		InterfacePtr<IPMUnknownData> datamodel (this, IID_IDATAMODEL);	
		const XPGArticleDataNode * childNode = static_cast<IArticleDataModel *>(datamodel->GetPMUnknown())->GetNthChild(nodeID->GetID(), nth);	
		if(childNode)
			return XPGUIArticleNodeID::Create(childNode->GetID(), childNode->GetData(), childNode->GetType(), childNode->GetArticleData());
		else
			return kInvalidNodeID;
		
	} while(kFalse);
	return kInvalidNodeID;
}


/* GetChildIndex
*/
int32 XPGUIArticleTVHierarchyAdapter::GetChildIndex(const NodeID& parent, const NodeID& child) const
{
    int32 result = -1;
	do
	{		
		TreeNodePtr<XPGUIArticleNodeID> parentFileNodeID(parent);
		ASSERT(parentFileNodeID);
		if(parentFileNodeID==nil)
		{
			break;
		}
		TreeNodePtr<XPGUIArticleNodeID> childFileNodeID(child);
		ASSERT(childFileNodeID);
		if(childFileNodeID==nil)
		{
			break;
		}

		InterfacePtr<IPMUnknownData> datamodel (this, IID_IDATAMODEL);				
		result = static_cast<IArticleDataModel *>(datamodel->GetPMUnknown())->GetChildIndexFor(parentFileNodeID->GetID(), childFileNodeID->GetID());

	} while(kFalse);
	
	return (result);
}


/* GetGenericNodeID
*/
NodeID_rv XPGUIArticleTVHierarchyAdapter::GetGenericNodeID() const
{
	return XPGUIArticleNodeID::Create();
}
