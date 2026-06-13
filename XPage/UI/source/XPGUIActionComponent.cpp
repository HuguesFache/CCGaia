#include "VCPlugInHeaders.h"

// Interface includes:
#include "IDialog.h"
#include "IDialogMgr.h"
#include "IApplication.h"
#include "IK2ServiceProvider.h"
#include "IK2ServiceRegistry.h"
#include "IPanelControlData.h"
#include "ISelectableDialogSwitcher.h"
#include "IActionIDToStringData.h"
#include "IStringData.h"
#include "ISubject.h"
#include "IActionStateList.h"
#include "ISelectionManager.h"
#include "ISelectionUtils.h"
#include "IXPageSuite.h"
#include "IHierarchy.h"
#include "IFrameList.h"
#include "IOwnedItem.h"
#include "IGeometry.h"
#include "ITextModel.h"
#include "ITransform.h"
#include "IWaxLine.h"
#include "IWaxStrand.h"
#include "IWaxIterator.h"
#include "IWaxLineShape.h"
#include "IWaxLineShape.h"
//#include "IFrameComposer.h"
#include "ITextParcelListComposer.h"
#include "IFrameUtils.h"
#include "IGraphicAttributeUtils.h"
#include "IGraphicStyleDescriptor.h"
#include "IIntData.h"
#include "IPageItemTypeUtils.h"
#include "IFrameListComposer.h"
#include "IBoundsData.h"
#include "ISpread.h"
#include "PMLineSeg.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"
#include "CoreResTypes.h"
#include "IDocument.h"
#include "IActiveContext.h"
#include "ITreeViewController.h"
#include "IWidgetUtils.h"
#include "IObserver.h"
#include "ISubject.h"
#include "ITextControlData.h"
#include "IXPageMgrAccessor.h"
#include "ISubject.h"
#include "IAssignmentMgr.h"
#include "ITextFrameColumn.h"
#include "IGeometryFacade.h"
#include "IXPGPreferences.h"
#include "IGroupItemUtils.h"
#include "ISpread.h"
#include "IUIDListData.h"
#include "IBoolData.h"
#include "IDocumentList.h"
#include "IAssemblageDataModel.h"
#include "IFormeData.h"
#include "IPanelMgr.h"

// General includes:
#include "CActionComponent.h"
#include "CAlert.h"
#include "FileUtils.h"
#include "ILayoutUtils.h"
#include "TransformUtils.h"
#include "ITransformFacade.h"
#include "FileUtils.h"
#include "SDKFileHelper.h"
#include "IXPageUtils.h"
#include "IFormeDataModel.h"
#include "IWebServices.h"

// Project includes:
#include "XPGUIFormeNodeID.h"
#include "XPGUIID.h"
#include "XPageUIUtils.h"
#include "XPGID.h"


/** Implements IActionComponent; performs the actions that are executed when the plug-in's
menu items are selected.


@ingroup xpageui

*/
class XPGUIActionComponent : public CActionComponent
	{
	public:
		/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		XPGUIActionComponent(IPMUnknown* boss);

		/** The action component should perform the requested action.
		This is where the menu item's action is taken.
		When a menu item is selected, the Menu Manager determines
		which plug-in is responsible for it, and calls its DoAction
		with the ID for the menu item chosen.

		@param actionID identifies the menu item that was selected.
		@param ac active context
		@param mousePoint contains the global mouse location at time of event causing action (e.g. context menus). kInvalidMousePoint if not relevant.
		@param widget contains the widget that invoked this action. May be nil. 
		*/
		virtual void DoAction(IActiveContext* ac, ActionID actionID, GSysPoint mousePoint, IPMUnknown* widget);
		void UpdateActionStates(IActiveContext* ac, IActionStateList* listToUpdate, GSysPoint mousePoint = kInvalidMousePoint, IPMUnknown* widget = nil);

	private:


		void DoHandleDynActionID(ActionID id);
		void DoRefresh(ClassID theMsg);
		//void DoJustify(UIDRef frameRef);
		//void DoOrderInCopyArticle();
		void DoNewForme(IPMUnknown* widget);
		void DoDeleteForme(IPMUnknown* widget);
		void DoNewClasseur(IPMUnknown* widget, bool16 classeurAssemblage = kFalse);
		void DoDeleteClasseur(IPMUnknown* widget, bool16 classeurAssemblage = kFalse);
		void DoNewAssemblage(IPMUnknown* widget);
		void DoDeleteAssemblage(IPMUnknown* widget);
				
		ErrorCode ResizeFrame(UIDRef spline, PMReal horOffset, PMReal vertOffset);
        bool16 firstInit;

	};

