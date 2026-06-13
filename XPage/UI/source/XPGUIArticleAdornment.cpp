
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
#include "IArticleData.h"
#include "IEncodingUtils.h"

// General includes:
#include "TransformUtils.h"
#include "Utils.h"
#include "AutoGSave.h"
#ifdef DEBUG
	#include "DebugClassUtils.h"
#endif

#include "CAlert.h"
#include "PlatformChar.h"
#include "IEncodingUtils.h"
#include <CType.h>

// Project includes:
#include "XPGUIID.h"
#include <string> 
#include <sstream> 

/** Implementation of IAdornmentShape 
*/
class XPGUIArticleAdornment : public CPMUnknown<IAdornmentShape>
{
	public:
		/** Constructor.
			@param boss
		*/
		XPGUIArticleAdornment(IPMUnknown* boss);

		/** Destructor.
		*/
		virtual ~XPGUIArticleAdornment();	
		
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

		/** See IAdornmentShape::GetPrintedAdornmentBounds
		 */
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

	private :

		PMPoint GetLeftTopPoint(IGeometry * itemGeo);
		PMString GetDisplayString(IShape* iShape); 
};

/*
*/
CREATE_PMINTERFACE(XPGUIArticleAdornment, kXPGUIArticleAdornmentImpl)

/*
*/
XPGUIArticleAdornment::XPGUIArticleAdornment(IPMUnknown* boss) 
                      :CPMUnknown<IAdornmentShape>(boss)
{
}

/*
*/
XPGUIArticleAdornment::~XPGUIArticleAdornment(){}

/*
*/
IAdornmentShape::AdornmentDrawOrder XPGUIArticleAdornment::GetDrawOrderBits()
{
	return kAfterShape;
}

/*
*/
void XPGUIArticleAdornment::DrawAdornment(IShape* iShape, IAdornmentShape::AdornmentDrawOrder drawOrder, 
			GraphicsData* gd, int32 flags){
	
    do {
		
		if (drawOrder != kAfterShape) 
			break;		

		if (flags & IShape::kPrinting) 
			break;		
        
		// Get the forme data of the item and make it a string
        
        PMString display = GetDisplayString(iShape);
      

		if(display == kNullString || display == "")
			break;     
   
        PMString::ConversionError convError= PMString::kNoError;
        int32 decimalColor = display.GetAsNumber(&convError);
        
        // Conversion  decimal -> RGB  
        
        PMReal r  = ((decimalColor & 16711680 ) / 65536);  
        PMReal g  = ((decimalColor & 65280) / 256);
        PMReal b  = (decimalColor & 255);       
       
   
		InterfacePtr<IGeometry> itemGeometry(iShape, UseDefaultIID());
		InterfacePtr<IHierarchy> itemHier(iShape, UseDefaultIID());
		if (itemGeometry == nil || itemHier == nil)
			break;

        PMReal zoom = 1.0;
        PMReal fontSize = 10.0;
		PMReal xOffset = 1.5, yOffset = 1.5; 
		PMReal xBoxOffset = 4.0, yBoxOffset = 2.0;
	

		// Check current zoom factor and increade font size if necessary
		InterfacePtr<ILayoutControlData> layoutControlData (Utils<ILayoutUIUtils>()->QueryFrontLayoutData());
		InterfacePtr<IPanorama> panorama (layoutControlData, UseDefaultIID());
	
        if(panorama)
			zoom = panorama->GetXScaleFactor();		
    
        xOffset = xOffset * (1.0 / zoom);
		yOffset = yOffset * (1.0 / zoom);

		IGraphicsPort* gPort = gd->GetGraphicsPort();
		if (gPort == nil)
			break;

		AutoGSave autoGSave(gPort);

		InterfacePtr<ITransform> itemTransform (itemGeometry, UseDefaultIID());

		PMReal rotationAngle = itemTransform->GetItemRotationAngle();
		PMReal yScale = itemTransform->GetItemScaleY();

		if(yScale < 0){
			gPort->scale(1,-1);
			if(rotationAngle == 180)
				gPort->rotate(-180);
		}

		PMPoint leftTop    = itemGeometry->GetStrokeBoundingBox().LeftTop();
        PMPoint rightBottom = itemGeometry->GetStrokeBoundingBox().RightBottom ();

		PMReal left = leftTop.X();		
        PMReal right = rightBottom.X();
		
        PMReal top = leftTop.Y();
		PMReal bottom = rightBottom.Y();
		
		PMRect boundsRect = PMRect(left, top, right, bottom);

		gPort->newpath();
		gPort->setrgbcolor(r /256.0, g /256.0 , b /256.0);
		gPort->setopacity(0.4, kTrue); 
        gPort->rectpath(boundsRect);

		gPort->fill();

		gPort->rectpath(boundsRect);
		gPort->setrgbcolor(r / 256.0, g / 256.0, b / 256.0);
		gPort->setlinewidth(2);
		gPort->stroke();

		gPort->setrgbcolor(r / 256.0 ,g / 256.0, b / 256.0 );

	} while(false);


}

/* Returns the bounds of the adornment in view co-ordinates.
*/
PMRect XPGUIArticleAdornment::GetPaintedAdornmentBounds
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
		if(display == kNullString || display == "")
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

		PMReal xOffset = 1.5, yOffset = 1.5; 
		PMReal zoom = 1.0;

		// Check current zoom factor and increade font size if necessary
		InterfacePtr<ILayoutControlData> layoutControlData (Utils<ILayoutUIUtils>()->QueryFrontLayoutData());
		InterfacePtr<IPanorama> panorama (layoutControlData, UseDefaultIID());
		if(panorama)
			zoom = panorama->GetXScaleFactor();

    	xOffset  = xOffset  * (1.0 / zoom);
		yOffset  = yOffset  * (1.0 / zoom);



	} while(kFalse);

	return result;
}

PMRect XPGUIArticleAdornment::GetPrintedAdornmentBounds(IShape *  iShape, AdornmentDrawOrder  drawOrder, const PMRect &  itemBounds,  const PMMatrix &  innertoview)
{
	 PMRect result = itemBounds; 
     return result;
}
/*
*/
void XPGUIArticleAdornment::AddToContentInkBounds(IShape* iShape, PMRect* inOutBounds)
{
}

/*	Returning a default value of 0.0 means the order is unimportant relative to 
	other "standard" priority adornments.
*/
PMReal XPGUIArticleAdornment::GetPriority()
{
	return 0.0;
}

/*	
*/
void XPGUIArticleAdornment::Inval(IShape* iShape, IAdornmentShape::AdornmentDrawOrder drawOrder, GraphicsData* gd, 
			ClassID reasonForInval, int32 flags)
{

}

/*
*/
bool16 XPGUIArticleAdornment::WillPrint()
{
	return kFalse;
}

/*
*/
bool16 XPGUIArticleAdornment::WillDraw
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

PMPoint XPGUIArticleAdornment::GetLeftTopPoint(IGeometry * itemGeo){

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

PMString XPGUIArticleAdornment::GetDisplayString(IShape* iShape){

	PMString display = kNullString;
	do{
		// Get the forme data of the item and make it a string
		InterfacePtr<IArticleData> articleData (iShape, UseDefaultIID());
		if(articleData == nil)
			break;
        
		if (articleData->GetStatus() == "")
			break;

        display.Append(articleData->GetStatus());
	} while(kFalse);
	return display;
}

bool16 XPGUIArticleAdornment::HitTest(IShape *iShape,AdornmentDrawOrder adornmentDrawOrder, IControlView *layoutView, const PMRect &mouseRect)
{
	return kFalse;
}