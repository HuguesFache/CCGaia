/*
//	File:	PrsTCLParser.cpp
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
#include "GlobalDefs.h"

// General includes
#include "IPMStream.h"
#include "StreamUtil.h"
#include "CAlert.h"
#include "WideString.h"
#include "ILayoutUtils.h"
#include "CoreResTypes.h"
#include "ErrorUtils.h"
#include "FileUtils.h"
#include "FontMgrID.h"
#include "LocaleSetting.h"
#include "SDKLayoutHelper.h"

#include <iostream>

// Interface includes
#include "ISwatchUtils.h"
#include "ISwatchList.h"
#include "ISelectionUtils.h"
#include "ILayoutSelectionSuite.h"
#include "IWorkspace.h"
#include "IDocument.h"
#include "IFrameContentUtils.h"
#include "IFrameUtils.h"
#include "ILayoutTarget.h"
#include "IMultiColumnTextFrame.h"
#include "IFrameList.h"
#include "IHierarchy.h"
#include "IPageItemTypeUtils.h"
#include "IPageList.h"
#include "ISpread.h"
#include "IParserSuite.h"
#include "IClassIDData.h"
#include "IPageItemAdornmentList.h"
#include "IRenderingObject.h"
#include "IPersistUIDData.h"
#include "ITextStoryThreadDict.h"
#include "ITextStoryThread.h"
#include "ITextModelCmds.h"
#include "ITableTextContainer.h"
#include "ITableSelectionSuite.h"
#include "IWideStringData.h"
#include "IIntData.h"
#include "IBook.h"
#include "IBookManager.h"
#include "ICommandMgr.h"
#include "IIdleTask.h"
#include "IMainItemTOPData.h"
#include "TextEditorID.h"

#if !SERVER
#include "ILayoutUIUtils.h"
#else
#include "IApplication.h"
#include "IDocumentList.h"
#include "IErrorList.h"
#endif

// Project includes
#include "PrsTCLParser.h"
#include "ITCLReader.h"
#include "ITCLReferencesList.h"
#include "TCLError.h"
#include "IPageMakeUpPrefs.h"
#include "Transcoder.h"
#include "Blocs.h"
#include "Docs.h"
#include "Divers.h"
#include "Styles.h"
#include "Textes.h"
#include "Paragraphes.h"
#include "Reglages.h"
#include "Pages.h"
#include "Calques.h"
#include "Systeme.h"
#include "Tableaux.h"
#include "XRail.h"
#include "Livres.h"
#include "Hyperlinks.h"
#include "XPage.h"

#if XRAIL
     #include "XRLID.h"
     #include "IXRailPrefsData.h"
#endif


#if PERMREFS
	#include "PermRefs.h"
#endif

#if COMPOSER
	#include "Composer.h"
#endif

#if XREFS
	#include "XRefs.h"
#endif

#include "Log.h"
#include "DLBID.h"
#include "TLBID.h"
#include "PLBID.h"

CREATE_PMINTERFACE(PrsTCLParser,kPrsTCLParserImpl)

/* Constructor
*/
PrsTCLParser::PrsTCLParser(IPMUnknown * boss)
: CPMUnknown<ITCLParser>(boss)
{
	// Create boss objects
	reader = (ITCLReader *) ::CreateObject(kTCLReadWriteBoss,IID_ITCLREADER);

	docHelper = (IDocHelper *) ::CreateObject(kDocHelperBoss,IID_IDOCHELPER);
	
	styleHelper = (IStyleHelper *) ::CreateObject(kTextHelperBoss,IID_ISTYLEHELPER);
	txtAttrHelper = (ITxtAttrHelper *) styleHelper->QueryInterface(IID_ITXTATTRHELPER);

	itemManager = (IItemManager *) ::CreateObject(kFrameHelperBoss,IID_IITEMMANAGER);
	itemTransform = (IItemTransform *) itemManager->QueryInterface(IID_IITEMTRANSFORM);
	textFrameHelper = (ITextFrameHelper *) itemManager->QueryInterface(IID_ITEXTFRAMEHELPER);

	pageHelper = (IPageHelper *) ::CreateObject(kPageHelperBoss,IID_IPAGEHELPER);

	if(reader == nil || docHelper == nil || styleHelper == nil || txtAttrHelper == nil
	   || itemManager == nil || itemTransform == nil || textFrameHelper == nil || pageHelper == nil)
	{
		ASSERT_FAIL("PrsTCLParser::PrsTCLParser -> reader or helper interface nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	trsCoder = new Transcoder(DEFAULT_TABLE);
	colorList = nil;

	tclFile = IDFile();
	newFileName = "";
	
	curDB = nil;
	curDoc = nil;
	curBook = UIDRef::gNull;

	IDocument * openedDoc = nil;
#if !SERVER
	openedDoc = Utils<ILayoutUIUtils>()->GetFrontDocument();
#else
	// In InDesign Server mode, current doc is the latest opened document
	InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
	InterfacePtr<IDocumentList> documentList(application->QueryDocumentList());
	if(documentList->GetDocCount() > 0)
		openedDoc = documentList->GetNthDoc(documentList->GetDocCount()-1);
	else
		openedDoc = nil;
#endif

	this->setCurDoc(openedDoc);

	// Initialize preferences
	InterfacePtr<IPageMakeUpPrefs> storedPrefs ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
	if (storedPrefs == nil)
	{
		ASSERT_FAIL("PrsTCLParser::PrsTCLParser() storedPrefs invalid");
		return;
	}

	bool16 stopOnError = storedPrefs->getStopOnError();
	bool16 stopDisplay = storedPrefs->getStopDisplay();

	bool16 useImportPath, import, textbox, nonPrinting;
	PMString picPath;
	int32 colorIndex;
	int32 colorTint;
	storedPrefs->getPicturePrefs(&useImportPath, &import, &picPath, &textbox, &nonPrinting, &colorIndex, &colorTint);

	bool16 useSavePath, useOpenPath;
	PMString savePath, openPath;
	storedPrefs->getSavePrefs(&useSavePath, &savePath);
	storedPrefs->getOpenPrefs(&useOpenPath, &openPath);

	this->SetStopOnError(stopOnError);
	this->SetStopDisplay(stopDisplay);
	this->setPicturePrefs(useImportPath, import,  picPath, textbox, nonPrinting, colorIndex, colorTint);
	this->setSavePrefs(useSavePath, savePath);
	this->setOpenPrefs(useOpenPath, openPath);

	// Initialize internal tables
	theInfoTbl = new infoTbl();
	theInfoTbl->CJVect = nil ;

	// Initialize log file
	logfile = nil;	
}

/* Destructor
*/
PrsTCLParser::~PrsTCLParser()
{
	if(reader != nil)
		reader->Release();

	if(docHelper != nil)
		docHelper->Release();

	if(styleHelper != nil)
		styleHelper->Release();

	if(txtAttrHelper != nil)
		txtAttrHelper->Release();

	if(itemManager != nil)
		itemManager->Release();

	if(itemTransform != nil)
		itemTransform->Release();

	if(textFrameHelper != nil)
		textFrameHelper->Release();
	
	if(pageHelper != nil)
		pageHelper->Release();
	
	delete trsCoder;
	itemList.clear();
	storyList.clear();
	tableList.clear();
	delete[] colorList;

	deleteCJ();

	if (theInfoTbl != nil)
		delete theInfoTbl ;

	if(logfile != nil)
	{
		if(logfile->GetStreamState() != kStreamStateClosed)
			logfile->Close();

		logfile->Release();
	}
}

// ------------------
//  Accessor methods
// ------------------
ITCLReader * PrsTCLParser::QueryTCLReader()
{
	if(reader != nil)
	{
		reader->AddRef();
		return reader;
	}
	return nil;
}

IDocHelper * PrsTCLParser::QueryDocHelper()
{
	if(docHelper != nil)
	{
		docHelper->AddRef();
		return docHelper;
	}
	return nil;
}

IStyleHelper * PrsTCLParser::QueryStyleHelper()
{
	if(styleHelper != nil)
	{
		styleHelper->AddRef();
		return styleHelper;
	}
	return nil;
}

ITxtAttrHelper * PrsTCLParser::QueryTxtAttrHelper()
{
	if(txtAttrHelper != nil)
	{
		txtAttrHelper->AddRef();
		return txtAttrHelper;
	}
	return nil;
}

IItemManager * PrsTCLParser::QueryItemManager()
{
	if(itemManager != nil)
	{
		itemManager->AddRef();
		return itemManager;
	}
	return nil;
}

IItemTransform * PrsTCLParser::QueryItemTransform()
{
	if(itemTransform != nil)
	{
		itemTransform->AddRef();
		return itemTransform;
	}
	return nil;
}

ITextFrameHelper * PrsTCLParser::QueryTextFrameHelper()
{
	if(textFrameHelper != nil)
	{
		textFrameHelper->AddRef();
		return textFrameHelper;
	}
	return nil;
}


IPageHelper * PrsTCLParser::QueryPageHelper()
{
	if(pageHelper != nil)
	{
		pageHelper->AddRef();
		return pageHelper;
	}
	return nil;
}

void PrsTCLParser::setTranscodeTable(int32 table)
{
	trsCoder->setCurrentTable(table);
}



bool8 PrsTCLParser::SetInputFile(IDFile file)
{
	tclFile = file;
	
	InterfacePtr<IPMStream> TCLStream (StreamUtil::CreateFileStreamRead(file));
	if(TCLStream == nil)
		return kFalse;

	if(reader != nil)
	{
		reader->SetStream(TCLStream);
	}
	
	return kTrue;
}

void PrsTCLParser::SetNewFileName(PMString newName)
{
	this->newFileName = newName;
}

inline sColor PrsTCLParser::getColorList(int16 index)
{
	return this->colorList[index];
}

inline void PrsTCLParser::setColorList(int16 index, sColor col)
{
	this->colorList[index] = col;
}

inline TCLRef PrsTCLParser::getCurItem()
{
	return this->curItem;
}

inline void PrsTCLParser::setCurItem(TCLRef ref, bool16 selectTOP)
{
	this->previousItem = this->curItem;
	this->curItem = ref;
	this->topSelected = selectTOP;

	// Invalid table selection
	this->curTable = kInvalidUID;
	this->curCells = kZeroGridArea;
}

inline TCLRef PrsTCLParser::getPreviousItem()
{
	return this->previousItem;
}

void PrsTCLParser::setCurCells(InterfacePtr<ITableModel>& tableModel, GridArea targetArea, int32 startRow, int32 startCol, int32 endRow, int32 endCol)
{
		// Decrement indexes, because they are 0-based
	--startRow;
	--startCol;
	--endRow;
	--endCol;

	if(startCol == -1)
	{
		if(startRow == -1)
			this->curCells = GridArea(targetArea.GetRows().start ,0,targetArea.GetRows().start + targetArea.GetRows().count , tableModel->GetTotalCols().count);
		else
			this->curCells = GridArea(targetArea.GetRows().start + startRow, 0, targetArea.GetRows().start + endRow + 1, tableModel->GetTotalCols().count);
	}
	else if(startRow == -1)
		this->curCells = GridArea(targetArea.GetRows().start, startCol, targetArea.GetRows().start + targetArea.GetRows().count, endCol+1);	
	else 
		this->curCells = GridArea(targetArea.GetRows().start + startRow, startCol, targetArea.GetRows().start + endRow + 1, endCol+1);		

	// Check if it's the selection fits the concerned area
	if(!targetArea.Contains(this->curCells) || !tableModel->IsAnchor(this->curCells.GetTopLeft()))
		throw TCLError(PMString(kErrInvalidCellSelection));

	// Invalidate frame selection
	this->curItem = kInvalidTCLRef;
	this->topSelected = kFalse;
}

inline void PrsTCLParser::setCurCells(const GridArea& area)
{
	this->curCells = area;
	// Invalidate frame selection
	this->curItem = kInvalidTCLRef;
	this->topSelected = kFalse;
}

inline GridArea PrsTCLParser::getCurCells()
{
	return this->curCells;
}

inline UID PrsTCLParser::getCurTable()
{
	return this->curTable;
}


inline void PrsTCLParser::setCurTable(const PMString& table)
{
	this->curTable = getTable(table);
}

void PrsTCLParser::setTable(PMString ref, UID table)
{
	::InsertOrReplaceKeyValue(this->tableList,ref,table);
}

UID PrsTCLParser::getTable(PMString ref)
{
	UID table = kInvalidUID;

	int32 location = ::FindLocation(this->tableList, ref);
	
	if(location >= 0)
		table = this->tableList[location].Value();

	return table;
}

inline int16 PrsTCLParser::getCurPage()
{
	return this->curPage;
}

inline void PrsTCLParser::setCurPage(int16 num)
{
	this->curPage = num;
}

inline IDocument * PrsTCLParser::getCurDoc()
{
	return this->curDoc;
}

inline IDataBase * PrsTCLParser::getCurDataBase()
{
	return this->curDB;
}

void PrsTCLParser::setCurDoc(IDocument * doc)
{
	curDoc = doc;
	curItem = kInvalidTCLRef;
	topSelected = kFalse;
	previousItem = kInvalidTCLRef;
	curPage = 1;
	curTable = kInvalidUID;
	curCells = kZeroGridArea;
	nbItemRegistered = 0;
	txtBuf.Clear();
	itemList.clear();
	storyList.clear();
	tableList.clear();
	
	ResetCurrentPermRefs();

	if(curDoc != nil)
	{
		curDB = ::GetDataBase(doc);

		// Read item list if any
		this->InitializeRefList();

		// Turn undo support off for current document
		curDB->BeginTransaction();
		InterfacePtr<ICommandMgr> cmdMgr (curDoc, UseDefaultIID());
		cmdMgr->SetUndoSupport(kFalse);
		curDB->EndTransaction();
	}		
	else
		curDB = nil;

	// Reinitialize color list (some colors may no longer exist)
	this->InitializeColor();
}

void PrsTCLParser::setCurBook(UIDRef book)
{
	if(book != curBook)
	{
		if(curBook != UIDRef::gNull) // Close next old current book before setting new one
		{		
			InterfacePtr<ICommand> closeBookCmd (CmdUtils::CreateCommand(kCloseBookCmdBoss));
			closeBookCmd->SetItemList(UIDList(curBook));

			InterfacePtr<IBoolData> allowCancelData (closeBookCmd, IID_IALLOWCANCELCLOSEBOOKDATA);
			allowCancelData->Set(kFalse);

			InterfacePtr<IIntData> uiflagsData (closeBookCmd, IID_ICLOSEBOOKUIFLAG);
			uiflagsData->Set(K2::kSuppressUI);
			
			CmdUtils::ScheduleCommand(closeBookCmd);
			CmdUtils::ProcessScheduledCmds();
		}
		curBook = book;
	}
}

UIDRef PrsTCLParser::getCurBook()
{
	return curBook;
}

void PrsTCLParser::getPicturePrefs(bool8 * useImportPath, bool8 * import, PMString * picPath, bool8 * textbox,
								   bool8 * nonPrinting, int32 * colorIndex, int32 * colorTint)
{
	 if(useImportPath != nil) 
		 *useImportPath = prefs.useImportPath;
	 if(import != nil) 
		 *import = prefs.autoPictImport;
	 if(picPath != nil) 
		 *picPath = prefs.picturePath;
	 if(textbox !=nil) 
		 *textbox = prefs.createTextBox;
	 if(nonPrinting != nil) 
		 *nonPrinting = prefs.isNonPrinting;
	 if(colorIndex != nil) 
		 *colorIndex = prefs.colorIndex;
	 if(colorTint != nil) 
		 *colorTint = prefs.colorTint;
}

void PrsTCLParser::setPicturePrefs(bool8 useImportPath, bool8 import, PMString picPath, bool8 textbox,
								   bool8 nonPrinting, int32 colorIndex, int32 colorTint)
{
	prefs.autoPictImport = import;
	prefs.picturePath = picPath;
	if(picPath.LastIndexOfCharacter(kDirSeparatorChar) != picPath.NumUTF16TextChars() - 1)
		prefs.picturePath += kDirSeparator;
	prefs.createTextBox = textbox;
	prefs.isNonPrinting = nonPrinting;
	prefs.useImportPath = useImportPath;
	prefs.colorIndex = colorIndex;
	prefs.colorTint = colorTint;
}

void PrsTCLParser::getSavePrefs(bool8 * usePath, PMString * savePath)
{
	if(usePath != nil) 
		 *usePath = prefs.useSavePath;
	 if(savePath != nil) 
		 *savePath = prefs.savePath;
}

void PrsTCLParser::setSavePrefs(bool8 usePath, PMString savePath)
{
	prefs.useSavePath = usePath;
	prefs.savePath = savePath;
	if(savePath.LastIndexOfCharacter(kDirSeparatorChar) != savePath.NumUTF16TextChars() - 1)
		prefs.savePath += kDirSeparator;
}

void PrsTCLParser::getOpenPrefs(bool8 * usePath, PMString * openPath)
{
	if(usePath != nil) 
		 *usePath = prefs.usePathForOpen;
	 if(openPath != nil) 
		 *openPath = prefs.openPath;
}

void PrsTCLParser::setOpenPrefs(bool8 usePath, PMString openPath)
{
	prefs.usePathForOpen = usePath;
	prefs.openPath = openPath;
	if(openPath.LastIndexOfCharacter(kDirSeparatorChar) != openPath.NumUTF16TextChars() - 1)
		prefs.openPath += kDirSeparator;
}

void PrsTCLParser::getExportPrefs(PMString * exportPath)
{
	if(exportPath != nil)
		*exportPath= prefs.exportPath; 
}

void PrsTCLParser::setExportPrefs(PMString exportPath)
{
	prefs.exportPath = exportPath;
	if(exportPath.LastIndexOfCharacter(kDirSeparatorChar) != exportPath.NumUTF16TextChars() - 1)
		prefs.exportPath += kDirSeparator;
}

void PrsTCLParser::WriteLogFile(PMString msg)
{
	if(logfile)
	{
		msg.SetTranslatable(kFalse);
		msg += kNewLine;
		logfile->XferByte((uchar *) msg.GetPlatformString().c_str(), msg.GetPlatformString().size());
	}
}

inline bool8 PrsTCLParser::StopOnError()
{
	return prefs.stopOnError;
}

inline void PrsTCLParser::SetStopOnError(bool8 stop)
{
	prefs.stopOnError = stop;
}

inline bool8 PrsTCLParser::StopDisplay()
{
	return prefs.stopDisplay;
}

inline void PrsTCLParser::SetStopDisplay(bool8 stop)
{
	prefs.stopDisplay = stop;
}

inline int32 PrsTCLParser::getNbItemRegisteredAtBeginning()
{
	return this->nbItemRegistered;
}

void PrsTCLParser::SetCurrentPermRefs(PermRefStruct ref, TextIndex startPosition, UID story)
{
	this->curPermRef = ref;
	this->startPosPermRef = startPosition;

	int32 pos = ::FindLocation(this->storyList, ref);
	// Check whether ref already exists
	if(pos != -1)
	{
		K2Vector<UID>& storiesUID = this->storyList[pos].Value();
		
		K2Vector<UID>::iterator iter = ::K2find(storiesUID.begin(), storiesUID.end(),story);
		// Check whether story already exists, if not add to the end of the list
		if(iter == storiesUID.end())
			storiesUID.push_back(story);					
	}
	else
	{
		// New ref, add it to the list
		K2Vector<UID> storiesUID;
		storiesUID.push_back(story);
		::InsertKeyValue(this->storyList,ref,storiesUID);
	}
}

void PrsTCLParser::GetCurrentPermRefs(PermRefStruct& ref, TextIndex& startPosition)
{
	ref = this->curPermRef;
	startPosition = this->startPosPermRef;
}

void PrsTCLParser::ResetCurrentPermRefs()
{
	this->curPermRef = kNullPermRef;
	this->startPosPermRef = INVALID_PERMREFS;
}

K2Vector<UID> PrsTCLParser::getStories(PermRefStruct permRef)
{
	K2Vector<UID> stories;

	int32 location = ::FindLocation(this->storyList, permRef);
	
	if(location >= 0)
		stories = this->storyList[location].Value();

	return stories;
}

void PrsTCLParser::AppendPermRefsList(const PermRefVector& permRefsList)
{
	PermRefVector::const_iterator iter = permRefsList.begin();
	while(iter != permRefsList.end())
	{
		PermRefStruct refToAppend = iter->Key();
		K2Vector<UID> storiesToAppend = iter->Value();

		int32 pos = ::FindLocation(this->storyList, refToAppend);
		// Check whether ref already exists
		if(pos != -1)
		{
			K2Vector<UID>& storiesUID = this->storyList[pos].Value();
			
			for(int32 i = 0 ; i < storiesToAppend.size() ; ++i)
			{
				K2Vector<UID>::iterator iterStories = ::K2find(storiesUID.begin(), storiesUID.end(),storiesToAppend[i]);
				// Check whether story already exists, if not add to the end of the list
				if(iterStories == storiesUID.end())
					storiesUID.push_back(storiesToAppend[i]);					
			}
		}
		else
		{
			// New ref, add it to the list
			::InsertKeyValue(this->storyList,refToAppend,storiesToAppend);
		}

		++iter;
	}
}

/* getItem
*/
UID PrsTCLParser::getItem(TCLRef ref)
{
	UID item = kInvalidUID;

	int32 location = ::FindLocation(this->itemList, ref);
	
	if(location >= 0)
		item = this->itemList[location].Value();

	return item;
}

/* deleteItem
*/
void PrsTCLParser::deleteItem(TCLRef ref)
{
	this->itemList.erase(this->itemList.begin() + ::FindLocation(this->itemList, ref));
}

/* setItem
*/
void PrsTCLParser::setItem(TCLRef ref, UID item)
{
	RefVector::iterator iter = ::K2find(this->itemList.begin(), this->itemList.end(), ref);
	if (iter == this->itemList.end())
	 this->itemList.push_back(TCLRefItem(ref, item));
	else
	{
		// An item already have this reference, get it and reset its ITCLRefData interface
		UID oldReferencedItem = iter->Value();
		InterfacePtr<ITCLRefData> tclDataToReset (this->curDB, oldReferencedItem, IID_ITCLREFDATA);
		if(tclDataToReset) // oldReferenced may not exist anymore, after a cell merge for instance, inline items can be deleted
		{
			InterfacePtr<ICommand> setTCLRefCmd (CmdUtils::CreateCommand(kPrsSetTCLRefDataCmdBoss));
			setTCLRefCmd->SetUndoability(ICommand::kAutoUndo);
			setTCLRefCmd->SetItemList(UIDList(tclDataToReset));
			
			InterfacePtr<ITCLRefData> cmdData (setTCLRefCmd, IID_ITCLREFDATA);
			cmdData->Set(kDefaultTCLRefData);
			CmdUtils::ProcessCommand(setTCLRefCmd);
		}

		// Reference the new item
		iter->SetValue(item);
	}	

	// Store TCL reference in item itself as well
	InterfacePtr<ITCLRefData> tclData (this->curDB, item, IID_ITCLREFDATA);
	
	InterfacePtr<ICommand> setTCLRefCmd (CmdUtils::CreateCommand(kPrsSetTCLRefDataCmdBoss));
	setTCLRefCmd->SetUndoability(ICommand::kAutoUndo);
	setTCLRefCmd->SetItemList(UIDList(tclData));
	
	InterfacePtr<ITCLRefData> cmdData (setTCLRefCmd, IID_ITCLREFDATA);
	cmdData->Set(ref);
	CmdUtils::ProcessCommand(setTCLRefCmd);
}

/* InitializeRefList
*/
void PrsTCLParser::InitializeRefList()
{
	// This function does the following things if there is a document open (the frontmost):
	// 1. Get the item list stored in this document
	// 2. If there is an item selectionned, it's added to the list with the TCL reference 0
	// 3. Add the linked frames (if any and if the item is text frame) to the list,
	// with references 1, 2, 3, etc... deleting the existing items with these references	
	
	// Test if a document is open
	if(curDoc != nil)
	{
		// Test if there is an item list stored in the document
		InterfacePtr<ITCLReferencesList> docRefList(curDoc,UseDefaultIID());

		if(docRefList != nil)
		{		
			itemList = docRefList->GetItemList();
			storyList = docRefList->GetStoryList(); // Initialize story list & table list
			tableList = docRefList->GetTableList(); 

			PMString info("Nb item enregistre : ");
			info.AppendNumber(itemList.size());
			//CAlert::InformationAlert(info);

			/*if(itemList.size() - 1 > MAX_BLOC) // Should never occur
			{
				throw TCLError(PMString(kErrTooManySavedItems));
			}*/
		}	

#if !SERVER // In InDesign Server version, there are no selection available

		// Fill the beginning of the list with selected frames (erase existing item)
		InterfacePtr<ISelectionManager> selectionMgr (Utils<ISelectionUtils>()->QueryActiveSelection ());
		if(selectionMgr == nil)
		{
			ASSERT_FAIL("PrsTCLParser::InitializeRefList -> selectionMgr nil");
			throw TCLError(PMString(kErrNilInterface));
		}
		
		UIDList frameSelectedList;
		
		InterfacePtr<IParserSuite> prsSuite (selectionMgr, UseDefaultIID());
		if(prsSuite != nil) // prsSuite == nil means there is no active layout selection
		{
			prsSuite->GetSelectedItems(frameSelectedList);
		}
	
		// Test if one or more frame are selected
		if(!frameSelectedList.IsEmpty())
		{
			InterfacePtr<IHierarchy> firstFrame(nil);
			
			if(Utils<IFrameContentUtils>()->DoesContainTextFrame(&frameSelectedList,kFalse))
			{
				// there is at least one text frame, looking for it
				
				int32 i = 0, textflags;
				while(i < frameSelectedList.Length())
				{
					InterfacePtr<IHierarchy> frameHier (frameSelectedList.GetRef(i),UseDefaultIID()); 
					if(frameHier == nil)
					{
						ASSERT_FAIL("PrsTCLParser::InitializeRefList -> frameHier nil");
						throw TCLError(PMString(kErrNilInterface));
					}
					
					if(Utils<IFrameUtils>()->IsTextFrame(frameHier, &textflags))
					{
						// It's a text frame, test if it's linked
						if((textflags & IFrameUtils::kTF_InLink) || (textflags & IFrameUtils::kTF_OutLink))  
						{
							// Looking for the first frame
							InterfacePtr<IHierarchy> multiCol (frameHier->QueryChild(0));
							if(multiCol == nil)
							{
								ASSERT_FAIL("PrsTCLParser::InitializeRefList -> multiCol nil");
								throw TCLError(PMString(kErrNilInterface));
							}

							InterfacePtr<IMultiColumnTextFrame> txtFrame (multiCol,UseDefaultIID());
							if(txtFrame == nil)
							{
								ASSERT_FAIL("PrsTCLParser::InitializeRefList -> txtFrame nil");
								throw TCLError(PMString(kErrNilInterface));
							}

							InterfacePtr<IFrameList> frList (txtFrame->QueryFrameList());
							if(frList == nil)
							{
								ASSERT_FAIL("PrsTCLParser::InitializeRefList -> frList nil");
								throw TCLError(PMString(kErrNilInterface));
							}

							// Get the first frame from the frame list
							InterfacePtr<IHierarchy> frameItem (::GetDataBase(frList),frList->GetNthFrameUID(0),UseDefaultIID());
							if(frameItem == nil)
							{		
								ASSERT_FAIL("PrsTCLParser::InitializeRefList -> frameItem nil");
								throw TCLError(PMString(kErrNilInterface));
							}

							InterfacePtr<IHierarchy> multiCol2 (frameItem->QueryParent());
							if(multiCol2 == nil)
							{		
								ASSERT_FAIL("PrsTCLParser::InitializeRefList -> multiCol2 nil");
								throw TCLError(PMString(kErrNilInterface));
							}

							InterfacePtr<IHierarchy> splineItem (multiCol2->QueryParent());
							if(splineItem == nil)
							{		
								ASSERT_FAIL("PrsTCLParser::InitializeRefList -> splineItem nil");
								throw TCLError(PMString(kErrNilInterface));
							}

							firstFrame = splineItem;
						}
						else // this a single frame or the first frame of linked frames
							firstFrame = frameHier;

						break;
					}
					i++;
				}
				
				// Fill the item list
				InterfacePtr<IHierarchy> multiCol (firstFrame->QueryChild(0));
				if(multiCol == nil)
				{		
					ASSERT_FAIL("PrsTCLParser::InitializeRefList -> multiCol nil");
					throw TCLError(PMString(kErrNilInterface));
				}

				InterfacePtr<IMultiColumnTextFrame> txtFrame (multiCol,UseDefaultIID());
				if(txtFrame == nil)
				{		
					ASSERT_FAIL("PrsTCLParser::InitializeRefList -> txtFrame nil");
					throw TCLError(PMString(kErrNilInterface));
				}

				InterfacePtr<IFrameList> iFrameList (txtFrame->QueryFrameList());
				if(iFrameList == nil)
				{		
					ASSERT_FAIL("PrsTCLParser::InitializeRefList -> iFrameList nil");
					throw TCLError(PMString(kErrNilInterface));
				}
				
				IDataBase * db = ::GetDataBase(iFrameList);

				// Insert the first frame in the list as 0 referenced item
				itemList.push_back(TCLRefItem(kZeroTCLRef,::GetUID(firstFrame)));

				// Insert the linked frames in the list with references 1, 2, 3...
				int32 nbFrame = iFrameList->GetFrameCount();
				int32 nbSpline = 1;
				for(int n = 1; n < nbFrame ; n++)
				{
					// Check if it's not a column of the same frame already inserted in the list
					InterfacePtr<IHierarchy> currentFrame (db,iFrameList->GetNthFrameUID(n),UseDefaultIID());
					if(currentFrame == nil)
					{		
						ASSERT_FAIL("PrsTCLParser::InitializeRefList -> currentFrame nil");
						throw TCLError(PMString(kErrNilInterface));
					}

					UIDList list(db);
					currentFrame->GetAncestors(&list,IID_IGRAPHICFRAMEDATA);
					UID splineUID = list.First(); // should have only one element, the splineItem
					if(itemList.back().Value() != splineUID)
					{
						InterfacePtr<IHierarchy> hier (db,splineUID,UseDefaultIID());
						if(hier == nil)
						{		
							ASSERT_FAIL("PrsTCLParser::InitializeRefList -> hier nil");
							throw TCLError(PMString(kErrNilInterface));
						}
#if REF_AS_STRING
						PMString nbSplineStr;
						nbSplineStr.AppendNumber(nbSpline);
						::InsertOrReplaceKeyValue(itemList,nbSplineStr,splineUID);
#else
						::InsertOrReplaceKeyValue(itemList,nbSpline,splineUID);
#endif
						nbSpline++;	
					}
				}
				this->nbItemRegistered = nbSpline;
			}
			else
			{
				// there are no text frames, the first in the list becomes the 0 referenced
				InterfacePtr<IHierarchy> otherFrame (frameSelectedList.GetDataBase(),frameSelectedList.First(),UseDefaultIID());
				if(otherFrame == nil)
				{		
					ASSERT_FAIL("PrsTCLParser::InitializeRefList -> otherFrame nil");
					throw TCLError(PMString(kErrNilInterface));
				}

				firstFrame = otherFrame;
				// Set reference frame
				itemList.push_back(TCLRefItem(kZeroTCLRef,::GetUID(otherFrame)));
				this->nbItemRegistered = 1;
			}

			// Select the first frame
			if(firstFrame != nil)
			{
				this->itemManager->SelectPageItem(::GetUIDRef(firstFrame));
				this->curItem = kZeroTCLRef;				

				InterfacePtr<IPageList> pageList (curDoc, UseDefaultIID());

				this->curPage = pageList->GetPageIndex(Utils<ILayoutUtils>()->GetOwnerPageUID(firstFrame)) + 1;
			}
				

		}	
#endif		
	}

	//permits to show the item's list
	//it's useful for developers but not for users
	/*
	RefVector::iterator iter;
	for(iter = itemList.begin(); iter < itemList.end(); iter++)
	{
		PMString info("TCL ref : "); info.AppendNumber(iter->Key()); info.Append("\n");
		info.Append("UID : ");info.AppendNumber(iter->Value().Get());

		this->itemManager->SelectPageItemCmd(UIDRef(::GetDataBase(doc),iter->Value()));
		CAlert::InformationAlert(info);
	}
	*/
}

/* InitializeColor
*/
void PrsTCLParser::InitializeColor()
{
	if(colorList != nil)
		delete[] colorList;

	colorList = new sColor[MAX_COLOR + 1];// the max reference is MAX_COLOR and the number of possible fields in the table is MAX_COLOR+1
	
	// Get the active swatch list	
	InterfacePtr<IWorkspace> workspace;
	if(this->curDoc)
		workspace = InterfacePtr<IWorkspace>(curDoc->GetDocWorkSpace(), UseDefaultIID());
	else
		workspace = InterfacePtr<IWorkspace>(GetExecutionContextSession()->QueryWorkspace());

	InterfacePtr<ISwatchList> swList (workspace, IID_ISWATCHLIST);

	IDataBase * db = ::GetDataBase(swList);

	ColorArray colorArray;

	// Fill the array with InDesign default colors (create them if missing)
	colorList[0].colorUID = swList->GetPaperSwatchUID();
	colorList[0].name = Utils<ISwatchUtils>()->GetSwatchName(db, colorList[0].colorUID);

	colorArray.push_back(1);
	colorArray.push_back(0.9);
	colorArray.push_back(0.1);
	colorArray.push_back(0);
	colorList[1].colorUID = swList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();	
	if(colorList[1].colorUID == kInvalidUID || !Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,colorList[1].colorUID))
	{
		PMString swtchName = Utils<ISwatchUtils>()->FormatSwatchNameWithColorValue(swList, kPMCsCalCMYK,colorArray, kTrue);
		colorList[1].colorUID = Utils<ISwatchUtils>()->CreateProcessCMYKSwatch(workspace,swtchName,1.0,0.9,0.1,0.0);
		colorList[1].name = swtchName;
	}
	else
		colorList[1].name = Utils<ISwatchUtils>()->GetSwatchName(db,colorList[1].colorUID);
	colorArray.clear();

	colorArray.push_back(1);
	colorArray.push_back(0);
	colorArray.push_back(0);
	colorArray.push_back(0);
	colorList[2].colorUID = swList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
	if(colorList[2].colorUID == kInvalidUID || !Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,colorList[2].colorUID))
	{
		PMString swtchName = Utils<ISwatchUtils>()->FormatSwatchNameWithColorValue(swList, kPMCsCalCMYK,colorArray, kTrue);
		colorList[2].colorUID = Utils<ISwatchUtils>()->CreateProcessCMYKSwatch(workspace,swtchName,1.0,0.0,0.0,0.0);
		colorList[2].name = swtchName;
	}
	else
		colorList[2].name = Utils<ISwatchUtils>()->GetSwatchName(db,colorList[2].colorUID);
	colorArray.clear();

	colorArray.push_back(0);
	colorArray.push_back(0);
	colorArray.push_back(1);
	colorArray.push_back(0);
	colorList[3].colorUID = swList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
	if(colorList[3].colorUID == kInvalidUID || !Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,colorList[3].colorUID))
	{
		PMString swtchName = Utils<ISwatchUtils>()->FormatSwatchNameWithColorValue(swList, kPMCsCalCMYK,colorArray, kTrue);
		colorList[3].colorUID = Utils<ISwatchUtils>()->CreateProcessCMYKSwatch(workspace,swtchName,0.0,0.0,1.0,0.0);
		colorList[3].name = swtchName;
	}
	else
		colorList[3].name = Utils<ISwatchUtils>()->GetSwatchName(db,colorList[3].colorUID);
	colorArray.clear();

	colorArray.push_back(0);
	colorArray.push_back(1);
	colorArray.push_back(0);
	colorArray.push_back(0);
	colorList[4].colorUID = swList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
	if(colorList[4].colorUID == kInvalidUID || !Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,colorList[4].colorUID))
	{
		PMString swtchName = Utils<ISwatchUtils>()->FormatSwatchNameWithColorValue(swList, kPMCsCalCMYK,colorArray, kTrue);
		colorList[4].colorUID = Utils<ISwatchUtils>()->CreateProcessCMYKSwatch(workspace,swtchName,0.0,1.0,0.0,0.0);
		colorList[4].name = swtchName;
	}
	else
		colorList[4].name = Utils<ISwatchUtils>()->GetSwatchName(db,colorList[4].colorUID);
	colorArray.clear();

	colorList[5].colorUID = swList->GetBlackSwatchUID();
	colorList[5].name = Utils<ISwatchUtils>()->GetSwatchName(db, colorList[5].colorUID);

	colorList[6].colorUID = swList->GetRegistrationSwatchUID();
	colorList[6].name = Utils<ISwatchUtils>()->GetSwatchName(db, colorList[6].colorUID);

	colorArray.push_back(0.15);
	colorArray.push_back(1);
	colorArray.push_back(1);
	colorArray.push_back(0);
	colorList[7].colorUID = swList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
	if(colorList[7].colorUID == kInvalidUID || !Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,colorList[7].colorUID))
	{
		PMString swtchName = Utils<ISwatchUtils>()->FormatSwatchNameWithColorValue(swList, kPMCsCalCMYK,colorArray, kTrue);
		colorList[7].colorUID = Utils<ISwatchUtils>()->CreateProcessCMYKSwatch(workspace,swtchName,0.15,1.0,1.0,0.0);
		colorList[7].name = swtchName;
	}
	else
		colorList[7].name = Utils<ISwatchUtils>()->GetSwatchName(db, colorList[7].colorUID);
	colorArray.clear();
	
	colorArray.push_back(0.75);
	colorArray.push_back(0.05);
	colorArray.push_back(1);
	colorArray.push_back(0);
	colorList[8].colorUID = swList->FindSwatch(kPMCsCalCMYK,colorArray).GetUID();
	if(colorList[8].colorUID == kInvalidUID || !Utils<ISwatchUtils>()->SwatchIsGlobalSwatch(db,colorList[8].colorUID))
	{
		PMString swtchName = Utils<ISwatchUtils>()->FormatSwatchNameWithColorValue(swList, kPMCsCalCMYK,colorArray, kTrue);
		colorList[8].colorUID = Utils<ISwatchUtils>()->CreateProcessCMYKSwatch(workspace,swtchName,0.75,0.05,1.0,0.0);
		colorList[8].name = swtchName;
	}
	else
		colorList[8].name = Utils<ISwatchUtils>()->GetSwatchName(db, colorList[8].colorUID);;
	colorArray.clear();

	colorList[9].colorUID = swList->GetNoneSwatchUID();
	colorList[9].name = Utils<ISwatchUtils>()->GetSwatchName(db, colorList[9].colorUID);

	// Fill the rest of the array with default value
	for(int32 i = 10; i < MAX_COLOR + 1 ; i++)
	{
		colorList[i].colorUID = kInvalidUID;
		colorList[i].name = PMString("");
		colorList[i].name.SetTranslatable(kFalse);
	}
}

