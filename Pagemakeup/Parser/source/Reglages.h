/*
//	File:	Textes.h
//
//	Date:	24-Sep-2003
//
//  Author : Nicolas Metaye
//
*/


#ifndef __Reglages_h__
#define __Reglages_h__

#include "ITCLParser.h"

class ITCLParser;


/** Func_DC
	Implement the DC command
*/
class Func_DC
{
public :

	/**
		Constructor
		Read DC command parameters and call IDCall_DC
		@param IN parser : TCL parser
	*/
	Func_DC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DC();

private :
		
	Vector theColors ;


	/**
		Set the colors in the colors' table
	*/
	void IDCall_DC(ITCLParser *  parser);
};


/** Func_CN
	Implement the CN command
*/
class Func_CN
{
public :

	/**
		Constructor
		Read CN command parameters and call IDCall_CN
		@param IN parser : TCL parser
	*/
	Func_CN(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CN();

private :
		
	int32 refColor;
	PMString swtchName;
	bool8 isSpot, isHidden;
	PMReal cyan, magenta, yellow, black;
	PMReal red, green, blue;
	int32 classid;


	/**
		Create a new color
	*/
	void IDCall_CN(ITCLParser *  parser);
};


/** Func_DP
	Implement the DP command
*/
class Func_DP
{
public :

	/**
		Constructor
		Read DP command parameters and call IDCall_DP
		@param IN parser : TCL parser
	*/
	Func_DP(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DP();

private :
		
	Vector theFonts;


	/**
		Set the fonts in the fonts' table
	*/
	void IDCall_DP(ITCLParser *  parser);
};

/** Func_DG
	Implement the DG command
*/
class Func_DG
{
public :

	/**
		Constructor
		Read DG command parameters and call IDCall_DG
		@param IN parser : TCL parser
	*/
	Func_DG(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DG();

private :
		
	bool8 autoKern, autoLead, useLigatures;
	/**
		Set general parameters
	*/
	void IDCall_DG(ITCLParser *  parser);
};

/** Func_DJ
	Implement the DJ command (do nothing, only for quark compatibility)
*/
class Func_DJ
{
public :

	/**
		Constructor
		Read DJ command parameters 
		@param IN parser : TCL parser
	*/
	Func_DJ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DJ() {};
};

/** Func_GN
	Implement the GN command
*/
class Func_GN
{
public :

	/**
		Constructor
		Read GN command parameters and call IDCall_GN
		@param IN parser : TCL parser
	*/
	Func_GN(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_GN();

private :
		
	int32 refGradient;
	PMString swtchName;
	int32 nbStop;
	int8 type;

	K2Vector<PMReal> stopPositions;
	K2Vector<PMReal> midPoints;
	K2Vector<int32> refColors;

	/**
		Create a new gradient
	*/
	void IDCall_GN(ITCLParser *  parser);
};

/** Func_CDeux
	Implement the C2 command
*/
class Func_CDeux
{
public :

	/**
		Constructor
		Read CN command parameters and call IDCall_CDeux
		@param IN parser : TCL parser
	*/
	Func_CDeux(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CDeux() {}

private :
		
	int32 refTint, baseColor;
	PMReal tint;


	/**
		Create a new tinted color
	*/
	void IDCall_CDeux(ITCLParser * parser);
};

#endif//__Reglages_h__