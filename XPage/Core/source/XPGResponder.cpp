#include "VCPlugInHeaders.h"

// Interface includes:
#include "IClassIDData.h"
#include "ISignalMgr.h"
#include "IDocument.h"
#include "IDocumentSignalData.h"
#include "IUIDData.h"
#include "IDialog.h"
#include "CAlert.h"
#include "ISession.h"
#include "IUIDRefListData.h"
#include "IPageList.h"
#include "INewLayerCmdData.h"
#include "ILayerList.h"
#include "IUnitOfMeasure.h"
#include "ISpread.h"
#include "IHierarchy.h"
#include "IGeometry.h"
#include "StreamUtil.h"
#include "IXRailPageSlugData.h"
#include "IDocumentLayer.h"
#include "ISpreadLayer.h"
#include "IDialogMgr.h"
#include "IApplication.h"
#include "LocaleSetting.h"
#include "IItemLockData.h"
#include "ISelectionManager.h"
#include "ISelectionUtils.h"
#include "IAssignmentMgr.h"
#include "IAssignmentUtils.h"
#include "IInCopyWorkFlowUtils.h"
#include "IItemLockData.h"
#include "ITextModel.h"
#include "IUIFlagData.h"
#include "CAlert.h"
#include "ILiveEditFacade.h"
#include "ILinkFacade.h"
#include "ILinksManager.h"
#include "IObserver.h"
#include "IDataLink.h"
#include "IInCopyDocUtils.h"
#include "ILinkUtils.h"
#include "ILinkResource.h"
#include "ILinkObject.h"
#include "IURIUtils.h"
#include "IXPageUtils.h"
#include "IPlacedArticleData.h"

// Implementation includes:

#include "CreateObject.h"
#include "CResponder.h"
#include "FileUtils.h"
#include "CoreResTypes.h"
#include "PMString.h"
#include "IStringData.h"
#include "ErrorUtils.h"
#include "TransformUtils.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"
#include "IMasterSpreadList.h"
#include "IMasterSpread.h"

// Project includes
#include "XPGID.h"
#include "XPGUIID.h"
#include "TextID.h"
#include "PageItemScrapID.h"
#include "IXPageSuite.h"

#include "ImageID.h"
#include "ImageImportUIID.h"
#include "EPSUIID.h"

class XPGResponder : public CResponder
{
	public:
		XPGResponder(IPMUnknown* boss);
		virtual void Respond(ISignalMgr* signalMgr);		

    private:
        void ImportAutoArticles(UIDRef doc, const K2Vector<int32>& tabIDPages);

};

CREATE_PMINTERFACE(XPGResponder, kXPGResponderImpl)

XPGResponder::XPGResponder(IPMUnknown* boss) 
			 :CResponder(boss){}


