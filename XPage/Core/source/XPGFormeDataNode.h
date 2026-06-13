

#ifndef __XPGFormeDataNode_H_DEFINED__
#define __XPGFormeDataNode_H_DEFINED__

/** Class to represent a generic node representing image data within a tree
*/

class XPGFormeDataNode
{
public:

	enum NodeType { kClasseurNode = 0, kFormeNode } ;

	typedef struct formedata
	{
		PMString name, classeurName;
		PMString posX, posY;
		IDFile matchingFile;
		IDFile formeFile;

		// kTrue when the carton was created with the "Carton photo"
		// checkbox ticked. Sourced from the isPhotoCarton attribute on the
		// FORME element of the descriptif XML; missing on legacy descriptifs
		// (defaults to kFalse).
		bool16 isPhotoCarton;

		formedata() : isPhotoCarton(kFalse) {}

	} FormeData;

	/** Constructor */
	XPGFormeDataNode(NodeType type);

	/** Destructor */
	virtual ~XPGFormeDataNode();

	// +K2Vector support

	/**	Copy constructor
		@param rhs [IN] node reference
		@return  
	 */
	XPGFormeDataNode(const XPGFormeDataNode& rhs);

	/**	Operator assignment
		@param rhs [IN] node reference
		@return XPGImageDataNode& 
	 */
	XPGFormeDataNode& operator=(const XPGFormeDataNode& rhs);

	/** For K2Vector copy semantics
	*/
	typedef object_type data_type;
	// -K2Vector support

	/**	Operator equality
		@param rhs [IN] node reference
		@return bool 
	 */
	bool operator==(const XPGFormeDataNode& rhs) const;


	/**	Accessor for child by zerobased index in parent's list of kids
		@param indexInParent [IN] which child by zero-based index
		@return child at given index 
	 */
	const XPGFormeDataNode& GetNthChild(int32 indexInParent) const;

	/**	Accessor for parent
		@return reference to parent node
	 */
	XPGFormeDataNode* GetParent() const;

	/**	Accessor for size of child list on this node
		@return int32 giving the number of children on this node 
	 */
	int32 ChildCount() const;

	/**	Add child to this node (at end of list)
		@param o [IN] child node to add
	 */
	void AddChild(const XPGFormeDataNode& o);

	/**	Remove specified child from our list
		@param o [IN] specifies child to remove
	 */
	void RemoveChild(const XPGFormeDataNode& o);

	/** Mutator for the parent of this node
		@param p [IN] specifies new parent to set
	 */
	void SetParent(const XPGFormeDataNode* p);

	/**
		Data accessors for forme node 
	*/

	PMString GetID() const; // ID is the file path
	void SetID(const PMString& id);
	
	NodeType GetType() const;
	
	PMString GetData() const; // data is the forme absolute file path for kForme nodes, it's a rubrique name for kRubrique nodes
	void SetData(const PMString& data);
	
	void SetFormeData(const PMString& name, const PMString& classeurName, const IDFile& matchingFile, const IDFile& formeFile, const PMString& posX, const PMString& posY, const bool16 isPhotoCarton = kFalse);
	const FormeData * GetFormeData() const; // Forme data is data extracted from description file for kFormeNode nodes, it's nil for kRubrique nodes

private:

	K2Vector<XPGFormeDataNode> fChildren;	
	PMString fIDData, fData;
	NodeType type;
	FormeData * formeData;
	XPGFormeDataNode* fParent;
	void deepcopy(const XPGFormeDataNode& rhs);
};

#endif // __XPGFormeDataNode_H_DEFINED__

