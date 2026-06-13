
#include "VCPluginHeaders.h"

// Interface includes

// Other API includes
#include "K2Vector.tpp"
#include "CAlert.h"

// Project includes
#include "XPGArticleDataNode.h"

/* Constructor
*/
XPGArticleDataNode::XPGArticleDataNode(NodeType paramtype)
: fParent(nil), fIDData(kNullString), fData(kNullString), articleData(nil)
{
	
	this->type = paramtype;
	articleData = new ArticleData();
	articleData->artID = kNullString;
	articleData->artSubject = kNullString;
	articleData->artPath = kNullString;
	articleData->artSnippetPath = kNullString;
	articleData->artCouleurStatus = kNullString;
	articleData->artLibStatus = kNullString;
	articleData->artNomProprietaire = kNullString;
	articleData->artPreview = kNullString;
	articleData->artRubrique = kNullString;
	articleData->artSsRubrique = kNullString;
	articleData->artNbPhotos = 0;
	articleData->artType = 0;
	articleData->artNbSignes = 0;
	articleData->artFolio = kNullString;
	articleData->artDossierCarton = kNullString;
	articleData->artFichierCarton = kNullString;
	articleData->images.clear();
}


/* Destructor
*/
XPGArticleDataNode::~XPGArticleDataNode()
{
	if(articleData)
		delete articleData;
}

/* Copy constructor
*/
XPGArticleDataNode::XPGArticleDataNode(const XPGArticleDataNode& rhs) 
: fParent(nil), fIDData(kNullString), articleData(nil)
{
	deepcopy(rhs);
}

/* Overloaded =
*/
XPGArticleDataNode& XPGArticleDataNode::operator=(const XPGArticleDataNode& rhs)
{
	deepcopy(rhs);
	return *this;
}


/* Overloaded ==
*/
bool XPGArticleDataNode::operator==(const XPGArticleDataNode& rhs) const
{
	if(this->type == rhs.type)
	{
		bool16 equal = this->fParent == rhs.fParent && 
					this->fIDData == rhs.fIDData &&
					this->fData == rhs.fData &&
					this->fChildren == rhs.fChildren;
		if(articleData)
			return	equal &&
				articleData->artID == rhs.articleData->artID &&
				articleData->artSubject == rhs.articleData->artSubject &&
				articleData->artPath == rhs.articleData->artPath &&
				articleData->artSnippetPath == rhs.articleData->artSnippetPath &&
				articleData->artCouleurStatus == rhs.articleData->artCouleurStatus &&
				articleData->artLibStatus == rhs.articleData->artLibStatus &&
				articleData->artNomProprietaire == rhs.articleData->artNomProprietaire &&
				articleData->artPreview == rhs.articleData->artPreview &&
				articleData->artRubrique == rhs.articleData->artRubrique &&
				articleData->artSsRubrique == rhs.articleData->artSsRubrique &&
				articleData->artFolio == rhs.articleData->artFolio &&
				articleData->artNbPhotos == rhs.articleData->artNbPhotos &&
				articleData->artType == rhs.articleData->artType &&
				articleData->artNbSignes == rhs.articleData->artNbSignes;
		else
			return equal;
	}
	else
		return kFalse;
}


/* deepcopy
*/
void XPGArticleDataNode::deepcopy(const XPGArticleDataNode& rhs)
{
	if(articleData)
		delete articleData;

	this->fChildren.clear();
	this->fChildren.assign( rhs.fChildren.begin(), rhs.fChildren.end());
	this->fParent = rhs.fParent;
	this->fIDData = rhs.fIDData;
	this->fData = rhs.fData;
	
	this->type = rhs.type;

	articleData = new ArticleData();
	articleData->artID = rhs.articleData->artID;
	articleData->artSubject = rhs.articleData->artSubject;
	articleData->artPath = rhs.articleData->artPath;
	articleData->artSnippetPath = rhs.articleData->artSnippetPath;
	articleData->artCouleurStatus = rhs.articleData->artCouleurStatus;
	articleData->artLibStatus = rhs.articleData->artLibStatus;
	articleData->artNomProprietaire = rhs.articleData->artNomProprietaire;
	articleData->artPreview = rhs.articleData->artPreview;
	articleData->artRubrique = rhs.articleData->artRubrique;
	articleData->artSsRubrique = rhs.articleData->artSsRubrique;
	articleData->artFolio = rhs.articleData->artFolio;
	articleData->artDossierCarton = rhs.articleData->artDossierCarton;
	articleData->artFichierCarton = rhs.articleData->artFichierCarton;
	articleData->artNbPhotos = rhs.articleData->artNbPhotos;
	articleData->artType = rhs.articleData->artType;
	articleData->artNbSignes = rhs.articleData->artNbSignes;
	articleData->images.assign(rhs.articleData->images.begin(), rhs.articleData->images.end());
}


