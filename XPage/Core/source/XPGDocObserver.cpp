//========================================================================================
//  
//  $File: //depot/indesign_6.0/highprofile/source/sdksamples/persistentlist/XPGDocObserver.cpp $
//  
//  Owner: Adobe Developer Technologies
//  
//  $Author: Trias Developpement
//  
//  $DateTime: 2010/11/04 15:33:07 $
//  
//  $Revision:  
//  
//  $Change:  
//  
//  Copyright Adobe Systems Incorporated. All rights reserved.
//  
//  NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance 
//  with the terms of the Adobe license agreement accompanying it.  If you have received
//  this file from a source other than Adobe, then your use, modification, or 
//  distribution of it requires the prior written permission of Adobe.
//  
//  Plug-in includes
//===========================

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IDocument.h"
#include "IHierarchy.h"
#include "ISubject.h"

// API includes:
#include "CAlert.h"
#include "CmdUtils.h"
#include "CObserver.h"
#include "PageItemScrapID.h" // for kDeletePageItemCmdBoss
#include "StandOffID.h"		 // for kDeleteStandOffItemCmdBoss and kDeleteInsetPageItemCmdBoss
#include "UIDList.h"
#include "CAlert.h"
#include "K2Pair.h"
#include "IPageItemTypeUtils.h"
#include "KeyValuePair.h"
#include "IPageItemAdornmentList.h"
#include "IClassIDData.h"
#include "IFormeData.h"
#include "IWebServices.h"
#include "IPlacedArticleData.h"
#include "IXPageUtils.h"

// Project includes:
#include "XPGID.h"
#include "XPGUIID.h"



class XPGDocObserver : public CObserver{


	public:
		/**	Constructor.
			@param boss IN interface ptr from boss object on which this interface is aggregated.
		*/
		XPGDocObserver(IPMUnknown* boss);

		/**	Destructor.
		*/
		virtual ~XPGDocObserver();

		/** Called by the application to allow the observer to attach to the 
		 * 	subjects to be observed (kDocBoss)
		*/
		void AutoAttach();

		/** Called by the application to allow the observer to detach from the 
		 * 	subjects being observed.
		*/
		void AutoDetach();

 		virtual void Update(const ClassID& theChange, ISubject* theSubject, const PMIID& protocol, void* changedBy);

	protected:
		/**	Attaches this observer to a document.
		 * 	@param iDocument IN The document to which we want to attach.
		*/
		void AttachDocument(IDocument* iDocument);

		/**	Detaches this observer from a document.
		 * 	@param iDocument IN The document from which we want to detach.
		*/
		void DetachDocument(IDocument* iDocument);

		/**	Maintain document consistency :
			1) notifying XRail when all items of a placed story have been deleted
			2) delete resa redacs when all items of a forme have been deleted
		*/
		void HandlePageItemBeingDeleted(const UIDList& items);

		/** Helper methods
		*/
		void HandleItemBeingDeleted(IHierarchy * itemHier,
									K2Vector<KeyValuePair <PMString, UIDList> >& deletedStories);
		void DeleteAdornments(const UIDList& itemList);

		/** Phase 1 auto-renumber : in kNotDone (before the delete cmd
		 *	actually runs) we walk the doomed items, collect their unit
		 *	IDs and the flat list of leaf UIDs, then call RenumberUnit
		 *	with the leaves passed as `excludeItems` so the doc walk
		 *	ignores them — the result is the same as renumbering after
		 *	the delete, but without depending on a kDone notification
		 *	that this protocol may not fire.
		 */
		void CollectDoomedItems(const UIDList& items,
		                        K2Vector<PMString>& outUnits,
		                        UIDList& outLeaves);
		void CollectDoomedItemsRecursive(IHierarchy * itemHier,
		                                 K2Vector<PMString>& outUnits,
		                                 UIDList& outLeaves);

