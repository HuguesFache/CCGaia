/*
//	File:	Textes.h
//
//	Date:	24-Sep-2003
//
//  Author : Nicolas Metaye
//
*/


#ifndef __Textes_h__
#define __Textes_h__

class ITCLParser;

#include "PMUTypes.h"

/** Func_TD
	Implement the TD command
*/
class Func_TD
{
public :

	/**
		Constructor
		Read TD command parameters and call IDCall_TD
		@param IN parser : TCL parser
	*/
	Func_TD(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TD();

private :
		
	int32 refColor; // reference of the color


	/**
		Set the color attribute in the attributeBossList
	*/
	void IDCall_TD(ITCLParser *  parser);
};


/** Func_TC
	Implement the SN command
*/
class Func_TC
{
public :

	/**
		Constructor
		Read TC command parameters and call IDCall_TC
		@param IN parser : TCL parser
	*/
	Func_TC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TC();

private :
		
	PMReal fontSize; // value of the font size


	/**
		Set the color attribute in the attributeBossList
	*/
	void IDCall_TC(ITCLParser *  parser);
};


/** Func_TH
	Implement the SN command
*/
class Func_TH
{
public :

	/**
		Constructor
		Read TH command parameters and call IDCall_TH
		@param IN parser : TCL parser
	*/
	Func_TH(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TH();

private :
		
	bool8 flag ;
	PMReal scale; // value of the XScale


	/**
		Set the XScale attribute in the attributeBossList
	*/
	void IDCall_TH(ITCLParser *  parser);
};


/** Func_TV
	Implement the SN command
*/
class Func_TV
{
public :

	/**
		Constructor
		Read TV command parameters and call IDCall_TV
		@param IN parser : TCL parser
	*/
	Func_TV(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TV();

private :
		
	bool8 flag ;
	PMReal scale; // value of the YScale


	/**
		Set the YScale attribute in the attributeBossList
	*/
	void IDCall_TV(ITCLParser *  parser);
};


/** Func_TA
	Implement the TA command
*/
class Func_TA
{
public :

	/**
		Constructor
		Read TA command parameters and call IDCall_TA
		@param IN parser : TCL parser
	*/
	Func_TA(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TA();

private :
		
	PMReal tracking ; //value of the trackKern attribute

	/**
		Set the trackKern attribute in the attributeBossList
	*/
	void IDCall_TA(ITCLParser *  parser);
};


/** Func_TL
	Implement the TL command
*/
class Func_TL
{
public :

	/**
		Constructor
		Read TL command parameters and call IDCall_TL
		@param IN parser : TCL parser
	*/
	Func_TL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TL();

private :
		
	PMReal baseLine ; //value of the BaseLine Shift attribute

	/**
		Set the BaseLine Shift attribute in the attributeBossList
	*/
	void IDCall_TL(ITCLParser *  parser);
};


/** Func_TO
	Implement the TOcommand
*/
class Func_TO
{
public :

	/**
		Constructor
		Read TO command parameters and call IDCall_TT
		@param IN parser : TCL parser
	*/
	Func_TO(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TO();

private :
		
	bool8 overPrintBool ; //value of the OverPrint attribute

	/**
		Set the Tint attribute in the attributeBossList
	*/
	void IDCall_TO(ITCLParser *  parser);
};

/** Func_TT
	Implement the TT command
*/
class Func_TT
{
public :

	/**
		Constructor
		Read TT command parameters and call IDCall_TT
		@param IN parser : TCL parser
	*/
	Func_TT(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TT();

private :
		
	PMReal trame ; //value of the Tint attribute

	/**
		Set the Tint attribute in the attributeBossList
	*/
	void IDCall_TT(ITCLParser *  parser);
};


/** Func_TN
	Implement the TN command
*/
class Func_TN
{
public :

	/**
		Constructor
		Read TN command parameters and call IDCall_TN
		@param IN parser : TCL parser
	*/
	Func_TN(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TN();

private :
		
	PMString fontName ; //font name attribute

	/**
		Set the font attribute in the attributeBossList
	*/
	void IDCall_TN(ITCLParser *  parser);
};


/** Func_TS
	Implement the TS command
*/
class Func_TS
{
public :

