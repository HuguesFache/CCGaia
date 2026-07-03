/*
//	File:	XPGUIDisplayImagePanelView.cpp
//
//	Date:	07-Juillet-2006
//
//  Author : Trias Developpement
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "AutoGSave.h"
#include "IGraphicsPort.h"
#include "IImageStream.h"
#include "IImportProvider.h"
#include "IImportPreview.h"
#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "IPMStream.h"
#include "ISysFileData.h"
#include "IInterfaceColors.h"

// General includes:
#include "AGMGraphicsContext.h"
#include "StreamUtil.h"
#include "GraphicsExternal.h"	// _t_AGM...
#include "DVControlView.h"
#include "ProgressBar.h"
#include "AcquireModalCursor.h"
// Project includes:
#include "XPGUIID.h"
#include "XPGUIStripExif.h"

#include <vector>

  
/** Color space family from AGM, subsetted to meet our needs here.
	@ingroup paneltreeview
*/
enum SubsettedAGMColorSpaceFamily
{
	/** */
	kDontCare,
	/** */
	kAGMCsRGB
	/** */
};


/** Implements IControlView; provides custom control view for a widget.
*/

class XPGUIDisplayImagePanelView : public DVControlView
{
public:
	/** Constructor
	@param boss interface on boss object on which interface is being aggregated
	*/
	XPGUIDisplayImagePanelView(IPMUnknown* boss);

	/** Destructor
	*/
	virtual ~XPGUIDisplayImagePanelView();	

	/** Called when widget is being initialised.
	@param widgetId [IN] specifies WidgetID to associate with this widget
	@param frame [IN] specifies bounding box for the control
	@param rsrcID [IN] specifies resource to associate with this widget
	*/
	virtual void Init(
		const WidgetID& widgetId, const PMRect& frame, RsrcID rsrcID);

	/** Called when widget should draw.
	@param viewPort [IN] specifies viewport
	@param updateRgn [IN] specifies update region
	*/
	 virtual void Draw(IViewPort* viewPort, SysRgn updateRgn);

private:
	void deleteBuffers();
	ErrorCode createPreview(
		const IDFile& previewFile, uint32 nWidthWidget, uint32 nHeightWidget, uint8 backGrey);

	AGMImageRecord *fpCurAGMImage;
	IDFile fCurImageSysFile;
	uint8* fDataBuffer;
	int32 fCachedImWidth;
	int32 fCachedImHeight;
	int32 fOrientation;		// tag EXIF Orientation 1..8 de l'image courante
};


/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PERSIST_PMINTERFACE(XPGUIDisplayImagePanelView, kXPGUIDisplayImagePanelViewImpl)


/* Constructor
*/
XPGUIDisplayImagePanelView::XPGUIDisplayImagePanelView(IPMUnknown* boss)
    : DVControlView(boss),
      fpCurAGMImage(nil),
	  fDataBuffer(nil),
	  fCachedImWidth(0),
	 fCachedImHeight(0),
	  fOrientation(1)
{
}


/* Destructor
*/
XPGUIDisplayImagePanelView::~XPGUIDisplayImagePanelView()
{
    deleteBuffers();
    
}

void XPGUIDisplayImagePanelView::deleteBuffers()
{
	if(fpCurAGMImage) {
		//delete (uint8 *)fpCurAGMImage->baseAddr;
		delete [] fDataBuffer;
		delete fpCurAGMImage;
		fpCurAGMImage = nil;
	}

	
}

/* Init
*/
void XPGUIDisplayImagePanelView::Init(
	const WidgetID& widgetId, const PMRect& frame, RsrcID rsrcID)
{
	DVControlView::Init(widgetId, frame, rsrcID);
}


/* Draw
*/

