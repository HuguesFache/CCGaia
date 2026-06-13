/*
//
//	Author: Trias Developpement
//
//	Date: 27-Juin-2006
//
//  File : XPGLSIDData.cpp
//
*/

#include "VCPluginHeaders.h"

// Interface includes:
#include "ILSIDData.h"
#include "IPMStream.h"

// General includes:
#include "CAlert.h"

// Project includes:
#include "XPGID.h"

/** Implements ILSIDData
*/
class XPGLSIDData : public CPMUnknown<ILSIDData>
{
public:


	/**	Constructor
		@param boss 
	 */
    XPGLSIDData(IPMUnknown* boss );

	/**	Destructor
	 */
	virtual ~XPGLSIDData();

	virtual void Set(LSID id);

	virtual LSID Get() const;

	/** This is a persistent implementation
	*/
	virtual void ReadWrite(IPMStream *  s, ImplementationID  prop);

private:
	LSID data;
};

CREATE_PERSIST_PMINTERFACE(XPGLSIDData, kXPGLSIDDataPersistImpl)

/* Constructor
*/
XPGLSIDData::XPGLSIDData(IPMUnknown *boss) : CPMUnknown<ILSIDData>(boss), data(kInvalidLSID){}

/* Destructor
*/
XPGLSIDData::~XPGLSIDData(){}

void XPGLSIDData::Set(LSID id){
	PreDirty();
	data = id;
}

LSID XPGLSIDData::Get() const{
	return data;
}

void XPGLSIDData::ReadWrite(IPMStream *  s, ImplementationID  prop){
	s->XferInt32((int32&)data);
}

