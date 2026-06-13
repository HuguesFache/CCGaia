/*
 //	File:	PLBPageHelper.cpp
 //
 //	Author:	Nicolas METAYE
 //
 //	Date:	28-Oct-2003
 //
 //	ADOBE SYSTEMS INCORPORATED
 //	Copyright 2003 Trias Developpement. All rights reserved.
 //
 */

// Plug-in includes
#include "VCPlugInHeaders.h"

// General includes
#include "CmdUtils.h"
#include "UIDList.h"
#include "LayoutID.h"
#include "SDKUtilities.h"
#include "ErrorUtils.h"

// Interface includes
#include "ICommand.h"
#include "IPageCmdData.h"
#include "IApplyMasterCmdData.h"
#include "IBoolData.h"
#include "ISectionCmdData.h"
#include "ILayoutCmdData.h"
#include "IUIDData.h"
#include "IGeometry.h"
#include "IPageList.h"
#include "IMasterSpreadList.h"
#include "IApplyMasterCmdData.h"
#include "ITextDataValidation.h"
#include "IMasterSpreadCmdData.h"

// Project includes
#include "PLBPageHelper.h"

CREATE_PMINTERFACE(PLBPageHelper,kPLBPageHelperImpl)

/* Constructor
 */
PLBPageHelper::PLBPageHelper(IPMUnknown * boss)
: CPMUnknown<IPageHelper>(boss)
{
}

/* Destructor
 */
PLBPageHelper::~PLBPageHelper()
{
}


/* CreateNewPageCmd
 */

ErrorCode PLBPageHelper::CreateNewPageCmd(const UIDRef& spreadToAddTo,int32 numToInsert, int32 posToInsertAt, UIDRef masterUID, bool8 allowUndo)
{
    
    ErrorCode status = kFailure ;
    PMString error(kPlbNilInterface);
    
    do
    {
        
        // Create a NewPageCmd:
        InterfacePtr<ICommand>	newPageCmd(CmdUtils::CreateCommand(kNewPageCmdBoss));
        if(newPageCmd == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewPageCmd -> newPageCmd nil");
            break;
        }
        
        // Set command undoability
        if(!allowUndo)
            newPageCmd->SetUndoability(ICommand::kAutoUndo);
        
        
        // Get an IPageCmdData Interface for the NewPageCmd:
        InterfacePtr<IPageCmdData> newPageData(newPageCmd, IID_IPAGECMDDATA);
        if(newPageData == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewPageCmd -> newPageData nil");
            break;
        }
        
        // Set the IPageCmdData Interface's data:
        //newPageData->Set(spreadToAddTo, numToInsert, posToInsertAt, pageSize,kDefaultBinding);
        newPageData->SetNewPageCmdData(spreadToAddTo, numToInsert, posToInsertAt);
        
        
        // Get an IApplyMasterCmdData Interface for the NewPageCmd:
        InterfacePtr<IApplyMasterCmdData> masterPageData(newPageCmd,IID_IAPPLYMASTERCMDDATA);
        if(masterPageData == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewPageCmd -> masterPageData nil");
            break;
        }
        
        
        // Set IApplyMasterCmdData Interface's data:
        masterPageData->SetApplyMasterCmdData(masterUID, IApplyMasterCmdData::kKeepCurrentPageSize);
        
        
        // Get an IBoolData Interface for the NewPageCmd:
        InterfacePtr<IBoolData> doShuffle(newPageCmd, IID_IBOOLDATA);
        if(doShuffle == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewPageCmd -> doShuffle nil");
            break;
        }
        
        
        // Set the IBoolData Interface's data to shuffle pages:
        //reallocate pages to spreads
        doShuffle->Set(kTrue);
        // Process the NewPageCmd:
        
        if (CmdUtils::ProcessCommand(newPageCmd) != kSuccess)
        {
            error = PMString(kNewPageCmdFailed);
            break;
        }
        
        status = kSuccess ;
        
    } while(false);
    
    if(status != kSuccess)
        ErrorUtils::PMSetGlobalErrorCode(kFailure);
    
    return status;
}


/* CreateNewPageUIDListCmd
 */

