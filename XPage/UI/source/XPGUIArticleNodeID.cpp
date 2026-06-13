
#include "VCPluginHeaders.h"

// Interface includes:
#include "IPMStream.h"

// Project includes:
#include "XPGUIArticleNodeID.h"
#include "CAlert.h"

/* Private constructors
*/

XPGUIArticleNodeID::XPGUIArticleNodeID(PMString id, PMString data, XPGArticleDataNode::NodeType paramtype, const XPGArticleDataNode::ArticleData * paramarticleData)
{
	
	this->fIDData = id;
	this->fData = data;
	this->type = paramtype;
	this->articleData = nil;
	if(paramarticleData)
	{
		this->articleData = new XPGArticleDataNode::ArticleData();
		this->articleData->artID = paramarticleData->artID;
		this->articleData->artSubject = paramarticleData->artSubject;
		this->articleData->artPath = paramarticleData->artPath;
		this->articleData->artSnippetPath = paramarticleData->artSnippetPath;
		this->articleData->artCouleurStatus = paramarticleData->artCouleurStatus;
		this->articleData->artLibStatus = paramarticleData->artLibStatus;
		this->articleData->artNomProprietaire = paramarticleData->artNomProprietaire;
		this->articleData->artPreview = paramarticleData->artPreview;
		this->articleData->artRubrique = paramarticleData->artRubrique;
		this->articleData->artSsRubrique = paramarticleData->artSsRubrique;
		this->articleData->artNbPhotos = paramarticleData->artNbPhotos;
		this->articleData->artType = paramarticleData->artType;
		this->articleData->artNbSignes = paramarticleData->artNbSignes;
		this->articleData->artFolio = paramarticleData->artFolio;
		this->articleData->images.assign(paramarticleData->images.begin(), paramarticleData->images.end());
		
	}
}
/* Compare
*/
int32 XPGUIArticleNodeID::Compare(const NodeIDClass* nodeID) const
{
	const XPGUIArticleNodeID* fileNode = static_cast<const XPGUIArticleNodeID*>(nodeID);
	ASSERT(nodeID);

	const XPGUIArticleNodeID* thisNode = const_cast<const XPGUIArticleNodeID*>(this);

	if(thisNode->type == fileNode->GetType()){

		int32 compareResult = thisNode->fIDData.Compare(kFalse, fileNode->GetID());
		if(compareResult != 0)
			return compareResult;

		else if(articleData){
			bool equal = thisNode->articleData->artID == fileNode->articleData->artID &&
				thisNode->articleData->artSubject == fileNode->articleData->artSubject &&
				thisNode->articleData->artPath == fileNode->articleData->artPath &&
				thisNode->articleData->artSnippetPath == fileNode->articleData->artSnippetPath &&
				thisNode->articleData->artCouleurStatus == fileNode->articleData->artCouleurStatus &&
				thisNode->articleData->artLibStatus == fileNode->articleData->artLibStatus &&
				thisNode->articleData->artNomProprietaire == fileNode->articleData->artNomProprietaire &&
				thisNode->articleData->artPreview == fileNode->articleData->artPreview &&
				thisNode->articleData->artRubrique == fileNode->articleData->artRubrique &&
				thisNode->articleData->artSsRubrique == fileNode->articleData->artSsRubrique &&
				thisNode->articleData->artNbPhotos == fileNode->articleData->artNbPhotos &&
				thisNode->articleData->artType == fileNode->articleData->artType &&
				thisNode->articleData->artNbSignes == fileNode->articleData->artNbSignes &&
				thisNode->articleData->artFolio == fileNode->articleData->artFolio;

			if(equal)
				return 0;
			else
				return -1;
		}
		else
			return 0;
	}
	else{
		if(this->type < fileNode->GetType())
			return -1;
		else
			return 1;
	}
}


/* Clone
*/
NodeIDClass* XPGUIArticleNodeID::Clone() const {

	return new XPGUIArticleNodeID(this->GetID(), this->fData, this->type, this->articleData);
}

/* Read
*/
void XPGUIArticleNodeID::Read(IPMStream* stream)
{
	fIDData.ReadWrite(stream);
	fData.ReadWrite(stream);
	int16 typeIntValue = XPGArticleDataNode::kCorpsNode;
	stream->XferInt16(typeIntValue);
	type = (XPGArticleDataNode::NodeType) typeIntValue;
	bool16 ArticleDataExist;
	stream->XferBool(ArticleDataExist);
	if(ArticleDataExist){
		if(!articleData)
		{
			articleData = new XPGArticleDataNode::ArticleData();
		}

		articleData->artID.ReadWrite(stream);
		articleData->artSubject.ReadWrite(stream);
		articleData->artPath.ReadWrite(stream);
		articleData->artSnippetPath.ReadWrite(stream);
		articleData->artCouleurStatus.ReadWrite(stream);
		articleData->artLibStatus.ReadWrite(stream);
		articleData->artNomProprietaire.ReadWrite(stream);
		articleData->artPreview.ReadWrite(stream);
		articleData->artRubrique.ReadWrite(stream);
		articleData->artSsRubrique.ReadWrite(stream);
		articleData->artFolio.ReadWrite(stream);
		stream->XferInt32(articleData->artNbPhotos);
		stream->XferInt32(articleData->artType);
		stream->XferInt32(articleData->artNbSignes);
		int32 nbImages;
		articleData->images.clear();
		stream->XferInt32((int32&)nbImages);
		for(int32 i = 0 ; i < nbImages ; ++i){
			PMString imageID;
			imageID.ReadWrite(stream);
			articleData->images.push_back(imageID);
		}
		//TODOarticleData->images.clear();
	}
	else
	{
		if(articleData)
		{
			delete articleData;
		}
		articleData = nil;
	}
}


/* Write
*/
void XPGUIArticleNodeID::Write(IPMStream* stream) const
{
	(const_cast<XPGUIArticleNodeID*>(this)->fIDData).ReadWrite(stream);
	(const_cast<XPGUIArticleNodeID*>(this)->fData).ReadWrite(stream);
	int16 typeIntValue = type;
	stream->XferInt16(typeIntValue);
	bool16 ArticleDataExist = (articleData != nil);
	if(ArticleDataExist)
	{
		stream->XferBool(ArticleDataExist);
		
		articleData->artID.ReadWrite(stream);
		articleData->artSubject.ReadWrite(stream);
		articleData->artPath.ReadWrite(stream);
		articleData->artSnippetPath.ReadWrite(stream);
		articleData->artCouleurStatus.ReadWrite(stream);
		articleData->artLibStatus.ReadWrite(stream);
		articleData->artNomProprietaire.ReadWrite(stream);
		articleData->artPreview.ReadWrite(stream);
		articleData->artRubrique.ReadWrite(stream);
		articleData->artSsRubrique.ReadWrite(stream);
		articleData->artFolio.ReadWrite(stream);
		stream->XferInt32(articleData->artNbPhotos);
		stream->XferInt32(articleData->artType);
		stream->XferInt32(articleData->artNbSignes);
		
		int32 nbImages = articleData->images.size();
		stream->XferInt32((int32&)nbImages);
		for(int32 i = 0 ; i < nbImages ; ++i){
			articleData->images[i].ReadWrite(stream);
		}
	}
	else
	{
		stream->XferBool(ArticleDataExist);
	}
}
