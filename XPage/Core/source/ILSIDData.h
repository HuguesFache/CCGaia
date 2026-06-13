
#ifndef __ILSIDData_h__
#define __ILSIDData_h__

#include "IPMUnknown.h"
#include "XMLReference.h"

#include "XPGID.h"

//========================================================================================
// CLASS ILSIDData
//========================================================================================

class ILSIDData : public IPMUnknown
{
public:
     
	enum { kDefaultIID = IID_ILSIDDATA };
    
	virtual void Set(LSID id) =0;

	virtual LSID Get() const =0;
};

#endif // __ILSIDData_h__
