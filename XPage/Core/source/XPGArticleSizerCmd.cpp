#include "VCPlugInHeaders.h"

// Interface includes:
#include "IGeometry.h"
#include "IHierarchy.h"
#include "IInlineData.h"
#include "IRefPointUtils.h"
#include "IStoryList.h"
#include "ITextModel.h"
#include "ITextParcelList.h"
#include "IGeometryFacade.h"

// General includes:
#include "CmdUtils.h"
#include "Command.h"
#include "ErrorUtils.h"
#include "K2Vector.tpp"
#include "OwnedItemDataList.h"

// Project includes
#include "XPGID.h"

/** Implementation of command to resize inlines to fit within the column that contains them. It leaves ones that are smaller
	than this width unaffected. 

	@ingroup XPGArticleSizerCmd
*/
class XPGArticleSizerCmd : public Command
{
public:
	/** Constructor.
		@param boss IN interface ptr from boss object on which this interface is aggregated.*/
	XPGArticleSizerCmd(IPMUnknown* boss);
	
protected:

	/** @see Command::Do
	 */
	virtual void Do();	

	/** @see Command::CreateName
	 */
	virtual PMString* CreateName();

	/**	Find all the in-lines in given story
		@param textModelUIDRef IN text model containing the inlines of interest
		@param outList OUT parameter holding the inline boss objects
	 */
	void CollectInlinesInStory(const UIDRef& textModelUIDRef, K2Vector<UIDRef>& outList);

	/**	Resize an inline in given text model (maybe, if it needs to be scaled to fit in column)
		@param textModelUIDRef IN text model in which inline is to be scaled to fit
		@param inlineBossUIDRef IN boss object of type kInlineBoss
	 */
	void ResizeInlineGraphicToFit(const UIDRef& textModelUIDRef, const UIDRef& inlineBossUIDRef);

	/**	Get the width of the column into which this inline is being placed
		@param textModelUIDRef IN text model of interest
		@param uidRef IN inline thing, we're going to query for IInlineData on this to find out where in the story the inline is
		@return PMReal composed width of column
	 */
	PMReal CalcColumnParcelWidth(const UIDRef& textModelUIDRef, const UIDRef& uidRef) const;

	/**	Method to calculate width to scale inline to if required
		@param uidRef IN the image whose width we'll get from its IGeometry
		@return PMReal the current width before scaling this image
	 */
	PMReal CalcInlineGraphicWidth(const UIDRef& uidRef) const;

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGArticleSizerCmd, kXPGArticleSizerCmdImpl)

/* Constructor
*/
XPGArticleSizerCmd::XPGArticleSizerCmd(IPMUnknown* boss) : Command(boss)	
{
	this->SetUndoability(kAutoUndo);
}

/* Do
*/
void XPGArticleSizerCmd::Do(){

	ErrorCode retval = kFailure;
	do{

		IDataBase* db = fItemList.GetDataBase();
		if (!db) 
			break;

		int32 nbItems = fItemList.size();

		if(nbItems == 0) 
			break;

		for (int32 i = 0; i < nbItems; i++) {
			 
			K2Vector<UIDRef> inlinesList;
			UIDRef story = fItemList.GetRef(i);
			this->CollectInlinesInStory(story, inlinesList);
			K2Vector<UIDRef>::const_iterator iter;

			for (iter = inlinesList.begin(); iter < inlinesList.end(); ++iter){
				this->ResizeInlineGraphicToFit(story, *iter);
			}
		}
		retval = kSuccess;

	} while (kFalse);

	if (retval != kSuccess) {
		ErrorUtils::PMSetGlobalErrorCode(kFailure);
	}
}


/* CreateName
*/
PMString* XPGArticleSizerCmd::CreateName()
{
	return new PMString("XPGArticleSizerCmd");
}

