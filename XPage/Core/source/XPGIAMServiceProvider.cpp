#include "VCPlugInHeaders.h"

// Interface includes:
#include "IInCopyWorkFlowUtils.h"
#include "IAMServiceProvider.h"
#include "IAMUIService.h"
#include "IAMService.h"
#include "IAMServiceData.h"
#include "IAMUIServiceProvider.h"
#include "IAMLockable.h"
#include "ISelectionManager.h"
#include "ISelectionUtils.h"
#include "IStringData.h"
#include "IBoolData.h"
#include "IClassIDData.h"
#include "DocumentID.h"
#include "ErrorUtils.h"
#include "IWorkgroupStoryFacade.h"
#include "IAssignedDocument.h"
#include "IAssignmentMgr.h"
#include "FileUtils.h"
#include "IHierarchy.h"
#include "IXPageSuite.h"
#include "IXPGPreferences.h"
#include "IWebServices.h"
#include "IXPageUtils.h"
#include "ITextModel.h"
#include "IFrameList.h"
#include "LocaleSetting.h"
#include "XPGID.h"
#include "XPGUIID.h"
#include "CAlert.h"
#include "ILiveEditFacade.h"
#include "ILinkResourceFactory.h"
#include "IURIUtils.h"
#include "ILinkResourceStateUpdateCmdData.h"
#include "IArticleData.h"

class XPGIAMServiceProvider : public CPMUnknown <IAMServiceProvider> {

public:

	XPGIAMServiceProvider(IPMUnknown* boss);
	virtual	~XPGIAMServiceProvider();

	virtual bool16 CanService(const IDFile& file, const IAMServiceProvider::AssetHints* hints = nil);
	virtual bool16 CanService(const IManageableAsset* asset);
	virtual bool16 CanUpdateScriptedAssetData(UIDRef assetRef, const PMString& scriptString, int32 scriptID);
	virtual IAMService* GetService(int32 uiServiceID);
	virtual ErrorCode Init();
	virtual bool16 GetLinkResourceId(const IDFile& inFile, ILinkResource::ResourceId& outResourceId);
	virtual bool16 IsTemporaryVersion(const IDFile& file);
	virtual bool16 IsVersioned(const IDFile& file);
	virtual IOpenFileCmdData::OpenFlags OverrideOpenFlags(const IDFile& file, IOpenFileCmdData::OpenFlags inOpenFlags);
	virtual ErrorCode UpdateScriptedAssetData(UIDRef assetRef, const PMString& scriptString, int32 scriptID);
	virtual bool16 IsWriteable(const IDFile& assetFile);
	virtual bool16 GetLockData(const IDFile& assetFile, IManagedStatus::StatusStringTable& outLockData, UIFlags uiFlags = kFullUI);

};

CREATE_PMINTERFACE(XPGIAMServiceProvider, kXPGIAMServiceProviderImpl)

XPGIAMServiceProvider::XPGIAMServiceProvider(IPMUnknown* boss)
	: CPMUnknown<IAMServiceProvider>(boss) {}

XPGIAMServiceProvider::~XPGIAMServiceProvider() {}

