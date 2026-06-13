/*
//	File:	XPGUIPlacedFormeHeaderAdornment.cpp
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
class XPGUIPlacedFormeHeaderAdornment:public CPMUnknown<IAdornmentShape>{

	public:
		/** Constructor.
			@param boss
		*/
		XPGUIPlacedFormeHeaderAdornment(IPMUnknown* boss);

		/** Destructor.
		*/
		virtual ~XPGUIPlacedFormeHeaderAdornment();	
		
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
		virtual PMRect				  GetPaintedAdornmentBounds 
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
};

namespace {
	// Build the persistent header label : "{Classeur} / {Carton} / {Type N}".
	// Each component is dropped if empty, so a block missing the classeur
	// part still produces "{Carton} / {Type N}" or just "{Type N}".
	// {Type N} matches the creation-time format
	// (XPGUIFormeAdornmentShape::GetDisplayString) — e.g. "Photo 1",
	// "Légende 2", "Crédit 3", "Article", "Contenu Fixe".
	// Returns kNullString when the item carries no useful FormeData type
	// (the caller breaks early and the header isn't drawn).
	PMString BuildHeaderLabel(IShape* iShape) {
		PMString display = kNullString;
		do {
			InterfacePtr<IFormeData> formeData(iShape, UseDefaultIID());
			if (formeData == nil) break;

			// Type + index part — same format as the creation adornment
			// so the user sees identical wording during edit and after.
			PMString typeLabel = kNullString;
			const int16 photoIndex = formeData->GetPhotoIndex();
			switch (formeData->GetType()) {
				case IFormeData::kArticle :      typeLabel = kXPGUIArticleTypeKey; break;
				case IFormeData::kPhoto :        typeLabel = kXPGUIPhotoTypeKey;   break;
				case IFormeData::kCredit :       typeLabel = kXPGUICreditKey;      break;
				case IFormeData::kLegend :       typeLabel = kXPGUILegendKey;      break;
				case IFormeData::kFixedContent : typeLabel = kXPGUIkFixedContentKey; break;
				default : break;
			}
			if (typeLabel == kNullString) break;
			typeLabel.Translate();
			if (photoIndex != -1) {
				typeLabel += " ";
				typeLabel.AppendNumber(photoIndex + 1);
			}

			// Classeur + carton parts — extracted from the unique name.
			PMString classeurName, formeName;
			Utils<IXPageUtils>()->SplitFormeDescription(formeData->GetUniqueName(), classeurName, formeName);

			// Compose "{Classeur} / {Carton} / {Type N}" with graceful
			// fallback when one of the parts is empty.
			if (!classeurName.IsEmpty()) {
				display = classeurName;
				if (!formeName.IsEmpty()) {
					display += " / ";
					display += formeName;
				}
				display += " / ";
				display += typeLabel;
			}
			else if (!formeName.IsEmpty()) {
				display = formeName;
				display += " / ";
				display += typeLabel;
			}
			else {
				display = typeLabel;
			}
		} while (kFalse);

		return display;
	}
}

/*
*/
CREATE_PMINTERFACE(XPGUIPlacedFormeHeaderAdornment, kXPGUIPlacedFormeHeaderAdornmentImpl)

/*
*/
XPGUIPlacedFormeHeaderAdornment::XPGUIPlacedFormeHeaderAdornment(IPMUnknown* boss):CPMUnknown<IAdornmentShape>(boss){
	  
}

/*
*/
XPGUIPlacedFormeHeaderAdornment::~XPGUIPlacedFormeHeaderAdornment(){}

/*
*/
IAdornmentShape::AdornmentDrawOrder XPGUIPlacedFormeHeaderAdornment::GetDrawOrderBits(){
	return kAfterShape;
}

/*
*/
void XPGUIPlacedFormeHeaderAdornment::DrawAdornment(IShape* iShape, IAdornmentShape::AdornmentDrawOrder drawOrder,	GraphicsData* gd, int32 flags){

	do {	
		if (drawOrder != kAfterShape) 
			break;		 

		if (flags & IShape::kPrinting)  		 
			break;		 
		
		// Build the same "Type N" label as the creation-time adornment
		// (Photo 1, Légende 2, Crédit 3, Article, Contenu Fixe). The
		// previous header showed the carton name extracted from
		// IFormeData::GetUniqueName() — illisible sur des petits cadres.
		PMString label = BuildHeaderLabel(iShape);
		if (label == kNullString)
			break;

		int32 numUTF16;
		const textchar * buff = label.GrabUTF16Buffer(&numUTF16);

		// The labels are created using the default font.
		InterfacePtr<IFontMgr> fontMgr(GetExecutionContextSession(), UseDefaultIID());
		if (fontMgr == nil)
			break;

		InterfacePtr<IPMFont> theFont(fontMgr->QueryFont(fontMgr->GetDefaultFontName()));
		InterfacePtr<IGeometry> itemGeometry(iShape, UseDefaultIID());
		InterfacePtr<IHierarchy> itemHier(iShape, UseDefaultIID());
		if (theFont == nil || itemGeometry == nil || itemHier == nil)
			break;

		// Style harmonisé avec l'adornment de création (XPGUIFormeAdornmentShape) :
		// fond jaune, bordure rouge, texte noir, font 14, ancré DANS le bloc
		// en haut-gauche (et non plus au-dessus en blanc/noir/rouge font 10).
		PMReal fontSize = 14.0;
		PMReal xOffset = 2.5, yOffset = 2.0;
		PMReal xBoxOffset = 4.0, yBoxOffset = 2.0;
		PMReal zoom = 1.0;

		// Check current zoom factor and increase font size if necessary
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

		// Position : DANS le bloc, ancré top-left (yBoxOffset px de marge
		// depuis le haut, xBoxOffset px depuis la gauche). Identique à
		// XPGUIFormeAdornmentShape::DrawAdornment.
		PMReal left = leftTop.X() + xBoxOffset;
		PMReal right = left + width + (2 * xOffset);
		PMReal top = leftTop.Y() + yBoxOffset;
		PMReal bottom = top + height + (2 * yOffset);

		PMRect boundsRect = PMRect(left, top, right, bottom);
		gPort->newpath();
		gPort->setrgbcolor(255/256.0, 252/256.0, 191/256.0); // yellow fill
		gPort->rectpath(boundsRect);
		gPort->fill();

		gPort->rectpath(boundsRect);
		gPort->setrgbcolor(1.0, 0.0, 0.0); // red border
		gPort->setlinewidth(0);
		gPort->stroke();

		PMReal x = boundsRect.Left() + xOffset;
		PMReal y = boundsRect.Top() + height + yOffset;

		// Set the drawing color for the port to a special value for the label
		gPort->setrgbcolor(0, 0, 0); // black text

		// Set the font in the port
		gPort->selectfont(theFont, fontSize);

		// Draw the string in the port
		gPort->show(x, y, numUTF16, buff);
		gPort->grestore();

	} while(kFalse);
}

