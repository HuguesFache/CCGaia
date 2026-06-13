/*
//	File:	ITCLReader.h
//
//  Author: Trias
//
//	Date:	11-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __ITCLReader_h__
#define __ITCLReader_h__

#include "PrsID.h"
#include "ITCLParser.h"

class IPMUnknown;

/** ITCLReader
	Provides method to read TCL files, to extract 
	command's parameters and to get the state of the reading : 
	current unit, current char, end of command reached...
*/
class ITCLReader : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_ITCLREADER };

		/**
			Accessor methods
		*/
		virtual void SetStream(IPMStream * TCLStream) =0;
		virtual bool8 IsEndOfCommand() =0;
		virtual void SetEndOfCommand(bool8 end) =0;
		virtual int32 GetNumParam() =0;
		virtual void SetNumParam(int32 num) =0;
		virtual uchar16 GetCurrentANSIChar() =0;
		virtual UTF32TextChar GetCurrentChar() =0;
		virtual void SetUniteCourante(uchar unite) =0;
		virtual uchar GetUniteCourante() =0;

		/**
			Read char by char the TCL file 
		*/
		virtual void ReadChar() =0;

		/** 
			Return the file size in bytes
		*/
		virtual int32 GetFileLengthInBytes() =0;

		/** 
			Return the current number of bytes read
		*/
		virtual int32 GetNbBytesRead() =0;

		/**
			Return the current line number in the file read
		*/
		virtual int32 GetLineNumber() =0;

		/**
			Strip white spaces
		*/
		virtual void FiltreBlanc() =0;

		/**
			Strip all the characters until we meet the end of the command
		*/
		virtual void FiltreFin() =0 ;

		/**
			Strip white spaces and put in upper case the first character read
		*/
		virtual void FiltreBlancUpper() =0;

		/**
			Test if the next character (after stripping white spaces) is the sign Equal
			Return kTrue if it is this character, kFalse if it's an end of command character
			(']'), else throw an error
		*/
		virtual bool8 TestEgal() =0;

		/** 
			Test if the next character (after stripping white spaces) is the sign Equal
			Throw an error if it isn't
		*/
		virtual void MustEgal() =0;		

		/**
			Extract a string between double quotes from the stream, throw an error if no string is found
			@param IN LongueurMax : maximal length of the string
			Return the extracted string
		*/
		virtual PMString ExtractString(int16 LongueurMax) =0;

		/**
			Extract a character between single quotes from the stream, throw an error if no character is found
			Return the character extracted
		*/
		virtual uchar ExtractChar() =0;

		/**
			Extract an integer from the stream, throw an error if what is found is not an integer
			@param IN ValDef : default value if no integer found
			@param IN ValMin, ValMax : range in which the extracted integer must be
			Return the integer extracted
		*/
		virtual int32 ExtractInt(int32 ValDef,int32 ValMin,int32 ValMax) =0;

		/**
			Same function than the previous one, extract an uint32 instead of an int32
		*/
		virtual uint32 ExtractLongInt(uint32 ValDef,uint32 ValMin,uint32 ValMax) =0;

		/**
			Extract a boolean (true or false) from the stream, throw an error what is found is not a boolean
			@param IN ValDef : default value if no boolean found
			Return the boolean extracted
		*/
		virtual bool8 ExtractBooleen(bool8 ValDef) =0;

		/**
			Extract a character between single quotes from the stream, representing a unit,
			throw an error if no character is found
			Return the character extracted
		*/
		virtual uchar ExtractUnite() =0;

		/**
			Extract a real value from the stream, convert it in point from the current unit, 
			throw an error if what is found isn't a real
			@param IN UniteDef : default unit if no real value is found
			@param IN SignDef : default sign if no real value is found
			@param IN EntDef : default absolute part if no real value is found
			@param IN FracDef : default fractional part if no real value is found
			Return the real value extracted
		*/
		virtual PMReal ExtractReel(uchar UniteDef,uchar SignDef,int16 EntDef,int16 FracDef) =0;

		/**
			Extract a real value from the stream, without convertion with unit, 
			throw an error if what is found isn't a real
			@param IN SignDef : default sign if no real value is found
			@param IN EntDef : default absolute part if no real value is found
			@param IN FracDef : default fractional part if no real value is found
			@param IN Min : minimum of the value
			@param IN Max : maximum of the value
			Return the real value extracted
		*/
		virtual PMReal ExtractReel2(uchar SignDef,int16 EntDef,int16 FracDef, PMReal Min, PMReal Max) =0;


		/**
			Extract a real value from the stream, with forced convertion with unit, 
			throw an error if what is found isn't a real
			@param IN SignDef : default sign if no real value is found
			@param IN UniteDef : default unit if no unit value is found
			@param IN EntDef : default absolute part if no real value is found
			@param IN FracDef : default fractional part if no real value is found
			@param IN Min : minimum of the value
			@param IN Max : maximum of the value
			Return the real value extracted
		*/
		virtual PMReal ExtractReel3(uchar SignDef,uchar UniteDef,int16 EntDef,int16 FracDef, PMReal Min, PMReal Max) =0;

		/**
			Extract a real value, representing a percentage from the stream, throw an error if what is found isn't a real
			@param IN SignDef : default sign if no real value is found
			@param IN EntDef : default absolute part if no real value is found
			@param IN FracDef : default fractional part if no real value is found
			Return the real value extracted
		*/
		virtual PMReal ExtractPourcent(uchar SignDef,int16 EntDef,int16 FracDef) =0;

		/**
			Extract a TCL reference throw an error if no string is found
			@param IN ValDef : default value if no TCL reference found
			Return the extracted TCL reference
		*/
		virtual TCLRef ExtractTCLRef(TCLRef ValDef) =0;

		/**
			Convert a real value from a defined unit to points
			@param IN Unite : unit of the real value
			@param IN Sign : sign of the real value
			@param IN Entiere : absolute part of the real value
			@param IN Fractionnaire : fractionnal part of the real value
			Return the converted real
		*/
		virtual PMReal ConvertReel(uchar Unite, uchar Sign, int32 Entiere, int16 Fractionnaire) =0;

		/**
			Convert a real value from a defined unit to points
			@param IN Unite : unit of the real value
			@param IN number : real value (as string) to convert 
			Return the converted real
		*/
		virtual PMReal ConvertReel(uchar Unite, PMString number) =0;
};

#endif // __ITCLReader_h__
