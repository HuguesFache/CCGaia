/*
//	File:	PMUTypes.h
//
//	Date:	22-Mars-2006
//
//  Author : Trias
//
*/


#ifndef __PMUTypes_h__
#define __PMUTypes_h__

#include "GlobalDefs.h"

#include "K2Vector.h"
#include "K2Vector.tpp"
#include "KeyValuePair.h"

#if  REF_AS_STRING
#include "IStringData.h"
#else
#include "IIntData.h"
#endif

// -------------------
//  Preferences datas
// -------------------

typedef struct {

	// Saving documents
	bool8			useSavePath;
	PMString		savePath;				// Where to save documents
	
	// Opening docs
	bool8			usePathForOpen;
	PMString		openPath;
	
	// Exporting docs
	PMString 		exportPath;

	// Pictures
	bool8			autoPictImport;			// Want to import pictures
	bool8			useImportPath;			// Use relative path ?
	PMString		picturePath;			// Where pictures are located
	bool8			createTextBox;			// Create a text box for missing picture
	bool8			isNonPrinting;			// True if the box is to be non-printing
	int32			colorIndex;				// fill color to used when picture is missing
	int32			colorTint;				// fill color tint to used when picture is missing

	// Divers
	bool8			stopOnError;			// False -> continue, true ->stop
	bool8			stopDisplay;			// Display error

} PrefRec;


// ----------------------------------
//  Structures handled by the parser
// ----------------------------------

typedef struct {
	PMString name;
	UID colorUID;
} sColor;

typedef	KeyValuePair <int32, PMString> Elt;
typedef	K2Vector<Elt> Vector; // For font list and color list

// Item list types

#if REF_AS_STRING

typedef IStringData ITCLRefData;
typedef PMString TCLRef;

const TCLRef kInvalidTCLRef("",PMString::kDontTranslateDuringCall);
const TCLRef kZeroTCLRef("0",PMString::kDontTranslateDuringCall);
const TCLRef kDefaultTCLRef("1",PMString::kDontTranslateDuringCall);
const TCLRef kCurrentTCLRef("",PMString::kDontTranslateDuringCall);
const TCLRef kDefaultTCLRefData("",PMString::kDontTranslateDuringCall);

// required operator declaration
TCLRef operator++(TCLRef& ref);
TCLRef operator--(TCLRef& ref);
int32 operator-(const TCLRef& ref1, const TCLRef& ref2);

#else // REF_AS_INT32

typedef IIntData ITCLRefData;
typedef int32 TCLRef;

const TCLRef kInvalidTCLRef = -1;
const TCLRef kZeroTCLRef = 0;
const TCLRef kDefaultTCLRef = 1;
const TCLRef kCurrentTCLRef = -1;
const TCLRef kDefaultTCLRefData = 0;
#endif

typedef KeyValuePair<TCLRef,UID> TCLRefItem;
typedef K2Vector<TCLRefItem> RefVector;

// Story list types (for PermRefs)
typedef struct permrefstruct
{ 
	uint32 ref; 
	uint32 param1, param2, param3, param4; 

	typedef object_type data_type;
	
	permrefstruct()
	{
		ref = param1 = param2 = param3 = param4 = 0;
	}

	inline bool operator!= (const struct permrefstruct& permref) const
	{
		return !(*this == permref) ;
	}

	inline bool operator==  (const struct permrefstruct& permref) const
	{
		return ( this->ref == permref.ref &&
				 this->param1 == permref.param1 &&
				 this->param2 == permref.param2 &&
				 this->param3 == permref.param3 &&
				 this->param4 == permref.param4 
				);
	} 

} PermRefStruct;

typedef KeyValuePair<PermRefStruct,K2Vector<UID> > PermRefStories;
typedef K2Vector<PermRefStories> PermRefVector;

const PermRefStruct kNullPermRef;

// Table list
typedef KeyValuePair<PMString,UID> TCLRefTable;
typedef K2Vector<TCLRefTable> TableVector;

typedef struct cj {
	PMString name ;
	bool8 hyphenMode;
	bool8 capitalized; 
	int16 minWord;
	int16 afterFirst;
	int16 beforeLast;
	int16 limit;
	PMReal hyphenZone;
	bool8 lastWord;
	PMReal wordMin;PMReal wordMax;
	PMReal wordDes;
	PMReal letterMin;
	PMReal letterMax;
	PMReal letterDes;
	struct cj * next ;
} CJ, * CJPtr ;


typedef struct {
	Vector fontVect;
	Vector styleVect;
	CJPtr CJVect ;
} infoTbl;

// Struct for TOC style
typedef struct formatEntryParam {
		int32 formatEntryStyle, formatPageStyle, formatSeparatorStyle;
		int16 level, pageNumPos;
		bool16 levelSort;
		PMString separator;
} FormatEntryParam;

#endif // __PMUTypes_h__
