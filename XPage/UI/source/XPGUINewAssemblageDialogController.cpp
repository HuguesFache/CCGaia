
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IControlView.h"
#include "ICommand.h"
#include "ICoreFilename.h"
#include "IPanelControlData.h"
#include "IStringListControlData.h"
#include "IDropDownListController.h"
#include "IStringData.h"
#include "IXPageMgrAccessor.h"
#include "IStringListData.h"
#include "IXPageSuite.h"
#include "ISelectionUtils.h"
#include "ISelectionManager.h"
#include "IPMStream.h"
#include "IAssemblageDataModel.h"
#include "IActiveContext.h"
#include "IDropDownListController.h"
#include "IDocument.h"
#include "ILayoutSelectionSuite.h"
#include "IFormeData.h"
#include "ILayoutUIUtils.h"
#include "IGeometry.h"
#include "IXPageUtils.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"
#include "IHierarchy.h"
#include "ISpread.h"
#include "IUnitOfMeasure.h"
#include "IRefPointUtils.h"
#include "IWidgetUtils.h"
#include "FileUtils.h"
#include "StreamUtil.h"
#include "ILayoutUtils.h"
#include "ISnippetExport.h"
#include "ICoreFilename.h"

// General includes:
#include "CDialogController.h"
#include "CAlert.h"
#include "Utils.h"
#include "TransformUtils.h"
#include <map>

// Project includes:
#include "XPageUIUtils.h"
#include "XPGUIID.h"
#include "XPGID.h"

/**
	New assemblage dialog controller
*/
class XPGUINewAssemblageDialogController : public CDialogController
{
public:

	XPGUINewAssemblageDialogController(IPMUnknown* boss) : CDialogController(boss) {}

	virtual void InitializeDialogFields(IActiveContext* dlgContext);

	virtual WidgetID ValidateDialogFields(IActiveContext* myContext);

	virtual void ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);

private:

	UIDList selectedItems;
	std::map<PMString, UIDList> selectedFormes;
	bool16 forceCancel;
};

CREATE_PMINTERFACE(XPGUINewAssemblageDialogController, kXPGUINewAssemblageDialogControllerImpl)

/* ApplyFields
*/
void XPGUINewAssemblageDialogController::InitializeDialogFields(IActiveContext* dlgContext)
{

	InterfacePtr<IPanelControlData> panelCtrlData(this, UseDefaultIID());
	do {
		// First get selected forme items
		IDataBase * db = ::GetDataBase(dlgContext->GetContextDocument());
		selectedItems = UIDList(db);
		selectedFormes.clear();
		forceCancel = kFalse;

		InterfacePtr<IXPageSuite> xpgSuite(dlgContext->GetContextSelection(), IID_IXPAGESUITE);
		if (xpgSuite == nil) // nil means no active selection
			break;

		UIDList txtFrameList(db);
		UIDList graphicFrameList(db);
		xpgSuite->GetSelectedItems(txtFrameList, graphicFrameList, selectedItems);

		// Get and select all items of selected formes
		UIDList allItems(txtFrameList);
		allItems.Append(graphicFrameList);

		for (int32 i = 0; i < allItems.Length(); ++i)
		{
			InterfacePtr<IFormeData> formeData(allItems.GetRef(i), UseDefaultIID());
			if (!formeData || formeData->GetUniqueName() == kNullString)
				continue;

			PMString formeName = formeData->GetUniqueName();

			std::map<PMString, UIDList>::iterator iter = selectedFormes.find(formeName);
			if (iter == selectedFormes.end())
			{
				InterfacePtr<IHierarchy> itemHier(formeData, UseDefaultIID());
				InterfacePtr<ISpread> formeOwnerSpread(db, itemHier->GetSpreadUID(), UseDefaultIID());

				UIDList formeItems(db);
				Utils<IXPageUtils>()->GetAllFormeItemsOnSpread(formeName, formeOwnerSpread, formeItems);

				selectedFormes.insert(std::make_pair(formeName, formeItems));
			}
		}

		// Check whether selection contains all forme items or not
		std::map<PMString, UIDList>::iterator iter;
		UIDList txtFrameListToCheck(db), graphicFrameListToCheck(db);
		for (iter = selectedFormes.begin(); iter != selectedFormes.end(); ++iter)
		{
			UIDList formeItems = iter->second;
			Utils<IXPageUtils>()->CategorizeItems(formeItems, txtFrameListToCheck, graphicFrameListToCheck);
		}

		if (txtFrameListToCheck.Length() != txtFrameList.Length() || graphicFrameListToCheck.Length() != graphicFrameList.Length())
		{
			forceCancel = kTrue;
		}

		// Clear forme name
		SetTextControlData(kXPGUIAssemblageNameWidgetID, kNullString);

		// Clear classeur  list
		InterfacePtr<IStringListControlData> classeurListData(panelCtrlData->FindWidget(kXPGUIRubriqueListWidgetID), UseDefaultIID());
		classeurListData->Clear();

		// Populate classeur list
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IAssemblageDataModel> assemblageModel(pageMrgAccessor->QueryAssemblageDataModel());
		K2Vector<PMString> classeurList = assemblageModel->GetClasseurList();

		// The palette passes its currently-selected classeur to us via
		// IID_ICLASSEURNAMEDATA on the dialog boss (set by
		// XPageUIUtils::DisplayNewFormeDialog). We pre-select it below so
		// the user doesn't have to re-pick it inside the dialog.
		InterfacePtr<IStringData> presetClasseurData(this, IID_ICLASSEURNAMEDATA);
		PMString presetClasseur = presetClasseurData ? presetClasseurData->GetString() : kNullString;

		int32 indexToSelect = 0;
		for (int32 i = 0; i < classeurList.size(); ++i) {
			PMString classeur = classeurList[i];
			if (presetClasseur != kNullString && classeur == presetClasseur)
				indexToSelect = i;
			classeur.SetTranslatable(kFalse);
			classeurListData->AddString(classeur);
		}

		if (classeurList.size() > 0) {
			InterfacePtr<IDropDownListController> classeurListController(classeurListData, UseDefaultIID());
			classeurListController->Select(indexToSelect);
		}

	} while (kFalse);
}


