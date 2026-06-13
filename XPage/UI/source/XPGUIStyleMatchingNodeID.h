

#ifndef __XPGUIStyleMatchingNodeID__
#define __XPGUIStyleMatchingNodeID__

#include "NodeID.h"
#include "IPMStream.h"
#include "XPGUIID.h"

#include "XPGMatchingStyleDataNode.h"

class XPGUIStyleMatchingNodeID : public NodeIDClass
{
	public:

		enum { kNodeType = kXPGUIUpdateStyleMatchingWidgetBoss };
		static	NodeID_rv Create() { return new XPGUIStyleMatchingNodeID(); }

		static	NodeID_rv Create(const PMString& id, 
								 const PMString& data,
								 XPGMatchingStyleDataNode::NodeType type, 
								 const XPGMatchingStyleDataNode::MatchingStyleData * matchingStyleData)  

		{ 
			return new XPGUIStyleMatchingNodeID(id, data, type, matchingStyleData); 
		}

		virtual ~XPGUIStyleMatchingNodeID() { if(matchingStyleData) delete matchingStyleData; }
		

		virtual	NodeType GetNodeType() const { return kNodeType; } 

		virtual int32 Compare(const NodeIDClass* NodeID) const;

		virtual NodeIDClass* Clone() const;

		virtual void Read(IPMStream* stream);

		virtual void Write(IPMStream* stream) const;

		const PMString& GetID() const {
			return fIDData; 
		}

		const PMString& GetData() const { 
			return fData; 
		}

		XPGMatchingStyleDataNode::NodeType GetType() const {
			return type ; 
		}
		
		const XPGMatchingStyleDataNode::MatchingStyleData * GetMatchingStyleData(){ 
			return matchingStyleData ;
		}

	private:

		// Note we're keeping the destructor private to force use of the factory methods
		XPGUIStyleMatchingNodeID() : fIDData(kNullString), 
									 fData(kNullString), 
									 type(XPGMatchingStyleDataNode::kMatchingStyleNode), 
									 matchingStyleData(nil) {}

		XPGUIStyleMatchingNodeID(PMString id, 
								 PMString data, 
							     XPGMatchingStyleDataNode::NodeType nodetype, 
								 const XPGMatchingStyleDataNode::MatchingStyleData * matchingStyleData);


		
		PMString	fIDData, fData;
		XPGMatchingStyleDataNode::NodeType type;
		XPGMatchingStyleDataNode::MatchingStyleData * matchingStyleData;

};

#endif // __XPGUIStyleMatchingNodeID__


