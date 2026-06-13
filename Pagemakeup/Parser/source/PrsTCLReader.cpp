/*
//	File:	PrsTCLReader.cpp
//
//  Author: Trias
//
//	Date:	11-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

// Plug-in includes
#include "VCPlugInHeaders.h"

// General includes
#include "IPMStream.h"
#include "PlatformChar.h"

// Interface includes
#include "IUnitOfMeasure.h"
#include "ISubject.h"

// Project includes 
#include "PrsTCLReader.h"
#include "TCLError.h"

#include "StringUtils.h"

// C++ includes
#include <string>

#include "CAlert.h"

CREATE_PMINTERFACE(PrsTCLReader,kPrsTCLReaderImpl)

/* Constructor
*/
PrsTCLReader::PrsTCLReader(IPMUnknown * boss)
: CPMUnknown<ITCLReader>(boss)
{
	this->TCLStream = nil;
	this->EndOfCommand = kFalse;
	this->remaining = 0;
	this->filePtr = 0;
	this->UniteCourante = ' ';
	this->nbBytesRead = 0;
	this->totalBytes = 0;
}

/* Destructor
*/
PrsTCLReader::~PrsTCLReader()
{
}

/* SetStream
*/
void PrsTCLReader::SetStream(IPMStream * TCLStream)
{
	TCLStream->AddRef();
	this->TCLStream = TCLStream;
	
	this->EndOfCommand = kFalse;
	this->remaining = 0;
	this->filePtr = 0;
	this->UniteCourante = ' ';
	this->nbBytesRead = 0;
	this->lineNumber = 1;
	
	char * tmpBuf = new char[1024]; 

	// Calculate the stream length
	this->totalBytes = 0;        
	TCLStream->Seek(0, kSeekFromStart);
	do
	{
		int32 nbRead = TCLStream->XferByte((uchar *)tmpBuf,1024);
		totalBytes += nbRead;
		
	} while(TCLStream->GetStreamState() != kStreamStateEOF);
	

	// Skip BOM if any
	TCLStream->Seek(0, kSeekFromStart);
	int offset = 0; // ANSI, Default 
	TCLStream->XferByte((uchar *)tmpBuf,4); // Get the byte-order mark, if there is one 
    if ((uchar)tmpBuf[0] == 0xef && (uchar)tmpBuf[1] == 0xbb && (uchar)tmpBuf[2] == 0xbf) // utf-8 
        offset = 3;

	delete[] tmpBuf;

	// Restart the stream to the beginning of the file (excluding BOM if any)
	TCLStream->Seek(offset, kSeekFromStart);
	totalBytes -= offset;

	// Read whole file in memory	
	tmpBuf = new char[totalBytes+1];
	::memset(tmpBuf,'\0',totalBytes+1);
	TCLStream->XferByte((uchar*)tmpBuf,totalBytes);

	// Convert UTF-8 char buffer to an InDesign WideString
	std::string tmpBuf2 (tmpBuf, totalBytes); // to use with string utils

	StringUtils::ConvertUTF8ToWideString(tmpBuf2, fileBuf);
	
	TCLStream->Close();
	TCLStream->Release();
	TCLStream = nil;

	delete[] tmpBuf;
	
	// Notify the observers of the file length
	//InterfacePtr<ISubject> subject (this, UseDefaultIID());
	//subject->Change(kNewStreamMessage, IID_ITCLREADER);
}


/* GetFileLengthInBytes
*/
inline int32 PrsTCLReader::GetFileLengthInBytes()
{
	return totalBytes;
}

/* GetNbBytesRead
*/
inline int32 PrsTCLReader::GetNbBytesRead()
{
	return nbBytesRead;
}

/* GetLineCount
*/
inline int32 PrsTCLReader::GetLineNumber()
{
	return lineNumber;
}

/* GetCurrentANSIChar
*/
inline uchar16 PrsTCLReader::GetCurrentANSIChar()
{
	return this->utf16Char;
}

/* GetCurrentChar
*/
inline UTF32TextChar PrsTCLReader::GetCurrentChar()
{
	return this->character;
}

/* GetUniteCourante
*/
inline uchar PrsTCLReader::GetUniteCourante()
{
	return this->UniteCourante;
}

/* SetUniteCourante
*/
inline void PrsTCLReader::SetUniteCourante(uchar unite)
{
	this->UniteCourante = unite;
}

