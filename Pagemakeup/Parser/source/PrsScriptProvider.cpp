/*
//	File:	PrsScriptProvider.cpp
//
//	Author:	Trias
//
//	Date:	23/11/2005
//
*/

#include "VCPlugInHeaders.h"
#include "GlobalDefs.h"

// Interface includes:
#include "IScript.h"
#include "IScriptRequestData.h"
#include "IWorkspace.h"
#include "IDocument.h"
#include "ITCLReferencesList.h"
#include "ISelectionManager.h"
#include "ILayoutSelectionSuite.h"
#include "IParserSuite.h"
#include "IPageItemTypeUtils.h"
#include "IHierarchy.h"
#include "IMultiColumnTextFrame.h"
#include "ISelectionUtils.h"

#if !SERVER
	#include "ILayoutUIUtils.h"
#else
	#include "IApplication.h"
	#include "IDocumentList.h"
#endif

// General includes:
#include "CAlert.h"
#include "CScriptProvider.h"
#include "SDKFileHelper.h"

// Project includes:
#include "PrsScriptingDefs.h"
#include "PrsID.h"
#include "ITCLParser.h"
#include "TCLError.h"

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
    In the debug build, you can set a breakpoint in HandleMethod() to step through the 
    interaction between the script and this plug-in.


*/
class PrsScriptProvider : public CScriptProvider
{
public:
	/** Constructor.
	@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsScriptProvider(IPMUnknown* boss) : CScriptProvider(boss) {};

	/** Destructor. Does nothing.
	*/
	~PrsScriptProvider() {}

	/** This method is called if a provider can handle an event.
	@param eventID identifies the ID of the event to handle.
	@param data identifies an interface pointer used to extract data.
	@param parent identifies an interface pointer on the script object representing the parent of the application object.
	*/
	virtual ErrorCode HandleMethod(ScriptID eventID, IScriptRequestData* data, IScript* parent);

	/** This method is called if a provider can handle a property.
	@param propID identifies the ID of the property to handle.
	@param data identifies an interface pointer used to extract data.
	@param parent identifies an interface pointer on the script object 		representing the parent of the application object.
	*/
	virtual ErrorCode AccessProperty(ScriptID propID, IScriptRequestData* data, IScript* parent);

private:
	/** This method delegated to handle the e_ExecTCL event, which extracts the p_TCLFileName parameter, and then execute the TCL file
	@param data identifies an interface pointer used to extract data.
	@param parent identifies an interface pointer on the script object representing the parent of the application object.
	*/
	virtual ErrorCode ExecTCL(IScriptRequestData* data, IScript* parent);

	virtual ErrorCode GetPermRefs(IScriptRequestData* data, IScript* parent);
};


/* CREATE_PMINTERFACE
Binds the C++ implementation class onto its ImplementationID 
making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PrsScriptProvider, kPrsScriptProviderImpl)


/* HandleMethod */
ErrorCode PrsScriptProvider::HandleMethod(ScriptID eventID, IScriptRequestData* data, IScript* parent)
{
	ErrorCode status = kFailure;
	switch (eventID.Get())
	{
		case e_ExecuteTCLFile:
			status = ExecTCL(data, parent);
			break;
		
		case e_GetPermRefs :
			status = GetPermRefs(data, parent);
			break;
		default:
			status = CScriptProvider::HandleMethod(eventID, data, parent);
	}

    return status;
}


/* AccessProperty
*/
ErrorCode PrsScriptProvider::AccessProperty(ScriptID propID, IScriptRequestData* data, IScript* parent)
{
	return CScriptProvider::AccessProperty(propID, data, parent);
}


/* ExecTCL
*/
ErrorCode PrsScriptProvider::ExecTCL(
	IScriptRequestData* data, IScript* parent)
{
	int32 numError = -1; // AppleEventError
	IDFile TCLFile;
	ScriptData scriptData;
	ErrorCode status = kFailure;
	
	do
	{
		status = data->ExtractRequestData(p_FileName, scriptData);
		
		if(status != kSuccess)
			break;
		
		// Get TCL filename to execute
		PMString filename;
		status = scriptData.GetPMString(filename);
		
		if(status != kSuccess)
			break;
		
		status = kFailure;
		
		filename.SetTranslatable(kFalse);
		
		SDKFileHelper fileHelper(filename);
		if(fileHelper.IsExisting())
		{
			TCLFile = fileHelper.GetIDFile();
		}
		else
		{
			TCLError error(PMString(kErrCannotReadFile));
			numError = error.GetErrorCode();
			break;
		}
	
		InterfacePtr<ITCLParser> parser ((ITCLParser *) ::CreateObject(kTCLParserBoss,IID_ITCLPARSER));
		if (parser == nil)
		{
			numError = SHOULD_NOT_OCCUR_ERROR;
			break;	
		}
		
		// Don't display messages
		parser->SetStopDisplay(kTrue);
		
		// Launch the parser
		if(!parser->SetInputFile(TCLFile))
		{
			TCLError error(PMString(kErrCannotReadFile));
			numError = error.GetErrorCode();
			break;
		}

		numError = parser->LaunchParser();
		status = kSuccess;
		
	} while(kFalse);
	
	ScriptData outputData;
	outputData.SetInt32(numError);
	data->InsertRequestData(data->GetDesiredType(), outputData);
	
	return status;
}

