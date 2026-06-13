/*
//	File:	DLBDocHelper.cpp
//
//  Author: Trias
//
//	Date:	18-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

// Plug-in includes
#include "VCPlugInHeaders.h"

// General includes
#include "CmdUtils.h"
#include "UIDList.h"
#include "LayoutID.h"
#include "ILayoutUtils.h"
#include "ErrorUtils.h"
#include "CAlert.h"
#include "StreamUtil.h"

// Interface includes
#include "ICommand.h"
#include "INewDocCmdData.h"
#include "IDocument.h"
#include "IDocumentUtils.h"
#include "IWindowUtils.h"
#include "IDocFileHandler.h"
#include "IStringData.h"
#include "ICloseWinCmdData.h"
#include "IWindow.h"
#include "IPageMarginsCmdData.h"
#include "IPageColumnsCmdData.h"
#include "IDocSetupCmdData.h"
#include "IClassIDData.h"
#include "IPMStream.h"
#include "ICoreFilename.h"
#include "SDKFileHelper.h"
#include "FileUtils.h"
#include "ICloseWinCmdData.h"

// Project includes
#include "DLBDocHelper.h"

CREATE_PMINTERFACE(DLBDocHelper,kDLBDocHelperImpl)

/* Constructor
*/
DLBDocHelper::DLBDocHelper(IPMUnknown * boss)
: CPMUnknown<IDocHelper>(boss)
{
}

/* Destructor
*/
DLBDocHelper::~DLBDocHelper()
{
}


/* CreateNewDocCmd
*/
UIDRef DLBDocHelper::CreateNewDocCmd(PMRect pageSize,
									 int32 numPages, int32 numPagesPerSpread,
									 int32 numColumns, PMReal gutter, bool8 direction,
									 PMReal left, PMReal top, PMReal right, PMReal bottom, bool8 bWide,
									 bool8 allowUndo
									)
{
	UIDRef newDocUIDRef(nil,kInvalidUID);
	PMString error(kDlbNilInterface);

	do
	{
		// Create a new document
		InterfacePtr<ICommand> newDocCmd(CmdUtils::CreateCommand(kNewDocCmdBoss));

		if(newDocCmd == nil)
		{
			ASSERT_FAIL("DLBDocHelper::CreateNewDocCmd -> newDocCmd nil");
			break;
		}
		
		// Set command undoability
		if(!allowUndo)
			newDocCmd->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<INewDocCmdData> newDocCmdData(newDocCmd, UseDefaultIID());
		if(newDocCmdData == nil)
		{
			ASSERT_FAIL("DLBDocHelper::CreateNewDocCmd -> newDocCmdData nil");
			break;
		}


		newDocCmdData->SetWideOrientation(bWide);

		// Set new document datas
		//newDocCmdData->SetPageSizePref(pageSize);
		newDocCmdData->SetNewDocumentPageSize(PMPageSize(pageSize));
		newDocCmdData->SetPagesPerSpread(numPagesPerSpread);
		newDocCmdData->SetNumPages(numPages);
		newDocCmdData->SetMargins(left,top,right,bottom);
		if (direction) {
			newDocCmdData->SetColumns_4(numColumns,gutter,IColumns::kHorizontalColumnOrientation);
		}
		else {
			newDocCmdData->SetColumns_4(numColumns,gutter,IColumns::kVerticalColumnOrientation);
		}
		
	
		if (CmdUtils::ProcessCommand(newDocCmd) != kSuccess)
		{	
			error = PMString(kNewDocCmdFailed);
			break;
		}

		// Get the new document UIDRef
		const UIDList * uidlist = newDocCmd->GetItemList();
		newDocUIDRef = uidlist->GetRef(0);

	} while(false);
	
	if(newDocUIDRef == UIDRef(nil,kInvalidUID))
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return newDocUIDRef;
}