ErrorCode PLBPageHelper::CreateNewPageUIDListCmd(const UIDRef& spreadToAddTo,int32 numToInsert, int32 posToInsertAt, UIDRef masterUID, UIDList * pageList, bool8 allowUndo)
{
    
    ErrorCode status = kFailure ;
    PMString error(kPlbNilInterface);
    
    do
    {
        
        // Create a NewPageCmd:
        InterfacePtr<ICommand>	newPageCmd(CmdUtils::CreateCommand(kNewPageCmdBoss));
        if(newPageCmd == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewPageCmd -> newPageCmd nil");
            break;
        }
        
        // Set command undoability
        if(!allowUndo)
            newPageCmd->SetUndoability(ICommand::kAutoUndo);
        
        
        // Get an IPageCmdData Interface for the NewPageCmd:
        InterfacePtr<IPageCmdData> newPageData(newPageCmd, IID_IPAGECMDDATA);
        if(newPageData == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewPageCmd -> newPageData nil");
            break;
        }
        
        // Set the IPageCmdData Interface's data:
        //newPageData->Set(spreadToAddTo, numToInsert, posToInsertAt, pageSize,kDefaultBinding);
        newPageData->SetNewPageCmdData(spreadToAddTo, numToInsert, posToInsertAt);
        
        
        // Get an IApplyMasterCmdData Interface for the NewPageCmd:
        InterfacePtr<IApplyMasterCmdData> masterPageData(newPageCmd,IID_IAPPLYMASTERCMDDATA);
        if(masterPageData == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewPageCmd -> masterPageData nil");
            break;
        }
        
        // Set IApplyMasterCmdData Interface's data:
        masterPageData->SetApplyMasterCmdData(masterUID, IApplyMasterCmdData::kKeepCurrentPageSize);
        
        // Get an IBoolData Interface for the NewPageCmd:
        InterfacePtr<IBoolData> doShuffle(newPageCmd, IID_IBOOLDATA);
        if(doShuffle == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewPageCmd -> doShuffle nil");
            break;
        }
        
        
        // Set the IBoolData Interface's data to shuffle pages:
        //reallocate pages to spreads
        doShuffle->Set(kTrue);
        // Process the NewPageCmd:
        
        if (CmdUtils::ProcessCommand(newPageCmd) != kSuccess)
        {
            error = PMString(kNewPageCmdFailed);
            break;
        }
        
        pageList->Append(*(newPageCmd->GetItemList())) ;
        
        status = kSuccess ;
        
    } while(false);
    
    if(status != kSuccess)
        ErrorUtils::PMSetGlobalErrorCode(kFailure);
    
    return status;
}


/* CreateNewSectionCmd
 */

ErrorCode PLBPageHelper::CreateNewSectionCmd(const UIDRef& sectionList, UID startPageUID,int32 startPageNumber, PMString prefixe, PMString marker,
                                             int32 pageNumStyle, bool16 autoNumber, bool16 includeSectionPrefix, bool8 allowUndo)
{
    
    ErrorCode status = kFailure;
    PMString error(kPlbNilInterface);
    
    do
    {
        // Create a NewSectionCmd:
        InterfacePtr<ICommand> newSectionCmd(CmdUtils::CreateCommand(kNewSectionCmdBoss));
        if(newSectionCmd == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewSectionCmd -> newSectionCmd nil");
            break;
        }
        
        // Set command undoability
        if(!allowUndo)
            newSectionCmd->SetUndoability(ICommand::kAutoUndo);
        
        // Get an ISectionCmdData Interface for the NewSectionCmd:
        InterfacePtr<ISectionCmdData> newSectionData(newSectionCmd,IID_ISECTIONCMDDATA);
        if(newSectionData == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewSectionCmd ->  newSectionData nil");
            break;
        }
        
        ClassID styleClass = kInvalidClass;
        switch(pageNumStyle)
        {
            case 1 : styleClass = kArabicPageNumberingStyleServiceBoss; break;
            case 2 : styleClass = kRomanUpperPageNumberingStyleServiceBoss; break;
            case 3 : styleClass = kRomanLowerPageNumberingStyleServiceBoss; break;
            case 4 : styleClass = kLettersUpperPageNumberingStyleServiceBoss; break;
            case 5 : styleClass = kLettersLowerPageNumberingStyleServiceBoss; break;
            default : styleClass = kArabicPageNumberingStyleServiceBoss; break;
        }
        
        // Set the ISectionCmdData Interface's data:
        newSectionData->Set(sectionList,startPageUID, startPageNumber, &prefixe, &marker,styleClass, autoNumber) ;
        newSectionData->SetPageNumberIncludeSectionPrefix(includeSectionPrefix);
        
        // Process the NewSectionCmd:
        if (CmdUtils::ProcessCommand(newSectionCmd) != kSuccess)
        {
            error = PMString(kNewSectionCmdFailed);
            break;
        }
        
        status = kSuccess ;
        
    } while(false);
    
    if(status != kSuccess)
        ErrorUtils::PMSetGlobalErrorCode(kFailure);
    
    return status;
}


