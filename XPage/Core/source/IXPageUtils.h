
#ifndef __IXPageUtils_h__
#define __IXPageUtils_h__

#include "K2Vector.h"
#include "KeyValuePair.h"
#include "IDocument.h"
#include "IPanelControlData.h"
#include "K2Pair.h"
#include "IXPGPreferences.h"
#include "IXRailPageSlugData.h"
#include "ITextModel.h"
#include "IStoryList.h"
#include "IKeyValueData.h"
#include "ILayerList.h"
#include "IGuidePrefs.h"
#include "ISpreadList.h"
#include "ITextUtils.h"
#include "ISpread.h"
#include "IAssignment.h"
#include "IStyleGroupHierarchy.h"

#include "IPMUnknown.h"
#include "XPGID.h"

#include <map>

typedef KeyValuePair <PMString, PMString> IDValuePair;
typedef K2Vector <IDValuePair> IDValueList;

struct SortAscent
{
	bool operator()(IDValuePair s1, IDValuePair s2)
	{
		PMString s1Tmp = s1.Value();
		PMString s2Tmp = s2.Value();
		
		return s1Tmp < s2Tmp;
	}
};

#ifdef WINDOWS
	#define PLATFORM_PATH_DELIMITER kTextChar_ReverseSolidus
#endif

#ifdef MACINTOSH
	#define PLATFORM_PATH_DELIMITER kTextChar_Colon
#endif

//========================================================================================
// Interface IXPageUtils
//========================================================================================

class IXPageUtils : public IPMUnknown
{
public:

	enum { kDefaultIID = IID_IXPAGEUTILS };
    
	/**
		Get list of subdirectories, returning pairs of (file path, file name)
	*/
	virtual IDValueList GetSubDirectories(const PMString& dirPath) =0;
	
	/** Extract filename from path
	*/
	virtual PMString TruncatePath(const PMString& fromthis) =0;

	/** Import XML article into a target story
	*/
	virtual ErrorCode ImportArticle(UIDList targetStory, IDFile articleFile, IDFile matchingFile, PMString& error, IDFile descriptionFile = IDFile(), PMString articleId = kNullString) =0;
	
	/** Import XML article into a target story
	 */
	virtual ErrorCode ImportImages(K2Vector<UIDRef> blocsImages, IDFile articleFile, PMString& error) = 0;
	virtual PMString GetImageFileName(IDataBase* db, UID curPict) = 0;

	/** Applique un crop "mapping exact" a une image placee dans un cadre graphique.
		La zone de crop (en pixels de la vignette BR) est mise a l'echelle pour
		remplir exactement le cadre (echelles X/Y independantes -> peut deformer).
		@param frameRef [IN] le cadre graphique conteneur
		@param imageItemRef [IN] l'image placee (enfant du cadre)
		@param cropX,cropY,cropW,cropH [IN] zone de crop en pixels BR
		@param brFile [IN] vignette BR, lue pour obtenir les dimensions pixel
		@return kSuccess si le crop a ete applique ; kFailure si crop absent/invalide
				(l'appelant retombe alors sur FitContentProp)
	*/
	virtual ErrorCode ApplyCropToImage(const UIDRef& frameRef, const UIDRef& imageItemRef,
									   const PMReal& cropX, const PMReal& cropY,
									   const PMReal& cropW, const PMReal& cropH,
									   const IDFile& brFile) = 0;

	/** Load all tags in an XML article file into document's tag list
	*/
	virtual ErrorCode LoadTagList(IDFile articleFile, IDocument * destDoc) =0;

	/** Import a forme (snippet file) into the specified spread target at the specified position
	*/
	virtual ErrorCode ImportForme(UIDRef doc, IDFile formeFileToImport, PBPMPoint where, UIDRef targetSpread, IDFile matchingFile, PMString& error, UIDList * formeItems = nil, bool16 doTranslate = kTrue, bool16 setFormeName = kTrue) =0;
	
	/** Set forme data in a spline item boss
	*/
	virtual ErrorCode SetFormeData(UIDRef splineItem, const PMString& formeUniqueName, const int16& type, const int16& photoIndex = -1, const PMString& posX = kNullString, const PMString& posY = kNullString) =0;

	/** Set forme data in a text frame
	*/
	virtual ErrorCode SetFormeDataOnStory(UIDRef storyRef, const PMString& formeUniqueName, const int16& type, const K2Vector<IDValuePair> matchingStyleList, const int16& photoIndex = -1) =0;

	/** Import credit or legend in target story, applying paragraph style from typo file associated to the story
	*/
	virtual ErrorCode ImportCreditOrLegend(UIDRef storyRef, const PMString& text, const bool16 isCredit) =0;

	/** Debug methods that write log file in InDesign installation directory 
	*/
	virtual void WriteLog(const PMString& msg) =0;
	
	/** Generate preview of items
	*/
	virtual ErrorCode DoSnapShot(UIDList items, IDFile snapShotFile) =0;

	/** Export stories in XML format into specified folder
	*/
	virtual ErrorCode ExportToXml(const UIDList& storyList, IDFile xmlFolder, PMString artID) =0;

    /** Export stories
	*/

	virtual int32 GetNbSignes (const UIDList& storyList, IDataBase * db) =0;

	virtual int32 GetPageID(UIDRef docUIDRef, int32 index) = 0;
	virtual int32 GetPageId(void) = 0;


