
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IWorkspace.h"
#include "ISession.h"
#include "IXPageUtils.h"
#include "IXPageMgrAccessor.h"
#include "IKeyValueData.h"
#include "IPMUnknownData.h"

// General includes:
#include "CPMUnknown.h"
#include "CAlert.h"
#include "ListTreeViewAdapter.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIStyleMatchingNodeID.h"
#include "XPGMatchingStyleDataNode.h"
#include "XPGID.h"


class XPGUIUpdateStyleMatchingHierarchyAdapter : public ListTreeViewAdapter {

	public:		
		XPGUIUpdateStyleMatchingHierarchyAdapter(IPMUnknown* boss);	
		virtual ~XPGUIUpdateStyleMatchingHierarchyAdapter();
		virtual int32 GetNumListItems() const;
		NodeID_rv GetRootNode() const;	
		virtual NodeID_rv GetNthListItem(const int32& nth) const;
	
};	
CREATE_PMINTERFACE(XPGUIUpdateStyleMatchingHierarchyAdapter, kXPGUIUpdateStyleMatchingHierarchyAdapterImpl)

XPGUIUpdateStyleMatchingHierarchyAdapter::XPGUIUpdateStyleMatchingHierarchyAdapter(IPMUnknown* boss)
										 :ListTreeViewAdapter(boss){}

XPGUIUpdateStyleMatchingHierarchyAdapter::~XPGUIUpdateStyleMatchingHierarchyAdapter(){}

int32 XPGUIUpdateStyleMatchingHierarchyAdapter::GetNumListItems()const{	

	InterfacePtr<IKeyValueData> listData(this, IID_IFORMEMATCHINGSTYLESLIST);
	return listData->GetKeyValueList().size();
}

NodeID_rv XPGUIUpdateStyleMatchingHierarchyAdapter::GetRootNode() const{
	
	PMString rootString("Root");
	rootString.SetTranslatable(kFalse);
	return XPGUIStyleMatchingNodeID::Create(rootString, "", XPGMatchingStyleDataNode::kMatchingStyleNode, NULL);
}

NodeID_rv XPGUIUpdateStyleMatchingHierarchyAdapter::GetNthListItem(const int32& nth) const{
	
	InterfacePtr<IKeyValueData> listData(this, IID_IFORMEMATCHINGSTYLESLIST);
	IDValuePair styleWithTag = listData->GetKeyValueList()[nth];
	return XPGUIStyleMatchingNodeID::Create(styleWithTag.Value(), styleWithTag.Key(), XPGMatchingStyleDataNode::kMatchingStyleNode, NULL);	
}

//	end, File: XPGUIUpdateStyleMatchingHierarchyAdapter.cpp
