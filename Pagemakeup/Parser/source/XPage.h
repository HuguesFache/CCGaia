/*
//	File:	XPage.h
//
//	Date:	21-Jul-2011
//
//  Author : Trias
//
*/


#ifndef __XPage_h__
#define __XPage_h__


class ITCLParser;

/** Func_AF
	Implement the AF command
*/
class Func_AF
{
public :

	/**
		Constructor
		Read MN command parameters and call IDCall_AF
		@param IN parser : TCL parser
	*/
	Func_AF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AF() {}

private :
	
	/**
		Import a forme into a page, optionaly creating a resa at the same time
	*/
	void IDCall_AF(ITCLParser *  parser);

	PMReal hPos, vPos;
	PMString formePath;
	int32 idResa;
};

/** Func_AC
 Implement the AI command
 */
class Func_AC
{
    public :
    
	/**
     Constructor
     Read MN command parameters and call IDCall_AC
     @param IN parser : TCL parser
     */
	Func_AC(ITCLParser *  parser);
    
	/**
     Destructor
     */
	~Func_AC() {}
    
    private :
	
	/**
     Import a story into a resa
     */
	void IDCall_AC(ITCLParser *  parser);
    
	int32 idResa;
	PMString pathArt, matchingPath;
};

/** Func_AZ
 Implement the AI command
 */
class Func_AZ
{
    public :
    
	/**
     Constructor
     Read MN command parameters and call IDCall_AC
     @param IN parser : TCL parser
     */
	Func_AZ(ITCLParser *  parser);
    
	/**
     Destructor
     */
	~Func_AZ() {}
    
    private :
	
	/**
     Import a story into a resa
     */
	void IDCall_AZ(ITCLParser *  parser);
    
	int32 idResa;
	PMString pathArt, matchingPath;
};

/** Func_AA
	Implement the AA command
*/
class Func_AA
{
public :

	/**
		Constructor
		Read AA command parameters and call IDCall_AA
		@param IN parser : TCL parser
	*/
	Func_AA(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AA() {}

private :
	
	/**
		Convert a resa into an InCopy Story
	*/
	void IDCall_AA(ITCLParser *  parser);

	int32 idResa;
	PMString inCopyAssignmentPath, topic, recipientName;
};

/** Func_AM
	Implement the AM command
*/
class Func_AM
{
public :

	/**
		Constructor
		Read AM command parameters and call IDCall_AM
		@param IN parser : TCL parser
	*/
	Func_AM(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AM() {}

private :
	
	/**
		Convert a resa into an InCopy Story
	*/
	void IDCall_AM(ITCLParser *  parser);

	PMString idStory;
	int16 typeMarbre;
};


/** Func_AB
	Implement the AB command
*/
class Func_AB
{
public :

	/**
		Constructor
		Read AB command parameters and call IDCall_AB
		@param IN parser : TCL parser
	*/
	Func_AB(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AB() {}

private :
	
	/**
		Import an assembly into a page, optionaly creating resa at the same time
	*/
	void IDCall_AB(ITCLParser *  parser);

	PMReal hPos, vPos;
	PMString assemblyPath;
	K2Vector<KeyValuePair<PMString, int32> > resas; // Pair of (ID_Forme, ID_Resas)
};

/** Func_AD
	Implement the AD command
*/
class Func_AD
{
public :

	/**
		Constructor
		Read AD command parameters and call IDCall_AD
		@param IN parser : TCL parser
	*/
	Func_AD(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AD() {}

private :
	
	/**
		Convert a resa into an InCopy Story
	*/
	void IDCall_AD(ITCLParser *  parser);

	int32 idStory;
};

/** Func_AE
	
 */
class Func_AE
{
    public :
    
    /**
     Constructor
     @param IN parser : TCL parser
     */
    Func_AE(ITCLParser *  parser);
    
    /**
     Destructor
     */
    ~Func_AE() {}
    
    private :
    
    /**
     Delete all "TriasStoryData" (= PlacedArticleData) in current doc
     */
    void IDCall_AE(ITCLParser *  parser);
    
};

#endif // __XPage_h__