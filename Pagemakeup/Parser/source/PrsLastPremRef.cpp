/*
//	File:	PrsLastPremRef.cpp
//
//  Author: Wilfried LEFEVRE
//
//	Date:	14/09/2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2005 Trias Developpement. All rights reserved.
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "ILastPermRefsTag.h"

// Implementation Includes
#include "CPMUnknown.h"

// Project includes:
#include "PrsID.h"
#include "GlobalDefs.h"
#include "CAlert.h"

class PrsLastPremRef:public CPMUnknown<ILastPermRefsTag>
{
	public:
	
		PrsLastPremRef(IPMUnknown* boss);		
		virtual ~PrsLastPremRef() {}		
		virtual const PermRefStruct& getLastReference();	
		virtual void setLastReference(const PermRefStruct& reference);
		
	private :		
		PermRefStruct reference;		 
};

CREATE_PMINTERFACE(PrsLastPremRef, kPrsLastPremRefImpl)


/* Constructor
*/
PrsLastPremRef::PrsLastPremRef(IPMUnknown* boss): CPMUnknown<ILastPermRefsTag>(boss){
	reference = kNullPermRef;
}


/* getLastReference
*/
const PermRefStruct& PrsLastPremRef::getLastReference(){
	return this->reference;
}

/* setLastReference
*/
void PrsLastPremRef::setLastReference(const PermRefStruct& reference){ 
	this->reference = reference;
}