		/** Phase 4 paste/duplicate hook. After a paste / paste-inside /
		 *	duplicate cmd has run, walk the new items and decide :
		 *	  - has IPlacedArticleData with non-empty article ID :
		 *	      kPhoto  → bump PhotoIndex to a high unique value so
		 *	                RenumberUnit pushes the paste to the end of
		 *	                the article's numbering ; mark article for renumber.
		 *	      kCredit / kLegend → demote to kNoType (the link to a slot
		 *	                of the source carton is meaningless here).
		 *	      other → leave alone.
		 *	  - no article ID, has IFormeData with non-empty UniqueName :
		 *	      → wipe IFormeData entirely (the block becomes "free", the
		 *	        user re-attaches it to a carton manually if needed).
		 *	  - nothing tracked → leave alone.
		 *	After the walk, RenumberUnit runs on each tracked article so the
		 *	bumped photos get their final sequential PhotoIndex.
		 */
		void HandlePastedItems(const UIDList& items);
		void HandlePastedItemsRecursive(IHierarchy * itemHier,
		                                const std::map<PMString, UIDList>& formeMap,
		                                int16& bumpCounter,
		                                std::map<int16, int16>& outOrigToBumped,
		                                K2Vector<PMString>& outArticlesToRenumber,
		                                UIDList& outPendingCreditsLegends);

	private:

		InterfacePtr<IXPGPreferences> xpgPrefs;

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGDocObserver, kXPGDocObserverImpl)

/* XPGDocObserver Constructor
*/
XPGDocObserver::XPGDocObserver(IPMUnknown* boss):CObserver(boss, IID_IDOCOBSERVER){
	xpgPrefs = InterfacePtr<IXPGPreferences>(GetExecutionContextSession(), UseDefaultIID());	
}

/* XPGDocObserver Destructor
*/
XPGDocObserver::~XPGDocObserver(){
	// Bug latent pré-existant : appeler Release() ici manuellement double-
	// décrémente le refcount, parce que xpgPrefs est un InterfacePtr<...>
	// membre dont le destructor relâche déjà la ref. Le boss IXPGPreferences
	// se retrouvait freed deux fois au shutdown — invisible tant que l'observer
	// n'était jamais instancié, mais qui tombait à l'exit d'InDesign avec
	// un purecall dans Shuksan dès que l'observer était bien réveillé.
	// On laisse InterfacePtr faire son travail.
}


/*	XPGDocObserver::AutoAttach
*/
void XPGDocObserver::AutoAttach(){
	CObserver::AutoAttach();
	InterfacePtr<IDocument> iDocument(this, UseDefaultIID());
	if (iDocument)
		this->AttachDocument(iDocument);
}


/*	XPGDocObserver::AutoDetach
*/
void XPGDocObserver::AutoDetach(){
	CObserver::AutoDetach();
	InterfacePtr<IDocument> iDocument(this, UseDefaultIID());
	if (iDocument)
		this->DetachDocument(iDocument);
}

/*	XPGDocObserver::Update
*/
void XPGDocObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID& protocol, void* changedBy){

	do{
		ICommand* iCommand = (ICommand*)changedBy;
		const UIDList itemList = iCommand->GetItemListReference();
		if (protocol == IID_IHIERARCHY_DOCUMENT){
			// Phase 4 paste/duplicate hook : InDesign uses kAddToHierarchyCmdBoss
			// (not kPasteCmdBoss as one might expect) for the actual hierarchy
			// mutation that follows a paste / duplicate / drag-copy. Fires in
			// kDone state — the new items are already in the doc with their
			// data copied from the source.
			//
			// kAddToHierarchyCmdBoss also fires for imports (Placer un carton),
			// drag-and-drops, and other "add item to layer" flows. To avoid
			// stripping FormeData off legitimately imported cartons, we
			// distinguish "real paste" (= duplicate of an in-doc UniqueName)
			// from "import" (= 1st occurrence of a UniqueName in the doc) via
			// the GetFormeDataList map ; HandlePastedItems applies the paste
			// logic only when the UniqueName is duplicated.
			if (theChange == kAddToHierarchyCmdBoss){
				if (itemList.Length() == 0)
					break;
				if (iCommand->GetCommandState() != ICommand::kDone)
					break;
				HandlePastedItems(iCommand->GetItemListReference());
				break;
			}
			if (theChange == kDeletePageItemCmdBoss){
				if (itemList.Length() == 0)
					break;
				if(iCommand->GetCommandState() != ICommand::kNotDone)
					break;

				PMString cmdName;
				iCommand->GetName(&cmdName);

				// GD 29.01.2013 — Ungroup uses kDeletePageItemCmdBoss internally
				// but doesn't actually destroy any user content ; skip.
				// Empty cmdName = standard interactive delete (Suppr key) :
				// the legacy article-cleanup branch rejected it (probably
				// over-cautious), but our Phase 1 renumber must run.
				const bool16 isRealDelete = (cmdName != "Ungroup");
				const bool16 isLegacyHandled = isRealDelete && (cmdName != "");

				if(!isRealDelete)
					break;

				// Legacy article-cleanup keeps the original "non-empty name" filter
				// to avoid touching InDesign internal cmds that we never validated.
				if(isLegacyHandled)
					HandlePageItemBeingDeleted(iCommand->GetItemListReference());

				// Phase 1 auto-renumber runs for every real delete.
				{
					K2Vector<PMString> affectedUnits;
					UIDList doomedLeaves(iCommand->GetItemListReference().GetDataBase());
					CollectDoomedItems(iCommand->GetItemListReference(), affectedUnits, doomedLeaves);

					if(affectedUnits.size() > 0){
						InterfacePtr<IDocument> theDoc(this, UseDefaultIID());
						if(theDoc != nil){
							for(int32 i = 0; i < affectedUnits.size(); ++i)
								Utils<IXPageUtils>()->RenumberUnit(theDoc, affectedUnits[i], &doomedLeaves);
						}
						// Note : pas de broadcast kXPGRefreshMsg ici. Des
						// observers tiers (panel, etc.) réagissent au msg
						// et lancent leurs propres cmds, qui peuvent se
						// retrouver mêlées au tear-down de doc/Shuksan
						// quand la cmd de delete englobante est elle-même
						// déclenchée par une fermeture de fichier — d'où
						// crash à la fermeture. RenumberUnit a déjà
						// appelé InvalidateViews, ça suffit pour redessiner
						// les ornements.
					}
				}
			}
		}
	} while (kFalse);
}

