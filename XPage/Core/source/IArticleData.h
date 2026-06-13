
#ifndef __IArticleData_h__
#define __IArticleData_h__

class PMString;
class IPMUnknown;

#include "XPGID.h"

/**	data interface that stores forme data
*/
class IArticleData : public IPMUnknown
{
public:
	enum	{kDefaultIID = IID_IARTICLEDATA};

	
	/** Set the forme name. 
	 */
	virtual void SetUniqueName(const PMString& value) = 0;

	/** Get the forme name.
	 */
	virtual const PMString&	GetUniqueName() = 0;


    virtual void SetStatus(const PMString& value) = 0;
	virtual const PMString&	GetStatus() = 0;


};

#endif // __IFormeData_h__

// End, IFormeData.h.