/* SetDocName
*/
ErrorCode DLBDocHelper::SetDocName(UIDRef doc, PMString name)
{
	ErrorCode status = kFailure;
	PMString error(kDlbNilInterface);

	do
	{
		InterfacePtr<IDocument> newDoc (doc,UseDefaultIID());
		if(newDoc == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetDocName -> newDoc nil");
			error = PMString(kDlbNilParameter);
			break;
		}
	
		// Set the name
		newDoc->SetName(name);

		status = kSuccess;

	} while(false);
	
	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* SetDocNameCmd
*/
ErrorCode DLBDocHelper::SetDocNameCmd(UIDRef doc, PMString name, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kDlbNilInterface);

	do
	{
		// Check the parameters
		if(doc.GetUID() == kInvalidUID || doc.GetDataBase() == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetDocNameCmd -> invalid doc");
			error = PMString(kDlbNilParameter);
			break;
		}
		
		InterfacePtr<ICommand> setDocNameCmd(CmdUtils::CreateCommand(kSetDocNameCmdBoss));
		if(setDocNameCmd == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetDocNameCmd -> setDocNameCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			setDocNameCmd->SetUndoability(ICommand::kAutoUndo);

		setDocNameCmd->SetItemList(UIDList(doc));

		InterfacePtr<IStringData> iStr (setDocNameCmd, UseDefaultIID());
		if(iStr == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetDocNameCmd -> iStr nil");
			break;
		}

		// Set the name
		iStr->Set(name);

		status = CmdUtils::ProcessCommand(setDocNameCmd);
		if(status != kSuccess)
			error = PMString(kSetDocNameCmdFailed);

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* OpenWindowCmd
*/
ErrorCode DLBDocHelper::OpenWindowCmd(UIDRef doc, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kDlbNilInterface);

	do
	{
		// Check the parameters
		if(doc.GetUID() == kInvalidUID || doc.GetDataBase() == nil)
		{
			ASSERT_FAIL("DLBDocHelper::OpenWindowCmd -> invalid doc");
			error = PMString(kDlbNilParameter);
			break;
		}

		InterfacePtr<ICommand> newWinCmd(CmdUtils::CreateCommand(kOpenLayoutCmdBoss));
		if(newWinCmd == nil)
		{
			ASSERT_FAIL("DLBDocHelper::OpenWindowCmd -> newWinCmd nil");
			break;
		}
		
		// Set command undoability
		if(!allowUndo)
			newWinCmd->SetUndoability(ICommand::kAutoUndo);

		newWinCmd->SetItemList(UIDList(doc));
		
		// Open the window
		status = CmdUtils::ProcessCommand(newWinCmd);
		if(status != kSuccess)
			error = PMString(kNewWinCmdFailed);

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* CloseWindowCmd
*/
ErrorCode DLBDocHelper::CloseWindowCmd(UIDRef doc, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kDlbNilInterface);
/*
	do
	{
		// Check the parameters
		if(doc.GetUID() == kInvalidUID || doc.GetDataBase() == nil)
		{
			ASSERT_FAIL("DLBDocHelper::CloseWindowCmd -> invalid doc");
			error = PMString(kDlbNilParameter);
			break;
		}

		InterfacePtr<ICommand> closeWinCmd(CmdUtils::CreateCommand(kCloseLayoutCmdBoss));
		if(closeWinCmd == nil)
		{
			ASSERT_FAIL("DLBDocHelper::CloseWindowCmd -> closeWinCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			closeWinCmd->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<ICloseWinCmdData> winToClose (closeWinCmd,IID_ICLOSEWINCMDDATA);
		if(winToClose == nil)
		{
			ASSERT_FAIL("DLBDocHelper::CloseWindowCmd -> winToClose nil");
			break;
		}

		// Get the frontmost window opened on the document and then close it
		winToClose->Set(Utils<IWindowUtils>()->GetFrontDocWindow(doc.GetDataBase()));

		status = CmdUtils::ProcessCommand(closeWinCmd);
		if(status != kSuccess)
			error = PMString(kCloseWinCmdFailed);

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status,kFalse,&error);
*/
	return status;
}

/* SaveDoc
*/
ErrorCode DLBDocHelper::SaveDoc(UIDRef doc, UIFlags flags)
{
	ErrorCode status = kFailure;
	PMString error(kDlbNilInterface);

	do
	{/*
		InterfacePtr<IDocFileHandler> docFileHandler (Utils<IDocumentUtils>()->QueryDocFileHandler(doc));

		if(docFileHandler == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SaveDoc -> docFileHandler nil");
			break;
		}
		
		if(docFileHandler->CanSave(doc))
		{
			docFileHandler->Save(doc,flags);
		}
		status = kSuccess;
		*/


	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* SaveAsDoc
*/
ErrorCode DLBDocHelper::SaveAsDoc(UIDRef doc, PMString absoluteName)
{
	ErrorCode status = kFailure;
	PMString error(kDlbNilInterface);

	do
	{

		// Get an IDocFileHandler interface for the document:
		InterfacePtr<IDocFileHandler> docFileHandler(Utils<IDocumentUtils>()->QueryDocFileHandler(doc));
		if(docFileHandler == nil)
			break;

		// Use IDocFileHandler's SaveAs() command to implement SaveAsDocCmd:
		if(!docFileHandler->CanSaveAs(doc))
		{
			error = PMString(kErrImpossibleToSave);
			break;
		}		
		
		IDFile file = FileUtils::PMStringToSysFile(absoluteName);
		IDFile parent;
		/*
		if(FileUtils::GetParentDirectory(file, parent) == kFalse)// Invalid path
		{
       		error = PMString(kErrImpossibleToSave); 
            break;
        }
		*/
		
		FileUtils::GetParentDirectory(file, parent) ;
			
		if(FileUtils::DoesFileExist (parent) == kFalse)// Invalid path
		{
       		error = PMString(kErrImpossibleToSave); 
            break;
        }
		
		docFileHandler->SaveAs(doc,&file);

		status = ErrorUtils::PMGetGlobalErrorCode();
		if(status != kSuccess)
		{
			ErrorUtils::PMSetGlobalErrorCode(kTrue);
			error = PMString(kErrImpossibleToSave);
			break;
		}

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);
		
	return status;
}




/* SetPageSizeCmd
*/
ErrorCode DLBDocHelper::SetPageSizeCmd(UIDRef doc, PMRect newPageSize, bool16 bWide, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kDlbNilInterface);

	do
	{
		// Check the parameters
		if(doc.GetUID() == kInvalidUID || doc.GetDataBase() == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetMarginCmd -> invalid doc");
			error = PMString(kDlbNilParameter);
			break;
		}

		// Create a SetPageSizeCmd:
		InterfacePtr<ICommand> pageSizeCmd(CmdUtils::CreateCommand(kSetPageSizeCmdBoss));

		if(pageSizeCmd == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetPageSizeCmd -> pageSizeCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			pageSizeCmd->SetUndoability(ICommand::kAutoUndo);

		// Get an IDocSetupCmdData Interface for the SetPageSizeCmd:
		InterfacePtr<IDocSetupCmdData> pageData(pageSizeCmd, IID_IDOCSETUPCMDDATA);
		if(pageData == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetPageSizeCmd -> pageData nil");
			break;
		}

		// Set the IDocSetupCmdData Interface's data:
		//pageData->Set(doc, newPageSize, 1, 2, bWide, kDefaultBinding);
		pageData->SetDocSetupCmdData(doc, PMPageSize(newPageSize), 1, 2, bWide, kDefaultBinding);

		// Process the SetPageSizeCmd:
		status = CmdUtils::ProcessCommand(pageSizeCmd);
		if (status != kSuccess)
			error = PMString(kSetPageSizeCmdFailed);

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}





/* SetMarginCmd
*/
ErrorCode DLBDocHelper::SetMarginCmd(UIDList& pageList, PMReal mg,PMReal mh,PMReal md,PMReal mb, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kDlbNilInterface);

	do
	{

		// Create a SetPageMarginsCmd:
		InterfacePtr<ICommand>setPMCmd(CmdUtils::CreateCommand(kSetPageMarginsCmdBoss));
		if(setPMCmd == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetMarginCmd -> setMPCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			setPMCmd->SetUndoability(ICommand::kAutoUndo);

		// Set the SetPageMarginsCmd's ItemList:
		setPMCmd->SetItemList(pageList);


		// Get an IPageMarginsCmdData Interface for the SetPageMarginsCmd:
		InterfacePtr<IPageMarginsCmdData> cmdData(setPMCmd,	IID_IPAGEMARGINSCMDDATA);
		if(cmdData == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetMarginCmd -> cmdData nil");
			break;
		}

		// Set the IPageMarginsCmdData Interface's data:
		cmdData->Set(mg, mh, md, mb,kTrue);

		// Process the SetPageMarginsCmd:
		status = CmdUtils::ProcessCommand(setPMCmd);
		if (status != kSuccess)
			error = PMString(kSetMarginCmdFailed);

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* SetColumnCmd
*/
ErrorCode DLBDocHelper::SetColumnCmd(UIDList& pageList,PMRealList& columns, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kDlbNilInterface);

	do
	{
	
		// Create a SetPageColumnsCmd:
		InterfacePtr<ICommand>setPCCmd(CmdUtils::CreateCommand(kSetPageColumnsCmdBoss));
		if(setPCCmd == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetColumnCmd -> setPCCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			setPCCmd->SetUndoability(ICommand::kAutoUndo);


		// Set the SetPageColumnsCmd's ItemList:
		setPCCmd->SetItemList(pageList);

		// Get an IPageColumnsCmdData Interface for the SetPageColumnsCmd:
		InterfacePtr<IPageColumnsCmdData> cmdData(setPCCmd,IID_IPAGECOLUMNSCMDDATA);
		if(cmdData == nil)
		{
			ASSERT_FAIL("DLBDocHelper::SetColumnCmd -> cmdData nil");
			break;
		}

		// Set the IPageColumnsCmdData Interface's data:
		//Porting CS5 : Set to SetPageColumnsCmdData
		cmdData->SetPageColumnsCmdData_(columns, IColumns::kHorizontalColumnOrientation, kTrue, kFalse);

		// Process the SetPageColumnsCmd:
		status = CmdUtils::ProcessCommand(setPCCmd);
		if (status != kSuccess)
			error = PMString(kSetColumnCmdFailed);


	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}




/* IsGoodPath
*/
ErrorCode DLBDocHelper::ConvertPath(PMString &fileName,IDFile& file)
{
	ErrorCode status = kFailure;
	PMString error(kErrDocNotFound);
	/*
	do
	{
		
		int32 err = 0;

		InterfacePtr<ICoreFilename> 
			cfn((ICoreFilename*)::CreateObject(kCoreFilenameBoss, IID_ICOREFILENAME));
		if (cfn != nil)
		{
			err = cfn->Initialize(&fileName);
    
			// if success
			if (err == 0)
			{
				err = cfn->GetSysFile(&file); 
				if (err == 1)
				{ 
					error = PMString("Different platform expected."); 
					break;
				}
			}
			else if (err == 1)
			{ 
				error = PMString("Invalid filename or volume name."); 
				break;
			}
			else if (err == 2)
			{ 
				error = PMString("FSMakeFSSpec() failed."); 
				break;
			}
			else if (err == 3)
			{ 
				error = PMString("Invalid path or filename."); 
				break;
			}
			else if (err == 4)
			{ 
				error = PMString("Invalid dirID."); 
				break;
			}
			else if (err == 5)
			{ 
				error = PMString("Problem with delimiter."); 
				break;
			}

			// Open a write stream to the picture file
			//InterfacePtr<IPMStream> verifyStream (SDKUtilities::CreateFileStreamWrite(fileName));
			InterfacePtr<IPMStream> verifyStream (StreamUtil::CreateFileStreamWrite(file));
			if(verifyStream == nil)
			{
				error = PMString("erreur chemin fichier");
				break;
			}
		}
		else
		{ 
			ASSERT_FAIL("SDKUtilities::PMStringToSysFile(), Invalid ICoreFilename*."); 
			break;
		}

		status = kSuccess ;


	} while(false);
	
	*/
	
	SDKFileHelper fileHelper(fileName);
	if(fileHelper.IsExisting())
	{
		file = fileHelper.GetIDFile();
		status = kSuccess;
	}
	
	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}
