
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

// General includes:
#include "CTreeViewWidgetMgr.h"
#include "CreateObject.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"
#include "SysControlIds.h"
#include "OpenPlaceID.h"
#include "IXPageUtils.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIFormeNodeID.h"
#include "CAlert.h"

/**
	Implements ITreeViewWidgetMgr, providing methods to create and describe the widgets for 
	nodes in the tree. This interface is similar to a widget factory in that this is where 
	you will create widgets for the nodes in the tree.
	
	For further reading on ITreeViewWidgetMgr interface, see the Widgets chapter

*/

class XPGUIFormesTVWidgetMgr: public CTreeViewWidgetMgr
{
public:
	/**
		Constructor
		@param boss object on which this interface is being aggregated
	*/	
	XPGUIFormesTVWidgetMgr(IPMUnknown* boss);

	/**
		Destructor
	*/	
	virtual ~XPGUIFormesTVWidgetMgr() {}

	/** Create instance of the widget that represents given node in the tree.
		@param node [IN] specifies node of interest
		@return return reference to a newly instantiated object that is suitable for this node
	*/	
	virtual	IControlView* CreateWidgetForNode(
		const NodeID& node) const;
	/**
		Retrieve ID of a widget that has the right appearance for the node
		that you're trying to display. Here we use the same widget types for all the nodes,
		but vary the appearance by showing or hiding the expander widget depending on the number
		of children associated with a node.
		@param node [IN] specifies node of interest
		@return the ID of a widget that has the correct appearance for the given node type
	*/	
	virtual	WidgetID GetWidgetTypeForNode(const NodeID& node) const;

	/** Determine how to render the given node to the specified control.
		Figure out how you're going to render it based on properties of the node
		such as whether is has children or not (don't show expander if no children, for instance).
		@param node [IN] refers to the node that we're trying to render
		@param widget [IN] the control into which this node's appearance is going to be rendered
		@param message [IN] specifies ???
		@return kTrue if the node has been rendered successfully, kFalse otherwise
	*/	
	virtual	bool16 ApplyNodeIDToWidget(
		const NodeID& node, 
		IControlView* widget, 
		int32 message = 0) const;

	/** Given a particular node, how far should it be indented? This method
		returns a value in pixels that is used to indent the displayed node 
		from the left edge of the tree-view control.
		@param node [IN] specifies node of interest
		@return an indent for this particular node from the left edge of tree-view
	*/
	virtual PMReal GetIndentForNode(const NodeID& node) const;

private:
	void indent(const NodeID& node, IControlView* widget, IControlView* staticTextWidget) const;

};	

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGUIFormesTVWidgetMgr, kXPGUIFormesTVWidgetMgrImpl)

/* Constructor
*/
XPGUIFormesTVWidgetMgr::XPGUIFormesTVWidgetMgr(IPMUnknown* boss) :
	CTreeViewWidgetMgr(boss)
{
}


/* CreateWidgetForNode
*/
IControlView* XPGUIFormesTVWidgetMgr::CreateWidgetForNode(
	const NodeID& node) const
{
	IControlView* retval= (IControlView*) ::CreateObject(
		::GetDataBase(this),
		RsrcSpec(LocaleSetting::GetLocale(), 
		kXPGUIPluginID, 
		kViewRsrcType, 
		kXPGUIFormeNodeWidgetRsrcID),
		IID_ICONTROLVIEW);

	ASSERT(retval);
	return retval;
}



/* GetWidgetTypeForNode
*/
WidgetID XPGUIFormesTVWidgetMgr::GetWidgetTypeForNode(
	const NodeID& node) const
{
	return kXPGUIFormeNodeWidgetId;
}


/* ApplyNodeIDToWidget
*/
bool16 XPGUIFormesTVWidgetMgr::ApplyNodeIDToWidget(
	const NodeID& node, IControlView* widget, int32 message) const
{
	CTreeViewWidgetMgr::ApplyNodeIDToWidget(node, widget);
	do
	{
		// If this node doesn't have any children, don't show expander Widget
		InterfacePtr<IPanelControlData> panelControlData(widget, UseDefaultIID());
	
		// Set expander widget to hidden state if this node has no kids
		IControlView* expanderWidget = panelControlData->FindWidget(kTreeNodeExpanderWidgetID);
		InterfacePtr<const ITreeViewHierarchyAdapter> adapter(this, UseDefaultIID());
	
		if (expanderWidget) {
			if (adapter->GetNumChildren(node) <= 0) {
				expanderWidget->HideView();
			}
			else {
				expanderWidget->ShowView();
			}
		}

		TreeNodePtr<XPGUIFormeNodeID> nodeID(node);
		IControlView* displayStringView = panelControlData->FindWidget( kXPGUIFormeNameWidgetID );
		
		PMString stringToDisplay = kNullString;
		InterfacePtr<IUIFontSpec> fontSpec (displayStringView, UseDefaultIID());
	
		if(nodeID->GetType() == XPGFormeDataNode::kClasseurNode)
		{
			fontSpec->SetFontID(kDialogWindowSystemScriptFontId);
			fontSpec->SetHiliteFontID(kDialogWindowSystemScriptFontId);

			PMString rubriqueName = nodeID->GetData();

			stringToDisplay = rubriqueName;
		}
		else
		{
		    fontSpec->SetFontID(kPaletteWindowSystemScriptFontId);
			fontSpec->SetHiliteFontID(kPaletteWindowSystemScriptFontId);	
			
			PMString subjectName = nodeID->GetFormeData()->name;
			stringToDisplay = subjectName;
		
		}
		
		if(stringToDisplay.IsEmpty()) 
			stringToDisplay="Empty?";	
		

		stringToDisplay.SetTranslatable( kFalse );

		InterfacePtr<ITextControlData> textControlData(displayStringView, UseDefaultIID());
		textControlData->SetString(stringToDisplay);
		
		// Adjust indenting- this may be temporary
		this->indent( node, widget, displayStringView );
		
	} while(kFalse);

	return kTrue;
}


/* GetIndentForNode
*/
PMReal XPGUIFormesTVWidgetMgr::GetIndentForNode(const NodeID& node) const
{
	return 18.0;
}


/* indent
*/
void XPGUIFormesTVWidgetMgr::indent(
	const NodeID& node, 
	IControlView* widget, 
	IControlView* staticTextWidget) const
{	
	do
	{
		ASSERT(widget);
		ASSERT(staticTextWidget);
		if(widget == nil || staticTextWidget == nil) {
			break;
		}
		const PMReal curindent = this->GetIndent(node);	
		// adjust the size to fit the text 
		PMRect widgetFrame = widget->GetFrame();
		widgetFrame.Left() = curindent;
		widget->SetFrame( widgetFrame );
		// Call window changed to force FittedStaticText to resize
		staticTextWidget->WindowChanged();
		PMRect staticTextFrame = staticTextWidget->GetFrame();
		staticTextFrame.Right( widgetFrame.Right() );
		// Don't at present take account of scroll bar dimension?
		staticTextWidget->SetFrame( staticTextFrame );

	} while(kFalse);
}

//	end, File: XPGUIFormesTVWidgetMgr.cpp