/* CREATE_PMINTERFACE
Binds the C++ implementation class onto its
ImplementationID making the C++ code callable by the
application.
*/
CREATE_PMINTERFACE(XPGUIActionComponent, kXPGUIActionComponentImpl)

/* XPGUIActionComponent Constructor
*/
XPGUIActionComponent::XPGUIActionComponent(IPMUnknown* boss): CActionComponent(boss),firstInit(kTrue)
	{
	}

/* DoAction
*/
void XPGUIActionComponent::DoAction(IActiveContext* ac, ActionID actionID, GSysPoint mousePoint, IPMUnknown* widget)
	{

	switch (actionID.Get())
		{

#if !INCOPY
		case kXPGUIRefreshAssemblageActionID:
			this->DoRefresh(kXPGRefreshAssemblageMsg);
			break;
		
		case kXPGUINewFormeActionID:
			this->DoNewForme(widget);
			break;

		case kXPGUIDeleteFormeActionID:
			this->DoDeleteForme(widget);
			break;

		case kXPGUINewClasseurActionID:
			this->DoNewClasseur(widget);
			break;

		case kXPGUIDeleteClasseurActionID:
			this->DoDeleteClasseur(widget);
			break;

		case kXPGUINewClasseurAssemblageActionID:
			this->DoNewClasseur(widget, kTrue);
			break;

		case kXPGUIDeleteClasseurAssemblageActionID:
			this->DoDeleteClasseur(widget, kTrue);
			break;

		case kXPGUINewAssemblageActionID:
			this->DoNewAssemblage(widget);
			break;

		case kXPGUIDeleteAssemblageActionID:
			this->DoDeleteAssemblage(widget);
			break;

#endif
		default:            
			this->DoHandleDynActionID(actionID);
			break;		
		}	
}

