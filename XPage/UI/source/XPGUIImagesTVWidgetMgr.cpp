
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
#include "IColorData.h"

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
#include "XPGUIImageNodeID.h"
#include "IXPGPreferences.h"
#include "XPageUIUtils.h"
#include "CAlert.h"
/**
	Implements ITreeViewWidgetMgr, providing methods to create and describe the widgets for 
	nodes in the tree. This interface is similar to a widget factory in that this is where 
	you will create widgets for the nodes in the tree.
	
	For further reading on ITreeViewWidgetMgr interface, see the Widgets chapter

*/

class XPGUIImagesTVWidgetMgr: public CTreeViewWidgetMgr
{
public:
	/**
		Constructor
		@param boss object on which this interface is being aggregated
	*/	
	XPGUIImagesTVWidgetMgr(IPMUnknown* boss);

	/**
		Destructor
	*/	
	virtual ~XPGUIImagesTVWidgetMgr() {}

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
	void indent(
		const NodeID& node, IControlView* widget, IControlView* staticTextWidget) const;

};	

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGUIImagesTVWidgetMgr, kXPGUIImagesTVWidgetMgrImpl)


namespace
{
	// Resolve an EtatImage "Couleur" string (e.g. "#ff6e00" or "vert") to an
	// RGB triple in [0,1]. Returns kFalse if the string couldn't be parsed —
	// caller should hide the swatch in that case so we don't paint a wrong
	// colour silently. The named-colour list mirrors the small set Gaia
	// emits today; extend here as new names appear server-side.
	bool16 ResolveCouleurString(const PMString& couleur, PMReal& r, PMReal& g, PMReal& b)
	{
		PMString s(couleur);
		s.StripWhiteSpace(PMString::kLeadingAndTrailingWhiteSpace);
		if (s.IsEmpty())
			return kFalse;

		// Hex form "#RRGGBB". Manual nibble decode since PMString::GetAsNumber
		// only handles base 10.
		const std::string raw = s.GetPlatformString().c_str();
		if (raw.size() == 7 && raw[0] == '#')
		{
			int32 packed = 0;
			bool ok = true;
			for (int i = 1; i < 7 && ok; ++i)
			{
				const char c = raw[i];
				int nibble;
				if      (c >= '0' && c <= '9') nibble = c - '0';
				else if (c >= 'a' && c <= 'f') nibble = 10 + (c - 'a');
				else if (c >= 'A' && c <= 'F') nibble = 10 + (c - 'A');
				else { ok = false; break; }
				packed = (packed << 4) | nibble;
			}
			if (ok)
			{
				r = ((packed >> 16) & 0xFF) / 255.0;
				g = ((packed >> 8)  & 0xFF) / 255.0;
				b = (packed         & 0xFF) / 255.0;
				return kTrue;
			}
		}

		// Named (French) colours used by Gaia in etatsimages.
		struct Named { const char* name; int32 rgb; };
		static const Named kNames[] = {
			{"vert",       0x00CC00},
			{"rouge",      0xFF0000},
			{"bleu",       0x0033FF},
			{"jaune",      0xFFFF00},
			{"orange",     0xFF9900},
			{"noir",       0x000000},
			{"blanc",      0xFFFFFF},
			{"gris",       0x808080},
			{"violet",     0x9933FF},
			{"rose",       0xFF3399},
			{"marron",     0x8B4513},
			{"cyan",       0x00FFFF},
			{"magenta",    0xFF00FF},
		};
		for (size_t i = 0; i < sizeof(kNames) / sizeof(kNames[0]); ++i)
		{
			if (s.Compare(kFalse, kNames[i].name) == 0)
			{
				r = ((kNames[i].rgb >> 16) & 0xFF) / 255.0;
				g = ((kNames[i].rgb >> 8)  & 0xFF) / 255.0;
				b = (kNames[i].rgb         & 0xFF) / 255.0;
				return kTrue;
			}
		}
		return kFalse;
	}
}

/* Constructor
*/
XPGUIImagesTVWidgetMgr::XPGUIImagesTVWidgetMgr(IPMUnknown* boss) :
	CTreeViewWidgetMgr(boss)
{
}


/* CreateWidgetForNode
*/
IControlView* XPGUIImagesTVWidgetMgr::CreateWidgetForNode(
	const NodeID& node) const
{
	IControlView* retval= (IControlView*) ::CreateObject(
		::GetDataBase(this),
		RsrcSpec(LocaleSetting::GetLocale(), 
		kXPGUIPluginID, 
		kViewRsrcType, 
		kXPGUIImageNodeWidgetRsrcID),
		IID_ICONTROLVIEW);

	ASSERT(retval);
	return retval;
}



/* GetWidgetTypeForNode
*/
WidgetID XPGUIImagesTVWidgetMgr::GetWidgetTypeForNode(
	const NodeID& node) const
{
	return kXPGUIImageNodeWidgetId;
}