/* Returns the bounds of the adornment in view co-ordinates.
*/
PMRect XPGUIPlacedFormeHeaderAdornment::GetPaintedAdornmentBounds(IShape* iShape, AdornmentDrawOrder	drawOrder, const PMRect& itemBounds, const PMMatrix& innertoview){
	
	PMRect result = itemBounds;
	do{
		// Same label logic as DrawAdornment — keep these in sync so
		// the bounds match the actual drawn text width.
		PMString label = BuildHeaderLabel(iShape);
		if (label == kNullString)
			break;

		int32 numUTF16;
		const textchar * buff = label.GrabUTF16Buffer(&numUTF16);

		// The labels are created using the default font.
		InterfacePtr<IFontMgr> fontMgr(GetExecutionContextSession(), UseDefaultIID());
		if (fontMgr == nil)
			break;

		InterfacePtr<IPMFont> theFont(fontMgr->QueryFont(fontMgr->GetDefaultFontName()));
		InterfacePtr<IGeometry> itemGeometry(iShape, UseDefaultIID());
		InterfacePtr<IHierarchy> itemHier(iShape, UseDefaultIID());
		if (!theFont || !itemGeometry || !itemHier)
			break;

		// Match DrawAdornment offsets — see comment there.
		PMReal fontSize = 14.0;
		PMReal xOffset = 2.5, yOffset = 2.0;
		PMReal xBoxOffset = 4.0, yBoxOffset = 2.0;
		PMReal zoom = 1.0;

		// Check current zoom factor and increase font size if necessary
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

		// Painted area extends INWARD from itemBounds top-left (the label
		// sits inside the block, like the creation adornment).
		result.Right() = itemBounds.Left() +  width  + (2 * xOffset) + xBoxOffset + 1;
		result.Bottom() = itemBounds.Top() +  height + (2 * yOffset) + yBoxOffset + 1;

	} while(kFalse);

	return result;
}

PMRect XPGUIPlacedFormeHeaderAdornment::GetPrintedAdornmentBounds(IShape *  iShape, AdornmentDrawOrder  drawOrder, const PMRect &  itemBounds,  const PMMatrix &  innertoview)
{
	 PMRect result = itemBounds; 
     return result;
}

/*
*/
void XPGUIPlacedFormeHeaderAdornment::AddToContentInkBounds(IShape* iShape, PMRect* inOutBounds){
}

/*	Returning a default value of 0.0 means the order is unimportant relative to 
	other "standard" priority adornments.
*/
PMReal XPGUIPlacedFormeHeaderAdornment::GetPriority(){
	return 0.0;
}

/*	
*/
void XPGUIPlacedFormeHeaderAdornment::Inval(IShape* iShape, IAdornmentShape::AdornmentDrawOrder drawOrder, GraphicsData* gd, ClassID reasonForInval, int32 flags){
}

/*
*/
bool16 XPGUIPlacedFormeHeaderAdornment::WillPrint(){
	return kFalse;
}

/*
*/
bool16 XPGUIPlacedFormeHeaderAdornment::WillDraw(IShape* iShape, AdornmentDrawOrder drawOrder, GraphicsData* gd, int32 flags){
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

PMPoint XPGUIPlacedFormeHeaderAdornment::GetLeftTopPoint(IGeometry * itemGeo){

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

PMString XPGUIPlacedFormeHeaderAdornment::SplitFormeDescription(const PMString& uniqueName){

	PMString formeName = kNullString;
	do{			
		K2::scoped_ptr<PMString> name (uniqueName.GetItem ("_", 2)); // Forme name in second position 
		if(name == nil)
			break;

		formeName = PMString(*name);

	}while(kFalse);
	return formeName;
 }  
 
bool16 XPGUIPlacedFormeHeaderAdornment::HitTest(IShape *iShape,AdornmentDrawOrder adornmentDrawOrder, IControlView *layoutView, const PMRect &mouseRect)
{
	return kFalse;
}