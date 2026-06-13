/*
//	File:	Styles.cpp
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
#include "IStyleGroupManager.h"
#include "IStyleUnpacker.h"
#include "ITextAttributes.h"
#include "IAttrReport.h"

//Project includes
#include "Styles.h"
#include "GlobalDefs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "ITextFrameHelper.h"
#include "IStyleHelper.h"

#include "ITextModel.h"
#include "ITextFocus.h"


/* Constructor
*/
Func_SN::Func_SN(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SN::Func_SN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		paraStyleName =  reader->ExtractString(255);

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
		IDCall_SN(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_SN::~Func_SN()
{
}


/* IDCall_SN
*/
void Func_SN::IDCall_SN(ITCLParser * parser)
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
			ASSERT_FAIL("Func_SN::IDCall_SN -> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(txtFrameHelper->GetCreation())
		{
			error = PMString(kErrNotAllowedDuringDefine);
			break;
		}

		// get the para style nametable
		IDocument * doc = parser->getCurDoc();
		
		InterfacePtr<IWorkspace> iWorkspace (doc->GetDocWorkSpace(),IID_IWORKSPACE);
		if (iWorkspace == nil)
		{
			ASSERT_FAIL("Func_SN::IDCall_SN -> iWorkspace nil");
			error = PMString(kErrNilInterface);
			break;
		}
		InterfacePtr<IStyleGroupManager> paraStyleNameTable(iWorkspace,IID_IPARASTYLEGROUPMANAGER);
		if (paraStyleNameTable == nil)
		{
			ASSERT_FAIL("Func_SN::IDCall_SN -> paraStyleNameTable nil");
			error = PMString(kErrNilInterface);
			break;
		}
		// we can now get the style outof the style table
		UID paraStyleUID = paraStyleNameTable->FindByName(paraStyleName);
		if (paraStyleUID == kInvalidUID)
		{
			error = PMString(kErrStyleNotFound);
			break;
		}

		TextIndex insertPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		txtFrameHelper->ApplyParaStyleCmd(txtModel, insertPos, paraStyleUID) ;
		
		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_SB::Func_SB(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SN::Func_SN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		charStyleName =  reader->ExtractString(255);

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
		IDCall_SB(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_SB::~Func_SB()
{
}


/* IDCall_SB
*/
void Func_SB::IDCall_SB(ITCLParser * parser)
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
			ASSERT_FAIL("Func_SB::IDCall_SB -> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(txtFrameHelper->GetCreation())
		{
			error = PMString(kErrNotAllowedDuringDefine);
			break;
		}

		// get the para style nametable
		IDocument * doc = parser->getCurDoc();
		
		InterfacePtr<IWorkspace> iWorkspace (doc->GetDocWorkSpace(),IID_IWORKSPACE);
		if (iWorkspace == nil)
		{
			ASSERT_FAIL("Func_SB::IDCall_SB -> iWorkspace nil");
			error = PMString(kErrNilInterface);
			break;
		}
		InterfacePtr<IStyleGroupManager> charStyleNameTable(iWorkspace,IID_ICHARSTYLEGROUPMANAGER);
		if (charStyleNameTable == nil)
		{
			ASSERT_FAIL("Func_SB::IDCall_SB -> charStyleNameTable nil");
			error = PMString(kErrNilInterface);
			break;
		}
		// we can now get the style outof the style table
		UID charStyleUID = kInvalidUID;
		
		if(charStyleName == "[None]") // No character style
			charStyleUID =  charStyleNameTable->GetRootStyleUID();
		else
		{	
			charStyleUID = charStyleNameTable->FindByName(charStyleName);
			if (charStyleUID == kInvalidUID)
			{	
				error = PMString(kErrStyleNotFound);
				break;
			}
		}

		TextIndex insertPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		txtFrameHelper->ApplyCharStyleCmd(txtModel, insertPos,charStyleUID) ;
		
		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_SD::Func_SD(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SD::Func_SD -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		styleNum = reader->ExtractInt(0,0,MAX_STYLE);
		styleName =  reader->ExtractString(255);
		nextStyle = reader->ExtractInt(0,-1,MAX_STYLE); // -1 means Root Style (used for paragraph style)
		basedOn = reader->ExtractInt(0,-1,MAX_STYLE); 
		isParaStyle= reader->ExtractBooleen(kFalse);

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
		IDCall_SD(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_SD::~Func_SD()
{
}


/* IDCall_SD
*/
void Func_SD::IDCall_SD(ITCLParser * parser)
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
			ASSERT_FAIL("Func_SD::IDCall_SD -> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if(txtFrameHelper->GetCreation())
		{
			error = PMString(kAlreadyDefiningStyle);
			break;
		}

		//made the styles' table
		if(parser->AddStyle(styleNum,styleName) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//Clean the attributeList and the values which concern style
		txtFrameHelper->Clean();

		txtFrameHelper->SetStyleName(styleName) ;

		txtFrameHelper->SetBasedOnNum(basedOn);
		txtFrameHelper->SetNextStyleNum(nextStyle);

		txtFrameHelper->SetCreation(kTrue) ;

		txtFrameHelper->SetIsParaStyle(isParaStyle);

		status=kSuccess ;


	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_SF::Func_SF(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SF::Func_SF -> reader nil");
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
		IDCall_SF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_SF::~Func_SF()
{
}


/* IDCall_SF
*/
void Func_SF::IDCall_SF(ITCLParser * parser)
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
			ASSERT_FAIL("Func_SD::IDCall_SD -> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IStyleHelper> styleHelper(parser->QueryStyleHelper());
		if(styleHelper == nil)
		{		
			ASSERT_FAIL("Func_SF::IDCall_SF -> styleHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(!txtFrameHelper->GetCreation())
		{
			error = PMString(kErrNotDefining);
			break;
		}

		// get the para style nametable
		IDocument * doc = parser->getCurDoc();

		UIDRef docWorkSpaceRef = doc->GetDocWorkSpace() ;

		//we get the pointer of the attrBossList
		AttributeBossList * attrListPtr = txtFrameHelper->GetAttrListPtr() ;
		if(attrListPtr == nil)
		{		
			ASSERT_FAIL("Func_SF::IDCall_SF -> attrListPtr nil");
			error = PMString(kErrNilInterface);
			break;
		}

		int32 basedOnNum = txtFrameHelper->GetBasedOnNum();
		int32 nextStyleNum = txtFrameHelper->GetNextStyleNum();

		PMString basedOnName, nextStyleName;
		if(basedOnNum > 0)
		{
			if(parser->GetStyleName(basedOnNum, basedOnName) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}					
		}
		
		if(nextStyleNum > 0)
		{				
			if(parser->GetStyleName(nextStyleNum, nextStyleName) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}					
		}
		 
		InterfacePtr<IStyleGroupManager> charStyleNameTable (docWorkSpaceRef,IID_ICHARSTYLEGROUPMANAGER);
		if(charStyleNameTable == nil)
		{		
			ASSERT_FAIL("Func_SF::IDCall_SF -> charStyleNameTable nil");
			error = PMString(kErrNilInterface);
			break;
		}

		if (txtFrameHelper->IsParaStyle())
		{
				
			// Create a paragraph style 
			InterfacePtr<IStyleGroupManager> paraStyleNameTable (docWorkSpaceRef,IID_IPARASTYLEGROUPMANAGER);
			if(paraStyleNameTable == nil)
			{		
				ASSERT_FAIL("Func_SF::IDCall_SF -> paraStyleNameTable nil");
				error = PMString(kErrNilInterface);
				break;
			}

			UID basedOn = kInvalidUID;
			if(basedOnNum == 0)		
			{		
				// try to set the basedOn style with the default paragraph style
				PMString defaultStyleName("NormalParagraphStyle", PMString::kUnknownEncoding);
				basedOn = paraStyleNameTable->FindByName(defaultStyleName);
			}
			else if(basedOnNum == -1)
				basedOn = paraStyleNameTable->GetRootStyleUID();
			else
				basedOn = paraStyleNameTable->FindByName(basedOnName);
			
			UID nextStyle = kInvalidUID;
			if(!nextStyleName.IsEmpty())
				nextStyle = paraStyleNameTable->FindByName(nextStyleName);
			else if(nextStyleNum == -1)
				nextStyle = paraStyleNameTable->GetRootStyleUID();

			if((status = styleHelper->CreateParaStyleCmd(txtFrameHelper->GetStyleName(),docWorkSpaceRef,attrListPtr,basedOn,nextStyle,kFalse)) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			// we used to create an associated character style : it's not correct in the reverse TCL process
			// because it may exist a character style with the same name and which has nothing to do with
			// this paragraph style

			//int32 count = attrListPtr->CountBosses() ;
			//
			//AttributeBossList * attrListTmp = attrListPtr->Duplicate();
			//
			//for (int32 i=0 ;i<count ;i++)
			//{
			//	InterfacePtr<IAttrReport> attrReport ((IAttrReport *) attrListPtr->QueryBossN(i,IID_IATTRREPORT));
			//	if(attrReport == nil)
			//	{		
			//		ASSERT_FAIL("Func_SF::IDCall_SF -> attrReport nil");
			//		error = PMString(kErrNilInterface);
			//		break;
			//	}

			//	if (attrReport->IsParagraphAttribute())
			//		attrListTmp->ClearOverride(attrListPtr->GetClassN(i)) ;
			//}
			//
			//txtFrameHelper->SetAttrList(*attrListTmp);
			//
			//delete attrListTmp;

			//basedOn = nextStyle = kInvalidUID;
			//if(!basedOnName.IsEmpty())
			//	basedOn = charStyleNameTable->FindByName(basedOnName);

			//if((status = styleHelper->CreateCharStyleCmd(txtFrameHelper->GetStyleName(),docWorkSpaceRef,txtFrameHelper->GetAttrListPtr(), basedOn,kFalse)) != kSuccess)
			//{
			//	error = ErrorUtils::PMGetGlobalErrorString();
			//	break;
			//}

		}	
		else
		{
			UID basedOn = kInvalidUID;
				
			if(!basedOnName.IsEmpty())
				basedOn = charStyleNameTable->FindByName(basedOnName);

			if((status = styleHelper->CreateCharStyleCmd(txtFrameHelper->GetStyleName(),docWorkSpaceRef, attrListPtr, basedOn, kFalse)) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		//we have to clean the attributeboss list and the isParaStyle booleen in the txtFrameHelper in order to delete the properties of the style
		//it's important if we create again a style or if we use a style later

		//Clean the attributeList and the values which concern style
		txtFrameHelper->Clean();

		status = kSuccess ;


	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}




/* Constructor
*/
Func_SY::Func_SY(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SY::Func_SY -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		paraStyleNum =  reader->ExtractInt(0,-1,MAX_STYLE);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); 
		parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_SY(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_SY::~Func_SY()
{
}


/* IDCall_SY
*/
void Func_SY::IDCall_SY(ITCLParser * parser)
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
			ASSERT_FAIL("Func_SY::IDCall_SY -> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(txtFrameHelper->GetCreation())
		{
			error = PMString(kErrNotAllowedDuringDefine);
			break;
		}

		// get the para style nametable
		IDocument * doc = parser->getCurDoc();
		
		InterfacePtr<IWorkspace> iWorkspace (doc->GetDocWorkSpace(),IID_IWORKSPACE);
		if (iWorkspace == nil)
		{
			ASSERT_FAIL("Func_SY::IDCall_SY -> iWorkspace nil");
			error = PMString(kErrNilInterface);
			break;
		}
		InterfacePtr<IStyleGroupManager> paraStyleNameTable(iWorkspace,IID_IPARASTYLEGROUPMANAGER);
		if (paraStyleNameTable == nil)
		{
			ASSERT_FAIL("Func_SY::IDCall_SY -> paraStyleNameTable nil");
			error = PMString(kErrNilInterface);
			break;
		}

		PMString paraStyleName("");
		UID paraStyleUID = kInvalidUID;
		if(paraStyleNum == -1) // Use by reverse TCL to identify root style
		{
			paraStyleUID = paraStyleNameTable->GetRootStyleUID();
		}
		else
		{
			//interrogation of the styles' table
			if(parser->GetStyleName(paraStyleNum,paraStyleName) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			// we can now get the style out of the style table
			paraStyleUID = paraStyleNameTable->FindByName(paraStyleName);
			if (paraStyleUID == kInvalidUID)
			{
				error = PMString(kErrStyleNotFound);
				break;
			}
		}

		TextIndex insertPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		txtFrameHelper->ApplyParaStyleCmd(txtModel, insertPos, paraStyleUID) ;
		
		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_SS::Func_SS(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SS::Func_SN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		charStyleNum =  reader->ExtractInt(0,-1,MAX_STYLE);

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
		IDCall_SS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_SS::~Func_SS()
{
}


/* IDCall_SS
*/
void Func_SS::IDCall_SS(ITCLParser * parser)
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
			ASSERT_FAIL("Func_SS::IDCall_SS -> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(txtFrameHelper->GetCreation())
		{
			error = PMString(kErrNotAllowedDuringDefine);
			break;
		}

		// get the para style nametable
		IDocument * doc = parser->getCurDoc();
		
		InterfacePtr<IWorkspace> iWorkspace (doc->GetDocWorkSpace(),IID_IWORKSPACE);
		if (iWorkspace == nil)
		{
			ASSERT_FAIL("Func_SS::IDCall_SS -> iWorkspace nil");
			error = PMString(kErrNilInterface);
			break;
		}
		InterfacePtr<IStyleGroupManager> charStyleNameTable(iWorkspace,IID_ICHARSTYLEGROUPMANAGER);
		if (charStyleNameTable == nil)
		{
			ASSERT_FAIL("Func_SS::IDCall_SS -> charStyleNameTable nil");
			error = PMString(kErrNilInterface);
			break;
		}

		PMString charStyleName("");
		UID charStyleUID = kInvalidUID;
		if(charStyleNum == -1) // Use by reverse TCL to identify root style
		{
			charStyleUID = charStyleNameTable->GetRootStyleUID();
		}
		else
		{
			//interrogation of the styles' table
			if(parser->GetStyleName(charStyleNum,charStyleName) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			// we can now get the style outof the style table
			charStyleUID = charStyleNameTable->FindByName(charStyleName);
			if (charStyleUID == kInvalidUID)
			{
				error = PMString(kErrStyleNotFound);
				break;
			}
		}

		TextIndex insertPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> txtModel (parser->QueryCurrentModelTextIndex(insertPos));
		if (txtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		txtFrameHelper->ApplyCharStyleCmd(txtModel, insertPos, charStyleUID) ;
		
		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}