/* GetNumParam
*/
inline int32 PrsTCLReader::GetNumParam()
{
	return this->numParam;
}

/* SetNumParam
*/
inline void PrsTCLReader::SetNumParam(int32 num)
{
	this->numParam = num;
}

/* IsEndOfCommand
*/
inline bool8 PrsTCLReader::IsEndOfCommand()
{
	return this->EndOfCommand;
}

/* SetEndOfCommand
*/
inline void PrsTCLReader::SetEndOfCommand(bool8 end)
{
	this->EndOfCommand = end;
}

/* ReadChar
*/
void PrsTCLReader::ReadChar()
{
	//// Bufferized reading in the file
	//if(remaining == 0)
	//{
	//	remaining = TCLStream->XferByte(fileBuf,FBUF_SIZE);
	//	if(remaining == 0 
	//	   && (TCLStream->GetStreamState() == kStreamStateEOF
	//		   || TCLStream->GetStreamState() == kStreamStateFailure))
	//	{
	//		character = '?';
	//		TCLStream->Close();
	//		throw TCLError(PMString(kErrEOF));
	//	}
	//	filePtr = 0;
	//}

	//remaining--;
	//character = fileBuf[filePtr++];

	//// Update file infos
	//nbBytesRead += sizeof(character);
	//
	//if(character == ']')
	//	EndOfCommand = kTrue;
	//else
	//	if(character == '\r')
	//		lineNumber++;

	do
	{
		if(filePtr == fileBuf.Length()) throw TCLError(PMString(kErrEOF));
	
		character = fileBuf[filePtr];
		++filePtr;
		if(character == '\r')
		{
			++lineNumber;
			character = '\n'; // this way, we don't go out of loop
		}

	} while(character == '\n');

	
	if(!character.isExtraWide())
		utf16Char = character.GetValue();
	else
		utf16Char = '\0';

	if(utf16Char == ']')
		EndOfCommand = kTrue;

	// Notify the observers of the file length
	//InterfacePtr<ISubject> subject (this, UseDefaultIID());
	//subject->Change(kBytesReadMessage, IID_ITCLREADER);
}

/* FiltreBlanc
*/
void PrsTCLReader::FiltreBlanc()
{
	do
	{
		ReadChar();
	} while (utf16Char == ' ');
}


/* FiltreFin
*/
void PrsTCLReader::FiltreFin()
{
	do
	{
		ReadChar();
	} while (utf16Char != ']');
}


/* FiltreBlancUpper
*/
void PrsTCLReader::FiltreBlancUpper()
{
	FiltreBlanc();
	if((utf16Char >= 'a') && (utf16Char <= 'z'))
	{
		utf16Char = utf16Char + 'A' - 'a';
	}	
}

/* ExtractString
*/
PMString PrsTCLReader::ExtractString(int16 LongueurMax)
{
	PMString chaine;
	int16 count=0;
	
	numParam++;

	if (EndOfCommand==kTrue)
		throw TCLError(PMString(kErrNoString));

	FiltreBlanc();

	if ((utf16Char==']') || (utf16Char==';'))
		throw TCLError(PMString(kErrNoString));

	if (utf16Char != '"')
		throw TCLError(PMString(kErrNoStringHeader));

	do 
	{
		ReadChar();
		//if (utf16Char==';') 
		//	throw TCLError(PMString(kErrNoEOString));
		
		if (utf16Char==']') 
			EndOfCommand = kFalse; // Allow character ']' in strings (in ReadChar(), EndOfCommand is set to True when ']' is read
		
		if (utf16Char=='"')
			break;
		else
		{
			chaine.AppendW(character);
			count = count+1;
			if (count>LongueurMax) throw TCLError(PMString(kErrLength));
		}

	} while (kTrue);

	FiltreBlanc();

	if ((utf16Char!=';')&&(utf16Char!=']')) 
		throw TCLError(PMString(kErrEOVariable));

	chaine.SetTranslatable(kFalse);
	return chaine;
}

/* ExtractChar
*/
uchar PrsTCLReader::ExtractChar()

