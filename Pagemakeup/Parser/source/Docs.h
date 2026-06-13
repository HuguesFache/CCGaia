/*
//	File:	Docs.h
//
//	Date:	24-Apr-2003
//
//  Author : Trias
//
*/


#ifndef __Docs_h__
#define __Docs_h__

class ITCLParser;

#include "PMUTypes.h"

class TCLError;

#include "K2Vector.h"
#include "K2Pair.h"

/** Func_DN
	Implement the DN command
*/
class Func_DN
{
public :

	/**
		Constructor
		Read DN command parameters and call IDCall_DN
		@param IN parser : TCL parser
	*/
	Func_DN(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DN();

private :
	
	PMString NomDoc; // Document name
	PMReal largeur,hauteur; // width and height
	int16 col,nbPage,langage; // Number of columns, of pages and default language
	PMReal lGout; // Gutter width
	PMReal mg,md,mh,mb; // Margins (left, right, top, bottom)
	bool8 recto,interAuto,approcheAuto; // Page per spread (true->2, false->1), auto leading, auto kerning
	PMReal interPercent; // Percentage for autoleading
	int16 pagesPerSpread;

	/**
		Create a new document according to the parameters read
		@param IN parser : TCL parser
	*/
	void IDCall_DN(ITCLParser *  parser);
};


/** Func_DF
	Implement the DF command
*/
class Func_DF
{
public :

	/**
		Constructor
		Read DF command parameters and call IDCall_DF
		@param IN parser : TCL parser
	*/
	Func_DF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DF();

private :

	bool8 docObligatoire, // Check document open or not
		  dechainer, // Unlink the selected text frame or not
		  blocRelative; // Define an item referenced as 0
	int16 numBloc; // Minimal number of items

	/** 
		Process the DF command
		@param IN parser : TCL parser
	*/
	void IDCall_DF(ITCLParser *  parser);
};


/** Func_DM
	Implement the DM command
*/
class Func_DM
{
public :

	/**
		Constructor
		Read DM command parameters and call IDCall_DM
		@param IN parser : TCL parser
	*/
	Func_DM(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DM();

private :
	
	PMReal largeur,hauteur; // width and height
	int16 col,nbPage,langage; // Number of columns, of pages and default language
	PMReal lGout; // Gutter width
	PMReal mg,md,mh,mb; // Margins (left, right, top, bottom)
	bool8 interAuto,approcheAuto; // Page per spread (true->2, false->1), auto leading, auto kerning
	PMReal interPercent; // Percentage for autoleading

	/**
		Create a new document according to the parameters read
		@param IN parser : TCL parser
	*/
	void IDCall_DM(ITCLParser *  parser);
};



/** Func_DS
	Implement the DS command
*/
class Func_DS
{
public :

	/**
		Constructor
		Read DS command parameters and call IDCall_DS
		@param IN parser : TCL parser
	*/
	Func_DS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DS();

private :
	
	PMString name;
	bool8 closeDoc ;

	/**
		save teh front document
		@param IN parser : TCL parser
	*/
	void IDCall_DS(ITCLParser *  parser);
};


/** Func_DE
	Implement the DE command
*/
class Func_DE
{
public :

	/**
		Constructor
		Read DE command parameters and call IDCall_DE
		@param IN parser : TCL parser
	*/
	Func_DE(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DE();

private :
	
	PMReal superScriptPos, superScriptSize;

	/**
		Define superscript preferences
		@param IN parser : TCL parser
	*/
	void IDCall_DE(ITCLParser *  parser);
};

/** Func_DH
	Implement the DH command
*/
class Func_DH
{
public :

	/**
		Constructor
		Read DH command parameters and call IDCall_DH
		@param IN parser : TCL parser
	*/
	Func_DH(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DH();

private :
	
	PMReal subScriptPos, subScriptSize;

	/**
		Define subscript preferences
		@param IN parser : TCL parser
	*/
	void IDCall_DH(ITCLParser *  parser);
};

/** Func_DK
	Implement the DK command
*/
class Func_DK
{
public :

	/**
		Constructor
		Read DK command parameters and call IDCall_DK
		@param IN parser : TCL parser
	*/
	Func_DK(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DK();

private :
	
	PMReal smallCapSize;

	/**
		Define superscript preferences
		@param IN parser : TCL parser
	*/
	void IDCall_DK(ITCLParser *  parser);
};

/** Func_DB
	Implement the DB command
*/
class Func_DB
{
public :

	/**
		Constructor
		Read DB command parameters and call IDCall_DB
		@param IN parser : TCL parser
	*/
	Func_DB(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DB();

private :

	/**
		Close the current document without saving
		@param IN parser : TCL parser
	*/
	void IDCall_DB(ITCLParser *  parser);
};

/** Func_DO
	Implement the DO command
*/
class Func_DO
{
public :

	/**
		Constructor
		Read DO command parameters and call IDCall_DO
		@param IN parser : TCL parser
	*/
	Func_DO(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DO();

private :

	PMString name;

	/**
		Open a document
		@param IN parser : TCL parser
	*/
	void IDCall_DO(ITCLParser *  parser);
	
	
};

/** Func_CP
	Implement the CP command
*/
class Func_CP
{
public :

	/**
		Constructor
		Read CP command parameters and call IDCall_CP
		@param IN parser : TCL parser
	*/
	Func_CP(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CP();

private :

	PMString filename;
	int16 numPage;
	PMReal scale;
	PMReal width, height;
	PMReal desiredRes;
	TCLRef refItem;
	int32 quality;
	int32 formatExport; // JPEG, TIFF, GIFF
	int32 encoding;  // 0 : kSSJPEGBaseline,  1: kSSJPEGProgressive

	/**
		Export page or text frame to a JPEG file
		@param IN parser : TCL parser
	*/
	void IDCall_CP(ITCLParser *  parser);
	
};


/** Func_CQ
 Implement the CQ command
 */
class Func_CQ
{

public :
		
	/**
	 Constructor
	 Read CQ command parameters and call IDCall_CQ
	 @param IN parser : TCL parser
	*/
	Func_CQ(ITCLParser *  parser);
		
	/**
	 Destructor
	*/
	~Func_CQ();
		
private :
		
	PMString filename;
	int16 numPage;
	PMReal scale;
	PMReal width, height;
	PMReal desiredRes;
	TCLRef refItem;
	int32 quality;
	int32 formatExport;		// JPEG, TIFF, GIFF
	int32 encoding;			// 0 : kSSJPEGBaseline,  1: kSSJPEGProgressive
	int32 colorSpace;            
	bool8 addTransparencyAlpha ; 		  
	bool8 simulateOverprint ;   
	
	/**
	 Export page or text frame to a JPEG file
	 @param IN parser : TCL parser
	*/
	void IDCall_CQ(ITCLParser *  parser);
};


/** Func_DEtoile
	Implement the D* command
*/
class Func_DEtoile
{
public :

	/**
		Constructor
		Read D* command parameters and call IDCall_DEtoile
		@param IN parser : TCL parser
	*/
	Func_DEtoile(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DEtoile() {}

private :

	PMReal width, height;

	/**
		Resize document with given with and heigth
		@param IN parser : TCL parser
	*/
	void IDCall_DEtoile(ITCLParser *  parser);
	
	K2Vector<K2Pair<PMPoint, UIDList> > ComputeOffset(IDataBase *, UID spreadUID, PMReal deltaX, PMReal deltaY, int32 startContentLayer, int32 endContentLayer);
};

#endif // __Docs_h__