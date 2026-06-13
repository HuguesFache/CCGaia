/*
//	File:	PermRefsUtils.cpp
//
//	Date:	27-oct-2005
//
//	Author : Trias
*/

#include "VCPlugInHeaders.h"

// Project includes
#include "PermRefsUtils.h"
#include "IPermRefsTag.h"
#include "PrsID.h"
#include "GlobalDefs.h"
#include "ITCLWriter.h"
#include "ITextAttrPositionMode.h"
#include "ITextAttrCapitalMode.h"
#include "IItemStrand.h"
#include "ITableFrame.h"
#include "IWorkspace.h"

// API includes
#include "AttributeBossList.h"
#include "TextAttributeRunIterator.h"
#include "ErrorUtils.h"
#include "CAlert.h"
#include "StreamUtil.h"
#include "ILayoutUtils.h"
#include "TextIterator.h"

// Interface includes
#include "ITextAttrUtils.h"
#include "ICommand.h"
#include "ITextModelCmds.h"
#include "ITextModel.h"
#include "IAttributeStrand.h"
#include "IStoryList.h"
#include "ITCLReferencesList.h"
#include "IPMStream.h"
#include "IDocument.h"
#include "IFrameList.h"
#include "ITextFrameColumn.h"
#include "IPageList.h"
#include "IHierarchy.h"
#include "ISectionList.h"
#include "IUIDData.h"
#include "IRangeData.h"
#include "IWaxStrand.h"
#include "ITextParcelList.h"
#include "IParcelList.h"
#include "ITextParcelListComposer.h"
#include "IWaxIterator.h"
#include "IWaxLine.h"
#include "ITextStrand.h"
#include "PMLineSeg.h"
#include "ITableModelList.h"
#include "ITableModel.h"
#include "ProgressBar.h"
#include "FileUtils.h"
#include "IStringData.h"
#include "TextWalkerServiceProviderID.h"
#include "IFindChangeOptions.h"
#include "IFindChangeService.h"

#include "K2Pair.h"
#include <boost/regex.hpp>
#include <boost/regex/icu.hpp>
#include "StringUtils.h"
#include <iostream>
#include <string>
#include <fstream>

K2Vector<UID>	PermRefsUtils::currentStories;
int32			PermRefsUtils::currentStory = -1;
PermRefStruct	PermRefsUtils::currentRef = kNullPermRef;
TextIndex		PermRefsUtils::startIndex = 0; 

ErrorCode PermRefsUtils::ApplyPermRefs(PermRefStruct ref, ITextModel * txtModel, TextIndex start, TextIndex end)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do{
		// Create an attribute boss
		InterfacePtr<IPermRefsTag> txtAttrPermRef ((IPermRefsTag *) ::CreateObject(kPermRefsAttrBoss, IID_IPERMREFSTAG));
		if(txtAttrPermRef == nil)
			break;

		txtAttrPermRef->setReference(ref);

		InterfacePtr<ICommand> applyCmd(Utils<ITextAttrUtils>()->BuildApplyTextAttrCmd(txtModel, start, end - start, txtAttrPermRef, kCharAttrStrandBoss));
		if (CmdUtils::ProcessCommand(applyCmd) != kSuccess){
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

#if AUTO_TAGGING 	
		// Process last permRef on workspace with command
		InterfacePtr<ICommand> lastPermRefTagCmd(CmdUtils::CreateCommand(kPrsLastPremRefCmdBoss));
		InterfacePtr<ILastPermRefsTag> lastPermRefTag(lastPermRefTagCmd, IID_ILASTPERMREFSTAG);
		lastPermRefTag->setLastReference(ref);
		if (CmdUtils::ProcessCommand(lastPermRefTagCmd) != kSuccess){
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
#endif	
		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return status;
}

ErrorCode PermRefsUtils::UpdatePermRefs(PermRefStruct ref, ITextModel * txtModel, boost::shared_ptr< WideString > replaceString, bool16 deleteNextSpaceorCR)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	InDesign::TextRange txtRange (txtModel, 0,  txtModel->TotalLength()-1);

	// Use TextAttributeRunIterator to find our kPermRefsAttrBoss
	K2Vector<InDesign::TextRange> textRanges;
	textRanges.push_back(txtRange);

	K2Vector<ClassID> attributeClasses;
	attributeClasses.push_back(kPermRefsAttrBoss);

	// Construct the iterator.
	TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());

	bool8 refFound = kFalse;
	while(runIter)
	{
		RangeData range = runIter.GetRunRange();

		InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
		if(currentTag != nil && currentTag->getReference() == ref)
		{	
			refFound = kTrue;
			// Reference found, replace text now
			InterfacePtr<ITextModelCmds> modelCmds(txtModel, UseDefaultIID()); 
			
			InterfacePtr<ICommand> clearCmd(Utils<ITextAttrUtils>()->BuildClearTextAttrCmd(txtModel, range, kPermRefsAttrBoss, kCharAttrStrandBoss));
			if((status = CmdUtils::ProcessCommand(clearCmd)) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}

			
			InterfacePtr<ICommand> replaceCmd(modelCmds->ReplaceCmd(range.Start(nil), range.End() - range.Start(nil),replaceString));
			if (replaceCmd == nil)
				break;
			
			if((status = CmdUtils::ProcessCommand(replaceCmd)) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}
						
			boost::shared_ptr< AttributeBossList > list (new AttributeBossList);
			list->ApplyAttribute(currentTag);			

			InterfacePtr<ICommand> applyCmd(modelCmds->ApplyCmd(range.Start(nil),replaceString->Length(),list,kCharAttrStrandBoss));
			if(applyCmd == nil)
				break;

			if((status = CmdUtils::ProcessCommand(applyCmd)) != kSuccess)
			{
				error = PMString(kErrCmdFailed);
				break;
			}
			
			if(deleteNextSpaceorCR)
			{
				int32 nbCharSkipped = 0;
				// BUG FIX v4.3.7 : put TextIterator in a new scope so that it's is deleted 
				// from the stack before the processing of Delete Command, otherwise InDesign crashes !
				{
					TextIterator iter (txtModel, range.Start(nil) + replaceString->Length());				
					TextIterator charBefore = iter; 
					if(iter.Position() > 0)
						charBefore = iter-1;				

					while(*iter == kTextChar_Space)
					{					
						++nbCharSkipped;
						++iter;
					}

					TextIndex pos = charBefore.Position();
					if( (*iter == kTextChar_CR) 
						&& (pos <= 0 || *charBefore == kTextChar_CR)
						&& (iter.Position() != txtModel->GetPrimaryStoryThreadSpan() - 1))
						++nbCharSkipped;
				}

				if(nbCharSkipped > 0)
				{
					InterfacePtr<ICommand> deleteTxtCmd(modelCmds->DeleteCmd(range.Start(nil) + replaceString->Length(),nbCharSkipped));				
					if((status = CmdUtils::ProcessCommand(deleteTxtCmd)) != kSuccess)
					{
						error = PMString(kErrCmdFailed);
						break;
					}						
				}
			}			

			int32 lengthToCheck = (txtModel->TotalLength()- 1) - (range.Start(nil)+ replaceString->Length());

			textRanges.clear();
			InDesign::TextRange txtRange (txtModel, range.Start(nil) + replaceString->Length(), lengthToCheck);
			textRanges.push_back(txtRange);

			runIter = TextAttributeRunIterator (textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
		}
		else 
			++runIter;
	}

	if(!refFound)
		error = PMString(kErrPermRefsInvalidRef);	

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return status;
}

ErrorCode PermRefsUtils::LookForPermRefs(PermRefStruct ref, IDataBase * db, K2Vector<UID> stories, UID& story, TextIndex& start, TextIndex& end)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);

	do
	{
		InDesign::TextRange txtRange;
		
		if(ref != currentRef || currentStories != stories)
		{
			currentRef = ref;
			currentStories = stories;
			currentStory = 0;			
			startIndex = 0;
		}
	
		bool8 refFound = kFalse;
		while(currentStory < currentStories.size())
		{
			InterfacePtr<ITextModel> txtModel (db, currentStories[currentStory], UseDefaultIID());
			if(txtModel == nil)
				break;

			txtRange = InDesign::TextRange(txtModel, startIndex,  txtModel->TotalLength()-1 - startIndex);	

			// Use TextAttributeRunIterator to find our kPermRefsAttrBoss
			K2Vector<InDesign::TextRange> textRanges;
			textRanges.push_back(txtRange);

			K2Vector<ClassID> attributeClasses;
			attributeClasses.push_back(kPermRefsAttrBoss);

			// Construct the iterator
			TextAttributeRunIterator runIter = TextAttributeRunIterator(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
			while(runIter)
			{
				RangeData range = runIter.GetRunRange();

				InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
				if(currentTag != nil && currentTag->getReference() == ref)
				{	
					refFound = kTrue;
					start = range.Start(nil);
					end = range.End();
					story = currentStories[currentStory];

					startIndex = range.End();
					status = kSuccess;

					break;
				}
				else 
					++runIter;
			}

			if(!refFound)
			{
				currentStory++;
				startIndex = 0;
			}
			else
				break;
		}

		if(!refFound)
		{
			error = PMString(kErrPermRefsInvalidRef);
			startIndex = 0;
			currentStory = 0;
		}
	}
	while(kFalse);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return status;
}

