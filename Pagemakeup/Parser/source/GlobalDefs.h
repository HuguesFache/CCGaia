/*
//	File:	GlobalDefs.h
//
//	Date:	7-May-2003
//
//  Author : Trias
//
*/

#ifndef __GlobalDefs_h__
#define __GlobalDefs_h__

#ifdef WINDOWS
	#define kNewLine "\r\n"
	#define kDirSeparatorChar '\\'
	#define kDirSeparator "\\"
#else
	#define kNewLine "\r"
	#define kDirSeparatorChar ':'
	#define kDirSeparator ":"
#endif

//pas besoin de commenter
#define APL_INI_USE_USERFOLDER	0

//-------------------
//  DEMO OPTION
//-------------------
#define DEMO			0

//---------------------------
//  TCL REFERENCE TYPE OPTION (SCHNEIDER, PPI, NEWSMED)
//---------------------------
#define REF_AS_STRING	0

//-------------------
//  SERVER OPTION
//-------------------
#ifndef SERVER
#define SERVER 0
#endif

//-------------------
//  REVERSE OPTION
//-------------------
#define REVERSE			0

//-------------------
//  PERMREFS OPTION
//-------------------
#define PERMREFS		1

//-------------------
//  COMPOSER OPTION
//-------------------
#define COMPOSER		0

//-------------------
//  XRAIL OPTION
//-------------------
#define MULTIBASES		0
#define XRAIL			1
//-------------------------
//  CROSS REFERENCES OPTION
//-------------------------
#define XREFS			0

//--------------------------------------------------
//  SHOW WARNING ABOUT HIDDEN PERSISTENT DATA OPTION
//--------------------------------------------------
#define CRITICALDATA	0

//-------------------------------------
//  AUTO SAVE CONVERTED DOCUMENT OPTION
//-------------------------------------
#define CONVERT_AUTOSAVE	0

//-------------------
//  HYPERLINKS MODULE (MEDIAGERANCE)
//-------------------
#define HYPERLINKS			0

//------------------
//  REVERSE WITH LOG (+ OTHER MEDIAGERANCE SPECIAL FEATURES)
//------------------	
#define REVERSE_CHECKER		0

//------------------
//  COULAGE_AUTO (PPI, NEWSMED)
//------------------
#define COULAGE_AUTO		0

//------------------
//  TRAFFIC_PRESSE
//------------------
#define TRAFFIC_PRESSE		0

//------------------
//  Mise a jour affectations (TRIBUNE)
//------------------
#define AFFECT				0

//------------------
//  PageRef (MEDIAGERANCE SPECIAL FEATURES)
//------------------
#define PAGE_REF 0

//------------------
// Export des infos de Resa redactionnelle (TRIBUNE)
//------------------
#define XPAGE				1

//------------------
//  Tagger automatiquement avec PermRef  (GEDIMAT)
//------------------
#define AUTO_TAGGING		0

//------------------
//  Fonctions specifiques POINT4/ILC
//------------------
#define ILC					0

// Composer defines
#define DIR_DOWN			0
#define DIR_UP				1
#define DIR_OPT				2

#define TYPE_TEXT			0
#define TYPE_IMAGE			1

//------------------
//	 VERSION NUMBER USED FOR REGISTRATION
//------------------
#define ID_CURRENTVERSION			0x00000002L

#define		FIRST_KEY_CODE 		 	2
#define		SECOND_KEY_CODE			5
#define		THIRD_KEY_CODE			10
#define		FOURTH_KEY_CODE			12

#define		IDVERSION				14
#define		MACWINFLAG				15

// Serial Number
#define		SERIAL_1				1
#define		SERIAL_2				0
#define		SERIAL_3				4
#define		SERIAL_4				6
#define		SERIAL_5				3

// Product code
#define		PRODUCT_CODE			7

// Number of users
//
#define		USER_1					8
#define		USER_2					13

#define		PERMREFS_OPT			17
#define		PAGELINK				18
#define		REVERSETCL				19
#define		COMPOSERTOOL			9

#define		TIMEBOMBED				16


