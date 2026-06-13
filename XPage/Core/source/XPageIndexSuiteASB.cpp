/*
//	File:	XPageIndexSuiteASB.cpp
//
//	Author:	Trias Developpement
//
//	Date:	23/07/2006
//
*/

#include "VCPlugInHeaders.h"								

// Interface headers
#include "IXPageIndexSuite.h"				

// API includes:
#include "SelectionASBTemplates.tpp"


class XPageIndexSuiteASB : public CPMUnknown<IXPageIndexSuite>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	XPageIndexSuiteASB(IPMUnknown *iBoss);
	
	/** Destructor. */
	virtual ~XPageIndexSuiteASB(void);

	/**
		See IXPageIndexSuite::GetSelectedText
	*/
	virtual void GetSelectedText(WideString& txt, UIDRef& txtModel, bool16& multiLineSelection, TextIndex& start, TextIndex& end);
};

/* CREATE_PMINTERFACE
 	Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPageIndexSuiteASB, kXPGIndexSuiteASBImpl)

/* XPageIndexSuiteASB Constructor
*/
XPageIndexSuiteASB::XPageIndexSuiteASB(IPMUnknown* iBoss) :
	CPMUnknown<IXPageIndexSuite>(iBoss)
{
}

/* XPageIndexSuiteASB Destructor
*/
XPageIndexSuiteASB::~XPageIndexSuiteASB(void)
{
}


/* GetSelectedText
*/
void XPageIndexSuiteASB::GetSelectedText(WideString& txt, UIDRef& txtModel, bool16& multiLineSelection, TextIndex& start, TextIndex& end)
{
	CallEach(make_functor(&IXPageIndexSuite::GetSelectedText, txt, txtModel, multiLineSelection, start, end), this);
}
