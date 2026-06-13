
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IControlView.h"
#include "IImportProvider.h"
#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "IPanelControlData.h"
#include "ITextControlData.h"
#include "ITreeViewHierarchyAdapter.h"
#include "ITextValue.h"

// General includes:
#include "CTreeViewWidgetMgr.h"
#include "CreateObject.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"
#include "SysControlIds.h"
#include "OpenPlaceID.h"
#include "IColorData.h"
#include "ITip.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "XMLDefs.h"
#include "XPGUIArticleNodeID.h"
#include "CAlert.h"

#include <string>

namespace
{
	// Resolve an etat colour string (as emitted by Gaia in etatsarticles) to RGB
	// in [0,1]. Accepts hex form "#RRGGBB" or a French colour name. Mirrors the
	// linked-photos palette (XPGUIImagesTVWidgetMgr) so both panels share the
	// same colour semantics whatever the backend (PHP or 4D).
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

		// Named (French) colours used by Gaia in etatsarticles.
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

		// Legacy decimal form (4D sends the packed RGB as a base-10 integer).
		PMString::ConversionError convError = PMString::kNoError;
		const int32 packed = s.GetAsNumber(&convError);
		if (convError == PMString::kNoError && packed != 0)
		{
			r = ((packed >> 16) & 0xFF) / 255.0;
			g = ((packed >> 8)  & 0xFF) / 255.0;
			b = (packed         & 0xFF) / 255.0;
			return kTrue;
		}
		return kFalse;
	}
}


class XPGUIArticleTVWidgetMgr: public CTreeViewWidgetMgr
{
public:

	XPGUIArticleTVWidgetMgr(IPMUnknown* boss);
	virtual ~XPGUIArticleTVWidgetMgr() {}

	virtual	IControlView* CreateWidgetForNode(const NodeID& node) const;
	
	virtual	WidgetID GetWidgetTypeForNode(const NodeID& node) const;

	virtual	bool16 ApplyNodeIDToWidget(	const NodeID& node, IControlView* widget, int32 message = 0) const;

	virtual PMReal GetIndentForNode(const NodeID& node) const;

private:
	void indent(const NodeID& node, IControlView* widget, IControlView* staticTextWidget) const;
};	

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGUIArticleTVWidgetMgr, kXPGUIArticleTVWidgetMgrImpl)

/* Constructor
*/
XPGUIArticleTVWidgetMgr::XPGUIArticleTVWidgetMgr(IPMUnknown* boss) :
	CTreeViewWidgetMgr(boss)
{
}


/* CreateWidgetForNode
*/
IControlView* XPGUIArticleTVWidgetMgr::CreateWidgetForNode(const NodeID& node) const{
	IControlView* retval= (IControlView*) ::CreateObject(
		::GetDataBase(this),
		RsrcSpec(LocaleSetting::GetLocale(), 
		kXPGUIPluginID, 
		kViewRsrcType, 
		kXPGUIArticleNodeWidgetRsrcID),
		IID_ICONTROLVIEW);
	ASSERT(retval);
	return retval;
}



/* GetWidgetTypeForNode
*/
WidgetID XPGUIArticleTVWidgetMgr::GetWidgetTypeForNode(
	const NodeID& node) const
{
	return kXPGUIArticleNodeWidgetId;
}


/* ApplyNodeIDToWidget
*/
bool16 XPGUIArticleTVWidgetMgr::ApplyNodeIDToWidget(const NodeID& node, IControlView* widget, int32 message) const
{	
	CTreeViewWidgetMgr::ApplyNodeIDToWidget(node, widget);
	do{
		WidgetID widgetID = widget->GetWidgetID();  

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

		TreeNodePtr<XPGUIArticleNodeID> nodeID(node);
		if(nodeID->GetType() == XPGArticleDataNode::kCorpsNode){

			IControlView* displayStringView = panelControlData->FindWidget(kXPGUITextSubjectWidgetID);
			InterfacePtr<ITextControlData> textControlData(displayStringView, UseDefaultIID() );

			PMString textSubject = nodeID->GetArticleData()->artSubject;
			if(textSubject.IsEmpty())  
				textSubject="Empty?";	
			textSubject.SetTranslatable(kFalse);
			textControlData->SetString(textSubject);
			
			// Status pastille colour. The backend (PHP/Gaia) sends the etat's
			// HTML colour ("#RRGGBB" or a French name), like the linked-photos
			// palette; resolve it to RGB the same way. Falls back to black if
			// the string is empty or unrecognised.
			PMReal rRed = 0.0, rGreen = 0.0, rBlue = 0.0;
			ResolveCouleurString(nodeID->GetArticleData()->artCouleurStatus, rRed, rGreen, rBlue);

			// Set ColorArray RGB
			ColorArray colorArray;
            colorArray.push_back(rRed);
            colorArray.push_back(rGreen);
            colorArray.push_back(rBlue);

			// RGB color space
			int16 colorSpace = kPMCsCalRGB;

			// Set Color Data
	        IControlView* displayColorStatusView = panelControlData->FindWidget(kXPGUITextStatusWidgetID );
			InterfacePtr<IColorData> colorControlData (displayColorStatusView, UseDefaultIID());
			colorControlData->SetColorData(colorSpace, colorArray);

			// Set nb Photos
			if(nodeID->GetArticleData()->artNbPhotos > 0){

				InterfacePtr<ITextValue> nbPhotosValue (panelControlData->FindWidget(kXPGUITextPictureCountWidgetID),UseDefaultIID()) ;
				int32 nbPhotos = nodeID->GetArticleData()->artNbPhotos;
				nbPhotosValue->SetTextFromValue(nbPhotos);

				IControlView* pictureIconView = panelControlData->FindWidget(kXPGUITextPictureIconWidgetID);
				pictureIconView->SetRsrcPluginID(kXPGUIPluginID);
				pictureIconView->SetRsrcID(kXPGUIPictureIconResourceID);
			}

			// Adjust indenting- this may be temporary
			this->indent(node, widget, displayStringView);
		}		
	} while(kFalse);

	return kTrue;
}


/* GetIndentForNode
*/
PMReal XPGUIArticleTVWidgetMgr::GetIndentForNode(const NodeID& node) const
{
	// No left indent: the article type icon column (0-18) was removed, so
	// child nodes start flush left. The expander (if ever shown) lives in the
	// first 18px of the node itself.
	return 0.0;
}


/* indent
*/
void XPGUIArticleTVWidgetMgr::indent(const NodeID& node, IControlView* widget,	IControlView* staticTextWidget) const{	

	do{
		ASSERT(widget);
		//ASSERT(staticTextWidget);
		if(widget == nil || staticTextWidget == nil) {
			break;
		}
		const PMReal currentindent = this->GetIndent(node);	
		// adjust the size to fit the text 

		PMRect widgetFrame = widget->GetFrame();
		PMReal widthBefore = widgetFrame.Width();

		widgetFrame.Left() = currentindent;
		widget->SetFrame( widgetFrame );
		
		// Call window changed to force FittedStaticText to resize
		staticTextWidget->WindowChanged();
		PMRect staticTextFrame = staticTextWidget->GetFrame();
		staticTextFrame.Right(widgetFrame.Right());

		// Don't at present take account of scroll bar dimension?
		staticTextWidget->SetFrame( staticTextFrame );

	} while(kFalse);
}

//	end, File: XPGUIArticleTVWidgetMgr.cpp
