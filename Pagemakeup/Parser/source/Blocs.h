/*
//	File:	Blocs.h
//
//	Date:	23-Apr-2003
//
//  Author : Trias
//
*/


#ifndef __Blocs_h__
#define __Blocs_h__

class ITCLParser;

#include "PMUTypes.h"

#include "PMPathPoint.h"

/** Func_BN
	Implement the BN command
*/
class Func_BN
{
public :

	/**
		Constructor
		Read DN command parameters and call IDCall_DN
		@param IN parser : TCL parser
	*/
	Func_BN(ITCLParser *  parser);

	Func_BN() {} ; // Default constructor, do nothing

	/**
		Destructor
	*/
	~Func_BN();

protected :

	ErrorCode ApplyTextFrameAttr(ITCLParser * parser, UIDRef& imFrameRef, PMString& error);

	PMReal hPos, vPos, // x and y position of the left top corner
		   largeur, hauteur; // Width and height of the frame
	PMReal gout, retrait,vJustMax, dec; // Gutter width, inset, 
	int8 alignVert, topAlign, // Vertical justification, first line offset metric
		 alignBloc; // Frame alignment for relative frame creation
	bool8 chainage, ancrage, relative; // Linking or not , make an inline or not, relative frame creation or not
	TCLRef refBloc, blocChaine, blocAncrage, ref; // TCL reference, text frame to link with, text frame receiving the inline, relative item TCL reference
	int32 nbCol, angle, page, // number of columns, angle of rotation,  page number
		  coulFond,trameFond; // Color and tint

	// Coulage auto
	bool8 coulageAuto;
	PMReal offset;

private :
	/**
		Create a new text frame according to the parameters read
		@param IN parser : TCL parser
	*/
	void IDCall_BN(ITCLParser *  parser);
};

/** Func_BI
	Implement the BI command
*/
class Func_BI
{
public :

	/**
		Constructor
		Read BI command parameters and call IDCall_BI
		@param IN parser : TCL parser
	*/
	Func_BI(ITCLParser *  parser);

	Func_BI() {}; // Default constructor, do nothing

	/**
		Destructor
	*/
	~Func_BI();

protected :

	ErrorCode ApplyImageFrameAttr(ITCLParser * parser, UIDRef& imFrameRef, PMString& error);
	
	PMString nomFichier; // File name of the picture to import
	PMReal hPos, vPos, // x and y position of the left top corner
		   largeur, hauteur, // Width and height of the frame
		   decH, decV; // Horizontal and vertical gap of the image inside the frame
	PMReal echH, echV, taille; // x scale, y scale, corner width (if not a normal corner)
	int8 align, // Frame alignment for relative frame creation
		  type; // Corner type
	bool8 ancrage, relative, cellAncrage; // make an inline or not, relative frame creation or not, maken an inline in a table cell or not
	TCLRef refBloc, blocAncrage, ref; // TCL reference, text frame receiving the inline, relative item TCL reference
	int32 angle, page, // angle of rotation,  page number							
		  coulFond,trameFond; // Color, tint
	
	// Coulage auto
	bool8 coulageAuto;
	PMReal offset;

private :
	/**
		Create a new image frame according to the parameters read
		@param IN parser : TCL parser
	*/
	void IDCall_BI(ITCLParser *  parser);
};

/** Func_BC
	Implement the BC command
*/
class Func_BC
{
public :

	/**
		Constructor
		Read BC command parameters and call IDCall_BC
		@param IN parser : TCL parser
	*/
	Func_BC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BC();

private :

	TCLRef refBloc; // TCL reference
	PMReal graisse, taille, trameGap,trameFilet; // Stroke weight, corner width (if not a normal corner)
	int8 type, forme, strokeAlignment; // Stroke type, corner type and stroke alignment
	int32 coulFilet, coulGap; // Stroke color and tint

	/** 
		Set stroke parameters of a frame 
		@param IN parser : TCL parser
	*/
	void IDCall_BC(ITCLParser *  parser);
};

/** Func_BH
	Implement the BH command
*/
class Func_BH
{
public :

