/*
//	File:	XPGUIPlacedFormeFooterAdornment.cpp
//
//	Date:	24-Oct-2005
//
//  Author : Trias Developpement
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IAdornmentShape.h"
#include "IGeometry.h"
#include "IGraphicsPort.h"
#include "IHierarchy.h"
#include "IInterfaceColors.h"
#include "IShape.h"
#include "IViewPortAttributes.h"
#include "IFontMgr.h"
#include "IPMFont.h"
#include "IFontInstance.h"
#include "ILayoutControlData.h"
#include "IPanorama.h"
#include "ILayoutUIUtils.h"
#include "CAlert.h"
#include "IXPageUtils.h"

// General includes:
#include "TransformUtils.h"
#include "Utils.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "IFormeData.h"

/** Implementation of IAdornmentShape to decorate custom datalinked container page 
	items
	@ingroup customdatalink
*/
class XPGUIPlacedFormeFooterAdornment:public CPMUnknown<IAdornmentShape>{

	public:
		/** Constructor.
			@param boss
		*/
		XPGUIPlacedFormeFooterAdornment(IPMUnknown* boss);

		/** Destructor.
		*/
		virtual ~XPGUIPlacedFormeFooterAdornment();	
		
		/** See IAdornmentShape::GetDrawOrderBits
		 */
		virtual AdornmentDrawOrder	GetDrawOrderBits();

			/** See IAdornmentShape::Draw
		 */
		virtual void				DrawAdornment
			(
				IShape* iShape, 
				AdornmentDrawOrder drawOrder, 
				GraphicsData* gd, 
				int32 flags
			);
	
		/** See IAdornmentShape::GetPaintedBBox
		 */
		virtual PMRect				GetPaintedAdornmentBounds
			(
				IShape*				iShape,
				AdornmentDrawOrder	drawOrder,
				const PMRect&		itemBounds, // This is the painted bounds of the owing page item
				const PMMatrix&		innertoview	// NOTE: this is inner to view not pb to view
			);

		virtual PMRect GetPrintedAdornmentBounds  
			( 
				IShape *  iShape,  
				AdornmentDrawOrder  drawOrder,  
				const PMRect &  itemBounds,  
				const PMMatrix &  innertoview   
			);

			/** See IAdornmentShape::AddToContentInkBounds
		 */
		virtual void				AddToContentInkBounds
			(
				IShape* iShape, 
				PMRect* inOutBounds
			);
		/** See IAdornmentShape::GetPriority
		 */
		virtual PMReal				GetPriority();

			/** See IAdornmentShape::Inval
		 */
		virtual void				Inval
			(
				IShape* iShape, 
				AdornmentDrawOrder drawOrder, 
				GraphicsData* gd, 
				ClassID reasonForInval, 
				int32 flags
			);

		/** See IAdornmentShape::WillPrint
		 */
		virtual bool16				WillPrint();

		/** See IAdornmentShape::WillDraw
		 */
		virtual bool16				WillDraw
			(
				IShape* iShape,
				AdornmentDrawOrder	drawOrder,	
				GraphicsData*		gd,
				int32				flags
			);

		virtual bool16  HitTest 
			(
				IShape *iShape, 
				AdornmentDrawOrder adornmentDrawOrder, 
				IControlView *layoutView, 
				const PMRect &mouseRect
			);

	private :		
				
		PMString SplitFormeDescription(const PMString& uniqueName);
		PMPoint GetLeftTopPoint(IGeometry * itemGeo);	
		UIDRef GetLeftTopMostItem(const K2Vector<UIDRef>& items);
		K2Vector<UIDRef> formeItems;
};

/*
*/
CREATE_PMINTERFACE(XPGUIPlacedFormeFooterAdornment, kXPGUIPlacedFormeFooterAdornmentImpl)

/*
*/
XPGUIPlacedFormeFooterAdornment::XPGUIPlacedFormeFooterAdornment(IPMUnknown* boss):CPMUnknown<IAdornmentShape>(boss){
	 formeItems.clear();
}

/*
*/
XPGUIPlacedFormeFooterAdornment::~XPGUIPlacedFormeFooterAdornment(){}

/*
*/
IAdornmentShape::AdornmentDrawOrder XPGUIPlacedFormeFooterAdornment::GetDrawOrderBits(){
	return kAfterShape;
}

