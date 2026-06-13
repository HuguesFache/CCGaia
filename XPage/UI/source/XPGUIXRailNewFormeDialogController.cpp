#include "VCPlugInHeaders.h"
#include <vector>

// Interface includes:
#include "IControlView.h"
#include "ICommand.h"
#include "ICoreFilename.h"
#include "IPanelControlData.h"
#include "ITriStateControlData.h"
#include "IStringListControlData.h"
#include "IDropDownListController.h"
#include "IStringData.h"
#include "IArticleDataModel.h"
#include "IXPageMgrAccessor.h"
#include "IStringListData.h"
#include "IXPageSuite.h"
#include "ISelectionUtils.h"
#include "ISelectionManager.h"
#include "IPMStream.h"
#include "IFormeDataModel.h"
#include "IObserver.h"
#include "ISubject.h"
#include "IActiveContext.h"
#include "IDropDownListController.h"
#include "IUIDData.h"
#include "IUIDListData.h"
#include "IDocument.h"
#include "ILayoutSelectionSuite.h"
#include "IFormeData.h"
#include "IClassIDData.h"
#include "ILayoutUIUtils.h"
#include "IGeometry.h"

// General includes:

#include "CDialogController.h"
#include "IUnitOfMeasure.h"
#include "GenericID.h"
#include "FileUtils.h"
#include "CmdUtils.h"
#include "K2Pair.h"
#include "CAlert.h"
#include "Utils.h"
#include "DBUtils.h"
#include "TransformUtils.h"
#include "ITextModel.h"
#include "IAssignmentUtils.h"
#include "IAttributeStrand.h"
#include "IKeyValueData.h"
#include "IXPageUtils.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"
#include "ILayoutUtils.h"
#include "IHierarchy.h"
#include "IMultiColumnTextFrame.h"
#include "ITextControlData.h"

// General includes (STL):
#include <map>
#include <algorithm>

// Project includes:
#include "XPageUIUtils.h"
#include "XPGUIID.h"
#include "XPGID.h"

/**
	Preferences dialog controller
*/
class XPGUIXRailNewFormeDialogController : public CDialogController
{
	public:

		XPGUIXRailNewFormeDialogController(IPMUnknown* boss) : CDialogController(boss),
			mSavedDB(nil), mWasApplied(kFalse) {}

		virtual void InitializeDialogFields( IActiveContext* dlgContext);

		virtual WidgetID ValidateDialogFields( IActiveContext* myContext);

		virtual void ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId);

		virtual void ClosingDialog (IActiveContext *myContext);

	private:
		void EnableDefineCreditLegend(bool16 doEnable);
		void ResetFormeTypes(UIDList formeItems);
		void AddFormeAdornments(UIDList formeItems);
		void RemoveFormeAdornments(UIDList formeItems);
		void RestoreSavedFormeData();

		// Per-block IFormeData snapshot taken at dialog open. Used to roll
		// the persistence back when the user cancels — without it, the
		// destructive reset+rewrite the dialog performs on its working set
		// would leave the blocks stripped of their original carton link, so
		// re-opening the dialog on the same selection would show an empty
		// form. Only items that actually had IFormeData are saved.
		struct SavedFormeData {
			UID itemUID;
			PMString uniqueName;
			int16 type;
			int16 photoIndex;
			K2Vector<IDValuePair> matchingTags;
		};
		std::vector<SavedFormeData> mSavedItems;
		IDataBase* mSavedDB;
		bool16 mWasApplied;
};

CREATE_PMINTERFACE(XPGUIXRailNewFormeDialogController, kXPGUIXRailNewFormeDialogControllerImpl)

