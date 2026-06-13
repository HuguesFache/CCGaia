
#ifndef __ITextData_h__
#define __ITextData_h__

class PMString;
class IPMUnknown;

#include "XPGID.h"

/**	data interface that stores forme data
*/
class ITextData : public IPMUnknown
{
public:

	enum	{kDefaultIID = IID_ITEXTDATA};

	enum type { kNoType =0, kMarbre };
	
	/** Set the forme name. 
	 */
	virtual void SetUniqueName(const PMString& value) = 0;

	/** Get the forme name.
	 */
	virtual const PMString&	GetUniqueName() = 0;


    /** Set the forme name. 
	 */
	virtual void SetPath(const PMString& value) = 0;

	/** Get the forme name.
	 */
	virtual const PMString&	GetPath() = 0;

	/** Set the frame forme type. 
	 */
	virtual void SetType(const int16& value) = 0;

	/** Get the frame forme type.
	 */
	virtual const int16&	GetType() = 0;

	/** Get the count of Characters
	 */

	virtual const int32& GetNbSignes()=0;

		/** Set the count of Characters
	 */

	virtual void SetNbSignes(const int32& value)=0;

};

#endif // __ITextData_h__

// End, ITextData.h.