/* CreateNewSectionCmd
 */

ErrorCode PLBPageHelper::ModifSectionCmd(IDataBase *db, const UIDRef& sectionList, UID sectionID, UID startPageUID, int32 startPageNumber, PMString prefixe, PMString marker,
    int32 pageNumStyle, bool16 autoNumber, bool16 includeSectionPrefix, bool8 allowUndo)
{

    ErrorCode status = kFailure;
    PMString error(kPlbNilInterface);

    do
    {

        InterfacePtr<ICommand> myCmd(CmdUtils::CreateCommand(kModifySectionSettingsCmdBoss));
        if (myCmd == nil)
        {
            ASSERT_FAIL("PLBPageHelper::ModifSectionCmd -> myCmd nil");
            break;
        }

        // Get an ISectionCmdData Interface for the NewSectionCmd:
        InterfacePtr<ISectionCmdData> modifSectionData(myCmd, IID_ISECTIONCMDDATA);
        if (modifSectionData == nil)
        {
            ASSERT_FAIL("PLBPageHelper::ModifSectionCmd ->  modifSectionData nil");
            break;
        }

        ClassID styleClass = kInvalidClass;
        switch (pageNumStyle)
        {
        case 1: styleClass = kArabicPageNumberingStyleServiceBoss; break;
        case 2: styleClass = kRomanUpperPageNumberingStyleServiceBoss; break;
        case 3: styleClass = kRomanLowerPageNumberingStyleServiceBoss; break;
        case 4: styleClass = kLettersUpperPageNumberingStyleServiceBoss; break;
        case 5: styleClass = kLettersLowerPageNumberingStyleServiceBoss; break;
        default: styleClass = kArabicPageNumberingStyleServiceBoss; break;
        }

        // Set the ISectionCmdData Interface's data:
        modifSectionData->Set(sectionList, startPageUID, startPageNumber, &prefixe, &marker, styleClass, autoNumber);
        modifSectionData->SetPageNumberIncludeSectionPrefix(includeSectionPrefix);
         
        myCmd->SetItemList(UIDList(db, sectionID));
        myCmd->SetUndoability(ICommand::kAutoUndo);
        
        // Process the NewSectionCmd:
        if (CmdUtils::ProcessCommand(myCmd) != kSuccess)
        {
            error = PMString(kNewSectionCmdFailed);
            break;
        }

        status = kSuccess;

    } while (false);

    if (status != kSuccess)
        ErrorUtils::PMSetGlobalErrorCode(kFailure);

    return status;
}



/* DeletePageCmd
 */

ErrorCode PLBPageHelper::DeletePageCmd(const UIDRef& pageToDelete, bool8 allowUndo)
{
    
    ErrorCode status = kFailure ;
    PMString error(kPlbNilInterface);
    
    do
    {
        // Create a DeletePageCmd:
        InterfacePtr<ICommand> deleteCmd(CmdUtils::CreateCommand(kDeletePageCmdBoss));
        if(deleteCmd == nil)
        {
            ASSERT_FAIL("PLBPageHelper::DeletePageCmd -> newPageCmd nil");
            break;
        }
        
        // Set command undoability
        if(!allowUndo)
            deleteCmd->SetUndoability(ICommand::kAutoUndo);
        
        
        // Set the DeletePageCmd's ItemList:
        deleteCmd->SetItemList(UIDList(pageToDelete));
        
        // Process the DeletePageCmd:
        if (CmdUtils::ProcessCommand(deleteCmd) != kSuccess)
        {
            error = PMString(kDeletePageCmdFailed);
            break;
        }
        
        status = kSuccess ;
        
    } while(false);
    
    if(status != kSuccess)
        ErrorUtils::PMSetGlobalErrorCode(kFailure);
    
    return status;
}



/* SelectPageCmd
 */

