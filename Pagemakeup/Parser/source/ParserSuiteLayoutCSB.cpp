/*
//	File:	ParserSuiteLayoutCSB.h
//
//	Author:	Trias
//
//	Date:	19/07/2005
//
*/

#include "VCPlugInHeaders.h"								// For MSVC

// Interface headers
#include "ILayoutTarget.h"
#include "IParserSuite.h"										// Superclass declaration


class ParserSuiteLayoutCSB : public CPMUnknown<IParserSuite>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	ParserSuiteLayoutCSB(IPMUnknown* iBoss);
	
	/** Destructor. */
	virtual	~ParserSuiteLayoutCSB(void);

//	Suite Member functions
public:
	
	virtual void GetSelectedItems(UIDList& items);
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(ParserSuiteLayoutCSB, kPrsSuiteLayoutCSBImpl)

/* Constructor
*/
ParserSuiteLayoutCSB::ParserSuiteLayoutCSB(IPMUnknown* iBoss) :
	CPMUnknown<IParserSuite>(iBoss)
{
}

/* Destructor
*/
ParserSuiteLayoutCSB::~ParserSuiteLayoutCSB(void)
{
}

/* GetSelectedItems
*/
void ParserSuiteLayoutCSB::GetSelectedItems(UIDList& items)
{
	InterfacePtr<ILayoutTarget>		iLayoutTarget(this, UseDefaultIID());
	items = (iLayoutTarget->GetUIDList(kStripStandoffs));
}