	/**
		Constructor
		Read BH command parameters and call IDCall_BH
		@param IN parser : TCL parser
	*/
	Func_BH(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BH();

private :

	TCLRef refBloc; // TCL reference
	PMReal haut, bas, gauche, droit; // Standoff margins (top, bottom, left, right)
	int8 type; // Standoff type
	bool8 invert; // Invert standoff type

	/** 
		Set text wrap parameters of a frame
		@param IN parser : TCL parser
	*/
	void IDCall_BH(ITCLParser *  parser);
};

/** Func_BJ
	Implement the BJ command
*/
class Func_BJ
{
public :

	/**
		Constructor
		Read BJ command parameters and call IDCall_BJ
		@param IN parser : TCL parser
	*/
	Func_BJ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BJ();

private :

	TCLRef refBloc; // TCL reference 
	PMReal haut, bas, gauche, droit; // Insets (top, bottom, left, right)

	/** 
		Set text insets of a frame
		@param IN parser : TCL parser
	*/
	void IDCall_BJ(ITCLParser *  parser);
};

/** Func_FH
	Implement the FH command
*/
class Func_FH
{
public :

	/**
		Constructor
		Read FH command parameters and call IDCall_FH
		@param IN parser : TCL parser
	*/
	Func_FH(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_FH();

private :

	TCLRef refBloc, ref; // TCL reference of the rule, TCL reference of the item for relative mode
	int32 coulFilet,trameFilet, page; // Stroke color and tint, page number for absolute mode
	PMReal hPos, vPos, // x and y position of the left side
		largeur, graisse; // Width and stroke weight
	int8 typeFilet, // Stroke type
		 typeFleche, align, strokeAlign; // Arrow type, rule alignment for relative rule creation
	bool8 relative; // relative rule creation or not

	// Coulage auto
	bool8 coulageAuto;
	PMReal offset;

	/**
		Create an horizontal rule according to the parameters read
		@param IN parser : TCL parser
	*/
	void IDCall_FH(ITCLParser *  parser);
};

/** Func_FV
	Implement the FV command
*/
class Func_FV
{
public :

	/**
		Constructor
		Read FV command parameters and call IDCall_FV
		@param IN parser : TCL parser
	*/
	Func_FV(ITCLParser *  parser);

	/*
		Destructor
	*/
	~Func_FV();

private :

	TCLRef refBloc, ref; // TCL reference of the rule, TCL reference of the item for relative mode
	int32 coulFilet,trameFilet, page; // Stroke color and tint, page number for absolute mode
	PMReal hPos, vPos, // x and y position of the left side
		   hauteur, graisse; // Height and stroke weight
	int8 typeFilet, // Stroke type
		 typeFleche, align, strokeAlign; // Arrow type, rule alignment for relative rule creation
	bool8 relative; // relative rule creation or not

	/**
		Create a vertical rule according to the parameters read
		@param IN parser : TCL parser
	*/
	void IDCall_FV(ITCLParser *  parser);
};

/** Func_BS
	Implement the BS command

*/
class Func_BS
{
public :

	/**
		Constructor
		Read BS command parameters and call IDCall_BS
		@param IN parser : TCL parser
	*/
	Func_BS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BS();

private :

	TCLRef refBloc ;// TCL reference
	bool16 selectTOP; // TRUE to write text to frame's TOP

	/**
		Select a frame read
		@param IN parser : TCL parser
	*/
	void IDCall_BS(ITCLParser *  parser);
};

/** Func_BG
	Implement the BG command

*/
class Func_BG
{
public :

	/**
		Constructor
		Read BG command parameters and call IDCall_BG
		@param IN parser : TCL parser
	*/
	Func_BG(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BG();

	static UIDList GetAndCheckItemsToGroup(ITCLParser * parser, bool16 groupage, IDataBase * db, const K2Vector<TCLRef>& refList, bool16& errorOccured, PMString& error);

private :

	bool8 groupage ; 
	K2Vector<TCLRef> refBlocList ;
	/**
		Group frames
		@param IN parser : TCL parser
	*/
	void IDCall_BG(ITCLParser *  parser);
};


/** Func_BV
	Implement the BV command

*/
class Func_BV
{
public :