/* SaveItemList
*/
ErrorCode PrsTCLParser::SaveItemList()
{
	ErrorCode status = kFailure;
	PMString error = PMString(kErrNilInterface);
	do
	{

		if(this->curDoc == nil) // No front doc, can't save
			break;

		UIDList docToSave(this->curDoc);
		InterfacePtr<ICommand> saveItemsCmd (CmdUtils::CreateCommand(kPrsSetDocTCLReferencesListCmdBoss));
		if(saveItemsCmd == nil)
		{		
			ASSERT_FAIL("PrsTCLParser::SaveItemList -> saveItemsCmd nil");
			break;
		}
		
		saveItemsCmd->SetUndoability(ICommand::kAutoUndo);
		saveItemsCmd->SetItemList(docToSave);

		InterfacePtr<ITCLReferencesList> cmdData (saveItemsCmd, UseDefaultIID());
		if(cmdData == nil)
		{		
			ASSERT_FAIL("PrsTCLParser::SaveItemList -> cmdData nil");
			break;
		}

		cmdData->SetItemList(this->itemList);
		cmdData->SetStoryList(this->storyList);
		cmdData->SetTableList(this->tableList);

		if(CmdUtils::ProcessCommand(saveItemsCmd) != kSuccess)
			break;

		// Add the adornments
		UIDList items(this->curDB);
		RefVector::iterator iter;
		for(iter = this->itemList.begin(); iter < itemList.end() ; iter++)
		{
			UID itemUID = iter->Value();
			InterfacePtr<IPageItemAdornmentList> adornmentList (this->curDB, itemUID, UseDefaultIID());
			if(adornmentList && !adornmentList->HasAdornment(kPrsTCLAdornmentBoss))
				items.Append(itemUID);
		}

		InterfacePtr<ICommand>  addPageItemAdornmentCmd(CmdUtils::CreateCommand(kAddPageItemAdornmentCmdBoss));
		InterfacePtr<IClassIDData>  classIDData(addPageItemAdornmentCmd, UseDefaultIID());

		classIDData->Set(kPrsTCLAdornmentBoss);
		addPageItemAdornmentCmd->SetItemList(items);
		
		if(CmdUtils::ProcessCommand(addPageItemAdornmentCmd) != kSuccess)
		{
			error = PMString(kErrTCLAdornment);
			ErrorUtils::PMSetGlobalErrorCode(kFailure,&error);
			break;
		}

		status = kSuccess;
		
	} while(false);

	return status;
}

