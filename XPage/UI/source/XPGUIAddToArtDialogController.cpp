/*
//    File:    XPGUIAddItemsToArtDialogController.cpp
//
//  Author : Trias Developpement
*/
#include "VCPlugInHeaders.h"

// General includes:
#include "CDialogController.h"
#include "IAssignedDocument.h"
#include "IDocument.h"
#include "IActiveContext.h"
#include "UIDList.h"
#include "CAlert.h"
#include "IDropDownListController.h"
#include "IPanelControlData.h"
#include "IStringListControlData.h"
#include "IAssignmentMgr.h"
#include "ISelectionUtils.h"
#include "FileUtils.h"
#include "Utils.h"
#include "IAssignmentUtils.h"
#include "ITextModel.h"
#include "IPMStream.h"
#include "StreamUtil.h"
#include "ISnippetExport.h"
#include "IInCopyWorkFlowUtils.h"
#include "InCopyImportID.h"
#include "IUIDData.h"
#include "IPageItemAdornmentList.h"
#include "IClassIDData.h"
#include "K2Vector.h"
#include "K2Vector.tpp"
#include "KeyValuePair.h"
#include "IObserver.h"
#include "IWidgetUtils.h"
#include "ISubject.h"
#include "IPanelMgr.h"
#include "IApplication.h"
#include "ILayoutUIUtils.h"
#include "IXPageUtils.h"

// Project includes:
#include "IXPageSuite.h"
#include "IPlacedArticleData.h"
#include "XPGUIID.h"
#include "XPGID.h"

class XPGUIAddToArtDialogController : public CDialogController{

    public:
    
        XPGUIAddToArtDialogController(IPMUnknown* boss):CDialogController(boss){
            doc = nil;
            assignmentList.clear();
        }

        virtual ~XPGUIAddToArtDialogController() {
            if(doc)    doc->Release();
        }

        virtual void InitializeDialogFields(IActiveContext* dlgContext);
        virtual WidgetID ValidateDialogFields(IActiveContext* myContext);
        virtual void ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);

    private :

        ErrorCode AddToAssignment(IAssignment* assignment, const UIDList& items);
        ErrorCode AddToPlacedStory(IPlacedArticleData* placedArticleData, const UIDList& items);
        void AddAdonments();

        K2Vector<KeyValuePair<PMString, UIDRef> > assignmentList;
        IDocument * doc;
            
};

CREATE_PMINTERFACE(XPGUIAddToArtDialogController, kXPGUIAddToArtDialogControllerImpl)

void XPGUIAddToArtDialogController::InitializeDialogFields(IActiveContext* dlgContext){
    
    InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
    do{

         InterfacePtr<ITriStateControlData> triStateControlData (panelControlData->FindWidget(kXPGUIRadioPlacedStoryWidgetID), UseDefaultIID());
        if(!triStateControlData)
            break;
        triStateControlData->SetState(ITriStateControlData::kSelected);

        // Clear stories list
        InterfacePtr<IDropDownListController> storiesListController (panelControlData->FindWidget(kXPGUIAssignmentListWidgetID), UseDefaultIID());
        if(!storiesListController)
            break;

        InterfacePtr<IStringListControlData> storiesListControlData(storiesListController, UseDefaultIID());
        if(!storiesListControlData)
            break;

        storiesListControlData->Clear();

    } while(kFalse);
}

// --------------------------------------------------------------------------------------
// ValidateFields
// --------------------------------------------------------------------------------------
WidgetID XPGUIAddToArtDialogController::ValidateDialogFields(IActiveContext* myContext){
    
    WidgetID badWidgetID = CDialogController::ValidateDialogFields(myContext);
    InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
    
    do{
        IControlView* controlView = panelControlData->FindWidget(kXPGUIAssignmentListWidgetID);
        InterfacePtr<IStringListControlData> stringListControlData(controlView, UseDefaultIID());
        if (stringListControlData->Length() == 0){
            badWidgetID = kXPGUIAssignmentListWidgetID;
            break;
        }

    }while(kFalse);
    return badWidgetID;
}

