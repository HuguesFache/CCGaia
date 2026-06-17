
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
#include "CAlert.h"
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

/** Oversampling factor for carton thumbnails.
	The preview buffer is allocated at widget_size * this_factor, and scaled
	back to widget size by AGM during draw. This produces a smoother result
	than asking the JPEG import provider to rescale directly to widget size.
*/
static const uint32 kXPGUIThumbnailOversampleFactor = 3;


/** Implements IControlView; provides custom control view for a widget.
*/

class XPGUIDisplayImageFormePanelView : public DVControlView
{
public:
	/** Constructor
	@param boss interface on boss object on which interface is being aggregated
	*/
	XPGUIDisplayImageFormePanelView(IPMUnknown* boss);

	/** Destructor
	*/
	virtual ~XPGUIDisplayImageFormePanelView();	

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
};


/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PERSIST_PMINTERFACE(XPGUIDisplayImageFormePanelView, kXPGUIDisplayImageFormePanelViewImpl)


/* Constructor
*/
XPGUIDisplayImageFormePanelView::XPGUIDisplayImageFormePanelView(IPMUnknown* boss)
    : DVControlView(boss),
      fpCurAGMImage(nil),
	  fDataBuffer(nil),
	  fCachedImWidth(0),
	 fCachedImHeight(0)
{
}


/* Destructor
*/
XPGUIDisplayImageFormePanelView::~XPGUIDisplayImageFormePanelView()
{
    deleteBuffers();
    
}

void XPGUIDisplayImageFormePanelView::deleteBuffers()
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
void XPGUIDisplayImageFormePanelView::Init(
	const WidgetID& widgetId, const PMRect& frame, RsrcID rsrcID)
{
	DVControlView::Init(widgetId, frame, rsrcID);
}


/* Draw
*/

void XPGUIDisplayImageFormePanelView::Draw(IViewPort* viewPort, SysRgn updateRgn)
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
			// The image is stored oversampled (see kXPGUIThumbnailOversampleFactor).
			// Compute the final displayed size after the port scales it down.
			const PMReal displayScale = PMReal(1) / PMReal(kXPGUIThumbnailOversampleFactor);
			PMReal imageWidth  = (fpCurAGMImage->bounds.xMax - fpCurAGMImage->bounds.xMin) * displayScale;
			PMReal imageHeight = (fpCurAGMImage->bounds.yMax - fpCurAGMImage->bounds.yMin) * displayScale;
			PMReal xOffset = frame.GetHCenter() - imageWidth/2;
			PMReal yOffset = frame.GetVCenter() - imageHeight/2;
			// Centered, with graphics-port scaling so the oversampled buffer is
			// resampled by AGM (better quality than letting the import provider
			// rescale directly to widget size).
			gPort->translate(xOffset, yOffset);
			gPort->scale(displayScale, displayScale);
			PMMatrix theMatrix;	// No transform
			ASSERT(fpCurAGMImage);
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

ErrorCode XPGUIDisplayImageFormePanelView::createPreview(
	const IDFile& previewFile, 
	uint32 width, 
	uint32 height, 
	uint8 backGrey
	)
{
	// Copie assainie du fichier (APP1/Exif retire). Doit rester vivante tant que
	// le flux memoire ci-dessous est utilise -> declaree avant fileStream.
	std::vector<char> sanitizedBuf;
	const bool16 haveSanitized = XPGUIReadFileStrippingExif(previewFile, sanitizedBuf);

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

					// Oversample the preview so AGM can do the final downscale to
					// widget size with proper smoothing. Writing the preview
					// directly at widget dimensions forces the JPEG import
					// provider to rescale with a low-quality sampler, which
					// produces visibly jagged thumbnails.
					const uint32 hiresWidth  = width  * kXPGUIThumbnailOversampleFactor;
					const uint32 hiresHeight = height * kXPGUIThumbnailOversampleFactor;

					bool16 reallocateNeeded = kTrue;
					if( (this->fCachedImHeight == static_cast<int32>(hiresHeight)) &&
						(this->fCachedImWidth == static_cast<int32>(hiresWidth))) {
							reallocateNeeded = kFalse;
						}

					if(reallocateNeeded) {
						// Trash any existing storage
						this->deleteBuffers();

						fpCurAGMImage = new AGMImageRecord;
						memset (fpCurAGMImage, 0, sizeof(AGMImageRecord));
						fpCurAGMImage->bounds.xMin 			= 0;
						fpCurAGMImage->bounds.yMin 			= 0;
						fpCurAGMImage->bounds.xMax 			= hiresWidth;
						fpCurAGMImage->bounds.yMax 			= hiresHeight;
						fpCurAGMImage->byteWidth 			= 3*hiresWidth;
						fpCurAGMImage->colorSpace 			= kAGMCsRGB;
						fpCurAGMImage->bitsPerPixel 		= 24;
						fpCurAGMImage->decodeArray 			= 0;
						fpCurAGMImage->colorTab.numColors 	= 0;
						fpCurAGMImage->colorTab.theColors 	= nil;

						this->fDataBuffer =	new uint8[((fpCurAGMImage->byteWidth) * hiresHeight)];
						ASSERT(this->fDataBuffer);
						fpCurAGMImage->baseAddr = static_cast<void *>(this->fDataBuffer);

						this->fCachedImHeight = hiresHeight;
						this->fCachedImWidth  = hiresWidth;
					}
					ASSERT(fpCurAGMImage);
					//set the background to be grey
					::memset(fpCurAGMImage->baseAddr, backGrey, (fpCurAGMImage->byteWidth) * hiresHeight);

					if (fpCurAGMImage->baseAddr) {
						AcquireWaitCursor busyCursor;
						preview->Create24bitRGBPreview( (uint8*)fpCurAGMImage->baseAddr,
							hiresWidth, hiresHeight, fileStream, kTrue );
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