	/**
		Constructor
		Read BV command parameters and call IDCall_BV
		@param IN parser : TCL parser
	*/
	Func_BV(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BV();

private :

	bool8 verrouillage ;

	K2Vector<TCLRef> listRef;

	/**
		lock frame
		@param IN parser : TCL parser
	*/
	void IDCall_BV(ITCLParser *  parser);
};


/** Func_BD
	Implement the BD command

*/
class Func_BD
{
public :

	/**
		Constructor
		Read BD command parameters and call IDCall_BD
		@param IN parser : TCL parser
	*/
	Func_BD(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BD();

private :

	K2Vector<TCLRef> refBlocList ;

	/**
		delete frame(s)
		@param IN parser : TCL parser
	*/
	void IDCall_BD(ITCLParser *  parser);
};




/** Func_B1
	Implement the B1 command
*/
class Func_B1
{
public :

	/**
		Constructor
		Read B1 command parameters and call IDCall_B1
		@param IN parser : TCL parser
	*/
	Func_B1(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_B1();

private :

	TCLRef refBloc ; //ref of the item 

	/**
		Bring the item to the front
		@param IN parser : TCL parser
	*/
	void IDCall_B1(ITCLParser *  parser);
};


/** Func_B2
	Implement the B2 command
*/
class Func_B2
{
public :

	/**
		Constructor
		Read B2 command parameters and call IDCall_B2
		@param IN parser : TCL parser
	*/
	Func_B2(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_B2();

private :

	TCLRef refBloc ; //ref of the item 

	/**
		Send the item to the back
		@param IN parser : TCL parser
	*/
	void IDCall_B2(ITCLParser *  parser) ;
};


/** Func_BE
	Implement the BE command
*/
class Func_BE
{
public :

	/**
		Constructor
		Read BE command parameters and call IDCall_BE
		@param IN parser : TCL parser
	*/
	Func_BE(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BE();

private :

	TCLRef refBloc ; //ref of the item 
	PMReal angle ; //skew angle for the item

	/**
		Skew the item
		@param IN parser : TCL parser
	*/
	void IDCall_BE(ITCLParser *  parser) ;
};


/** Func_BP
	Implement the BP command
*/
class Func_BP
{
public :

	/**
		Constructor
		Read BP command parameters and call IDCall_BP
		@param IN parser : TCL parser
	*/
	Func_BP(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BP();

private :

	TCLRef refBloc ; //ref of the item 
	PMReal skewAngle ; //skew angle for the item
	PMReal rotationAngle ; //skew rotation for the item
	PMReal horOffset, verOffset;

	/**
		Skew an rotate the item
		@param IN parser : TCL parser
	*/
	void IDCall_BP(ITCLParser *  parser) ;
};


/** Func_FD
	Implement the FD command
*/
class Func_FD
{
public :

	/**
		Constructor
		Read FD command parameters and call IDCall_FD
		@param IN parser : TCL parser
	*/
	Func_FD(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_FD();

private :

	TCLRef refBloc, ref;// TCL reference of the rule, TCL reference of the item for relative mode
	int32 coulFilet,trameFilet, page; // Stroke color and tint, page number for absolute mode
	PMReal hPos1, vPos1, hPos2, vPos2, // x and y position of the left and right sides
		graisse; //stroke weight
	int8 typeFilet, // Stroke type
		 typeFleche, align, strokeAlign; // Arrow type, rule alignment for relative rule creation
	bool8 relative; // relative rule creation or not

	/**
		Create an diagonal rule according to the parameters read
		@param IN parser : TCL parser
	*/
	void IDCall_FD(ITCLParser *  parser);
};


/** Func_BPLUS
	Implement the B+ command
*/
class Func_BPLUS
{
public :

	/**
		Constructor
		Read BPLUS command parameters and call IDCall_BPLUS
		@param IN parser : TCL parser
	*/
	Func_BPLUS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BPLUS();

private :

	/**
		select the next bloc
		@param IN parser : TCL parser
	*/
	void IDCall_BPLUS(ITCLParser *  parser);
};

/** Func_BMOINS
	Implement the B- command
*/
class Func_BMOINS
{
public :

	/**
		Constructor
		Read BMOINS command parameters and call IDCall_BMOINS
		@param IN parser : TCL parser
	*/
	Func_BMOINS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BMOINS();

private :

