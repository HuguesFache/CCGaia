/*
//	File:	IResaPubList.h
//
//  Author: Trias
//
//	Date:	3-Mars-2008
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2008 Trias Developpement. All rights reserved.
//
*/

#ifndef __IResaPubList_h__
#define __IResaPubList_h__

#include "XRLID.h"
#include "K2Vector.h"
#include "K2Vector.tpp"

class IPMUnknown;

// Structure pour les reserves de pub
typedef struct resapub 
{
	typedef object_type data_type; // Pour l'utiliser avec des K2Vector

	PMString resaID;
	PMString pubID;
	int32 pageID;
	UID resaUID;
	 
	resapub()
	{
		resaID = kNullString;
		pubID = kNullString;
		pageID = 0;
		resaUID = kInvalidUID;		 
	}

	inline bool operator==  (const struct resapub& aPub) const
	{
		return ( this->resaID.Compare(kFalse, aPub.resaID) == 0);			 
	} 
	
} ResaPub;


/** Provides get and set methods to handle a list of ResaPub
*/
class IResaPubList : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_IRESAPUBLIST };

		virtual void SetResaPubList(const K2Vector<ResaPub>& newList) =0;
		virtual K2Vector<ResaPub> GetResaPubList() =0;			
};

#endif // __IResaPubList_h__
