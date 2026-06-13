/*
//	File:	XPGImportSnippetObserver.cpp
//
//	Date:	23-Nov-2006
//
*/

#include "VCPluginHeaders.h"

// Interface includes
#include "ISubject.h"
#include "IClassIDData.h"
#include "IPMUnknownData.h"
#include "IUIDListData.h"
#include "IHierarchy.h"
#include "IGraphicFrameData.h"

// General includes
#include "CObserver.h"
#include "GenericID.h"
#include "OpenPlaceID.h"
#include "PageItemScrapID.h"
#include "FileUtils.h"
#include "UIDList.h"

// Project includes
#include "XPGID.h"


#include "DebugClassUtils.h"
#include "CAlert.h"

/** XPGImportSnippetObserver
	Observes import snippet actions in order to register items

	@author Trias Developpement
*/
class XPGImportSnippetObserver : public CObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGImportSnippetObserver( IPMUnknown* boss );
	/** Destructor
	*/
	virtual ~XPGImportSnippetObserver() {}

	/** 
		Attaches this observer to the document it's aggregated onto
	*/
	void AutoAttach();

	/** 
		Detaches this observer to the document it's aggregated onto
	*/
	void AutoDetach();

	/**
		Update is called for all registered observers, and is
		the method through which changes are broadcast. 
		@param theChange this is specified by the agent of change; it can be the class ID of the agent,
		or it may be some specialised message ID.
		@param theSubject this provides a reference to the object which has changed; in this case, the
		widget boss objects that are being observed.
		@param protocol the protocol along which the change occurred.
		@param changedBy this can be used to provide additional information about the change or a reference
		to the boss object that caused the change.
	*/
	void Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy);

private:

	IDataBase * db; // Database of the document this observer is aggregated onto

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGImportSnippetObserver, kXPGImportSnippetObserverImpl)

/* Constructor
*/
XPGImportSnippetObserver::XPGImportSnippetObserver( IPMUnknown* boss ) :
CObserver( boss )
{
	db = ::GetDataBase(this);
}

/* AutoAttach
*/
void XPGImportSnippetObserver::AutoAttach()
{
	CObserver::AutoAttach();
}

/* AutoDetach
*/
void XPGImportSnippetObserver::AutoDetach()
{
	CObserver::AutoDetach();
}

/* Update
*/
void XPGImportSnippetObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{
	if(theChange == kAddToHierarchyCmdBoss) // called each time an item is created from a snippet (manually or automatically via command IE)
	{
		const UIDList * itemList = (static_cast<ICommand *>(changedBy))->GetItemList();
		if(itemList)
		{
			InterfacePtr<IUIDListData> importedItems (this, IID_ILASTIMPORTEDSNIPPET);
			UIDList * newList = new UIDList(db);

			if(importedItems->GetRef())
				newList->Append(*(importedItems->GetRef()));

			for(int32 i = 0 ; i < itemList->Length() ; ++i)
			{
				InterfacePtr<IGraphicFrameData> graphicFrameData (itemList->GetRef(i), UseDefaultIID());
				if(graphicFrameData) // We only cares about items which aggregate IID_IGRAPHICFRAMEDATA (that is to say kSplineItemBoss);
					newList->Append(itemList->At(i));
			}

			importedItems->Set(newList);
		}
	}
}
