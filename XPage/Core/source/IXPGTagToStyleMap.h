
#ifndef __IXPGTagToStyleMap_h__
#define __IXPGTagToStyleMap_h__

#include "IPMUnknown.h"
#include "XPGID.h"


//========================================================================================
// CLASS IXPGTagToStyleMap
//========================================================================================

class IXPGTagToStyleMap : public IPMUnknown
{
public:

	enum { kDefaultIID = IID_IXPGTAGTOSTYLEMAP };

	virtual void Copy(IXPGTagToStyleMap * otherMap) =0;
    
	virtual void ClearMap() =0;

	virtual int32 GetMappingCount() const =0;

	virtual void AddTagToParaStyleMapping(const PMString& tag, UID paraStyle) =0;

	virtual PMString GetTagAt(int32 pos) const =0;

	virtual UID GetParaStyleMappedToTag(const PMString& tag) const =0;

};

#endif // __IXPGTagToStyleMap_h__
