#include "VCPluginHeaders.h"

// Interface includes
#include "IControlView.h"
#include "ISubject.h"
#include "IPanelControlData.h"
#include "IDropDownListController.h"
#include "IStringListControlData.h"
#include "IXPageMgrAccessor.h"
#include "IArticleDataModel.h"
#include "IStringListData.h"
#include "IUIDListData.h"
#include "ITriStateControlData.h"
#include "ILayoutSelectionSuite.h"
#include "ISelectionManager.h"
#include "IXPageSuite.h"
#include "IUIDData.h"
#include "IHierarchy.h"
#include "IMultiColumnTextFrame.h"
#include "IDocument.h"
#include "ILayoutUIUtils.h"
#include "IFormeData.h"
#include "DebugClassUtils.h"
#include "IAssignmentUtils.h"
#include "IKeyValueData.h"
#include "IPageItemTypeUtils.h"  // IsGroup() — for recursive flatten of selection

// General includes
#include "SelectionObserver.h"
#include "FileUtils.h"
#include "CAlert.h"
#include "ErrorUtils.h"
#include "IAttributeStrand.h"
#include "IStyleInfo.h"
#include "IDialogMgr.h"
#include "IDialog.h"
#include "IApplication.h"
#include "RsrcSpec.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "IXPageUtils.h"

// Project includes
#include "XPGUIID.h"
#include "XPGID.h"


/** XPGUIXRailNewFormeDialogObserver

	@author Trias Developpement
*/

class XPGUIXRailNewFormeDialogObserver : public ActiveSelectionObserver
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	XPGUIXRailNewFormeDialogObserver( IPMUnknown* boss );
	/** Destructor
	*/
	virtual ~XPGUIXRailNewFormeDialogObserver() ;
	
	/** 
		Initialises widgets and attaches widget observers.
	*/
	void AutoAttach();

	/** 
		Detaches widget observers. 
	*/
	void AutoDetach();

	/**
		Update is called for all registered observers, and is
		the method through which changes are broadcast. 
		@param theChange this is specified by the agent of change; it can be the class ID of the agent,
		or it may be some specialised message ID.
		@param theSubject this provides a reference to the object which has changed; in this case, the
		widget boss objects that are being observed.
		@param protocol the protocol along which the change occurred.
		@param changedBy this can be used to provide additional information about the change or a reference
		to the boss object that caused the change.
	*/
	void Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy);

protected:

	virtual void HandleSelectionChanged(const ISelectionMessage* message);

private:
	void AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID);
	void DetachWidget(const InterfacePtr<IPanelControlData>&, const WidgetID& widgetID, const PMIID& interfaceID);
     
	void HandleSelectedPhotoChanged(bool16 updateSelection = kFalse);
	void HandleUseCreditLegendStateChanged(const WidgetID& widget, bool16 isSelected);
    void HandleChkBoxFixedContentStateChanged(const WidgetID& widget, bool16 isSelected);
	// Move-with-shift : remove the photo at fromIndex, insert at toIndex,
	// and shift the slots in between by 1. creditList / legendList move
	// in sync. IFormeData::photoIndex is rewritten on every block whose
	// slot changed (photos + linked credit/legend stories).
	void MovePhotoTo(int32 fromIndex, int32 toIndex);
    
    int32 selectedPhotoIndex;
	IDataBase * db; 

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUIXRailNewFormeDialogObserver, kXPGUIXRailNewFormeDialogObserverImpl)

/* Constructor
*/
XPGUIXRailNewFormeDialogObserver::XPGUIXRailNewFormeDialogObserver( IPMUnknown* boss ) :
								ActiveSelectionObserver( boss, IID_INEWFORMEOBSERVER ),
								selectedPhotoIndex(-1) {

	db = ::GetDataBase(this);
}

/* Destructor
*/
XPGUIXRailNewFormeDialogObserver::~XPGUIXRailNewFormeDialogObserver(){

	
}

/* AutoAttach
*/
void XPGUIXRailNewFormeDialogObserver::AutoAttach(){
	ActiveSelectionObserver::AutoAttach();
	do{
		// Initialise widget state.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)
			break;
		
		// Reset selected photo index
		selectedPhotoIndex = -1;

		// Get notification when rubrique changes
		AttachWidget(panelControlData, kXPGUIRubriqueListWidgetID, 		IID_ISTRINGLISTCONTROLDATA);
		AttachWidget(panelControlData, kXPGUIPhotoListWidgetID, 		IID_ISTRINGLISTCONTROLDATA);
		// Position dropdown next to the photo dropdown — picking a slot
		// triggers a move-with-shift via MovePhotoTo().
		AttachWidget(panelControlData, kXPGUIPhotoPositionListWidgetID,	IID_ISTRINGLISTCONTROLDATA);
		// Crédit / Légende checkboxes — checking acts as "Définir" (uses
		// the currently-selected text frame), unchecking clears the link.
		// The previous separate "Définir" buttons (kXPGUIDefineCreditPhotoWidgetID /
		// kXPGUIDefineLegendPhotoWidgetID) have been removed from the .fr.
		AttachWidget(panelControlData, kXPGUICreditPhotoWidgetID, 		IID_ITRISTATECONTROLDATA);
		AttachWidget(panelControlData, kXPGUILegendPhotoWidgetID, 		IID_ITRISTATECONTROLDATA);
        AttachWidget(panelControlData, kXPGUIFixedContentWidgetID,      IID_ITRISTATECONTROLDATA);
	} while(false);
	
}

