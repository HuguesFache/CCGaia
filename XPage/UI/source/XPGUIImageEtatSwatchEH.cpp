// XPGUIImageEtatSwatchEH.cpp
//
// Event handler aggregated on kXPGUIImageEtatSwatchWidgetBoss — the clickable
// colour swatch of each Photos liées row. A click opens the modal "Changer
// l'état de l'image" dialog; on confirmation it calls the Plugin_SetImageState
// web service and refreshes the row's pastille.
//
// The swatch boss inherits the SDK UIColorSwatchWidget boss, so display +
// IColorData keep working; we only override IID_IEVENTHANDLER. Every event
// except the click is left to CEventHandler's default (returns kFalse =
// not handled, so it propagates normally).

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IEventHandler.h"
#include "IEvent.h"
#include "IWidgetParent.h"
#include "ITreeNodeIDData.h"
#include "IControlView.h"
#include "IColorData.h"
#include "IApplication.h"
#include "IDialog.h"
#include "IDialogMgr.h"
#include "IIntData.h"
#include "IXPageMgrAccessor.h"
#include "IImageDataModel.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"

// General includes:
#include "CEventHandler.h"
#include "CreateObject.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"
#include "CoreResTypes.h"
#include "NodeID.h"
#include "Utils.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGID.h"
#include "XPGUIImageNodeID.h"
#include "XPGImageDataNode.h"
#include "XPageUIUtils.h"


class XPGUIImageEtatSwatchEH : public CEventHandler
{
public:
	XPGUIImageEtatSwatchEH(IPMUnknown* boss) : CEventHandler(boss) {}
	virtual ~XPGUIImageEtatSwatchEH() {}

	// Claim the click on button-down so the swatch becomes the mouse-tracking
	// target and the matching LButtonUp is delivered here (otherwise the parent
	// row could capture the gesture and our LButtonUp would never fire).
	virtual bool16 LButtonDn(IEvent* /*e*/) { return kTrue; }

	// A full click on the pastille opens the chooser. Other events fall through
	// to CEventHandler's defaults.
	virtual bool16 LButtonUp(IEvent* e);

private:
	// Open the modal état chooser; returns the chosen état id (or currentEtat
	// unchanged on Cancel).
	int32 RunChooserDialog(int32 currentEtat) const;
	// Apply the new état: web service + local model update + swatch repaint.
	void  ApplyNewEtat(XPGUIImageNodeID* node, int32 newEtat);
};

CREATE_PMINTERFACE(XPGUIImageEtatSwatchEH, kXPGUIImageEtatSwatchEHImpl)


bool16 XPGUIImageEtatSwatchEH::LButtonUp(IEvent* /*e*/)
{
	do
	{
		// Walk up from the swatch to the row boss (kXPGUIImageNodeWidgetBoss),
		// which carries the tree node id data.
		InterfacePtr<IWidgetParent> wparent(this, UseDefaultIID());
		if (wparent == nil)
			break;
		InterfacePtr<ITreeNodeIDData> nodeData((ITreeNodeIDData*)wparent->QueryParentFor(IID_ITREENODEIDDATA));
		if (nodeData == nil)
			break;

		TreeNodePtr<XPGUIImageNodeID> node(nodeData->Get());
		if (node == nil)
			break;
		if (node->GetType() != XPGImageDataNode::kImageNode)
			break; // rubrique header row — no état.

		const XPGImageDataNode::ImageData* img = node->GetImageData();
		if (img == nil)
			break;

		const int32 currentEtat = img->etatImage;
		const int32 newEtat = this->RunChooserDialog(currentEtat);

		if (newEtat != currentEtat)
			this->ApplyNewEtat(node, newEtat);

	} while (kFalse);

	return kTrue; // consume the click.
}


int32 XPGUIImageEtatSwatchEH::RunChooserDialog(int32 currentEtat) const
{
	int32 result = currentEtat;

	do
	{
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		if (application == nil)
			break;
		InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
		if (dialogMgr == nil)
			break;

		PMLocaleId nLocale = LocaleSetting::GetLocale();
		RsrcSpec dialogSpec(nLocale, kXPGUIPluginID, kViewRsrcType,
			kXPGUIChooseEtatImageDialogResourceID, kTrue);

		IDialog* dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);
		if (dialog == nil)
			break;

		IControlView* panel = dialog->GetDialogPanel();

		// Pass the current état in for preselection. The controller overwrites
		// it with the chosen état id on OK; on Cancel it stays as-is.
		InterfacePtr<IIntData> choiceData(panel, IID_IXPGETATCHOICE);
		if (choiceData != nil)
			choiceData->Set(currentEtat);

		dialog->Open();
		dialog->WaitForDialog();

		if (choiceData != nil)
			result = choiceData->Get();

	} while (kFalse);

	return result;
}


void XPGUIImageEtatSwatchEH::ApplyNewEtat(XPGUIImageNodeID* node, int32 newEtat)
{
	const PMString imageIdStr = node->GetID();
	PMString::ConversionError convErr = PMString::kNoError;
	const int32 idImage = imageIdStr.GetAsNumber(&convErr);
	if (convErr != PMString::kNoError)
		return;

	InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
	if (xpgPrefs == nil)
		return;

	// Web service: Plugin_SetImageState (same client/address as the other
	// XPage prefs/image services — TEC server URL).
	InterfacePtr<IWebServices> baseHTTP(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
	if (baseHTTP == nil)
		return;
	if (!baseHTTP->SetImageState_v2(xpgPrefs->GetTEC_URL(), idImage, newEtat))
		return; // server didn't confirm "ok" — leave the pastille unchanged.

	// 1. Update the authoritative model node so a later full redraw keeps the
	//    new state.
	InterfacePtr<IXPageMgrAccessor> pageMgrAccessor(GetExecutionContextSession(), UseDefaultIID());
	if (pageMgrAccessor != nil)
	{
		InterfacePtr<IImageDataModel> model(pageMgrAccessor->QueryImageDataModel());
		if (model != nil)
		{
			XPGImageDataNode* modelNode = model->GetNode(imageIdStr);
			if (modelNode != nil && modelNode->GetImageData() != nil)
			{
				XPGImageDataNode::ImageData data = *(modelNode->GetImageData());
				data.etatImage = newEtat;
				modelNode->SetImageData(data);
			}
		}
	}

	// 2. Update the displayed node copy so a later re-apply of this row (scroll,
	//    selection change) keeps the new state before the next full rebuild.
	node->SetEtatImage(newEtat);

	// 3. Repaint THIS swatch right now. The EH sits on the swatch boss, so we
	//    set its IColorData directly and invalidate — no dependency on the tree
	//    re-applying the node (which wasn't refreshing the pastille).
	PMReal r, g, b;
	ColorArray colorArray;
	if (XPageUIUtils::GetEtatImageColor(newEtat, r, g, b))
	{
		colorArray.push_back(r);
		colorArray.push_back(g);
		colorArray.push_back(b);
	}
	else
	{
		// No état / unknown colour — neutral light grey, matching the widget mgr.
		colorArray.push_back(0.80);
		colorArray.push_back(0.80);
		colorArray.push_back(0.80);
	}

	InterfacePtr<IColorData> colorData(this, UseDefaultIID());
	if (colorData != nil)
		colorData->SetColorData(kPMCsCalRGB, colorArray);

	InterfacePtr<IControlView> swatchView(this, UseDefaultIID());
	if (swatchView != nil)
		swatchView->Invalidate();
}


// End, XPGUIImageEtatSwatchEH.cpp.
