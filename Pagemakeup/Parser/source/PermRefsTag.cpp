/*
//	File:	PermRefsTag.cpp
//
//  Author: Trias
//
//	Date:	14/09/2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2005 Trias Developpement. All rights reserved.
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IPermRefsTag.h"
#include "IPMStream.h"

// Implementation Includes
#include "CPMUnknown.h"

// Project includes:
#include "PrsID.h"
#include "GlobalDefs.h"
#include "CAlert.h"

class PermRefsTag : public CPMUnknown<IPermRefsTag>
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on 
			which this interface is aggregated.
		*/
		PermRefsTag(IPMUnknown* boss);

		/**
			Destructor.
		*/
		virtual ~PermRefsTag() {}
		
		virtual const PermRefStruct& getReference();	
		virtual void setReference(const PermRefStruct& reference);

		/**
		Because this is a persistent interface, it must support the ReadWrite method. This method is used  
		for both writing information out to the database and reading information in from the database.		
		@param s contains the stream to be written or read.
		@param prop the implementation ID.
		*/
		void ReadWrite(IPMStream* s, ImplementationID prop);
		
	private :
		
		PermRefStruct reference;	
	 
};
		

/*
	CREATE_PERSIST_PMINTERFACE 
	Connects the implementation class to the application. You will note that the data tag
	boss (kPermRefsAttrBoss) does not aggregate the persistence interface (IPMPersist). Text
	attributes must however be persistent. They are stored together in a collection.
*/
CREATE_PERSIST_PMINTERFACE(PermRefsTag, kPermRefsTagImpl)


/* Constructor
*/
PermRefsTag::PermRefsTag(IPMUnknown* boss): CPMUnknown<IPermRefsTag>(boss){
	reference = kNullPermRef;
}


/* getReference
*/
const PermRefStruct& PermRefsTag::getReference(){
	return this->reference;
}

/* setReference
*/
void PermRefsTag::setReference(const PermRefStruct& reference){
	PreDirty();
	this->reference = reference;
}


/*	ReadWrite
*/
void PermRefsTag::ReadWrite(IPMStream* s, ImplementationID prop){

	// ReadWrite Reference
	s->XferInt32((int32&)reference.ref);
	s->XferInt32((int32&)reference.param1);
	s->XferInt32((int32&)reference.param2);
	s->XferInt32((int32&)reference.param3);
	s->XferInt32((int32&)reference.param4);
}
