/*
//	File:	Log.h
//
//	Date:	9-Nov-2005
//
//  Author : Trias
//
*/


#ifndef __Log_h__
#define __Log_h__

class ITCLParser;
class ITCLReader;

class IPMStream;

/** Func_UL
	Implement the UL command
*/
class Func_UL
{
public :

	/**
		Constructor
		Read UL command parameters and call IDCall_UL
		@param IN parser : TCL parser
	*/
	Func_UL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_UL();

	IPMStream * QueryLogFileStream();

private :
	
	PMString fileName;
	IPMStream * logfileStream;

	/**
		Create and open a log file
	*/
	void IDCall_UL(ITCLParser *  parser);
};

#endif // __Log_h__
