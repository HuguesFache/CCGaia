/*
//	File:	XRefs.h
//
//	Date:	12-Jan-2007
//
//  Author : Trias
//
*/


#ifndef __XRefs_h__
#define __XRefs_h__

#include "GlobalDefs.h"

#if XREFS

class ITCLParser;

/** Func_X1
	Implement the X1 command
*/
class Func_X1
{
public :

	/**
		Constructor
		Read X1 command parameters and call IDCall_X1
		@param IN parser : TCL parser
	*/
	Func_X1(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_X1() {}

private :

	PMString markerName;
	
	/**
		Insert a XRef source marker in text flow
	*/
	void IDCall_X1(ITCLParser *  parser);
};

/** Func_X2
	Implement the X2 command
*/
class Func_X2
{
public :

	/**
		Constructor
		Read X2 command parameters and call IDCall_X2
		@param IN parser : TCL parser
	*/
	Func_X2(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_X2() {}

private :

	PMString fileName;
	PMString markerName;
	bool16 useFormatName;
	PMString format;
	
	/**
		Insert a CrossRef into text model flow
	*/
	void IDCall_X2(ITCLParser *  parser);
};


/** Func_X3
	Implement the X3 command
*/
class Func_X3
{
public :

	/**
		Constructor
		Read X3 command parameters and call IDCall_X3
		@param IN parser : TCL parser
	*/
	Func_X3(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_X3() {}

private :
	
	/**
		Update all CrossRefs in current document
	*/
	void IDCall_X3(ITCLParser *  parser);
};

#endif // XREFS

#endif // __XRefs_h__