IAMService* XPGIAMServiceProvider::GetService(int32 assetServiceID) {


	IAMService* service(nil);
	switch (assetServiceID) {
	case IAMServiceProvider::enInitialize:
		service = ::CreateObject2<IAMService>(kICInitializeStoryServiceBoss);
		break;

	case IAMServiceProvider::enCheckOut: {
		PMString section = kNullString, ssSection = kNullString,
			colorStatus = kNullString, incopyPath = kNullString,
			currentStatus = kNullString, idArticle = kNullString,
			sujet = kNullString, proprio = kNullString,
			result = kNullString;

		IDFile assignFile;
		bool16 lock = kTrue, isChecked = kFalse;;

		InterfacePtr<ISelectionManager> selectionMgr(Utils<ISelectionUtils>()->QueryActiveSelection());
		IDataBase* db = selectionMgr->GetDataBase();
		if (db == nil)
			break;

		InterfacePtr<IXPageSuite> xpgSuite(selectionMgr, IID_IXPAGESUITE);
		if (xpgSuite == nil)
			break;

		UIDList textFrames, graphicFrames, selectedItems;
		xpgSuite->GetSelectedItems(textFrames, graphicFrames, selectedItems);

		// Gather all InCopy assignments
		InterfacePtr<IAssignmentMgr> assignMgr(GetExecutionContextSession(), UseDefaultIID());
		K2Vector<PMString> assignFilePaths;

		for (int32 i = 0; i < selectedItems.Length(); ++i) {
			UIDRef storyRef = selectedItems.GetRef(i);

			// Get assignment related to this story
			InterfacePtr<IAssignment> assignment(nil);
			InterfacePtr<IAssignedStory> assignedStory(nil);

			assignMgr->QueryAssignmentAndAssignedStory(storyRef, assignment, assignedStory);
			if (assignment == nil || assignedStory == nil)
				continue;

			PMString assignFilePath = assignment->GetFile();
			if (::K2find(assignFilePaths, assignFilePath) == assignFilePaths.end())
				assignFilePaths.push_back(assignFilePath);
		}

		if (assignFilePaths.size() == 1) {
			// Get assignment related to this story
			InterfacePtr<IAssignment> assignment(nil);
			InterfacePtr<IAssignedStory> assignedStory(nil);

			assignMgr->QueryAssignmentAndAssignedStory(selectedItems.GetRef(0), assignment, assignedStory);
			if (assignment == nil || assignedStory == nil)
				break;

			UIDList storyList;
			assignMgr->GetStoryListOnAssignment(assignment, storyList);

			// On recupere l'identifiant de l'article selectionne
			assignFile = FileUtils::PMStringToSysFile(assignFilePaths[0]);
			idArticle = Utils<IXPageUtils>()->GetStoryIDFromAssignmentFile(assignFile);

			// Son statut
			//TODO PLUGINS INCOPY
			/*InterfacePtr<IXPGPreferences> xpgPrefs(GetExecutionContextSession(), UseDefaultIID());
			InterfacePtr<IWebServices> xrailConnexion(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
			xrailConnexion->SetServerAddress(xpgPrefs->GetXRail_URL());

			if (!xrailConnexion->StoryGetInfo(idArticle, section, ssSection, currentStatus, colorStatus, incopyPath, sujet, proprio, result))
			{
				//Si le serveur XRail ne repond pas, on autorise l'extraction de l'article
				ErrorUtils::PMSetGlobalErrorCode(kSuccess);
				service = (IAMService*)::CreateObject(kICCheckOutServiceBoss, IID_IAMSERVICE);
				break;
			}

			//Si l'article n'exsite pas dans la base ou autre erreur, on autorise l'extraction de l'article
			if (result != "0") {
				service = (IAMService*)::CreateObject(kICCheckOutServiceBoss, IID_IAMSERVICE);
				break;
			}

			// On verifie est ce qu'on a le droit d'extraire le bloc
			int i = 1;
			K2::scoped_ptr<PMString> status(xpgPrefs->GetStatus().GetItem(";", i));
			while (status != nil) {
				if (currentStatus == *status)
					lock = kFalse;
				++i;
				status.reset(xpgPrefs->GetStatus().GetItem(";", i));
			}
			if (!lock) {
				service = (IAMService*)::CreateObject(kICCheckOutServiceBoss, IID_IAMSERVICE);
				// Check Out the selected assignment after update
				Utils<Facade::ILiveEditFacade>()->CheckOut(storyList, ICommand::kRegularUndo, nil, kFalse, kFullUI);
				break;
			}
			// On met a jour la reserve de l'article en interdisant l'extraction
			InterfacePtr<IBoolData> isDisplayShape(GetExecutionContextSession(), IID_IDISPLAYSHAPE);
			if (isDisplayShape->GetBool() == kTrue) {
				InterfacePtr<ICommand> labelCommand(CmdUtils::CreateCommand(kXPGSetArticleDataCmdBoss));
				if (labelCommand == nil)
					break;

				labelCommand->SetItemList(UIDList(selectedItems.GetRef(0)));
				InterfacePtr<IArticleData> labelData(labelCommand, UseDefaultIID());
				if (labelData == nil)
					break;

				labelData->SetStatus(colorStatus);
				CmdUtils::ProcessCommand(labelCommand);

				InterfacePtr<ICommand> addPageItemAdornmentCmd(CmdUtils::CreateCommand(kAddPageItemAdornmentCmdBoss));
				InterfacePtr<IClassIDData> classIDData(addPageItemAdornmentCmd, UseDefaultIID());
				classIDData->Set(kXPGUIArticleAdornmentBoss);
				addPageItemAdornmentCmd->SetItemList(UIDList(selectedItems.GetRef(0)));
				CmdUtils::ProcessCommand(addPageItemAdornmentCmd);
			}*/
		}
	}

	case IAMServiceProvider::enCancelCheckOut:
		service = (IAMService*)::CreateObject(kICCancelCheckOutServiceBoss, IID_IAMSERVICE);
		break;

	case IAMServiceProvider::enCheckIn:
		service = (IAMService*)::CreateObject(kICCheckInServiceBoss, IID_IAMSERVICE);
		break;

	}
	return service;
}


