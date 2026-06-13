/*
//	File:	XPage.cpp
//
//	Date:	21-Jul-2011
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"
#include "ErrorUtils.h"
#include "FileUtils.h"

// Interface includes
#include "IDocument.h"
#include "IPageList.h"
#include "ISpread.h"
#include "IMasterPage.h"
#include "IRefPointUtils.h"
#include "IUnitOfMeasure.h"
#include "IPageList.h"

#include "IOverrideMasterPageItemData.h"
#include "IGeometry.h"
#include "ILayoutUtils.h"
#include "IHierarchy.h"


// Project includes
#include "XPage.h"
#include "IDTime.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "DebugClassUtils.h"
#include "TransformUtils.h"

#if XRAIL
    #include "IXRailPageSlugData.h"
#endif

#if XPAGE
	#include "XPGID.h"
    #include "IResaRedacDataList.h"
    #include "IXPageUtils.h"
    #include "IFormeData.h"
    #include "IPlacedArticleData.h"
    #include "IFrameType.h"
    #include "IItemManager.h"
#endif
/* Constructor
*/
Func_AF::Func_AF(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		formePath = reader->ExtractString(255);
		idResa = reader->ExtractInt(-1, 0, kMaxInt32);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_AF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_AF
*/
void Func_AF::IDCall_AF(ITCLParser * parser)
{	
#if XPAGE
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDocument * doc = parser->getCurDoc();
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		// Check whether current page is valid
		int32 page = parser->getCurPage();
		
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		if(page > pageList->GetPageCount())
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}
		else if (page < 1)
		{
			error = PMString(kErrNumPageTooSmall);
			break;
		}

		// Transform import position into pasteboard coordinates
		PMPoint insertPos (hPos, vPos);
		InterfacePtr<IGeometry> currentPageGeo (db, pageList->GetNthPageUID(page - 1), UseDefaultIID());
		::TransformInnerPointToPasteboard(currentPageGeo, &insertPos);

		// Get target spread
		InterfacePtr<IHierarchy> currentPageHier (currentPageGeo, UseDefaultIID());
		UIDRef targetSpread (db, currentPageHier->GetSpreadUID());

		// Process the import
		UIDList formeItems(db);
		if(Utils<IXPageUtils>()->ImportForme(::GetUIDRef(doc), FileUtils::PMStringToSysFile(formePath), insertPos, targetSpread, IDFile(), error, &formeItems ) != kSuccess)
		{
			break;
		}

		if(idResa != -1 && !formeItems.IsEmpty())
		{
			// Get forme ID
			InterfacePtr<IFormeData> formeData (formeItems.GetRef(0), UseDefaultIID());
			PMString idForme = formeData->GetUniqueName();

			ResaRedac resa;
			resa.id = idResa;

			// Add resa to list
			InterfacePtr<IResaRedacDataList> docResaDataList (doc, UseDefaultIID());
			if(!docResaDataList)
			{
				break;
			}

			InterfacePtr<ICommand> deleteResaCmdBoss (CmdUtils::CreateCommand(kXPGSetResaRedacDataListCmdBoss));
			deleteResaCmdBoss->SetItemList(UIDList(docResaDataList));

			InterfacePtr<IResaRedacDataList> cmdDataDel (deleteResaCmdBoss, UseDefaultIID());
			cmdDataDel->CopyFrom(docResaDataList);
			cmdDataDel->RemoveResaRedac(idForme);

			if(CmdUtils::ProcessCommand(deleteResaCmdBoss) != kSuccess) {
				break;
			}		

			InterfacePtr<ICommand> applyResaCmdBoss (CmdUtils::CreateCommand(kXPGSetResaRedacDataListCmdBoss));
			applyResaCmdBoss->SetItemList(UIDList(docResaDataList));

			InterfacePtr<IResaRedacDataList> cmdData (applyResaCmdBoss, UseDefaultIID());
			cmdData->CopyFrom(docResaDataList);
			cmdData->AddResaRedac(idForme, resa);

			if(CmdUtils::ProcessCommand(applyResaCmdBoss) != kSuccess) {
				break;
			}		
		}

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
#endif
}