/*	XPGDocObserver::HandlePageItemDeleted
*/
void XPGDocObserver::HandlePageItemBeingDeleted(const UIDList& items){

	do{
		int32 nbItems = items.Length();
		K2Vector<KeyValuePair <PMString, UIDList> > deletedStories;

		// Parcourir les articles a supprimer
		for(int32 i=0; i < nbItems; ++i){
			InterfacePtr<IHierarchy> itemHier (items.GetRef(i), UseDefaultIID());
			if(!itemHier)
				continue;

			this->HandleItemBeingDeleted(itemHier, deletedStories);
		}

		// On supprime definitivement un article lorsque le nb des items selectionnes est equivalent au nb d'items existants en page
		if(deletedStories.size() > 0)
		{
			InterfacePtr<IDocument> theDoc (this, UseDefaultIID());
			K2Vector<KeyValuePair <PMString, K2Pair <PMString, UIDList> > > placedStoriesList = Utils<IXPageUtils>()->GetPlacedStoriesList(theDoc);
						
			for(int32 i = 0; i < deletedStories.size(); i++){	

				PMString idArt = deletedStories[i].Key();
				if(idArt == kNullString)
					continue;

				int32 index = ::FindLocation(placedStoriesList, idArt); 
				if(index == -1)
					continue;

				UIDList storyItems = deletedStories[i].Value();
				int32 itemsCount = storyItems.size();

				// Si ok , supprime definitivement l'article
				if(itemsCount == placedStoriesList[index].Value().second.size()){
				 
					// Delete data of placed story				
					InterfacePtr<ICommand> deletePlacedArticleDataCmd(CmdUtils::CreateCommand(kXPGSetPlacedArticleDataCmdBoss));
					InterfacePtr<IPlacedArticleData> placedArticleData(deletePlacedArticleDataCmd, IID_IPLACEDARTICLEDATA);
					placedArticleData->SetUniqueId(kNullString);
					placedArticleData->SetStoryFolder(kNullString);
					deletePlacedArticleDataCmd->SetItemList(storyItems);
					if(CmdUtils::ProcessCommand(deletePlacedArticleDataCmd)!= kSuccess)
						continue;					
				

					// Delete adromnment from page item with kXPGUIArticleAdornmentBoss
					this->DeleteAdornments(storyItems);	

					// Send notification so that texte panel is updated
					InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
					sessionSubject->Change(kXPGRefreshMsg, IID_IREFRESHPROTOCOL);	
				}
			}			
		}
	} while (false);
}

/*	XPGDocObserver::AttachDocument
*/
void XPGDocObserver::AttachDocument(IDocument* iDocument){

	do{
		if(iDocument == nil)		
			break;

		InterfacePtr<ISubject> iDocSubject(iDocument, UseDefaultIID());
		if(iDocSubject == nil)		
			break;		

		//	Protocols used for page item model changes
		if (!iDocSubject->IsAttached(ISubject::kRegularAttachment,this, IID_IHIERARCHY_DOCUMENT, IID_IDOCOBSERVER))	 
			iDocSubject->AttachObserver(ISubject::kRegularAttachment,this, IID_IHIERARCHY_DOCUMENT, IID_IDOCOBSERVER);
		 
	} while (kFalse);
}


