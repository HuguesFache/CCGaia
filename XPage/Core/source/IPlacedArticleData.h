
#ifndef __IPlacedArticleData_h__
#define __IPlacedArticleData_h__

class PMString;
class IPMUnknown;

#include "XPGID.h"

/**	data interface that stores forme data
*/
class IPlacedArticleData : public IPMUnknown
{
public:
	enum	{kDefaultIID = IID_IPLACEDARTICLEDATA};
	
	/** Set the story id. 
	 */
	virtual void SetUniqueId(const PMString& value) = 0;

	/** Get the story id.
	 */
	virtual const PMString&	GetUniqueId() = 0;

	/** Set the story topic. 
	 */
    virtual void SetStoryTopic(const PMString& value) = 0;

	/** Get the story topic.
	 */
	virtual const PMString&	GetStoryTopic() = 0;

	/** Set the story path. 
	 */
    virtual void SetStoryFolder(const PMString& value) = 0;

	/** Get the story path.
	 */
	virtual const PMString&	GetStoryFolder() = 0;
};

#endif // __IFormeData_h__

// End, IFormeData.h.



