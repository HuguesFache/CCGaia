#include "VCPlugInHeaders.h"

// Interface includes:
#include "IControlView.h"
#include "IImportProvider.h"
#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "IPanelControlData.h"
#include "ITextControlData.h"
#include "ITreeViewHierarchyAdapter.h"
#include "IUIFontSpec.h"
#include "IDropDownListController.h"

// General includes:
#include "CTreeViewWidgetMgr.h"
#include "IStringListControlData.h"
#include "IStringListData.h"
#include "CreateObject.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"
#include "SysControlIds.h"
#include "OpenPlaceID.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIStyleMatchingNodeID.h"
#include "XPageUIUtils.h"
#include "CAlert.h"
#include "IXPGPreferences.h"


class XPGUIUpdateStyleMatchingWidgetMgr: public CTreeViewWidgetMgr {

	public:
		
		XPGUIUpdateStyleMatchingWidgetMgr(IPMUnknown* boss);		
		virtual ~XPGUIUpdateStyleMatchingWidgetMgr() {}
		
		virtual	IControlView* CreateWidgetForNode(const NodeID& node) const;
		
		virtual	WidgetID GetWidgetTypeForNode(const NodeID& node) const;	
		virtual	bool16 ApplyNodeIDToWidget(	const NodeID& node, IControlView* widget, int32 message = 0) const;		
		virtual PMReal GetIndentForNode(const NodeID& node) const;

	private:		
		
		void indent(const NodeID& node, IControlView* widget, IControlView* staticTextWidget) const;	
		K2Vector<Tag> tagList;
};	

CREATE_PMINTERFACE(XPGUIUpdateStyleMatchingWidgetMgr, kXPGUIUpdateStyleMatchingWidgetMgrImpl)

XPGUIUpdateStyleMatchingWidgetMgr:: XPGUIUpdateStyleMatchingWidgetMgr(IPMUnknown* boss)
								  : CTreeViewWidgetMgr(boss)
{
		//Fill list and Select default item if any 
		InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
		tagList = xpgPrefs->GetListeMatchingTag();		
}


/* CreateWidgetForNode
*/
IControlView* XPGUIUpdateStyleMatchingWidgetMgr::CreateWidgetForNode(const NodeID& node) const{

	IControlView* retval= (IControlView*) ::CreateObject(::GetDataBase(this), 
														 RsrcSpec(LocaleSetting::GetLocale(), 
														 kXPGUIPluginID, 
														 kViewRsrcType, 
														 kXPGUIUpdateStyleMatchingRsrcID),
														 IID_ICONTROLVIEW);
	return retval;
}


/* GetIndentForNode
*/
PMReal XPGUIUpdateStyleMatchingWidgetMgr::GetIndentForNode(const NodeID& node) const{
	return 18.0;
}

/* GetWidgetTypeForNode
*/
WidgetID XPGUIUpdateStyleMatchingWidgetMgr::GetWidgetTypeForNode(const NodeID& node) const{
	return kXPGUIUpdateStyleMatchingWidgetID;
}


/* ApplyNodeIDToWidget
*/
bool16 XPGUIUpdateStyleMatchingWidgetMgr::ApplyNodeIDToWidget(const NodeID& node, IControlView* widget, int32 message) const{

	CTreeViewWidgetMgr::ApplyNodeIDToWidget(node, widget);
	
	do{
		// If this node doesn't have any children, don't show expander Widget
		InterfacePtr<IPanelControlData> panelControlData(widget, UseDefaultIID());
	
		// Set expander widget to hidden state if this node has no kids
		IControlView* expanderWidget = panelControlData->FindWidget(kTreeNodeExpanderWidgetID);
		InterfacePtr<const ITreeViewHierarchyAdapter> adapter(this, UseDefaultIID());			
	
		if (expanderWidget) {
			if (adapter->GetNumChildren(node) <= 0)
				expanderWidget->HideView();			
			else 
				expanderWidget->ShowView();			
		}

		// Display Style name
		TreeNodePtr<XPGUIStyleMatchingNodeID> nodeID(node);

		PMString styleName = nodeID->GetID();
		styleName.SetTranslatable(kFalse);

		PMString styleTag = nodeID->GetData();
		styleTag.SetTranslatable(kFalse);

		// Display forme name
		IControlView* displayStringView = panelControlData->FindWidget( kXPGUIUpdateStyleNameWidgetID );
		InterfacePtr<ITextControlData> textControlData(displayStringView, UseDefaultIID());
		textControlData->SetString(styleName);

		IControlView * dropDownListview = panelControlData->FindWidget(kXPGUIUpdateStyleTagListWidgetID);
		InterfacePtr<IStringListControlData> dropDownListData (dropDownListview, UseDefaultIID());		
		
		for(int32 i = 0; i < tagList.size() ; i++){
			PMString tagName = tagList[i].value;
			tagName.SetTranslatable(kFalse);
			dropDownListData->AddString(tagName);
		}

		int32 indexSelected = 0;
		if(styleTag != kNullString)
		{
			indexSelected = dropDownListData->GetIndex(styleTag);		
		}
		InterfacePtr<IDropDownListController> dropDownListController (dropDownListData, UseDefaultIID());
		dropDownListController->Select(indexSelected);


	} while(kFalse);
	return kTrue;
}


//	end, File: XPGUIUpdateStyleMatchingWidgetMgr.cpp
