
#include "VCPlugInHeaders.h"

// Interfaces:
#include "IPanelControlData.h"
#include "IListControlDataOf.h"
#include "IControlView.h"

// Includes:
#include "PalettePanelView.h"
#include "XPGID.h"
#include "XPGUIID.h"
// Utility files:

#include "IPalettePanelUtils.h"

#include "CAlert.h"

/**
	Implements IControlView for the panel, based on PalettePanelView. This is required to 
	support the correct behaviour on ConstrainDimensions being sent to the view.

	@ingroup detailcontrollistsize
	
*/
class  XPGUITextesPanelView : public PalettePanelView
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is being aggregated.
	*/
	XPGUITextesPanelView(IPMUnknown* boss);

	/**
		Destructor.
	*/
	virtual ~XPGUITextesPanelView();

	virtual void Resize (const PMPoint &dimensions, bool16 invalidate) ;

	/**
		Override ErasablePanelView::ConstrainDimensions. This ensures that the list box
		does not become smaller when the list-element size is reduced.
		@param dimensions set up the dimension through this out-parameter.
	*/
	virtual PMPoint	ConstrainDimensions(const PMPoint& dimensions) const;

	virtual void Invalidate(PMRect * bbox = nil);

private:
	static const int kMinDCLSizPalWidth;
	static const int kMaxDCLSizPalWidth;
	static const int kMinDCLSizPalHeight;
	static const int kMaxDCLSizPalHeight;

	//void ResizeListBox(const PMPoint& dimensions);
};


const int XPGUITextesPanelView::kMinDCLSizPalWidth	=	382;
const int XPGUITextesPanelView::kMaxDCLSizPalWidth	=	750;

#if TRAFFIC && !INCOPY
const int XPGUITextesPanelView::kMinDCLSizPalHeight	=	200;
#else
const int XPGUITextesPanelView::kMinDCLSizPalHeight	=	207;
#endif

// Bumped from 346 to 700 — la valeur d'origine plafonnait le redim
// vertical sur les 4 palettes XPage (XDA / XPage / Photos liées /
// Assemblages) qui partagent toutes ce IControlView impl, ce qui
// rendait inopérant le bump de la `Frame` naturelle dans XPGUI_InDesign.fr.
// 700 laisse de la marge pour des palettes natives 600px.
const int XPGUITextesPanelView::kMaxDCLSizPalHeight	=	700;


/* CREATE_PERSIST_PMINTERFACE
 Binds the C++ persistent implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PERSIST_PMINTERFACE(XPGUITextesPanelView, kXPGUITextesPanelViewImpl)

/* Constructor
*/
XPGUITextesPanelView::XPGUITextesPanelView(IPMUnknown* boss) :
	PalettePanelView(boss) 
{
}

/* Destructor
*/
XPGUITextesPanelView::~XPGUITextesPanelView()
{
}

/* DoPostCreate 
*/
void XPGUITextesPanelView::Resize(const PMPoint &dimensions, bool16 invalidate)
{
	PalettePanelView::Resize(dimensions,invalidate);

	//ResizeListBox(dimensions);
}

/* ConstrainDimensions
*/
PMPoint XPGUITextesPanelView::ConstrainDimensions(const PMPoint& dimensions) const
{
	PMPoint constrainedDim = dimensions;

	// Width can vary if not above maximum or below minimum
	if(constrainedDim.X() > kMaxDCLSizPalWidth) 
	{
		constrainedDim.X(kMaxDCLSizPalWidth);
	}
	else
		if(constrainedDim.X() < kMinDCLSizPalWidth) 
		{
			constrainedDim.X(kMinDCLSizPalWidth);
		}

	// Height can vary if not above maximum or below minimum
	if(constrainedDim.Y() > kMaxDCLSizPalHeight) 
	{
		constrainedDim.Y(kMaxDCLSizPalHeight);
	}
	else 
	{
		if(constrainedDim.Y() < kMinDCLSizPalHeight) 
		{
			constrainedDim.Y(kMinDCLSizPalHeight);
		}
	}
	
	return constrainedDim;
}


void XPGUITextesPanelView::Invalidate(PMRect * bbox)
{
	PalettePanelView::Invalidate(bbox);

	PMPoint dimensions(GetFrame().Width(), GetFrame().Height());

	//ResizeListBox(dimensions);
}
/*
void XPGUITextesPanelView::ResizeListBox(const PMPoint& dimensions)
{
	InterfacePtr<IPanelControlData> panelControlData (this, UseDefaultIID());

	IControlView * listbox = panelControlData->FindWidget(kTGTUIAttrListBoxWidgetID);
	InterfacePtr< IListControlDataOf<IControlView*> > listData(listbox, UseDefaultIID());

	PMReal newCellWidth = (dimensions.X() - 32 - 4) / 3;

	for(int32 i = 0 ; i < listData->Length() ; ++i)
	{
		IControlView * attribute = (*listData)[i];
		InterfacePtr<IPanelControlData> attrControlData (attribute, UseDefaultIID());

		IControlView * idAttr = attrControlData->FindWidget(kTGTUIAttrIDElementTextWidgetID);
		IControlView * labelAttr = attrControlData->FindWidget(kTGTUIAttrLabelElementTextWidgetID);
		IControlView * valAttr = attrControlData->FindWidget(kTGTUIAttrValueElementTextWidgetID);

		PMRect frame;

		idAttr->SetFrame(PMRect(1, 0,1 + newCellWidth, 17), kFalse);
		labelAttr->SetFrame(PMRect(newCellWidth+2, 0, (2*newCellWidth) + 2, 17), kFalse);
		valAttr->SetFrame(PMRect((2*newCellWidth) + 3, 0, (3*newCellWidth)+3, 17), kFalse);
	}
	listbox->Invalidate();
}
*/
// End, XPGUITextesPanelView.cpp.