/* UpdateActionStates
*/
void XPGUIActionComponent::UpdateActionStates(IActiveContext *  ac, IActionStateList *  listToUpdate, GSysPoint  mousePoint, IPMUnknown *  widget) 
{
	int32 i;
	//on recupere le login
	InterfacePtr<IStringData> gLogin (GetExecutionContextSession(), IID_ILOGINSTRING);
	if (gLogin->Get() == "") {
		//pas de login, on desactive tout
		// Gain access to palette manager
		InterfacePtr<IApplication> app(GetExecutionContextSession()->QueryApplication());			
		InterfacePtr<IPanelMgr> panelMgr(app->QueryPanelManager()); 
		// Hide all palettes
		IControlView * pal1 = panelMgr->GetVisiblePanel(kXPGUITextesPanelWidgetID);
		if(pal1) {				
			pal1->Enable(kFalse);
			panelMgr->HidePanelByWidgetID(kXPGUITextesPanelWidgetID);
		}

		
		//on desactive les menus
		for(i = 0 ; i < listToUpdate->Length() ; ++i){
			ActionID theAction = listToUpdate->GetNthAction(i);
			listToUpdate->SetNthActionState(i, kFalse);
		}
	}
	else {
		for(i = 0 ; i < listToUpdate->Length() ; ++i){
			ActionID theAction = listToUpdate->GetNthAction(i);
			switch(theAction.Get()){
				
				case kXPGUINewFormeActionID:
					{		
						int enabled = kDisabled_Unselected;
						// Enable only if there is an active forme selection
						InterfacePtr<IXPageSuite> xpgSuite (ac->GetContextSelection(), IID_IXPAGESUITE);
						if(xpgSuite != nil){ // nil means no active selection => don't activate
							UIDList textFrames, graphicFrames, selectedItems;
							xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);	
							if(!textFrames.IsEmpty()){
								enabled = kEnabledAction;
								if(selectedItems.Length() > 1 && !Utils<IGroupItemUtils>()->CanEnableGroup(&selectedItems))
									enabled = kDisabled_Unselected;
							}	
						}					
						InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
						listToUpdate->SetNthActionState(i, enabled && xpgPrefs->GetCreationForme());

					}
					break;

				case kXPGUINewAssemblageActionID:
					{		
						int enabled = kDisabled_Unselected;
						// Enable only if there is an active forme selection
						InterfacePtr<IXPageSuite> xpgSuite (ac->GetContextSelection(), IID_IXPAGESUITE);
						if(xpgSuite != nil){ // nil means no active selection => don't activate
							UIDList textFrames, graphicFrames, selectedItems;
							xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);	
							if(!textFrames.IsEmpty()){
								enabled = kEnabledAction;
								UIDList allItems(textFrames);
								allItems.Append(graphicFrames);
								for(int32 j = 0 ; j < allItems.Length() ; ++j)
								{
									InterfacePtr<IFormeData> formeData (allItems.GetRef(j), UseDefaultIID());
									if(!formeData || formeData->GetUniqueName() == kNullString)
									{
										enabled = kDisabled_Unselected;
										break;
									}
								}
							}	
						}					
						InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
						listToUpdate->SetNthActionState(i, enabled && xpgPrefs->GetCreationForme());

					}
					break;

				case kXPGUIDeleteAssemblageActionID:
				case kXPGUIDeleteFormeActionID:
					{
						int enabled = kDisabled_Unselected;		
						WidgetID treeViewID = (theAction.Get() == kXPGUIDeleteFormeActionID) ? kXPGUIFormesViewWidgetID : kXPGUIAssemblagesViewWidgetID;
						InterfacePtr<ITreeViewController> formeListController ((ITreeViewController*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, treeViewID, IID_ITREEVIEWCONTROLLER));
						NodeIDList selectedItems;
						formeListController->GetSelectedItems(selectedItems);
						if(selectedItems.size()>0){	
							TreeNodePtr<XPGUIFormeNodeID> nodeID(selectedItems[0]);
							if(nodeID->GetType() == XPGFormeDataNode::kFormeNode)
								enabled = kEnabledAction;
						}		
						InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
						listToUpdate->SetNthActionState(i, enabled && xpgPrefs->GetCreationForme());
					}
					break;

				case kXPGUINewClasseurActionID:
				case kXPGUINewClasseurAssemblageActionID:
					{
						// "New classeur" opens a dialog that prompts for a name —
						// always enabled when classeur creation is allowed at all.
						// (Older code also required the palette dropdown to hold
						// a non-existent name; that left the action greyed out
						// because the dropdown shows an existing classeur in the
						// normal case.)
						InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
						listToUpdate->SetNthActionState(i, xpgPrefs && xpgPrefs->GetCreationForme() ? kEnabledAction : kDisabled_Unselected);
					}
					break;

				case kXPGUIDeleteClasseurActionID:
				case kXPGUIDeleteClasseurAssemblageActionID:
					{
						int enabled = kDisabled_Unselected;	
						InterfacePtr<ITextControlData> classeurData((ITextControlData*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, kXPGUIComboClasseurListWidgetID, IID_ITEXTCONTROLDATA));
						PMString classeurName = classeurData->GetString();

						// Check whether this classeur already exists or not
						InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
						if(theAction.Get() == kXPGUIDeleteClasseurActionID)
						{
							InterfacePtr<IFormeDataModel> model (pageMrgAccessor->QueryFormeDataModel());
							if(classeurName != kNullString && model->DoesClasseurExist(classeurName))
								enabled = kEnabledAction;
						}
						else
						{
							InterfacePtr<IAssemblageDataModel> model (pageMrgAccessor->QueryAssemblageDataModel());
							if(classeurName != kNullString && model->DoesClasseurExist(classeurName))
								enabled = kEnabledAction;
						}

						InterfacePtr<IXPGPreferences> xpgPrefs (GetExecutionContextSession(), UseDefaultIID());
						listToUpdate->SetNthActionState(i, enabled && xpgPrefs->GetCreationForme());
					}
					break;

				default :
					break;    
			}
		}
	}
}
		
	
void XPGUIActionComponent::DoHandleDynActionID(ActionID id){

    if(firstInit){
       
        InterfacePtr<IActionIDToStringData> defaultAction (this, IID_IACTIONIDTOLEVEL1);
        PMString data =defaultAction->GetFirstString();
	    InterfacePtr<IStringData> currentLevel1 (GetExecutionContextSession(), IID_ICURRENTLEVEL1);
		currentLevel1->Set(data);
        firstInit=kFalse;
    }
	
	InterfacePtr<IActionIDToStringData> action2numero (this, IID_IACTIONIDTONUMEROS);
	PMString data = action2numero->GetString(id);
	if(data != kNullString){

		InterfacePtr<IStringData> currentNumero (GetExecutionContextSession(), IID_ICURRENTNUMERO);
		currentNumero->Set(data);
		InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
		sessionSubject->Change (kXPGNumeroChangeMsg, IID_ICURRENTNUMERO);
	}
	else{

		InterfacePtr<IActionIDToStringData> action2level2 (this, IID_IACTIONIDTOLEVEL2);
		data = action2level2->GetString(id);
		if(data != kNullString){
			// data is a second level filter
			InterfacePtr<IStringData> currentLevel2 (GetExecutionContextSession(), IID_ICURRENTLEVEL2);
			currentLevel2->Set(data);

			// reset numero
			InterfacePtr<IStringData> currentNumero (GetExecutionContextSession(), IID_ICURRENTNUMERO);
			currentNumero->Set(kNullString);

			InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
			sessionSubject->Change(kXPGNumeroChangeMsg, IID_ICURRENTNUMERO);
		}
		else {

			InterfacePtr<IActionIDToStringData> action2level1 (this, IID_IACTIONIDTOLEVEL1);
			data = action2level1->GetString(id);

			// data is a first level filter
			InterfacePtr<IStringData> currentLevel1 (GetExecutionContextSession(), IID_ICURRENTLEVEL1);
			currentLevel1->Set(data);

			// reset second level filter and numero
			InterfacePtr<IStringData> currentLevel2 (GetExecutionContextSession(), IID_ICURRENTLEVEL2);
			currentLevel2->Set(kNullString);
			InterfacePtr<IStringData> currentNumero (GetExecutionContextSession(), IID_ICURRENTNUMERO);
			currentNumero->Set(kNullString);

			InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
			sessionSubject->Change(kXPGNumeroChangeMsg, IID_ICURRENTNUMERO);
		}
	}
}

