/*
//	File:	TLBStyleHelper.cpp
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
#include "ErrorUtils.h"
#include "K2SmartPtr.h"
#include "AttributeBossList.h"

// Interface includes
#include "ICommand.h"
#include "IStyleInfo.h"
#include "ITextAttributes.h"
#include "IApplyStyleData.h"
#include "IStyleGroupManager.h"
#include "IUIDData.h"

// Project includes
#include "TLBStyleHelper.h"

CREATE_PMINTERFACE(TLBStyleHelper,kTLBStyleHelperImpl)

/* Constructor
*/
TLBStyleHelper::TLBStyleHelper(IPMUnknown * boss)
: CPMUnknown<IStyleHelper>(boss)
{
}

/* Destructor
*/
TLBStyleHelper::~TLBStyleHelper()
{
}

/* CreateParaStyleCmd
*/
ErrorCode TLBStyleHelper::CreateParaStyleCmd(PMString name, UIDRef workspace,const AttributeBossList * overrides, UID basedOn, UID nextStyle, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(workspace.GetDataBase() == nil || workspace.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("TLBStyleHelper::CreateParaStyleCmd -> invalid workspace");
			error = PMString(kTlbNilParameter);
			break;
		}


		InterfacePtr<IStyleGroupManager> paraNameTable(workspace, IID_IPARASTYLEGROUPMANAGER);
		if (paraNameTable == nil)
		{
			ASSERT_FAIL("TLBStyleHelper::CreateParaStyleCmd -> document invalid");
			error = PMString(kNewStyleCmdFailed);
			break;
		}
		

		// Look up the style in the paragraph style table:
		UID styleUID = paraNameTable->FindByName(name);
		
		//if the style exists, edit it
		if (styleUID != kInvalidUID)
		{
			if(EditTextStyleAttributesCmd(workspace, styleUID, overrides, allowUndo) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			status = kSuccess;
		}
		else
		{
			InterfacePtr<ICommand> cmdNewStyle(CmdUtils::CreateCommand(kCreateParaStyleCmdBoss));
			if(cmdNewStyle == nil)
			{
				ASSERT_FAIL("TLBStyleHelper::CreateParaStyleCmd -> cmdNewStyle nil");
				break;
			}

			// Set command undoability
			if(!allowUndo)
				cmdNewStyle->SetUndoability(ICommand::kAutoUndo);

			cmdNewStyle->SetItemList(UIDList(workspace));

			// Style info
			InterfacePtr<IStyleInfo> iStyleInfo(cmdNewStyle, UseDefaultIID());
			if(iStyleInfo == nil)
			{
				ASSERT_FAIL("TLBStyleHelper::CreateParaStyleCmd -> iStyleInfo nil");
				break;
			}

			iStyleInfo->SetBasedOn(basedOn);
			iStyleInfo->SetNextStyle(nextStyle);
			iStyleInfo->SetName(name);	
			iStyleInfo->SetStyleType(IStyleInfo::kParagraphStyle);
			iStyleInfo->SetIsStyleImported(kFalse);
			
			// Set style attributes
			InterfacePtr<ITextAttributes> iTextAttributes(cmdNewStyle, UseDefaultIID());
			if(iTextAttributes == nil)
			{
				ASSERT_FAIL("TLBStyleHelper::CreateParaStyleCmd -> iTextAttributes nil");
				break;
			}

			iTextAttributes->ApplyAttributes(overrides);
		
			status = CmdUtils::ProcessCommand(cmdNewStyle);
			if(status != kSuccess)
				error = PMString(kNewStyleCmdFailed);
		}

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;

}

/* DefaultTextStyleCmd
*/
ErrorCode TLBStyleHelper::DefaultTextStyleCmd(UIDRef workspace,ClassID strand, UID style, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(workspace.GetDataBase() == nil || workspace.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("TLBfHelper::DefaultTextStyleCmd -> invalid workspace");
			error = PMString(kTlbNilParameter);
			break;
		}

		InterfacePtr<ICommand> defaultStyleCmd(CmdUtils::CreateCommand(kDefaultTextStyleCmdBoss));
		if(defaultStyleCmd == nil)
		{
			ASSERT_FAIL("TLBStyleHelper::DefaultTextStyleCmd -> defaultStyleCmd nil");
			break;
		}

		// Set command undoability
		if(!allowUndo)
			defaultStyleCmd->SetUndoability(ICommand::kAutoUndo);

		defaultStyleCmd->SetItemList(UIDList(workspace));
		
		InterfacePtr<IApplyStyleData> data(defaultStyleCmd, IID_IAPPLYSTYLEDATA);
		if(data == nil)
		{
			ASSERT_FAIL("TLBStyleHelper::DefaultTextStyleCmd -> data nil");
			break;
		}

		boost::shared_ptr<AttributeBossList> emptyList (new AttributeBossList());
		data->Set(strand, style, emptyList);
		
		status = CmdUtils::ProcessCommand(defaultStyleCmd);
		if(status != kSuccess)
			error = PMString(kDefaultStyleCmdFailed);

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/*	CreateCharStyle
*/
ErrorCode TLBStyleHelper::CreateCharStyleCmd(PMString name, UIDRef workspace,const AttributeBossList * overrides, UID basedOn, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(workspace.GetDataBase() == nil || workspace.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("TLBStyleHelper::CreateCharStyleCmd -> invalid workspace");
			error = PMString(kTlbNilParameter);
			break;
		}	

		InterfacePtr<IStyleGroupManager> charNameTable(workspace, IID_ICHARSTYLEGROUPMANAGER);
		if (charNameTable == nil)
		{
			ASSERT_FAIL("TLBStyleHelper::CreateCharStyleCmd -> document invalid");
			error = PMString(kNewStyleCmdFailed);
			break;
		}
		

		// Look up the style in the paragraph style table:
		UID styleUID = charNameTable->FindByName(name);
		
		//if the style exists, edit it...
		if (styleUID != kInvalidUID)
		{
			if(EditTextStyleAttributesCmd(workspace, styleUID, overrides, allowUndo) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			status = kSuccess;
		}
		else
		{
			// Create the newStyleCmd:
			InterfacePtr<ICommand> newStyleCmd(CmdUtils::CreateCommand(kCreateCharStyleCmdBoss));
			
			// Set command undoability
			if(!allowUndo)
				newStyleCmd->SetUndoability(ICommand::kAutoUndo);

			// Set the UIDList with the document:
			newStyleCmd->SetItemList(UIDList(workspace));

			// Aquire the IStyleInfo interface:
			InterfacePtr<IStyleInfo> styleInfo(newStyleCmd, IID_ISTYLEINFO);			
			if (styleInfo == nil)
			{
				ASSERT_FAIL("TLBStyleHelper::CreateCharStyleCmd: styleInfo invalid");
				error = PMString(kNewStyleCmdFailed);
				break;
			}
			
			// Set the style info:
			styleInfo->SetName(name);
			styleInfo->SetBasedOn(basedOn);
			styleInfo->SetNextStyle(kInvalidUID);
			styleInfo->SetStyleType(IStyleInfo::kCharacterStyle);
			styleInfo->SetIsStyleImported(kFalse);

			// Set style attributes
			InterfacePtr<ITextAttributes> iTextAttributes(newStyleCmd, UseDefaultIID());
			if(iTextAttributes == nil)
			{
				ASSERT_FAIL("TLBStyleHelper::CreateCharStyleCmd -> iTextAttributes nil");
				break;
			}

			iTextAttributes->ApplyAttributes(overrides);

			// Execute the command and report any errors
			status = CmdUtils::ProcessCommand(newStyleCmd);
			if(status != kSuccess)
				error = PMString(kNewStyleCmdFailed);
		}
	} while (false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

ErrorCode TLBStyleHelper::EditTextStyleAttributesCmd(UIDRef workspace, UID styleToEdit, const AttributeBossList * attributes, bool8 allowUndo)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{	
		if(workspace.GetDataBase() == nil || workspace.GetUID() == kInvalidUID)
		{
			ASSERT_FAIL("TLBStyleHelper::EditTextStyleAttributesCmd -> invalid workspace");
			error = PMString(kTlbNilParameter);
			break;
		}	

		// Create the editStyleCmd
		InterfacePtr<ICommand> editStyleCmd(CmdUtils::CreateCommand(kEditTextStyleCmdBoss));

		// Set the UIDList with the document:
		editStyleCmd->SetItemList(UIDList(workspace));
		
		// Set command undoability
		if(!allowUndo)
			editStyleCmd->SetUndoability(ICommand::kAutoUndo);

		// Get new and old style
		InterfacePtr<IStyleInfo> newStyle(editStyleCmd, IID_ISTYLEINFO);
		if (newStyle == nil)
			break;

		InterfacePtr<IStyleInfo> OrigStyle(workspace.GetDataBase(), styleToEdit, IID_ISTYLEINFO);
		if (OrigStyle == nil)
			break;

		
		// Set the style to edit
		InterfacePtr<IUIDData> uidData (editStyleCmd, UseDefaultIID());
		if(uidData == nil)
		{
			ASSERT_FAIL("TLBStyleHelper::uidData -> iTextAttributes nil");
			break;
		}

		uidData->Set(UIDRef(workspace.GetDataBase(),styleToEdit));

		// Set the IStyleInfo Interface's data:
		// copying all the stuff over from the old style
		
		newStyle->SetBasedOn(OrigStyle->GetBasedOn());
		newStyle->SetNextStyle(OrigStyle->GetNextStyle());
		newStyle->SetCharStyle(OrigStyle->GetCharStyle());
		newStyle->SetName(OrigStyle->GetName());
		newStyle->SetStyleType(OrigStyle->GetStyleType());

		// Set style attributes : first copy original attributes, then apply new ones
		InterfacePtr<ITextAttributes> origAttr (OrigStyle, UseDefaultIID());
		if(origAttr == nil)
			break;

		InterfacePtr<ITextAttributes> iTextAttributes(editStyleCmd, UseDefaultIID());
		if(iTextAttributes == nil)
		{
			ASSERT_FAIL("TLBStyleHelper::uidData -> iTextAttributes nil");
			break;
		}

		iTextAttributes->ApplyAttributes(origAttr->GetBossList());
		iTextAttributes->ApplyAttributes(attributes);

		
		// Execute the command and report any errors
		status = CmdUtils::ProcessCommand(editStyleCmd);
		if(status != kSuccess)
			error = PMString(kEditStyleCmdFailed);

	} while (false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

