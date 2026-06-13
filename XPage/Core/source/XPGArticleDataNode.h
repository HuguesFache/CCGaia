
#ifndef __XPGArticleDataNode_H_DEFINED__
#define __XPGArticleDataNode_H_DEFINED__

/** Class to represent a generic node representing text data within a tree, 
*/
class XPGArticleDataNode
{
public:

	enum NodeType { kNoType = 0, kCorpsNode } ;

	typedef struct articledata 
	{
		PMString artID;					//id de l'article
		PMString artSubject;			//sujet de l'article
		PMString artPath;				//path vers le xml de l'article
		PMString artSnippetPath;		//path vers le snippet, pour les articles incopy froid
		PMString artCouleurStatus;		//couleur du status
		PMString artLibStatus;			//libelle du status. Ces deux valeurs servent pour envoyer le status de l'article
										//a xrail, apres placement, pour afficher la reserve
		PMString artNomProprietaire;	//nom du proprio de l'article ??? pas utile 
		PMString artPreview;			//debut du texte de l'article
		PMString artRubrique;			//id de la rubrique de l'article. 
		PMString artSsRubrique;			//id de la sous rubrique
		PMString artFolio;				//folio de l'article
		PMString artDossierCarton;		//classeur par defaut (element <DossierCarton> du XML), peut etre vide
		PMString artFichierCarton;		//carton par defaut (element <FichierCarton> du XML), peut etre vide
		int32 artNbPhotos, artType, artNbSignes;		//nb de photos, type de l'article et nb de signes
		 		
		K2Vector<PMString> images;		//liste des photos associees a l'article

	} ArticleData;

	/** Constructor */
	XPGArticleDataNode(NodeType type);

	/** Destructor */
	virtual ~XPGArticleDataNode();

	// +K2Vector support

	/**	Copy constructor
		@param rhs [IN] node reference
		@return  
	 */
	XPGArticleDataNode(const XPGArticleDataNode& rhs);

	/**	Operator assignment
		@param rhs [IN] node reference
		@return XPGTexteDataNode& 
	 */
	XPGArticleDataNode& operator=(const XPGArticleDataNode& rhs);

	/** For K2Vector copy semantics
	*/
	typedef object_type data_type;
	// -K2Vector support

	/**	Operator equality
		@param rhs [IN] node reference
		@return bool 
	 */
	bool operator==(const XPGArticleDataNode& rhs) const;


	/**	Accessor for child by zerobased index in parent's list of kids
		@param indexInParent [IN] which child by zero-based index
		@return child at given index 
	 */
	const XPGArticleDataNode& GetNthChild(int32 indexInParent) const;

	/**	Accessor for parent
		@return reference to parent node
	 */
	XPGArticleDataNode* GetParent() const;

	/**	Accessor for size of child list on this node
		@return int32 giving the numero of children on this node 
	 */
	int32 ChildCount() const;	

	/**	Add child to this node (at end of list)
		@param o [IN] child node to add
	 */
	void AddChild(const XPGArticleDataNode& o);

	/**	Remove specified child from our list
		@param o [IN] specifies child to remove
	 */
	void RemoveChild(const XPGArticleDataNode& o);

	/** Mutator for the parent of this node
		@param p [IN] specifies new parent to set
	 */
	void SetParent(const XPGArticleDataNode* p);

	/**
		Data accessors for text node 
	*/

	PMString GetID() const; // TRAFFIC : article ID
							// otherwise : ID is the path of the descriptif file (this ID is suffixed for "Hors Texte" nodes)
	void SetID(const PMString& id);

	PMString GetData() const; // data is the article file path for kCorps or kHorsTexte nodes, it's a rubrique name for kRubrique nodes
	void SetData(const PMString& data);

	NodeType GetType() const;

	void SetArticleData(const PMString& matching, const PMString& artID, const PMString& artSubject, const PMString& artPath, const PMString& artSnippetPath, const PMString& artCouleurStatus, 
								   const PMString& artLibStatus, const PMString& artNomProprietaire, const PMString& artRubrique, const PMString& artSsRubrique);
	
	void SetArticleDataFromXML(const PMString& artPreview, const int32& artNbPhotos, const int32& artNbSignes, const int32& artType, const PMString& folio, const K2Vector<PMString>& images,
							   const PMString& dossierCarton = kNullString, const PMString& fichierCarton = kNullString);

	const ArticleData * GetArticleData() const; // Text data is data extracted from description file for kCorps or kHorsTexte nodes, it's nil for kRubrique nodes
	
private:

	K2Vector<XPGArticleDataNode> fChildren;	
	PMString fIDData, fData;	
	NodeType type;
	ArticleData * articleData;
	XPGArticleDataNode* fParent;
	void deepcopy(const XPGArticleDataNode& rhs);

	/* Here we put default constructor so that it can't be used (it doesn't make sense) */
	XPGArticleDataNode() {}
};

#endif // __XPGArticleDataNode_H_DEFINED__

