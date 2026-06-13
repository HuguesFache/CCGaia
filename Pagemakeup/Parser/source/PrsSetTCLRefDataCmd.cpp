/*
//
//	File:	PrsSetTCLRefDataCmd.cpp
//
//  Author: Trias
//
//	Date:	29-nov-2006
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2006 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"

// Interface includes
#include "ISubject.h"
#include "IScriptLabel.h"
#include "PMUTypes.h"

// Project includes
#include "PrsID.h"


/* SerializeTCLRefToString
 *
 * Encode a TCLRef into a flat PMString suitable for an IScriptLabel value.
 * Format depends on the REF_AS_STRING build flag (see PMUTypes.h):
 *   * String build: the TCLRef IS already a PMString — copy as-is.
 *   * Int build:    decimal representation.
 *
 * The chosen format must round-trip with ParseTCLRefFromString below
 * (used by the open-doc reconciliation in PrsRespService).
*/
static void SerializeTCLRefToString(const TCLRef& ref, PMString& out)
{
#if REF_AS_STRING
	out = ref;
#else
	out.Clear();
	out.AppendNumber(ref);
#endif
}


/* WriteTCLRefLabel
 *
 * Stamp the page item's IScriptLabel with the current TCLRef value under
 * the well-known key "PMU_TCLRef". Survives the things the binary
 * persistence does NOT survive: IDML round-trip, snippet copy, opening
 * the doc in an InDesign without Pagemakeup loaded.
 *
 * The matching read-back lives in PrsRespService::Respond's
 * kDuringOpenDocSignalResponderService branch.
*/
static const char* const kPrsTCLRefLabelKey = "PMU_TCLRef";

static void WriteTCLRefLabel(const UIDRef& itemRef, const TCLRef& ref)
{
	InterfacePtr<IScriptLabel> labelData(itemRef, IID_ISCRIPTLABEL);
	if (labelData == nil)
		return; // item type doesn't expose script labels — nothing we can do.

	PMString labelKey(kPrsTCLRefLabelKey, PMString::kUnknownEncoding);
	labelKey.SetTranslatable(kFalse);

	PMString labelValue;
	if (ref == kInvalidTCLRef)
	{
		// Clear by writing empty — IScriptLabel doesn't expose a per-key
		// remove, and GetTag on a missing key also returns "", so writing
		// "" is observationally equivalent to absence.
		labelValue = kNullString;
	}
	else
	{
		SerializeTCLRefToString(ref, labelValue);
	}
	labelValue.SetTranslatable(kFalse);
	labelData->SetTag(labelKey, labelValue);
}

/** PrsSetTCLRefDataCmd
*/
class PrsSetTCLRefDataCmd : public Command
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsSetTCLRefDataCmd(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsSetTCLRefDataCmd();
	
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

private :

};


CREATE_PMINTERFACE(PrsSetTCLRefDataCmd, kPrsSetTCLRefDataCmdImpl)

/* Constructor
*/
PrsSetTCLRefDataCmd::PrsSetTCLRefDataCmd(IPMUnknown* boss) 
: Command(boss)
{
}

/* Destructor
*/
PrsSetTCLRefDataCmd::~PrsSetTCLRefDataCmd()
{
}

/* Do
*/
void PrsSetTCLRefDataCmd::Do()
{
	ErrorCode status = kFailure;

	do
	{
		if(fItemList.Length() != 1)
			break;

		InterfacePtr<ITCLRefData> tclRefData (fItemList.GetRef(0), IID_ITCLREFDATA);
		if(tclRefData == nil)
			break;

		// Get the command data and apply new reference
		InterfacePtr<ITCLRefData> cmdData (this, IID_ITCLREFDATA);
		const TCLRef newRef = cmdData->Get();
		tclRefData->Set(newRef);

		// Mirror the value in the item's IScriptLabel so it survives IDML
		// round-trip, snippets, and being opened by an InDesign without
		// Pagemakeup loaded. PrsRespService re-hydrates the binary impl
		// from this label on next open if needed.
		WriteTCLRefLabel(fItemList.GetRef(0), newRef);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
	{
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}

/* DoNotify
*/
void PrsSetTCLRefDataCmd::DoNotify()
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
			subject->Change(kPrsSetTCLRefDataCmdBoss, IID_ITCLREFDATA, this);

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
PMString * PrsSetTCLRefDataCmd::CreateName()
{
	PMString * name = new PMString("Set TCL Reference");
	name->SetTranslatable(kFalse);
	return name;
}