/* ApplyFields
*/
void XPGUIXRailNewFormeDialogController::InitializeDialogFields( IActiveContext* dlgContext) 
{

	InterfacePtr<IPanelControlData> panelCtrlData (this, UseDefaultIID());
	do{
		// First get selected forme items
		IDataBase * db = ::GetDataBase(dlgContext->GetContextDocument());		
		UIDList * txtFrameList = new UIDList(db);
		UIDList * photoList = new UIDList(db);	
		UIDList * formeItems = new UIDList(db);
		UIDList * formeStyles = new UIDList(db);
        UIDList * fixedContentFrameList = new UIDList(db);

		InterfacePtr<IXPageSuite> xpgSuite (dlgContext->GetContextSelection(), IID_IXPAGESUITE);
		if(xpgSuite == nil) // nil means no active selection
			break;
			
		xpgSuite->GetSelectedItems(*txtFrameList, *photoList, *formeItems);

		// Snapshot the existing IFormeData on every selected block before
		// ResetFormeTypes wipes it. We use this snapshot to hydrate the
		// dialog from a placed-then-edited carton: name, classeur,
		// "Carton photo" flag, photo order, credit/legend mapping, fixed
		// content blocks. New blocks the user added (no persistence) are
		// treated as fresh and tagged at the end of the photo list.
		const int32 photoCount = photoList->Length();
		const int32 txtCount = txtFrameList->Length();
		K2Vector<int16> photoOrigType;
		K2Vector<int16> photoOrigIdx;
		K2Vector<int16> txtOrigType;
		K2Vector<int16> txtOrigIdx;

		// Reset the cancel-rollback state for this dialog session. Every
		// item that has IFormeData gets archived into mSavedItems below;
		// ClosingDialog restores from this if the user cancels.
		mSavedItems.clear();
		mSavedDB = db;
		mWasApplied = kFalse;

		std::map<PMString, int32> nameTally;
		for(int32 i = 0; i < photoCount; ++i){
			int16 t = IFormeData::kNoType;
			int16 p = -1;
			InterfacePtr<IFormeData> fd(photoList->GetRef(i), UseDefaultIID());
			if(fd != nil){
				t = fd->GetType();
				p = fd->GetPhotoIndex();
				const PMString& uname = fd->GetUniqueName();
				if(!uname.IsEmpty()) nameTally[uname]++;

				SavedFormeData saved;
				saved.itemUID = (*photoList)[i];
				saved.uniqueName = uname;
				saved.type = t;
				saved.photoIndex = p;
				saved.matchingTags = fd->GetMatchingTagsNames();
				mSavedItems.push_back(saved);
			}
			photoOrigType.push_back(t);
			photoOrigIdx.push_back(p);
		}
		for(int32 i = 0; i < txtCount; ++i){
			int16 t = IFormeData::kNoType;
			int16 p = -1;
			InterfacePtr<IFormeData> fd(txtFrameList->GetRef(i), UseDefaultIID());
			if(fd != nil){
				t = fd->GetType();
				p = fd->GetPhotoIndex();
				const PMString& uname = fd->GetUniqueName();
				if(!uname.IsEmpty()) nameTally[uname]++;

				SavedFormeData saved;
				saved.itemUID = (*txtFrameList)[i];
				saved.uniqueName = uname;
				saved.type = t;
				saved.photoIndex = p;
				saved.matchingTags = fd->GetMatchingTagsNames();
				mSavedItems.push_back(saved);
			}
			txtOrigType.push_back(t);
			txtOrigIdx.push_back(p);
		}

		// Pick the most-frequent UniqueName as the carton being modified.
		// Ties go to the first encountered entry — the std::map iteration
		// order is deterministic but unspecified by us; in a tie the user
		// should re-select, so we don't try to be clever here.
		PMString hydratedUniqueName = kNullString;
		int32 bestTally = 0;
		for(std::map<PMString, int32>::iterator it = nameTally.begin(); it != nameTally.end(); ++it){
			if(it->second > bestTally){ bestTally = it->second; hydratedUniqueName = it->first; }
		}

		PMString hydratedFormeName = kNullString;
		PMString hydratedClasseur  = kNullString;
		bool16 hasHydration = kFalse;
		if(bestTally > 0){
			Utils<IXPageUtils>()->SplitFormeDescription(hydratedUniqueName, hydratedClasseur, hydratedFormeName);
			if(!hydratedFormeName.IsEmpty() && !hydratedClasseur.IsEmpty())
				hasHydration = kTrue;
		}

		// Reset forme data in forme items
		UIDList formeDataItems (*txtFrameList);
		formeDataItems.Append(*photoList);

		ResetFormeTypes(formeDataItems);

		// Pre-fill forme name from hydration when available, otherwise empty.
		if(hasHydration){
			PMString hydratedFormeNameDisplay = hydratedFormeName;
			hydratedFormeNameDisplay.SetTranslatable(kFalse);
			SetTextControlData(kXPGUIFormeNameWidgetID, hydratedFormeNameDisplay);
		}
		else {
			SetTextControlData(kXPGUIFormeNameWidgetID, kNullString);
		}

		// Clear rubrique file list
		InterfacePtr<IStringListControlData> rubriqueListData (panelCtrlData->FindWidget(kXPGUIRubriqueListWidgetID), UseDefaultIID());
		rubriqueListData->Clear();

		// Choose the classeur to pre-select: hydration wins over the XDA
		// palette default, since the user is clearly modifying a known carton.
		InterfacePtr<IStringData> classeurData (this, IID_ICLASSEURNAMEDATA);
		PMString selectedClasseurName = hasHydration ? hydratedClasseur : classeurData->GetString();

		// Populate rubrique file list
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IFormeDataModel> formeModel (pageMrgAccessor->QueryFormeDataModel());
		K2Vector<PMString> classeurList = formeModel->GetClasseurList();
		int32 selectID = 0;
		for(int32 i = 0 ; i < classeurList.size() ; ++i){
			PMString classeur = classeurList[i];
			if (classeur == selectedClasseurName) {
				selectID = i;
			}
			classeur.SetTranslatable(kFalse);
			rubriqueListData->AddString(classeur);
		}

		if(classeurList.size() > 0){
			InterfacePtr<IDropDownListController> rubriqueListController (rubriqueListData, UseDefaultIID());
			rubriqueListController->Select(selectID);
		}
		else
			CAlert::InformationAlert(PMString(kXPGErrMatchingFileNotFound));

		
		// Do matching file list initialization through observer
		InterfacePtr<ISubject> rubriqueListSubject (rubriqueListData, UseDefaultIID());
		InterfacePtr<IObserver> dialogObserver (this, IID_INEWFORMEOBSERVER);
		
		dialogObserver->Update(kPopupChangeStateMessage, rubriqueListSubject, IID_ISTRINGLISTCONTROLDATA, nil);

		// Clear photo list
		InterfacePtr<IStringListControlData> photoListStringData (panelCtrlData->FindWidget(kXPGUIPhotoListWidgetID), UseDefaultIID());
		photoListStringData->Clear();

		// Reorder photoList so the photo dropdown follows the carton's
		// original order (origPhotoIndex ascending). Blocks that were not
		// kPhoto in the original carton — kFixedContent images, or
		// brand-new blocks the user just added — get pushed to the end so
		// they receive fresh photo numbers without disturbing the existing
		// numbering. Stable sort keeps the relative order of ties (matters
		// for the trailing "new blocks" group).
		if(hasHydration && photoCount > 1){
			K2Vector<int32> perm;
			K2Vector<int32> sortKeys;
			for(int32 i = 0; i < photoCount; ++i){
				perm.push_back(i);
				sortKeys.push_back((photoOrigType[i] == IFormeData::kPhoto && photoOrigIdx[i] >= 0)
					? static_cast<int32>(photoOrigIdx[i]) : kMaxInt32);
			}
			std::stable_sort(perm.begin(), perm.end(), [&sortKeys](int32 a, int32 b){
				return sortKeys[a] < sortKeys[b];
			});

			UIDList * sortedPhotoList = new UIDList(db);
			K2Vector<int16> sortedOrigType;
			K2Vector<int16> sortedOrigIdx;
			for(int32 i = 0; i < photoCount; ++i){
				sortedPhotoList->Append((*photoList)[perm[i]]);
				sortedOrigType.push_back(photoOrigType[perm[i]]);
				sortedOrigIdx.push_back(photoOrigIdx[perm[i]]);
			}
			delete photoList;
			photoList = sortedPhotoList;
			photoOrigType = sortedOrigType;
			photoOrigIdx = sortedOrigIdx;
		}

		// Build a reverse lookup origPhotoIndex → new index, so we can
		// re-link kCredit / kLegend text frames (which reference the
		// original photo index) to the right slot after re-numbering.
		std::map<int16, int32> origIdxToNewIdx;
		for(int32 i = 0; i < photoList->Length(); ++i){
			if(photoOrigType[i] == IFormeData::kPhoto && photoOrigIdx[i] >= 0)
				origIdxToNewIdx[photoOrigIdx[i]] = i;
		}

		// Fill photo list according to the number of graphic frames found in the selected forme
		UIDList * creditList = new UIDList(db);
		UIDList * legendList = new UIDList(db);

		// Position dropdown lives next to the photo dropdown. We populate
		// it with the same number of entries as the photo list ("Position
		// 1", "Position 2", ...) so the user can move the currently-
		// selected photo to any other slot. The observer hooks
		// kPopupChangeStateMessage on this dropdown to trigger the move.
		InterfacePtr<IStringListControlData> positionListStringData (panelCtrlData->FindWidget(kXPGUIPhotoPositionListWidgetID), UseDefaultIID());
		if(positionListStringData)
			positionListStringData->Clear();

		for(int32 i = 0 ; i < photoList->Length() ; ++i){

			PMString photo (kXPGUIPhotoTextKey, PMString::kTranslateDuringCall);
			photo.AppendNumber(i + 1);
			photo.SetTranslatable(kFalse);
			photoListStringData->AddString(photo);

			if(positionListStringData){
				PMString pos (kXPGUIPhotoPositionTextKey, PMString::kTranslateDuringCall);
				pos.AppendNumber(i + 1);
				pos.SetTranslatable(kFalse);
				positionListStringData->AddString(pos);
			}

			// Preallocate lists for text frames (credit and legend) associated to photo
			creditList->Append(kInvalidUID); // Same UID as the photo means a credit is required but not set yet, kInvalidUID means it's not required (default)
			legendList->Append(kInvalidUID); // Same UID as the photo means a legend is required but not set yet, kInvalidUID means it's not required (default)

			// Update item forme data for drawing forme adornment
			Utils<IXPageUtils>()->SetFormeData(photoList->GetRef(i), kNullString, IFormeData::kPhoto, i);
		}

		// Hydrate credit/legend lists from existing kCredit / kLegend
		// text frames found in the selection. Each such text frame's
		// IFormeData carries the original photo index it linked to; we
		// resolve that to the new slot through origIdxToNewIdx.
		// We also rewrite the IFormeData type/photoIndex on the text
		// frame's spline so the adornment shape (Légende N / Crédit N)
		// shows above each block — the earlier ResetFormeTypes wiped the
		// per-block type, so without this the adornments would be missing.
		if(hasHydration){
			for(int32 i = 0; i < txtCount; ++i){
				const int16 ttype = txtOrigType[i];
				if(ttype != IFormeData::kCredit && ttype != IFormeData::kLegend)
					continue;
				std::map<int16, int32>::iterator slotIt = origIdxToNewIdx.find(txtOrigIdx[i]);
				if(slotIt == origIdxToNewIdx.end())
					continue;

				// Read story UID off the text frame the same way
				// HandleDefineLinkedFrame does — the dialog stores story
				// UIDs (not text-frame UIDs) in creditList / legendList.
				InterfacePtr<IHierarchy> txtFrameHier(txtFrameList->GetRef(i), UseDefaultIID());
				if(txtFrameHier == nil) continue;
				InterfacePtr<IMultiColumnTextFrame> txtFrame(db, txtFrameHier->GetChildUID(0), UseDefaultIID());
				if(txtFrame == nil) continue;
				const UID storyUID = txtFrame->GetTextModelUID();
				if(storyUID == kInvalidUID) continue;

				UIDList * targetList = (ttype == IFormeData::kCredit) ? creditList : legendList;
				targetList->Replace(slotIt->second, storyUID);

				// Restore the type on the spline so the adornment label
				// is drawn. SetFormeData wipes matchingTags as a side
				// effect — that's fine: ClosingDialog rolls them back on
				// cancel, and on apply CreateForme rewrites them anyway.
				Utils<IXPageUtils>()->SetFormeData(txtFrameList->GetRef(i), kNullString, ttype, slotIt->second);
			}
		}

		// Hydrate the fixed-content frame list from blocks whose original
		// IFormeData type was kFixedContent. Both text frames and graphic
		// frames can carry that flag. We also restore the kFixedContent
		// type on each block (overriding the kPhoto the photo loop just
		// set on graphic frames) so the "Fixe" adornment is drawn.
		if(hasHydration){
			for(int32 i = 0; i < txtCount; ++i){
				if(txtOrigType[i] == IFormeData::kFixedContent){
					fixedContentFrameList->Append((*txtFrameList)[i]);
					Utils<IXPageUtils>()->SetFormeData(txtFrameList->GetRef(i), kNullString, IFormeData::kFixedContent, -1);
				}
			}
			for(int32 i = 0; i < photoList->Length(); ++i){
				if(photoOrigType[i] == IFormeData::kFixedContent){
					fixedContentFrameList->Append((*photoList)[i]);
					Utils<IXPageUtils>()->SetFormeData(photoList->GetRef(i), kNullString, IFormeData::kFixedContent, -1);
				}
			}
		}

		// Hydrate the "Carton photo" checkbox. The flag isn't on the
		// per-block IFormeData — it lives on the carton's descriptif XML —
		// so we read it through the model once we know the carton's
		// (name, classeur) pair.
		{
			InterfacePtr<ITriStateControlData> chkboxCartonPhoto (panelCtrlData->FindWidget(kXPGUICartonPhotoWidgetID), UseDefaultIID());
			if(chkboxCartonPhoto != nil){
				if(hasHydration && formeModel != nil && formeModel->IsPhotoCarton(hydratedFormeName, hydratedClasseur))
					chkboxCartonPhoto->Select(kTrue, kFalse);
				else
					chkboxCartonPhoto->Deselect(kTrue, kFalse);
			}
		}
	
		// Reset dialog internal data
		InterfacePtr<IUIDData> storyData (this, IID_ISTORYDATA);
		InterfacePtr<IUIDListData> txtFrameListData (this, IID_ITEXTFRAMELIST);
		InterfacePtr<IUIDListData> photoListData (this, IID_IPHOTOLIST);
		InterfacePtr<IUIDListData> creditListData (this, IID_ICREDITLIST);
		InterfacePtr<IUIDListData> legendListData (this, IID_ILEGENDLIST);
		InterfacePtr<IUIDListData> formeItemListData (this, IID_IFORMEITEMLIST);
		InterfacePtr<IUIDListData> formeStylesListData (this, IID_IFORMESTYLESLIST);
        InterfacePtr<IUIDListData> fixedContentFrameListData (this, IID_IFIXEDCONTENTFRAMELIST);

		UIDList textItems (*txtFrameList);
		for(int32 i = 0; i < textItems.Length(); i++ ){
		
			UIDRef storyRef = textItems.GetRef(i);
			Utils<IAssignmentUtils>()->NormalizeTextModelRef(storyRef);
			InterfacePtr<ITextModel> txtModel (storyRef, UseDefaultIID());
			if(txtModel == nil)
				continue;

			InterfacePtr<IAttributeStrand> paraStrand ((IAttributeStrand*) txtModel->QueryStrand(kParaAttrStrandBoss,IID_IATTRIBUTESTRAND));
			TextIndex pos = 0;
			int32 totalLength = txtModel->TotalLength() - 1;
			while(pos < totalLength){
				int32 count = 0;
				UID paraStyle = paraStrand->GetStyleUID(pos, &count);
				if(formeStyles->DoesNotContain(paraStyle))
					formeStyles->Append(paraStyle);
				pos += count;
			}
		}

		formeStylesListData->Set(formeStyles);
		
		storyData->Set(UIDRef::gNull);
		txtFrameListData->Set(txtFrameList);
		
		photoListData->Set(photoList);
		creditListData->Set(creditList);
		legendListData->Set(legendList);
		formeItemListData->Set(formeItems);
        fixedContentFrameListData->Set(fixedContentFrameList);

		// Make observer attach to widgets
		dialogObserver->AutoAttach();
		
		if(photoList->Length() > 0)	{
			EnableDefineCreditLegend(kTrue);
			InterfacePtr<IDropDownListController> photoListController (photoListStringData, UseDefaultIID());
			photoListController->Select(IDropDownListController::kBeginning);			
		}
		else{
			EnableDefineCreditLegend(kFalse);
		}
		
		// Add forme adornments
		AddFormeAdornments(formeDataItems);

		// Invalidate document for refreshing adornments
		Utils<ILayoutUIUtils>()->InvalidateViews(dlgContext->GetContextDocument());

		// Pre-flight style validation. Was previously surfaced as a CAlert
		// from the Formes-panel "Créer carton" button, which got annoying
		// fast — render it inline above the OK/Cancel buttons instead.
		// Layout strategy: the .fr ships at the expanded height (with the
		// validation area visible). On a clean validation we hide the
		// widget, anchor the OK/Cancel at the compact y, and shrink the
		// dialog frame so it only grows when there's something to read.
		//
		// Targets are absolute (NOT relative subtraction): the dialog
		// widget's frame state can persist across re-opens, so reading
		// GetFrame() and applying `-= kShrink` would compound and the
		// dialog would shrink further every time the user re-opens it.
		IControlView * validationView = panelCtrlData->FindWidget(kXPGUIValidationMessageWidgetID);
		if(validationView != nil){
			PMString validationMessage;
			XPageUIUtils::ValidateSelectionForCarton(*txtFrameList, validationMessage);

			// Keep these in sync with XPGUI_InDesign.fr (kXPGUINewFormeDialogResourceID).
			const PMReal kExpandedDialogHeight = 310;
			const PMReal kExpandedButtonTop    = 280;
			const PMReal kExpandedButtonBottom = 300;
			const PMReal kCompactDialogHeight  = 225;
			const PMReal kCompactButtonTop     = 195;
			const PMReal kCompactButtonBottom  = 215;

			const bool16 compact = validationMessage.IsEmpty();
			if(compact){
				validationView->HideView();
			}
			else{
				validationView->ShowView();
				InterfacePtr<ITextControlData> validationData(validationView, UseDefaultIID());
				if(validationData != nil){
					validationMessage.SetTranslatable(kFalse);
					validationData->SetString(validationMessage);
				}
			}

			const PMReal buttonTop    = compact ? kCompactButtonTop    : kExpandedButtonTop;
			const PMReal buttonBottom = compact ? kCompactButtonBottom : kExpandedButtonBottom;
			const PMReal dialogHeight = compact ? kCompactDialogHeight : kExpandedDialogHeight;

			IControlView * okView = panelCtrlData->FindWidget(kOKButtonWidgetID);
			if(okView != nil){
				PMRect f = okView->GetFrame();
				f.Top()    = buttonTop;
				f.Bottom() = buttonBottom;
				okView->SetFrame(f);
			}
			IControlView * cancelView = panelCtrlData->FindWidget(kCancelButton_WidgetID);
			if(cancelView != nil){
				PMRect f = cancelView->GetFrame();
				f.Top()    = buttonTop;
				f.Bottom() = buttonBottom;
				cancelView->SetFrame(f);
			}

			// panelCtrlData's boss aggregates IControlView (the panel root
			// view), so query it directly rather than via FindWidget —
			// FindWidget walks descendants and won't return the root.
			// WindowChanged() then propagates new bounds to the host OS window.
			InterfacePtr<IControlView> rootView(panelCtrlData, IID_ICONTROLVIEW);
			if(rootView != nil){
				PMRect f = rootView->GetFrame();
				f.Bottom() = f.Top() + dialogHeight;
				rootView->SetFrame(f);
				rootView->WindowChanged();
			}
		}

	} while(kFalse);
}