{  
	uchar CaractereUnitaire ='\0';

	numParam++;

	FiltreBlanc();

	if ((utf16Char==']') || (utf16Char==';')) throw TCLError(PMString(kErrNoChar));
	if (utf16Char != 0x27) throw TCLError(PMString(kErrCharHeader));

	ReadChar();
	if ((utf16Char==';')||(utf16Char==']')) throw TCLError(PMString(kErrNoEOChar));
	if (utf16Char!=0x27)
	{
		CaractereUnitaire=utf16Char;
		ReadChar();
		if ((utf16Char==';')||(utf16Char==']')) throw TCLError(PMString(kErrNoEOChar));
		if (utf16Char!=0x27) throw TCLError(PMString(kErrNoEOChar));
	}

	FiltreBlanc();
	if ((utf16Char!=';')&&(utf16Char!=']')) throw TCLError(PMString(kErrEOVariable));
	
	return CaractereUnitaire;
}

/* TestEgal
*/
bool8 PrsTCLReader::TestEgal(void)
{
	bool8 Egal = kFalse;

    FiltreBlanc();
    if (utf16Char=='=')
       Egal=kTrue;
    else
    {
       if (utf16Char==']')
			Egal=kFalse;
       else
			throw TCLError(PMString(kErrEgal));
    }
    return Egal;
}

/* MustEgal
*/
void PrsTCLReader::MustEgal()
{
	bool8 Egal = TestEgal();
	if(Egal == kFalse)
		throw TCLError(PMString(kErrNoEgal));
}

/* ExtractInt
*/
int32 PrsTCLReader::ExtractInt(int32 ValDef,int32 ValMin,int32 ValMax)

{
   int32 intermediaire;
   int32 Integer = ValDef;
   uchar Sign = '+';

   numParam++;
   
// Dans le cas d'une variable optionnelle test de la fin de la commande
// et retour sans lire de caractere

   if (EndOfCommand==kTrue)
		return Integer;

   FiltreBlanc();

   if(utf16Char == ';')
		return Integer;

   if (utf16Char==']')
		return Integer;

   if (utf16Char=='-')
   {
      Sign='-';
      FiltreBlanc();
   }
   else
   {
      if (utf16Char=='+')
      {
         FiltreBlanc();
      }
   }

   intermediaire=0;
   while ((utf16Char>='0') && (utf16Char<='9'))
   {
      intermediaire=(intermediaire*10)+ (utf16Char-'0');
      if (intermediaire >= 0xFFFFFFFF) throw TCLError(PMString(kErrIntTooBig));
      FiltreBlanc();
   }

   if (Sign=='-')
		Integer = -intermediaire;
   else
		Integer = intermediaire;

   if ((utf16Char==']')||(utf16Char==';'))
   {
      if (Integer>ValMax) throw TCLError(PMString(kErrSupMax));
      if (Integer<ValMin) throw TCLError(PMString(kErrInfMin));
   }
   else
      throw TCLError(PMString(kErrBadInt));

   
   return Integer;
}

uint32 PrsTCLReader::ExtractLongInt(uint32 ValDef,uint32 ValMin,uint32 ValMax)
{
   uint32 intermediaire, LongInteger = ValDef;
   uchar Sign = '+';

   numParam++;
   
// Dans le cas d'une variable optionnelle test de la fin de la commande
// et retour sans lire de caractere

   if (EndOfCommand==kTrue)
		return LongInteger;

   FiltreBlanc();

   if(utf16Char == ';')
		return LongInteger;

   if (utf16Char==']')
		return LongInteger;

   if (utf16Char=='-')
   {
      Sign='-';
      FiltreBlanc();
   }
   else
   {
      if (utf16Char=='+')
      {
         FiltreBlanc();
      }
   }

   intermediaire=0;
   while ((utf16Char>='0') && (utf16Char<='9'))
   {
      intermediaire=(intermediaire*10)+ (utf16Char-'0');
      if (intermediaire > 0xFFFFFFFF) throw TCLError(PMString(kErrLongIntTooBig));
      FiltreBlanc();
   }

   LongInteger = intermediaire;

   if ((utf16Char==']')||(utf16Char==';'))
   {
      if (LongInteger > ValMax) throw TCLError(PMString(kErrSupMax));
      if (LongInteger < ValMin) throw TCLError(PMString(kErrInfMin));
   }
   else
	  throw TCLError(PMString(kErrBadInt));

   return LongInteger;
}

