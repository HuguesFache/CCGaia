#include "VCPlugInHeaders.h"

// Interface includes:
#include "IControlView.h"
#include "ICommand.h"
#include "ICoreFilename.h"
#include "IPanelControlData.h"
#include "IStringListControlData.h"
#include "IDropDownListController.h"
#include "IStringData.h"
#include "IArticleDataModel.h"
#include "IXPageMgrAccessor.h"
#include "IStringListData.h"
#include "IXPageSuite.h"
#include "ISelectionUtils.h"
#include "ISelectionManager.h"
#include "IPMStream.h"
#include "IFormeDataModel.h"
#include "IObserver.h"
#include "ISubject.h"
#include "IActiveContext.h"
#include "IDropDownListController.h"
#include "IUIDData.h"
#include "IUIDListData.h"
#include "IDocument.h"
#include "ILayoutSelectionSuite.h"
#include "IFormeData.h"
#include "IClassIDData.h"
#include "IGeometry.h"
#include "ITreeViewController.h"
#include "IWidgetUtils.h"
#include "XPGUIFormeNodeID.h"
#include "IPointerData.h"
#include "StreamUtil.h"
#include "IDOMElement.h"
#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "IDOMServices.h"
#include "IIDXMLDOMNList.h"
#include "IIDXMLDOMNode.h"
#include "IIDXMLDOMDocument.h"
#include "IScript.h"
#include "IScriptUtils.h"
#include "ISAXEntityResolver.h"
#include "ISAXParserOptions.h"
#include "StringUtils.h"
#include "ISAXServices.h"
#include "ITreeViewMgr.h"
#include "IMatchingStylesList.h"
#include "ITreeViewHierarchyAdapter.h"

// General includes:

#include "CDialogController.h"
#include "IUnitOfMeasure.h"
#include "GenericID.h"
#include "FileUtils.h"
#include "CmdUtils.h"
#include "K2Pair.h"
#include "CAlert.h"
#include "Utils.h"
#include "DBUtils.h"
#include "TransformUtils.h"
#include "ITextModel.h"
#include "IAssignmentUtils.h"
#include "IAttributeStrand.h"
#include "IKeyValueData.h"
#include "ISAXContentHandler.h"
#include "IXPageUtils.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"

// Project includes:
#include "XPGID.h"
#include "XPGUIID.h"
#include "XPGUIStyleMatchingNodeID.h"

/** 
	Preferences dialog controller
*/
class XPGUIUpdateFormeDialogController : public CDialogController 
{
	public:
	
		XPGUIUpdateFormeDialogController(IPMUnknown* boss) : CDialogController(boss),
			                                                 formeName(kNullString), 
			                                                 classeurName(kNullString) {}

		virtual void InitializeDialogFields( IActiveContext* dlgContext);
		virtual WidgetID ValidateDialogFields( IActiveContext* myContext);
		virtual void ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId);

	private :

		K2Vector<IDValuePair> ParseFileMatching(const IDFile& matchingFile);
		PMString formeName, classeurName;
};



CREATE_PMINTERFACE(XPGUIUpdateFormeDialogController, kXPGUIUpdateFormeDialogControllerImpl)


/* ApplyFields
*/
void XPGUIUpdateFormeDialogController::InitializeDialogFields( IActiveContext* dlgContext){

	InterfacePtr<IPanelControlData> panelCtrlData (this, UseDefaultIID());

	InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
	do{
		
		InterfacePtr<IPointerData> formeDataNode (this, IID_INODEDATA);		
		XPGUIFormeNodeID * formeNodeID = (XPGUIFormeNodeID *) formeDataNode->Get();
		
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());	
		InterfacePtr<IFormeDataModel> model (pageMrgAccessor->QueryFormeDataModel());		
		XPGFormeDataNode * formeNode = model->GetNode(formeNodeID->GetID());	
		
		InterfacePtr<IStringData> formeData (this, IID_IFORMENAMEDATA);
		formeName = formeData->GetString();
		formeName.SetTranslatable(kFalse);
		this->SetTextControlData(kXPGUIFormeNameWidgetID, formeName);

		InterfacePtr<IStringData> classeurData (this, IID_ICLASSEURNAMEDATA);
		classeurName = classeurData->GetString();

		// Matching File
		IDFile matchingFile; 
		FileUtils::GetParentDirectory(formeNode->GetFormeData()->formeFile, matchingFile);
		FileUtils::GetParentDirectory(matchingFile, matchingFile);
		FileUtils::AppendPath(&matchingFile, "Typographie");
		FileUtils::AppendPath(&matchingFile, formeName+".xml" );
		
		// Vector (key-value) to return
		K2Vector<IDValuePair> matchingStyles;

		// Parse XML and Get list of styles and tags
		if(FileUtils::DoesFileExist(matchingFile) )
			matchingStyles = ParseFileMatching(matchingFile);		
	
		InterfacePtr<ITreeViewMgr> treeViewMgr(panelCtrlData->FindWidget(kXPGUIUpdateStyleMatchingViewWidgetID), UseDefaultIID());
		InterfacePtr<IKeyValueData> matchingStylesList (treeViewMgr, IID_IFORMEMATCHINGSTYLESLIST);   
		matchingStylesList->SetKeyValueList(matchingStyles);		
		
		treeViewMgr->ClearTree();
		treeViewMgr->ChangeRoot(kTrue);   

	} while(kFalse);
}