/* ClosingDialog
*/
void XPGUIXRailNewFormeDialogController::ClosingDialog (IActiveContext* myContext)
{
	// Detach observer
	InterfacePtr<IObserver> dialogObserver (this, IID_INEWFORMEOBSERVER);
	dialogObserver->AutoDetach();

	// Remove forme adornments
	InterfacePtr<IUIDListData> txtFrameListData (this, IID_ITEXTFRAMELIST);
	InterfacePtr<IUIDListData> photoListData (this, IID_IPHOTOLIST);

	UIDList formeDataItems (*txtFrameListData->GetRef());
	formeDataItems.Append(*photoListData->GetRef());

	RemoveFormeAdornments(formeDataItems);

	// Cancel path: roll the per-block IFormeData snapshot back, otherwise
	// the destructive reset+rewrite InitializeDialogFields performed at
	// dialog open would leave the blocks stripped of their carton link —
	// the user could no longer re-open the dialog with the same selection
	// and find the carton hydrated. On the OK path mWasApplied is set in
	// ApplyDialogFields and we leave CreateForme's writes alone.
	if(!mWasApplied)
		RestoreSavedFormeData();

	mSavedItems.clear();
	mSavedDB = nil;
}

/* RestoreSavedFormeData
   Walk the snapshot built in InitializeDialogFields and write each block's
   IFormeData back to its original (uniqueName, type, photoIndex,
   matchingTags). Uses XPGSetFormeDataCmd directly because XPageUtils'
   SetFormeData / SetFormeDataOnStory wrappers don't surface matchingTags,
   and we need all four fields restored to faithfully recover the carton
   link plus per-block credit/legend matching styles.
*/
void XPGUIXRailNewFormeDialogController::RestoreSavedFormeData()
{
	if(mSavedDB == nil || mSavedItems.empty()) return;

	for(int32 i = 0; i < mSavedItems.size(); ++i){
		const SavedFormeData& saved = mSavedItems[i];

		InterfacePtr<ICommand> cmd(CmdUtils::CreateCommand(kXPGSetFormeDataCmdBoss));
		if(cmd == nil) continue;
		cmd->SetItemList(UIDList(mSavedDB, saved.itemUID));
		InterfacePtr<IFormeData> cmdData(cmd, UseDefaultIID());
		if(cmdData == nil) continue;
		cmdData->SetUniqueName(saved.uniqueName);
		cmdData->SetType(saved.type);
		cmdData->SetPhotoIndex(saved.photoIndex);
		cmdData->SetMatchingTagsNames(saved.matchingTags);
		CmdUtils::ProcessCommand(cmd);
	}

	// Repaint so any adornments owned by other systems (placed forme
	// adornments etc.) reflect the restored state.
	InterfacePtr<IDocument> doc(mSavedDB, mSavedDB->GetRootUID(), UseDefaultIID());
	if(doc != nil)
		Utils<ILayoutUIUtils>()->InvalidateViews(doc);
}


