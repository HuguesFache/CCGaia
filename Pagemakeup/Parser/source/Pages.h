/*
//	File:	Textes.h
//
//	Date:	24-Sep-2003
//
//  Author : Nicolas Metaye
//
*/


#ifndef __Pages_h__
#define __Pages_h__


class ITCLParser;


/** Func_GI
	Implement the GI command
*/
class Func_GI
{
public :

	/**
		Constructor
		Read GI command parameters and call IDCall_GI
		@param IN parser : TCL parser
	*/
	Func_GI(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_GI();

private :
		
	int32 after ;
	int32 nbPage ;
	bool8 applyPreviousMaster;


	/**
		launch the cmd which creates page
	*/
	void IDCall_GI(ITCLParser *  parser);
};


/** Func_GA
	Implement the GA command
*/
class Func_GA
{
public :

	/**
		Constructor
		Read GA command parameters and call IDCall_GA
		@param IN parser : TCL parser
	*/
	Func_GA(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_GA();

private :
		
	int32 numPage ;


	/**
		set the current page
	*/
	void IDCall_GA(ITCLParser *  parser);
};

/** Func_OV
 Implement the OV command
 */
class Func_OV
{
    public :
    
	/**
     Constructor
     Read GA command parameters and call IDCall_OV
     @param IN parser : TCL parser
     */
	Func_OV(ITCLParser *  parser);
    
	/**
     Destructor
     */
	~Func_OV();
    
    private :
    
	int32 numPage ;
    
    /**
     set the current page
     */
	void IDCall_OV(ITCLParser *  parser);
};



/** Func_DD
	Implement the DD command
*/
class Func_DD
{
public :

	/**
		Constructor
		Read DD command parameters and call IDCall_DD
		@param IN parser : TCL parser
	*/
	Func_DD(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DD();

private :
		
	int32 numPage;

	int32 numStart;

	PMString prefixe ;
	
    PMString marker;

	int32 pageStyle ;

	bool16 autoNumber, includeSectionPrefix;

	/**
		set the current page
	*/
	void IDCall_DD(ITCLParser *  parser);
};


/** Func_GPLUS
	Implement the G+ command
*/
class Func_GPLUS
{
public :

	/**
		Constructor
		Read GPLUS command parameters and call IDCall_GPLUS
		@param IN parser : TCL parser
	*/
	Func_GPLUS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_GPLUS();

private :
		
	/**
		set the current page + 1
	*/
	void IDCall_GPLUS(ITCLParser *  parser);
};


/** Func_GMOINS
	Implement the G- command
*/
class Func_GMOINS
{
public :

	/**
		Constructor
		Read GMOINS command parameters and call IDCall_GMOINS
		@param IN parser : TCL parser
	*/
	Func_GMOINS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_GMOINS();

private :
		
	/**
		set the current page - 1
	*/
	void IDCall_GMOINS(ITCLParser *  parser);
};



/** Func_GS
	Implement the GS command
*/
class Func_GS
{
public :

	/**
		Constructor
		Read GS command parameters and call IDCall_GS
		@param IN parser : TCL parser
	*/
	Func_GS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_GS();

private :
		
	int32 numPage ;


	/**
		delete page passed in argument
	*/
	void IDCall_GS(ITCLParser *  parser);
};

/** Func_GB
	Implement the GB command
*/
class Func_GB
{
public :

	/**
		Constructor
		Read GB command parameters and call IDCall_GB
		@param IN parser : TCL parser
	*/
	Func_GB(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_GB();
		
};


/** Func_MA
	Implement the MA command
*/
class Func_MA
{
public :

	/**
		Constructor
		Read MA command parameters and call IDCall_MA
		@param IN parser : TCL parser
	*/
	Func_MA(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_MA();

private :
	
	PMString masterName;	
	int32 startPage, endPage;


	/**
		Apply master to a range of pages
	*/
	void IDCall_MA(ITCLParser *  parser);
};


/** Func_MD
	Implement the MD command
*/
class Func_MD
{
public :

	/**
		Constructor
		Read MD command parameters and call IDCall_MD
		@param IN parser : TCL parser
	*/
	Func_MD(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_MD();

private :
	
	PMString masterName, basedOn;	
	int32 nbPages;


	/**
		Create a new master
	*/
	void IDCall_MD(ITCLParser *  parser);
};

/** Func_MF
	Implement the MF command
*/
class Func_MF
{
public :

	/**
		Constructor
		End up the creation of a new master
		@param IN parser : TCL parser
	*/
	Func_MF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_MF();
};

/** Func_ON
	Implement the ON command
*/
class Func_ON
{
public :

	/**
		Constructor
		Read ON command parameters and call IDCall_ON
		@param IN parser : TCL parser
	*/
	Func_ON(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_ON() {}

private :
	
	/**
		Set next odd page as current page
	*/
	void IDCall_ON(ITCLParser *  parser);
};

/** Func_EN
	Implement the EN command
*/
class Func_EN
{
public :

	/**
		Constructor
		Read EN command parameters and call IDCall_EN
		@param IN parser : TCL parser
	*/
	Func_EN(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_EN() {}

private :
	
	/**
		Set next even page as current page
	*/
	void IDCall_EN(ITCLParser *  parser);
};

/** Func_OL
	Implement the OL command
*/
class Func_OL
{
public :

	/**
		Constructor
		Read OL command parameters and call IDCall_OL
		@param IN parser : TCL parser
	*/
	Func_OL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_OL() {}

private :
	
	/**
		Set last odd page as current page
	*/
	void IDCall_OL(ITCLParser *  parser);
};

/** Func_EL
	Implement the EL command
*/
class Func_EL
{
public :

	/**
		Constructor
		Read EL command parameters and call IDCall_EL
		@param IN parser : TCL parser
	*/
	Func_EL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_EL() {}

private :
	
	/**
		Set last even page as current page
	*/
	void IDCall_EL(ITCLParser *  parser);
};


#endif //__Pages_h__