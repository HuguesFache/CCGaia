/*
//	File:	Divers.h
//
//	Date:	24-Apr-2003
//
//  Author : Trias
//
*/


#ifndef __Divers_h__
#define __Divers_h__

class ITCLParser;

#include "PMUTypes.h"
#include "Blocs.h"

#include "IXPSetDropShadowCmdData.h"
#include "IXPSetBasicAttributesCmdData.h"
#include "IXPSetVignetteCmdData.h"
#include "IGeometry.h"
#include "TransformUtils.h"

class ITCLReader;
class IItemTransform;
class IItemManager;
class IPageList;
class IFrameList;
class IHierarchy;
class IDataLink;

/** Func_DU
	Implement the DU command
*/
class Func_DU
{
public :

	/**
		Constructor
		Read DU command parameters and call IDCall_DU
		@param IN parser : TCL parser
	*/
	Func_DU(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DU();

private :
		
	uchar Unite; // Default unit to set

	/**
		Set the default unit
	*/
	void IDCall_DU(ITCLReader * reader);
};


/** Func_AN
	Implement the AN command
*/
class Func_AN
{
public :

	/**
		Constructor
		Read AN command parameters and call IDCall_AN
		@param IN parser : TCL parser
	*/
	Func_AN(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AN();

private :


	/**
		insert the auto page number character
	*/
	void IDCall_AN(ITCLParser *  parser);
};


/** Func_CS
	Implement the CS command
*/
class Func_CS
{
public :

	/**
		Constructor
		Read CS command parameters and call IDCall_CS
		@param IN parser : TCL parser
	*/
	Func_CS(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CS();

private :

	int32 table;

	/**
		set the table of character
	*/
	void IDCall_CS(ITCLParser *  parser);
};


/** Func_CC
	Implement the CC command
*/
class Func_CC
{
public :

	/**
		Constructor
		Read CC command parameters and call IDCall_CC
		@param IN parser : TCL parser
	*/
	Func_CC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CC();

private :

	int32 code;

	/**
		set the table of character
	*/
	void IDCall_CC(ITCLParser *  parser);
};


/** Func_CM
	Implement the CM command
*/
class Func_CM
{
public :

	/**
		Constructor
		Read CM command parameters and call IDCall_CM
		@param IN parser : TCL parser
	*/
	Func_CM(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_CM();

};

/** Func_DT
	Implement the DT command
*/
class Func_DT
{
public :

	/**
		Constructor
		Read DT command parameters and call IDCall_DT
		@param IN parser : TCL parser
	*/
	Func_DT(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DT();

private :

	PMString msg;

	/**
		set the table of character
	*/
	void IDCall_DT(ITCLParser *  parser);
};


/** Func_EV
	Implement the EV command
*/
class Func_EV
{
public :

	/**
		Constructor
		Read EV command parameters and call IDCall_EV
		@param IN parser : TCL parser
	*/
	Func_EV(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_EV();

private :

	TCLRef first,last;

	PMReal minSpace, maxSpace, maxDepth ;

	bool8 fillingFrame ;



	/**
		balance the frames vertically
	*/
	void IDCall_EV(ITCLParser *  parser);
};


/** Func_ET
	Implement the ET command
*/
class Func_ET
{
public :

	/**
		Constructor
		Read ET command parameters and call IDCall_ET
		@param IN parser : TCL parser
	*/
	Func_ET(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_ET();

private :

	int16 startPage, endPage;

	bool8 eraseExisting, checkMissingPict, updateMissingPict;

	/**
		Export doc to an EPS file 
	*/
	void IDCall_ET(ITCLParser *  parser);

	bool16 HandleItem(IHierarchy * itemHier, ITCLParser *  parser);
};


/** Func_EI
	Implement the EI command
*/
class Func_EI
{
public :

	/**
		Constructor
		Read EI command parameters and call IDCall_EI
		@param IN parser : TCL parser
	*/
	Func_EI(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_EI();

private :

	PMString fileName, fileExt;
	
	bool8 readerSpread, bleed, opiReplace, omitPDF, omitEPS, omitBmp;
	
	PMReal bTop, bBottom, bInside, bOutside;
	
	int8 format, psLevel, colorSpace, preview, bmpSampling, includeFonts;

	/**
		Set EPS export preferences 
	*/
	void IDCall_EI(ITCLParser *  parser);
};

/** Func_EX
 Implement the EX command
 */
class Func_EX
{
	public :
	
	/**
	 Constructor
	 Read EX command parameters and call IDCall_EX
	 @param IN parser : TCL parser
	 */
	Func_EX(ITCLParser *  parser);
	
	/**
	 Destructor
	 */
	~Func_EX();
	
	private :
	
	PMString fileName;
	bool8 eraseExisting;
	
