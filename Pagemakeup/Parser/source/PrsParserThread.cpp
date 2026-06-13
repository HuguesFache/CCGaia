/*
//	File:	PrsParserThread.cpp
//
//  Author: Trias
//
//	Date:	25-Jun-2006
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2006 Trias Developpement. All rights reserved.
//
*/

// Plug-in includes
#include "VCPlugInHeaders.h"
#include "GlobalDefs.h"

// Interfaces includes
#include "IHierarchy.h"
#include "ICommandMgr.h"
#include "ILayoutUIUtils.h"
#include "IDocument.h"
#include "ISubject.h"
#include "IIntData.h"
#include "IBoolData.h"

//#include "CIdleTaskThread.h"
#include "CAlert.h"
#include "FileUtils.h"
#include "ErrorUtils.h"

// Project includes
#include "ITCLParser.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "Blocs.h"
#include "Docs.h"
#include "Divers.h"
#include "Styles.h"
#include "Textes.h"
#include "Paragraphes.h"
#include "Reglages.h"
#include "Pages.h"
#include "Calques.h"
#include "Systeme.h"
#include "Tableaux.h"
#include "Livres.h"
#include "PermRefs.h"
#include "XRail.h"
#include "Log.h"
#include "TCLError.h"
#include "CIdleTaskThread.h"

#include "PrsID.h"

class PrsParserThread : public CIdleTaskThread
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which theParser interface is aggregated.
	*/
	PrsParserThread(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsParserThread();

	void RunThread(uint32 flags);

	const char* TaskName();
	
};


CREATE_PMINTERFACE(PrsParserThread, kPrsParserThreadImpl)

/* Constructor
*/
PrsParserThread::PrsParserThread(IPMUnknown* boss) 
: CIdleTaskThread(boss)
{
}

/* Destructor
*/
PrsParserThread::~PrsParserThread()
{
}

const char * PrsParserThread::TaskName()
{
	return kPrsThreadKey;
}

void PrsParserThread::RunThread(uint32 flags)
{
	InterfacePtr<ITCLParser> theParser (this, UseDefaultIID());

	int32 numError = 0;

	/** TEMPORARY VERSION 4.2.1 */
	InterfacePtr<IBoolData> parserThreadRunning (GetExecutionContextSession(), IID_ITCLPARSER);
	parserThreadRunning->Set(kTrue);
	/** TEMPORARY VERSION 4.2.1 */

	do
	{
		if(theParser->ProcessTCL(numError)) break;

		this->YieldToEventLoop(0);

	} while (kTrue);

	theParser->FinalizeProcess();

	// Set result's error code
	InterfacePtr<IIntData> result (this, UseDefaultIID());
	result->Set(numError);

	/** TEMPORARY VERSION 4.2.1 */
	parserThreadRunning->Set(kFalse);
	/** TEMPORARY VERSION 4.2.1 */

	// Notify observers that thread has ended
	InterfacePtr<ISubject> parserSubject (this, UseDefaultIID());
	parserSubject->Change(kParserThreadFinishMessage);	
}