	/**
		Constructor
		Read TS command parameters and call IDCall_TS
		@param IN parser : TCL parser
	*/
	Func_TS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TS();

private :
		
	int32 value ; //value that we have to analyse

	/**
		Set the value attribute in the attributeBossList
	*/
	void IDCall_TS(ITCLParser *  parser);
};


/** Func_TZ
	Implement the TZ command
*/
class Func_TZ
{
public :

	/**
		Constructor
		Read TZ command parameters and call IDCall_TZ
		@param IN parser : TCL parser
	*/
	Func_TZ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TZ();

private :

	/**
		Set the noBreak attribute at true in the attributeBossList
	*/
	void IDCall_TZ(ITCLParser *  parser);
};


/** Func_TI
	Implement the TI command
*/
class Func_TI
{
public :

	/**
		Constructor
		Read TI command parameters and call IDCall_TI
		@param IN parser : TCL parser
	*/
	Func_TI(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TI();

private :

	/**
		Set the noBreak attribute at false in the attributeBossList
	*/
	void IDCall_TI(ITCLParser *  parser);
};


/** Func_TP
	Implement the TP command
*/
class Func_TP
{
public :

	/**
		Constructor
		Read TP command parameters and call IDCall_TP
		@param IN parser : TCL parser
	*/
	Func_TP(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TP();

private :
		
	int32 refFont ; //ref font

	/**
		Set the font attribute in the attributeBossList
	*/
	void IDCall_TP(ITCLParser *  parser);
};


/** Func_TB
	Implement the TB command
*/
class Func_TB
{
public :

	/**
		Constructor
		Read TB command parameters and call IDCall_TB
		@param IN parser : TCL parser
	*/
	Func_TB(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TB();

private :
		

	/**
		Insert the tabulation character
	*/
	void IDCall_TB(ITCLParser *  parser);
};


/** Func_LF
	Implement the LF command
*/
class Func_LF
{
public :

	/**
		Constructor
		Read LF command parameters and call IDCall_LF
		@param IN parser : TCL parser
	*/
	Func_LF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_LF();

private :
		

	/**
		Insert the newLine character
	*/
	void IDCall_LF(ITCLParser *  parser);
};



/** Func_EC
	Implement the EC command
*/
class Func_EC
{
public :

	/**
		Constructor
		Read EC command parameters and call IDCall_EC
		@param IN parser : TCL parser
	*/
	Func_EC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_EC();

private :
		

	/**
		Insert cadratin space
	*/
	void IDCall_EC(ITCLParser *  parser);
};


/** Func_ED
	Implement the ED command
*/
class Func_ED
{
public :

	/**
		Constructor
		Read ED command parameters and call IDCall_ED
		@param IN parser : TCL parser
	*/
	Func_ED(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_ED();

private :
		

	/**
		Insert half cadratin space
	*/
	void IDCall_ED(ITCLParser *  parser);
};

/** Func_EF
	Implement the EF command
*/
class Func_EF
{
public :

	/**
		Constructor
		Read EF command parameters and call IDCall_EF
		@param IN parser : TCL parser
	*/
	Func_EF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_EF();

private :
		

	/**
		Insert fine space
	*/
	void IDCall_EF(ITCLParser *  parser);
};


/** Func_EP
	Implement the EP command
*/
class Func_EP
{
public :

	/**
		Constructor
		Read EP command parameters and call IDCall_EP
		@param IN parser : TCL parser
	*/
	Func_EP(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_EP();

private :
		

	/**
		Insert point space
	*/
	void IDCall_EP(ITCLParser *  parser);
};

/** Func_II
	Implement the II command
*/
class Func_II
{
public :

	/**
		Constructor
		Read II command parameters and call IDCall_II
		@param IN parser : TCL parser
	*/
	Func_II(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_II();

private :
		

	/**
		Insert IndentToHere character
	*/
	void IDCall_II(ITCLParser *  parser);
};

/** Func_TF
	Implement the TF command
*/
class Func_TF
{
public :

	/**
		Constructor
		Read TF command parameters and call IDCall_TF
		@param IN parser : TCL parser
	*/
	Func_TF(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TF();

private :
		

	/**
		Insert IndentToHere character
	*/
	void IDCall_TF(ITCLParser *  parser);
};

/** Func_TK
	Implement the TK command
*/
class Func_TK
{
public :

