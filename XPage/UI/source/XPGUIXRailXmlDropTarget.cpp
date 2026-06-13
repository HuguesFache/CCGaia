
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IPMDataObject.h"
#include "IFlavorData.h"
#include "IPMStream.h"
#include "ISysFileListData.h"
#include "IDataExchangeHandler.h"
#include "IXMLUtils.h"
#include "IStringData.h"
#include "IDialogMgr.h"
#include "IDialog.h"
#include "IApplication.h"
#include "ISysFileData.h"
#include "ITextSelectionSuite.h"
#include "ISelectionUtils.h"
#include "ITextModel.h"
#include "ISelectionManager.h"
#include "IFrameType.h"
#include "IGeometry.h"
#include "IUnitOfMeasure.h"
#include "ISnippetExport.h"
#include "IInCopyWorkFlowUtils.h"
#include "InCopyImportID.h"
#include "IAssignedDocument.h"

// General includes:
#include "CDragDropTargetFlavorHelper.h"
#include "CAlert.h"
#include "PMFlavorTypes.h"
#include "DataObjectIterator.h"
#include "UIDList.h"
#include "PMString.h"
#include "IHierarchy.h"
#include "FileUtils.h"
#include "IFrameUtils.h"
#include "StringUtils.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"
#include "CoreResTypes.h"
#include "AcquireModalCursor.h"
#include "DebugClassUtils.h"
#include "IStringListData.h"
#include "ILayoutUtils.h"
#include "TransformUtils.h"
#include "ISpread.h"
#include "ISubject.h"
#include "ILayoutUIUtils.h"
#include "IAssignmentMgr.h"
#include "IUIColorUtils.h"
#include "IAssignmentUtils.h"
#include "StreamUtil.h"
#include "IAssignSetPropsCmdData.h"
#include "IXPageUtils.h"
#include "IPlacedArticleData.h"
#include "IFormeData.h"
#include "IWebServices.h"
#include "IXPageMgrAccessor.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "XPageUIUtils.h"
#include "XMLDefs.h"

/** XPGUIXRailXmlDropTarget
	Provides the drop behaviour. Our drag and drop target accepts xml files from XPage Textes palette
*/
class XPGUIXRailXmlDropTarget : public CDragDropTargetFlavorHelper
{
	public:

		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		XPGUIXRailXmlDropTarget(IPMUnknown* boss);

