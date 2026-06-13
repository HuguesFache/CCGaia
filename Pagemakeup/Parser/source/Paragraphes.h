/*
//	File:	Textes.h
//
//	Date:	24-Sep-2003
//
//  Author : Nicolas Metaye
//
*/


#ifndef __Paragraphes_h__
#define __Paragraphes_h__

class ITCLParser;

#include "TabStop.h"


/** Func_PE
	Implement the PE command
*/
class Func_PE
{
public :

	/**
		Constructor
		Read PE command parameters and call IDCall_PE
		@param IN parser : TCL parser
	*/
	Func_PE(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PE();

private :
		
	PMReal spaceBefore ; //value of the spaceBefore attribute
	PMReal spaceAfter ; //value of the spaceAfter attribute


	/**
		Set the space attribute in the attributeBossList
	*/
	void IDCall_PE(ITCLParser *  parser);
};


/** Func_PG
	Implement the PG command
*/
class Func_PG
{
public :

	/**
		Constructor
		Read PG command parameters and call IDCall_PG
		@param IN parser : TCL parser
	*/
	Func_PG(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PG();

private :
		
	bool8 gridAlign ;

	/**
		Set the grid alignment attribute in the attributeBossList
	*/
	void IDCall_PG(ITCLParser *  parser);
};


/** Func_PH
	Implement the PH command
*/
class Func_PH
{
public :

	/**
		Constructor
		Read PH command parameters and call IDCall_PH
		@param IN parser : TCL parser
	*/
	Func_PH(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PH();

private :

	bool8 keepLines ;
	bool8 all ;
	int32 firstLines ;
	int32 lastLines ;

	/**
		Set the KeepLines attribute in the attributeBossList
	*/
	void IDCall_PH(ITCLParser *  parser);
};


/** Func_PI
	Implement the PI command
*/
class Func_PI
{
public :

	/**
		Constructor
		Read PI command parameters and call IDCall_PI
		@param IN parser : TCL parser
	*/
	Func_PI(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PI();

private :

	PMReal lead;

	/**
		Set the lead attribute in the attributeBossList
	*/
	void IDCall_PI(ITCLParser *  parser);
};


/** Func_PJ
	Implement the PJ command
*/
class Func_PJ
{
public :

	/**
		Constructor
		Read PJ command parameters and call IDCall_PJ
		@param IN parser : TCL parser
	*/
	Func_PJ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PJ();

private :

	int32 align;

	/**
		Set the align attribute in the attributeBossList
	*/
	void IDCall_PJ(ITCLParser *  parser);
};


/** Func_PF
	Implement the PF command
*/
class Func_PF
{
public :

	/**
		Constructor
		Read PF command parameters and call IDCall_PF
		@param IN parser : TCL parser
	*/
	Func_PF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PF();

private :


	/**
		Set the align attribute in the attributeBossList
	*/
	void IDCall_PF(ITCLParser *  parser);
};


/** Func_PA
	Implement the PA command
*/
class Func_PA
{
public :

	/**
		Constructor
		Read PA command parameters and call IDCall_PA
		@param IN parser : TCL parser
	*/
	Func_PA(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PA();

private :

	PMReal stroke ;
	bool8 flag ;
	int16 type ;
	PMReal indentL ;
	PMReal indentR ;
	PMReal offset ;
	int32 refColor ;
	PMReal tint ;
	bool8 ruleOn;

	/**
		Set the paragraph rule above attribute in the attributeBossList
	*/
	void IDCall_PA(ITCLParser *  parser);
};


/** Func_PP
	Implement the PP command
*/
class Func_PP
{
public :

	/**
		Constructor
		Read PP command parameters and call IDCall_PP
		@param IN parser : TCL parser
	*/
	Func_PP(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PP();

private :

	PMReal stroke ;
	bool8 flag ;
	int16 type ;
	PMReal indentL ;
	PMReal indentR ;
	PMReal offset ;
	int32 refColor ;
	PMReal tint ;
	bool8 ruleOn;


	/**
		Set the paragraph rule below attribute in the attributeBossList
	*/
	void IDCall_PP(ITCLParser *  parser);
};


/** Func_PR
	Implement the PRcommand
*/
class Func_PR
{
public :

	/**
		Constructor
		Read PR command parameters and call IDCall_PR
		@param IN parser : TCL parser
	*/
	Func_PR(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PR();

private :

	PMReal indentL ;
	PMReal indentR ;
	PMReal alinea ;


	/**
		Set the indent attribute in the attributeBossList
	*/
	void IDCall_PR(ITCLParser *  parser);
};



/** Func_PT
	Implement the PT command
*/
class Func_PT
{
public :

	/**
		Constructor
		Read PT command parameters and call IDCall_PT
		@param IN parser : TCL parser
	*/
	Func_PT(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PT();

private :

	TabStopTable * tabTable ;
	
	/**
		Set the tabs attribute in the attributeBossList
	*/
	void IDCall_PT(ITCLParser *  parser);
};


/** Func_PC
	Implement the PC command
*/
class Func_PC
{
public :

	/**
		Constructor
		Read PC command parameters and call IDCall_PC
		@param IN parser : TCL parser
	*/
	Func_PC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PC();

private :

	PMString name ;

	/**
		Apply the hyphen's and justification's rule
	*/
	void IDCall_PC(ITCLParser *  parser);
};


/** Func_PK
	Implement the PK command
*/
class Func_PK
{
public :

	/**
		Constructor
		Read PK command parameters and call IDCall_PK
		@param IN parser : TCL parser
	*/
	Func_PK(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PK();

private :

	bool8 keepWithNextLines;

	/**
		Apply the KeepWithNextLine attribute
	*/
	void IDCall_PK(ITCLParser *  parser);
};


/** Func_PL
	Implement the PL command
*/
class Func_PL
{
public :

	/**
		Constructor
		Read PL command parameters and call IDCall_PL
		@param IN parser : TCL parser
	*/
	Func_PL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PL();

private :

	int16 numberOfChars, numberOfLines;

	/**
		Apply the KeepWithNextLine attribute
	*/
	void IDCall_PL(ITCLParser *  parser);
};


/** Func_CJ
	Implement the CJ command
*/
class Func_CJ
{
public :

	/**
		Constructor
		Read CJ command parameters and call IDCall_CJ
		@param IN parser : TCL parser
	*/
	Func_CJ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CJ();

private :

	PMString name ;
	bool8 hyphenMode, capitalized, lastWord ;
	int16 minWord, afterFirst, beforeLast, limit ;
	PMReal hyphenZone ;
	PMReal wordMin, wordMax, wordDes, letterMin, letterMax, letterDes ;


	/**
		Add an hyphen's and justification's rule
	*/
	void IDCall_CJ(ITCLParser *  parser);
};


/** Func_PM
	Implement the PM command
*/
class Func_PM
{
public :

	/**
		Constructor
		Read PM command parameters and call IDCall_PM
		@param IN parser : TCL parser
	*/
	Func_PM(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_PM();

private :

	int16 numTab;
	TabStop tab;
	
	/**
		Modify a tabulation
	*/
	void IDCall_PM(ITCLParser *  parser);
};

#endif //__Paragraphes_h__