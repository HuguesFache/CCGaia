/*
//	File:	PrsTCLParser.h
//
//  Author: Trias
//
//	Date:	11-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __PrsTCLParser_h__
#define __PrsTCLParser_h__

// API includes
#include "CPMUnknown.h"

// Project includes
#include "ITCLParser.h"
#include "IDocHelper.h"
#include "IStyleHelper.h"
#include "ITxtAttrHelper.h"
#include "IItemManager.h"
#include "IItemTransform.h"
#include "ITextFrameHelper.h"
#include "IPageHelper.h"
#include "Transcoder.h"
#include "ITableModel.h"

#define BUF_SIZE 1024 // Buffer size for text


/** PrsTCLParser
	Implements the ITCLParser interface
*/
class PrsTCLParser : public CPMUnknown<ITCLParser> 
{

	public :
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PrsTCLParser(IPMUnknown * boss);

		/** 
			Destructor
		*/
		~PrsTCLParser();

		// ---------------------------------------------
		//  Query method to get an ITCLReader interface
		// ---------------------------------------------
		virtual ITCLReader * QueryTCLReader();

		// ----------------------------------------
		//  Query methods to get helper interfaces
		// ----------------------------------------
		virtual IDocHelper * QueryDocHelper();
		virtual IStyleHelper * QueryStyleHelper();
		virtual ITxtAttrHelper * QueryTxtAttrHelper();
		virtual IItemManager * QueryItemManager();
		virtual IItemTransform * QueryItemTransform();
		virtual ITextFrameHelper * QueryTextFrameHelper();
		virtual IPageHelper * QueryPageHelper();

		// ----------------------------------
		//  Accessor methods to parser datas
		// ----------------------------------
		virtual void setTranscodeTable(int32 table);
		virtual sColor getColorList(int16 index);
		virtual void setColorList(int16 index, sColor col);
		virtual UID getItem(TCLRef ref);
		virtual void setItem(TCLRef ref, UID item);
		virtual void deleteItem(TCLRef ref);
		virtual TCLRef getCurItem();
		virtual void setCurItem(TCLRef ref, bool16 selectTOP) ;
		virtual TCLRef getPreviousItem();
		virtual int16 getCurPage();
		virtual void setCurPage(int16 num);
		virtual int32 getNbItemRegisteredAtBeginning();
		virtual IDocument * getCurDoc();
		virtual IDataBase * getCurDataBase();
		virtual void setCurDoc(IDocument * doc);
		virtual UID getCurTable();
		virtual void setCurTable(const PMString& table);
		virtual void setTable(PMString ref, UID table);
		virtual UID getTable(PMString ref);
		virtual void setCurCells(InterfacePtr<ITableModel>& tableModel, GridArea targetArea, int32 startRow, int32 startCol, int32 endRow, int32 endCol);
		virtual void setCurCells(const GridArea& area);
		virtual GridArea getCurCells();
		virtual void setCurBook(UIDRef book);
		virtual UIDRef getCurBook();

		// Query the current text model and return the textindex of the end of text in the current text frame
		// or in the current cell (must be a unique cell selection), according to the current selection
		virtual	ITextModel * QueryCurrentModelTextIndex(TextIndex& endOfStoryThread);

		// ---------------------------------------------
		//  Accessor methods to the parser preferences
		// ---------------------------------------------
		virtual void getPicturePrefs(bool8 * useImportPath, bool8 * import, PMString * picPath, bool8 * textbox,
									 bool8 * nonPrinting, int32 * colorIndex, int32 * colorTint);
		virtual void setPicturePrefs(bool8 useImportPath, bool8 import, PMString picPath, bool8 textbox,
									 bool8 nonPrinting, int32 colorIndex, int32 colorTint);
		virtual void getSavePrefs(bool8 * usePath, PMString * savePath);
		virtual void setSavePrefs(bool8 usePath, PMString savePath);
		virtual void getOpenPrefs(bool8 * usePath, PMString * openPath);
		virtual void setOpenPrefs(bool8 usePath, PMString openPath);
		virtual void getExportPrefs(PMString * exportPath);
		virtual void setExportPrefs(PMString exportPath);

		virtual bool8 StopOnError();
		virtual void SetStopOnError(bool8 stop);
		
		virtual bool8 StopDisplay();
		virtual void SetStopDisplay(bool8 stop);

		// -------------------------------------------
		//  TCL file definition and parser processing
		// -------------------------------------------
		virtual bool8 SetInputFile(IDFile file);
		virtual int32 LaunchParser();

