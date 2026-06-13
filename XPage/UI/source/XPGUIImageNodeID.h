

#ifndef __XPGUIImageNodeID__
#define __XPGUIImageNodeID__

#include "NodeID.h"
#include "IPMStream.h"
#include "XPGUIID.h"

#include "XPGImageDataNode.h"

/** Represents a node in some notional file system; this could be a platform file
	system, or a file within a repository such as Perforce.
	Overriding this class lets us populate the tree with our own data type that
	represents whatever we want on each node.

	
	@ingroup paneltreeview
*/
class XPGUIImageNodeID : public NodeIDClass
{
	public:
		enum { kNodeType = kXPGUIImagesViewWidgetBoss }; 


		/**	Factory method to create 
		new object of this class
			@return   new instance of this class 
		 */
		static	NodeID_rv Create() { return new XPGUIImageNodeID(); }

		/**	Factory method to create new object of this class
			@return  new instance of this class
		 */
		static	NodeID_rv Create(const PMString& id, const PMString& data, XPGImageDataNode::NodeType type, const XPGImageDataNode::ImageData * imageData)  
			{ return new XPGUIImageNodeID(id, data, type, imageData); }

		/**	Destructor
		 */
		virtual ~XPGUIImageNodeID() { if(imageData) delete imageData; }
		

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

		XPGImageDataNode::NodeType GetType() const { return type ; }
		
		const XPGImageDataNode::ImageData * GetImageData()  { return imageData ; }

		/** Update the cached etatImage id of this displayed node so the row
			repaints with the new state colour without a full model rebuild.
			No-op on rubrique nodes (no imageData). */
		void SetEtatImage(int32 etat) { if(imageData) imageData->etatImage = etat; }

	private:
		// Note we're keeping the destructor private to force use of the factory methods
		XPGUIImageNodeID() : fIDData(kNullString), fData(kNullString), type(XPGImageDataNode::kRubriqueNode), imageData(nil) {}

		XPGUIImageNodeID(PMString id, PMString data, XPGImageDataNode::NodeType nodetype, const XPGImageDataNode::ImageData * imageData);

		PMString	fIDData, fData;
		XPGImageDataNode::NodeType type;
		XPGImageDataNode::ImageData * imageData;
};

#endif // __XPGUIImageNodeID__


