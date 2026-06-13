
#ifndef __IPointerData_h__
#define __IPointerData_h__

#include "IPMUnknown.h"
#include "XPGID.h"

//========================================================================================
// CLASS IPointerData
//========================================================================================

class IPointerData : public IPMUnknown
{
public:
     
	virtual void Set(void * ptr) =0;

	virtual void * Get() const =0;
};

#endif // __IPointerData_h__