/* ValidateFields
*/
WidgetID XPGUINewAssemblageDialogController::ValidateDialogFields(IActiveContext* myContext) {

	WidgetID result = kNoInvalidWidgets;
	InterfacePtr<IPanelControlData> panelCtrlData(this, UseDefaultIID());

	// if selection is incorrect, do not validate to force user to cancel
	if (forceCancel)
	{
		CAlert::InformationAlert(PMString(kXPGUIInvalidAssemblySelectionKey));
		return kXPGUIAssemblageNameWidgetID;
	}

	// Get forme name
	PMString assemblageName = GetTextControlData(kXPGUIAssemblageNameWidgetID);
	if (assemblageName == kNullString)
		return kXPGUIAssemblageNameWidgetID;

	if (!XPageUIUtils::IsValidName(assemblageName)) {
		CAlert::InformationAlert(kXPGUIInvalidAssemblageNameKey);
		return kXPGUIAssemblageNameWidgetID;
	}

	// Get selected classeur
	InterfacePtr<IDropDownListController> classeurListController(panelCtrlData->FindWidget(kXPGUIRubriqueListWidgetID), UseDefaultIID());
	int32 selectedClasseur = classeurListController->GetSelected();
	if (selectedClasseur == IDropDownListController::kNoSelection)
		return kXPGUIRubriqueListWidgetID;

	InterfacePtr<IStringListControlData> classeursListData(classeurListController, UseDefaultIID());
	PMString classeurName = classeursListData->GetString(selectedClasseur);

	// Check unicity of assemblage name 
	InterfacePtr<IXPageMgrAccessor> pageMrgAccessor(GetExecutionContextSession(), UseDefaultIID());
	InterfacePtr<IAssemblageDataModel> assemblageModel(pageMrgAccessor->QueryAssemblageDataModel());
	if (assemblageModel->DoesAssemblageExist(assemblageName, classeurName)) {
		// The assemblage already exist, ask user if he wants to replace it
		int16 rep = CAlert::ModalAlert(PMString(kXPGUIReplaceAssemblageKey), PMString(kSDKDefOKButtonApplicationKey), PMString(kSDKDefCancelButtonApplicationKey), kNullString, 1, CAlert::eWarningIcon);
		if (rep == 2) // if the user clicked on Cancel" button
			return kXPGUIAssemblageNameWidgetID;
	}

	return result;
}

