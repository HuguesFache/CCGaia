/*
//	File:	PermRefsSuiteASB.cpp
//
//	Author:	Trias
//
//	Date:	27/10/2005
//
*/

#include "VCPlugInHeaders.h"								

// Interface headers
#include "IPermRefsSuite.h"				

// API includes:
#include "SelectionASBTemplates.tpp"


class PermRefsSuiteASB : public CPMUnknown<IPermRefsSuite>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	PermRefsSuiteASB(IPMUnknown *iBoss);
	
	/** Destructor. */
	virtual ~PermRefsSuiteASB(void);

	/**
		See IPermRefsSuite::GetTextSelection
	*/
	virtual void GetTextSelection(UIDRef& txtModel, TextIndex& start, TextIndex& end);
};

/* CREATE_PMINTERFACE
 	Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PermRefsSuiteASB, kPermRefsSuiteASBImpl)

/* PermRefsSuiteASB Constructor
*/
PermRefsSuiteASB::PermRefsSuiteASB(IPMUnknown* iBoss) :
	CPMUnknown<IPermRefsSuite>(iBoss)
{
}

/* PermRefsSuiteASB Destructor
*/
PermRefsSuiteASB::~PermRefsSuiteASB(void)
{
}


/* GetTextSelection
*/
void PermRefsSuiteASB::GetTextSelection(UIDRef& txtModel, TextIndex& start, TextIndex& end)
{
	CallEach(make_functor(&IPermRefsSuite::GetTextSelection, txtModel, start, end), this);
}