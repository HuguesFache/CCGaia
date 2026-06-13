
#include "VCPlugInHeaders.h"

// Interface includes:
#include "ITreeNodeIDData.h"
#include "IDragDropController.h"
#include "IDataExchangeHandler.h"
#include "ISysFileListData.h"
#include "IPMDataObject.h"

// General includes:
#include "CDragDropSource.h"
#include "FileUtils.h"
#include "DocumentID.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIFormeNodeID.h"
#include "CAlert.h"

/** Provides the node with drag source capability for the IDFile
	(kPMSysFileFlavor) associated with the widget. This allows the
	IDFile selected in a tree view node to be dragged from the
	panel onto a drop target that accepts a IDFile. For example
	a layout view on a document.

	@ingroup paneltreeview
	
*/
class XPGUIFormeDragDropSource : public CDragDropSource
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIFormeDragDropSource(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XPGUIFormeDragDropSource();

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
		@param contentFile [OUT] specifies forme snippet file
		@param matchingFile [OUT] specifies forme matching file
		@return kTrue if a draggable IDFile can be identified, kFalse otherwise.
	 */
	bool16 getFormeData(IDFile& snippetFile, IDFile& matchingFile) const;


	/** Adds kPMSysFileFlavor data to the drag describing the IDFile associated
		with this widget.
		@param controller [IN] in charge of this widget's DND capabilities.
		@return kTrue if valid content has been added to the drag, kFalse otherwise.
	 */
	bool16 doAddSysFileDragContent(IDragDropController* controller);

	// Directory separator
	static const PMString dirSep;

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGUIFormeDragDropSource, kXPGUIFormeDragDropSourceImpl)

/* Constructor
*/


#ifdef WINDOWS
const PMString XPGUIFormeDragDropSource::dirSep = "\\";
#else
const PMString XPGUIFormeDragDropSource::dirSep = "/";
#endif

XPGUIFormeDragDropSource::XPGUIFormeDragDropSource(IPMUnknown* boss)
: CDragDropSource(boss)
{
}


/* Destructor
*/
XPGUIFormeDragDropSource::~XPGUIFormeDragDropSource()
{
}


/* WillDrag
*/
bool16 XPGUIFormeDragDropSource::WillDrag(IEvent* event) const
{
	
	IDFile snippetFile, matchingFile;
	return this->getFormeData(snippetFile, matchingFile);
}


/* DoAddDragContent
*/
bool16 XPGUIFormeDragDropSource::DoAddDragContent(
	IDragDropController* controller)
{
	
	return this->doAddSysFileDragContent(controller);
}


/* getFormeData
*/
bool16 XPGUIFormeDragDropSource::getFormeData(IDFile& snippetFile, IDFile& matchingFile) const
{
	
	bool16 result = kFalse;
	do {
		
		InterfacePtr<ITreeNodeIDData> nodeData(this, UseDefaultIID());
		TreeNodePtr<XPGUIFormeNodeID> nodeID(nodeData->Get());
		if(nodeID->GetType() == XPGFormeDataNode::kFormeNode){				
			
			IDFile SnipetParentFile,matchingParentFile,
				   SnipetFileTmp,matchingFileTmp;

			PMString SnipetPathTmp,matchingPathTmp;
			
			FileUtils::GetParentDirectory(nodeID->GetFormeData()->formeFile, SnipetParentFile); 
			FileUtils::GetParentDirectory(nodeID->GetFormeData()->matchingFile, matchingParentFile); 
			
			SnipetPathTmp  =FileUtils::SysFileToPMString(SnipetParentFile)+ dirSep+
						    nodeID->GetFormeData()->name+".inds";
		
			if(!FileUtils::DoesFileExist(FileUtils::PMStringToSysFile (SnipetPathTmp)))
				break;	

			snippetFile  = FileUtils::PMStringToSysFile (SnipetPathTmp);

			if(nodeID->GetFormeData()->matchingFile != IDFile())
			{
				FileUtils::GetParentDirectory(nodeID->GetFormeData()->matchingFile, matchingParentFile);

				matchingPathTmp=FileUtils::SysFileToPMString(matchingParentFile)+ dirSep+
				          	nodeID->GetFormeData()->name+".xml";
				
				if(!FileUtils::DoesFileExist(FileUtils::PMStringToSysFile (matchingPathTmp)))
					break;	
		
				matchingFile = FileUtils::PMStringToSysFile (matchingPathTmp);
			}
			
			result = kTrue;
		}
		
	} while(false);
	return result;
}

/* doAddSysFileDragContent
*/
bool16 XPGUIFormeDragDropSource::doAddSysFileDragContent(
	IDragDropController* controller)
{
	
	bool16 result = kFalse;
	do	{
		// Stop if we can't determine the IDFile we are associated with.
		IDFile snippetFile, matchingFile;
		if (this->getFormeData(snippetFile, matchingFile) == kFalse)
		{
			break;
		}
		// Point the controller at the handler.
		InterfacePtr<IDataExchangeHandler> dataExchangeHandler(controller->QueryHandler(XPageFormeFlavor));
		if (dataExchangeHandler->IsEmpty() == kFalse)
		{
			dataExchangeHandler->Clear();
		}
		controller->SetSourceHandler(dataExchangeHandler);

		// Add the IDFile to be dragged.
		InterfacePtr<ISysFileListData> sysFileListData(dataExchangeHandler, IID_ISYSFILELISTDATA);

		sysFileListData->Append(snippetFile);
		sysFileListData->Append(matchingFile);
	    // Indicate the flavour in the drag object. 
		InterfacePtr<IPMDataObject> pmDataObject(controller->AddDragItem(1));
		PMFlavorFlags flavorFlags = kNormalFlavorFlag;
		pmDataObject->PromiseFlavor(XPageFormeFlavor, flavorFlags);
		result = kTrue;

	} while(false);

	return result; 
}



// End, XPGUIFormeDragDropSource.cpp.