/* ApplyNodeIDToWidget
*/
bool16 XPGUIImagesTVWidgetMgr::ApplyNodeIDToWidget(
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

		TreeNodePtr<XPGUIImageNodeID> nodeID(node);

		IControlView* displayStringView = panelControlData->FindWidget( kXPGUIImageNameWidgetID );
		IControlView* etatSwatchView = panelControlData->FindWidget(kXPGUIImageEtatSwatchWidgetID);
		IControlView* placedIconView = panelControlData->FindWidget(kXPGUIImagePlacedIconWidgetID);

		PMString stringToDisplay = kNullString;
		InterfacePtr<IUIFontSpec> fontSpec (displayStringView, UseDefaultIID());


		if(nodeID->GetType() == XPGImageDataNode::kRubriqueNode)
		{
			fontSpec->SetFontID(kDialogWindowSystemScriptFontId);
			fontSpec->SetHiliteFontID(kDialogWindowSystemScriptFontId);

			PMString rubriqueName = nodeID->GetData();
			stringToDisplay = rubriqueName;

			// No state pastille / placed status on rubrique header rows.
			if (etatSwatchView)
				etatSwatchView->HideView();
			if (placedIconView)
				placedIconView->HideView();
		}
		else
		{
			fontSpec->SetFontID(kPaletteWindowSystemScriptFontId);
			fontSpec->SetHiliteFontID(kPaletteWindowSystemScriptFontId);

			PMString subjectName = nodeID->GetImageData()->name;
			stringToDisplay = subjectName;

			// Pastille colour: look up the row's etatImage id in the cached
			// etatsimages list and resolve its Couleur string.
			if (etatSwatchView)
			{
				bool16 painted = kFalse;
				const int32 etatID = nodeID->GetImageData()->etatImage;
				if (etatID > 0)
				{
					InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
					if (xpgPrefs != nil)
					{
						EtatImageList etats = xpgPrefs->GetEtatsImages();
						for (int32 i = 0; i < etats.size(); ++i)
						{
							if (etats[i].ID == etatID)
							{
								PMReal r, g, bl;
								if (ResolveCouleurString(etats[i].Couleur, r, g, bl))
								{
									ColorArray colorArray;
									colorArray.push_back(r);
									colorArray.push_back(g);
									colorArray.push_back(bl);
									InterfacePtr<IColorData> colorData(etatSwatchView, UseDefaultIID());
									if (colorData)
										colorData->SetColorData(kPMCsCalRGB, colorArray);
									painted = kTrue;
								}
								break;
							}
						}
					}
				}
				// Always keep the swatch visible: clicking it opens the
				// "change état" dialog, including for photos that have no
				// state yet. When we couldn't resolve a colour (no etat or
				// unknown colour string) paint a neutral light grey so the
				// pastille reads as "no state — click to set".
				if (!painted)
				{
					ColorArray greyArray;
					greyArray.push_back(0.80);
					greyArray.push_back(0.80);
					greyArray.push_back(0.80);
					InterfacePtr<IColorData> colorData(etatSwatchView, UseDefaultIID());
					if (colorData)
						colorData->SetColorData(kPMCsCalRGB, greyArray);
				}
				etatSwatchView->ShowView();
			}

			// Placed-status icon: green check if the image's id appears in
			// any placed-image link of the front document. Hidden otherwise.
			if (placedIconView)
			{
				const PMString imageId = nodeID->GetID();
				if (XPageUIUtils::IsImagePlacedInFrontDoc(imageId))
				{
					placedIconView->SetRsrcPluginID(kXPGUIPluginID);
					placedIconView->SetRsrcID(kXPGUIPlacedImageIconResourceID);
					placedIconView->ShowView();
				}
				else
				{
					placedIconView->HideView();
				}
			}

	}
		
		if(stringToDisplay.IsEmpty()) {
			stringToDisplay="Empty?";	
		}

		stringToDisplay.SetTranslatable( kFalse );

		InterfacePtr<ITextControlData> textControlData( displayStringView, UseDefaultIID());
		textControlData->SetString(stringToDisplay);
		
		// Adjust indenting- this may be temporary
		this->indent( node, widget, displayStringView );
		
	} while(kFalse);

	return kTrue;
}


/* GetIndentForNode
*/
PMReal XPGUIImagesTVWidgetMgr::GetIndentForNode(const NodeID& node) const
{
	return 18.0;
}


/* indent
*/
void XPGUIImagesTVWidgetMgr::indent(
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
		const PMReal curIndent = this->GetIndent(node);	
		// adjust the size to fit the text 
		PMRect widgetFrame = widget->GetFrame();
		widgetFrame.Left() = curIndent;
		widget->SetFrame( widgetFrame );
		// Call window changed to force FittedStaticText to resize
		staticTextWidget->WindowChanged();
		PMRect staticTextFrame = staticTextWidget->GetFrame();
		staticTextFrame.Right( widgetFrame.Right() );
		// Don't at present take account of scroll bar dimension?
		staticTextWidget->SetFrame( staticTextFrame );
	} while(kFalse);
}

//	end, File: XPGUIImagesTVWidgetMgr.cpp
