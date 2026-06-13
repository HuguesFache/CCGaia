
#ifndef __ITextDataModel_H_DEFINED__
#define __ITextDataModel_H_DEFINED__

#include "IPMUnknown.h"
#include "IXPageUtils.h" // For IDValuePair definition
#include "XPGID.h"

class XPGArticleDataNode;

class IArticleDataModel : public IPMUnknown
{
public:
	enum { kDefaultIID = IID_IARTICLEDATAMODEL };

	/**	Accessor for the root path
		@return PMString giving the path associated with the root node
	 */
	virtual PMString GetRootID() const =0 ;


	virtual XPGArticleDataNode* GetNode(const PMString& id) const = 0;

	/**	Given a path, discover the path of its parent
		@param path [IN] specifies given path
		@return Returns path as string
	 */
	virtual const XPGArticleDataNode * GetParent(const PMString& id) const=0;

	/**	Determine the numero of children given a path
		@param path [IN] specifies given path
		@return Returns numero of children
	 */
	virtual int32 GetChildCount(const PMString& id)=0; 

	/**	Get the path associated with the specified child
		@param path [IN] specifies given path
		@param nth [IN] specifies which child
		@return Returns path as string
	 */
	virtual const XPGArticleDataNode * GetNthChild(const PMString& id, int32 nth)=0; 

	/**	Determine the index in the parent's list of kids for 
		a particular child
		@param par [IN] specifies parent
		@param kid [IN] specifies kid
		@return Returns index 
	 */
	virtual int32 GetChildIndexFor(const PMString& par, const PMString& kid)=0; 

	/**	Call when you want to force a recalculation
	 */
    virtual void Rebuild(const PMString& parutionID, const PMString& rubID, const PMString& ssRubID)=0;

};


#endif // __ITextDataModel_H_DEFINED__

