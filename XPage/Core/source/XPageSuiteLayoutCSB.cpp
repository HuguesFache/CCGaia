/*
//	File:	XPageSuiteLayoutCSB.cpp
//
//	Author:	Trias Developpement
//
//	Date:	23/07/2006
//
*/

#include "VCPlugInHeaders.h"								

// Interface headers
#include "IXPageSuite.h"	
#include "ILayoutTarget.h"
#include "IMultiColumnTextFrame.h"
#include "ITextModel.h"
#include "IHierarchy.h"
#include "IFormeData.h"
#include "IPageItemTypeUtils.h"
#include "IXPageUtils.h"

// Project includes

// General includes:
#include "SelectionExtTemplates.tpp"

class XPageSuiteLayoutCSB : public CPMUnknown<IXPageSuite>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	XPageSuiteLayoutCSB(IPMUnknown *iBoss);
	
	/** Destructor. */
	virtual ~XPageSuiteLayoutCSB(void);

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
CREATE_PMINTERFACE(XPageSuiteLayoutCSB, kXPGLayoutCSBImpl)

/* XPageSuiteLayoutCSB Constructor
*/
XPageSuiteLayoutCSB::XPageSuiteLayoutCSB(IPMUnknown* iBoss) :
	CPMUnknown<IXPageSuite>(iBoss)
{
}

/* XPageSuiteLayoutCSB Destructor
*/
XPageSuiteLayoutCSB::~XPageSuiteLayoutCSB(void)
{
}


/* GetTextFrameStoryRef
*/
void XPageSuiteLayoutCSB::GetTextFrameStoryRef(UIDRef& txtFrameStoryRef)
{
	InterfacePtr<ILayoutTarget> target (this, UseDefaultIID());
	
	UIDList targetFrame = target->GetUIDList(kStripStandoffs);

	if(targetFrame.Length() == 1)
	{	
		InterfacePtr<IHierarchy> targetHier (targetFrame.GetRef(0), UseDefaultIID());
		if(targetHier && targetHier->GetChildCount() == 1)
		{
			InterfacePtr<IMultiColumnTextFrame> isTextFrame (targetFrame.GetDataBase(), targetHier->GetChildUID(0), UseDefaultIID());
			if(isTextFrame)
			{
				txtFrameStoryRef = UIDRef(targetFrame.GetDataBase(), isTextFrame->GetTextModelUID());
			}			
		}
	}
}

/* GetTextFrameRef
*/
void XPageSuiteLayoutCSB::GetTextFrameRef(UIDRef& txtFrameRef)
{
	InterfacePtr<ILayoutTarget> target (this, UseDefaultIID());
	
	UIDList targetFrame = target->GetUIDList(kStripStandoffs);

	if(targetFrame.Length() == 1)
	{	
		InterfacePtr<IHierarchy> targetHier (targetFrame.GetRef(0), UseDefaultIID());
		if(targetHier && targetHier->GetChildCount() == 1){
			InterfacePtr<IMultiColumnTextFrame> isTextFrame (targetFrame.GetDataBase(), targetHier->GetChildUID(0), UseDefaultIID());
			if(isTextFrame)
				txtFrameRef = ::GetUIDRef(targetHier);
		}
	}
}

/** GetSelectedItems
*/
void XPageSuiteLayoutCSB::GetSelectedItems(UIDList& textFrames, UIDList& graphicFrames, UIDList& selectedItems)
{
	InterfacePtr<ILayoutTarget> target (this, UseDefaultIID());
	selectedItems = target->GetUIDList(kStripStandoffs);

	Utils<IXPageUtils>()->CategorizeItems(selectedItems, textFrames, graphicFrames);
}

/** GetSelectedForme
*/
void XPageSuiteLayoutCSB::GetSelectedForme(PMString& idForme, UIDList& textFrames, UIDList& graphicFrames, UIDList& formeItems)
{
	InterfacePtr<ILayoutTarget> target (this, UseDefaultIID());
	UIDList selectedItems = target->GetUIDList(kStripStandoffs);

	// Inspect items looking for forme data
	idForme = kNullString;
	for(int32 i = 0 ; i < selectedItems.Length() ; ++i) 
	{
		UIDRef anItem = selectedItems.GetRef(i);
		if(Utils<IPageItemTypeUtils>()->IsGroup(anItem))
		{
			InterfacePtr<IHierarchy> itemHier (anItem, UseDefaultIID());
			for(int32 j = 0 ; j < itemHier->GetChildCount() ; ++j)
				selectedItems.Append(itemHier->GetChildUID(j));
		}
		else{
			
			InterfacePtr<IFormeData> formeData (anItem, UseDefaultIID());
			if(!formeData) // Strip items out of a forme
				continue;	

			PMString anIDForme = formeData->GetUniqueName();
			if(anIDForme != kNullString)
			{
				if(idForme == kNullString)
					idForme = anIDForme;
				else if(idForme != anIDForme)
				{
					idForme = kNullString;
					break;
				}
			}
		}	
	}

	if(!selectedItems.IsEmpty() && idForme != kNullString)
	{
		// Only items of one forme were detected, now get all items of this forme and categorize them
		IDataBase* db = selectedItems.GetDataBase();
		InterfacePtr<IHierarchy> firstItemHier (db, selectedItems[0], UseDefaultIID());
		InterfacePtr<ISpread> owningSpread (db, firstItemHier->GetSpreadUID(), UseDefaultIID());

		Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(idForme, owningSpread, formeItems);

		Utils<IXPageUtils>()->CategorizeItems(formeItems, textFrames, graphicFrames);
	}
}
