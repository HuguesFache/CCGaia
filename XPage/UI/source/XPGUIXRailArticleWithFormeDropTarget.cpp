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
#include "IStringData.h"
#include "IXPageMgrAccessor.h"
#include "IMultiColumnTextFrame.h"
#include "ISelectionManager.h"
#include "ISelectionUtils.h"
#include "ITextModel.h"
#include "ITextSelectionSuite.h"
#include "IPlacePIData.h"
#include "IFormeData.h"
#include "IXMLUtils.h"
#include "IWebServices.h"
#include "ILinkFacade.h"
#include "IAssignSetPropsCmdData.h"

// General includes:
#include "CDragDropTargetFlavorHelper.h"
#include "CAlert.h"
#include "PMFlavorTypes.h"
#include "DataObjectIterator.h"
#include "UIDList.h"
#include "PMString.h"
#include "IHierarchy.h"
#include "FileUtils.h"
#include "ErrorUtils.h"
#include "StreamUtil.h"
#include "TransformUtils.h"
#include "StringUtils.h"
#include "OpenPlaceID.h"
#include "IFrameType.h"
#include "IGeometry.h"
#include "Utils.h"
#include "ILayoutUtils.h"
#include "IUnitOfMeasure.h"
#include "IDropDownListController.h"
#include "IWidgetUtils.h"
#include "IStringListData.h"
#include "ITreeViewController.h"
#include "ILinkManager.h"
#include "ILinkUtils.h"
#include "IAssignment.h"
#include "ISnippetExport.h"
#include "Utils.h"
#include "IInCopyWorkFlowUtils.h"
#include "IUIColorUtils.h"
#include "IAssignmentUtils.h"
#include "IAssignmentMgr.h"
#include "InCopyImportID.h"
#include "ISubject.h"
#include "IXPageUtils.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "XPageUIUtils.h"
#include "XMLDefs.h"

/** XPGUIXRailArticleWithFormeDropTarget.
	Provides the drop behaviour. Our drag and drop target accepts inds files from XPage Formes palette
*/
class XPGUIXRailArticleWithFormeDropTarget : public CDragDropTargetFlavorHelper
{
	public:

		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		XPGUIXRailArticleWithFormeDropTarget(IPMUnknown* boss);