	/**
	 Export doc to an IDML file 
	 */
	void IDCall_EX(ITCLParser *  parser);
};

/** Func_JV
	Implement the JV command
*/
class Func_JV
{
public :

	/**
		Constructor
		Read JV command parameters and call IDCall_JV
		@param IN parser : TCL parser
	*/
	Func_JV(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_JV();

private :
	
	TCLRef refBloc;
	bool8 justifyVerticaly, justifyHorizontaly, leftJustify, topJustify, allowOutOfSpread;
	PMReal maxHor, maxVer;
	
	/**
		Resize text frame so that it's not overset 
	*/
	void IDCall_JV(ITCLParser *  parser);

	void IDCall_JV_Image(ITCLParser * parser, bool8 justifyVerticaly, bool8 justifyHorizontaly, PMReal maxHor, PMReal maxVer);
};

/** Func_XX
	Implement the XX command
*/
class Func_XX
{
public :

	/**
		Constructor
		Read XX command parameters and call IDCall_XX
		@param IN parser : TCL parser
	*/
	Func_XX(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_XX();

private :
	
	TCLRef refBloc;
	
	/**
		Delete all text in text frame
	*/
	void IDCall_XX(ITCLParser *  parser);
};

/** Func_XC
	Implement the XC command
*/
class Func_XC
{
public :

	/**
		Constructor
		Read XC command parameters and call IDCall_XC
		@param IN parser : TCL parser
	*/
	Func_XC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_XC();

private :
	
	TCLRef refBlocSrc, refBlocDest;
	bool8 deleteText;
	
	/**
		Delete and copy all text in source frame at the end of dest frame
	*/
	void IDCall_XC(ITCLParser *  parser);
};

/** Func_AU
	Implement the AU command (do nothing, only for Quark compatibility)
*/
class Func_AU
{
public :

	/**
		Constructor
		Read AU command parameters
		@param IN parser : TCL parser
	*/
	Func_AU(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AU();

private :
		
	bool8 useNewConvert;

};

/** Func_AP
	Implement the AP command (do nothing, only for Quark compatibility)
*/
class Func_AP
{
public :

	/**
		Constructor
		Read AP command parameters 
		@param IN parser : TCL parser
	*/
	Func_AP(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AP();

private :
		
	bool8 resetMode;

};

/** Func_AL
	Implement the AL command (do nothing, only for Quark compatibility)
*/
class Func_AL
{
public :

	/**
		Constructor
		Read AL command parameters
		@param IN parser : TCL parser
	*/
	Func_AL(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AL();

private :
		
	int8 modeAlign;

};

/** Func_IC
	Implement the IC command (do nothing, only for Quark compatibility)
*/
class Func_IC
{
public :

	/**
		Constructor
		Read IC command parameters
		@param IN parser : TCL parser
	*/
	Func_IC(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_IC();

private :

	PMString imagePath;

	/**
		Set the picture path for importing 
	*/
	void IDCall_IC(ITCLParser * parser);

};

/** Func_EH
	Implement the EH command (do nothing, only for Quark compatibility)
*/
class Func_EH
{
public :

	/**
		Constructor
		Read EH command parameters
		@param IN parser : TCL parser
	*/
	Func_EH(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_EH();

private :

	TCLRef refBloc1, refBloc2;
	int16 mode;
	PMReal offset;

	/**
		balance the frames horizontally
	*/
	void IDCall_EH(ITCLParser * parser);

};

/** Func_XD
	Implement the XD command
*/
class Func_XD
{
public :

	/**
		Constructor
		Read XD command parameters
		@param IN parser : TCL parser
	*/
	Func_XD(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_XD();

private :

	TCLRef refBloc;
	int32 id, type, allowHoles, invert, index, flags;
	PMReal minSize, outTop, outLeft, outBottom, outRight, maxError, whiteness;

	/**
		
	*/
	void IDCall_XD(ITCLParser * parser);

};

/** Func_XH
	Implement the XH command
*/
class Func_XH
{
public :

	/**
		Constructor
		Read XH command parameters
		@param IN parser : TCL parser
	*/
	Func_XH(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_XH();

private :

	TCLRef refBloc;
	int32 id, type, allowHoles, invert, index, flags;
	PMReal minSize, outTop, outLeft, outBottom, outRight, maxError, whiteness;

	/**
		
	*/
	void IDCall_XH(ITCLParser * parser);

};

/** Func_XR
	Implement the XR command
*/
class Func_XR
{
public :

	/**
		Constructor
		Read XR command parameters
		@param IN parser : TCL parser
	*/
	Func_XR(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_XR() {};

private :

	TCLRef refBloc;
	bool8 runaroundsides;
};

/** Func_ER
	Implement the ER command
*/
class Func_ER
{
public :

