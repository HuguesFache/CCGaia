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
#include "ISpreadList.h"
#include "IHierarchy.h"
#include "IMasterPage.h"
#include "ISectionList.h"
#include "SectionID.h"
#include "IPageList.h"
#include "IMasterSpreadList.h"
#include "ISectionCmdData.h"
#include "ISection.h"
#include "IMasterSpreadUtils.h"

//Project includes
#include "Pages.h"
#include "GlobalDefs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "ITextFrameHelper.h"
#include "IItemManager.h"
#include "IPageHelper.h"

#include "ITextModel.h"
#include "ITextFocus.h"


/* Constructor
*/
Func_GI::Func_GI(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_GI::Func_GI -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		after = reader->ExtractInt(0,0,MAX_PAGE) ;
		nbPage = reader->ExtractInt(0,MIN_PAGE,MAX_PAGE) ;
		applyPreviousMaster = reader->ExtractBooleen(kFalse);
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
		IDCall_GI(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_GI::~Func_GI()
{
}


/* IDCall_GI
*/
void Func_GI::IDCall_GI(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IPageHelper> pageHelper(parser->QueryPageHelper());
		if(pageHelper == nil)
		{		
			ASSERT_FAIL("Func_GI::IDCall_GI-> pageHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}


		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BH::IDCall_BH -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDocument * doc = parser->getCurDoc();
		IDataBase* db = parser->getCurDataBase();
		
		// Get the list of spreads in this document
		InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
		if (spreadList == nil)
		{
			ASSERT_FAIL("Func_GI::IDCall_GI -> spreadList nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		if (!after) //if after == 0 then we have to use the current page
		{
			after = parser->getCurPage();
		}
		
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		if(pageList == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		if(after == MAX_PAGE || after > pageList->GetPageCount())
			after = pageList->GetPageCount();

		int32 posToInsert = -1 ;
		UIDRef spreadUIDRef = UIDRef::gNull;
		UIDRef masterPageUIDRef = UIDRef::gNull;

		// Get after page's spread and position within it
		InterfacePtr<IHierarchy> afterPageHier (db, pageList->GetNthPageUID(after-1), UseDefaultIID());
		InterfacePtr<IHierarchy> spreadLayer (afterPageHier->QueryParent());

		spreadUIDRef = UIDRef(db, afterPageHier->GetSpreadUID());
		posToInsert = spreadLayer->GetChildIndex(afterPageHier) + 1;

		if(applyPreviousMaster)
		{
			// Get after page's master
			InterfacePtr<IMasterPage> afterPageMaster (afterPageHier, UseDefaultIID());
			masterPageUIDRef = UIDRef(db, afterPageMaster->GetMasterSpreadUID());
		}

		if(pageHelper->CreateNewPageCmd(spreadUIDRef,nbPage,posToInsert,masterPageUIDRef) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess ;

	}while(false);
	
	if (seq != nil)
	{
		if (status != kSuccess)
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);		
		}
		else
			CmdUtils::EndCommandSequence(seq);
	}
	else
		throw TCLError(error);
		
}



/* Constructor
*/
Func_GA::Func_GA(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_GA::Func_GA -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		numPage = reader->ExtractInt(0,0,MAX_PAGE) ;
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
		IDCall_GA(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_GA::~Func_GA()
{
}


/* IDCall_GA
*/
void Func_GA::IDCall_GA(ITCLParser * parser)
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

		InterfacePtr<IPageList> pageList (parser->getCurDoc(), UseDefaultIID());
		if(pageList == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		int32 nb = pageList->GetPageCount();

		if(numPage == MAX_PAGE)
			numPage = nb;

		if (numPage > nb)
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}

		parser->setCurPage(numPage);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}

/* Constructor
 */
Func_OV::Func_OV(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{
		ASSERT_FAIL("Func_OV::Func_OV -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}
    
	try
	{
		reader->MustEgal();
        
		numPage = reader->ExtractInt(0,0,MAX_PAGE) ;
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
		IDCall_OV(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}


/* Destructor
 */
Func_OV::~Func_OV()
{
}


/* IDCall_OV
 */
void Func_OV::IDCall_OV(ITCLParser * parser)
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
        
        UIDList pageUIDs;
        // Get the document's database
        
        IDataBase* iDatabase = parser->getCurDataBase();
        
        UIDList pagesList(iDatabase);
        InterfacePtr<IPageList> pageList (parser->getCurDoc(), UseDefaultIID());
		UID overridePageUID = pageList->GetNthPageUID(numPage-1);
		pagesList.Append(overridePageUID);
        CmdUtils::ProcessCommand(Utils<IMasterSpreadUtils>()->CreateOverrideMasterPageItemsCmd(pagesList));
        
		status = kSuccess ;
        
	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
    
}




/* Constructor
*/
Func_DD::Func_DD(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DD::Func_DD -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		numPage = reader->ExtractInt(0,0,MAX_PAGE);

		numStart = reader->ExtractInt(-1,-1,MAX_PAGE) ;

		prefixe = reader->ExtractString(255);

		pageStyle = reader->ExtractInt(1,1,MAX_PAGESTYLE) ;

		autoNumber = reader->ExtractBooleen(kFalse);

		includeSectionPrefix = reader->ExtractBooleen(kFalse);
        
#if XRAIL
		marker = reader->ExtractString(255);
#endif		
		
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
		IDCall_DD(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_DD::~Func_DD()
{
}


/* IDCall_DD
*/
void Func_DD::IDCall_DD(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IPageHelper> pageHelper(parser->QueryPageHelper());
		if(pageHelper == nil)
		{		
			ASSERT_FAIL("Func_DD::IDCall_DD-> pageHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_BH::IDCall_BH -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDocument * doc = parser->getCurDoc();
		IDataBase* db = parser->getCurDataBase();
		
		if (numPage == 0) //if numPage == 0 then we have to use the current page
			numPage = parser->getCurPage();

		if(numStart == 0)
			numStart = parser->getCurPage();
		
		// Get the list of pages in this document
		InterfacePtr<IPageList> pageList(doc, UseDefaultIID());

		if (numPage > pageList->GetPageCount()) 
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}

		UID pageUID = pageList->GetNthPageUID(numPage-1);

		// Get the list of sections in this document
		
		InterfacePtr<ISectionList> sectionList(doc, IID_ISECTIONLIST);

/*		// 2016++
		//kModifySectionSettingsCmdBoss
		//kDeleteSectionCmdBoss
		// GD-HF : a termes regarder pour gerer le cas d'un numero de section sur la page 1 (page defaut),on ne peut pas le supprimer, il faudrait le modifier.
		int32 nbSections = sectionList->GetSectionCount();
		if(nbSections > 1) {
			UID curSection, curPage;
			int32 index;
			for (index = nbSections-1; index > 1; index--) {
				curSection = sectionList->GetNthSectionUID(index);
				InterfacePtr<ISection> getSection (sectionList->QueryNthSection(index));
				curPage = getSection->GetStartPageUID();
				if (curPage == pageUID) {
					sectionList->RemoveSections(index, 1);
				}
			}
			
		}
		// 2016--

		if(numStart == -1) // Keep current page number in this case
			numStart = sectionList->GetPageNumber(pageUID);

		UIDRef sectionListUIDRef = ::GetUIDRef(sectionList);
		
#if XRAIL
		if(pageHelper->CreateNewSectionCmd(sectionListUIDRef, pageUID,numStart, prefixe, marker, pageStyle, autoNumber, includeSectionPrefix) != kSuccess)
#else
		if(pageHelper->CreateNewSectionCmd(sectionListUIDRef, pageUID,numStart, prefixe, PMString(""), pageStyle, autoNumber, includeSectionPrefix) != kSuccess)
#endif			
		{	
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
*/
        // 2025++
        bool16 done = kFalse;
		int32 nbSections = sectionList->GetSectionCount();
		UIDRef sectionListUIDRef = ::GetUIDRef(sectionList);
		UID curSection, curPage;
        int32 index;
        
        for (index = 0; index < nbSections; index++) {
			curSection = sectionList->GetNthSectionUID(index);
			InterfacePtr<ISection> getSection(sectionList->QueryNthSection(index));
			curPage = getSection->GetStartPageUID();
			if (numStart == -1) // Keep current page number in this case
				numStart = sectionList->GetPageNumber(pageUID);

			if (curPage == pageUID) {
				//sectionList->RemoveSections(index, 1);
#if XRAIL
				if (pageHelper->ModifSectionCmd(db, sectionListUIDRef, curSection, pageUID, numStart, prefixe, marker, pageStyle, autoNumber, includeSectionPrefix) != kSuccess)
#else
				if (pageHelper->ModifSectionCmd(db, sectionListUIDRef, curSection, pageUID, numStart, prefixe, PMString(""), pageStyle, autoNumber, includeSectionPrefix) != kSuccess)
#endif			
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
				else {
					done = kTrue;
				}
			}
		}

		if (!done) {
#if XRAIL
			if (pageHelper->CreateNewSectionCmd(sectionListUIDRef, pageUID, numStart, prefixe, marker, pageStyle, autoNumber, includeSectionPrefix) != kSuccess)
#else
			if (pageHelper->CreateNewSectionCmd(sectionListUIDRef, pageUID, numStart, prefixe, PMString(""), pageStyle, autoNumber, includeSectionPrefix) != kSuccess)
#endif			
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}
        // 2025--

		status = kSuccess ;

	}while(false);

	if (seq != nil)
	{
		if (status != kSuccess)
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);		
		}
		else
			CmdUtils::EndCommandSequence(seq);
	}
	else
		throw TCLError(error);
		
}




/* Constructor
*/
Func_GPLUS::Func_GPLUS(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_GPLUS::Func_GPLUS -> reader nil");
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
		IDCall_GPLUS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_GPLUS::~Func_GPLUS()
{
}


/* IDCall_GPLUS
*/
void Func_GPLUS::IDCall_GPLUS(ITCLParser * parser)
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

		InterfacePtr<IPageList> pageList (parser->getCurDoc(), UseDefaultIID());
		if(pageList == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		int32 nb = pageList->GetPageCount();

		int32 numPage = parser->getCurPage();

		if (numPage + 1 > nb)
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}

		parser->setCurPage(numPage + 1);


		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_GMOINS::Func_GMOINS(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_GMOINS::Func_GMOINS -> reader nil");
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
		IDCall_GMOINS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_GMOINS::~Func_GMOINS()
{
}


/* IDCall_GMOINS
*/
void Func_GMOINS::IDCall_GMOINS(ITCLParser * parser)
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

		InterfacePtr<IPageList> pageList (parser->getCurDoc(), UseDefaultIID());
		if(pageList == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		int32 nb = pageList->GetPageCount();

		int32 numPage = parser->getCurPage();

		if (numPage - 1 < 1)
		{
			error = PMString(kErrNumPageTooSmall);
			break;
		}

		parser->setCurPage(numPage - 1);


		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}



/* Constructor
*/
Func_GS::Func_GS(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_GS::Func_GS -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		numPage = reader->ExtractInt(0,0,MAX_PAGE) ;
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
		IDCall_GS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_GS::~Func_GS()
{
}


/* IDCall_GS
*/
void Func_GS::IDCall_GS(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IPageHelper> pageHelper(parser->QueryPageHelper());
		if(pageHelper == nil)
		{		
			ASSERT_FAIL("Func_GS::IDCall_GS-> pageHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_GS::IDCall_GS -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<IPageList> pageList (parser->getCurDoc(), UseDefaultIID());
		if(pageList == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		int32 nbPage = pageList->GetPageCount();

		if (!numPage) //numpage == 0
		{
			numPage = parser->getCurPage() ;
		}

		if(numPage == MAX_PAGE)
			numPage = nbPage;

		if (numPage > nbPage) 
		{
			ASSERT_FAIL("Func_GS::IDCall_GS -> numPage > nbPage");
			error = PMString(kErrNumPageTooBig);
			break;
		}


		if (numPage == parser->getCurPage()) // Gestion of the current page
		{
			if (numPage == nbPage) //if we delete the last page, we have to change the current page
			{
				if (nbPage!=1) // if it's not the last page of the document
					parser->setCurPage(numPage-1) ;
				else //else we raise an error and break
				{
					error = PMString(kErrImpossibleToDelete);
					break;
				}
			}
		}

		//we get the UIDRef of the page to delete
		UIDRef pageUIDRef(parser->getCurDataBase(), pageList->GetNthPageUID(numPage-1));

		if(pageHelper->DeletePageCmd(pageUIDRef) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess ;

	}while(false);
	
	if (seq != nil)
	{
		if (status != kSuccess)
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);		
		}
		else
			CmdUtils::EndCommandSequence(seq);
	}
	else
		throw TCLError(error);
		
}


/* Constructor
*/
Func_GB::Func_GB(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_GB::Func_GBS -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		reader->ExtractString(255) ;
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

	if(!reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_GB::~Func_GB()
{
}

/* Constructor
*/
Func_MA::Func_MA(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_MA::Func_MA -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		masterName = reader->ExtractString(255);
		startPage = reader->ExtractInt(1,0,MAX_PAGE);
		endPage = reader->ExtractInt(1,0,MAX_PAGE);
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
		IDCall_MA(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_MA::~Func_MA()
{
}


/* IDCall_MA
*/
void Func_MA::IDCall_MA(ITCLParser * parser)
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
		
		if(startPage == CURRENT_PAGE)
		{
			if(parser->getCurPage() != -1)
				startPage = parser->getCurPage() ;
			else
				startPage = 1;
		}
		
		if(endPage == CURRENT_PAGE)
		{
			if(parser->getCurPage() != -1)
				endPage = parser->getCurPage() ;
			else
				endPage = 1;
		}
		
		InterfacePtr<IPageHelper> pageHelper(parser->QueryPageHelper());
		if(pageHelper == nil)
		{		
			ASSERT_FAIL("Func_MA::IDCall_MA-> pageHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Determine master prefix from master name
		PMString masterPrefix("A"); // Default prefix
		int32 pos = masterName.IndexOfCharacter(PlatformChar('-'));
		if(pos >= 0)
		{
			PMString * tmp = masterName.Substring(0,pos);
			masterPrefix = *tmp;
			masterName.Remove(0,pos+1);
			delete tmp;
		}

		status = pageHelper->ApplyMasterSpreadCmd(::GetUIDRef(parser->getCurDoc()),
												  masterPrefix, masterName,
												  startPage, endPage);
    #if XRAIL
        //si on est avec XRail, on ne renvoie pas d'erreur pour l'application des maquettes
        status = kSuccess;
    #endif
		if(status != kSuccess)
			error = ErrorUtils::PMGetGlobalErrorString();

	}while(false);
	
	if (status != kSuccess)
	{
		throw TCLError(error);
	}	
}


/* Constructor
*/
Func_MD::Func_MD(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_MD::Func_MD -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		masterName = reader->ExtractString(255);
		basedOn = reader->ExtractString(255);
		nbPages = reader->ExtractInt(1,1,MAX_PAGE);
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
		IDCall_MD(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_MD::~Func_MD()
{
}


/* IDCall_MD
*/
void Func_MD::IDCall_MD(ITCLParser * parser)
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
		
		InterfacePtr<IPageHelper> pageHelper(parser->QueryPageHelper());
		if(pageHelper == nil)
		{		
			ASSERT_FAIL("Func_MD::IDCall_MD-> pageHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Determine master prefix from master name
		PMString masterPrefix("A"); // Default prefix
		int32 pos = masterName.IndexOfCharacter(PlatformChar('-'));
		if(pos >= 0)
		{
			PMString * tmp = masterName.Substring(0,pos);
			masterPrefix = *tmp;
			masterName.Remove(0,pos+1);
			delete tmp;
		}

		UIDRef newMaster = kInvalidUIDRef;

		// First, check if master already exists
		InterfacePtr<IMasterSpreadList> masterList (parser->getCurDoc(), UseDefaultIID());
		if(masterList == nil)
			break;

		UID masterUID = masterList->FindMasterByName(masterPrefix,masterName);
		if(masterUID == kInvalidUID)
		{
			// Determine basedOn prefix
			PMString basedOnPrefix("A");
			pos = basedOn.IndexOfCharacter(PlatformChar('-'));
			if(pos >= 0)
			{
				PMString * tmp = basedOn.Substring(0,pos);
				basedOnPrefix = *tmp;
				basedOn.Remove(0,pos+1);
				delete tmp;
			}
			
			status = pageHelper->CreateNewMasterCmd(::GetUIDRef(parser->getCurDoc()),
													masterPrefix, masterName,
													basedOnPrefix, basedOn, nbPages, newMaster);
		}
		else
		{
			newMaster = UIDRef(parser->getCurDataBase(),masterUID);
			status = kSuccess;
		}

		if(status != kSuccess)
			error = ErrorUtils::PMGetGlobalErrorString();
		else
		{
			InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
			if(itemManager == nil)
				throw TCLError(PMString(kErrNilInterface));

			itemManager->SetMasterInCreation(newMaster);
		}

	}while(false);
	
	if (status != kSuccess)
	{
		throw TCLError(error);
	}		
}

/* Constructor
*/
Func_MF::Func_MF(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_MF::Func_MF -> reader nil");
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
	{
		InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
		if(itemManager == nil)
			throw TCLError(PMString(kErrNilInterface));

		itemManager->EndMasterCreation();
	}
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_MF::~Func_MF()
{
}


/* Constructor
*/
Func_ON::Func_ON(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_ON::Func_ON -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

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
		IDCall_ON(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* IDCall_ON
*/
void Func_ON::IDCall_ON(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IPageList> pageList (parser->getCurDoc(), UseDefaultIID());

		int32 curPage = parser->getCurPage();

		PageType type = pageList->GetPageType(pageList->GetNthPageUID(curPage-1));

		if(type == kRightPage)
			curPage += 2;
		else
			++curPage;

		if(curPage > pageList->GetPageCount())
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}

		parser->setCurPage(curPage);

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}


/* Constructor
*/
Func_EN::Func_EN(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_EN::Func_EN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

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
		IDCall_EN(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* IDCall_EN
*/
void Func_EN::IDCall_EN(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IPageList> pageList (parser->getCurDoc(), UseDefaultIID());

		int32 curPage = parser->getCurPage();

		PageType type = pageList->GetPageType(pageList->GetNthPageUID(curPage-1));

		if(type == kLeftPage)
			curPage += 2;
		else
			++curPage;

		if(curPage > pageList->GetPageCount())
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}

		parser->setCurPage(curPage);

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_OL::Func_OL(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_OL::Func_OL -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

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
		IDCall_OL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* IDCall_OL
*/
void Func_OL::IDCall_OL(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IPageList> pageList (parser->getCurDoc(), UseDefaultIID());

		int32 lastOddPageIndex = pageList->GetPageCount()-1;
		PageType lastPageType = pageList->GetPageType(pageList->GetNthPageUID(lastOddPageIndex));
		
		if(lastPageType == kLeftPage)
			--lastOddPageIndex;

		if(lastOddPageIndex < 0)
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}

		parser->setCurPage(lastOddPageIndex+1);

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_EL::Func_EL(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_EL::Func_EL -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

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
		IDCall_EL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* IDCall_EL
*/
void Func_EL::IDCall_EL(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IPageList> pageList (parser->getCurDoc(), UseDefaultIID());

		int32 lastEvenPageIndex = pageList->GetPageCount()-1;
		PageType lastPageType = pageList->GetPageType(pageList->GetNthPageUID(lastEvenPageIndex));
		
		if(lastPageType == kRightPage)
			--lastEvenPageIndex;

		if(lastEvenPageIndex < 0)
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}

		parser->setCurPage(lastEvenPageIndex+1);

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}