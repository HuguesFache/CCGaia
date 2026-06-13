
#ifndef __IXPageMgrAccessor_h__
#define __IXPageMgrAccessor_h__

#include "IPMUnknown.h"
#include "IArticleDataModel.h"
#include "IImageDataModel.h"
#include "IFormeDataModel.h"
#include "IAssemblageDataModel.h"

//========================================================================================
// CLASS IXPageMgrAccessor
//========================================================================================

class IXPageMgrAccessor : public IPMUnknown
{
public:

	enum { kDefaultIID = IID_IXPAGEMGRACCESSOR };
     
	/**
		Returns a reference to the text data model
	*/
	virtual IArticleDataModel * QueryArticleDataModel() =0;

	/**
		Returns a reference to the image data model
	*/
	virtual IImageDataModel * QueryImageDataModel(IImageDataModel::ImageType typeImage = IImageDataModel::kPhoto) =0;

	/** 
		Returns a reference to the template data model
	*/
	virtual IFormeDataModel * QueryFormeDataModel() =0;

	/**
		Returns a reference to the assembly data model
	*/
	virtual IAssemblageDataModel * QueryAssemblageDataModel() =0;

};

#endif // __IXPageMgrAccessor_h__
