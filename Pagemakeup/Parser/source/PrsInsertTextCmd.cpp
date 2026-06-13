/*
//
//	File:	PrsInsertTextCmd.cpp
//
//  Author: Trias
//
//	Date:	27-mar-2006
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"

// Interface includes
#include "ISubject.h"
#include "IWideStringData.h"
#include "IIntData.h"
#include "ITextModel.h"

// Project includes
#include "PrsID.h"

/** PrsInsertTextCmd
*/
class PrsInsertTextCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsInsertTextCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsInsertTextCmd();
	
		/**
		@return kFalse don't use this command if memory is low. 
	*/
	bool16 LowMemIsOK() const { return kFalse; }

protected:
	/**
		The Do method is where all the action happens. 
	*/
	void Do();

	/**
		Notify interested observer about the change we have done in this command.
	*/
	void DoNotify();

	/**
		Gives the name that shows up under the Edit menu and allows the user to undo or redo the command.

		@return pointer to a PMString allocated using the new operator. Deleted by caller.
	*/
	PMString* CreateName();
};


CREATE_PMINTERFACE(PrsInsertTextCmd, kPrsInsertTextCmdImpl)

/* Constructor
*/
PrsInsertTextCmd::PrsInsertTextCmd(IPMUnknown* boss) 
: Command(boss)
{
	SetUndoability(ICommand::kAutoUndo);
}

/* Destructor
*/
PrsInsertTextCmd::~PrsInsertTextCmd()
{
}

/* Do
*/
void PrsInsertTextCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)
			break;

		InterfacePtr<ITextModel> txtModel (fItemList.GetRef(0), UseDefaultIID());
		if(txtModel == nil)
			break;

		// Get the command datas
		InterfacePtr<IWideStringData> txtToInsert(this, UseDefaultIID());

		boost::shared_ptr<WideString> toInsert (new WideString(txtToInsert->Get()));
		
		InterfacePtr<IIntData> insertPos (this, UseDefaultIID());
	
		txtModel->Insert(insertPos->Get(), toInsert.get());

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void PrsInsertTextCmd::DoNotify()
{
	// No notification to speed up processing
	/*
	ErrorCode status = kFailure;
			
	do
	{
		if (fItemList.Length() != 1)
			break;
		else
		{
			InterfacePtr<ISubject> subject (fItemList.GetRef(0), UseDefaultIID());
			if(subject == nil)
			{
				ASSERT_FAIL("PrsInsertTextCmd::DoNotify -> subject nil");
				break;
			}

			// Notify the subject of the change
			subject->Change(kPrsInsertTextCmdBoss, IID_ITEXTMODEL, this);

			status = kSuccess;
		}

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
	*/
}


/* CreateName
*/
PMString * PrsInsertTextCmd::CreateName()
{
	PMString * name = new PMString("Insert Text");
	name->SetTranslatable(kFalse);
	return name;
}