/* ResizeInlineGraphicToFit
*/
void XPGArticleSizerCmd::ResizeInlineGraphicToFit(const UIDRef& textModelUIDRef,  const UIDRef& inlineBossUIDRef){

	do{
		InterfacePtr<IHierarchy> hier(inlineBossUIDRef, UseDefaultIID());
		if (!hier) {
			break;
		}
		if (hier->GetChildCount() < 1) {
			break;
		}
		UID uid = hier->GetChildUID(0);
		ASSERT(uid != kInvalidUID);
		if (uid == kInvalidUID) {
			break;
		}
		UIDRef placedThingUIDRef(inlineBossUIDRef.GetDataBase(),uid);

		PBPMPoint referencePoint = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kTopLeftReferencePoint, 
																					UIDList(placedThingUIDRef));

		PMReal colWidth = this->CalcColumnParcelWidth(textModelUIDRef, inlineBossUIDRef);
		PMReal inlineWidth = this->CalcInlineGraphicWidth(placedThingUIDRef);
		if (colWidth > inlineWidth) {
			// No need to scale
			break;
		}
		if (colWidth== PMReal(0.0) || inlineWidth == PMReal(0.0)) {
			break;
		}
		PMReal xFactor= colWidth / inlineWidth;
		PMReal yFactor = xFactor;

		InterfacePtr<IGeometry> pageItemGeometry(placedThingUIDRef, UseDefaultIID());
		ASSERT(pageItemGeometry);
		if (!pageItemGeometry) {
			break;
		}


		// This is why we need a command; the TransformCmdUtils impl. calls IUtils<IPageItemUtils>()->ScaleRect
		// which changes the model...
		//...actually IGeometryFacade::ResizeItems already uses a command internally

		ErrorCode err =  Utils<Facade::IGeometryFacade>()->ResizeItems( UIDList(placedThingUIDRef), 
							Transform::PasteboardCoordinates(), Geometry::PathBounds(),
							referencePoint, Geometry::MultiplyBy( xFactor, yFactor));
		ASSERT(err == kSuccess);
	} while (kFalse);
}

/* CollectInlinesInStory
*/
void XPGArticleSizerCmd::CollectInlinesInStory(const UIDRef& textModelUIDRef, K2Vector<UIDRef>& outList)
{
	do{
		InterfacePtr<ITextModel> textModel(textModelUIDRef, UseDefaultIID());
		ASSERT(textModel);
		if (!textModel) {
			break;
		}
		IDataBase* db = ::GetDataBase(textModel);
		OwnedItemDataList ownedList;
		Utils<ITextUtils>()->CollectOwnedItems(textModel, 0, textModel->GetPrimaryStoryThreadSpan(), &ownedList);
		for (int32 i = 0; i < ownedList.size(); i++) {
			if ((ownedList[i].fClassID == kInlineBoss)) {
				outList.push_back(UIDRef(db, ownedList[i].fUID));
			}
		}

	} while (kFalse);
}


/* CalcColumnParcelWidth
*/
PMReal XPGArticleSizerCmd::CalcColumnParcelWidth(const UIDRef& textModelUIDRef,
											const UIDRef& uidRef) const
{
	// uidRef refers to a kInlineBoss
	// so we can get e.g. an IInlineData from it
	PMReal retval=(0.0);
	do
	{
		InterfacePtr<ITextModel> textModel(textModelUIDRef, UseDefaultIID());
		ASSERT(textModel);
		if (!textModel) {
			break;
		}
		InterfacePtr<IInlineData> iInlineData(uidRef, UseDefaultIID());
		ASSERT(iInlineData);
		if (!iInlineData) {
			break;
		}
		// Find out where in the text the inline is anchored
		TextIndex ilgIndex = iInlineData->GetILGIndex();
		ASSERT(ilgIndex >= 0 && ilgIndex < textModel->GetPrimaryStoryThreadSpan());

		PMRect parcelCBounds;
		InterfacePtr<ITextParcelList> tpl(textModel->QueryTextParcelList(ilgIndex));
		ASSERT(tpl);
		if (!tpl){
			break;
		}
		// Calculate the column width based on the text frame this is being inserted into	
		bool16 canGetParcelBounds = Utils<ITextUtils>()->GetParcelContentBounds(tpl, ilgIndex, &parcelCBounds);
		if (!canGetParcelBounds) {
			break;
		}

		retval = parcelCBounds.Width();

	} while (kFalse);
	return retval;
}

/* calcInlineWIdth
*/
PMReal XPGArticleSizerCmd::CalcInlineGraphicWidth(const UIDRef& uidRef) const
{
	PMReal retval = 0.0;
	do
	{
		// this type definitely aggregates an IGeometry
		InterfacePtr<IGeometry> iGeometry(uidRef, UseDefaultIID());
		ASSERT(iGeometry);
		if (!iGeometry) {
			break;
		}
		PMRect pmRect = iGeometry->GetStrokeBoundingBox();
		retval= pmRect.Width();
	} while (kFalse);
	return retval;
}

// End, XPGArticleSizerCmd.cpp.
