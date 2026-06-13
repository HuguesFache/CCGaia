/*
//	File:	XPGUILinkArtDialogController.cpp
//
//  Author : Trias Developpement
*/
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IActiveContext.h"
#include "ITextControlData.h"
#include "IControlView.h"
#include "IPanelControlData.h"
#include "IStringListControlData.h"
#include "IStringListData.h"
#include "IStringData.h"
#include "IDropDownListController.h"
#include "ISelectionUtils.h"
#include "ISelectionManager.h"
#include "IXPageSuite.h"
#include "IDocument.h"
#include "IAssignmentMgr.h"
#include "IAssignedDocument.h"
#include "IAssignment.h"
#include "IAssignmentUtils.h"
#include "ILayoutUIUtils.h"
#include "IDocument.h"
#include "IUIColorUtils.h"
#include "ISnippetExport.h"
#include "IInCopyWorkFlowUtils.h"
#include "StreamUtil.h"
#include "InCopyImportID.h"
#include "IWebServices.h"
#include "ILayoutUtils.h"
#include "IHierarchy.h"
#include "IXPGPreferences.h"
#include "ITextModel.h"
#include "IDocumentCommands.h"
#include "IGeometry.h"
#include "IUIDData.h"
#include "IUIDListData.h"
#include "IUnitOfMeasure.h"
#include "IDocumentSignalData.h"
#include "TransformTypes.h"
#include "TransformUtils.h"
#include "IPageList.h"
#include "ISpreadLayer.h"
#include "ICommand.h"
#include "IDateTimeData.h"
#include "ISpread.h"
#include "ITextModel.h"
#include "ILayoutUtils.h"
#include "IPMPointData.h"
#include "IXMLUtils.h"
#include "IXPageUtils.h"
#include "ISysFileData.h"
#include "IFrameType.h"
#include "IFrameList.h"
#include "IPageItemTypeUtils.h"
#include "ITextFrameColumn.h"

// General includes:
#include "CDialogController.h"
#include "FileUtils.h"
#include "ErrorUtils.h"
#include "CAlert.h"
#include "StringUtils.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "XRLID.h"
#include "IXRailPageSlugData.h"
#include "IXPGPreferences.h"
#include "IDataBase.h"
#include "IIntData.h"
#include "IDTime.h"
#include "IAssignSetPropsCmdData.h"
#include "IInCopyDocUtils.h"
#include "XMLDefs.h"
#include "IInCopyWorkflow.h"
#include "ISpreadList.h"
#include "XPageUIUtils.h"
#include "IXPageMgrAccessor.h"
#include "IPlacedArticleData.h"
#include "IFormeData.h"

class XPGUILinkArtDialogController : public CDialogController{

	public:
	
        XPGUILinkArtDialogController(IPMUnknown* boss) : CDialogController(boss), idPage(-1){}
		
		virtual ~XPGUILinkArtDialogController() {}	 		
		virtual void InitializeDialogFields(IActiveContext* dlgContext);		
		virtual WidgetID ValidateDialogFields(IActiveContext* myContext);			
		virtual void ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);		

	private :

		int32  idPage;
		bool16 DoStoryLink(const bool16& changeForme, IDocument * doc, const PMString& idArt, 
							const PMString& rub, const PMString& ssRub, 
							const PMString& rubName, const PMString& ssRubName,
							const PMString& subject, const PMString& articleSnippet,
							const PMString& recipient, const PMString& idStatus, 
							const PMString& libelleStatus, const PMString& couleurStatus, 
							const int32 typeArt,
							const PMPoint& currentPoint, const UIDRef& targetSpread,
							const IDFile& matchingFile, const PMString& articleXMLFile);
		
		UIDRef GetTargetStoryFromSnipet(const UIDList& formeItems);
};

CREATE_PMINTERFACE(XPGUILinkArtDialogController, kXPGUILinkArtDialogControllerImpl)

