/*
//	File:	XRLUIIntList.cpp
//
//	Date:	01/06/2007
//
//  Author: Trias
//
//	Copyright Trias Developpement. All rights reserved.
//	
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IIntList.h"

// General includes:
#include "K2Vector.h"
#include "CAlert.h"

// Project includes:
#include "XRLUIID.h"

/** XRLUIIntList
*/
class XRLUIIntList : public CPMUnknown<IIntList>
{
	public:
		/** Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		XRLUIIntList(IPMUnknown* boss);

		/** Destructor.
		*/
		virtual ~XRLUIIntList();

		virtual void  SetIntList (const K2Vector<int32> &newList);

		virtual void  Clear ();

		virtual const K2Vector<int32> &  GetIntList ();

	private :

		K2Vector<int32> intList;

};

CREATE_PMINTERFACE(XRLUIIntList, kXRLUIIntListImpl)

XRLUIIntList::XRLUIIntList(IPMUnknown* boss) : CPMUnknown<IIntList>(boss) 
{
}

XRLUIIntList::~XRLUIIntList()
{
	
}

void  XRLUIIntList::SetIntList (const K2Vector< int32 > &newList)
{
	intList = newList;
}

void  XRLUIIntList::Clear ()
{
	K2Vector<int32> emptyList;
	intList = emptyList;
}

const K2Vector< int32 > &  XRLUIIntList::GetIntList ()
{
	return intList;
}