	/**
		Constructor
		Read TK command parameters and call IDCall_TK
		@param IN parser : TCL parser
	*/
	Func_TK(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TK();

private :
		
	PMReal value;
	bool8 autoKern;

	/**
		add a column break character
	*/
	void IDCall_TK(ITCLParser *  parser);
};

/** Func_NB
	Implement the NB command
*/
class Func_NB
{
public :

	/**
		Constructor
		Read NB command parameters and call IDCall_NB
		@param IN parser : TCL parser
	*/
	Func_NB(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_NB();

private :

	/**
		add a frame break character
	*/
	void IDCall_NB(ITCLParser *  parser);
};

/** Func_TE
	Implement the TE command
*/
class Func_TE
{
public :

	/**
		Constructor
		Read TE command parameters and call IDCall_TE
		@param IN parser : TCL parser
	*/
	Func_TE(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TE();

private :

	/**
		add a right indent tab character
	*/
	void IDCall_TE(ITCLParser *  parser);
};

/** Func_TG
	Implement the TG command
*/
class Func_TG
{
public :

	/**
		Constructor
		Read TG command parameters and call IDCall_TG
		@param IN parser : TCL parser
	*/
	Func_TG(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TG();

private :

	int32 value ;

	/**
		Set a specific attribute
	*/
	void IDCall_TG(ITCLParser *  parser);
};

/** Func_TJ
	Implement the TJ command
*/
class Func_TJ
{
public :

	/**
		Constructor
		Read TJ command parameters and call IDCall_TJ
		@param IN parser : TCL parser
	*/
	Func_TJ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TJ();

private :

	int32 value ;

	/**
		Reset a specific attribute
	*/
	void IDCall_TJ(ITCLParser *  parser);
};

/** Func_CL
	Implement the CL command
*/
class Func_CL
{
public :

	/**
		Constructor
		Read CL command parameters and call IDCall_CL
		@param IN parser : TCL parser
	*/
	Func_CL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CL();

private :

	int32 language;

	/**
		Set the composition language
	*/
	void IDCall_CL(ITCLParser *  parser);
};

/** Func_TM
	Implement the TM command
*/
class Func_TM
{
public :

	/**
		Constructor
		Read TM command parameters and call IDCall_TM
		@param IN parser : TCL parser
	*/
	Func_TM(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TM() {};

private :

	PMString fontStyle;

	/**
		Set the font style
	*/
	void IDCall_TM(ITCLParser *  parser);
};

/** Func_TY
	Implement the TY command
*/
class Func_TY
{
public :

	/**
		Constructor
		Read TY command parameters and call IDCall_TY
		@param IN parser : TCL parser
	*/
	Func_TY(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TY() {};

private :
		
	int32 method; // reference of the color


	/**
		Set pair kerning method in the attributeBossList
	*/
	void IDCall_TY(ITCLParser *  parser);
};

/** Func_TStar
	Implement the T* command
*/
class Func_TStar
{
public :

	/**
		Constructor
		Read T* command parameters and call IDCall_TStar
		@param IN parser : TCL parser
	*/
	Func_TStar(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_TStar() {};

private :
		
	TCLRef refBloc; // Reference of the frame the text on path will be added to
	PMReal startPos, endPos; // Text start and end positions

	// TOP options
	int8 txtAlignType, pathAlignType, effectType;
	int16 overlapOffset;
	bool16 flip;

	/**
		Add Text On Path to a bloc
	*/
	void IDCall_TStar(ITCLParser *  parser);
};

/** Func_NT
 Implement the NT command
 */
class Func_NT
	{
		public :
		
		/**
		 Constructor
		 Read NT command parameters and call IDCall_NT
		 @param IN parser : TCL parser
		 */
		Func_NT(ITCLParser *  parser);
		
		/**
		 Destructor
		 */
		~Func_NT();
		
		private :
		
		int32 value ; //value that we have to analyse
		TCLRef refBloc ; //ref du bloc
		
		/**
		 Set the value attribute in the attributeBossList
		 */
		void IDCall_NT(ITCLParser *  parser);
	};



#endif //__Textes_h__