void XPGResponder::Respond(ISignalMgr* signalMgr)
{
    // Get a UIDRef for the document.  It will be an invalid UIDRef
    // for BeforeNewDoc, BeforeOpenDoc, AfterSaveACopy, and AfterCloseDoc because
    // the document doesn't exist at that point.
    InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
    InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());

    if (docData == nil)
    {
        ASSERT_FAIL("Invalid IDocumentSignalData* - XPageResponder::Respond");
        return;
    }
    UIDRef doc = docData->GetDocument();
    
    
	ServiceID serviceTrigger = signalMgr->GetServiceID();
	switch (serviceTrigger.Get()){

        case kDuringOpenDocSignalResponderService:
        {
            if (xpgPrefs->GetGestionIDMS() == 1) {
                InterfacePtr<IDocument> theDoc(doc, UseDefaultIID());
                if (!theDoc->IsReadOnly()) {
                    int32 nbPages = GetNbPages(doc);
                    K2Vector<int32> tabIDPages;

                    for (int16 i = 0; i < nbPages; i++) {
                        int32 idPage = Utils<IXPageUtils>()->GetPageID(doc, i);
                        if (idPage > 0) {
                            tabIDPages.push_back(idPage);
                        }
                    }

                    if (tabIDPages.size() == nbPages) { // we only handle IDMS stories if all page IDs are set correctly
                        ImportAutoArticles(doc, tabIDPages);
                    }
                }
            }
			break;
		}

        case kAfterNewDocSignalResponderService:
        case kAfterOpenDocSignalResponderService:
        {
            // Wake up XPGDocObserver on this document so its hierarchy
            // hook (kDeletePageItemCmdBoss → auto-renumber) actually
            // receives notifications. The framework doesn't auto-attach
            // CObserver-based observers on kDocBoss by itself ; we have
            // to drive it from a signal responder. Pattern lifted from
            // the SDK persistentlist sample (PstLstDocResponder).
            InterfacePtr<IObserver> iDocObserver(doc, IID_IDOCOBSERVER);
            if (iDocObserver != nil)
                iDocObserver->AutoAttach();
            break;
        }

        case kBeforeCloseDocSignalResponderService:
		{
			// Symmetric to AutoAttach in the open/new branches above —
			// detach XPGDocObserver before the doc tears down so we
			// don't leave dangling subject pointers.
			InterfacePtr<IObserver> iDocObserverToDetach(doc, IID_IDOCOBSERVER);
			if (iDocObserverToDetach != nil)
				iDocObserverToDetach->AutoDetach();

			InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
			UIDRef docRef = docData->GetDocument();
			InterfacePtr<IDocument> theDoc (docRef, UseDefaultIID());
			if(!theDoc)
				break;
			IDataBase * db = ::GetDataBase(theDoc);
			if(db == nil)
				break;
			
			InterfacePtr<IAssignmentMgr> assignmentMgr (GetExecutionContextSession(), UseDefaultIID());
			//IAssignedDocument * assignDoc = assignmentMgr->GetAssignedDocument(db);
			//if(!assignDoc)
			//	break;
			
			InterfacePtr<IAssignedDocument> assignedDoc(theDoc, UseDefaultIID());
			if(assignedDoc == nil)
				break;
			
			InterfacePtr<IAssignment> assignment (db, assignedDoc->GetAssignments()[0], UseDefaultIID());
			if(assignment == nil)
				break;
			
			// Check in stories of assignment before closing document
			UIDList storyList = assignment->GetStories();
			
			//Utils<Facade::ILiveEditFacade>()->CancelCheckOut(storyList);
			Utils<Facade::ILiveEditFacade>()->CheckIn ( storyList, Utils<Facade::ILiveEditFacade>()->enUnlockAsset, nil, nil, ICommand::kAutoUndo, K2::kSuppressUI);
			
//#if INCOPY
/*		    do{
				InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
                UIDRef docRef = docData->GetDocument();		   
				InterfacePtr<IDocument> theDoc (docRef, UseDefaultIID());
                if(!theDoc)
                    break;    
				
				IDataBase * db = ::GetDataBase(theDoc);
				if(db == nil)
					break;
				
				// Detach observer
				InterfacePtr<IObserver> iDocObserver(theDoc, IID_IDOCOBSERVER);
				if (iDocObserver) 
					iDocObserver->AutoDetach();
				 
				
				//Doc Name	
				InterfacePtr<IAssignedDocument> assignedDoc(theDoc, UseDefaultIID());
				if(assignedDoc == nil)
					break;
				
				InterfacePtr<IAssignment> assignment (db, assignedDoc->GetAssignments()[0], UseDefaultIID());
				if(assignment == nil)
					break;
				
                // Check in stories of assignment before closing document
				UIDList storyList = assignment->GetStories();
            	Utils<Facade::ILiveEditFacade>()->CheckIn(storyList);
				break;        
            }while(kFalse);          
 */
//#endif				
     	}             
    	default:
            break;                         
	}
}

