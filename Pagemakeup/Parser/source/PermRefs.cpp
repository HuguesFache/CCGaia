/*
//	File:	PermRefs.cpp
//
//	Date:	13-Sep-2005
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// Project includes
#include "GlobalDefs.h"
#include "PermRefs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "ErrorUtils.h"
#include "PermRefsUtils.h"
#include "CAlert.h"
#include "StringUtils.h"

// Interfaces includes
#include "ITextModel.h"
#include "IUnitOfMeasure.h"
#include "IStoryList.h"
#include "IDocument.h"
#include "ITCLReferencesList.h"
#include "AttributeBossList.h"
#include "TextAttributeRunIterator.h"
#include "IStoryList.h"
#include "IRangeData.h"
#include "IPermRefsTag.h"

// Traffic Presse includes
#if TRAFFIC_PRESSE
#include "ITPConnexion.h"
#include "TPCID.h"
#endif

/* Constructor
*/
Func_HD::Func_HD(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_HD::Func_HD -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		ref = reader->ExtractLongInt(2000,1,MAX_BLOC);
		param1 = reader->ExtractLongInt(0,0,MAX_BLOC);
		param2 = reader->ExtractLongInt(0,0,MAX_BLOC);
		param3 = reader->ExtractLongInt(0,0,MAX_BLOC);
		param4 = reader->ExtractLongInt(0,0,MAX_BLOC);		
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
		IDCall_HD(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_HD::~Func_HD()
{
}

void Func_HD::IDCall_HD(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		TextIndex startPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(startPos));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
	
		PermRefStruct thePermref;
		thePermref.ref = ref;
		thePermref.param1 = param1;
		thePermref.param2 = param2;
		thePermref.param3 = param3;
		thePermref.param4 = param4;

		parser->SetCurrentPermRefs(thePermref, startPos, ::GetUID(txtModel));

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
	
}

/* Constructor
*/
Func_HF::Func_HF(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_HF::Func_HF -> reader nil");
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
		IDCall_HF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_HF::~Func_HF()
{
}

/* IDCall_HF
*/
void Func_HF::IDCall_HF(ITCLParser *  parser)
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

		PermRefStruct refToInsert;
		TextIndex startPos = INVALID_PERMREFS;
		
		parser->GetCurrentPermRefs(refToInsert, startPos);

		if(refToInsert == kNullPermRef)
		{		
			error = PMString("Invalid PermRefs reference");
			break;
		}

		TextIndex curPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(curPos));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
        status = PermRefsUtils::ApplyPermRefs(refToInsert, txtModel, startPos, curPos) ;
		if(status != kSuccess)
			error = ErrorUtils::PMGetGlobalErrorString();

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_HS::Func_HS(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_HS::Func_HS -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		ref = reader->ExtractLongInt(2000,1,MAX_BLOC);
		param1 = reader->ExtractLongInt(0,0,MAX_BLOC);
		param2 = reader->ExtractLongInt(0,0,MAX_BLOC);
		param3 = reader->ExtractLongInt(0,0,MAX_BLOC);
		param4 = reader->ExtractLongInt(0,0,MAX_BLOC);	
		deleteNextSpaceOrCRIfEmpty = kFalse;
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
		bool8 hrRead = kFalse, escapeCharRead = kFalse;
		txtBuf.reset(new WideString());
		// Read text between [HS] and [HR]
		try
		{
			do
			{
				reader->ReadChar();
				if (reader->GetCurrentANSIChar() == '[')
				{
					reader->SetEndOfCommand(kFalse);
					reader->FiltreBlancUpper();
					uchar16 c1 = reader->GetCurrentANSIChar();
					UTF32TextChar c1Bis = reader->GetCurrentChar();
					reader->FiltreBlancUpper();
					uchar16 c2 = reader->GetCurrentANSIChar();
					if(c1 == 'H' && c2 == 'R')
					{
						if(reader->TestEgal())		
							deleteNextSpaceOrCRIfEmpty = reader->ExtractBooleen(kFalse);		
						else
							deleteNextSpaceOrCRIfEmpty = kFalse;

						if(reader->IsEndOfCommand())
							hrRead = kTrue;
						else
							throw TCLError(PMString());
					}
					else
					{
						txtBuf->Append('[');
						txtBuf->Append(c1Bis);
						txtBuf->Append(reader->GetCurrentChar());					
					}
				}
				else if (reader->GetCurrentANSIChar() == '^') // escape char
				{
					escapeCharRead = kTrue;
				}
				else
				{
					if(escapeCharRead)
					{
						UTF32TextChar escapeChar= reader->GetCurrentChar();
						if(escapeChar == 'n')
							txtBuf->Append(kTextChar_LF);
						else if(escapeChar == 't')
							txtBuf->Append(kTextChar_Tab);
						else if(escapeChar == 'm')
							txtBuf->Append(kTextChar_EmSpace);
						else if(escapeChar == '|')
							txtBuf->Append(kTextChar_HairSpace);
						else if(escapeChar == 's')
							txtBuf->Append(kTextChar_HardSpace);
						else if(escapeChar == '<')
							txtBuf->Append(kTextChar_ThinSpace);
						else if(escapeChar == '>')
							txtBuf->Append(kTextChar_EnSpace);
						else if(escapeChar == '~')
							txtBuf->Append(kTextChar_NoBreakHyphen);
						else
						{
							txtBuf->Append('^');
							txtBuf->Append(escapeChar);
						}

						escapeCharRead = kFalse;
					}
					else
						txtBuf->Append(reader->GetCurrentChar());	
				}
			} while (!hrRead);
		}
		catch(TCLError e)
		{
			e.setMessage(PMString(kErrPermRefsHS));
			throw TCLError(e);
		}

		IDCall_HS(parser);
	}
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_HS::~Func_HS()
{
}

/* IDCall_HS
*/
void Func_HS::IDCall_HS(ITCLParser *  parser)
{
	ErrorCode status = kSuccess; // this command never fails (we don't consider that a missing ref is an error)
	PMString error("");
	
	do
	{
		// Look for hidden reference inspecting the owner story
		PermRefStruct thePermref;
		thePermref.ref = ref;
		thePermref.param1 = param1;
		thePermref.param2 = param2;
		thePermref.param3 = param3;
		thePermref.param4 = param4;

		if(deleteNextSpaceOrCRIfEmpty && !txtBuf->empty())
			deleteNextSpaceOrCRIfEmpty = kFalse;

		InterfacePtr<IStoryList> storyList (parser->getCurDoc(), UseDefaultIID());	
		if(storyList == nil)
			error = PMString(kErrNilInterface);

		int32 nbStories = storyList->GetUserAccessibleStoryCount();

		for(int32 i = 0 ; i < nbStories ; ++i){

			InterfacePtr<ITextModel> txtModel (storyList->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			if(txtModel && PermRefsUtils::UpdatePermRefs(thePermref, txtModel, txtBuf, deleteNextSpaceOrCRIfEmpty) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				ErrorUtils::PMSetGlobalErrorCode(kSuccess);
				continue;
			}	
		}

	} while(kFalse);
	
	if (status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_HR::Func_HR(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_HR::Func_HR -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		if(reader->TestEgal())		
			reader->ExtractBooleen(kFalse);
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
		throw TCLError(PMString(kErrPermRefsHR));
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* Destructor
*/
Func_HR::~Func_HR()
{
}

/* Constructor
*/
Func_HC::Func_HC(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_HC::Func_HC -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		
		param1 = reader->ExtractLongInt(1,1,MAX_BLOC);
		param2 = reader->ExtractLongInt(0,0,MAX_BLOC);
		param3 = reader->ExtractLongInt(0,0,MAX_BLOC);
		param4 = reader->ExtractLongInt(0,0,MAX_BLOC);		
		separatorType = reader->ExtractInt(0,0,1);
		nbSeparator = reader->ExtractInt(1,0,MAX_BLOC);
		nbCopy = reader->ExtractInt(0,0,MAX_BLOC);
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
		IDCall_HC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_HC
*/
void Func_HC::IDCall_HC(ITCLParser *  parser)
{
	ErrorCode status = kSuccess;
	PMString error("");
	
	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		IDocument * doc = parser->getCurDoc();

		// Define PermRef filter
		PermRefStruct permrefFilter;
		permrefFilter.ref = -1;
		permrefFilter.param1 = param1;
		permrefFilter.param2 = param2;
		permrefFilter.param3 = param3;
		permrefFilter.param4 = param4;

		// Define separator
		textchar separator = kTextChar_CR;
		switch (separatorType)
		{
			case 0 : separator = kTextChar_CR; break;
			case 1 : separator = kTextChar_Space; break;
			default : break;
		}

		// Go through each story looking for the targeted tagged text
		InterfacePtr<IStoryList> storyList (doc, UseDefaultIID());
		
		int32 nbStory = storyList->GetUserAccessibleStoryCount();
		for(int32 i = 0 ; i < nbStory ; ++i)
		{
			PermRefVector permRefsList;
			InterfacePtr<ITextModel> txtModel (storyList->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			PermRefsUtils::CopyContiguousRef(txtModel, permrefFilter, nbCopy, separator, nbSeparator, permRefsList);

			if(!permRefsList.empty())
				parser->AppendPermRefsList(permRefsList);			
		}

	} while(kFalse);

	if (status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_HH::Func_HH(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader (parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		
		ref = reader->ExtractLongInt(0,0,MAX_BLOC);
		serverAddress = reader->ExtractString(255);
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
		IDCall_HH(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}


/* IDCall_HH
*/
void Func_HH::IDCall_HH(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error("");
	
	do
	{
		PermRefStruct thePermref;
		thePermref.ref = ref;

		// Go through each stories thePermref is referenced into
		K2Vector<UID> ownerStories = parser->getStories(thePermref); 

		if(ownerStories.size() == 0)
		{
			PMString refString;
			refString.AppendNumber(ref);

			error = PMString(kErrInvalidLegaleRef);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, refString);			
			break;
		}

		for(int32 i = 0 ; i < ownerStories.size() ; ++i)
		{
			InterfacePtr<ITextModel> txtModel (parser->getCurDataBase(), ownerStories[i], UseDefaultIID());
			if(txtModel == nil)
			{
				error = PMString(kErrNilInterface);
				continue;
			}

			PMReal refHeight = PermRefsUtils::GetHeight(thePermref, txtModel);
			if(refHeight == 0)
			{
				PMString refString;
				refString.AppendNumber(ref);

				error = PMString(kErrNullLegaleHeight);
				error.Translate();
				StringUtils::ReplaceStringParameters(&error, refString);
				break;
			}

#if TRAFFIC_PRESSE
			InterfacePtr<ITPConnexion> tpConnexion (::CreateObject2<ITPConnexion>(kTrafficPresseConnexionBoss));
			if(tpConnexion) // can be nil if TrafficPresseConnexion plugin is missing
			{
				// Convert height to millimeters
				InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));
				refHeight = uom->PointsToUnits(refHeight);

				// Notify TrafficPresse of the reference's height
				tpConnexion->SetServerAddress(serverAddress);
				if(!tpConnexion->NotifyAdHeight(ref, refHeight))
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
			}
			else
			{
				error = PMString(kErrTrafficPluginMissing);
				break;
			}
#endif
			status = kSuccess;
		}

	} while(kFalse);

	if (status != kSuccess)
		throw TCLError(error);
}
