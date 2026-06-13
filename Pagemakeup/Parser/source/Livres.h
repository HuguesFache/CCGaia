/*
//	File:	Livres.h
//
//	Date:	25-July-2006
//
//  Author : Trias
//
*/


#ifndef __Livres_h__
#define __Livres_h__

class ITCLParser;

#include "PMUTypes.h"

class TCLError;


/** Func_NL
	Implement the NL command
*/
class Func_NL
{
public :
/**
		Constructor
		Read NL command parameters and call IDCall_NL
		@param IN parser : TCL parser
	*/
	Func_NL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_NL() {}

private :

	PMString bookname;
	int8 paginateOption;
	bool16 insertBlank, autoPagination;

	/**
		Create a new empty book and set it as current book
		@param IN parser : TCL parser
	*/
	void IDCall_NL(ITCLParser *  parser);
	
};

/** Func_LO
	Implement the LO command
*/
class Func_LO
{
public :
/**
		Constructor
		Read NL command parameters and call IDCall_LO
		@param IN parser : TCL parser
	*/
	Func_LO(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_LO() {}

private :

	PMString bookname;

	/**
		Open a book
		@param IN parser : TCL parser
	*/
	void IDCall_LO(ITCLParser *  parser);
	
};

/** Func_FL
	Implement the FL command
*/
class Func_FL
{
public :
/**
		Constructor
		Read FL command parameters and call IDCall_FL
		@param IN parser : TCL parser
	*/
	Func_FL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_FL() {}

private :

	/**
		Close current book
		@param IN parser : TCL parser
	*/
	void IDCall_FL(ITCLParser *  parser);
	
};

/** Func_SL
	Implement the SL command
*/
class Func_SL
{
public :
/**
		Constructor
		Read FL command parameters and call IDCall_SL
		@param IN parser : TCL parser
	*/
	Func_SL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SL() {}

private :

	/**
		Save current book
		@param IN parser : TCL parser
	*/
	void IDCall_SL(ITCLParser *  parser);	
};


/** Func_LPlus
	Implement the L+ command
*/
class Func_LPlus
{
public :
/**
		Constructor
		Read L+ command parameters and call IDCall_LPlus
		@param IN parser : TCL parser
	*/
	Func_LPlus(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_LPlus() {}

private :

	int16 masterIndex, insertIndex;	
	K2Vector<IDFile> documents;

	/**
		Add the set of documents in parameter to the book and possibly set master document
		@param IN parser : TCL parser
	*/
	void IDCall_LPlus(ITCLParser *  parser);
	
};

/** Func_RL
	Implement the RL command
*/
class Func_RL
{
public :
/**
		Constructor
		Read RL command parameters and call IDCall_RL
		@param IN parser : TCL parser
	*/
	Func_RL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_RL() {}

private :

	int16 indexFrom;

	/**
		Repaginate book
		@param IN parser : TCL parser
	*/
	void IDCall_RL(ITCLParser *  parser);
	
};

/** Func_MT
	Implement the MT command
*/
class Func_MT
{
public :
/**
		Constructor
		Read MT command parameters and call IDCall_MT
		@param IN parser : TCL parser
	*/
	Func_MT(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_MT() {}

private :

	TCLRef refBloc;
	PMString tocStyleName;

	/**
		Make (or update) table of content
		@param IN parser : TCL parser
	*/
	void IDCall_MT(ITCLParser *  parser);
	
};

/** Func_ST
	Implement the ST command
*/
class Func_ST
{
public :
/**
		Constructor
		Read ST command parameters and call IDCall_ST
		@param IN parser : TCL parser
	*/
	Func_ST(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_ST() {}

private :

	PMString tocStyleName, tocTitle;
	int32 tocTitleStyle;
	bool16 includeHidLayer, runIn, includeBookMarks;

	K2Vector<KeyValuePair<int32,FormatEntryParam> > entries;

	/**
		Create a style of table of content
		@param IN parser : TCL parser
	*/
	void IDCall_ST(ITCLParser *  parser);
	
};

/** Func_IG
	Implement the IG command
*/
class Func_IG
{
public :
/**
		Constructor
		Read IG command parameters and call IDCall_IG
		@param IN parser : TCL parser
	*/
	Func_IG(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_IG() {}

	static PMString ParseEscapeChars(const PMString& s);

private :

	TCLRef refBloc;
	PMString title, topicSep, pageSep, entrySep, xrefSep, pageRangeSep, entryEndSep;
	
	int32 titleStyle, sectionStyle, pageNumStyle, xrefStyle, refCharStyle;
	bool16 includeHidLayer, runIn, includeSection, includeEmptySection;

	K2Vector<int32> topicStyles;

	/**
		Create a style of table of content
		@param IN parser : TCL parser
	*/
	void IDCall_IG(ITCLParser *  parser);
	
};

#endif // __Livres_h__