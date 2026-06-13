
#include "VCPlugInHeaders.h"

// API includes
#include "CPMUnknown.h"

// Project includes:
#include "XPGID.h" 
#include "IFormeData.h"
#include "IPMStream.h"
#include "UIDList.h"

#include "K2Vector.h"
#include "K2Vector.tpp"
#include "KeyValuePair.h"

typedef KeyValuePair<PMString, PMString> IDValuePair;

/** Persistent implementation of IFormeData
*/
class XPGFormeData : public CPMUnknown<IFormeData>
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGFormeData(IPMUnknown* boss);
	
	/** Destructor
	 */
	virtual ~XPGFormeData() {};


	virtual void SetUniqueName(const PMString& value);
	virtual const PMString&	GetUniqueName();

	virtual void SetType(const int16& value);
	virtual const int16&	GetType();

	virtual void SetPhotoIndex(const int16& value);
	virtual const int16&	GetPhotoIndex();

	virtual void SetNbSignes(const int32& value);
	virtual const int32& GetNbSignes();

	virtual const K2Vector<IDValuePair> GetMatchingTagsNames();
	virtual void SetMatchingTagsNames(const K2Vector<IDValuePair> values);

	virtual void SetTECstoryID(const PMString& value);
	virtual const PMString&	GetTECstoryID();

	/** This is a persistent implementation
	*/
	void ReadWrite(IPMStream *  s, ImplementationID  prop);

private:

	PMString	uniqueName;
	int16 type, photoIndex;
	int32 nbSignes;
	K2Vector<IDValuePair> matchingStyles;
	PMString	TECstoryID;
};

/* Makes the implementation available to the application.
*/
CREATE_PERSIST_PMINTERFACE(XPGFormeData, kXPGFormeDataImpl)

/* Constructor
*/
XPGFormeData::XPGFormeData(IPMUnknown* boss) : CPMUnknown<IFormeData>(boss), 
												uniqueName(kNullString), 
												type(kNoType), 
												photoIndex(-1), 
												nbSignes(-1)
{
	matchingStyles.clear();
}

void XPGFormeData::SetUniqueName(const PMString& value)
{
	PreDirty();
	uniqueName = value;
	
}

const PMString&	XPGFormeData::GetUniqueName()
{	
	return uniqueName;
}


const K2Vector<IDValuePair> XPGFormeData::GetMatchingTagsNames()
{
	return matchingStyles;
}

void XPGFormeData::SetMatchingTagsNames(const K2Vector<IDValuePair> value)
{
	PreDirty();
	matchingStyles = value;
}

void XPGFormeData::SetType(const int16& value)
{	
	PreDirty();
	type = value;
}

const int16& XPGFormeData::GetType()
{
	return type;
}

void XPGFormeData::SetPhotoIndex(const int16& value)
{
	PreDirty();
	photoIndex = value;		
}

const int16&	XPGFormeData::GetPhotoIndex()
{
	return photoIndex;
}
const int32&	XPGFormeData::GetNbSignes()
{	
	return nbSignes;
}

void XPGFormeData::SetNbSignes(const int32& value)
{
	PreDirty();
	nbSignes = value;
}


void XPGFormeData::ReadWrite(IPMStream *  s, ImplementationID  prop)
{
	uniqueName.ReadWrite(s);
	
	s->XferInt16((int16&)type);
	s->XferInt16((int16&)photoIndex);

	for(int32 i=0; i < matchingStyles.size(); i++){
         matchingStyles[i].Key().ReadWrite(s);
		 matchingStyles[i].Value().ReadWrite(s);
    }
}

void XPGFormeData::SetTECstoryID(const PMString& value)
{
	PreDirty();
	this->TECstoryID = value;
	
}

const PMString&	XPGFormeData::GetTECstoryID()
{	
	return this->TECstoryID;
}

// End XPGFormeData.cpp