// --------------------------------------------------------------------------------------
// ApplyFields
// --------------------------------------------------------------------------------------
void XPGUIAddToArtDialogController::ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId){

    do{

        InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
        if(!panelControlData)
            break;
    
        InterfacePtr<IUIDData> uiDataTarget(this, UseDefaultIID());
        if(!uiDataTarget)
            break;

        InterfacePtr<ITriStateControlData> triStatePlacedStoryControlData (panelControlData->FindWidget(kXPGUIRadioPlacedStoryWidgetID), UseDefaultIID());
        if(!triStatePlacedStoryControlData)
            break;

        // Get selected items
        UIDList selectedItems, txtFrames, graphicFrames;
        InterfacePtr<ISelectionManager> selectionMgr (Utils<ISelectionUtils>()->QueryActiveSelection());
        InterfacePtr<IXPageSuite> xpgSuite (selectionMgr, IID_IXPAGESUITE);
        if(xpgSuite == nil) // xpgSuite == nil means there is no active selection (neither layout nor text)
            break;

        xpgSuite->GetSelectedItems(txtFrames, graphicFrames, selectedItems);
    
        // Process command add to story
        if(triStatePlacedStoryControlData->GetState() == ITriStateControlData::kSelected){
            UIDRef placedStoryTargetRef = uiDataTarget->GetRef();
            InterfacePtr<IPlacedArticleData> placedArticleData (placedStoryTargetRef, UseDefaultIID());
            if(placedArticleData && placedArticleData->GetUniqueId() != kNullString){
                if(this->AddToPlacedStory(placedArticleData, selectedItems) != kSuccess){
                    CAlert::InformationAlert(placedArticleData->GetStoryTopic());// alert
                    break;
                }
            }
        }
        
        else{
            UIDRef assignementTargetRef = uiDataTarget->GetRef();
            InterfacePtr<IAssignment> assignment(assignementTargetRef, UseDefaultIID());
            if(assignment){
                if(this->AddToAssignment(assignment, txtFrames) != kSuccess){
                    CAlert::InformationAlert(assignment->GetName());//alert
                    break;
                }
            }
        }

    }while(kFalse);
}


ErrorCode XPGUIAddToArtDialogController::AddToAssignment(IAssignment* assignment, const UIDList& txtFrames){

    ErrorCode errorCode = kFailure;

    do{
        IDataBase* db = txtFrames.GetDataBase();

        // Add items to assignment and update it
        UIDList textStories(db);
        if(Utils<IXPageUtils>()->DoAddToAssignment(assignment, txtFrames, textStories, kTrue) != kSuccess)
            break;
        
        InterfacePtr<IDocument> docAdd(db, db->GetRootUID(), UseDefaultIID());
        Utils<ILayoutUIUtils>()->InvalidateViews(docAdd);

        errorCode = kSuccess;
    }while(kFalse);
    return errorCode;

}


ErrorCode XPGUIAddToArtDialogController::AddToPlacedStory(IPlacedArticleData* placedArticleData, const UIDList& items){

    ErrorCode errorCode = kFailure;
    do{
        // Save new data of placed story
         InterfacePtr<ICommand> placedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));

        InterfacePtr<IPlacedArticleData> newPlacedArticleData(placedArticleDataCmd, IID_IPLACEDARTICLEDATA);
        newPlacedArticleData->SetUniqueId(placedArticleData->GetUniqueId());
        newPlacedArticleData->SetStoryFolder(placedArticleData->GetStoryFolder());
        newPlacedArticleData->SetStoryTopic(placedArticleData->GetStoryTopic());

        placedArticleDataCmd->SetItemList(items);
        if(CmdUtils::ProcessCommand(placedArticleDataCmd)!= kSuccess)
            break;
        
        errorCode = kSuccess;

    }while(kFalse);
    return errorCode;
}
