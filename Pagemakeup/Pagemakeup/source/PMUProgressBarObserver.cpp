/*
//	File:	PMUProgressBarObserver.cpp
//
//	Date:	14-Apr-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//	
//	NOTICE: Adobe permits you to use, modify, and distribute this file in
//	accordance with the terms of the Adobe license agreement accompanying it.
//	If you have received this file from a source other than Adobe, then your
//	use, modification, or distribution of it requires the prior written
//	permission of Adobe.
//
*/

#include "VCPluginHeaders.h"

// Interface includes
#include "IControlView.h"
#include "ISubject.h"
#include "IProgressBarControlData.h"
#include "IApplication.h"
#include "IProgressBarManager.h"

// General includes
#include "CObserver.h"
#include "CAlert.h"

// Project includes
#include "PMUID.h"

// Parser includes
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "PrsID.h"


/** PMUProgressBarObserver
	Lauch different functions when icons are clicked.
*/
class PMUProgressBarObserver : public CObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PMUProgressBarObserver( IPMUnknown* boss );
	/** Destructor
	*/
	virtual ~PMUProgressBarObserver() ;

	/**
		Update is called for all registered observers, and is
		the method through which changes are broadcast. 
		@param theChange this is specified by the agent of change; it can be the class ID of the agent,
		or it may be some specialised message ID.
		@param theSubject this provides a reference to the object which has changed; in this case, the
.		TCL reader that is being observed
		@param protocol the protocol along which the change occurred.
		@param changedBy this can be used to provide additional information about the change or a reference
		to the boss object that caused the change.
	*/
	void Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy);

private :

	int32 bytesRead;
	int32 totalBytes;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(PMUProgressBarObserver, kPMUProgressBarObserverImpl)

/* Constructor
*/
PMUProgressBarObserver::PMUProgressBarObserver( IPMUnknown* boss ) :
	CObserver( boss ),
	bytesRead(0),
	totalBytes(0)
{
}

/* Destructor
*/
PMUProgressBarObserver::~PMUProgressBarObserver()
{
}


/* Update
*/
void PMUProgressBarObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{
	do
	{
	/*	InterfacePtr<ITCLReader> reader (theSubject,UseDefaultIID());
		if (reader == nil)
		{
			ASSERT_FAIL("PMUProgressBarObserver::Update -> reader nil");
			break;
		}

		if (theChange == kNewStreamMessage)
		{
			totalBytes = reader->GetFileLengthInBytes();
			// Reset the progress bar;
			InterfacePtr<IProgressBarControlData> pgBarData (this,IID_IPROGRESSBARCONTROLDATA);
			if (pgBarData == nil)
			{
				ASSERT_FAIL("PMUProgressBarObserver::Update -> pgBarData nil");
				break;
			}
			pgBarData->SetRange(0, totalBytes);
			pgBarData->SetValue(0);
		}
		else 
			if(theChange == kBytesReadMessage)
			{
				bytesRead = reader->GetNbBytesRead();
				// Increment the progress bar to match the current number of bytes read
				InterfacePtr<IProgressBarControlData> pgBarData (this,IID_IPROGRESSBARCONTROLDATA);
				if (pgBarData == nil)
				{	
					ASSERT_FAIL("PMUProgressBarObserver::Update -> pgBarData nil");
					break;
				}
				pgBarData->SetValue(bytesRead);
			}
	*/
	} while(false);
}


// End, PMUProgressBarObserver.cpp.


