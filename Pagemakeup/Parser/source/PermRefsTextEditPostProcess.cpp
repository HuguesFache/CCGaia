/*
//	File:	PermRefsTextEditPostProcess.cpp
//
//	Author:	Trias
//
//	Date:	05/03/2006
//
*/

#include "VCPlugInHeaders.h"								

// Interface headers
#include "IPermRefsTag.h"
#include "IPermRefsSuite.h"
#include "ISelectionUtils.h"
#include "IStoryList.h"
#include "ITCLReferencesList.h"
#include "TextAttributeRunIterator.h"

// General includes:
#include "CTextCommandPostProcess.h"
#include "VOSSavedData.h"
#include "PermRefsUtils.h"

#include "CAlert.h"
#include "DebugClassUtils.h"

class PermRefsTextEditPostProcess : public CTextCommandPostProcess
{

public:

	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	PermRefsTextEditPostProcess(IPMUnknown *iBoss);
	
	/** Destructor. */
	virtual ~PermRefsTextEditPostProcess(void);

	virtual void  InsertPostProcess (ITextModel *model, TextIndex position, const WideString *data, const ILanguage *pLanguage=nil); 

	virtual void PastePostProcess (ITextModel *model, TextIndex position, const PasteData *data, int32 subrange);

};

/* CREATE_PMINTERFACE
 	Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PermRefsTextEditPostProcess, kPermRefsTextEditPostProcessImpl)

/* PermRefsTextEditPostProcess Constructor
*/
PermRefsTextEditPostProcess::PermRefsTextEditPostProcess(IPMUnknown* iBoss) :
	CTextCommandPostProcess(iBoss)
{
}

/* PermRefsTextEditPostProcess Destructor
*/
PermRefsTextEditPostProcess::~PermRefsTextEditPostProcess(void)
{
}


void PermRefsTextEditPostProcess::InsertPostProcess(ITextModel *model, TextIndex position, const WideString *data, const ILanguage *pLanguage)
{
	PermRefStruct permrefToCompare = kNullPermRef;
	if(position != 0 && position < model->TotalLength()-1)
	{
		if(PermRefsUtils::IsAnyTagInRange(model,position-1, 1, permrefToCompare))
		{
			if(PermRefsUtils::IsSameTagInRange(model,position+data->Length(), 1, permrefToCompare))
			{
				PermRefsUtils::ApplyPermRefs(permrefToCompare, model, position, position+data->Length());
			}
		}
	}
}

void PermRefsTextEditPostProcess::PastePostProcess(ITextModel *model, TextIndex position, const PasteData *data, int32 subrange)
{
	// Check whether story is user accessible : if so, data has been pasted, if not data has copied (or cutted)
	InterfacePtr<IStoryList> storyList (data->GetStoryRef().GetDataBase(), data->GetStoryRef().GetDataBase()->GetRootUID(), UseDefaultIID());
	if(storyList && (storyList->GetUserAccessibleStoryIndex(data->GetStoryRef().GetUID()) != -1))
	{
		PermRefStruct permrefToCompare = kNullPermRef;
		bool16 pastedInTag = kFalse;
		if(position != 0 && position < model->TotalLength()-1)
		{
			if(PermRefsUtils::IsAnyTagInRange(model,position-1, 1, permrefToCompare))
			{
				// If pasted into a tag, tag is applied to the whole data
				if(PermRefsUtils::IsSameTagInRange(model,position+data->GetDataLength(), 1, permrefToCompare))
				{
					PermRefsUtils::ApplyPermRefs(permrefToCompare, model, position, position+data->GetDataLength());
					pastedInTag = kTrue;

				}				
			}
		}
		
		if(!pastedInTag)
		{
			// Look for permrefs to update the document's internal stored list
			InterfacePtr<ITCLReferencesList> docRefList (storyList, UseDefaultIID());
			if(docRefList == nil)
				return;

			PermRefVector storyList = docRefList->GetStoryList();

			if(PermRefsUtils::UpdatePermRefsList(storyList, model, position, position + data->GetDataLength()))
			{
				// PermRef list has been updated, save it
				UIDList docToSave(docRefList);
				InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
				
				saveItemsCmd->SetUndoability(ICommand::kAutoUndo);
				saveItemsCmd->SetItemList(docToSave);

				InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());
				cmdData->SetStoryList(storyList);
				cmdData->SetItemList(docRefList->GetItemList());
				cmdData->SetTableList(docRefList->GetTableList());

				CmdUtils::ProcessCommand(saveItemsCmd);				
			}
		}
	}
}
