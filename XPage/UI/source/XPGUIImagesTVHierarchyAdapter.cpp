
#include "VCPlugInHeaders.h"

// Interface includes:
#include "ITreeViewHierarchyAdapter.h"
#include "IWorkspace.h"
#include "ISession.h"
#include "IXPageMgrAccessor.h"
#include "IWidgetParent.h"
#include "IPanelControlData.h"
#include "IControlView.h"

// General includes:
#include "CPMUnknown.h"
#include "IPMUnknownData.h"
#include "CAlert.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIImageNodeID.h"
#include "XPGImageDataNode.h"
#include "IImageDataModel.h"


/** Implementation of ITreeViewHierarchyAdapter, adapts our tree model to the needs of the 
	tree-view widget. Implements an adapter for the tree hierarchy; this is where the hierarchy 
	is accessed by the application core - you'll find that this is the only implementation 
	you provide with methods like GetNumChildren or GetParent.

	For further reading on ITreeViewHiearchyAdapter interface, see the Widgets chapter
	of the User Interface technote.




*/

class XPGUIImagesTVHierarchyAdapter : 
	public CPMUnknown<ITreeViewHierarchyAdapter>
{
public:
	/** Constructor
	*/	
	XPGUIImagesTVHierarchyAdapter(IPMUnknown* boss);
	/** Destructor
	*/	
	virtual ~XPGUIImagesTVHierarchyAdapter();	

	/** 
		Acquire reference to root node of our tree. 
		@return reference to root node
	*/	
	virtual NodeID_rv GetRootNode() const;

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
	virtual NodeID_rv GetNthChild(
		const NodeID& node, const int32& nth) const;

	/** Determines the index of given child in the parent's list of children.
		Given the parent and an instance of a child, indicate what the index of
		the given child node should be in the list of children of the parent, if any.
	
		@param parent (in) reference to the parent node
		@param child (in) reference to the child node, whose index we're seeking
		@return index of child in parent's kids, or (-1) if the child doesn't belong to this parent
	*/	
	virtual int32 GetChildIndex(
		const NodeID& parent, const NodeID& child) const;

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
CREATE_PMINTERFACE(XPGUIImagesTVHierarchyAdapter, kXPGUIImagesTVHierarchyAdapterImpl)

/* Constructor
*/
XPGUIImagesTVHierarchyAdapter::XPGUIImagesTVHierarchyAdapter(IPMUnknown* boss) : 
	CPMUnknown<ITreeViewHierarchyAdapter>(boss)
{
}


/* Destructor
*/
XPGUIImagesTVHierarchyAdapter::~XPGUIImagesTVHierarchyAdapter()
{
}


/* GetRootNode
*/
NodeID_rv XPGUIImagesTVHierarchyAdapter::GetRootNode() const
{
	// When we call ITreeViewMgr::ChangeRoot, then this is the first
	// method that gets called
	InterfacePtr<IPMUnknownData> datamodel (this, IID_IDATAMODEL);
	PMString rootID = static_cast<IImageDataModel *>(datamodel->GetPMUnknown())->GetRootID();
	if(rootID.IsEmpty())
		return XPGUIImageNodeID::Create();
	else
	{
		XPGImageDataNode * rootNode = static_cast<IImageDataModel *>(datamodel->GetPMUnknown())->GetNode(rootID);
		return XPGUIImageNodeID::Create(rootID, rootNode->GetData(), rootNode->GetType(), rootNode->GetImageData());
	}
}


/* GetParentNode
*/
NodeID_rv XPGUIImagesTVHierarchyAdapter::GetParentNode(const NodeID& node) const
{
	// The root has no parent: this will be a general result	
	do
	{
		TreeNodePtr<XPGUIImageNodeID> nodeID(node);
		if (nodeID == nil)
		{
			break; 
		}

		PMString id = nodeID->GetID();

		InterfacePtr<IPMUnknownData> datamodel (this, IID_IDATAMODEL);
		
		if(id == static_cast<IImageDataModel *>(datamodel->GetPMUnknown())->GetRootID())
		{
			break;
		}

		const XPGImageDataNode * parentNode = static_cast<IImageDataModel *>(datamodel->GetPMUnknown())->GetParent(id);
		if(parentNode)
			return XPGUIImageNodeID::Create(parentNode->GetID(), parentNode->GetData(), parentNode->GetType(), parentNode->GetImageData());
		else
			return kInvalidNodeID;

	} while(kFalse);

	return kInvalidNodeID;	
}


/*
*/
int32 XPGUIImagesTVHierarchyAdapter::GetNumChildren(const NodeID& node) const
{
	int32 retval=0;
	do
	{
		TreeNodePtr<XPGUIImageNodeID> nodeID(node);
		if (nodeID == nil)
		{
			break;
		}

		InterfacePtr<IPMUnknownData> datamodel (this, IID_IDATAMODEL);
	
		retval = static_cast<IImageDataModel *>(datamodel->GetPMUnknown())->GetChildCount(nodeID->GetID());

	} while(kFalse);
	
	return retval;
}

/* GetNthChild
*/
NodeID_rv XPGUIImagesTVHierarchyAdapter::GetNthChild(
	const NodeID& node, const int32& nth) const
{
	do
	{
		TreeNodePtr<XPGUIImageNodeID> nodeID(node);
		if (nodeID == nil)
		{
			break;
		}

		InterfacePtr<IPMUnknownData> datamodel (this, IID_IDATAMODEL);
	
		const XPGImageDataNode * childNode = static_cast<IImageDataModel *>(datamodel->GetPMUnknown())->GetNthChild(nodeID->GetID(), nth);	
		if(childNode)
			return XPGUIImageNodeID::Create(childNode->GetID(), childNode->GetData(), childNode->GetType(), childNode->GetImageData());
		else
			return kInvalidNodeID;
		
	} while(kFalse);
	return kInvalidNodeID;
}


/* GetChildIndex
*/
int32 XPGUIImagesTVHierarchyAdapter::GetChildIndex(
	const NodeID& parent, const NodeID& child) const
{
	do
	{
		TreeNodePtr<XPGUIImageNodeID> parentFileNodeID(parent);
	
		if(parentFileNodeID==nil)
		{
			break;
		}
		TreeNodePtr<XPGUIImageNodeID> childFileNodeID(child);
		
		if(childFileNodeID==nil)
		{
			break;
		}

		InterfacePtr<IPMUnknownData> datamodel (this, IID_IDATAMODEL);
				
		return static_cast<IImageDataModel *>(datamodel->GetPMUnknown())->GetChildIndexFor(parentFileNodeID->GetID(), childFileNodeID->GetID());			
	} while(kFalse);

	return (-1);
}


/* GetGenericNodeID
*/
NodeID_rv XPGUIImagesTVHierarchyAdapter::GetGenericNodeID() const
{
	return XPGUIImageNodeID::Create();
}

//	end, File: XPGUIImagesTVHierarchyAdapter.cpp
