
#include "VCPlugInHeaders.h"

// Interface includes:
#include "ITreeNodeIDData.h"

// General includes:
#include "AbstractTip.h"

// Project includes:
#include "XPGUIID.h"
#include "XPGUIArticleNodeID.h"

/** Infobulle d'une ligne de la liste des articles (palette XPage) :
	affiche le commentaire de l'article (<commentaires> de l'OBJRART.xml).
	Aggregee sur kXPGUIArticleNodeWidgetBoss ; le noeud est lu via
	ITreeNodeIDData (meme principe que ConditionTagTVNodeTip du SDK).
*/
class XPGUIArticleNodeTip : public AbstractTip
{
public:
	XPGUIArticleNodeTip(IPMUnknown* boss) : AbstractTip(boss) {}
	virtual ~XPGUIArticleNodeTip() {}

	virtual PMString GetTipText(const PMPoint& mouseLocation);
};

CREATE_PMINTERFACE(XPGUIArticleNodeTip, kXPGUIArticleNodeTipImpl)


PMString XPGUIArticleNodeTip::GetTipText(const PMPoint& /*mouseLocation*/)
{
	PMString tip;
	do{
		InterfacePtr<ITreeNodeIDData> nodeData(this, UseDefaultIID());
		if(nodeData == nil)
			break;

		TreeNodePtr<XPGUIArticleNodeID> nodeID(nodeData->Get());
		if(nodeID == nil || nodeID->GetArticleData() == nil)
			break;

		tip = nodeID->GetArticleData()->artCommentaire;
		tip.SetTranslatable(kFalse);
	} while(kFalse);

	return tip;
}

//	end, File: XPGUIArticleNodeTip.cpp
