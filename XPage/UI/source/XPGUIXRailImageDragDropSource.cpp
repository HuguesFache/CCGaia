
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IDragDropController.h"
#include "IDataExchangeHandler.h"
#include "ISysFileListData.h"
#include "IPMDataObject.h"
#include "IControlView.h"
#include "ITreeNodeIDData.h"
#include "IStringData.h"

// General includes:
#include "CDragDropSource.h"
#include "FileUtils.h"
#include "DocumentID.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIImageNodeID.h"
#include "XPGImageDataNode.h"
#include "XPageUIUtils.h"

/** Provides the node with drag source capability for the IDFile
	(kPMSysFileFlavor) associated with the widget. This allows the
	IDFile selected in a tree view node to be dragged from the
	panel onto a drop target that accepts a IDFile. For example
	a layout view on a document.

	@ingroup paneltreeview
	
*/
class XPGUIXRailImageDragDropSource : public CDragDropSource
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIXRailImageDragDropSource(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XPGUIXRailImageDragDropSource();

	/** Returns kTrue if a IDFile is associated with the widget.
		@param event [IN] that signifies the drag.
		@return kTrue if a drag can be initiated, otherwise kFalse.
	*/
	bool16 WillDrag(IEvent* event) const;

	/** Adds the IDFile to the drag.
		@param controller [IN] in charge of this widget's DND capabilities.
		@return kTrue if valid content has been added to the drag, kFalse otherwise.
	*/
	bool16 DoAddDragContent(IDragDropController* controller);

private:
	/** Gets the IDFile associated with this widget.
		@param contentFile [OUT] specifies associated IDFile.
		@param legend [OUT] photo legend text from the article description.
		@param credit [OUT] photo credit text from the article description.
		@param articleId [OUT] id of the article the dragged photo belongs to.
		@return kTrue if a draggable IDFile can be identified, kFalse otherwise.
	 */
	bool16 getContentFile(IDFile& contentFile, PMString& legend, PMString& credit, PMString& articleId, PMString& cropData) const;


	/** Adds kPMSysFileFlavor data to the drag describing the IDFile associated
		with this widget.
		@param controller [IN] in charge of this widget's DND capabilities.
		@return kTrue if valid content has been added to the drag, kFalse otherwise.
	 */
	bool16 doAddSysFileDragContent(IDragDropController* controller);
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGUIXRailImageDragDropSource, kXPGUIXRailImageDragDropSourceImpl)

/* Constructor
*/
XPGUIXRailImageDragDropSource::XPGUIXRailImageDragDropSource(IPMUnknown* boss)
							  :CDragDropSource(boss){}


/* Destructor
*/
XPGUIXRailImageDragDropSource::~XPGUIXRailImageDragDropSource(){}


/* WillDrag
*/
bool16 XPGUIXRailImageDragDropSource::WillDrag(IEvent* event) const
{
	IDFile contentFile;
	PMString legend, credit, articleId, cropData;
	return this->getContentFile(contentFile, legend, credit, articleId, cropData);
}


/* DoAddDragContent
*/
bool16 XPGUIXRailImageDragDropSource::DoAddDragContent(
	IDragDropController* controller)
{
	return this->doAddSysFileDragContent(controller);
}


/* getContentFile
*/
bool16 XPGUIXRailImageDragDropSource::getContentFile(
	IDFile& contentFile, PMString& legend, PMString& credit, PMString& articleId, PMString& cropData) const
{
	// The legacy path read everything from the Articles palette tree (it
	// queried `kXPGUIArticleViewWidgetID` via QueryRelatedWidget and walked
	// the article ImageList). That dependency is gone now that the Photos
	// liées palette is selection-driven (XPGUIXRailImagesPanelObserver),
	// and dereferencing the controller without a null check was crashing
	// in WillDrag every time the user clicked a row when the Articles
	// palette wasn't open. We read everything from the photo node we sit
	// on — its ImageData carries the HR path, legend, credit, and
	// articleID (populated by the OBJRART SAX handler + the model).
	bool16 result = kFalse;
	do {
		InterfacePtr<ITreeNodeIDData> nodeData(this, UseDefaultIID());
		if (nodeData == nil)
			break;

		TreeNodePtr<XPGUIImageNodeID> photoNodeID(nodeData->Get());
		if (!photoNodeID)
			break;
		if (photoNodeID->GetType() != XPGImageDataNode::kImageNode)
			break; // rubrique row — nothing draggable.

		const XPGImageDataNode::ImageData* img = photoNodeID->GetImageData();
		if (img == nil)
			break;

		IDFile imageHRFile = FileUtils::PMStringToSysFile(img->hrFile);
		if (!FileUtils::DoesFileExist(imageHRFile))
			break;

		contentFile = imageHRFile;
		legend      = img->legend;
		credit      = img->credit;
		articleId   = img->articleID;
		// Crop rect + chemin BR, transportes pour etre appliques au drop.
		cropData    = XPageUIUtils::PackCropPayload(img->cropX, img->cropY, img->cropW, img->cropH, img->brFile);

		result = kTrue;
	} while (false);
	return result;
}

/* doAddSysFileDragContent
*/
bool16 XPGUIXRailImageDragDropSource::doAddSysFileDragContent(IDragDropController* controller){
	bool16 result = kFalse;
	do{
		// Stop if we can't determine the IDFile we are associated with.
		IDFile contentFile;
		PMString legend = kNullString, credit = kNullString, articleId = kNullString, cropData = kNullString;
		if (this->getContentFile(contentFile, legend, credit, articleId, cropData) == kFalse)
			break;

		// Point the controller at the handler.
		InterfacePtr<IDataExchangeHandler> dataExchangeHandler(controller->QueryHandler(XPageImageFlavor));
		if (dataExchangeHandler->IsEmpty() == kFalse)
			dataExchangeHandler->Clear();

		controller->SetSourceHandler(dataExchangeHandler);

		// Add the IDFile to be dragged.
		InterfacePtr<ISysFileListData> sysFileListData(dataExchangeHandler, IID_ISYSFILELISTDATA);
		sysFileListData->Append(contentFile);

		// Add the legend and credit of the photo
		InterfacePtr<IStringData> creditData(dataExchangeHandler, IID_ICREDITPHOTO);
		InterfacePtr<IStringData> legendData(dataExchangeHandler, IID_ILEGENDPHOTO);
		creditData->Set(credit);
		legendData->Set(legend);

		// Add the source article id so the drop target can detect a drop on
		// a frame that doesn't belong to this article and propagate the
		// article identifiers to it.
		InterfacePtr<IStringData> articleIdData(dataExchangeHandler, IID_IPHOTOARTICLEID);
		if (articleIdData != nil)
			articleIdData->Set(articleId);

		// Add the crop payload (crop rect + BR path) so the drop target can
		// crop the placed image instead of a plain proportional fit.
		InterfacePtr<IStringData> cropDataData(dataExchangeHandler, IID_IPHOTOCROPDATA);
		if (cropDataData != nil)
			cropDataData->Set(cropData);

		// Indicate the flavour in the drag object.
		InterfacePtr<IPMDataObject> pmDataObject(controller->AddDragItem(1));

		PMFlavorFlags flavorFlags = kNormalFlavorFlag;
		pmDataObject->PromiseFlavor(XPageImageFlavor, flavorFlags);

		result = kTrue;

	} while(false);

	return result;
}

// End, XPGUIXRailImageDragDropSource.cpp.
