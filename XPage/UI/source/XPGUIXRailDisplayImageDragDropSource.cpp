
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IDragDropController.h"
#include "IDataExchangeHandler.h"
#include "ISysFileListData.h"
#include "IPMDataObject.h"
#include "IControlView.h"
#include "ITreeViewController.h"
#include "IWidgetUtils.h"
#include "IStringData.h"

// General includes:
#include "CDragDropSource.h"
#include "FileUtils.h"
#include "DocumentID.h"
#include "Utils.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIImageNodeID.h"
#include "XPGImageDataNode.h"
#include "XPageUIUtils.h"

/** Drag source for the image preview of the Photos liées palette.

	The list rows already drag via XPGUIXRailImageDragDropSource (it sits on
	the tree node and reads the node's own ImageData). The preview panel is a
	GenericPanelWidget — it has no tree node of its own — so this source pulls
	the *currently selected* row from the sibling list widget
	(kXPGUIImagesViewWidgetID) and reads the exact same XPGUIImageNodeID
	ImageData. The drag payload built in doAddSysFileDragContent is therefore
	byte-for-byte identical to the list drag: same XPageImageFlavor, same
	ISysFileListData + crédit + légende + articleID. The drop side can't tell
	which widget started the drag.

	@ingroup paneltreeview
*/
class XPGUIXRailDisplayImageDragDropSource : public CDragDropSource
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIXRailDisplayImageDragDropSource(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XPGUIXRailDisplayImageDragDropSource();

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
	/** Gets the IDFile of the row currently selected in the sibling list.
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
CREATE_PMINTERFACE(XPGUIXRailDisplayImageDragDropSource, kXPGUIXRailDisplayImageDragDropSourceImpl)

/* Constructor
*/
XPGUIXRailDisplayImageDragDropSource::XPGUIXRailDisplayImageDragDropSource(IPMUnknown* boss)
							  :CDragDropSource(boss){}


/* Destructor
*/
XPGUIXRailDisplayImageDragDropSource::~XPGUIXRailDisplayImageDragDropSource(){}


/* WillDrag
*/
bool16 XPGUIXRailDisplayImageDragDropSource::WillDrag(IEvent* event) const
{
	IDFile contentFile;
	PMString legend, credit, articleId, cropData;
	return this->getContentFile(contentFile, legend, credit, articleId, cropData);
}


/* DoAddDragContent
*/
bool16 XPGUIXRailDisplayImageDragDropSource::DoAddDragContent(
	IDragDropController* controller)
{
	return this->doAddSysFileDragContent(controller);
}


/* getContentFile
*/
bool16 XPGUIXRailDisplayImageDragDropSource::getContentFile(
	IDFile& contentFile, PMString& legend, PMString& credit, PMString& articleId, PMString& cropData) const
{
	// The preview has no tree node of its own, so resolve the photo from the
	// row currently highlighted in the list (kXPGUIImagesViewWidgetID) — the
	// same row whose ImageData populated this preview. We read the exact same
	// XPGUIImageNodeID::GetImageData() the list drag source reads, so the
	// resulting drag is identical.
	bool16 result = kFalse;
	do {
		InterfacePtr<IControlView> previewWidget(this, UseDefaultIID());
		if (previewWidget == nil)
			break;

		InterfacePtr<ITreeViewController> imageListController(
			(ITreeViewController*)Utils<IWidgetUtils>()->QueryRelatedWidget(
				previewWidget, kXPGUIImagesViewWidgetID, IID_ITREEVIEWCONTROLLER));
		if (imageListController == nil)
			break;

		NodeIDList selectedItems;
		imageListController->GetSelectedItems(selectedItems);
		if (selectedItems.size() == 0)
			break;

		TreeNodePtr<XPGUIImageNodeID> photoNodeID(selectedItems[0]);
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
bool16 XPGUIXRailDisplayImageDragDropSource::doAddSysFileDragContent(IDragDropController* controller){
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

// End, XPGUIXRailDisplayImageDragDropSource.cpp.
