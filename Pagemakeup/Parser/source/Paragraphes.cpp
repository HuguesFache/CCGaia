/*
//	File:	Textes.cpp
//
//	Date:	24-Sep-2003
//
//  Author : Nicolas Metaye
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"
#include "SDKUtilities.h"
#include "ILayoutUtils.h"
#include "ErrorUtils.h"

// Interface includes
#include "IDocument.h"
#include "IWorkSpace.h"
#include "ITextAttributes.h"
#include "ITextAttrRealNumber.h"
#include "ITextModel.h"
#include "IAttributeStrand.h"
#include "ITextAttrTabSettings.h"

//Project includes
#include "Paragraphes.h"
#include "GlobalDefs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "ITextFrameHelper.h"
#include "ITxtAttrHelper.h"
#include "Transcoder.h"


/* Constructor
*/
Func_PE::Func_PE(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PE::Func_PE -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		spaceBefore = reader->ExtractReel('D','+',0,0);
		spaceAfter = reader->ExtractReel('D','+',0,0);

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
		IDCall_PE(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PE::~Func_PE()
{
}


/* IDCall_PE
*/
void Func_PE::IDCall_PE(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PE::IDCall_PE-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PE::IDCall_PE -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();
			
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_PE::IDCall_PE -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if(txtAttrHelper->AddSpaceBeforeAttribute(attrListPtr,spaceBefore) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(txtAttrHelper->AddSpaceAfterAttribute(attrListPtr,spaceAfter) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}


		if (txtFrameHelper->GetCreation()) 
			txtFrameHelper->SetIsParaStyle(kTrue);
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_PG::Func_PG(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PG::Func_PG -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		gridAlign = reader->ExtractBooleen(kFalse) ;
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
		IDCall_PG(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PG::~Func_PG()
{
}


/* IDCall_PG
*/
void Func_PG::IDCall_PG(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PG::IDCall_PG-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PG::IDCall_PG -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();
			
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_PG::IDCall_PG -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if(txtAttrHelper->AddGridAlignmentAttribute(attrListPtr,gridAlign) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (txtFrameHelper->GetCreation()) 
			txtFrameHelper->SetIsParaStyle(kTrue);
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_PH::Func_PH(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PH::Func_PH -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		keepLines = reader->ExtractBooleen(kFalse) ;
		all = reader->ExtractBooleen(kFalse) ;
		firstLines = reader->ExtractInt(2,MIN_LINE,MAX_LINE) ;
		lastLines = reader->ExtractInt(2,MIN_LINE,MAX_LINE) ;

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
		IDCall_PH(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PH::~Func_PH()
{
}


/* IDCall_PH
*/
void Func_PH::IDCall_PH(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PH::IDCall_PH-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PH::IDCall_PH -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();
			
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_PH::IDCall_PH -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}


		if(txtAttrHelper->AddKeepLinesAttribute(attrListPtr,keepLines) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(txtAttrHelper->AddKeepTogetherAttribute(attrListPtr,all) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		if(txtAttrHelper->AddFirstLinesAttribute(attrListPtr,firstLines) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(txtAttrHelper->AddLastLinesAttribute(attrListPtr,lastLines) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (txtFrameHelper->GetCreation()) 
			txtFrameHelper->SetIsParaStyle(kTrue);
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_PK::Func_PK(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PK::Func_PK -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		keepWithNextLines = reader->ExtractBooleen(kFalse);
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
		IDCall_PK(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PK::~Func_PK()
{
}


/* IDCall_PK
*/
void Func_PK::IDCall_PK(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PK::IDCall_PK-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PK::IDCall_PK -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();
			
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_PK::IDCall_PK -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		int16 numberOfLines = 0;
		
		if(keepWithNextLines)
			numberOfLines = 1;

		if(txtAttrHelper->AddKeepWithNextAttribute(attrListPtr,numberOfLines) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (txtFrameHelper->GetCreation()) 
			txtFrameHelper->SetIsParaStyle(kTrue);
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}

/* Constructor
*/
Func_PI::Func_PI(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PI::Func_PI -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		lead = reader->ExtractReel3('+','D',0,0,MIN_LEAD,MAX_LEAD);
		reader->ExtractReel('D','+',0,0); // "additionel" parameter in Quark (what is it used for ??)
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
		IDCall_PI(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PI::~Func_PI()
{
}


/* IDCall_PI
*/
void Func_PI::IDCall_PI(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PI::IDCall_PI-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PI::IDCall_PI -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();


		// Set autoleading attribute (on / off);
		if (lead == 0) //if autoleading == on
			lead=-1; // -1 to set auto leading
		//else we set the leadingvalue
		
		if(txtAttrHelper->AddAutoLeadModeAttribute(attrListPtr,lead) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) 
			//txtFrameHelper->SetIsParaStyle(kTrue);
		//else//if we are not creating a style we have to apply the attribute
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeCharCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_PJ::Func_PJ(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PJ::Func_PJ -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		align = reader->ExtractInt(0,MIN_ALIGN,MAX_ALIGN);
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
		IDCall_PJ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PJ::~Func_PJ()
{
}


/* IDCall_PJ
*/
void Func_PJ::IDCall_PJ(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PJ::IDCall_PJ-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PJ::IDCall_PJ -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();
			
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_PJ::IDCall_PJ -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if(txtAttrHelper->AddAlignAttribute(attrListPtr,align) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		//if (align > 2)
		//{
		//	if(txtAttrHelper->AddAlignLastAttribute(attrListPtr,align) != kSuccess)
		//	{
		//		error = ErrorUtils::PMGetGlobalErrorString();
		//		break;
		//	}
		//}

		if (txtFrameHelper->GetCreation()) 
			txtFrameHelper->SetIsParaStyle(kTrue);
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_PF::Func_PF(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PF::Func_PF -> reader nil");
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
		IDCall_PF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PF::~Func_PF()
{
}


/* IDCall_PF
*/
void Func_PF::IDCall_PF(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PF::IDCall_PF-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		parser->InsertCharacter(kTextChar_CR);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_PA::Func_PA(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PA::Func_PA -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		stroke = reader->ExtractReel3('+','D',MIN_PARARULE_STROKE,0,PMReal(MIN_PARARULE_STROKE),PMReal(MAX_PARARULE_STROKE));
		flag = reader->ExtractBooleen(kFalse);
		indentL = reader->ExtractReel3('+','D',0,0,PMReal(MIN_INDENT),PMReal(MAX_INDENT));
		indentR = reader->ExtractReel3('+','D',0,0,PMReal(MIN_INDENT),PMReal(MAX_INDENT));
		offset = reader->ExtractReel3('+','D',0,0,PMReal(MIN_OFFSET),PMReal(MAX_OFFSET));
		type = reader->ExtractInt(0,0,18);
		refColor = reader->ExtractInt(5,-1,MAX_COLOR) ;
		tint = reader->ExtractReel2('+',MAX_TINT,0,PMReal(MIN_TINT),PMReal(MAX_TINT));
		ruleOn = reader->ExtractBooleen(kTrue);

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
		IDCall_PA(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PA::~Func_PA()
{
}


/* IDCall_PA
*/
void Func_PA::IDCall_PA(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PA::IDCall_PA-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PA::IDCall_PA -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();


		//Set the paragraph rule above attribute
		if(txtAttrHelper->AddParaRuleAboveOnAttribute(attrListPtr,ruleOn) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		if(refColor != -1)
		{
			UID colorUID = parser->getColorList(refColor).colorUID ;

			if (colorUID == kInvalidUID)
			{
				ASSERT_FAIL("Func_PA::IDCall_PA -> colorUID == kInvalidUID");
				error = PMString(kErrColorNotFound);
				break;
			}
			
			//set the color attribute (if refColor = -1, color = Text Color which is the default attribute)
			if(txtAttrHelper->AddParaRuleAboveColorAttribute(attrListPtr,colorUID) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		//set the indent left attribute
		if(txtAttrHelper->AddParaRuleAboveIndentLAttribute(attrListPtr,indentL) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the indent right attribute
		if(txtAttrHelper->AddParaRuleAboveIndentRAttribute(attrListPtr,indentR) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the offset attribute
		if(txtAttrHelper->AddParaRuleAboveOffsetAttribute(attrListPtr,offset) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the stroke attribute
		if(txtAttrHelper->AddParaRuleAboveStrokeAttribute(attrListPtr,stroke) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the tint attribute
		if(txtAttrHelper->AddParaRuleAboveTintAttribute(attrListPtr,tint) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		//set the type attribute
		if(txtAttrHelper->AddParaRuleAboveTypeAttribute(attrListPtr,type) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the mode
		if (flag)
		{
			if(txtAttrHelper->AddParaRuleAboveModeAttribute(attrListPtr,1) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			if(txtAttrHelper->AddParaRuleAboveModeAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		if (txtFrameHelper->GetCreation()) 
			txtFrameHelper->SetIsParaStyle(kTrue);
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_PP::Func_PP(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PP::Func_PP -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		stroke = reader->ExtractReel3('+','D',MIN_PARARULE_STROKE,0,PMReal(MIN_PARARULE_STROKE),PMReal(MAX_PARARULE_STROKE));
		flag = reader->ExtractBooleen(kFalse) ;
		indentL = reader->ExtractReel3('+','D',0,0,PMReal(MIN_INDENT),PMReal(MAX_INDENT));
		indentR = reader->ExtractReel3('+','D',0,0,PMReal(MIN_INDENT),PMReal(MAX_INDENT));
		offset = reader->ExtractReel3('+','D',0,0,PMReal(MIN_OFFSET),PMReal(MAX_OFFSET));
		type  = reader->ExtractInt(0,0,18);
		refColor = reader->ExtractInt(5,-1,MAX_COLOR) ;
		tint = reader->ExtractReel2('+',MAX_TINT,0,PMReal(MIN_TINT),PMReal(MAX_TINT));
		ruleOn = reader->ExtractBooleen(kTrue);

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
		IDCall_PP(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PP::~Func_PP()
{
}


/* IDCall_PP
*/
void Func_PP::IDCall_PP(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PP::IDCall_PP-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PP::IDCall_PP -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();
			
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_PP::IDCall_PP -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		//Set the paragraph rule Below attribute
		if(txtAttrHelper->AddParaRuleBelowOnAttribute(attrListPtr,ruleOn) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		if(refColor != -1)
		{
			UID colorUID = parser->getColorList(refColor).colorUID ;

			if (colorUID == kInvalidUID)
			{
				ASSERT_FAIL("Func_PA::IDCall_PA -> colorUID == kInvalidUID");
				error = PMString(kErrColorNotFound);
				break;
			}
			
			//set the color attribute (if refColor = -1, color = Text Color which is the default attribute)
			if(txtAttrHelper->AddParaRuleBelowColorAttribute(attrListPtr,colorUID) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		//set the indent left attribute
		if(txtAttrHelper->AddParaRuleBelowIndentLAttribute(attrListPtr,indentL) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the indent right attribute
		if(txtAttrHelper->AddParaRuleBelowIndentRAttribute(attrListPtr,indentR) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the offset attribute
		if(txtAttrHelper->AddParaRuleBelowOffsetAttribute(attrListPtr,offset) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the stroke attribute
		if(txtAttrHelper->AddParaRuleBelowStrokeAttribute(attrListPtr,stroke) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the tint attribute
		if(txtAttrHelper->AddParaRuleBelowTintAttribute(attrListPtr,tint) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		//set the type attribute
		if(txtAttrHelper->AddParaRuleBelowTypeAttribute(attrListPtr,type) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the mode
		if (flag)
		{
			if(txtAttrHelper->AddParaRuleBelowModeAttribute(attrListPtr,1) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			if(txtAttrHelper->AddParaRuleBelowModeAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		if (txtFrameHelper->GetCreation()) 
			txtFrameHelper->SetIsParaStyle(kTrue);
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_PR::Func_PR(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PR::Func_PR -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		indentL = reader->ExtractReel3('+','P',0,0,PMReal(0),PMReal(MAX_INDENT));
		indentR = reader->ExtractReel3('+','P',0,0,PMReal(0),PMReal(MAX_INDENT));
		alinea = reader->ExtractReel3('+','P',0,0,PMReal(-MAX_INDENT),PMReal(MAX_INDENT));

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
		IDCall_PR(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PR::~Func_PR()
{
}


/* IDCall_PR
*/
void Func_PR::IDCall_PR(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PR::IDCall_PR-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PR::IDCall_PR -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();
			
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_PR::IDCall_PR -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}


		//Set the indent first left line attribute
		if(txtAttrHelper->AddIndent1LAttribute(attrListPtr,alinea) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the indent left attribute
		if(txtAttrHelper->AddIndentLAttribute(attrListPtr,indentL) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the indent right attribute
		if(txtAttrHelper->AddIndentRAttribute(attrListPtr,indentR) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (txtFrameHelper->GetCreation()) 
			txtFrameHelper->SetIsParaStyle(kTrue);
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_PT::Func_PT(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PT::Func_PT -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		tabTable = new TabStopTable();
		while (!reader->IsEndOfCommand())
		{
			PMReal largCol = reader->ExtractReel3('+','P',0,0,-1,PMReal(MAX_INDENT));
			int32 typeTab = reader->ExtractInt(MIN_TYPETAB,MIN_TYPETAB,MAX_TYPETAB) ;
			uchar c = reader->ExtractChar();
			TabStop tab;
			tab.SetPosition(largCol);
			switch(typeTab)
			{
			case 0 :
				tab.SetAlignment(TabStop::kTabAlignLeft);
				break;

			case 1 :
				tab.SetAlignment(TabStop::kTabAlignCenter);
				break;

			case 2 :
				tab.SetAlignment(TabStop::kTabAlignRight);
				break;

			case 3 :
				tab.SetAlignment(TabStop::kTabAlignChar,c);
				break;

			default :
				tab.SetAlignment(TabStop::kTabAlignLeft);
				break;
			}

			tabTable->Insert(tab);
		}

		if(tabTable->begin()->GetPosition() == -1)
			tabTable->clear();
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
		IDCall_PT(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PT::~Func_PT()
{
	if (tabTable != nil)
		delete tabTable ;
}


/* IDCall_PT
*/
void Func_PT::IDCall_PT(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PT::IDCall_PT-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PT::IDCall_PT -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();
			
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_PT::IDCall_PT -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		//set the indent right attribute
		if(txtAttrHelper->AddTabsAttribute(attrListPtr,tabTable) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (txtFrameHelper->GetCreation()) 
			txtFrameHelper->SetIsParaStyle(kTrue);
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_CJ::Func_CJ(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CJ::Func_CJ -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		
		name = reader->ExtractString(255);

		hyphenMode = reader->ExtractBooleen(kTrue);

		capitalized = reader->ExtractBooleen(kFalse);

		minWord = reader->ExtractInt(6,MIN_HYPHWORD,MAX_HYPHWORD) ;

		beforeLast = reader->ExtractInt(3,MIN_HYPHCHAR,MAX_HYPHCHAR) ;

		afterFirst = reader->ExtractInt(3,MIN_HYPHCHAR,MAX_HYPHCHAR) ;

		limit= reader->ExtractInt(3,MIN_HYPHLIMIT,MAX_HYPHLIMIT) ;

		hyphenZone = reader->ExtractReel3('+','P',0,0,PMReal(MIN_HYPHZONE),PMReal(MAX_HYPHZONE));
		
		lastWord = reader->ExtractBooleen(kFalse);

		wordMin= reader->ExtractReel2('+',50,0,PMReal(MIN_HYPHWD),PMReal(MAX_HYPHWD))/100;
		
		wordDes= reader->ExtractReel2('+',100,0,PMReal(MIN_HYPHWD),PMReal(MAX_HYPHWD))/100;

		wordMax= reader->ExtractReel2('+',200,0,PMReal(MIN_HYPHWD),PMReal(MAX_HYPHWD))/100;

		letterMin= reader->ExtractReel2('+',0,0,PMReal(MIN_HYPHCH),PMReal(MAX_HYPHCH))/100;
		
		letterDes= reader->ExtractReel2('+',0,0,PMReal(MIN_HYPHCH),PMReal(MAX_HYPHCH))/100;

		letterMax= reader->ExtractReel2('+',0,0,PMReal(MIN_HYPHCH),PMReal(MAX_HYPHCH))/100;

		// Parameter not taken into account with InDesign (functionnality doesn't exist)
		// just here to ensure compatibilty with TCL files generated for Quark 
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

	if(reader->IsEndOfCommand())
		IDCall_CJ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_CJ::~Func_CJ()
{

}


/* IDCall_CJ
*/
void Func_CJ::IDCall_CJ(ITCLParser * parser)
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

		parser->AddCJ(name,hyphenMode,capitalized,minWord,afterFirst,beforeLast,limit,hyphenZone,lastWord,wordMin,wordMax,wordDes,letterMin,letterMax,letterDes) ;
		
		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_PC::Func_PC(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PC::Func_PC -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		
		name = reader->ExtractString(255);

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
		IDCall_PC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PC::~Func_PC()
{

}


/* IDCall_PC
*/
void Func_PC::IDCall_PC(ITCLParser * parser)
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

		bool8 hyphenMode, capitalized, lastWord ;
		int16 minWord, afterFirst, beforeLast, limit ;
		PMReal hyphenZone ;
		PMReal wordMin, wordMax, wordDes, letterMin, letterMax, letterDes ;

		if (parser->getCJ(name,hyphenMode,capitalized,minWord,afterFirst,beforeLast,limit,hyphenZone,lastWord,wordMin,wordMax,wordDes,letterMin,letterMax,letterDes) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PR::IDCall_PR-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PR::IDCall_PR -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();
			
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_PR::IDCall_PR -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		//Set the hyphen cap attribute
		if(txtAttrHelper->AddHyphenCapAttribute(attrListPtr,capitalized) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the hyphen weight (min word authorized to hyphenate)
		if(txtAttrHelper->AddShortestWordAttribute(attrListPtr,minWord) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(txtAttrHelper->AddLadderAttribute(attrListPtr,limit) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the hyphen zone
		if(txtAttrHelper->AddHyphenZoneAttribute(attrListPtr,hyphenZone) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the hyphen last word option
		if(txtAttrHelper->AddHyphenLastWordAttribute(attrListPtr,lastWord) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		//Set the number of caracter before the hyphen (between the first char and the hyphen)
		if(txtAttrHelper->AddMinAfterAttribute(attrListPtr,afterFirst) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//Set the number of character after the hyphen (between the hyphen and the last)
		if(txtAttrHelper->AddMinBeforeAttribute(attrListPtr,beforeLast) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(txtAttrHelper->AddHyphenModeAttribute(attrListPtr,hyphenMode) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//Set the letter space attributes
		if(txtAttrHelper->AddLetterspaceDesAttribute(attrListPtr,letterDes) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(txtAttrHelper->AddLetterspaceMinAttribute(attrListPtr,letterMin) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(txtAttrHelper->AddLetterspaceMaxAttribute(attrListPtr,letterMax) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//Set the word space attributes
		if(txtAttrHelper->AddWordspaceDesAttribute(attrListPtr,wordDes) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the indent left attribute
		if(txtAttrHelper->AddWordspaceMinAttribute(attrListPtr,wordMin) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//set the indent right attribute
		if(txtAttrHelper->AddWordspaceMaxAttribute(attrListPtr,wordMax) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (txtFrameHelper->GetCreation()) 
			txtFrameHelper->SetIsParaStyle(kTrue);
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_PL::Func_PL(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PL::Func_PL -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		
		numberOfChars = reader->ExtractInt(0,0,150);
		numberOfLines = reader->ExtractInt(0,0,25);
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
		IDCall_PL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PL::~Func_PL()
{
}


/* IDCall_PL
*/
void Func_PL::IDCall_PL(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_PL::IDCall_PL-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_PL::IDCall_PL -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();
			
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_PL::IDCall_PL -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		//set the indent right attribute
		if(txtAttrHelper->AddDropCapAttributes(attrListPtr, numberOfChars, numberOfLines) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (txtFrameHelper->GetCreation()) 
		{
			txtFrameHelper->SetIsParaStyle(kTrue);
		}
			
		else
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_PM::Func_PM(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_PM::Func_PM -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
	
		numTab = reader->ExtractInt(0,1,20);
		int32 typeTab = reader->ExtractInt(MIN_TYPETAB,MIN_TYPETAB,MAX_TYPETAB) ;
		uchar c = reader->ExtractChar();
		
		switch(typeTab)
		{
		case 0 :
			tab.SetAlignment(TabStop::kTabAlignLeft);
			break;

		case 1 :
			tab.SetAlignment(TabStop::kTabAlignCenter);
			break;

		case 2 :
			tab.SetAlignment(TabStop::kTabAlignRight);
			break;

		case 3 :
			tab.SetAlignment(TabStop::kTabAlignChar,c);
			break;

		default :
			tab.SetAlignment(TabStop::kTabAlignLeft);
			break;
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
		IDCall_PM(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_PM::~Func_PM()
{
}


/* IDCall_PM
*/
void Func_PM::IDCall_PM(ITCLParser * parser)
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

		InterfacePtr<ITextFrameHelper> txtFrameHelper(parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		
		// Get the current text model, if any
		TextIndex insertPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		InterfacePtr<IAttributeStrand> paraAttrStrand ((IAttributeStrand *) txtModel->QueryStrand(kParaAttrStrandBoss,IID_IATTRIBUTESTRAND)); 
		if(paraAttrStrand == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		const AttributeBossList * currentAttrList = paraAttrStrand->GetLocalOverrides(insertPos, nil);
		
		TabStopTable currentTabTable, newTabTable;
		
		InterfacePtr<ITextAttrTabSettings> txtAttrTabs ((ITextAttrTabSettings *) currentAttrList->QueryByClassID(kTextAttrTabsBoss,IID_ITEXTATTRTABS));
		if(txtAttrTabs == nil) // no local overrides for tabs, check tabs attributes in paragrap style
		{
			UID style = paraAttrStrand->GetStyleUID(insertPos, nil); 
			if(style == kInvalidUID)
			{
				error = PMString(kErrNilInterface);
				break;
			}
			
			InterfacePtr<ITextAttributes> styleAttr (parser->getCurDataBase(), style, UseDefaultIID());
			if(styleAttr == nil)
			{
				error = PMString(kErrNilInterface);
				break;
			}
			
			InterfacePtr<ITextAttrTabSettings> styleTxtAttrTabs ((ITextAttrTabSettings *) styleAttr->QueryByClassID(kTextAttrTabsBoss,IID_ITEXTATTRTABS));
			if(styleTxtAttrTabs == nil) // no local overrides for tabs, check tabs attributes in paragrap style
			{
				error = PMString(kErrNoTabsDefined);
				break;
			}
			
			currentTabTable = styleTxtAttrTabs->GetTabStopTable();
			
		}
		else
		{
			currentTabTable = txtAttrTabs->GetTabStopTable();
		}
		
		if(numTab > currentTabTable.size())
		{
			error = PMString(kErrNumTab);
			break;	
		}
		
		// Fill in the tab list with existing tabs, replacing the one at position pos	
		for(int i = 0 ; i < numTab - 1 ; ++i)
		{
			newTabTable.Insert(currentTabTable[i]);
		}
		
		tab.SetPosition(currentTabTable[numTab - 1].GetPosition());
		newTabTable.Insert(tab);
				
		for(int i = numTab ; i < currentTabTable.size() ; ++i)
		{
			newTabTable.Insert(currentTabTable[i]);
		}
			
		AttributeBossList * attrListPtr = new AttributeBossList();
		if(attrListPtr == nil)
		{		
			error = PMString(kErrNilInterface);
			break;
		}

		//set the tab attribute
		if(txtAttrHelper->AddTabsAttribute(attrListPtr,&newTabTable) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		txtFrameHelper->ApplyAttributeParaCmd(txtModel, insertPos, attrListPtr);

		delete attrListPtr ;
		
		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}






