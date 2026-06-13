/*
//	File:	XPGScriptProvider.cpp
//
//	Author:	Trias Developpement
//
//	Date:	23/11/2005
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IScript.h"
#include "IScriptRequestData.h"
#include "IFormeData.h"
#include "IDocument.h"

// General includes:
#include "CAlert.h"
#include "ResourceEnabler.h"
#include "CScriptProvider.h"
#include "UIDList.h"

// Project includes:
#include "XPGScriptingDefs.h"
#include "IXPageUtils.h"
#include "XPGID.h"
#include "IPlacedArticleData.h"
#include "IArticleData.h"

/** Demonstrates extending the Application script object, adding both an event and a property.

    To see the interaction between a script and this plug-in, copy the following code 
    into a file you create named sayhello.js, then place this JavaScript file into 
    a InDesign CSx/Presets/Scripts folder.

    var said = "hello";
    app.speak(said);
    var yourresponse = app.response;
    app.speak(yourresponse);
    
    From the application main menu, choose Window|Automation|Scripts if the 
    Scripts palette is not already visible. Double-click on sayhello.js to start the script. 
    In the debug build, you can set a breakpoint in HandleEvent() to step through the 
    interaction between the script and this plug-in.


*/
class XPGScriptProvider : public CScriptProvider
{
public:
	/** Constructor.
	@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGScriptProvider(IPMUnknown* boss) : CScriptProvider(boss) {};

	/** Destructor. Does nothing.
	*/
	~XPGScriptProvider() {}

	/** This method is called if a provider can handle a property.
	@param propID identifies the ID of the property to handle.
	@param data identifies an interface pointer used to extract data.
	@param script identifies an interface pointer on the script object 		representing the parent of the application object.
	*/
	virtual ErrorCode AccessProperty(ScriptID propID, IScriptRequestData* data, IScript* script);

private:

	/** Set or get the value of IFormeData on a page item.
		@param propID identifies the ID of the property to handle (p_FormeData).
		@param data identifies an interface pointer used to extract parameter data.
		@param script identifies an interface pointer on a type of page item script object.
		@return kSuccess on success, other ErrorCode otherwise.
	*/
	ErrorCode AccessFormeDataProperty(ScriptID propID, IScriptRequestData* data, IScript* script);	
	
	/** Set or get the value of IPlacedArticleData on a spline item.
	 @param propID identifies the ID of the property to handle (p_PlacedArticleData).
	 @param data identifies an interface pointer used to extract parameter data.
	 @param script identifies an interface pointer on a type of page item script object.
	 @return kSuccess on success, other ErrorCode otherwise.
	 */
	ErrorCode AccessPlacedArticleDataProperty(ScriptID propID, IScriptRequestData* data, IScript* script);

	ErrorCode AccessArticleDataProperty(ScriptID propID, IScriptRequestData* data, IScript* script);
};


/* CREATE_PMINTERFACE
Binds the C++ implementation class onto its ImplementationID 
making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPGScriptProvider, kXPGScriptProviderImpl)


/* AccessProperty
*/
ErrorCode XPGScriptProvider::AccessProperty(ScriptID propID, IScriptRequestData* data, IScript* script)
{
	ErrorCode result = kFailure;
	do
	{
		switch (propID.Get())
		{
			case p_FormeData:
			{
				result = this->AccessFormeDataProperty(propID, data, script);
				break;
			}

			case p_PlacedArticleData:
			{
				result = this->AccessPlacedArticleDataProperty(propID, data, script);
				break;
			}
				
			case p_ArticleData:
			{
				result = this->AccessArticleDataProperty(propID, data, script);
				break;
			}
				
			default:
			{
				result = CScriptProvider::AccessProperty(propID, data, script);
				break;
			}
		}
	} while (false);

	if(result != kSuccess)
		result = kXPGFailedToLoadFormeDataErrorCode;

	return result;
}

/*
*/
ErrorCode XPGScriptProvider::AccessFormeDataProperty(ScriptID propID, IScriptRequestData* data, IScript* script)
{
	ErrorCode result = kFailure;
	do {
		// The script object identifies the page item whose IFormeData is to be changed.
	
		if (data->IsPropertyPut()) {

			// Get the IFormeData value passed by the script.	
			if (!data->HasRequestData(propID)) break;
            
			ScriptData inputdata;
			if(data->ExtractRequestData(propID, inputdata) != kSuccess) break;
            
			ScriptListData formeProperties;
			if(inputdata.GetList(formeProperties) != kSuccess) break;
            
			PMString name, TECstoryID;
			int16 type, photoIndex;
            
			if(formeProperties.size() != 3) break;
			if(formeProperties[0].GetPMString(name) != kSuccess) break;
			if(formeProperties[1].GetInt16(&type) != kSuccess) break;
			if(formeProperties[2].GetInt16(&photoIndex) != kSuccess) break;
			
			// Set the IFormeData interface on spline items
			result = Utils<IXPageUtils>()->SetFormeData(::GetUIDRef(script), name, type, photoIndex, TECstoryID);			
		}
		else if (data->IsPropertyGet()) {
					
			// Get the IFormeData value.
			InterfacePtr<IFormeData> formeData (script, IID_IFORMEDATA);
			if(!formeData)
				break;
			
			PMString name = formeData->GetUniqueName();
			int16 type = formeData->GetType();
			int16 photoIndex = formeData->GetPhotoIndex();

			ScriptListData formeProperties;
			formeProperties.push_back(ScriptData(name));
			formeProperties.push_back(ScriptData(type));
			formeProperties.push_back(ScriptData(photoIndex));

			// Return the value to the caller.
			ScriptData outputData;
			outputData.SetList(formeProperties);
			data->AppendReturnData(script, propID, outputData);	

			result = kSuccess;
		}	
	} while (false);
	
	return result;
}

