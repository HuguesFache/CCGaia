/*
//	File:	ITCLParser.h
//
//  Author: Trias
//
//	Date:	11-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __ITCLParser_h__
#define __ITCLParser_h__

#include "PrsID.h"
#include "IUIDListData.h"

#include "TableTypes.h"

#include "PMUTypes.h"

class IPMUnknown;
class IPMStream;
class ITCLReader;
class IStyleHelper;
class ITxtAttrHelper;
class IDocHelper;
class IItemManager;
class IItemTransform;
class IPageHelper;
class ITextFrameHelper;
class AttributeBossList;
class ITextModel;
class IDocument;
class ITableModel;

class PrsParserThread;

/** ITCLParser
	Parse TCL files using kTCLReaderBoss and executes TCL commands
*/
class ITCLParser : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_ITCLPARSER };

		/** 
			Accessor method for ITCLReader interface
		*/
		virtual ITCLReader * QueryTCLReader() =0;

		/**  
			Query methods to get helper interfaces
		*/
		virtual IDocHelper * QueryDocHelper() =0;
		virtual IStyleHelper * QueryStyleHelper() =0;
		virtual ITxtAttrHelper * QueryTxtAttrHelper() =0;
		virtual IItemManager * QueryItemManager() =0;
		virtual IItemTransform * QueryItemTransform() =0;
		virtual ITextFrameHelper * QueryTextFrameHelper() =0;
		virtual IPageHelper * QueryPageHelper() =0;

		/**
			Save the item list for the current front document
		*/
		virtual ErrorCode SaveItemList() =0;
		
		/** 
			Set a new TCL file to read
		*/
		
		virtual bool8 SetInputFile(IDFile file) =0;


		/** 
			set the character's table used by the transcoder
		*/

		virtual void setTranscodeTable(int32 table) =0;

		/** 
			Get a color in the color list
			@param IN index : index of the color in the list
			Return the color for this index
		*/
		virtual sColor getColorList(int16 index) =0;

		/**
			Set a color in the color list
			@param IN index : index of the color
			@param IN col : the color
		*/	
		virtual void setColorList(int16 index, sColor col) =0;

		/**
			Get an item in the item list
			@param IN ref : TCL reference of the item
			Return the item for this reference
		*/
		virtual UID getItem(TCLRef ref) =0;

		/** Set an item in the item list
			@param IN ref : TCL reference of the item
			@param IN item : the item
		*/

		virtual void setItem(TCLRef ref, UID item) =0;

		virtual void deleteItem(TCLRef ref) =0 ;

		/**
			Accessor methods for the TCL reference of the current item selectionned
		*/
		virtual TCLRef getCurItem() =0;
		virtual void setCurItem(TCLRef ref, bool16 selectTOP = kFalse) =0;

		/** 
			Accessor method for getting the previous current item's reference
		*/
		virtual TCLRef getPreviousItem() =0;

		/** 
			Accessor methods for the current page number
		*/
		virtual int16 getCurPage() =0;
		virtual void setCurPage(int16 num) =0;

		/**
			Accessor methods for the current document
		*/
		virtual IDocument * getCurDoc() =0;
		virtual IDataBase * getCurDataBase() =0;
		virtual void setCurDoc(IDocument * doc) =0;

		/** 
			Return the number of items that were assigned a TCL reference on parser start
		*/
		virtual int32 getNbItemRegisteredAtBeginning() =0;

		/**
			Accessor methods for the current book
		*/
		virtual void setCurBook(UIDRef book) =0;
		virtual UIDRef getCurBook() =0;

		/**
			Accessor methods for the current table and the current cells in this table
		*/
		virtual UID getCurTable() =0;
		virtual void setCurTable(const PMString& table) =0;
		virtual void setCurCells(InterfacePtr<ITableModel>& tableModel, GridArea targetArea, int32 startRow, int32 startCol, int32 endRow, int32 endCol) =0;
		virtual void setCurCells(const GridArea& area) =0;
		virtual GridArea getCurCells() =0;

		/** Set an table in the table list
			@param IN ref : TCL reference of the table
			@param IN table : the table
		*/
		virtual void setTable(PMString ref, UID table) =0;
		virtual UID getTable(PMString ref) =0;


		/** 
			Get method for the picture importation preferences
			@param OUT useImportPath : true if pictures are stored in directory
			@param OUT import : true if pictures import is allowed
			@param OUT picPath : absolute path of the picture location
			@param OUT textbox : create a text box for missing picture
			@param OUT nonPrinting : true if the box is to be non-printing
			@param OUT colorIndex : fill color to used when picture is missing
			@param OUT colorTint : fill color tint to used when picture is missing
		*/
		virtual void getPicturePrefs(bool8 * useImportPath, bool8 * import, PMString * picPath, bool8 * textbox,
									 bool8 * nonPrinting, int32 * colorIndex, int32 * colorTint) =0;

		/** 
			Set method for the picture importation preferences
			@param IN useImportPath : true if pictures are stored in directory
			@param IN import : true if pictures import is allowed
			@param IN picPath : absolute path of the picture location
			@param IN textbox : create a text box for missing picture
			@param IN nonPrinting : true if the box is to be non-printing
			@param IN colorIndex : fill color to used when picture is missing
			@param IN colorTint : fill color tint to used when picture is missing
		*/
		virtual void setPicturePrefs(bool8 useImportPath, bool8 import, PMString picPath, bool8 textbox,
									 bool8 nonPrinting, int32 colorIndex, int32 colorTint) =0;

		/**
			Accessor methods for the absolute path of the directory where documents should be opened/saved/exported
		*/
		virtual void getSavePrefs(bool8 * usePath, PMString * savePath) =0;
		virtual void setSavePrefs(bool8 usePath, PMString savePath) = 0;
		virtual void getOpenPrefs(bool8 * usePath, PMString * openPath) = 0;
		virtual void setOpenPrefs(bool8 usePath, PMString openPath) = 0;
		virtual void getExportPrefs(PMString * exportPath) = 0;
		virtual void setExportPrefs(PMString exportPath) = 0;

		/**
			Accessor methods to set the StopOnError preference
		*/
		virtual bool8 StopOnError() =0;
		virtual void SetStopOnError(bool8 stop) =0;
		
		virtual bool8 StopDisplay() =0;
		virtual void SetStopDisplay(bool8 stop) =0;
		
		/**
			Start the parsing process
		*/
		virtual int32 LaunchParser() =0;

		//  add a couple int32,string in the style vector
		virtual ErrorCode AddStyle(int32 styleNum, PMString styleName) =0;

		//  get the string name in the string table
		virtual ErrorCode GetStyleName(int32 styleNum,PMString& value) =0;

		//  add a couple int32,string in the police's vector
		virtual ErrorCode AddFont(int32 fontNum, PMString fontName) =0;

		//  add a CJ rule in the CJ's vector
		virtual void AddCJ(PMString name, bool8 hyphenMode, bool8 capitalized, int16 minWord,
						 int16 afterFirst, int16 beforeLast, int16 limit, PMReal hyphenZone, 
						 bool8 lastWord, PMReal wordMin, PMReal wordMax, PMReal wordDes, 
						 PMReal letterMin, PMReal letterMax, PMReal letterDes) =0;

		virtual ErrorCode getCJ(PMString name, bool8& hyphenMode, bool8& capitalized, int16& minWord,
						 int16& afterFirst, int16& beforeLast, int16& limit, PMReal& hyphenZone, 
						 bool8& lastWord, PMReal& wordMin, PMReal& wordMax, PMReal& wordDes, 
						 PMReal& letterMin, PMReal& letterMax, PMReal& letterDes) =0;

		virtual void deleteCJ() =0;

		// Query the current text model and return the textindex of the end of text in the current text frame
		// or in the current cell (must be a unique cell selection), according to the current selection
		virtual	ITextModel * QueryCurrentModelTextIndex(TextIndex& endOfStoryThread) =0;

		//  get the string name in the police's table
		virtual ErrorCode GetFontName(int32 fontNum,PMString& value) =0;

		//get the CurrentBox UIDRef
		virtual ErrorCode getCurBoxUIDRef( UIDRef &theBox)=0;

		// add UNICODE character c to the buffer
		virtual void InsertCharacter(UTF32TextChar c) =0; 

		// log file
		virtual void WriteLogFile(PMString msg) =0;

		// Rename file (DR command)
		virtual void SetNewFileName(PMString newName) =0;

		// PermRefs related methods
		virtual void SetCurrentPermRefs(PermRefStruct ref, TextIndex startPosition, UID story) =0;
		virtual void GetCurrentPermRefs(PermRefStruct& ref, TextIndex& startPosition) =0;
		virtual void ResetCurrentPermRefs() =0;
		virtual K2Vector<UID> getStories(PermRefStruct permRef) =0;
		virtual void AppendPermRefsList(const PermRefVector& permRefsList) =0;		

private :

		/**
			Methods designed for PrsParserThread so that it can yield whenever 
			a character is read or a command is processed
			See comparison between PrsParserThread::RunThread and PrsParser::Launch parser
			implementations
		*/
		friend class PrsParserThread;
		virtual bool16 ProcessTCL(int32& numError) =0;
		virtual void FinalizeProcess() =0;
};

#endif // __ITCLParser_h__