/* ValidateFields
*/
WidgetID XPGUIXRailNewFormeDialogController::ValidateDialogFields( IActiveContext* myContext){

	WidgetID result = kNoInvalidWidgets;
	InterfacePtr<IPanelControlData> panelCtrlData (this, UseDefaultIID());
	// Get forme name
	PMString formeName = GetTextControlData(kXPGUIFormeNameWidgetID);
	if(formeName == kNullString)
		result = kXPGUIFormeNameWidgetID;
	
	if(!XPageUIUtils::IsValidName(formeName)){
		CAlert::InformationAlert(kXPGUIInvalidFormeNameKey);
		return kXPGUIFormeNameWidgetID;
	}
	// Get selected rubrique
	InterfacePtr<IDropDownListController> rubriqueListController (panelCtrlData->FindWidget(kXPGUIRubriqueListWidgetID), UseDefaultIID());
	int32 selectedRubrique = rubriqueListController->GetSelected();
	if(selectedRubrique == IDropDownListController::kNoSelection)
		return kXPGUIRubriqueListWidgetID;	
	
	InterfacePtr<IUIDListData> photoListData (this, IID_IPHOTOLIST);
	int32 nbPhotos = photoListData->GetRef()->Length();
	InterfacePtr<IUIDListData> creditListData (this, IID_ICREDITLIST);
	InterfacePtr<IUIDListData> legendListData (this, IID_ILEGENDLIST);
	IDataBase * db = ::GetDataBase(this);
	// Defensive: with the new workflow (checkbox = bind), the
	// "placeholder" state (creditList[i] == photoUID) is never set, so
	// these branches are dead in practice. Kept as a safety net for any
	// pre-existing carton that might somehow carry the placeholder ;
	// returns the checkbox widget ID since the previous Définir buttons
	// no longer exist.
	for(int32 i = 0 ; i < nbPhotos ; ++i){
		if(creditListData->GetItemRef(i).GetUID() ==  photoListData->GetItemRef(i).GetUID()){
			CAlert::InformationAlert("Vous n'avez pas correctement défini les crédits");
			return kXPGUICreditPhotoWidgetID;
		}
		if(legendListData->GetItemRef(i).GetUID() ==  photoListData->GetItemRef(i).GetUID()){
			CAlert::InformationAlert("Vous n'avez pas correctement défini les légendes");
			return kXPGUILegendPhotoWidgetID;
		}
	}
	// Check unicity of forme name 
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor(GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IFormeDataModel> formeModel (pageMrgAccessor->QueryFormeDataModel()); 
	//InterfacePtr<IStringData> classeurData (this, IID_ICLASSEURNAMEDATA);
	InterfacePtr<IStringListControlData> classeurListData(panelCtrlData->FindWidget(kXPGUIRubriqueListWidgetID), UseDefaultIID());
	PMString classeurName = classeurListData->GetString(selectedRubrique);
	//CAlert::InformationAlert(classeurName);
	if(formeModel->DoesFormeExist(formeName, classeurName))	{
		// The forme already exist, ask user if he wants to replace it
		int16 rep = CAlert::ModalAlert(PMString(kXPGUIReplaceFormeKey),PMString(kSDKDefOKButtonApplicationKey),PMString(kSDKDefCancelButtonApplicationKey),kNullString,1,CAlert::eWarningIcon) ;
		if (rep == 2) // if the user clicked on Cancel" button
			return kXPGUIFormeNameWidgetID;
	}
	return result;
}

/* ApplyFields
*/
void XPGUIXRailNewFormeDialogController::ApplyDialogFields( IActiveContext* myContext, const WidgetID& widgetId)
{
	do
	{
		InterfacePtr<IPanelControlData> panelCtrlData (this, UseDefaultIID());

		// Get forme name
		PMString formeName = GetTextControlData(kXPGUIFormeNameWidgetID);
		
		InterfacePtr<IUIDListData> txtFrameListData (this, IID_ITEXTFRAMELIST);		
		if(txtFrameListData->GetRef()->IsEmpty()) // Should not occur
			break;

		InterfacePtr<IUIDListData>  photoListData  (this, IID_IPHOTOLIST);
		InterfacePtr<IUIDData>      storyData      (this, IID_ISTORYDATA);
		InterfacePtr<IUIDListData>  creditListData (this, IID_ICREDITLIST);
		InterfacePtr<IUIDListData>  legendListData (this, IID_ILEGENDLIST);
		InterfacePtr<IUIDListData>  formeItemListData (this, IID_IFORMEITEMLIST);
		InterfacePtr<IUIDListData>  formeStylesListData (this, IID_IFORMESTYLESLIST);
        InterfacePtr<IUIDListData>  fixedContentFrameListData  (this, IID_IFIXEDCONTENTFRAMELIST);
			
		// recuperation des extremites de la forme
		PMPoint leftTopMost(kMaxInt32, kMaxInt32), rightBottomMost(-kMaxInt32, -kMaxInt32);
		UIDList newPageItems (*(formeItemListData->GetRef()));
		IDataBase * db = newPageItems.GetDataBase();
		UID ownerPageUID;
		for(int32 i = 0 ; i < newPageItems.Length() ; ++i) {
			
			InterfacePtr<IHierarchy> hier (db, newPageItems[i], UseDefaultIID());
			if(hier == nil)
				continue;
			ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(hier);
			InterfacePtr<IGeometry> ownerPageGeo (db, ownerPageUID, UseDefaultIID());
			if (ownerPageGeo == nil)
				continue;
		
			
			InterfacePtr<IGeometry> itemGeo ( newPageItems.GetRef(i), UseDefaultIID());
			PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();
			PMPoint rightBottom = itemGeo->GetStrokeBoundingBox().RightBottom();
			::TransformInnerPointToPasteboard (itemGeo, &leftTop);
			::TransformInnerPointToPasteboard (itemGeo, &rightBottom);

			if(leftTop.X() < leftTopMost.X())
				leftTopMost.X() = leftTop.X();
			if(leftTop.Y() < leftTopMost.Y())
				leftTopMost.Y() = leftTop.Y();
		
			if(rightBottom.X() > rightBottomMost.X())
				rightBottomMost.X() = rightBottom.X();
			if(rightBottom.Y() > rightBottomMost.Y())
				rightBottomMost.Y() = rightBottom.Y();
		}
		
		// Conversion en millim￨tres des dimensions
		InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));		

		PMString largeur, hauteur, posx, posy;
		largeur.AppendNumber(uom->PointsToUnits(rightBottomMost.X() - leftTopMost.X()),3,kTrue,kTrue);
		hauteur.AppendNumber(uom->PointsToUnits(rightBottomMost.Y() - leftTopMost.Y()),3,kTrue,kTrue);
		posx.AppendNumber(leftTopMost.X());
		posy.AppendNumber(leftTopMost.Y());

		// Get classeur name
		PMString classeurName = kNullString;
		InterfacePtr<IDropDownListController> classeursListController (panelCtrlData->FindWidget(kXPGUIRubriqueListWidgetID), UseDefaultIID());
		int32 selectedIndex = classeursListController->GetSelected();			
		if(selectedIndex != IDropDownListController::kNoSelection){
			InterfacePtr<IStringListControlData> classeursListData (classeursListController, UseDefaultIID());
			classeurName = classeursListData->GetString(selectedIndex);
		}
		
		// Get matching list styles with tags
		InterfacePtr<IKeyValueData> formeMatchingStyleData (this, IID_IFORMEMATCHINGSTYLESLIST);

		// Estimate Numbers of characters
		int32 nbSignesEstim = 0, nbLinesEstim = 0, nbWordsEstim = 0, nbParaEstim = 0;
		Utils<IXPageUtils>()->GetStatisticsText(*txtFrameListData->GetRef(), nbSignesEstim, nbParaEstim, nbWordsEstim, nbLinesEstim);

		// Read "Carton photo" checkbox state — flags this carton as one whose
		// purpose is to add images to other cartons. Persisted as an attribute
		// on the descriptif XML, see XPGFileSystemFormeModel::CreateForme.
		bool16 isPhotoCarton = kFalse;
		InterfacePtr<ITriStateControlData> chkboxCartonPhoto (panelCtrlData->FindWidget(kXPGUICartonPhotoWidgetID), UseDefaultIID());
		if (chkboxCartonPhoto != nil)
			isPhotoCarton = (chkboxCartonPhoto->GetState() == ITriStateControlData::kSelected);

		// Photo / Crédit / Légende lists nettoyage avant CreateForme :
		// HandleChkBoxFixedContentStateChanged a marqué certains blocs en
		// kFixedContent in-place, mais ils sont restés dans photoList (et
		// leur crédit/légende dans creditList/legendList). Si on laisse
		// CreateForme tel quel, le snippet sauvegardé reproduit cet état
		// pourri (la légende reste liée au slot d'une photo qui n'existe
		// plus dans la séquence). On reconstruit donc les 3 listes en
		// parallèle, en sautant les slots dont la photo est passée Fixed,
		// et on démote les crédits/légendes orphelins en kNoType.
		IDataBase * dbCarton = formeItemListData->GetRef()->GetDataBase();
		UIDList cleanPhotoList(dbCarton);
		UIDList cleanCreditList(dbCarton);
		UIDList cleanLegendList(dbCarton);
		{
			const UIDList * origPhotoList  = photoListData->GetRef();
			const UIDList * origCreditList = creditListData->GetRef();
			const UIDList * origLegendList = legendListData->GetRef();
			const int32 nbPhotos = origPhotoList->Length();
			for(int32 i = 0; i < nbPhotos; ++i){
				const UID photoUID  = origPhotoList->At(i);
				const UID creditUID = origCreditList->At(i);
				const UID legendUID = origLegendList->At(i);

				InterfacePtr<IFormeData> fd(origPhotoList->GetRef(i), UseDefaultIID());
				const bool16 isFixed = (fd != nil && fd->GetType() == IFormeData::kFixedContent);

				if(isFixed){
					// Photo passée Fixed → ne participe plus à la
					// numérotation. Démote son éventuel crédit/légende
					// pour qu'il ne pointe pas dans le vide.
					if(creditUID != kInvalidUID && creditUID != photoUID)
						Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(dbCarton, creditUID), kNullString, IFormeData::kNoType, formeMatchingStyleData->GetKeyValueList());
					if(legendUID != kInvalidUID && legendUID != photoUID)
						Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(dbCarton, legendUID), kNullString, IFormeData::kNoType, formeMatchingStyleData->GetKeyValueList());
					continue;
				}

				cleanPhotoList.Append(photoUID);
				cleanCreditList.Append(creditUID);
				cleanLegendList.Append(legendUID);
			}
		}

		// Create the forme
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IFormeDataModel>   formeModel (pageMrgAccessor->QueryFormeDataModel());
		if(formeModel->CreateForme(formeName, classeurName, formeMatchingStyleData->GetKeyValueList(),
								   *txtFrameListData->GetRef(), cleanPhotoList, *fixedContentFrameListData->GetRef(),
								   storyData->GetRef(), cleanCreditList,
								   cleanLegendList, *formeItemListData->GetRef(), posx, posy, largeur, hauteur, nbSignesEstim,
								   isPhotoCarton)!= kSuccess)
		{

			CAlert::InformationAlert(kXPGUIFailedToCreateFormeKey);
			break;
		}

		// Renumérotage post-création : si l'utilisateur a tagué un bloc
		// en kFixedContent pendant l'édition, le crédit/légende qui lui
		// était lié reste orphelin (toujours kCredit/kLegend pointant
		// sur le slot disparu). RenumberUnit balaie le carton/article,
		// repère ces orphelins et les démote en kNoType.
		// Récupère l'unité depuis n'importe quel bloc du carton — après
		// CreateForme, l'UniqueName (et l'éventuel ID d'article) sont à
		// jour sur tous les items.
		if(formeItemListData->GetRef()->Length() > 0){
			PMString unitId = Utils<IXPageUtils>()->GetUnitId(formeItemListData->GetRef()->GetRef(0));
			if(!unitId.IsEmpty()){
				IDataBase * dbFrame = formeItemListData->GetRef()->GetDataBase();
				InterfacePtr<IDocument> docForRenum(dbFrame, dbFrame->GetRootUID(), UseDefaultIID());
				Utils<IXPageUtils>()->RenumberUnit(docForRenum, unitId);
			}
		}

		// Notify the Formes panel so it rebuilds its tree with the new
		// carton. The post-DisplayNewFormeDialog refresh in
		// XPGUIFormesPanelObserver::CreateForme is unreliable: the dialog
		// is opened kModeless, so WaitForDialog can return before this
		// ApplyDialogFields ever runs and the refresh fires against a
		// model that doesn't know about the new forme yet.
		InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
		if(sessionSubject != nil)
			sessionSubject->Change(kXPGRefreshMsg, IID_IREFRESHPROTOCOL);

		// CreateForme rewrote IFormeData on every block, so the snapshot
		// would be stale; suppress the cancel-rollback in ClosingDialog.
		mWasApplied = kTrue;

	} while(kFalse);
}

