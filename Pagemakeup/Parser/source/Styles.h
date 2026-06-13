/*
//	File:	Styles.h
//
//	Date:	24-Sep-2003
//
//  Author : Nicolas Metaye
//
*/


#ifndef __Styles_h__
#define __Styles_h__

class ITCLParser;


/** Func_SN
	Implement the SN command
*/
class Func_SN
{
public :

	/**
		Constructor
		Read SN command parameters and call IDCall_SN
		@param IN parser : TCL parser
	*/
	Func_SN(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SN();

private :
		
	PMString paraStyleName; // paragraph style's name

	/**
		Set the UID of the paragraph style in the ITextFrameHelper
	*/
	void IDCall_SN(ITCLParser *  parser);
};


/** Func_SB
	Implement the SB command
*/
class Func_SB
{
public :

	/**
		Constructor
		Read SB command parameters and call IDCall_SB
		@param IN parser : TCL parser
	*/
	Func_SB(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SB();

private :
		
	PMString charStyleName; // character style's name

	/**
		Set the attributeBossList of the character style in the ITextFrameHelper
	*/
	void IDCall_SB(ITCLParser *  parser);
};

/** Func_SD
	Implement the SD command
*/
class Func_SD
{
public :

	/**
		Constructor
		Read SD command parameters and call IDCall_SD
		@param IN parser : TCL parser
	*/
	Func_SD(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SD();

private :
	
	int32 styleNum, nextStyle, basedOn;
	PMString styleName; // style's name
	bool8 isParaStyle;

	/**
		stores the style's name in the ITextFrameHelper
	*/
	void IDCall_SD(ITCLParser *  parser);
};

/** Func_SF
	Implement the SF command
*/
class Func_SF
{
public :

	/**
		Constructor
		Read SF command parameters and call IDCall_SF
		@param IN parser : TCL parser
	*/
	Func_SF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SF();

private :
	

	/**
		creates style with informations stored previously in the ITextFrameHelper
	*/
	void IDCall_SF(ITCLParser *  parser);
};


/** Func_SY
	Implement the SY command
*/
class Func_SY
{
public :

	/**
		Constructor
		Read SY command parameters and call IDCall_SY
		@param IN parser : TCL parser
	*/
	Func_SY(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SY();

private :
		
	int32 paraStyleNum; // paragraph style's number in style table

	/**
		Set the UID of the paragraph style in the ITextFrameHelper
	*/
	void IDCall_SY(ITCLParser *  parser);
};

/** Func_SS
	Implement the SS command
*/
class Func_SS
{
public :

	/**
		Constructor
		Read SY command parameters and call IDCall_SS
		@param IN parser : TCL parser
	*/
	Func_SS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SS();

private :
		
	int32 charStyleNum; // paragraph style's number in style table

	/**
		Set the UID of the paragraph style in the ITextFrameHelper
	*/
	void IDCall_SS(ITCLParser *  parser);
};

#endif //__Styles_h__