/*
//	File:	Log.cpp
//
//	Date:	9-Nov-2005
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

#include "VCPlugInHeaders.h"

// Project includes
#include "GlobalDefs.h"
#include "Log.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "ErrorUtils.h"
#include "FileUtils.h"
#include "StreamUtil.h"

// Interfaces includes
#include "IPMStream.h"
#include "IDocument.h"

/* Constructor
*/
Func_UL::Func_UL(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		throw TCLError(PMString(kErrNilInterface));
	}

	logfileStream = nil;

	try
	{
		reader->MustEgal();

		fileName = reader->ExtractString(255);
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
		IDCall_UL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_UL::~Func_UL()
{
	if(logfileStream != nil)
		logfileStream->Release();
}

/* IDCall_UL
*/
void Func_UL::IDCall_UL(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		IDFile logfile = FileUtils::PMStringToSysFile(fileName);
		IDFile parent;		

		FileUtils::GetParentDirectory(logfile, parent) ;		
		if(FileUtils::DoesFileExist(parent) == kFalse)// Invalid path
		{
       		error = PMString(kErrBadPath); 
            break;
        }

		logfileStream = StreamUtil::CreateFileStreamWrite(logfile,kOpenOut | kOpenApp,kTextType,kAnyCreator);
		if(logfileStream == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Write current doc name
		IDocument * curDoc = parser->getCurDoc();
		if(curDoc)
		{
			PMString docName;
			curDoc->GetName(docName);
			PMString output = "[DN=\"" + docName + "\"]" + kNewLine;
			logfileStream->XferByte((uchar *) output.GetPlatformString().c_str(), output.GetPlatformString().size());
		}

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

IPMStream * Func_UL::QueryLogFileStream()
{
	if(logfileStream != nil)
	{
		logfileStream->AddRef();
		return logfileStream;
	}
	return nil;
}