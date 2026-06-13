#include "VCPluginHeaders.h"

// Interfaces includes
#include "IIDXMLElement.h"
#include "IImportXMLData.h"
#include "IPMUnknownData.h"
#include "IPostImportResponderData.h"
#include "IXMLImporter.h"
#include "IXMLImportOptionsPool.h"
#include "IXMLPostImportIteration.h"
#include "IXMLReferenceData.h"
#include "IXMLUtils.h"
#include "IToolBoxUtils.h"
#include "ITool.h"
#include "TextEditorID.h"
#include "IPMUnknown.h"
#include "IK2ServiceRegistry.h"
#include "ISAXServices.h"
#include "IK2ServiceProvider.h"
#include "ISAXContentHandler.h"
#include "ISAXEntityResolver.h"
#include "ISAXParserOptions.h"
#include "IUIDData.h"
#include "StreamUtil.h"
#include "FileUtils.h"
#include "StringUtils.h"
#include "IUIDListData.h"
#include "IGeometry.h"
#include "CAlert.h"
#include "TransformUtils.h"
#include "IHierarchy.h"
#include "IMultiColumnTextFrame.h"
#include "ITextFrameColumn.h"
#include "IFrameType.h"
#include "ITextModelCmds.h"
#include "IXPGTagToStyleMap.h"
#include "IXPageUtils.h"

// Project includes
#include "XPGID.h"
#include "XMLDefs.h"

/** Implementation of a post-import iterator (IXMLPostImportIteration)
*/
class XPGXMLPostImportIterationBase : public CPMUnknown<IXMLPostImportIteration> {

	public:

		/**	Constructor
		 *	@param boss IN refers to boss object on which this implementation is aggregated
		 */
		XPGXMLPostImportIterationBase(IPMUnknown * boss);

		/** 
		 *	@desc Destructor		
		 */
		~XPGXMLPostImportIterationBase();
		
		/**
		 * This method is called by XMLPostImportIterationResponder during the setup period.  
		 */
		virtual bool16 ShouldIterate (IPostImportResponderData *responderData);
		
		 
		/**		
		 * This method is called by XMLPostImportIterationResponder before the depth-first iteration.
		 */   
        virtual ErrorCode BeforeIteration (IPostImportResponderData *responderData);
				  
		/**	
		  * This method is called by XMLPostImportIterationResponder during the depth-first iteration.  
		  */ 
		virtual ErrorCode HandleElement (IIDXMLElement *element, IIDXMLDOMNode *domNode); 
				
		/**
		  * This method is called by XMLPostImportIterationResponder after the depth-first iteration.  
		  */  
		virtual ErrorCode AfterIteration();

	protected :

		UIDRef GetWiderItem(const UIDList& items);
		UIDList GetCartonItemsFromTarget(const UIDRef& itemRef, IDataBase* db);
		K2Vector<IDValuePair> LoadTagWithContentText(const IDFile& descFile);
		K2Vector<PMString> LoadTagsFromCarton(const UIDList& items);

		static const PMString articleElement;	
		UIDRef storyRef;
		XMLReference articleXMLRef;
		PMString descFilePath, articleId; 
		UIDList formeItems;
};

CREATE_PMINTERFACE(XPGXMLPostImportIterationBase, kXPGXMLPostImportIterationBaseImpl)

const PMString XPGXMLPostImportIterationBase::articleElement = txtTag;

/**
 */
XPGXMLPostImportIterationBase::XPGXMLPostImportIterationBase(IPMUnknown * boss):CPMUnknown<IXMLPostImportIteration>(boss), 
																				articleXMLRef(kInvalidXMLReference),
																				storyRef(UIDRef::gNull), 
																				descFilePath(kNullString), 
																				articleId(kNullString){}
/**
 *
 */
XPGXMLPostImportIterationBase::~XPGXMLPostImportIterationBase(){}

/**
 */