// Export type (for export button on PageMakeUp palette)
#define PDF_EXPORT 0
#define EPS_EXPORT 1


// Document min/max value for page size and number of page
#define MIN_PAGE     1     // call Utils<ILayoutUtils>()->GetMinNumPages() (in ILayoutUtils.h)
#define MAX_PAGE     9999  // call Utils<ILayoutUtils>()->GetMaxNumPages() (in ILayoutUtils.h)
#define MIN_WIDTH    12    // call Utils<ILayoutUtils>()->GetMinPageWidth() (in ILayoutUtils.h)
#define MIN_HEIGHT   12    // call Utils<ILayoutUtils>()->GetMinPageLength() (in ILayoutUtils.h)
#define MAX_WIDTH    15552 // call Utils<ILayoutUtils>()->GetMaxPageWidth() (in ILayoutUtils.h)
#define MAX_HEIGHT   15552 // call Utils<ILayoutUtils>()->GetMaxPageLength() (in ILayoutUtils.h)

// Defines used for menus and for the handling of the [ES] command
	
#define	MAC_COLOR	  		1
#define	MAC_BW				2
#define	MAC_DCS				3
#define	MAC_DCS_2			4
#define	PC_COLOR			5
#define	PC_BW				6
#define	PC_DCS				7
#define	PC_DCS_2			8

#define	OPI_ALL				1
#define	OPI_NOTIFF			2
#define	OPI_NOTIFFEPS		3

#define	DATA_ASCII			1
#define	DATA_BINARY			2

// CN parameter
#define CCID_RGB      		 'QRGB'
#define CCID_LAB	  		 'QLAB'
#define CCID_CMYK	  		 'QCMK'
#define CCID_FOCOL	  		 'Foc0'
#define CCID_TOYO     		 'Toy0'
#define CCID_DIC      		 'Dic0'
#define CCID_TRUMATCH 		 'Tru0'
#define CCID_PROCESS_U		 'PnPU'
#define CCID_S2P_EURO 		 'PnCE'
#define CCID_PROCESS_C		 'PnPr'
#define CCID_S2P      		 'PnPS'
#define CCID_METAL	  		 'PnMC'
#define CCID_PASTEL_U 		 'PnAU'
#define CCID_PASTEL_C 		 'PnAC'
#define CCID_SOLID_U  		 'PnUn'
#define CCID_SOLID_C  		 'PnCo'
#define CCID_SOLID_M  		 'PnMt'

#define INVALID_PERMREFS	-1 // Invalid PermRefs reference

