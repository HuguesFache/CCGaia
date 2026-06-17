#include "VCPlugInHeaders.h"

// Interface includes
#include "ISubject.h"
#include "IWidgetParent.h"
#include "ITreeNodeIDData.h"
#include "ITriStateControlData.h"
#include "IPMUnknownData.h"
#include "IWidgetUtils.h"

// General includes
#include "CObserver.h"
#include "CreateObject.h"    // ::CreateObject2<>
#include "NodeID.h"          // TreeNodePtr<>
#include "IntNodeID.h"

// Project includes
#include "XRLUIID.h"
#include "IXRLUICommentList.h"
#include "IWebServices.h"
#include "XRCID.h"           // kXRCXRailClientBoss
#include "DocUtils.h"        // GetGoodUrlXR

/** Observer aggregated on kXRLUICommentsCellCheckWidgetBoss — la case à cocher
	de validation d'une ligne de la palette Commentaires. Mirror du pattern
	ConditionTagEyeballObserver du SDK : la case dérive d'un checkbox standard
	(le toggle visuel est géré par l'event handler de base), cet observer ne
	fait qu'écouter le changement d'état pour appeler le service serveur.

	Au toggle utilisateur (kTrueStateMessage / kFalseStateMessage) on :
	  1. retrouve l'index de ligne via le ITreeNodeIDData du node parent,
	  2. lit l'IDComm dans le modèle IXRLUICommentList,
	  3. appelle Plugin_Comm_Valid (toggle côté Gaia),
	  4. synchronise le flag check du modèle pour éviter un retour visuel au
	     recyclage du tree view.

	SetState(..., kFalse, kFalse) dans le widget mgr n'émet pas de message :
	seuls les clics utilisateur déclenchent cet Update.
*/
class XRLUICommentsCellCheckObserver : public CObserver
{
public:
	XRLUICommentsCellCheckObserver(IPMUnknown* boss) : CObserver(boss) {}
	virtual ~XRLUICommentsCellCheckObserver() {}

	virtual void AutoAttach();
	virtual void AutoDetach();
	virtual void Update(const ClassID& theChange, ISubject* theSubject, const PMIID& protocol, void* changedBy);

private:
	int32 ResolveRowIndex() const;
};

CREATE_PMINTERFACE(XRLUICommentsCellCheckObserver, kXRLUICommentsCellCheckObserverImpl)

void XRLUICommentsCellCheckObserver::AutoAttach()
{
	InterfacePtr<ISubject> subject(this, IID_ISUBJECT);
	if (subject != nil)
		subject->AttachObserver(this, IID_ITRISTATECONTROLDATA);
}

void XRLUICommentsCellCheckObserver::AutoDetach()
{
	InterfacePtr<ISubject> subject(this, IID_ISUBJECT);
	if (subject != nil)
		subject->DetachObserver(this, IID_ITRISTATECONTROLDATA);
}

int32 XRLUICommentsCellCheckObserver::ResolveRowIndex() const
{
	// L'IntNodeID (index de ligne) est porté par le node parent, pas par la
	// case elle-même : on remonte la hiérarchie de widgets jusqu'au porteur de
	// ITreeNodeIDData. Même mécanique que ConditionTagEyeballObserver.
	InterfacePtr<IWidgetParent> widgetParent(this, UseDefaultIID());
	if (widgetParent == nil)
		return -1;
	InterfacePtr<ITreeNodeIDData> nodeData((ITreeNodeIDData*)widgetParent->QueryParentFor(IID_ITREENODEIDDATA));
	if (nodeData == nil)
		return -1;
	TreeNodePtr<IntNodeID> nodeID(nodeData->Get());
	if (nodeID == nil)
		return -1;
	return nodeID->Get();
}

void XRLUICommentsCellCheckObserver::Update(const ClassID& theChange, ISubject* /*theSubject*/, const PMIID& /*protocol*/, void* /*changedBy*/)
{
	if (theChange != kTrueStateMessage && theChange != kFalseStateMessage)
		return;

	const int32 rowIndex = this->ResolveRowIndex();
	if (rowIndex < 0)
		return;

	// Modèle de données accroché sur le tree view (IID_IXRLUIDATAMODEL).
	IPMUnknown* dataSlot = Utils<IWidgetUtils>()->QueryRelatedWidget(this, kXRLUICommentsViewWidgetID, IID_IXRLUIDATAMODEL);
	if (dataSlot == nil)
		return;
	InterfacePtr<IPMUnknownData> dataModel(dataSlot, IID_IXRLUIDATAMODEL);
	if (dataModel == nil)
		return;
	InterfacePtr<IXRLUICommentList> list(dataModel->GetPMUnknown(), IID_IXRLUICOMMENTLIST);
	if (list == nil || rowIndex >= list->GetCount())
		return;

	const int32 idComm = list->GetAt(rowIndex).idComm;

	PMString baseName;
	const PMString serverURL = GetGoodUrlXR(baseName);
	if (serverURL.IsEmpty())
		return;

	InterfacePtr<IWebServices> http(::CreateObject2<IWebServices>(kXRCXRailClientBoss));
	if (http == nil)
		return;

	http->CommValid(serverURL, idComm);

	// Garde le modèle synchrone avec l'état affiché (kTrueStateMessage = coché).
	list->SetCheckAt(rowIndex, theChange == kTrueStateMessage);
}