/*	XPGDocObserver::DetachDocument
*/
void XPGDocObserver::DetachDocument(IDocument* iDocument){
	do{
		if (iDocument == nil)		
			break;
	
		InterfacePtr<ISubject> iDocSubject(iDocument, UseDefaultIID());	
		if(iDocSubject == nil)
			break;	

		if (iDocSubject->IsAttached(ISubject::kRegularAttachment,this, IID_IHIERARCHY_DOCUMENT, IID_IDOCOBSERVER))	
			iDocSubject->DetachObserver(ISubject::kRegularAttachment,this, IID_IHIERARCHY_DOCUMENT, IID_IDOCOBSERVER);
	
	} while (kFalse);
}

void XPGDocObserver::HandleItemBeingDeleted(IHierarchy * itemHier,
											K2Vector<KeyValuePair <PMString, UIDList> >& deletedStories){

	if(Utils<IPageItemTypeUtils>()->IsGroup(itemHier)){
		// If it's a group, handle children item recursively
		for(int32 i = 0 ; i < itemHier->GetChildCount() ; ++i){
			InterfacePtr<IHierarchy> childHier (itemHier->QueryChild(i));
			HandleItemBeingDeleted(childHier, deletedStories);
		}
	}
	else{
		// Check out item if is an InCopy story
		UIDRef itemRef = ::GetUIDRef(itemHier);
		UIDRef storyRef = itemRef;
		InterfacePtr<IPlacedArticleData> placedArticleData(itemRef, UseDefaultIID());

		if(placedArticleData){
			PMString idArt = placedArticleData->GetUniqueId();
			PMString pathArt = placedArticleData->GetStoryFolder();
			if(!idArt.IsEmpty() && !pathArt.IsEmpty()){
				// Add item to the group of items it belongs to
				K2Vector<KeyValuePair <PMString, UIDList> >::iterator iter = ::K2find(deletedStories, idArt);
				if(iter == deletedStories.end()) {
					deletedStories.push_back(KeyValuePair <PMString, UIDList> (idArt, UIDList(itemRef)));
				}
				else if(iter != deletedStories.end())
					iter->Value().Append(itemRef);
			}
		}
	}
}

/* Phase 1 auto-renumber — see header.
*/
void XPGDocObserver::CollectDoomedItems(const UIDList& items,
                                        K2Vector<PMString>& outUnits,
                                        UIDList& outLeaves){
	const int32 nbItems = items.Length();
	for(int32 i = 0; i < nbItems; ++i){
		InterfacePtr<IHierarchy> itemHier(items.GetRef(i), UseDefaultIID());
		if(itemHier == nil)
			continue;
		CollectDoomedItemsRecursive(itemHier, outUnits, outLeaves);
	}
}

void XPGDocObserver::CollectDoomedItemsRecursive(IHierarchy * itemHier,
                                                 K2Vector<PMString>& outUnits,
                                                 UIDList& outLeaves){
	if(itemHier == nil)
		return;

	if(Utils<IPageItemTypeUtils>()->IsGroup(itemHier)){
		// Recurse into groups so we catch every leaf — the deletion cmd
		// may be invoked on a group whose children carry the FormeData /
		// PlacedArticleData we need to read.
		const int32 nbChildren = itemHier->GetChildCount();
		for(int32 i = 0; i < nbChildren; ++i){
			InterfacePtr<IHierarchy> childHier(itemHier->QueryChild(i));
			CollectDoomedItemsRecursive(childHier, outUnits, outLeaves);
		}
		return;
	}

	UIDRef itemRef = ::GetUIDRef(itemHier);

	// Always add the leaf UID to the exclude list so the eventual
	// RenumberUnit walk skips it, even if it has no unit (defensive).
	outLeaves.Append(itemRef.GetUID());

	const PMString unitId = Utils<IXPageUtils>()->GetUnitId(itemRef);
	if(unitId.IsEmpty())
		return;

	// Dedupe — a delete touching N items of the same carton triggers
	// only one renumber pass.
	for(int32 j = 0; j < outUnits.size(); ++j){
		if(outUnits[j] == unitId)
			return;
	}
	outUnits.push_back(unitId);
}