/* ProcessTCL
*/
bool16 PrsTCLParser::ProcessTCL(int32& numError)
{
	uchar16 c1, c2;
	try
	{		
		reader->ReadChar();
		if (reader->GetCurrentChar() == '[')
		{
			reader->SetEndOfCommand(kFalse);
			reader->FiltreBlancUpper();
			c1 = reader->GetCurrentANSIChar();
			reader->FiltreBlancUpper();
			c2 = reader->GetCurrentANSIChar();
			reader->SetNumParam(0);		

#if XRAIL      
			// Pour ne pas qu'a la generation CdF sur InDD Desktop l'utilisateur ait le message
			// de changement d'etat page a chaque fermeture de nouveau document xrail genere,
			// PageMakeUp passe automatiquement le mode "robot" du plugin XRail a "vrai".
			// A la fin du traitement TCL, PageMakeUp remet le mode "robot" a "faux".		

            // On recupere les dernieres preferences
           	InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace()); 
        	InterfacePtr<IXRailPrefsData>  xrailPrefsData ((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));	
		
			// XRail prefs
			PMString adresseIP = xrailPrefsData->GeturlXR();
	        PMString lastUser = xrailPrefsData->GetLastUser(); 

			K2Vector<PMString>listeBases;
			K2Vector<PMString>IPBases;
#if MULTIBASES == 1
			listeBases = xrailPrefsData->GetListeBases();
			IPBases = xrailPrefsData->GetIPBases();
#endif
			// Execute command
            InterfacePtr<ICommand> setPrefsCmd(CmdUtils::CreateCommand(kXRLSetPrefsCmdBoss));
		    InterfacePtr<IXRailPrefsData> prefsData(setPrefsCmd, UseDefaultIID());  

			// Set prefs XRail
            prefsData->SeturlXR(adresseIP);
  	    	prefsData->SetLastUser(lastUser);
	        prefsData->SetModeRobot(kTrue);     
#if MULTIBASES
			prefsData->SetListeBases(listeBases);     
			prefsData->SetIPBases(IPBases);     
#endif

		    if (CmdUtils::ProcessCommand(setPrefsCmd) != kSuccess){
				ASSERT_FAIL("command setPrefsCmd failed");
			}
#endif

			switch (c1)
			{
				case 'A' : 
					switch(c2)
					{
						case 'N' : { Func_AN an(this); } break;
						
						case 'V' : { 
							FlushBuffer();
							Func_AV av(this); } break;

						case 'U' : { Func_AU au(this); } break;

						case 'P' : { Func_AP ap(this); } break;
						
						case 'L' : { Func_AL al(this); } break;

						case 'R' : { 
							FlushBuffer();
							Func_AR ar(this); } break;

						case 'G' : {
							FlushBuffer();
							Func_AG(this); } break;


						case 'T' : {
							FlushBuffer();
							Func_AT(this); } break;

						case 'I' : { Func_AI(this); } break;

#if COMPOSER
						case 'W' : { 
							FlushBuffer();
							Func_AW(this); } break;

						case 'S' : { 
							FlushBuffer();
							Func_AS(this); } break;
#endif

#if XPAGE
						case 'A' : { 
							FlushBuffer();
							Func_AA(this); } break;

						case 'B' : {
							FlushBuffer();
							Func_AB(this); } break;

						case 'F' : { 
							FlushBuffer();
							Func_AF(this); } break;

						case 'C' : {
							FlushBuffer();
							Func_AC(this); } break;
                            
						case 'Z' : {
							FlushBuffer();
							Func_AZ(this); } break;
                            
						case 'M' : {
							FlushBuffer();
							Func_AM(this); } break;

						case 'D' : {
							FlushBuffer();
							Func_AD(this); } break;
                            
                        case 'E' : {
                            FlushBuffer();
                            Func_AE(this); } break;
#endif
						default  :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'B' : 
					FlushBuffer();
					switch(c2)
					{
						case 'A' : { Func_BA ba(this); } break;
						case 'C' : { Func_BC bc(this); } break;
						case 'H' : { Func_BH bh(this); } break;
						case 'I' : { Func_BI bi(this); } break;
						case 'J' : { Func_BJ bj(this); } break;
						case 'N' : { Func_BN bn(this); } break;
						case 'M' : { Func_BM bm(this); } break;
						case 'F' : { Func_BF bf(this); } break;
						case 'S' : { Func_BS bn(this); } break;
						case 'G' : { Func_BG bn(this); } break;
						case 'V' : { Func_BV bn(this); } break;
						case 'D' : { Func_BD bn(this); } break;
						case '1' : { Func_B1 b1(this); } break;
						case '2' : { Func_B2 b2(this); } break;
						case 'E' : { Func_BE be(this); } break;
						case 'P' : { Func_BP bp(this); } break;
						case 'U' : { Func_BU bu(this); } break;
						case 'Q' : { Func_BQ bq(this); } break;
						case 'O' : { Func_BO bo(this); } break;
						case 'R' : { Func_BR br(this); } break;
						case 'Z' : { Func_BZ bz(this); } break;
						case 'L' : { Func_BL bl(this); } break;
#if COMPOSER
						case 'W' : { Func_BW bw(this); } break;
						case '3' : { Func_B3 b3(this); } break;
#endif
						case '+' : { Func_BPLUS bplus(this); } break;
						case '-' : { Func_BMOINS bmoins(this); } break;

						case '$' : { Func_BDollar bdollar(this); } break;
						case '%' : { Func_BPCent bpcent(this); } break;
						case '!' : { Func_BExcl bexcl(this); } break;
						case '*' : { Func_BStar bstar(this); } break;						

						default  :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'C' :
					switch(c2)
					{
						case 'N' : { Func_CN cn(this); } break;
						case 'S' : { Func_CS cs(this); } break;
						case 'C' : { Func_CC cc(this); } break;
						case 'M' : { Func_CM cm(this); } break;
						case 'J' : { Func_CJ cj(this); } break;
						case 'L' : { 
							FlushBuffer();
							Func_CL cl(this); } break;
						case 'P' : {
							FlushBuffer();
							Func_CP cp(this); } break;
						case 'Q' : {
							FlushBuffer();
							Func_CQ cq(this); } break;
						case 'T' : { 
							FlushBuffer();
							Func_CT ct(this); } break;

						case '+' : { Func_CPlus cplus(this); } break;
						case 'O' : { Func_CO co(this); } break;
						case '2' : { Func_CDeux cdeux(this); } break;
						case '#' : { Func_CDiese cdiese(this); } break;
						default  :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'D' :
					switch(c2)
					{
						case 'F' : { Func_DF df(this); } break;
						case 'M' : { 
							FlushBuffer();
							Func_DM dm(this); } break;
						case 'N' : { 
							FlushBuffer();
							Func_DN dn(this); } break;
						case 'S' : { 
							FlushBuffer();
							Func_DS ds(this); } break;
						case 'U' : { Func_DU du(this); } break;
						case 'C' : { Func_DC dc(this); } break;
						case 'P' : { Func_DP dp(this); } break;
						case 'T' : { Func_DT dt(this); } break;
						case 'D' : { Func_DD dd(this); } break;
						case 'E' : { Func_DE de(this); } break;
						case 'H' : { Func_DH dh(this); } break;
						case 'K' : { Func_DK dk(this); } break;
						case 'G' : { Func_DG dg(this); } break;
						case 'J' : { Func_DJ dh(this); } break;
						case 'B' : { 
							FlushBuffer();
							Func_DB db(this); } break;
						case 'O' : {
							FlushBuffer();
							Func_DO do_(this); } break;

						case 'R' : { Func_DR dr(this); } break;

#if COMPOSER
						case 'W' : { Func_DW(this); } break;
#endif

						case 'Z' : { 
							FlushBuffer();
							Func_DZ(this); } break;

						case '$' : { Func_DDollar(this); } break;
						case '*' : { Func_DEtoile(this); } break;

						default  :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'E' :
					switch(c2)
					{
						case 'C' : { Func_EC ec(this); } break;
						case 'D' : { Func_ED ed(this); } break;
						case 'F' : { Func_EF ef(this); } break;
						case 'P' : { Func_EP ep(this); } break;
						case 'V' : { 
							FlushBuffer();
							Func_EV ev(this); } break;
						case 'T' : { 
							FlushBuffer();
							Func_ET ev(this); } break;
						case 'I' : { Func_EI ei(this); } break;
						case 'H' : { Func_EH eh(this); } break;
						case 'R' : { Func_ER er(this); } break;
						case 'B' : { Func_EB eb(this); } break;
						case 'L' : { Func_EL el(this); } break;
						case 'N' : { Func_EN en(this); } break;
						case 'E' : { 
							FlushBuffer();
							Func_EE ee(this); } break;
						case 'X' : { 
							FlushBuffer();
							Func_EX ex(this); } break;
						default  :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'F' :
					FlushBuffer();
					switch(c2)
					{
						case 'A' : { Func_FA fa(this); } break;
						case 'H' : { Func_FH fh(this); } break;
						case 'V' : { Func_FV fv(this); } break;
						case 'D' : { Func_FD fd(this); } break;
						case 'F' : { Func_FF ff(this); } break;
						case 'P' : { Func_FP fp(this); } break;
						case 'W' : { Func_FW fw(this); } break;
						case 'L' : { Func_FL fl(this); } break;
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'G' :
					FlushBuffer();
					switch(c2)
					{
						case 'I' : { Func_GI gi(this); } break;
						case 'A' : { Func_GA ga(this); } break;
						case 'B' : { Func_GB gb(this);} break ;
						case '+' : { Func_GPLUS gplus(this);} break ;
						case '-' : { Func_GMOINS gmoins(this);} break ;
						case 'S' : { Func_GS gs(this);} break ;
						case 'N' : { Func_GN gn(this);} break;
						case 'O' : { Func_GO go(this);} break;
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;
				

				case 'H' :
					FlushBuffer();
					switch(c2)
					{
#if PERMREFS
						case 'D' : { Func_HD hd(this); } break;
						case 'F' : { Func_HF hf(this); } break;
						case 'S' : { Func_HS hs(this); } break;
						case 'R' : { Func_HR hr(this); } break;
						case 'C' : { Func_HC hc(this); } break;
						case 'H' : { Func_HH hh(this); } break;
#endif

#if HYPERLINKS
						case 'K' : { Func_HK hk(this); } break;
						case 'L' : { Func_HL hl(this); } break;
						case '1' : { Func_H1 h1(this); } break;
						case '2' : { Func_H2 h2(this); } break;
						case '3' : { Func_H3 h3(this); } break;
#endif
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'I' :
					
					switch(c2)
					{							
						case 'I' : { Func_II ii(this); } break;
						case 'C' : { Func_IC ic(this); } break;
						case 'P' : { Func_IP ip(this); } break;
						case 'E' : { 
							FlushBuffer();
							Func_IE ie(this); } break;

												
						case 'G' : {
							FlushBuffer();
							Func_IG ig(this); } break;

#if COMPOSER
						case 'B' : { Func_IB(this); } break;
#endif
                        
                        case 'T' : { Func_IT ip(this); } break; // GD 29.05.2017 pour Foucauld Perotin
                        
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;
					
				case 'J' :
					
					switch(c2)
					{
						case 'V' : { FlushBuffer(); 
										Func_JV jv(this); } break;
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'L' :
					switch(c2)
					{
						case 'F' : { Func_LF lf(this); } break;
						case 'S' : { Func_LS ls(this); } break;
						case 'N' : { Func_LN ln(this); } break;
						case 'O' : { Func_LO lo(this); } break;
						case 'H' : { Func_LH lh(this); } break;
						case '+' : { Func_LPlus lplus(this); } break;
						case 'B' : { Func_LB lb(this); } break;
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;
					
				case 'M' :
					switch(c2)
					{
						case 'A' : { Func_MA ma(this); } break;
						case 'D' : { Func_MD md(this); } break;
						case 'F' : { Func_MF mf(this); } break;

						case 'C' : { 
							FlushBuffer();
							Func_MC(this); } break;

						case 'N' : { Func_MN mn(this); } break;

						case 'Z' : { Func_MZ mz(this); } break;
						case 'T' : { 
								FlushBuffer();
								Func_MT mt(this); } break;

#if COMPOSER
						case 'I' : { Func_MI mi(this); } break;
#endif
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;
					
				case 'N' :
					FlushBuffer();
					switch(c2)
					{
						case'B' : { Func_NB nb(this); } break;
						case 'L' : { Func_NL nl(this); } break;
						case 'T' : { Func_NT nt(this); } break;
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'O' :
					switch(c2)
					{
						case 'E' : { Func_OE oe(this); } break;
						case 'T' : { Func_OT ot(this); } break;
						case 'S' : { Func_OS os(this); } break;
						case 'F' : { Func_OF of(this); } break;
						case 'N' : { Func_ON on(this); } break;
						case 'L' : { Func_OL on(this); } break;
						case 'V' : { Func_OV ov(this); } break;
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'P' :
					FlushBuffer();
					switch(c2)
					{
						case 'E' : { Func_PE pe(this); } break;
						case 'G' : { Func_PG pg(this); } break;
						case 'H' : { Func_PH ph(this); } break;
						case 'I' : { Func_PI ph(this); } break;
						case 'J' : { Func_PJ pj(this); } break;
						case 'F' : { Func_PF pf(this); } break;
						case 'A' : { Func_PA pa(this); } break;
						case 'P' : { Func_PP pp(this); } break;
						case 'R' : { Func_PR pr(this); } break;
						case 'T' : { Func_PT pt(this); } break;
						case 'C' : { Func_PC pc(this); } break;
						case 'K' : { Func_PK pk(this); } break;
						case 'L' : { Func_PL pl(this); } break;
						case 'M' : { Func_PM pm(this); } break;
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'R' :
					switch(c2)
					{
						case 'B' : { 
							FlushBuffer();
							Func_RB rb(this); } break;

						case '+' : { 
							FlushBuffer();
							Func_RPlus rplus(this); } break;
						case 'O' : { Func_RO ro(this); } break;

						case 'L' : { Func_RL rl(this); } break;

						case 'U' : { Func_RU ru(this); } break;

						case 'K' : { Func_RK rk(this); } break;

						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'S' :
					FlushBuffer();
					switch(c2)
					{
						case 'E' : { Func_SE se(this); } break;
						case 'N' : { Func_SN sn(this); } break;
						case 'B' : { Func_SB sb(this); } break;
						case 'D' : { Func_SD sd(this); } break;
						case 'F' : { Func_SF sf(this); } break;
						case 'S' : { Func_SS ss(this); } break;
						case 'Y' : { Func_SY sy(this); } break;

						case 'C' : { 
							FlushBuffer();
							Func_SC sc(this); } break;

						case 'H' : { 
							FlushBuffer();
							Func_SH sc(this); } break;

						case 'P' : { Func_SP sp(this); } break;
						case 'L' : { Func_SL sl(this); } break;
						case 'T' : { Func_ST st(this); } break;
#if HYPERLINKS
						case 'I' : { Func_SI si(this); } break;
#endif
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;
				
				case 'T' :
					if (c2 != 'B')
							FlushBuffer();
					switch(c2)
					{
						case 'D' : { Func_TD td(this); } break;
						case 'C' : { Func_TC tc(this); } break;
						case 'H' : { Func_TH th(this); } break;
						case 'V' : { Func_TV tv(this); } break;
						case 'A' : { Func_TA ta(this); } break;
						case 'L' : { Func_TL tl(this); } break;
						case 'T' : { Func_TT tt(this); } break;
						case 'N' : { Func_TN tn(this); } break;
						case 'O' : { Func_TO to(this); } break;
						case 'S' : { Func_TS ts(this); } break;
						case 'Z' : { Func_TZ tz(this); } break;
						case 'I' : { Func_TI ti(this); } break;
						case 'P' : { Func_TP tp(this); } break;
						case 'B' : { Func_TB tb(this); } break;
						case 'K' : { Func_TK tk(this); } break;
						case 'F' : { Func_TF tf(this); } break;
						case 'E' : { Func_TE te(this); } break;
						case 'G' : { Func_TG tg(this); } break;
						case 'J' : { Func_TJ tj(this); } break;
						case 'M' : { Func_TM tm(this); } break;
						case 'Y' : { Func_TY ty(this); } break;

						case '*' : { Func_TStar tstar(this); } break;

						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				case 'U' :
					
					switch(c2)
					{
						case 'L' : 
							{
								Func_UL ul (this);
								logfile = ul.QueryLogFileStream();
							}
							break;
					}
					break;

				case 'X' :
					FlushBuffer();
					switch(c2)
					{
						case 'A' : { Func_XA xx(this); } break;
						case 'X' : { Func_XX xx(this); } break;
						case 'C' : { Func_XC xc(this); } break;
						case 'D' : { Func_XD xd(this); } break;
						case 'H' : { Func_XH xh(this); } break;
						case 'R' : { Func_XR xr(this); } break;
#if XRAIL
						case 'Y' : { Func_XY xy(this); } break;
						case 'Z' : { Func_XZ xz(this); } break;
#endif

#if XREFS
						case '1' : { Func_X1 x1(this); } break;
						case '2' : { Func_X2 x1(this); } break;
						case '3' : { Func_X3 x1(this); } break;
#endif
						default :
							throw TCLError(PMString(kErrUnknownCommand));
							break;
					}
					break;

				
				case 'W' :
						FlushBuffer();
						switch(c2)
						{
							case 'W' : { Func_WW(this); } break;
							default :
								throw TCLError(PMString(kErrUnknownCommand));
								break;
						}
						break;
				
				default :
					throw TCLError(PMString(kErrUnknownCommand));
					break;
			}

		} 
		else  
		{			
			// It's a standard character
			InsertCharacter(reader->GetCurrentChar());
		}


#if XRAIL    
			// Pour ne pas qu'a la generation CdF sur InDD Desktop l'utilisateur ait le message
			// de changement d'etat page a chaque fermeture de nouveau document xrail genere,
			// PageMakeUp passe automatiquement le mode "robot" du plugin XRail a "vrai".
			// A la fin du traitement TCL, PageMakeUp remet le mode "robot" a "faux".
  
            // On recupere les dernieres preferences
           	InterfacePtr<IWorkspace> workspace(GetExecutionContextSession()->QueryWorkspace()); 
        	InterfacePtr<IXRailPrefsData>  xrailPrefsData ((IXRailPrefsData*)workspace->QueryInterface(IID_IXRAILPREFSDATA));	
      
			// XRail prefs
			PMString adresseIP = xrailPrefsData->GeturlXR();
	        PMString lastUser = xrailPrefsData->GetLastUser(); 

			// XPub prefs
			K2Vector<PMString>listeBases;
			K2Vector<PMString>IPBases;
#if MULTIBASES
            listeBases = xrailPrefsData->GetListeBases();
			IPBases = xrailPrefsData->GetIPBases();
#endif
            
            InterfacePtr<ICommand> setPrefsCmd(CmdUtils::CreateCommand(kXRLSetPrefsCmdBoss));
		    InterfacePtr<IXRailPrefsData> prefsData(setPrefsCmd, UseDefaultIID());   

            prefsData->SeturlXR(adresseIP);
  	    	prefsData->SetLastUser(lastUser);
	        prefsData->SetModeRobot(kFalse);  
#if MULTIBASES
			prefsData->SetListeBases(listeBases);     
			prefsData->SetIPBases(IPBases); 
#endif

		    if (CmdUtils::ProcessCommand(setPrefsCmd) != kSuccess)
			        ASSERT_FAIL("command setPrefsCmd failed");		    
#endif



	}		
	catch(TCLError& error)
	{			
		if(error.getMessage().Compare(kFalse,PMString(kErrEOF)) == 0) // Save the item list of the documents handled
		{
#if SERVER	
    
  			PMString info( FileUtils::SysFileToPMString(tclFile) + " has been processed");
			info.SetTranslatable(kFalse);						
			CAlert::InformationAlert(info);			
			
			// Remove this message from the error list (it should not be considered as an error)
			InterfacePtr<IErrorList> errorList (GetExecutionContextSession(), UseDefaultIID()); 
			errorList->RemoveError(errorList->GetNumErrors()-1);
#endif
			if(!prefs.stopDisplay)
				CAlert::InformationAlert(PMString(kEndExecuteTCL));	

			try // Add try / catch to prevent crash (if there is no front document when called)
			{
				FlushBuffer();					
			}
			catch(...){}				

			// Save ItemList and add adornments
			this->SaveItemList();						

			// Rename TCL file if required (DR command has been called)
			if(this->newFileName != "")
			{	
				PMString tclfileAbsName = FileUtils::SysFileToPMString(tclFile);

				if(newFileName.Compare(kFalse,tclfileAbsName) != 0)
				{					
					IDFile newFile;
					FileUtils::PMStringToIDFile(newFileName,newFile);

					FileUtils::SwapFiles(tclFile,newFile);
				}
			}				

			numError = 0;

			return kTrue;

		}
		
		ErrorUtils::PMSetGlobalErrorCode(kSuccess);

		// Save ItemList and add adornments
		this->SaveItemList();

		// Get error number
		numError = error.GetErrorCode();
		
		// Line number 
		PMString ErrorLine(kErrorLineKey);
		ErrorLine.Translate(); ErrorLine.Append(" ");
		ErrorLine.AppendNumber(reader->GetLineNumber());
		ErrorLine.Append(", ");

		PMString CommandName(kCommandKey);
		CommandName.Translate(); CommandName.Append(" ");
		PlatformChar pc1, pc2;
		pc1.Set(c1); 
		pc2.Set(c2);
		CommandName.Append(PlatformChar(pc1));
		CommandName.Append(PlatformChar(pc2));
#if SERVER
		CommandName.Insert(ErrorLine);
		CommandName.SetTranslatable(kFalse);
		CAlert::ErrorAlert(CommandName);
		CAlert::ErrorAlert(error.getMessage());
		
		PMString str = "Error occured while processing " + FileUtils::SysFileToPMString(tclFile) + " : ";
		str.AppendNumber(numError);
		str.SetTranslatable(kFalse);
		CAlert::InformationAlert(str);
#else
		CommandName.Append(".");
		CommandName.Append(kNewLine);
#endif
		// Commande name
		PMString message(error.getMessage());
		if(!message.HasTranslated())
			message.Translate();

		message.Insert(CommandName);
		message.Insert(ErrorLine);
		message.SetTranslated();
		error.setMessage(message);

		// write error in log file, if any
		WriteLogFile(message);	

		if(prefs.stopOnError)
		{
			if(!prefs.stopDisplay)
				error.DisplayError();
			return kTrue;
		}
	}
	return kFalse;
}

/* FinalizeProcess
*/
void PrsTCLParser::FinalizeProcess()
{
#if SERVER
	// Close all open documents
	// In InDesign Server mode, current doc is the latest opened document
	InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
	InterfacePtr<IDocumentList> documentList(application->QueryDocumentList());
	for(int32 i = 0 ; i < documentList->GetDocCount() ; ++i)
	{
		SDKLayoutHelper layoutHelper;
		layoutHelper.CloseDocument(::GetUIDRef(documentList->GetNthDoc(i)),kFalse,K2::kSuppressUI, kFalse, IDocFileHandler::kSchedule); 
	}
	
	CmdUtils::ProcessScheduledCmds();

	// Close all open books
	InterfacePtr<IBookManager> bookMgr (GetExecutionContextSession(), UseDefaultIID());

	for(int32 i = 0 ; i < bookMgr->GetBookCount() ; ++i)
	{
		InterfacePtr<ICommand> closeBookCmd (CmdUtils::CreateCommand(kCloseBookCmdBoss));
		closeBookCmd->SetItemList(UIDList(bookMgr->GetNthBook(i)));

		InterfacePtr<IBoolData> allowCancelData (closeBookCmd, IID_IALLOWCANCELCLOSEBOOKDATA);
		allowCancelData->Set(kFalse);

		InterfacePtr<IIntData> uiflagsData (closeBookCmd, IID_ICLOSEBOOKUIFLAG);
		uiflagsData->Set(K2::kSuppressUI);
		
		CmdUtils::ProcessCommand(closeBookCmd);	
	}
#else
	// Make sure current book has been closed
	this->setCurBook(UIDRef::gNull);

	// Refresh document view (mainly for references display)
	if(this->curDoc)
	{
		Utils<ILayoutUIUtils>()->InvalidateViews(this->curDoc);

		// Reset undo support on leaving
		this->curDB->BeginTransaction();
		InterfacePtr<ICommandMgr> cmdMgr (this->curDoc, UseDefaultIID());
		cmdMgr->SetUndoSupport(kTrue);
		this->curDB->EndTransaction();
	}
#endif
}

/* LaunchParser
*/
int32 PrsTCLParser::LaunchParser()
{	
	int32 numError = 0;

	if(reader == nil) return 9999;

	do
	{
		if(ProcessTCL(numError)) break;

	} while (kTrue);

	FinalizeProcess();

	return numError;
}

ErrorCode PrsTCLParser::getCurBoxUIDRef( UIDRef &theBox)
{
	ErrorCode status = kFailure;
	PMString error("");
	theBox=UIDRef(nil,kInvalidUID);
	do
	{
		// Test if a frame is selected
		if(this->curItem == kInvalidTCLRef)
		{
			error = PMString(kErrNoSelection);
			break;
		}
					
		InterfacePtr<IHierarchy> frameHier (UIDRef(this->curDB,this->getItem(this->curItem)),UseDefaultIID());
		if(frameHier == nil)
		{		
			ASSERT_FAIL("PrsTCLParser::getCurBoxUIDRef -> frameHier nil");
			error=PMString(kErrNilInterface);
			break;
		}

		if(Utils<IFrameUtils>()->IsTextFrame(frameHier))
		{
			InterfacePtr<IHierarchy> multiColHier (frameHier->QueryChild(0));
			if(multiColHier == nil)
			{		
				ASSERT_FAIL("PrsTCLParser::getCurBoxUIDRef -> multiColHier nil");
				error=PMString(kErrNilInterface);
				break;
			}

			theBox=::GetUIDRef(multiColHier);
			status = kSuccess; 
		}
		else
			error=PMString(kErrTextBoxRequired);
	
	}while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(kFailure, &error);
	
	return status ;
			
}

// Query the current text model and return the textindex of the end of text in the current text frame
// or in the current cell (must be a unique cell selection), according to the current selection
ITextModel * PrsTCLParser::QueryCurrentModelTextIndex(TextIndex& endOfStoryThread)
{
	ErrorCode status = kFailure;
	PMString error(kErrNilInterface);
	
	ITextModel * curTxtModel = nil;

	do
	{
	
		if(this->curCells != kZeroGridArea) // current selection is a cell selection
		{
			InterfacePtr<ITableModel> tableModel (curDB, this->curTable, UseDefaultIID());
			InterfacePtr<ITableTextContainer> tableTextContainer (tableModel, UseDefaultIID());

			curTxtModel = tableTextContainer->QueryTextModel(); // Process the AddRef, we don't wrap the QueryTextModel using InterfacePtr, cause this is a Query method as well

			// Get the story thread of the current cell (the top left cell in the area if it's a multi-cell selection)
			InterfacePtr<ITextStoryThreadDict> textStoryThreadDict(tableModel, UseDefaultIID());

			const GridID gridID = tableModel->GetGridID(this->curCells.GetTopLeft());
			InterfacePtr<ITextStoryThread> cellStoryThread (textStoryThreadDict->QueryThread(gridID));

			endOfStoryThread = cellStoryThread->GetTextEnd()-1;
		}
		else
		{
			// Test if a frame is selected
			if(this->curItem == kInvalidTCLRef)
			{
				error = PMString(kErrNoSelection);
				ErrorUtils::PMSetGlobalErrorCode(kFailure);
				break;
			}

			InterfacePtr<IHierarchy> frameHier (UIDRef(this->curDB,this->getItem(this->curItem)),UseDefaultIID());
			if(frameHier == nil) break;

			if(this->topSelected)
			{
				InterfacePtr<IMainItemTOPData> mainTOPData (frameHier, UseDefaultIID());
				if(mainTOPData && mainTOPData->GetTOPSplineItemUID() != kInvalidUID)
				{
					InterfacePtr<IMultiColumnTextFrame> txtFrame (mainTOPData->QueryTOPMCTextFrame());
					curTxtModel = txtFrame->QueryTextModel(); // Process the AddRef, we don't wrap the QueryTextModel using InterfacePtr, cause this is a Query method as well

					endOfStoryThread = curTxtModel->GetPrimaryStoryThreadSpan() - 1;	

					status = kSuccess; 
				}
				else
				{
					error=PMString(kErrTOPRequired);
					ErrorUtils::PMSetGlobalErrorCode(kFailure);
				}
			}
			else
			{
				if(Utils<IFrameUtils>()->IsTextFrame(frameHier))
				{
					InterfacePtr<IMultiColumnTextFrame> txtFrame (this->curDB, frameHier->GetChildUID(0), UseDefaultIID());
					if(txtFrame == nil) break;

					curTxtModel = txtFrame->QueryTextModel(); // Process the AddRef, we don't wrap the QueryTextModel using InterfacePtr, cause this is a Query method as well
					if(curTxtModel == nil) break;

					endOfStoryThread = curTxtModel->GetPrimaryStoryThreadSpan() - 1;	

					status = kSuccess; 
				}
				else
				{
					error=PMString(kErrTextBoxRequired);
					ErrorUtils::PMSetGlobalErrorCode(kFailure);
				}
			}
		}

	} while(kFalse);

	return curTxtModel;
}

void PrsTCLParser::FlushBuffer()
{
	if(!txtBuf.empty()) // Do nothing if no text
	{
		if(this->curCells != kZeroGridArea) // text will inserted in a table
		{	
			
			InterfacePtr<ITableModel> tableModel (curDB, this->curTable, UseDefaultIID());
			InterfacePtr<ITableTextContainer> tableTextContainer (tableModel, UseDefaultIID());
			InterfacePtr<ITextStoryThreadDict> textStoryThreadDict(tableModel, UseDefaultIID());

			const GridID gridID = tableModel->GetGridID(this->curCells.GetTopLeft());
			
			InterfacePtr<ITextStoryThread> cellStoryThread (textStoryThreadDict->QueryThread(gridID));
			
			InterfacePtr<ICommand> insertCmd (CmdUtils::CreateCommand(kPrsInsertTextCmdBoss));
			insertCmd->SetItemList(UIDList(tableTextContainer->GetTextModelRef()));

			InterfacePtr<IWideStringData> txtData (insertCmd, UseDefaultIID());
			txtData->Set(txtBuf);

			InterfacePtr<IIntData> positionData (insertCmd, UseDefaultIID());
			positionData->Set(cellStoryThread->GetTextEnd()-1);

			CmdUtils::ProcessCommand(insertCmd);
			
			txtBuf.Clear();
		}
		else
		{
			// Test if a frame is selected
			if(this->curItem == kInvalidTCLRef)
				throw TCLError(PMString(kErrNoSelection));
						
			InterfacePtr<IHierarchy> frameHier (UIDRef(this->curDB,this->getItem(this->curItem)),UseDefaultIID());
			if(frameHier == nil)
			{		
				ASSERT_FAIL("PrsTCLParser::getCurBoxUIDRef -> frameHier nil");
				throw TCLError(PMString(kErrNilInterface));
			}
			
			if(this->topSelected)
			{
				InterfacePtr<IMainItemTOPData> mainTOPData (frameHier, UseDefaultIID());
				if(mainTOPData && mainTOPData->GetTOPSplineItemUID() != kInvalidUID)
				{
					InterfacePtr<IMultiColumnTextFrame> txtFrame (mainTOPData->QueryTOPMCTextFrame());
					if(this->textFrameHelper->InsertTextAtEndCmd(::GetUIDRef(txtFrame),txtBuf, kFalse) != kSuccess)
					{
						ASSERT_FAIL("PrsTCLParser::FlushBuffer -> insert failed");
						throw TCLError(kErrCmdFailed);
					}										
					txtBuf.Clear();
				}
				// No error is thrown when there is no TOP
				txtBuf.Clear();				
			}
			else
			{
				if(Utils<IFrameUtils>()->IsTextFrame(frameHier))
				{
					InterfacePtr<IHierarchy> multiColHier (frameHier->QueryChild(0));
					if(multiColHier == nil)
					{		
						ASSERT_FAIL("PrsTCLParser::FlushBuffer -> multiColHier nil");
						throw TCLError(PMString(kErrNilInterface));
					}
					
					if(this->textFrameHelper->InsertTextAtEndCmd(::GetUIDRef(multiColHier),txtBuf, kFalse) != kSuccess)
					{
						ASSERT_FAIL("PrsTCLParser::FlushBuffer -> insert failed");
						throw TCLError(kErrCmdFailed);
					}										
					txtBuf.Clear();
				}
				else // Frame selected isn't a text frame
				{
					txtBuf.Clear();
					
					/* error isn't thrown (compatibility with Quark behavior) */
					//throw TCLError(PMString(kErrInsertTxt));
				}
			}
		}
	}
}


void PrsTCLParser::InsertCharacter(UTF32TextChar c)
{
	txtBuf.Append(c) ;
	if(txtBuf.Length() >= BUF_SIZE)
		FlushBuffer();
}


/* AddStyle
*/
ErrorCode PrsTCLParser::AddStyle(int32 styleNum, PMString styleName)
{
	ErrorCode status = kFailure;
	do
	{
		if (theInfoTbl->styleVect.size()>0)
		{
			//we verify that the key we add doesn't exist
			Vector::iterator iter = K2find(theInfoTbl->styleVect.begin(), theInfoTbl->styleVect.end(), styleNum);

			// If found the result is an iterator to the KeyValuePair with Key == styleNum.
			// If not found the result is v.end().
			
			if (iter != theInfoTbl->styleVect.end())//if the key already exists
				iter->Value() = styleName;
			else
				theInfoTbl->styleVect.insert(theInfoTbl->styleVect.end(), Elt(styleNum, styleName));
		}
		else
			theInfoTbl->styleVect.insert(theInfoTbl->styleVect.end(), Elt(styleNum, styleName));
		
		status = kSuccess ;

	} while(false);

	return status;
}


/* GetStyleName
*/
ErrorCode PrsTCLParser::GetStyleName(int32 styleNum,PMString& value)
{
	ErrorCode status = kFailure;
	do
	{
		//search of the style's name
		Vector::const_iterator iter = K2find(theInfoTbl->styleVect.begin(), theInfoTbl->styleVect.end(), styleNum);

		// If found the result is an iterator to the KeyValuePair with Key == styleNum.
		// If not found the result is v.end().
		
		if (iter == theInfoTbl->styleVect.end())//if the style doesn't exist
		{
			ASSERT_FAIL("PrsTCLParser::GetStyleName -> style doesn't exist");
			PMString error(kErrStyleNotFound);
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			break;
		}

		value = iter->Value();

		status = kSuccess ;

	} while(false);

	return status;
}



/* AddFont
*/
ErrorCode PrsTCLParser::AddFont(int32 fontNum, PMString fontName)
{
	ErrorCode status = kFailure;
	do
	{
		if (theInfoTbl->fontVect.size()>0)
		{
			//we verify that the key we add doesn't exist
			Vector::iterator iter = K2find(theInfoTbl->fontVect.begin(), theInfoTbl->fontVect.end(), fontNum);

			// If found the result is an iterator to the KeyValuePair with Key == fontNum.
			// If not found the result is theInfoTbl->fontVect.end().
			
			if (iter != theInfoTbl->fontVect.end())//if the key is already used
				iter->Value() = fontName;
			else
				theInfoTbl->fontVect.insert(theInfoTbl->fontVect.end(), Elt(fontNum, fontName));
		}
		else
			theInfoTbl->fontVect.insert(theInfoTbl->fontVect.end(), Elt(fontNum, fontName));

		status = kSuccess ;

	} while(false);

	return status;
}

/* AddCJ
*/
void PrsTCLParser::AddCJ(PMString name, bool8 hyphenMode, bool8 capitalized, int16 minWord,
						 int16 afterFirst, int16 beforeLast, int16 limit, PMReal hyphenZone, 
						 bool8 lastWord, PMReal wordMin, PMReal wordMax, PMReal wordDes, 
						 PMReal letterMin, PMReal letterMax, PMReal letterDes)
{

	//we verify the existence of the CJ's rule 
	//if it already exist a CJ's rule, we rewrite it with the new parameters
	//else we create a new CJ's rule

	if (theInfoTbl->CJVect == nil)//if it's the first add
	{
		CJPtr newCJ = new CJ() ;
		newCJ->name = name ;
		newCJ->hyphenMode = hyphenMode ;
		newCJ->capitalized = capitalized ;
		newCJ->minWord = minWord ;
		newCJ->afterFirst = afterFirst ;
		newCJ->beforeLast = beforeLast ;
		newCJ->limit = limit ;
		newCJ->hyphenZone = hyphenZone ;
		newCJ->lastWord = lastWord;
		newCJ->wordMin = wordMin ;
		newCJ->wordMax = wordMax ;
		newCJ->wordDes = wordDes ;
		newCJ->letterMin = letterMin ;
		newCJ->letterMax = letterMax ;
		newCJ->letterDes = letterDes ;
		newCJ->next = nil ;

		theInfoTbl->CJVect = newCJ;
	}
	else
	{
		CJPtr tmp = theInfoTbl->CJVect ;
		bool8 found = (tmp->name == name);

		while (tmp->next != nil && !found)
		{
			tmp = tmp->next ;

			if (tmp->name == name)
				found = kTrue ;
		}

		if (found) // we change parameters
		{
			tmp->hyphenMode = hyphenMode ;
			tmp->capitalized = capitalized ;
			tmp->minWord = minWord ;
			tmp->afterFirst = afterFirst ;
			tmp->beforeLast = beforeLast ;
			tmp->limit = limit ;
			tmp->hyphenZone = hyphenZone ;
			tmp->lastWord = lastWord;
			tmp->wordMin = wordMin ;
			tmp->wordMax = wordMax ;
			tmp->wordDes = wordDes ;
			tmp->letterMin = letterMin ;
			tmp->letterMax = letterMax ;
			tmp->letterDes = letterDes ;
		}
		else //we add at the end
		{
			CJPtr newCJ = new CJ() ;
			newCJ->name = name ;
			newCJ->hyphenMode = hyphenMode ;
			newCJ->capitalized = capitalized ;
			newCJ->minWord = minWord ;
			newCJ->afterFirst = afterFirst ;
			newCJ->beforeLast = beforeLast ;
			newCJ->limit = limit ;
			newCJ->hyphenZone = hyphenZone ;
			newCJ->lastWord = lastWord ;
			newCJ->wordMin = wordMin ;
			newCJ->wordMax = wordMax ;
			newCJ->wordDes = wordDes ;
			newCJ->letterMin = letterMin ;
			newCJ->letterMax = letterMax ;
			newCJ->letterDes = letterDes ;
			newCJ->next = nil;
			tmp->next = newCJ ;
		}

	}

}

/* getCJ
*/
ErrorCode PrsTCLParser::getCJ(PMString name, bool8& hyphenMode, bool8& capitalized, int16& minWord,
						 int16& afterFirst, int16& beforeLast, int16& limit, PMReal& hyphenZone, 
						 bool8& lastWord, PMReal& wordMin, PMReal& wordMax, PMReal& wordDes, 
						 PMReal& letterMin, PMReal& letterMax, PMReal& letterDes)
{
	ErrorCode status = kFailure;
	do
	{

		CJPtr tmp = theInfoTbl->CJVect ;
		bool8 found = kFalse ;

		while (tmp != nil && !found)
		{
			if (tmp->name == name)
				found = kTrue ;
			else
				tmp = tmp->next ;
		}

		if (!found)
		{
			ASSERT_FAIL("PrsTCLParser::GetCJ -> CJ doesn't exist");
			throw TCLError(PMString(kErrCJNotFound));
			break;
		}

		hyphenMode = tmp->hyphenMode ;
		capitalized = tmp->capitalized;
		minWord = tmp->minWord ;
		afterFirst = tmp->afterFirst ;
		beforeLast = tmp->beforeLast ;
		limit = tmp->limit ;
		hyphenZone = tmp->hyphenZone ;
		lastWord = tmp->lastWord ;
		wordMin = tmp->wordMin ;
		wordMax = tmp->wordMax ;
		wordDes = tmp->wordDes ;
		letterMin = tmp->letterMin  ;
		letterMax = tmp->letterMax  ;
		letterDes = tmp->letterDes ;

		status = kSuccess ;

	} while(false);

	return status;
}


void PrsTCLParser::deleteCJ()
{
	CJPtr tmp = theInfoTbl->CJVect ;
	while (tmp != nil)
	{
		CJPtr toDelete ;
		toDelete = tmp ;
		tmp = tmp->next ;
		delete toDelete ;
	}
}



/* GetFontName
*/
ErrorCode PrsTCLParser::GetFontName(int32 fontNum,PMString& value)
{
	ErrorCode status = kFailure;
	do
	{
		//search of the font's name
		Vector::const_iterator iter = K2find(theInfoTbl->fontVect.begin(), theInfoTbl->fontVect.end(), fontNum);

		// If found the result is an iterator to the KeyValuePair with Key == policeNum.
		// If not found the result is theInfoTbl->fontVect.end().
		
		if (iter == theInfoTbl->fontVect.end())//if the font doesn't exist
		{
			ASSERT_FAIL("PrsTCLParser::GetFontName -> font doesn't exist");
			throw TCLError(PMString(kErrFontNotFound));
			break;
		}

		value = iter->Value();

		status = kSuccess ;

	} while(false);

	return status;
}