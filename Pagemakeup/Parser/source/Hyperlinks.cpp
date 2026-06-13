/*
//	File:	Hyperlinks.cpp
//
//	Date:	09-07-2007
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "ILayoutUtils.h"

// Interface includes
#include "ICommand.h"
#include "IHyperlinkTable.h"
#include "IBookMarkData.h"
#include "IDocument.h"
#include "IUIDData.h"
#include "ITextModel.h"
#include "ISubject.h"
#include "IIntData.h"
#include "IHierarchy.h"
#include "IRealNumberData.h"
#include "IHyperlinkDestination.h"
#include "IStringData.h"
#include "IBoolData.h"
#include "IPageList.h"
#include "IRangeData.h"
#include "IHyperlinkCmdData.h"
#include "IUIColorUtils.h"
#include "IPageItemTypeUtils.h"
#include "IMultiColumnTextFrame.h"

// Project includes
#include "Hyperlinks.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"

#include "CAlert.h"
#include "DebugClassUtils.h"

/* Constructor
*/
Func_SI::Func_SI(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_SI::Func_SI -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		bkType = reader->ExtractInt(0, 0, 2);
		bkName = reader->ExtractString(255);
		bkParentName = reader->ExtractString(255);

		if(bkType == 2)
			hlDestName = reader->ExtractString(255);
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
		IDCall_SI(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));


}

