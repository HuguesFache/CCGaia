/*
//	File:	XRail.h
//
//	Date:	19-Apr-2006
//
//  Author : Trias
//
*/


#ifndef __XRail_h__
#define __XRail_h__

class ITCLParser;

/** Func_MN
	Implement the MN command
*/
class Func_MN
{
public :

	/**
		Constructor
		Read MN command parameters and call IDCall_MN
		@param IN parser : TCL parser
	*/
	Func_MN(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_MN() {}

private :
	
	/**
		Write out some information about pages and masters (for XRail)
	*/
	void IDCall_MN(ITCLParser *  parser);
	
};

/** Func_XY
	Implement the XY command
*/
class Func_XY
{
public :

	/**
		Constructor
		Read XY command parameters and call IDCall_XY
		@param IN parser : TCL parser
	*/
	Func_XY(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_XY() {}

private :
	
	/**
		Stores page ID from XRail
	*/
	void IDCall_XY(ITCLParser *  parser);

	int16 numPage;
	int32 idPage;
	PMString nomBase;
};

/** Func_XZ
	Implement the XZ command
*/
class Func_XZ
{
public :

	/**
		Constructor
		Read XZ command parameters and call IDCall_XZ
		@param IN parser : TCL parser
	*/
	Func_XZ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_XZ() {}

private :
	
	/**
		Set date for page handled by XRail
	*/
	//void IDCall_XZ(ITCLParser *  parser);

};

#endif // __XRail_h__