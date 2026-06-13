
#ifndef __IImageDataModel_H_DEFINED__
#define __IImageDataModel_H_DEFINED__

#include "IPMUnknown.h"
#include "XPGID.h"
#include "K2Vector.h"
#include "K2Vector.tpp"

class XPGImageDataNode;

class IImageDataModel : public IPMUnknown
{
public:
	enum { kDefaultIID = IID_IIMAGEDATAMODEL };

	enum ImageType { kPhoto, kInfographie, kOther };

	/**	Accessor for the root path
		@return PMString giving the path associated with the root node
	 */
	virtual PMString GetRootID() const =0 ;


	virtual XPGImageDataNode* GetNode(const PMString& id) const = 0;

	/**	Given a path, discover the path of its parent
		@param path [IN] specifies given path
		@return Returns path as string
	 */
	virtual const XPGImageDataNode * GetParent(const PMString& id) const=0;

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
	virtual const XPGImageDataNode * GetNthChild(const PMString& id, int32 nth)=0; 

	/**	Determine the index in the parent's list of kids for 
		a particular child
		@param par [IN] specifies parent
		@param kid [IN] specifies kid
		@return Returns index 
	 */
	virtual int32 GetChildIndexFor(const PMString& par, const PMString& kid)=0; 

	/**	Call when you want to force a recalculation
	 */
	virtual void Rebuild(const PMString& numeroId, ImageType typeImage = kPhoto)=0;

	/**	Multi-article variant of Rebuild used by the Photos liées panel's
		"Voir toutes les photos" mode. Each (articleIds[i], articleFiles[i])
		pair points at one OBJRART file (articleFiles[i] is the assignment
		file path; the OBJRART lives in the same folder under the name
		articleIds[i] + ".OBJRART.xml"). Implementations replace the
		current tree with the union of every article's photos.
		Default impl is a no-op so models that don't support this mode
		(e.g. TEC) keep compiling.
	*/
	virtual void RebuildFromArticles(const K2Vector<PMString>& articleIds,
									 const K2Vector<PMString>& articleFiles) {}

	/**
		Set a filter (an article ID) for images
	*/
	virtual void SetFilter(const PMString& filter, const PMString& filterPath)=0;

	/** Allows direct access to a image data without having to rebuild the whole model
		Requires a the article file path this image is linked to
	*/
	virtual bool16 GetImageData(const PMString& id, const IDFile& linkedArticleFile, IDFile& brFile, const bool16& wantNB,
								IDFile& hrFile, PMString& legend, PMString& credit) =0;
};


#endif // __IImageDataModel_H_DEFINED__