/* IDCall_SI
*/
void Func_SI::IDCall_SI(ITCLParser * parser)
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

		// Get table of links which references all bookmarks
		InterfacePtr<IHyperlinkTable> hyperlinkTable (doc, UseDefaultIID());
		int32 nbBookMarks = hyperlinkTable->GetBookmarkCount();

		// If name is empty, generate name automatically
		if(bkName.IsEmpty())
			hyperlinkTable->GenerateNewHyperlinkBookmarkName (&bkName);
		else // Otherwise check bookmark name unicity
		{			
			int32 i = 0;
			for( ; i < nbBookMarks ; ++i)
			{
				InterfacePtr<IBookMarkData> bkData (db, hyperlinkTable->GetNthBookmark(i), UseDefaultIID());
				if(bkData->GetName().Compare(kFalse, bkName) == 0)
					break;
			}

			if(i < nbBookMarks) // error, name already exists
			{
				error = PMString(kErrBkNameAlreadyExists);
				break;
			}
		}
		
		// Look for parent if asked for
		UID parentUID = ::GetUID(doc);
		int32 indentLevel = 0;
		if(!bkParentName.IsEmpty())
		{
			int32 i = 0;
			for( ; i < nbBookMarks ; ++i)
			{
				InterfacePtr<IBookMarkData> bkParentData (db, hyperlinkTable->GetNthBookmark(i), UseDefaultIID());
				if(bkParentData->GetName().Compare(kFalse, bkParentName) == 0)
				{
					parentUID = ::GetUID(bkParentData);
					indentLevel = bkParentData->GetIndentLevel() + 1;
					break;
				}
			}

			if(i == nbBookMarks) // error, name doesn't exist
			{
				error = PMString(kErrBkParentNotExists);
				break;
			}
		}

		// Create hyperlink destination before, if required
		UID hlDestUID = kInvalidUID;
		if(bkType == 0)
		{
			// Get current story uid			
			TextIndex insertPos = kInvalidTextIndex;
			InterfacePtr<ITextModel> curTxtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (curTxtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			InterfacePtr<ICommand> newHLDestCmd (CmdUtils::CreateCommand(kNewHyperlinkTextDestCmdBoss));
			newHLDestCmd->SetUndoability(ICommand::kAutoUndo);
			
			InterfacePtr<IUIDData> txtStoryData (newHLDestCmd, UseDefaultIID());
			txtStoryData->Set(db, ::GetUID(curTxtModel));

			InterfacePtr<IIntData> indexData (newHLDestCmd, UseDefaultIID());
			indexData->Set(insertPos);

			if(CmdUtils::ProcessCommand(newHLDestCmd) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}

			hlDestUID = newHLDestCmd->GetItemList()->At(0);
		}
		else if(bkType == 1)		
		{
			// Get current item UID
			UID refUID = parser->getItem(parser->getCurItem());
			if(refUID == kInvalidUID)
			{
				error = PMString(kErrInvalidItem);
				break;
			}

			// Get owner page uid for this item
			InterfacePtr<IHierarchy> curItemHier (db, refUID, UseDefaultIID());
			UID ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(curItemHier);

			InterfacePtr<ICommand> newHLDestCmd (CmdUtils::CreateCommand(kNewHyperlinkPageDestCmdBoss));
			newHLDestCmd->SetUndoability(ICommand::kAutoUndo);

			InterfacePtr<IUIDData> pageData (newHLDestCmd, UseDefaultIID());
			pageData->Set(db, ownerPageUID);

			InterfacePtr<IRealNumberData> zoomData (newHLDestCmd, UseDefaultIID());
			zoomData->Set(0.7234);

			if(CmdUtils::ProcessCommand(newHLDestCmd) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}

			hlDestUID = newHLDestCmd->GetItemList()->At(0);
		}
		else // bkType = 2 => hyperlink destination already exist
		{
			InterfacePtr<IHyperlinkTable> hyperlinkTable (doc, UseDefaultIID());
			int32 nbHLDests = hyperlinkTable->GetHyperlinkDestinationCount();

			int32 i = 0;
			for( ; i < nbHLDests ; ++i)
			{
				InterfacePtr<IHyperlinkDestination> hlDestData (db, hyperlinkTable->GetNthHyperlink(i), UseDefaultIID());
				PMString comparedName = kNullString;
				hlDestData->GetName(&comparedName);
				if(comparedName.Compare(kFalse, hlDestName) == 0)
				{
					hlDestUID = ::GetUID(hlDestData);
					break;
				}
			}

			if(i >= nbHLDests) // error, name does not exist
			{
				error = PMString(kErrHyperlinkDestNotExist);
				break;
			}
		}
		
		// Create insert bookmark cmd
		InterfacePtr<ICommand> newBookMarkCmd (CmdUtils::CreateCommand(kNewBookmarkCmdBoss));
		newBookMarkCmd->SetUndoability(ICommand::kAutoUndo);
		newBookMarkCmd->SetItemList(UIDList(doc));

		InterfacePtr<IBookMarkData> cmdData (newBookMarkCmd, UseDefaultIID());
		cmdData->SetName(bkName);
		cmdData->SetContainingBookMarkUID(parentUID);
		cmdData->SetDestinationUID(hlDestUID);
		cmdData->SetIndentLevel(indentLevel);

		if(CmdUtils::ProcessCommand(newBookMarkCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}
		
		InterfacePtr<IBookMarkData> bkData (newBookMarkCmd->GetItemList()->GetRef(0), UseDefaultIID());
		if(bkData->ValidateBookmark() != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_H1::Func_H1(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_H1::Func_H1 -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		hlDestName = reader->ExtractString(255);
		visible =  reader->ExtractBooleen(kTrue);
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
		IDCall_H1(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));

}


/* IDCall_H1
*/
void Func_H1::IDCall_H1(ITCLParser * parser)
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

		if(CheckHyperlinkDestNameUnicity(doc, hlDestName, error) != kSuccess)
			break;		

		// Get current story uid			
		TextIndex insertPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> curTxtModel (parser->QueryCurrentModelTextIndex(insertPos));
		if (curTxtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		InterfacePtr<ICommand> newHLDestCmd (CmdUtils::CreateCommand(kNewHyperlinkTextDestCmdBoss));
		newHLDestCmd->SetUndoability(ICommand::kAutoUndo);
		
		InterfacePtr<IUIDData> txtStoryData (newHLDestCmd, UseDefaultIID());
		txtStoryData->Set(db, ::GetUID(curTxtModel));

		InterfacePtr<IIntData> indexData (newHLDestCmd, UseDefaultIID());
		indexData->Set(insertPos);

		InterfacePtr<IStringData> nameData (newHLDestCmd, IID_IHYPERLINKNAMEDATA);
		nameData->Set(hlDestName);

		InterfacePtr<IBoolData> visibleData (newHLDestCmd, UseDefaultIID());
		visibleData->Set(visible);

		if(CmdUtils::ProcessCommand(newHLDestCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

ErrorCode Func_H1::CheckHyperlinkDestNameUnicity(IDocument * doc, PMString& name, PMString& error)
{
	ErrorCode status = kFailure;
	do
	{
		IDataBase * db = ::GetDataBase(doc);

		// Get table of links which references all hyperlink destinations
		InterfacePtr<IHyperlinkTable> hyperlinkTable (doc, UseDefaultIID());
		int32 nbHLDests = hyperlinkTable->GetHyperlinkDestinationCount();

		// If name is empty, generate name automatically
		if(name.IsEmpty())
			hyperlinkTable->GenerateNewHyperlinkDestinationName (&name);
		else // Otherwise check hyperlink destination name unicity
		{			
			int32 i = 0;
			for( ; i < nbHLDests ; ++i)
			{
				InterfacePtr<IHyperlinkDestination> hlDestData (db, hyperlinkTable->GetNthHyperlink(i), UseDefaultIID());
				PMString comparedName = kNullString;
				hlDestData->GetName(&comparedName);
				if(comparedName.Compare(kFalse, name) == 0)
					break;
			}

			if(i < nbHLDests) // error, name already exists
			{
				error = PMString(kErrHLDestNameAlreadyExists);
				break;
			}
		}

		status = kSuccess;

	} while(kFalse);

	return status;
}

/* Constructor
*/
Func_H2::Func_H2(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_H2::Func_H2 -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		hlDestName = reader->ExtractString(255);
		pageNumber = reader->ExtractInt(0, 0, MAX_PAGE);
		visible =  reader->ExtractBooleen(kTrue);
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
		IDCall_H2(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));

}


/* IDCall_H2
*/
void Func_H2::IDCall_H2(ITCLParser * parser)
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

		if(Func_H1::CheckHyperlinkDestNameUnicity(doc, hlDestName, error) != kSuccess)
			break;	
	
		// Get page uid
		if(pageNumber == CURRENT_PAGE)
		{
			if(parser->getCurPage() != -1)
				pageNumber = parser->getCurPage() ;
			else
				pageNumber = 1;
		}

		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		if(pageNumber > pageList->GetPageCount())
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}

		InterfacePtr<ICommand> newHLDestCmd (CmdUtils::CreateCommand(kNewHyperlinkPageDestCmdBoss));
		newHLDestCmd->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<IUIDData> frameData (newHLDestCmd, UseDefaultIID());
		frameData->Set(db, pageList->GetNthPageUID(pageNumber-1));

		InterfacePtr<IRealNumberData> zoomData (newHLDestCmd, UseDefaultIID());
		zoomData->Set(0.7234);

		InterfacePtr<IStringData> nameData (newHLDestCmd, IID_IHYPERLINKNAMEDATA);
		nameData->Set(hlDestName);

		InterfacePtr<IBoolData> visibleData (newHLDestCmd, UseDefaultIID());
		visibleData->Set(visible);

		if(CmdUtils::ProcessCommand(newHLDestCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}


/* Constructor
*/
Func_H3::Func_H3(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_H3::Func_H3 -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		hlDestName = reader->ExtractString(255);
		url = reader->ExtractString(255);
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
		IDCall_H3(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));

}


/* IDCall_H3
*/
void Func_H3::IDCall_H3(ITCLParser * parser)
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
		
		if(Func_H1::CheckHyperlinkDestNameUnicity(doc, hlDestName, error) != kSuccess)
			break;	

		InterfacePtr<ICommand> newHLDestCmd (CmdUtils::CreateCommand(kNewHyperlinkURLDestCmdBoss));
		newHLDestCmd->SetUndoability(ICommand::kAutoUndo);
		newHLDestCmd->SetItemList(UIDList(doc));

		InterfacePtr<IStringData> urlData (newHLDestCmd, UseDefaultIID());
		urlData->Set(url);

		InterfacePtr<IStringData> nameData (newHLDestCmd, IID_IHYPERLINKNAMEDATA);
		nameData->Set(hlDestName);

		if(CmdUtils::ProcessCommand(newHLDestCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/** Static initialization
*/
PMString Func_HK::hlName = kNullString;
PMString Func_HK::hlDestName = kNullString;
UIDRef Func_HK::hlDestRef = UIDRef::gNull;
bool16 Func_HK::visible = kTrue;
TextIndex Func_HK::insertPos = kInvalidTextIndex;
UIDRef Func_HK::txtModelRef = UIDRef::gNull;
int32 Func_HK::borderWidth = 1;
int32 Func_HK::hilightType = 0;
UID Func_HK::borderColorUID = kInvalidUID;
int32 Func_HK::outlineStyle = 0;

/* Constructor
*/
Func_HK::Func_HK(ITCLParser *  parser) 
: endPos(kInvalidTextIndex)

{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_HK::Func_HK -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		hlType = reader->ExtractInt(0, 0, 2);
		hlName = reader->ExtractString(255);
		hlDestName = reader->ExtractString(255);
		borderColor = reader->ExtractInt(-1,-1,MAX_COLOR);
		hilightType = reader->ExtractInt(0, 0, 3);
		outlineStyle = reader->ExtractInt(0, 0, 1);
		borderWidth = reader->ExtractInt(1,1,3);		
		visible = reader->ExtractBooleen(kTrue);

		if(hlType == 2) // Special case, use by reverse TCL
		{
			insertPos = reader->ExtractInt(0, 0, kMaxInt32);
			endPos = reader->ExtractInt(0, 0, kMaxInt32);			
		}
		else
			insertPos = kInvalidTextIndex; 

		// Reinitialize global variables		
		txtModelRef = UIDRef::gNull;
		hlDestRef = UIDRef::gNull;
		borderColorUID = kInvalidUID;
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
		IDCall_HK(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));

}

/* CreateTextSourceHyperlinkCmd
*/
UIDRef Func_HK::CreateTextSourceHyperlinkCmd(const PMString& name, ITextModel * txtModel, const RangeData& txtRange, const bool16& visible, PMString& error)
{
	UIDRef hlSrcRef = UIDRef::gNull;

	// GD 7.6.0 ++
	// NOTE : cette fonction plante en CS5.5, le code CS2 ne fonctionne plus.
	// voir /Developer/Adobe_InDesign_CS5.5_Products_SDK/source/open/components/hyperlinkpanel/HyperlinkPanelUtils.cpp > CreateHyperlinkFromSelectedURL
	// pour re-developper Func_HK::CreateTextSourceHyperlinkCmd ainsi que CreateHyperlinkCmd ci-dessous, en attente accord (et budget) NewsMed qui se montrent
	// intéressés pour pouvoir mettre des hyperliens sur leur PA.
	// GD 7.6.0 --
	
	do
	{
		InterfacePtr<ICommand> newHLSrcCmd (CmdUtils::CreateCommand(kNewHyperlinkTextSourceCmdBoss));
		newHLSrcCmd->SetUndoability(ICommand::kAutoUndo);

		InterfacePtr<IUIDData> txtModelData (newHLSrcCmd, UseDefaultIID());
		txtModelData->Set(txtModel);

		InterfacePtr<IRangeData> rangeData (newHLSrcCmd, UseDefaultIID());
		rangeData->SetRange(txtRange);

		InterfacePtr<IBoolData> visibleData (newHLSrcCmd, UseDefaultIID());
		visibleData->Set(visible);

		InterfacePtr<IStringData> nameData (newHLSrcCmd, IID_IHYPERLINKNAMEDATA);
		nameData->Set(name);

		if(CmdUtils::ProcessCommand(newHLSrcCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		hlSrcRef = newHLSrcCmd->GetItemList()->GetRef(0);

	} while(kFalse);

	return hlSrcRef;
}

/* CreateHyperlinkCmd
*/
ErrorCode Func_HK::CreateHyperlinkCmd(const PMString& name, const UIDRef& source, const UIDRef& dest, PMString& error)
{
	ErrorCode status = kFailure;

	do
	{
		InterfacePtr<ICommand> newHLCmd (CmdUtils::CreateCommand(kNewHyperlinkCmdBoss));
		newHLCmd->SetUndoability(ICommand::kAutoUndo);
		newHLCmd->SetItemList(UIDList(source));

		InterfacePtr<IHyperlinkCmdData> linkData (newHLCmd, IID_IHYPERLINKCMDDATA);
		linkData->Set(dest, &name);
		linkData->SetBorderLineWidth(borderWidth);		
		linkData->SetHyperlinkHilight((IHyperlinkAppearanceData::HyperlinkHilight) hilightType);
		if(borderColorUID != kInvalidUID)
			linkData->SetBorderColorUID(borderColorUID);

		linkData->SetHyperlinkOutlineLineStyle((IHyperlinkAppearanceData::HyperlinkOutlineLineStyle) outlineStyle);
		linkData->SetVisible(visible);

		if(CmdUtils::ProcessCommand(newHLCmd) != kSuccess)
		{
			error = PMString(kErrCmdFailed);
			break;
		}

		status = kSuccess;

	} while(kFalse);

	return status;
}
/* IDCall_HK
*/
void Func_HK::IDCall_HK(ITCLParser * parser)
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

		// Check hyperlink source name is not empty
		if(hlName.IsEmpty())
		{
			error = PMString(kErrHyperlinkNameEmpty);
			break;
		}

		// Check hyperlink destination exist
		if(hlDestName.IsEmpty())
		{
			error = PMString(kErrHyperlinkDestNotExist);
			break;
		}

		// Check hyperlink border color exist
		if(borderColor != -1)
		{
			borderColorUID = Utils<IUIColorUtils>()->GetUIColor(borderColor, doc).GetUID() ;
			if (borderColorUID == kInvalidUID)
			{
				error = PMString(kErrColorNotFound);
				break;
			}
		}
		else
			borderColorUID = kInvalidUID;
		
		InterfacePtr<IHyperlinkTable> hyperlinkTable (doc, UseDefaultIID());
		int32 nbHLDests = hyperlinkTable->GetHyperlinkDestinationCount();

		// GD 7.6.0 - 28.03.2014 ++
		// Portage CS2 > CS5.5
		//int32 i = 0;
		//for( ; i < nbHLDests ; ++i)
		//{
			//InterfacePtr<IHyperlinkDestination> hlDestData (db, hyperlinkTable->GetNthHyperlink(i), UseDefaultIID());
		
		int32 i = nbHLDests;
		
		UniqueKeyToUIDMap::const_iterator first = hyperlinkTable->GetDestinationKeyToUIDMapBeginIter();
		UniqueKeyToUIDMap::const_iterator last = hyperlinkTable->GetDestinationKeyToUIDMapEndIter();
		for(; first != last; ++first)
		{
			UID destUID = first->second;
			InterfacePtr<IHyperlinkDestination> hlDestData (db, destUID, UseDefaultIID());
		// GD 7.6.0 - 28.03.2014 --
		
			PMString comparedName = kNullString;
			hlDestData->GetName(&comparedName);
			if(comparedName.Compare(kFalse, hlDestName) == 0)
			{
				hlDestRef = ::GetUIDRef(hlDestData);
				i = 0;
				break;
			}
		}

		if(i >= nbHLDests) // error, name does not exist
		{
			error = PMString(kErrHyperlinkDestNotExist);
			break;
		}
				
		if(hlType == 0) // Text source
		{
			// Get start index for creating hyperlink text source at HL call
			InterfacePtr<ITextModel> curTxtModel (parser->QueryCurrentModelTextIndex(insertPos));
			if (curTxtModel == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}

			txtModelRef = ::GetUIDRef(curTxtModel);
		}
		else if(hlType == 1)// Page item source : create hyperlink source item immediatly
		{			
			// Get current item UID
			UID refUID = parser->getItem(parser->getCurItem());
			if(refUID == kInvalidUID)
			{
				error = PMString(kErrInvalidItem);
				break;
			}

			InterfacePtr<ICommand> newHLSrcCmd (CmdUtils::CreateCommand(kNewHyperlinkPgItemSourceCmdBoss));
			newHLSrcCmd->SetUndoability(ICommand::kAutoUndo);

			InterfacePtr<IUIDData> frameData (newHLSrcCmd, UseDefaultIID());
			frameData->Set(db, refUID);

			InterfacePtr<IBoolData> visibleData (newHLSrcCmd, UseDefaultIID());
			visibleData->Set(visible);

			InterfacePtr<IStringData> nameData (newHLSrcCmd, IID_IHYPERLINKNAMEDATA);
			nameData->Set(hlName);

			if(CmdUtils::ProcessCommand(newHLSrcCmd) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}

			UIDRef hlSrcRef = newHLSrcCmd->GetItemList()->GetRef(0);

			// Finally create hyperlink
			if(CreateHyperlinkCmd(hlName, hlSrcRef, hlDestRef, error) != kSuccess)
				break;
		}
		else if(hlType == 2) // special case, create text source item immediatly
		{
			// Check parameters integrity
			UID refUID = parser->getItem(parser->getCurItem());
			if(refUID == kInvalidUID)
			{
				error = PMString(kErrInvalidItem);
				break;
			}

			InterfacePtr<IHierarchy> frameHier (db, refUID, UseDefaultIID());
			if(!Utils<IPageItemTypeUtils>()->IsTextFrame(frameHier))
			{
				error = PMString(kErrTextBoxRequired);
				break;
			}		

			InterfacePtr<IMultiColumnTextFrame> txtFrame (db, frameHier->GetChildUID(0), UseDefaultIID());
			InterfacePtr<ITextModel> txtModel (txtFrame->QueryTextModel());

			if(endPos <= insertPos || endPos >= txtModel->GetPrimaryStoryThreadSpan() - 1)
			{
				error = PMString(kErrInvalidHyperlinkIndex);
				break;
			}

			// Create hyperlink source item
			UIDRef hlSrcRef = CreateTextSourceHyperlinkCmd(hlName, txtModel, RangeData(insertPos, endPos), visible, error);
			if(hlSrcRef == UIDRef::gNull)
				break;

			// Finally create hyperlink
			if(CreateHyperlinkCmd(hlName, hlSrcRef, hlDestRef, error) != kSuccess)
				break;
		}

		status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

/* Constructor
*/
Func_HL::Func_HL(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_HL::Func_HL -> reader nil");
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
		IDCall_HL(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));

}


/* IDCall_HL
*/
void Func_HL::IDCall_HL(ITCLParser * parser)
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

		if(Func_HK::txtModelRef == UIDRef::gNull || Func_HK::insertPos == kInvalidTextIndex)
		{
			error = PMString(kErrInvalidHLCall);
			break;
		}		

		// Get range for hyperlink text source
		TextIndex endPos = kInvalidTextIndex;
		InterfacePtr<ITextModel> curTxtModel (parser->QueryCurrentModelTextIndex(endPos));
		if (curTxtModel == nil)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}		

		RangeData sourceRange (Func_HK::insertPos, endPos-1);

		// Create hyperlink source item
		UIDRef hlSrcRef = Func_HK::CreateTextSourceHyperlinkCmd(Func_HK::hlName, curTxtModel, sourceRange, Func_HK::visible, error);
		if(hlSrcRef == UIDRef::gNull)
			break;

		// Finally create hyperlink
		if(Func_HK::CreateHyperlinkCmd(Func_HK::hlName, hlSrcRef, Func_HK::hlDestRef, error) != kSuccess)
			break;

		status = kSuccess;
		
	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}