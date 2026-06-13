/*
//	File:	FLBItemManager.cpp
//
//  Author: Trias
//
//	Date:	18-Jun-2003
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
#include "SplineID.h"
#include "ImageID.h"
#include "TransformUtils.h"
#include "SDKFileHelper.h"
#include "ErrorUtils.h"
#include "CAlert.h"
#include "PageItemScrapID.h"
#include "StreamUtil.h"
#include "FileUtils.h"
#include "LocaleSetting.h"

#include <iostream>

// Interface includes
#include "ICommand.h"
#include "IHierarchy.h"
#include "IGeometry.h"
#include "IDocument.h"
#include "IGeometryFacade.h"
#include "IMultiColumnItemData.h"
#include "IMultiColumnTextFrame.h"
#include "ITextFrameColumn.h"
#include "INewPageItemCmdData.h"
#include "IPageList.h"
#include "ISelectionUtils.h"
#include "ILayoutSelectionSuite.h"
#include "ISpread.h"
#include "ISpreadLayer.h"
#include "IPlacePIData.h"
//#include "IImportFileCmdData.h"
#include "INewPageItemCmdData.h"
#include "IGroupItemUtils.h"
#include "IDocumentLayer.h"
#include "ILockPosition.h"
#include "IImageAttributes.h"
#include "IGroupCmdData.h"
#include "IIntData.h"
#include "IGraphicFrameData.h"
#include "IWaxStrand.h"
#include "IWaxLine.h"
#include "IWaxRun.h"
#include "IWaxIterator.h"
#include "IUIDData.h"

#include "IFrameList.h"
#include "ITextUtils.h"
#include "IFrameListComposer.h"
#include "IImportResourceCmdData.h"
#include "URI.h"
#include "IURIUtils.h"
#include "SDKUtilities.h"
#include "IEPSTagTable.h"
#include "IPlaceGun.h"
#include "IPageItemTypeUtils.h"
#include "ISwatchUtils.h"
#include "IGraphicStateUtils.h"
#include "IRenderingObject.h"
#include "ISwatchList.h"
#include "IWorkspace.h"

// Project includes
#include "FLBItemManager.h"
#include "SDKLayoutHelper.h"


CREATE_PMINTERFACE(FLBItemManager,kFLBItemManagerImpl)

/* Constructor
*/
FLBItemManager::FLBItemManager(IPMUnknown * boss)
: CPMUnknown<IItemManager>(boss)
{
	masterInCreation = kInvalidUIDRef;
}

/* Destructor
*/
FLBItemManager::~FLBItemManager()
{
}