/* AutoDetach
*/
void XPGUIXRailNewFormeDialogObserver::AutoDetach()
{
	ActiveSelectionObserver::AutoDetach();

	do{
		// Detach widget observers.
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		if (panelControlData == nil)		
			break;		

		DetachWidget(panelControlData, kXPGUIRubriqueListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		DetachWidget(panelControlData, kXPGUIPhotoListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		DetachWidget(panelControlData, kXPGUIPhotoPositionListWidgetID, IID_ISTRINGLISTCONTROLDATA);
		DetachWidget(panelControlData, kXPGUICreditPhotoWidgetID, IID_ITRISTATECONTROLDATA);
		DetachWidget(panelControlData, kXPGUILegendPhotoWidgetID, IID_ITRISTATECONTROLDATA);
        DetachWidget(panelControlData, kXPGUIFixedContentWidgetID, IID_ITRISTATECONTROLDATA);
	} while(false);
}

/* Update
*/
void XPGUIXRailNewFormeDialogObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy)
{	
	ActiveSelectionObserver::Update(theChange, theSubject, protocol, changedBy);
	InterfacePtr<IControlView> controlView(theSubject, UseDefaultIID());	
	if ((protocol == IID_ISTRINGLISTCONTROLDATA) && (theChange == kPopupChangeStateMessage)){
		if(controlView != nil){
			WidgetID widgetID = controlView->GetWidgetID();
			switch (widgetID.Get())	{

				case kXPGUIPhotoListWidgetID :
					HandleSelectedPhotoChanged(kTrue);
					break;

				case kXPGUIPhotoPositionListWidgetID : {
					// User picked a target position. Move the currently-
					// selected photo to that slot, shifting the others.
					if(selectedPhotoIndex == -1)
						break;
					InterfacePtr<IPanelControlData> panelData(this, UseDefaultIID());
					InterfacePtr<IDropDownListController> posCtrl(panelData->FindWidget(kXPGUIPhotoPositionListWidgetID), UseDefaultIID());
					if(posCtrl == nil)
						break;
					int32 targetIndex = posCtrl->GetSelected();
					if(targetIndex == IDropDownListController::kNoSelection || targetIndex == selectedPhotoIndex)
						break;
					MovePhotoTo(selectedPhotoIndex, targetIndex);
					break;
				}

				default:
					break;
			}
		}
	}
	else if ((protocol == IID_ITRISTATECONTROLDATA)){
		if(controlView != nil){
			WidgetID widgetID = controlView->GetWidgetID();			
			switch (widgetID.Get())	{

				case kXPGUICreditPhotoWidgetID :

				case kXPGUILegendPhotoWidgetID : 
					HandleUseCreditLegendStateChanged(widgetID, theChange == kTrueStateMessage); break;
				 	break;
                    
                case kXPGUIFixedContentWidgetID :
                    HandleChkBoxFixedContentStateChanged(widgetID, theChange == kTrueStateMessage); break;
                    break;
				
				default:
					break;
			}
		}
	}
}		


/* AttachWidget
*/
void XPGUIXRailNewFormeDialogObserver::AttachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID){
	do{
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)		
			break;	

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
			break;
		
		subject->AttachObserver(this, interfaceID, IID_INEWFORMEOBSERVER);
	}
	while(kFalse);
}

/* DetachWidget
*/
void XPGUIXRailNewFormeDialogObserver::DetachWidget(const InterfacePtr<IPanelControlData>& panelControlData, const WidgetID& widgetID, const PMIID& interfaceID){
	do{
		IControlView* controlView = panelControlData->FindWidget(widgetID);
		if (controlView == nil)	
			break;

		InterfacePtr<ISubject> subject(controlView, UseDefaultIID());
		if (subject == nil)
			break;

		subject->DetachObserver(this, interfaceID, IID_INEWFORMEOBSERVER);

	}while(kFalse);
}