void XPGUIDisplayImagePanelView::Draw(IViewPort* viewPort, SysRgn updateRgn)
{
	
 	
	AGMGraphicsContext gc(viewPort, this, updateRgn);
	InterfacePtr<IGraphicsPort> gPort(gc.GetViewPort(), IID_IGRAPHICSPORT);	// IID_IGRAPHICSPORT);
	ASSERT(gPort);
	if(!gPort) {
		return;
	}

	AutoGSave autoGSave(gPort);

	do {
		ErrorCode resultOfCreatingPreview = kFailure;

		InterfacePtr<IInterfaceColors>
			iInterfaceColors(GetExecutionContextSession(), IID_IINTERFACECOLORS);
		ASSERT(iInterfaceColors);
		if(!iInterfaceColors){
			break;
		}

		RealAGMColor defaultGreyFill;
		iInterfaceColors->GetRealAGMColor(kInterfacePaletteFill, defaultGreyFill); 
		
		PMRect frame = GetFrame();
		frame.MoveTo(0, 0);             // get into local coordinates

		uint32 nWidthWidget=1;
		uint32 nHeightWidget=1;
	

		// Dropped the 3d border effect
		nWidthWidget = ToInt32(frame.Width());  
		nHeightWidget = ToInt32(frame.Height());
		
		// get filespec currently associated with widget
		InterfacePtr<ISysFileData> iImageSysFile(this, IID_ISYSFILEDATA);
		ASSERT(iImageSysFile);
		if(!iImageSysFile)
		{
			break;
		}
		IDFile previewFile = iImageSysFile->GetSysFile(); 
		
		if(previewFile != IDFile())
		{
			// Take average greyvalue of what we're filling background to
			uint8 backgroundGrey = ToInt32(Round(255.0* (defaultGreyFill.red +  defaultGreyFill.green + defaultGreyFill.blue)/3.0));
			// Create another preview, if we need to
			resultOfCreatingPreview = this->createPreview(previewFile, nWidthWidget, nHeightWidget, backgroundGrey);
		}
		else
		{
			resultOfCreatingPreview = kFailure;
			this->fCurImageSysFile = previewFile;
		}			
		
		// if we got an image, then display it
		// otherwise, draw a red diagonal line through the widget
		if (resultOfCreatingPreview == kSuccess) {
			// Fill the background with palette default background fill
			gPort->setrgbcolor(defaultGreyFill.red, defaultGreyFill.green, defaultGreyFill.blue);
			gPort->rectpath(frame);
			gPort->fill();
			// Image ancree en haut a gauche de la zone (0,0) et clippee a la zone :
			// l'apercu reste toujours dans son cadre, jamais sous les champs
			// credit/legende. La taille a deja ete ajustee au ratio dans
			// createPreview, donc l'image occupe l'espace disponible.
			gPort->rectclip(frame);
			ASSERT(fpCurAGMImage);
			// Rotation/miroir selon le tag EXIF Orientation. La matrice envoie le
			// rectangle du buffer (0,0,bw,bh) sur le rectangle affiche, ancre en
			// (0,0). PMMatrix(a,b,c,d,e,f) : x'=a*x+c*y+e, y'=b*x+d*y+f.
			const PMReal bw = fpCurAGMImage->bounds.xMax - fpCurAGMImage->bounds.xMin;
			const PMReal bh = fpCurAGMImage->bounds.yMax - fpCurAGMImage->bounds.yMin;
			PMMatrix theMatrix;	// orientation 1 : identite
			switch(fOrientation) {
				case 2: theMatrix = PMMatrix(-1, 0,  0, 1, bw,  0); break;	// miroir horizontal
				case 3: theMatrix = PMMatrix(-1, 0,  0,-1, bw, bh); break;	// 180 deg
				case 4: theMatrix = PMMatrix( 1, 0,  0,-1,  0, bh); break;	// miroir vertical
				case 5: theMatrix = PMMatrix( 0, 1,  1, 0,  0,  0); break;	// transpose
				case 6: theMatrix = PMMatrix( 0, 1, -1, 0, bh,  0); break;	// 90 deg horaire
				case 7: theMatrix = PMMatrix( 0,-1, -1, 0, bh, bw); break;	// transverse
				case 8: theMatrix = PMMatrix( 0,-1,  1, 0,  0, bw); break;	// 90 deg anti-horaire
				default: break;												// 1 : identite
			}
			gPort->image(fpCurAGMImage, theMatrix, 0);
		}
		else {
			// Fill with the palette default background fill
			gPort->setrgbcolor(defaultGreyFill.red, defaultGreyFill.green, defaultGreyFill.blue);
			gPort->rectpath(frame);
			gPort->fill();
		}	
	} while(kFalse);
}
/*
*/

