/*
//	File:	PrsDrawHandler.cpp
//
//  Author: Wilfried LEFEVRE
//
//	Date:	20/07/2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2005 Trias Developpement. All rights reserved.
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IDrwEvtHandler.h"
#include "IDrwEvtDispatcher.h"
#include "IDocument.h"
#include "IShape.h"

// Implementation Includes
#include "CPMUnknown.h"
#include "ILayoutUIUtils.h"
#include "DocumentContextID.h"
#include "IPrintable.h"

// Project includes:
#include "PrsID.h"

#include "CAlert.h"


/** Implements the code necessary to process draw events.
	Register()/UnRegister() allows install/uninstall into 
	the draw event dispatcher. HandleEvent() is called when 
	the event registered for happens.
*/
class PrsDrawHandler : public CPMUnknown<IDrwEvtHandler>
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on 
			which this interface is aggregated.
		*/
		PrsDrawHandler(IPMUnknown* boss);

		/**
			Destructor.
		*/
		virtual ~PrsDrawHandler() {}
		
		/**
		Called so handler can register for particular draw events 
		using the RegisterHandler() method in IDrwEvtDispatcher.
		@param eventDispatcher (in) specifies active draw event dispatcher.
		*/
		virtual void Register(IDrwEvtDispatcher* eventDispatcher);
		
		/**
		Called so handler can unregister for particular draw events 
		using the UnRegisterHandler() method in IDrwEvtDispatcher.
		Not actually necessary since all handlers get unregistered 
		automatically at quit time, but it is good form.
		@param eventDispatcher (in) specifies active draw event dispatcher.
		*/
		virtual void UnRegister(IDrwEvtDispatcher* eventDispatcher);
		
		/**
		If event handled here, return kTrue and the item should 
		not be drawn.  Else event not handled here, return kFalse 
		and continue drawing item.
		@param eventID (in) specifies ID of the specific event.
		@param eventData (in) specifies pointer to the draw event data.
		In this case, we are being passed a UIDRef of the spread
		to be printed.
		*/
		virtual bool16 HandleEvent(ClassID eventID, void* eventData);
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PrsDrawHandler, kPrsDrawHandlerImpl)


/* Constructor
*/
PrsDrawHandler::PrsDrawHandler(IPMUnknown* boss) 
	: CPMUnknown<IDrwEvtHandler>(boss)
{
}


/* Register
*/
void PrsDrawHandler::Register(IDrwEvtDispatcher* eventDispatcher)
{
	eventDispatcher->RegisterHandler(
		kDrawShapeMessage, this, kDEHLowestPriority);
}


/* UnRegister
*/
void PrsDrawHandler::UnRegister(IDrwEvtDispatcher* eventDispatcher)
{
	eventDispatcher->UnRegisterHandler(
		kDrawShapeMessage, this);
}


/* HandleEvent
*/
bool16 PrsDrawHandler::HandleEvent(
	ClassID eventID, void* eventData)
{
	bool16 bHandled = kFalse;
	
	do
	{
		// Make sure this is a kDrawShapeMessage.
		if (eventID.Get() != kDrawShapeMessage)
		{
			break;
		}

		DrawEventData* ed = static_cast<DrawEventData*>(eventData);
		ASSERT(ed);

		// Make sure we are printing.
		if ((ed->flags & IShape::kPrinting) == 0)
		{
			break;
		}
		
		/*
		UID uid2Test = ::GetUID(ed->changedBy);
		PMString debug;
		debug.AppendNumber(uid2Test.Get());
		debug.SetTranslatable(kFalse);
		CAlert::InformationAlert(debug);
		*/
		
		
		UIDRef uid2Test =  ::GetUIDRef(ed->changedBy);
		
		InterfacePtr<IPrintable> printProperty (uid2Test, UseDefaultIID());
		if(printProperty == nil) // not a spline item, do nothing 
		{
			break;
		}
		
		bHandled = !(printProperty->isPrintable());
		
		
	} while(0);

	return bHandled;
}