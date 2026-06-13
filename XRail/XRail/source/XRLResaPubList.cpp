/*
//
//	File: XRLResaPubList.cpp
//
//  Author: Trias
//
//	Date:	3-Mar-2008
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2008 Trias Developpement. All rights reserved.
//
*/

#include "VCPlugInHeaders.h"

// API includes
#include "CPMUnknown.h"

// Project includes:
#include "IResaPubList.h"
#include "IPMStream.h"

#include "CAlert.h"


/** XRLResaPubList
	Persistent implementation of ITCLReferencesList, added to the kDocBoss
*/

class XRLResaPubList : public CPMUnknown<IResaPubList>
{
public:
	/**
		Constructor.

		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRLResaPubList(IPMUnknown* boss);


	/** Destructor
	*/
	~XRLResaPubList();

	/**
		Accessor methods for ResaPub list
	*/	
	virtual void SetResaPubList(const K2Vector<ResaPub>& newList);
	virtual K2Vector<ResaPub> GetResaPubList();	


	/**
		Because this is a persistent interface, it must support the ReadWrite method. This method is used for 
		both writing information out to the database and reading information in from the database.

		@param stream contains the stream to be written or read.
		@param implementation the implementation ID.
	*/
	void ReadWrite(IPMStream* stream, ImplementationID implementation);

private:
	
	K2Vector<ResaPub> resaPubList;
};

CREATE_PERSIST_PMINTERFACE(XRLResaPubList, kXRLResaPubListImpl)

/* Constructor
*/
XRLResaPubList::XRLResaPubList(IPMUnknown* boss)
: CPMUnknown<IResaPubList>(boss)
{
}

/* Destructor
*/
XRLResaPubList::~XRLResaPubList()
{
}


/* SetResaPubList
*/
void XRLResaPubList::SetResaPubList(const K2Vector<ResaPub>& newList)
{
	PreDirty();
	resaPubList = newList;	
}

/* GetResaPubList
*/
K2Vector<ResaPub> XRLResaPubList::GetResaPubList()
{
	return resaPubList;
}


/* ReadWrite
*/
void XRLResaPubList::ReadWrite(IPMStream* stream, ImplementationID implementation)
{
	if(stream->IsReading()) // Fill the vector with the saved value
	{
		int32 length = 0;
		stream->XferInt32(length);
	
		resaPubList.clear();
		for(int32 i = 0; i < length; i++)
		{
			ResaPub aPub;
			aPub.resaID.ReadWrite(stream);
			aPub.pubID.ReadWrite(stream);
			stream->XferObject(aPub.resaUID);
			stream->XferInt32(aPub.pageID);	
			resaPubList.push_back(aPub);
		}
	}
	else // Save the item in the vector
	{
		int32 length = resaPubList.size();
		stream->XferInt32(length);

		K2Vector<ResaPub>::iterator iter;
		for (iter = resaPubList.begin(); iter < resaPubList.end(); iter++)
		{
			ResaPub aPub = *iter;
			aPub.resaID.ReadWrite(stream);
			aPub.pubID.ReadWrite(stream);
			stream->XferObject(aPub.resaUID);
			stream->XferInt32(aPub.pageID);		
		}	
	}
}

