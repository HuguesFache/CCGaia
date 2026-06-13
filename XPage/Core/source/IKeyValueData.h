
#ifndef __IKeyValueData_h__
#define __IKeyValueData_h__

#include "IPMUnknown.h"
#include "XPGID.h"
#include "K2Vector.h"
#include "K2Vector.tpp"
#include "KeyValuePair.h"

typedef KeyValuePair<PMString, PMString> IDValuePair;


class IKeyValueData : public IPMUnknown
{
public:
     
	enum { kDefaultIID = IID_IKEYVALUEDATA };
    
	virtual void SetKeyValueList(const K2Vector<IDValuePair> & keyValueList) =0;

	virtual K2Vector<IDValuePair> GetKeyValueList() const =0;
};

#endif // __IKeyValueData_h__