bool16 XPGXMLPostImportIterationBase::ShouldIterate(IPostImportResponderData* responderData){	

	IPMUnknown* xmlImporter = responderData->GetImporter();
		
	// Refers to kXMLImporterBoss		
	InterfacePtr<IPMUnknownData> importXMLData(xmlImporter, UseDefaultIID());
	InterfacePtr<IImportXMLData> importData(importXMLData->GetPMUnknown(), UseDefaultIID());
	InterfacePtr<IXMLImportOptionsPool> importDataOptions (importXMLData->GetPMUnknown(), UseDefaultIID());

	int32 index = 0, count = 0;
	importDataOptions->GetValue(WideString(kXPGXMLIndexTargetKey), index); // store related description file in options pool (for status updating after import)
	importDataOptions->GetValue(WideString(kXPGXMLCountTargetKey), count); // store article'id

	if(index == count - 1){

		int32 val = 0;
		importDataOptions->GetValue(WideString(kXPGXMLImportTargetKey), val);	
		UIDRef lastStoryRef = UIDRef(importData->GetDataBase(), UID((uint32)val));
		if(lastStoryRef.GetUID() == kInvalidUID) {// It's not an xml importation done using XPage, so don't register
			return kFalse;
		}
		Utils<IXMLUtils>()->GetActualContent(lastStoryRef); // Must be a story UIDRef since we are only dealing with text frames
		
		// Get all items from last story
		formeItems = this->GetCartonItemsFromTarget(lastStoryRef, importData->GetDataBase());	
		if(formeItems.size() == 0){
			return kFalse;
		}
		
		// Get wider item in forme
		storyRef = this->GetWiderItem(formeItems);		
		if(storyRef.GetUID() == kInvalidUID){ // It's not an xml importation done using XPage
			return kFalse;	
		}

	}else
		return kFalse;	
	
	return kTrue; 
}

/*
*/
ErrorCode XPGXMLPostImportIterationBase::BeforeIteration(IPostImportResponderData* responderData){

	ErrorCode err(kSuccess);

	do {
		IPMUnknown* xmlImporter = responderData->GetImporter();
		
		// Refers to kXMLImporterBoss		
		InterfacePtr<IPMUnknownData> importXMLData(xmlImporter, UseDefaultIID());
		InterfacePtr<IXMLImportOptionsPool> importDataOptions (importXMLData->GetPMUnknown(), UseDefaultIID());

		// Get back article's description file and articleId
		WideString valW;
		importDataOptions->GetValue(WideString(kXPGXMLDescFileKey), valW);
		descFilePath = PMString(valW);
		
		valW.clear();
		importDataOptions->GetValue(WideString(kXPGXMLArticleIdKey), valW);
		articleId = PMString(valW);

		// Added version 1.5.9 : a bug that occurs whenever the pen tool (or maybe some others) is selected after an import,
		// on next click, InDesign crashes becauses active selection is a text selection while current tool is not the text tool.
		// So to fix, we just set the active tool to the text tool if not set yet
#if !XPAGESERVER
		InterfacePtr<ITool> activeTool(Utils<IToolBoxUtils>()->QueryActiveTool());
		if (!activeTool->IsTextTool()) {
			InterfacePtr<ITool> textTool (Utils<IToolBoxUtils>()->QueryTool(kIBeamToolBoss));
			Utils<IToolBoxUtils>()->SetActiveTool(textTool);
		}				
#endif
	} while(kFalse);
	return err;
}

/*
*/

ErrorCode XPGXMLPostImportIterationBase::HandleElement(IIDXMLElement* element, IIDXMLDOMNode *domNode){
	ErrorCode err = kSuccess;
	if(element->GetTagString().compare(WideString(articleElement)) == 0)
		articleXMLRef = element->GetXMLReference();
	return err;
}