void XPGResponder::ImportAutoArticles(UIDRef doc, const K2Vector<int32>& IDPages) {
    do {
        
        InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IWebServices> baseHTTP (::CreateObject2<IWebServices>(kXRCXRailClientBoss));
        
        K2Vector<int32> TabIDStories, TabPagesIDS;
        K2Vector<PMString> TabIDMSpaths, TabXMLStorypaths, TabSubjects;
        K2Vector<PMReal> TabX, TabY;
        
        if(!baseHTTP->GetListeIDMS_v2(xpgPrefs->GetTEC_URL(), IDPages, TabIDStories, TabPagesIDS, TabX, TabY, TabIDMSpaths, TabXMLStorypaths, TabSubjects))
		{
            CAlert::InformationAlert("Impossible de r�cup�rer les articles � importer");
            break;
		}
        
        // Get the list of pages
        InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
        int32 nbPages = pageList->GetPageCount();
        // Create converter (coordinates/dimensions are sent in millimeters)
        InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));
        if(uom == nil)
            break;
        
        InterfacePtr<IDocument> theDoc (doc, UseDefaultIID());
        IDataBase * db = ::GetDataBase(theDoc);
        K2Vector<KeyValuePair <PMString, K2Pair <PMString, UIDList> > > placedArticleList = Utils<IXPageUtils>()->GetPlacedStoriesList(theDoc);
        
        int32 nbStories = TabIDStories.size();
        for(int32 i = 0 ; i < nbStories ; ++i) {
            
            UIDList formeItems(db);
            PMString error;
            PMString idArt;
            idArt.AppendNumber(TabIDStories[i]);
            
            // Get the page where the item for the IDMS must be created
            UID pageUID = kInvalidUID;
            for (int16 j = 0; j < nbPages; j++)
            {
                int32 curpageID = Utils<IXPageUtils>()->GetPageID(doc, j);
                if(curpageID == TabPagesIDS[i])
                {
                    pageUID = pageList->GetNthPageUID(j);
                    break;
                }
            }
            
            if(pageUID == kInvalidUID) // this page doesn't exist in this document
            {
                CAlert::ErrorAlert(kXRLErrorInvalidResaPageKey);
                continue;
            }
            
            // Compute IDMS coordinates in pasteboard coordinates
            PMPoint leftTop(uom->UnitsToPoints(TabX[i]), uom->UnitsToPoints(TabY[i]));
            InterfacePtr<IGeometry> ownerPageGeo (UIDRef(db, pageUID), UseDefaultIID());
            ::TransformInnerPointToPasteboard(ownerPageGeo, &leftTop);
            
            // Get target spread
            InterfacePtr<IHierarchy> currentPageHier (ownerPageGeo, UseDefaultIID());
            UIDRef targetSpread (db, currentPageHier->GetSpreadUID());
            
            // Check if story is already in page
            UIDList storyItems(db);
            for(int32 k= 0; k < placedArticleList.size(); k++) {
                if(placedArticleList[k].Key().IsEqual(idArt)) {
                    storyItems.Append(placedArticleList[k].Value().second);
                    // if yes, delete items from document before IDMS import
                    InterfacePtr<ICommand> deletePageItemCmd (CmdUtils::CreateCommand(kDeleteCmdBoss));
                    deletePageItemCmd->SetItemList(storyItems);
                    if(CmdUtils::ProcessCommand(deletePageItemCmd) != kSuccess)
                    {
                        CAlert::InformationAlert("XPage - Erreur suppression article avant reimport IDMS:"+TabIDMSpaths[i]);
						ErrorUtils::PMSetGlobalErrorCode(kSuccess);
                        break;
                    }
                    break;
                }			
            }
            
			IDFile mySnippet;
			mySnippet = FileUtils::PMStringToSysFile(TabIDMSpaths[i]);
			FileUtils::AppendPath(&mySnippet, idArt + ".idms");
			if (FileUtils::DoesFileExist(mySnippet)) {
				// Import snippet
				if (Utils<IXPageUtils>()->ImportForme(doc, mySnippet, leftTop, targetSpread, IDFile(), error, &formeItems, kTrue, kTrue) != kSuccess) {
					CAlert::InformationAlert("XPage - Erreur import IDMS:"+TabIDMSpaths[i]);
					ErrorUtils::PMSetGlobalErrorCode(kSuccess);
					continue;
				}
				// Save article data
				InterfacePtr<ICommand> placedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
				InterfacePtr<IPlacedArticleData> placedArticleData(placedArticleDataCmd, IID_IPLACEDARTICLEDATA);
				placedArticleData->SetUniqueId(idArt);
				placedArticleData->SetStoryFolder(TabIDMSpaths[i]);
				placedArticleData->SetStoryTopic(TabSubjects[i]);
				placedArticleDataCmd->SetItemList(formeItems);
                if (CmdUtils::ProcessCommand(placedArticleDataCmd) != kSuccess) {
                    CAlert::InformationAlert("Impossible de renseigner les datas: "+TabIDMSpaths[i]);
					continue;
                }

				//on cree un fichier pour dire que le idms a ete mis a jour
                if (xpgPrefs->GetNoUpdateFile() != "1") {IDFile updateFile = FileUtils::PMStringToSysFile(TabIDMSpaths[i]);
				FileUtils::AppendPath(&updateFile, idArt + ".update");
				InterfacePtr<IPMStream> writeStream(StreamUtil::CreateFileStreamWrite(updateFile, kOpenOut | kOpenTrunc, kTextType));
				if (writeStream == nil)
				{
					ASSERT_FAIL("XPGResponder::ImportAutoArticles -> writeStream nil");
					continue;
				}
				writeStream->Close();
			}
			}
			else {
				CAlert::InformationAlert("XPage - Fichier Introuvable: "+ TabIDMSpaths[i]);
				ErrorUtils::PMSetGlobalErrorCode(kSuccess);
				break;
			}
            
        }
        
    } while (kFalse);
    
}

