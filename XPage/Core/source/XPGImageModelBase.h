#ifndef __XPGImageModelBase_H_DEFINED__
#define __XPGImageModelBase_H_DEFINED__

#include "IPMUnknown.h"
#include "XPGID.h"

// Impl includes
#include <map>

/**
	Base implementation for image model
*/
class XPGImageModelBase : public CPMUnknown<IImageDataModel>
{
public:

	/** Default constructor */
	XPGImageModelBase(IPMUnknown* boss);

	/**	Destructor
	 */
	virtual ~XPGImageModelBase();

	/**	Accessor for the root path
		@return PMString giving the path associated with the root node
	 */
	virtual PMString GetRootID() const;


	virtual XPGImageDataNode* GetNode(const PMString& id) const;

	/**	Given a path, discover the path of its parent
		@param path [IN] specifies given path
		@return Returns path as string
	 */
	virtual const XPGImageDataNode * GetParent(const PMString& id) const;

	/**	Determine the numero of children given a path
		@param path [IN] specifies given path
		@return Returns numero of children
	 */
	virtual int32 GetChildCount(const PMString& id); 

	/**	Get the path associated with the specified child
		@param path [IN] specifies given path
		@param nth [IN] specifies which child
		@return Returns path as string
	 */
	virtual const XPGImageDataNode * GetNthChild(const PMString& id, int32 nth); 

	/**	Determine the index in the parent's list of kids for 
		a particular child
		@param par [IN] specifies parent
		@param kid [IN] specifies kid
		@return Returns index 
	 */
	virtual int32 GetChildIndexFor(const PMString& par, const PMString& kid); 

protected:

	/** Cache the kids of the given node in file system
	*/
	virtual void cacheChildren(const PMString& id) =0;

	
	/**	Determine if this is a path that we want to put in the data model
		@param p [IN] specifies path of interest
		@return bool16 if the path is a file system path of interest
	 */
	bool16 validPath(const PMString& p);

	/**	Destroy the tree represented in this 
	 */
	void deleteTree();

	std::map<PMString, XPGImageDataNode* > fIdNodeMap;
	XPGImageDataNode* fRootNode;

	PMString racineArbo;
	PMString imageDir;
};


#endif // __XPGImageModelBase_H_DEFINED__