	/**
		select the next bloc
		@param IN parser : TCL parser
	*/
	void IDCall_BMOINS(ITCLParser *  parser);
};



/** Func_BF
	Implement the BF command
*/
class Func_BF
{
public :

	/**
		Constructor
		Read BF command parameters and call IDCall_BF
		@param IN parser : TCL parser
	*/
	Func_BF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BF();

private :

	TCLRef refBloc ;
	bool8 center, scale;

	/**
		place the item in the frame (at center or with scale)
	*/
	void IDCall_BF(ITCLParser *  parser);
};


/** Func_BM
	Implement the BM command
*/
class Func_BM
{
public :

	/**
		Constructor
		Read BM command parameters and call IDCall_BM
		@param IN parser : TCL parser
	*/
	Func_BM(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BM();

private :

	PMReal hPos, vPos, // x and y position of the left top corner
		   largeur, hauteur; // Width and height of the frame
	PMReal gout, retrait,vJustMax, dec; // Gutter width, inset, 
	int8 alignVert, // Vertical justification
		 alignBloc; // Frame alignment for relative frame creation
	bool8 relative; // relative frame creation or not
	TCLRef refBloc,ref; // TCL reference, item TCL reference for relative mode
	int32 nbCol, angle, coulFond,trameFond, page ; // number of columns, rotation angle, Color and tint, page number for absolute mode

	/**
		modify the text frame according to the parameters read
		@param IN parser : TCL parser
	*/
	void IDCall_BM(ITCLParser *  parser);
};

/** Func_AV
	Implement the AV command
*/
class Func_AV
{
public :

	/**
		Constructor
		Read AV command parameters and call IDCall_AV
		@param IN parser : TCL parser
	*/
	Func_AV(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AV();

private :

	TCLRef refBloc, ref;
	int8 alignBloc;
	PMReal offset;

	/**
		Resize item according to an other item's position
	*/
	void IDCall_AV(ITCLParser *  parser);
};

/** Func_BA
	Implement the BA command
*/
class Func_BA
{
public :
	
	
	/**
		Constructor
		Read BA command parameters and call IDCall_BA
		@param IN parser : TCL parser
	*/
	Func_BA(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BA();

private :

	TCLRef refBloc;
	bool8 supprImpr;
	
	/**
		Set an item as printable or not
	*/
	void IDCall_BA(ITCLParser *  parser);
};

/** Func_BU
	Implement the BU command
*/
class Func_BU
{
public :
	
	
	/**
		Constructor
		Read BA command parameters and call IDCall_BA
		@param IN parser : TCL parser
	*/
	Func_BU(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BU();

private :

	TCLRef refBloc;
	bool8 flipH, flipV;
	
	/**
		Define skew angle of text inside a text frame
	*/
	void IDCall_BU(ITCLParser *  parser);
};

/** Func_BQ
	Implement the BQ command
*/
class Func_BQ
{
public :
	
	
	/**
		Constructor
		Read BQ command parameters and call IDCall_BQ
		@param IN parser : TCL parser
	*/
	Func_BQ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BQ();

private :

	TCLRef refBloc;
	bool8 positionType;
	PMReal yOffset;
	
	/**
		Define properties of an inline frame
	*/
	void IDCall_BQ(ITCLParser *  parser);
};

/** Func_FF
	Implement the FF command
*/
class Func_FF
{
public :
	
	
	/**
		Constructor
		Read BQ command parameters and call IDCall_FF
		@param IN parser : TCL parser
	*/
	Func_FF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_FF();

private :
	
	/**
		Select previous selected frame
	*/
	void IDCall_FF(ITCLParser *  parser);
};

/** Func_BO
	Implement the BO command (do nothing, only for Quark compatibility)
*/
class Func_BO
{
public :
	
	
	/**
		Constructor
		Read BO command parameters
		@param IN parser : TCL parser
	*/
	Func_BO(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BO() {}

private :

	/**
		Set color and tint on a tiff gray scale image
	*/
	void IDCall_BO(ITCLParser *  parser);

	TCLRef refBloc;

	int32 coulFond; // Color
	PMReal trameFond; // tint
};
	
/** Func_BR
	Implement the BR command 
*/
class Func_BR
{
public :
	
	
	/**
		Constructor
		Read BR command parameters
		@param IN parser : TCL parser
	*/
	Func_BR(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BR() {};

private :

	TCLRef refBloc;
	
	/*
		Write the item's dimensions into the log file, if any
	*/
	void IDCall_BR(ITCLParser *  parser);

};

/** Func_BZ
	Implement the Func_BZ command 
*/
class Func_BZ
{
public :
	
	
	/**
		Constructor
		Read Func_BZ command parameters
		@param IN parser : TCL parser
	*/
	Func_BZ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BZ() {};

private :

