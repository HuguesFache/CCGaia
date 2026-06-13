#ifndef __IXRailPageSlugData__
#define __IXRailPageSlugData__

#include "IPMUnknown.h"
#include "XRLID.h"

/**	Interface for XRail data.
 */
class IXRailPageSlugData : public IPMUnknown
{
public:
	enum { kDefaultIID = IID_PAGESLUGDATA };

	/** 
		Set ID of page
	*/
	virtual void SetID(const int32 id) = 0;
	virtual int32 GetID() = 0;

#if MULTIBASES == 1
	/** 
		Set BaseName
	*/
	virtual void SetBaseName(const PMString basename) = 0;
	virtual PMString GetBaseName() = 0;
#endif
};

#endif // __IXRailPageSlugData__