#define MAX_BLOC			2147483647 //2^(32-1) -1 => valeur max d'un int32
#define MAX_STYLE			2147483647 //2^(32-1) -1 => valeur max d'un int32
#define MIN_COLOR			10 // Minimum reference number of color stocked by the parser
#define MAX_COLOR			256 // Maximum reference number of color stocked by the parser
#define MIN_FONT			1	  //Minimum reference number for a font
#define MAX_FONT			256	  //Maximum reference number for a font
#define MAX_COL				20 // Maximum number of columns in a text frame
#define MIN_LINE			1 //minimum number of line
#define MAX_LINE			50 //maximum number of line
#define MIN_LEAD			-1 //minimum number of leading
#define MAX_LEAD			5000 //maximum number of leading
#define MIN_ALIGN			0 //minimum reference used for alignment
#define MAX_ALIGN			8 //maximum reference used for alignment
#define CURRENT_PAGE		0
#define MIN_BLOC			-2147483648 // Minimum reference number for an item => valeur min d'un int32
#define MIN_FONTSIZE		0.1  //minimum font size authorized
#define MAX_FONTSIZE		1296	//maximum font size authorized
#define MIN_XSCALE			1  //minimum XScale authorized
#define MAX_XSCALE			1000  //maximum XScale authorized
#define MIN_YSCALE			1  //minimum YScale authorized
#define MAX_YSCALE			1000  //maximum YScale authorized
#define MAX_TRACKING		10000  //maximum tracking authorized
#define MIN_TRACKING		-1000 //minimum tracking authorized
#define MIN_BL				-5000	//minimum BaseLine authorized
#define MAX_BL				5000	//maximum BaseLine authorized
#define MIN_TINT			-1		//minimum Tint percentage authorized
#define MAX_TINT			100		//maximum Tint percentage authorized
#define MIN_PARARULE_STROKE	0		//minimum stroke authorized
#define MAX_PARARULE_STROKE	1000	//maximum stroke authorized
#define MIN_OFFSET			-1296	//minimum offset authorized
#define MAX_OFFSET			1296		//maximum offset authorized
#define MIN_INDENT			-8640	//minimum indent authorized
#define MAX_INDENT			8640		//maximum indent authorized
#define MIN_TYPETAB			0		//minimum type tab authorized
#define MAX_TYPETAB			3		//maximum type tab authorized
#define MIN_PAGESTYLE		1		
#define MAX_PAGESTYLE		5
#define MIN_SKEW			-89		//min skew angle authorized
#define MAX_SKEW			89		//max skew angle authorized
#define MIN_ROTATION		-180	//min rotation angle authorized
#define MAX_ROTATION		180		//max rotation angle authorized
#define MIN_GUTTER			0	//min gutter value authorized
#define MAX_GUTTER			8640	//max gutter value authorized
#define MIN_TABLE			0	//min value for a character's table
#define MAX_TABLE			6	//max value for a character's table
#define MIN_CODECHAR		1	//min value in a character's table
#define MAX_CODECHAR		kMaxInt16 //max value in a character's table
#define MIN_HYPHWORD		3			//min length ofthe hyphenate word
#define MAX_HYPHWORD		25	//max length of hyphenate word
#define MIN_HYPHCHAR		1	//min number of character before and after the hyphen
#define MAX_HYPHCHAR		15	//max number of character before and after the hyphen
#define MIN_HYPHLIMIT		0	//min hyphen authorized
#define MAX_HYPHLIMIT		25	//max hyphen authorized
#define MIN_HYPHZONE		0	//min value of the hyphen zone
#define MAX_HYPHZONE		8640	//max value of the hyphen zone
#define MIN_HYPHWD			0	//min percent of word spacind (min, max and desire)
#define MAX_HYPHWD			1000	//max percent of word spacind (min, max and desire)
#define MIN_HYPHCH			-100	//min percent of letter spacind (min, max and desire)
#define MAX_HYPHCH			500 //max percent of letter spacind (min, max and desire)

#define ToJPEGFormat				1	// JPEG preview format in CP Command
#define ToGIFFormat					2	// GIF preview format in CP Command
#define ToTIFFFormat				3	// TIFF preview format in CP Command

#define BASELINE_JPEG_ENCODING		0 
#define PROGRESSIVE_JPEG_ENCODING	1	

#define TOP_ALIGNEMENT		0
#define BOTTOM_ALIGNEMENT	1
#define TEXT_ALIGNEMENT		2

// SpÈcial Gedimat : Balisage automatique 
#define AUTO_TAGGING_EXPRESSION L"(\\d{8}\\s+\\d+,*\\d*\\s*Ä)" // Expression rÈguliËre -> le balisage automatique du couple (Reference, Prix)
#define AUTO_TAGGING_EXPRESSION2 L"(\\d{8})"					  // Expression rÈguliËre -> le balisage automatique du couple (Reference, Prix)
#define REFERENCE_LENGTH 8 

// Special ILC/POINT4
#define AUTO_TAGGING_EXPRESSION_ILC L"(\\d{3,8})\\s+(\\d+,\\d+)\\s+(\\d+,\\d+)?"
// V2 : Pour eviter, dans le cas d'un couple ref/prix sans prix secondaire mais avec a la ligne un libelle de produit de la forme
// (\\d+,\\d+) (ex : "4,2 m"), qu'on ramasse ce libelle en considerant que c'est le prix secondaire.
#define AUTO_TAGGING_EXPRESSION_ILC_V2 L"(\\d{3,8})\\s+(0,00|1,11)\\s+(0,00|1,11)?"

#endif // __GlobalDefs_h__