/* Constructor
*/
Func_AC::Func_AC(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		idResa = reader->ExtractInt(-1, 0, kMaxInt32);
		pathArt = reader->ExtractString(255);
		matchingPath = reader->ExtractString(255);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_AC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_AC
*/
void Func_AC::IDCall_AC(ITCLParser * parser)
{	
#if XPAGE
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDocument * doc = parser->getCurDoc();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		IDataBase * db = parser->getCurDataBase();

		// Check whether current page is valid
		int32 page = parser->getCurPage();
		
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		if(page > pageList->GetPageCount())
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}
		else if (page < 1)
		{
			error = PMString(kErrNumPageTooSmall);
			break;
		}

		// Get forme on master
		InterfacePtr<IResaRedacDataList> docResaDataList (doc, UseDefaultIID());
		if (docResaDataList == nil)
		{
			// Résa Rédactionnelles feature was removed.
			error = PMString(kErrInvalidResa);
			break;
		}
		PMString idForme = docResaDataList->GetIDForme(idResa);
		if(idForme == kNullString)
		{
			error = PMString(kErrInvalidResa);
			break;
		}
		//on recupere les blocs de la forme
		InterfacePtr<IHierarchy> currentPageHier (db, pageList->GetNthPageUID(page - 1), UseDefaultIID());
		InterfacePtr<ISpread> currentSpread (db, currentPageHier->GetSpreadUID(), UseDefaultIID());

		UIDList formeItems(db);
		if(Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(idForme, currentSpread, formeItems) != kSuccess)
		{
			error = PMString(kErrInvalidResa);
			break;
		}

		if(formeItems.IsEmpty())
		{
			error = PMString(kErrInvalidResa);
			break;
		}
		
		// Get curent page index from text frame
		UIDList targetStories(db);
		K2Vector<UIDRef> targetPictures;
		for(int32 i = 0; i < formeItems.size(); ++i){
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
		
		// Try to use matching file associated with the story if any
		IDFile matchingFile = FileUtils::PMStringToSysFile(matchingPath);	 
		
		// Import de l'article
		PMString errorImport = kNullString;
		PMString idStory;
		idStory.AppendNumber(idResa);
		IDFile xmlFileToImport = FileUtils::PMStringToSysFile(pathArt);
		status = Utils<IXPageUtils>()->ImportArticle(targetStories, xmlFileToImport, matchingFile, errorImport, xmlFileToImport, idStory);
		if (status != kSuccess) {
			CAlert::InformationAlert("import article ￩chou￩");
			break;
		}
		error = kNullString;
		status = Utils<IXPageUtils>()->ImportImages(targetPictures, xmlFileToImport, errorImport);
		if (status != kSuccess) {
			CAlert::InformationAlert("import images ￩chou￩");
			break;
		}
		
				// Save new data of placed story
 		InterfacePtr<ICommand> placedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
		InterfacePtr<IPlacedArticleData> placedArticleData(placedArticleDataCmd, IID_IPLACEDARTICLEDATA);
		placedArticleData->SetUniqueId(idStory);

		IDFile xmlFolder = xmlFileToImport;
		FileUtils::GetParentDirectory(xmlFolder, xmlFolder);
		CAlert::InformationAlert("AC: " + FileUtils::SysFileToPMString(xmlFolder));
		placedArticleData->SetStoryFolder(FileUtils::SysFileToPMString(xmlFolder));

		//placedArticleData->SetStoryFolder(FileUtils::SysFileToPMString(xmlFileToImport));
		placedArticleDataCmd->SetItemList(formeItems);
		if(CmdUtils::ProcessCommand(placedArticleDataCmd)!= kSuccess)
			break; 
		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
		
#endif	
}

/* Constructor
*/
Func_AA::Func_AA(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		idResa = reader->ExtractInt(0, 0, kMaxInt32);
		inCopyAssignmentPath = reader->ExtractString(255);
		topic = reader->ExtractString(255);
		recipientName = reader->ExtractString(255);		
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_AA(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_AA
*/
void Func_AA::IDCall_AA(ITCLParser * parser)
{	
#if XPAGE
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDocument * doc = parser->getCurDoc();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		// Check whether current page is valid
		int32 page = parser->getCurPage();
		
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		if(page > pageList->GetPageCount())
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}
		else if (page < 1)
		{
			error = PMString(kErrNumPageTooSmall);
			break;
		}

		// Get forme on master
		InterfacePtr<IResaRedacDataList> docResaDataList (doc, UseDefaultIID());
		if (docResaDataList == nil)
		{
			// Résa Rédactionnelles feature was removed — no impl is registered
			// on the document. Fail this TCL command cleanly.
			error = PMString(kErrInvalidResa);
			break;
		}
		PMString idForme = docResaDataList->GetIDForme(idResa);
		if(idForme == kNullString)
		{
			error = PMString(kErrInvalidResa);
			break;
		}

		InterfacePtr<IMasterPage> masterPage (db, pageList->GetNthPageUID(page - 1), UseDefaultIID());
		InterfacePtr<ISpread> masterSpread (db, masterPage->GetMasterSpreadUID(), UseDefaultIID());
		
		UIDList formeItems(db);
		if(Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(idForme, masterSpread, formeItems) != kSuccess)
		{
			error = PMString(kErrInvalidResa);
			break;
		}

		if(!formeItems.IsEmpty()) // Found forme on the master,so we check consistency and override the forme items on the page
		{
			// Check whether the whole forme is laying on one page
			PMPoint formeLeftTop = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kTopLeftReferencePoint, formeItems/*, nil, &Geometry::OuterStrokeBounds()*/);
			PMPoint formeRightBottom = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kBottomRightReferencePoint, formeItems/*, nil, &Geometry::OuterStrokeBounds()*/);
			
			int32 formePageIndex1 = 0, formePageIndex2 = 0;
			InterfacePtr<IGeometry> pageGeo1 (masterSpread->QueryNearestPage(formeLeftTop, &formePageIndex1));
			InterfacePtr<IGeometry> pageGeo2 (masterSpread->QueryNearestPage(formeRightBottom, &formePageIndex2));
			
			if(formePageIndex1 != formePageIndex2)
			{
				error = PMString(kErrInvalidResa);
				break;
			}

			// Check whether forme items actually are on the master of the current page
			if(masterPage->GetMasterSpreadPageIndex() != formePageIndex1)
			{
				error = PMString(kErrInvalidResa);
				break;
			}
			
			// Handle shifting if overridden from a single page master in a facing pages spread (will have a shift of +/- (1/2 page width))
			UID currentPageUID = ::GetUID(masterPage);
			PageType currentPageType = pageList->GetPageType(currentPageUID);

			PMMatrix shift;
			if(currentPageType != kUnisexPage && masterSpread->GetNumPages() == 1)
			{
				PMReal pageWidth = pageGeo1->GetStrokeBoundingBox().Width();
				if(currentPageType == kLeftPage)
					shift.Translate(-pageWidth/2, 0);
				else
					shift.Translate(pageWidth/2, 0);
			}

			// Override forme items

			UIDList pageUIDList(db);
			PMMatrixCollection shiftList;
			for(int32 i = 0 ; i < formeItems.Length() ; ++i)
			{
				pageUIDList.Append(currentPageUID);
				shiftList.push_back(shift);
			}

			InterfacePtr<ICommand> overrideItemCmd (CmdUtils::CreateCommand(kOverrideMasterPageItemCmdBoss));

			InterfacePtr<IOverrideMasterPageItemData> cmdData (overrideItemCmd, IID_IOVERRIDEMASTERPAGEITEMDATA );
			cmdData->SetOverrideMasterPageItemData(formeItems, pageUIDList, shiftList);

			if(CmdUtils::ProcessCommand(overrideItemCmd) != kSuccess)
			{
				error = PMString(kErrInvalidResa);
				break;		
			}
			
			// Update forme items
			formeItems.Clear();
			for(int32 i = 0 ; i < cmdData->GetOverrideCount() ; ++i)
			{
				UIDRef overridenItem, dummy1, dummy2;
				PMMatrix dummyShift;
				cmdData->GetNthOverride(i, &dummy1, &dummy2, &overridenItem, &dummyShift);

				formeItems.Append(overridenItem);
			}	
		}
		else
		{
			// There is no such forme on master, try finding it directly on the current page
			InterfacePtr<IHierarchy> currentPageHier (db, pageList->GetNthPageUID(page - 1), UseDefaultIID());
			InterfacePtr<ISpread> currentSpread (db, currentPageHier->GetSpreadUID(), UseDefaultIID());

			if(Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(idForme, currentSpread, formeItems) != kSuccess)
			{
				error = PMString(kErrInvalidResa);
				break;
			}

			if(formeItems.IsEmpty())
			{
				error = PMString(kErrInvalidResa);
				break;
			}

			// Check whether forme is actually on the current page
			PMPoint formeLeftTop = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kTopLeftReferencePoint , formeItems/*, nil, &Geometry::OuterStrokeBounds()*/);
			
			int32 formePageIndex = 0;
			InterfacePtr<IGeometry> pageGeo (currentSpread->QueryNearestPage(formeLeftTop, &formePageIndex));
			
			if(currentSpread->GetPageIndex(::GetUID(currentPageHier)) != formePageIndex)
			{
				error = PMString(kErrInvalidResa);
				break;
			}
		}

		// Create InCopy article
		status = Utils<IXPageUtils>()->DoNewAssignment(formeItems, FileUtils::PMStringToSysFile(inCopyAssignmentPath), topic, recipientName, kTrue, kNullString, kFalse);

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
#endif
}

