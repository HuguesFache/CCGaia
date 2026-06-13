

#ifndef __XPGUIFormeNodeID__
#define __XPGUIFormeNodeID__

#include "NodeID.h"
#include "IPMStream.h"
#include "XPGUIID.h"

#include "XPGFormeDataNode.h"

/** Represents a node in some notional file system; this could be a platform file
	system, or a file within a repository such as Perforce.
	Overriding this class lets us populate the tree with our own data type that
	represents whatever we want on each node.

	
	@ingroup paneltreeview
*/
class XPGUIFormeNodeID : public NodeIDClass
{
	public:

		enum { kNodeType = kXPGUIFormesViewWidgetBoss }; 


		/**	Factory method to create 
		new object of this class
			@return   new instance of this class 
		 */
		static	NodeID_rv Create() { return new XPGUIFormeNodeID(); }

		/**	Factory method to create new object of this class
			@return  new instance of this class
		 */
		static	NodeID_rv Create(const PMString& id, const PMString& data, XPGFormeDataNode::NodeType type, const XPGFormeDataNode::FormeData * formeData)  
			{ return new XPGUIFormeNodeID(id, data, type, formeData); }

		/**	Destructor
		 */
		virtual ~XPGUIFormeNodeID() { if(formeData) delete formeData; }
		

		/**	@return type of this node
		 */
		virtual	NodeType GetNodeType() const { return kNodeType; } 

		/**	Comparator function
			@param NodeID [IN] specifies the node that we should compare against
			@return  Comparison results
		 */
		virtual int32 Compare(const NodeIDClass* NodeID) const;

		/**	Create a copy of this
			@return  
		 */
		virtual NodeIDClass* Clone() const;

		/**	Read this from the given stream
			@param stream [IN] specified stream
			@return  
		 */
		virtual void Read(IPMStream* stream);

		/**	Write this to the given stream
			@param [OUT] stream 
			@return  
		 */
		virtual void Write(IPMStream* stream) const;

		/**	Image data accessor
		 */
		const PMString& GetID() const { return fIDData; }

		const PMString& GetData() const { return fData; }

		XPGFormeDataNode::NodeType GetType() const { return type ; }
		
		const XPGFormeDataNode::FormeData * GetFormeData()  { return formeData ; }

	private:
		// Note we're keeping the destructor private to force use of the factory methods
		XPGUIFormeNodeID() : fIDData(kNullString), fData(kNullString), type(XPGFormeDataNode::kClasseurNode), formeData(nil) {}

		XPGUIFormeNodeID(PMString id, PMString data, XPGFormeDataNode::NodeType nodetype, const XPGFormeDataNode::FormeData * formeData);

		PMString	fIDData, fData;
		XPGFormeDataNode::NodeType type;
		XPGFormeDataNode::FormeData * formeData;
};

#endif // __XPGUIFormeNodeID__


