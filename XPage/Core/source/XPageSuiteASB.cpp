/*
//	File:	XPageSuiteASB.cpp
//
//	Author:	Trias Developpement
//
//	Date:	23/07/2006
//
*/

#include "VCPlugInHeaders.h"								

// Interface headers
#include "IXPageSuite.h"				

// API includes:
#include "SelectionASBTemplates.tpp"


class XPageSuiteASB : public CPMUnknown<IXPageSuite>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	XPageSuiteASB(IPMUnknown *iBoss);
	
	/** Destructor. */
	virtual ~XPageSuiteASB(void);

	/**
		See IXPageSuite::GetTextFrameStoryRef
	*/
	virtual void GetTextFrameStoryRef(UIDRef& txtFrameStoryRef);
	
	/**
		See IXPageSuite::GetTextFrameRef
	*/
	virtual void GetTextFrameRef(UIDRef& txtFrameRef);

	/**
		See IXPageSuite::GetSelectedItems
	*/
	virtual void GetSelectedItems(UIDList& textFrames, UIDList& graphicFrames, UIDList& selectedItems);

	/** 
		See IXPageSuite::GetSelectedForme
	*/
	virtual void GetSelectedForme(PMString& idForme, UIDList& textFrames, UIDList& graphicFrames, UIDList& formeItems);
};

/* CREATE_PMINTERFACE
 	Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPageSuiteASB, kXPGSuiteASBImpl)

/* XPageSuiteASB Constructor
*/
XPageSuiteASB::XPageSuiteASB(IPMUnknown* iBoss) :
	CPMUnknown<IXPageSuite>(iBoss)
{
}

/* XPageSuiteASB Destructor
*/
XPageSuiteASB::~XPageSuiteASB(void)
{
}


/* GetTextFrameStoryRef
*/
void XPageSuiteASB::GetTextFrameStoryRef(UIDRef& txtFrameStoryRef)
{
	CallEach(make_functor(&IXPageSuite::GetTextFrameStoryRef, txtFrameStoryRef), this);
}

/* GetTextFrameRef
*/
void XPageSuiteASB::GetTextFrameRef(UIDRef& txtFrameRef)
{
	CallEach(make_functor(&IXPageSuite::GetTextFrameRef, txtFrameRef), this);
}

/* GetSelectedItems
*/
void XPageSuiteASB::GetSelectedItems(UIDList& textFrames, UIDList& graphicFrames, UIDList& selectedItems)
{
	CallEach(make_functor(&IXPageSuite::GetSelectedItems, textFrames, graphicFrames, selectedItems), this);
}

/* GetSelectedForme
*/
void XPageSuiteASB::GetSelectedForme(PMString& idForme, UIDList& textFrames, UIDList& graphicFrames, UIDList& formeItems)
{
	CallEach(make_functor(&IXPageSuite::GetSelectedForme, idForme, textFrames, graphicFrames, formeItems), this);
}
