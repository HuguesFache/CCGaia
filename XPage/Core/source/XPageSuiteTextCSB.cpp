/*
//	File:	XPageSuiteTextCSB.cpp
//
//	Author:	Trias Developpement
//
//	Date:	23/07/2006
//
*/

#include "VCPlugInHeaders.h"								

// Interface headers
#include "IXPageSuite.h"	
#include "ITextTarget.h"
#include "IFrameList.h"
#include "IHierarchy.h" 
#include "IGraphicFrameData.h"
#include "ITextModel.h"
#include "IMultiColumnTextFrame.h"
#include "ITextUtils.h"
#include "ITextFrameColumn.h"
#include "IFormeData.h"
#include "IXPageUtils.h"

// Project includes

// General includes:
#include "SelectionExtTemplates.tpp"
#include "UIDList.h"

class XPageSuiteTextCSB : public CPMUnknown<IXPageSuite>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	XPageSuiteTextCSB(IPMUnknown *iBoss);
	
	/** Destructor. */
	virtual ~XPageSuiteTextCSB(void);

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
CREATE_PMINTERFACE(XPageSuiteTextCSB, kXPGTextCSBImpl)

/* XPageSuiteTextCSB Constructor
*/
XPageSuiteTextCSB::XPageSuiteTextCSB(IPMUnknown* iBoss) :
	CPMUnknown<IXPageSuite>(iBoss)
{
}

/* XPageSuiteTextCSB Destructor
*/
XPageSuiteTextCSB::~XPageSuiteTextCSB(void)
{
}


/* GetTextFrameStoryRef
*/
void XPageSuiteTextCSB::GetTextFrameStoryRef(UIDRef& txtFrameStoryRef)
{
	InterfacePtr<ITextTarget> txtTarget (this, UseDefaultIID());
	
	txtFrameStoryRef = txtTarget->GetTextModel();
}

/* GetTextFrameRef
*/
void XPageSuiteTextCSB::GetTextFrameRef(UIDRef& txtFrameRef)
{
	InterfacePtr<ITextTarget> txtTarget (this, UseDefaultIID());
	
	InterfacePtr<ITextModel> txtModel (txtTarget->GetTextModel(), UseDefaultIID());
	InterfacePtr<IFrameList> frameList (txtModel->QueryFrameList());

	IDataBase * db = ::GetDataBase(frameList);
	InterfacePtr<IHierarchy> txtFrame (db, frameList->GetNthFrameUID(0), UseDefaultIID());
	
	UIDList ancestor(db);
	txtFrame->GetAncestors(&ancestor, IID_IGRAPHICFRAMEDATA);

	if(ancestor.Length() == 1)
		txtFrameRef = UIDRef(db, ancestor[0]);
}

/** GetSelectedItems
*/
void XPageSuiteTextCSB::GetSelectedItems(UIDList& textFrames, UIDList& graphicFrames, UIDList& selectedItems)
{
	textFrames.Clear();
	graphicFrames.Clear();
	selectedItems.Clear();

	InterfacePtr<ITextTarget> txtTarget (this, UseDefaultIID());
	InterfacePtr<ITextModel> txtModel (txtTarget->GetTextModel(), UseDefaultIID());
	InterfacePtr<IFrameList> frameList (txtModel->QueryFrameList());
	textFrames = UIDList(::GetDataBase(frameList));
	// FIX Bug Version 7.5.6 : ajout de la ligne ci-dessous
	graphicFrames = UIDList(::GetDataBase(frameList));
	int32 frameCount = frameList->GetFrameCount();
	UIDRef lastFrame = UIDRef::gNull;
	
	for(int32 i = 0 ; i < frameCount ; ++i){
		InterfacePtr<ITextFrameColumn> txtFrame (frameList->QueryNthFrame(i));
		InterfacePtr<IHierarchy> frameHier (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
		UIDRef frameRef = ::GetUIDRef(frameHier);
		if(frameRef != lastFrame){
			lastFrame = frameRef;
			textFrames.Append(frameRef.GetUID());
		}
	}
	
	selectedItems = textFrames;
}

/** GetSelectedForme
*/
void XPageSuiteTextCSB::GetSelectedForme(PMString& idForme, UIDList& textFrames, UIDList& graphicFrames, UIDList& formeItems)
{
	InterfacePtr<ITextTarget> txtTarget (this, UseDefaultIID());
	InterfacePtr<ITextModel> txtModel (txtTarget->GetTextModel(), UseDefaultIID());
	InterfacePtr<IFrameList> frameList (txtModel->QueryFrameList());

	if(frameList->GetFrameCount() > 0)
	{
		InterfacePtr<ITextFrameColumn> txtFrame (frameList->QueryNthFrame(0));
		InterfacePtr<IHierarchy> frameHier (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
		
		InterfacePtr<IFormeData> formeData (frameHier, UseDefaultIID());
		idForme = formeData->GetUniqueName();
		if(idForme != kNullString)
		{
			// Selected model belongs to a forme, now get all items of this forme and categorize them
			InterfacePtr<ISpread> owningSpread (::GetDataBase(frameHier), frameHier->GetSpreadUID(), UseDefaultIID());

			Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(idForme, owningSpread, formeItems);

			Utils<IXPageUtils>()->CategorizeItems(formeItems, textFrames, graphicFrames);
		}
	}
}
