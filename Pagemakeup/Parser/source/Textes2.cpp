/*
//	File:	Textes2.cpp
//
//	Date:	21/08/2006
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"
#include "ILayoutUtils.h"
#include "ErrorUtils.h"

// Interface includes
#include "IDocument.h"
#include "IPageItemTypeUtils.h"
#include "IMainItemTOPData.h"
#include "IPathGeometry.h"
#include "IBoolData.h"
#include "IRealNumberData.h"
#include "ITOPOptionsCmdData.h"

//Project includes
#include "Textes.h"
#include "GlobalDefs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"


/* Constructor
*/
Func_TStar::Func_TStar(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TStar::Func_TStar -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		startPos = reader->ExtractReel('D','+',0,0);
		endPos = reader->ExtractReel('D','+',0,0);		
		txtAlignType = reader->ExtractInt(4, 1, 8); // see TOPOptionsType.h to figure out the values
		pathAlignType = reader->ExtractInt(3, 1, 3); // see TOPOptionsType.h to figure out the values
		effectType = reader->ExtractInt(1, 1, 5); // see TOPOptionsType.h to figure out the values
		overlapOffset = reader->ExtractInt(0, -800, 800);
		flip = reader->ExtractBooleen(kFalse);

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
		IDCall_TStar(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_TStar
*/
void Func_TStar::IDCall_TStar(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase* db = parser->getCurDataBase() ;

		// Get spline item the text on path is going to be added to
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();

		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		// Check whether the item is valid to have a text on path
		InterfacePtr<IMainItemTOPData> mainItemTOPData(db, refUID, UseDefaultIID());

		if (mainItemTOPData->GetTOPSplineItemUID() != kInvalidUID) 
		{
			// This spline already has text on path associated.	
			break;
		}

		// There are some constraints to the kind of path
		// text on a path can be applied to. Check them.
		InterfacePtr<IPathGeometry> pathGeometry(mainItemTOPData, UseDefaultIID());
	
		if (pathGeometry->GetNumPaths() != 1) 
		{
			// Can't add text on a path to a compound path or empty path.
			break;
		}

		if (pathGeometry->GetNumSegments(0) == 0) 
		{
			// Can't add text on a path to a single point path.
			break;
		}

		Utils<IPageItemTypeUtils> pageItemTypeUtils;
		if (pageItemTypeUtils->IsInline(pathGeometry)) 
		{
			// Can't add text on a path to inlines.
			break;
		}

		// Handle flip
		if(flip)
		{
			// Invert path align
			if(pathAlignType == kPathAlignTop)
				pathAlignType = kPathAlignBottom;
			else if(pathAlignType == kPathAlignBottom)
				pathAlignType = kPathAlignTop;
		}

		// Create the TOP
		InterfacePtr<ICommand> addTextOnPathCmd(CmdUtils::CreateCommand(kAddTextOnPathCmdBoss));
	
		InterfacePtr<IBoolData> defaultCreation(addTextOnPathCmd, UseDefaultIID());
		defaultCreation->Set(kFalse);

		InterfacePtr<ITOPOptionsCmdData> topOptionsData (addTextOnPathCmd, UseDefaultIID());
		topOptionsData->Set((TextAlignType)txtAlignType, (PathAlignType) pathAlignType, (EffectType) effectType, overlapOffset, flip);

		addTextOnPathCmd->SetItemList(UIDList(db, refUID));
		if(CmdUtils::ProcessCommand(addTextOnPathCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Set TOP Position, if asked for
		if(startPos != 0 || endPos != 0)
		{
			InterfacePtr<ICommand> setTOPPositionCmd (CmdUtils::CreateCommand(kPrsSetTOPPositionCmdBoss));
			setTOPPositionCmd->SetUndoability(ICommand::kAutoUndo);
			setTOPPositionCmd->SetItemList(UIDList(db, refUID));

			InterfacePtr<IRealNumberData> startPosData (setTOPPositionCmd, IID_ISTARTPOSDATA);
			startPosData->Set(startPos);

			InterfacePtr<IRealNumberData> endPosData (setTOPPositionCmd, IID_IENDPOSDATA);
			endPosData->Set(endPos);
			
			status = CmdUtils::ProcessCommand(setTOPPositionCmd);		
		}
		else
			status = kSuccess;

	} while(false);

	if (status == kSuccess)
	{
		//parser->setItem(refBloc,newFrame);
		parser->setCurItem(refBloc);
	}
	else
		throw TCLError(error);
}