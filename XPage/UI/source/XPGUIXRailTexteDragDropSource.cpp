
#include "VCPlugInHeaders.h"

// Interface includes:
#include "ITreeNodeIDData.h"
#include "IDragDropController.h"
#include "IDataExchangeHandler.h"
#include "IStringListControlData.h"
#include "ITriStateControlData.h"
#include "ISysFileListData.h"
#include "IStringListData.h"
#include "ILayoutUIUtils.h"
#include "IPMDataObject.h"
#include "IStringData.h"
#include "IControlView.h"
#include "IPageItemScrapData.h"
#include "IPageItemLayerData.h"
#include "IPointListData.h"
#include "IDropDownListController.h"
#include "IWidgetUtils.h"
#include "IXPageMgrAccessor.h"
#include "IXPGPreferences.h"

// General includes:
#include "CDragDropSource.h"
#include "FileUtils.h"
#include "DocumentID.h"
#include "CAlert.h"
#include "ErrorUtils.h"
#include "StringUtils.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "XPGUIArticleNodeID.h"
#include "XPGUIFormeNodeID.h"


/** Provides the node with drag source capability for the pub
	 associated with the widget.

	@ingroup paneltreeview
	
*/
class XPGUIXRailTexteDragDropSource : public CDragDropSource
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIXRailTexteDragDropSource(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~XPGUIXRailTexteDragDropSource();

	/** Returns kTrue if there is a front document.
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

	bool16 doAddArticleDragContent(IDragDropController* controller);

	bool16 GetArticleData(IDFile& articleFile, IDFile& formeFile, IDFile& matchingFile, PMString& articleId, 
						  IDFile& imageFile, PMString& legend, PMString& credit, int32& artType,
						  PMString& articleSnippetFile, PMString& articleSubject, PMString& idStatus, PMString& libelleStatus,
						  PMString& couleurStatus, PMString& rubrique, PMString& ssRubrique, PMString& artFolio) const;

};

CREATE_PMINTERFACE(XPGUIXRailTexteDragDropSource, kXPGUIXRailTexteDragDropSourceImpl)

/* Constructor
*/
XPGUIXRailTexteDragDropSource::XPGUIXRailTexteDragDropSource(IPMUnknown* boss):CDragDropSource(boss){}


/* Destructor
*/
XPGUIXRailTexteDragDropSource::~XPGUIXRailTexteDragDropSource(){}


/* WillDrag
*/
bool16 XPGUIXRailTexteDragDropSource::WillDrag(IEvent* event) const{
	IDocument* theFrontDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	if (theFrontDoc == nil)
		return kFalse; 
	return kTrue;
}


/* DoAddDragContent
*/
bool16 XPGUIXRailTexteDragDropSource::DoAddDragContent(IDragDropController* controller){
	return this->doAddArticleDragContent(controller);
}


/* doAddPubDragContent
*/
bool16 XPGUIXRailTexteDragDropSource::doAddArticleDragContent(IDragDropController* controller){
	
	bool16 result = kFalse;
	do{
		// Stop if we can't determine the IDFile we are associated with.
		IDFile articleFile, formeFile, matchingFile, imageFile;

		PMString articleId = kNullString, legend = kNullString, credit = kNullString, articleSnippetFile = kNullString, articleSubject = kNullString, 
				 articleIdStatus = kNullString, articleLibelleStatus = kNullString, articleCouleurStatus = kNullString, 
				 articleRubrique = kNullString, articleSsRubrique = kNullString, articleFolio = kNullString;
					
		int32 articleType = 0;	
		if (this->GetArticleData(articleFile, formeFile, matchingFile, articleId, imageFile, legend, credit, articleType, articleSnippetFile, articleSubject, 
								 articleIdStatus, articleLibelleStatus, articleCouleurStatus, articleRubrique, articleSsRubrique, articleFolio) == kFalse)
		{
			break;
		}
	
		PMFlavor flavor = kNoFlavor;

#if !INCOPY
		if(controller->IsUserUsingCopyGesture()) // If user is pressing alt/option key
			flavor = XPageTextOnlyFlavor;
		else
			flavor = XPageXMLWithFormeFlavor;
#else
		flavor = XPageTextOnlyFlavor;
#endif
		
		InterfacePtr<IDataExchangeHandler> dataExchangeHandler (controller->QueryHandler(flavor));

		controller->SetSourceHandler(dataExchangeHandler);
		dataExchangeHandler->Clear();		 
		
		// Add the IDFiles to be dragged.
		InterfacePtr<ISysFileListData> sysFileData(dataExchangeHandler, IID_ISYSFILELISTDATA);
		sysFileData->Append(articleFile);
		sysFileData->Append(formeFile);
		sysFileData->Append(matchingFile);
		
		PMString articleTypeString;
		articleTypeString.AppendNumber(articleType);

		K2Vector<PMString> articleData;
		articleData.push_back(articleId);
		articleData.push_back(articleTypeString);
		articleData.push_back(articleSnippetFile);
		articleData.push_back(articleSubject);
		articleData.push_back(articleIdStatus);
		articleData.push_back(articleLibelleStatus);
		articleData.push_back(articleCouleurStatus);
		articleData.push_back(articleRubrique);
		articleData.push_back(articleSsRubrique);	
		articleData.push_back(articleFolio);	

		// Pass the article's article Snippet File to the handler for use at a later time
		InterfacePtr<IStringListData> textData (dataExchangeHandler, IID_ISTRINGLISTDATA);
		textData->SetStringList(articleData);		
				
		// Indicate the flavour in the drag object. 
		InterfacePtr<IPMDataObject> pmDataObject(controller->AddDragItem(1));
		PMFlavorFlags flavorFlags = kNormalFlavorFlag;
		
		pmDataObject->PromiseFlavor(flavor, flavorFlags);

		result = kTrue;
	
	}while(kFalse);
	return result; 
}

/* getXmlFiles
*/
bool16 XPGUIXRailTexteDragDropSource::GetArticleData(IDFile& articleFile, IDFile& formeFile, IDFile& matchingFile, PMString& articleId, 
													 IDFile& imageFile, PMString& legend, PMString& credit, int32& artType, PMString& articleSnippetFile, 
													 PMString& articleSubject, PMString& idStatus, PMString& libelleStatus, PMString& couleurStatus, 
													 PMString& rubrique, PMString& ssRubrique, PMString& artFolio) const
{
	bool16 result = kFalse;
	
	do {
		InterfacePtr<ITreeNodeIDData> nodeData(this, UseDefaultIID());		
		TreeNodePtr<XPGUIArticleNodeID> nodeID(nodeData->Get());
		if(!nodeID)
			break;

#ifdef MACINTOSH	
		PMString contentPath = FileUtils::DecomposeUnicode(nodeID->GetArticleData()->artPath);
#else
		PMString contentPath = nodeID->GetArticleData()->artPath;
#endif	
        articleId = nodeID->GetID();
		artType = nodeID->GetArticleData()->artType;
		articleSubject = nodeID->GetArticleData()->artSubject;
		libelleStatus = nodeID->GetArticleData()->artLibStatus;
		couleurStatus = nodeID->GetArticleData()->artCouleurStatus;
		rubrique = nodeID->GetArticleData()->artRubrique;
		ssRubrique = nodeID->GetArticleData()->artSsRubrique;
		articleFile = FileUtils::PMStringToSysFile(contentPath);
		
		if(!FileUtils::DoesFileExist(articleFile)){
			ErrorUtils::PMSetGlobalErrorCode(kSuccess); // Reset global error code
            
			PMString error(kXPGErrImportXmlFailed);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, FileUtils::SysFileToPMString(articleFile));
			CAlert::InformationAlert(error);
            CAlert::InformationAlert(FileUtils::SysFileToPMString(articleFile));
            break;
		}	
		result = kTrue;
	} while(false);

	return result;
}

// End, XPGUIXRailTexteDragDropSource.cpp.
