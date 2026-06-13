/*
//	File:	PermRefsUtils.h
//
//	Date:	27-oct-2005
//
//	Author : Trias
*/

#ifndef __PermRefsUtils_h__
#define __PermRefsUtils_h__

#include "K2SmartPtr.h"
#include "BaseType.h" 
#include "ITCLParser.h"
#include "K2Pair.h"

class ITextModel;
class WideString;
class ITableFrame;

// This class provides different static functions dealing with PermRefs
class PermRefsUtils 
{

public :

	static ErrorCode ApplyPermRefs(PermRefStruct ref, ITextModel * txtModel, TextIndex start, TextIndex end);

	static ErrorCode UpdatePermRefs(PermRefStruct ref, ITextModel * txtModel, boost::shared_ptr< WideString > replaceString, bool16 deleteNextSpaceorCR = kFalse);

	static ErrorCode LookForPermRefs(PermRefStruct ref, IDataBase * db,  K2Vector<UID> stories, UID& story, TextIndex& start, TextIndex& end);
	
	static ErrorCode GetPermRefs(ITextModel * txtModel, TextIndex& start, TextIndex& end, K2Vector<PermRefStruct>& listRef);
	
	static void		 GetTagBounds(ITextModel * txtModel, TextIndex& leftBound, TextIndex& rightBound);
	
	static ErrorCode ExportPermRefs(const IDocument * doc, const IDFile& exportFile);

	static ErrorCode ExportPermRefsWithContent (const IDocument * doc, const IDFile& exportFile);

	// Debug : dumpe l'index de references persiste sur le document
	// (itemList/storyList/tableList) et le compare aux vrais tags trouves dans les
	// articles. Sert a diagnostiquer la corruption / explosion de taille du document.
	static ErrorCode ExportTCLReferencesListDebug (const IDocument * doc, const IDFile& exportFile);

	// Repare l'index de references : reconstruit storyList a partir des vrais tags,
	// et purge des itemList/tableList toute entree a UID invalide ou reference nulle.
	// report recoit un resume avant/apres des tailles de listes.
	static ErrorCode RepairTCLReferencesList (const IDocument * doc, PMString& report);

	static bool8 	 UpdatePermRefsList(PermRefVector& permrefsList, ITextModel * txtModel, TextIndex start, TextIndex end);
	
	static bool16    IsAnyTagInRange(ITextModel *model,const TextIndex& position, const int32& length, PermRefStruct& permref);

	static bool16    IsSameTagInRange(ITextModel *model,const TextIndex& position, const int32& length, const PermRefStruct& permref);

	static ErrorCode CopyContiguousRef(ITextModel *model, const PermRefStruct& permref, const int32& nbCopy, 
									   const textchar& separator, const int32& nbSep, PermRefVector& permRefsList);
	
	static PMReal 	 GetHeight(PermRefStruct ref, ITextModel * txtModel);

	static PMString	 GetTextAttribute(ClassID attrID, const IPMUnknown * inter);

	static ErrorCode DeleteAllPermRefs(const IDocument * doc);

	static ErrorCode AutoTaggingDocumentPermRefs(const IDocument * doc);

	static ErrorCode AutoTaggingDocumentWithReferencePermRefs(const IDocument * doc);

	static PMString  StripString(const PMString& string, int32& numberOfCharsStripped);

	static ErrorCode ILC_ExportPermRefs(const IDocument * doc, const IDFile& exportFile);
	
	static ErrorCode ILC_AutoTaggingDocumentWithReferencePermRefs(const IDocument * doc);
	
	// GD 8.0.3 ++
	//static ErrorCode ILC_AutoTaggingDocumentWithReferencePermRefsV3(const IDocument * doc, const char * regexexpression);
	static ErrorCode ILC_AutoTaggingDocumentWithReferencePermRefsV3(const IDocument * doc, const wchar_t * regexexpression);
	// GD 8.0.3 --
	
	// GD 8.0.6 ++
	static void ILC_ComputeRef(PMString codePrix, int32& refAsNumber);
	// GD 8.0.6 --
	

private :

	static K2Vector<UID> currentStories;
	static PermRefStruct currentRef;
	static int32 currentStory;
	static TextIndex startIndex;

};

#endif //__PermRefsUtils_h__