void XPGUIXRailNewFormeDialogObserver::HandleSelectionChanged(const ISelectionMessage* message){
	do{
		InterfacePtr<IXPageSuite> xpgSuite (fCurrentSelection, IID_IXPAGESUITE);
		if(xpgSuite == nil) // nil means no active selection
			break;

		UIDList textFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);
        
        // GD 27.04.2023 ++ (multi-selection / groups: later edit)
        // Enable the "Contenu Fixe" checkbox whenever the user has at least
        // one block selected — including groups, multiple frames, or a mix.
        // The actual apply path (HandleChkBoxFixedContentStateChanged)
        // walks the selection recursively and only touches blocks that
        // belong to the forme being created, so it's safe to let the user
        // tick the box even with a heterogeneous selection.
        //
        // The checkbox initial state is only auto-set in the single-block
        // case (where we can read it back from fixedContentFrameListData).
        // For multi-selection we leave it deselected — the user ticks to
        // apply to all, unticks to revert.
        InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
        InterfacePtr<ITriStateControlData> chkboxFixedContentWidget (panelControlData->FindWidget(kXPGUIFixedContentWidgetID), UseDefaultIID());
        InterfacePtr<IControlView> controlViewFixedContentWidget (panelControlData->FindWidget(kXPGUIFixedContentWidgetID), UseDefaultIID());
        if (selectedItems.Length() == 0) {
            chkboxFixedContentWidget->Deselect(kTrue, kFalse);
            controlViewFixedContentWidget->Disable();
        } else {
            controlViewFixedContentWidget->Enable();

            const bool16 monoSelection = ((graphicFrames.Length() == 1) && (textFrames.Length() == 0))
                || ((graphicFrames.Length() == 0) && (textFrames.Length() == 1));
            if (monoSelection) {
                InterfacePtr<IUIDListData> fixedContentFrameListData(this, IID_IFIXEDCONTENTFRAMELIST);
                int32 indexFound = -1;
                if (graphicFrames.Length() == 1) {
                    indexFound = fixedContentFrameListData->GetRef()->Location(graphicFrames[0]);
                } else {
                    indexFound = fixedContentFrameListData->GetRef()->Location(textFrames[0]);
                }
                if (indexFound > -1) {
                    chkboxFixedContentWidget->Select(kTrue, kFalse);
                } else {
                    chkboxFixedContentWidget->Deselect(kTrue, kFalse);
                }
            } else {
                chkboxFixedContentWidget->Deselect(kTrue, kFalse);
            }
        }
        // GD 27.04.2023 --
        
		if(graphicFrames.Length() != 1)
			break;

		// Get selected graphic frame index
		InterfacePtr<IUIDListData> photoListData (this, IID_IPHOTOLIST);
		int32 index = photoListData->GetRef()->Location(graphicFrames[0]);
		if(index == -1)
			break;

		// Update selected index in photo list
        // GD 27.04.2023 ligne ici remontee plus haut : InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		InterfacePtr<IDropDownListController> photoListController (panelControlData->FindWidget(kXPGUIPhotoListWidgetID), UseDefaultIID());
		photoListController->Select(index, kTrue, kFalse);

		// Update widgets
		HandleSelectedPhotoChanged();

	} while(kFalse);
}