/* ValidateFields
*/
WidgetID XPGUIUpdateFormeDialogController::ValidateDialogFields( IActiveContext* myContext)
{
	WidgetID result = kNoInvalidWidgets;

	if(GetTextControlData(kXPGUIFormeNameWidgetID) == kNullString)
		return kXPGUIFormeNameWidgetID;
	return result;
}

/* ApplyFields
*/
void XPGUIUpdateFormeDialogController::ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId) 
{		
	InterfacePtr<IPanelControlData> panelCtrlData (this, UseDefaultIID());	

	do{
		InterfacePtr<IPointerData> formeDataNode (this, IID_INODEDATA);		
		XPGUIFormeNodeID * formeNodeID = (XPGUIFormeNodeID *) formeDataNode->Get();
		
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());	
		InterfacePtr<IFormeDataModel> model (pageMrgAccessor->QueryFormeDataModel());		
		XPGFormeDataNode * formeNode = model->GetNode(formeNodeID->GetID());		
		const XPGFormeDataNode * formeNodeParent = model->GetParent(formeNodeID->GetID());		
		
		//Update Matching File XML
		InterfacePtr<ITreeViewMgr> treeViewMgr(panelCtrlData->FindWidget(kXPGUIUpdateStyleMatchingViewWidgetID), UseDefaultIID());
		if(treeViewMgr == nil)
			break;
		
		InterfacePtr<ITreeViewHierarchyAdapter> treeViewHierAdapter (treeViewMgr, UseDefaultIID());
		if(treeViewHierAdapter == nil)
			break;

		NodeID styleMatchingRootNode = treeViewHierAdapter->GetRootNode();
		int32 treeViewHierAdapterCount = treeViewHierAdapter->GetNumChildren(styleMatchingRootNode);
		
		K2Vector<IDValuePair> stylesTags;
		PMString tagName = kNullString, styleName = kNullString;

		for(int32 i = 0; i < treeViewHierAdapterCount; ++i) {

			tagName.clear(); 
			styleName.clear();

			/* BUG FIX : don't use TreeNodePtr this way, it causes leaks :

			   TreeNodePtr<XPGUIStyleMatchingNodeID> nodePtr (treeViewHierAdapter->GetNthChild(styleMatchingRootNode, i));
			*/
			NodeID childNode = treeViewHierAdapter->GetNthChild(styleMatchingRootNode, i);			 
			TreeNodePtr<XPGUIStyleMatchingNodeID> nodePtr (childNode);
			styleName = nodePtr->GetID();
			
			InterfacePtr<IControlView> nodeView (treeViewMgr->QueryWidgetFromNode(childNode));
			if(nodeView == nil)
				continue;
			
			InterfacePtr<IDropDownListController> tagListWidgetController ((IDropDownListController*)Utils<IWidgetUtils>()->QueryRelatedWidget(nodeView, kXPGUIUpdateStyleTagListWidgetID, IID_IDROPDOWNLISTCONTROLLER ));
			if(tagListWidgetController == nil)
				continue;

			int32 selectedIndex = tagListWidgetController->GetSelected();			
			if(selectedIndex != IDropDownListController::kNoSelection){
				InterfacePtr<IStringListControlData> tagListWidgetData (tagListWidgetController, UseDefaultIID());
				tagName = tagListWidgetData->GetString(selectedIndex);
			}
			if(tagName != kNullString && styleName != kNullString ){
				 stylesTags.push_back(KeyValuePair<PMString, PMString>(tagName, styleName)); 				
			}		
		}		
	} while(kFalse);
}

K2Vector<IDValuePair> XPGUIUpdateFormeDialogController::ParseFileMatching(const IDFile& matchingFile){

	ErrorCode status = kFailure;
	PMString error = kNullString;
	
	// Parse matching file to get mapping styles
	InterfacePtr<IK2ServiceRegistry> serviceRegistry(GetExecutionContextSession(), UseDefaultIID()); 
	InterfacePtr<IK2ServiceProvider> xmlProvider(serviceRegistry->QueryServiceProviderByClassID(kXMLParserService, kXMLParserServiceBoss)); 

	InterfacePtr<ISAXServices> saxServices(xmlProvider, UseDefaultIID()); 
	InterfacePtr<ISAXContentHandler> contentHandler(::CreateObject2<ISAXContentHandler>(kXPGXMLMatchingFileHandler2Boss, ISAXContentHandler::kDefaultIID)); 

	contentHandler->Register(saxServices);
	InterfacePtr<ISAXEntityResolver> entityResolver(::CreateObject2<ISAXEntityResolver>(kXMLEmptyEntityResolverBoss, ISAXEntityResolver::kDefaultIID));
	InterfacePtr<ISAXParserOptions> parserOptions(saxServices, UseDefaultIID());
	
	// These commitments: follow 
	// the existing ImportXMLFileCmd
	
	parserOptions->SetNamespacesFeature(kFalse);
	parserOptions->SetShowWarningAlert(kFalse);
	
	InterfacePtr<IPMStream> xmlFileStream(StreamUtil::CreateFileStreamRead(matchingFile));
	saxServices->ParseStream(xmlFileStream, contentHandler, entityResolver);	
	xmlFileStream->Close();
	xmlFileStream.reset(nil);	

	InterfacePtr<IKeyValueData> formeMatchingStyleData (contentHandler, IID_IFORMEMATCHINGSTYLESLIST);
	return formeMatchingStyleData->GetKeyValueList();
	
}		