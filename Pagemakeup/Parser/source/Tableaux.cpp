/*
//	File:	Tableaux.cpp
//
//	Date:	03-Mar-2006
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"
#include "TransformUtils.h"
#include "PMLineSeg.h"

// Interface includes
#include "IDocument.h"
#include "ITableUtils.h"
#include "IFrameUtils.h"
#include "IMultiColumnTextFrame.h"
#include "IGeometry.h"
#include "ITextModel.h"
#include "IHierarchy.h"
#include "ITableModel.h"
#include "ITableCommands.h"
#include "ITableAttrInt32.h"
#include "ITableAttrBool16.h"
#include "ITableAttrRealNumber.h"
#include "ICellStrokeAttrData.h"
#include "ITableLineType.h"
#include "ISwatchUtils.h"
#include "ITableAttrUID.h"
#include "ITextStoryThreadDict.h"
#include "ITableTextContainer.h"
#include "ITextStoryThread.h"
#include "TextIterator.h"
#include "ITextParcelList.h"
#include "IWaxStrand.h"
#include "ITableLayout.h"
#include "ITableAttributes.h"
#include "ITableAttrAccessor.h"
#include "IItemStrand.h"
#include "IDataLinkReference.h"
#include "IDataLink.h"
#include "IPageItemTypeUtils.h"
#include "IFrameType.h"

#include "ITableFrame.h"
#include "IPathUtils.h"
#include "ITextUtils.h"
#include "IWaxIterator.h"
#include "IWaxLine.h"

// Project includes
#include "Tableaux.h"
#include "GlobalDefs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "ImageID.h"
#include "TCLError.h"
#include "Utils.h"

/* Constructor
*/
Func_AT::Func_AT(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_AT::Func_AT -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		
		tableName = reader->ExtractString(255);
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);
		nbBodyRows = reader->ExtractInt(1,1,10000);
		nbCol = reader->ExtractInt(1,1,200);
		nbHeaderRows = reader->ExtractInt(0,0,25);
		nbFooterRows = reader->ExtractInt(0,0,25);
		tableWidth = reader->ExtractReel('D','+',0,0);
		tableHeight = reader->ExtractReel('D','+',0,0);		
		repeatHeader = reader->ExtractInt(0,0,2); // values from namespace Tables :  eStartEachTextColumn = 0, eStartEachTextFrame = 1, eStartEachPage = 2 
		skipFirst = reader->ExtractBooleen(kFalse);
		repeatFooter = reader->ExtractInt(0,0,2);
		skipLast = reader->ExtractBooleen(kFalse);

		spaceBefore = reader->ExtractReel('D','+',4,0);	
		spaceAfter = reader->ExtractReel('D','-',4,0);

		strokeWeight = reader->ExtractReel('D','+',1,0);
		strokeType = reader->ExtractInt(0,0,18);
		strokeColor = reader->ExtractInt(5,0,MAX_COLOR);
		strokeTint = reader->ExtractPourcent('+',100,0)*100.0;
		strokeOverprint = reader->ExtractBooleen(kFalse);
		gapColor = reader->ExtractInt(9,0,MAX_COLOR);
		gapTint = reader->ExtractPourcent('+',100,0)*100.0;
		gapOverprint = reader->ExtractBooleen(kFalse);

		drawingOrder = reader->ExtractInt(0,0,3); // eBestJoins = 0, eRowOnTop = 1, eColumnOnTop = 2, eInDesign2Compatibility = 3

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
		IDCall_AT(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_AT
*/
void Func_AT::IDCall_AT(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");
	UID newTable;

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
		
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		// Check whether the receiver is text frame
		InterfacePtr<IHierarchy> refBlocHier (db, parser->getItem(refBloc), UseDefaultIID());
		if(refBlocHier == nil)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		if(!Utils<IFrameUtils>()->IsTextFrame(refBlocHier))
		{
			error = PMString(kErrTextBoxRequired);
			break;
		}

		// Get the story the new table will anchored in (at the end)
		InterfacePtr<IMultiColumnTextFrame> txtFrame (db, refBlocHier->GetChildUID(0), UseDefaultIID());
		InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());

		TextIndex insertPos = txtModel->GetPrimaryStoryThreadSpan()-1;

		Utils<ITableUtils> tableUtils;
		if(!tableUtils->CanInsertTableAt(txtModel,insertPos))
		{
			error = PMString(kErrTableInsertionNotPossible);
			break;
		}

		int32 numRows = nbHeaderRows + nbFooterRows + nbBodyRows;
		PMReal rowHeight = tableHeight/numRows;
		PMReal colWidth = tableWidth/nbCol;

		tableUtils->InsertTable(txtModel, insertPos, 0, numRows, nbCol, nbHeaderRows, nbFooterRows,
								 rowHeight , colWidth, kTextContentType, ITableUtils::eNoSelection );

		
		UIDRef newTableRef = tableUtils->GetTableModel(txtModel, insertPos);
	
		newTable = newTableRef.GetUID();

		boost::shared_ptr<AttributeBossList> attrList (new AttributeBossList);

		// Header and Footer attributes
		InterfacePtr<ITableAttrInt32> startHeadersAttr (::CreateObject2<ITableAttrInt32>(kStartHeadersOnAttrBoss));
		startHeadersAttr->Set(repeatHeader);
		attrList->ApplyAttribute(startHeadersAttr);

		InterfacePtr<ITableAttrInt32> startFootersAttr (::CreateObject2<ITableAttrInt32>(kStartFootersOnAttrBoss));
		startFootersAttr->Set(repeatFooter);
		attrList->ApplyAttribute(startFootersAttr);

		InterfacePtr<ITableAttrBool16> skipFirstAttr (::CreateObject2<ITableAttrBool16>(kSkipFirstHeaderAttrBoss ));		
		skipFirstAttr->Set(skipFirst);
		attrList->ApplyAttribute(skipFirstAttr);

		InterfacePtr<ITableAttrBool16> skipLastAttr (::CreateObject2<ITableAttrBool16>(kSkipLastFooterAttrBoss ));
		skipLastAttr->Set(skipLast);
		attrList->ApplyAttribute(skipLastAttr);

		// Spacing options
		InterfacePtr<ITableAttrRealNumber> spaceAfterAttr (::CreateObject2<ITableAttrRealNumber>(kTableAttrAfterSpaceBoss));
		spaceAfterAttr->Set(spaceAfter);
		attrList->ApplyAttribute(spaceAfterAttr);

		InterfacePtr<ITableAttrRealNumber> spaceBeforeAttr (::CreateObject2<ITableAttrRealNumber>(kTableAttrBeforeSpaceBoss));
		spaceBeforeAttr->Set(spaceBefore);
		attrList->ApplyAttribute(spaceBeforeAttr);

		// Stroke Drawing order
		InterfacePtr<ITableAttrInt32> orderAttr (::CreateObject2<ITableAttrInt32>(kTableAttrStrokeOrderBoss));		
		orderAttr->Set(drawingOrder);
		attrList->ApplyAttribute(orderAttr);

		InterfacePtr<ITableCommands> tableCommands (newTableRef, UseDefaultIID());
		status = tableCommands->ApplyTableOverrides(attrList.get());


		// Border options
		ClassID strokeColorSides[4] = { kTableAttrOuterTopStrokeColorBoss, kTableAttrOuterLeftStrokeColorBoss, kTableAttrOuterBottomStrokeColorBoss , kTableAttrOuterRightStrokeColorBoss };
		ClassID strokeTypeSides[4] = { kTableAttrOuterTopStrokeTypeBoss, kTableAttrOuterLeftStrokeTypeBoss, kTableAttrOuterBottomStrokeTypeBoss, kTableAttrOuterRightStrokeTypeBoss };
		ClassID strokeTintSides[4] = { kTableAttrOuterTopStrokeTintBoss, kTableAttrOuterLeftStrokeTintBoss, kTableAttrOuterBottomStrokeTintBoss, kTableAttrOuterRightStrokeTintBoss };
		ClassID strokeOverprintSides[4] = { kTableAttrOuterTopStrokeOverprintBoss, kTableAttrOuterLeftStrokeOverprintBoss, kTableAttrOuterBottomStrokeOverprintBoss, kTableAttrOuterRightStrokeOverprintBoss };
		ClassID strokeWeightSides[4] = { kTableAttrOuterTopStrokeWeightBoss, kTableAttrOuterLeftStrokeWeightBoss, kTableAttrOuterBottomStrokeWeightBoss, kTableAttrOuterRightStrokeWeightBoss };
		ClassID gapColorSides[4] = { kTableAttrOuterTopStrokeGapColorBoss, kTableAttrOuterLeftStrokeGapColorBoss, kTableAttrOuterBottomStrokeGapColorBoss, kTableAttrOuterRightStrokeGapColorBoss };
		ClassID gapTintSides[4] = { kTableAttrOuterTopStrokeGapTintBoss, kTableAttrOuterLeftStrokeGapTintBoss, kTableAttrOuterBottomStrokeGapTintBoss, kTableAttrOuterRightStrokeGapTintBoss };
		ClassID gapOverprintSides[4] = { kTableAttrOuterTopStrokeGapOverprintBoss, kTableAttrOuterLeftStrokeGapOverprintBoss, kTableAttrOuterBottomStrokeGapOverprintBoss, kTableAttrOuterRightStrokeGapOverprintBoss };
		
		UID colorUID = parser->getColorList(strokeColor).colorUID;
		if(colorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			colorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
		}

		UID gapColorUID = parser->getColorList(gapColor).colorUID;
		if(gapColorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			gapColorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
		}

		ITableLineType::SType lineType = Func_OS::GetSType(strokeType);

		attrList->ClearAllOverrides();

		for(int32 i = 0 ; i < 4 ; ++i)
		{
			InterfacePtr<ITableAttrUID> strokeColorAttr (::CreateObject2<ITableAttrUID>(strokeColorSides[i]));
			strokeColorAttr->Set(colorUID);
			attrList->ApplyAttribute(strokeColorAttr);

			InterfacePtr<ITableAttrRealNumber> strokeWeightAttr (::CreateObject2<ITableAttrRealNumber>(strokeWeightSides[i]));
			strokeWeightAttr->Set(strokeWeight);
			attrList->ApplyAttribute(strokeWeightAttr);

			InterfacePtr<ITableLineType> strokeTypeAttr (::CreateObject2<ITableLineType>(strokeTypeSides[i]));
			strokeTypeAttr->Set(lineType);
			attrList->ApplyAttribute(strokeTypeAttr);

			InterfacePtr<ITableAttrBool16> strokeOverprintAttr (::CreateObject2<ITableAttrBool16>(strokeOverprintSides[i]));		
			strokeOverprintAttr->Set(strokeOverprint);
			attrList->ApplyAttribute(strokeOverprintAttr);

			InterfacePtr<ITableAttrRealNumber> strokeTintAttr (::CreateObject2<ITableAttrRealNumber>(strokeTintSides[i]));
			strokeTintAttr->Set(strokeTint);
			attrList->ApplyAttribute(strokeTintAttr);

			InterfacePtr<ITableAttrUID> gapColorAttr (::CreateObject2<ITableAttrUID>(gapColorSides[i]));
			gapColorAttr->Set(gapColorUID);
			attrList->ApplyAttribute(gapColorAttr);

			InterfacePtr<ITableAttrRealNumber> gapTintAttr (::CreateObject2<ITableAttrRealNumber>(gapTintSides[i]));
			gapTintAttr->Set(gapTint);
			attrList->ApplyAttribute(gapTintAttr);

			InterfacePtr<ITableAttrBool16> gapOverprintAttr (::CreateObject2<ITableAttrBool16>(gapOverprintSides[i]));		
			gapOverprintAttr->Set(gapOverprint);
			attrList->ApplyAttribute(gapOverprintAttr);
		}

		status = tableCommands->ApplyTableOverrides(attrList.get());		
				
	} while(kFalse);

	if (status == kSuccess)
	{
		parser->setTable(tableName,newTable);
		parser->setCurTable(tableName);
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_CT::Func_CT(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CT::Func_CT -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		tableName = reader->ExtractString(255);
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
		parser->setCurTable(tableName);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Constructor
*/
Func_SC::Func_SC(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SC::Func_SC -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		startCol = reader->ExtractInt(0,0,10000);
		startRow = reader->ExtractInt(0,0,10000);
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
	{
		endRow = startRow;
		endCol = startCol;
		IDCall_SC(parser);
	}
	else
	{
		endCol = reader->ExtractInt(1,1,10000);
		endRow = reader->ExtractInt(1,1,10000);		
		if(reader->IsEndOfCommand())
			IDCall_SC(parser);
		else
			throw TCLError(PMString(kErrNoEndOfCommand));
	}
}

/* IDCall_SC
*/
void Func_SC::IDCall_SC(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");
	GridArea newSel = kZeroGridArea;
	do
	{
		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		InterfacePtr<ITableModel> curTableModel(parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());

		parser->setCurCells(curTableModel, curTableModel->GetBodyArea(), startRow, startCol, endRow, endCol);

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);	
}

/* Constructor
*/
Func_SH::Func_SH(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SH::Func_SH -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		startCol = reader->ExtractInt(0,0,10000);
		startRow = reader->ExtractInt(0,0,10000);
		selType = reader->ExtractInt(0,0,1); // Header -> 0, Footer -> 1
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
	{
		endRow = startRow;
		endCol = startCol;
		IDCall_SH(parser);
	}
	else
	{
		endCol = reader->ExtractInt(1,1,10000);
		endRow = reader->ExtractInt(1,1,10000);		
		if(reader->IsEndOfCommand())
			IDCall_SH(parser);
		else
			throw TCLError(PMString(kErrNoEndOfCommand));
	}
}

/* IDCall_SH
*/
void Func_SH::IDCall_SH(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");
	GridArea newSel = kZeroGridArea;
	do
	{
		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		InterfacePtr<ITableModel> curTableModel(parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());

		if(selType == 0)
			parser->setCurCells(curTableModel, curTableModel->GetHeaderArea(), startRow, startCol, endRow, endCol);
		else
			parser->setCurCells(curTableModel, curTableModel->GetFooterArea(), startRow, startCol, endRow, endCol);		

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);	
}

/* Constructor
*/
Func_MC::Func_MC(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_MC::Func_MC -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		if(reader->TestEgal())
			merge = reader->ExtractBooleen(kFalse);
		else
			merge = kTrue;
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
		IDCall_MC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_MC
*/
void Func_MC::IDCall_MC(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		if(parser->getCurCells() == kZeroGridArea)
		{
			error = PMString(kErrNoCurrentCells);
			break;
		}

		InterfacePtr<ITableCommands> curTableModel (parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());

		if(merge)
			status = curTableModel->MergeCells(parser->getCurCells());
		else
			status = curTableModel->UnmergeCell(parser->getCurCells().GetTopLeft());

	} while(kFalse);

	if (status == kSuccess)
	{
		// The merged cell becomes the current selection
		GridAddress anchor = parser->getCurCells().GetTopLeft();
		parser->setCurCells(GridArea(anchor, GridAddress(anchor.row+1,anchor.col+1)));
	}		
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_CPlus::Func_CPlus(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CPlus::Func_CPlus -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		if(reader->TestEgal())
		{		
			col = reader->ExtractInt(0,0,10000);
			pos = reader->ExtractInt(0,0,1);
			nb = reader->ExtractInt(1,1,10000);
			colWidth = reader->ExtractReel('D','+',0,0);
		}
		else
		{
			col = 0;
			pos = 1;
			nb = 1;
			colWidth = 0.0;
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
		IDCall_CPlus(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_CPlus
*/
void Func_CPlus::IDCall_CPlus(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		InterfacePtr<ITableCommands> curTableModel (parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());

		// Determine column index
		if(col == 0)
		{
			InterfacePtr<ITableModel> tableModel (curTableModel, UseDefaultIID());
			col = tableModel->GetTotalCols().count-1;
		}
		else
			--col;

		Tables::ERelativePosition relPos = Tables::eBefore;
		if(pos == 1)
			relPos = Tables::eAfter;
		
		status = curTableModel->InsertColumns(ColRange(col,nb), relPos, colWidth);
		

	} while(kFalse);

	if (status == kSuccess)
	{
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_RPlus::Func_RPlus(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_RPlus::Func_RPlus -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		if(reader->TestEgal())
		{
			pos = reader->ExtractInt(0,0,1);
			nb = reader->ExtractInt(1,1,10000);
			rowHeight = reader->ExtractReel('D','+',0,0);
		}
		else
		{
			nb = 1;
			pos = 1;
			rowHeight = 0.0;			
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
		IDCall_RPlus(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_RPlus
*/
void Func_RPlus::IDCall_RPlus(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");
	ColRange colRange;
	int32 rowIndex = 0;

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		if(parser->getCurCells() == kZeroGridArea)
		{
			error = PMString(kErrNoCurrentCells);
			break;
		}

		InterfacePtr<ITableCommands> curTableModel (parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());
		
		InterfacePtr<ITableModel> tableModel (curTableModel, UseDefaultIID());
		colRange = tableModel->GetTotalCols(); 

		// Determine row index from current selection
		rowIndex = parser->getCurCells().bottomRow - 1;

		Tables::ERelativePosition relPos = Tables::eBefore;
		if(pos == 1)
			relPos = Tables::eAfter;

		status = curTableModel->InsertRows(RowRange(rowIndex,nb), relPos, rowHeight);

	} while(kFalse);

	if (status == kSuccess)
	{
		// Set newly inserted rows as the current selection
		parser->setCurCells(GridArea(RowRange(rowIndex+1,nb), colRange));
	}
	else
		throw TCLError(error);
}


/* Constructor
*/
Func_CO::Func_CO(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CO::Func_CO -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		col = reader->ExtractInt(1,1,10000);
		colWidth = reader->ExtractReel('D','+',0,0);
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
		IDCall_CO(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_CO
*/
void Func_CO::IDCall_CO(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		// Decrement column index (0-based)
		--col;

		InterfacePtr<ITableCommands> curTableModel (parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());
		status = curTableModel->ResizeCols(ColRange(col,1), colWidth);
		

	} while(kFalse);

	if (status == kSuccess)
	{
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_OT::Func_OT(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_OT::Func_OT -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		topInset = reader->ExtractReel('D','+',0,0);
		leftInset = reader->ExtractReel('D','+',0,0);
		bottomInset = reader->ExtractReel('D','+',0,0);
		rightInset = reader->ExtractReel('D','+',0,0);
		vj = reader->ExtractInt(0,0,3); // Top = 0, Center = 1, Bottom = 2, Justify = 3
		maxVJSpace = reader->ExtractReel('D','+',0,0);
		FLOMetric = reader->ExtractInt(0,0,4); // Ascent = 0, xHeight = 1, Cap height = 2, Leading = 3, Fixed = 4 
		minFLO = reader->ExtractReel('D','+',0,0);
		rotation = reader->ExtractInt(0,0,359);
		clip = reader->ExtractBooleen(kFalse);
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
		IDCall_OT(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_OT
*/
void Func_OT::IDCall_OT(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		if(parser->getCurCells() == kZeroGridArea)
		{
			error = PMString(kErrNoCurrentCells);
			break;
		}
 
		boost::shared_ptr<AttributeBossList> attrList (new AttributeBossList);

		// Insets
		InterfacePtr<ITableAttrRealNumber> topInsetAttr (::CreateObject2<ITableAttrRealNumber>(kCellAttrTopInsetBoss));
		topInsetAttr->Set(topInset);
		attrList->ApplyAttribute(topInsetAttr);

		InterfacePtr<ITableAttrRealNumber> leftInsetAttr (::CreateObject2<ITableAttrRealNumber>(kCellAttrLeftInsetBoss));
		leftInsetAttr->Set(leftInset);
		attrList->ApplyAttribute(leftInsetAttr);

		InterfacePtr<ITableAttrRealNumber> bottomInsetAttr (::CreateObject2<ITableAttrRealNumber>(kCellAttrBottomInsetBoss));
		bottomInsetAttr->Set(bottomInset);
		attrList->ApplyAttribute(bottomInsetAttr);

		InterfacePtr<ITableAttrRealNumber> rightInsetAttr (::CreateObject2<ITableAttrRealNumber>(kCellAttrRightInsetBoss));
		rightInsetAttr->Set(rightInset);
		attrList->ApplyAttribute(rightInsetAttr);		

		// Vertical justification
		InterfacePtr<ITableAttrInt32> vjAttr (::CreateObject2<ITableAttrInt32>(kTextCellAttrVerticalJustificationBoss));
		vjAttr->Set(vj);
		attrList->ApplyAttribute(vjAttr);

		InterfacePtr<ITableAttrRealNumber> maxVJSpaceAttr (::CreateObject2<ITableAttrRealNumber>(kTextCellAttrMaxVJInterParaSpaceBoss));
		maxVJSpaceAttr->Set(maxVJSpace);
		attrList->ApplyAttribute(maxVJSpaceAttr);
		
		// First baseline
		Text::FirstLineOffsetMetric FLOm;
		switch (FLOMetric)
		{
			case 0 : FLOm = Text::kFLOAscent; break;
			case 1 : FLOm = Text::kFLOxHeight; break;
			case 2 : FLOm = Text::kFLOCapHeight; break;
			case 3 : FLOm = Text::kFLOLeading; break;
			case 4 : FLOm = Text::kFLOFixedHeight; break;
			default : FLOm = Text::kFLOInvalid; break;
		}

		InterfacePtr<ITableAttrInt32> FLOffsetMetricAttr (::CreateObject2<ITableAttrInt32>(kTextCellAttrFirstLineOffsetMetricBoss));
		FLOffsetMetricAttr->Set(FLOm);
		attrList->ApplyAttribute(FLOffsetMetricAttr);

		InterfacePtr<ITableAttrRealNumber> minFLOffsetAttr (::CreateObject2<ITableAttrRealNumber>(kTextCellAttrMinFirstLineOffsetBoss));
		minFLOffsetAttr->Set(minFLO);
		attrList->ApplyAttribute(minFLOffsetAttr);

		// Clipping
		InterfacePtr<ITableAttrBool16> clipAttr (::CreateObject2<ITableAttrBool16>(kCellAttrClipBoss));
		clipAttr->Set(clip);
		attrList->ApplyAttribute(clipAttr);		

		// Text rotation		
		InterfacePtr<ITableAttrRealNumber> rotationAttr (::CreateObject2<ITableAttrRealNumber>(kCellAttrRotationBoss));
		rotationAttr->Set(rotation);
		attrList->ApplyAttribute(rotationAttr);

		InterfacePtr<ITableCommands> curTableModel (parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());
		status = curTableModel->ApplyCellOverrides(parser->getCurCells(), attrList.get());		

	} while(kFalse);

	if (status == kSuccess)
	{
	}
	else
		throw TCLError(error);
}


/* Constructor
*/
Func_OS::Func_OS(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_OS::Func_OS -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		sides = reader->ExtractInt(0,0,63); // eNoSides = 0, eLeftSide = 1,  eRightSide = 2,  eTopSide = 4, eBottomSide = 8, eInteriorRows = 16, eInteriorCols = 32
		strokeWeight = reader->ExtractReel('D','+',0,0);
		strokeType = reader->ExtractInt(0,0,18);
		strokeColor = reader->ExtractInt(0,0,MAX_COLOR);
		strokeTint = reader->ExtractPourcent('+',100,0)*100.0;
		strokeOverprint = reader->ExtractBooleen(kFalse);
		gapColor = reader->ExtractInt(9,0,MAX_COLOR);
		gapTint = reader->ExtractPourcent('+',100,0)*100.0;
		gapOverprint = reader->ExtractBooleen(kFalse);

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
		IDCall_OS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_OS
*/
void Func_OS::IDCall_OS(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		if(parser->getCurCells() == kZeroGridArea)
		{
			error = PMString(kErrNoCurrentCells);
			break;
		}

		UID colorUID = parser->getColorList(strokeColor).colorUID;
		if(colorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			colorUID = Utils<ISwatchUtils>()->GetBlackSwatchUID(db); // color doesn't exist, replaced with black
		}

		ICellStrokeAttrData::Data strokeData;
		strokeData.attrs.Set(ICellStrokeAttrData::eWeight);
		strokeData.attrs.Add(ICellStrokeAttrData::eColor);
		strokeData.attrs.Add(ICellStrokeAttrData::eTint);
		strokeData.attrs.Add(ICellStrokeAttrData::eOverprint);
		strokeData.attrs.Add(ICellStrokeAttrData::eLineType);
		strokeData.attrs.Add(ICellStrokeAttrData::eGapColor);
		strokeData.attrs.Add(ICellStrokeAttrData::eGapTint);
		strokeData.attrs.Add(ICellStrokeAttrData::eGapOverprint);

		strokeData.sides = (Tables::ESelectionSides) sides;
		strokeData.color = colorUID;
		strokeData.weight = strokeWeight;
		strokeData.tintPercent = strokeTint;
		strokeData.overprint = strokeOverprint;
		strokeData.lineType = this->GetSType(strokeType);

		UID gapColorUID = parser->getColorList(gapColor).colorUID;
		if(gapColorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			gapColorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with black
		}

		strokeData.gapColor = gapColorUID;
		strokeData.gapTintPercent = gapTint;
		strokeData.gapOverprint = gapOverprint;

		InterfacePtr<ITableCommands> curTableModel (parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());
		status = curTableModel->ApplyCellStrokes(parser->getCurCells(),strokeData);		

	} while(kFalse);

	if (status == kSuccess)
	{
	}
	else
		throw TCLError(error);
}

ITableLineType::SType Func_OS::GetSType(int32 strokeType)
{
	ITableLineType::SType  lineType;
	switch(strokeType)
	{
		case 0 : lineType = ITableLineType::SType(kSolidPathStrokerBoss, kInvalidUID); break;
		case 1 : lineType = ITableLineType::SType(kThickThickPathStrokerBoss, kInvalidUID); break;
		case 2 : lineType = ITableLineType::SType(kThickThinPathStrokerBoss, kInvalidUID); break;
		case 3 : lineType = ITableLineType::SType(kThickThinThickPathStrokerBoss, kInvalidUID); break;
		
		case 4 : lineType = ITableLineType::SType(kThinThickPathStrokerBoss, kInvalidUID); break;
		case 5 : lineType = ITableLineType::SType(kThinThickThinPathStrokerBoss, kInvalidUID); break;
		case 6 : lineType = ITableLineType::SType(kThinThinPathStrokerBoss, kInvalidUID); break;

		case 7 : lineType = ITableLineType::SType(kTriplePathStrokerBoss, kInvalidUID); break;

		case 8 : lineType = ITableLineType::SType(kCannedDash3x2PathStrokerBoss, kInvalidUID); break;
		case 9 : lineType = ITableLineType::SType(kCannedDash4x4PathStrokerBoss, kInvalidUID); break;
		
		case 10 : lineType = ITableLineType::SType(kLeftSlantHashPathStrokerBoss , kInvalidUID); break;
		case 11 : lineType = ITableLineType::SType(kRightSlantHashPathStrokerBoss , kInvalidUID); break;
		case 12 : lineType = ITableLineType::SType(kStraightHashPathStrokerBoss, kInvalidUID); break;


		case 13 : lineType = ITableLineType::SType(kCannedDotPathStrokerBoss, kInvalidUID); break;
		case 14 : lineType = ITableLineType::SType(kSingleWavyPathStrokerBoss, kInvalidUID); break;
		case 15 : lineType = ITableLineType::SType(kWhiteDiamondPathStrokerBoss, kInvalidUID); break;
		case 16 : lineType = ITableLineType::SType(kJapaneseDotsPathStrokerBoss , kInvalidUID); break;				

		case 17 : lineType = ITableLineType::SType(kDashedPathStrokerBoss, kInvalidUID); break;
		case 18 : lineType = ITableLineType::SType(kNonePathStrokerBoss, kInvalidUID); break;
					
		default : lineType = ITableLineType::SType(kSolidPathStrokerBoss, kInvalidUID); break;
	}

	return lineType;
}

/* Constructor
*/
Func_OF::Func_OF(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_OF::Func_OF-> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		fillColor = reader->ExtractInt(9,0,MAX_COLOR);
		fillTint = reader->ExtractPourcent('+',100,0)*100.0;
		fillOverprint = reader->ExtractBooleen(kFalse);
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
		IDCall_OF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_OF
*/
void Func_OF::IDCall_OF(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		if(parser->getCurCells() == kZeroGridArea)
		{
			error = PMString(kErrNoCurrentCells);
			break;
		}

		UID colorUID = parser->getColorList(fillColor).colorUID;
		if(colorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			colorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with black
		}

		boost::shared_ptr<AttributeBossList> attrList (new AttributeBossList);

		InterfacePtr<ITableAttrUID> fillColorAttr (::CreateObject2<ITableAttrUID>(kCellAttrFillColorBoss));
		fillColorAttr->Set(colorUID);
		attrList->ApplyAttribute(fillColorAttr);

		InterfacePtr<ITableAttrBool16> clipAttr (::CreateObject2<ITableAttrBool16>(kCellAttrFillOverprintBoss));
		clipAttr->Set(fillOverprint);
		attrList->ApplyAttribute(clipAttr);		
	
		InterfacePtr<ITableAttrRealNumber> fillTintAttr (::CreateObject2<ITableAttrRealNumber>(kCellAttrFillTintBoss));
		fillTintAttr->Set(fillTint);
		attrList->ApplyAttribute(fillTintAttr);

		InterfacePtr<ITableCommands> curTableModel (parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());
		status = curTableModel->ApplyCellOverrides(parser->getCurCells(), attrList.get());		

	} while(kFalse);

	if (status == kSuccess)
	{
	}
	else
		throw TCLError(error);
}


/* Constructor
*/
Func_RO::Func_RO(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_RO::Func_RO -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		rowHeight = reader->ExtractReel('D','+',0,0);
		autoGrow = reader->ExtractBooleen(TRUE); 
		maxHeight = reader->ExtractReel('D','+',600,0);
		startRowOn = reader->ExtractInt(0,0,5); // eStartAnywhere = 0, eStartNextColumn = 1, eStartNextFrame = 2, eStartNextPage = 3, eStartNextOddPage = 4, eStartNextEvenPage = 5 
		keepWithNext = reader->ExtractBooleen(kFalse);
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
		IDCall_RO(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_RO
*/
void Func_RO::IDCall_RO(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");
	int32 rowIndex = 0;

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		if(parser->getCurCells() == kZeroGridArea)
		{
			error = PMString(kErrNoCurrentCells);
			break;
		}

		boost::shared_ptr<AttributeBossList> attrList (new AttributeBossList);

		InterfacePtr<ITableAttrInt32> rowTypeAttr (::CreateObject2<ITableAttrInt32>(kRowAttrAutoGrowBoss));
		rowTypeAttr->Set(autoGrow);
		attrList->ApplyAttribute(rowTypeAttr);

		InterfacePtr<ITableAttrRealNumber> maxSizeAttr (::CreateObject2<ITableAttrRealNumber>(kRowAttrMaxRowSizeBoss));
		maxSizeAttr->Set(maxHeight);
		attrList->ApplyAttribute(maxSizeAttr);		
	
		InterfacePtr<ITableAttrRealNumber> minSizeAttr (::CreateObject2<ITableAttrRealNumber>(kRowAttrMinRowSizeBoss));
		minSizeAttr->Set(rowHeight);
		attrList->ApplyAttribute(minSizeAttr);

		InterfacePtr<ITableAttrRealNumber> rowHeightAttr (::CreateObject2<ITableAttrRealNumber>(kRowAttrHeightBoss));
		rowHeightAttr->Set(rowHeight);
		attrList->ApplyAttribute(rowHeightAttr);

		// Keep options
		InterfacePtr<ITableAttrInt32> keepAttr (::CreateObject2<ITableAttrInt32>(kRowAttrKeepsBoss));
		keepAttr->Set(keepWithNext);
		attrList->ApplyAttribute(keepAttr);

		InterfacePtr<ITableAttrInt32> startOnAttr (::CreateObject2<ITableAttrInt32>(kRowKeepsStartOnBoss));
		if(startRowOn > 0) ++startRowOn; // incremented to match enum Tables::ERowStartAtValue
		startOnAttr->Set(startRowOn);
		attrList->ApplyAttribute(startOnAttr);
		 

		InterfacePtr<ITableCommands> curTableModel (parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());
		status = curTableModel->ApplyRowOverrides(parser->getCurCells().GetRows(), attrList.get());
		

	} while(kFalse);

	if (status == kSuccess)
	{
	}
	else
		throw TCLError(error);
}


/* Constructor
*/
Func_SP::Func_SP(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SP::Func_SP -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		rowOrColumnPattern = reader->ExtractInt(0,0,1); // Row = 0, Column = 1
		firstCount = reader->ExtractInt(0,0,9999);		
		firstStrokeWeight = reader->ExtractReel('D','+',1,0);
		firstStrokeType = reader->ExtractInt(0,0,18);
		firstStrokeColor = reader->ExtractInt(5,0,MAX_COLOR);
		firstStrokeTint = reader->ExtractPourcent('+',100,0)*100.0;
		firstStrokeOverprint = reader->ExtractBooleen(kFalse);
		firstGapColor = reader->ExtractInt(9,0,MAX_COLOR);
		firstGapTint = reader->ExtractPourcent('+',100,0)*100.0;
		firstGapOverprint = reader->ExtractBooleen(kFalse);
		skipFirst = reader->ExtractInt(0,0,9999);
		secondCount = reader->ExtractInt(0,0,9999);		
		secondStrokeWeight = reader->ExtractReel('D','+',0,25);
		secondStrokeType = reader->ExtractInt(0,0,18);
		secondStrokeColor = reader->ExtractInt(5,0,MAX_COLOR);
		secondStrokeTint = reader->ExtractPourcent('+',100,0)*100.0;
		secondStrokeOverprint = reader->ExtractBooleen(kFalse);
		secondGapColor = reader->ExtractInt(9,0,MAX_COLOR);
		secondGapTint = reader->ExtractPourcent('+',100,0)*100.0;
		secondGapOverprint = reader->ExtractBooleen(kFalse);
		skipSecond = reader->ExtractInt(0,0,9999);
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
		IDCall_SP(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_SP
*/
void Func_SP::IDCall_SP(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");
	int32 rowIndex = 0;

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}
		
		UID firstColorUID = parser->getColorList(firstStrokeColor).colorUID;
		if(firstColorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			firstColorUID = Utils<ISwatchUtils>()->GetBlackSwatchUID(db); // color doesn't exist, replaced with black
		}
		
		UID secondColorUID = parser->getColorList(secondStrokeColor).colorUID;
		if(secondColorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			secondColorUID = Utils<ISwatchUtils>()->GetBlackSwatchUID(db); // color doesn't exist, replaced with black
		}

		UID firstGapColorUID = parser->getColorList(firstGapColor).colorUID;
		if(firstGapColorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			firstGapColorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
		}
		
		UID secondGapColorUID = parser->getColorList(secondGapColor).colorUID;
		if(secondGapColorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			secondGapColorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
		}

		ClassID firstCountBoss, firstWeightBoss, firstColorBoss, firstTypeBoss, firstTintBoss, firstOverprintBoss, firstGapColorBoss, firstGapTintBoss, firstGapOverprintBoss, firstSkipBoss,
				secondCountBoss, secondWeightBoss, secondColorBoss, secondTypeBoss, secondTintBoss, secondOverprintBoss, secondGapColorBoss, secondGapTintBoss, secondGapOverprintBoss, secondSkipBoss;
		
		if(!rowOrColumnPattern)
		{
			firstCountBoss = kTableAttrRowStrokePatternFirstCountBoss;
			firstWeightBoss = kTableAttrRowStrokePatternFirstWeightBoss;
			firstColorBoss = kTableAttrRowStrokePatternFirstColorBoss;
			firstTypeBoss = kTableAttrRowStrokePatternFirstTypeBoss;
			firstTintBoss = kTableAttrRowStrokePatternFirstTintBoss;
			firstOverprintBoss = kTableAttrRowStrokePatternFirstOverprintBoss;
			firstGapColorBoss = kTableAttrRowStrokePatternFirstGapColorBoss; 
			firstGapTintBoss = kTableAttrRowStrokePatternFirstGapTintBoss;
			firstGapOverprintBoss = kTableAttrRowStrokePatternFirstGapOverprintBoss;
			firstSkipBoss = kTableAttrRowStrokePatternStartValueBoss;
			secondCountBoss = kTableAttrRowStrokePatternSecondCountBoss;
			secondWeightBoss = kTableAttrRowStrokePatternSecondWeightBoss;
			secondColorBoss = kTableAttrRowStrokePatternSecondColorBoss;
			secondTypeBoss = kTableAttrRowStrokePatternSecondTypeBoss;
			secondTintBoss = kTableAttrRowStrokePatternSecondTintBoss;
			secondOverprintBoss = kTableAttrRowStrokePatternSecondOverprintBoss;
			secondGapColorBoss = kTableAttrRowStrokePatternSecondGapColorBoss;
			secondGapTintBoss = kTableAttrRowStrokePatternSecondGapTintBoss;
			secondGapOverprintBoss = kTableAttrRowStrokePatternSecondGapOverprintBoss;
			secondSkipBoss = kTableAttrRowStrokePatternEndValueBoss;
		}
		else
		{
			firstCountBoss = kTableAttrColStrokePatternFirstCountBoss;
			firstWeightBoss = kTableAttrColStrokePatternFirstWeightBoss;
			firstColorBoss = kTableAttrColStrokePatternFirstColorBoss;
			firstTypeBoss = kTableAttrColStrokePatternFirstTypeBoss;
			firstTintBoss = kTableAttrColStrokePatternFirstTintBoss;
			firstOverprintBoss = kTableAttrColStrokePatternFirstOverprintBoss;
			firstGapColorBoss = kTableAttrColStrokePatternFirstGapColorBoss; 
			firstGapTintBoss = kTableAttrColStrokePatternFirstGapTintBoss;
			firstGapOverprintBoss = kTableAttrColStrokePatternFirstGapOverprintBoss;
			firstSkipBoss = kTableAttrColStrokePatternStartValueBoss;
			secondCountBoss = kTableAttrColStrokePatternSecondCountBoss;
			secondWeightBoss = kTableAttrColStrokePatternSecondWeightBoss;
			secondColorBoss = kTableAttrColStrokePatternSecondColorBoss;
			secondTypeBoss = kTableAttrColStrokePatternSecondTypeBoss;
			secondTintBoss = kTableAttrColStrokePatternSecondTintBoss;
			secondOverprintBoss = kTableAttrColStrokePatternSecondOverprintBoss;
			secondGapColorBoss = kTableAttrColStrokePatternSecondGapColorBoss;
			secondGapTintBoss = kTableAttrColStrokePatternSecondGapTintBoss;
			secondGapOverprintBoss = kTableAttrColStrokePatternSecondGapOverprintBoss;
			secondSkipBoss = kTableAttrColStrokePatternEndValueBoss;
		}

		boost::shared_ptr<AttributeBossList> attrList (new AttributeBossList);

		InterfacePtr<ITableAttrInt32> firstCountAttr (::CreateObject2<ITableAttrInt32>(firstCountBoss));
		firstCountAttr->Set(firstCount);
		attrList->ApplyAttribute(firstCountAttr);

		InterfacePtr<ITableAttrRealNumber> firstWeightAttr (::CreateObject2<ITableAttrRealNumber>(firstWeightBoss));
		firstWeightAttr->Set(firstStrokeWeight);
		attrList->ApplyAttribute(firstWeightAttr);

		InterfacePtr<ITableAttrUID> firstColorAttr (::CreateObject2<ITableAttrUID>(firstColorBoss));
		firstColorAttr->Set(firstColorUID);
		attrList->ApplyAttribute(firstColorAttr);

		InterfacePtr<ITableLineType> firstTypeAttr (::CreateObject2<ITableLineType>(firstTypeBoss));
		firstTypeAttr->Set(Func_OS::GetSType(firstStrokeType));
		attrList->ApplyAttribute(firstTypeAttr);

		InterfacePtr<ITableAttrRealNumber> firstTintAttr (::CreateObject2<ITableAttrRealNumber>(firstTintBoss));
		firstTintAttr->Set(firstStrokeTint);
		attrList->ApplyAttribute(firstTintAttr);

		InterfacePtr<ITableAttrBool16> firstOverprintAttr (::CreateObject2<ITableAttrBool16>(firstOverprintBoss));
		firstOverprintAttr->Set(firstStrokeOverprint);
		attrList->ApplyAttribute(firstOverprintAttr);		
	
		InterfacePtr<ITableAttrUID> firstGapColorAttr (::CreateObject2<ITableAttrUID>(firstGapColorBoss));
		firstGapColorAttr->Set(firstGapColorUID);
		attrList->ApplyAttribute(firstGapColorAttr);

		InterfacePtr<ITableAttrRealNumber> firstGapTintAttr (::CreateObject2<ITableAttrRealNumber>(firstGapTintBoss));
		firstGapTintAttr->Set(firstGapTint);
		attrList->ApplyAttribute(firstGapTintAttr);

		InterfacePtr<ITableAttrBool16> firstGapOverprintAttr (::CreateObject2<ITableAttrBool16>(firstGapOverprintBoss));
		firstGapOverprintAttr->Set(firstGapOverprint);
		attrList->ApplyAttribute(firstGapOverprintAttr);

		InterfacePtr<ITableAttrInt32> firstSkipAttr (::CreateObject2<ITableAttrInt32>(firstSkipBoss));
		firstSkipAttr->Set(skipFirst);
		attrList->ApplyAttribute(firstSkipAttr);


		InterfacePtr<ITableAttrInt32> secondCountAttr (::CreateObject2<ITableAttrInt32>(secondCountBoss));
		secondCountAttr->Set(secondCount);
		attrList->ApplyAttribute(secondCountAttr);

		InterfacePtr<ITableAttrRealNumber> secondWeightAttr (::CreateObject2<ITableAttrRealNumber>(secondWeightBoss));
		secondWeightAttr->Set(secondStrokeWeight);
		attrList->ApplyAttribute(secondWeightAttr);

		InterfacePtr<ITableAttrUID> secondColorAttr (::CreateObject2<ITableAttrUID>(secondColorBoss));
		secondColorAttr->Set(secondColorUID);
		attrList->ApplyAttribute(secondColorAttr);

		InterfacePtr<ITableLineType> secondTypeAttr (::CreateObject2<ITableLineType>(secondTypeBoss));
		secondTypeAttr->Set(Func_OS::GetSType(secondStrokeType));
		attrList->ApplyAttribute(secondTypeAttr);

		InterfacePtr<ITableAttrRealNumber> secondTintAttr (::CreateObject2<ITableAttrRealNumber>(secondTintBoss));
		secondTintAttr->Set(secondStrokeTint);
		attrList->ApplyAttribute(secondTintAttr);

		InterfacePtr<ITableAttrBool16> secondOverprintAttr (::CreateObject2<ITableAttrBool16>(secondOverprintBoss));
		secondOverprintAttr->Set(secondStrokeOverprint);
		attrList->ApplyAttribute(secondOverprintAttr);		
	
		InterfacePtr<ITableAttrUID> secondGapColorAttr (::CreateObject2<ITableAttrUID>(secondGapColorBoss));
		secondGapColorAttr->Set(secondGapColorUID);
		attrList->ApplyAttribute(secondGapColorAttr);

		InterfacePtr<ITableAttrRealNumber> secondGapTintAttr (::CreateObject2<ITableAttrRealNumber>(secondGapTintBoss));
		secondGapTintAttr->Set(secondGapTint);
		attrList->ApplyAttribute(secondGapTintAttr);

		InterfacePtr<ITableAttrBool16> secondGapOverprintAttr (::CreateObject2<ITableAttrBool16>(secondGapOverprintBoss));
		secondGapOverprintAttr->Set(secondGapOverprint);
		attrList->ApplyAttribute(secondGapOverprintAttr);

		InterfacePtr<ITableAttrInt32> secondSkipAttr (::CreateObject2<ITableAttrInt32>(secondSkipBoss));
		secondSkipAttr->Set(skipSecond);
		attrList->ApplyAttribute(secondSkipAttr);


		InterfacePtr<ITableCommands> curTableModel (parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());
		status = curTableModel->ApplyTableOverrides(attrList.get());
		

	} while(kFalse);

	if (status == kSuccess)
	{
	}
	else
		throw TCLError(error);
}


/* Constructor
*/
Func_FP::Func_FP(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_FP::Func_FP -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		firstCount = reader->ExtractInt(0,0,9999);		
		firstFillColor = reader->ExtractInt(5,0,MAX_COLOR);
		firstFillTint = reader->ExtractPourcent('+',20,0)*100.0;
		firstFillOverprint = reader->ExtractBooleen(kFalse);
		skipFirst = reader->ExtractInt(0,0,9999);
		secondCount = reader->ExtractInt(0,0,9999);		
		secondFillColor = reader->ExtractInt(9,0,MAX_COLOR);
		secondFillTint = reader->ExtractPourcent('+',100,0)*100.0;
		secondFillOverprint = reader->ExtractBooleen(kFalse);
		skipSecond = reader->ExtractInt(0,0,9999);
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
		IDCall_FP(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}


/* IDCall_FP
*/
void Func_FP::IDCall_FP(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");
	int32 rowIndex = 0;

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}
		
		UID firstColorUID = parser->getColorList(firstFillColor).colorUID;
		if(firstColorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			firstColorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
		}
		
		UID secondColorUID = parser->getColorList(secondFillColor).colorUID;
		if(secondColorUID == kInvalidUID)
		{
			error = PMString(kErrNoSuchColor);
			secondColorUID = Utils<ISwatchUtils>()->GetNoneSwatchUID(db); // color doesn't exist, replaced with none
		}

		boost::shared_ptr<AttributeBossList> attrList (new AttributeBossList);

		InterfacePtr<ITableAttrInt32> firstCountAttr (::CreateObject2<ITableAttrInt32>(kTableAttrRowFillPatternFirstCountBoss));
		firstCountAttr->Set(firstCount);
		attrList->ApplyAttribute(firstCountAttr);

		InterfacePtr<ITableAttrUID> firstColorAttr (::CreateObject2<ITableAttrUID>(kTableAttrRowFillPatternFirstColorBoss));
		firstColorAttr->Set(firstColorUID);
		attrList->ApplyAttribute(firstColorAttr);

		InterfacePtr<ITableAttrRealNumber> firstTintAttr (::CreateObject2<ITableAttrRealNumber>(kTableAttrRowFillPatternFirstTintBoss));
		firstTintAttr->Set(firstFillTint);
		attrList->ApplyAttribute(firstTintAttr);

		InterfacePtr<ITableAttrBool16> firstOverprintAttr (::CreateObject2<ITableAttrBool16>(kTableAttrRowFillPatternFirstOverprintBoss));
		firstOverprintAttr->Set(firstFillOverprint);
		attrList->ApplyAttribute(firstOverprintAttr);		

		InterfacePtr<ITableAttrInt32> firstSkipAttr (::CreateObject2<ITableAttrInt32>(kTableAttrRowFillPatternStartValueBoss));
		firstSkipAttr->Set(skipFirst);
		attrList->ApplyAttribute(firstSkipAttr);


		InterfacePtr<ITableAttrInt32> secondCountAttr (::CreateObject2<ITableAttrInt32>(kTableAttrRowFillPatternSecondCountBoss));
		secondCountAttr->Set(secondCount);
		attrList->ApplyAttribute(secondCountAttr);

		InterfacePtr<ITableAttrUID> secondColorAttr (::CreateObject2<ITableAttrUID>(kTableAttrRowFillPatternSecondColorBoss));
		secondColorAttr->Set(secondColorUID);
		attrList->ApplyAttribute(secondColorAttr);

		InterfacePtr<ITableAttrRealNumber> secondTintAttr (::CreateObject2<ITableAttrRealNumber>(kTableAttrRowFillPatternSecondTintBoss));
		secondTintAttr->Set(secondFillTint);
		attrList->ApplyAttribute(secondTintAttr);

		InterfacePtr<ITableAttrBool16> secondOverprintAttr (::CreateObject2<ITableAttrBool16>(kTableAttrRowFillPatternSecondOverprintBoss));
		secondOverprintAttr->Set(secondFillOverprint);
		attrList->ApplyAttribute(secondOverprintAttr);		

		InterfacePtr<ITableAttrInt32> secondSkipAttr (::CreateObject2<ITableAttrInt32>(kTableAttrRowFillPatternEndValueBoss));
		secondSkipAttr->Set(skipSecond);
		attrList->ApplyAttribute(secondSkipAttr);


		InterfacePtr<ITableCommands> curTableModel (parser->getCurDataBase(), parser->getCurTable(), UseDefaultIID());
		status = curTableModel->ApplyTableOverrides(attrList.get());
		

	} while(kFalse);

	if (status == kSuccess)
	{
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_MZ::Func_MZ(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_MZ::Func_MZ -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		direction = reader->ExtractInt(1,1,2); // vertical = 1, horizontal = 2
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
		IDCall_MZ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_MZ
*/
void Func_MZ::IDCall_MZ(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		GridArea curCells = parser->getCurCells();

		if(curCells == kZeroGridArea)
		{
			error = PMString(kErrNoCurrentCells);
			break;
		}

		InterfacePtr<ITableModel> tableModel (db, parser->getCurTable(), UseDefaultIID());
		InterfacePtr<ITableTextContainer> tableTextContainer (tableModel, UseDefaultIID());

		InterfacePtr<ITextModel> txtModel (tableTextContainer->QueryTextModel()); 		
		
		InterfacePtr<ITextStoryThreadDict> textStoryThreadDict(tableModel, UseDefaultIID());
		
		InterfacePtr<ITableCommands> tableCommands (tableModel, UseDefaultIID()); 
		
		status = kSuccess;
		if(direction == 2)
		{
			for(int32 row = curCells.topRow ; row < curCells.bottomRow ; ++row)
			{
				WideString curContent;

				int32 startMergeCol = -1, col = -1;

				// Get the story thread of the current cell (the top left cell in the area if it's a multi-cell selection)
				InterfacePtr<ITextStoryThread> cellStoryThread (textStoryThreadDict->QueryThread(tableModel->GetGridID(GridAddress(row, curCells.leftCol))));

				int32 curThreadLength = 0;
				TextIndex curThreadStart = cellStoryThread->GetTextStart(&curThreadLength);		

				bool16 curContainsOwnedItem = kFalse;
				WideString curTxtContent;
				
				for(col = curCells.leftCol + 1 ; col < curCells.rightCol ; ++col)
				{
					GridAddress gridAddress (row, col);

					// Get the story thread of the cell
					InterfacePtr<ITextStoryThread> cellStoryThread (textStoryThreadDict->QueryThread(tableModel->GetGridID(gridAddress)));
					if(cellStoryThread == nil) // not a bug, it occurs whenever we refer to a part of an already merged cell
						continue;					

					int32 threadLength = 0;
					TextIndex threadStart = cellStoryThread->GetTextStart(&threadLength);
					
					bool16 containsOwnedItem = kFalse;
					WideString txtContent;
					if(this->Compare(txtModel, curThreadStart, curThreadLength, threadStart, threadLength, containsOwnedItem, txtContent))
					{
						if(startMergeCol == -1)
						{
							startMergeCol = col-1;
							curContainsOwnedItem = containsOwnedItem;
							curTxtContent = txtContent;
						}
					}
					else
					{
						if(startMergeCol != -1) // Content are equals merge cells
						{
							// Merge contiguous cells
							status = this->MergeCells(GridArea(row, startMergeCol, row+1, col), curContainsOwnedItem, curTxtContent, tableCommands, direction);
							if(status != kSuccess)
								break;							

							startMergeCol = -1;						
						}
						
						curThreadStart = threadStart;
						curThreadLength = threadLength;
						curContainsOwnedItem = kFalse;
						curTxtContent.Clear();
					}
				}

				if(startMergeCol != -1)
				{
					// Merge contiguous cells
					status = this->MergeCells(GridArea(row, startMergeCol, row+1, col), curContainsOwnedItem, curTxtContent, tableCommands, direction);
					if(status != kSuccess)
						break;			
				}
			}
		}
		else
		{
			for(int32 col = curCells.leftCol ; col < curCells.rightCol ; ++col)
			{
				WideString curContent;

				int32 startMergeRow = -1, row = -1;

				// Get the story thread of the current cell (the top left cell in the area if it's a multi-cell selection)
				InterfacePtr<ITextStoryThread> cellStoryThread (textStoryThreadDict->QueryThread(tableModel->GetGridID(GridAddress(curCells.topRow, col))));

				int32 curThreadLength = 0;
				TextIndex curThreadStart = cellStoryThread->GetTextStart(&curThreadLength);

				bool16 curContainsOwnedItem = kFalse;
				WideString curTxtContent;

				for(row = curCells.topRow + 1 ; row < curCells.bottomRow ; ++row)
				{
					GridAddress gridAddress (row, col);

					// Get the story thread of the cell
					InterfacePtr<ITextStoryThread> cellStoryThread (textStoryThreadDict->QueryThread(tableModel->GetGridID(gridAddress)));
					if(cellStoryThread == nil) // not a bug, it occurs whenever we refer to a part of an already merged cell
						continue;

					int32 threadLength = 0;
					TextIndex threadStart = cellStoryThread->GetTextStart(&threadLength);					

					bool16 containsOwnedItem = kFalse;
					WideString txtContent;
					if(this->Compare(txtModel, curThreadStart, curThreadLength, threadStart, threadLength, containsOwnedItem, txtContent))
					{
						if(startMergeRow == -1)
							startMergeRow = row-1;
					}
					else
					{
						if(startMergeRow != -1)
						{
							// Merge contiguous cells							
							status = this->MergeCells(GridArea(startMergeRow, col, row, col+1), curContainsOwnedItem, curTxtContent, tableCommands, direction);
							if(status != kSuccess)
								break;							

							startMergeRow = -1;						
						}
						
						curThreadStart = threadStart;
						curThreadLength = threadLength;
						curContainsOwnedItem = kFalse;
						curTxtContent.Clear();
					}
				}

				if(startMergeRow != -1)
				{
					// Merge contiguous cells					
					status = this->MergeCells(GridArea(startMergeRow, col, row, col+1), curContainsOwnedItem, curTxtContent, tableCommands, direction);
					if(status != kSuccess)
						break;			
				}
			}
		}
		
	} while(kFalse);

	if (status == kSuccess)
	{
	}
	else
		throw TCLError(error);
}

bool16 Func_MZ::Compare(ITextModel * txtModel, TextIndex threadStart, int32 length, TextIndex threadStart2, int32 length2, bool16& containsOwnedItem, WideString& txtContent)
{
	bool16 equal = kFalse;

	IDataBase * db = ::GetDataBase(txtModel);

	// Compare string contents
	WideString content, content2;

	std::copy(TextIterator (txtModel, threadStart), TextIterator (txtModel, threadStart + length-1), std::back_inserter(content));
	std::copy(TextIterator (txtModel, threadStart2), TextIterator (txtModel, threadStart2 + length2-1), std::back_inserter(content2));

	if(content.compare(content2) == 0) // same content
	{
		OwnedItemDataList ownedList, ownedList2;
		Utils<ITextUtils>()->CollectOwnedItems(txtModel, threadStart, length, &ownedList);
		Utils<ITextUtils>()->CollectOwnedItems(txtModel, threadStart2, length2, &ownedList2);
		
		if(ownedList.empty())// no embedded items (ownedList2 must be empty too), just text which is the same for both cells
		{
			txtContent = content;
			equal = kTrue;
		}
		else
		{
			containsOwnedItem = kTrue;
			equal = kTrue;
			for(int32 i = 0 ; i < ownedList.size() && equal; ++i) // both list must have same size
			{
				if(ownedList[i].fClassID == ownedList2[i].fClassID)
				{
					if(ownedList[i].fClassID == kInlineBoss)  // we automatically consider items which are not inlines as equal
					{
						IDataBase * db = ::GetDataBase(txtModel);
						InterfacePtr<IHierarchy> inlineHier (db, ownedList[i].fUID, UseDefaultIID());
						InterfacePtr<IHierarchy> inlineHier2 (db, ownedList2[i].fUID, UseDefaultIID());
						
						InterfacePtr<IHierarchy> inlineSpline (inlineHier->QueryChild(0));
						InterfacePtr<IHierarchy> inlineSpline2 (inlineHier2->QueryChild(0));

						// Compare geometry first (including transformation)
						InterfacePtr<IGeometry> splineGeo (inlineSpline, UseDefaultIID());
						InterfacePtr<ITransform> splineTransform (inlineSpline, UseDefaultIID());
						InterfacePtr<IGeometry> splineGeo2 (inlineSpline2, UseDefaultIID());
						InterfacePtr<ITransform> splineTransform2 (inlineSpline2, UseDefaultIID());

						PMReal rotAngle = 0.0, rotAngle2 = 0.0, xSkewAngle = 0.0, xSkewAngle2 = 0.0, xScale = 0.0, xScale2 = 0.0, yScale = 0.0, yScale2 = 0.0;
						splineTransform->GetInnerToParentMatrix().GetMatrixInfo(&rotAngle, &xSkewAngle, &xScale, &yScale);
						splineTransform2->GetInnerToParentMatrix().GetMatrixInfo(&rotAngle2, &xSkewAngle2, &xScale2, &yScale2);
						
						if(splineGeo->GetStrokeBoundingBox() == splineGeo2->GetStrokeBoundingBox()
						 && splineGeo->GetPathBoundingBox() == splineGeo2->GetPathBoundingBox()
						 && rotAngle == rotAngle2 && xSkewAngle == xSkewAngle2 && xScale == xScale2 && yScale == yScale2)
						{
							InterfacePtr<IFrameType> frameType (inlineSpline, UseDefaultIID());
							InterfacePtr<IFrameType> frameType2 (inlineSpline2, UseDefaultIID());

							// Inline Items have same geometry, so compare contents now
							if(frameType->IsGraphicFrame() && frameType2->IsGraphicFrame())
							{
								if(!frameType->IsEmpty() && !frameType2->IsEmpty()) // if both graphic frames are empty, it's ok
								{																
									InterfacePtr<IDataLinkReference> dataLinkRef (db, inlineSpline->GetChildUID(0), UseDefaultIID());
									InterfacePtr<IDataLinkReference> dataLinkRef2 (db, inlineSpline2->GetChildUID(0), UseDefaultIID());
								
									InterfacePtr<IDataLink> dataLink (db, dataLinkRef->GetUID(), UseDefaultIID());
									InterfacePtr<IDataLink> dataLink2 (db, dataLinkRef2->GetUID(), UseDefaultIID());

									if(dataLink->GetFullName()->Compare(kTrue, *(dataLink2->GetFullName())) != 0) // same image path
									{
										containsOwnedItem = kFalse;
										equal = kFalse;
									}
									else
									{
										// Compare geometry (including transformations) of image item
										InterfacePtr<IGeometry> imageGeo (dataLinkRef, UseDefaultIID());
										InterfacePtr<IGeometry> imageGeo2 (dataLinkRef2, UseDefaultIID());
										InterfacePtr<ITransform> imageTransform (dataLinkRef, UseDefaultIID());
										InterfacePtr<ITransform> imageTransform2 (dataLinkRef2, UseDefaultIID());

										PMMatrix imageMatrix = imageTransform->GetInnerToParentMatrix();
										PMMatrix imageMatrix2 = imageTransform2->GetInnerToParentMatrix();										
										
										if(imageGeo->GetStrokeBoundingBox(imageMatrix) != imageGeo2->GetStrokeBoundingBox(imageMatrix2)
										 || imageGeo->GetPathBoundingBox(imageMatrix) != imageGeo2->GetPathBoundingBox(imageMatrix2))
										{
											containsOwnedItem = kFalse;
											equal = kFalse;
										}
									}
								}
								else if(!frameType->IsEmpty() || !frameType2->IsEmpty())
								{
									containsOwnedItem = kFalse;
									equal = kFalse;
								}
								// else both graphic frames are empty, it's ok
							}
							else if(frameType->IsTextFrame () && frameType2->IsTextFrame())
							{
								InterfacePtr<IMultiColumnTextFrame> txtFrame (db, inlineSpline->GetChildUID(0), UseDefaultIID());
								InterfacePtr<IMultiColumnTextFrame> txtFrame2 (db, inlineSpline2->GetChildUID(0), UseDefaultIID());

								InterfacePtr<ITextModel> txtModel1 (txtFrame->QueryTextModel());
								InterfacePtr<ITextModel> txtModel2 (txtFrame2->QueryTextModel());

								// Compare text contents (without taking inlines into account)
								WideString inlineTxtContent, inlineTxtContent2;

								std::copy(TextIterator (txtModel1, 0), TextIterator (txtModel1, txtModel1->TotalLength()-1), std::back_inserter(inlineTxtContent));
								std::copy(TextIterator (txtModel2, 0), TextIterator (txtModel2, txtModel2->TotalLength()-1), std::back_inserter(inlineTxtContent2));

								if(inlineTxtContent.compare(inlineTxtContent2) != 0)
								{
									containsOwnedItem = kFalse;
									equal = kFalse;
								}
							}
							else if(frameType->GetFrameType() != frameType2->GetFrameType())
							{
								containsOwnedItem = kFalse;
								equal = kFalse;
							}

						}
						else 
						{
							containsOwnedItem = kFalse;
							equal = kFalse;
						}
					}
				}
				else
				{
					containsOwnedItem = kFalse;
					equal = kFalse;
				}				
			}
		}
	}
	
	return equal;
}

/* MergeCells
*/
ErrorCode Func_MZ::MergeCells(const GridArea& area, bool16 containsOwnedItem, WideString content, ITableCommands * tableCommands, int8 direction)
{
	if(containsOwnedItem)
	{
		WideString emptyString;

		// make all cells empty but the top left so that after merging the cell will contain only one cell content
		if(direction == 2)
		{			
			for(int32 col = area.leftCol + 1 ; col < area.rightCol ; ++col)			
				tableCommands->SetCellText(emptyString, GridAddress(area.topRow, col));
		}
		else
		{
			for(int32 row = area.topRow + 1 ; row < area.bottomRow ; ++row)
				tableCommands->SetCellText(emptyString, GridAddress(row, area.leftCol));
		}

		// Then merge cells
		if(tableCommands->MergeCells(area) != kSuccess)
			return kFailure;
	}
	else
	{
		if(tableCommands->MergeCells(area) != kSuccess)
			return kFailure;
		
		if(tableCommands->SetCellText(content, area.GetTopLeft()) != kSuccess)
			return kFailure;
	}

	return kSuccess;
}

/* Constructor
*/
Func_RU::Func_RU(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_RU::Func_RU -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		direction = reader->ExtractInt(1,1,2); // rows = 1, columns = 2
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
		IDCall_RU(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_RU
*/
void Func_RU::IDCall_RU(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDataBase * db = parser->getCurDataBase();

		if(parser->getCurTable() == kInvalidUID)
		{
			error = PMString(kErrNoCurrentTable);
			break;
		}

		GridArea curCells = parser->getCurCells();

		if(curCells == kZeroGridArea)
		{
			error = PMString(kErrNoCurrentCells);
			break;
		}

		InterfacePtr<ITableModel> tableModel (db, parser->getCurTable(), UseDefaultIID());
		InterfacePtr<ITableTextContainer> tableTextContainer (tableModel, UseDefaultIID());
		InterfacePtr<ITableLayout> tableLayout (tableModel, UseDefaultIID());
		InterfacePtr<ITableAttrAccessor> tableAttrs (tableModel, UseDefaultIID());

		// Get wax strand of text story associated with the table
		InterfacePtr<ITextParcelList> frameList (tableTextContainer->QueryTextParcelList()); 
		InterfacePtr<IWaxStrand> waxStrand (frameList, UseDefaultIID());

		
		InterfacePtr<ITextStoryThreadDict> textStoryThreadDict(tableModel, UseDefaultIID());
		
		InterfacePtr<ITableCommands> tableCommands (tableModel, UseDefaultIID()); 
		
		status = kSuccess;
		if(direction == 1)
		{
			PMReal maxHeight = -1;
			PMReal sumHeight = 0.0;
			for(int32 row = curCells.topRow ; row < curCells.bottomRow ; ++row)
			{
				InterfacePtr<ITextStoryThread> cellStoryThread (textStoryThreadDict->QueryThread(tableModel->GetGridID(GridAddress(row, 0))));
				InterfacePtr<ITextParcelList> cellParcelList (cellStoryThread, UseDefaultIID());

				// Get rowHeight
				PMReal rowHeight = tableLayout->GetRowHeight(row);
				sumHeight += rowHeight;

				// Check whether cell is overset
				TextIndex textEndInCell = cellStoryThread->GetTextEnd();
				TextIndex firstOverset = cellParcelList->GetFirstOversetTextIndex();

				
				if(firstOverset != kInvalidTextIndex && firstOverset != textEndInCell) 
				{
					// cell is overset, we take its height
					if(rowHeight > maxHeight)
						maxHeight = rowHeight;

					//PMString debug("Row : ");
					//debug.AppendNumber(row);
					//debug += "\n";
					//debug += "Height : ";
					//debug.AppendNumber(rowHeight);
					//CAlert::InformationAlert(debug);
				}		
				else
				{
					TextIndex textStartInCell = cellStoryThread->GetTextStart();

					// First remove vertical justification
					InterfacePtr<ITableAttrInt32> vjAttr ((ITableAttrInt32 *) tableAttrs->QueryCellAttribute(GridAddress(row, 0), kTextCellAttrVerticalJustificationBoss, IID_ITABLEATTRINT32));
					int32 currentVJ = vjAttr->Get();

					boost::shared_ptr<AttributeBossList> attrList (new AttributeBossList);

					InterfacePtr<ITableAttrInt32> tmpVJAttr (::CreateObject2<ITableAttrInt32>(kTextCellAttrVerticalJustificationBoss));
					tmpVJAttr->Set(Text::kVJTop);
					attrList->ApplyAttribute(tmpVJAttr);
					tableCommands->ApplyCellOverrides(GridArea(GridAddress(row, 0), GridSpan(1,1)), attrList.get());

					// Recompose so that vertical justification change is applied
					ParcelKey firstParcel, secondParcel;
					cellParcelList->GetParcelsContaining(RangeData(textStartInCell, textEndInCell), &firstParcel, &secondParcel);
					firstParcel = cellParcelList->GetFirstDamagedParcel();
					//if(firstParcel.IsValid())
						//porting CS5 cellParcelList->Recompose(firstParcel, secondParcel, nil);

					// Top and bottom insets have to be taken into account while computing text height
					PMReal topInset = 0.0, bottomInset = 0.0;
					InterfacePtr<ITableAttrRealNumber> topInsetAttr ((ITableAttrRealNumber *) tableAttrs->QueryCellAttribute(GridAddress(row, 0), kCellAttrTopInsetBoss, IID_ITABLEATTRREALNUMBER));
					InterfacePtr<ITableAttrRealNumber> bottomInsetAttr((ITableAttrRealNumber *) tableAttrs->QueryCellAttribute(GridAddress(row, 0), kCellAttrBottomInsetBoss, IID_ITABLEATTRREALNUMBER));

					topInset = topInsetAttr->Get();
					bottomInset = bottomInsetAttr->Get();

					if(topInset < 0.5) 
						topInset = 0.5;

					// cell is not overset, we take text height					
					PMPoint refFirstLine = kZeroPoint, refLastLine = kZeroPoint;
					
					PMLineSeg firstline, lastline;

					K2::scoped_ptr<IWaxIterator> waxIter (waxStrand->NewWaxIterator());
					IWaxLine * firstLine = waxIter->GetFirstWaxLine(textStartInCell);
					IWaxLine * lastLine = waxIter->GetFirstWaxLine(textEndInCell-1);			
				
					//UID frameUID = kInvalidUID;
					//waxStrand->GetTextIndexToWaxLineData(textStartInCell, kFalse,nil,nil,nil,&frameUID);
					//waxStrand->GetTextIndexToWaxLineData(textEndInCell-1, kFalse,nil,nil,nil,nil,nil,nil,&lastline,&refLastLine);																			
					
					PMReal textHeight = lastLine->GetYPosition() - firstLine->GetYPosition() + firstLine->GetTOFLineHeight() + topInset + bottomInset;
	
					PMString debug("Text height :");
					debug.AppendNumber(textHeight);
					CAlert::InformationAlert(debug);

					if(textHeight > maxHeight)
						maxHeight = textHeight;

					// Reset vertical justification to its previous value
					attrList->ClearAllOverrides();
					tmpVJAttr->Set(currentVJ);
					attrList->ApplyAttribute(vjAttr);
					tableCommands->ApplyCellOverrides(GridArea(GridAddress(row, 0), GridSpan(1,1)), attrList.get());

		
					//InterfacePtr<ITextFrame> txtFrame (::GetDataBase(tableModel), frameUID, UseDefaultIID());
					//InterfacePtr<IHierarchy> spline (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
					//InterfacePtr<IGeometry>splineGeo (spline, UseDefaultIID());

					//::InnerToPasteboard(splineGeo, &refFirstLine);
					//::InnerToPasteboard(splineGeo, &refLastLine);

					//UIDRef spreadLayer = UIDRef(::GetDataBase(txtFrame), spline->GetLayerUID());														

					//PMPointList ptList;
					//ptList.push_back(refFirstLine);
					//ptList.push_back(refLastLine);
					//
					//Utils<IPathUtils>()->CreateLineSpline(spreadLayer, ptList, INewPageItemCmdData::kDefaultGraphicAttributes);

					//PMString debug("Row : ");
					//debug.AppendNumber(row);
					//debug += "\n";
					//debug += "Text Height : ";
					//debug.AppendNumber(textHeight);
					//CAlert::InformationAlert(debug);
				}
			}

			PMReal distributedHeight = maxHeight;
			if( (maxHeight * curCells.GetRows().count) < sumHeight)
				distributedHeight = sumHeight / curCells.GetRows().count;
			
			// Set new height of rows
			tableCommands->ResizeRows(curCells.GetRows(), distributedHeight);		
		}
		else
		{
			for(int32 col = curCells.leftCol ; col < curCells.rightCol ; ++col)
			{
			}
		}

		status = kSuccess;

	} while(kFalse);

	if (status == kSuccess)
	{
	}
	else
		throw TCLError(error);
}