/* EnableDefineCreditLegend
*/
void XPGUIXRailNewFormeDialogController::EnableDefineCreditLegend(bool16 doEnable)
{
	// "Définir" buttons (kXPGUIDefineCreditPhotoWidgetID /
	// kXPGUIDefineLegendPhotoWidgetID) are gone — the checkbox itself
	// triggers the bind now (cf. XPGUIXRailNewFormeDialogObserver).
	InterfacePtr<IPanelControlData> panelCtrlData (this, UseDefaultIID());
	IControlView * useCreditWidgetView = panelCtrlData->FindWidget(kXPGUICreditPhotoWidgetID);
	IControlView * useLegendWidgetView = panelCtrlData->FindWidget(kXPGUILegendPhotoWidgetID);
	if(useCreditWidgetView) useCreditWidgetView->Enable(doEnable);
	if(useLegendWidgetView) useLegendWidgetView->Enable(doEnable);
	IControlView * statusCredit = panelCtrlData->FindWidget(kXPGUIStatusCreditPhotoWidgetID);
	IControlView * statusLegend = panelCtrlData->FindWidget(kXPGUIStatusLegendPhotoWidgetID);
	if(statusCredit) statusCredit->ShowView(doEnable);
	if(statusLegend) statusLegend->ShowView(doEnable);

	if(!doEnable && useCreditWidgetView != nil && useLegendWidgetView != nil)
	{
		InterfacePtr<ITriStateControlData> useCreditWidget (useCreditWidgetView, UseDefaultIID());
		InterfacePtr<ITriStateControlData> useLegendWidget (useLegendWidgetView, UseDefaultIID());
		if(useCreditWidget) useCreditWidget->Deselect(kTrue,kFalse);
		if(useLegendWidget) useLegendWidget->Deselect(kTrue,kFalse);
	}
}

