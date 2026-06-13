
#ifndef __IAssemblageDataModel_H_DEFINED__
#define __IAssemblageDataModel_H_DEFINED__

#include "IPMUnknown.h"
#include "IDataBase.h"
#include "IXPageUtils.h"
#include "XPGID.h"

class XPGFormeDataNode;

class IAssemblageDataModel : public IPMUnknown
{
public:
	enum { kDefaultIID = IID_IASSEMBLAGEDATAMODEL };

	/**	Accessor for the root path
	@return PMString giving the path associated with the root node
	*/
	virtual PMString GetRootID() const =0 ;


	virtual XPGFormeDataNode* GetNode(const PMString& id) const = 0;

	/**	Given a path, discover the path of its parent
	@param path [IN] specifies given path
	@return Returns path as string
	*/
	virtual const XPGFormeDataNode * GetParent(const PMString& id) const=0;

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
	virtual const XPGFormeDataNode * GetNthChild(const PMString& id, int32 nth)=0; 

	/**	Determine the index in the parent's list of kids for 
	a particular child
	@param par [IN] specifies parent
	@param kid [IN] specifies kid
	@return Returns index 
	*/
	virtual int32 GetChildIndexFor(const PMString& par, const PMString& kid)=0; 

	/**	Call when you want to force a recalculation
	*/
	virtual void Rebuild(const PMString& classeurName)=0;

	/** Return list of classeurs used to filter formes
	*/
	virtual K2Vector<PMString> GetClasseurList() =0;

	/** Return true if forme exist for this classeur, false otherwise
	*/
	virtual bool16 DoesAssemblageExist(const PMString& assemblageName, const PMString& classeurName) const =0;

	/** Create a new forme
	*/
	virtual ErrorCode CreateAssemblage(const PMString& name, const PMString& classeurName, const UIDList& assemblageItemsToExport) =0;

	/** Delete a forme
	*/
	virtual ErrorCode DeleteAssemblage(const PMString& name, const PMString& classeurName) =0;
	
	
	virtual IDFile GetAssemblageRoot() = 0;

	/** Create a new classeur
	*/
	virtual ErrorCode CreateClasseur(const PMString& classeurName) =0;

	/** Delete a classeur
	*/
	virtual ErrorCode DeleteClasseur(const PMString& classeurName) =0;

	/** Return true if classeur exist, false otherwise (case insensitive)
	*/
	virtual bool16 DoesClasseurExist(const PMString& classeurName) =0;

};


#endif // __IAssemblageDataModel_H_DEFINED__

