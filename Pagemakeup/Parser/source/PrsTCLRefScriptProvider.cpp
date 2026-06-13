/*
//	File:	PrsTCLRefScriptProvider.cpp
//
//	Author:	Trias
//
//	Date:	23/11/2006
//
*/

#include "VCPlugInHeaders.h"

#include "GlobalDefs.h"

// Interface includes:
#include "IScriptRequestData.h"
#include "IDocument.h"
#include "IScriptUtils.h"
#include "ILayoutTarget.h"
#include "ITCLParser.h"
#include "ITextTarget.h"
#include "ITextModel.h"
#include "IAttributeStrand.h"
#include "IPermRefsTag.h"
#include "ITCLReferencesList.h"
#include "ITextScriptUtils.h"

// General includes:
#include "CScriptProvider.h"
#include "ResourceEnabler.h"
#include "Utils.h"
#include "GenericID.h"

// Project includes:
#include "PrsID.h"
#include "PrsScriptingDefs.h"
#include "PermRefsUtils.h"

/** Handles scripting of the TCL Reference's data property
	(p_TCLRef) added onto spline items in interface ITCLRefData. See the Provider
	statement for kPrsTCLRefScriptProviderBoss in Prs.fr for the list of scripting objects
	on which the p_TCLRef property is available.

 */
class PrsTCLRefScriptProvider : public CScriptProvider
{
public:
	/**	Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	 */
	PrsTCLRefScriptProvider(IPMUnknown* boss): CScriptProvider(boss) {}

	/**	Destructor.
	 */
	~PrsTCLRefScriptProvider() {}

	/** Called if a provider can handle a method/event.
		@param eventID identifies the ID of the method/event to handle.
		@param data identifies an interface pointer used to extract data.
		@param script identifies an interface pointer on the script object representing the parent of the application object.
	 */
	virtual ErrorCode  HandleMethod(ScriptID eventID, IScriptRequestData * data, IScript* script);    

	/** Called if a provider can handle a property.
		@param propID identifies the ID of the property to handle.
		@param data identifies an interface pointer used to extract data.
		@param script identifies an interface pointer on the script object representing the parent of the application object.
	 */
	virtual ErrorCode  AccessProperty(ScriptID propID, IScriptRequestData * data, IScript* script);

private:
	/** Set or get the value of ITCLRefData on a page item.
		@param propID identifies the ID of the property to handle (p_TCLRef).
		@param data identifies an interface pointer used to extract parameter data.
		@param script identifies an interface pointer on a type of page item script object.
		@return kSuccess on success, other ErrorCode otherwise.
	*/
	ErrorCode AccessTCLRefDataProperty(ScriptID propID, IScriptRequestData * data, IScript* script);	

	ErrorCode AccessPermRefTagProperty(ScriptID propID, IScriptRequestData * data, IScript* script);
};

/*	Make the implementation available to the application.
*/
CREATE_PMINTERFACE(PrsTCLRefScriptProvider, kPrsTCLRefScriptProviderImpl)

/*
*/
ErrorCode PrsTCLRefScriptProvider::HandleMethod(ScriptID eventID, IScriptRequestData* data, IScript* script)
{
	// No events.
	return CScriptProvider::HandleMethod(eventID, data, script);
}

/*
*/
ErrorCode PrsTCLRefScriptProvider::AccessProperty(ScriptID propID, IScriptRequestData* data, IScript* script)
{
	ErrorCode result = kFailure;
	do
	{
		switch (propID.Get())
		{
			case p_TCLRef:
			{
				result = this->AccessTCLRefDataProperty(propID, data, script);
				break;
			}

			case p_PermRefTag:
			{
				result = this->AccessPermRefTagProperty(propID, data, script);
				break;
			}

			default:
			{
				result = CScriptProvider::AccessProperty(propID, data, script);
				break;
			}
		}
	} while (false);

	return result;
}

