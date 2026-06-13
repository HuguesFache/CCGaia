/*
//	File:	Divers2.cpp
//
//	Date:	20-Nov-2006
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"
#include "FileUtils.h"
#include "OpenPlaceID.h"
#include "Utils.h"
#include "ErrorUtils.h"
#include "TransformUtils.h"
#include "StringUtils.h"

// Project includes
#include "GlobalDefs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "IItemManager.h"
#include "IItemTransform.h"
#include "IPageHelper.h"
#include "TCLError.h"
#include "Divers.h"
#include "StreamUtil.h"
#include "PermRefsUtils.h"
#include "Blocs.h"
#include "NameInfo.h"
#include "ILayoutUtils.h"

//Interfaces Includes
#include "ISnippetExport.h"
#include "ISnippetImport.h"
#include "IDOMElement.h"
#include "IPageList.h"
#include "IHierarchy.h"
#include "IDocument.h"
#include "IPMStream.h"
#include "IUIDListData.h"
#include "IDocumentLayer.h"
#include "ILayerUtils.h"
#include "IGeometry.h"
#include "ISubject.h"
#include "IObserver.h"
#include "IMultiColumnTextFrame.h"
#include "ITextModel.h"
#include "IPageItemTypeUtils.h"
#include "IDataLinkReference.h"
#include "IDataLink.h"
#include "IUpdateLink.h"
#include "IDataLinkHelper.h"
#include "IRestoreLinkCmdData.h"
#include "ILinksManager.h"
#include "ITransformFacade.h"
#include "IMargins.h"
#include "IColumns.h"
#include "ISpreadLayer.h"
#include "ISpread.h"
#include "IMasterPage.h"
#include "IUIDData.h"
#include "IPathGeometry.h"
#include "IPathUtils.h"
#include "IGeometryFacade.h"
#include "IUnitOfMeasure.h"
#include "ISectionList.h"

#include "CAlert.h"
#include "DebugClassUtils.h"

/* Constructor
*/
Func_EE::Func_EE(ITCLParser *  parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		filename = reader->ExtractString(255);

		do
		{
			itemsToExport.push_back(reader->ExtractTCLRef(kCurrentTCLRef));		
		}
		while(!reader->IsEndOfCommand());
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
		IDCall_EE(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_EE
*/
void Func_EE::IDCall_EE(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		IDFile snippetFile = FileUtils::PMStringToSysFile(filename);
		IDFile tmp;

		// Check whether file path is correct or not
		if(!FileUtils::GetParentDirectory(snippetFile, tmp))
		{
			error = PMString(kErrBadPath);
			break;
		}

		// Get items' UIDs and check whether they all belong to same page/spread
		bool16 errorOccured = kFalse;
		UIDList theSnippet (Func_BG::GetAndCheckItemsToGroup(parser, kTrue, db, itemsToExport, errorOccured, error));
		if(errorOccured)
			break;
		
		InterfacePtr<IPMStream> snippetStream (StreamUtil::CreateFileStreamWrite(snippetFile, kOpenOut|kOpenTrunc, kTextType, kAnyCreator));

		status = Utils<ISnippetExport>()->ExportPageitems (snippetStream, theSnippet);

		snippetStream->Close();

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_IE::Func_IE(ITCLParser *  parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		filename = reader->ExtractString(255);
		hPos = reader->ExtractReel('P','+',0,0);
		vPos = reader->ExtractReel('P','+',0,0);
		relative = reader->ExtractBooleen(kFalse);
		if(relative)
			ref = reader->ExtractTCLRef(kCurrentTCLRef);
		else
			page = reader->ExtractInt(CURRENT_PAGE,CURRENT_PAGE,MAX_PAGE);
		alignBloc = reader->ExtractInt(TOP_ALIGNEMENT,TOP_ALIGNEMENT,TEXT_ALIGNEMENT);
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
		IDCall_IE(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_IE
*/
void Func_IE::IDCall_IE(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	ICommandSequence * seq = nil;
	UIDList * newPageItems = nil;

	do
	{
		IDocument * doc = parser->getCurDoc();
		IDataBase * db = parser->getCurDataBase();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		// Get helper interfaces
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		// Define the snippet's left top position
		PMPoint leftTop(hPos,vPos);	

		PMPointList points;
		points.push_back(leftTop);
		
		UID ownerSpreadLayer = kInvalidUID;
		
		InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(doc));

		// Get spread to import into and paste coordinates
		if(relative)
		{
				if(ref == kCurrentTCLRef)
					ref = parser->getCurItem();

				UID refUID = parser->getItem(ref);
				if(refUID == kInvalidUID)
				{
					error = PMString(kErrInvalidItem);
					break;
				}

				ownerSpreadLayer = itemManager->GetPBCoordRel(UIDRef(db,refUID),points,alignBloc, activeLayer);
				if(ownerSpreadLayer == kInvalidUID)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
		}
		else
		{
			if(page == CURRENT_PAGE)
			{
				if(parser->getCurPage() != -1)
					page = parser->getCurPage() ;
				else
					page = 1;
			}

			ownerSpreadLayer = itemManager->GetPBCoordAbs(doc,page,points,activeLayer);
			
			if(ownerSpreadLayer == kInvalidUID)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}		
		
		InterfacePtr<IHierarchy> theSpreadLayer (db, ownerSpreadLayer, UseDefaultIID());							
		InterfacePtr<IDOMElement> ownerSpread (db, theSpreadLayer->GetSpreadUID(), UseDefaultIID());
		
		// Open file, if any
		IDFile snippetFile = FileUtils::PMStringToSysFile(filename);		

		// Check whether file path is correct or not
		if(!FileUtils::DoesFileExist(snippetFile))
		{
			error = PMString(kErrBadPath);
			break;
		}

		// Import snippet
		InterfacePtr<ISubject> docSubject (doc, UseDefaultIID());
		InterfacePtr<IObserver> importObserver (doc, IID_PASTEIMPORTITEMOBSERVER);

		docSubject->AttachObserver(importObserver, IID_IHIERARCHY_DOCUMENT, IID_PASTEIMPORTITEMOBSERVER); 

		InterfacePtr<IPMStream> snippetStream (StreamUtil::CreateFileStreamRead(snippetFile));		
		if(snippetStream == nil)
		{
			error = PMString(kErrBadPath);
			break;
		}

		ErrorCode importResult = Utils<ISnippetImport>()->ImportFromStream(snippetStream, ownerSpread);

		docSubject->DetachObserver(importObserver, IID_IHIERARCHY_DOCUMENT, IID_PASTEIMPORTITEMOBSERVER); 

		if(importResult != kSuccess)
		{
			error = PMString(kErrSnippetImportFailed);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, filename);

			break;
		}

		// Get back uids of newly created page items from import, then clean up list	
		InterfacePtr<IUIDListData> importedItems (doc, IID_IPRSLASTIMPORTEDSNIPPET);
		newPageItems = new UIDList(*(importedItems->GetRef()));
		importedItems->Set(nil);

		UIDList snippetItems(db);
		PMPoint leftTopMost(kMaxInt32, kMaxInt32);

		for(int32 i = 0 ; i < newPageItems->Length() ; ++i)
		{
			InterfacePtr<IGeometry> itemGeo (newPageItems->GetRef(i), UseDefaultIID());
			PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();

			::TransformInnerPointToPasteboard(itemGeo, &leftTop);

			if(leftTop.X() < leftTopMost.X())
				leftTopMost.X() = leftTop.X();

			if(leftTop.Y() < leftTopMost.Y())
				leftTopMost.Y() = leftTop.Y();

			snippetItems.Append(newPageItems->At(i));
		}

		// Compute delta between required position and current position after importation
		PMPoint delta = points[0] - leftTopMost ;

		status = Utils<Facade::ITransformFacade>()->TransformItems( snippetItems, Transform::PasteboardCoordinates(), kZeroPoint, 
					 Transform::TranslateBy(delta.X(), delta.Y()));

		if(status != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}	

		status = kSuccess;
		
	} while(kFalse);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);		
			// Register items in the document and the parser
			if(newPageItems)
			{
				for(int32 i = 0 ; i < newPageItems->Length() ; ++i)
				{
					InterfacePtr<ITCLRefData> tclRefData (newPageItems->GetRef(i), IID_ITCLREFDATA);
					if(tclRefData->Get() != kDefaultTCLRefData)
						parser->setItem(tclRefData->Get(), newPageItems->At(i));

					// If it'a text frame, look for some permrefs
					if(Utils<IPageItemTypeUtils>()->IsTextFrame(tclRefData))
					{
						InterfacePtr<IHierarchy> itemHier (tclRefData, UseDefaultIID());
						InterfacePtr<IMultiColumnTextFrame> txtFrame (::GetDataBase(tclRefData), itemHier->GetChildUID(0), UseDefaultIID());
						InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());

						PermRefVector permRefsList;
						if(PermRefsUtils::UpdatePermRefsList(permRefsList, txtModel, 0, txtModel->TotalLength()-1))
						{
							parser->AppendPermRefsList(permRefsList);
						}
					}
				}
				delete newPageItems;
			}
		}
		else
		{			
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			CmdUtils::EndCommandSequence(seq);

			if(newPageItems)
				delete newPageItems;

			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_RK::Func_RK(ITCLParser *  parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		imageDirPath = reader->ExtractString(255);
		relinkType =  reader->ExtractInt(0, 0, 2);
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
		IDCall_RK(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_RK
*/
void Func_RK::IDCall_RK(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		// Check if there is a document open
		IDocument * doc = parser->getCurDoc();
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}		
		
		IDataBase * db = parser->getCurDataBase();

		// Check whether image folder exist
		IDFile imageFolder = FileUtils::PMStringToSysFile(imageDirPath);
		if(!FileUtils::DoesFileExist(imageFolder))
		{
			// error = PMString(kErrBadPath); NOT AN ERROR ANYMORE => there may be no image folder
			status = kSuccess;
			break;
		}
		
		// Check whether it's really a directory
		if(!FileUtils::IsDirectory(imageFolder))
		{
			error = PMString(kErrInvalidFolder);
			break;
		}

		// Load link list (because it is modified as links are updated)
		InterfacePtr<ILinksManager> linkMgr (doc, UseDefaultIID());
		int32 nbLinks = linkMgr->GetLinkCount();
		
		K2Vector<UIDRef> linkList;
		for(int32 i = 0 ; i < nbLinks ; ++i)
			linkList.push_back(linkMgr->GetNthLinkUID(i));

		InterfacePtr<IDataLinkHelper> dataLinkHelper (::CreateObject2<IDataLinkHelper>(kDataLinkHelperBoss));

		// Iterate through each link
		switch(relinkType)
		{
			case 0 : // all images
						for(int32 i = 0 ; i < nbLinks ; ++i)
						{
							InterfacePtr<IDataLink>  link (linkList[i], UseDefaultIID());
							if(link == nil)
								continue;

							// Porting CS5 : InterfacePtr<IHierarchy> imageItem (db, dataLinkHelper->GetAssociatedPageItem(link), UseDefaultIID());
							//if(imageItem == nil)
							//	continue;
						
							UpdateLink(db, link, imageFolder, parser);
						}
						break;

			case 1 : // all images in pages
						for(int32 i = 0 ; i < nbLinks ; ++i)
						{
							InterfacePtr<IDataLink>  link (linkList[i], UseDefaultIID());
							if(link == nil)
								continue;

							//Porting CS5 : InterfacePtr<IHierarchy> imageItem (db, dataLinkHelper->GetAssociatedPageItem(link), UseDefaultIID());
							//if(imageItem == nil)
							//	continue;

							//if(Utils<ILayoutUtils>()->IsAMaster(Utils<ILayoutUtils>()->GetOwnerPageUID(imageItem), db) == kFalse)
								
							UpdateLink(db, link, imageFolder, parser);
						}
						break;

			case 2 : // all images in masters
						for(int32 i = 0 ; i < nbLinks ; ++i)
						{
							InterfacePtr<IDataLink>  link (linkList[i], UseDefaultIID());
							if(link == nil)
								continue;

							// Porting CS5 : InterfacePtr<IHierarchy> imageItem (db, dataLinkHelper->GetAssociatedPageItem(link), UseDefaultIID());
							//if(imageItem == nil)
							//	continue;

							//if(Utils<ILayoutUtils>()->IsAMaster(Utils<ILayoutUtils>()->GetOwnerPageUID(imageItem), db))
								
							UpdateLink(db, link, imageFolder, parser);
						}
						break;

			default: break;
		}				

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

ErrorCode Func_RK::UpdateLink(IDataBase * db, IDataLink * link, const IDFile& imageDir, ITCLParser * parser)
{
	ErrorCode status = kFailure;
	do
	{														
		// Get link file name
		PMString * linkName = link->GetBaseName();

		// Check whether this file name exists in image folder
		IDFile imageFile = imageDir;
		FileUtils::AppendPath(&imageFile, *linkName);
		if(FileUtils::DoesFileExist(imageFile))
		{																		
			// File found, restore link
			InterfacePtr<IDataLinkHelper> dataLinkHelper (::CreateObject2<IDataLinkHelper>(kDataLinkHelperBoss));
			InterfacePtr<IDataLink> newLink (dataLinkHelper->CreateDataLink(imageFile));

			NameInfo ni;
			PMString formatName;	
			uint32 fileType;
			newLink->GetNameInfo(&ni, &formatName, &fileType);

			InterfacePtr<ICommand> restoreLinkCmd (CmdUtils::CreateCommand(kRestoreLinkCmdBoss));
			//restoreLinkCmd->SetUndoability(ICommand::kAutoUndo);

			InterfacePtr<IRestoreLinkCmdData> relinkCmdData(restoreLinkCmd, IID_IRESTORELINKCMDDATA);
			relinkCmdData->Set(db, ::GetUID(link), &ni, &formatName, fileType, IDataLink::kLinkNormal);

			if (CmdUtils::ProcessCommand(restoreLinkCmd) != kSuccess) 
				continue;
			
			// Reimport image
			/*InterfacePtr<IUpdateLink> updateLinkHelper (dataLinkHelper, UseDefaultIID());
			if(updateLinkHelper->IsOPIorDCS(link))								
				updateLinkHelper->UpdateOPIandDCS(link);								
			else						
			{
				UID newLinkUID = kInvalidUID;
				updateLinkHelper->DoUpdateLink(link, &newLinkUID);																						
			}*/
		}
		else
		{
			// Write log file
			PMString missingMsg(kPrsMissingLinkMsg);
			missingMsg.Translate();
			StringUtils::ReplaceStringParameters(&missingMsg, imageFile.GetFileName(), FileUtils::SysFileToPMString(imageDir));
			parser->WriteLogFile(missingMsg);
		}
					
		status = kSuccess;

	} while(kFalse);

	return status;
}


/* Constructor
*/
Func_AI::Func_AI(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		reader->ExtractTCLRef(kCurrentTCLRef);
		reader->ExtractReel('P','+',0,0);
		reader->ExtractReel('P','+',0,0);
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

	if(!reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Constructor
*/
Func_CDiese::Func_CDiese(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_BDollar::Func_BDollar -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);	
		justify = reader->ExtractBooleen(kTrue);
		includeStroke = reader->ExtractBooleen(kFalse);
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
		IDCall_CDiese(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* GetPositionRelativeToColumn
*/
void Func_CDiese::GetPositionRelativeToColumn(IDocument * doc, int32 page, int32 column, PMReal offset, PMPoint& leftTop, PMReal& width, UID& ownerSpreadLayer)
{
	IDataBase * db = ::GetDataBase(doc);

	// Get page index
	InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
	UID pageUID = pageList->GetNthPageUID(page - 1);
	
	// Get master page applied to this page
	InterfacePtr<IGeometry> pageGeo (db, pageUID, UseDefaultIID());

	PMRect marginsRect;
	InterfacePtr<IMargins> pageMargins (db, pageUID, UseDefaultIID());
	pageMargins->GetMargins(&marginsRect, kTrue);

	// Get the first column width
	InterfacePtr<IColumns> pageColumns (db, pageUID, IID_ICOLUMNS);
	
	PMRealList colPos;
	pageColumns->GetColumns_(&colPos);

	width = colPos[(column*2)+1] - colPos[column*2];

	leftTop.X() = marginsRect.Left() + colPos[column*2];
	leftTop.Y() = marginsRect.Top();		

	// Convert text frame left top to pasteboard coordinates
	::TransformInnerPointToPasteboard(pageGeo, &leftTop);

	leftTop.Y() += offset;

	// Get owner spread layer
	InterfacePtr<IDocumentLayer> activeLayer(Utils<ILayerUtils>()->QueryDocumentActiveLayer(doc));

	InterfacePtr<IHierarchy> pageHier(pageGeo,UseDefaultIID());
	InterfacePtr<ISpread> iSpread(db, pageHier->GetSpreadUID(),UseDefaultIID());
	InterfacePtr<ISpreadLayer> iSpreadLayer (iSpread->QueryLayer(activeLayer));

	ownerSpreadLayer = ::GetUID(iSpreadLayer);
}

/* GetPositionRelativeToItem
*/
void Func_CDiese::GetPositionRelativeToItem(IDocument * doc, UID itemUID, PMReal offset, bool16 includeStroke, PMPoint& leftTop, PMReal& width, UID& ownerSpreadLayer)
{
	IDataBase * db = ::GetDataBase(doc);

	// Compute frame position
	InterfacePtr<IGeometry> relativeItemGeo (db, itemUID, UseDefaultIID());
	PMRect itemRect;
	
	if(includeStroke)
		itemRect = relativeItemGeo->GetStrokeBoundingBox();
	else
		itemRect = relativeItemGeo->GetPathBoundingBox();

	::TransformInnerRectToPasteboard(relativeItemGeo, &itemRect);

	leftTop = itemRect.LeftBottom();
	leftTop.Y() += offset;

	width = itemRect.Width();

	// Get owner spread layer
	InterfacePtr<IHierarchy> relativeItemHier (relativeItemGeo, UseDefaultIID());
	ownerSpreadLayer = relativeItemHier->GetLayerUID();	
}


/* IDCall_CDiese
*/
void Func_CDiese::IDCall_CDiese(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
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

		InterfacePtr<IItemTransform> itemTransform(parser->QueryItemTransform());
		InterfacePtr<IPageHelper> pageHelper(parser->QueryPageHelper());

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if (seq == nil) 
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDataBase * db = parser->getCurDataBase();

		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		// Check whether item is contained within its owner page margins and columns
		InterfacePtr<IGeometry> itemGeo (db, refUID, UseDefaultIID());
		PMRect itemRect;
		
		if(includeStroke)
			itemRect = itemGeo->GetStrokeBoundingBox();
		else
			itemRect = itemGeo->GetPathBoundingBox();

		InterfacePtr<IHierarchy> itemHier (itemGeo, UseDefaultIID());

		UID ownerPage = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);

		// Convert item coordinates to page coordinates
		InterfacePtr<IGeometry> pageGeo (db, ownerPage, UseDefaultIID());
		
		::TransformInnerRectToPasteboard(itemGeo, &itemRect);
		::TransformPasteboardRectToInner(pageGeo, &itemRect);

		// Get item's owner column box
		InterfacePtr<IColumns> pageColumns (db, ownerPage, IID_ICOLUMNS);
		if(!pageColumns) // Item is on a spread
		{
			error = PMString(kErrItemOutOfColumns);
			break;
		}
		
		PMRect marginsRect;
		InterfacePtr<IMargins> pageMargins (db, ownerPage, UseDefaultIID());
		pageMargins->GetMargins(&marginsRect, kTrue);

		PMRealList colPos;
		pageColumns->GetColumns_(&colPos);

		int32 ownerColumnIndex = 0;
		int32 numColumns = pageColumns->GetNumberColumns_();

		PMReal relItemLeft = itemRect.Left() - marginsRect.Left();
		
		for( ; ownerColumnIndex < numColumns ; ++ownerColumnIndex)
		{			
			if(relItemLeft >= colPos[ownerColumnIndex*2] && relItemLeft <= colPos[(ownerColumnIndex*2)+1] )
				break;
			else
			{
				// BUG FIX : round value, because there may have been a lake of precision when moving an item to the next column due to InnerToPasteboard
				PMReal threshold = 0.05;
				if(::abs(relItemLeft - colPos[ownerColumnIndex*2]) <= threshold)
					break;
			}
		}	
		
		if(ownerColumnIndex >= numColumns)
		{
			error = PMString(kErrItemOutOfColumns);
			break;
		}
		
		PMReal marginsBottom = pageGeo->GetStrokeBoundingBox().Height() - marginsRect.Bottom();
				
		// Check containement
		if( itemRect.Top() >= marginsRect.Top() 
		 && itemRect.Bottom() <= marginsBottom)
		{
			// Do nothing, item fill in column
			//CAlert::InformationAlert("OK");
		}
		else
		{
			// Move item to next column or page
			InterfacePtr<IGeometry> newOwnerPageGeo (nil);
			InterfacePtr<IPageList> pageList (doc, UseDefaultIID());

			int32 columnToJustify = ownerColumnIndex; // Save column index for later vertical justification
			if(ownerColumnIndex + 1 >= pageColumns->GetNumberColumns_())
			{
				// Move item to first column of next page
				ownerColumnIndex = 0;
				
				InterfacePtr<IHierarchy> pageHier (pageGeo, UseDefaultIID());
				InterfacePtr<IHierarchy> spreadLayer (pageHier->QueryParent());
				UIDRef currentSpreadRef = UIDRef(db, pageHier->GetSpreadUID());
								
				InterfacePtr<IHierarchy> newOwnerPageHier (nil);
								
				if(pageList->GetPageIndex(ownerPage) + 1 == pageList->GetPageCount())
				{					
					// Add a new page with the same master page as the last one			
			
					int32 posToInsert = spreadLayer->GetChildIndex(pageHier) + 1;
				
					// Get last page's master
					InterfacePtr<IMasterPage> pageMaster (pageHier, UseDefaultIID());
					UIDRef masterPageUIDRef = UIDRef(db, pageMaster->GetMasterSpreadUID());					

					if(pageHelper->CreateNewPageCmd(currentSpreadRef,1,posToInsert,masterPageUIDRef) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}		

					newOwnerPageHier = InterfacePtr<IHierarchy>(db, pageList->GetNthPageUID(pageList->GetPageCount() - 1), UseDefaultIID());
				}
				else
					newOwnerPageHier = InterfacePtr<IHierarchy>(db, pageList->GetNthPageUID(pageList->GetPageIndex(ownerPage) + 1), UseDefaultIID());
					
				UIDRef newSpreadRef = UIDRef(db, newOwnerPageHier->GetSpreadUID());
					
				// Get item's new owner page geometry
				newOwnerPageGeo = InterfacePtr<IGeometry>(newOwnerPageHier, UseDefaultIID());

				if(newSpreadRef != currentSpreadRef)
				{
					// We have to change item owner spread before moving it to its new position
					InterfacePtr<ICommand> changeSpreadCmd (CmdUtils::CreateCommand(kMoveToSpreadCmdBoss));
					changeSpreadCmd->SetItemList(UIDList(db, refUID));

					InterfacePtr<IUIDData> spreadUID (changeSpreadCmd, UseDefaultIID());
					spreadUID->Set(newSpreadRef);
				
					if(CmdUtils::ProcessCommand(changeSpreadCmd) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}		
				}
			}
			else
			{
				++ownerColumnIndex;
				newOwnerPageGeo = pageGeo; // Owner page remains the same
			}

			InterfacePtr<IColumns> newPageColumns (newOwnerPageGeo, IID_ICOLUMNS);
			InterfacePtr<IMargins> newPageMargins (newPageColumns, UseDefaultIID());

			PMRealList newColPos;
			newPageColumns->GetColumns_(&newColPos);

			PMReal top = 0, left = 0;
			if(pageList->GetPageType(::GetUID(newOwnerPageGeo)) == kLeftPage)
				newPageMargins->GetMargins(nil, &top, &left, nil);
			else
				newPageMargins->GetMargins(&left, &top, nil, nil);

			PMPoint leftTop (left + newColPos[ownerColumnIndex*2], top);
			::TransformInnerPointToPasteboard(newOwnerPageGeo,&leftTop);

			// Take stroke into account when placing item in new column, only for items created via BN or BI (rectangle items)
			InterfacePtr<IPathGeometry> itemPathGeo (itemGeo, UseDefaultIID());
			if(!includeStroke && Utils<IPathUtils>()->IsRectangle(itemPathGeo))
			{
				PMReal diff = itemGeo->GetPathBoundingBox().Top() - itemGeo->GetStrokeBoundingBox().Top();
				leftTop.X() -= diff;
			}

			if(itemTransform->MoveCmd(UIDRef(db, refUID),leftTop) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}	

			// Resize item if its new column has not the same size as the previous one
			PMReal colWidth = newColPos[(ownerColumnIndex*2)+1] - newColPos[ownerColumnIndex*2];
			if(itemRect.Width() != colWidth)
			{
				PMReal xFactor = 1 + ((colWidth-itemRect.Width())/itemRect.Width());
				
				Geometry::BoundsKind boundsKind = includeStroke ? Geometry::OuterStrokeBounds() : Geometry::PathBounds();
				
				ErrorCode resizeCmd = Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(itemGeo), Transform::PasteboardCoordinates(), 
										boundsKind , leftTop, Geometry::MultiplyBy(xFactor,Geometry::KeepCurrentValue()));					

				if(resizeCmd != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}

			if(justify)
			{
				// Collect all item in the column
				InterfacePtr<IHierarchy> pageHier (pageGeo, UseDefaultIID());

				// Get owner spread
				InterfacePtr<ISpread> spreadHier (db, pageHier->GetSpreadUID(), UseDefaultIID());
				
				UIDList itemsOnPage(db);
				spreadHier->GetItemsOnPage(spreadHier->GetPageIndex(::GetUID(pageGeo)), &itemsOnPage, kFalse);

				int32 numItems = itemsOnPage.Length();

				K2Vector<UIDRef> itemsInColumn;
				PMReal itemsTotalHeight = 0.0;
				for(int32 i = 0 ; i < itemsOnPage.Length() ; ++i)
				{
					InterfacePtr<IGeometry> itemGeo (itemsOnPage.GetRef(i), UseDefaultIID());
					PMRect itemBounds;
					if(includeStroke)
						itemBounds = itemGeo->GetStrokeBoundingBox();
					else
						itemBounds = itemGeo->GetPathBoundingBox();

					::TransformInnerRectToPasteboard(itemGeo, &itemBounds);
					::TransformPasteboardRectToInner(pageGeo, &itemBounds);

					// Check containement
					if(	itemBounds.Left() >= (marginsRect.Left() + colPos[columnToJustify*2])
					 && itemBounds.Left() <= (marginsRect.Left() + colPos[(columnToJustify*2) + 1])
					 && itemBounds.Top() >= marginsRect.Top() 
					 && itemBounds.Bottom() <= marginsBottom)
					{
						itemsInColumn.push_back(itemsOnPage.GetRef(i));
						itemsTotalHeight += itemGeo->GetStrokeBoundingBox().Height();
					}
				}

				if(itemsInColumn.size() > 1)
				{
					PMReal colHeight = pageGeo->GetStrokeBoundingBox().Height() - marginsRect.Bottom() - marginsRect.Top();
					PMReal amountToAdd = (colHeight - itemsTotalHeight ) / (itemsInColumn.size() - 1);

					if(amountToAdd > 0)
					{
						// Sort items in columns from top to bottom				
						std::sort(itemsInColumn.begin(),itemsInColumn.end(), CompareTop());

						// Get top item
						InterfacePtr<IGeometry> topItemGeo (itemsInColumn[0], UseDefaultIID());
						
						PMPoint topPoint (topItemGeo->GetStrokeBoundingBox().LeftTop());
						::TransformInnerPointToPasteboard(topItemGeo,&topPoint);

						PMReal pos = topPoint.Y() + topItemGeo->GetStrokeBoundingBox().Height() + amountToAdd ;

						// Move items so that vertical space is distributed equally
						for(int32 i = 1 ; i < itemsInColumn.size() ; ++i)
						{
							InterfacePtr<IGeometry> itemGeom(itemsInColumn[i], UseDefaultIID()) ;
			
							PMPoint newLeftTop (itemGeom->GetStrokeBoundingBox().LeftTop());
							::TransformInnerPointToPasteboard(itemGeom,&newLeftTop);

							newLeftTop.Y(pos);
							
							if(itemTransform->MoveCmd(itemsInColumn[i],newLeftTop) != kSuccess)
							{
								error = ErrorUtils::PMGetGlobalErrorString();
								break;
							}

							pos = pos + itemGeom->GetStrokeBoundingBox().Height() +  amountToAdd ;				
						}
					}
				}
			}
		}

		status = kSuccess;

	} while(false);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurItem(refBloc);
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);		
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_XA::Func_XA(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

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
		IDCall_XA(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_XA
*/
void Func_XA::IDCall_XA(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		PMString xpaInfo;

		// Create millimeters converters to output dimensions in this unit measure
		InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));

		// Write information about pages and columns
		InterfacePtr<IPageList> pageList (parser->getCurDoc(), UseDefaultIID());
		
		InterfacePtr<IGeometry> firstPageGeo (db, pageList->GetNthPageUID(0), UseDefaultIID());
		InterfacePtr<IColumns> firstPageColumns (firstPageGeo, IID_ICOLUMNS);			

		xpaInfo += "NbPages=";
		xpaInfo.AppendNumber(pageList->GetPageCount());
		xpaInfo += ";H=";
		xpaInfo.AppendNumber(uom->PointsToUnits(firstPageGeo->GetStrokeBoundingBox().Height()));
		xpaInfo += ";L=";
		xpaInfo.AppendNumber(uom->PointsToUnits(firstPageGeo->GetStrokeBoundingBox().Width()));
		xpaInfo += ";NbCol=";
		xpaInfo.AppendNumber(firstPageColumns->GetNumberColumns_());
		xpaInfo += ";WGoutiere=";
		xpaInfo.AppendNumber(uom->PointsToUnits(firstPageColumns->GetGutter_()));

		// Write first page section
		InterfacePtr<ISectionList> sectionList (pageList, UseDefaultIID());
		PMString firstPageSection;
		sectionList->GetPageString(::GetUID(firstPageGeo), &firstPageSection, kFalse);

		xpaInfo += ";DebSection=";
		xpaInfo += firstPageSection;

		// Write margins information
		InterfacePtr<IMargins> firstPageMargins (firstPageGeo, UseDefaultIID());	
		PMRect margins;
		firstPageMargins->GetMargins(&margins);

		xpaInfo += ";MT=";
		xpaInfo.AppendNumber(uom->PointsToUnits(margins.Top()));
		xpaInfo += ";MB=";
		xpaInfo.AppendNumber(uom->PointsToUnits(margins.Bottom()));
		xpaInfo += ";ML=";
		xpaInfo.AppendNumber(uom->PointsToUnits(margins.Left()));
		xpaInfo += ";MR=";
		xpaInfo.AppendNumber(uom->PointsToUnits(margins.Right()));
		xpaInfo += "\r\n";
		
		// TODO : Loop for all spreads to export frame position
		// InterfacePtr<ISpreadList> spreadList (pageList, UseDefaultIID());

		xpaInfo.SetTranslatable(kFalse);
		parser->WriteLogFile(xpaInfo);
		status = kSuccess;
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}