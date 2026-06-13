
#include "VCPlugInHeaders.h"

// API includes
#include "CPMUnknown.h"

// Project includes:
#include "XPGID.h" 
#include "IPlacedArticleData.h"
#include "IPMStream.h"
#include "FileUtils.h"
#include "CAlert.h"

class XPGPlacedArticleData : public CPMUnknown<IPlacedArticleData>
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGPlacedArticleData(IPMUnknown* boss);
	
	/** Destructor
	 */
	virtual ~XPGPlacedArticleData() {};


	/** Set the story id. 
	 */
	virtual void SetUniqueId(const PMString& value);

	/** Get the story id. 
	 */
	virtual const PMString&	GetUniqueId();

	/** Set the story path. 
	 */
    virtual void SetStoryFolder(const PMString& value);

	/** Get the story path. 
	 */
	virtual const PMString&	GetStoryFolder();

		/** Set the story topic. 
	 */
    virtual void SetStoryTopic(const PMString& value);

	/** Get the story topic.
	 */
	virtual const PMString&	GetStoryTopic();

	/** This is a persistent implementation
	*/
	void ReadWrite(IPMStream *  s, ImplementationID  prop);

private:
	PMString uniqueId;
	PMString storyPath;
	PMString storyTopic;

};

/* Makes the implementation available to the application.
*/
CREATE_PERSIST_PMINTERFACE(XPGPlacedArticleData, kXPGPlacedArticleDataImpl)

/* Constructor
*/
XPGPlacedArticleData::XPGPlacedArticleData(IPMUnknown* boss):CPMUnknown<IPlacedArticleData>(boss), uniqueId(kNullString), storyPath(kNullString){}

void XPGPlacedArticleData::SetUniqueId(const PMString& value){
	PreDirty();
	uniqueId = value;
	
}

const PMString&	XPGPlacedArticleData::GetUniqueId(){	
	return uniqueId;
}

void XPGPlacedArticleData::SetStoryFolder(const PMString& value){
	PreDirty();
	storyPath = value;	
}

const PMString&	XPGPlacedArticleData::GetStoryFolder(){
	return storyPath;
}

/** Set the story topic. 
 */
void XPGPlacedArticleData::SetStoryTopic(const PMString& value){
	PreDirty();
	storyTopic = value;
}

/** Get the story topic.
 */
	
const PMString&	XPGPlacedArticleData::GetStoryTopic(){
	return storyTopic;
}

void XPGPlacedArticleData::ReadWrite(IPMStream *  s, ImplementationID  prop){
	uniqueId.ReadWrite(s);
    storyPath.ReadWrite(s);
	storyTopic.ReadWrite(s);
}

// End XPGPlacedArticleData.cpp