		/**
			this method defines the target response to the drag. It is called when the mouse enters the panel. We
			inidcate we can accept drags with a XPageTextOnlyFlavor flavor, that we do not want the default cursor

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

	private:		
	
		ErrorCode DoStoryLink(const UIDList& formeItems, const PMString& idArt, const IDFile& xmlFileToImport, const PMString& artRubrique, const PMString& artSsRubrique, const PMString& artSubject, 
							  const PMString& classeurName, const PMString& formeName, const PMString& artIdStatus, const PMString& artLibelleStatus, const PMString& artCouleurStatus, 
							  const PMString& artFolio, int16& artType);

		ErrorCode DoNewAssignement(const UIDList& formeItemsToLink, const PMString& idArt, const PMString& articleSubject, 
								   const PMString& creatorLogin, int32& idPage, const PMString& rub, 
								   const PMString& ssRub, PMString& storyFullPath);
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUIXRailXmlDropTarget, kXPGUIXRailXmlDropTargetImpl)
DEFINE_HELPER_METHODS(XPGUIXRailXmlDropTarget)

/* XPGUIXRailXmlDropTarget Constructor
*/
XPGUIXRailXmlDropTarget::XPGUIXRailXmlDropTarget(IPMUnknown* boss)
: CDragDropTargetFlavorHelper(boss), HELPER_METHODS_INIT(boss)
{
}

DragDrop::TargetResponse XPGUIXRailXmlDropTarget::CouldAcceptTypes(const IDragDropTarget* target, DataObjectIterator* dataIter, const IDragDropSource* fromSource, const IDragDropController* controller) const
{
	DataExchangeResponse response;
	response = dataIter->FlavorExistsWithPriorityInAllObjects(XPageTextOnlyFlavor);
	if(response.CanDo() && Utils<IFrameUtils>()->IsTextFrame((IPMUnknown*) target)){	
 		return DragDrop::TargetResponse(response, DragDrop::kDropWillCopy, DragDrop::kUseDefaultTrackingFeedback, DragDrop::kTargetWillProvideCursorFeedback);
	}

	return DragDrop::kWontAcceptTargetResponse;
}

/*
	the drop has occured, we need to handle it.
*/
ErrorCode	
XPGUIXRailXmlDropTarget::ProcessDragDropCommand(IDragDropTarget* target , IDragDropController* controller, DragDrop::eCommandType action)
{
	ErrorCode returnCode = kFailure;
	
	do{
		if (action != DragDrop::kDropCommand)  
			break;
		 
		// We should ensure the drag has been internalized, if we are coming from a custom source, the handler may not be initialised
		if (controller->InternalizeDrag(XPageTextOnlyFlavor, XPageTextOnlyFlavor) != kSuccess)	
			break;

		InterfacePtr<IDataExchangeHandler> handler (controller->QuerySourceHandler());	
		InterfacePtr<ISysFileListData> fileData (handler, IID_ISYSFILELISTDATA);
		IDFile xmlFileToImport = fileData->GetSysFileItem(0);
		if(!FileUtils::DoesFileExist(xmlFileToImport)) // Message alert
			break;

		IDataBase *db = ::GetDataBase(target);
		if(!db)
			break;

		// Get the target story
		UIDRef targetStory = ::GetUIDRef(target);	

		InterfacePtr<IAssignmentMgr> assignmentMgr(GetExecutionContextSession(), UseDefaultIID());
		if(assignmentMgr->IsAssigned(targetStory)){
			PMString error(kXPGUITextLinkDestErrorKey);
			error.Translate();		 
			CAlert::ErrorAlert(error);	
			break;
		}

		// Get all item of carton from target story.
		UIDList formeItems(db);
		if(Utils<IXPageUtils>()->GetCartonItemsFromTarget(targetStory, formeItems) != kSuccess) // Apapap Message alert
			break;	

		if(formeItems.size() == 0)
			break;
		
		// Get curent page index from text frame
		UIDList targetStories(targetStory.GetDataBase());
		K2Vector<UIDRef> targetPictures;
		for(int32 i = 0; i < formeItems.size(); ++i){			
            // GD 27.04.2023 ++
            UIDRef parentUID = formeItems.GetRef(i);
            InterfacePtr<IFormeData> formeData(parentUID, UseDefaultIID());
            if (formeData != nil) {
                if (formeData->GetType() == IFormeData::kFixedContent)
                    continue;
            }
            // GD 27.04.2023 --
			InterfacePtr<IFrameType> frameType (db, formeItems[i], UseDefaultIID());
			if(frameType && frameType->IsTextFrame()){
				InterfacePtr<IHierarchy> itemHier (frameType, UseDefaultIID());
				InterfacePtr<IMultiColumnTextFrame> txtFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
				targetStories.Append(txtFrame->GetTextModelUID());
				}
			else if (frameType && frameType->IsGraphicFrame()) {
				targetPictures.push_back(::GetUIDRef(frameType));
			}
		}	
			
		// Get story data
		InterfacePtr<IStringListData> textData (handler, IID_ISTRINGLISTDATA);
		PMString artId = textData->GetStringList()[0];
		PMString artType = textData->GetStringList()[1];
		PMString artSubject = textData->GetStringList()[3];
		PMString artIdStatus = textData->GetStringList()[4];
		PMString artLibelleStatus = textData->GetStringList()[5];
		PMString artCouleurStatus = textData->GetStringList()[6];
		PMString artRubrique = textData->GetStringList()[7];
		PMString artSsRubrique = textData->GetStringList()[8];		
		PMString artFolio = textData->GetStringList()[9];
		
		// Get Story Text type : MarbreGeneral, MarbreParution
		PMString::ConversionError convError;
		int16 kArticleType = artType.GetAsNumber(&convError);
		if (convError != PMString::kNoError)
			break;
		
		// Get forme data
		InterfacePtr<IFormeData> formeData(targetStory, UseDefaultIID());
		if(formeData == nil)// Apapap Message alert
			break;

		PMString formeDescription = formeData->GetUniqueName();
		if(formeDescription == kNullString)
			break;		 
			
		// Get cartn and classeur from unique carton name
		PMString classeur, carton;
		Utils<IXPageUtils>()->SplitFormeDescription(formeDescription, classeur, carton);
		// Try to use matching file associated with the story if any	
		InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());

        IDFile cheatFile;
        PMString temp;
        FileUtils::GetPathOnly(xmlFileToImport, temp, kTrue);
        cheatFile = FileUtils::PMStringToSysFile(temp);
        IDFile matchingFile;
        
        FileUtils::AppendPath(&cheatFile, "cheatforme.xml");
        if (FileUtils::DoesFileExist(cheatFile)) { // Apapap Message alert
            matchingFile = cheatFile;
        }
        else {
            // Try to use matching file associated with the story if any
            PMString racine = xpgPrefs->GetCheminFormes();
            FileUtils::PMStringToIDFile(racine, matchingFile);
            FileUtils::AppendPath(&matchingFile, classeur);
            FileUtils::AppendPath(&matchingFile, "Typographie");
            FileUtils::AppendPath(&matchingFile, carton + ".xml");
        }
		
        
		if(!FileUtils::DoesFileExist(matchingFile)){  
			ErrorUtils::PMSetGlobalErrorCode(kSuccess); // Reset global error code
			PMString error(kXPGErrImportXmlFailed);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, FileUtils::SysFileToPMString(matchingFile));
            CAlert::InformationAlert(error);
            //CAlert::InformationAlert(matchingFilePath);
            CAlert::InformationAlert(FileUtils::SysFileToPMString(matchingFile));
			break;
		}
		