/* Phase 4 paste/duplicate hook — see header. */
void XPGDocObserver::HandlePastedItems(const UIDList& items){
	InterfacePtr<IDocument> theDoc(this, UseDefaultIID());
	if(theDoc == nil) return;

	// Build a doc-wide UniqueName→UIDList map ONCE for this Update call.
	// We use it to distinguish a real paste (UniqueName already present
	// elsewhere in the doc, count > 1) from a fresh import / Placer
	// (UniqueName unique, count == 1 — the just-added item itself). If
	// we acted on imports, every Placer would strip the FormeData off
	// the carton it just imported.
	std::map<PMString, UIDList> formeMap = Utils<IXPageUtils>()->GetFormeDataList(theDoc);

	K2Vector<PMString> articlesToRenumber;
	int16 bumpCounter = 0;
	// Pass 1 builds this map { origPhotoIdx → newBumpedIdx } so pass 2
	// can re-link a pasted credit/legend to its sibling pasted photo.
	std::map<int16, int16> origToBumped;
	// Pass 1 stashes pasted credits/legends here for pass 2 to handle —
	// the photo remap must be complete before we decide what to link them to.
	UIDList pendingCreditsLegends(items.GetDataBase());

	// === Pass 1 : photos (bump + remap) + carton-only kPhoto wipe.
	const int32 nbItems = items.Length();
	for(int32 i = 0; i < nbItems; ++i){
		InterfacePtr<IHierarchy> itemHier(items.GetRef(i), UseDefaultIID());
		if(itemHier == nil) continue;
		HandlePastedItemsRecursive(itemHier, formeMap, bumpCounter,
		                           origToBumped, articlesToRenumber, pendingCreditsLegends);
	}

	// === Pass 2 : credits/legends. Re-link to their sibling pasted photo
	// if available, else demote orphan / wipe carton link.
	const int32 nbCL = pendingCreditsLegends.Length();
	IDataBase * db = items.GetDataBase();
	for(int32 i = 0; i < nbCL; ++i){
		UIDRef clRef(db, pendingCreditsLegends[i]);

		InterfacePtr<IFormeData> fd(clRef, UseDefaultIID());
		if(fd == nil) continue;

		const int16 origPhotoIdx = fd->GetPhotoIndex();
		PMString articleId;
		{
			InterfacePtr<IPlacedArticleData> ad(clRef, IID_IPLACEDARTICLEDATA);
			if(ad != nil) articleId = ad->GetUniqueId();
		}

		if(!articleId.IsEmpty()){
			// Case (b) — credit/legend belongs to an article. Re-link to
			// a sibling photo only if it landed in the SAME notification
			// (rare in practice — InDesign typically splits paste across
			// notifications, in which case origToBumped is empty here and
			// we orphan. Article blocks shouldn't be hand-duplicated by
			// the user anyway, so a clean orphan is fine).
			std::map<int16, int16>::const_iterator hit = origToBumped.find(origPhotoIdx);
			if(hit != origToBumped.end()){
				Utils<IXPageUtils>()->SetPhotoIndexOnly(clRef, hit->second);
			}
			else{
				Utils<IXPageUtils>()->SetFormeData(clRef, kNullString, IFormeData::kNoType, -1);
			}
		}
		else{
			// Case (a) — no article. Wipe carton link.
			Utils<IXPageUtils>()->SetFormeData(clRef, kNullString, IFormeData::kNoType, -1);
		}
	}

	if(articlesToRenumber.size() == 0)
		return;

	// === Renumber every article whose photo set just gained a paste.
	for(int32 i = 0; i < articlesToRenumber.size(); ++i)
		Utils<IXPageUtils>()->RenumberUnit(theDoc, articlesToRenumber[i]);
}