ErrorCode PLBPageHelper::SelectPageCmd(UIDRef spread, UIDRef doc, ILayoutControlData* layoutData, bool16 bSet, IGeometry* pageGeometry, bool8 allowUndo)
{
    
    ErrorCode status = kFailure ;
    PMString error(kPlbNilInterface);
    
    do
    {
        // Create a SetPageCmd:
        InterfacePtr<ICommand> setPageCmd(CmdUtils::CreateCommand(kSetPageCmdBoss));
        if(setPageCmd == nil)
        {
            ASSERT_FAIL("PLBPageHelper::SelectPageCmd -> setPageCmd nil");
            break;
        }
        
        // Set command undoability
        if(!allowUndo)
            setPageCmd->SetUndoability(ICommand::kAutoUndo);
        
        
        // Set the SetPageCmd's ItemList:
        setPageCmd->SetItemList(UIDList(spread));
        
        // Get an ILayoutCmdData Interface for the SetPageCmd:
        InterfacePtr<ILayoutCmdData> layoutDataCmd(setPageCmd, IID_ILAYOUTCMDDATA);
        if(layoutData == nil)
        {
            ASSERT_FAIL("PLBPageHelper::SelectPageCmd -> layoutDate nil");
            break;
        }
        
        // Set the ILayoutCmdData Interface's data:
        layoutDataCmd->Set(doc, layoutData);
        
        // Get an IBoolData Interface for the SetPageCmd:
        InterfacePtr<IBoolData> boolData(setPageCmd, IID_IBOOLDATA);
        if(boolData == nil)
        {
            ASSERT_FAIL("PLBPageHelper::SelectPageCmd -> boolDate nil");
            break;
        }
        
        // Set the IBoolData Interface's data:
        boolData->Set(bSet);
        
        // Get an IUIDData Interface for the SetPageCmd:
        InterfacePtr<IUIDData> uidData(setPageCmd, IID_IUIDDATA);
        if(uidData == nil)
        {
            ASSERT_FAIL("PLBPageHelper::SelectPageCmd -> uidDate nil");
            break;
        }
        
        // Set the IUIDData Interface's data:
        uidData->Set(pageGeometry);
        
        // Process the SetPageCmd:
        if (CmdUtils::ProcessCommand(setPageCmd) != kSuccess)
        {
            error = PMString(kSelectPageCmdFailed);
            break;
        }
        
        status = kSuccess ;
        
    } while(false);
    
    if(status != kSuccess)
        ErrorUtils::PMSetGlobalErrorCode(kFailure);
    
    return status;
}

ErrorCode PLBPageHelper::ApplyMasterSpreadCmd(UIDRef doc, PMString masterPrefix, PMString masterName, int32 startPage, int32 endPage, bool8 allowUndo)
{
    
    ErrorCode status = kFailure ;
    PMString error(kPlbNilInterface);
    
    do
    {
        // Find master spread UID
        InterfacePtr<IMasterSpreadList> masterList (doc, UseDefaultIID());
        if(masterList == nil)
        {
            ASSERT_FAIL("PLBPageHelper::ApplyMasterSpreadCmd -> masterList nil");
            break;
        }
        
        UID masterUID = kInvalidUID;
        if(masterName != kNullString)
        {
            masterUID = masterList->FindMasterByName(masterPrefix, masterName);
            if(masterUID == kInvalidUID)
            {
                error = PMString(kErrInvalidMaster);
                break;
            }
        }
        
        //Find pages' UIDs
        InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
        if(pageList == nil)
        {
            ASSERT_FAIL("PLBPageHelper::ApplyMasterSpreadCmd -> pageList nil");
            break;
        }
        
        UIDList pageUIDList(doc.GetDataBase());
        
        if(endPage < startPage || endPage > pageList->GetPageCount() || startPage < 1)
        {
            error = PMString(kErrInvalidPages);
            break;
        }
        
        for(int32 i = startPage ; i <= endPage ; ++i)
            pageUIDList.Append(pageList->GetNthPageUID(i-1));
        
        // Create a applyMasterCmd:
        InterfacePtr<ICommand> applyMasterCmd(CmdUtils::CreateCommand(kApplyMasterSpreadCmdBoss));
        if(applyMasterCmd == nil)
        {
            ASSERT_FAIL("PLBPageHelper::ApplyMasterSpreadCmd -> applyMasterCmd nil");
            break;
        }
        
        // Set command undoability
        if(!allowUndo)
            applyMasterCmd->SetUndoability(ICommand::kAutoUndo);
        
        // Set the applyMasterCmd's ItemList:
        applyMasterCmd->SetItemList(pageUIDList);
        
        InterfacePtr<IApplyMasterCmdData> cmdData (applyMasterCmd, UseDefaultIID());
        if(cmdData == nil)
        {
            ASSERT_FAIL("PLBPageHelper::ApplyMasterSpreadCmd -> cmdData nil");
            break;
        }
        
        cmdData->SetApplyMasterCmdData(UIDRef(doc.GetDataBase(),masterUID), IApplyMasterCmdData::kKeepCurrentPageSize);
        
        // Process the applyMasterCmd:
        if (CmdUtils::ProcessCommand(applyMasterCmd) != kSuccess)
        {
            error = PMString(kApplyMasterCmdFailed);
            break;
        }
        
        status = kSuccess ;
        
    } while(false);
    
    if(status != kSuccess)
        ErrorUtils::PMSetGlobalErrorCode(kFailure);
    
    return status;
}

