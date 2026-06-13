
#ifndef __IFormeDataModel_H_DEFINED__
#define __IFormeDataModel_H_DEFINED__

#include "IPMUnknown.h"
#include "IDataBase.h"
#include "IXPageUtils.h"
#include "XPGID.h"

class XPGFormeDataNode;

class IFormeDataModel : public IPMUnknown
{
public:
	enum { kDefaultIID = IID_IFORMEDATAMODEL };

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

	/** Returns kTrue if the named forme was created with the "Carton photo"
		flag set in its descriptif XML. Returns kFalse for legacy descriptifs
		(attribute absent), missing files, or any parsing error — callers
		should treat the result as a best-effort hint, not a strict failure
		signal.
	*/
	virtual bool16 IsPhotoCarton(const PMString& formeName, const PMString& classeurName) =0;

	/** Return true if forme exist for this classeur, false otherwise
	*/
	virtual bool16 DoesFormeExist(const PMString& formeName, const PMString& classeurName) const =0;

	/** Create a new forme.
		@param isPhotoCarton kTrue when the carton being created is a "photo
			carton" (a carton whose role is to add images to other cartons
			later). Persisted as the isPhotoCarton attribute on the FORME
			element of the descriptif XML; readers default to kFalse when
			the attribute is missing, so existing cartons stay backward-compat.
	*/
	virtual ErrorCode CreateForme(const PMString& name, const PMString& classeurName, K2Vector<IDValuePair> matchingStyles,
								  const UIDList& textFrames, const UIDList& photoFrames, const UIDList& fixedContentFrames,
								  const UIDRef& storyRef, const UIDList& creditRefs,
								  const UIDList& legendRefs, const UIDList& formeItemsToExport,
								  const PMString posx, const PMString posy, const PMString largeur, const PMString hauteur, const int32 nbsignesestim,
								  const bool16 isPhotoCarton) =0;

	/** Delete a forme
	*/
	virtual ErrorCode DeleteForme(const PMString& name, const PMString& classeurName) =0;

	/** Create a new classeur
	*/
	virtual ErrorCode CreateClasseur(const PMString& classeurName) =0;

	/** Delete a classeur
	*/
	virtual ErrorCode DeleteClasseur(const PMString& classeurName) =0;

	/** Return true if classeur exist, false otherwise (case insensitive)
	*/
	virtual bool16 DoesClasseurExist(const PMString& classeurName) =0;

	/** Return list of formes for a classeur
	*/
	virtual K2Vector<PMString> GetFormeList(const PMString& classeurName) =0;

	/** Allows direct access to a forme data without having to rebuild the whole model
	*/
	virtual bool16 GetForme(const PMString& name, const PMString& classeurName, 
							IDFile& formeFile, IDFile& matchingFile, PMString& posX, PMString& posY) =0;
	
};


#endif // __IFormeDataModel_H_DEFINED__