		// ---------------------------------------------------
		//  Initialization of the lists handled by the parser
		// ---------------------------------------------------
		virtual void InitializeColor();
		virtual void InitializeRefList();

		// ------------------------------------------------------
		//  Save the current item list in the frontmost document
		// ------------------------------------------------------
		virtual ErrorCode SaveItemList(); 

		// ------------------------------------------------------
		//  add a couple int32,string in the style's vector
		// ------------------------------------------------------
		virtual ErrorCode AddStyle(int32 styleNum, PMString styleName) ;

		// ------------------------------------------------------
		//  get the string name in the style's table
		// ------------------------------------------------------
		virtual ErrorCode GetStyleName(int32 styleNum,PMString& value) ;

		// ------------------------------------------------------
		//  add a couple int32,string in the font's vector
		// ------------------------------------------------------
		virtual ErrorCode AddFont(int32 fontNum, PMString fontName) ;

		// ------------------------------------------------------
		//  add a CJ rule in the CJ's vector
		// ------------------------------------------------------
		virtual void AddCJ(PMString name, bool8 hyphenMode, bool8 capitalized, int16 minWord,
						 int16 afterFirst, int16 beforeLast, int16 limit, PMReal hyphenZone, 
						 bool8 lastWord, PMReal wordMin, PMReal wordMax, PMReal wordDes, 
						 PMReal letterMin, PMReal letterMax, PMReal letterDes) ;

		virtual ErrorCode getCJ(PMString name, bool8& hyphenMode, bool8& capitalized, int16& minWord,
						 int16& afterFirst, int16& beforeLast, int16& limit, PMReal& hyphenZone, 
						 bool8& lastWord, PMReal& wordMin, PMReal& wordMax, PMReal& wordDes, 
						 PMReal& letterMin, PMReal& letterMax, PMReal& letterDes) ;



		virtual void deleteCJ() ;

		// ------------------------------------------------------
		// PermRefs related methods
		// ------------------------------------------------------
		virtual void SetCurrentPermRefs(PermRefStruct ref, TextIndex startPosition, UID story);
		virtual void GetCurrentPermRefs(PermRefStruct& ref, TextIndex& startPosition);
		virtual void ResetCurrentPermRefs();
		virtual K2Vector<UID> getStories(PermRefStruct permRef);
		virtual void AppendPermRefsList(const PermRefVector& permRefsList);

		// ------------------------------------------------------
		//  get the string name in the font's table
		// ------------------------------------------------------
		virtual ErrorCode GetFontName(int32 fontNum,PMString& value) ;

		//get the CurrentBox UIDRef
		virtual ErrorCode getCurBoxUIDRef( UIDRef &theBox);

		virtual void InsertCharacter(UTF32TextChar c); // add UNICODE character c to the buffer

		virtual void WriteLogFile(PMString msg);

		virtual void SetNewFileName(PMString newName);	

	private :

		PrefRec prefs; // Parser preferences
		RefVector itemList; // TCL referenced items list
		PermRefVector storyList; // PermRefs list

		TableVector tableList; // table list		

		WideString txtBuf; // Buffer for the text between TCL commands
		void FlushBuffer(); // Empty the buffer into the current item if it's a text frame or in the current cell
		
		ITCLReader * reader; // TCLReader for file reading
		Transcoder * trsCoder; // Character transcoder

		// Helper interfaces
		IDocHelper * docHelper;
		IStyleHelper * styleHelper;
		ITxtAttrHelper * txtAttrHelper;
		IItemManager * itemManager;
		IItemTransform * itemTransform;
		ITextFrameHelper * textFrameHelper;
		IPageHelper * pageHelper;
		
		TCLRef curItem; // Current selected item 
		bool16 topSelected; // Indicating whether we are writing text to TOP or not
		TCLRef previousItem; // Previous selected item 
		int16 curPage; // Current page
		UID curTable; // Current table
		GridArea curCells; // Current selected cell(s)

		PermRefStruct curPermRef; // Current PermRefs reference
		TextIndex startPosPermRef;

		int32 nbItemRegistered; // Number of items added to item list at parser initialization
		
		sColor * colorList; // Color list

		infoTbl * theInfoTbl ;
		
		IDocument * curDoc;
		IDataBase * curDB;
		UIDRef curBook;

		IPMStream * logfile; // log file stream

		IDFile tclFile; // TCL file being parsed
		PMString newFileName;

		// Methods designed for parser thread (used by AutoPageLink WebService)
		virtual bool16 ProcessTCL(int32& numError);
		virtual void FinalizeProcess();
		friend class PrsParserThread;
};

#endif // __PrsTCLParser_h__
