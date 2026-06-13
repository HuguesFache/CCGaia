/*
//	File:	XRefs.cpp
//
//	Date:	12-Jan-2007
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

#include "XRefs.h"

#if XREFS

// General includes
#include "CAlert.h"
#include "ErrorUtils.h"
#include "FileUtils.h"
#include "StringUtils.h"
#include "XRefID.h"
#include "SDKLayoutHelper.h"

// Interface includes
#include "IDocument.h"
#include "IRangeData.h"
#include "IStringData.h"
#include "ITextModel.h"
#include "ISysFileData.h"
#include "IXRefFormatData.h"
#include "IStoryList.h"
#include "IUIDData.h"
#include "IBoolData.h"
#include "IXRefCrossRefList.h"
#include "IXRefCrossRef.h"
#include "IXRefSourceMarkerList.h"

// Project includes
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"

/* Constructor
*/
Func_X1::Func_X1(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		markerName = reader->ExtractString(255);
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
		IDCall_X1(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_X1
*/
void Func_X1::IDCall_X1(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check whether there is a current text frame
		TextIndex insertIndex = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertIndex));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Create InsertSourceMarker command
		InterfacePtr<ICommand> insertSourceMarkerCmd (CmdUtils::CreateCommand(kXRefInsertSourceMarkerCmdBoss));
		if(!insertSourceMarkerCmd)
		{
			error = PMString(kErrCrossRefPluginMissing);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, PMString("X1",-1,PMString::kNoTranslate));
			break;
		}

		
		insertSourceMarkerCmd->SetItemList(UIDList(txtModel));

		// Check whether marker does not already exist in current document
		InterfacePtr<IXRefSourceMarkerList> markerList (parser->getCurDoc(), UseDefaultIID());
		if(markerList->IsNameUsed(markerName))
		{
			error = PMString (kErrCrossRefMarkerAlreadyExist);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, markerName);
			break;
		}

		// Set Name
		InterfacePtr<IStringData> nameData (insertSourceMarkerCmd, UseDefaultIID());
		nameData->Set(markerName);

		// Set Insert Index
		InterfacePtr<IRangeData> indexData (insertSourceMarkerCmd, UseDefaultIID());
		indexData->SetRange(RangeData(insertIndex, insertIndex, RangeData::kLeanForward));

		// Process command		
		if(CmdUtils::ProcessCommand(insertSourceMarkerCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_X2::Func_X2(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		fileName = reader->ExtractString(255);
		markerName = reader->ExtractString(255);
		useFormatName = reader->ExtractBooleen(kFalse);
		format = reader->ExtractString(255);
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
		IDCall_X2(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}


/* IDCall_X2
*/
void Func_X2::IDCall_X2(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check whether there is a current text frame
		TextIndex insertIndex = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertIndex));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		// Create InsertCrossRef command
		InterfacePtr<ICommand> insertCrossRefCmd (CmdUtils::CreateCommand(kXRefInsertCrossRefCmdBoss));
		if(!insertCrossRefCmd)
		{
			error = PMString(kErrCrossRefPluginMissing);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, PMString("X2",-1,PMString::kNoTranslate));
			break;
		}
		
		insertCrossRefCmd->SetItemList(UIDList(txtModel));

		InterfacePtr<ISysFileData> sourceFileData (insertCrossRefCmd, UseDefaultIID());
		IDFile sourceFile = IDFile();
		if(fileName != kNullString)
		{
			// Check whether marker's source file is valid
			sourceFile = FileUtils::PMStringToSysFile(fileName);
			if(!FileUtils::DoesFileExist(sourceFile))
			{
				error = PMString(kErrFileNotExist);
				error.Translate();
				StringUtils::ReplaceStringParameters(&error, fileName);
				break;
			}

			// Check whether marker exist in source file
			SDKLayoutHelper layoutHelper;
			UIDRef sourceDoc = layoutHelper.OpenDocument(sourceFile, kSuppressUI);
			InterfacePtr<IXRefSourceMarkerList> markerList (sourceDoc, UseDefaultIID());
			bool16 markerExist = markerList->IsNameUsed(markerName);
			markerList.reset(nil);
			layoutHelper.CloseDocument(sourceDoc, kFalse, kSuppressUI, kFalse, IDocFileHandler::kSchedule);
			CmdUtils::ProcessScheduledCmds();

			if(!markerExist)
			{
				error = PMString (kErrCrossRefMarkerNotExistInDoc);
				error.Translate();
				StringUtils::ReplaceStringParameters(&error, markerName, FileUtils::SysFileToPMString(sourceFile));
				break;
			}			
		}
		else
		{
			// Check whether marker exist in current document
			InterfacePtr<IXRefSourceMarkerList> markerList (parser->getCurDoc(), UseDefaultIID());
			if(!markerList->IsNameUsed(markerName))
			{
				error = PMString (kErrCrossRefMarkerNotExist);
				error.Translate();
				StringUtils::ReplaceStringParameters(&error, markerName);
				break;
			}
		}
		sourceFileData->Set(sourceFile);		

		// Set Source Marker the Cross Ref refers to, using its name and the file it's is contained in
		InterfacePtr<IStringData> markerNameData (insertCrossRefCmd, UseDefaultIID());
		markerNameData->Set(markerName);

		// Set Insert Index
		InterfacePtr<IRangeData> indexData (insertCrossRefCmd, UseDefaultIID());
		indexData->SetRange(RangeData(insertIndex, insertIndex, RangeData::kLeanForward));

		// Set Cross Ref format
		InterfacePtr<IXRefFormatData> xrefFormat (insertCrossRefCmd, UseDefaultIID());
		PMString formatName = kNullString, formatDef = kNullString;
		if(useFormatName)
			formatName = format;
		else
			formatDef = format;

		xrefFormat->Set(formatName, formatDef);

		// Process command		
		if(CmdUtils::ProcessCommand(insertCrossRefCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;	

	} while(kFalse);	

	if(status != kSuccess)
		throw TCLError(error);		
}

/* Constructor
*/
Func_X3::Func_X3(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->FiltreFin();	
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
		IDCall_X3(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_X3
*/
void Func_X3::IDCall_X3(ITCLParser * parser)
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

		// Iterate through IXRefCrossRefList interface on the document to collect Cross References' UIDRefs
		UIDList crossRefList(parser->getCurDataBase());
		InterfacePtr<IXRefCrossRefList> xrefList (doc, UseDefaultIID());

		int32 nbXRefs = xrefList->GetCrossRefCount();
		if(nbXRefs == 0)
		{
			// Nothing to update, leave command now
			status = kSuccess;
			break;
		}

		for(int32 i = 0 ; i < nbXRefs ; ++i)
			crossRefList.Append(xrefList->GetNthCrossRefUID(i));

		// Create and process update crossref command		
		InterfacePtr<ICommand> updateCrossRefCmd (CmdUtils::CreateCommand(kXRefUpdateCrossRefCmdBoss));
		if(!updateCrossRefCmd)
		{
			error = PMString(kErrCrossRefPluginMissing);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, PMString("X3",-1,PMString::kNoTranslate));
			break;
		}
		updateCrossRefCmd->SetItemList(crossRefList);

		// Process command		
		if(CmdUtils::ProcessCommand(updateCrossRefCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

#if SERVER
		// Iterate through each cross reference to check their state and add messages to the error list
		for(int32 i = 0 ; i < nbXRefs ; ++i)
		{
			InterfacePtr<IXRefCrossRef> aCrossRef (db, crossRefList[i], UseDefaultIID());
			if(aCrossRef->IsInvalid())
			{
				CAlert::WarningAlert(aCrossRef->GetCompositionStateMessage());
			}

			if(aCrossRef->IsUnresolved())
			{
				CAlert::WarningAlert(aCrossRef->GetSourceStateMessage());
			}
		}
#endif
		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

#endif // XREFS