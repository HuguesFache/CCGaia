/*
//	File:	PermRefsSuiteTextCSB.cpp
//
//	Author:	Trias
//
//	Date:	27/10/2005
//
*/

#include "VCPlugInHeaders.h"								

// Interface headers
#include "IPermRefsSuite.h"	
#include "ITextTarget.h"
#include "ISelectionUtils.h"
#include "ITextSelectionSuite.h"
#include "ITextFocus.h"
#include "IItemLockData.h"
#include "IBoolData.h"
#include "IPermRefsTag.h"
#include "IAttributeStrand.h"

// General includes:
#include "TextAttributeRunIterator.h"
#include "SelectionExtTemplates.tpp"

#include "RangeData.h"
#include "CAlert.h"

class PermRefsSuiteTextCSB : public CPMUnknown<IPermRefsSuite>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	PermRefsSuiteTextCSB(IPMUnknown *iBoss);
	
	/** Destructor. */
	virtual ~PermRefsSuiteTextCSB(void);

	/**
		See IPermRefsSuite::GetTextSelection
	*/
	virtual void GetTextSelection(UIDRef& txtModel, TextIndex& start, TextIndex& end);

};

/* CREATE_PMINTERFACE
 	Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PermRefsSuiteTextCSB, kPermRefsSuiteTextCSBImpl)

/* PermRefsSuiteTextCSB Constructor
*/
PermRefsSuiteTextCSB::PermRefsSuiteTextCSB(IPMUnknown* iBoss) :
	CPMUnknown<IPermRefsSuite>(iBoss)
{
}

/* PermRefsSuiteTextCSB Destructor
*/
PermRefsSuiteTextCSB::~PermRefsSuiteTextCSB(void)
{
}


/* GetTextSelection
*/
void PermRefsSuiteTextCSB::GetTextSelection(UIDRef& txtModel, TextIndex& start, TextIndex& end)
{
	InterfacePtr<ITextTarget> txtTarget (this, UseDefaultIID());
	
	txtModel = txtTarget->GetTextModel();
	
	RangeData range = txtTarget->GetRange();

	start = range.Start(nil);
	end = range.End();
}
