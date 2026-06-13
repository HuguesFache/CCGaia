/*
//
//	Author: Trias Developpement
//
//	Date: 22-Juin-2006
//
//  File : XPGPageMgrAccessor.cpp
//
*/

#include "VCPluginHeaders.h"

// Interface includes:
#include "IXPageMgrAccessor.h"
// General includes:
#include "CAlert.h"

// Project includes:
#include "XPGID.h"


/** Implements IStartupShutdownService; purpose is to install the idle task.
*/
class XPGPageMgrAccessor : public CPMUnknown<IXPageMgrAccessor>
{
public:


	/**	Constructor
		@param boss 
	 */
    XPGPageMgrAccessor(IPMUnknown* boss );

	/**	Destructor
	 */
	virtual ~XPGPageMgrAccessor();

	/**
		Returns a reference to the text data model
	*/
	virtual IArticleDataModel * QueryArticleDataModel();

	/**
		Returns a reference to the image data model
	*/
	virtual IImageDataModel * QueryImageDataModel(IImageDataModel::ImageType typeImage = IImageDataModel::kPhoto);

	/**
		Returns a reference to the template data model
	*/
	virtual IFormeDataModel * QueryFormeDataModel();
	
	/**
		Returns a reference to the assembly data model
	*/
	virtual IAssemblageDataModel * QueryAssemblageDataModel();
	
	
private:

	IArticleDataModel * pageMgr;

};

CREATE_PMINTERFACE(XPGPageMgrAccessor, kXPageMgrAccessorImpl)


/* Constructor
*/
XPGPageMgrAccessor::XPGPageMgrAccessor(IPMUnknown *boss) : CPMUnknown<IXPageMgrAccessor>(boss)
{
	pageMgr = ::CreateObject2<IArticleDataModel>(kXPageManagerBoss);
}

/* Destructor
*/
XPGPageMgrAccessor::~XPGPageMgrAccessor()
{
	pageMgr->Release();
}

IArticleDataModel * XPGPageMgrAccessor::QueryArticleDataModel()
{
	return static_cast<IArticleDataModel *> (pageMgr->QueryInterface(IID_IARTICLEDATAMODEL));
}

IImageDataModel * XPGPageMgrAccessor::QueryImageDataModel(IImageDataModel::ImageType typeImage)
{
	IImageDataModel * dataModel = nil;
	switch(typeImage)
	{
		case IImageDataModel::kPhoto: 
			dataModel = static_cast<IImageDataModel *> (pageMgr->QueryInterface(IID_IIMAGEDATAMODEL)); 
			break;
		case IImageDataModel::kOther: 
			dataModel = static_cast<IImageDataModel *> (pageMgr->QueryInterface(IID_IIMAGEDATAMODEL)); 
			break;
	}

	return dataModel;
}

IFormeDataModel * XPGPageMgrAccessor::QueryFormeDataModel()
{
	return static_cast<IFormeDataModel *> (pageMgr->QueryInterface(IID_IFORMEDATAMODEL));
}

IAssemblageDataModel * XPGPageMgrAccessor::QueryAssemblageDataModel()
{
	return static_cast<IAssemblageDataModel *> (pageMgr->QueryInterface(IID_IASSEMBLAGEDATAMODEL));
}
