#include "VCPluginHeaders.h"

// Interface includes
#include "IActionIDToStringData.h"

// Project includes
#include "K2Vector.h"
#include "KeyValuePair.h"
#include "K2Vector.tpp"

#include "XPGUIID.h"

/**  Class representing a folder hierarchy. 
	@ingroup paneltreeview
*/
class ActionIDToStringDataImpl : CPMUnknown<IActionIDToStringData>
{
public:
	/** Default constructor */
	ActionIDToStringDataImpl(IPMUnknown* boss);

	/**	Destructor
	 */
	virtual ~ActionIDToStringDataImpl();

	virtual int32 Length();

    virtual PMString GetFirstString();

    virtual PMString GetString(ActionID actionID);
    
    virtual void RemoveEntry(ActionID actionID);
    
    virtual void AddEntry(ActionID actionID, PMString someObject);

 

private:
	
	K2Vector<KeyValuePair<ActionID, PMString> > action2stringMap;

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(ActionIDToStringDataImpl, kActionIDToStringDataImpl)


/* Constructor
*/
ActionIDToStringDataImpl::ActionIDToStringDataImpl(IPMUnknown* boss) : 
	CPMUnknown<IActionIDToStringData>(boss)
{
}
	
/* Destructor
*/
ActionIDToStringDataImpl::~ActionIDToStringDataImpl(){}

int32 ActionIDToStringDataImpl::Length(){
	return action2stringMap.size();
}

PMString ActionIDToStringDataImpl::GetString(ActionID actionID){
	PMString data = kNullString;
	int32 pos = ::FindLocation(action2stringMap, actionID);
	if(pos != -1)
		data = action2stringMap[pos].Value();

	return data;
}

PMString ActionIDToStringDataImpl::GetFirstString(){    
 	return action2stringMap[0].Value();
}

void ActionIDToStringDataImpl::RemoveEntry(ActionID actionID){
	int32 pos = ::FindLocation(action2stringMap, actionID);
	if(pos != -1)
		action2stringMap.erase(action2stringMap.begin() + pos);		
}

void ActionIDToStringDataImpl::AddEntry(ActionID actionID, PMString someObject){
	::InsertOrReplaceKeyValue(action2stringMap, actionID, someObject);
}