/* ApplyFields
*/
void XPGUINewAssemblageDialogController::ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId)
{
	do
	{

		InterfacePtr<IPanelControlData> panelCtrlData(this, UseDefaultIID());

		// Get assemblage name
		PMString assemblageName = GetTextControlData(kXPGUIAssemblageNameWidgetID);

		// recuperation des extremites de l'assemblage
		PBPMPoint assemblageLeftTop = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kTopLeftReferencePoint, selectedItems/*, nil, &Geometry::OuterStrokeBounds()*/);
		PBPMPoint assemblageRightBottom = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kBottomRightReferencePoint, selectedItems/*, nil, &Geometry::OuterStrokeBounds()*/);

		// Conversion en millim￨tres des dimensions
		InterfacePtr<IUnitOfMeasure> uom((IUnitOfMeasure *)::CreateObject(kMillimetersBoss, IID_IUNITOFMEASURE));

		PMString largeur, hauteur;
		largeur.AppendNumber(uom->PointsToUnits(assemblageRightBottom.X() - assemblageLeftTop.X()), 3, kTrue, kTrue);
		hauteur.AppendNumber(uom->PointsToUnits(assemblageRightBottom.Y() - assemblageLeftTop.Y()), 3, kTrue, kTrue);

		// Get classeur name
		PMString classeurName = kNullString;
		InterfacePtr<IDropDownListController> classeurListController(panelCtrlData->FindWidget(kXPGUIRubriqueListWidgetID), UseDefaultIID());
		int32 selectedIndex = classeurListController->GetSelected();
		if (selectedIndex != IDropDownListController::kNoSelection) {
			InterfacePtr<IStringListControlData> classeursListData(classeurListController, UseDefaultIID());
			classeurName = classeursListData->GetString(selectedIndex);
		}

		// Create the assemblage
		InterfacePtr<IXPageMgrAccessor> pageMrgAccessor(GetExecutionContextSession(), UseDefaultIID());
		InterfacePtr<IAssemblageDataModel>   assemblageModel(pageMrgAccessor->QueryAssemblageDataModel());

		if (assemblageModel->CreateAssemblage(assemblageName, classeurName, selectedItems) != kSuccess)
		{
			CAlert::InformationAlert(kXPGUIFailedToCreateAssemblageKey);
			break;
		}

		K2Vector<PMString> formeIDs;
		K2Vector<PMRect> formePositions;

		std::map<PMString, UIDList>::iterator iter;

		IDFile descriptionFolder = assemblageModel->GetAssemblageRoot();
		FileUtils::AppendPath(&descriptionFolder, classeurName);
		FileUtils::AppendPath(&descriptionFolder, "Description");
		FileUtils::CreateFolderIfNeeded(descriptionFolder);
		FileUtils::AppendPath(&descriptionFolder, assemblageName);
		if (FileUtils::IsDirectory(descriptionFolder)) {
			//recuperer tous les fichiers du dossier et les supprimer
			// Filter avec l'extention du fichier
			PMString filter = "idms";

			K2Vector<IDFile> fileList;
			Utils<IXPageUtils>()->ListPath(descriptionFolder, filter, fileList);


			InterfacePtr<ICoreFilename> coreFile(::CreateObject2<ICoreFilename>(kCoreFilenameBoss));
			for (int32 i = 0; i < fileList.size(); ++i)
			{
				coreFile->Initialize(&fileList[i]);
				coreFile->Delete();
			}
			filter = "jpg";

			Utils<IXPageUtils>()->ListPath(descriptionFolder, filter, fileList);

			for (int32 i = 0; i < fileList.size(); ++i)
			{
				coreFile->Initialize(&fileList[i]);
				coreFile->Delete();
			}
		}
		FileUtils::CreateFolderIfNeeded(descriptionFolder);


		for (iter = selectedFormes.begin(); iter != selectedFormes.end(); ++iter)
		{
			UIDList formeItems = iter->second;
			PMPoint leftTopMost(kMaxInt32, kMaxInt32), rightBottomMost(-kMaxInt32, -kMaxInt32);
			IDataBase * db = formeItems.GetDataBase();
			InterfacePtr<IHierarchy> hier(db, formeItems[0], UseDefaultIID());
			if (hier == nil)
				continue;
			UID ownerPageUID;
			ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(hier);
			InterfacePtr<IGeometry> ownerPageGeo(db, ownerPageUID, UseDefaultIID());
			if (ownerPageGeo == nil)
				continue;

			IDFile currentFile = descriptionFolder;
			FileUtils::AppendPath(&currentFile, iter->first + ".idms");

			InterfacePtr<IPMStream> snippetStream(StreamUtil::CreateFileStreamWrite(currentFile, kOpenOut | kOpenTrunc, 'ICx4', 'InCp'));
			if (Utils<ISnippetExport>()->ExportPageitems(snippetStream, formeItems) != kSuccess) {
				snippetStream->Close();
				break;
			}
			snippetStream->Close();
			currentFile = descriptionFolder;
			FileUtils::AppendPath(&currentFile, iter->first + ".jpg");
			if (Utils<IXPageUtils>()->DoSnapShot(formeItems, currentFile) != kSuccess)
				break;

			/*
			for (int32 i = 0; i < formeItems.Length(); ++i) {
				InterfacePtr<IGeometry> itemGeo(formeItems.GetRef(i), UseDefaultIID());
				PMPoint leftTop = itemGeo->GetStrokeBoundingBox().LeftTop();
				PMPoint rightBottom = itemGeo->GetStrokeBoundingBox().RightBottom();
				::TransformInnerPointToPasteboard(itemGeo, &leftTop);
				::TransformInnerPointToPasteboard(itemGeo, &rightBottom);

				if (leftTop.X() < leftTopMost.X())
					leftTopMost.X() = leftTop.X();
				if (leftTop.Y() < leftTopMost.Y())
					leftTopMost.Y() = leftTop.Y();

				if (rightBottom.X() > rightBottomMost.X())
					rightBottomMost.X() = rightBottom.X();
				if (rightBottom.Y() > rightBottomMost.Y())
					rightBottomMost.Y() = rightBottom.Y();
			}


			// Calcul en millimètres des dimensions
			InterfacePtr<IUnitOfMeasure> uom((IUnitOfMeasure *)::CreateObject(kMillimetersBoss, IID_IUNITOFMEASURE));
			if (uom == nil)
				continue;
			PMReal formeWidth = uom->PointsToUnits(rightBottomMost.X() - leftTopMost.X());
			PMReal formeHeight = uom->PointsToUnits(rightBottomMost.Y() - leftTopMost.Y());

			// Calcul de l'origine dans le repere de la page
			::TransformPasteboardPointToInner(ownerPageGeo, &leftTopMost);

			PMReal posx = uom->PointsToUnits(leftTopMost.X());
			PMReal posy = uom->PointsToUnits(leftTopMost.Y());

			// Get dimensions and relative position for each forme
			PBPMPoint formeLeftTop = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kTopLeftReferencePoint, formeItems/);
			PBPMPoint formeRightBottom = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kBottomRightReferencePoint, formeItems/);

			PMReal formeWidth = uom->PointsToUnits(formeRightBottom.X() - formeLeftTop.X());
			PMReal formeHeight = uom->PointsToUnits(formeRightBottom.Y() - formeLeftTop.Y());

			PMPoint relPos(uom->PointsToUnits(formeLeftTop.X() - assemblageLeftTop.X()),
				uom->PointsToUnits(formeLeftTop.Y() - assemblageLeftTop.Y()));

			// First transform coordinates from item inner space to pasteboard space
			//::TransformInnerPointToPasteboard(ownerPageGeo, &leftTop);
			//::TransformInnerPointToPasteboard(itemGeo, &center);
			// Then transform coordinates from pasteboard space to page space
			::TransformPasteboardPointToInner(ownerPageGeo, &relPos);
			//::TransformPasteboardPointToInner(curPageGeo, &center);
			

			IDFile currentFile = descriptionFolder;
			FileUtils::AppendPath(&currentFile, iter->first + ".ini");
			PMString output;
			output.AppendNumber(posx);
			output += "\r\n";
			output.AppendNumber(posy);
			output += "\r\n";
			output.AppendNumber(formeWidth);
			output += "\r\n";
			output.AppendNumber(formeHeight);
			output += "\r\n";

			InterfacePtr<IPMStream> txtFileOutStream(StreamUtil::CreateFileStreamWrite(currentFile));
			if (txtFileOutStream->GetStreamState() == kStreamStateGood)
			{
				txtFileOutStream->SetEndOfStream();
				txtFileOutStream->XferByte((uchar *)output.GetPlatformString().c_str(), output.GetPlatformString().size());
				txtFileOutStream->Flush();
			}
			txtFileOutStream->Close();*/
		}


	} while (kFalse);
}