	TCLRef refBloc;
	int16 type, index;
	PMReal inset, tolerance;
	bool16 invert, allowHoles, limitBloc, useHighRes;
	uint8 threshold;

	/*
		Set clipping path around picture
	*/
	void IDCall_BZ(ITCLParser *  parser);

};

/** Func_BExcl
	Implement the Func_BExcl command 
*/
class Func_BExcl : public Func_BI
{
public :
	
	
	/**
		Constructor
		Read Func_BExcl command parameters
		@param IN parser : TCL parser
	*/
	Func_BExcl(ITCLParser *  parser);

	Func_BExcl() {}; // Default constructor, do nothing

	/**
		Destructor
	*/
	~Func_BExcl() {};

protected :
	
	K2Vector<PMPathPointList> ptsList;
	K2Vector<bool16> isClosedList;

	/*
		Create a graphic frame specifying path points
	*/
	void IDCall_BPExcl(ITCLParser *  parser);

};

/** Func_BStar
	Implement the Func_BStar command (OBSOLETE -> it exists only for backward compatibility, replaced by B*)
*/
class Func_BStar : public Func_BN
{
public :
	
	
	/**
		Constructor
		Read Func_BStar command parameters
		@param IN parser : TCL parser
	*/
	Func_BStar(ITCLParser *  parser);

	Func_BStar() {}; // Default constructor, do nothing

	/**
		Destructor
	*/
	~Func_BStar() {};

protected :
	
	K2Vector<PMPathPointList> ptsList;
	K2Vector<bool16> isClosedList;

	/*
		Create a text frame specifying path points
	*/
	void IDCall_BPStar(ITCLParser *  parser);

};

/** Func_BDollar
	Implement the B$ command (OBSOLETE -> it exists only for backward compatibility, replaced by B!)
*/
class Func_BDollar : public Func_BExcl
{
public :
	
	
	/**
		Constructor
		Read B$ command parameters
		@param IN parser : TCL parser
	*/
	Func_BDollar(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BDollar() {};
};

/** Func_BPCent
	Implement the Func_BPCent command 
*/
class Func_BPCent : public Func_BStar
{
public :
	
	
	/**
		Constructor
		Read Func_BPCent command parameters
		@param IN parser : TCL parser
	*/
	Func_BPCent(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BPCent() {};

};

/** Func_BL
	Implement the Func_BL command 
*/
class Func_BL
{
public :
	
	
	/**
		Constructor
		Read Func_BL command parameters
		@param IN parser : TCL parser
	*/
	Func_BL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BL() {};

private : 

	/**
		Reimport an image in a frame
	*/
	void IDCall_BL(ITCLParser *  parser);

	TCLRef refBloc;
	PMString nomFichier;
	PMReal hPos, vPos, // x and y position of the left top corner
		   largeur, hauteur, // Width and height of the frame
		   decH, decV, // Horizontal and vertical gap of the image inside the frame
		   echH, echV; // x scale, y scale, corner width (if not a normal corner)

	int32 coulFond; // Color
	PMReal trameFond; // tint

};

/** Func_IT
Implement the Func_IT command
*/
class Func_IT
{
    public :
	
	
	/**
     Constructor
     Read Func_IT command parameters
     @param IN parser : TCL parser
     */
	Func_IT(ITCLParser *  parser);
    
	/**
     Destructor
     */
	~Func_IT() {};
    
    private :
    
	/**
     import a tagged text in a frame
     */
	void IDCall_IT(ITCLParser *  parser);
    
	TCLRef refBloc;
	PMString nomFichier;
    
};

#endif // __Blocs_h__