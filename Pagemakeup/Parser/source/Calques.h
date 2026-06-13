/*
//	File:	Calques.h
//
//	Date:	02-08-2005
//
//  Author : Trias
//
*/


#ifndef __Calques_h__
#define __Calques_h__

#include "PMUTypes.h"

class ITCLParser;


/** Func_LS
	Implement the LS command
*/
class Func_LS
{
public :

	/**
		Constructor
		Read LS command parameters and call IDCall_LS
		@param IN parser : TCL parser
	*/
	Func_LS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_LS();

private :
		
	PMString layerName;

	/**
		Set active layer
	*/
	void IDCall_LS(ITCLParser *  parser);
};

/** Func_LN
	Implement the LN command
*/
class Func_LN
{
public :

	/**
		Constructor
		Read LN command parameters and call IDCall_LN
		@param IN parser : TCL parser
	*/
	Func_LN(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_LN();

private :
		
	PMString layerName;

	/**
		Create a new layer
	*/
	void IDCall_LN(ITCLParser *  parser);
};

/** Func_LH
	Implement the LH command
*/
class Func_LH
{
public :

	/**
		Constructor
		Read LH command parameters and call IDCall_LH
		@param IN parser : TCL parser
	*/
	Func_LH(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_LH() {}

private :
		
	PMString layerName;
	bool16 hide, dontQuitIfNotExist;

	/**
		Hide/Show a layer
	*/
	void IDCall_LH(ITCLParser *  parser);
};

/** Func_LB
	Implement the LB command
*/
class Func_LB
{
public :

	/**
		Constructor
		Read LB command parameters and call IDCall_LB
		@param IN parser : TCL parser
	*/
	Func_LB(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_LB() {}

private :
		
	TCLRef refItem;
	PMString layerName;

	/**
		Move an item to a new layer
	*/
	void IDCall_LB(ITCLParser *  parser);
};

#endif // __Calques_h__