void XPGUIXRailNewFormeDialogObserver::HandleSelectedPhotoChanged(bool16 updateSelection){

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	do{
		InterfacePtr<IDropDownListController> photoListController (panelControlData->FindWidget(kXPGUIPhotoListWidgetID), UseDefaultIID());	
		selectedPhotoIndex = photoListController->GetSelected();
		if(selectedPhotoIndex == IDropDownListController::kNoSelection)
			break;

		InterfacePtr<IUIDListData> photoListData(this, IID_IPHOTOLIST);
		InterfacePtr<IUIDListData> creditListData(this, IID_ICREDITLIST);
		InterfacePtr<IUIDListData> legendListData(this, IID_ILEGENDLIST);

		

		// Get related photo UID
		UID photoUID = photoListData->GetItemRef(selectedPhotoIndex).GetUID();
		UID creditUID = creditListData->GetItemRef(selectedPhotoIndex).GetUID();
		UID legendUID = legendListData->GetItemRef(selectedPhotoIndex).GetUID();

		// Set widgets state. The "Définir" buttons (defineCredit /
		// defineLegend) have been removed — the checkbox itself is the
		// trigger now (cf. HandleUseCreditLegendStateChanged below).
		InterfacePtr<ITriStateControlData> useCreditWidget (panelControlData->FindWidget(kXPGUICreditPhotoWidgetID), UseDefaultIID());
		InterfacePtr<ITriStateControlData> useLegendWidget (panelControlData->FindWidget(kXPGUILegendPhotoWidgetID), UseDefaultIID());
		IControlView * statusCredit = panelControlData->FindWidget(kXPGUIStatusCreditPhotoWidgetID);
		IControlView * statusLegend = panelControlData->FindWidget(kXPGUIStatusLegendPhotoWidgetID);

		if(creditUID == kInvalidUID){
			useCreditWidget->Deselect(kTrue, kFalse);
			statusCredit->HideView();
			statusCredit->SetRsrcID(kXPGUIKOIconResourceID);
		}
		else{
			useCreditWidget->Select(kTrue, kFalse);
			statusCredit->ShowView();
			if(creditUID != photoUID)
				statusCredit->SetRsrcID(kXPGUIOKIconResourceID);
			else
				statusCredit->SetRsrcID(kXPGUIKOIconResourceID);

			statusCredit->Invalidate();
		}
		if(legendUID == kInvalidUID){
			useLegendWidget->Deselect(kTrue, kFalse);
			statusLegend->HideView();
			statusLegend->SetRsrcID(kXPGUIKOIconResourceID);
		}
		else{
			useLegendWidget->Select(kTrue, kFalse);
			statusLegend->ShowView();

			if(legendUID != photoUID)
				statusLegend->SetRsrcID(kXPGUIOKIconResourceID);
			else
				statusLegend->SetRsrcID(kXPGUIKOIconResourceID);
			statusLegend->Invalidate();
		}
		// Select graphic frame that corresponds to the photo
		if(updateSelection)	{
			fCurrentSelection->DeselectAll(nil);
			InterfacePtr<ILayoutSelectionSuite> layoutSuite (fCurrentSelection, UseDefaultIID());
			layoutSuite->SelectPageItems(UIDList(photoListData->GetDataBase(), photoUID), Selection::kReplace, Selection::kDontScrollLayoutSelection);
		}

		// Mirror the photo dropdown's selection on the Position dropdown
		// so the user always sees the current slot of the photo they're
		// working on. Silent (notify=kFalse) — picking the same slot in
		// Position is a no-op anyway, but better not to fire MovePhotoTo
		// from a passive sync.
		InterfacePtr<IDropDownListController> posListController(panelControlData->FindWidget(kXPGUIPhotoPositionListWidgetID), UseDefaultIID());
		if(posListController && posListController->GetSelected() != selectedPhotoIndex)
			posListController->Select(selectedPhotoIndex, kTrue, kFalse);

	}while(kFalse);
}

