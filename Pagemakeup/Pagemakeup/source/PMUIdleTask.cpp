/*
//	File:	PMUIdleTask.cpp
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


#include "VCPlugInHeaders.h"

// Impl includes
#include "CIdleTask.h"


// Interface includes
#include "IControlView.h"
#include "IPanelControlData.h"
#include "ISubject.h"
#include "ITriStateControlData.h"
#include "IIdleTaskMgr.h"

// General includes
#include "CObserver.h"
#include "CAlert.h"
#include "StreamUtil.h"

// Project includes
#include "PMUID.h"
#include "PMUUtils.h"

// Parser includes
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "PrsID.h"

/** PMUIdleTask
	Implements IIdleTask, to refresh the progress far on PageMakeUp panel.
*/
class PMUIdleTask : public CIdleTask
{
	public:

		/**	Constructor
			@param boss boss object on which this interface is aggregated
		*/
		PMUIdleTask(IPMUnknown* boss);

		/**	Destructor
		*/
		virtual ~PMUIdleTask() {}


		/**	Called by the idle task manager in the app core when the task is running
			
			@param appFlags specifies set of flags which task can check to see if it should do something
				this time round
			@param timeCheck 
			@return uint32 giving the interval (msec) that should elapse before calling this back
		 */
		virtual uint32 RunTask( uint32 appFlags, IdleTimer *timeCheck);

		/**	Get name of task
			@return const char* name of task
		 */
		virtual const char* TaskName();

	protected:

		/**	Update the progress bar
		*/
		void refresh();

};


CREATE_PMINTERFACE(PMUIdleTask, kPMUIdleTaskImpl)

/* Constructor
*/
PMUIdleTask::PMUIdleTask(IPMUnknown *boss)
	:CIdleTask(boss)
{

}



/* RunTask
*/
uint32 PMUIdleTask::RunTask( uint32 appFlags, IdleTimer *timeCheck)
{
	
	
	this->refresh();

	return 50;
}

/* TaskName
*/
const char* PMUIdleTask::TaskName()
{
	PMString name("Refresh progress bar");
	name.SetTranslated();
	return name.GrabCString();
}

/* refresh
*/
void PMUIdleTask::refresh()
{
	do 
	{
		InterfacePtr<IControlView> ctrlView (this,UseDefaultIID());
		if(ctrlView == nil)
		{
			ASSERT_FAIL("PMUProgressBarObserver::Update -> ctrlView nil");
			break;
		}

		ctrlView->Invalidate();

	} while(false);
}


