
#ifndef __XPGUITexteNodeID__
#define __XPGUITexteNodeID__

#include "NodeID.h"
#include "IPMStream.h"
#include "XPGUIID.h"
#include "CAlert.h"
#include "XPGArticleDataNode.h"

/** Represents a node in some notional file system; this could be a platform file
	system, or a file within a repository such as Perforce.
	Overriding this class lets us populate the tree with our own data type that
	represents whatever we want on each node.

*/
class XPGUIArticleNodeID : public NodeIDClass
{
	public:
		enum { kNodeType = kXPGUIArticleViewWidgetBoss }; 


		/**	Factory method to create new object of this class
			@return   new instance of this class 
		 */
		static	NodeID_rv Create() { return new XPGUIArticleNodeID(); }

		/**	Factory method to create new object of this class
			@return  new instance of this class
		 */
		static	NodeID_rv Create(const PMString& id, const PMString& data, XPGArticleDataNode::NodeType type, const XPGArticleDataNode::ArticleData * articleData)  
			{
				return new XPGUIArticleNodeID(id, data, type, articleData); 
			}

		/**	Destructor
		 */
		virtual ~XPGUIArticleNodeID() { if(articleData) delete articleData;}
		

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

		/**	
			Text data accessor
		 */
		const PMString& GetID() const { return fIDData; }

		const PMString& GetData() const { return fData; }

		XPGArticleDataNode::NodeType GetType() const { return type ; }
		
		const XPGArticleDataNode::ArticleData * GetArticleData() { return articleData ; }

	private:
		// Note we're keeping the destructor private to force use of the factory methods
		// Note we're keeping the destructor private to force use of the factory methods
		XPGUIArticleNodeID() : fIDData(kNullString), 
			fData(kNullString),
			type(XPGArticleDataNode::kCorpsNode), 
			articleData(nil) {}

		XPGUIArticleNodeID(PMString id, PMString data, XPGArticleDataNode::NodeType type, const XPGArticleDataNode::ArticleData * articleData);

		PMString fIDData, fData;
		XPGArticleDataNode::NodeType type;
		XPGArticleDataNode::ArticleData * articleData;
};

#endif // __XPGUITexteNodeID__


