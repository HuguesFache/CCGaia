/*
//	File:	Livres.cpp
//
//	Date:	25-July-2006
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"
#include "GlobalDefs.h"

// General includes
#include "ErrorUtils.h"
#include "StringUtils.h"
#include "FileUtils.h"
#include "TOCEntryInfo.h"
#include "SDKLayoutHelper.h"
#include "IndexingID.h"

// Interface includes
#include "IBookCmdData.h"
#include "IBookContentCmdData.h"
#include "IBookPaginateOptions.h"
#include "ISetMasterDocCmdData.h"
#include "IBookContentMgr.h"
#include "IBookManager.h"
#include "IIntData.h"
#include "IBoolData.h"
#include "IFrameUtils.h"
#include "IHierarchy.h"
#include "ITOCStyleNameTable.h"
#include "IDocument.h"
#include "ITOCStyleInfo.h"
#include "IMultiColumnTextFrame.h"
#include "ITextModel.h"
#include "IStringListData.h"
#include "ITOCCmdData.h"
#include "IDataLinkReference.h"
#include "IPersistUIDData.h"
#include "ITOCDataLinkHelper.h"
#include "IBookUtils.h"
#include "IDocFileHandler.h"
#include "IDocumentUtils.h"
#include "IUIDRefListData.h"
#include "IUIFlagData.h"
#include "ICommandMgr.h"
#include "IStyleGroupManager.h"
#include "IStringData.h"
#include "IStyleInfo.h"
#include "IDocumentList.h"
#include "IApplication.h"
#include "IDataLink.h"
#include "IIndexOptions.h"
#include "IIndexCmdData.h"
#include "IActiveTopicListContext.h"
#include "IUIDData.h"
#include "IPlaceGun.h"
#include "IReplaceCmdData.h"

// Project includes
#include "Livres.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"

#include "CAlert.h"

/* Constructor
*/
Func_NL::Func_NL(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_NL::Func_NL -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		bookname = reader->ExtractString(255);		
		paginateOption = reader->ExtractInt(0,0,2);
		insertBlank = reader->ExtractBooleen(kFalse);
		autoPagination = reader->ExtractBooleen(kTrue);
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
		IDCall_NL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_NL
*/
void Func_NL::IDCall_NL(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	ICommandSequence* seq = nil;
	UIDRef bookUIDRef = UIDRef::gNull;
	
	do
	{
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDFile bookFile = FileUtils::PMStringToSysFile(bookname);

		IDFile dummy;						
		if(FileUtils::GetParentDirectory(bookFile, dummy) == kFalse) // Invalid path
		{
			error = PMString(kErrBadPath); 
			break;
		}

		// Create the book
		InterfacePtr<ICommand> newBookCmd (CmdUtils::CreateCommand(kNewBookCmdBoss));
		
		InterfacePtr<IBookCmdData> cmdData (newBookCmd, UseDefaultIID());
		cmdData->SetSysFile(bookFile);		
		cmdData->SetProtectionLevel(IDataBase::kProtectMiniSave);
		cmdData->SetOpenUIFlag(K2::kSuppressUI);
		cmdData->SetOpenOption(IBook::kOpenShared);

		if(CmdUtils::ProcessCommand(newBookCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		bookUIDRef = newBookCmd->GetItemList()->GetRef(0);

		// Set paginate option
		InterfacePtr<ICommand> setPaginateOptionCmd (CmdUtils::CreateCommand(kSetPaginateOptionsCmdBoss));
		InterfacePtr<IBookPaginateOptions> bookOptions (setPaginateOptionCmd, UseDefaultIID());
		bookOptions->SetCurrentBookUIDRef(bookUIDRef);
		bookOptions->SetPaginateOptions((IBookPaginateOptions::Options) paginateOption);
		bookOptions->SetInsertBlankPage(insertBlank);
		bookOptions->SetAutoRepaginateFlag(autoPagination);

		if(CmdUtils::ProcessCommand(setPaginateOptionCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Save book
		InterfacePtr<ICommand> saveBookCmd (CmdUtils::CreateCommand(kSaveBookCmdBoss));
		saveBookCmd->SetItemList(UIDList(bookUIDRef));

		if(CmdUtils::ProcessCommand(saveBookCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurBook(bookUIDRef);
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_LO::Func_LO(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_LO::Func_LO -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		bookname = reader->ExtractString(255);
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
		IDCall_LO(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_LO
*/
void Func_LO::IDCall_LO(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	UIDRef bookRef = UIDRef::gNull;
	
	do
	{
		IDFile bookFile = FileUtils::PMStringToSysFile(bookname);

		// Open up the book
		InterfacePtr<ICommand> openBookCmd (CmdUtils::CreateCommand(kOpenBookCmdBoss));
		
		InterfacePtr<IBookCmdData> openCmdData (openBookCmd, UseDefaultIID());
		openCmdData->SetSysFile(bookFile);		
		openCmdData->SetProtectionLevel(IDataBase::kProtectMiniSave);
		openCmdData->SetOpenUIFlag(K2::kSuppressUI);
		openCmdData->SetOpenOption(IBook::kOpenShared);
		
		if(CmdUtils::ProcessCommand(openBookCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		bookRef = openBookCmd->GetItemList()->GetRef(0);

		status = kSuccess;

	}while(kFalse);

	if(status == kSuccess)
		parser->setCurBook(bookRef);
	else 
		throw TCLError(error);
}

/* Constructor
*/
Func_FL::Func_FL(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_FL::Func_FL -> reader nil");
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
		IDCall_FL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_FL
*/
void Func_FL::IDCall_FL(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	UIDRef bookRef = UIDRef::gNull;
	
	do
	{
		UIDRef curBookRef = parser->getCurBook();
		if(curBookRef == UIDRef::gNull)
		{
			error = PMString(kErrNoCurrentBook);
			break;
		}

		// Close the current book
		parser->setCurBook(UIDRef::gNull);
		
		status = kSuccess;

	}while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_SL::Func_SL(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SL::Func_SL -> reader nil");
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
		IDCall_SL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_SL
*/
void Func_SL::IDCall_SL(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	UIDRef bookRef = UIDRef::gNull;
	
	do
	{
		UIDRef curBookRef = parser->getCurBook();
		if(curBookRef == UIDRef::gNull)
		{
			error = PMString(kErrNoCurrentBook);
			break;
		}

		// Save book
		InterfacePtr<ICommand> saveBookCmd (CmdUtils::CreateCommand(kSaveBookCmdBoss));
		saveBookCmd->SetItemList(UIDList(curBookRef));

		if(CmdUtils::ProcessCommand(saveBookCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		status = kSuccess;

	}while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_LPlus::Func_LPlus(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_LPlus::Func_LPlus -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		masterIndex = reader->ExtractInt(-1,-1,kMaxInt16);
		insertIndex = reader->ExtractInt(0,-1,kMaxInt16);
		while(!reader->IsEndOfCommand()) 
		{
			PMString docAbsName = reader->ExtractString(255);
			IDFile docFile = FileUtils::PMStringToSysFile(docAbsName);
			if(!FileUtils::DoesFileExist(docFile))
			{
				PMString errorString(kErrFileNotExist);
				errorString.Translate();
				StringUtils::ReplaceStringParameters(&errorString, docAbsName);
					
				throw TCLError(errorString);
			}

			documents.push_back(docFile);
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
		IDCall_LPlus(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_LPlus
*/
void Func_LPlus::IDCall_LPlus(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	ICommandSequence* seq = nil;
	
	do
	{
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		UIDRef curBookRef = parser->getCurBook();
		if(curBookRef == UIDRef::gNull)
		{
			error = PMString(kErrNoCurrentBook);
			break;
		}

		// Get book file
		InterfacePtr<IBook> curBook (curBookRef, UseDefaultIID());
		InterfacePtr<IBookContentMgr> bookContentMgr (curBookRef, UseDefaultIID());

		IDFile bookFile = curBook->GetBookFileSpec();

		if(insertIndex == -1) // Remove all documents from the book
		{
			// First gather each document
			UIDList docsToRemove(curBookRef.GetDataBase());

			for(int32 i = 0 ; i < bookContentMgr->GetContentCount() ; ++i)
				docsToRemove.Append(bookContentMgr->GetNthContent(i));

			InterfacePtr<ICommand> removeDocFromBookCmd( CmdUtils::CreateCommand(kRemoveDocFromBookCmdBoss)); 

			InterfacePtr<IBookContentCmdData> removeDocsCmdData(removeDocFromBookCmd, UseDefaultIID()); 
			removeDocsCmdData->SetTargetBook(bookFile);
			removeDocsCmdData->SetContentList(&docsToRemove);

			CmdUtils::ProcessCommand(removeDocFromBookCmd);

			insertIndex = 0;
		}

		if(!documents.empty())
		{
			// Get document list to check which documents are already opened
			// Prerequisite for this command : all documents in the book must be opened before the command processing, otherwise there are memory leaks !	
			InterfacePtr<IApplication> app (GetExecutionContextSession()->QueryApplication());
			InterfacePtr<IDocumentList> docList (app->QueryDocumentList());
			
			K2Vector<UIDRef> docToCloseRefs;
			SDKLayoutHelper layoutHelper;
			for(int32 i = 0 ; i < documents.size() ; ++i)
			{
				IDocument * doc = docList->FindDoc(documents[i]);
				if(!doc) 		
					docToCloseRefs.push_back(layoutHelper.OpenDocument(documents[i],K2::kSuppressUI));				
			}

			// Add documents to the new book
			InterfacePtr<ICommand> constructContentCmd( CmdUtils::CreateCommand(kConstructContentCmdBoss)); 
			InterfacePtr<IBookContentCmdData> contentCmdData(constructContentCmd, UseDefaultIID()); 
			contentCmdData->SetTargetBook(bookFile); 
			contentCmdData->SetContentFile(documents);

			if(CmdUtils::ProcessCommand(constructContentCmd) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}			

			UIDList* contentList = const_cast<UIDList*>(constructContentCmd->GetItemList());  

			InterfacePtr<ICommand> addDocToBookCmd (CmdUtils::CreateCommand(kAddDocToBookCmdBoss));

			InterfacePtr<IBookContentCmdData> bookContentCmdData (addDocToBookCmd, UseDefaultIID());
			bookContentCmdData->SetTargetBook(bookFile);
			bookContentCmdData->SetContentFile(documents);
			bookContentCmdData->SetContentList(contentList);
			bookContentCmdData->SetDestPosition(insertIndex-1);

			if(CmdUtils::ProcessCommand(addDocToBookCmd) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			// Close documents which have been opened specially for this command
			for(int32 i = 0 ; i < docToCloseRefs.size() ; ++i)
				layoutHelper.CloseDocument(docToCloseRefs[i], kFalse, kSuppressUI, kFalse, IDocFileHandler::kSchedule);
		}

		if(masterIndex != -1)
		{
			// Set master document in book

			int32 nbDocs =  bookContentMgr->GetContentCount();
			if(masterIndex > nbDocs - 1)
			{
				error = PMString(kErrInvalidIndex);
				break;
			}

			InterfacePtr<ICommand> setBookMasterCmd (CmdUtils::CreateCommand(kSetBookMasterDocCmdBoss));
			
			InterfacePtr<ISetMasterDocCmdData> setBookMasterCmdData (setBookMasterCmd, IID_ISETMASTERDOCCMDDATA);
			setBookMasterCmdData->SetBookFile(bookFile);
			setBookMasterCmdData->SetMasterDoc(bookContentMgr->GetNthContent(masterIndex));
		
			if(CmdUtils::ProcessCommand(setBookMasterCmd) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		status = kSuccess;

	} while(kFalse);

	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			CmdUtils::ProcessScheduledCmds(); // To close documents which were opened during process
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			CmdUtils::EndCommandSequence(seq);
			CmdUtils::ProcessScheduledCmds(); // To close documents which were opened during process
			throw TCLError(error);			
		}
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_RL::Func_RL(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_RL::Func_RL -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		indexFrom = reader->ExtractInt(0,0,kMaxInt16);
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
		IDCall_RL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_RL
*/
void Func_RL::IDCall_RL(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		// Get current book
		UIDRef curBookRef = parser->getCurBook();
		if(curBookRef == UIDRef::gNull)
		{
			error = PMString(kErrNoCurrentBook);
			break;
		}

		IDataBase * db = curBookRef.GetDataBase();

		// Set paginate option
		InterfacePtr<IBookPaginateOptions> bookOptions (curBookRef, UseDefaultIID());

		InterfacePtr<ICommand> setRepaginateCmd (CmdUtils::CreateCommand(kSetRepaginationCmdBoss));

		InterfacePtr<IBookPaginateOptions> cmdOptions (setRepaginateCmd, UseDefaultIID());
		cmdOptions->SetCurrentBookUIDRef(curBookRef);
		cmdOptions->SetPaginateOptions(bookOptions->GetPaginateOptions());
		cmdOptions->SetInsertBlankPage(bookOptions->GetInsertBlankPage());
		cmdOptions->SetAutoRepaginateFlag(bookOptions->GetAutoRepaginateFlag());

		InterfacePtr<IBoolData> doRepaginate (setRepaginateCmd, UseDefaultIID());
		doRepaginate->Set(kTrue); // Turn repagination on

		InterfacePtr<IIntData> startFrom (setRepaginateCmd, UseDefaultIID());
		startFrom->Set(indexFrom);

		// Get document list to check which documents are already opened
		// Prerequisite for this command : all documents in the book must be opened before the command processing, otherwise there are memory leaks !	
		InterfacePtr<IApplication> app (GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> docList (app->QueryDocumentList());
			
		K2Vector<UIDRef> docToCloseRefs;
		SDKLayoutHelper layoutHelper;

		InterfacePtr<IBookContentMgr> contentMgr (bookOptions, UseDefaultIID());						
		for(int32 i = 0 ; i < contentMgr->GetContentCount() ; ++i)
		{
			InterfacePtr<IDataLink> contentLink (db, contentMgr->GetNthContent(i), UseDefaultIID());
			PMString * theDocFullName = contentLink->GetFullName();
			IDFile theDocFile = FileUtils::PMStringToSysFile(*theDocFullName);

			IDocument * doc = docList->FindDoc(theDocFile);
			if(!doc) 			
				docToCloseRefs.push_back(layoutHelper.OpenDocument(theDocFile, K2::kSuppressUI));				
		}

		if(CmdUtils::ProcessCommand(setRepaginateCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Close documents which have been opened specially for this command
		for(int32 i = 0 ; i < docToCloseRefs.size() ; ++i)
			layoutHelper.CloseDocument(docToCloseRefs[i], kFalse, kSuppressUI, kFalse, IDocFileHandler::kSchedule);

		CmdUtils::ProcessScheduledCmds();

		status = kSuccess;

	} while(kFalse);

	if (status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_MT::Func_MT(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_MT::Func_MT -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);		
		tocStyleName = reader->ExtractString(255);
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
		IDCall_MT(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


void Func_MT::IDCall_MT(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error("");
	do
	{
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		IDocument * doc = parser->getCurDoc();
		IDataBase * db = parser->getCurDataBase();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		// Check whether target frame for TOC is a text frame
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
		
		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IHierarchy> frameHier (db, refUID, UseDefaultIID());
		if(!Utils<IFrameUtils>()->IsTextFrame(frameHier))
		{
			error=PMString(kErrTextBoxRequired);
			break;
		}


		// Get target text model
		InterfacePtr<IMultiColumnTextFrame> txtFrame (db, frameHier->GetChildUID(0), UseDefaultIID());

		InterfacePtr<ITextModel> tocTxtModel (txtFrame->QueryTextModel());		

		UIDRef txtModelRef = ::GetUIDRef(tocTxtModel);

		// Get UID of TOC style mentioned in parameter
		InterfacePtr<ITOCStyleNameTable> TOCStyleList (doc->GetDocWorkSpace(), IID_ITOCSTYLENAMETABLE);

		int32 styleCount = TOCStyleList->GetNumStyles();
		UID styleUID = kInvalidUID;
		for(int32 i = 0 ; i < styleCount ; ++i)
		{
			styleUID = TOCStyleList->GetNthStyle(i);	
			InterfacePtr<ITOCStyleInfo> iStyleInfo(db, styleUID, IID_ITOCSTYLEINFO);
			PMString styleName = iStyleInfo->GetName();
	
			if(styleName == tocStyleName)
				break;

			styleUID = kInvalidUID;
		}

		if(styleUID == kInvalidUID)
		{
			error = PMString(kErrStyleNotFound);
			break;
		}

		// Check whether a TOC exist for this story (is it an update or not ?)
		InterfacePtr<IDataLinkReference> pDataLink(txtModelRef, IID_IDATALINKREFERENCE );

		// if the IDataLinkReference exists on the story, get the UID of the kGenericDataLinkBoss
		UID datalinkUID = pDataLink->GetUID();
		// Try to get the pointer to the implementation of IID_ITOCSTYLEPERSISTUIDDATA
		// which is of interface type IPersistUIDData
		InterfacePtr<IPersistUIDData> pTOCStyleUIDData(db, datalinkUID, IID_ITOCSTYLEPERSISTUIDDATA);
		if (!pTOCStyleUIDData || (pTOCStyleUIDData->GetUID() == kInvalidUID))
		{
			{
				// Create a new kGenericDataLinkBoss so that it creates an empty TOC in the text frame (avoid getting the place gun loaded after processing these commands)
				InterfacePtr<IPersistUIDData> newTOCStyleUIDData (::CreateObject2<IPersistUIDData>(kDataLinkBoss, IID_ITOCSTYLEPERSISTUIDDATA));

				InterfacePtr<ITOCDataLinkHelper> tocLinkHelper (::CreateObject2<ITOCDataLinkHelper>(kTOCDataLinkHelperBoss, IID_ITOCDATALINKHELPER));
				tocLinkHelper->AddTOCDataLink(::GetUIDRef(newTOCStyleUIDData), txtModelRef, UIDRef(db, styleUID));		
			}
		}

		InterfacePtr<ITOCStyleInfo> tocStyleInfo (db, styleUID, UseDefaultIID());

		// Start creating TOC
		InterfacePtr<ICommand> createTOCCmd(CmdUtils::CreateCommand(kCreateTOCCmdBoss));
		
		K2Vector<PMString> stylesInTOC; // Set up CreateTOCCmd data for all the paragraph styles which are to be considered for generating the TOC
		
		const FormatEntriesInfo& entryMap = tocStyleInfo->GetTOCEntryMap();
		int32 len = entryMap.size();
		for (int32 j = 0; j < len; ++j)
		{
			PMString locStyleName = entryMap[j].Key();
			stylesInTOC.push_back(locStyleName);
		}		
	
		InterfacePtr<IStringListData> iData(createTOCCmd, IID_ISTRINGLISTDATA);
		iData->SetStringList(stylesInTOC);

		InterfacePtr<ITOCCmdData> createTOCCmdData(createTOCCmd, IID_ITOCCMDDATA);
		//Set iCreateTOCCmdData attributes using information retrieved from selected style
		createTOCCmdData->SetTargetItem(::GetUIDRef(doc));
		createTOCCmdData->SetIncludeHidderLayerFlag(tocStyleInfo->GetIncludeHidderLayerFlag());
		createTOCCmdData->SetRunInFlag(tocStyleInfo->GetRunInFlag());
		createTOCCmdData->SetIncludeBookmarks(tocStyleInfo->GetIncludeBookmarks());
		createTOCCmdData->SetFormatEntryMap(entryMap);
		createTOCCmdData->SetTOCStyleName(tocStyleInfo->GetName());
		createTOCCmdData->SetIncludeOversetFlag(kFalse);

		// Code for including the documents in the book 
		UIDRef bookRef = parser->getCurBook();
		bool16 includeBookPubs = kFalse;
		if (bookRef != UIDRef::gNull)
		{
			InterfacePtr<IBook> curBook (bookRef, UseDefaultIID());
		
			int32 dummy = 0;
			includeBookPubs = Utils<IBookUtils>()->IsBookContent(doc, curBook, dummy);
			if(includeBookPubs)
			{
				createTOCCmdData->SetBookName(FileUtils::SysFileToPMString(curBook->GetBookFileSpec()));
				createTOCCmdData->SetIncludeBookPubs(includeBookPubs);
			}		
		}
			
		createTOCCmdData->SetTextModelUIDRef(::GetUIDRef(tocTxtModel));
		createTOCCmdData->SetReplaceExistingTOCFlag(kTrue); // Specify whether Existing TOC is to be replaced or not (always TRUE because we create an empty TOC when it doesn't exist)

		bool16 showAlert = CAlert::GetShowAlerts();
		CAlert::SetShowAlerts(kFalse); // BUG FIX : switch off alert displayed whenever there is an overset text frame even though SetIncludeOversetFlag is set to kTrue
		//  Process CreateTOCCmd
		if(CmdUtils::ProcessCommand(createTOCCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		CAlert::SetShowAlerts(showAlert);

		// Get the newly created text frame from CreateTOCCmd
		UIDRef textModelUIDRef = createTOCCmdData->GetTextModelUIDRef();

		// Set TOC format
		K2Vector<TOCEntryInfo> tocEntryInfoList = createTOCCmdData->GetTOCEntryInfoList();
		
		InterfacePtr<ICommand> formatTOCCmd(CmdUtils::CreateCommand(kFormatTOCCmdBoss));
			
		InterfacePtr<ITOCCmdData> formatTOCCmdData(formatTOCCmd, IID_ITOCCMDDATA);
		formatTOCCmdData->SetTargetItem(::GetUIDRef(doc));
		formatTOCCmdData->SetTextModelUIDRef(textModelUIDRef);

		// Specify whether Existing TOC is to be replaced or not
		formatTOCCmdData->SetReplaceExistingTOCFlag(kTrue);
		formatTOCCmdData->SetTOCStyleName(tocStyleInfo->GetName());
		// set the list of created TOC entries
		formatTOCCmdData->SetTOCEntryInfoList(tocEntryInfoList);
		// set other properties
		formatTOCCmdData->SetIncludeHidderLayerFlag(tocStyleInfo->GetIncludeHidderLayerFlag());
		formatTOCCmdData->SetIncludeBookPubs(includeBookPubs);
		formatTOCCmdData->SetRunInFlag(tocStyleInfo->GetRunInFlag());
		formatTOCCmdData->SetIncludeBookmarks(tocStyleInfo->GetIncludeBookmarks());
		formatTOCCmdData->SetIncludeOversetFlag(kTrue);
		// set the the map of all the paragraph styles and their formatting 
		// attributes as stored in the TOC style
		formatTOCCmdData->SetFormatEntryMap(entryMap);
		formatTOCCmdData->SetTOCTitle(tocStyleInfo->GetTOCTitle());
		formatTOCCmdData->SetTOCTitleFormatStyle(tocStyleInfo->GetTOCTitleFormatStyle());

		if(CmdUtils::ProcessCommand(formatTOCCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

		// Close each document which was opened during TOC generation
		/*
		InterfacePtr<IUIDRefListData>  docsToClose (createTOCCmd, IID_ITOCCLOSEDDOCUIDLIST);		
		
		K2Vector<UIDRef>::iterator iter = docsToClose->begin();
		while(status == kSuccess && iter != docsToClose->end())
		{
			UIDRef documentUIDRef = *iter;
			InterfacePtr<IDocFileHandler> docFileHandler(Utils<IDocumentUtils>()->QueryDocFileHandler(documentUIDRef));

			if (docFileHandler->CanClose(documentUIDRef)) 
			{
				 // BUG FIX : transaction level is 1 at this step (??), so we get an assert when is document is closed																
				documentUIDRef.GetDataBase()->EndTransaction(); // Calling EndTransaction will set transaction level to 0 before closing doc

				docFileHandler->Close(documentUIDRef, K2::kSuppressUI, kFalse, IDocFileHandler::kSchedule);
				status = ErrorUtils::PMGetGlobalErrorCode();
			}

			++iter;
		}
		*/
				
	} while(kFalse);
	
	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			CmdUtils::ProcessScheduledCmds(); // Process CloseDoc commands above
			parser->setCurItem(refBloc);
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_ST::Func_ST(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_ST::Func_ST -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		tocStyleName = reader->ExtractString(255);

		tocTitle = reader->ExtractString(255);
		tocTitleStyle = reader->ExtractInt(-1,-1,MAX_STYLE);

		includeHidLayer = reader->ExtractBooleen(kFalse);
		runIn = reader->ExtractBooleen(kFalse);
		includeBookMarks = reader->ExtractBooleen(kTrue);

		entries.clear();
		do
		{
			FormatEntryParam entryParam;

			int32 paraStyleEntry = reader->ExtractInt(-1,-1,MAX_STYLE);
			entryParam.formatEntryStyle = reader->ExtractInt(-2,-2,MAX_STYLE);
			entryParam.formatPageStyle = reader->ExtractInt(-1,-1,MAX_STYLE);

			entryParam.level = reader->ExtractInt(1,1,10);
			entryParam.levelSort = reader->ExtractBooleen(kFalse);
			entryParam.pageNumPos = reader->ExtractInt(0,0,2);

			entryParam.separator = reader->ExtractString(255);
			entryParam.formatSeparatorStyle = reader->ExtractInt(-1,-1,MAX_STYLE);

			entries.push_back(KeyValuePair<int32, struct formatEntryParam>(paraStyleEntry, entryParam));

		} while(!reader->IsEndOfCommand());

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
		IDCall_ST(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

void Func_ST::IDCall_ST(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");
	do
	{
		IDocument * doc = parser->getCurDoc();
		IDataBase * db = parser->getCurDataBase();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		// Check whether style already exists or not
		InterfacePtr<ITOCStyleNameTable> TOCStyleList (doc->GetDocWorkSpace(), IID_ITOCSTYLENAMETABLE);
		bool16 exist = (TOCStyleList->FindByName(tocStyleName) != kInvalidUID);
	
		// Create the command for creating a TOC style
		InterfacePtr<ICommand> createOrEditTOCStyleCmd (nil);
		if(exist)
		{
			createOrEditTOCStyleCmd = InterfacePtr<ICommand> (CmdUtils::CreateCommand(kEditTOCStyleCmdBoss));
			InterfacePtr<IStringData> styleToEdit (createOrEditTOCStyleCmd, IID_ITOCSTYLENAME);
			styleToEdit->Set(tocStyleName);
		}
		else
			createOrEditTOCStyleCmd = InterfacePtr<ICommand> (CmdUtils::CreateCommand(kCreateTOCStyleCmdBoss));
		
		createOrEditTOCStyleCmd->SetItemList(UIDList(doc->GetDocWorkSpace()));
		
		InterfacePtr<ITOCCmdData> cmdData (createOrEditTOCStyleCmd, UseDefaultIID());
		cmdData->SetTOCStyleName(tocStyleName);

		cmdData->SetIncludeHidderLayerFlag(includeHidLayer);
		cmdData->SetRunInFlag(runIn);
		cmdData->SetIncludeBookmarks(includeBookMarks);

		// Look for title format style's UID
		UID titleFormatStyleUID = kInvalidUID;
		PMString titleFormatStyleName = kNullString;
		InterfacePtr<IStyleGroupManager> paraStyleNameTable(doc->GetDocWorkSpace(),IID_IPARASTYLEGROUPMANAGER);
		InterfacePtr<IStyleGroupManager> charStyleNameTable(doc->GetDocWorkSpace(),IID_ICHARSTYLEGROUPMANAGER);
		
		if(tocTitleStyle == -1) // -1 means [No Paragraph Style]
			titleFormatStyleUID = paraStyleNameTable->GetRootStyleUID();
		else
		{
			if(parser->GetStyleName(tocTitleStyle,titleFormatStyleName) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			// we can now get the style out of the style table
			titleFormatStyleUID = paraStyleNameTable->FindByName(titleFormatStyleName);
			if (titleFormatStyleUID == kInvalidUID)
			{
				error = PMString(kErrStyleNotFound);
				break;
			}
		}
		
		cmdData->SetTOCTitle(tocTitle);
		cmdData->SetTOCTitleFormatStyle(titleFormatStyleUID);

		FormatEntriesInfo formatEntriesInfo;
	
		bool16 failed = kFalse;
		for(int32 i = 0 ; i < entries.size() ; ++i)
		{
			
			TOCFormatEntryInfo entryInfo;
			PMString entryStyleName = kNullString, formatEntryName = kNullString, formatPageName = kNullString, formatSeparatorName = kNullString;
			
			if(entries[i].Key() == -1) // -1 means [No Paragraph Style]
			{
				InterfacePtr<IStyleInfo> noStyleInfo(db, paraStyleNameTable->GetRootStyleUID(), UseDefaultIID());
				entryStyleName = noStyleInfo->GetName();
			}
			else
			{				
				if(parser->GetStyleName(entries[i].Key(),entryStyleName) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					failed = kTrue;
					break;
				}
			}

			UID entryUID = paraStyleNameTable->FindByName(entryStyleName);
			if(entryUID == kInvalidUID)
			{
				error = PMString(kErrStyleNotFound); failed = kTrue;
				break;
			}

			if(entries[i].Value().formatEntryStyle == -1) // -1 means [No Paragraph Style]
				entryInfo.fParaFormatStyle = paraStyleNameTable->GetRootStyleUID();
			else if(entries[i].Value().formatEntryStyle == -2)	// -2 means [Same Style]
				entryInfo.fParaFormatStyle = kInvalidUID;
			else
			{
				if(parser->GetStyleName(entries[i].Value().formatEntryStyle,formatEntryName) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString(); failed = kTrue; 
					break;
				}

				entryInfo.fParaFormatStyle = paraStyleNameTable->FindByName(formatEntryName);
				if(entryInfo.fParaFormatStyle == kInvalidUID)
				{
					error = PMString(kErrStyleNotFound); failed = kTrue;
					break;
				}
			}
				
			if(entries[i].Value().formatPageStyle != -1) // -1 means [None] character style
			{
				if(parser->GetStyleName(entries[i].Value().formatPageStyle,formatPageName) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString(); failed = kTrue; 
					break;
				}

				entryInfo.fPageNumFormatStyle = charStyleNameTable->FindByName(formatPageName);
				if(entryInfo.fPageNumFormatStyle == kInvalidUID)
				{
					error = PMString(kErrStyleNotFound); failed = kTrue;
					break;
				}
			}
			else
				entryInfo.fPageNumFormatStyle = kInvalidUID;
			
			if(entries[i].Value().formatSeparatorStyle != -1) // -1 means [None] character style
			{
				if(parser->GetStyleName(entries[i].Value().formatSeparatorStyle,formatSeparatorName) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString(); failed = kTrue; 
					break;
				}

				entryInfo.fSeparatorStyle = charStyleNameTable->FindByName(formatSeparatorName);
				if(entryInfo.fSeparatorStyle == kInvalidUID)
				{
					error = PMString(kErrStyleNotFound); failed = kTrue;
					break;
				}
			}
			else
				entryInfo.fSeparatorStyle = kInvalidUID;
	
			entryInfo.fLevel = entries[i].Value().level;
			entryInfo.fLevelSort = entries[i].Value().levelSort;
			entryInfo.fPosition = (ITOCCmdData::PageNumPosition)entries[i].Value().pageNumPos;
			if(!entries[i].Value().separator.IsEmpty())
				entryInfo.fSeparator = WideString(entries[i].Value().separator);

			formatEntriesInfo.push_back(KeyValuePair<PMString, TOCFormatEntryInfo  >(entryStyleName , entryInfo));
		}

		if(failed)
			break;

		cmdData->SetFormatEntryMap(formatEntriesInfo);
		
		status = CmdUtils::ProcessCommand(createOrEditTOCStyleCmd);

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_IG::Func_IG(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_IG::Func_IG -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		refBloc = reader->ExtractTCLRef(kCurrentTCLRef);		
		title =  reader->ExtractString(255);
		titleStyle = reader->ExtractInt(-1,-1,MAX_STYLE);
		
		includeHidLayer = reader->ExtractBooleen(kFalse);
		runIn = reader->ExtractBooleen(kFalse);
		includeSection = reader->ExtractBooleen(kTrue);
		includeEmptySection = reader->ExtractBooleen(kFalse);
		
		topicSep = ParseEscapeChars(reader->ExtractString(255));
		pageSep =  ParseEscapeChars(reader->ExtractString(255));
		entrySep = ParseEscapeChars(reader->ExtractString(255));
		xrefSep = ParseEscapeChars(reader->ExtractString(255));
		pageRangeSep = ParseEscapeChars(reader->ExtractString(255));
		entryEndSep = ParseEscapeChars(reader->ExtractString(255));

		sectionStyle = reader->ExtractInt(-1,-1,MAX_STYLE);
		pageNumStyle = reader->ExtractInt(-1,-1,MAX_STYLE);
		xrefStyle = reader->ExtractInt(-1,-1,MAX_STYLE);
		refCharStyle = reader->ExtractInt(-1,-1,MAX_STYLE);

		topicStyles.clear();
		int32 level = 1;
		do
		{		
			int32 topicStyle = reader->ExtractInt(-1,-1,MAX_STYLE);
		
			topicStyles.push_back(topicStyle);
			++level;

		} while(!reader->IsEndOfCommand());

		if(level > 5)
		{
			throw TCLError(PMString(kErrTooManyLevels)); // TODO
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
		IDCall_IG(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

void Func_IG::IDCall_IG(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");
	K2Vector<UIDRef> docToCloseRefs;
	SDKLayoutHelper layoutHelper;

	do
	{	
		IDocument * doc = parser->getCurDoc();
		IDataBase * db = parser->getCurDataBase();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		// Check whether target frame for TOC is a text frame
		if(refBloc == kCurrentTCLRef)
			refBloc = parser->getCurItem();
		
		UID refUID = parser->getItem(refBloc);
		if(refUID == kInvalidUID)
		{
			error = PMString(kErrInvalidItem);
			break;
		}

		InterfacePtr<IHierarchy> frameHier (db, refUID, UseDefaultIID());
		if(!Utils<IFrameUtils>()->IsTextFrame(frameHier))
		{
			error=PMString(kErrTextBoxRequired);
			break;
		}

		// Get target text model
		InterfacePtr<IMultiColumnTextFrame> txtFrame (db, frameHier->GetChildUID(0), UseDefaultIID());

		InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());		
		
		// Check whether an index exist for this story (is it an update or not ?)
		InterfacePtr<IDataLinkReference> pDataLink(txtModel, IID_IDATALINKREFERENCE );

		// if the IDataLinkReference exists on the story, get the UID of the kGenericDataLinkBoss
		UID datalinkUID = pDataLink->GetUID();

		// Try to get the pointer to the implementation of IID_IIDXINSTANCEPERSISTUIDDATA
		// which is of interface type IPersistUIDRefData
		UID indexUID = kInvalidUID;

		InterfacePtr<IPersistUIDData> pIndexUIDData(db, datalinkUID, IID_IIDXINSTANCEPERSISTUIDDATA);
		if(pIndexUIDData && (pIndexUIDData->GetUID() != kInvalidUID))
		{
			indexUID = pIndexUIDData->GetUID();
		}

		InterfacePtr<IStyleGroupManager> paraStyleNameTable(doc->GetDocWorkSpace(),IID_IPARASTYLEGROUPMANAGER);
		InterfacePtr<IStyleGroupManager> charStyleNameTable(doc->GetDocWorkSpace(),IID_ICHARSTYLEGROUPMANAGER);

		// Collect specified style names
		InterfacePtr<IStyleInfo> rootParaStyle (db, paraStyleNameTable->GetRootStyleUID(), UseDefaultIID());
		InterfacePtr<IStyleInfo> rootCharStyle (db, charStyleNameTable->GetRootStyleUID(), UseDefaultIID());
		
		PMString titleStyleName, sectionStyleName, pageNumStyleName, xrefStyleName, refCharStyleName;
		if(titleStyle == -1) // -1 means [No Paragraph Style]
			titleStyleName = rootParaStyle->GetName();
		else
		{
			if(parser->GetStyleName(titleStyle,titleStyleName) != kSuccess)					
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		if(sectionStyle == -1) // -1 means [No Paragraph Style]
			sectionStyleName = rootParaStyle->GetName();
		else
		{
			if(parser->GetStyleName(sectionStyle,sectionStyleName) != kSuccess)					
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		if(pageNumStyle == -1) // -1 means [No Paragraph Style]
			pageNumStyleName = rootCharStyle->GetName();
		else
		{
			if(parser->GetStyleName(pageNumStyle,pageNumStyleName) != kSuccess)					
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		if(xrefStyle == -1) // -1 means [No Paragraph Style]
			xrefStyleName = rootCharStyle->GetName();
		else
		{
			if(parser->GetStyleName(xrefStyle,xrefStyleName) != kSuccess)					
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		if(refCharStyle == -1) // -1 means [No Paragraph Style]
			refCharStyleName = rootCharStyle->GetName();
		else
		{
			if(parser->GetStyleName(refCharStyle,refCharStyleName) != kSuccess)					
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		K2Vector<PMString> topicStyleNames;
		for(int32 i = 0 ; i < topicStyles.size() ; ++i)
		{
			if(topicStyles[i] == -1) // -1 means [No Paragraph Style]
				topicStyleNames.push_back(rootParaStyle->GetName());
			else
			{
				PMString topicStyleName;
				if(parser->GetStyleName(topicStyles[i] ,topicStyleName) != kSuccess)					
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
				topicStyleNames.push_back(topicStyleName);
			}
		}

		if(topicStyleNames.size() != topicStyles.size())
			break;	

		// Create a command to generate the index
		InterfacePtr<ICommand> genIndexCmd(CmdUtils::CreateCommand(kGenerateIndexCmdBoss));

		// Command Data 1: The list of topics to be put into the index
		// NOTE: This Active topic list is very important for the generate index command.
		// If this is not available then no index will be generated.
		InterfacePtr<IActiveTopicListContext> activeTopicListContext(GetExecutionContextSession(), UseDefaultIID());

		UIDRef	activeTopicList = activeTopicListContext->GetActiveTopicList();
		genIndexCmd->SetItemList(UIDList(activeTopicList));

		// Command Data 2: Set the index option preferences		
		InterfacePtr<IIndexOptions> indexOptions(genIndexCmd, UseDefaultIID());
		indexOptions->SetTitle(WideString(title));		
		indexOptions->SetTitleParagraphStyle(titleStyleName);
		indexOptions->SetScope(IIndexOptions::kDocumentScope);

		// Code for including the documents in the book 
		UIDRef bookRef = parser->getCurBook();
		if (bookRef != UIDRef::gNull) // Check whether there is an active book
		{
			IDataBase * dbBook = bookRef.GetDataBase();
			InterfacePtr<IBook> curBook (bookRef, UseDefaultIID());
		
			int32 dummy = 0;
			if(Utils<IBookUtils>()->IsBookContent(doc, curBook, dummy))
			{
				indexOptions->SetBookName(FileUtils::SysFileToPMString(curBook->GetBookFileSpec()));			
				indexOptions->SetScope(IIndexOptions::kBookScope);

				// Get document list to check which documents are already opened
				// Prerequisite for this command : all documents in the book must be opened before the command processing
				InterfacePtr<IApplication> app (GetExecutionContextSession()->QueryApplication());
				InterfacePtr<IDocumentList> docList (app->QueryDocumentList());								
				InterfacePtr<IBookContentMgr> contentMgr (curBook, UseDefaultIID());						
				for(int32 i = 0 ; i < contentMgr->GetContentCount() ; ++i)
				{
					InterfacePtr<IDataLink> contentLink (dbBook, contentMgr->GetNthContent(i), UseDefaultIID());
					PMString * theDocFullName = contentLink->GetFullName();
					IDFile theDocFile = FileUtils::PMStringToSysFile(*theDocFullName);

					IDocument * doc = docList->FindDoc(theDocFile);
					if(!doc) 			
						docToCloseRefs.push_back(layoutHelper.OpenDocument(theDocFile, K2::kSuppressUI));				
				}

			}		
		}
			
		indexOptions->SetShowEntriesOnHiddenLayerFlag(includeHidLayer);
		if(runIn)
			indexOptions->SetIndexFormat(IIndexOptions::kRunIn);
		else
			indexOptions->SetIndexFormat(IIndexOptions::kNested);

		indexOptions->SetShowSectionHeadersFlag(includeSection);
		indexOptions->SetShowEmptySectionsFlag(includeEmptySection);
		indexOptions->SetFollowingTopicSeparator(WideString(topicSep));
		indexOptions->SetBetweenPageNumberSeparator(WideString(pageSep));
		indexOptions->SetBetweenEntriesSeparator(WideString(entrySep));
		indexOptions->SetBeforeXRefSeparator(WideString(xrefSep));
		indexOptions->SetPageRangeSeparator(WideString(pageRangeSep));
		indexOptions->SetEntryEndSeparator(WideString(entryEndSep));

		for(int32 level = 1 ; level <= topicStyleNames.size() ; ++level)
			indexOptions->SetTopicLevelParagraphStyle(level, topicStyleNames[level-1]);
 
		indexOptions->SetSectionHeaderParagraphStyle(sectionStyleName);
		indexOptions->SetPageNumberCharacterStyle(pageNumStyleName);
		indexOptions->SetCrossRefCharacterStyle(xrefStyleName);
		indexOptions->SetReferencedCharacterStyle(refCharStyleName);

		//The ReplaceExistingIndexFlag needs to be set to kTrue if you want to replace the existing index.
		indexOptions->SetReplaceExistingIndexFlag(indexUID != kInvalidUID);

		// Command Data 3: The document that is the target of the command
		InterfacePtr<IIndexCmdData> indexCmdData(genIndexCmd, UseDefaultIID());
		indexCmdData->SetTargetItemRef(doc->GetDocWorkSpace());
	 
		// Command Data 4: Whether we want to update the previously generated (already existing) index story
		//The piUpdateSelectedIndexStory is used to determine the index stories that need to be updated.
		//If you set piUpdateSelectedIndexStory to kTrue and provide a story UIDRef then that index story 
		//would be the only one to be replaced. If piUpdateSelectedIndexStory is set to kTrue but
		//UIDRef is not set properly  then the index would be created but not placed anywhere. 
		//If piUpdateSelectedIndexStory is set to kFalse then all the index stories would get replaced.
		
		InterfacePtr<IBoolData> updateSelectedIndexStory(genIndexCmd, IID_IUPDATESELECTEDINDEXSTORY);
		updateSelectedIndexStory->Set(kTrue);
			
		// Command Data 5: The UID of the selected index story
		// NOTE: The Index will be placed into this story if and only if the story is the generated index.
		// If you specify an arbitrary UID, the index would be created but not placed anywhere.
		InterfacePtr<IUIDData> piUIDData(genIndexCmd, IID_IUIDDATA);		
		piUIDData->Set(::GetUIDRef(txtModel));				

		if((status = CmdUtils::ProcessCommand(genIndexCmd)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(indexUID == kInvalidUID)
		{
			//get the UID of the index in the Place Gun
			InterfacePtr<IPlaceGun> placeGun(db, db->GetRootUID(), UseDefaultIID());
			UID myNewIndex = placeGun->GetFirstPlaceGunItemUID();

			if(myNewIndex != kInvalidUID) // if there is no entry in the index, it is not created, even if the previous did not fail
			{							  // Instead, we get an invalid UID in the place gun, in this case we don't process a replace command, otherwise ID crash
				//Now do the replace
				InterfacePtr<ICommand> replaceCmd(CmdUtils::CreateCommand(kReplaceCmdBoss));
				InterfacePtr<IReplaceCmdData> replaceData(replaceCmd, IID_IREPLACECMDDATA);
				replaceData->Set(db, ::GetUID(frameHier), myNewIndex, false);
				status = CmdUtils::ProcessCommand(replaceCmd);
			}
		}

		// Close documents which  have been opened specially (book index) for this command
		for(int32 i = 0 ; i < docToCloseRefs.size() ; ++i)
			layoutHelper.CloseDocument(docToCloseRefs[i], kFalse, kSuppressUI, kFalse, IDocFileHandler::kSchedule);
		
		CmdUtils::ProcessScheduledCmds();

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

PMString Func_IG::ParseEscapeChars(const PMString& s)
{
	WideString parsed;

	int32 length = s.NumUTF16TextChars();
	for(int32 i = 0 ; i < length ; ++i)
	{
		if(s[i].IsChar('^') && (i + 1 < length)) // EscapeChar detected
		{
			++i;
			uchar16 escapeChar = s[i].GetValue();
			if(escapeChar == '8')
				parsed.Append(kTextChar_Bullet);
			else if(escapeChar == 'p')
				parsed.Append(kTextChar_CR);
			else if(escapeChar == 'n')
				parsed.Append(kTextChar_LF);
			else if(escapeChar == 't')
				parsed.Append(kTextChar_Tab);
			else if(escapeChar == 'h')
				parsed.Append(kTextChar_BreakRunInStyle);
			else if(escapeChar == '_')
				parsed.Append(kTextChar_EmDash);
			else if(escapeChar == 'm')
				parsed.Append(kTextChar_EmSpace);
			else if(escapeChar == '>')
				parsed.Append(kTextChar_EnSpace);
			else if(escapeChar == 'f')
				parsed.Append(kTextChar_FlushSpace);
			else if(escapeChar == '=')
				parsed.Append(kTextChar_EnDash);
			else if(escapeChar == 's')
				parsed.Append(kTextChar_HardSpace);
			else if(escapeChar == '|')
				parsed.Append(kTextChar_HairSpace);
			else if(escapeChar == '<')
				parsed.Append(kTextChar_ThinSpace);
			else if(escapeChar == '~')
				parsed.Append(kTextChar_NoBreakHyphen);
			else if(escapeChar == '-')
				parsed.Append(kTextChar_DiscretionaryHyphen);
		}
		else
			parsed.Append(s[i].GetValue());
	}

	return PMString(parsed);
}