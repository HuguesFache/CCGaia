/*
//	File:	Transcoder.h
//
//	Date:	22-Apr-2003
//
//  Author :	Trias
//
*/

#ifndef __Transcoder_h__
#define __Transcoder_h__

// Table definitions
#define 	DEFAULT_TABLE		0
#define		MACINTOSH_TABLE		1
#define		WINDOWS_TABLE		2
#define		MSDOS_TABLE			3
#define		TURF_TABLE			4
#define		MEDIA_SYS_TABLE		5
#define		CALLIGRAME			6

// Special characters
#define DEL_CH		0xFF
#define	SUP_E		0xC2		// 
#define SUP_R		0xC0		// 
#define X_EF		0x0F		// kTextChar_ThinSpace
#define X_ED		0xA0		// kTextChar_EnSpace
#define	X_EC		0x1F		// kTextChar_EmSpace
#define	X_EP		0x10		// kTextChar_FigureSpace
#define	X_PF		0x0D		// Simulate a [PF] kTextChar_CR
#define X_LF		0x07		// Simulate a [LF] kTextChar_LF
#define	X_TAB		0x09		// Simulate a [TB]  kTextChar_Tab
#define X_PAGENUM	0x0018		// Simulate the auto page number kTextChar_PageNumber

/** Transcoder
	Contains special tables for character conversion
*/
class Transcoder
{

public :
	
	/**
		Constructor
		@param IN whichTable : table numero
	*/
	Transcoder(int8 whichTable);
	
	/**
		Destructor
	*/
	~Transcoder();

	/**
		Accessor methods
	*/
	int8 getCurrentTable();
	void setCurrentTable(int8 whichTable);

	/**
		Return the character in parameter transcoded using the current transcoding table
		@param IN theCharacter : character to convert
	*/
	bool8 Transcode(uchar& theCharacter);

private :

	int8 whichTable; // Indicate which table is currently used
	uchar * Code; // Transcoding table
};

#endif // __Transcoder_h__