void XPGUILinkArtDialogController::InitializeDialogFields(IActiveContext* dlgContext)
{
}

// --------------------------------------------------------------------------------------
// ValidateFields
// --------------------------------------------------------------------------------------
WidgetID XPGUILinkArtDialogController::ValidateDialogFields(IActiveContext* myContext)
{	
	WidgetID result = CDialogController::ValidateDialogFields(myContext);
	do
	{
			
	}while(kFalse);		
	return result;
}

void XPGUILinkArtDialogController::ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId)
{
	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());		
	do{
		IDocument * doc = myContext->GetContextDocument();
		if(! doc)
			break;

		IDataBase * db = ::GetDataBase(doc);
		if(! db)
			break;

		PMString error = kNullString, rubID = kNullString, ssrubID = kNullString, rubName = kNullString,
				 ssRubName = kNullString, recipientID = kNullString, articleId = kNullString, articleSnippetFile = kNullString, 
				 articleSubject = kNullString, articleXMLFile = kNullString ;

		InterfacePtr<IStringData> articleData (this, IID_IARTICLEIDDATA);
		articleId = articleData->GetString();
		
		InterfacePtr<IStringData> articleSnippetFileData (this, IID_IARTICLESNIPPETFILEDATA);
		articleSnippetFile = articleSnippetFileData->GetString();

		InterfacePtr<IStringData> articleXMLFileData (this, IID_IARTICLEXMLFILEDATA);
		articleXMLFile = articleXMLFileData->GetString();
		
		InterfacePtr<IStringData> articleSubjectData (this, IID_IARTICLETOPICDATA);
		articleSubject = articleSubjectData->GetString();

		InterfacePtr<IStringData> articlerubData (this, IID_IARTICLERUBDATA);
		rubID = articlerubData->GetString();

		InterfacePtr<IStringData> articlessrubData (this, IID_IARTICLESUBRUBDATA);
		ssrubID = articlessrubData->GetString();

		InterfacePtr<ISysFileData> matchingData (this, UseDefaultIID());
		IDFile matchingFile = matchingData->GetSysFile();

		InterfacePtr<IPMPointData>  pmPointData (this, UseDefaultIID());
		PMPoint currentPoint = pmPointData->GetPMPoint();

		InterfacePtr<IUIDData> uidData(this, UseDefaultIID());
		UIDRef targetSpread = uidData->GetRef();	

		// Update Story status 
		InterfacePtr<IStringData> articleIdStatusData (this, IID_IARTICLEIDSTATUS);
		PMString idStatus = articleIdStatusData->GetString();
	
		InterfacePtr<IStringData> articlelibelleStatusData (this, IID_IARTICLELIBELLESTATUS);
		PMString libelleStatus = articlelibelleStatusData->GetString();

		InterfacePtr<IStringData> articlecouleurStatusData (this, IID_IARTICLECOULEURSTATUS);
		PMString couleurStatus = articlecouleurStatusData->GetString();

		InterfacePtr<IIntData> articleTypeData (this, IID_IARTICLETYPEDATA);

		InterfacePtr<ITriStateControlData> changeFormeStateControlData (panelControlData->FindWidget(kXPGUIChangeFormeStatesWidgetID), UseDefaultIID());

		bool16 changeForme = kFalse;
        if( articleTypeData->Get() ==  kMEPJavaStory || changeFormeStateControlData->GetState() == ITriStateControlData::kSelected){
            IDFile formeFile;
			// Get selected forme in article panel
			InterfacePtr<IDropDownListController> classeurListController(panelControlData->FindWidget(kXPGUIClasseurListWidgetID), UseDefaultIID());
			InterfacePtr<IDropDownListController> cartonListController(panelControlData->FindWidget(kXPGUICartonListWidgetID), UseDefaultIID());
			
			int32 selectedClasseur = classeurListController->GetSelected();
			int32 selectedCarton = cartonListController->GetSelected();

			if(selectedClasseur == IDropDownListController::kNoSelection || selectedCarton == IDropDownListController::kNoSelection)
				break;

			InterfacePtr<IStringListControlData> classeurListData (classeurListController, UseDefaultIID());
			InterfacePtr<IStringListControlData> cartonListData (cartonListController, UseDefaultIID());

			PMString classeur = classeurListData->GetString(selectedClasseur);
			PMString carton = cartonListData->GetString(selectedCarton);
			PMString posX, posY;

			InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
			InterfacePtr<IFormeDataModel> formeModel (pageMrgAccessor->QueryFormeDataModel());
		
			if(!formeModel->GetForme(carton, classeur, formeFile, matchingFile, posX, posY))
				break;		
								
			if(!FileUtils::DoesFileExist(formeFile))
				break;	
			
			if(!FileUtils::DoesFileExist(matchingFile))
				break;

			articleSnippetFile = FileUtils::SysFileToPMString(formeFile);
			changeForme = kTrue;
		}  
		
		// Story Link		
		if(!this->DoStoryLink(changeForme, doc, articleId, rubID, ssrubID, rubName, ssRubName, articleSubject, articleSnippetFile, 
							  recipientID, idStatus, libelleStatus, couleurStatus, articleTypeData->Get(),
							  currentPoint, targetSpread, matchingFile, articleXMLFile))
		{
			//Impossible de lier cet article a la page
			CAlert::InformationAlert(kXPGUIStoryLinkErrorKey);
			break;	
		}	 
														    

	}													      
	while(kFalse);
}

