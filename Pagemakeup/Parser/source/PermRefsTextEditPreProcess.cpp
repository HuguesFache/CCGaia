/*
//	File:	PermRefsTextEditPreProcess.cpp
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
#include "ITCLReferencesList.h"

// General includes:
#include "TextAttributeRunIterator.h"
#include "CTextCommandPreProcess.h"
#include "VOSSavedData.h"

#include "CAlert.h"
#include "DebugClassUtils.h"

class PermRefsTextEditPreProcess : public CTextCommandPreProcess
{

public:

	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	PermRefsTextEditPreProcess(IPMUnknown *iBoss);
	
	/** Destructor. */
	virtual ~PermRefsTextEditPreProcess(void);

	virtual bool16  DeletePreProcess (ITextModel *model, TextIndex &position, int32 &length);

	virtual WideString * ReplacePreProcess (ITextModel *model, TextIndex &position, int32 &length, const WideString *data, const ILanguage *language=nil);

private:

};

/* CREATE_PMINTERFACE
 	Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PermRefsTextEditPreProcess, kPermRefsTextEditPreProcessImpl)

/* PermRefsTextEditPreProcess Constructor
*/
PermRefsTextEditPreProcess::PermRefsTextEditPreProcess(IPMUnknown* iBoss) :
	CTextCommandPreProcess(iBoss)
{
}

/* PermRefsTextEditPreProcess Destructor
*/
PermRefsTextEditPreProcess::~PermRefsTextEditPreProcess(void)
{
}

bool16 PermRefsTextEditPreProcess::DeletePreProcess (ITextModel *model, TextIndex &position, int32 &length) 
{
	// Look for tag in deleted text
	K2Vector<PermRefStruct> refsToUpdate, refsFound;
	
	K2Vector<ClassID> attributeClasses;
	attributeClasses.push_back(kPermRefsAttrBoss);

	K2Vector<InDesign::TextRange> textRanges;
	textRanges.push_back(InDesign::TextRange(model, position, length));

	TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
	while(runIter)
	{					
		InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
		if(currentTag != nil && currentTag->getReference() != kNullPermRef)
		{
			PermRefStruct deletedPermRef = currentTag->getReference();
			if (K2notfound(refsFound, deletedPermRef))
			{
				refsFound.push_back(deletedPermRef);

				// a tag is being deleted, look for the same tag in the rest of the story, if any
				bool16 foundSameTag = kFalse;
				if(length < model->TotalLength() - 1)
				{
					K2Vector<InDesign::TextRange> textRanges2;
					if(position != 0)
						textRanges2.push_back(InDesign::TextRange(model, 0, position));
					
					if(position + length != model->TotalLength() - 1)
						textRanges2.push_back(InDesign::TextRange(model, position + length, model->TotalLength() - 1 - (position + length)));
					
					TextAttributeRunIterator runIter2(textRanges2.begin(), textRanges2.end(), attributeClasses.begin(), attributeClasses.end());
					
					while(runIter2 && !foundSameTag)
					{
						InterfacePtr<IPermRefsTag> someTag ((IPermRefsTag *) runIter2->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
						if(someTag != nil && someTag->getReference() == deletedPermRef)
							foundSameTag = kTrue;

						++runIter2;
					}
				}

				if(!foundSameTag)
				{
					refsToUpdate.push_back(deletedPermRef);				
				}
			}
		}

		++runIter;
	}

	IDataBase * db = ::GetDataBase(model);
	InterfacePtr<ITCLReferencesList> docRefList (db, db->GetRootUID(), UseDefaultIID());
	if(docRefList && !refsToUpdate.empty())
	{
		UID modelUID = ::GetUID(model);
		PermRefVector storyList = docRefList->GetStoryList();
		for(int32 i = 0 ; i < refsToUpdate.size() ; ++i)
		{
			int32 location = ::FindLocation(storyList, refsToUpdate[i]);
			if(location >= 0)
			{
				K2Vector<UID> stories = storyList[location].Value();
				/*int32 storyLocation = stories.Location(modelUID);
				if(storyLocation >= 0)
				{
					stories.erase(stories.begin() + storyLocation);
					if(stories.size() == 0)
						storyList.erase(storyList.begin() + location);
					else
						storyList[location].Value() = stories;
				}*/
				auto iter = K2find(stories, modelUID);
				if (iter != stories.end()) //found
				{
					stories.erase(iter);
					if (stories.size() == 0)
						storyList.erase(storyList.begin() + location);
					else
						storyList[location].Value() = stories;
				}
			}			
		}

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

	return CTextCommandPreProcess::DeletePreProcess(model, position, length);
}

WideString * PermRefsTextEditPreProcess::ReplacePreProcess (ITextModel *model, TextIndex &position, int32 &length, const WideString *data, const ILanguage *language)
{
	K2Vector<ClassID> attributeClasses;
	attributeClasses.push_back(kPermRefsAttrBoss);

	K2Vector<InDesign::TextRange> textRanges;
	textRanges.push_back(InDesign::TextRange(model, position, length));

	
	TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());

	bool16 abortReplace = kFalse;
	while(runIter && !abortReplace)
	{					
		InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
		if(currentTag != nil && currentTag->getReference() != kNullPermRef)
			abortReplace = kTrue;

		++runIter;
	}

	if(abortReplace)
	{
		length = 0;
		return new WideString();
	}
	
	
	return CTextCommandPreProcess::ReplacePreProcess(model, position, length,data, language);
}


/*
PasteData * PermRefsTextEditPreProcess::PastePreProcess (ITextModel *model, TextIndex &position, const PasteData *data, int32 subrange)
{
	TextIndex start = 0, end = 0;					
	UIDRef modelRef;

	InterfacePtr<IPermRefsSuite> opsTagSuite (Utils<ISelectionUtils>()->GetActiveSelection(), UseDefaultIID());
	
	if(opsTagSuite)
	{
		opsTagSuite->GetTextSelection(modelRef, start, end);
		InterfacePtr<ITextModel> txtModel (modelRef, UseDefaultIID());

		if(start == end)
		{
			if(start != 0 && start < txtModel->TotalLength()-1)
			{
				PermRefStruct permrefToCompare = kNullPermRef;
				if(isAnyTagInRange(txtModel,start, 1, permrefToCompare))
				{
					if(isSameTagInRange(txtModel,start-1, 1, permrefToCompare))
						ErrorUtils::PMSetGlobalErrorCode(kCancel);
				}
			}
		}
		else
		{
			if(isAnyTagInRange(txtModel,start, end - start))
				ErrorUtils::PMSetGlobalErrorCode(kCancel);
		}
	}

	return CTextCommandPreProcess::PastePreProcess(model, position, data, subrange);
}
*/

