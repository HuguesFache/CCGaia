/*
//	File:	XPGUIFormeAdornmentShape.cpp
//
//	Date:	16-avr-2008
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
#include "IFormeData.h"

// General includes:
#include "TransformUtils.h"
#include "Utils.h"
#include "AutoGSave.h"
#ifdef DEBUG
#include "DebugClassUtils.h"
#endif

#include "CAlert.h"

// Project includes:
#include "XPGUIID.h"


/** Implementation of IAdornmentShape 
*/
class XPGUIFormeAdornmentShape : public CPMUnknown<IAdornmentShape>
{
	public:
		/** Constructor.
			@param boss
		*/
		XPGUIFormeAdornmentShape(IPMUnknown* boss);

		/** Destructor.
		*/
		virtual ~XPGUIFormeAdornmentShape();	
		
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
				IShape* 			iShape,
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

	private:

		PMPoint GetLeftTopPoint(IGeometry * itemGeo);
		PMString GetDisplayString(IShape* iShape); // Return a string representing forme type of the item, nothing if not a forme item
};

/*
*/
CREATE_PMINTERFACE(XPGUIFormeAdornmentShape, kXPGUIFormeAdornmentShapeImpl)

/*
*/
XPGUIFormeAdornmentShape::XPGUIFormeAdornmentShape(IPMUnknown* boss) : CPMUnknown<IAdornmentShape>(boss)
{
}

/*
*/
XPGUIFormeAdornmentShape::~XPGUIFormeAdornmentShape()
{
}

/*
*/
IAdornmentShape::AdornmentDrawOrder XPGUIFormeAdornmentShape::GetDrawOrderBits()
{
	return kAfterShape;
}

/*
*/
void XPGUIFormeAdornmentShape::DrawAdornment(IShape* iShape, IAdornmentShape::AdornmentDrawOrder drawOrder, 
			GraphicsData* gd, int32 flags)
{
	do {
	
		if (drawOrder != kAfterShape) {
			break;
		}

		if (flags & IShape::kPrinting) {
			break;
		}
		
		// Get the forme data of the item and make it a string
		PMString display = GetDisplayString(iShape);
		if(display == kNullString)
			break;

		int32 numUTF16;
		const textchar * buff = display.GrabUTF16Buffer(&numUTF16);
	
		// The labels are created using the default font.
		InterfacePtr<IFontMgr> fontMgr(GetExecutionContextSession(), UseDefaultIID());
		if (fontMgr == nil)
			break;

		InterfacePtr<IPMFont> theFont(fontMgr->QueryFont(fontMgr->GetDefaultFontName()));
		InterfacePtr<IGeometry> itemGeometry(iShape, UseDefaultIID());
		InterfacePtr<IHierarchy> itemHier(iShape, UseDefaultIID());
		if (theFont == nil || itemGeometry == nil || itemHier == nil)
			break;

		PMReal fontSize = 14.0;
		PMReal xOffset = 2.5, yOffset = 2.0;
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
        fontInst->MeasureWText(display.GrabUTF16Buffer(nil), display.WCharLength(), width, &height);

		 if(height == 0)
        {
			// From top of ascender to bottom of descender
        	height = fontInst->GetAscent();
		}

		// Set up the port.
		IGraphicsPort* gPort = gd->GetGraphicsPort();
		if (gPort == nil)
			break;

		AutoGSave autoGSave(gPort);

		InterfacePtr<ITransform> itemTransform (itemGeometry, UseDefaultIID());

		PMReal rotationAngle = itemTransform->GetItemRotationAngle();
		PMReal yScale = itemTransform->GetItemScaleY();

		if(yScale < 0)
		{
			gPort->scale(1,-1);
			if(rotationAngle == 180)
				gPort->rotate(-180);
		}

		PMPoint leftTop = itemGeometry->GetStrokeBoundingBox().LeftTop();

		PMReal left = leftTop.X() + xBoxOffset;
		PMReal right = left + width + (2 * xOffset);
		PMReal top = leftTop.Y() + yBoxOffset;
		PMReal bottom = top + height + (2 * yOffset);
		
		PMRect boundsRect = PMRect(left,top,right,bottom);

		gPort->newpath();
		gPort->setrgbcolor(255/256.0, 252/256.0, 191/256.0 ); // Yellow fill
		gPort->rectpath(boundsRect);
		gPort->fill();


		gPort->rectpath(boundsRect);
		gPort->setrgbcolor( 1.0, 0.0, 0.0 ); // Red border
		gPort->setlinewidth(0);
		gPort->stroke();
	
		PMReal x = boundsRect.Left() + xOffset; 
		PMReal y = boundsRect.Top() + height + yOffset; 

		// Set the drawing color for the port to a special value for the label
		gPort->setrgbcolor(0, 0, 0);

		// Set the font in the port
		gPort->selectfont(theFont, fontSize);

		// Draw the string in the port
		gPort->show(x, y, numUTF16, buff);

	} while(false);


}

