/*
//	File:	Systeme.h
//
//	Date:	30-Nov-2005
//
//  Author : Trias
//
*/


#ifndef __Systeme_h__
#define __Systeme_h__

class ITCLParser;
class ITCLReader;

/** Func_DR
	Implement the DRU command
*/
class Func_DR
{
public :

	/**
		Constructor
		Read DR command parameters and call IDCall_DR
		@param IN parser : TCL parser
	*/
	Func_DR(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DR() {};

private :
		
	PMString newName;

	/**
		Rename the TCL file
	*/
	void IDCall_DR(ITCLParser * parser);
};

#endif //__Systeme_h__