ErrorCode XPGUIDisplayImagePanelView::createPreview(
	const IDFile& previewFile,
	uint32 width,
	uint32 height,
	uint8 backGrey
	)
{
	// Copie assainie du fichier (APP1/Exif retire). Doit rester vivante tant que
	// le flux memoire ci-dessous est utilise -> declaree avant fileStream.
	std::vector<char> sanitizedBuf;
	int32 nativeW = 0, nativeH = 0, orientation = 1;
	const bool16 haveSanitized = XPGUIReadFileStrippingExif(previewFile, sanitizedBuf, nativeW, nativeH, orientation);
	this->fOrientation = orientation;

	// Taille du buffer NON tourne (ce que Create24bitRGBPreview va remplir). On
	// inscrit l'image AFFICHEE (apres rotation EXIF) dans le widget en preservant
	// son ratio ; pour les orientations 5..8 (90/270deg) la largeur et la hauteur
	// affichees sont echangees. drawW/drawH restent les dimensions du buffer non
	// tourne ; la rotation finale est appliquee cote Draw.
	uint32 drawW = width;
	uint32 drawH = height;
	if(nativeW > 0 && nativeH > 0 && width > 0 && height > 0) {
		const bool swapWH = (orientation >= 5 && orientation <= 8);
		const double dispW = swapWH ? nativeH : nativeW;	// dimensions affichees
		const double dispH = swapWH ? nativeW : nativeH;
		const double sx = static_cast<double>(width)  / dispW;
		const double sy = static_cast<double>(height) / dispH;
		const double scale = (sx < sy) ? sx : sy;
		drawW = static_cast<uint32>(nativeW * scale + 0.5);
		drawH = static_cast<uint32>(nativeH * scale + 0.5);
		if(drawW < 1) drawW = 1;
		if(drawH < 1) drawH = 1;
	}

	do
	{
		// get source stream (image file to preview)
		InterfacePtr<IPMStream> fileStream(
			haveSanitized
				? StreamUtil::CreatePointerStreamRead(sanitizedBuf.data(), sanitizedBuf.size())
				: StreamUtil::CreateFileStreamRead(previewFile));

		if(fileStream == nil) {
			break;
		}
		InterfacePtr<IK2ServiceRegistry>	serviceRegistry(GetExecutionContextSession(), UseDefaultIID());
		ASSERT(serviceRegistry);
		if(!serviceRegistry) {
			break;
		}
		int	numHandlers = serviceRegistry->GetServiceProviderCount(kImportProviderService);
		for (int i = 0; i < numHandlers; i++) {

			InterfacePtr<IK2ServiceProvider> provider(
				serviceRegistry->QueryNthServiceProvider(kImportProviderService, i));
			InterfacePtr<IImportProvider> importProvider(provider, IID_IIMPORTPROVIDER);
			
			if (importProvider && 
				importProvider->CanImportThisStream(fileStream) == IImportProvider::kFullImport) {
			
				InterfacePtr<IImportPreview> preview(importProvider, IID_IIMPORTPREVIEW);
				if(preview) {
				

					
					bool16 reallocateNeeded = kTrue;
					// Realloc si la taille d'affichage (ajustee au ratio) a change.
					if( (this->fCachedImHeight == static_cast<int32>(drawH)) &&
						(this->fCachedImWidth == static_cast<int32>(drawW)) ) {
							reallocateNeeded = kFalse;
						}

					if(reallocateNeeded) {
						// Trash any existing storage
						this->deleteBuffers();

						fpCurAGMImage = new AGMImageRecord;
						memset (fpCurAGMImage, 0, sizeof(AGMImageRecord));
						fpCurAGMImage->bounds.xMin 			= 0;
						fpCurAGMImage->bounds.yMin 			= 0;
						fpCurAGMImage->bounds.xMax 			= drawW;
						fpCurAGMImage->bounds.yMax 			= drawH;
						fpCurAGMImage->byteWidth 			= 3*drawW; //BYTES2ROWBYTES(3*drawW);
						fpCurAGMImage->colorSpace 			= kAGMCsRGB;
						fpCurAGMImage->bitsPerPixel 		= 24;
						fpCurAGMImage->decodeArray 			= 0;
						fpCurAGMImage->colorTab.numColors 	= 0;
						fpCurAGMImage->colorTab.theColors 	= nil;

						this->fDataBuffer =	new uint8[((fpCurAGMImage->byteWidth) * drawH)];
						ASSERT(this->fDataBuffer);
						fpCurAGMImage->baseAddr = static_cast<void *>(this->fDataBuffer);

						this->fCachedImHeight = drawH;
						this->fCachedImWidth = drawW;
					}
					ASSERT(fpCurAGMImage);
					//set the background to be grey
					::memset(fpCurAGMImage->baseAddr, backGrey, (fpCurAGMImage->byteWidth) * drawH);

					if (fpCurAGMImage->baseAddr) {
						AcquireWaitCursor busyCursor;
						preview->Create24bitRGBPreview( (uint8*)fpCurAGMImage->baseAddr,
							drawW, drawH, fileStream, kTrue );
						fCurImageSysFile = previewFile;
						// Exit, we don't need another handler

						return kSuccess;
					}
				}
			}
			// Be sure to reset the stream if we're trying out another handler
			fileStream->Seek(0,kSeekFromStart);
		} // i loop over handlers

		

	} while(false);
	
	return kFailure;
}
