

#ifndef __XPGMatchingStyleDataNode_H_DEFINED__
#define __XPGMatchingStyleDataNode_H_DEFINED__

/** Class to represent a generic node representing image data within a tree
*/

class XPGMatchingStyleDataNode
{
public:

	enum NodeType { kMatchingStyleNode =0, kNoMatchingStyleNode } ;

	typedef struct matchingStyleData 
	{
		PMString name, styleTag;
		
	} MatchingStyleData;

	XPGMatchingStyleDataNode(NodeType type);

	virtual ~XPGMatchingStyleDataNode();

	XPGMatchingStyleDataNode(const XPGMatchingStyleDataNode& rhs);

	/**	Operator assignment
		@param rhs [IN] node reference
		@return XPGImageDataNode& 
	 */
	XPGMatchingStyleDataNode& operator=(const XPGMatchingStyleDataNode& rhs);

	/** For K2Vector copy semantics
	*/
	typedef object_type data_type;
	// -K2Vector support

	/**	Operator equality
		@param rhs [IN] node reference
		@return bool 
	 */
	bool operator==(const XPGMatchingStyleDataNode& rhs) const;


	/**	Accessor for child by zerobased index in parent's list of kids
		@param indexInParent [IN] which child by zero-based index
		@return child at given index 
	 */
	const XPGMatchingStyleDataNode& GetNthChild(int32 indexInParent) const;

	/**	Accessor for parent
		@return reference to parent node
	 */
	XPGMatchingStyleDataNode* GetParent() const;

	/**	Accessor for size of child list on this node
		@return int32 giving the number of children on this node 
	 */
	int32 ChildCount() const;

	/**	Add child to this node (at end of list)
		@param o [IN] child node to add
	 */
	void AddChild(const XPGMatchingStyleDataNode& o);

	/**	Remove specified child from our list
		@param o [IN] specifies child to remove
	 */
	void RemoveChild(const XPGMatchingStyleDataNode& o);

	/** Mutator for the parent of this node
		@param p [IN] specifies new parent to set
	 */
	void SetParent(const XPGMatchingStyleDataNode* p);
	
	PMString GetID() const; 
	void SetID(const PMString& id);    
	
	PMString GetData() const; 
	void SetData(const PMString& data);

	NodeType GetType() const;

	const MatchingStyleData * GetMatchingStyleData() const; 

private:

	K2Vector<XPGMatchingStyleDataNode> fChildren;	
	PMString fIDData, fData;
	NodeType type;	
	MatchingStyleData * matchingStyleData;
	XPGMatchingStyleDataNode* fParent;	
	void deepcopy(const XPGMatchingStyleDataNode& rhs);
};

#endif // __XPGMatchingStyleDataNode_H_DEFINED__

