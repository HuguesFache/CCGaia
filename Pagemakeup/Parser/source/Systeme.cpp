/*
//	File:	Systeme.cpp
//
//	Date:	30-Nov-2005
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"

// Project includes
#include "Systeme.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "FileUtils.h"

/* Constructor
*/
Func_DR::Func_DR(ITCLParser *  parser)
{
	PMString info;

	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DU::Func_DU -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		newName = reader->ExtractString(255);
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
		IDCall_DR(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}



/* IDCall_DR
*/
void Func_DR::IDCall_DR(ITCLParser * parser)
{
	IDFile parent;
	IDFile file = FileUtils::PMStringToSysFile(newName);
	FileUtils::GetParentDirectory(file, parent) ;
		
	if(FileUtils::DoesFileExist (parent) == kFalse)// Invalid path
		throw TCLError(PMString(kErrBadPath));
    
	parser->SetNewFileName(newName);
}