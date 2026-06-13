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
#include "LanguageID.h"

// Interface includes
#include "IDocument.h"
#include "IWorkSpace.h"
#include "ITextAttributes.h"
#include "ISwatchList.h"
#include "ITextAttrRealNumber.h"
#include "IDocFontMgr.h"
#include "ITextAttrUID.h"
#include "ITextAttributes.h"
#include "IFontFamily.h"
#include "IFontMgr.h"
#include "ITextModel.h"
#include "IAttributeStrand.h"
#include "IRangeData.h"
#include "IIntData.h"
#include "ICompositionStyle.h"

//Project includes
#include "Textes.h"
#include "GlobalDefs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "ITextFrameHelper.h"
#include "ITxtAttrHelper.h"

#include "ITextModel.h"
#include "ITextFocus.h"

/* Constructor
*/
Func_TD::Func_TD(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TD::Func_TD -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		refColor =  reader->ExtractInt(0,0,MAX_COLOR);

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
		IDCall_TD(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TD::~Func_TD()
{
}


/* IDCall_TD
*/
void Func_TD::IDCall_TD(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TD::IDCall_TD-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TD::IDCall_TD -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}


		UID colorUID = parser->getColorList(refColor).colorUID ;

		if (colorUID == kInvalidUID)
		{
			ASSERT_FAIL("Func_TD::IDCall_TD -> colorUID == kInvalidUID");
			error = PMString(kErrColorNotFound);
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

		if(txtAttrHelper->AddColorAttribute(attrListPtr,colorUID)!=kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TC::Func_TC(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TC::Func_TC -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		fontSize = reader->ExtractReel3('+','D',12,0,PMReal(MIN_FONTSIZE),PMReal(MAX_FONTSIZE));

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
		IDCall_TC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TC::~Func_TC()
{
}


/* IDCall_TC
*/
void Func_TC::IDCall_TC(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TC::IDCall_TC-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TC::IDCall_TC -> txtAttrHelper nil");
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


		if(txtAttrHelper->AddFontSizeAttribute(attrListPtr,fontSize) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TH::Func_TH(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TH::Func_TH -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		flag = reader->ExtractBooleen(kFalse);
		if (flag)
			scale = reader->ExtractReel2('+',MIN_XSCALE,0,PMReal(MIN_XSCALE),PMReal(MAX_XSCALE));
		else
			scale = reader->ExtractReel2('+',12,0,PMReal(MIN_FONTSIZE),PMReal(MAX_FONTSIZE));

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
		IDCall_TH(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TH::~Func_TH()
{
}


/* IDCall_TH
*/
void Func_TH::IDCall_TH(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TH::IDCall_TH-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TH::IDCall_TH -> txtAttrHelper nil");
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


		if (flag)//if we change the XScale as a percentage
			scale=scale/100;//convert to a percentage
		else //if we have to calculate the percentage with the fontsize
		{
			// First, we create a text attribute real number to get the font size stored previously
			InterfacePtr<ITextAttrRealNumber> textAttrRealNumber((ITextAttrRealNumber *) attrListPtr->QueryByClassID(kTextAttrPointSizeBoss,IID_ITEXTATTRREALNUMBER));

			if (textAttrRealNumber == nil)
			{
				ASSERT_FAIL("Func_TH::IDCall_TH -> textAttrRealNumber invalid");
				error = PMString(kTlbNilInterface);
				break;
			}

			// Now we'll get the attribute's value:			
			PMReal fontsize = textAttrRealNumber->GetRealNumber();
			scale = scale/fontsize ;
			
		}



		if(txtAttrHelper->AddXScaleAttribute(attrListPtr,scale) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
		{
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (txtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtFrameHelper->ApplyAttributeCharCmd(txtModel, insertPos,attrListPtr);

			delete attrListPtr ;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_TV::Func_TV(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TV::Func_TV -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		flag = reader->ExtractBooleen(kFalse);
		if (flag)
			scale = reader->ExtractReel2('+',MIN_YSCALE,0,PMReal(MIN_YSCALE),PMReal(MAX_YSCALE));
		else
			scale = reader->ExtractReel2('+',12,0,PMReal(MIN_FONTSIZE),PMReal(MAX_FONTSIZE));

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
		IDCall_TV(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TV::~Func_TV()
{
}


/* IDCall_TV
*/
void Func_TV::IDCall_TV(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TV::IDCall_TV-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TV::IDCall_TV -> txtAttrHelper nil");
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


		if (flag)//if we change the XScale as a percentage
			scale=scale/100;//convert to a percentage
		else //if we have to calculate the percentage with the fontsize
		{
			// First, we create a text attribute real number to get the font size stored previously
			InterfacePtr<ITextAttrRealNumber> textAttrRealNumber((ITextAttrRealNumber *) attrListPtr->QueryByClassID(kTextAttrPointSizeBoss,IID_ITEXTATTRREALNUMBER));

			if (textAttrRealNumber == nil)
			{
				ASSERT_FAIL("Func_TV::IDCall_TV -> textAttrRealNumber invalid");
				error = PMString(kTlbNilInterface);
				break;
			}

			// Now we'll get the attribute's value:			
			PMReal fontsize = textAttrRealNumber->GetRealNumber();
			scale = scale/fontsize ;
			
		}

		if(txtAttrHelper->AddYScaleAttribute(attrListPtr,scale) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TA::Func_TA(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TA::Func_TA -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		tracking = reader->ExtractReel2('+',0,0,PMReal(MIN_TRACKING),PMReal(MAX_TRACKING))/PMReal(1000);
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
		IDCall_TA(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TA::~Func_TA()
{
}


/* IDCall_TA
*/
void Func_TA::IDCall_TA(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TA::IDCall_TA-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TA::IDCall_TA -> txtAttrHelper nil");
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


		if(txtAttrHelper->AddTrackKernAttribute(attrListPtr,tracking) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TL::Func_TL(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TL::Func_TL -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		baseLine = reader->ExtractReel3('+','D',0,0,PMReal(MIN_BL),PMReal(MAX_BL));
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
		IDCall_TL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TL::~Func_TL()
{
}


/* IDCall_TL
*/
void Func_TL::IDCall_TL(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TL::IDCall_TL-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TL::IDCall_TL -> txtAttrHelper nil");
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


		if(txtAttrHelper->AddBLShiftAttribute(attrListPtr,baseLine) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TT::Func_TT(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TT::Func_TT -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		trame = reader->ExtractReel2('+',100,0,PMReal(MIN_TINT),PMReal(MAX_TINT));
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
		IDCall_TT(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TT::~Func_TT()
{
}


/* IDCall_TT
*/
void Func_TT::IDCall_TT(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TT::IDCall_TT-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TT::IDCall_TT -> txtAttrHelper nil");
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


		if(txtAttrHelper->AddTintAttribute(attrListPtr,trame) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TO::Func_TO(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TT::Func_TO -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}
	try
	{
		reader->MustEgal();
		overPrintBool = reader->ExtractBooleen(kFalse);
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
		IDCall_TO(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TO::~Func_TO()
{
}

/* IDCall_TO
*/
void Func_TO::IDCall_TO(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TT::IDCall_TO-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TT::IDCall_TO -> txtAttrHelper nil");
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


		if(txtAttrHelper->AddOverPrintFillAttribute(attrListPtr, overPrintBool) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TN::Func_TN(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TN::Func_TN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		fontName =  reader->ExtractString(255);

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
		IDCall_TN(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TN::~Func_TN()
{
}


/* IDCall_TN
*/
void Func_TN::IDCall_TN(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TN::IDCall_TN-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TN::IDCall_TN -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		IDocument * doc = parser->getCurDoc();

		InterfacePtr<IDocFontMgr> iDocFontMgr(doc->GetDocWorkSpace(), UseDefaultIID());
		if (iDocFontMgr == nil) 
		{
			ASSERT_FAIL("Func_TN::IDCall_TN -> iDocFontMgr nil");
			error = PMString(kErrNilInterface);
			break;
		}
		// get the uid for the font specified in tokenval (e.g. "Times")

		// if it doesn't exist, don't create it!
		UID fontUID = kInvalidUID;
		PMString dummy;	
		if(fontName.IsEmpty())
			fontUID = iDocFontMgr->GetDefaultFamilyUID();
		else
			fontUID= iDocFontMgr->GetFontFamilyUID(fontName, dummy, kTrue);

		if(fontUID == kInvalidUID)
		{
			error = PMString(kErrFontNotFound);
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


		if(txtAttrHelper->AddFontAttribute(attrListPtr,fontUID) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TS::Func_TS(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TS::Func_TS -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		value =  reader->ExtractInt(0,0,0x3FFF);

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
		IDCall_TS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TS::~Func_TS()
{
}


/* IDCall_TS
*/
void Func_TS::IDCall_TS(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TS::IDCall_TS-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TS::IDCall_TS -> txtAttrHelper nil");
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


		bool8 toSetNormal = kTrue ;

		int16 fontStyle;

		// Bold & Italic attributes
		if(value & 0x0001)
		{
			fontStyle = 1;
			if(value & 0x0002)
				fontStyle = 3;
		}
		else if(value & 0x0002)
			fontStyle = 2;
		else
			fontStyle = 0;

		if(txtAttrHelper->AddFontStyleAttribute(attrListPtr,fontStyle) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		// Underline attribute
		if(value & 0x0004)
		{
			if(txtAttrHelper->AddUnderlineAttribute(attrListPtr,kTrue) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			if(txtAttrHelper->AddUnderlineAttribute(attrListPtr,kFalse) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Word Underline attribute (not yet implemented in InDesign)
		if(value & 0x0008)
		{
		}

		// StrikeThrough attribute 
		if(value & 0x0010)
		{
			if(txtAttrHelper->AddStrikeThruAttribute(attrListPtr,kTrue) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			if(txtAttrHelper->AddStrikeThruAttribute(attrListPtr,kFalse) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Outline attribute (not yet implemented in InDesign)
		if(value & 0x0020)
		{
		}
		
		// Shadow attribute (not yet implemented in InDesign)
		if(value & 0x0040)
		{
		}

		// All Caps attribute
		if(value & 0x0080)
		{
			if(txtAttrHelper->AddCapitalAttribute(attrListPtr,2) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			toSetNormal = kFalse;
		}

		// Small Caps attribute
		if(value & 0x0100)
		{
			if(txtAttrHelper->AddCapitalAttribute(attrListPtr,1) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			toSetNormal = kFalse;
		}

		//if there isn't capital attribute, we add the normal capital attribute
		if(toSetNormal)
		{
			if(txtAttrHelper->AddCapitalAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		toSetNormal = kTrue;

		// SuperScript attribute
		if(value & 0x0200)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,1) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			toSetNormal = kFalse;
		}

		// SubScript attribute
		if(value & 0x0400)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,2) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			toSetNormal = kFalse;
		}

		// Superior attribute
		if(value & 0x0800)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,3) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			toSetNormal = kFalse;
		}

		// Inferior attribute
		if(value & 0x1000)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,4) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			toSetNormal = kFalse;
		}

		//if there isn't capital attribute, we add the normal capital attribute
		if(toSetNormal)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// NoBreak attribute
		if(value & 0x2000)
		{
			if(txtAttrHelper->AddNoBreakAttribute(attrListPtr,kTrue) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		else
		{
			if(txtAttrHelper->AddNoBreakAttribute(attrListPtr,kFalse) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TZ::Func_TZ(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TZ::Func_TZ -> reader nil");
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
		IDCall_TZ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TZ::~Func_TZ()
{
}


/* IDCall_TZ
*/
void Func_TZ::IDCall_TZ(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TZ::IDCall_TZ-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TZ::IDCall_TZ -> txtAttrHelper nil");
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


		if(txtAttrHelper->AddNoBreakAttribute(attrListPtr,kTrue) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TI::Func_TI(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TI::Func_TI -> reader nil");
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
		IDCall_TI(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TI::~Func_TI()
{
}


/* IDCall_TI
*/
void Func_TI::IDCall_TI(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TI::IDCall_TI-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TI::IDCall_TI -> txtAttrHelper nil");
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


		if(txtAttrHelper->AddNoBreakAttribute(attrListPtr,kFalse) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TP::Func_TP(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TP::Func_TP -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		refFont =  reader->ExtractInt(0,MIN_FONT,MAX_FONT);

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
		IDCall_TP(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TP::~Func_TP()
{
}


/* IDCall_TP
*/
void Func_TP::IDCall_TP(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TP::IDCall_TP-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TP::IDCall_TP -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		
		PMString fontName("");

		if(parser->GetFontName(refFont,fontName) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}


		IDocument * doc = parser->getCurDoc();

		InterfacePtr<IDocFontMgr> iDocFontMgr(doc->GetDocWorkSpace(), UseDefaultIID());
		if (iDocFontMgr == nil) 
		{
			ASSERT_FAIL("Func_TP::IDCall_TP -> iDocFontMgr nil");
			error = PMString(kErrNilInterface);
			break;
		}
		// get the uid for the font specified in tokenval (e.g. "Times")

		// if it doesn't exist, don't create it!
		UID fontUID = iDocFontMgr->GetFontGroupUID(fontName, kTrue); 

		AttributeBossList * attrListPtr ;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();


		if(txtAttrHelper->AddFontAttribute(attrListPtr,fontUID) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TB::Func_TB(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TB::Func_TB -> reader nil");
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
		IDCall_TB(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TB::~Func_TB()
{
}


/* IDCall_TB
*/
void Func_TB::IDCall_TB(ITCLParser * parser)
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

		parser->InsertCharacter(kTextChar_Tab);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_LF::Func_LF(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_LF::Func_LF -> reader nil");
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
		IDCall_LF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_LF::~Func_LF()
{
}


/* IDCall_LF
*/
void Func_LF::IDCall_LF(ITCLParser * parser)
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

		parser->InsertCharacter(kTextChar_LF);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}

/* Constructor
*/
Func_II::Func_II(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_II::Func_II -> reader nil");
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
		IDCall_II(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_II::~Func_II()
{
}


/* IDCall_II
*/
void Func_II::IDCall_II(ITCLParser * parser)
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

		parser->InsertCharacter(kTextChar_IndentToHere);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_TF::Func_TF(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TF::Func_TF -> reader nil");
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
		IDCall_TF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TF::~Func_TF()
{
}


/* IDCall_TF
*/
void Func_TF::IDCall_TF(ITCLParser * parser)
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

		TextIndex insertPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
			
		InterfacePtr<ICommand> breakCommand(CmdUtils::CreateCommand(kInsertBreakCharacterCmdBoss)); 
		if(breakCommand == nil) 
		{ 
			error = PMString(kErrNilInterface);
			break; 
		} 
		breakCommand->SetItemList(UIDList(::GetUIDRef(txtModel))); 
		
		InterfacePtr<IRangeData> rangeData(breakCommand, UseDefaultIID());  
		if(rangeData == nil) 
		{
			error = PMString(kErrNilInterface);
			break; 
		}
		
		rangeData->Set(insertPos,insertPos);
		
		InterfacePtr<IIntData> intData(breakCommand,UseDefaultIID()); 
		if(intData == nil) 
		{ 
			error = PMString(kErrNilInterface);
			break; 
		} 
		intData->Set(Text::kAtColumn); 
		
		if(CmdUtils::ProcessCommand(breakCommand) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_EC::Func_EC(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_EC::Func_EC -> reader nil");
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
		IDCall_EC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_EC::~Func_EC()
{
}


/* IDCall_EC
*/
void Func_EC::IDCall_EC(ITCLParser * parser)
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

		parser->InsertCharacter(kTextChar_EmSpace);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_ED::Func_ED(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_ED::Func_ED -> reader nil");
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
		IDCall_ED(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_ED::~Func_ED()
{
}


/* IDCall_ED
*/
void Func_ED::IDCall_ED(ITCLParser * parser)
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

		parser->InsertCharacter(kTextChar_EnSpace);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_EF::Func_EF(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_EF::Func_EF -> reader nil");
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
		IDCall_EF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_EF::~Func_EF()
{
}


/* IDCall_EF
*/
void Func_EF::IDCall_EF(ITCLParser * parser)
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

		parser->InsertCharacter(kTextChar_ThinSpace);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_EP::Func_EP(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_EP::Func_EP -> reader nil");
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
		IDCall_EP(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_EP::~Func_EP()
{
}


/* IDCall_EP
*/
void Func_EP::IDCall_EP(ITCLParser * parser)
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

		parser->InsertCharacter(kTextChar_FigureSpace);
		
		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_TK::Func_TK(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TK::Func_TK -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		value = reader->ExtractReel2('+',0,0,PMReal(MIN_TRACKING),PMReal(MAX_TRACKING))/PMReal(1000);
		autoKern = reader->ExtractBooleen(kFalse);
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
		IDCall_TK(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TK::~Func_TK()
{
}


/* IDCall_TK
*/
void Func_TK::IDCall_TK(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TK::IDCall_TK-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TK::IDCall_TK -> txtAttrHelper nil");
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

		if(autoKern)
			value = kAutoKernMagicNumber;

		if(txtAttrHelper->AddPairKernAttribute(attrListPtr,value) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_NB::Func_NB(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_NB::Func_NB -> reader nil");
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
		IDCall_NB(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_NB::~Func_NB()
{
}


/* IDCall_SB
*/
void Func_NB::IDCall_NB(ITCLParser * parser)
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

		TextIndex insertPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
			
		InterfacePtr<ICommand> breakCommand(CmdUtils::CreateCommand(kInsertBreakCharacterCmdBoss)); 
		if(breakCommand == nil) 
		{ 
			error = PMString(kErrNilInterface);
			break; 
		} 
		breakCommand->SetItemList(UIDList(::GetUIDRef(txtModel))); 
		
		InterfacePtr<IRangeData> rangeData(breakCommand, UseDefaultIID());  
		if(rangeData == nil) 
		{
			error = PMString(kErrNilInterface);
			break; 
		}
		
		rangeData->Set(insertPos,insertPos);
		
		InterfacePtr<IIntData> intData(breakCommand,UseDefaultIID()); 
		if(intData == nil) 
		{ 
			error = PMString(kErrNilInterface);
			break; 
		} 
		intData->Set(Text::kAtFrameBox); 
		
		if(CmdUtils::ProcessCommand(breakCommand) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}

/* Constructor
*/
Func_TE::Func_TE(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_II::Func_II -> reader nil");
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
		IDCall_TE(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TE::~Func_TE()
{
}


/* IDCall_TE
*/
void Func_TE::IDCall_TE(ITCLParser * parser)
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

		parser->InsertCharacter(kTextChar_RightAlignedTab);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}

/* Constructor
*/
Func_TG::Func_TG(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TS::Func_TS -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		value =  reader->ExtractInt(0,0,0x3FFF);

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
		IDCall_TG(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TG::~Func_TG()
{
}


/* IDCall_TG
*/
void Func_TG::IDCall_TG(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TS::IDCall_TS-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TS::IDCall_TS -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr;
		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();

		int16 fontStyle;

		// Bold & Italic attributes
		if(value & 0x0001)
		{
			fontStyle = 1;
			if(value & 0x0002)
				fontStyle = 3;
		}
		else if(value & 0x0002)
			fontStyle = 2;
		else
			fontStyle = 0;

		if(fontStyle != 0)
		{
			if(txtAttrHelper->AddFontStyleAttribute(attrListPtr,fontStyle) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		
		// Underline attribute
		if(value & 0x0004)
		{
			if(txtAttrHelper->AddUnderlineAttribute(attrListPtr,kTrue) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Word Underline attribute (not yet implemented in InDesign)
		if(value & 0x0008)
		{
		}

		// StrikeThrough attribute 
		if(value & 0x0010)
		{
			if(txtAttrHelper->AddStrikeThruAttribute(attrListPtr,kTrue) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Outline attribute (not yet implemented in InDesign)
		if(value & 0x0020)
		{
		}
		
		// Shadow attribute (not yet implemented in InDesign)
		if(value & 0x0040)
		{
		}

		// All Caps attribute
		if(value & 0x0080)
		{
			if(txtAttrHelper->AddCapitalAttribute(attrListPtr,2) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Small Caps attribute
		if(value & 0x0100)
		{
			if(txtAttrHelper->AddCapitalAttribute(attrListPtr,1) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// SuperScript attribute
		if(value & 0x0200)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,1) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// SubScript attribute
		if(value & 0x0400)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,2) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Superior attribute
		if(value & 0x0800)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,3) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Inferior attribute
		if(value & 0x1000)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,4) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// NoBreak attribute
		if(value & 0x2000)
		{
			if(txtAttrHelper->AddNoBreakAttribute(attrListPtr,kTrue) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		
		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TJ::Func_TJ(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_TJ::Func_TJ -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		value =  reader->ExtractInt(0,0,0x3FFF);

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
		IDCall_TJ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_TJ::~Func_TJ()
{
}


/* IDCall_TJ
*/
void Func_TJ::IDCall_TJ(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TS::IDCall_TS-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TS::IDCall_TS -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr;
		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();

		// Bold & Italic attributes
		if(value & 0x0001 || value & 0x0002)
		{
			if(txtAttrHelper->AddFontStyleAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
		
		// Underline attribute
		if(value & 0x0004)
		{
			if(txtAttrHelper->AddUnderlineAttribute(attrListPtr,kFalse) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Word Underline attribute (not yet implemented in InDesign)
		if(value & 0x0008)
		{
		}

		// StrikeThrough attribute 
		if(value & 0x0010)
		{
			if(txtAttrHelper->AddStrikeThruAttribute(attrListPtr,kFalse) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Outline attribute (not yet implemented in InDesign)
		if(value & 0x0020)
		{
		}
		
		// Shadow attribute (not yet implemented in InDesign)
		if(value & 0x0040)
		{
		}

		// All Caps attribute
		if(value & 0x0080)
		{
			if(txtAttrHelper->AddCapitalAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Small Caps attribute
		if(value & 0x0100)
		{
			if(txtAttrHelper->AddCapitalAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// SuperScript attribute
		if(value & 0x0200)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// SubScript attribute
		if(value & 0x0400)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Superior attribute
		if(value & 0x0800)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// Inferior attribute
		if(value & 0x1000)
		{
			if(txtAttrHelper->AddPositionAttribute(attrListPtr,0) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		// NoBreak attribute
		if(value & 0x2000)
		{
			if(txtAttrHelper->AddNoBreakAttribute(attrListPtr,kFalse) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_CL::Func_CL(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CL::Func_CL -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		language = reader->ExtractInt(0,0,101);

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
		IDCall_CL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_CL::~Func_CL()
{
}


/* IDCall_CL
*/
void Func_CL::IDCall_CL(ITCLParser * parser)
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
			ASSERT_FAIL("Func_CL::IDCall_CL-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_CL::IDCall_CL -> txtAttrHelper nil");
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

		LanguageID lang;
		switch (language)
		{
			case 0 : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishUS); break;
			case 1 : lang = MakeLanguageID(kLanguageFrench,kSubLanguageFrench); break;
			case 2 : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishUK); break;
			case 3 : lang = MakeLanguageID(kLanguageGerman,kSubLanguageGerman); break;
			case 4 : lang = MakeLanguageID(kLanguageItalian,kSubLanguageItalian); break;
			case 5 : lang = MakeLanguageID(kLanguageDutch,kSubLanguageDutch); break;
			case 6 : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishUS); break;
			case 7 : lang = MakeLanguageID(kLanguageSwedish,kSubLanguageSwedish); break;
			case 8 : lang = MakeLanguageID(kLanguageSpanish,kSubLanguageSpanish); break;
			case 9 : lang = MakeLanguageID(kLanguageDanish,kSubLanguageNeutral); break;
			case 10 : lang = MakeLanguageID(kLanguagePortuguese,kSubLanguagePortuguese); break;
			case 11 : lang = MakeLanguageID(kLanguageFrench,kSubLanguageFrenchCanadian); break;
			case 12 : lang = MakeLanguageID(kLanguageNorwegian,kSubLanguageNorwegianBokmal); break;
			case 13 : lang = MakeLanguageID(kLanguageHebrew,kSubLanguageNeutral); break;
			case 14 : lang = MakeLanguageID(kLanguageJapanese,kSubLanguageNeutral); break;
			case 15 : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishAustralian); break;
			case 16 : lang = MakeLanguageID(kLanguageArabic,kSubLanguageArabic); break;
			case 17 : lang = MakeLanguageID(kLanguageFinnish,kSubLanguageNeutral); break;
			case 18 : lang = MakeLanguageID(kLanguageFrench,kSubLanguageFrenchSwiss); break;
			case 19 : lang = MakeLanguageID(kLanguageGerman,kSubLanguageGermanSwiss); break;
			case 20 : lang = MakeLanguageID(kLanguageGreek,kSubLanguageNeutral); break;
			case 21 : lang = MakeLanguageID(kLanguageIcelandic,kSubLanguageNeutral); break;
			case 22 : lang = MakeLanguageID(kLanguageMaltese,kSubLanguageNeutral); break;
			case 24 : lang = MakeLanguageID(kLanguageTurkish,kSubLanguageNeutral); break;
			case 25 : lang = MakeLanguageID(kLanguageCroatian,kSubLanguageNeutral); break;
			case 33 : lang = MakeLanguageID(kLanguageHindi,kSubLanguageNeutral); break;
			case 34 : lang = MakeLanguageID(kLanguageUrdu,kSubLanguageUrduPakistan); break;
			case 36 : lang = MakeLanguageID(kLanguageItalian,kSubLanguageItalianSwiss); break;
			case 39 : lang = MakeLanguageID(kLanguageRomanian,kSubLanguageNeutral); break;
			case 41 : lang = MakeLanguageID(kLanguageLithuanian,kSubLanguageNeutral); break;
			case 42 : lang = MakeLanguageID(kLanguagePolish,kSubLanguageNeutral); break;
			case 43 : lang = MakeLanguageID(kLanguageHungarian,kSubLanguageNeutral); break;
			case 44 : lang = MakeLanguageID(kLanguageEstonian,kSubLanguageNeutral); break;
			case 45 : lang = MakeLanguageID(kLanguageLatvian,kSubLanguageNeutral); break;
			case 46 : lang = MakeLanguageID(kLanguageSami,kSubLanguageNeutral); break;
			case 48 : lang = MakeLanguageID(kLanguageFarsi,kSubLanguageNeutral); break;
			case 49 : lang = MakeLanguageID(kLanguageRussian,kSubLanguageNeutral); break;
			case 50 : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishEire); break;
			case 51 : lang = MakeLanguageID(kLanguageKorean,kSubLanguageNeutral); break;
			case 52 : lang = MakeLanguageID(kLanguageChinese,kSubLanguageChineseSimplified); break;
			case 53 : lang = MakeLanguageID(kLanguageChinese,kSubLanguageChineseTraditional); break;
			case 54 : lang = MakeLanguageID(kLanguageThai,kSubLanguageNeutral); break;
			case 56 : lang = MakeLanguageID(kLanguageCzech,kSubLanguageNeutral); break;
			case 57 : lang = MakeLanguageID(kLanguageSlovak,kSubLanguageNeutral); break;
			case 60 : lang = MakeLanguageID(kLanguageBengali,kSubLanguageNeutral); break;
			case 61 : lang = MakeLanguageID(kLanguageByeloRussian,kSubLanguageNeutral); break;
			case 62 : lang = MakeLanguageID(kLanguageUkrainian,kSubLanguageNeutral); break;
			case 65 : lang = MakeLanguageID(kLanguageSerbian,kSubLanguageNeutral); break;
			case 66 : lang = MakeLanguageID(kLanguageSlovenian,kSubLanguageNeutral); break;
			case 67 : lang = MakeLanguageID(kLanguageMacedonian,kSubLanguageNeutral); break;
			case 68 : lang = MakeLanguageID(kLanguageCroatian,kSubLanguageNeutral); break;
			case 71 : lang = MakeLanguageID(kLanguagePortuguese,kSubLanguagePortugueseBrazilian); break;
			case 72 : lang = MakeLanguageID(kLanguageBulgarian,kSubLanguageNeutral); break;
			case 73 : lang = MakeLanguageID(kLanguageCatalan,kSubLanguageNeutral); break;
			case 75 : lang = MakeLanguageID(kLanguageGaelic,kSubLanguageGaelicScotland); break;
			case 81 : lang = MakeLanguageID(kLanguageGaelic,kSubLanguageGaelicIreland); break;
			case 82 : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishCanadian); break;
			case 84 : lang = MakeLanguageID(kLanguageArmenian,kSubLanguageNeutral); break;
			case 85 : lang = MakeLanguageID(kLanguageGeorgian,kSubLanguageNeutral); break;
			case 92 : lang = MakeLanguageID(kLanguageGerman,kSubLanguageGermanAustrian); break;
			case 94 : lang = MakeLanguageID(kLanguageGujarati,kSubLanguageNeutral); break;
			case 95 : lang = MakeLanguageID(kLanguagePunjabi,kSubLanguageNeutral); break;
			case 96 : lang = MakeLanguageID(kLanguageUrdu,kSubLanguageUrduIndia); break;
			case 97 : lang = MakeLanguageID(kLanguageVietnamese,kSubLanguageUrduIndia); break;			
			case 98 : lang = MakeLanguageID(kLanguageFrench ,kSubLanguageFrenchBelgian); break;
			case 100 : lang = MakeLanguageID(kLanguageChinese ,kSubLanguageChineseSingapore); break;
			case 101 : lang = MakeLanguageID(kLanguageNorwegian,kSubLanguageNorwegianNynorsk); break;
	
			default : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishUS); break;
		}


		if(txtAttrHelper->AddLanguageAttribute(attrListPtr, parser->getCurDoc(), lang) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TM::Func_TM(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{

		reader->MustEgal();

		fontStyle =  reader->ExtractString(255);

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
		IDCall_TM(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_TM
*/
void Func_TM::IDCall_TM(ITCLParser * parser)
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
			ASSERT_FAIL("Func_TM::IDCall_TM-> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_TM::IDCall_TM -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrListPtr;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();


		if(txtAttrHelper->AddFontStyleAttribute(attrListPtr,fontStyle) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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
Func_TY::Func_TY(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
		throw TCLError(PMString(kErrNilInterface));

	try
	{

		reader->MustEgal();

		method =  reader->ExtractInt(0,0,2);

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
		IDCall_TY(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_TY
*/
void Func_TY::IDCall_TY(ITCLParser * parser)
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

		AttributeBossList * attrListPtr;

		if (txtFrameHelper->GetCreation()) //if we are creating a style we have to use the general attribute boss list
		{
			//we get the pointer of the attrBossList
			attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		}
		else 
			attrListPtr = new AttributeBossList();


		if(txtAttrHelper->AddPairKernMethodAttribute(attrListPtr,method) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if (!txtFrameHelper->GetCreation()) //if we are not creating a style we have to apply the attribute
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


Func_NT::Func_NT(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_NT::Func_NT -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}
	
	try
	{
		
		reader->MustEgal();
		refBloc = reader->ExtractTCLRef(kDefaultTCLRef);
		value =  reader->ExtractInt(0,0,0x3FFF);
		
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
		IDCall_NT(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
 */
Func_NT::~Func_NT()
{
}


/* IDCall_NT
 */
void Func_NT::IDCall_NT(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");
	
	do
	{
		
		status = kSuccess ;
		
	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
	
}