ErrorCode PLBPageHelper::CreateNewMasterCmd(UIDRef doc, PMString masterPrefix, PMString masterName, PMString basedOnPrefix, PMString basedOn,int32 nbPages, UIDRef& newMaster, bool8 allowUndo)
{
    ErrorCode status = kFailure ;
    PMString error(kPlbNilInterface);
    
    do
    {
        // Find basedOn spread UID
        InterfacePtr<IMasterSpreadList> masterList (doc, UseDefaultIID());
        if(masterList == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewMasterCmd -> masterList nil");
            break;
        }
        
        
        UID basedOnUID = kInvalidUID;
        
        if(basedOn != kNullString)
        {
            basedOnUID= masterList->FindMasterByName(basedOnPrefix, basedOn);
            if(basedOnUID == kInvalidUID)
            {
                error = PMString(kErrInvalidBasedOn);
                break;
            }
        }
        
        // Create a NewMasterCmd:
        InterfacePtr<ICommand> newMasterCmd(CmdUtils::CreateCommand(kNewMasterSpreadCmdBoss));
        if(newMasterCmd == nil)
        {
            ASSERT_FAIL("PLBPageHelper::CreateNewMasterCmd -> newMasterCmd nil");
            break;
        }
        
        // Set command undoability
        if(!allowUndo)
            newMasterCmd->SetUndoability(ICommand::kAutoUndo);
        
        InterfacePtr<IMasterSpreadCmdData> cmdData (newMasterCmd, UseDefaultIID());
        if(cmdData == nil)
            break;
        
        cmdData->Set(doc, nbPages, IMasterSpreadList::kAtTheEnd);
        cmdData->SetMasterSpreadBasename(masterName);
        cmdData->SetMasterSpreadPrefix(masterPrefix);
        cmdData->SetBasedOnMasterUID(basedOnUID);
        
        if (CmdUtils::ProcessCommand(newMasterCmd) != kSuccess)
        {	
            error = PMString(kNewMasterCmdFailed);
            break;
        }
        
        newMaster = newMasterCmd->GetItemList()->GetRef(0);
        
        status = kSuccess ;
        
    } while(false);
    
    if(status != kSuccess)
        ErrorUtils::PMSetGlobalErrorCode(kFailure);
    
    return status;
}


/* MoveToSpreadCmd
 */
// GD 01.03.04 - Fonction pas d￩finie dans .h, ￠ v￩rifier donc.
/*
 ErrorCode PLBPageHelper::MoveToSpreadCmd(UIDRef spread, bool8 allowUndo)
 {
 
	ErrorCode status = kFailure ;
	PMString error(kPlbNilInterface);
 
	do
	{
 // Create a moveCmd
 InterfacePtr<ICommand> moveCmd(CmdUtils::CreateCommand(kMoveToSpreadCmdBoss));
 if(moveCmd == nil)
 {
 ASSERT_FAIL("PLBPageHelper::MoveToSpread -> moveCmd nil");
 break;
 }
 
 // Set command undoability
 if(!allowUndo)
 moveCmd->SetUndoability(ICommand::kAutoUndo);
 
 // Get an IUIDData Interface for the moveCmd:
 InterfacePtr<IUIDData> uidData(moveCmd, IID_IUIDDATA);
 if(uidData == nil)
 {
 ASSERT_FAIL("PLBPageHelper::MoveToSpreadCmd -> uidData nil");
 break;
 }
 
 // Set the IUIDData Interface's data:
 uidData->Set(spread);
 
 // Process the SetPageCmd:
 if (CmdUtils::ProcessCommand(moveCmd) != kSuccess)
 {	
 error = PMString(kMoveToSpreadCmdFailed);
 break;
 }
 
 status = kSuccess ;
 
	} while(false);
	
	if(status != kSuccess)
 ErrorUtils::PMSetGlobalErrorCode(kFailure,kFalse,&error);
 
	return status;
 }
 */