// New workflow (GD): the Crédit / Légende checkbox itself is the "Définir"
// trigger. Checking = bind the currently-selected text frame to the photo
// from the dropdown ; unchecking = clear that binding. The previous
// separate "Définir" buttons have been removed from the resource.
//
// Validation on check :
//   * a photo must be selected in the dropdown (selectedPhotoIndex != -1) ;
//   * exactly one text frame in the current selection ;
//   * that text frame must belong to the carton being created ;
//   * its story must not already be the article story, nor already used
//     as a credit or a legend on another photo.
// On any failure we surface a CAlert and silently revert the checkbox via
// Deselect(...,kFalse) so the user sees the rejection without firing the
// observer recursively.
void XPGUIXRailNewFormeDialogObserver::HandleUseCreditLegendStateChanged(const WidgetID& widget, bool16 isSelected){

	InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
	do{
		if(selectedPhotoIndex == -1)
			break;

		InterfacePtr<IUIDListData> photoListData(this, IID_IPHOTOLIST);
		UID photoUID = photoListData->GetItemRef(selectedPhotoIndex).GetUID();

		InterfacePtr<IUIDListData> creditListData(this, IID_ICREDITLIST);
		InterfacePtr<IUIDListData> legendListData(this, IID_ILEGENDLIST);
		InterfacePtr<IUIDListData> frameListData = (widget == kXPGUICreditPhotoWidgetID)
			? InterfacePtr<IUIDListData>(this, IID_ICREDITLIST)
			: InterfacePtr<IUIDListData>(this, IID_ILEGENDLIST);

		if(isSelected){
			// "Définir" path — bind the currently-selected text frame.
			InterfacePtr<IXPageSuite> xpgSuite(fCurrentSelection, IID_IXPAGESUITE);
			if(xpgSuite == nil){
				InterfacePtr<ITriStateControlData> chkbox(panelControlData->FindWidget(widget), UseDefaultIID());
				if(chkbox) chkbox->Deselect(kTrue, kFalse);
				break;
			}

			UIDList textFrames, graphicFrames, selectedItems;
			xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);

			InterfacePtr<ITriStateControlData> chkbox(panelControlData->FindWidget(widget), UseDefaultIID());

			if(textFrames.Length() != 1){
				CAlert::InformationAlert(kXPGUINoTextFrameSelectedKey);
				if(chkbox) chkbox->Deselect(kTrue, kFalse);
				break;
			}

			InterfacePtr<IUIDListData> txtFrameListData(this, IID_ITEXTFRAMELIST);
			if(txtFrameListData->GetRef()->Location(textFrames[0]) == -1){
				CAlert::InformationAlert(kXPGUISelectionDontBelongToFormeKey);
				if(chkbox) chkbox->Deselect(kTrue, kFalse);
				break;
			}

			InterfacePtr<IHierarchy> txtFrameHier(textFrames.GetRef(0), UseDefaultIID());
			InterfacePtr<IMultiColumnTextFrame> txtFrame(textFrames.GetDataBase(), txtFrameHier->GetChildUID(0), UseDefaultIID());
			UID selectedStoryUID = txtFrame->GetTextModelUID();

			InterfacePtr<IUIDData> storyData(this, IID_ISTORYDATA);
			if(selectedStoryUID == storyData->GetItemUID()){
				CAlert::InformationAlert(kXPGUITextFrameAlreadyDefined2Key);
				if(chkbox) chkbox->Deselect(kTrue, kFalse);
				break;
			}

			if(creditListData->GetRef()->Location(selectedStoryUID) != -1
				|| legendListData->GetRef()->Location(selectedStoryUID) != -1){
				CAlert::InformationAlert(kXPGUITextFrameAlreadyDefinedKey);
				if(chkbox) chkbox->Deselect(kTrue, kFalse);
				break;
			}

			// Apply
			IDataBase * dbFrame = textFrames.GetDataBase();
			InterfacePtr<IKeyValueData> formeMatchingStyleData(this, IID_IFORMEMATCHINGSTYLESLIST);
			UIDList * copy = new UIDList(*frameListData->GetRef());
			UID oldStoryUID = frameListData->GetRef()->At(selectedPhotoIndex);
			copy->Replace(selectedPhotoIndex, selectedStoryUID);
			frameListData->Set(copy);

			const int16 newType = (widget == kXPGUICreditPhotoWidgetID) ? IFormeData::kCredit : IFormeData::kLegend;

			// Wipe the type on a previously-bound story (if it wasn't the
			// photo placeholder UID) so its old adornment stops drawing.
			if(oldStoryUID != kInvalidUID && oldStoryUID != photoUID)
				Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(dbFrame, oldStoryUID), kNullString, IFormeData::kNoType, formeMatchingStyleData->GetKeyValueList());

			Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(dbFrame, selectedStoryUID), kNullString, newType, formeMatchingStyleData->GetKeyValueList(), selectedPhotoIndex);

			HandleSelectedPhotoChanged();
			InterfacePtr<IDocument> doc(dbFrame, dbFrame->GetRootUID(), UseDefaultIID());
			Utils<ILayoutUIUtils>()->InvalidateViews(doc);
		}
		else{
			// Unchecking — clear the binding (existing behaviour).
			UIDList * copy = new UIDList(*frameListData->GetRef());
			UID oldCreditOrLegendStoryUID = frameListData->GetRef()->At(selectedPhotoIndex);
			copy->Replace(selectedPhotoIndex, kInvalidUID);
			frameListData->Set(copy);

			IDataBase * dbFrame = frameListData->GetRef()->GetDataBase();
			InterfacePtr<IKeyValueData> formeMatchingStyleData(this, IID_IFORMEMATCHINGSTYLESLIST);

			if(formeMatchingStyleData && oldCreditOrLegendStoryUID != kInvalidUID && oldCreditOrLegendStoryUID != photoUID){
				Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(db, oldCreditOrLegendStoryUID), kNullString, IFormeData::kNoType, formeMatchingStyleData->GetKeyValueList());
				InterfacePtr<IDocument> doc(dbFrame, dbFrame->GetRootUID(), UseDefaultIID());
				Utils<ILayoutUIUtils>()->InvalidateViews(doc);
			}
			HandleSelectedPhotoChanged();
		}

	}while(kFalse);
}