bool16 XPGUILinkArtDialogController::DoStoryLink(const bool16& changeForme, IDocument * doc, const PMString& idArt, const PMString& rub, 
												 const PMString& ssRub, const PMString& rubName, const PMString& ssRubName,  
												 const PMString& articleSubject, const PMString& articleSnippet, const PMString& recipient,
												 const PMString& idStatus, const PMString& libelleStatus,
												 const PMString& couleurStatus, 
												 const int32 typeArt, const PMPoint& currentPoint, const UIDRef& targetSpread,
												 const IDFile& matchingFile, const PMString& articleXMLFile)
{
	bool16 result = kFalse;
	do{
		
		
		IDataBase * db = ::GetDataBase(doc);
	
		UIDList formeItemsToLink(db);
		PMString error = kNullString;

		if( targetSpread == UIDRef::gNull)
			break;
	
		// Import snippet 
		if(Utils<IXPageUtils>()->ImportForme(::GetUIDRef(doc), 
											FileUtils::PMStringToSysFile(articleSnippet), 
											currentPoint,
											targetSpread, 
											matchingFile, 
											error,
											&formeItemsToLink, kTrue, kTrue)!= kSuccess)
		{
			CAlert::InformationAlert(error);
			break;
		}		
		
		UIDList targetStories(db);
		K2Vector<UIDRef> targetPictures;
		for(int32 i = 0 ; i < formeItemsToLink.Length() ; ++i){
            // GD 27.04.2023 ++
            UIDRef parentUID = formeItemsToLink.GetRef(i);
            InterfacePtr<IFormeData> formeData(parentUID, UseDefaultIID());
            if (formeData != nil) {
                if (formeData->GetType() == IFormeData::kFixedContent)
                    continue;
            }
            // GD 27.04.2023 --
			InterfacePtr<IFrameType> frameType (db, formeItemsToLink[i], UseDefaultIID());
			if(frameType && frameType->IsTextFrame()){
				InterfacePtr<IHierarchy> itemHier (frameType, UseDefaultIID());
				InterfacePtr<IMultiColumnTextFrame> txtFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
				targetStories.Append(txtFrame->GetTextModelUID());
			}
			else if (frameType && frameType->IsGraphicFrame()) {
				targetPictures.push_back(::GetUIDRef(frameType));
			}
		}
	    
		// Import article
		error = kNullString;		
		IDFile xmlFileToImport = FileUtils::PMStringToSysFile(articleXMLFile);
		if(Utils<IXPageUtils>()->ImportArticle(targetStories, xmlFileToImport, matchingFile, error, xmlFileToImport, idArt) != kSuccess){
			CAlert::InformationAlert(error);
			break;
		}		
		result= kTrue;

		error = kNullString;
		if(Utils<IXPageUtils>()->ImportImages(targetPictures, xmlFileToImport, error) != kSuccess){
			CAlert::InformationAlert(error);
			break;
		}
		
		//Ajout par HF
		//Comme on se fout de l'affectation (on est sur du drag n drop d'un article depuis la palette)
		//on stocke de la persistence dans les blocs
		InterfacePtr<ICommand> placedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
		InterfacePtr<IPlacedArticleData> placedArticleData(placedArticleDataCmd, IID_IPLACEDARTICLEDATA);
		placedArticleData->SetUniqueId(idArt);
		IDFile xmlFolder = xmlFileToImport;
		FileUtils::GetParentDirectory(xmlFolder, xmlFolder);
		placedArticleData->SetStoryFolder(FileUtils::SysFileToPMString(xmlFolder));
		placedArticleData->SetStoryTopic(articleSubject);
		placedArticleDataCmd->SetItemList(formeItemsToLink);
		if(CmdUtils::ProcessCommand(placedArticleDataCmd)!= kSuccess)
			break; 
		
		
		// Invalidate document so that assignment adornments get paint			
		Utils<ILayoutUIUtils>()->InvalidateViews(doc);
		result= kTrue;

	}while(kFalse);
	return result;
}

