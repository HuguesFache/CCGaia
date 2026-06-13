
#include "VCPlugInHeaders.h"

// API includes
#include "CPMUnknown.h"

// Project includes:
#include "XPGID.h" 
#include "IArticleData.h"
#include "IPMStream.h"

class XPGArticleData : public CPMUnknown<IArticleData>
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGArticleData(IPMUnknown* boss);
	
	/** Destructor
	 */
	virtual ~XPGArticleData() {};


	/** Set the forme name. 
	 */
	virtual void SetUniqueName(const PMString& value);

	/** Get the forme name.
	 */
	virtual const PMString&	GetUniqueName();

	/** Set the status name. 
	 */
    virtual void SetStatus(const PMString& value);

	/** Get the status name.
	 */
	virtual const PMString&	GetStatus();

	/** This is a persistent implementation
	*/
	void ReadWrite(IPMStream *  s, ImplementationID  prop);

private:
	PMString	uniqueName;
	PMString	currentStatus;

};

/* Makes the implementation available to the application.
*/
CREATE_PERSIST_PMINTERFACE(XPGArticleData, kXPGArticleDataImpl)

/* Constructor
*/
XPGArticleData::XPGArticleData(IPMUnknown* boss)
: CPMUnknown<IArticleData>(boss), uniqueName(kNullString), currentStatus(kNullString)
{
}

void XPGArticleData::SetUniqueName(const PMString& value)
{
	PreDirty();
	uniqueName = value;
	
}

const PMString&	XPGArticleData::GetUniqueName()
{	
	return uniqueName;
}

void XPGArticleData::SetStatus(const PMString& value)
{
	PreDirty();
	currentStatus = value;
	
}

const PMString&	XPGArticleData::GetStatus()
{	
	return currentStatus;
}

void XPGArticleData::ReadWrite(IPMStream *  s, ImplementationID  prop)
{
	uniqueName.ReadWrite(s);
    currentStatus.ReadWrite(s);
}

// End XPGArticleData.cpp


