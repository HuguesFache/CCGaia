/*
//
//	File:	PrsSetTOPPositionCmd.cpp
//
//  Author: Trias
//
//	Date:	6-Jun-2008
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2008 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"

// Interface includes
#include "ISubject.h"
#include "IRealNumberData.h"
#include "IMainItemTOPData.h"

// Project includes
#include "PrsID.h"

/** PrsSetTOPPositionCmd
*/
class PrsSetTOPPositionCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsSetTOPPositionCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsSetTOPPositionCmd();
	
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


CREATE_PMINTERFACE(PrsSetTOPPositionCmd, kPrsSetTOPPositionCmdImpl)

/* Constructor
*/
PrsSetTOPPositionCmd::PrsSetTOPPositionCmd(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
PrsSetTOPPositionCmd::~PrsSetTOPPositionCmd()
{
}

/* Do
*/
void PrsSetTOPPositionCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)
			break;

		InterfacePtr<IMainItemTOPData> topData (fItemList.GetRef(0), UseDefaultIID());
		if(topData == nil)
			break;

		// Get the command datas
		InterfacePtr<IRealNumberData> startPosData(this, IID_ISTARTPOSDATA);
		InterfacePtr<IRealNumberData> endPosData(this, IID_IENDPOSDATA);

		// Set new positions
		topData->SetAbscissae(startPosData->GetRealNumber(), endPosData->GetRealNumber());

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void PrsSetTOPPositionCmd::DoNotify()
{	
	ErrorCode status = kFailure;
			
	do
	{
		if (fItemList.Length() != 1)
			break;
		else
		{
			InterfacePtr<ISubject> subject (fItemList.GetRef(0), UseDefaultIID());

			// Notify the subject of the change
			subject->Change(kPrsSetTOPPositionCmdBoss, IID_IMAINITEMTOPDATA, this);

			status = kSuccess;
		}

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}


/* CreateName
*/
PMString * PrsSetTOPPositionCmd::CreateName()
{
	PMString * name = new PMString("Set TOP Position");
	name->SetTranslatable(kFalse);
	return name;
}
