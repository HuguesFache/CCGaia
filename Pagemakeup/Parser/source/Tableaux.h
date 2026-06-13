/*
//	File:	Tableaux.h
//
//	Date:	03-Mar-2006
//
//  Author : Trias
//
*/

#ifndef __Tableaux_h__
#define __Tableaux_h__

#include "ITableLineType.h"

#include "PMUTypes.h"

class ITCLParser;
class ITableCommands;


/** Func_AT
	Implement the AT command
*/
class Func_AT
{
public :

	/**
		Constructor
		Read AT command parameters and call IDCall_AT
		@param IN parser : TCL parser
	*/
	Func_AT(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AT() {};

private :
		
	TCLRef refBloc; // reference of bloc in which the new table will be appended
	PMString tableName;

	int32 nbBodyRows, nbCol, nbHeaderRows, nbFooterRows;
	PMReal tableHeight, tableWidth;

	int32 repeatHeader, repeatFooter;
	bool16 skipFirst, skipLast;

	PMReal spaceBefore, spaceAfter;

	int32 strokeColor, gapColor, strokeType, drawingOrder;
	PMReal strokeWeight, strokeTint, gapTint;
	bool16 strokeOverprint, gapOverprint;

	/**
		Create a new table
	*/
	void IDCall_AT(ITCLParser *  parser);

};

/** Func_CT
	Implement the CT command
*/
class Func_CT
{
public :

	/**
		Constructor
		Read CT command parameters and set a current table
		@param IN parser : TCL parser
	*/
	Func_CT(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CT() {};

private :
		
	PMString tableName;
};


/** Func_SC
	Implement the SC command
*/
class Func_SC
{
public :

	/**
		Constructor
		Read SC command parameters
		@param IN parser : TCL parser
	*/
	Func_SC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SC() {};

private :

	/**
		Set as current selection a body cell, or a body row or a body column
	*/
	void IDCall_SC(ITCLParser *  parser);

	int32 startRow, startCol, endRow, endCol;

};


/** Func_SH
	Implement the SH command
*/
class Func_SH
{
public :

	/**
		Constructor
		Read SC command parameters
		@param IN parser : TCL parser
	*/
	Func_SH(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SH() {};

private :

	/**
		Set a current selection of header (or footer) cells, or a header(or footer) row or a header(or footer) column
	*/
	void IDCall_SH(ITCLParser *  parser);
		
	int8 selType;
	int32 startRow, startCol, endRow, endCol;

};

/** Func_MC
	Implement the MC command
*/
class Func_MC
{
public :

	/**
		Constructor
		Read MC command parameters and call IDCall_MC
		@param IN parser : TCL parser
	*/
	Func_MC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_MC() {};

private :
		
	bool16 merge;

	/**
		Merge cells
	*/
	void IDCall_MC(ITCLParser *  parser);

};

class Func_CPlus
{
public :

	/**
		Constructor
		Read C+ command parameters and call IDCall_CPlus
		@param IN parser : TCL parser
	*/
	Func_CPlus(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CPlus() {};

private :

	PMReal colWidth;
	int8 pos;
	int32 nb;
	int32 col;

	/**
		Insert Columns
	*/
	void IDCall_CPlus(ITCLParser *  parser);

};

class Func_RPlus
{
public :

	/**
		Constructor
		Read C+ command parameters and call IDCall_RPlus
		@param IN parser : TCL parser
	*/
	Func_RPlus(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_RPlus() {};

private :

	PMReal rowHeight; 
	int8 pos;
	int32 nb;

	/**
		Insert rows
	*/
	void IDCall_RPlus(ITCLParser *  parser);

};

class Func_CO
{
public :

	/**
		Constructor
		Read C+ command parameters and call IDCall_CO
		@param IN parser : TCL parser
	*/
	Func_CO(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CO() {};

private :

	int32 col;
	PMReal colWidth;

	/**
		Resize columns
	*/
	void IDCall_CO(ITCLParser *  parser);

};

class Func_OT
{
public :

