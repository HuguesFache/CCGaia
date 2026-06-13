/*
//	File:	PrsTCLReader.h
//
//  Author: Trias
//
//	Date:	11-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __PrsTCLReader_h__
#define __PrsTCLReader_h__

// API includes
#include "CPMUnknown.h"

// project includes
#include "ITCLReader.h"


/** PrsTCLReader
	Implements the ITCLReader interface
*/
class PrsTCLReader : public CPMUnknown<ITCLReader> 
{

	public :
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PrsTCLReader(IPMUnknown * boss);

		/** 
			Destructor
		*/
		~PrsTCLReader();

		// ------------------
		//  Accessor methods
		// ------------------
		virtual void SetStream(IPMStream * TCLStream);
		virtual bool8 IsEndOfCommand();
		virtual void SetEndOfCommand(bool8 end);
		virtual int32 GetNumParam();
		virtual void SetNumParam(int32 num);
		virtual uchar16 GetCurrentANSIChar();
		virtual UTF32TextChar GetCurrentChar();
		virtual void SetUniteCourante(uchar unite);
		virtual uchar GetUniteCourante();

		// ------------------------------
		//  File reading and information
		// ------------------------------
		virtual void ReadChar();
		virtual int32 GetFileLengthInBytes();
		virtual int32 GetNbBytesRead();
		virtual int32 GetLineNumber();

		// -----------------------
		//  White space stripping
		// -----------------------
		virtual void FiltreBlanc();
		virtual void FiltreBlancUpper();

		//characters stripping
		virtual void FiltreFin() ;

		// -----------------------------------
		//  Methods to extract TCL parameters
		// -----------------------------------
		virtual bool8 TestEgal();
		virtual void MustEgal();
		virtual PMString ExtractString(int16 LongueurMax);
		virtual uchar ExtractChar();
		virtual int32 ExtractInt(int32 ValDef,int32 ValMin,int32 ValMax);
		virtual uint32 ExtractLongInt(uint32 ValDef,uint32 ValMin,uint32 ValMax);
		virtual bool8 ExtractBooleen(bool8 ValDef);
		virtual uchar ExtractUnite();
		virtual PMReal ExtractReel(uchar UniteDef,uchar SignDef,int16 EntDef,int16 FracDef);
		virtual PMReal ExtractReel2(uchar SignDef,int16 EntDef,int16 FracDef, PMReal Min, PMReal Max) ;
		virtual PMReal ExtractReel3(uchar SignDef,uchar UniteDef,int16 EntDef,int16 FracDef, PMReal Min, PMReal Max) ;
		virtual PMReal ExtractPourcent(uchar SignDef,int16 EntDef,int16 FracDef);
		virtual TCLRef ExtractTCLRef(TCLRef ValDef);

		// ------------
		//  Conversion
		// ------------
		virtual PMReal ConvertReel(uchar Unite, uchar Sign, int32 Entiere, int16 Fractionnaire);
		virtual PMReal ConvertReel(uchar Unite, PMString number);
	
	private :
	
		IPMStream * TCLStream; // Stream from TCL file

		WideString fileBuf; // Buffer for file reading
		int32 filePtr, // Pointer to next character to read
			  remaining; // Number of characters remaining to read in the buffer

		bool8 EndOfCommand; // Indicate when a whole command has been read
		int32 numParam; // Position of the current parameter being read
		UTF32TextChar character; // Current character being read
		uchar16 utf16Char;
		uchar UniteCourante; // Current default unit define for real TCL parameters

		int32 totalBytes; // file length in bytes
		int32 nbBytesRead; // number of bytes already read
		int32 lineNumber; // current line number in the file read
};

#endif // __PrsTCLReader_h__