/* CreateTextFrameCmd
*/
UIDRef FLBItemManager::CreateTextFrameCmd(IDataBase * db, PMPointList bounds, UID ownerSpreadLayer,int32 numColumns, PMReal gutterWidth, bool8 allowUndo)
{
	UIDRef txtFrameRef(nil,kInvalidUID);
	PMString error(kFlbNilInterface);

	do
	{
		if(db == nil)
		{
			ASSERT_FAIL("FLBItemManager::CreateTextFrameCmd -> invalid db");
			error = PMString(kFlbNilParameter);
			break;
		}

		// Command to create a text frame
		InterfacePtr<ICommand> createTextFrameCmd(CmdUtils::CreateCommand(kCreateMultiColumnItemCmdBoss));
		if(createTextFrameCmd == nil)
		{
			ASSERT_FAIL("FLBItemManager::CreateTextFrameCmd -> createTextFrameCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			createTextFrameCmd->SetUndoability(ICommand::kAutoUndo);	

		// Set frame size and frame parent
		InterfacePtr<INewPageItemCmdData> data(createTextFrameCmd, UseDefaultIID());
		if(data == nil)
		{
			ASSERT_FAIL("FLBItemManager::CreateTextFrameCmd -> data nil");
			break;
		}

		data->Set(db,kSplineItemBoss, ownerSpreadLayer, bounds);

		// Set number of colums and gutter
		InterfacePtr<IMultiColumnItemData> iMultiColumnItemData(createTextFrameCmd,UseDefaultIID());
		if(iMultiColumnItemData == nil)
		{
			ASSERT_FAIL("FLBItemManager::CreateTextFrameCmd -> iMultiColumnItemData nil");
			break;
		}

		iMultiColumnItemData->SetNumberOfColumns(numColumns);
		iMultiColumnItemData->SetGutterWidth(gutterWidth);

		if(CmdUtils::ProcessCommand(createTextFrameCmd) != kSuccess)
		{
			error = PMString(kCreateTextFrameCmdFailed);
			break;
		}

		// Get the UIDRef of the text frame created
		txtFrameRef = createTextFrameCmd->GetItemList()->GetRef(0); 

	} while(false);

	if(txtFrameRef == UIDRef(nil,kInvalidUID))
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return txtFrameRef;
}

/* GetPBCoordAbs
*/
UID FLBItemManager::GetPBCoordAbs(IDocument * doc, int32 numPage, PMPointList & pointsToTransform, IDocumentLayer * docLayer)
{
	UID ownerSpreadLayer = kInvalidUID;
	PMString error(kFlbNilInterface);

	do
	{
		// Looking for the uid of the page where the new frame should be added
		UID pageUID = kInvalidUID;
		if(masterInCreation == kInvalidUIDRef)
		{
			InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
			if(pageList == nil)
			{
				ASSERT_FAIL("FLBItemManager::GetPBCoordAbs -> pageList nil");
				error = PMString(kFlbNilParameter);
				break;
			}

			pageUID = pageList->GetNthPageUID(numPage - 1);
		}
		else
		{
			InterfacePtr<ISpread> masterSpread (masterInCreation, UseDefaultIID());
			if(masterSpread == nil)
				break;

			pageUID = masterSpread->GetNthPageUID(numPage-1);
		}

		// Convert all the point in the list (considered as in described in the 
		// page coordinates system) in pasteboard coordinates
		InterfacePtr<IGeometry> pageGeo (::GetDataBase(doc),pageUID,UseDefaultIID());
		if(pageGeo == nil)
		{
			error = PMString(kErrNoSuchPage);
			break;
		}

		int32 nbPoints = pointsToTransform.size();

		// List for the converted points
		PMPointList transformedPoints;

		for(int i = 0; i < nbPoints; i++)
		{
			PMPoint point(pointsToTransform[i]);
			::TransformInnerPointToPasteboard(pageGeo,&point);
			transformedPoints.push_back(point);
		}
		
		// Looking for the spread layer that will own the new page item built with the points
		InterfacePtr<IHierarchy> pageHier(pageGeo,UseDefaultIID());
		if(pageHier == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordAbs -> pageHier nil");
			break;
		}

		InterfacePtr<ISpread> iSpread(::GetDataBase(doc),pageHier->GetSpreadUID(),UseDefaultIID());
		if(iSpread == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordAbs -> iSpread nil");
			break;
		}

		InterfacePtr<ISpreadLayer> iSpreadLayer (iSpread->QueryLayer(docLayer));
		if(iSpreadLayer == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordAbs -> iSpreadLayer nil");
			break;
		}

		ownerSpreadLayer = ::GetUID(iSpreadLayer);
		// Return the list of converted points
		pointsToTransform = transformedPoints;

	} while(false);

	if(ownerSpreadLayer == kInvalidUID)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return ownerSpreadLayer;
}

/* GetPBCoordRel
*/
UID FLBItemManager::GetPBCoordRel(UIDRef spline, PMPointList & pointsToTransform, int8 alignment, IDocumentLayer * docLayer)
{
	UID ownerSpreadLayer = kInvalidUID;
	PMString error(kFlbNilInterface);

	do
	{
		PMPoint refPoint = GetAbsReferencePoint(spline, alignment);

		int32 nbPoints = pointsToTransform.size();

		// List for the converted points
		PMPointList transformedPoints;

		// Transform the points from reference item coordinate system, relative to the reference point,
		// to pasteboard coordinate system
		for(int i = 0; i < nbPoints; i++)
		{
			PMPoint point(pointsToTransform[i]);
			point.X() = point.X() + refPoint.X();
			point.Y() = point.Y() + refPoint.Y();
			
			transformedPoints.push_back(point);
		}
		
		// Looking for the spread layer that will own the new page item
		InterfacePtr<IHierarchy> refHier (spline,UseDefaultIID());
		if(refHier == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordRel -> refHier nil");
			break;
		}
		
		InterfacePtr<ISpread> iSpread(::GetDataBase(refHier),refHier->GetSpreadUID(),UseDefaultIID());
		if(iSpread == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordRel -> iSpread nil");
			break;
		}

		InterfacePtr<ISpreadLayer> iSpreadLayer (iSpread->QueryLayer(docLayer));
		if(iSpreadLayer == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordRel -> iSpreadLayer nil");
			break;
		}
		
		ownerSpreadLayer = ::GetUID(iSpreadLayer);
		
		// Return the list of converted points
		pointsToTransform = transformedPoints;

	} while(false);

	if(ownerSpreadLayer == kInvalidUID)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return ownerSpreadLayer;
}

PMPoint FLBItemManager::GetRelReferencePoint(UIDRef spline, int8 alignment)
{
	ErrorCode status = kFailure;
	PMString error;
	PMPoint refPoint;
	do
	{
		InterfacePtr<IGeometry> refGeo (spline,UseDefaultIID());
		if(refGeo == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordRel -> refGeo nil");
			error = PMString(kErrNotSpline);
			break;
		}
		
		// Set the reference point according to the alignment
		PMRect refBounds = refGeo->GetStrokeBoundingBox();
		
		// Get the reference point and convert it in pasteboard coordinates
		switch(alignment)
		{
			case TOP_ALIGNEMENT : 
				refPoint = refBounds.LeftTop(); 
				status = kSuccess;
				break;
			case BOTTOM_ALIGNEMENT : 
				refPoint = refBounds.LeftBottom(); 
				status = kSuccess;
				break;
			case TEXT_ALIGNEMENT : 
				{
					refPoint = PMPoint();
					InterfacePtr<IGraphicFrameData> frameData (spline, UseDefaultIID()); 
					if (frameData != nil) 
					{ 
						UID textFrameUID = frameData->GetTextContentUID();
						InterfacePtr<IMultiColumnTextFrame> textFrame(spline.GetDataBase(), textFrameUID, UseDefaultIID()); 
						if(textFrame != nil)
						{
							InterfacePtr<IWaxStrand> waxStrand (spline.GetDataBase(),textFrame->GetFrameListUID(), UseDefaultIID());
							if(waxStrand == nil)
								break;

							InterfacePtr<IFrameList> frameList (waxStrand, UseDefaultIID());
							if(frameList == nil)
								break;

							InterfacePtr<ITextModel> txtModel (frameList->QueryTextModel());
							if(txtModel == nil)
								break;							


							ITextFrameColumn * txtFrame = frameList->QueryFrameContaining(txtModel->GetPrimaryStoryThreadSpan()-1,nil);
							TextIndex lastChar;			
							if(txtFrame == nil)
							{
								txtFrame = frameList->QueryNthFrame(frameList->GetFrameCount()-1);
								lastChar = txtFrame->TextStart() + txtFrame->TextSpan() - 1;
							}	
							else
								lastChar = txtModel->GetPrimaryStoryThreadSpan()- 1;
							
							waxStrand->GetTextIndexToWaxLineData(lastChar, kFalse,nil,nil,nil,nil,nil,nil,&refPoint);

							txtFrame->Release();

							status = kSuccess;
						}
					}
					
				}
				break;
					
			default : refPoint = PMPoint(); break;
		}
	}
	while(kFalse);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return refPoint;
}


PMPoint FLBItemManager::GetAbsReferencePoint(UIDRef spline, int8 alignment)
{
	ErrorCode status = kFailure;
	PMString error;
	PMPoint refPoint;
	do
	{
		InterfacePtr<IGeometry> refGeo (spline,UseDefaultIID());
		if(refGeo == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordRel -> refGeo nil");
			error = PMString(kErrNotSpline);
			break;
		}
		
		// Set the reference point according to the alignment
		PMRect refBounds = refGeo->GetStrokeBoundingBox();
		
		// Get the reference point and convert it in pasteboard coordinates
		switch(alignment)
		{
			case TOP_ALIGNEMENT : 
				refPoint = refBounds.LeftTop(); 
				status = kSuccess;
				break;
			case BOTTOM_ALIGNEMENT : 
				refPoint = refBounds.LeftBottom(); 
				status = kSuccess;
				break;
			case TEXT_ALIGNEMENT : 
				{
					refPoint = PMPoint();
					InterfacePtr<IGraphicFrameData> frameData (spline, UseDefaultIID()); 
					if (frameData != nil) 
					{ 
						UID textFrameUID = frameData->GetTextContentUID();
						InterfacePtr<IMultiColumnTextFrame> textFrame(spline.GetDataBase(), textFrameUID, UseDefaultIID()); 
						if(textFrame != nil)
						{
							InterfacePtr<IWaxStrand> waxStrand (spline.GetDataBase(),textFrame->GetFrameListUID(), UseDefaultIID());
							if(waxStrand == nil)
								break;

							InterfacePtr<IFrameList> frameList (waxStrand, UseDefaultIID());
							if(frameList == nil)
								break;

							InterfacePtr<ITextModel> txtModel (frameList->QueryTextModel());
							if(txtModel == nil)
								break;							


							ITextFrameColumn * txtFrame = frameList->QueryFrameContaining(txtModel->GetPrimaryStoryThreadSpan()-1,nil);
							TextIndex lastChar;			
							if(txtFrame == nil)
							{
								txtFrame = frameList->QueryNthFrame(frameList->GetFrameCount()-1);
								lastChar = txtFrame->TextStart() + txtFrame->TextSpan() - 1;
							}	
							else
								lastChar = txtModel->GetPrimaryStoryThreadSpan()- 1;
							
							waxStrand->GetTextIndexToWaxLineData(lastChar, kFalse,nil,nil,nil,nil,nil,nil,&refPoint);

							// Get the last spline's geometry, in case text frame are linked, to convert refpoint
							InterfacePtr<IHierarchy> lastSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
							if(lastSpline == nil)
								break;

							InterfacePtr<IGeometry> lastSplineGeo (lastSpline,UseDefaultIID());
							if(lastSplineGeo == nil)
								break;

							refGeo = lastSplineGeo;
							txtFrame->Release();

							status = kSuccess;
						}
					}
					
				}
				break;
					
			default : refPoint = PMPoint(); break;
		}

		::TransformInnerPointToPasteboard(refGeo,&refPoint);
	}
	while(kFalse);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return refPoint;
}

/* CheckContainmentInOwnerSpread
*/
bool8 FLBItemManager::CheckContainmentInOwnerSpread(IGeometry * itemGeo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);
	bool8 IsContained = kFalse;

	do
	{
		// Check the spline item geometry
		if(itemGeo == nil)
		{
			ASSERT_FAIL("FLBItemManager::CheckContainmentInOwnerSpread -> itemGeo nil");
			error = PMString(kFlbNilParameter);
			break;
		}

		// Get the owner spread geometry
		InterfacePtr<IHierarchy> itemHier (itemGeo,UseDefaultIID());
		if(itemHier == nil)
		{
			ASSERT_FAIL("FLBItemManager::CheckContainmentInOwnerSpread -> itemHier nil");
			break;
		}

		InterfacePtr<IGeometry> spreadGeo(::GetDataBase(itemGeo),itemHier->GetSpreadUID(),UseDefaultIID());
		if(spreadGeo == nil)
		{
			ASSERT_FAIL("FLBItemManager::CheckContainmentInOwnerSpread -> spreadGeo nil");
			break;
		}

		// Get the spline item and spread bounds and use utility fonction to do the verification
		PBPMRect spreadBounds = Utils<Facade::IGeometryFacade>()->GetItemBounds(::GetUIDRef(spreadGeo), Transform::PasteboardCoordinates(), Geometry::OuterStrokeBounds());
		PBPMRect itemBounds = Utils<Facade::IGeometryFacade>()->GetItemBounds(::GetUIDRef(itemGeo), Transform::PasteboardCoordinates(), Geometry::OuterStrokeBounds());
		IsContained = (itemBounds.Bottom() <= spreadBounds.Bottom()
					&& itemBounds.Top() >= spreadBounds.Top()
					&& itemBounds.Left() >= spreadBounds.Left()
					&& itemBounds.Right() <= spreadBounds.Right());					
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return IsContained;
}

/* SelectPageItem
*/
ErrorCode FLBItemManager::SelectPageItem(UIDRef pageItem)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);
#if !SERVER // Selection architecture is not available in server version
	do
	{
		if(pageItem.GetDataBase() == nil || pageItem.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemManager::SelectPageItem -> invalid pageitem");
			error = PMString(kFlbNilParameter);
			break;
		}
		
		InterfacePtr<ISelectionManager>	selectionManager (Utils<ISelectionUtils> ()->QueryActiveSelection ()); 
		InterfacePtr<ILayoutSelectionSuite> layoutSelectionSuite(selectionManager, UseDefaultIID()); 

		if (!layoutSelectionSuite) 
		{ 
			ASSERT_FAIL("FLBItemManager::SelectPageItem -> select nil");
			break;
		}

		selectionManager->DeselectAll(nil); 
		layoutSelectionSuite->SelectPageItems(UIDList(pageItem), Selection::kReplace, Selection::kDontScrollLayoutSelection); 
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

#else
	status = kSuccess;
#endif

	return status;
}


/* SelectPageItems
*/
ErrorCode FLBItemManager::SelectPageItems(UIDList itemsToSelect)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		bool8 quit = kFalse;
		for (int32 i = 0 ; i < itemsToSelect.Length() ; i++ ) 
		{
			UIDRef pageItem = itemsToSelect.GetRef(i) ;
			if(pageItem.GetDataBase() == nil || pageItem.GetUID() == kInvalidUID)
			{
				quit = kTrue ;
				break;
			}
		}
		if (quit)
		{
			ASSERT_FAIL("FLBItemManager::SelectPageItems -> invalid List of pageitems");
			error = PMString(kFlbNilParameter);
			break;
		}

		InterfacePtr<ISelectionManager>	selectionManager (Utils<ISelectionUtils> ()->QueryActiveSelection ()); 
		InterfacePtr<ILayoutSelectionSuite> layoutSelectionSuite(selectionManager, UseDefaultIID()); 

		if (!layoutSelectionSuite) 
		{ 
			ASSERT_FAIL("FLBItemManager::SelectPageItem-> select nil");
			break;
		}
		 
		selectionManager->DeselectAll(nil); 
		layoutSelectionSuite->SelectPageItemsInternal(itemsToSelect, Selection::kReplace, Selection::kDontScrollLayoutSelection, kTrue); 
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* ImportImageCmd
*/
ErrorCode FLBItemManager::ImportImageCmd(IDataBase * db, UIDRef& imageItem, PMString picturePath, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);
	UIDRef result = UIDRef::gNull;
    UID retVal = kInvalidUID;
	do{
		
		SDKFileHelper helper(picturePath);
		IDFile f;

		// Check file path
		if(!helper.IsExisting()){
			error = PMString(kErrCannotOpenFile);
			break;
		}

		// Import image command
		/*	
		InterfacePtr<ICommand> importFileCmd(CmdUtils::CreateCommand(kImportAndLoadPlaceGunCmdBoss));
		if(importFileCmd == nil)
		{
			ASSERT_FAIL("FLBItemManager::ImportImageCmd -> importFileCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			importFileCmd->SetUndoability(ICommand::kAutoUndo);
	
		InterfacePtr<IImportResourceCmdData> importResourceCmdData(importFileCmd, IID_IIMPORTRESOURCECMDDATA); // no DefaultIID for this
	    ASSERT(importResourceCmdData);
		if(!importResourceCmdData){
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			break;
		}
	 	// TODO: LINKREWORK fix.
 
		URI tmpURI;
		Utils<IURIUtils>()->IDFileToURI(helper.GetIDFile(), tmpURI);

		importResourceCmdData->Set(db, tmpURI, kMinimalUI);
		ErrorCode err = CmdUtils::ProcessCommand(importFileCmd);
		ASSERT(err == kSuccess);
		if(err != kSuccess)
		{
			break;
		}   
	    */
	
		// Import image
		InterfacePtr<ICommand> importFileCmd(CmdUtils::CreateCommand(kImportResourceCmdBoss));
		if(importFileCmd == nil)
		{
			ASSERT_FAIL("FLBItemManager::ImportImageCmd -> importFileCmd nil");
			break;
		}
		// Set command undoability
		if(!allowUndo)
			importFileCmd->SetUndoability(ICommand::kAutoUndo);
		
		InterfacePtr<IImportResourceCmdData> data(importFileCmd, IID_IIMPORTRESOURCECMDDATA);
		
		// IDFile to URI
		URI tmpURI;
		Utils<IURIUtils>()->IDFileToURI(helper.GetIDFile(), tmpURI);
	
        // Set the file to import
        data->Set(db, tmpURI, K2::kMinimalUI);
		if(CmdUtils::ProcessCommand(importFileCmd) != kSuccess)
			break;
		
		// Get the contents of the place gun
		InterfacePtr<IPlaceGun> placeGun(db, db->GetRootUID(), UseDefaultIID());
        ASSERT(placeGun);
        if (!placeGun)
        {
            ErrorUtils::PMSetGlobalErrorCode(kFailure);
            break;
        }

		InterfacePtr<ICommand> clearPlaceGun(CmdUtils::CreateCommand(kClearPlaceGunCmdBoss));
        ASSERT(clearPlaceGun);
        if (!clearPlaceGun)
        {
            ErrorUtils::PMSetGlobalErrorCode(kFailure);
            break;
        }
		
        InterfacePtr<IUIDData> uidData(clearPlaceGun, UseDefaultIID());
        ASSERT(uidData);
        if (!uidData)
        {
            ErrorUtils::PMSetGlobalErrorCode(kFailure);
            break;
        }
        uidData->Set(placeGun);
        CmdUtils::ProcessCommand(clearPlaceGun);


		imageItem = UIDRef(db, importFileCmd->GetItemList()->First());
		UIDRef itemToCheck = imageItem;

		// This is to handle a special : sometimes item resulting from importation
		// isn't directly an image item (kImageItem, kEPSItem, kPlacedPDFItem), but
		// a kSplineItemBoss, father of an image item
		InterfacePtr<IHierarchy> itemHier (imageItem, UseDefaultIID());
		if(itemHier){
			if(itemHier->GetChildCount() > 0){
				InterfacePtr<IHierarchy> child (itemHier->QueryChild(0));
				itemToCheck = ::GetUIDRef(child);
			}
		}
		else
		{
			error = PMString(kErrNotImageFile);
			break;
		}
		
		// Check if it is an image file
		if(!Utils<IPageItemTypeUtils>()->IsGraphic(itemToCheck))
		{
			error = PMString(kErrNotImageFile);
			break;
		}
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* PlaceItemInGraphicFrameCmd
*/
ErrorCode FLBItemManager::PlaceItemInGraphicFrameCmd(UIDRef graphicFrame, UIDRef item, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		if(graphicFrame.GetDataBase() == nil || graphicFrame.GetDataBase() != item.GetDataBase()
		   || graphicFrame.GetUID() == kInvalidUID || item.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("FLBItemManager::PlaceItemInGraphicFrameCmd -> invalid(s) parameter(s)");
			error = PMString(kFlbNilParameter);
			break;
		}

		InterfacePtr<ICommand> placeItemCmd(CmdUtils::CreateCommand(kPlaceItemInGraphicFrameCmdBoss));
		if(placeItemCmd == nil)
		{
			ASSERT_FAIL("FLBItemManager::PlaceItemInGraphicFrameCmd -> placeItemCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			placeItemCmd->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<IPlacePIData> placeData(placeItemCmd,IID_IPLACEPIDATA);
		if(placeData == nil)
		{
			ASSERT_FAIL("FLBItemManager::PlaceItemInGraphicFrameCmd -> placeData nil");
			break;
		}

		// Set the frame that will receive the item
		placeData->Set(graphicFrame,nil,kFalse);

		// Set the item to place
		placeItemCmd->SetItemList(UIDList(item));
		status = CmdUtils::ProcessCommand(placeItemCmd);

		if(status != kSuccess)
			error = PMString(kPlaceItemCommandFailed);

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* ImportImageInGraphicFrameCmd
*/
ErrorCode FLBItemManager::ImportImageInGraphicFrameCmd(UIDRef graphicFrame,PMString picturePath, UIDRef& imageItem, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
	
		if(this->ImportImageCmd(graphicFrame.GetDataBase(),imageItem, picturePath, allowUndo) != kSuccess)
			break;


		// Delete existing image item, if any
		InterfacePtr<IHierarchy> containerFrame (graphicFrame, UseDefaultIID());
		if(containerFrame && containerFrame->GetChildCount() > 0)
		{
			InterfacePtr<IHierarchy> frameToDelete (containerFrame->QueryChild(0));

			InterfacePtr<ICommand> deletePageItem (CmdUtils::CreateCommand(kDeleteCmdBoss));
			deletePageItem->SetItemList(UIDList(frameToDelete));
			if(CmdUtils::ProcessCommand(deletePageItem) != kSuccess)
				break;
		}

		status = this->PlaceItemInGraphicFrameCmd(graphicFrame, imageItem, allowUndo);

	} while(false);

	return status;
}



ErrorCode FLBItemManager::GroupPageItemsCmd(UIDList * itemsToGroup, UIDRef& groupItem, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		//Get the items' Hierarchy in order to get the items' spread Layer
		UIDRef refBloc1(itemsToGroup->GetRef(0)) ;
		InterfacePtr<IHierarchy> pageItemHier (refBloc1,IID_IHIERARCHY) ;
		if(pageItemHier == nil)
		{
			ASSERT_FAIL("FLBItemManager::GroupPageItemsCmd -> pageItemHier nil");
			break;
		}

		//Get the items' spread layer in order to get the items' document layer
		InterfacePtr<ISpreadLayer> spreadLayer (UIDRef(refBloc1.GetDataBase(),pageItemHier->GetLayerUID()),IID_ISPREADLAYER) ;
		if(spreadLayer == nil)
		{
			ASSERT_FAIL("FLBItemManager::GroupPageItemsCmd -> spreadLayer nil");
			break;
		}

		//Get the items' document Layer in order to verify if we can group the items
		InterfacePtr<IDocumentLayer> docLayer(spreadLayer->QueryDocLayer()) ;

		if (!(Utils<IGroupItemUtils>()->CanEnableGroup(itemsToGroup, docLayer )) )
		{
			error = PMString(kErrUnableToGroup) ;
			break;
		}


		// Create a GroupCmd:
		InterfacePtr<ICommand> groupCmd(CmdUtils::CreateCommand(kGroupCmdBoss));
		if(groupCmd == nil)
		{
			ASSERT_FAIL("FLBItemManager::GroupPageItemsCmd -> groupCmd nil");
			break;
		}
			// Set command undoability
		if(!allowUndo)
			groupCmd->SetUndoability(ICommand::kAutoUndo);

		// Get an IGroupCmdData Interface for the GroupCmd:
		InterfacePtr<IGroupCmdData> groupData(groupCmd, IID_IGROUPCMDDATA);
		if(groupData == nil)
		{
			ASSERT_FAIL("FLBItemManager::GroupPageItemsCmd -> groupData nil");
			break;
		}

		groupData->Set(itemsToGroup);

		// Process the GroupCmd: 
		status = CmdUtils::ProcessCommand(groupCmd);
		if (status != kSuccess)
		{
			groupItem = UIDRef::gNull;
			error = PMString(kGroupCommandFailed) ;
		}
		else
			groupItem = groupCmd->GetItemListReference().GetRef(0); 

	} while(false) ;

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;

}


//Lock Or Unlock a page item
ErrorCode FLBItemManager::LockPageItemCmd(UIDRef itemToSet, bool8 toLock, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		// get the ILockPosition of the item to lock or unlock in order to verify its status
		InterfacePtr<ILockPosition> lockPosition(itemToSet,IID_ILOCKPOSITION);
		if(lockPosition == nil)
		{
			ASSERT_FAIL("FLBItemManager::LockPageItemCmd -> lockPosition nil");
			break;
		}

		// Create a SetLockPositionCmd:
		InterfacePtr<ICommand> setLPCmd(CmdUtils::CreateCommand(kSetLockPositionCmdBoss));
		if(setLPCmd == nil)
		{
			ASSERT_FAIL("FLBItemManager::LockPageItemCmd -> setLPCmd nil");
			break;
		}

		// Set the SetLockPositionCmd's ItemList:
		setLPCmd->SetItemList(UIDList(itemToSet));

			// Set command undoability
		if(!allowUndo)
			setLPCmd->SetUndoability(ICommand::kAutoUndo);
		
		// Get an IIntData Interface for the SetLockPositionCmd:
		InterfacePtr<IIntData> intData(setLPCmd, IID_IINTDATA);
		if(intData == nil)
		{
			ASSERT_FAIL("FLBItemManager::LockPageItem -> intData nil");
			break;
		}

		// Set the IIntData Interface's data:
		if (toLock)
			intData->Set(ILockPosition::kLock);
		else
			intData->Set(ILockPosition::kUnlock);

		// Process the SetLockPositionCmd:
		status = CmdUtils::ProcessCommand(setLPCmd);
		if (status != kSuccess)
			error = PMString(kLockCmdFailed) ;

	}while(false) ;

	if (status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


ErrorCode FLBItemManager::DeletePageItemsCmd(UIDList itemsToDelete, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kFlbNilInterface);

	do
	{
		// Create a DeleteCmd:
		InterfacePtr<ICommand> deleteCmd(CmdUtils::CreateCommand(kDeleteCmdBoss));
		if(deleteCmd == nil)
		{
			ASSERT_FAIL("FLBItemManager::DeletePageItemsCmd -> groupCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			deleteCmd->SetUndoability(ICommand::kAutoUndo);

		// Set the DeleteCmd's ItemList

        if(itemsToDelete ==nil)
            break;

        if(itemsToDelete.Length() ==0)
            break;

   
		deleteCmd->SetItemList(itemsToDelete);

		// Process the DeleteCmd: 
		status = CmdUtils::ProcessCommand(deleteCmd);
		if (status != kSuccess)
			error = PMString("kDeleteCommandFailed") ;

	} while(false) ;

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;

}



/* GetOwnerSpreadLayer
*/
UID FLBItemManager::GetOwnerSpreadLayer(IDocument * doc, int32 numPage, IDocumentLayer * docLayer)
{
	UID ownerSpreadLayer = kInvalidUID;
	PMString error(kFlbNilInterface);

	do
	{
		// Looking for the uid of the page where the new frame should be added
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		if(pageList == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordAbs -> pageList nil");
			error = PMString(kFlbNilParameter);
			break;
		}

		UID pageUID = pageList->GetNthPageUID(numPage - 1);

		// Convert all the point in the list (considered as in described in the 
		// page coordinates system) in pasteboard coordinates
		InterfacePtr<IGeometry> pageGeo (::GetDataBase(doc),pageUID,UseDefaultIID());
		if(pageGeo == nil)
		{
			error = PMString(kErrNoSuchPage);
			break;
		}

		// Looking for the spread layer that will own the new page item built with the points
		InterfacePtr<IHierarchy> pageHier(pageGeo,UseDefaultIID());
		if(pageHier == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordAbs -> pageHier nil");
			break;
		}

		InterfacePtr<ISpread> iSpread(::GetDataBase(doc),pageHier->GetSpreadUID(),UseDefaultIID());
		if(iSpread == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordAbs -> iSpread nil");
			break;
		}

		InterfacePtr<ISpreadLayer> iSpreadLayer (iSpread->QueryLayer(docLayer));
		if(iSpreadLayer == nil)
		{
			ASSERT_FAIL("FLBItemManager::GetPBCoordAbs -> iSpreadLayer nil");
			break;
		}

		ownerSpreadLayer = ::GetUID(iSpreadLayer);

	} while(false);

	if(ownerSpreadLayer == kInvalidUID)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return ownerSpreadLayer;
}

void FLBItemManager::SetMasterInCreation(UIDRef masterSpread)
{
	masterInCreation = masterSpread;
}

void FLBItemManager::EndMasterCreation()
{
	masterInCreation = kInvalidUIDRef;
}

UID FLBItemManager::ResolveRGBColorSwatch (const PMString swatchName, const PMReal rRed, const PMReal rGreen, const PMReal rBlue){
	
	 UID retval = kInvalidUID;
     do{
		 // Get active swatch list. If there is no open document, we will get one from application workspace.
         InterfacePtr<ISwatchList> swatchList(Utils<ISwatchUtils> ()->QueryActiveSwatchList());
     	
         //If the swatch already exists, just return it
      	 UID colorUID = swatchList->FindSwatch(swatchName).GetUID();
		 if(colorUID != kInvalidUID){
			 retval = colorUID;		
			 break;
		 }         

		 // Init color array
		 ColorArray colorArray;
	     colorArray.push_back(rRed);
	     colorArray.push_back(rGreen);
	     colorArray.push_back(rBlue);

		 // Create a temporary gradient object.
		 InterfacePtr<IRenderingObject> renderingObject(Utils<IGraphicStateUtils>()->CreateTemporaryRenderObject(kPMColorBoss));
		 ASSERT(renderingObject);
		 if(!renderingObject) 
			 break;
		 
		 renderingObject->SetVisibility(kTrue);
		 renderingObject->SetSwatchName(swatchName);
		 renderingObject->SetCanDelete(kTrue);
		 //renderingObject->SetCreatorID(kPantoneCoatedNewLibCreatorID);
		 
		 InterfacePtr<IColorData> colorData( renderingObject, UseDefaultIID());
		 ASSERT(colorData);
		 if(!colorData)	
            break;		 

		 colorData->SetColorData(kPMCsCalRGB, colorArray);
		 
		 InterfacePtr<IInkData> newInkData(renderingObject, UseDefaultIID()); 
		 ASSERT(newInkData);
         if (newInkData)
             newInkData->SetInkType(IInkData::kSpotInk);

		 Utils<ISwatchUtils> swatchUtils;
		 InterfacePtr<IWorkspace> workspacePtr (swatchList, IID_IWORKSPACE );
		 if (swatchUtils)
		   retval = swatchUtils->CreateNewSwatch (kPMColorBoss, renderingObject, workspacePtr, kFalse); 
        
     }while(kFalse);
     return retval;
}