/* Constructor
 */
Func_AZ::Func_AZ(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));
    
	try
	{
		reader->MustEgal();
		idResa = reader->ExtractInt(-1, 0, kMaxInt32);
		pathArt = reader->ExtractString(255);
		matchingPath = reader->ExtractString(255);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");
        
		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}
    
	if(reader->IsEndOfCommand())
		IDCall_AZ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_AR
 */
void Func_AZ::IDCall_AZ(ITCLParser * parser)
{
#if XPAGE
	ErrorCode status = kFailure;
	PMString error("");
    
	do
	{
		IDocument * doc = parser->getCurDoc();
        
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);
			break;
		}
        
		IDataBase * db = parser->getCurDataBase();
        
		// Check whether current page is valid
		int32 page = parser->getCurPage();
		
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		if(page > pageList->GetPageCount())
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}
		else if (page < 1)
		{
			error = PMString(kErrNumPageTooSmall);
			break;
		}

		// Get forme on master
		InterfacePtr<IResaRedacDataList> docResaDataList (doc, UseDefaultIID());
		if (docResaDataList == nil)
		{
			// Résa Rédactionnelles feature was removed.
			error = PMString(kErrInvalidResa);
			break;
		}
		PMString idForme = docResaDataList->GetIDForme(idResa);
		if(idForme == kNullString)
		{
			error = PMString(kErrInvalidResa);
			break;
		}
        
		InterfacePtr<IMasterPage> masterPage (db, pageList->GetNthPageUID(page - 1), UseDefaultIID());
		InterfacePtr<ISpread> masterSpread (db, masterPage->GetMasterSpreadUID(), UseDefaultIID());
		
		UIDList formeItems(db);
		if(Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(idForme, masterSpread, formeItems) != kSuccess)
		{
			error = PMString(kErrInvalidResa);
			break;
		}
        
		if(!formeItems.IsEmpty()) // Found forme on the master,so we check consistency and override the forme items on the page
		{
			// Check whether the whole forme is laying on one page
			PMPoint formeLeftTop = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kTopLeftReferencePoint, formeItems/*, nil, &Geometry::OuterStrokeBounds()*/);
			PMPoint formeRightBottom = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kBottomRightReferencePoint, formeItems/*, nil, &Geometry::OuterStrokeBounds()*/);
			
			int32 formePageIndex1 = 0, formePageIndex2 = 0;
			InterfacePtr<IGeometry> pageGeo1 (masterSpread->QueryNearestPage(formeLeftTop, &formePageIndex1));
			InterfacePtr<IGeometry> pageGeo2 (masterSpread->QueryNearestPage(formeRightBottom, &formePageIndex2));
			
			if(formePageIndex1 != formePageIndex2)
			{
				error = PMString(kErrInvalidResa);
				break;
			}
            
			// Check whether forme items actually are on the master of the current page
			if(masterPage->GetMasterSpreadPageIndex() != formePageIndex1)
			{
				error = PMString(kErrInvalidResa);
				break;
			}
			
			// Handle shifting if overridden from a single page master in a facing pages spread (will have a shift of +/- (1/2 page width))
			UID currentPageUID = ::GetUID(masterPage);
			PageType currentPageType = pageList->GetPageType(currentPageUID);
            
			PMMatrix shift;
			if(currentPageType != kUnisexPage && masterSpread->GetNumPages() == 1)
			{
				PMReal pageWidth = pageGeo1->GetStrokeBoundingBox().Width();
				if(currentPageType == kLeftPage)
					shift.Translate(-pageWidth/2, 0);
				else
					shift.Translate(pageWidth/2, 0);
			}
            
			// Override forme items
            
			UIDList pageUIDList(db);
			PMMatrixCollection shiftList;
			for(int32 i = 0 ; i < formeItems.Length() ; ++i)
			{
				pageUIDList.Append(currentPageUID);
				shiftList.push_back(shift);
			}
            
			InterfacePtr<ICommand> overrideItemCmd (CmdUtils::CreateCommand(kOverrideMasterPageItemCmdBoss));
            
			InterfacePtr<IOverrideMasterPageItemData> cmdData (overrideItemCmd, IID_IOVERRIDEMASTERPAGEITEMDATA );
			cmdData->SetOverrideMasterPageItemData(formeItems, pageUIDList, shiftList);
            
			if(CmdUtils::ProcessCommand(overrideItemCmd) != kSuccess)
			{
				error = PMString(kErrInvalidResa);
				break;
			}
			
			// Update forme items
			formeItems.Clear();
			for(int32 i = 0 ; i < cmdData->GetOverrideCount() ; ++i)
			{
				UIDRef overridenItem, dummy1, dummy2;
				PMMatrix dummyShift;
				cmdData->GetNthOverride(i, &dummy1, &dummy2, &overridenItem, &dummyShift);
                
				formeItems.Append(overridenItem);
			}
		}
		else
		{
			// There is no such forme on master, try finding it directly on the current page
			InterfacePtr<IHierarchy> currentPageHier (db, pageList->GetNthPageUID(page - 1), UseDefaultIID());
			InterfacePtr<ISpread> currentSpread (db, currentPageHier->GetSpreadUID(), UseDefaultIID());
            
			if(Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(idForme, currentSpread, formeItems) != kSuccess)
			{
				error = PMString(kErrInvalidResa);
				break;
			}
            
			if(formeItems.IsEmpty())
			{
				error = PMString(kErrInvalidResa);
				break;
			}
            
			// Check whether forme is actually on the current page
			PMPoint formeLeftTop = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kTopLeftReferencePoint , formeItems/*, nil, &Geometry::OuterStrokeBounds()*/);
			
			int32 formePageIndex = 0;
			InterfacePtr<IGeometry> pageGeo (currentSpread->QueryNearestPage(formeLeftTop, &formePageIndex));
			
			if(currentSpread->GetPageIndex(::GetUID(currentPageHier)) != formePageIndex)
			{
				error = PMString(kErrInvalidResa);
				break;
			}
		}
        
        
        
        // Get curent page index from text frame
		UIDList targetStories(db);
		K2Vector<UIDRef> targetPictures;
		for(int32 i = 0; i < formeItems.size(); ++i){
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
		
		// Try to use matching file associated with the story if any
		IDFile matchingFile = FileUtils::PMStringToSysFile(matchingPath);
		
		// Import de l'article
		PMString errorImport = kNullString;
		PMString idStory;
		idStory.AppendNumber(idResa);
		IDFile xmlFileToImport = FileUtils::PMStringToSysFile(pathArt);
		status = Utils<IXPageUtils>()->ImportArticle(targetStories, xmlFileToImport, matchingFile, errorImport, xmlFileToImport, idStory);
		if (status != kSuccess) {
			break;
		}
        error = kNullString;
		status = Utils<IXPageUtils>()->ImportImages(targetPictures, xmlFileToImport, errorImport);
		if (status != kSuccess) {
			break;
		}
        // Save new data of placed story
 		InterfacePtr<ICommand> placedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
		InterfacePtr<IPlacedArticleData> placedArticleData(placedArticleDataCmd, IID_IPLACEDARTICLEDATA);
		placedArticleData->SetUniqueId(idStory);
		IDFile xmlFolder = xmlFileToImport;
		FileUtils::GetParentDirectory(xmlFolder, xmlFolder);
		CAlert::InformationAlert("AZ: " + FileUtils::SysFileToPMString(xmlFolder));
		
		//placedArticleData->SetStoryFolder(FileUtils::SysFileToPMString(xmlFileToImport));
		placedArticleDataCmd->SetItemList(formeItems);
		if(CmdUtils::ProcessCommand(placedArticleDataCmd)!= kSuccess)
			break;
		status = kSuccess;
	} while(kFalse);
    
	if(status != kSuccess)
		throw TCLError(error);