/* ExtractUnite
*/
uchar PrsTCLReader::ExtractUnite()
{
   uchar Unite = ' ';
   
   numParam++;

   FiltreBlancUpper();
   if ((utf16Char==']')||(utf16Char==';')) throw TCLError(PMString(kErrNoUnite));
   if ((utf16Char!='P')&&(utf16Char!='D')&&(utf16Char!='C')&&
	   (utf16Char!='M')&&(utf16Char!='N'))
	   throw TCLError(PMString(kErrBadUnite));

   Unite = utf16Char;

   FiltreBlanc();
   if ((utf16Char!=']')&&(utf16Char!=';')) 
	   throw TCLError(PMString(kErrBadEndUnite));

   return Unite;
}

/* ExtractBooleen
*/
bool8 PrsTCLReader::ExtractBooleen(bool8 ValDef)

{
   bool8 Booleen=ValDef;

   numParam++;

   if (EndOfCommand==kTrue)
	  return Booleen;
  
   FiltreBlancUpper();
   if ((utf16Char==']')||(utf16Char==';'))
	  return Booleen;
 
   if ((utf16Char!='F')&&(utf16Char!='T'))
   		throw TCLError(PMString(kErrBadBool));

   if (utf16Char=='F')
   {
      FiltreBlancUpper();
      if (utf16Char!='A') throw TCLError(PMString(kErrBadBool));
      FiltreBlancUpper();
      if (utf16Char!='L') throw TCLError(PMString(kErrBadBool));
      FiltreBlancUpper();
      if (utf16Char!='S') throw TCLError(PMString(kErrBadBool));
      FiltreBlancUpper();
      if (utf16Char!='E') throw TCLError(PMString(kErrBadBool));
      Booleen=kFalse;
   }
   else
   {
      FiltreBlancUpper();
      if (utf16Char!='R') throw TCLError(PMString(kErrBadBool));
      FiltreBlancUpper();
      if (utf16Char!='U') throw TCLError(PMString(kErrBadBool));
      FiltreBlancUpper();
      if (utf16Char!='E') throw TCLError(PMString(kErrBadBool));
      Booleen=kTrue;
   }
   FiltreBlanc();
   if ((utf16Char!=']')&&(utf16Char!=';'))
   	   throw TCLError(PMString(kErrBadEndBool));

   return Booleen;
}

// -----------------------------------------------------------------------
// Convert the given value into a real.
// -----------------------------------------------------------------------

PMReal PrsTCLReader::ConvertReel(uchar Unite, uchar Sign, int32 Entiere, int16 Fractionnaire)
{
	PMString sValue;
	PlatformChar pc;
	pc.Set(Sign);
	sValue.Append(pc);
	sValue.AppendNumber(Entiere);
	sValue.Append(".");
	sValue.AppendNumber(Fractionnaire);

	return ConvertReel(Unite, sValue);
}

PMReal PrsTCLReader::ConvertReel(uchar Unite, PMString number)
{
	PMString::ConversionError error = PMString::kNoError;

	PMReal convertedValue = PMReal(number.GetAsDouble(&error));

	if(error != PMString::kNoError)
		throw TCLError(PMString(kErrConv));

	ClassID measureID;

	switch (Unite) {
      case 'P':
      case 'p':	
		measureID = kPicasBoss;
        break;
        
      case 'D':
      case 'd':
		measureID = kPointsBoss;
		break;

      case 'C':
      case 'c':
		measureID = kCicerosBoss;
        break;
        
      case 'M':
      case 'm':
		measureID = kCentimetersBoss;
        break;
        
      case 'N':
      case 'n':
		measureID = kMillimetersBoss;
		break;
         
      case '%':
		convertedValue = convertedValue / 100.0;
		measureID = kInvalidClass;
        break;

	  default:
		  measureID = kInvalidClass;
		  break;
   }
   
	if(measureID != kInvalidClass)
	{
		InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(measureID,IID_IUNITOFMEASURE));

		if(uom == nil)
		throw TCLError(PMString(kErrConv));

		// Do the conversion
		convertedValue = uom->UnitsToPoints(convertedValue);
	}
   	return convertedValue; 
}

// extraction d'une valeur r￩elle en forcant l'unite

PMReal PrsTCLReader::ExtractReel3(uchar SignDef,uchar UniteDef,int16 EntDef,int16 FracDef, PMReal Min, PMReal Max)

