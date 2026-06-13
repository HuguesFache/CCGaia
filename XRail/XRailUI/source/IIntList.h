/*
//	File:	IIntList.h
//
//  Author: Trias
//
//	Date:	01/06/2007
//
//	Copyright Trias. All rights reserved.
//
*/

#ifndef __IIntList_h__
#define __IIntList_h__

#include "IPMUnknown.h"
#include "K2Vector.h"

#include "XRLUIID.h"

class IPMUnknown;

class IIntList : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_IINTLIST };

		virtual void  SetIntList (const K2Vector< int32 > &newList) =0;

		virtual void  Clear () =0;

		virtual const K2Vector< int32 > &  GetIntList () =0;

};

#endif // __IIntList_h__