// Move-with-shift : remove the photo at `fromIndex`, insert it at
// `toIndex`, and shift the photos in between by 1. creditList /
// legendList move in lockstep — they are parallel lists indexed by
// photo slot. After the rearrangement, IFormeData::photoIndex is
// rewritten on every block in the affected range (the moved photo +
// the photos that shifted + their linked credit/legend stories) so
// the adornment numbering tracks the dropdown ordering.
//
// Triggered by the user picking a slot in the Position dropdown.
// Range-checked defensively even though Update() filters early.
void XPGUIXRailNewFormeDialogObserver::MovePhotoTo(int32 fromIndex, int32 toIndex){
	do{
		InterfacePtr<IPanelControlData> panelControlData(this, UseDefaultIID());
		InterfacePtr<IUIDListData> photoListData(this, IID_IPHOTOLIST);
		InterfacePtr<IUIDListData> creditListData(this, IID_ICREDITLIST);
		InterfacePtr<IUIDListData> legendListData(this, IID_ILEGENDLIST);
		InterfacePtr<IKeyValueData> formeMatchingStyleData(this, IID_IFORMEMATCHINGSTYLESLIST);

		const int32 n = photoListData->GetRef()->Length();
		if(fromIndex < 0 || fromIndex >= n || toIndex < 0 || toIndex >= n || fromIndex == toIndex)
			break;

		UIDList * photoCopy  = new UIDList(*photoListData->GetRef());
		UIDList * creditCopy = new UIDList(*creditListData->GetRef());
		UIDList * legendCopy = new UIDList(*legendListData->GetRef());

		// Stash the moved photo + its linked credit/legend stories.
		const UID savedPhoto  = photoCopy->At(fromIndex);
		const UID savedCredit = creditCopy->At(fromIndex);
		const UID savedLegend = legendCopy->At(fromIndex);

		// Shift the slots between fromIndex and toIndex by one position.
		if(fromIndex < toIndex){
			// Move down : slots [fromIndex+1, toIndex] shift up by 1.
			for(int32 i = fromIndex; i < toIndex; ++i){
				photoCopy->Replace(i,  photoCopy->At(i + 1));
				creditCopy->Replace(i, creditCopy->At(i + 1));
				legendCopy->Replace(i, legendCopy->At(i + 1));
			}
		}
		else{
			// Move up : slots [toIndex, fromIndex-1] shift down by 1.
			for(int32 i = fromIndex; i > toIndex; --i){
				photoCopy->Replace(i,  photoCopy->At(i - 1));
				creditCopy->Replace(i, creditCopy->At(i - 1));
				legendCopy->Replace(i, legendCopy->At(i - 1));
			}
		}

		// Drop the moved photo at its new slot.
		photoCopy->Replace(toIndex,  savedPhoto);
		creditCopy->Replace(toIndex, savedCredit);
		legendCopy->Replace(toIndex, savedLegend);

		photoListData->Set(photoCopy);
		creditListData->Set(creditCopy);
		legendListData->Set(legendCopy);

		IDataBase * dbFrame = photoListData->GetRef()->GetDataBase();

		// Rewrite IFormeData on every block whose slot changed — that's
		// the inclusive range [min(from,to), max(from,to)]. We touch the
		// photo itself and any linked credit/legend story.
		const int32 lo = (fromIndex < toIndex) ? fromIndex : toIndex;
		const int32 hi = (fromIndex < toIndex) ? toIndex   : fromIndex;
		for(int32 i = lo; i <= hi; ++i){
			const UID photoUID  = photoListData->GetRef()->At(i);
			const UID creditUID = creditListData->GetRef()->At(i);
			const UID legendUID = legendListData->GetRef()->At(i);

			Utils<IXPageUtils>()->SetFormeData(photoListData->GetRef()->GetRef(i), kNullString, IFormeData::kPhoto, i);

			// Skip kInvalidUID (no link) and slots where the list still
			// holds the photo's own UID (legacy placeholder, never set
			// by the new workflow but defensively handled).
			if(creditUID != kInvalidUID && creditUID != photoUID)
				Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(dbFrame, creditUID), kNullString, IFormeData::kCredit, formeMatchingStyleData->GetKeyValueList(), i);
			if(legendUID != kInvalidUID && legendUID != photoUID)
				Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(dbFrame, legendUID), kNullString, IFormeData::kLegend, formeMatchingStyleData->GetKeyValueList(), i);
		}

		// Re-select the moved photo at its new slot. Photo dropdown
		// fires kTrue notify so HandleSelectedPhotoChanged refreshes
		// the layout selection + the credit/legend status icons.
		// Position dropdown is set silently (kFalse notify) — we don't
		// want to recurse into MovePhotoTo from our own update.
		selectedPhotoIndex = toIndex;
		InterfacePtr<IDropDownListController> photoListController(panelControlData->FindWidget(kXPGUIPhotoListWidgetID), UseDefaultIID());
		if(photoListController)
			photoListController->Select(toIndex, kTrue, kTrue);
		InterfacePtr<IDropDownListController> posListController(panelControlData->FindWidget(kXPGUIPhotoPositionListWidgetID), UseDefaultIID());
		if(posListController)
			posListController->Select(toIndex, kTrue, kFalse);

		// Force adornment redraw on every page item we touched.
		InterfacePtr<IDocument> doc(dbFrame, dbFrame->GetRootUID(), UseDefaultIID());
		Utils<ILayoutUIUtils>()->InvalidateViews(doc);
	} while(kFalse);
}

