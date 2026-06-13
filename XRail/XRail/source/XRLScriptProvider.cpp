/*
//	File:	XRLScriptProvider.cpp
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
#include "IDocument.h"
#include "IPageList.h"
#include "IIntData.h"
#include "ILayoutUIUtils.h"

// General includes:
#include "CAlert.h"
#include "ResourceEnabler.h"
#include "CScriptProvider.h"
#include "UIDList.h"

// Project includes:
#include "XRLScriptingDefs.h"
#include "XRLID.h"
#include "DocUtils.h"
#include "IXRailPageSlugData.h"

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
class XRLScriptProvider : public CScriptProvider
{
public:
	/** Constructor.
	@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XRLScriptProvider(IPMUnknown* boss) : CScriptProvider(boss) {};

	/** Destructor. Does nothing.
	*/
	~XRLScriptProvider() {}

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
	ErrorCode AccessPageDataProperty(ScriptID propID, IScriptRequestData* data, IScript* script);

};


/* CREATE_PMINTERFACE
Binds the C++ implementation class onto its ImplementationID 
making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XRLScriptProvider, kXRLScriptProviderImpl)


/* AccessProperty
*/
ErrorCode XRLScriptProvider::AccessProperty(ScriptID propID, IScriptRequestData* data, IScript* script)
{
	ErrorCode result = kFailure;
	do
	{
		switch (propID.Get())
		{
			case p_XRailPageData:
			{
				result = this->AccessPageDataProperty(propID, data, script);
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
		result = kFailure;//kXPGFailedToLoadFormeDataErrorCode;

	return result;
}

/*
*/
ErrorCode XRLScriptProvider::AccessPageDataProperty(ScriptID propID, IScriptRequestData* data, IScript* script)
{
	ErrorCode result = kFailure;
	do {
		// The script object identifies the page item whose IFormeData is to be changed.
	
		int32 pageid;
		PMString baseName;
		if (data->IsPropertyPut()) {

			if (!data->HasRequestData(propID)) break;

			ScriptData inputdata;
			if(data->ExtractRequestData(propID, inputdata) != kSuccess) break;

			ScriptListData pageProperties;
			if(inputdata.GetList(pageProperties) != kSuccess) break;

            //CAlert::InformationAlert("data->IsPropertyPut()) v9");
			if(pageProperties.size() != 2) break;
			if(pageProperties[0].GetInt32(&pageid) != kSuccess) break;
			if(pageProperties[1].GetPMString(baseName) != kSuccess) break;
            
            InterfacePtr<IDocument> frontDoc ( Utils<IScriptUtils>()->QueryDocumentFromScript(script,data->GetRequestContext()));
            if(frontDoc == nil) break;
            UIDRef docUIDRef = ::GetUIDRef(frontDoc);
            
            UIDRef pageUIDref = ::GetUIDRef(script);
            UID pageUID = pageUIDref.GetUID();
            
            InterfacePtr<IPageList> pageList(docUIDRef, UseDefaultIID());
            int32 curIndex = pageList->GetPageIndex(pageUID);
            
			SetPageID(::GetUIDRef(frontDoc), curIndex, pageid, baseName);
            
            //PMString titi;
            //titi.AppendNumber(pageid);
            //CAlert::InformationAlert(titi);
		}
		else if (data->IsPropertyGet()) {
            //CAlert::InformationAlert("data->IsPropertyGet()) v9");
            PMString basename;
			
            InterfacePtr<IDocument> frontDoc ( Utils<IScriptUtils>()->QueryDocumentFromScript(script,data->GetRequestContext()));
            if(frontDoc == nil) break;
            UIDRef docUIDRef = ::GetUIDRef(frontDoc);
            
            UIDRef pageUIDref = ::GetUIDRef(script);
            UID pageUID = pageUIDref.GetUID();
            
            InterfacePtr<IXRailPageSlugData> readdata (docUIDRef.GetDataBase(), pageUID, IID_PAGESLUGDATA);
            
            int32 currentID = readdata->GetID();
#if MULTIBASES == 1
            baseName = readdata->GetBaseName();
#endif
            
			ScriptListData pageProperties;
			pageProperties.push_back(ScriptData(currentID));
            pageProperties.push_back(ScriptData(basename));

			// Return the value to the caller.
			ScriptData outputData;
			outputData.SetList(pageProperties);
			data->AppendReturnData(script, propID, outputData);	

            //PMString tutu;
            //tutu.AppendNumber(currentID);
            //CAlert::InformationAlert(tutu);
			
			result = kSuccess;
		}	
	} while (false);
	
	return result;
}