ErrorCode PermRefsUtils::GetPermRefs(ITextModel * txtModel, TextIndex& start, TextIndex& end, K2Vector<PermRefStruct>& listRef)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	do
	{
		GetTagBounds(txtModel, start, end);
		
		InDesign::TextRange txtRange (txtModel, start, end - start);

		// Use TextAttributeRunIterator to find our kPermRefsAttrBoss
		K2Vector<InDesign::TextRange> textRanges;
		textRanges.push_back(txtRange);

		K2Vector<ClassID> attributeClasses;
		attributeClasses.push_back(kPermRefsAttrBoss);

		// Construct the iterator.
		TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
		
		while(runIter)
		{

			InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
			if(currentTag != nil && currentTag->getReference() != kNullPermRef)
			{
				listRef.push_back(currentTag->getReference());
			} 
			++runIter;
		}
	}
	while(kFalse);
	
	status = kSuccess;

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return status;
}

void PermRefsUtils::GetTagBounds(ITextModel * txtModel, TextIndex& leftBound, TextIndex& rightBound)
{
	InterfacePtr<IAttributeStrand> attrStrand ((IAttributeStrand *)txtModel->QueryStrand(kCharAttrStrandBoss,IID_IATTRIBUTESTRAND));
		
	DataWrapper<AttributeBossList> attrList = attrStrand->GetLocalOverrides(leftBound,nil, nil);
	InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) (attrList.get())->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
	if(currentTag != nil && currentTag->getReference() != kNullPermRef)
	{
		PermRefStruct ref = currentTag->getReference();

		bool16 keepOn = (--leftBound >=0);
		while(keepOn)
		{
			attrList = attrStrand->GetLocalOverrides(leftBound,nil, nil);

			currentTag = InterfacePtr<IPermRefsTag>((IPermRefsTag *) (attrList.get())->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
			if(currentTag != nil && currentTag->getReference() == ref)
				keepOn = (--leftBound >=0);
			else							
				keepOn = kFalse;
		}
		++leftBound;
	}

	TextIndex limit = txtModel->TotalLength()- 1;
	attrList = attrStrand->GetLocalOverrides(rightBound,nil, nil);
	currentTag = InterfacePtr<IPermRefsTag>((IPermRefsTag *) (attrList.get())->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
	if(currentTag != nil && currentTag->getReference() != kNullPermRef)
	{
		PermRefStruct ref = currentTag->getReference();

		bool16 keepOn = (++rightBound <= limit);
		while(keepOn)
		{
			attrList = attrStrand->GetLocalOverrides(rightBound,nil, nil);

			currentTag = InterfacePtr<IPermRefsTag>((IPermRefsTag *) (attrList.get())->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
			if(currentTag != nil && currentTag->getReference() == ref)
			{
				keepOn = (++rightBound <= limit);
			}
			else
				keepOn = kFalse;
		}

		if(rightBound > limit)
			rightBound = limit;
	}
}

ErrorCode PermRefsUtils::ExportPermRefs(const IDocument * doc, const IDFile& exportFile)
{
	ErrorCode status = kFailure;
	do
	{
		// Open file stream
		InterfacePtr<IPMStream> exportStream(StreamUtil::CreateFileStreamWrite(exportFile, kOpenOut|kOpenTrunc, kTextType,  kAnyCreator));
		if (exportStream == nil) 
			break;
		
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		InterfacePtr<ISectionList> sectionList (doc, UseDefaultIID());

		// Iterate story list
		InterfacePtr<IStoryList> storyList (doc, UseDefaultIID());
		
		int32 nbStories = storyList->GetUserAccessibleStoryCount();
		
		for(int32 i = 0 ; i < nbStories ; ++i)
		{
			InterfacePtr<ITextModel> txtModel (storyList->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			InDesign::TextRange txtRange (txtModel, 0,  txtModel->TotalLength()-1);

			InterfacePtr<IFrameList> frameList (txtModel->QueryFrameList());

			// Use TextAttributeRunIterator to find our kPermRefsAttrBoss
			K2Vector<InDesign::TextRange> textRanges;
			textRanges.push_back(txtRange);

			K2Vector<ClassID> attributeClasses;
			attributeClasses.push_back(kPermRefsAttrBoss);

			// Construct the iterator.
			TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
			
			bool8 refFound = kFalse;
			while(runIter)
			{
				InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
				if(currentTag != nil && currentTag->getReference() != kNullPermRef)
				{
					PermRefStruct permref = currentTag->getReference();	
					
					InterfacePtr<ITextFrameColumn> ownerFrame (frameList->QueryFrameContaining(runIter.GetRunRange().Start(nil),nil));
					
					PMString refInfo("[HD=");
					refInfo.AppendNumber(permref.ref);
					refInfo += ";";
					// Insert page number
					if(ownerFrame)
					{
						InterfacePtr<IHierarchy> ownerFrameHier (ownerFrame, UseDefaultIID());
						refInfo += "\"";
						int32 pageIndex = pageList->GetPageIndex( Utils<ILayoutUtils>()->GetOwnerPageUID(ownerFrameHier));
						if(pageIndex >= 0)
						{
							PMString pageNumber;
							sectionList->GetPageString( Utils<ILayoutUtils>()->GetOwnerPageUID(ownerFrameHier), &pageNumber, kFalse, kFalse);
							refInfo += pageNumber;
						}
						else
							refInfo += "XX";
						refInfo += "\";";
					}
					else
						refInfo += "\"XX\";";
					refInfo.AppendNumber(permref.param1);
					refInfo += ";";
					refInfo.AppendNumber(permref.param2);
					refInfo += ";";
					refInfo.AppendNumber(permref.param3);
					refInfo += ";";
					refInfo.AppendNumber(permref.param4);
					refInfo += "]";
					refInfo += kNewLine;
					refInfo.SetTranslatable(kFalse);
					exportStream->XferByte((uchar *) refInfo.GetPlatformString().c_str(), refInfo.GetPlatformString().size());	
				}
				
				++runIter;
			}
		}
		
		exportStream->Close();
	
		status = kSuccess;
	
	}while(kFalse);
	
	
	return status;
}

ErrorCode PermRefsUtils::ExportPermRefsWithContent(const IDocument * doc, const IDFile& exportFile)
{
	ErrorCode status = kFailure;
	PMString error = PMString(kErrNilInterface);
	do
	{
		// Open file stream
		InterfacePtr<IPMStream> exportStream(StreamUtil::CreateFileStreamWrite(exportFile, kOpenOut|kOpenTrunc, kTextType,  kAnyCreator));
		if (exportStream == nil) 
			break;
		
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		InterfacePtr<ISectionList> sectionList (doc, UseDefaultIID());

		IDataBase * db = ::GetDataBase(doc);
		if (db == nil) 
			break;

		// Iterate story list
		InterfacePtr<IStoryList> storyList (doc, UseDefaultIID());		
		int32 nbStories = storyList->GetUserAccessibleStoryCount();
		
		for(int32 i = 0 ; i < nbStories ; ++i)
		{

			InterfacePtr<ITextModel> txtModel (storyList->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			InDesign::TextRange txtRange (txtModel, 0,  txtModel->TotalLength()-1);

			InterfacePtr<IFrameList> frameList (txtModel->QueryFrameList());

			// Use TextAttributeRunIterator to find our kPermRefsAttrBoss
			K2Vector<InDesign::TextRange> textRanges;
			textRanges.push_back(txtRange);

			K2Vector<ClassID> attributeClasses;
			attributeClasses.push_back(kPermRefsAttrBoss);

			// Construct the iterator.
			TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
			
			bool8 refFound = kFalse;
			while(runIter)
			{

				// Export Perms ref with parameters
				InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
				if(currentTag != nil && currentTag->getReference() != kNullPermRef)
				{

					PermRefStruct permref = currentTag->getReference();	

					InterfacePtr<ITextFrameColumn> ownerFrame (frameList->QueryFrameContaining(runIter.GetRunRange().Start(nil),nil));
					PMString refInfo("[HD=");
					refInfo.AppendNumber(permref.ref);
					refInfo += ";";
					
					// Insert page number
					if(ownerFrame)
					{
						InterfacePtr<IHierarchy> ownerFrameHier (ownerFrame, UseDefaultIID());
						refInfo += "\"";
						int32 pageIndex = pageList->GetPageIndex( Utils<ILayoutUtils>()->GetOwnerPageUID(ownerFrameHier));
						if(pageIndex >= 0)
						{
							PMString pageNumber;
							sectionList->GetPageString( Utils<ILayoutUtils>()->GetOwnerPageUID(ownerFrameHier), &pageNumber, kFalse, kFalse);
							refInfo += pageNumber;
						}
						else
							refInfo += "XX";

						refInfo += "\";";
					}
					else
						refInfo += "\"XX\";";
									
					refInfo.AppendNumber(permref.ref);
					refInfo += ";";
					refInfo.AppendNumber(permref.param1);
					refInfo += ";";
					refInfo.AppendNumber(permref.param2);
					refInfo += ";";
					refInfo.AppendNumber(permref.param3);
					refInfo += ";";
					refInfo.AppendNumber(permref.param4);
					refInfo += "]";

					// Range of permRef text Content
					RangeData range = runIter.GetRunRange();

					// Begin and end of text index
					TextIndex rangeStart = range.Start(nil);
					TextIndex rangeEnd = range.End();
					TextIndex pos = 0;
					int32 runLength = 0;
					
					TextIterator beginIter(txtModel, rangeStart);
					TextIterator endIter(txtModel, rangeEnd);
					
					WideString text;
					// Iterate text selection to filter out some characters
					for (TextIterator iter = beginIter; iter != endIter; iter++) 
						text.Append(*iter);					 		
					
					// Convert WideString to PMString
					refInfo.Append(PMString(text));					
						
					// Fin de commande HD
					refInfo.Append("[HF]");	

					// New Line for next command if any
					refInfo += kNewLine;					
					refInfo.SetTranslatable(kFalse);
					exportStream->XferByte((uchar *)refInfo.GetPlatformString().c_str(), refInfo.GetPlatformString().size());	
				}	
				++runIter;
			}
		}		
		exportStream->Close();	
		status = kSuccess;
	}while(kFalse);	
	return status;
}
// Helper : ecrit une ligne (ASCII/Latin1) suivie d'un saut de ligne dans le flux.
static void WriteDebugLine(IPMStream * s, PMString text)
{
	text += kNewLine;
	text.SetTranslatable(kFalse);
	s->XferByte((uchar *) text.GetPlatformString().c_str(), text.GetPlatformString().size());
}

ErrorCode PermRefsUtils::ExportTCLReferencesListDebug(const IDocument * doc, const IDFile& exportFile)
{
	ErrorCode status = kFailure;
	do
	{
		if(doc == nil)
			break;

		IDataBase * db = ::GetDataBase(doc);
		if(db == nil)
			break;

		InterfacePtr<ITCLReferencesList> docRefList (doc, UseDefaultIID());
		if(docRefList == nil)
			break;

		InterfacePtr<IPMStream> s(StreamUtil::CreateFileStreamWrite(exportFile, kOpenOut|kOpenTrunc, kTextType, kAnyCreator));
		if(s == nil)
			break;

		// Limite d'affichage detaille par liste. Les compteurs globaux restent
		// exacts ; on borne juste le detail pour ne pas produire un fichier
		// gigantesque (ni saturer la memoire) sur un document deja corrompu.
		const int32 kMaxDump = 2000;

		RefVector		itemList  = docRefList->GetItemList();
		PermRefVector	storyList = docRefList->GetStoryList();
		TableVector		tableList = docRefList->GetTableList();

		PMString docName;
		const_cast<IDocument*>(doc)->GetName(docName);

		WriteDebugLine(s, PMString("=== TCL References List - DEBUG DUMP ==="));
		{ PMString l("Document: "); l.Append(docName); WriteDebugLine(s, l); }

		int32 i;

		// ---- itemList ----
		WriteDebugLine(s, PMString(""));
		{ PMString l("--- itemList (TCLRef bloc -> UID)   count="); l.AppendNumber((int32)itemList.size()); WriteDebugLine(s, l); }
		int32 itemZero = 0, itemInvalid = 0;
		for(i = 0 ; i < (int32)itemList.size() ; ++i)
		{
			TCLRef ref = itemList[i].Key();
			UID    uid = itemList[i].Value();
			bool16 isZero = (ref == kZeroTCLRef);
			bool16 isInvalid = !db->IsValidUID(uid);
			if(isZero) itemZero++;
			if(isInvalid) itemInvalid++;
			if(i < kMaxDump)
			{
				PMString l("  ["); l.AppendNumber(i); l.Append("] ref="); l.AppendNumber((int32)ref);
				l.Append(" uid="); l.AppendNumber((int32)uid.Get());
				if(isZero) l.Append(" [ZERO]");
				if(isInvalid) l.Append(" [INVALID_UID]");
				WriteDebugLine(s, l);
			}
		}
		{ PMString l("  zeroRefCount="); l.AppendNumber(itemZero); l.Append("  invalidUidCount="); l.AppendNumber(itemInvalid); WriteDebugLine(s, l); }

		// ---- storyList ----
		WriteDebugLine(s, PMString(""));
		{ PMString l("--- storyList (PermRef -> stories)   count="); l.AppendNumber((int32)storyList.size()); WriteDebugLine(s, l); }
		int32 totalStoryUid = 0, invalidStoryUid = 0;
		for(i = 0 ; i < (int32)storyList.size() ; ++i)
		{
			PermRefStruct pr = storyList[i].Key();
			K2Vector<UID> uids = storyList[i].Value();
			totalStoryUid += (int32)uids.size();
			int32 invalidHere = 0, j;
			for(j = 0 ; j < (int32)uids.size() ; ++j)
				if(!db->IsValidUID(uids[j])) { invalidHere++; invalidStoryUid++; }
			if(i < kMaxDump)
			{
				PMString l("  ["); l.AppendNumber(i); l.Append("] ref="); l.AppendNumber((int32)pr.ref);
				l.Append(" p1="); l.AppendNumber((int32)pr.param1);
				l.Append(" p2="); l.AppendNumber((int32)pr.param2);
				l.Append(" p3="); l.AppendNumber((int32)pr.param3);
				l.Append(" p4="); l.AppendNumber((int32)pr.param4);
				l.Append(" nbStories="); l.AppendNumber((int32)uids.size());
				if(invalidHere > 0) { l.Append(" invalidUid="); l.AppendNumber(invalidHere); }
				WriteDebugLine(s, l);
			}
		}
		{ PMString l("  totalStoryUidRefs="); l.AppendNumber(totalStoryUid); l.Append("  invalidStoryUidCount="); l.AppendNumber(invalidStoryUid); WriteDebugLine(s, l); }

		// ---- tableList ----
		WriteDebugLine(s, PMString(""));
		{ PMString l("--- tableList (table ref -> UID)   count="); l.AppendNumber((int32)tableList.size()); WriteDebugLine(s, l); }
		int32 tableInvalid = 0;
		for(i = 0 ; i < (int32)tableList.size() ; ++i)
		{
			PMString tref = tableList[i].Key();
			UID uid = tableList[i].Value();
			bool16 isInvalid = !db->IsValidUID(uid);
			if(isInvalid) tableInvalid++;
			if(i < kMaxDump)
			{
				PMString l("  ["); l.AppendNumber(i); l.Append("] ref=");
				tref.SetTranslatable(kFalse);
				l.Append(tref);
				l.Append(" uid="); l.AppendNumber((int32)uid.Get());
				if(isInvalid) l.Append(" [INVALID_UID]");
				WriteDebugLine(s, l);
			}
		}
		{ PMString l("  invalidUidCount="); l.AppendNumber(tableInvalid); WriteDebugLine(s, l); }

		// ---- verite terrain : scan reel des tags kPermRefsAttrBoss ----
		WriteDebugLine(s, PMString(""));
		WriteDebugLine(s, PMString("--- ACTUAL tags scan (kPermRefsAttrBoss) ---"));
		PermRefVector realStoryList;
		int32 totalTagRuns = 0;
		InterfacePtr<IStoryList> sl (doc, UseDefaultIID());
		int32 nbStories = (sl != nil) ? sl->GetUserAccessibleStoryCount() : 0;
		for(i = 0 ; i < nbStories ; ++i)
		{
			InterfacePtr<ITextModel> txtModel (sl->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			if(txtModel == nil)
				continue;
			int32 total = txtModel->TotalLength();
			if(total <= 0)
				continue;

			K2Vector<InDesign::TextRange> textRanges;
			textRanges.push_back(InDesign::TextRange(txtModel, 0, total - 1));
			K2Vector<ClassID> attributeClasses;
			attributeClasses.push_back(kPermRefsAttrBoss);
			TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
			while(runIter)
			{
				InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
				if(currentTag != nil && currentTag->getReference() != kNullPermRef)
					totalTagRuns++;
				++runIter;
			}

			UpdatePermRefsList(realStoryList, txtModel, 0, total - 1);
		}
		{ PMString l("storiesScanned="); l.AppendNumber(nbStories); WriteDebugLine(s, l); }
		{ PMString l("totalTagRuns="); l.AppendNumber(totalTagRuns); WriteDebugLine(s, l); }
		{ PMString l("distinctPermRefs (rebuilt storyList size)="); l.AppendNumber((int32)realStoryList.size()); WriteDebugLine(s, l); }

		WriteDebugLine(s, PMString(""));
		WriteDebugLine(s, PMString("=== INTERPRETATION ==="));
		WriteDebugLine(s, PMString("Si storyList.count >> distinctPermRefs, ou si invalidUidCount est"));
		WriteDebugLine(s, PMString("eleve, l'index persiste est corrompu/gonfle. Lancer alors"));
		WriteDebugLine(s, PMString("'Debug : reparer l'index des references' puis enregistrer le document."));

		s->Close();
		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return status;
}

ErrorCode PermRefsUtils::RepairTCLReferencesList(const IDocument * doc, PMString& report)
{
	ErrorCode status = kFailure;
	do
	{
		if(doc == nil)
			break;

		IDataBase * db = ::GetDataBase(doc);
		if(db == nil)
			break;

		InterfacePtr<ITCLReferencesList> docRefList (doc, UseDefaultIID());
		if(docRefList == nil)
			break;

		RefVector		oldItemList  = docRefList->GetItemList();
		PermRefVector	oldStoryList = docRefList->GetStoryList();
		TableVector		oldTableList = docRefList->GetTableList();

		int32 oldItemCount  = (int32)oldItemList.size();
		int32 oldStoryCount = (int32)oldStoryList.size();
		int32 oldTableCount = (int32)oldTableList.size();

		// 1) Reconstruire storyList a partir de la verite terrain : les vrais tags
		//    kPermRefsAttrBoss poses dans les articles. C'est la source fiable, on
		//    n'accorde aucune confiance au storyList persiste (potentiellement corrompu).
		PermRefVector newStoryList;
		InterfacePtr<IStoryList> sl (doc, UseDefaultIID());
		int32 nbStories = (sl != nil) ? sl->GetUserAccessibleStoryCount() : 0;
		for(int32 i = 0 ; i < nbStories ; ++i)
		{
			InterfacePtr<ITextModel> txtModel (sl->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			if(txtModel == nil)
				continue;
			int32 total = txtModel->TotalLength();
			if(total <= 0)
				continue;
			UpdatePermRefsList(newStoryList, txtModel, 0, total - 1);
		}

		// 2) Purger itemList : on garde les entrees a reference non nulle ET UID valide.
		RefVector newItemList;
		for(RefVector::iterator it = oldItemList.begin(); it < oldItemList.end(); ++it)
			if(it->Key() != kZeroTCLRef && db->IsValidUID(it->Value()))
				newItemList.push_back(*it);

		// 3) Purger tableList : on garde les entrees a UID valide.
		TableVector newTableList;
		for(TableVector::iterator it = oldTableList.begin(); it < oldTableList.end(); ++it)
			if(db->IsValidUID(it->Value()))
				newTableList.push_back(*it);

		// 4) Ecrire les listes nettoyees via la commande dediee (annulable).
		UIDList docToSave(::GetUIDRef(const_cast<IDocument*>(doc)));
		InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
		if(saveItemsCmd == nil)
			break;

		saveItemsCmd->SetUndoability(ICommand::kAutoUndo);
		saveItemsCmd->SetItemList(docToSave);

		InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());
		if(cmdData == nil)
			break;

		cmdData->SetItemList(newItemList);
		cmdData->SetStoryList(newStoryList);
		cmdData->SetTableList(newTableList);

		if(CmdUtils::ProcessCommand(saveItemsCmd) != kSuccess)
			break;

		report = PMString("");
		report.SetTranslatable(kFalse);
		report.Append("itemList: ");  report.AppendNumber(oldItemCount);  report.Append(" -> "); report.AppendNumber((int32)newItemList.size());
		report.Append("\nstoryList: "); report.AppendNumber(oldStoryCount); report.Append(" -> "); report.AppendNumber((int32)newStoryList.size());
		report.Append("\ntableList: "); report.AppendNumber(oldTableCount); report.Append(" -> "); report.AppendNumber((int32)newTableList.size());

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(kFailure);

	return status;
}

bool8 PermRefsUtils::UpdatePermRefsList(PermRefVector& permrefsList, ITextModel * txtModel, TextIndex start, TextIndex end)
{
	bool8 refFound = kFalse;
	
	K2Vector<InDesign::TextRange> textRanges;
	textRanges.push_back(InDesign::TextRange(txtModel, start, end-start));
		
	K2Vector<ClassID> attributeClasses;
	attributeClasses.push_back(kPermRefsAttrBoss);			

	UID modelUID = ::GetUID(txtModel);

	// Construct the iterator
	TextAttributeRunIterator runIter = TextAttributeRunIterator(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
	while(runIter)
	{
		InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
		if(currentTag != nil && currentTag->getReference() != kNullPermRef)			
		{
			refFound = kTrue;

			PermRefStruct thePermRef = currentTag->getReference();

			int32 pos = ::FindLocation(permrefsList, thePermRef);
			// Check whether ref already exists
			if(pos != -1)
			{
				K2Vector<UID>& storiesUID = permrefsList[pos].Value();
				
				K2Vector<UID>::iterator iter = ::K2find(storiesUID.begin(), storiesUID.end(),modelUID);
				// Check whether story already exists, if not add to the end of the list
				if(iter == storiesUID.end())
					storiesUID.push_back(modelUID);					
			}
			else
			{
				// New ref, add it to the list
				K2Vector<UID> storiesUID;
				storiesUID.push_back(modelUID);
				::InsertKeyValue(permrefsList,thePermRef,storiesUID);
			}
		}

		++runIter;
	}	

	return refFound;
}

bool16 PermRefsUtils::IsAnyTagInRange(ITextModel *model,const TextIndex& position, const int32& length, PermRefStruct& permref)
{
	K2Vector<ClassID> attributeClasses;
	attributeClasses.push_back(kPermRefsAttrBoss);

	K2Vector<InDesign::TextRange> textRanges;
	textRanges.push_back(InDesign::TextRange(model, position, length));

	TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());

	bool16 found = kFalse;
	while(runIter && !found)
	{					
		InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
		if(currentTag != nil && currentTag->getReference() != kNullPermRef)
		{
			found = kTrue;
			permref = currentTag->getReference();
		}

		++runIter;
	}

	return found;
}

bool16 PermRefsUtils::IsSameTagInRange(ITextModel *model,const TextIndex& position, const int32& length, const PermRefStruct& permref)
{
	K2Vector<ClassID> attributeClasses;
	attributeClasses.push_back(kPermRefsAttrBoss);

	K2Vector<InDesign::TextRange> textRanges;
	textRanges.push_back(InDesign::TextRange(model, position, length));

	TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());

	bool16 found = kFalse;
	while(runIter && !found)
	{					
		InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
		if(currentTag != nil && currentTag->getReference() == permref)
			found = kTrue;
		
		++runIter;
	}

	return found;
}

ErrorCode PermRefsUtils::CopyContiguousRef(ITextModel *model, const PermRefStruct& permref, const int32& nbCopy, 
										   const textchar& separator, const int32& nbSep, PermRefVector& permRefsList)
{
	ErrorCode status = kFailure;

	do
	{
		// Start iterating permref tags in the story
		K2Vector<ClassID> attributeClasses;
		attributeClasses.push_back(kPermRefsAttrBoss);

		K2Vector<InDesign::TextRange> textRanges;
		textRanges.push_back(InDesign::TextRange(model, 0, model->TotalLength()-1));

		TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
		
		TextIndex startCopy = kInvalidTextIndex, endCopy = kInvalidTextIndex;
		while(runIter)
		{					
			InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
			if(currentTag != nil)
			{
				PermRefStruct currentRef = currentTag->getReference();
				if( (permref.param1 == currentRef.param1) &&
					(permref.param2	== 0 || permref.param2 == currentRef.param2) &&
					(permref.param3	== 0 || permref.param3 == currentRef.param3) &&
					(permref.param4	== 0 || permref.param4 == currentRef.param4)
				   )
				{
					RangeData range = runIter.GetRunRange();
					if(startCopy == kInvalidTextIndex)
					{
						// Found first matching tag
						startCopy = range.Start(nil);
						endCopy = range.End();
					}
					else
					{
						// Found another contiguous mathching tag
						endCopy = range.End();
					}

					// Update PermRefs attribute to set entity index (last parameter)
					PermRefStruct firstEntity = currentRef;
					if(permref.param2 == 0)
						firstEntity.param1 += 1;
					else if(permref.param3 == 0)
						firstEntity.param2 += 1;
					else if(permref.param4 == 0)
						firstEntity.param3 += 1;
					else
						firstEntity.param4 += 1;

					ApplyPermRefs(firstEntity, model, range.Start(nil), range.End());
				}	
				else
				{
					if(currentRef.ref != 0 && startCopy != kInvalidTextIndex) // Found a non-matching tag after having found some others that matches,
													   // stop research now
						break;						
				}	
			}

			++runIter;
		}

		status = kSuccess;
		TextIndex startCopySave = startCopy;

		if(startCopy != kInvalidTextIndex)
		{
			int32 copyLength = endCopy - startCopy;
			InterfacePtr<ITextModelCmds> txtModelCmds (model, UseDefaultIID());			

			for(int32 i = 0 ; i < nbCopy ; ++i)
			{
				// Append separator
				boost::shared_ptr< WideString > separatorText (new WideString());
				for(int32 j = 0 ; j < nbSep ; ++j)
					separatorText->Append(separator);

				InterfacePtr<ICommand> insertCmd (txtModelCmds->InsertCmd(endCopy, separatorText));
				if(CmdUtils::ProcessCommand(insertCmd) != kSuccess)
				{
					status = kFailure;
					break;
				}

				TextIndex startDest = endCopy + nbSep;				
				
				// Copy range of tagged text that matches the filter
				InterfacePtr<ICommand> copyStoryRangeCmd(CmdUtils::CreateCommand(kCopyStoryRangeCmdBoss));
				
				// Refer the command to the source story and range to be copied.
				InterfacePtr<IUIDData> sourceUIDData(copyStoryRangeCmd, UseDefaultIID());
				sourceUIDData->Set(model);

				InterfacePtr<IRangeData> sourceRangeData(copyStoryRangeCmd, UseDefaultIID());
				sourceRangeData->Set(startCopy, endCopy);

				// Refer the command to the destination story and the range to be replaced.
				copyStoryRangeCmd->SetItemList(UIDList(model));
				InterfacePtr<IRangeData> destRangeData(copyStoryRangeCmd, IID_IRANGEDATA2);
				destRangeData->Set(startDest, startDest);

				if(CmdUtils::ProcessCommand(copyStoryRangeCmd) != kSuccess)
				{
					status = kFailure;
					break;
				}
				
				startCopy = startDest;
				endCopy = startCopy + copyLength;

				// Update PermRefs attribute to set entity index (last parameter)				
				textRanges.clear();
				textRanges.push_back(InDesign::TextRange(model, startCopy, endCopy-startCopy));
				runIter = TextAttributeRunIterator (textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
	
				while(runIter)
				{					
					InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
					if(currentTag != nil)
					{
						PermRefStruct currentRef = currentTag->getReference();
						if(currentRef != kNullPermRef)
						{
							RangeData range = runIter.GetRunRange();
							PermRefStruct nthEntity = currentRef;							
							if(permref.param2 == 0)
								nthEntity.param1 += 1;
							else if(permref.param3 == 0)
								nthEntity.param2 += 1;
							else if(permref.param4 == 0)
								nthEntity.param3 += 1;
							else
								nthEntity.param4 += 1;								

							ApplyPermRefs(nthEntity, model, range.Start(nil), range.End());
						}						
					}
					++runIter;
				}
			}
		}	

		// Gather all updated permrefs
		UpdatePermRefsList(permRefsList, model, startCopySave, endCopy);
	
	} while(kFalse);
	
	return status;
}

PMReal PermRefsUtils::GetHeight(PermRefStruct ref, ITextModel * txtModel)
{
	PMReal height = 0;
	IDataBase * db = ::GetDataBase(txtModel);

	// Get ITextParcelList interface on kFrameListBoss, used to compute text height
	InterfacePtr<ITextParcelList> txtParcelList (db, txtModel->GetFrameListUID(), UseDefaultIID());
	InterfacePtr<IParcelList> parcelList (txtParcelList, UseDefaultIID());
	InterfacePtr<IWaxStrand> waxStrand(txtParcelList, UseDefaultIID());
	
	// Recompose story before if necessary to get an exact height
	ParcelKey firstDamagedParcelKey = txtParcelList->GetFirstDamagedParcel();
	if (firstDamagedParcelKey.IsValid() == kTrue)
	{
		InterfacePtr<ITextParcelListComposer> textParcelListComposer(txtParcelList, UseDefaultIID());		
		textParcelListComposer->RecomposeThruTextIndex(kInvalidParcelIndex);
	}	
	
	// Use TextAttributeRunIterator to find our kPermRefsAttrBoss
	InDesign::TextRange txtRange (txtModel, 0,  txtModel->TotalLength()-1);
	K2Vector<InDesign::TextRange> textRanges;
	textRanges.push_back(txtRange);

	K2Vector<ClassID> attributeClasses;
	attributeClasses.push_back(kPermRefsAttrBoss);

	// Construct the iterator.
	TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
	
	while(runIter)
	{		
		InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
		if(currentTag != nil && currentTag->getReference() == ref)
		{						
			// Get all parcels that display this reference
			RangeData refRange = runIter.GetRunRange();
			ParcelKey startParcel, endParcel;
			txtParcelList->GetParcelsContaining(refRange, &startParcel, &endParcel);

			if(!startParcel.IsValid() || !endParcel.IsValid())
			{
				 // It means that a piece of tagged text is overset at least
				 // we cannot compute the exact height, so we reset it to zero and quit
				height = 0;
				break;
			}

			int32 currentParcelIndex = parcelList->GetParcelIndex(startParcel);
			int32 endParcelIndex = parcelList->GetParcelIndex(endParcel);

			// Compute height of each parcel inspecting waxlines
			K2::scoped_ptr<IWaxIterator> firstIterator(waxStrand->NewWaxIterator());
			K2::scoped_ptr<IWaxIterator> lastIterator(waxStrand->NewWaxIterator());
		
			do 
			{			
				TextIndex start = txtParcelList->GetTextStart(startParcel);
				int32 span = txtParcelList->GetTextSpan(startParcel);
				if (span <= 0)
					continue;
				
				TextIndex end = start + span - 1;

				if(start < refRange.Start(nil))					
					start = refRange.Start(nil);					
				
				if(end > refRange.End())
					end = refRange.End();

				// Get the wax lines that display the first and last characters in the parcel.
				IWaxLine* firstLine = firstIterator->GetFirstWaxLine(start);
				IWaxLine* lastLine = lastIterator->GetFirstWaxLine(end);    
			
				if (firstLine == nil || lastLine == nil) {
					continue;
				}

				// Estimate the depth of the text.  
				height += lastLine->GetYPosition() - (firstLine->GetYPosition() - firstLine->GetTOFLineHeight());

				// Go to next parcel
				startParcel = parcelList->GetNextParcelKey(startParcel);

				// Compare parcel index to check whether we inspected all parcel
				if(startParcel.IsValid())
					currentParcelIndex = parcelList->GetParcelIndex(startParcel);
				else // startParcel was the last parcel, end loop
					currentParcelIndex = endParcelIndex + 1;

			} while(currentParcelIndex  <= endParcelIndex);
		}

		++runIter;
	}
	return height;
}

PMString PermRefsUtils::GetTextAttribute(ClassID attrID, const IPMUnknown * inter){

	PMString command = kNullString;
	int32 valTJ = 0,valTG = 0;

	if(attrID == kTextAttrPositionModeBoss){

		InterfacePtr<ITextAttrPositionMode> txtAttrPos(inter, IID_ITEXTATTRPOSITIONMODE );
		
		IDrawingStyle::PositionMode mode = IDrawingStyle::kPosNormal;
		if(txtAttrPos != nil)
		{
			mode = txtAttrPos->GetMode();
			txtAttrPos->Release();
		}

		if(mode == IDrawingStyle::kPosNormal)
			valTJ += 512;
		
		else
		{
			if(mode == IDrawingStyle::kPosSuperscript)
				valTG += 512;
		
			else if(mode == IDrawingStyle::kPosSubscript)
				valTG += 1024;

			else if(mode == IDrawingStyle::kPosSuperior)
				valTG += 2048;

			else if(mode == IDrawingStyle::kPosInferior)
				valTG += 4096;
		}
		command += "[TG=";
		command.AppendNumber(valTG);
		command += "]";
	}
    else if(attrID == kTextAttrCapitalModeBoss){

		InterfacePtr<ITextAttrCapitalMode> txtAttrCapital(inter, IID_ITEXTATTRCAPITALMODE );
		IDrawingStyle::CapitalizeMode mode = IDrawingStyle::kCapNormal;

		if(txtAttrCapital != nil)
		{
			mode = txtAttrCapital->GetMode();
			txtAttrCapital->Release();
		}

		if(mode == IDrawingStyle::kCapNormal)
			valTJ += 128;
		
		else
		{
			if(mode == IDrawingStyle::kCapAll)
				valTG += 128;
			
			else if(mode == IDrawingStyle::kCapSmallLowercase)
				valTG += 256;	 
		}
		command += "[TG=";
		command.AppendNumber(valTG);
		command += "]";
	}
	return command;
}

ErrorCode PermRefsUtils::DeleteAllPermRefs(const IDocument * doc)
{
	ErrorCode status = kFailure;
	PMString error = PMString(kErrNilInterface);
	do{

		// Iterate story list
		InterfacePtr<IStoryList> storyList (doc, UseDefaultIID());		
		int32 nbStories = storyList->GetUserAccessibleStoryCount();

		for(int32 i = 0 ; i < nbStories ; ++i){

			InterfacePtr<ITextModel> txtModel (storyList->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			InDesign::TextRange txtRange (txtModel, 0,  txtModel->TotalLength()-1);

			InterfacePtr<IFrameList> frameList (txtModel->QueryFrameList());

			// Use TextAttributeRunIterator to find our kPermRefsAttrBoss
			K2Vector<InDesign::TextRange> textRanges;
			textRanges.push_back(txtRange);

			K2Vector<ClassID> attributeClasses;
			attributeClasses.push_back(kPermRefsAttrBoss);

			// Construct the iterator.
			TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
			
			bool8 refFound = kFalse;
			while(runIter){

				// Get Range Data from TextAttribute
				RangeData range = runIter.GetRunRange();

				// Export Perms ref with parameters
				InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
				if(currentTag){
					InterfacePtr<ICommand> clearCmd(Utils<ITextAttrUtils>()->BuildClearTextAttrCmd(txtModel, range, kPermRefsAttrBoss, kCharAttrStrandBoss));
					if((status = CmdUtils::ProcessCommand(clearCmd)) != kSuccess){
						error = PMString(kErrCmdFailed);
						break;
					}
				}
				++runIter;
			}
		}		
		status = kSuccess;
	
	}while(kFalse);	
	return status;
}

ErrorCode PermRefsUtils::AutoTaggingDocumentPermRefs(const IDocument * doc)
{
	ErrorCode status = kFailure;
	PMString error = PMString(kErrNilInterface);
	do{			
		// Iterate story list
		InterfacePtr<IStoryList> storyList (doc, UseDefaultIID());		
		int32 nbStories = storyList->GetUserAccessibleStoryCount();	
		
		for(int32 i = 0 ; i < nbStories ; ++i){			
			// Inspect TextModel if is it
			InterfacePtr<ITextModel> txtModel (storyList->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			if(txtModel == nil)
				continue;			
		
			InterfacePtr<ITextStrand> txtStrand (static_cast<ITextStrand* >(txtModel->QueryStrand(kTextDataStrandBoss, IID_ITEXTSTRAND)));
			if(txtStrand == nil)
				continue;	 		
			
			WideString text;
			TextIndex index = 0;					
			while(index < txtModel->TotalLength()){

				// Get a "chunk" of text (text can be divided in several chunk)
				int32 length;
				DataWrapper<textchar> chunk = txtStrand->FindChunk(index,&length);
				int32 indexChunk = 0;  				
				while(indexChunk < length){ 
					text.Append(chunk[indexChunk]);					
					indexChunk++;
				}
				index += length;	
			}

			PMString allContentText(text); 
	
			// Regex Process
			std::string target(allContentText.GetPlatformString()); 
			std::string::const_iterator start, end;
			start = target.begin();
			end = target.end();	
			boost::smatch iter;
			boost::u32regex expression = boost::make_u32regex(AUTO_TAGGING_EXPRESSION);
			boost::match_flag_type flags = boost::match_default;

			while(boost::u32regex_search(start, end, iter, expression, flags)){

				int32 numberOfCharsStripped;

				WideString resultCoded (iter.str().c_str());
				PMString regexResultStripped = StripString(PMString(resultCoded), numberOfCharsStripped);

				// Separer la reference et le prix par une espace
				// La longueur de la reference est parametrable (8 caracteres pour un premier temps)

				regexResultStripped.Insert(kTextChar_Space, REFERENCE_LENGTH);
				regexResultStripped.Insert(kTextChar_Space, regexResultStripped.NumUTF16TextChars()- 1);

				K2::scoped_ptr<PMString> refString (regexResultStripped.GetItem(" ", 1));
				K2::scoped_ptr<PMString> PriceString (regexResultStripped.GetItem(" ", 2));

				if(refString || PriceString){
					
					int32 refIndexStart = allContentText.IndexOfString(*refString);
					int32 refIndexEnd = refIndexStart + REFERENCE_LENGTH + numberOfCharsStripped;					
					
					PMString::ConversionError convInfo = PMString::kNoError;
					int32 refAsNumber =  refString->GetAsNumber(&convInfo);

					if(convInfo == PMString::kNoError){
						
						PermRefStruct permRefStruct;	

						permRefStruct.ref = refAsNumber;
						permRefStruct.param1 = 0;
						permRefStruct.param2 = 0;
						permRefStruct.param3 = 0;
						permRefStruct.param4 = 0;

						ApplyPermRefs(permRefStruct, txtModel, refIndexEnd, refIndexEnd + (*PriceString).NumUTF16TextChars());
					}
				}
				start = iter[0].second;			
			 }		
		}

		status = kSuccess;
	}while(kFalse);		
	return status;

}
PMString PermRefsUtils::StripString(const PMString& string, int32& numberOfCharsStripped  )
{
	// Strip Some caracters	
	int32 numberTemp = 0;
	WideString resultRegexStriped (string);

	numberTemp = resultRegexStriped.Strip(kTextChar_WhiteSpace);
	numberTemp = numberTemp + resultRegexStriped.Strip(kTextChar_Tab);
	numberTemp = numberTemp + resultRegexStriped.Strip(kTextChar_Space);
	numberTemp = numberTemp + resultRegexStriped.Strip(kTextChar_ZeroSpaceNoBreak);
	numberOfCharsStripped = numberTemp;

	return PMString(resultRegexStriped);
}


ErrorCode PermRefsUtils::AutoTaggingDocumentWithReferencePermRefs(const IDocument * doc)
{
	ErrorCode status = kFailure;
	PMString error = PMString(kErrNilInterface);
	do{			
		// Iterate story list
		InterfacePtr<IStoryList> storyList (doc, UseDefaultIID());		
		int32 nbStories = storyList->GetUserAccessibleStoryCount();	
		
		for(int32 i = 0 ; i < nbStories ; ++i){			
			// Inspect TextModel if is it
			InterfacePtr<ITextModel> txtModel (storyList->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			if(txtModel == nil)
				continue;			
		
			InterfacePtr<ITextStrand> txtStrand (static_cast<ITextStrand* >(txtModel->QueryStrand(kTextDataStrandBoss, IID_ITEXTSTRAND)));
			if(txtStrand == nil)
				continue;	 		
			
			WideString text;
			TextIndex index = 0;					
			while(index < txtModel->TotalLength()){
				// Get a "chunk" of text (text can be divided in several chunk)
				int32 length;
				DataWrapper<textchar> chunk = txtStrand->FindChunk(index,&length);
				int32 indexChunk = 0;  				
				while(indexChunk < length){ 
					text.Append(chunk[indexChunk]);					
					indexChunk++;
				}
				index += length;	
			}

			PMString allContentText(text); 
	
			// Regex Process
			std::string target(allContentText.GetPlatformString()); 
			std::string::const_iterator start, end;
			start = target.begin();
			end = target.end();	
					
			boost::smatch iter;
			boost::u32regex expression = boost::make_u32regex(AUTO_TAGGING_EXPRESSION2);
			boost::match_flag_type flags = boost::match_default;

			while(boost::u32regex_search(start, end, iter, expression, flags)){

				// On supprime les espaces blancs a gauche, a droite et au milieu du resultat
				PMString refString = PMString(iter.str().c_str());

				// Separer la reference et le prix par une espace
				if(refString != kNullString){
					
					// Position de la reference dans le text model
					int32 refIndexStart = allContentText.IndexOfString(refString);
					int32 refIndexEnd = refIndexStart + REFERENCE_LENGTH;					
					
					PMString::ConversionError convInfo = PMString::kNoError;
					int32 refAsNumber = refString.GetAsNumber(&convInfo);

					if(convInfo == PMString::kNoError){
						
						PermRefStruct permRefStruct;	

						permRefStruct.ref = refAsNumber;
						permRefStruct.param1 = 0;
						permRefStruct.param2 = 0;
						permRefStruct.param3 = 0;
						permRefStruct.param4 = 1;

						ApplyPermRefs(permRefStruct, txtModel, refIndexStart, refIndexEnd);
					}
				}
				start = iter[0].second;			
			 }		
		}

		status = kSuccess;
	}while(kFalse);		
	return status;

}

//#define AUTO_TAGGING_EXPRESSION_ILC_V3 "(\\d{3,8})\\s+(0,00|1,11)\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD]+(a,aa)\\s+(b,bb)\\r[\\w\\s\\x80-\\xFD]+(c,cc)\\s+(d,dd)"
//#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE2 "(\\d{3,8})\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD]+(a,aa|b,bb)\\r[\\w\\s\\x80-\\xFD]+(c,cc|d,dd)"
//#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE3 "(\\d{3,8})\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD]+(a,aa|b,bb)"
//#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE4 "(\\d{3,8})\\s+(0,00|1,11)\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD]+(a,aa)\\s+(b,bb)"

// GD 8.0.8 ++
/*
#define AUTO_TAGGING_EXPRESSION_ILC_V3_ICU L"(\\d{3,8})\\s+(0,00|1,11)\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD]+(a,aa)\\s+(b,bb)\\r[\\w\\s\\x80-\\xFD]+(c,cc)\\s+(d,dd)"
#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE2_ICU L"(\\d{3,8})\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD]+(a,aa|b,bb)\\r[\\w\\s\\x80-\\xFD]+(c,cc|d,dd)"
#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE3_ICU L"(\\d{3,8})\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD]+(a,aa|b,bb)"
#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE4_ICU L"(\\d{3,8})\\s+(0,00|1,11)\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD]+(a,aa)\\s+(b,bb)"
#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE5_ICU L"(\\d{3,8})\\s+(0,00|1,11)\\s+(w,ww)\\s+(y,yy|z,zz)"
*/

#define AUTO_TAGGING_EXPRESSION_ILC_V3_ICU L"(\\d{3,8})\\s+(0,00|1,11)\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD\\x28\\x29]+(a,aa)\\s+(b,bb)\\r[\\w\\s\\x80-\\xFD\\x28\\x29]+(c,cc)\\s+(d,dd)"
#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE2_ICU L"(\\d{3,8})\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD\\x28\\x29]+(a,aa|b,bb)\\r[\\w\\s\\x80-\\xFD\\x28\\x29]+(c,cc|d,dd)"
#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE3_ICU L"(\\d{3,8})\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD\\x28\\x29]+(a,aa|b,bb)"
#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE4_ICU L"(\\d{3,8})\\s+(0,00|1,11)\\s+(0,00|1,11)\\r[\\w\\s\\x80-\\xFD\\x28\\x29]+(a,aa)\\s+(b,bb)"
#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE5_ICU L"(\\d{3,8})\\s+(0,00|1,11)\\s+(w,ww)\\s+(y,yy|z,zz)"
#define AUTO_TAGGING_EXPRESSION_ILC_V3_CASE6_ICU L"(\\d{3,8})\\s+(a,aa)\\s+(b,bb)"
// GD 8.0.8 --

ErrorCode PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefs(const IDocument * doc)
{
	ErrorCode status = kFailure;
	PMString error = PMString(kErrNilInterface);
	
	do{			
		// Iterate story list
		InterfacePtr<IStoryList> storyList (doc, UseDefaultIID());		
		int32 nbStories = storyList->GetUserAccessibleStoryCount();	
		
		for(int32 i = 0 ; i < nbStories ; ++i){		
			
			// Inspect TextModel if is it
			InterfacePtr<ITextModel> txtModel (storyList->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			if(txtModel == nil)
				continue;			
			
			InterfacePtr<ITextStrand> txtStrand (static_cast<ITextStrand* >(txtModel->QueryStrand(kTextDataStrandBoss, IID_ITEXTSTRAND)));
			if(txtStrand == nil)
				continue;	 		
			
			WideString text;
			TextIndex index = 0;					
			while(index < txtModel->TotalLength()){
				
				// Get a "chunk" of text (text can be divided in several chunk)
				int32 length;
				DataWrapper<textchar> chunk = txtStrand->FindChunk(index,&length);
				int32 indexChunk = 0;  				
				while(indexChunk < length){ 
					text.Append(chunk[indexChunk]);					
					indexChunk++;
				}
				index += length;	
			}
			
			PMString allContentText(text);
			std::string target(allContentText.GetPlatformString());
			
			std::string::const_iterator start = target.begin();
			std::string::const_iterator end = target.end();	
			
			boost::smatch iter;
			boost::u32regex expression = boost::make_u32regex(AUTO_TAGGING_EXPRESSION_ILC_V2);
			boost::match_flag_type flags = boost::match_default;
			
			while(boost::u32regex_search(start, end, iter, expression, flags)){
				
				PMString result (iter[0].str().c_str());
				int32 resultIndexStart = allContentText.IndexOfString(result);
				
				PMString refString (iter[1].str().c_str());
				PMString prix (iter[2].str().c_str());
				PMString prixSecondaire (iter[3].str().c_str());
				
				if ((refString != kNullString) && (prix != kNullString)) {
					
					// Position de la r￩ference dans le text model
					int32 prixIndexStart = result.IndexOfString(prix);
					int32 prixIndexEnd = prixIndexStart + prix.NumUTF16TextChars();					
					
					PMString::ConversionError convInfo = PMString::kNoError;
					int32 refAsNumber =  refString.GetAsNumber(&convInfo);
					
					if(convInfo == PMString::kNoError){
						
						PermRefStruct permRefStruct;
						
						// GD 5.5.6 ++
						// Il peut n'y avoir qu'un seul prix et qu'il soit a 1,11
						// Note : ne marche pas si une meme ref produit a deux prix tous les deux a 1,11 mais
						// ce cas est normalement impossible car pas de sens de repeter le meme prix.
						if(prix.GetWChar(0) == '1')
							refAsNumber += 100000000;
						// GD 5.5.6 --
						
						permRefStruct.ref = refAsNumber;
						permRefStruct.param1 = 0;
						permRefStruct.param2 = 0;
						permRefStruct.param3 = 0;
						permRefStruct.param4 = 0;
						
						ApplyPermRefs(permRefStruct, txtModel, resultIndexStart + prixIndexStart, resultIndexStart + prixIndexEnd);
						//CAlert::InformationAlert("DEBUG - PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefs STEP 4 inloop\nrefString : "+refString+"\nprix : "+prix+"\nprixSecondaire : "+prixSecondaire);
					}
				}
				
				if ((refString != kNullString) && (prixSecondaire != kNullString)) {
					
					// Position de la r￩ference dans le text model
					int32 prixIndexStart = result.IndexOfString(prixSecondaire);
					int32 prixIndexEnd = prixIndexStart + prixSecondaire.NumUTF16TextChars();					
					
					PMString::ConversionError convInfo = PMString::kNoError;
					int32 refAsNumber = refString.GetAsNumber(&convInfo);
					
					if(convInfo == PMString::kNoError){
						
						PermRefStruct permRefStruct;
						
						if(prixSecondaire.GetWChar(0) == '1')
							refAsNumber += 100000000;
						
						permRefStruct.ref = refAsNumber;
						permRefStruct.param1 = 0;
						permRefStruct.param2 = 0;
						permRefStruct.param3 = 0;
						permRefStruct.param4 = 0;
						
						ApplyPermRefs(permRefStruct, txtModel, resultIndexStart + prixIndexStart, resultIndexStart + prixIndexEnd);
						//CAlert::InformationAlert("DEBUG - PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefs STEP 5 inloop\nrefString : "+refString+"\nprix : "+prix+"\nprixSecondaire : "+prixSecondaire);
					}
				}
				
				start = iter[0].second;			
			}		
		}
		
		status = kSuccess;
	}while(kFalse);
	
	// GD 8.0.3 ++
	// On passe en UNICODE pour s'affranchir du probleme des accents et caracteres speciaux
	PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefsV3(doc, AUTO_TAGGING_EXPRESSION_ILC_V3_ICU);
	PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefsV3(doc, AUTO_TAGGING_EXPRESSION_ILC_V3_CASE2_ICU);
	PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefsV3(doc, AUTO_TAGGING_EXPRESSION_ILC_V3_CASE3_ICU);
	PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefsV3(doc, AUTO_TAGGING_EXPRESSION_ILC_V3_CASE4_ICU);
	// GD 8.0.3 --
	PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefsV3(doc, AUTO_TAGGING_EXPRESSION_ILC_V3_CASE5_ICU);	// GD 8.0.6
	PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefsV3(doc, AUTO_TAGGING_EXPRESSION_ILC_V3_CASE6_ICU);	// GD 8.0.8
	
	return status;
	
}

// GD 8.0.3 ++
//ErrorCode PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefsV3(const IDocument * doc, const char * regexexpression)
ErrorCode PermRefsUtils::ILC_AutoTaggingDocumentWithReferencePermRefsV3(const IDocument * doc, const wchar_t * regexexpression)
// GD 8.0.3 --
{
	ErrorCode status = kFailure;
	PMString error = PMString(kErrNilInterface);
	
	do{			
		// Iterate story list
		InterfacePtr<IStoryList> storyList (doc, UseDefaultIID());		
		int32 nbStories = storyList->GetUserAccessibleStoryCount();	
		
		for(int32 i = 0 ; i < nbStories ; ++i){		
			
			// Inspect TextModel if is it
			InterfacePtr<ITextModel> txtModel (storyList->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			if(txtModel == nil)
				continue;			
			
			InterfacePtr<ITextStrand> txtStrand (static_cast<ITextStrand* >(txtModel->QueryStrand(kTextDataStrandBoss, IID_ITEXTSTRAND)));
			if(txtStrand == nil)
				continue;	 		
			
			WideString text;
			TextIndex index = 0;					
			while(index < txtModel->TotalLength()){
				
				// Get a "chunk" of text (text can be divided in several chunk)
				int32 length;
				DataWrapper<textchar> chunk = txtStrand->FindChunk(index,&length);
				int32 indexChunk = 0;  				
				while(indexChunk < length){ 
					text.Append(chunk[indexChunk]);					
					indexChunk++;
				}
				index += length;	
			}
			
			std::string target;
			StringUtils::ConvertWideStringToUTF8(text, target);
			PMString allContentText(target.c_str());
			 
			std::string::const_iterator start = target.begin();
			std::string::const_iterator end = target.end();
				
			boost::smatch iter;
			boost::u32regex expression = boost::make_u32regex(regexexpression);
			boost::match_flag_type flags = boost::match_default;
			
			while(boost::u32regex_search(start, end, iter, expression, flags)){
				
				PMString result (iter.str().c_str());
				int32 resultIndexStart = allContentText.IndexOfString(result);
				
				PMString refString (iter[1].str().c_str());
				PMString prix (iter[2].str().c_str());
				PMString prixSecondaire (iter[3].str().c_str());
				PMString prixSecondaire2 (iter[4].str().c_str());
				PMString prixSecondaire3 (iter[5].str().c_str());
				PMString prixSecondaire4 (iter[6].str().c_str());
				PMString prixSecondaire5 (iter[7].str().c_str());
				
				if ((refString != kNullString) && (prix != kNullString)) {
					
					// Position de la r￩ference dans le text model
					int32 prixIndexStart = result.IndexOfString(prix);
					int32 prixIndexEnd = prixIndexStart + prix.NumUTF16TextChars();					
					
					PMString::ConversionError convInfo = PMString::kNoError;
					int32 refAsNumber =  refString.GetAsNumber(&convInfo);
					
					if(convInfo == PMString::kNoError){
						
						PermRefStruct permRefStruct;
						
						ILC_ComputeRef(prix, refAsNumber);
						
						permRefStruct.ref = refAsNumber;
						permRefStruct.param1 = 0;
						permRefStruct.param2 = 0;
						permRefStruct.param3 = 0;
						permRefStruct.param4 = 0;
						
						ApplyPermRefs(permRefStruct, txtModel, resultIndexStart + prixIndexStart, resultIndexStart + prixIndexEnd);
					}
				}
				
				if ((refString != kNullString) && (prixSecondaire != kNullString)) {
					
					// Position de la r￩ference dans le text model
					int32 prixIndexStart = result.IndexOfString(prixSecondaire);
					int32 prixIndexEnd = prixIndexStart + prixSecondaire.NumUTF16TextChars();					
					
					PMString::ConversionError convInfo = PMString::kNoError;
					int32 refAsNumber = refString.GetAsNumber(&convInfo);
					
					if(convInfo == PMString::kNoError){
						
						PermRefStruct permRefStruct;
						
						ILC_ComputeRef(prixSecondaire, refAsNumber);
						
						permRefStruct.ref = refAsNumber;
						permRefStruct.param1 = 0;
						permRefStruct.param2 = 0;
						permRefStruct.param3 = 0;
						permRefStruct.param4 = 0;
						
						ApplyPermRefs(permRefStruct, txtModel, resultIndexStart + prixIndexStart, resultIndexStart + prixIndexEnd);		
					}
				}
				
				if ((refString != kNullString) && (prixSecondaire2 != kNullString)) {
					
					// Position de la r￩ference dans le text model
					int32 prixIndexStart = result.IndexOfString(prixSecondaire2);
					int32 prixIndexEnd = prixIndexStart + prixSecondaire2.NumUTF16TextChars();					
					
					PMString::ConversionError convInfo = PMString::kNoError;
					int32 refAsNumber = refString.GetAsNumber(&convInfo);
					
					if(convInfo == PMString::kNoError){
						
						PermRefStruct permRefStruct;
						
						ILC_ComputeRef(prixSecondaire2, refAsNumber);
						
						permRefStruct.ref = refAsNumber;
						permRefStruct.param1 = 0;
						permRefStruct.param2 = 0;
						permRefStruct.param3 = 0;
						permRefStruct.param4 = 0;
						
						ApplyPermRefs(permRefStruct, txtModel, resultIndexStart + prixIndexStart, resultIndexStart + prixIndexEnd);		
					}
				}
				
				if ((refString != kNullString) && (prixSecondaire3 != kNullString)) {
					
					// Position de la r￩ference dans le text model
					int32 prixIndexStart = result.IndexOfString(prixSecondaire3);
					int32 prixIndexEnd = prixIndexStart + prixSecondaire3.NumUTF16TextChars();					
					
					PMString::ConversionError convInfo = PMString::kNoError;
					int32 refAsNumber = refString.GetAsNumber(&convInfo);
					
					if(convInfo == PMString::kNoError){
						
						PermRefStruct permRefStruct;
						
						ILC_ComputeRef(prixSecondaire3, refAsNumber);
						
						permRefStruct.ref = refAsNumber;
						permRefStruct.param1 = 0;
						permRefStruct.param2 = 0;
						permRefStruct.param3 = 0;
						permRefStruct.param4 = 0;
						
						ApplyPermRefs(permRefStruct, txtModel, resultIndexStart + prixIndexStart, resultIndexStart + prixIndexEnd);		
					}
				}
				
				if ((refString != kNullString) && (prixSecondaire4 != kNullString)) {
					
					// Position de la r￩ference dans le text model
					int32 prixIndexStart = result.IndexOfString(prixSecondaire4);
					int32 prixIndexEnd = prixIndexStart + prixSecondaire4.NumUTF16TextChars();					
					
					PMString::ConversionError convInfo = PMString::kNoError;
					int32 refAsNumber = refString.GetAsNumber(&convInfo);
					
					if(convInfo == PMString::kNoError){
						
						PermRefStruct permRefStruct;
						
						ILC_ComputeRef(prixSecondaire4, refAsNumber);
						
						permRefStruct.ref = refAsNumber;
						permRefStruct.param1 = 0;
						permRefStruct.param2 = 0;
						permRefStruct.param3 = 0;
						permRefStruct.param4 = 0;
						
						ApplyPermRefs(permRefStruct, txtModel, resultIndexStart + prixIndexStart, resultIndexStart + prixIndexEnd);		
					}
				}
				
				if ((refString != kNullString) && (prixSecondaire5 != kNullString)) {
					
					// Position de la r￩ference dans le text model
					int32 prixIndexStart = result.IndexOfString(prixSecondaire5);
					int32 prixIndexEnd = prixIndexStart + prixSecondaire5.NumUTF16TextChars();					
					
					PMString::ConversionError convInfo = PMString::kNoError;
					int32 refAsNumber = refString.GetAsNumber(&convInfo);
					
					if(convInfo == PMString::kNoError){
						
						PermRefStruct permRefStruct;
						
						ILC_ComputeRef(prixSecondaire5, refAsNumber);
						
						permRefStruct.ref = refAsNumber;
						permRefStruct.param1 = 0;
						permRefStruct.param2 = 0;
						permRefStruct.param3 = 0;
						permRefStruct.param4 = 0;
						
						ApplyPermRefs(permRefStruct, txtModel, resultIndexStart + prixIndexStart, resultIndexStart + prixIndexEnd);		
					}
				}
				
				start = iter[0].second;			
			}		
		}
		
		status = kSuccess;
	}while(kFalse);		
	return status;
	
}

// GD 8.0.6 ++
void PermRefsUtils::ILC_ComputeRef(PMString codePrix, int32& refAsNumber)
{
	
	if(codePrix.GetWChar(0) == '1')
		refAsNumber += 100000000;
	if(codePrix.GetWChar(0) == 'a')
		refAsNumber += 200000000;
	if(codePrix.GetWChar(0) == 'b')
		refAsNumber += 300000000;
	if(codePrix.GetWChar(0) == 'c')
		refAsNumber += 400000000;
	if(codePrix.GetWChar(0) == 'd')
		refAsNumber += 500000000;
	if(codePrix.GetWChar(0) == 'w')
		refAsNumber += 600000000;
	if(codePrix.GetWChar(0) == 'y')
		refAsNumber += 700000000;
	if(codePrix.GetWChar(0) == 'z')
		refAsNumber += 800000000;
	
}
// GD 8.0.6 --

ErrorCode PermRefsUtils::ILC_ExportPermRefs(const IDocument * doc, const IDFile& exportFile)
{
	ErrorCode status = kFailure;
	do{
		// Open file stream
		InterfacePtr<IPMStream> exportStream(StreamUtil::CreateFileStreamWrite(exportFile, kOpenOut|kOpenTrunc, kTextType,  kAnyCreator));
		if (exportStream == nil) 
			break;		
		
		// Iterate story list
		InterfacePtr<IStoryList> storyList (doc, UseDefaultIID());		
		int32 nbStories = storyList->GetUserAccessibleStoryCount();
		
		for(int32 i = 0 ; i < nbStories ; ++i){
			
			InterfacePtr<ITextModel> txtModel (storyList->GetNthUserAccessibleStoryUID(i), UseDefaultIID());
			InDesign::TextRange txtRange (txtModel, 0,  txtModel->TotalLength()-1);
			
			// Use TextAttributeRunIterator to find our kPermRefsAttrBoss
			K2Vector<InDesign::TextRange> textRanges;
			textRanges.push_back(txtRange);
			
			K2Vector<ClassID> attributeClasses;
			attributeClasses.push_back(kPermRefsAttrBoss);
			
			// Construct the iterator.
			TextAttributeRunIterator runIter(textRanges.begin(), textRanges.end(), attributeClasses.begin(), attributeClasses.end());
			
			bool8 refFound = kFalse;
			while(runIter){
				
				InterfacePtr<IPermRefsTag> currentTag ((IPermRefsTag *) runIter->QueryByClassID(kPermRefsAttrBoss, IID_IPERMREFSTAG));
				if(currentTag != nil && currentTag->getReference() != kNullPermRef)	{
					
					PermRefStruct permref = currentTag->getReference();	
					
					PMString refInfo("[HD=");
					refInfo.AppendNumber(permref.ref);
					refInfo += "]";
					refInfo += kNewLine;
					refInfo.SetTranslatable(kFalse);
					exportStream->XferByte((uchar *) refInfo.GetPlatformString().c_str(), refInfo.GetPlatformString().size());	
				}				
				++runIter;
			}
		}
		
		exportStream->Close();
		
		status = kSuccess;
		
	}while(kFalse);	
	return status;
}