/*
*/
void XPGUIPlacedFormeFooterAdornment::DrawAdornment (IShape* iShape, IAdornmentShape::AdornmentDrawOrder drawOrder,	GraphicsData* gd, int32 flags){

	do {	
		if (drawOrder != kAfterShape) 
			break;		 

		if (flags & IShape::kPrinting)  		 
			break;		 
		
		// Get the forme name of the item and make it a string		
		InterfacePtr<IFormeData> formeData (iShape, UseDefaultIID());
		if(formeData == nil)
			break;
		
		PMString formeName = kNullString, dummy = kNullString;
		Utils<IXPageUtils>()->SplitFormeDescription(formeData->GetUniqueName(), dummy, formeName);
		if(formeName == kNullString)
			break;

		UIDRef LeftTopMostItem = this->GetLeftTopMostItem(this->formeItems);
		
		int32 numUTF16;
		const textchar * buff = formeName.GrabUTF16Buffer(&numUTF16);
	
		// The labels are created using the default font.
		InterfacePtr<IFontMgr> fontMgr(GetExecutionContextSession(), UseDefaultIID());
		if (fontMgr == nil)
			break;

		InterfacePtr<IPMFont> theFont(fontMgr->QueryFont(fontMgr->GetDefaultFontName()));
		InterfacePtr<IGeometry> itemGeometry(iShape, UseDefaultIID());
		InterfacePtr<IHierarchy> itemHier(iShape, UseDefaultIID());
		if (theFont == nil || itemGeometry == nil || itemHier == nil)
			break;
		
		PMReal fontSize = 10.0;
		PMReal xOffset = 1.5, yOffset = 1.5; 
		PMReal xBoxOffset = 4.0, yBoxOffset = 2.0;
		PMReal zoom = 1.0;
 
		// Check current zoom factor and increade font size if necessary
		InterfacePtr<ILayoutControlData> layoutControlData (Utils<ILayoutUIUtils>()->QueryFrontLayoutData());
		InterfacePtr<IPanorama> panorama (layoutControlData, UseDefaultIID());
		if(panorama)
			zoom = panorama->GetXScaleFactor();		
 
		fontSize = fontSize * (1.0 / zoom); // this way, tag has always the same size, whatever the zoom factor is
		xOffset = xOffset * (1.0 / zoom);
		yOffset = yOffset * (1.0 / zoom);

		PMMatrix matrix(	fontSize,			0.0, 
							0.0, 				fontSize, 
							0.0, 				0.0 
						);

		InterfacePtr<IFontInstance> fontInst(fontMgr->QueryFontInstance(theFont, matrix));
		if (fontInst == nil)
			break;

		// Now measure the length of our string using the glyphs from the font.
        PMReal width, height;
        fontInst->MeasureWText(buff, numUTF16, width, &height);
		if(height == 0)
			// From top of ascender to bottom of descender
        	height = fontInst->GetAscent();
		
		// Set up the port.
		IGraphicsPort* gPort = gd->GetGraphicsPort();
		if (gPort == nil)
			break;

		gPort->gsave();

		InterfacePtr<ITransform> itemTransform (itemGeometry, UseDefaultIID());
		PMReal rotationAngle = itemTransform->GetItemRotationAngle();
		PMReal yScale = itemTransform->GetItemScaleY();

		if(yScale < 0){
			gPort->scale(1,-1);
			if(rotationAngle == 180)
				gPort->rotate(-180);
		}

		PMPoint leftTop = itemGeometry->GetStrokeBoundingBox().LeftTop();
		PMPoint rightBottom = itemGeometry->GetStrokeBoundingBox().RightBottom();

		PMRect fillBoundsRect = PMRect(leftTop.X(), leftTop.Y(), rightBottom.X(), rightBottom.Y());
		gPort->newpath();
		gPort->setrgbcolor(0/256.0, 0/256.0, 0/256.0); // black fill
		gPort->setlinewidth(2);
		gPort->rectpath(fillBoundsRect);
		gPort->stroke();
		gPort->grestore();

	} while(kFalse);
}

/* Returns the bounds of the adornment in view co-ordinates.
*/
PMRect XPGUIPlacedFormeFooterAdornment::GetPaintedAdornmentBounds(IShape* iShape, AdornmentDrawOrder	drawOrder, const PMRect& itemBounds, const PMMatrix& innertoview){
	
	PMRect result = itemBounds;
	do{
		InterfacePtr<IFormeData> formeData (iShape, UseDefaultIID());
		if(formeData == nil)
			break;

		PMString formeName = kNullString, dummy = kNullString;
		Utils<IXPageUtils>()->SplitFormeDescription(formeData->GetUniqueName(), dummy, formeName);
		if(formeName == kNullString)
			break;	

		int32 numUTF16;
		const textchar * buff = formeName.GrabUTF16Buffer(&numUTF16);
	
		// The labels are created using the default font.
		InterfacePtr<IFontMgr> fontMgr(GetExecutionContextSession(), UseDefaultIID());
		if (fontMgr == nil)
			break;

		InterfacePtr<IPMFont> theFont(fontMgr->QueryFont(fontMgr->GetDefaultFontName()));
		InterfacePtr<IGeometry> itemGeometry(iShape, UseDefaultIID());
		InterfacePtr<IHierarchy> itemHier(iShape, UseDefaultIID());
		if (!theFont || !itemGeometry || !itemHier)
			break;

		PMReal fontSize = 10;
		PMReal xOffset = 1.5, yOffset = 1.5; 
		PMReal xBoxOffset = 4.0, yBoxOffset = 2.0;
		PMReal zoom = 1.0;
 
		// Check current zoom factor and increade font size if necessary
		InterfacePtr<ILayoutControlData> layoutControlData (Utils<ILayoutUIUtils>()->QueryFrontLayoutData());
		InterfacePtr<IPanorama> panorama (layoutControlData, UseDefaultIID());
		if(panorama)
			zoom = panorama->GetXScaleFactor(); 

		fontSize = fontSize * (1.0 / zoom);
		xOffset = xOffset * (1.0 / zoom);
		yOffset = yOffset * (1.0 / zoom);

		PMMatrix matrix(	fontSize,	0.0, 
							0.0, 		fontSize, 
							0.0, 		0.0 
						);

		InterfacePtr<IFontInstance> fontInst(fontMgr->QueryFontInstance(theFont, matrix));
		if (fontInst == nil)
			break;

		// Now measure the length of our string using the glyphs from the font.
        PMReal width, height;
		fontInst->MeasureWText(buff, numUTF16, width, &height);

		if(height == 0) 
			// From top of ascender to bottom of descender
        	height = fontInst->GetAscent();		 

		result.Right() = itemBounds.Left() -  width - (2 * xOffset) - xBoxOffset - 1;
		result.Bottom() = itemBounds.Top() - height - (2 * yOffset) - yBoxOffset - 1;

	} while(kFalse);

	return result;
}