void XPGUIActionComponent::DoRefresh(ClassID theMsg){
	InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
	sessionSubject->Change(theMsg, IID_IREFRESHPROTOCOL);
}

/*void XPGUIActionComponent::DoJustify(UIDRef frameRef)
{
ErrorCode status = kFailure;
PMString error("");
InterfacePtr<ICommandSequence>  seq (nil);
InterfacePtr<IMultiColumnTextFrame> txtFrame (nil);

Text::VerticalJustification vj = Text::kVJInvalid;

IDataBase * db = frameRef.GetDataBase();	
do
{
InterfacePtr<IHierarchy> frameHier (frameRef, UseDefaultIID());

UID ownerPage = ::GetOwnerPageUID(frameHier);

//Get text flags (we already know it is a text frame)
int32 textflags;
Utils<IFrameUtils>()->IsTextFrame(frameHier, &textflags);

// It's a text frame, so get the frame list
InterfacePtr<IHierarchy> multiCol (frameHier->QueryChild(0));

InterfacePtr<IMultiColumnTextFrame> tmptxtFrame (multiCol,UseDefaultIID());	

InterfacePtr<IFrameList> frameList (tmptxtFrame->QueryFrameList());			
InterfacePtr<ITextModel> txtModel (tmptxtFrame->QueryTextModel());

//InterfacePtr<IMultiColumnTextFrame> 
txtFrame = frameList->QueryFrameContaining(txtModel->GetPrimaryStoryThreadSpan()-1,nil);

if(txtFrame == nil)		
frameList = frameList->QueryNthFrame(frameList->GetFrameCount()-1);
// Get the last spline's geometry, in case text frame are linked, to convert refpoint
InterfacePtr<IHierarchy> lastSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
if(lastSpline == nil)
break;

frameRef = ::GetUIDRef(lastSpline);
ownerPage = ::GetOwnerPageUID(lastSpline);

// Set vertical justification to top in order the algorithm to work well			

vj = txtFrame->GetVerticalJustification();
InterfacePtr<ICommand> vertJustCmd(CmdUtils::CreateCommand(kSetFrameVertJustifyCmdBoss)); 

InterfacePtr<IHierarchy> txtFrameSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
vertJustCmd->SetItemList(UIDList(frameRef));

InterfacePtr<IIntData> vertJustCmdIntData(vertJustCmd, UseDefaultIID()); 
vertJustCmdIntData->Set(Text::kVJTop);
status = CmdUtils::ProcessCommand(vertJustCmd);
if(status != kSuccess)
error = ErrorUtils::PMGetGlobalErrorString();

// Recompose whole frame (required after modification of text frame settings)
seq = CmdUtils::BeginCommandSequence();

multiCol = InterfacePtr<IHierarchy> (txtFrameSpline->QueryChild(0));
InterfacePtr<IFrameComposer> frameComposer (multiCol, UseDefaultIID());	
frameComposer->RecomposeThisFrame();	

// if it's an inline, we've got to recompose its parent as well
InterfacePtr<IOwnedItem> inlineItem(nil);
if(Utils<IPageItemTypeUtils>()->IsInline(frameRef))
{
InterfacePtr<IHierarchy> frameHier (frameRef,UseDefaultIID());

// Get parent (kInlineBoss)
InterfacePtr<IOwnedItem> inlineBoss (db, frameHier->GetParentUID(), UseDefaultIID());

inlineItem = inlineBoss;

// Recompose owner model
InterfacePtr<ITextModel> txtModel (inlineBoss->QueryTextModel());

InterfacePtr<ITextParcelListComposer> composer (db, txtModel->GetFrameListUID (), UseDefaultIID());

composer->RecomposeThruTextIndex(txtModel->GetPrimaryStoryThreadSpan()-1);				
}				

PMReal diff;
InterfacePtr<IGeometry> frameGeo (frameRef, UseDefaultIID());

if(Utils<ITextUtils>()->IsOverset(frameList))
{
PMPoint limitLeftBottom;
// Resize the height till the end of the owner page or the owner frame (if it's an inline)
if(Utils<IPageItemTypeUtils>()->IsInline(frameRef))
{	
// Get owner frame, then owner spline

ITextFrameColumn* ownerFrame = inlineItem->QueryFrame() ;

if(ownerFrame == nil)
{
status = kSuccess; // owner frame is overset, inline is not displayed and thus can't be resized
break;
}

InterfacePtr<IHierarchy> ownerSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(ownerFrame));

InterfacePtr<IGeometry> ownerSplineGeo (ownerSpline,UseDefaultIID());

limitLeftBottom = ownerSplineGeo->GetStrokeBoundingBox().LeftBottom();
::TransformInnerPointToPasteboard (ownerSplineGeo, &limitLeftBottom);


}
else
{
InterfacePtr<ISpread> isSpread (db, ownerPage, UseDefaultIID());
if(isSpread)
{
InterfacePtr<IGeometry> spreadGeo (isSpread, UseDefaultIID());
InterfacePtr<ITransform> spreadTransform (spreadGeo, UseDefaultIID());	
limitLeftBottom = spreadGeo->GetStrokeBoundingBox(spreadTransform->GetInnerToParentMatrix()).LeftBottom();
}
else
{
InterfacePtr<IGeometry> pageGeo (db, ownerPage, UseDefaultIID());
limitLeftBottom = pageGeo->GetStrokeBoundingBox().LeftBottom();
::TransformInnerPointToPasteboard(pageGeo,&limitLeftBottom);
}										
}

PMPoint frameLeftBottomInit = frameGeo->GetStrokeBoundingBox().LeftBottom();
::TransformInnerPointToPasteboard(frameGeo, &frameLeftBottomInit);

diff = limitLeftBottom.Y() - frameLeftBottomInit.Y();

if(this->ResizeFrame(frameRef,0.0, diff) != kSuccess)
{
error = ErrorUtils::PMGetGlobalErrorString();
ErrorUtils::PMSetGlobalErrorCode(kSuccess);
break;
}

InterfacePtr<IFrameListComposer> frameComposer (frameList, UseDefaultIID());
frameComposer->RecomposeThruTextIndex(txtModel->GetPrimaryStoryThreadSpan()-1);

if(Utils<ITextUtils>()->IsOverset(frameList)) // text is too big to fit in the frame within the page
{
error = PMString(kErrJVImpossible);
break;
}
}
else
{
if(Utils<IPageItemTypeUtils>()->IsInline(frameRef))
{	
// Get owner frame, then owner spline
ITextFrameColumn * ownerFrame = inlineItem->QueryFrame();
if(ownerFrame == nil)
{
status = kSuccess; // owner frame is overset, inline is not displayed and thus can't be resized
break;
}
}
if(txtModel->GetPrimaryStoryThreadSpan()- 1 == 0)
{
status = kSuccess; // story is empty, no need to justify
break;
}
}

if(multiCol->GetChildCount() > 1)
{
// This frame contains several columns, it can't be justified with the position of the last line (fastest solution)
// Instead, we use a dichotomic algorithm
PMPoint leftTop = frameGeo->GetStrokeBoundingBox().LeftTop();
PMPoint bottomLeft = frameGeo->GetStrokeBoundingBox().LeftBottom();

::TransformInnerPointToPasteboard(frameGeo,&leftTop);
::TransformInnerPointToPasteboard(frameGeo,&bottomLeft);

diff = bottomLeft.Y() - leftTop.Y();

while(diff >= 10)
{
diff = diff/2;
if(!Utils<ITextUtils>()->IsOadmin	verset(frameList))
diff = -diff;

if(this->ResizeFrame(frameRef,0.0, diff) != kSuccess)
{
error = ErrorUtils::PMGetGlobalErrorString();
ErrorUtils::PMSetGlobalErrorCode(kSuccess);
break;
}

frameComposer->RecomposeThisFrame();

diff = ::abs(diff);
}

while(Utils<ITextUtils>()->IsOverset(frameList))
{
if(this->ResizeFrame(frameRef,0.0, 2) != kSuccess)
{
error = ErrorUtils::PMGetGlobalErrorString();
ErrorUtils::PMSetGlobalErrorCode(kSuccess);
break;
}

frameComposer->RecomposeThisFrame();
}	
}

InterfacePtr<IWaxStrand> waxStrand (frameList, UseDefaultIID());
K2::scoped_ptr<IWaxIterator> waxIterator (waxStrand->NewWaxIterator());


PMPoint deepestLastLine (0, -kMaxInt32);
// Get multicolumn item child (the columns) and adjust frame according to the deepest last line of column's last lines
for(int32 i = 0 ; i < multiCol->GetChildCount() ; ++i)
{
InterfacePtr<ITextFrameColumn> columnFrame (db, multiCol->GetChildUID(i), UseDefaultIID());
IWaxLine * lastLine = waxIterator->GetFirstWaxLine(columnFrame->TextStart() + columnFrame->TextSpan()-1);
InterfacePtr<IWaxLineShape> waxLineShape( lastLine, UseDefaultIID());

PMLineSeg lineSeg;
waxLineShape->GetSelectionLine(&lineSeg);

PMPoint lineBottomPoint = lineSeg.BottomPoint();
lastLine->GetToPasteboardMatrix().Transform(&lineBottomPoint);

if(lineBottomPoint.Y() > deepestLastLine.Y())
deepestLastLine = lineBottomPoint;

waxIterator->ClearCurrentLine();
}						

PMPoint leftBottom = frameGeo->GetStrokeBoundingBox().LeftBottom();
::TransformInnerPointToPasteboard(frameGeo, &leftBottom);			

// Stroke weight must be taken into account
InterfacePtr<IGraphicStyleDescriptor> frameDescr (frameGeo, UseDefaultIID());
PMReal strokeWeight = 0.0;
Utils<IGraphicAttributeUtils>()->GetStrokeWeight(strokeWeight, frameDescr);

// Resize frame to the right height
waxIterator.reset();
diff = deepestLastLine.Y() - leftBottom.Y() + strokeWeight;

status = this->ResizeFrame(frameRef,0.0, diff);
if(status != kSuccess)
{
error = ErrorUtils::PMGetGlobalErrorString();
ErrorUtils::PMSetGlobalErrorCode(kSuccess);
}

}while(false);	

if (seq != nil)
{
if(txtFrame && vj !=  Text::kVJInvalid)
{
// Reset vertical justification setting to its original value
InterfacePtr<ICommand> vertJustCmd(CmdUtils::CreateCommand(kSetFrameVertJustifyCmdBoss)); 

InterfacePtr<IHierarchy> txtFrameSpline (Utils<ITextUtils>()->QuerySplineFromTextFrame(txtFrame));
vertJustCmd->SetItemList(UIDList(frameRef));

InterfacePtr<IIntData> vertJustCmdIntData(vertJustCmd, UseDefaultIID()); 
vertJustCmdIntData->Set(vj);
status = CmdUtils::ProcessCommand(vertJustCmd);
}

if (status == kSuccess)
{			
CmdUtils::EndCommandSequence(seq);
}
else
{
//CmdUtils::AbortCommandSequence(seq);
CAlert::InformationAlert(error);
}
}
else
CAlert::InformationAlert(error);
}
*/
ErrorCode XPGUIActionComponent::ResizeFrame(UIDRef spline, PMReal horOffset, PMReal vertOffset)
	{
	ErrorCode status = kFailure;
	PMString error("");

	do
		{
		InterfacePtr<IGeometry> splineGeom (spline, UseDefaultIID());

		PMRect bounds = splineGeom->GetStrokeBoundingBox();
		if(bounds.Width() + horOffset <= 0.0 || bounds.Height() + vertOffset <= 0.0)
			{
			error = PMString(kResizeCmdFailed);
			break;
			}

		PMReal xScale = (bounds.Width() + horOffset) / bounds.Width();
		PMReal yScale = (bounds.Height() + vertOffset) / bounds.Height();

		PMPoint ref = bounds.LeftTop();
		::TransformInnerPointToPasteboard(splineGeom, &ref);


		status = Utils<Facade::ITransformFacade>()->TransformItems( UIDList(spline), Transform::PasteboardCoordinates(), ref, 
																	Transform::TranslateBy(xScale, yScale));


		//InterfacePtr<IBoundsData> boundsData (resizeCmd, UseDefaultIID());
		//boundsData->SetBoundsChecking (IBoundsData::kNoBoundsChecking );


		} while(false);

		if(status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(kFalse);

		return status;
	}



void XPGUIActionComponent::DoNewForme(IPMUnknown* widget)
{
	InterfacePtr<IObserver> formePanel ((IObserver*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, kXPGUIFormesPanelWidgetID, IID_IOBSERVER));
	InterfacePtr<ISubject> newFormeBtn ((ISubject*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, kXPGUINewFormeWidgetID, IID_ISUBJECT));
	formePanel->Update(kTrueStateMessage, newFormeBtn, IID_ITRISTATECONTROLDATA, nil);
}

void XPGUIActionComponent::DoDeleteForme(IPMUnknown* widget)
{
	InterfacePtr<IObserver> formePanel ((IObserver*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, kXPGUIFormesPanelWidgetID, IID_IOBSERVER));
	InterfacePtr<ISubject> deleteFormeBtn ((ISubject*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, kXPGUIDeleteFormeWidgetID, IID_ISUBJECT));
	formePanel->Update(kTrueStateMessage, deleteFormeBtn, IID_ITRISTATECONTROLDATA, nil);
}

void XPGUIActionComponent::DoNewClasseur(IPMUnknown* /*widget*/, bool16 classeurAssemblage)
{
	// Both forme and assemblage classeurs now share the dialog flow. The
	// dialog controller branches on the IID_IBOOLDATA flag set here to pick
	// the right model + target palette + refresh broadcast.
	XPageUIUtils::DisplayNewClasseurDialog(classeurAssemblage);
}

void XPGUIActionComponent::DoDeleteClasseur(IPMUnknown* widget, bool16 classeurAssemblage)	
{

	// Ask user for confirmation before
	int16 rep = CAlert::ModalAlert(PMString(kXPGUIConfirmDeleteClasseurKey), kYesString, kNoString, kNullString, 2,CAlert::eWarningIcon);
	if(rep == 1){
		InterfacePtr<ITextControlData> classeurData((ITextControlData*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, kXPGUIComboClasseurListWidgetID, IID_ITEXTCONTROLDATA));
		PMString classeurName = classeurData->GetString();
		ClassID theMsg = kXPGDeleteClasseurMsg;
		ErrorCode status = kFailure;

		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor (GetExecutionContextSession(), UseDefaultIID());
		
		if(!classeurAssemblage)
		{
			InterfacePtr<IFormeDataModel> model (pageMrgAccessor->QueryFormeDataModel());
			status = model->DeleteClasseur(classeurName);
		}
		else
		{
			theMsg = kXPGDeleteClasseurAssemblageMsg;
			InterfacePtr<IAssemblageDataModel> model (pageMrgAccessor->QueryAssemblageDataModel());
			status = model->DeleteClasseur(classeurName);
		}

		if(status == kSuccess)
		{
			// Notify forme palette
			InterfacePtr<ISubject> sessionSubject (GetExecutionContextSession(), UseDefaultIID());
			sessionSubject->Change(theMsg, IID_IREFRESHPROTOCOL);
		}
	}
}


void XPGUIActionComponent::DoNewAssemblage(IPMUnknown* widget)
{
	InterfacePtr<IObserver> assemblagePanel ((IObserver*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, kXPGUIAssemblagesPanelWidgetID, IID_IOBSERVER));
	InterfacePtr<ISubject> newAssemblageBtn ((ISubject*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, kXPGUINewAssemblageWidgetID, IID_ISUBJECT));
	assemblagePanel->Update(kTrueStateMessage, newAssemblageBtn, IID_ITRISTATECONTROLDATA, nil);
}

void XPGUIActionComponent::DoDeleteAssemblage(IPMUnknown* widget)
{
	InterfacePtr<IObserver> assemblagePanel ((IObserver*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, kXPGUIAssemblagesPanelWidgetID, IID_IOBSERVER));
	InterfacePtr<ISubject> deleteAssemblageBtn ((ISubject*)Utils<IWidgetUtils>()->QueryRelatedWidget(widget, kXPGUIDeleteAssemblageWidgetID, IID_ISUBJECT));
	assemblagePanel->Update(kTrueStateMessage, deleteAssemblageBtn, IID_ITRISTATECONTROLDATA, nil);
}

//  Code generated by DollyXs code generator
