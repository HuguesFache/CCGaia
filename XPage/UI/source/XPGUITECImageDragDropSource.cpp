
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IXPageMgrAccessor.h"
#include "IImageDataModel.h"
#include "IDragDropController.h"
#include "IDataExchangeHandler.h"
#include "ISysFileListData.h"
#include "IPMDataObject.h"
#include "ITreeViewController.h"
#include "IControlView.h"
#include "ITreeNodeIDData.h"
#include "IWidgetUtils.h"
#include "IIntData.h"
#include "IStringData.h"
#include "ITriStateControlData.h"

// General includes:
#include "CDragDropSource.h"
#include "FileUtils.h"
#include "DocumentID.h"
#include "Utils.h"
#include "CAlert.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIArticleNodeID.h"
#include "XPGUIImageNodeID.h"

/** Provides the node with drag source capability for the IDFile
	(kPMSysFileFlavor) associated with the widget. This allows the
	IDFile selected in a tree view node to be dragged from the
	panel onto a drop target that accepts a IDFile. For example
	a layout view on a document.

	@ingroup paneltreeview
	
*/
class XPGUITECImageDragDropSource : public CDragDropSource
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUITECImageDragDropSource(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XPGUITECImageDragDropSource();

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
		@return kTrue if a draggable IDFile can be identified, kFalse otherwise.
	 */
	bool16 getContentFile(IDFile& contentFile, PMString& legend, PMString& credit) const;


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
CREATE_PMINTERFACE(XPGUITECImageDragDropSource, kXPGUITECImageDragDropSourceImpl)

/* Constructor
*/
XPGUITECImageDragDropSource::XPGUITECImageDragDropSource(IPMUnknown* boss)
: CDragDropSource(boss)
{
}


/* Destructor
*/
XPGUITECImageDragDropSource::~XPGUITECImageDragDropSource()
{
}


/* WillDrag
*/
bool16 XPGUITECImageDragDropSource::WillDrag(IEvent* event) const
{
	IDFile contentFile;
	PMString legend, credit;
	return this->getContentFile(contentFile, legend, credit);
}


/* DoAddDragContent
*/
bool16 XPGUITECImageDragDropSource::DoAddDragContent(
	IDragDropController* controller)
{
	return this->doAddSysFileDragContent(controller);
}


/* getContentFile
*/
bool16 XPGUITECImageDragDropSource::getContentFile(
	IDFile& contentFile, PMString& legend, PMString& credit) const
{
	bool16 result = kFalse;
	do {
		// Get selected article XML file
		InterfacePtr<IControlView> imagePanelWidget (this, UseDefaultIID());
		InterfacePtr<ITreeViewController> articleListController ((ITreeViewController*)Utils<IWidgetUtils>()->QueryRelatedWidget(imagePanelWidget, kXPGUIArticleViewWidgetID, IID_ITREEVIEWCONTROLLER));

		NodeIDList selectedItems;
		articleListController->GetSelectedItems(selectedItems);

		if(selectedItems.size() == 0)
			break;

		TreeNodePtr<XPGUIArticleNodeID> articleNodeID(selectedItems[0]);
		IDFile articleFile = FileUtils::PMStringToSysFile(articleNodeID->GetArticleData()->artPath);

		// Get image's current index 		
		InterfacePtr<IIntData> imageIndexData (this, UseDefaultIID());
		int32 indexImg = imageIndexData->Get();

		// B&W checkbox was removed from the panel; always import images in colour.
		bool16 wantNB = kFalse;

		// Get image's high res file path from image model
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IImageDataModel> imageDataModel (pageMrgAccessor->QueryImageDataModel());
		IDFile imageHRFile;
		IDFile myFile = IDFile();
		if(!imageDataModel->GetImageData(articleNodeID->GetArticleData()->images[indexImg], articleFile, myFile, 
										wantNB, imageHRFile, legend, credit))
			break;

		if(!FileUtils::DoesFileExist(imageHRFile))
			break;	
		
		contentFile = imageHRFile;

		result = kTrue;

	} while(false);
	return result;
}

/* doAddSysFileDragContent
*/
bool16 XPGUITECImageDragDropSource::doAddSysFileDragContent(
	IDragDropController* controller)
{
	bool16 result = kFalse;

	do
	{
		// Stop if we can't determine the IDFile we are associated with.
		IDFile contentFile;
		PMString legend = kNullString, credit = kNullString;
		if (this->getContentFile(contentFile, legend, credit) == kFalse)
		{
			break;
		}
 
		// Point the controller at the handler.
		InterfacePtr<IDataExchangeHandler> dataExchangeHandler(controller->QueryHandler(XPageImageFlavor));
		if (dataExchangeHandler->IsEmpty() == kFalse)
		{
			dataExchangeHandler->Clear();
		}

		controller->SetSourceHandler(dataExchangeHandler);

		// Add the IDFile to be dragged.
		InterfacePtr<ISysFileListData> sysFileListData(dataExchangeHandler, IID_ISYSFILELISTDATA);
		sysFileListData->Append(contentFile);

		// Add the legend and credit of the photo
		InterfacePtr<IStringData> creditData(dataExchangeHandler, IID_ICREDITPHOTO);
		InterfacePtr<IStringData> legendData(dataExchangeHandler, IID_ILEGENDPHOTO);
		creditData->Set(credit);
		legendData->Set(legend);
			
		// Indicate the flavour in the drag object. 
		InterfacePtr<IPMDataObject> pmDataObject(controller->AddDragItem(1));
		
		PMFlavorFlags flavorFlags = kNormalFlavorFlag;
		pmDataObject->PromiseFlavor(XPageImageFlavor, flavorFlags);

		result = kTrue;

	} while(false);

	return result; 
}

// End, XPGUITECImageDragDropSource.cpp.