	/**
		Constructor
		Read OT command parameters and call IDCall_OT
		@param IN parser : TCL parser
	*/
	Func_OT(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_OT() {};

private :

	int32 vj, FLOMetric;
	PMReal topInset,leftInset, bottomInset, rightInset;
	PMReal maxVJSpace, minFLO;
	PMReal rotation;
	bool16 clip;

	/**
		Set text option for cells
	*/
	void IDCall_OT(ITCLParser *  parser);

};

class Func_OS
{
public :

	/**
		Constructor
		ReadOS command parameters and call IDCall_OT
		@param IN parser : TCL parser
	*/
	Func_OS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_OS() {};

	static ITableLineType::SType GetSType(int32 strokeType);

private :

	int32 sides, strokeColor, gapColor, strokeType;
	PMReal strokeWeight, strokeTint, gapTint;
	bool16 strokeOverprint, gapOverprint;

	/**
		Set cell(s) stroke
	*/
	void IDCall_OS(ITCLParser *  parser);

};

class Func_OF
{
public :

	/**
		Constructor
		ReadOF command parameters and call IDCall_OF
		@param IN parser : TCL parser
	*/
	Func_OF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_OF() {};

private :

	int32 fillColor;
	PMReal fillTint;
	bool16 fillOverprint;

	/**
		Set cell(s) fill
	*/
	void IDCall_OF(ITCLParser *  parser);

};

class Func_RO
{
public :

	/**
		Constructor
		Read RO command parameters and call IDCall_RO
		@param IN parser : TCL parser
	*/
	Func_RO(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_RO() {};

private :

	PMReal rowHeight,maxHeight;
	int8 startRowOn;
	bool16 keepWithNext, autoGrow;

	/**
		Set row height
	*/
	void IDCall_RO(ITCLParser *  parser);

};


class Func_SP
{
public :

	/**
		Constructor
		Read FP command parameters and call IDCall_SP
		@param IN parser : TCL parser
	*/
	Func_SP(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SP() {};

private :

	int8 rowOrColumnPattern;
	int32  firstCount, firstStrokeColor, firstGapColor, firstStrokeType,
		   secondCount, secondStrokeColor, secondGapColor, secondStrokeType,
		   skipFirst, skipSecond;
	PMReal firstStrokeWeight, firstStrokeTint, firstGapTint,
		   secondStrokeWeight, secondStrokeTint, secondGapTint;
	bool16 firstStrokeOverprint, firstGapOverprint,
		   secondStrokeOverprint, secondGapOverprint;

	/**
		Set table stroke patterns
	*/
	void IDCall_SP(ITCLParser *  parser);

};

class Func_FP
{
public :

	/**
		Constructor
		Read FP command parameters and call IDCall_FP
		@param IN parser : TCL parser
	*/
	Func_FP(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_FP() {};

private :

	int32  firstCount, firstFillColor, 
		   secondCount, secondFillColor,
		   skipFirst, skipSecond;
	PMReal firstFillTint, secondFillTint;
	bool16 firstFillOverprint, secondFillOverprint;

	/**
		Set table fill patterns
	*/
	void IDCall_FP(ITCLParser *  parser);

};

class Func_MZ
{
public :

	/**
		Constructor
		Read MZ command parameters and call IDCall_MZ
		@param IN parser : TCL parser
	*/
	Func_MZ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_MZ() {};

private :

	int8 direction;

	/**
		Merge cells automatically
	*/
	void IDCall_MZ(ITCLParser *  parser);

	bool16 Compare(ITextModel * txtModel, TextIndex threadStart, int32 length, TextIndex threadStart2, int32 length2, bool16& containsOwnedItem, WideString& txtContent);

	ErrorCode MergeCells(const GridArea& area, bool16 containsOwnedItem, WideString content, ITableCommands * tableCommands, int8 direction);

};

class Func_RU
{
public :

	/**
		Constructor
		Read RU command parameters and call IDCall_RU
		@param IN parser : TCL parser
	*/
	Func_RU(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_RU() {};

private :

	int8 direction;

	/**
		Distribute uniformly
	*/
	void IDCall_RU(ITCLParser *  parser);

};


#endif //__Tableaux_h__