		/**
			this method defines the target response to the drag. It is called when the mouse enters the panel. We
			inidcate we can accept drags with a XPageXMLWithFormeFlavor flavor, that we do not want the default cursor

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

	private :
		
		virtual ErrorCode DoStoryLink(const UIDList& formeItemsToLink, const PMString& idArt, const PMString& rub, const PMString& ssRub, const PMString& articleSubject);

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUIXRailArticleWithFormeDropTarget, kXPGUIXRailArticleWithFormeDropTargetImpl)
DEFINE_HELPER_METHODS(XPGUIXRailArticleWithFormeDropTarget)

/* XPGUIXRailArticleWithFormeDropTarget Constructor
*/
XPGUIXRailArticleWithFormeDropTarget:: XPGUIXRailArticleWithFormeDropTarget(IPMUnknown* boss)
									 : CDragDropTargetFlavorHelper(boss), 
									   HELPER_METHODS_INIT(boss)
{
}


DragDrop::TargetResponse XPGUIXRailArticleWithFormeDropTarget::CouldAcceptTypes(const IDragDropTarget* target, DataObjectIterator* dataIter, const IDragDropSource* fromSource, const IDragDropController* controller) const
{
	DataExchangeResponse response;
	response = dataIter->FlavorExistsWithPriorityInAllObjects(XPageXMLWithFormeFlavor);
	if(response.CanDo()) 	 
 		return DragDrop::TargetResponse(response, DragDrop::kDropWillCopy, DragDrop::kUseDefaultTrackingFeedback, DragDrop::kTargetWillProvideCursorFeedback);
 
	return DragDrop::kWontAcceptTargetResponse;
}

/*
	the drop has occured, we need to handle it.
*/
ErrorCode	
XPGUIXRailArticleWithFormeDropTarget::ProcessDragDropCommand(IDragDropTarget* target , IDragDropController* controller, DragDrop::eCommandType action)
{
	bool16 returnCode = kFailure;	
	do{
		if (action != DragDrop::kDropCommand)
			break;
	
		// we should ensure the drag has been internalized, if we are coming from a 
		// custom source, the handler may not be initialised
		if (controller->InternalizeDrag(XPageXMLWithFormeFlavor, XPageXMLWithFormeFlavor) != kSuccess)	
			break;

		InterfacePtr<IDataExchangeHandler> handler (controller->QuerySourceHandler());			

		InterfacePtr<ISysFileListData> fileData (handler, IID_ISYSFILELISTDATA);		
		IDFile xmlFileToImport = fileData->GetSysFileItem(0);
		IDFile formeFileToImport = fileData->GetSysFileItem(1);
		IDFile matchingFile = fileData->GetSysFileItem(2);		

		InterfacePtr<IStringListData> textData (handler, IID_ISTRINGLISTDATA);		
		PMString artId = textData->GetStringList()[0];
		PMString artType = textData->GetStringList()[1];
		PMString artSnippetFile = textData->GetStringList()[2];
		PMString artSubject = textData->GetStringList()[3];
		PMString artIdStatus = textData->GetStringList()[4];
		PMString artLibelleStatus = textData->GetStringList()[5];
		PMString artCouleurStatus = textData->GetStringList()[6];
		PMString artRubrique = textData->GetStringList()[7];
		PMString artSsRubrique = textData->GetStringList()[8];

		// Get mouse location
		InterfacePtr<IControlView> layoutView (target, UseDefaultIID());
		GSysPoint where = controller->GetDragMouseLocation();
		PMPoint currentPoint = Utils<ILayoutUIUtils>()->GlobalToPasteboard(layoutView, where);
		
		// Get target spread
		InterfacePtr<ISpread> targetSpread (Utils<IPasteboardUtils>()->QueryNearestSpread(layoutView, currentPoint));			

		// Get Current Document and DataBase
		InterfacePtr<ILayoutControlData> layoutCtrlData (layoutView, UseDefaultIID());
		IDocument * doc = layoutCtrlData->GetDocument();
		if(doc == nil)	break;

		IDataBase * db = ::GetDataBase(doc);
		if(db == nil)	break;	

		// Get Story Text type : MarbreGeneral, MarbreParution
		PMString::ConversionError convError;
		int8 kArtType = artType.GetAsNumber(&convError);
		if (convError != PMString::kNoError)
			break;

		
		if(kArtType == kMEPJavaStory){			
			// Article Java, show Dialog to link story 
			XPageUIUtils::DisplayLinkArticleDialog(artId, artSnippetFile, artSubject, matchingFile, currentPoint, artIdStatus, 
												   artLibelleStatus, artCouleurStatus, ::GetUIDRef(targetSpread), 
												   kArtType, FileUtils::SysFileToPMString(xmlFileToImport), artRubrique, artSsRubrique);		
		}			

		// Send notification so that texte panel is updated
		InterfacePtr<ISubject> sessionSubject(GetExecutionContextSession(), UseDefaultIID());
		sessionSubject->Change(kXPGRefreshMsg, IID_IREFRESHPROTOCOL);					
	
		returnCode = kSuccess;

	}while(false);

	return returnCode;
}

void XPGUIXRailArticleWithFormeDropTarget::DoDragEnter(IDragDropTarget * target){
	InterfacePtr<IDragDropController> controller (GetExecutionContextSession(), UseDefaultIID());
	controller->SetTrackingCursorFeedback(CursorSpec(kCrsrItemCreation));
}
		
ErrorCode XPGUIXRailArticleWithFormeDropTarget::DoStoryLink(const UIDList& formeItemsToLink, const PMString& idArt, 
															const PMString& rub, const PMString& ssRub, 
															const PMString& articleSubject)
{
	ErrorCode status = kFailure;
	status = kSuccess;
	return status;
}
// End, XPGUIXRailArticleWithFormeDropTarget.cpp.