#endif
}



/* Constructor
*/
Func_AM::Func_AM(ITCLParser *  parser)
{
#if XPAGE
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		idStory = reader->ExtractString(255);
		typeMarbre = reader->ExtractInt(0, 0, 1);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_AM(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
#endif
}


/* IDCall_AM
*/
void Func_AM::IDCall_AM(ITCLParser * parser)
{	
#if XPAGE
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDocument * doc = parser->getCurDoc();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

/*		// Move article to marbre
		status = Utils<IXPageUtils>()->DoStoryToMarbre(::GetUIDRef(doc), idStory, typeMarbre);
		if(status != kSuccess)
		{
			// Log it but don't break processing
			PMString msg(kPrsAMCommandFailed);
			msg.Translate();
			parser->WriteLogFile(msg);

			status = kSuccess;
		}
*/
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
#endif
}


/* Constructor
*/
Func_AB::Func_AB(ITCLParser *  parser)
{
#if XPAGE
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		assemblyPath = reader->ExtractString(255);

		while (!reader->IsEndOfCommand())
		{
			PMString idForme = reader->ExtractString(255);
			int32 idResa = reader->ExtractInt(-1, 0, kMaxInt32);
			resas.push_back(KeyValuePair<PMString, int32>(idForme, idResa));
		}
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_AB(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
#endif
}

/* IDCall_AB
*/
void Func_AB::IDCall_AB(ITCLParser * parser)
{	
#if XPAGE
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDocument * doc = parser->getCurDoc();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		// Check whether current page is valid
		int32 page = parser->getCurPage();
		
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		if(page > pageList->GetPageCount())
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}
		else if (page < 1)
		{
			error = PMString(kErrNumPageTooSmall);
			break;
		}

		// Transform import position into pasteboard coordinates
		PMPoint insertPos (hPos, vPos);
		InterfacePtr<IGeometry> currentPageGeo (db, pageList->GetNthPageUID(page - 1), UseDefaultIID());
		::TransformInnerPointToPasteboard(currentPageGeo, &insertPos);

		// Get target spread
		InterfacePtr<IHierarchy> currentPageHier (currentPageGeo, UseDefaultIID());
		UIDRef targetSpread (db, currentPageHier->GetSpreadUID());

		// Process the import
		UIDList assemblageItems(db);
		if(Utils<IXPageUtils>()->ImportForme(::GetUIDRef(doc), FileUtils::PMStringToSysFile(assemblyPath), insertPos, targetSpread, IDFile(), error, &assemblageItems ) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
			
		status = kSuccess;

		if(!resas.empty())
		{
			K2Vector<PMString> formeIDConvertedToResas;
			for(int32 i = 0 ; i < assemblageItems.Length() && status == kSuccess ; ++i)
			{
				InterfacePtr<IFormeData> formeData (assemblageItems.GetRef(i), UseDefaultIID());
				PMString newIDForme = formeData->GetUniqueName(); // Import process appended a timestamp

				PMString idFormeWithoutTimeStamp = newIDForme;
				int32 timeStampPos = idFormeWithoutTimeStamp.LastIndexOfCharacter(PlatformChar('_'));
				if(timeStampPos != -1)
					idFormeWithoutTimeStamp.Remove(timeStampPos, idFormeWithoutTimeStamp.NumUTF16TextChars()-timeStampPos); 				
			
				int32 pos = ::FindLocation(resas, idFormeWithoutTimeStamp);
				if(pos == -1)
				{
					error = PMString(kErrInvalidFormeID);
					status = kFailure;
					break;
				}

				if(K2find(formeIDConvertedToResas.begin(), formeIDConvertedToResas.end(), idFormeWithoutTimeStamp) == formeIDConvertedToResas.end())
				{
					// Store resa ID in document
					ResaRedac resa;
					resa.id = resas[pos].Value();

					InterfacePtr<IResaRedacDataList> docResaDataList (doc, UseDefaultIID());
					if(!docResaDataList)
						break;

					InterfacePtr<ICommand> applyResaCmdBoss (CmdUtils::CreateCommand(kXPGSetResaRedacDataListCmdBoss));
					applyResaCmdBoss->SetItemList(UIDList(docResaDataList));

					InterfacePtr<IResaRedacDataList> cmdData (applyResaCmdBoss, UseDefaultIID());
					cmdData->CopyFrom(docResaDataList);
					cmdData->AddResaRedac(newIDForme, resa); // Associate new forme ID for resa to work well

					if(CmdUtils::ProcessCommand(applyResaCmdBoss) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						status = kFailure;
						break;
					}

					formeIDConvertedToResas.push_back(idFormeWithoutTimeStamp);
				}
			}
		}

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
#endif
}

/* Constructor
*/
Func_AD::Func_AD(ITCLParser *  parser)
{
#if XPAGE
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		idStory = reader->ExtractInt(-1, 0, kMaxInt32);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_AD(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
#endif	
}

/* IDCall_AD
*/
void Func_AD::IDCall_AD(ITCLParser * parser)
{	
#if XPAGE
	ErrorCode status = kSuccess;
	PMString error("");

	do
	{
		IDocument * doc = parser->getCurDoc();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		// Check whether current page is valid
		int32 page = parser->getCurPage();
		
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		if(page > pageList->GetPageCount())
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}
		else if (page < 1)
		{
			error = PMString(kErrNumPageTooSmall);
			break;
		}

		//on recupere les blocs
		InterfacePtr<IResaRedacDataList> docResaDataList (doc, UseDefaultIID());
		if (docResaDataList == nil)
		{
			// Résa Rédactionnelles feature was removed.
			break;
		}
		PMString idForme = docResaDataList->GetIDForme(idStory);
		if(idForme == kNullString)
		{
			//error = PMString(kErrInvalidResa);
			break;
		}
		//on recupere les blocs de la forme
		InterfacePtr<IHierarchy> currentPageHier (db, pageList->GetNthPageUID(page - 1), UseDefaultIID());
		InterfacePtr<ISpread> currentSpread (db, currentPageHier->GetSpreadUID(), UseDefaultIID());

		UIDList formeItems(db);
		if(Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(idForme, currentSpread, formeItems) != kSuccess)
		{
			//error = PMString(kErrInvalidResa);
			break;
		}

		// Supprimer la resa
		InterfacePtr<ICommand> setResaRedacListCmdBoss (CmdUtils::CreateCommand(kXPGSetResaRedacDataListCmdBoss));
		setResaRedacListCmdBoss->SetItemList(UIDList(docResaDataList));

		InterfacePtr<IResaRedacDataList> cmdData (setResaRedacListCmdBoss, UseDefaultIID());
		cmdData->CopyFrom(docResaDataList);
		cmdData->RemoveResaRedac(idForme);

		if(CmdUtils::ProcessCommand(setResaRedacListCmdBoss) != kSuccess) {
			continue;		
		}
		
		if(formeItems.IsEmpty())
		{
			//error = PMString(kErrInvalidResa);
			break;
		}

		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			ASSERT_FAIL("Func_BN::IDCall_BN -> itemManager nil");
			error = PMString(kErrNilInterface);
			break;
		}
		if((status = itemManager->DeletePageItemsCmd(formeItems)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
#endif			
}

/* Constructor
 */
Func_AE::Func_AE(ITCLParser *  parser)
{
#if XPAGE
    InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
    if(reader == nil)
    {
        ASSERT_FAIL("Func_AE::Func_AE -> reader nil");
        throw TCLError(PMString(kErrNilInterface));
    }
    
    try
    {
        reader->FiltreBlanc();
    }
    // Catch TCL error while parameter reading to add parameter number to the error message
    catch(TCLError& error)
    {
        PMString parameter(kParameterKey);
        parameter.Translate(); parameter.Append(" ");
        parameter.AppendNumber(reader->GetNumParam());
        parameter.Append(" : ");
        
        PMString message(error.getMessage());
        message.Translate();
        message.Insert(parameter);
        error.setMessage(message);
        throw;
    }
    
    if(reader->IsEndOfCommand())
        IDCall_AE(parser);
    else
        throw TCLError(PMString(kErrNoEndOfCommand));
#endif
}

/* IDCall_AE
 */
void Func_AE::IDCall_AE(ITCLParser * parser)
{
#if XPAGE
    ErrorCode status = kSuccess;
    PMString error("");
    
    do
    {
        IDocument * doc = parser->getCurDoc();
        
        // Check if there is a document open
        if(doc == nil)
        {
            error = PMString(kErrNeedDoc);
            break;
        }
        
        IDataBase * db = parser->getCurDataBase();
        
        K2Vector<KeyValuePair <PMString, K2Pair <PMString, UIDList> > > placedArticleList = Utils<IXPageUtils>()->GetPlacedStoriesList(doc);
        for(int32 k= 0; k < placedArticleList.size(); k++) {
            UIDList  articleItems = placedArticleList[k].Value().second;
            PMString TriasStoryID = placedArticleList[k].Key();
            PMString ConsoleMessage;
            
            InterfacePtr<ICommand> placedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
            InterfacePtr<IPlacedArticleData> placedArticleData(placedArticleDataCmd, IID_IPLACEDARTICLEDATA);
            placedArticleData->SetUniqueId(kNullString);
            placedArticleData->SetStoryFolder(kNullString);
            placedArticleData->SetStoryTopic(kNullString);
            placedArticleDataCmd->SetItemList(articleItems);
            if(CmdUtils::ProcessCommand(placedArticleDataCmd)!= kSuccess) {
                ConsoleMessage = "Func_AE - Erreur suppression data article "+TriasStoryID;
                CAlert::InformationAlert(ConsoleMessage);
            } else {
                ConsoleMessage = "Func_AE - OK suppression data article "+TriasStoryID;
                CAlert::InformationAlert(ConsoleMessage);
            }
        }
        
    } while(kFalse);
    
    if(status != kSuccess)
        throw TCLError(error);
#endif			
}