/* Returns the bounds of the adornment in view co-ordinates.
*/
PMRect XPGUIFormeAdornmentShape::GetPaintedAdornmentBounds 
(
	IShape*				iShape,
	AdornmentDrawOrder	drawOrder,
	const PMRect&		itemBounds,  
	const PMMatrix&		innertoview
)
{
	PMRect result = itemBounds;
	do
	{
		// Get the forme data of the item and make it a string
		PMString display = GetDisplayString(iShape);
		if(display == kNullString)
			break;

		int32 numUTF16;
		const textchar * buff = display.GrabUTF16Buffer(&numUTF16);
	
		// The labels are created using the default font.
		InterfacePtr<IFontMgr> fontMgr(GetExecutionContextSession(), UseDefaultIID());
		if (fontMgr == nil)
			break;

		InterfacePtr<IPMFont> theFont(fontMgr->QueryFont(fontMgr->GetDefaultFontName()));
		InterfacePtr<IGeometry> itemGeometry(iShape, UseDefaultIID());
		InterfacePtr<IHierarchy> itemHier(iShape, UseDefaultIID());
		if (theFont == nil || itemGeometry == nil || itemHier == nil)
			break;

		PMReal fontSize = 14;
		PMReal xOffset = 2.5, yOffset = 2.0;
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

		PMMatrix matrix(	fontSize,			0.0,
							0.0, 				fontSize,
							0.0, 				0.0
						);

		InterfacePtr<IFontInstance> fontInst(fontMgr->QueryFontInstance(theFont, matrix));
		if (fontInst == nil)
			break;

		// Now measure the length of our string using the glyphs from the font.
        PMReal width, height;
		fontInst->MeasureWText(display.GrabUTF16Buffer(nil), display.WCharLength(), width, &height);

		if(height == 0)
        {
			// From top of ascender to bottom of descender
        	height = fontInst->GetAscent();
		}
	
		result.Right() = itemBounds.Left() +  width  + (2 * xOffset) + xBoxOffset + 1;
		result.Bottom() = itemBounds.Top() +  height + (2 * yOffset) + yBoxOffset + 1;

	} while(kFalse);

	return result;
}


PMRect XPGUIFormeAdornmentShape::GetPrintedAdornmentBounds(IShape *  iShape, AdornmentDrawOrder  drawOrder, const PMRect &  itemBounds,  const PMMatrix &  innertoview)
{
	 PMRect result = itemBounds; 
     return result;
}

/*
*/
void XPGUIFormeAdornmentShape::AddToContentInkBounds(IShape* iShape, PMRect* inOutBounds)
{
}

/*	Returning a default value of 0.0 means the order is unimportant relative to 
	other "standard" priority adornments.
*/
PMReal XPGUIFormeAdornmentShape::GetPriority()
{
	return 0.0;
}

/*	
*/
void XPGUIFormeAdornmentShape::Inval(IShape* iShape, IAdornmentShape::AdornmentDrawOrder drawOrder, GraphicsData* gd, 
			ClassID reasonForInval, int32 flags)
{

}

/*
*/
bool16 XPGUIFormeAdornmentShape::WillPrint()
{
	return kFalse;
}

/*
*/
bool16 XPGUIFormeAdornmentShape::WillDraw
		(
			IShape* 			iShape,
			AdornmentDrawOrder	drawOrder,
			GraphicsData*		gd,
			int32			flags
		)
{
	bool16 result = kFalse;
	do {
		if (drawOrder != kAfterShape) {
			break;
		}

		if (flags & IShape::kPrinting) {
			break;
		}
		
		result = kTrue;

	} while(false);

	return result;
}

PMPoint XPGUIFormeAdornmentShape::GetLeftTopPoint(IGeometry * itemGeo)
{
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

PMString XPGUIFormeAdornmentShape::GetDisplayString(IShape* iShape)
{
	PMString display = kNullString;

	do
	{
		// Get the forme data of the item and make it a string
		InterfacePtr<IFormeData> formeData (iShape, UseDefaultIID());
		if(formeData == nil)
			break;

		int16 photoIndex = formeData->GetPhotoIndex();
		switch(formeData->GetType())
		{
			case IFormeData::kArticle : 
				display = kXPGUIArticleTypeKey; 
				break;
			case IFormeData::kPhoto : 
				display = kXPGUIPhotoTypeKey; 
				break;
			case IFormeData::kCredit : 
				display = kXPGUICreditKey; 
				break;
			case IFormeData::kLegend : 
				display = kXPGUILegendKey; 
				break;
            case IFormeData::kFixedContent :
                display = kXPGUIkFixedContentKey;
                break;
			default : break;
		}

		if(display == kNullString)
			break;

		display.Translate();
		if(photoIndex != -1)
		{
			display += " ";
			display.AppendNumber(photoIndex + 1);
		}

	} while(kFalse);

	return display;
}

bool16 XPGUIFormeAdornmentShape::HitTest(IShape *iShape,AdornmentDrawOrder adornmentDrawOrder, IControlView *layoutView, const PMRect &mouseRect)
{
	return kFalse;
}