/* FlattenSelection
 *
 * Walk a UIDList of page items, expanding every group recursively into
 * its leaf children. Groups can nest (groups of groups), so we recurse
 * to whatever depth InDesign hands us. Output is the flat list of leaf
 * UIDs (no group items in the output).
*/
static void FlattenSelection(const UIDList& items, UIDList& outFlat)
{
    IDataBase* db = items.GetDataBase();
    const int32 n = items.Length();
    for (int32 i = 0; i < n; ++i)
    {
        UIDRef itemRef = items.GetRef(i);
        if (Utils<IPageItemTypeUtils>()->IsGroup(itemRef))
        {
            InterfacePtr<IHierarchy> hier(itemRef, UseDefaultIID());
            if (hier == nil)
                continue;
            UIDList children(db);
            const int32 nbChildren = hier->GetChildCount();
            for (int32 j = 0; j < nbChildren; ++j)
                children.Append(hier->GetChildUID(j));
            FlattenSelection(children, outFlat);
        }
        else
        {
            outFlat.Append(items[i]);
        }
    }
}

// GD 27.04.2023 — multi-block + recursive group descent (later edit)
void XPGUIXRailNewFormeDialogObserver::HandleChkBoxFixedContentStateChanged(const WidgetID& widget, bool16 isSelected){

    do{
        InterfacePtr<IXPageSuite> xpgSuite (fCurrentSelection, IID_IXPAGESUITE);
        if(xpgSuite == nil) // nil means no active selection
            break;

        UIDList textFrames, graphicFrames, selectedItems;
        xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);

        if (selectedItems.Length() == 0)
        {
            CAlert::InformationAlert("Vous devez sélectionner au moins un bloc texte ou image");
            break;
        }

        IDataBase * dbFrame = selectedItems.GetDataBase();

        // Expand groups recursively — if the user selected a group (or a
        // group of groups), we want every leaf inside it to be tagged.
        UIDList flatItems(dbFrame);
        FlattenSelection(selectedItems, flatItems);

        // Lookup tables for "does this block belong to the forme being
        // created" + the persistent list of fixed-content blocks the
        // dialog tracks.
        InterfacePtr<IUIDListData> txtFrameListData (this, IID_ITEXTFRAMELIST);
        InterfacePtr<IUIDListData> photoListData (this, IID_IPHOTOLIST);
        InterfacePtr<IUIDListData> fixedContentFrameListData (this, IID_IFIXEDCONTENTFRAMELIST);
        UIDList * copy = new UIDList(*fixedContentFrameListData->GetRef());

        int32 nbProcessed = 0;
        const int32 nbFlat = flatItems.Length();
        for (int32 idx = 0; idx < nbFlat; ++idx)
        {
            const UID frameUID = flatItems[idx];
            UIDRef frameRef = flatItems.GetRef(idx);

            // Determine the block's "natural" type (kArticle for text,
            // kPhoto for image) by which forme list it appears in. Blocks
            // outside the current forme are silently skipped — letting
            // the user select 5 frames where 3 are in the forme should
            // tag those 3, not abort the whole operation.
            int16 naturalType = IFormeData::kNoType;
            if (txtFrameListData->GetRef()->Location(frameUID) != -1)
                naturalType = IFormeData::kArticle;
            else if (photoListData->GetRef()->Location(frameUID) != -1)
                naturalType = IFormeData::kPhoto;
            else
                continue; // not part of this forme — skip.

            InterfacePtr<IFormeData> itemFormeData(frameRef, UseDefaultIID());
            if (itemFormeData == nil)
                continue;
            const int currentPhotoIndex = itemFormeData->GetPhotoIndex();

            if (isSelected)
            {
                // Tag as fixed content. Guard against double-Append if the
                // user re-checks while the UID is already in the list
                // (shouldn't happen in normal flow, but safe).
                if (copy->Location(frameUID) == -1)
                    copy->Append(frameUID);
                // SetFormeData targets the spline directly. We deliberately
                // don't use SetFormeDataOnStory here: that helper queries
                // ITextModel on the ref it's given, which is nil on a
                // graphic frame's spline (and not guaranteed on a text
                // frame's spline either), so the IFormeData type would
                // never get written and the "Contenu fixe" adornment
                // wouldn't draw.
                Utils<IXPageUtils>()->SetFormeData(frameRef, kNullString,
                    IFormeData::kFixedContent,
                    currentPhotoIndex);
            }
            else
            {
                const int32 loc = copy->Location(frameUID);
                if (loc != -1)
                    copy->Remove(loc);
                Utils<IXPageUtils>()->SetFormeData(frameRef, kNullString,
                    naturalType,
                    currentPhotoIndex);
            }
            ++nbProcessed;
        }

        if (nbProcessed == 0)
        {
            // None of the selected leaves belonged to the forme — nothing
            // to do, but warn the user so they don't think the checkbox
            // is broken.
            CAlert::InformationAlert(kXPGUISelectionDontBelongToFormeKey);
            delete copy;
            break;
        }

        fixedContentFrameListData->Set(copy);

        // Live renumber : we just toggled some photo blocks between
        // kPhoto and kFixedContent. The surviving kPhoto blocks must
        // be renumbered 0..N-1 (closing the gap left by Fixed photos)
        // and the credits/legends linked to a now-Fixed photo must be
        // demoted in IFormeData *and* dropped from creditList/legendList
        // (otherwise CreateForme at Apply would re-link them).
        //
        // We can't reuse Utils<IXPageUtils>()->RenumberUnit here because
        // dialog hydration wipes IFormeData::UniqueName on every photo
        // (controller passes kNullString to SetFormeData), so GetUnitId
        // would return empty and the doc walk would find nothing. We
        // work directly off the dialog's own parallel lists instead.
        // photoListData is already declared at the top of this function.
        InterfacePtr<IUIDListData> creditListData (this, IID_ICREDITLIST);
        InterfacePtr<IUIDListData> legendListData (this, IID_ILEGENDLIST);
        InterfacePtr<IKeyValueData> formeMatchingStyleData (this, IID_IFORMEMATCHINGSTYLESLIST);

        const UIDList * photoListPtr  = photoListData->GetRef();
        const UIDList * creditListPtr = creditListData->GetRef();
        const UIDList * legendListPtr = legendListData->GetRef();
        const int32 nbPhotos = photoListPtr->Length();

        // Pass 1 — assign a new sequential photoIndex to each surviving
        // kPhoto slot ; -1 means "this slot is now Fixed, skip".
        K2Vector<int16> newIdxBySlot;
        newIdxBySlot.reserve(nbPhotos);
        int16 nextIdx = 0;
        for (int32 slot = 0; slot < nbPhotos; ++slot) {
            InterfacePtr<IFormeData> fd(photoListPtr->GetRef(slot), UseDefaultIID());
            if (fd != nil && fd->GetType() == IFormeData::kPhoto)
                newIdxBySlot.push_back(nextIdx++);
            else
                newIdxBySlot.push_back(-1);
        }

        // Pass 2 — rewrite IFormeData on every photo + linked credit/legend.
        UIDList * creditCopyForLists = nil;
        UIDList * legendCopyForLists = nil;
        for (int32 slot = 0; slot < nbPhotos; ++slot) {
            UIDRef photoRef = photoListPtr->GetRef(slot);
            const UID  photoUID  = photoListPtr->At(slot);
            const UID  creditUID = creditListPtr->At(slot);
            const UID  legendUID = legendListPtr->At(slot);
            const int16 newIdx   = newIdxBySlot[slot];
            const bool16 photoIsLive = (newIdx != -1);

            // Photo : update its photoIndex if it's a live kPhoto.
            // Fixed photos are left untouched (HandleChkBoxFixed... above
            // already wrote kFixedContent on them).
            if (photoIsLive) {
                InterfacePtr<IFormeData> fd(photoRef, UseDefaultIID());
                if (fd != nil && fd->GetPhotoIndex() != newIdx)
                    Utils<IXPageUtils>()->SetFormeData(photoRef, kNullString, IFormeData::kPhoto, newIdx);
            }

            // Credit / Legend : renumber if the photo's still alive,
            // orphan (kNoType) if its photo is Fixed. Skip placeholder
            // entries (creditUID == photoUID) and empty slots.
            if (creditUID != kInvalidUID && creditUID != photoUID) {
                if (photoIsLive) {
                    Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(dbFrame, creditUID), kNullString, IFormeData::kCredit, formeMatchingStyleData->GetKeyValueList(), newIdx);
                }
                else {
                    Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(dbFrame, creditUID), kNullString, IFormeData::kNoType, formeMatchingStyleData->GetKeyValueList());
                    // Drop from the dialog list so Apply doesn't re-link.
                    if (creditCopyForLists == nil)
                        creditCopyForLists = new UIDList(*creditListPtr);
                    creditCopyForLists->Replace(slot, kInvalidUID);
                }
            }
            if (legendUID != kInvalidUID && legendUID != photoUID) {
                if (photoIsLive) {
                    Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(dbFrame, legendUID), kNullString, IFormeData::kLegend, formeMatchingStyleData->GetKeyValueList(), newIdx);
                }
                else {
                    Utils<IXPageUtils>()->SetFormeDataOnStory(UIDRef(dbFrame, legendUID), kNullString, IFormeData::kNoType, formeMatchingStyleData->GetKeyValueList());
                    if (legendCopyForLists == nil)
                        legendCopyForLists = new UIDList(*legendListPtr);
                    legendCopyForLists->Replace(slot, kInvalidUID);
                }
            }
        }

        if (creditCopyForLists != nil) creditListData->Set(creditCopyForLists);
        if (legendCopyForLists != nil) legendListData->Set(legendCopyForLists);

        // Invalidate document so that forme adornments get repainted on
        // every block we just touched.
        InterfacePtr<IDocument> doc (dbFrame, dbFrame->GetRootUID(), UseDefaultIID());
        Utils<ILayoutUIUtils>()->InvalidateViews(doc);

    }while(kFalse);
}