/*
*/
ErrorCode PrsTCLRefScriptProvider::AccessTCLRefDataProperty(ScriptID propID, IScriptRequestData* data, IScript* script)
{
	ErrorCode result = kFailure;
	do 
	{
		// The script object identifies the page item whose ITCLRefData is to be changed.

		if (data->IsPropertyPut())
		{
			// Get the ITCLRef value passed by the script.	
			if (!data->HasRequestData(propID)) break;
		
			TCLRef ref;
			ScriptData inputdata;
			if( data->ExtractRequestData(propID, inputdata) != kSuccess) break;

#if REF_AS_STRING
			if(inputdata.GetPMString(ref) != kSuccess) break;
#else
			if(inputdata.GetInt32(&ref) != kSuccess) break;
#endif
			
			// Set the ITCLRef value.
			IDataBase * db = ::GetDataBase(script);
			InterfacePtr<ITCLRefData> tclRefData (script, IID_ITCLREFDATA);
			if(tclRefData)
			{			
				InterfacePtr<ICommand> setTCLRefCmd (CmdUtils::CreateCommand(kPrsSetTCLRefDataCmdBoss));
				setTCLRefCmd->SetUndoability(ICommand::kAutoUndo);
				setTCLRefCmd->SetItemList(UIDList(tclRefData));
				
				InterfacePtr<ITCLRefData> cmdData (setTCLRefCmd, IID_ITCLREFDATA);
				cmdData->Set(ref);
				CmdUtils::ProcessCommand(setTCLRefCmd);
			}

			result = kSuccess;
		}
		else if (data->IsPropertyGet()) 
		{
			// Get the ITCLRef value.
			InterfacePtr<ITCLRefData> tclRefData (script, IID_ITCLREFDATA);
			
			TCLRef ref = kDefaultTCLRefData;
			if(tclRefData)			
				ref = tclRefData->Get();

			// Return the value to the caller.
			ScriptData outputData;
#if REF_AS_STRING
			outputData.SetPMString(ref);
#else
			outputData.SetInt32(ref);
#endif
			data->AppendReturnData(script, propID, outputData);	

			result = kSuccess;
		}
	} while (false);

	return result;
}


ErrorCode PrsTCLRefScriptProvider::AccessPermRefTagProperty(ScriptID propID, IScriptRequestData* data, IScript* script)
{
	ErrorCode result = kFailure;
	do 
	{
		InterfacePtr<ITextModel> txtModel (Utils<ITextScriptUtils>()->QueryTextModel(script));
		if(!txtModel)
			break;
	
		RangeData range = Utils<ITextScriptUtils>()->GetScriptTextRange(script);

		if (data->IsPropertyPut()) 
		{
			// Get permrefs value to set
			if (!data->HasRequestData(propID)) break;			
			
			ScriptData inputdata;
			if(data->ExtractRequestData(propID, inputdata) != kSuccess) break;

			ScriptListData permrefsData;
			if(inputdata.GetList(permrefsData) != kSuccess) break;

			PermRefStruct reference;
			permrefsData[0].GetInt32((int32 *) &(reference.ref));
			permrefsData[1].GetInt32((int32 *) &(reference.param1));
			permrefsData[2].GetInt32((int32 *) &(reference.param2));
			permrefsData[3].GetInt32((int32 *) &(reference.param3));
			permrefsData[4].GetInt32((int32 *) &(reference.param4));
		
			// Apply the tag to the character range			
			if(PermRefsUtils::ApplyPermRefs(reference, txtModel, range.Start(nil), range.End()) != kSuccess) break;

			result = kSuccess;
		}
		else if (data->IsPropertyGet()) 
		{	
			// Get tag value
			PermRefStruct reference = kNullPermRef;
			
			InterfacePtr<IAttributeStrand> charStrand (static_cast<IAttributeStrand* >(txtModel->QueryStrand(kCharAttrStrandBoss, IID_IATTRIBUTESTRAND)));
			DataWrapper<AttributeBossList> localCharOverrides = charStrand->GetLocalOverrides(range.Start(nil),nil);

			InterfacePtr<IPermRefsTag> permrefsTag ((IPermRefsTag *) (*localCharOverrides).QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
			if(permrefsTag != nil && permrefsTag->getReference() != kNullPermRef)
			{
				reference = permrefsTag->getReference();
			}

			// Return the value to the caller.
			ScriptListData permrefsData;
			permrefsData.push_back(ScriptData((int32) reference.ref));
			permrefsData.push_back(ScriptData((int32) reference.param1));
			permrefsData.push_back(ScriptData((int32) reference.param2));
			permrefsData.push_back(ScriptData((int32) reference.param3));
			permrefsData.push_back(ScriptData((int32) reference.param4));

			ScriptData outputData;
			outputData.SetList(permrefsData);
			data->AppendReturnData(script, propID, outputData);

			result = kSuccess;
		}
	} while (false);

	return result;
}
// PrsTCLRefScriptProvider.cpp
