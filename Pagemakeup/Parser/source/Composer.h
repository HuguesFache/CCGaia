/*
//	File:	Composer.h
//
//	Date:	7-Nov-2005
//
//  Author : Trias
//
*/


#ifndef __Composer_h__
#define __Composer_h__

#include "PMUTypes.h"
#include "PMRect.h" // for declaration of PBPMRect

class ITCLParser;
class ITCLReader;
class ITextFrameColumn;
class ITextModel;
class IWaxStrand;
class IGeometry;

/** Func_BW
	Implement the BW command
*/
class Func_BW
{
public :

	/**
		Constructor
		Read BW command parameters and call IDCall_BW
		@param IN parser : TCL parser
	*/
	Func_BW(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_BW();

private :
	
	TCLRef ref; 
	int32 round;
	uchar unit;
	int8 direction;
	PMReal tolerance;
	
	/**
		Round the height of a block to a certain value
	*/
	void IDCall_BW(ITCLParser *  parser);
};

/** Func_DW
	Implement the DW command
*/
class Func_DW
{
public :

	/**
		Constructor
		Read DW command parameters and call IDCall_DW
		@param IN parser : TCL parser
	*/
	Func_DW(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DW();

private :
	
	TCLRef ref;
	PMReal offset;

	/**
		Handle modification of depth of document according to the size of a given box
	*/
	void IDCall_DW(ITCLParser *  parser);
};


/** Func_B3
	Implement the B3 command
*/
class Func_B3
{
public :

	/**
		Constructor
		Read B3 command parameters and call IDCall_B3
		@param IN parser : TCL parser
	*/
	Func_B3(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_B3();

private :
	
	TCLRef ref;
	int8 boxType;
	bool16 back;

	/**
		Create a box of the same size of the document
	*/
	void IDCall_B3(ITCLParser *  parser);
};

/** Func_AW
	Implement the AW command
*/
class Func_AW
{
public :

	/**
		Constructor
		Read AW command parameters and call IDCall_AW
		@param IN parser : TCL parser
	*/
	Func_AW(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AW() {}

	static PMReal GetFrameLastLineAbsolutePosition(IWaxStrand * waxStrand, ITextFrameColumn * frame);	

private :
	
	TCLRef ref;
	PMReal threshold;
	int32 maxKerningValue;

	/**
		Adjust leading in text frame so that the whole text fits in the frame
	*/
	void IDCall_AW(ITCLParser *  parser);

	PMReal ComputeTotalLeading(IWaxStrand * waxStrand);
	bool16 ApplyScaleFactorToLeading(ITextModel * txtModel, PMReal yFactor);	
};

/** Func_MI
	Implement the MI command
*/
class Func_MI
{
public :

	/**
		Constructor
		Read MI command parameters and call IDCall_MI
		@param IN parser : TCL parser
	*/
	Func_MI(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_MI() {}

private :
	
	TCLRef ref;
	int8 corner, type;
	PMReal initialWidth, initialHeight;

	/**
		Allows to manipulate images while resizing a graphic frame
	*/
	void IDCall_MI(ITCLParser *  parser);

	ErrorCode FitImageInFrame(IGeometry * imageGeo, const PBPMRect& frameBBox, const PMReal& strokeWeight, const int8& anchor, const bool16& proportional);
};

/** Func_IB
	Implement the IB command
*/
class Func_IB
{
public :

	/**
		Constructor
		Read IB command parameters and call IDCall_IB
		@param IN parser : TCL parser
	*/
	Func_IB(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_IB() {}

private :
		
	PMReal bleed;

	/**
		Apply bleed to image frame placed in document borders
	*/
	void IDCall_IB(ITCLParser *  parser);
};

/** Func_AS
	Implement the AS command
*/
class Func_AS
{
public :

	/**
		Constructor
		Read AS command parameters and call IDCall_AS
		@param IN parser : TCL parser
	*/
	Func_AS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AS() {}

private :
	
	TCLRef ref;
	PMReal threshold, minSize, maxSize;

	/**
		Adjust font size in text frame so that the whole text fits in the frame
	*/
	void IDCall_AS(ITCLParser *  parser);

	void GetFontSizeBounds(ITextModel * txtModel, PMReal * minSize, PMReal * maxSize);
	void ApplyScaleFactorToFontSize(ITextModel * txtModel, PMReal factor);
};

#endif // __Composer_h__