/*
 */
ErrorCode XPGScriptProvider::AccessPlacedArticleDataProperty(ScriptID propID, IScriptRequestData* data, IScript* script)
{
	ErrorCode result = kFailure;
	do {
		// The script object identifies the page item whose IPlacedArticleData is to be changed.
		
		if (data->IsPropertyPut()) {
			
			// Get the IPlacedArticleData value passed by the script.	
			if (!data->HasRequestData(propID)) break;
			
			ScriptData inputdata;
			if(data->ExtractRequestData(propID, inputdata) != kSuccess) break;
			
			ScriptListData formeProperties;
			if(inputdata.GetList(formeProperties) != kSuccess) break;
			
			PMString StoryID, StoryfileFullpath, StorySubject;
			
			if(formeProperties.size() != 3) break;
			if(formeProperties[0].GetPMString(StoryID) != kSuccess) break;
			if(formeProperties[1].GetPMString(StoryfileFullpath) != kSuccess) break;
			if(formeProperties[2].GetPMString(StorySubject) != kSuccess) break;
			
			// Set the IPlacedArticleData interface on spline items
			InterfacePtr<ICommand> placedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
			
			InterfacePtr<IPlacedArticleData> placedArticleData (placedArticleDataCmd, IID_IPLACEDARTICLEDATA);
			placedArticleData->SetUniqueId(StoryID);
			placedArticleData->SetStoryFolder(StoryfileFullpath);
			placedArticleData->SetStoryTopic(StorySubject);
			
			placedArticleDataCmd->SetItemList(UIDList(::GetUIDRef(script)));
			result = CmdUtils::ProcessCommand(placedArticleDataCmd);
		}
		else if (data->IsPropertyGet()) {
			
			// Get the IPlacedArticleData value.
			InterfacePtr<IPlacedArticleData> placedArticleData (script, IID_IPLACEDARTICLEDATA);
			if(!placedArticleData)
				break;
			
			PMString StoryID = placedArticleData->GetUniqueId();
			PMString StoryfileFullpath = placedArticleData->GetStoryFolder();
			PMString StorySubject = placedArticleData->GetStoryTopic();
			
			ScriptListData formeProperties;
			formeProperties.push_back(ScriptData(StoryID));
			formeProperties.push_back(ScriptData(StoryfileFullpath));
			formeProperties.push_back(ScriptData(StorySubject));
			
			// Return the value to the caller.
			ScriptData outputData;
			outputData.SetList(formeProperties);
			data->AppendReturnData(script, propID, outputData);	
			
			result = kSuccess;
		}	
	} while (false);
	
	return result;
}

/*
 */
ErrorCode XPGScriptProvider::AccessArticleDataProperty(ScriptID propID, IScriptRequestData* data, IScript* script)
{
	ErrorCode result = kFailure;
	do {
		// The script object identifies the page item whose IPlacedArticleData is to be changed.
		
		if (data->IsPropertyPut()) {
			
			// Get the IPlacedArticleData value passed by the script.	
			if (!data->HasRequestData(propID)) break;
			
			ScriptData inputdata;
			if(data->ExtractRequestData(propID, inputdata) != kSuccess) break;
			
			ScriptListData formeProperties;
			if(inputdata.GetList(formeProperties) != kSuccess) break;
			
			PMString StoryID, StoryStatus;
			
			if(formeProperties.size() != 3) break;
			if(formeProperties[0].GetPMString(StoryID) != kSuccess) break;
            if(formeProperties[1].GetPMString(StoryStatus) != kSuccess) break;
			
			// Set the IPlacedArticleData interface on spline items
			InterfacePtr<ICommand> ArticleDataCmd (CmdUtils::CreateCommand(kXPGSetArticleDataCmdBoss));
			
			InterfacePtr<IArticleData> articleData (ArticleDataCmd, IID_IARTICLEDATA);
			articleData->SetUniqueName(StoryID);
			articleData->SetStatus(StoryStatus);
			
			ArticleDataCmd->SetItemList(UIDList(::GetUIDRef(script)));
			result = CmdUtils::ProcessCommand(ArticleDataCmd);
		}
		else if (data->IsPropertyGet()) {
			
			// Get the IPlacedArticleData value.
			InterfacePtr<IArticleData> articleData (script, IID_IARTICLEDATA);
			if(!articleData)
				break;
			
			PMString StoryID = articleData->GetUniqueName();
			PMString StoryStatus = articleData->GetStatus();
			
			ScriptListData formeProperties;
			formeProperties.push_back(ScriptData(StoryID));
			formeProperties.push_back(ScriptData(StoryStatus));
			
			// Return the value to the caller.
			ScriptData outputData;
			outputData.SetList(formeProperties);
			data->AppendReturnData(script, propID, outputData);	
			
			result = kSuccess;
		}	
	} while (false);
	
	return result;
}