PMRect XPGUIPlacedFormeFooterAdornment::GetPrintedAdornmentBounds(IShape *  iShape, AdornmentDrawOrder  drawOrder, const PMRect &  itemBounds,  const PMMatrix &  innertoview)
{
	 PMRect result = itemBounds; 
     return result;
}

/*
*/
void XPGUIPlacedFormeFooterAdornment::AddToContentInkBounds(IShape* iShape, PMRect* inOutBounds){
	this->formeItems.push_back(::GetUIDRef(iShape));
}

/*	Returning a default value of 0.0 means the order is unimportant relative to 
	other "standard" priority adornments.
*/
PMReal XPGUIPlacedFormeFooterAdornment::GetPriority(){
	return 0.0;
}

/*	
*/
void XPGUIPlacedFormeFooterAdornment::Inval(IShape* iShape, IAdornmentShape::AdornmentDrawOrder drawOrder, GraphicsData* gd, ClassID reasonForInval, int32 flags){
}

/*
*/
bool16 XPGUIPlacedFormeFooterAdornment::WillPrint(){
	return kFalse;
}

/*
*/
bool16 XPGUIPlacedFormeFooterAdornment::WillDraw(IShape* iShape, AdornmentDrawOrder drawOrder, GraphicsData* gd, int32 flags){
	bool16 result = kFalse;
	do {
		if (drawOrder != kAfterShape)  
			break;		

		if (flags & IShape::kPrinting)  
			break;			
		result = kTrue;

	} while(false);

	return result;
}

PMPoint XPGUIPlacedFormeFooterAdornment::GetLeftTopPoint(IGeometry * itemGeo){

	PMRect rect = itemGeo->GetStrokeBoundingBox();
	
	InterfacePtr<ITransform> itemTransform (itemGeo, UseDefaultIID());
	PMReal rotationAngle = itemTransform->GetItemRotationAngle();
	PMReal yScale = itemTransform->GetItemScaleY();

	PMPoint leftTop;
	if(rotationAngle == 360)
		rotationAngle = 0;
	
	if(rotationAngle >= 0 && rotationAngle < 90)
		leftTop = rect.LeftTop();	
	else if(rotationAngle >= 90 && rotationAngle < 180)
		leftTop = rect.LeftBottom();	
	else if(rotationAngle >= 180 && rotationAngle < 270)
		leftTop = rect.RightBottom();	
	else 
		leftTop = rect.RightTop();
	return leftTop;
}

/*
	@desc Get forme name from unique forme description
*/

PMString XPGUIPlacedFormeFooterAdornment::SplitFormeDescription(const PMString& uniqueName){

	PMString formeName = kNullString;
	do{			
		K2::scoped_ptr<PMString> name (uniqueName.GetItem ("_", 2)); // Forme name in second position 
		if(name == nil)
			break;
		
		formeName = PMString(*name);

	}while(kFalse);
	return formeName;
}

UIDRef XPGUIPlacedFormeFooterAdornment::GetLeftTopMostItem(const K2Vector<UIDRef>& items){
	
	UIDRef leftTopMostItem = UIDRef::gNull;
	do{
		PMPoint leftTopMost(kMaxInt32, kMaxInt32);
		
		// Le point le plus haut ￠ gauche du carton
		for(int32 i = 0; i < items.size(); i++){
			UIDRef itemRef = items[i];
			InterfacePtr<IGeometry> itemGeo(itemRef, UseDefaultIID());
			if(!itemGeo)
				continue;

			PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();
			::TransformInnerPointToPasteboard(itemGeo, &leftTop);			
			if(leftTop.X() < leftTopMost.X()){
				leftTopMost.X() = leftTop.X();
				leftTopMostItem = itemRef;
			}
		}
	}while(kFalse);
	return leftTopMostItem;
}

bool16 XPGUIPlacedFormeFooterAdornment::HitTest(IShape *iShape,AdornmentDrawOrder adornmentDrawOrder, IControlView *layoutView, const PMRect &mouseRect)
{
	return kFalse;
}		