ErrorCode PrsScriptProvider::GetPermRefs(IScriptRequestData* data, IScript* parent)
{
	ErrorCode status = kFailure;
	IDFile TCLFile;
	ScriptData scriptData;
	
	do
	{
		TCLRef refBloc = kZeroTCLRef;
		
		status = data->ExtractRequestData(p_Ref, scriptData);
		if(status == kSuccess) // means a parameter has been passed
		{
#if REF_AS_STRING
			status = scriptData.GetPMString(refBloc);	
#else
			status = scriptData.GetInt32(&refBloc);	
#endif		
			if(status != kSuccess)
				break;
		}
		
		// Select the text bloc
#if !SERVER
		IDocument * doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
#else
		// In InDesign Server mode, the front doc is the latest opened document
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> documentList(application->QueryDocumentList());
		IDocument * doc = nil;
		if(documentList->GetDocCount() > 0)
			doc = documentList->GetNthDoc(documentList->GetDocCount()-1);
#endif
		if(doc == nil)
			break;
		
		IDataBase * db = ::GetDataBase(doc);
		
		InterfacePtr<ITCLReferencesList> docStoredList (doc, UseDefaultIID());
		if(docStoredList == nil)
			break;
		
		InterfacePtr<ISelectionManager> selectionManager (Utils<ISelectionUtils>()->QueryActiveSelection());
		
		UID itemUID = kInvalidUID;
		if(refBloc != 0)
		{
			// Get uid of item referenced by refBloc
			RefVector itemList = docStoredList->GetItemList();
			
			int32 location = ::FindLocation(itemList, refBloc);
			if(location == -1)
				break;
			
			itemUID = itemList[location].Value();
							
			// Select this item
			InterfacePtr<ILayoutSelectionSuite> layoutSelSuite (selectionManager, UseDefaultIID());
			
			selectionManager->DeselectAll(nil);
			layoutSelSuite->SelectPageItems(UIDList(db, itemUID), Selection::kReplace, Selection::kDontScrollLayoutSelection);
		}
		else
		{
			// Get selected item uid
			UIDList frameSelectedList;
			InterfacePtr<IParserSuite> prsLayoutSuite (selectionManager, UseDefaultIID());
			if(prsLayoutSuite == nil) // no items selected
				break;
			
			prsLayoutSuite->GetSelectedItems(frameSelectedList);
			
			if(frameSelectedList.Length() != 1)
				break;
			
			itemUID = frameSelectedList[0];
		}
		
		// Check if it's a text box
		if(!Utils<IPageItemTypeUtils>()->IsTextFrame(UIDRef(db,itemUID)))
			break;
		
		// Get related story
		InterfacePtr<IHierarchy> itemHier (db, itemUID, UseDefaultIID());
		InterfacePtr<IMultiColumnTextFrame> txtFrame (db, itemHier->GetChildUID(0), UseDefaultIID());
		
		UID story = txtFrame->GetTextModelUID();
		
		PermRefVector storyList = docStoredList->GetStoryList();
		// Look for where this story is referenced and populate permref list
		ScriptListData PermRefsList;
		for(PermRefVector::iterator iter = storyList.begin() ; iter < storyList.end() ; iter++)
		{
			K2Vector<UID>& storiesUID = iter->Value();
			
			K2Vector<UID>::iterator iter2 = ::K2find(storiesUID.begin(), storiesUID.end(),story);
			if(iter2 != storiesUID.end())
			{
				ScriptData permref;
				permref.SetInt32(iter->Key().ref);
				PermRefsList.push_back(permref);
			}
		}

		ScriptData outputData;
		outputData.SetList(PermRefsList);
		data->InsertRequestData(data->GetDesiredType(), outputData);
		status = kSuccess;
		
	} while(kFalse);
	
	return status;
}