bool16 XPGIAMServiceProvider::CanService(const IDFile& file, const IAMServiceProvider::AssetHints* hints)
{

	PMString fileType = hints->kFileExtension;
	if (fileType == PMString(kXPGExtensionWithoutDotInCopyKey, PMString::kTranslateDuringCall) || fileType == PMString(kXPGExtensionAffectationWithoutDotKey, PMString::kTranslateDuringCall))
		return kTrue;
	return kFalse;
}


bool16 XPGIAMServiceProvider::CanService(const IManageableAsset* asset)
{
	return kTrue;
}

bool16 XPGIAMServiceProvider::CanUpdateScriptedAssetData(UIDRef assetRef, const PMString& scriptString, int32 scriptID)
{
	return kTrue;
}


ErrorCode XPGIAMServiceProvider::Init()
{

	return kSuccess;
}


bool16 XPGIAMServiceProvider::IsTemporaryVersion(const IDFile& file)
{
	return kTrue;
}


bool16 XPGIAMServiceProvider::IsVersioned(const IDFile& file)
{
	return kTrue;
}


IOpenFileCmdData::OpenFlags XPGIAMServiceProvider::OverrideOpenFlags(const IDFile& file, IOpenFileCmdData::OpenFlags inOpenFlags)
{
	return IOpenFileCmdData::kOpenDefault;
}

ErrorCode XPGIAMServiceProvider::UpdateScriptedAssetData(UIDRef assetRef, const PMString& scriptString, int32 scriptID)
{
	return kSuccess;
}


bool16 XPGIAMServiceProvider::IsWriteable(const IDFile& assetFile)
{
	return kTrue;
}

bool16 XPGIAMServiceProvider::GetLockData(const IDFile& assetFile, IManagedStatus::StatusStringTable& outLockData, UIFlags uiFlags)
{
	return kTrue;
}

bool16  XPGIAMServiceProvider::GetLinkResourceId(const IDFile& inFile, ILinkResource::ResourceId& outResourceId)
{

	return kFalse;

	/*bool succeeded  = Utils<IURIUtils>()->IDFileToURI(inFile, outResourceId);

		// Change state of ressource link
		InterfacePtr<ICommand> updateCmd(CmdUtils::CreateCommand(kLinkResourceStateUpdateCmdBoss));
		InterfacePtr<ILinkResourceStateUpdateCmdData> updateCmdData(updateCmd, UseDefaultIID());
		updateCmdData->SetUpdateAction(ILinkResourceStateUpdateCmdData::kUpdateState);
		updateCmdData->SetURI(outResourceId);
		updateCmdData->SetState(ILinkResource::kAvailable);
		CmdUtils::ProcessCommand(updateCmd);

		return succeeded;
	*/
}