{
   PMString number("");
   PMReal convDef;
   uchar Unite = UniteDef;

	// Calcul de la valeur par defaut
    // ------------------------------

   numParam++;

   convDef = ConvertReel(Unite,SignDef,EntDef,FracDef);
   
   if (EndOfCommand == kTrue)
		return convDef;
   
   // recherche de l'unit￩
   // --------------------
   if (UniteCourante != ' ')
      Unite=UniteCourante;

   FiltreBlancUpper();

   // If we are at the end of the command or the parameter
   if ((utf16Char == ']') || (utf16Char == ';')) 
   {
      //usingDefaultValue = false;
	   return convDef;
   }

   if ((utf16Char=='P')||(utf16Char=='D')||(utf16Char=='C')||
       (utf16Char=='M')||(utf16Char=='N'))
   {
      Unite=utf16Char;
      FiltreBlancUpper();
   }

	// Test du signe
	// -------------

   if (utf16Char=='-')
   {
      number.Append(utf16Char);
      FiltreBlancUpper();
   }
   else
   {
      if (utf16Char=='+')
      {
         FiltreBlancUpper();
      }
   }
// recherche de la partie entiere
    int16 Entiere=0;
	while ((utf16Char>='0') && (utf16Char<='9')) 
	{
		 Entiere=(Entiere*10)+ (utf16Char-'0');
		 if (Entiere > kMaxInt16) throw TCLError(PMString(kErrPEntTooBig));

		 FiltreBlanc();
	}

	number.AppendNumber(Entiere);

   // Partie fractionnaire
   // --------------------

	if (utf16Char == ',' || utf16Char == '.')
	{					// If we have a fractional part

		FiltreBlanc();
		number.Append('.');
		while ((utf16Char >= '0') && (utf16Char <= '9')) 
		{
			number.Append(utf16Char);
			FiltreBlanc();
		}
	}

	if ((utf16Char != ']') && (utf16Char != ';')) 
		throw TCLError(PMString(kErrBadReal));

	convDef = ConvertReel(Unite,number);

	if (convDef > Max)
		throw TCLError(PMString(kErrRealTooBig));

	if (convDef < Min)
		throw TCLError(PMString(kErrRealTooSmall));

	return convDef ;
}




// extraction d'une valeur r￩elle sans unite

PMReal PrsTCLReader::ExtractReel2(uchar SignDef,int16 EntDef,int16 FracDef, PMReal Min, PMReal Max)

{
   PMString number("");
   PMReal convDef;
   uchar Unite = '#'; //aucune unite

	// Calcul de la valeur par defaut
    // ------------------------------

   numParam++;
	
   convDef = ConvertReel(Unite,SignDef,EntDef,FracDef);
   
   if (EndOfCommand == kTrue)
		return convDef;

   FiltreBlancUpper();

   // If we are at the end of the command or the parameter
   if ((utf16Char == ']') || (utf16Char == ';')) 
   {
      //usingDefaultValue = false;
	   return convDef;
   }

   if ((utf16Char=='P')||(utf16Char=='D')||(utf16Char=='C')||
       (utf16Char=='M')||(utf16Char=='N'))
   {
      Unite=utf16Char;
      FiltreBlancUpper();
   }

	// Test du signe
	// -------------

   if (utf16Char=='-')
   {
      number.Append(utf16Char);
      FiltreBlancUpper();
   }
   else
   {
      if (utf16Char=='+')
      {
         FiltreBlancUpper();
      }
   }
    // recherche de la partie entiere
    int16 Entiere=0;
	while ((utf16Char>='0') && (utf16Char<='9')) 
	{
		 Entiere=(Entiere*10)+ (utf16Char-'0');
		 if (Entiere > 32767) throw TCLError(PMString(kErrPEntTooBig));

		 FiltreBlanc();
	}

	number.AppendNumber(Entiere);

   // Partie fractionnaire
   // --------------------

	if (utf16Char == ',' || utf16Char == '.')
	{					// If we have a fractional part
		FiltreBlanc();
		number.Append('.');				
		while ((utf16Char >= '0') && (utf16Char <= '9')) 
		{
			number.Append(utf16Char);
			FiltreBlanc();
		}
	}

	if ((utf16Char != ']') && (utf16Char != ';')) 
		throw TCLError(PMString(kErrBadReal));

	convDef = ConvertReel(Unite,number);

	if (convDef > Max)
		throw TCLError(PMString(kErrRealTooBig));

	if (convDef< Min)
		throw TCLError(PMString(kErrRealTooSmall));

	return convDef ;
}