	/**
		Constructor
		Read ER command parameters
		@param IN parser : TCL parser
	*/
	Func_ER(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_ER() {};

private :

	PMReal bTop, bBottom, bInside, bOutside;


	/**
		Set bleed preferences for EPS export
	*/
	void IDCall_ER(ITCLParser * parser);
};

/** Func_RB
	Implement the RB command
*/
class Func_RB
{
public :

	/**
		Constructor
		Read RB command parameters
		@param IN parser : TCL parser
	*/
	Func_RB(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_RB() {};

private :

	TCLRef refBloc;
	PMReal xScale, yScale, minSize, maxSize, minLeading, maxLeading;

	/**
		Resize a bloc like command + clic (Quark)
	*/
	void IDCall_RB(ITCLParser * parser);
};

/** Func_EB
	Implement the EB command
*/
class Func_EB
{
public :

	/**
		Constructor
		Read EB command parameters
		@param IN parser : TCL parser
	*/
	Func_EB(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_EB() {};	

private :

	int8 type;
	TCLRef refBloc;
	bool16 group;
	K2Vector<TCLRef> frameList;

	/**
		Equilibrate space between frames vertically or horizontally
	*/
	void IDCall_EB(ITCLParser * parser);
};


/** Func_IP
	Implement the IP command
*/
class Func_IP
{
public :

	/**
		Constructor
		Read IP command parameters
		@param IN parser : TCL parser
	*/
	Func_IP(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_IP() {};

private :

	int8 type;
	TCLRef refBloc;
	PMReal xScale, yScale;

	/**
		Move and scale a picture within an image frame
	*/
	void IDCall_IP(ITCLParser * parser);
};

/** Func_SE
	Implement the SE command
*/
class Func_SE
{
public :

	/**
		Constructor
		Read SE command parameters
		@param IN parser : TCL parser
	*/
	Func_SE(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_SE() {};

private :

	TCLRef refBloc;

	DropShadowData data;

	/**
		Move and scale a picture within an image frame
	*/
	void IDCall_SE(ITCLParser * parser);
};


/** Func_AR
	Implement the AR command
*/
class Func_AR
{
public :

	/**
		Constructor
		Read AR command parameters
		@param IN parser : TCL parser
	*/
	Func_AR(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AR() {};

private :

	PMString tclFilename;

	/**
		Move and scale a picture within an image frame
	*/
	void IDCall_AR(ITCLParser * parser);
};

/** Func_DZ
	Implement the DZ command
*/
class Func_DZ
{
public :

	/**
		Constructor
		Read DZ command parameters
		@param IN parser : TCL parser
	*/
	Func_DZ(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DZ() {};

private :

	PMString pdfFilename, bookExportStyle;
	int32 startPage, endPage;
	bool16 eraseExisting;
	bool16 exportBook;

	/**
		Export document to pdf
	*/
	void IDCall_DZ(ITCLParser * parser);
};

/** Func_DDollar
	Implement the D$ command
*/
class Func_DDollar
{
public :

	/**
		Constructor
		Read D$ command parameters
		@param IN parser : TCL parser
	*/
	Func_DDollar(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_DDollar() {};

private :

	PMString pdfStyleName;

	/**
		Save pdf export style into the document
	*/
	void IDCall_DDollar(ITCLParser * parser);
};

/** Func_AG
	Implement the AG command
*/
class Func_AG
{
public :

	/**
		Constructor
		Read AG command parameters
		@param IN parser : TCL parser
	*/
	Func_AG(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_AG() {};

private :

	TCLRef refBloc;
	bool16 returnChar, groupage;
	K2Vector<TCLRef> frameList;

	/**
		Save pdf export style into the document
	*/
	void IDCall_AG(ITCLParser * parser);
};

class Func_OE
{
public :

	/**
		Constructor
		Read OE command parameters
		@param IN parser : TCL parser
	*/
	Func_OE(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_OE() {};

private :

	TCLRef refBloc;

	BasicAttributesData  data;

	/** Apply transparency effect
	*/
	void IDCall_OE(ITCLParser * parser);
};

class Func_FA
{
	public :

	/**
		Constructor
		Read FA command parameters
		@param IN parser : TCL parser
	*/
	Func_FA(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_FA() {};

private :

	TCLRef refBloc;

	VignetteData data;

	/** Apply feather effect
	*/
	void IDCall_FA(ITCLParser * parser);
};

class Func_GO
{
	public :

	/**
		Constructor
		Read GO command parameters
		@param IN parser : TCL parser
	*/
	Func_GO(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_GO() {};

private :

	TCLRef refBloc;

	bool16 isFillAttr;
	PMReal angle, length, radius;
	PMPoint center;

	/** Apply feather effect
	*/
	void IDCall_GO(ITCLParser * parser);
};


class Func_WW
{
public :