UIDRef XPGUILinkArtDialogController::GetTargetStoryFromSnipet(const UIDList& formeItems){

		// Figure out where the story is, and get an image frame reference to import linked one if there is only in the forme
		UIDRef targetStory = UIDRef::gNull,   targetImageFrame = UIDRef::gNull;

		IDataBase* db  = formeItems.GetDataBase();

		UIDList linksItemsListTmp (db);
		int32 nbImgFrame = 0;

		for(int32 i = 0 ; i < formeItems.Length() ; ++i){
			
			InterfacePtr<IFrameType> frameType (db, formeItems[i], UseDefaultIID());
			InterfacePtr<IHierarchy> itemHierarchy( frameType, UseDefaultIID() );
            if (itemHierarchy ){
                 UIDList descendentList(db);
                 itemHierarchy->GetDescendents( &descendentList, IMultiColumnTextFrame::kDefaultIID );
                 if (!descendentList.IsEmpty())
                      linksItemsListTmp.Append(descendentList);                   
            }

			if(frameType->IsTextFrame()){
				if(targetStory == UIDRef::gNull){					
					InterfacePtr<IHierarchy> itemHier (frameType, UseDefaultIID());
					InterfacePtr<IMultiColumnTextFrame> txtFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
					targetStory = UIDRef(db, txtFrame->GetTextModelUID());						
				}
			}
			else if(frameType->IsGraphicFrame()){
				nbImgFrame++;
				targetImageFrame = formeItems.GetRef(i);
			}
		}

		if(nbImgFrame > 1)
			targetImageFrame = UIDRef::gNull;		

		// Link all Text Frames			
		int32 cp = 0;
		while( cp < linksItemsListTmp.Length() -1 ){			

			UIDList linksItemsList(db);
			linksItemsList.Append(linksItemsListTmp.At(cp));
			linksItemsList.Append(linksItemsListTmp.At(cp+1));	
			InterfacePtr<ICommand> textLinkCmd(CmdUtils::CreateCommand(kTextLinkCmdBoss));
			textLinkCmd->SetItemList(linksItemsList);
			CmdUtils::ProcessCommand(textLinkCmd);			
			cp++;			
		}

	return targetStory;
}

