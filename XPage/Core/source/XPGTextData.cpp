
#include "VCPlugInHeaders.h"

// API includes
#include "CPMUnknown.h"

// Project includes:
#include "XPGID.h" 
#include "ITextData.h"
#include "IPMStream.h"

/** Persistent implementation of IFormeData
*/
class XPGTextData : public CPMUnknown<ITextData>
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGTextData(IPMUnknown* boss);
	
	/** Destructor
	 */
	virtual ~XPGTextData() {};


	virtual void SetUniqueName(const PMString& value);
	virtual const PMString&	GetUniqueName();

	virtual void SetType(const int16& value);
	virtual const int16&	GetType();

	virtual void SetPath(const PMString& value);
	virtual const PMString&	GetPath();

	
	virtual void SetNbSignes(const int32& value);
	virtual const int32&	GetNbSignes();


	/** This is a persistent implementation
	*/
	void ReadWrite(IPMStream *  s, ImplementationID  prop);

private:
	PMString	uniqueName, path;
	int16 type;
	int32 nbSignes;
};

/* Makes the implementation available to the application.
*/
CREATE_PERSIST_PMINTERFACE(XPGTextData, kXPGTextDataImpl)

/* Constructor
*/
XPGTextData::XPGTextData(IPMUnknown* boss)
: CPMUnknown<ITextData>(boss), uniqueName(kNullString), type(kNoType), path(kNullString), nbSignes(-1)
{
}

void XPGTextData::SetUniqueName(const PMString& value)
{
	PreDirty();
	uniqueName = value;
	
}

const PMString&	XPGTextData::GetUniqueName()
{	
	return uniqueName;
}

void XPGTextData::SetType(const int16& value)
{	
	PreDirty();
	type = value;

}

const int16&	XPGTextData::GetType()
{
	return type;
}

void XPGTextData::SetPath(const PMString& value) {
    PreDirty();
	path = value;

}
const PMString&	XPGTextData::GetPath(){
    return path;
}

	
const int32&	XPGTextData::GetNbSignes()
{	
	return nbSignes;
}

void XPGTextData::SetNbSignes(const int32& value)
{
	PreDirty();
	nbSignes = value;
}


void XPGTextData::ReadWrite(IPMStream *  s, ImplementationID  prop)
{
	uniqueName.ReadWrite(s);
    path.ReadWrite(s);
	s->XferInt16((int16&)type);
	s->XferInt32((int32&)nbSignes);

}

// End XPGTextData.cpp


