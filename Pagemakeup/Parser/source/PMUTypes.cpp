/*
//	File:	PMUTypes.cpp
//
//	Date:	22-Mar-2006
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

#include "PMUTypes.h"

#include "TCLError.h"
#include "PrsID.h"


#if REF_AS_STRING

TCLRef operator++(TCLRef& ref) // Prefix increment operator for BG and B+ command
{ 
	PMString::ConversionError error = PMString::kNoError;
	
	int32 tmp = ref.GetAsNumber();
	if(error != PMString::kNoError) throw TCLError(PMString(kErrConv));

	++tmp;

	ref.AsNumber(tmp);
	return ref; 
}

TCLRef operator--(TCLRef& ref) // Prefix decrement operator for B- command
{ 
	PMString::ConversionError error = PMString::kNoError;
	
	int32 tmp = ref.GetAsNumber();
	if(error != PMString::kNoError) throw TCLError(PMString(kErrConv));

	--tmp;

	ref.AsNumber(tmp);
	return ref;
}

int32 operator-(const TCLRef& ref1, const TCLRef& ref2){
	PMString::ConversionError error = PMString::kNoError;
	
	int32 tmpRef1 = ref1.GetAsNumber();
	if(error != PMString::kNoError) throw TCLError(PMString(kErrConv));

	int32 tmpRef2 = ref2.GetAsNumber();
	if(error != PMString::kNoError) throw TCLError(PMString(kErrConv));

	return (tmpRef1-tmpRef2);
}

#endif