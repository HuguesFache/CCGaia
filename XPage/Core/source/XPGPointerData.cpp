/*
//
//	Author: Trias Developpement
//
//  File : XPGPointerData.cpp
//
*/

#include "VCPluginHeaders.h"

// Interface includes:
#include "IPointerData.h"
#include "IXPageUtils.h"

// General includes:
#include "CAlert.h"

// Project includes:
#include "XPGID.h"

/** 
*/
class XPGPointerData : public CPMUnknown<IPointerData>
{
public:


	/**	Constructor
		@param boss 
	 */
    XPGPointerData(IPMUnknown* boss );

	/**	Destructor
	 */
	virtual ~XPGPointerData();

	virtual void Set(void * ptr);

	virtual void * Get() const;

private:

	void * dataPtr;

};

CREATE_PMINTERFACE(XPGPointerData, kXPGPointerDataImpl)

/* Constructor
*/
XPGPointerData::XPGPointerData(IPMUnknown *boss) : CPMUnknown<IPointerData>(boss), dataPtr(nil)
{
}

/* Destructor
*/
XPGPointerData::~XPGPointerData()
{
}

void XPGPointerData::Set(void * ptr)
{
	dataPtr = ptr;
}

void * XPGPointerData::Get() const
{
	return dataPtr;
}
