/*
//	File:	Calques.cpp
//
//	Date:	02-08-2005
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "LayerID.h"

// Interface includes
#include "ICommand.h"
#include "ILayoutCmdData.h"
#include "ILayoutUIUtils.h"
#include "ILayoutControlData.h"
#include "INewLayerCmdData.h"
#include "IDocument.h"
#include "ILayerList.h"
#include "IBoolData.h"
#include "IDocumentLayer.h"
#include "IUIDData.h"

// Project includes
#include "Calques.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"

#include "CAlert.h"

/* Constructor
*/
Func_LS::Func_LS(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_LS::Func_LS -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		layerName = reader->ExtractString(255);
		
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
		IDCall_LS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* Destructor
*/
Func_LS::~Func_LS()
{
}

/* IDCall_LS
*/
void Func_LS::IDCall_LS(ITCLParser * parser)
{	
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
		
		// Create SetActiveLayer command
		InterfacePtr<ICommand> setActiveLayerCmd (CmdUtils::CreateCommand(kSetActiveLayerCmdBoss));
		if(setActiveLayerCmd == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		InterfacePtr<ILayerList> layerList (doc, UseDefaultIID());
		if(layerList == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		UID layer = layerList->FindByName(layerName);
		if(layer == kInvalidUID)
		{
			error = PMString(kErrInvalidLayer);
			break;
		}
		
		setActiveLayerCmd->SetItemList(UIDList(parser->getCurDataBase(), layer));


		InterfacePtr<ILayoutCmdData> cmdData (setActiveLayerCmd, UseDefaultIID());
		if(cmdData == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}

/*
		InterfacePtr<ILayoutControlData> layoutCtrlData (Utils<ILayoutUIUtils>()->QueryFrontLayoutData());
		if(layoutCtrlData == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}

		cmdData->Set(::GetUIDRef(doc),layoutCtrlData);
*/
		
		cmdData->Set(::GetUIDRef(doc), nil);

		if(CmdUtils::ProcessCommand(setActiveLayerCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}
		
		status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}


/* Constructor
*/
Func_LN::Func_LN(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_LN::Func_LN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		layerName = reader->ExtractString(255);
		
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
		IDCall_LN(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_LN::~Func_LN()
{
}

/* IDCall_LS
*/
void Func_LN::IDCall_LN(ITCLParser * parser)
{	
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
		
		// Check whether layer doesn't already exist
		InterfacePtr<ILayerList> layerList (doc, UseDefaultIID());
		if(layerList == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		UID layer = layerList->FindByName(layerName);
		if(layer != kInvalidUID)
		{
			error = PMString(kErrLayerExist);
			status = kSuccess;
			break;
		}
		
		// Create newLayer command
		InterfacePtr<ICommand> newLayerCmd (CmdUtils::CreateCommand(kNewLayerCmdBoss));
		if(newLayerCmd == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		InterfacePtr<INewLayerCmdData> cmdData (newLayerCmd, IID_INEWLAYERCMDDATA);
		if(cmdData == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		cmdData->Set(::GetUIDRef(doc), &layerName);
		
		if(CmdUtils::ProcessCommand(newLayerCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}
				
		status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_LH::Func_LH(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_LH::Func_LH -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		layerName = reader->ExtractString(255);
		hide = reader->ExtractBooleen(kTrue);
		dontQuitIfNotExist = reader->ExtractBooleen(kFalse);
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
		IDCall_LH(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_LH
*/
void Func_LH::IDCall_LH(ITCLParser * parser)
{	
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

		InterfacePtr<ILayerList> layerList (doc, UseDefaultIID());
		
		UID layer = layerList->FindByName(layerName);
		if(layer == kInvalidUID)
		{
			if(!dontQuitIfNotExist)
			{
				error = PMString(kErrInvalidLayer);
				break;
			}
			else
			{
				status = kSuccess;
				break;
			}
		}

		InterfacePtr<IDocumentLayer> theLayer (parser->getCurDataBase(), layer, UseDefaultIID());
		if(theLayer->IsVisible() == hide)
		{
			// Create ShowLayer command
			InterfacePtr<ICommand> showLayerCmd (CmdUtils::CreateCommand(kShowLayerCmdBoss));
			showLayerCmd->SetItemList(UIDList(theLayer));

			InterfacePtr<IBoolData> cmdData (showLayerCmd, UseDefaultIID());
			cmdData->Set(!hide);

			if(CmdUtils::ProcessCommand(showLayerCmd) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}
		}
		
		status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_LB::Func_LB(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_LB::Func_LB -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		refItem = reader->ExtractTCLRef(kCurrentTCLRef);
		layerName = reader->ExtractString(255);
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
		IDCall_LB(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_LB
*/
void Func_LB::IDCall_LB(ITCLParser * parser)
{	
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

		InterfacePtr<ILayerList> layerList (doc, UseDefaultIID());
		
		UID layer = layerList->FindByName(layerName);
		if(layer == kInvalidUID)
		{
			error = PMString(kErrInvalidLayer);
			break;
		}
		
		if(refItem == kCurrentTCLRef)
			refItem = parser->getCurItem();
		
		UID refUID = parser->getItem(refItem);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}
		
		// Create MoveToLayer command
		InterfacePtr<ICommand> move2LayerCmd (CmdUtils::CreateCommand(kMoveToLayerCmdBoss));
		move2LayerCmd->SetItemList(UIDList(UIDRef(db, refUID)));

		InterfacePtr<IUIDData> cmdData (move2LayerCmd, UseDefaultIID());
		cmdData->Set(db, layer);

		if(CmdUtils::ProcessCommand(move2LayerCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}
