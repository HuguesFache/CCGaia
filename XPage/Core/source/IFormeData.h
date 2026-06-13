
#ifndef __IFormeData_h__
#define __IFormeData_h__

class PMString;
class IPMUnknown;

#include "XPGID.h"
#include "K2Vector.h"
#include "K2Vector.tpp"
#include "KeyValuePair.h"

typedef KeyValuePair<PMString, PMString> IDValuePair;


/**	data interface that stores forme data
*/
class IFormeData : public IPMUnknown
{
public:
	enum	{kDefaultIID = IID_IFORMEDATA};

	enum type { kNoType =0, kArticle, kPhoto, kCredit, kLegend, kStyle, kFixedContent };
	
	/** Set the forme name. 
	 */
	virtual void SetUniqueName(const PMString& value) = 0;

	/** Get the forme name.
	 */
	virtual const PMString&	GetUniqueName() = 0;

	/** Set the frame forme type. 
	 */
	virtual void SetType(const int16& value) = 0;

	/** Get the frame forme type.
	 */
	virtual const int16&	GetType() = 0;

	/** Set the photo index (only used for kPhoto, kCredit and kLegend type) 
	 */
	virtual void SetPhotoIndex(const int16& value) = 0;

	/** Get the photo index
	 */
	virtual const int16&	GetPhotoIndex() = 0;

	/** Get the count of Characters
	 */
	virtual const int32& GetNbSignes()=0;

	/** Set the count of Characters
	 */
	virtual void SetNbSignes(const int32& value)=0;

	/** Set matching tags names
	 */
	virtual const K2Vector<IDValuePair> GetMatchingTagsNames() = 0;

	/** Get matching tags names
	 */
	virtual void SetMatchingTagsNames(const K2Vector<IDValuePair> values) = 0;
	
	/** Set the TECstoryID
	 */
	virtual void SetTECstoryID(const PMString& value) = 0;
	
	/** Get the TECstoryID
	 */
	virtual const PMString&	GetTECstoryID() = 0;
};

#endif // __IFormeData_h__

// End, IFormeData.h.