// extraction d'une valeur r￩elle

PMReal PrsTCLReader::ExtractReel(uchar UniteDef,uchar SignDef,int16 EntDef,int16 FracDef)

{
   PMString number("");
   PMReal convDef;
   uchar Unite = UniteDef;

	// Calcul de la valeur par defaut
    // ------------------------------

   numParam++;
	
   convDef = ConvertReel(UniteDef,SignDef,EntDef,FracDef);
   
   if (EndOfCommand == kTrue)
		return convDef;

   // recherche de l'unit￩
   // --------------------
   if (UniteCourante != ' ')
      Unite=UniteCourante;

   FiltreBlancUpper();

   // If we are at the end of the command or the parameter
   if ((utf16Char == ']') || (utf16Char == ';')) 
   {
      //usingDefaultValue = false;
	   return convDef;
   }

   if ((utf16Char=='P')||(utf16Char=='D')||(utf16Char=='C')||(utf16Char=='M')||(utf16Char=='N'))
   {
      Unite=utf16Char;
      FiltreBlancUpper();
   }

	// Test du signe
	// -------------

   if (utf16Char=='-')
   {
      number.Append(utf16Char);
      FiltreBlancUpper();
   }
   else
   {
      if (utf16Char=='+')
      {
         FiltreBlancUpper();
      }
   }

   // recherche de la partie entiere
    int16 Entiere=0;
	while ((utf16Char>='0') && (utf16Char<='9')) 
	{
		 Entiere=(Entiere*10)+ (utf16Char-'0');
		 if (Entiere > 32767) throw TCLError(PMString(kErrPEntTooBig));

		 FiltreBlanc();
	}

	number.AppendNumber(Entiere);

   // Partie fractionnaire
   // --------------------
	if (utf16Char == ',' || utf16Char == '.')
	{					// If we have a fractional part
		FiltreBlanc();
		number.Append('.');
		while ((utf16Char >= '0') && (utf16Char <= '9')) 
		{
			number.Append(utf16Char);
			FiltreBlanc();
		}
	}

	if ((utf16Char != ']') && (utf16Char != ';')) 
		throw TCLError(PMString(kErrBadReal));

	return ConvertReel(Unite, number);
}



PMReal PrsTCLReader::ExtractPourcent(uchar SignDef,int16 EntDef,int16 FracDef)
{
   PMString number("");   
   PMReal convDef;
   uchar Unite;

   numParam++;

   Unite='%';

   convDef = ConvertReel(Unite,SignDef,EntDef,FracDef);
   
   if (EndOfCommand == kTrue)
		return convDef;

   FiltreBlancUpper();
   if ((utf16Char==']')||(utf16Char==';'))
	 	return convDef;
    
    // Test du signe
   if (utf16Char=='-')
   {
	  number.Append(utf16Char);
      FiltreBlancUpper();
   }
   else
   {
      if (utf16Char=='+')
      {
         FiltreBlancUpper();
      }
   }

// recherche de la partie entiere
   int32 Entiere=0;
   while ((utf16Char>='0') && (utf16Char<='9'))
   {
      Entiere=(Entiere*10)+ (utf16Char-'0');
	  if (Entiere > 32767) throw TCLError(PMString(kErrPEntTooBig));

      FiltreBlanc();
   }
 
   number.AppendNumber(Entiere);

   if (utf16Char == ',' || utf16Char == '.')
   {
      FiltreBlanc();
	  number.Append('.');
	  while ((utf16Char >= '0') && (utf16Char <= '9')) 
	  {
		 number.Append(utf16Char);
		 FiltreBlanc();
	  }
   }
   if ((utf16Char!=']')&&(utf16Char!=';'))
       throw TCLError(PMString(kErrBadPourcent));
   
   return ConvertReel(Unite,number);
}

#if REF_AS_STRING
inline TCLRef PrsTCLReader::ExtractTCLRef(TCLRef ValDef)
{
	return ExtractString(32767);
}
#else
inline TCLRef PrsTCLReader::ExtractTCLRef(TCLRef ValDef)
{
	return ExtractInt(ValDef, MIN_BLOC, MAX_BLOC);	
}
#endif