/*
//	File:	PermRefs.h
//
//	Date:	13-Sep-2005
//
//  Author : Trias
//
*/


#ifndef __PermRefs_h__
#define __PermRefs_h__

class ITCLParser;
class ITCLReader;

/** Func_HD
	Implement the HD command
*/
class Func_HD
{
public :

	/**
		Constructor
		Read HD command parameters and call IDCall_HD
		@param IN parser : TCL parser
	*/
	Func_HD(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_HD();

private :
	
	uint32 ref;

	uint32 param1, param2, param3, param4;

	/**
		
	*/
	void IDCall_HD(ITCLParser *  parser);
};

/** Func_HF
	Implement the HF command
*/
class Func_HF
{
public :

	/**
		Constructor
		Read HF command parameters and call IDCall_HF
		@param IN parser : TCL parser
	*/
	Func_HF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_HF();

private :
		
	/**
		
	*/
	void IDCall_HF(ITCLParser *  parser);
};


/** Func_HS
	Implement the HS command
*/
class Func_HS
{
public :

	/**
		Constructor
		Read HS command parameters and call IDCall_HS
		@param IN parser : TCL parser
	*/
	Func_HS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_HS();

private :
	
	uint32 ref;

	uint32 param1, param2, param3, param4;

	boost::shared_ptr< WideString > txtBuf;

	bool16 deleteNextSpaceOrCRIfEmpty;

	/**
		
	*/
	void IDCall_HS(ITCLParser *  parser);
};

/** Func_HR
	Implement the HR command
*/
class Func_HR
{
public :

	/**
		Constructor
		Read HR command parameters 
		@param IN parser : TCL parser
	*/
	Func_HR(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_HR();

private :
			
};

/** Func_HC
	Implement the HC command
*/
class Func_HC
{
public :

	/**
		Constructor
		Read HC command parameters 
		@param IN parser : TCL parser
	*/
	Func_HC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_HC() {}

private :
	
	uint32 param1, param2, param3, param4;
	int8 separatorType;
	int32 nbSeparator, nbCopy;

	/**
		Copy contiguous text which have the same sets of permrefs just after itself
		Command dedicated to GCube
	*/
	void IDCall_HC(ITCLParser *  parser);

};

/** Func_HH
	Implement the HH command
*/
class Func_HH
{
public :

	/**
		Constructor
		Read HH command parameters 
		@param IN parser : TCL parser
	*/
	Func_HH(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_HH() {}

private :
	
	uint32 ref;
	PMString serverAddress;

	/**
		Compute height of text tagged with ref and return result to Traffic Presse
		Command dedicated to Traffic Presse
	*/
	void IDCall_HH(ITCLParser *  parser);

};

#endif // __PermRefs_h__