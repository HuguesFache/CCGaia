
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IPMDataObject.h"
#include "IFlavorData.h"
#include "IPMStream.h"
#include "ISysFileListData.h"
#include "IDataExchangeHandler.h"
#include "IDocument.h"
#include "IControlView.h"
#include "ILayoutUIUtils.h"
#include "IPMStream.h"
#include "IPasteboardUtils.h"
#include "ILayoutControlData.h"
#include "ISpread.h"
#include "IXPageUtils.h"
#include "IHierarchy.h"

// General includes:
#include "CDragDropTargetFlavorHelper.h"
#include "CAlert.h"
#include "PMFlavorTypes.h"
#include "DataObjectIterator.h"
#include "PMString.h"
#include "FileUtils.h"
#include "ErrorUtils.h"
#include "StreamUtil.h"
#include "TransformUtils.h"
#include "StringUtils.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"

/** XPGUIFormeDropTarget
	Provides the drop behaviour. Our drag and drop target accepts inds files from XPage Formes palette
*/
class XPGUIFormeDropTarget : public CDragDropTargetFlavorHelper
{
	public:

		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		XPGUIFormeDropTarget(IPMUnknown* boss);

		/**
			this method defines the target response to the drag. It is called when the mouse enters the panel. We
			inidcate we can accept drags with a XPageFormeFlavor flavor, that we do not want the default cursor

			@param dataIter IN iterator providing access to the data objects within the drag.
			@param fromSource IN the source of the drag.
			@param controller IN the drag drop controller mediating the drag.
			@return a target response (either won't accept or drop will copy).

			@see DragDrop::TargetResponse
		*/
		DragDrop::TargetResponse CouldAcceptTypes(const IDragDropTarget* target, DataObjectIterator* dataIter, const IDragDropSource* fromSource, const IDragDropController* controller) const;
		/**
			When the drop is performed, this method is called. We get the xml file and then do the import into the text frame
			@param controller IN the drag drop controller mediating the drag.
			@param type IN drag and drop command type
			@see DragDrop::eCommandType
		*/
		ErrorCode	ProcessDragDropCommand(IDragDropTarget* target , IDragDropController* controller, DragDrop::eCommandType action);
		
		/** Enable target helper to track drag (which means DoDragEnter will get called
		*/
		virtual bool16 HelperWillDoDragTracking() const { return kTrue; }
	
		/** Change cursor appearance when it's above target
		*/
		virtual void DoDragEnter(IDragDropTarget * target);

		DECLARE_HELPER_METHODS()
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUIFormeDropTarget, kXPGUIFormeDropTargetImpl)
DEFINE_HELPER_METHODS(XPGUIFormeDropTarget)

/* XPGUIFormeDropTarget Constructor
*/
XPGUIFormeDropTarget::XPGUIFormeDropTarget(IPMUnknown* boss)
: CDragDropTargetFlavorHelper(boss), HELPER_METHODS_INIT(boss)
{
}


DragDrop::TargetResponse XPGUIFormeDropTarget::CouldAcceptTypes(const IDragDropTarget* target, DataObjectIterator* dataIter, const IDragDropSource* fromSource, const IDragDropController* controller) const
{
	
	DataExchangeResponse response;
	response = dataIter->FlavorExistsWithPriorityInAllObjects(XPageFormeFlavor);
	if(response.CanDo()){	
 		return DragDrop::TargetResponse( response, DragDrop::kDropWillCopy,
 										 DragDrop::kUseDefaultTrackingFeedback,
 										 DragDrop::kTargetWillProvideCursorFeedback);
	}
	return DragDrop::kWontAcceptTargetResponse;
}

/*
	the drop has occured, we need to handle it.
*/
ErrorCode	
XPGUIFormeDropTarget::ProcessDragDropCommand(IDragDropTarget* target , IDragDropController* controller, DragDrop::eCommandType action){
	
	bool16 returnCode = kFailure;	
	do{
		if (action != DragDrop::kDropCommand)  
			break;
		  
		// We should ensure the drag has been internalized, if we are coming from a 
		// custom source, the handler may not be initialised
		if (controller->InternalizeDrag(XPageFormeFlavor, XPageFormeFlavor) != kSuccess)	
			break;

		InterfacePtr<IDataExchangeHandler> handler (controller->QuerySourceHandler());			
		
		InterfacePtr<ISysFileListData> fileData (handler, IID_ISYSFILELISTDATA);
		IDFile formeFileToImport = fileData->GetSysFileItem(0);	
		PMString temp = FileUtils::SysFileToPMString(formeFileToImport);
		bool16 renameForme = kTrue;
		if (temp.IndexOfString("Assemblages") > 0) {
			renameForme = kFalse;
		}
		IDFile matchingFile = fileData->GetSysFileItem(1);	
		
		// Get mouse location
		InterfacePtr<IControlView> layoutView (target, UseDefaultIID());
		GSysPoint where = controller->GetDragMouseLocation();
		PMPoint currentPoint = Utils<ILayoutUIUtils>()->GlobalToPasteboard(layoutView, where);
	
		// Get target spread
		InterfacePtr<ISpread> targetSpread (Utils<IPasteboardUtils>()->QueryNearestSpread(layoutView, currentPoint));							
		
		// Import snippet
		InterfacePtr<ILayoutControlData> layoutCtrlData (layoutView, UseDefaultIID());
		IDocument * doc = layoutCtrlData->GetDocument();
		PMString error = kNullString;
		if(Utils<IXPageUtils>()->ImportForme(::GetUIDRef(doc), formeFileToImport, currentPoint, ::GetUIDRef(targetSpread), matchingFile, error, nil, kTrue, renameForme) != kSuccess)
		{
			CAlert::InformationAlert(error);
			break;
		}
	
		returnCode = kSuccess;
		
	}while(false);

	return returnCode;
}

void XPGUIFormeDropTarget::DoDragEnter(IDragDropTarget * target)
{
	
	InterfacePtr<IDragDropController> controller (GetExecutionContextSession(), UseDefaultIID());
	controller->SetTrackingCursorFeedback(CursorSpec(kCrsrItemCreation));
}
		
// End, XPGUIFormeDropTarget.cpp.