	/**
		Constructor
		Read WW command parameters
		@param IN parser : TCL parser
	*/
	Func_WW(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_WW();

	static ErrorCode GetMasterTextFrames(IHierarchy * masterItem, UIDRef& leftTxtFrame, UIDRef& rightTxtFrame, PMString& error);

private :

	int16 startPage;
	TCLRef refBlocMaster, refOverridenBloc;
	bool16 autoPlace;

	/** Override an item from a master one
		Can be also used paired with FW to auto placement
	*/
	void IDCall_WW(ITCLParser * parser);
};

class Func_FW
{
public :

	/**
		Constructor
		Read WW command parameters
		@param IN parser : TCL parser
	*/
	Func_FW(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_FW() {}


private :

	static TCLRef refBloc;
	friend class Func_WW; //Allow access to refBloc to class Func_WW

	void IDCall_FW(ITCLParser * parser);

};

class Func_EE
{
public :

	/**
		Constructor
		Read EE command parameters
		@param IN parser : TCL parser
	*/
	Func_EE(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_EE() {}


private :

	PMString filename;
	K2Vector<TCLRef> itemsToExport;

	/**
		Export a snippet
	*/
	void IDCall_EE(ITCLParser * parser);

};

class Func_IE
{
public :
	/**
		Constructor
		Read IE command parameters
		@param IN parser : TCL parser
	*/
	Func_IE(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_IE() {}


private :

	PMString filename;
	int16 page;
	TCLRef ref;
	bool8 relative;
	int8 alignBloc;
	PMReal hPos, vPos;

	/**
		Import a snippet
	*/
	void IDCall_IE(ITCLParser * parser);

};


class Func_RK
{
public :
	/**
		Constructor
		Read RK command parameters
		@param IN parser : TCL parser
	*/
	Func_RK(ITCLParser *  parser);

	/**
		Destructor
	*/
	~Func_RK() {}


private :

	PMString imageDirPath;
	int8 relinkType;

	/**
		Relink images
	*/
	void IDCall_RK(ITCLParser * parser);

	ErrorCode UpdateLink(IDataBase * db, IDataLink * link, const IDFile& imageDir, ITCLParser * parser);

};

/** Func_AI
	Implement the AI command
*/
class Func_AI
{
public :

	/**
		Constructor
		Read AI command parameters and do nothing
		@param IN parser : TCL parser
	*/
	Func_AI(ITCLParser *  parser); // utilis￩ ￠ l'export pour renvoyer les dimensions et la r￩solution d'une image

	/**
		Destructor
	*/
	~Func_AI() {}

};

/** Func_CDiese
	Implement the C# command
*/
class Func_CDiese
{
public :
	
	/**
		Constructor
		Read Func_BDiese command parameters
		@param IN parser : TCL parser
	*/
	Func_CDiese(ITCLParser *  parser);

	Func_CDiese() {}; // Default constructor, do nothing

	/**
		Destructor
	*/
	~Func_CDiese() {};

	static void GetPositionRelativeToColumn(IDocument * doc, int32 page, int32 column, PMReal offset, PMPoint& leftTop, PMReal& width, UID& ownerSpreadLayer);
	static void GetPositionRelativeToItem(IDocument * doc, UID itemUID, PMReal offset, bool16 includeStroke, PMPoint& leftTop, PMReal& width, UID& ownerSpreadLayer);

protected :

	TCLRef refBloc;
	bool16 justify, includeStroke;

	/*
		Check whether a frame fits in its owning column. 
		If so, do nothing.
		If no, move it to the next column or the next page.

		To be used after B# calls.
	*/
	void IDCall_CDiese(ITCLParser *  parser);

	// Allow sort of items according to their height (from top to bottom)
	struct CompareTop
	{ 
		bool operator ()(const UIDRef& item1, const UIDRef& item2) const 
		{ 
			InterfacePtr<IGeometry> itemGeo1 (item1, UseDefaultIID());
			InterfacePtr<IGeometry> itemGeo2 (item2, UseDefaultIID());
			
			PMPoint leftTop1 = itemGeo1->GetStrokeBoundingBox().LeftTop();
			PMPoint leftTop2 = itemGeo2->GetStrokeBoundingBox().LeftTop();
				
			::TransformInnerPointToPasteboard(itemGeo1,&leftTop1);
			::TransformInnerPointToPasteboard(itemGeo2,&leftTop2);

			return leftTop1.Y() < leftTop2.Y();
		} 
	};

};

/** Func_XA
	Implement the XA command
*/
class Func_XA
{
public :

	/**
		Constructor
		Read XA command parameters and do nothing
		@param IN parser : TCL parser
	*/
	Func_XA(ITCLParser *  parser); // utilis￩ ￠ l'export pour renvoyer les dimensions et la r￩solution d'une image

	/**
		Destructor
	*/
	~Func_XA() {}

private :

	/**
		Command dedicated to XPA
	*/
	void IDCall_XA(ITCLParser * parser);

};

#endif //__Divers_h__