/* ResetFormeTypes
*/
void XPGUIXRailNewFormeDialogController::ResetFormeTypes(UIDList formeItems) {
	for(int32 i = 0 ; i < formeItems.Length() ; ++i)
		Utils<IXPageUtils>()->SetFormeData(formeItems.GetRef(i), kNullString, IFormeData::kNoType);
}

/* AddFormeAdornments
*/
void XPGUIXRailNewFormeDialogController::AddFormeAdornments(UIDList formeItems) {
	
	InterfacePtr<ICommand>  addPageItemAdornmentCmd(CmdUtils::CreateCommand(kAddPageItemAdornmentCmdBoss));
	InterfacePtr<IClassIDData>  classIDData(addPageItemAdornmentCmd, UseDefaultIID());
	classIDData->Set(kXPGUIFormeAdornmentBoss);
	addPageItemAdornmentCmd->SetItemList(formeItems);
	CmdUtils::ProcessCommand(addPageItemAdornmentCmd);
}

/* RemoveFormeAdornments
*/
void XPGUIXRailNewFormeDialogController::RemoveFormeAdornments(UIDList formeItems)
{	
	IDataBase * db = formeItems.GetDataBase();
	if(DBUtils::IsValidDataBase(db)) {

		InterfacePtr<ICommand>  removePageItemAdornmentCmd(CmdUtils::CreateCommand(kRemovePageItemAdornmentCmdBoss));
		InterfacePtr<IClassIDData>  classIDData(removePageItemAdornmentCmd, UseDefaultIID());
		classIDData->Set(kXPGUIFormeAdornmentBoss);
		removePageItemAdornmentCmd->SetItemList(formeItems);
		CmdUtils::ProcessCommand(removePageItemAdornmentCmd);
	}
}
