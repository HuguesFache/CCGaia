
#include "VCPlugInHeaders.h"

// Interface includes:
#include "ITreeViewHierarchyAdapter.h"
#include "IWorkspace.h"
#include "ISession.h"
#include "IFormeDataModel.h"
#include "IXPageMgrAccessor.h"
#include "IXPageUtils.h"

// General includes:
#include "CPMUnknown.h"
#include "CAlert.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIFormeNodeID.h"


/** Implementation of ITreeViewHierarchyAdapter, adapts our tree model to the needs of the 
	tree-view widget. Implements an adapter for the tree hierarchy; this is where the hierarchy 
	is accessed by the application core - you'll find that this is the only implementation 
	you provide with methods like GetNumChildren or GetParent.

	For further reading on ITreeViewHiearchyAdapter interface, see the Widgets chapter
	of the User Interface technote.

	
	@ingroup paneltreeview

*/

class XPGUIFormesTVHierarchyAdapter : 
	public CPMUnknown<ITreeViewHierarchyAdapter>
{
public:
	/** Constructor
	*/	
	XPGUIFormesTVHierarchyAdapter(IPMUnknown* boss);

	/** Destructor
	*/	
	virtual ~XPGUIFormesTVHierarchyAdapter();
	

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
private :

	IFormeDataModel * model; // reference to data model boss
};	

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGUIFormesTVHierarchyAdapter, kXPGUIFormesTVHierarchyAdapterImpl)

/* Constructor
*/
XPGUIFormesTVHierarchyAdapter::XPGUIFormesTVHierarchyAdapter(IPMUnknown* boss) : 
	CPMUnknown<ITreeViewHierarchyAdapter>(boss)
{
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
	model = pageMrgAccessor->QueryFormeDataModel();
}


/* Destructor
*/
XPGUIFormesTVHierarchyAdapter::~XPGUIFormesTVHierarchyAdapter()
{
	if(model)
		model->Release();
}


/* GetRootNode
*/
NodeID_rv XPGUIFormesTVHierarchyAdapter::GetRootNode() const
{
	// When we call ITreeViewMgr::ChangeRoot, then this is the first
	// method that gets called
	PMString rootID = model->GetRootID();
	if(rootID.IsEmpty())
		return XPGUIFormeNodeID::Create();
	else
	{
		XPGFormeDataNode * rootNode = model->GetNode(rootID);		
		return XPGUIFormeNodeID::Create(rootID, rootNode->GetData(), rootNode->GetType(), rootNode->GetFormeData());
	}
}


/* GetParentNode
*/
NodeID_rv XPGUIFormesTVHierarchyAdapter::GetParentNode(const NodeID& node) const
{
	// The root has no parent: this will be a general result	
	do
	{		
		TreeNodePtr<XPGUIFormeNodeID> nodeID(node);
		if (nodeID == nil)
		{
			break; 
		}
		
		PMString id = nodeID->GetID();
		if(id == model->GetRootID())
		{
			break;
		}		

		const XPGFormeDataNode * parentNode = model->GetParent(id);
		if(parentNode)
			return XPGUIFormeNodeID::Create(parentNode->GetID(), parentNode->GetData(), parentNode->GetType(), parentNode->GetFormeData());
		else
			return kInvalidNodeID;

	} while(kFalse);

	return kInvalidNodeID;	
}


/*
*/
int32 XPGUIFormesTVHierarchyAdapter::GetNumChildren(const NodeID& node) const
{
	int32 retval=0;
	do
	{
		TreeNodePtr<XPGUIFormeNodeID> nodeID(node);
		if (nodeID == nil)
		{
			break;
		}

		
		retval = model->GetChildCount(nodeID->GetID());
		
	} while(kFalse);
	
	return retval;
}

/* GetNthChild
*/
NodeID_rv XPGUIFormesTVHierarchyAdapter::GetNthChild(
	const NodeID& node, const int32& nth) const
{
	do
	{
		TreeNodePtr<XPGUIFormeNodeID> nodeID(node);
		if (nodeID == nil)
		{
			break;
		}
	
		const XPGFormeDataNode * childNode = model->GetNthChild(nodeID->GetID(), nth);	
		if(childNode)
			return XPGUIFormeNodeID::Create(childNode->GetID(), childNode->GetData(), childNode->GetType(), childNode->GetFormeData());
		else
			return kInvalidNodeID;
		
	} while(kFalse);
	return kInvalidNodeID;
}


/* GetChildIndex
*/
int32 XPGUIFormesTVHierarchyAdapter::GetChildIndex(
	const NodeID& parent, const NodeID& child) const
{
    int32 result = -1;
	do
	{		
		TreeNodePtr<XPGUIFormeNodeID> parentFileNodeID(parent);
		ASSERT(parentFileNodeID);
		if(parentFileNodeID==nil)
		{
			break;
		}
		TreeNodePtr<XPGUIFormeNodeID> childFileNodeID(child);
		ASSERT(childFileNodeID);
		if(childFileNodeID==nil)
		{
			break;
		}

		result = model->GetChildIndexFor(parentFileNodeID->GetID(), childFileNodeID->GetID());

	} while(kFalse);

	return (result);
}


/* GetGenericNodeID
*/
NodeID_rv XPGUIFormesTVHierarchyAdapter::GetGenericNodeID() const
{
	return XPGUIFormeNodeID::Create();
}

//	end, File: XPGUIFormesTVHierarchyAdapter.cpp