/* GetNthChild
*/
const XPGArticleDataNode& XPGArticleDataNode::GetNthChild(
	int32 indexInParent) const
{
	return this->fChildren.at(indexInParent);
}


/* AddChild
*/
void XPGArticleDataNode::AddChild(const XPGArticleDataNode& o)
{
	this->fChildren.push_back(o);
}


/* RemoveChild
*/
void XPGArticleDataNode::RemoveChild(const XPGArticleDataNode& o)
{
	K2Vector<XPGArticleDataNode>::iterator result = std::find(fChildren.begin(), 
													fChildren.end(), o);

	if(result != fChildren.end())
	{
		fChildren.erase(result);
	}
}


/* GetParent
*/
XPGArticleDataNode* XPGArticleDataNode::GetParent() const
{
	return this->fParent;
}


/* SetParent
*/
void XPGArticleDataNode::SetParent(const XPGArticleDataNode* p)
{
	this->fParent = const_cast<XPGArticleDataNode* >(p);
}


/* ChildCount
*/
int32 XPGArticleDataNode::ChildCount() const
{
	return this->fChildren.size();
}


/* SetID
*/
void XPGArticleDataNode::SetID(const PMString& id)
{
	this->fIDData = id;
}


/* GetID
*/
PMString XPGArticleDataNode::GetID() const
{
	return this->fIDData;
}

PMString XPGArticleDataNode::GetData() const
{
	return this->fData;
}

void XPGArticleDataNode::SetData(const PMString& data)
{
	this->fData = data;
}

XPGArticleDataNode::NodeType XPGArticleDataNode::GetType() const
{
	return this->type;
}

	void XPGArticleDataNode::SetArticleData(const PMString& matching, const PMString& artID,
									const PMString& artSubject, const PMString& artPath, const PMString& artSnippetPath, const PMString& artCouleurStatus, 
									const PMString& artLibStatus, const PMString& artNomProprietaire, const PMString& artRubrique, const PMString& artSsRubrique)
{
								   

	if(articleData){
		articleData->artID = artID;
		articleData->artSubject = artSubject;
		articleData->artPath = artPath;
		articleData->artSnippetPath = artSnippetPath;
		articleData->artCouleurStatus = artCouleurStatus;
		articleData->artLibStatus = artLibStatus;
		articleData->artNomProprietaire = artNomProprietaire;
		articleData->artRubrique = artRubrique;
		articleData->artSsRubrique = artSsRubrique;
			}
}

void XPGArticleDataNode::SetArticleDataFromXML(const PMString& artPreview, const int32& artNbPhotos, const int32& artNbSignes, const int32& artType, const PMString& artFolio, const K2Vector<PMString>& images,
											  const PMString& dossierCarton, const PMString& fichierCarton)
{
	if(articleData){
		articleData->artPreview = artPreview;
		articleData->artNbPhotos = artNbPhotos;
		articleData->artNbSignes = artNbSignes;
		articleData->artFolio = artFolio;
		articleData->artDossierCarton = dossierCarton;
		articleData->artFichierCarton = fichierCarton;
		articleData->images.assign(images.begin(), images.end());
		articleData->artType = artType;
	}
}

const XPGArticleDataNode::ArticleData * XPGArticleDataNode::GetArticleData() const
{
	return articleData;
}

//	end, File:	XPGTexteDataNode.cpp