		PMString errorImport = kNullString;
		if(Utils<IXPageUtils>()->ImportArticle(targetStories, xmlFileToImport, matchingFile, errorImport, xmlFileToImport, artId) != kSuccess){
			CAlert::InformationAlert(errorImport);
			break;
		}	

		errorImport = kNullString;
		if(Utils<IXPageUtils>()->ImportImages(targetPictures, xmlFileToImport, errorImport) != kSuccess){
			CAlert::InformationAlert(errorImport);
			break;
		}	
		
		// Notify XRail
	 	if(this->DoStoryLink(formeItems, artId, xmlFileToImport, artRubrique, artSsRubrique, artSubject, classeur, carton,
							 artIdStatus, artLibelleStatus, artCouleurStatus, artFolio, kArticleType)!= kSuccess )
		{	// Apapap Message alert liaison avec XRail impossible	 
			break;    
		}	

		returnCode = kSuccess;
	 
	}while(kFalse);

	return returnCode;
}

void XPGUIXRailXmlDropTarget::DoDragEnter(IDragDropTarget * target){
	InterfacePtr<IDragDropController> controller (GetExecutionContextSession(), UseDefaultIID());
	controller->SetTrackingCursorFeedback(CursorSpec(kCrsrPlaceTextInto));
}

ErrorCode XPGUIXRailXmlDropTarget::DoStoryLink(const UIDList& formeItems, const PMString& idArt, const IDFile& xmlFileToImport, const PMString& artRubrique, 
											   const PMString& artSsRubrique, const PMString& artSubject, const PMString& classeur, const PMString& carton,
											   const PMString& artIdStatus, const PMString& artLibelleStatus, const PMString& artCouleurStatus, 
											   const PMString& artFolio, int16& artType){
	ErrorCode returnCode = kFailure;	
	do{
		IDataBase * db = formeItems.GetDataBase();
		if(!db)
			break;

			// Delier l'article s'il y en a dans le carton
		InterfacePtr<IPlacedArticleData> oldPlacedArticleData(formeItems.GetRef(0), IID_IPLACEDARTICLEDATA);
		if(oldPlacedArticleData){
			if(oldPlacedArticleData->GetUniqueId() != kNullString){

				PMString oldArtId = oldPlacedArticleData->GetUniqueId();			 
				
				UIDList storyItems(formeItems.GetDataBase());
				Utils<IXPageUtils>()->GetPlacedArtItemsFromTarget(formeItems.GetRef(0), storyItems);

				// Save new data of placed story				
 				InterfacePtr<ICommand> deletePlacedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));

				InterfacePtr<IPlacedArticleData> placedArticleData(deletePlacedArticleDataCmd, IID_IPLACEDARTICLEDATA);
				placedArticleData->SetUniqueId(kNullString);
				placedArticleData->SetStoryFolder(kNullString);
				placedArticleData->SetStoryTopic(kNullString);

				deletePlacedArticleDataCmd->SetItemList(storyItems);
				if(CmdUtils::ProcessCommand(deletePlacedArticleDataCmd)!= kSuccess)
					break;
				
			}
		}
			
		// Save new data of placed story
 		InterfacePtr<ICommand> placedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
		InterfacePtr<IPlacedArticleData> placedArticleData(placedArticleDataCmd, IID_IPLACEDARTICLEDATA);
		placedArticleData->SetUniqueId(idArt);
		IDFile xmlFolder = xmlFileToImport;
		FileUtils::GetParentDirectory(xmlFolder, xmlFolder);
		placedArticleData->SetStoryFolder(FileUtils::SysFileToPMString(xmlFolder));
		placedArticleData->SetStoryTopic(artSubject);
		placedArticleDataCmd->SetItemList(formeItems);
		if(CmdUtils::ProcessCommand(placedArticleDataCmd)!= kSuccess)
			break; 

		// Calcul des cordonnees des items selectionnes			
		PMPoint leftTopMost (kMaxInt32, kMaxInt32), rightBottomMost (-kMaxInt32, -kMaxInt32);

		UID leftMostSelectedItem = kInvalidUID;		
		for(int32 i = 0 ; i < formeItems.Length() ; ++i) {			
			
			InterfacePtr<IGeometry> itemGeo (formeItems.GetRef(i), UseDefaultIID());
			PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();
			PMPoint rightBottom = itemGeo->GetStrokeBoundingBox().RightBottom();
			 
			::TransformInnerPointToPasteboard (itemGeo, &leftTop);
			::TransformInnerPointToPasteboard (itemGeo, &rightBottom);
	
			if(leftTop.X() < leftTopMost.X()){
				leftMostSelectedItem = formeItems.At(i);
				leftTopMost.X() = leftTop.X();
			}			
			if(leftTop.Y() < leftTopMost.Y())
				leftTopMost.Y() = leftTop.Y();		

			if(rightBottom.X() > rightBottomMost.X())
				rightBottomMost.X() = rightBottom.X();

			if(rightBottom.Y() > rightBottomMost.Y())
				rightBottomMost.Y() = rightBottom.Y();
		}

		// Send notification so that texte panel is updated
		InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
		sessionSubject->Change(kXPGRefreshMsg, IID_IREFRESHPROTOCOL);					

		returnCode = kSuccess;
		
	}while(kFalse);
	return returnCode;
}


ErrorCode XPGUIXRailXmlDropTarget::DoNewAssignement(const UIDList& formeItemsToLink, const PMString& idArt, const PMString& articleSubject, const PMString& creatorLogin, int32& idPage, const PMString& rub, const PMString& ssRub, PMString& storyFullPath){

	ErrorCode error = kFailure;
	do{
		
		// Get Path of the assignment	
		storyFullPath.Append(idArt);
		storyFullPath.Append(PMString(kXPGExtensionInCopyKey, PMString::kTranslateDuringCall));
		IDFile assignFile = FileUtils::PMStringToSysFile(storyFullPath);
	
		if(Utils<IXPageUtils>()->DoNewAssignment(formeItemsToLink, assignFile, articleSubject, creatorLogin, kTrue, idArt, kFalse) != kSuccess)
			break;


		error = kSuccess;
	}while(kFalse);

	return error;
}
	
// End, XPGUIXRailXmlDropTarget.cpp.