	/** Estimate Number of Charcters
	*/
	virtual void GetStatisticsText(UIDList itemsTexts, int32& nbCharcters, int32& nbParagraphs, int32& nbWords, int32& nbLines) =0;

	virtual ErrorCode DoNewAssignment(UIDList items, IDFile assignmentFile, const PMString& articleName, const PMString& recipientLogin, bool16 exportArticle, const PMString& TECStoryID, const bool16 doJPEG) =0;
	
	virtual ErrorCode DoAddToAssignment(IAssignment* assignment, const UIDList& items, UIDList& textStories, bool16 updateAssignment = kFalse) =0;

	virtual void ListPath(const IDFile path, const PMString & filter, K2Vector<IDFile> &fileList) =0;

	virtual bool16 validPath(const PMString & p) =0;

	virtual K2Vector<IDValuePair> ParseFileMatching(const IDFile& matchingFile) =0;

	/** Read the default carton location declared in an article XML file's
		<Proprietes> block : DossierCarton -> classeur, FichierCarton -> carton.
		Either out-param is left empty (kNullString) when the corresponding
		element is missing. Used to pre-select the dropdowns of the "lier
		article" dialog when an article carrying these hints is dropped on the
		page.
	*/
	virtual void GetArticleCartonDefaults(const IDFile& articleFile, PMString& dossierCarton, PMString& fichierCarton) =0;

	virtual std::map<PMString, UIDList> GetFormeDataList(IDocument* theDoc, bool16 includeMasters = kFalse) =0;	

	virtual ErrorCode GetCartonItemsFromTarget(UIDRef itemRef, UIDList& formeItems) =0;

	virtual K2Vector<KeyValuePair<PMString, K2Pair <PMString, UIDList> > > GetPlacedStoriesList(IDocument* theDoc) =0;

	virtual ErrorCode GetPlacedArtItemsFromTarget(UIDRef itemRef, UIDList& placedArticleItems) =0;

	virtual ErrorCode GetAllFormeItemsOnSpread(const PMString& idForme, ISpread* spread, UIDList& formeItems) =0;

	virtual void CategorizeItems(const UIDList& items, UIDList& textFrames, UIDList& graphicFrames) =0;

	virtual void SplitFormeDescription(const PMString& uniqueName, PMString& classeurName, PMString& formeName) =0;

	virtual PMString GetStoryIDFromAssignmentFile(const IDFile& assignmentFile) =0;

#if WINDOWS
	virtual bool16 DeleteDirectory(const TCHAR* sPath) =0;  // utility function to delete a directory and all its subfolders
#else
	virtual bool16 DeleteDirectory(const char* sPath) =0; 
#endif

	virtual void GotoStoryUI(const PMString& idStory) =0;

	virtual	UID	TriasFindStyleByName(IStyleGroupHierarchy *charStylesHierarchy, PMString searchStyle) = 0;

	virtual void HandleItemGroup(UIDList& storyList, UIDRef itemRef) = 0;

	/** Build a "unit identifier" for a single page item. The unit is the
	 *  scope across which kPhoto / kCredit / kLegend PhotoIndex must be
	 *  unique and sequential. We have two scopes :
	 *    * if the item carries an article ID (IPlacedArticleData::GetUniqueId
	 *      non-empty), the unit is the article — returns "ART:" + id.
	 *    * else if the item carries a carton name (IFormeData::GetUniqueName
	 *      non-empty), the unit is the carton — returns "CART:" + name.
	 *    * else the item is unscoped — returns kNullString.
	 *  Article takes precedence over carton (an item attached to an article
	 *  shares numbering with all other items of that article, regardless of
	 *  which carton it originally belonged to).
	 */
	virtual PMString GetUnitId(UIDRef itemRef) = 0;

	/** Renumber every kPhoto block in the given unit so PhotoIndex runs
	 *  0..N-1, preserving the current relative order (closes any gaps left
	 *  by deletion / FixedContent toggle / etc). For each kCredit / kLegend
	 *  block in the unit, look up its old PhotoIndex in the renumber map :
	 *    * if found → rewrite with the new PhotoIndex.
	 *    * if absent (its photo was deleted) → demote the block to kNoType
	 *      so its adornment disappears (the user keeps the text content).
	 *  kFixedContent / kArticle blocks keep their type ; their PhotoIndex
	 *  is irrelevant and untouched.
	 *  No-op if `unitId` is empty or doesn't match any block.
	 *
	 *  `excludeItems` (optional) : items whose UID is in this list are
	 *  skipped during the doc walk. Used by the document observer to
	 *  pre-renumber surviving members BEFORE a delete command actually
	 *  runs (the doomed items are still in the hierarchy at that point,
	 *  so we exclude them explicitly). Pass nil for "include everything".
	 */
	virtual ErrorCode RenumberUnit(IDocument* doc, const PMString& unitId, const UIDList* excludeItems = nil) = 0;

	/** Update only the PhotoIndex of an item's IFormeData, preserving its
	 *  current UniqueName, Type, and MatchingTagsNames. Used by the paste /
	 *  duplicate hook to push a freshly-pasted block to the end of its
	 *  unit's numbering before triggering a RenumberUnit pass.
	 *
	 *  No-op if the item has no IFormeData.
	 */
	virtual ErrorCode SetPhotoIndexOnly(UIDRef itemRef, int16 newPhotoIndex) = 0;
};

int32 GetNbPages(const UIDRef& documentUIDRef);

#endif // __IXPageUtils_h__
