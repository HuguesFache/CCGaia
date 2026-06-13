/*
//	File:	ParserSuiteASB.h
//
//	Author:	Trias
//
//	Date:	19/07/2005
//
*/

#include "VCPlugInHeaders.h"								

// Interface headers
#include "IParserSuite.h"				

// API includes:
#include "SelectionASBTemplates.tpp"

/** 
	Abstract selection boss (ASB) IBscMnuSuite implementation.  The purpose of this integrator suite is
	to determine how to forward the client request on to the CSB suite(s).  Note that the client does not
	interact with the CSB (which has all the real implementation of the suite) directly, the client interacts 
	with the ASB only.  Also note that the Suite API shouldn't contain model data that is specific to a 
	selection format (layout uidLists, text model/range, etc) so that the client code can be completely 
	decoupled from the underlying CSB.

	@ingroup basicmenu
	
*/
class ParserSuiteASB : public CPMUnknown<IParserSuite>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	ParserSuiteASB(IPMUnknown *iBoss);
	
	/** Destructor. */
	virtual ~ParserSuiteASB(void);

	/**
		See IParserSuite::GetSelectedItems
	*/
	virtual void GetSelectedItems(UIDList& items);
};

/* CREATE_PMINTERFACE
 	Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(ParserSuiteASB, kPrsSuiteASBImpl)

/* BscMnuSuiteASB Constructor
*/
ParserSuiteASB::ParserSuiteASB(IPMUnknown* iBoss) :
	CPMUnknown<IParserSuite>(iBoss)
{
}

/* BscMnuSuiteASB Destructor
*/
ParserSuiteASB::~ParserSuiteASB(void)
{
}


/* GetSelectedItems
*/
void ParserSuiteASB::GetSelectedItems(UIDList& items)
{
	CallEach(make_functor(&IParserSuite::GetSelectedItems, items), this);
}