ErrorCode XPGXMLPostImportIterationBase::AfterIteration(){

	ErrorCode err = kFailure;
	do{
		// Update description file
		if(descFilePath == kNullString) {
			break;	
		}

		// Get full text with tags
		IDFile descFile = FileUtils::PMStringToSysFile(descFilePath);	
		K2Vector<IDValuePair> firstList = this->LoadTagWithContentText(descFile);
		if(firstList.size() == 0){
			break;
		}

		// Get all tags from carton
		K2Vector<PMString> cartonTags = this->LoadTagsFromCarton(formeItems);
		if(cartonTags.size() == 0){
			break;
		}

		IDataBase * db = formeItems.GetDataBase();
		if(!db){
			break;
		}

		InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
	 
		// Get paragraph style to apply			
		for(int32 i = 0 ; i < firstList.size(); i++){

			PMString tag = firstList[i].Value();
			if(::K2find(cartonTags, tag) == cartonTags.end()){
				// Insert text
				InterfacePtr<IFrameType> frameType (storyRef, UseDefaultIID());
				if(!frameType)
					continue;

				if(frameType->IsTextFrame()){

					InterfacePtr<IHierarchy> itemHier (frameType, UseDefaultIID());
					if(!itemHier)
						continue;

					InterfacePtr<IMultiColumnTextFrame> isTextFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
					if(!isTextFrame)
						continue;

					InterfacePtr<ITextModel> txtModel (isTextFrame->QueryTextModel());
					if(txtModel){
						boost::shared_ptr<WideString> data (new WideString());
						data->Append(kTextChar_CR);
						data->Append(kTextChar_CR);

						// Unused style name
						PMString title(kXPGUnTaggedTextKey, PMString::kTranslateDuringCall);					
						StringUtils::ReplaceStringParameters(&title, xpgPrefs->GetStyleName(tag));
						title.SetTranslatable(kFalse);

						// Insert title
						data->Append(WideString(title));	

						// Insert content text
						data->Append(WideString(firstList[i].Key()));						

						// Process Command
						InterfacePtr<ITextModelCmds> txtModelCmds (txtModel, UseDefaultIID());
						InterfacePtr<ICommand> insertCmd(txtModelCmds->InsertCmd(txtModel->TotalLength()-1, data));
						CmdUtils::ProcessCommand(insertCmd);
					}
				}			
			}			 	
		}

		err = kSuccess;

	}while(kFalse);
	return err;
}

K2Vector<PMString> XPGXMLPostImportIterationBase::LoadTagsFromCarton(const UIDList& items){

	K2Vector<PMString> tags;
	do{
		IDataBase * db = items.GetDataBase();
		if(!db){
			break;
		}

		for(int32 i = 0; i < items.size(); i++){

			UIDRef curstoryRef = items.GetRef(i);

			InterfacePtr<IFrameType> frameType (curstoryRef, UseDefaultIID());
			if(!frameType)
				continue;

			if(frameType->IsTextFrame()){

				InterfacePtr<IHierarchy> itemHier (frameType, UseDefaultIID());
				if(!itemHier)
					continue;

				InterfacePtr<IMultiColumnTextFrame> isTextFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
				if(!isTextFrame)
					continue;

				InterfacePtr<ITextModel> txtModel (isTextFrame->QueryTextModel());
				if(!txtModel)
					continue;

				InterfacePtr<IXPGTagToStyleMap> tag2styleMap(txtModel, UseDefaultIID());
				if(!tag2styleMap)
					continue;

				int32 mapCount = tag2styleMap->GetMappingCount();
				for(int32 j = 0; j < mapCount; j++){
					PMString tagName = tag2styleMap->GetTagAt(j);
					tags.push_back(tagName);				
				}					
			}
		}
	}while(kFalse);

	return tags;
}