void XPGDocObserver::HandlePastedItemsRecursive(IHierarchy * itemHier,
                                                const std::map<PMString, UIDList>& formeMap,
                                                int16& bumpCounter,
                                                std::map<int16, int16>& outOrigToBumped,
                                                K2Vector<PMString>& outArticlesToRenumber,
                                                UIDList& outPendingCreditsLegends){
	if(itemHier == nil) return;

	if(Utils<IPageItemTypeUtils>()->IsGroup(itemHier)){
		const int32 nbChildren = itemHier->GetChildCount();
		for(int32 i = 0; i < nbChildren; ++i){
			InterfacePtr<IHierarchy> childHier(itemHier->QueryChild(i));
			HandlePastedItemsRecursive(childHier, formeMap, bumpCounter,
			                           outOrigToBumped, outArticlesToRenumber, outPendingCreditsLegends);
		}
		return;
	}

	UIDRef itemRef = ::GetUIDRef(itemHier);

	// Only items with FormeData and a UniqueName are paste-relevant.
	InterfacePtr<IFormeData> formeData(itemRef, UseDefaultIID());
	if(formeData == nil) return;

	const PMString uniqueName = formeData->GetUniqueName();
	if(uniqueName.IsEmpty()) return;
	const int16 type = formeData->GetType();

	// "Real paste" check : the item shares its UniqueName with at least
	// one other block in the doc. A fresh Placer/import has a unique
	// timestamp-suffixed UniqueName, so its count == 1 (just itself).
	std::map<PMString, UIDList>::const_iterator it = formeMap.find(uniqueName);
	if(it == formeMap.end() || it->second.Length() <= 1) return;

	// Article ID, if any.
	PMString articleId;
	{
		InterfacePtr<IPlacedArticleData> articleData(itemRef, IID_IPLACEDARTICLEDATA);
		if(articleData != nil)
			articleId = articleData->GetUniqueId();
	}

	switch(type){
		case IFormeData::kPhoto: {
			if(!articleId.IsEmpty()){
				// Case (b) kPhoto — bump to a unique high value so
				// RenumberUnit's sort puts every paste after the
				// existing photos. Record the mapping so pass 2 can
				// re-link sibling credits/legends.
				const int16 origIdx    = formeData->GetPhotoIndex();
				const int16 bumpedIdx  = static_cast<int16>(kMaxInt16 - bumpCounter);
				outOrigToBumped[origIdx] = bumpedIdx;
				Utils<IXPageUtils>()->SetPhotoIndexOnly(itemRef, bumpedIdx);
				++bumpCounter;

				PMString articleUnit;
				articleUnit.Append("ART:");
				articleUnit.Append(articleId);
				bool16 alreadyTracked = kFalse;
				for(int32 j = 0; j < outArticlesToRenumber.size(); ++j){
					if(outArticlesToRenumber[j] == articleUnit){
						alreadyTracked = kTrue;
						break;
					}
				}
				if(!alreadyTracked)
					outArticlesToRenumber.push_back(articleUnit);
			}
			else{
				// Case (a) kPhoto — would conflict with the source
				// carton's numbering. Wipe.
				Utils<IXPageUtils>()->SetFormeData(itemRef, kNullString, IFormeData::kNoType, -1);
			}
			break;
		}
		case IFormeData::kCredit:
		case IFormeData::kLegend: {
			// Defer to pass 2 — we need the full pass-1 photo remap
			// before we know whether to re-link or orphan.
			outPendingCreditsLegends.Append(itemRef.GetUID());
			break;
		}
		// kFixedContent / kArticle / kNoType / others : leave alone.
		// (Per user request : pasted "Contenu fixe" should keep its
		// tag in the source carton — no numbering conflict since
		// kFixedContent isn't part of the photo sequence.)
		default:
			break;
	}
}

void XPGDocObserver::DeleteAdornments(const UIDList& itemList){

	do{
		UIDList adorneditemList(itemList.GetDataBase());
		for (int32 i = 0; i < itemList.Length(); i++){		
			InterfacePtr<IPageItemAdornmentList> pageItemAdornmentList(itemList.GetRef(i), IID_IPAGEITEMADORNMENTLIST);
			if (pageItemAdornmentList && pageItemAdornmentList->HasAdornment(kXPGUIArticleAdornmentBoss))
				adorneditemList.Append(itemList[i]);		 
		}

		if (adorneditemList.Length() > 0){
			// Remove the adornment.
			InterfacePtr<ICommand>  removePageItemAdornmentCmd(CmdUtils::CreateCommand(kRemovePageItemAdornmentCmdBoss));         
			if (!removePageItemAdornmentCmd)        
				break;
        
			InterfacePtr<IClassIDData> classIDData(removePageItemAdornmentCmd, UseDefaultIID());        
			if (!classIDData)       
				break;
         
			classIDData->Set(kXPGUIArticleAdornmentBoss);
			removePageItemAdornmentCmd->SetItemList(adorneditemList);
			CmdUtils::ProcessCommand(removePageItemAdornmentCmd);
		}
	}while(kFalse);
}


// End, XPGDocObserver.cpp.

