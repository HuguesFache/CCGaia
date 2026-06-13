/*
//	File:	Hyperlinks.h
//
//	Date:	09-07-2007
//
//  Author : Trias
//
*/


#ifndef __Hyperlinks_h__
#define __Hyperlinks_h__

#include "PMUTypes.h"

class ITCLParser;


/** Func_SI
	Implement the SI command
*/
class Func_SI
{
public :

	/**
		Constructor
		Read SI command parameters and call IDCall_SI
		@param IN parser : TCL parser
	*/
	Func_SI(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SI() {}

private :
	
	int8 bkType;
	PMString bkName, bkParentName, hlDestName;

	/**
		Insert a new bookmark
	*/
	void IDCall_SI(ITCLParser *  parser);	
};

/** Func_H1
	Implement the H1 command
*/
class Func_H1
{
public :

	/**
		Constructor
		Read H1 command parameters and call IDCall_H1
		@param IN parser : TCL parser
	*/
	Func_H1(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_H1() {}

	static ErrorCode CheckHyperlinkDestNameUnicity(IDocument * doc, PMString& name, PMString& error);

private :
		
	PMString hlDestName;
	bool16 visible;

	/**
		Create a text destination hyperlink
	*/
	void IDCall_H1(ITCLParser *  parser);

};

/** Func_H2
	Implement the H2 command
*/
class Func_H2
{
public :

	/**
		Constructor
		Read H2 command parameters and call IDCall_H2
		@param IN parser : TCL parser
	*/
	Func_H2(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_H2() {}

private :
		
	PMString hlDestName;
	int16 pageNumber;
	bool16 visible;

	/**
		Create a page destination hyperlink
	*/
	void IDCall_H2(ITCLParser *  parser);

};

/** Func_H3
	Implement the H3 command
*/
class Func_H3
{
public :

	/**
		Constructor
		Read H3 command parameters and call IDCall_H3
		@param IN parser : TCL parser
	*/
	Func_H3(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_H3() {}

private :
		
	PMString hlDestName;
	PMString url;

	/**
		Create a URL destination hyperlink
	*/
	void IDCall_H3(ITCLParser *  parser);
};


/** Func_HK
	Implement the HK command
*/
class Func_HK
{
public :

	/**
		Constructor
		Read HK command parameters and call IDCall_HK
		@param IN parser : TCL parser
	*/
	Func_HK(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_HK() {}

private :
	
	int8 hlType;
	int32 borderColor;
	TextIndex endPos;
	
	static PMString hlDestName;	
	static bool16 visible;
	static PMString hlName;
	static TextIndex insertPos;
	static UIDRef txtModelRef;
	static UIDRef hlDestRef;
	static int32 borderWidth;
	static int32 hilightType;
	static UID borderColorUID;
	static int32 outlineStyle;

	friend class Func_HL; // Allow access to hlName, insertPos and txtModelRef to Func_HL

	/** Create a text hyperlink source item
	*/
	static UIDRef CreateTextSourceHyperlinkCmd(const PMString& name, ITextModel * txtModel, const RangeData& txtRange, const bool16& visible, PMString& error);

	/** Create an hyperlink from a source and a destination link
	*/
	static ErrorCode CreateHyperlinkCmd(const PMString& name, const UIDRef& source, const UIDRef& dest, PMString& error);

	/**
		Create a page item source hyperlink , or text source hyperlink if used in pair with HL
	*/
	void IDCall_HK(ITCLParser *  parser);

};

/** Func_HL
	Implement the HL command
*/
class Func_HL
{
public :

	/**
		Constructor
		Read HL command parameters and call IDCall_HL
		@param IN parser : TCL parser
	*/
	Func_HL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_HL() {}

private :

	/**
		End of a text source hyperlink
	*/
	void IDCall_HL(ITCLParser *  parser);

};

#endif // __Hyperlinks_h__