K2Vector<IDValuePair> XPGXMLPostImportIterationBase::LoadTagWithContentText(const IDFile& descFile){

	K2Vector<IDValuePair> firstList;

	do{
		// Parse description file to insert unused text
		InterfacePtr<IK2ServiceRegistry> serviceRegistry(GetExecutionContextSession(), UseDefaultIID()); 		
		InterfacePtr<IK2ServiceProvider> xmlProvider(serviceRegistry->QueryServiceProviderByClassID(kXMLParserService, kXMLParserServiceBoss)); 		
		InterfacePtr<ISAXServices> saxServices(xmlProvider, UseDefaultIID()); 
		
		InterfacePtr<ISAXContentHandler> contentHandler(::CreateObject2<ISAXContentHandler>(kXPGCheckTextImportHandlerBoss, ISAXContentHandler::kDefaultIID)); 
		contentHandler->Register(saxServices);		
		
		InterfacePtr<ISAXEntityResolver> entityResolver(::CreateObject2<ISAXEntityResolver>(kXMLEmptyEntityResolverBoss, ISAXEntityResolver::kDefaultIID));
		InterfacePtr<ISAXParserOptions> parserOptions(saxServices, UseDefaultIID());
		
		// These commitments: follow 
		// the existing ImportXMLFileCmd
		parserOptions->SetNamespacesFeature(kFalse);
	    parserOptions->SetShowWarningAlert(kFalse);
		
		
		InterfacePtr<IPMStream> xmlFileStream(StreamUtil::CreateFileStreamRead(descFile));
		saxServices->ParseStream(xmlFileStream, contentHandler, entityResolver);		

		if(ErrorUtils::PMGetGlobalErrorCode() != kSuccess){
			// Reset error code and quit
		 	ErrorUtils::PMSetGlobalErrorCode(kSuccess);
		 	PMString error (kXPGErrParseMatchingFailed);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, descFilePath);  
			break;
		}

		// Add story to import
		InterfacePtr<IKeyValueData> keyValueData(contentHandler, IID_ITEXTEMATCHINGSTYLESLIST);		
		firstList = keyValueData->GetKeyValueList();

	}while(kFalse);

	return firstList;
}



UIDRef XPGXMLPostImportIterationBase::GetWiderItem(const UIDList& items){

	UIDRef widerItemRef = UIDRef::gNull;
	PMReal utmostValue = 0.0;

	for(int32 i =0; i < items.size(); i++){

		UIDRef curstoryRef = items.GetRef(i);
		InterfacePtr<IGeometry> itemGeo (curstoryRef, UseDefaultIID());
		if(itemGeo == nil)
			continue;
		 
	 	PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();
		PMPoint rightBottom = itemGeo->GetStrokeBoundingBox().RightBottom();
		
		::TransformInnerPointToPasteboard(itemGeo, &leftTop);
		::TransformInnerPointToPasteboard(itemGeo, &rightBottom);

		PMReal surface = (rightBottom.X() - leftTop.X()) * (rightBottom.Y() - leftTop.Y());		 
		if(utmostValue < surface){
			utmostValue = surface;
			widerItemRef = curstoryRef;
		}	 	 
	}
	return widerItemRef;
}

UIDList XPGXMLPostImportIterationBase::GetCartonItemsFromTarget(const UIDRef& itemRef, IDataBase* db){
	
	UIDList curformeItems(db);
	do{
		InterfacePtr<ITextModel> txtModel (itemRef, UseDefaultIID());
		if(txtModel == nil) {
			break;
		}

		InterfacePtr<IFrameList> frameList (txtModel->QueryFrameList());
		if(frameList == nil) {
			break;
		}

		InterfacePtr<ITextFrameColumn> txtFrame (frameList->QueryNthFrame(0));
		if(txtFrame == nil) {
			break;
		}

		InterfacePtr<IMultiColumnTextFrame >multiTxtFrame (txtFrame->QueryMultiColumnTextFrame());
		if(multiTxtFrame == nil) {
			break;
		}

		InterfacePtr<IHierarchy> frameHierarchy(Utils<ITextUtils>()->QuerySplineFromTextFrame(multiTxtFrame));
		if(frameHierarchy == nil) {
			break;
		}

		// Get all item of carton from target story.			
		if(Utils<IXPageUtils>()->GetCartonItemsFromTarget(::GetUIDRef(frameHierarchy), curformeItems) != kSuccess) {
			break;	
		}
	}while(kFalse);

	return curformeItems;
}
