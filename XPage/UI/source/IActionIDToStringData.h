
#ifndef __IActionIDToStringData_h__
#define __IActionIDToStringData_h__

#include "IPMUnknown.h"
#include "ActionID.h"

//========================================================================================
// CLASS IActionIDToStringData
//========================================================================================

class IActionIDToStringData : public IPMUnknown
{
public:
     

    virtual int32 Length() = 0;

    virtual PMString GetString(ActionID actionID) = 0;
    
    virtual PMString GetFirstString() = 0;

    virtual void RemoveEntry(ActionID actionID) =0;
    
    virtual void AddEntry(ActionID actionID, PMString someObject)=0;
    
};

#endif // __IActionIDToStringData_h__
