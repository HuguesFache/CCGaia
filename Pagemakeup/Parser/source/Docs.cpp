/*
//	File:	Docs.cpp
//
//	Date:	24-Apr-2003
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"
#include "GlobalDefs.h"

// General includes
#include "AttributeBossList.h"
#include "TextID.h"
#include "ErrorUtils.h"
#include "ILayoutUtils.h"
#include "SpreadID.h"
#include "SDKLayoutHelper.h"
#include "LanguageID.h"
#include "SnapshotUtils.h"
#include "SnapshotUtilsEx.h"
#include "StreamUtil.h"
#include "FileUtils.h"
#include "TransformUtils.h"
#include "IAssignmentMgr.h"
#include "IAssignmentUtils.h"
#include "IItemLockData.h"
#include "IInCopyBridgeUtils.h"

// Interface includes
#include "IWorkSpace.h"
#include "ITextAttributes.h"
#include "IDocument.h"
#include "IStyleGroupManager.h"
#include "IHierarchy.h"
#include "ILayoutUtils.h"
#include "IPageSetupPrefs.h"
#include "IMarginPrefs.h"
#include "IColumnPrefs.h"
#include "ISpreadList.h"
#include "IMasterPage.h"
#include "IRealNumberData.h"
#include "IUIDData.h"
#include "IDocumentUIUtils.h"
#include "IPageList.h"
#include "ICompositionStyle.h"
#include "IGeometry.h"
#include "IPMStream.h"
#include "ICommandMgr.h"
#include "ISpread.h"
#include "IMasterSpreadList.h"
#include "ILayerList.h"
#include "IGuidePrefs.h"
#include "ITransformFacade.h"
#include "IMovePageData.h"
#include "IIntData.h"
#include "IUIFlagData.h"
#include "IDocumentUtils.h"

#if !SERVER
#include "ILayoutUIUtils.h"
#else
#include "IApplication.h"
#include "IDocumentList.h"
#endif 

// Project includes
#include "Docs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "IDocHelper.h"
#include "IStyleHelper.h"
#include "ITxtAttrHelper.h"
#include "ITextFrameHelper.h"
#include "IItemManager.h"
#include "IPageHelper.h"
#include "TCLError.h"

#include "CAlert.h"

/* Constructor
*/

Func_DN::Func_DN(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DN::Func_DN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		NomDoc = reader->ExtractString(50);
		NomDoc.SetTranslatable(kFalse);

		largeur = reader->ExtractReel('P','+',MIN_WIDTH,0);

		if(largeur < MIN_WIDTH)
			throw TCLError(PMString(kErrInfMin));

		if(largeur > MAX_WIDTH)
			throw TCLError(PMString(kErrSupMax));

		hauteur = reader->ExtractReel('P','+',MIN_HEIGHT,0);

		if(hauteur < MIN_HEIGHT)
			throw TCLError(PMString(kErrInfMin));

		if(hauteur > MAX_HEIGHT)
			throw TCLError(PMString(kErrSupMax));

		col = reader->ExtractInt(1,1,20);
		lGout = reader->ExtractReel('P','+',1,0);

		mg = reader->ExtractReel('P','+',1,0);
		md = reader->ExtractReel('P','+',1,0);
		mh = reader->ExtractReel('P','+',1,0);
		mb = reader->ExtractReel('P','+',1,0);

		nbPage = reader->ExtractInt(1,MIN_PAGE,MAX_PAGE);
		recto = reader->ExtractBooleen(kFalse);
		interAuto = reader->ExtractBooleen(kFalse);
		interPercent = reader->ExtractPourcent('+',20,0);
		langage = reader->ExtractInt(0,0,2);
		approcheAuto = reader->ExtractBooleen(kFalse);		
		pagesPerSpread = reader->ExtractInt(1, MIN_PAGE, MAX_PAGE);		
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DN(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* Destructor
*/
Func_DN::~Func_DN()
{
}

/* IDCall_DN
*/
void Func_DN::IDCall_DN(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error ("");
	UIDRef newDocUIDRef = UIDRef::gNull;

	do
	{
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_DN::IDCall_DN -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		// Get helper interfaces
		InterfacePtr<IDocHelper> docHelper (parser->QueryDocHelper());
		if(docHelper == nil)
		{		
			ASSERT_FAIL("Func_DN::IDCall_DN -> docHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IStyleHelper> styleHelper (parser->QueryStyleHelper());
		if(styleHelper == nil)
		{		
			ASSERT_FAIL("Func_DN::IDCall_DN -> styleHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_DN::IDCall_DN -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Save the item list for the future old front document
		parser->SaveItemList();

		// Create a new document
		PMRect pageSize (0.0,0.0,largeur,hauteur);
		if(recto)
			pagesPerSpread = 2;

		bool16 bWide;
		if (largeur < hauteur)
		{
			// Set up portrait orientation.
			bWide = kFalse;
		}
		else
		{
			// Set up landscape orientation.
			bWide = kTrue;
		}

		newDocUIDRef = docHelper->CreateNewDocCmd(pageSize, nbPage, pagesPerSpread, col, lGout, kFalse, mg, mh, md, mb, bWide);

		if(newDocUIDRef == UIDRef(nil,kInvalidUID)){
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Set the document name
		if(docHelper->SetDocName(newDocUIDRef,NomDoc) != kSuccess){
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		//Get spread list
		InterfacePtr<ISpreadList> spreadList (newDocUIDRef, UseDefaultIID());
		
		if(!recto && pagesPerSpread >= 2)
		{
			if(pagesPerSpread == 2) // Move the last page to first spread because in this case we want two pages on first spread
									// and after processing kNewDocCmdBoss with two pages per spread, we only have one...
			{
				// Get Page list
				InterfacePtr<IPageList> pageList (newDocUIDRef, UseDefaultIID());
				if(pageList->GetPageCount() > 1)
				{
					IDataBase * db = newDocUIDRef.GetDataBase();

					InterfacePtr<ICommand> movePageCmd (CmdUtils::CreateCommand(kMovePageCmdBoss));
					movePageCmd->SetItemList(UIDList(db, pageList->GetNthPageUID(pageList->GetPageCount()-1)));

					
					InterfacePtr<IMovePageData> movePageData (movePageCmd, UseDefaultIID());
					movePageData->Set(UIDRef(db,spreadList->GetNthSpreadUID(0)), 0);						

					InterfacePtr<IIntData> bindingSide (movePageCmd, IID_IBINDINGSIDE );
					bindingSide->Set(ISpread::kLeftSideOfBinding);

					if(CmdUtils::ProcessCommand(movePageCmd) != kSuccess)
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						break;
					}		
				}
			}

			// Set keep spread together property in this case
			UIDList spreadUIDList(newDocUIDRef.GetDataBase());
			int32 spreadCount = spreadList->GetSpreadCount();
			for(int32 i = 0 ; i < spreadCount ; ++i)
			{
				spreadUIDList.Append(spreadList->GetNthSpreadUID(i));
			}		

			InterfacePtr<ICommand> setIslandSpreadCmd (CmdUtils::CreateCommand(kSetIslandSpreadCmdBoss));
			setIslandSpreadCmd->SetItemList(spreadUIDList);

			InterfacePtr<IBoolData> activeIslandSpread (setIslandSpreadCmd, UseDefaultIID());
			activeIslandSpread->Set(kTrue);

			if(CmdUtils::ProcessCommand(setIslandSpreadCmd) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}	
		}										
		
#if !SERVER
		// Open a window for the document
		if(docHelper->OpenWindowCmd(newDocUIDRef) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
#endif

		InterfacePtr<IDocument> newDoc (newDocUIDRef,UseDefaultIID());
		InterfacePtr<ICommand> shuffle (Utils<ILayoutUtils>()->MakeShuffleCmd(newDoc));
		if(CmdUtils::ProcessCommand(shuffle) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		UIDRef docWorkSpaceRef = newDoc->GetDocWorkSpace();

		AttributeBossList * attrList = new AttributeBossList();

		LanguageID lang;
		// Default language attribute
		switch (langage)
		{
			case 0 : lang = MakeLanguageID(kLanguageFrench,kSubLanguageFrench); break;
			case 1 : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishUK); break;
			case 2 : lang = MakeLanguageID(kLanguageGerman,kSubLanguageDefault); break;
			default : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishUK); break;
		}

		if(txtAttrHelper->AddLanguageAttribute(attrList, newDoc, lang) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Set autoleading attribute (on / off);
		PMReal value = 12.0; // default : 12 pt leading
		if(interAuto)
			value = -1.0; // -1 to set auto leading
		
		if(txtAttrHelper->AddAutoLeadModeAttribute(attrList,value) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Set percentage for autoleading
		if(txtAttrHelper->AddAutoLeadAttribute(attrList,interPercent) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Set Pair/Kern
		value = 0.0;
		if(approcheAuto)
			value = kAutoKernMagicNumber;
		
		if(txtAttrHelper->AddPairKernAttribute(attrList,value) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Edit default paragraph style called "NormalParagraphStyle" internally, "[Basic Paragraph]" in the GUI
		InterfacePtr<IStyleGroupManager> paraStyleNameTable (docWorkSpaceRef,IID_IPARASTYLEGROUPMANAGER);
		if(paraStyleNameTable == nil)
		{		
			ASSERT_FAIL("Func_DN::IDCall_DN -> paraStyleNameTable nil");
			error = PMString(kErrNilInterface);
			break;
		}

		PMString defaultStyleName("NormalParagraphStyle",PMString::kUnknownEncoding);
		UID defaultStyleUID = paraStyleNameTable->FindByName(defaultStyleName);
		if (defaultStyleUID == kInvalidUID)
				defaultStyleUID = paraStyleNameTable->GetDefaultStyleUID();

		if(styleHelper->EditTextStyleAttributesCmd(docWorkSpaceRef,defaultStyleUID,attrList) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		delete attrList;

		status = kSuccess;

	} while (false); 
	
	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			
			// Reset undo support for ex current document, uf any
			if(parser->getCurDoc())
			{
				parser->getCurDataBase()->BeginTransaction();
				InterfacePtr<ICommandMgr> cmdMgr (parser->getCurDoc(), UseDefaultIID());
				cmdMgr->SetUndoSupport(kTrue);
				parser->getCurDataBase()->EndTransaction();
			}

			InterfacePtr<IDocument> newDoc (newDocUIDRef,UseDefaultIID());
			parser->setCurDoc(newDoc);

			// BUF FIX : 17/09/2007 : process here any scheduled command that could have been launched in DuringOpenDoc responder (see PrsRespService.cpp)
			CmdUtils::ProcessScheduledCmds();
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);

}




Func_DF::Func_DF(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DF::Func_DF -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		docObligatoire = reader->ExtractBooleen(kFalse);
		numBloc = reader->ExtractInt(0,0,MAX_BLOC);
		dechainer = reader->ExtractBooleen(kFalse);
		blocRelative = reader->ExtractBooleen(kFalse);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		message.SetTranslated();
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DF(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}


Func_DF::~Func_DF()
{
}


void Func_DF::IDCall_DF(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Get helper interfaces
		InterfacePtr<ITextFrameHelper> txtFrameHelper (parser->QueryTextFrameHelper());
		if(txtFrameHelper == nil)
		{		
			ASSERT_FAIL("Func_DF::IDCall_DF -> txtFrameHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Get the curent document
		IDocument * curDoc = parser->getCurDoc();

		// Check the conditions
		if(blocRelative)
		{
			docObligatoire = kTrue;
			if(numBloc == 0)
				numBloc = 1;
		}
		
		if(docObligatoire && curDoc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		if(numBloc > 0)
		{
			if(docObligatoire && parser->getNbItemRegisteredAtBeginning() < numBloc)
			{
				error = PMString(kErrNotEnoughItem);
				break;
			}
		}

		if(dechainer && parser->getNbItemRegisteredAtBeginning() >=2) 
		{
			// If there are at least 2 frames, they are going to be unlinked
			IDataBase * db = parser->getCurDataBase();
			ICommandSequence * seq = CmdUtils::BeginCommandSequence();
			if(seq == nil)
			{		
				ASSERT_FAIL("Func_DF::IDCall_DF -> seq nil");
				error = PMString(kErrNilInterface);
				break;
			}

			seq->SetUndoability(ICommand::kAutoUndo);
			
			ErrorCode unlinkStatus = kSuccess;
#if REF_AS_STRING
			TCLRef i;
			i.AppendNumber(parser->getNbItemRegisteredAtBeginning() - 1);
#else
			TCLRef i = parser->getNbItemRegisteredAtBeginning() - 1;
#endif
			while(i >= kZeroTCLRef)
			{
				InterfacePtr<IHierarchy> splineHier (db,parser->getItem(i),UseDefaultIID());
				if(splineHier == nil)
				{		
					ASSERT_FAIL("Func_DF::IDCall_DF -> splineHier nil");
					error = PMString(kErrNilInterface);
					unlinkStatus = kFailure;
					break;
				}

				InterfacePtr<IHierarchy> multiColumnHier (splineHier->QueryChild(0));
				if(multiColumnHier == nil)
				{		
					error = PMString(kErrTextBoxRequired);
					unlinkStatus = kFailure;
					break;
				}
				
				if(txtFrameHelper->UnLinkFramesCmd(::GetUIDRef(multiColumnHier)) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					unlinkStatus = kFailure;
					break;
				}

				--i;
			}

			status = unlinkStatus;

			if (seq != nil)
			{
				if (unlinkStatus != kSuccess)
					ErrorUtils::PMSetGlobalErrorCode(kFailure);
				
				CmdUtils::EndCommandSequence(seq);
			}		
		}
		else
			status = kSuccess;

	} while(false);
		
	if(status != kSuccess)
		throw TCLError(error);
}



/* Constructor
*/

Func_DM::Func_DM(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DM::Func_DM -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		largeur = reader->ExtractReel('P','+',MIN_WIDTH,0);

		if(largeur < MIN_WIDTH)
			throw TCLError(PMString(kErrInfMin));

		if(largeur > MAX_WIDTH)
			throw TCLError(PMString(kErrSupMax));

		hauteur = reader->ExtractReel('P','+',MIN_HEIGHT,0);

		if(hauteur < MIN_HEIGHT)
			throw TCLError(PMString(kErrInfMin));

		if(hauteur > MAX_HEIGHT)
			throw TCLError(PMString(kErrSupMax));

		col = reader->ExtractInt(1,1,20);

		lGout = reader->ExtractReel3('+','P',1,0,PMReal(MIN_GUTTER),PMReal(MAX_GUTTER));

		mg = reader->ExtractReel3('+','P',1,0,PMReal(MIN_GUTTER),PMReal(MAX_GUTTER));
		md = reader->ExtractReel3('+','P',1,0,PMReal(MIN_GUTTER),PMReal(MAX_GUTTER));
		mh = reader->ExtractReel3('+','P',1,0,PMReal(MIN_GUTTER),PMReal(MAX_GUTTER));
		mb = reader->ExtractReel3('+','P',1,0,PMReal(MIN_GUTTER),PMReal(MAX_GUTTER));

		nbPage = reader->ExtractInt(1,MIN_PAGE,MAX_PAGE);

		interAuto = reader->ExtractBooleen(kFalse);
		interPercent = reader->ExtractPourcent('+',20,0);
		langage = reader->ExtractInt(0,0,2);
		approcheAuto = reader->ExtractBooleen(kFalse);
		
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DM(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* Destructor
*/
Func_DM::~Func_DM()
{
}

/* IDCall_DM
*/
void Func_DM::IDCall_DM(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error ("");

	do
	{
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_DM::IDCall_DM -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IDocHelper> docHelper (parser->QueryDocHelper());
		if(docHelper == nil)
		{		
			ASSERT_FAIL("Func_DM::IDCall_DM -> docHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IStyleHelper> styleHelper (parser->QueryStyleHelper());
		if(styleHelper == nil)
		{		
			ASSERT_FAIL("Func_DM::IDCall_DM -> styleHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IPageHelper> pageHelper(parser->QueryPageHelper());
		if(pageHelper == nil)
		{		
			ASSERT_FAIL("Func_GS::IDCall_GS-> pageHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_DM::IDCall_DM -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		IDocument * doc = parser->getCurDoc();
		IDataBase * db = parser->getCurDataBase();


		InterfacePtr<ISpreadList> spreadList(doc, UseDefaultIID());
		if(spreadList == nil)
		{		
			ASSERT_FAIL("ITCLParser::GetNbPage-> spreadList nil");
			error = PMString(kErrNilInterface);
			break;
		}

		// Modify the front document
		PMRect pageSize (0.0,0.0,largeur,hauteur);

		bool16 bWide;
		if (largeur < hauteur)
		{
			// Set up portrait orientation.
			bWide = kFalse;
		}
		else
		{
			// Set up landscape orientation.
			bWide = kTrue;
		}

		UIDRef docUIDRef = ::GetUIDRef(doc);

		// Set the new pages' size
		if(docHelper->SetPageSizeCmd(docUIDRef,pageSize,bWide) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Iterate over all the spreads
		int32 spreadCount = spreadList->GetSpreadCount();

		bool8 quit = kFalse ;
		int32 posToInsert = -1 ;
		int32 childCount = 0 ;
		int nb = 0 ;
		UIDRef spreadUIDRef;
		UIDRef masterPageUIDRef;
		UIDList * pageList = new UIDList(db) ;
		int32 nbPageInDoc = 0 ;
		UIDRef curpageUIDRef;

		// Get number of pages
		InterfacePtr<IPageList> docPageList (doc, UseDefaultIID());
		if(docPageList == nil)
		{
			error = PMString(kErrNilInterface);
			break;
		}
		
		nbPageInDoc = docPageList->GetPageCount();

		for ( int32 spreadIndex = 0; spreadIndex < spreadCount; spreadIndex++ )
		{
			// Get the spread
			spreadUIDRef = UIDRef(db, spreadList->GetNthSpreadUID(spreadIndex));
			InterfacePtr<IHierarchy> spreadHierarchy(spreadUIDRef, UseDefaultIID());
			if (spreadHierarchy == nil)
			{
				ASSERT_FAIL("Func_DM::IDCall_DM -> spreadHierarchy nil");
				error = PMString(kErrNilInterface);
				quit = kTrue ;
				break;
			}

			childCount = spreadHierarchy->GetChildCount();
			//We want to get the first child of the spread (this child is the spreadLayer which contains pages)

			if (childCount < 1)
			{
				error = PMString(kErrSpreadHierarchy);
				quit = kTrue;
				break;
			}

			InterfacePtr<IHierarchy> spreadLayerHierarchy(spreadHierarchy->QueryChild(0));
			if (spreadLayerHierarchy == nil)
			{
				ASSERT_FAIL("Func_DM::IDCall_DM -> spreadLayerHierarchy nil");
				error = PMString(kErrNilInterface);
				quit = kTrue ;
				break;
			}

			childCount = spreadLayerHierarchy->GetChildCount();

			for ( int32 pageIndex = 0; pageIndex < childCount; pageIndex++ )
			{
				nb = nb + 1 ;
				if (nb > nbPage)// Gestion of the number of page in the document
				{
					// GD 01.03.04 ++
					curpageUIDRef = UIDRef(db,spreadLayerHierarchy->GetChildUID(pageIndex));
					if(pageHelper->DeletePageCmd(curpageUIDRef) != kSuccess)
					//if(pageHelper->DeletePageCmd(UIDRef(db,spreadLayerHierarchy->GetChildUID(pageIndex))) != kSuccess)
					// GD 01.03.04 --
					{
						error = ErrorUtils::PMGetGlobalErrorString();
						quit = kTrue ;
						break;
					}
				}
				else 
				{
					if (pageIndex == 0)//we take the masterPage of the spread only on the first page (all the pages of a spread have the the same master page)
					{
						InterfacePtr<IMasterPage> masterPage(UIDRef(db,spreadLayerHierarchy->GetChildUID(pageIndex)),IID_IMASTERPAGE);
						if (masterPage == nil)
						{
							ASSERT_FAIL("Func_DM::IDCall_DM -> masterPage nil");
							error = PMString(kErrNilInterface);
							quit = kTrue ;
							break;
						}
						masterPageUIDRef = UIDRef(db,masterPage->GetMasterSpreadUID());
					}
					pageList->Append(spreadLayerHierarchy->GetChildUID(pageIndex));
					
				}
			}

			if (quit)
				break;
		
		} // iterate spreads

		if (quit)
			break;

		//gestion of the number of page 
		if (nbPage>nbPageInDoc) //we have to add Page(s) to the document
		{
			if(pageHelper->CreateNewPageUIDListCmd(spreadUIDRef,(nbPage-nbPageInDoc),childCount,masterPageUIDRef,pageList) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
		}

		//Set margin values
		if(docHelper->SetMarginCmd(*pageList,mg,mh,md,mb) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Column positions are relative to the left margin.
		PMRealList pageCols;
		pageCols.clear();
		PMReal colStart = 0;	//left edge
		pageCols.push_back(colStart);
		PMReal colWidth = (largeur-mg-md - (col-1) * lGout)/col;
	
		for( int16 j = 0; j < col - 1; j++ )
		{
			colStart += colWidth;
			pageCols.push_back(colStart);
			colStart += lGout;
			pageCols.push_back(colStart);
		}
		if (col > 0)
			pageCols.push_back(largeur-md);

		//Set column values
		if(docHelper->SetColumnCmd(*pageList,pageCols) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		delete pageList ;

		InterfacePtr<IWorkspace> iWorkspace (doc->GetDocWorkSpace(),IID_IWORKSPACE);
		if (iWorkspace == nil)
		{
			ASSERT_FAIL("Func_DM::IDCall_DM -> iWorkspace nil");
			error = PMString(kErrNilInterface);
			break;
		}

		AttributeBossList * attrList = new AttributeBossList();

		// Default language attribute
		LanguageID lang;
		// Default language attribute
		switch (langage)
		{
			case 0 : lang = MakeLanguageID(kLanguageFrench,kSubLanguageFrench); break;
			case 1 : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishUK); break;
			case 2 : lang = MakeLanguageID(kLanguageGerman,kSubLanguageDefault); break;
			default : lang = MakeLanguageID(kLanguageEnglish,kSubLanguageEnglishUK); break;
		}

		if(txtAttrHelper->AddLanguageAttribute(attrList, doc,lang) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Set autoleading attribute (on / off);
		PMReal value = 12.0; // default : 12 pt leading
		if(interAuto)
			value = -1.0; // -1 to set auto leading
		
		if(txtAttrHelper->AddAutoLeadModeAttribute(attrList,value) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Set percentage for autoleading
		if(txtAttrHelper->AddAutoLeadAttribute(attrList,interPercent) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Set Pair/Kern
		value = 0.0; 
		if(approcheAuto)
			value = kAutoKernMagicNumber; 
		
		if(txtAttrHelper->AddPairKernAttribute(attrList,value) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Edit default paragraph style called "NormalParagraphStyle" internally, "[Basic Paragraph]" in the GUI
		InterfacePtr<IStyleGroupManager> paraStyleNameTable (::GetUIDRef(iWorkspace),IID_IPARASTYLEGROUPMANAGER);
		if(paraStyleNameTable == nil)
		{		
			ASSERT_FAIL("Func_DN::IDCall_DN -> paraStyleNameTable nil");
			error = PMString(kErrNilInterface);
			break;
		}

		PMString defaultStyleName("NormalParagraphStyle",PMString::kUnknownEncoding);
		UID defaultStyleUID = paraStyleNameTable->FindByName(defaultStyleName);
		if (defaultStyleUID == kInvalidUID)
				defaultStyleUID = paraStyleNameTable->GetDefaultStyleUID();

		if(styleHelper->EditTextStyleAttributesCmd(::GetUIDRef(iWorkspace),defaultStyleUID,attrList) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		delete attrList;

		status = kSuccess ;


	} while (false); 
	
	if (seq != nil)
	{
		if (status == kSuccess)
		{
			CmdUtils::EndCommandSequence(seq);
			parser->setCurPage(1);
		}
		else
		{
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
			CmdUtils::EndCommandSequence(seq);
			throw TCLError(error);
		}
	}
	else
		throw TCLError(error);

}



/* Constructor
*/

Func_DS::Func_DS(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DS::Func_DS -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		name = reader->ExtractString(255);
		closeDoc = reader->ExtractBooleen(kFalse);
		
		// dummy parameter, only for quark compatibility
		reader->ExtractInt(0,0,MAX_BLOC);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DS(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* Destructor
*/
Func_DS::~Func_DS()
{
}

/* IDCall_DS
*/
void Func_DS::IDCall_DS(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error ("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IDocHelper> docHelper (parser->QueryDocHelper());
		if(docHelper == nil)
		{		
			ASSERT_FAIL("Func_DS::IDCall_DS -> docHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		UIDRef docUIDRef = ::GetUIDRef(parser->getCurDoc());
		
		// Save item list
		if(parser->SaveItemList() != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		// Get Preferences
		bool8 useSavePath;
		PMString savePath;
		
		parser->getSavePrefs(&useSavePath,&savePath);
		
		if(useSavePath)
			name.Insert(savePath);

		//save the document
		if(docHelper->SaveAsDoc(docUIDRef,name) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
#if XPAGE
		
		InterfacePtr<IAssignmentMgr> assignMgr (GetExecutionContextSession(), UseDefaultIID());	
		if(assignMgr->SaveAssignments(parser->getCurDoc(), kSuppressUI, kTrue) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(Utils<IDocumentUtils>()->DoSave(parser->getCurDoc()) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
#endif 
		if (closeDoc)
		{
			SDKLayoutHelper layoutHelper;
			// Close doc must be scheduled, otherwise ID may crash
			status = layoutHelper.CloseDocument(::GetUIDRef(parser->getCurDoc()),kFalse,K2::kSuppressUI, kFalse, IDocFileHandler::kSchedule); 			
		}
		else
			status = kSuccess ;

	} while (false); 

	if (status == kSuccess)
	{
		if(closeDoc)
		{
			CmdUtils::ProcessScheduledCmds(); // avoid a bug that occurs when the doc is immediatly re-opened after closing
#if !SERVER
			parser->setCurDoc(Utils<ILayoutUIUtils>()->GetFrontDocument());
#else
			// In InDesign Server mode, current doc is the latest opened document (before the one which has just been closed)
			InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
			InterfacePtr<IDocumentList> documentList(application->QueryDocumentList());
			if(documentList->GetDocCount() > 0)
				parser->setCurDoc(documentList->GetNthDoc(documentList->GetDocCount()-1));
			else
				parser->setCurDoc(nil);
#endif
		}
	}
	else
		throw TCLError(error);
}

/* Constructor
*/

Func_DE::Func_DE(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DE::Func_DE -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		superScriptPos = reader->ExtractPourcent('+',33,0);
		superScriptSize = reader->ExtractPourcent('+',100,0);	
		
		// Parameter not taken into account with InDesign (functionnality doesn't exist)
		// just here to ensure compatibility with TCL files generated for Quark 
		reader->ExtractPourcent('+',100,0);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DE(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* Destructor
*/
Func_DE::~Func_DE()
{
}

/* IDCall_DE
*/
void Func_DE::IDCall_DE(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error ("");

	do
	{
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_DE::IDCall_DE -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);

		IDocument * doc = parser->getCurDoc();
		
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		InterfacePtr<ICommand> sizeCmd (CmdUtils::CreateCommand(kSuperScriptSizeCmdBoss));
		if(sizeCmd == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		InterfacePtr<IRealNumberData> sizeData (sizeCmd, UseDefaultIID());
		if(sizeData == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		sizeData->Set(superScriptSize * 100.0);
		
		InterfacePtr<IUIDData> sizeTarget (sizeCmd, UseDefaultIID());
		if(sizeTarget == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		sizeTarget->Set(doc->GetDocWorkSpace());
		
		// Process the SuperScriptSizeCmd:
		if (CmdUtils::ProcessCommand(sizeCmd) != kSuccess)
		{	
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		InterfacePtr<ICommand> posCmd (CmdUtils::CreateCommand(kSuperScriptPosCmdBoss));
		if(posCmd == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		InterfacePtr<IRealNumberData> posData (posCmd, UseDefaultIID());
		if(posData == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		posData->Set(superScriptPos * 100.0);
		
		InterfacePtr<IUIDData> posTarget (posCmd, UseDefaultIID());
		if(posTarget == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		posTarget->Set(doc->GetDocWorkSpace());
		
		// Process the SuperScriptPosCmd:
		if (CmdUtils::ProcessCommand(posCmd) != kSuccess)
		{	
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess ;
		
	} while(kFalse);
	
	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
		
		CmdUtils::EndCommandSequence(seq);		
	}
	else
		throw TCLError(error);
}

/* Constructor
*/

Func_DK::Func_DK(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DK::Func_DK -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		smallCapSize = reader->ExtractPourcent('+',100,0);		
		
		// Parameter not taken into account with InDesign (functionnality doesn't exist)
		// just here to ensure compatibility with TCL files generated for Quark 
		reader->ExtractPourcent('+',100,0);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DK(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* Destructor
*/
Func_DK::~Func_DK()
{
}

/* IDCall_DK
*/
void Func_DK::IDCall_DK(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error ("");

	do
	{
		IDocument * doc = parser->getCurDoc();
		
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		InterfacePtr<ICommand> sizeCmd (CmdUtils::CreateCommand(kSmallCapSizeCmdBoss));
		if(sizeCmd == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		sizeCmd->SetUndoability(ICommand::kAutoUndo);
		
		InterfacePtr<IRealNumberData> sizeData (sizeCmd, UseDefaultIID());
		if(sizeData == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		sizeData->Set(smallCapSize * 100.0);
		
		InterfacePtr<IUIDData> sizeTarget (sizeCmd, UseDefaultIID());
		if(sizeTarget == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		sizeTarget->Set(doc->GetDocWorkSpace());
		
		// Process the SubScriptPosCmd:
		if (CmdUtils::ProcessCommand(sizeCmd) != kSuccess)
		{	
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		status = kSuccess;
		
	} while(kFalse);
	
	
	if (status != kSuccess)
	{
		throw TCLError(error);
	}
}


/* Constructor
*/
Func_DH::Func_DH(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DH::Func_DH -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		subScriptPos = reader->ExtractPourcent('+',33,0);
		subScriptSize = reader->ExtractPourcent('+',100,0);
		
		// Parameter not taken into account with InDesign (functionnality doesn't exist)
		// just here to ensure compatibility with TCL files generated for Quark 
		reader->ExtractPourcent('+',100,0);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DH(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* Destructor
*/
Func_DH::~Func_DH()
{
}

/* IDCall_DH
*/
void Func_DH::IDCall_DH(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence * seq = nil;
	PMString error ("");

	do
	{
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
		{		
			ASSERT_FAIL("Func_DH::IDCall_DH -> seq nil");
			error = PMString(kErrNilInterface);
			break;
		}

		seq->SetUndoability(ICommand::kAutoUndo);
			
		IDocument * doc = parser->getCurDoc();
		
		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		InterfacePtr<ICommand> sizeCmd (CmdUtils::CreateCommand(kSubScriptSizeCmdBoss));
		if(sizeCmd == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		InterfacePtr<IRealNumberData> sizeData (sizeCmd, UseDefaultIID());
		if(sizeData == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		sizeData->Set(subScriptSize * 100.0);
		
		InterfacePtr<IUIDData> sizeTarget (sizeCmd, UseDefaultIID());
		if(sizeTarget == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		sizeTarget->Set(doc->GetDocWorkSpace());
		
		// Process the SubScriptSizeCmd:
		if (CmdUtils::ProcessCommand(sizeCmd) != kSuccess)
		{	
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		InterfacePtr<ICommand> posCmd (CmdUtils::CreateCommand(kSubScriptPosCmdBoss));
		if(posCmd == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		InterfacePtr<IRealNumberData> posData (posCmd, UseDefaultIID());
		if(posData == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		posData->Set(subScriptPos * 100.0);
		
		InterfacePtr<IUIDData> posTarget (posCmd, UseDefaultIID());
		if(posTarget == nil)
		{
			error = PMString(kErrNilInterface);
			break;	
		}
		
		posTarget->Set(doc->GetDocWorkSpace());
		
		// Process the SubScriptPosCmd:
		if (CmdUtils::ProcessCommand(posCmd) != kSuccess)
		{	
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess ;
		
	} while(kFalse);
	
	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
		
		CmdUtils::EndCommandSequence(seq);
	}
	else
		throw TCLError(error);
}

/* Constructor
*/
Func_DB::Func_DB(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DH::Func_DH -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->FiltreBlanc();
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DB(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* Destructor
*/
Func_DB::~Func_DB()
{
}

/* IDCall_DB
*/
void Func_DB::IDCall_DB(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error;
	
	do
	{		
		SDKLayoutHelper layoutHelper;
		// Close doc must be scheduled, otherwise ID may crash
		status = layoutHelper.CloseDocument(::GetUIDRef(parser->getCurDoc()),kFalse,K2::kSuppressUI, kFalse, IDocFileHandler::kSchedule); 
		
	} while(kFalse);
	
	if(status == kSuccess)
	{
		CmdUtils::ProcessScheduledCmds(); // avoid a bug that occurs when the doc is immediatly re-opened after closing
#if !SERVER
		parser->setCurDoc(Utils<ILayoutUIUtils>()->GetFrontDocument());
#else
		// In InDesign Server mode, current doc is the latest opened document (before the one which has just been closed)
		InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
		InterfacePtr<IDocumentList> documentList(application->QueryDocumentList());
		if(documentList->GetDocCount() > 0)
			parser->setCurDoc(documentList->GetNthDoc(documentList->GetDocCount()-1));
		else
			parser->setCurDoc(nil);
#endif
	}
	else
		throw TCLError(error);
}


/* Constructor
*/
Func_DO::Func_DO(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DO::Func_DO -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		
		name = reader->ExtractString(255);
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DO(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* Destructor
*/
Func_DO::~Func_DO()
{
}

/* IDCall_DO
*/
void Func_DO::IDCall_DO(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	PMString error;
	UIDRef docRef = UIDRef::gNull;
	
	do
	{
		InterfacePtr<IDocHelper> docHelper (parser->QueryDocHelper());
		if(docHelper == nil)
		{		
			ASSERT_FAIL("Func_DO::IDCall_DO -> docHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		// Save item list of the future old front doc, if any
		parser->SaveItemList();
		
		// Get open doc preferences
		bool8 usePath;
		PMString openPath;
		
		parser->getOpenPrefs(&usePath,&openPath);
		
		if(usePath)
			name.Insert(openPath);
		
		IDFile fileToOpen;
		if(docHelper->ConvertPath(name, fileToOpen) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
		
		SDKLayoutHelper layoutHelper;
		docRef = layoutHelper.OpenDocument(fileToOpen,K2::kSuppressUI);
		
		if(docRef == UIDRef::gNull)
		{
			error = PMString(kErrOpenDoc);
			break;
		}
#if !SERVER
		// Open a window for the document
		status = layoutHelper.OpenLayoutWindow(docRef);
#else
		status = kSuccess;
#endif
		if(status != kSuccess)
			error = PMString(kErrOpenDoc);
		
		
	} while(kFalse);
	
	if(status == kSuccess)
	{
		// Reset undo support for ex current document, if any
		if(parser->getCurDoc())
		{
			parser->getCurDataBase()->BeginTransaction();
			InterfacePtr<ICommandMgr> cmdMgr (parser->getCurDoc(), UseDefaultIID());
			cmdMgr->SetUndoSupport(kTrue);
			parser->getCurDataBase()->EndTransaction();
		}

		InterfacePtr<IDocument> openedDoc (docRef, UseDefaultIID());
		parser->setCurDoc(openedDoc);       

#if AFFECT

         InterfacePtr<IAssignmentMgr> assignmentMgr (GetExecutionContextSession(), UseDefaultIID());	
       	 IDataBase * db = ::GetDataBase(openedDoc);		
         IAssignedDocument * assignDoc = assignmentMgr->GetAssignedDocument(db);
	
         UIDList storyList(db);
		 assignmentMgr->GetStoryListOnDoc(assignDoc, storyList);
       	 UIDList unlockList(db);
     
	     for(int32 i = 0 ; i < storyList.size() ; ++i){
               	InterfacePtr<IItemLockData> lockData (storyList.GetRef(i), UseDefaultIID()); 
			    if(lockData->GetInsertLock()) unlockList.Append(storyList.At(i));							
         }		

		 if(!unlockList.IsEmpty()){
			    InterfacePtr<ICommand> editStoryCmd (CmdUtils::CreateCommand(kICBridgeEditInPlaceCmdBoss));		
			    editStoryCmd->SetItemList(unlockList);
			    InterfacePtr<IUIFlagData> uiflags (editStoryCmd, UseDefaultIID());
			    uiflags->Set(kSuppressUI);
			    CmdUtils::ProcessCommand(editStoryCmd);
		 }  

#endif         
   		// BUF FIX : 17/09/2007 : process here any scheduled command that could have been launched in DuringOpenDoc responder (see PrsRespService.cpp)
		CmdUtils::ProcessScheduledCmds();
	}
	else
		throw TCLError(error);	
}

/* Constructor
*/
Func_CP::Func_CP(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CP::Func_CP -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		
		filename = reader->ExtractString(255);
		numPage = reader->ExtractInt(0,0,MAX_PAGE);
		scale = reader->ExtractInt(0,0,250);
		width = reader->ExtractInt(0,0,2000);
		height = reader->ExtractInt(0,0,2000);
		
#if REF_AS_STRING
		refItem = reader->ExtractTCLRef(kInvalidTCLRef);
#else
		refItem = reader->ExtractTCLRef(kZeroTCLRef);
#endif
		quality = reader->ExtractInt(0,0,5);
		desiredRes = reader->ExtractInt(72,30,180);
		formatExport = reader->ExtractInt(1, 1, 3);
		encoding = reader->ExtractInt(BASELINE_JPEG_ENCODING, 0, 1);
	}
	
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_CP(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* Destructor
*/
Func_CP::~Func_CP()
{
}

/* IDCall_CP
*/
void Func_CP::IDCall_CP(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error(kErrNilInterface);
	SnapshotUtils * snapshotUtils = nil;
	
	do
	{
		IDocument * doc = parser->getCurDoc();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		UIDRef itemToExport = UIDRef::gNull;
#if REF_AS_STRING
		if(refItem == kInvalidTCLRef && numPage != 0) // Export a page
#else
		if(refItem == kZeroTCLRef && numPage != 0) 
#endif
		{
			InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
			if(pageList == nil)
				break;

			if(numPage == MAX_PAGE)
				numPage = pageList->GetPageCount();

			if (numPage > pageList->GetPageCount())
			{
				error = PMString(kErrNumPageTooBig);
				break;
			}

			itemToExport = UIDRef(db, pageList->GetNthPageUID(numPage-1));
		}
		else // Export an item
		{
			InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
			if(itemManager == nil)
				break;

			UID itemUID = parser->getItem(refItem);
			if(itemUID == kInvalidUID)
			{
				error = PMString(kErrInvalidItem);
				break;
			}

			itemToExport = UIDRef(db,itemUID);

			if(itemManager->SelectPageItem(itemToExport) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			parser->setCurItem(refItem);
		}
		
		PMReal xScale = 0, yScale = 0;
		if(scale != 0)
		{
			xScale = yScale = scale / 100;
		}
		else
		{
			InterfacePtr<IGeometry> itemToExportGeo (itemToExport, UseDefaultIID());
			if(itemToExportGeo == nil)
				break;

			PMRect bbox = itemToExportGeo->GetStrokeBoundingBox();
			PMReal itemWidth = bbox.Width();
			PMReal itemHeight = bbox.Height();

			if(width == 0)
			{
				PMReal ratio = itemWidth/itemHeight;
				width = (ratio * height);
			}
			else if(height == 0)
			{
				PMReal ratio = itemHeight/itemWidth;
				height = ratio * width;				
			}
		
			xScale = width / itemWidth;
			yScale = height / itemHeight;			
		}

		xScale /= (desiredRes/72);
		yScale /= (desiredRes/72);

		PMReal minRes = desiredRes;
		snapshotUtils =  new SnapshotUtils(itemToExport, 
										   xScale,     // X Scale
										   yScale,     // Y Scale
										   desiredRes,    // desired resolution of resulting snapshot
										   minRes,    // minimum resolution of resulting snapshot
										   0,     // Extend the bounds of the area to be drawn by the given amount of bleed
										   IShape::kPrinting,// Drawing flags (kPrinting suppresses drawing of margins and guides)
										   kTrue,    // kTrue forces images and graphics to draw at full resolution, kFalse to draw proxies
										   kFalse,    // kTrue to draw grayscale, kFalse to draw RGB
										   kFalse,			  // kTrue to add an alpha channel, kFalse no alpha channel
										   SnapshotUtils::kXPHigh, // Controls the quality level of transparency effects such as drop shadows,
										   2.0,               // Point size at which text will be greeked if its points size multiplied by the scaling is less than the greek below value.
										   kTrue,             // kTrue, anti aliasing will be turned on for all page items.
										   kTrue);            // determines whether overprint preview will be in effect  
										  
		

		if(snapshotUtils->GetStatus() != kSuccess)
		{
			delete snapshotUtils;
			// sometimes snapshot instanciation may fail, due to a lack of memorytry again...I don't know if it's useful to try again
			snapshotUtils =  new SnapshotUtils(itemToExport, xScale, yScale, desiredRes, minRes, 0, IShape::kPrinting, kTrue, kFalse, kFalse);  
			if(snapshotUtils->GetStatus() != kSuccess)
			{
				error = PMString(kErrSnapshotCreationFailed);
				break;
			}
		}

		IDFile jpegSysFile = FileUtils::PMStringToSysFile(filename);
		IDFile parent;		

		FileUtils::GetParentDirectory(jpegSysFile, parent) ;		
		if(FileUtils::DoesFileExist(parent) == kFalse)// Invalid path
		{
       		error = PMString(kErrBadPath); 
            break;
        }

		if(formatExport == ToJPEGFormat)
		{
			// create a stream to write out
			InterfacePtr<IPMStream> jpegStream(StreamUtil::CreateFileStreamWrite(jpegSysFile, kOpenOut|kOpenTrunc, 'JPEG',  kAnyCreator));
			if (jpegStream == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			// export to JPEG
			SnapshotUtils::SSJPEGQuality jpegQuality;
			switch(quality)
			{
				case 0 :
				case 1 :
					jpegQuality = SnapshotUtils::kSSJPEGLowQuality;
					break;
					
				case 2 :
				case 3 :
					jpegQuality = SnapshotUtils::kSSJPEGGoodQuality;
					break;
					
				case 4 :
					jpegQuality = SnapshotUtils::kSSJPEGExcellentQuality; 
					break;
					
				case 5 : 
					jpegQuality = SnapshotUtils::kSSJPEGGreatQuality; 
					break;
					
				default : 
					jpegQuality = SnapshotUtils::kSSJPEGGoodQuality;
					break;
			}
			
			SnapshotUtils::SSJPEGEncoding jpegEncoding = SnapshotUtils::kSSJPEGBaseline;
			if(encoding == PROGRESSIVE_JPEG_ENCODING) // 0 is JPEGBaseline (default), 1 for progressive encoding
			{
				jpegEncoding = SnapshotUtils::kSSJPEGProgressive;
			}
			
			if(snapshotUtils->ExportImageToJPEG(jpegStream, jpegEncoding, jpegQuality) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			if (jpegStream != nil)
			{
				jpegStream->Flush();
				jpegStream->Close();
			}
		}
		else if(formatExport == ToGIFFormat)
		{
			// create a stream to write out
			InterfacePtr<IPMStream> gifStream(StreamUtil::CreateFileStreamWrite(jpegSysFile, kOpenOut|kOpenTrunc, 'GIFF',  kAnyCreator));
			if (gifStream == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			if(snapshotUtils->ExportImageToGIF(gifStream) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			if (gifStream != nil)
			{
				gifStream->Flush();
				gifStream->Close();
			}
		}
		else if(formatExport == ToTIFFFormat)
		{
			// create a stream to write out
			InterfacePtr<IPMStream> tiffStream(StreamUtil::CreateFileStreamWrite(jpegSysFile, kOpenOut|kOpenTrunc, 'TIFF',  kAnyCreator));
			if (tiffStream == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			if(snapshotUtils->ExportImageToTIFF(tiffStream) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			if (tiffStream != nil)
			{
				tiffStream->Flush();
				tiffStream->Close();
			}
		}	
		status = kSuccess;
		
	} while(kFalse);
	
	if(snapshotUtils != nil)
		delete snapshotUtils;
	
	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
		
		CmdUtils::EndCommandSequence(seq);
	}
	else
		throw TCLError(error);	
}


/* Constructor
 */
Func_CQ::Func_CQ(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CQ::Func_CQ -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}
	
	try
	{
		reader->MustEgal();
		
		filename = reader->ExtractString(255);
		numPage = reader->ExtractInt(0, 0, MAX_PAGE);
		scale = reader->ExtractInt(0, 0, 250);
		width = reader->ExtractInt(0, 0, 2000);
		height = reader->ExtractInt(0, 0, 2000);
		
#if REF_AS_STRING
		refItem = reader->ExtractTCLRef(kInvalidTCLRef);
#else
		refItem = reader->ExtractTCLRef(kZeroTCLRef);
#endif
		quality = reader->ExtractInt(0, 0, 5);
		desiredRes = reader->ExtractInt(72, 30, 180);
		formatExport = reader->ExtractInt(1, 1, 3);
		encoding = reader->ExtractInt(BASELINE_JPEG_ENCODING, 0, 1);
	 
		colorSpace = reader->ExtractInt(1, 1, 4);         
		addTransparencyAlpha = reader->ExtractBooleen(kFalse); 		  
		simulateOverprint = reader->ExtractBooleen(kFalse) ;   
		
	}
	
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");
		
		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}
	
	if(reader->IsEndOfCommand())
		IDCall_CQ(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* Destructor
 */
Func_CQ::~Func_CQ()
{
}

/* IDCall_CQ
 */
void Func_CQ::IDCall_CQ(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error(kErrNilInterface);
	SnapshotUtilsEx * snapshotUtilsEx = nil;
	
	do
	{
		IDocument * doc = parser->getCurDoc();
		
		// Check if there is a document open
		if(doc == nil){
			error = PMString(kErrNeedDoc);		
			break;
		}
		
		IDataBase * db = parser->getCurDataBase();
		if(db == nil){
			break;
		}
		
		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;
		
		seq->SetUndoability(ICommand::kAutoUndo);
		
		UIDRef itemToExport = UIDRef::gNull;
#if REF_AS_STRING
		if(refItem == kInvalidTCLRef && numPage != 0) // Export a page
#else
			if(refItem == kZeroTCLRef && numPage != 0) 
#endif
			{
				InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
				if(pageList == nil)
					break;
				
				if(numPage == MAX_PAGE)
					numPage = pageList->GetPageCount();
				
				if (numPage > pageList->GetPageCount())
				{
					error = PMString(kErrNumPageTooBig);
					break;
				}
				
				itemToExport = UIDRef(db, pageList->GetNthPageUID(numPage-1));
			}
			else // Export an item
			{
				InterfacePtr<IItemManager> itemManager(parser->QueryItemManager());
				if(itemManager == nil)
					break;
				
				UID itemUID = parser->getItem(refItem);
				if(itemUID == kInvalidUID)
				{
					error = PMString(kErrInvalidItem);
					break;
				}
				
				itemToExport = UIDRef(db,itemUID);
				
				if(itemManager->SelectPageItem(itemToExport) != kSuccess)
				{
					error = ErrorUtils::PMGetGlobalErrorString();
					break;
				}
				parser->setCurItem(refItem);
			}
		
		PMReal xScale = 0, yScale = 0;
		if(scale != 0)
		{
			xScale = yScale = scale / 100;
		}
		else
		{
			InterfacePtr<IGeometry> itemToExportGeo (itemToExport, UseDefaultIID());
			if(itemToExportGeo == nil)
				break;
			
			PMRect bbox = itemToExportGeo->GetStrokeBoundingBox();
			PMReal itemWidth = bbox.Width();
			PMReal itemHeight = bbox.Height();
			
			if(width == 0)
			{
				PMReal ratio = itemWidth/itemHeight;
				width = (ratio * height);
			}
			else if(height == 0)
			{
				PMReal ratio = itemHeight/itemWidth;
				height = ratio * width;				
			}
			
			xScale = width / itemWidth;
			yScale = height / itemHeight;			
		}
		
		xScale /= (desiredRes/72);
		yScale /= (desiredRes/72);
		
		
		// export to JPEG
		SnapshotUtilsEx::SSColorSpaceFamily colorSpaceFamily;
		switch(colorSpace)
		{
			case 1 :
				colorSpaceFamily = SnapshotUtilsEx::kCsRGB;
				break;
			case 2 :
				colorSpaceFamily = SnapshotUtilsEx::kCsGray;
				break;
			case 3 :
				colorSpaceFamily = SnapshotUtilsEx::kCsCMYK; 
				break;
			case 4 : 
				colorSpaceFamily = SnapshotUtilsEx::kCsLab; 
				break;
			default : 
				colorSpaceFamily = SnapshotUtilsEx::kCsRGB;
				break;
		}
		
		
		PMReal minRes = desiredRes;
		snapshotUtilsEx =  new SnapshotUtilsEx(itemToExport, 
										       xScale,			     // X Scale
										       yScale,			     // Y Scale
										       desiredRes,		     // desired resolution of resulting snapshot
										       minRes,			     // minimum resolution of resulting snapshot
										       0.0,		             // Extend the bounds of the area to be drawn by the given amount of bleed
										       colorSpaceFamily,     // The colorspace of the snapshot.
										       addTransparencyAlpha, // kTrue to store transparency as an alpha channel, kFalse no alpha channel			  
										       simulateOverprint);   // kTrue to simulate overprint, ie the equivalent of turning on overprint preview. Uses more memory and somewhat slower.
	
		if(snapshotUtilsEx == nil)
		{
			error = PMString(kErrSnapshotCreationFailed);
			break;
		}
		
		// Process snapshotUtilsEx
		snapshotUtilsEx->Draw(IShape::kPrinting, kTrue, 2.0);
		
		IDFile jpegSysFile = FileUtils::PMStringToSysFile(filename);
		IDFile parent;		 
		FileUtils::GetParentDirectory(jpegSysFile, parent) ;		
		if(FileUtils::DoesFileExist(parent) == kFalse)// Invalid path
		{
       		error = PMString(kErrBadPath); 
            break;
        }
		
		if(formatExport == ToJPEGFormat)
		{
			// create a stream to write out
			InterfacePtr<IPMStream> jpegStream(StreamUtil::CreateFileStreamWrite(jpegSysFile, kOpenOut|kOpenTrunc, 'JPEG',  kAnyCreator));
			if (jpegStream == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			// export to JPEG
			SnapshotUtils::SSJPEGQuality jpegQuality;
			switch(quality)
			{
				case 0 :
				case 1 :
					jpegQuality = SnapshotUtils::kSSJPEGLowQuality;
					break;
				case 2 :
				case 3 :
					jpegQuality = SnapshotUtils::kSSJPEGGoodQuality;
					break;
				case 4 :
					jpegQuality = SnapshotUtils::kSSJPEGExcellentQuality; 
					break;
				case 5 : 
					jpegQuality = SnapshotUtils::kSSJPEGGreatQuality; 
					break;
				default : 
					jpegQuality = SnapshotUtils::kSSJPEGGoodQuality;
					break;
			}
			
			SnapshotUtils::SSJPEGEncoding jpegEncoding = SnapshotUtils::kSSJPEGBaseline;
			if(encoding == PROGRESSIVE_JPEG_ENCODING) // 0 is JPEGBaseline (default), 1 for progressive encoding
			{
				jpegEncoding = SnapshotUtils::kSSJPEGProgressive;
			}
			
			if(snapshotUtilsEx->ExportImageToJPEG(jpegStream, jpegEncoding, jpegQuality) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			if (jpegStream)
			{
				jpegStream->Flush();
				jpegStream->Close();
			}
		}
		else if(formatExport == ToGIFFormat)
		{
			// create a stream to write out
			InterfacePtr<IPMStream> gifStream(StreamUtil::CreateFileStreamWrite(jpegSysFile, kOpenOut|kOpenTrunc, 'GIFF',  kAnyCreator));
			if (gifStream == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			if(snapshotUtilsEx->ExportImageToGIF(gifStream) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			if (gifStream)
			{
				gifStream->Flush();
				gifStream->Close();
			}
		}
		else if(formatExport == ToTIFFFormat)
		{
			// create a stream to write out
			InterfacePtr<IPMStream> tiffStream(StreamUtil::CreateFileStreamWrite(jpegSysFile, kOpenOut|kOpenTrunc, 'TIFF',  kAnyCreator));
			if (tiffStream == nil)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			if(snapshotUtilsEx->ExportImageToTIFF(tiffStream) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			
			if (tiffStream)
			{
				tiffStream->Flush();
				tiffStream->Close();
			}
		}	
		status = kSuccess;
		
	} while(kFalse);
	
	if(snapshotUtilsEx)
		delete snapshotUtilsEx;
	
	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
		
		CmdUtils::EndCommandSequence(seq);
	}
	else
		throw TCLError(error);	
}
/* Constructor
*/
Func_DEtoile::Func_DEtoile(ITCLParser *  parser)
{	
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DEtoile::Func_DEtoile -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();
		width = reader->ExtractReel('P','+',0,0);
		height = reader->ExtractReel('P','+',0,0);				
	}
	// Catch TCL error while parameter reading to add parameter number to the error message
	catch(TCLError& error)
	{
		PMString parameter(kParameterKey);
		parameter.Translate(); parameter.Append(" ");
		parameter.AppendNumber(reader->GetNumParam());
		parameter.Append(" : ");

		PMString message(error.getMessage());
		message.Translate();
		message.Insert(parameter);
		error.setMessage(message);
		throw;
	}

	if(reader->IsEndOfCommand())
		IDCall_DEtoile(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
	
}

/* IDCall_DEtoile
*/
void Func_DEtoile::IDCall_DEtoile(ITCLParser *  parser)
{
	ErrorCode status = kFailure;
	ICommandSequence* seq = nil;
	PMString error(kErrNilInterface);
	
	do
	{
		IDocument * doc = parser->getCurDoc();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		IDataBase * db = parser->getCurDataBase();

		// Create a command sequence
		seq = CmdUtils::BeginCommandSequence();
		if(seq == nil)
			break;

		seq->SetUndoability(ICommand::kAutoUndo);

		// Get original page dimension
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		InterfacePtr<IGeometry> pageGeo (db, pageList->GetNthPageUID(0), UseDefaultIID());

		PMReal diffX = 0.0 , diffY = 0.0;
		PMReal currentHeight = pageGeo->GetStrokeBoundingBox().Height();
		PMReal currentWidth = pageGeo->GetStrokeBoundingBox().Width();

		if(width == 0)
			width = currentWidth;
		else
		{
			// Check whether width in parameter is valid
			if(width < MIN_WIDTH)
				throw TCLError(PMString(kErrInfMin));

			if(width > MAX_WIDTH)
				throw TCLError(PMString(kErrSupMax));

			diffX = width - currentWidth;
		}

		if(height == 0)
			height = currentHeight;
		else
		{
			// height whether width in parameter is valid
			if(height < MIN_HEIGHT)
				throw TCLError(PMString(kErrInfMin));

			if(height > MAX_HEIGHT)
				throw TCLError(PMString(kErrSupMax));	

			diffY = height - currentHeight;
		}

		// Get items position before page resizing because items will be moved (thus an item on right page may move on left page)
		// Their position have to be reset after resizing
		K2Vector<K2Pair<PMPoint, UIDList> > offsetList;

		// Determine the range of content spread layer within each spread's children 
		InterfacePtr<ILayerList> layerList (doc, UseDefaultIID());
		
		int32 nbLayers = layerList->GetCount();
		int32 startContentLayer, endContentLayer;
		
		InterfacePtr<IGuidePrefs> guidePrefs (doc->GetDocWorkSpace(), UseDefaultIID());
		if(guidePrefs && guidePrefs->GetGuidesInBack())
		{
			startContentLayer = nbLayers + 1;
			endContentLayer = (2 * nbLayers) - 1;
		}
		else
		{
			startContentLayer = 2;
			endContentLayer = nbLayers;
		}

		// Iterate through master spreads first
		InterfacePtr<IMasterSpreadList> masterSpreadList (doc, UseDefaultIID());
		int32 nbMasters = masterSpreadList->GetMasterSpreadCount();
		for(int32 i = 0 ; i < nbMasters ; ++i)
		{
			K2Vector<K2Pair<PMPoint, UIDList> > spreadOffset = this->ComputeOffset(db, masterSpreadList->GetNthMasterSpreadUID(i), diffX, diffY, startContentLayer, endContentLayer);
			offsetList.insert(offsetList.begin(), spreadOffset.begin(), spreadOffset.end());
		}
		
		// Iterate through master spreads first
		InterfacePtr<ISpreadList> spreadList (doc, UseDefaultIID());

		int32 nbSpreads = spreadList->GetSpreadCount();
		for(int32 i = 0 ; i < nbSpreads ; ++i)
		{
			K2Vector<K2Pair<PMPoint, UIDList> > spreadOffset = this->ComputeOffset(db, spreadList->GetNthSpreadUID(i), diffX, diffY, startContentLayer, endContentLayer);
			offsetList.insert(offsetList.begin(), spreadOffset.begin(), spreadOffset.end());
		}

		PMRect newPageDim(0,0,width,height);

		InterfacePtr<IDocHelper> docHelper (parser->QueryDocHelper());
		if(docHelper == nil)
			break;

		if(docHelper->SetPageSizeCmd(::GetUIDRef(doc),newPageDim, (width > height)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}
				
		status = kSuccess;

		// Reset items position
		K2Vector<K2Pair<PMPoint, UIDList> >::iterator iter = offsetList.begin();

		while(iter != offsetList.end())
		{
			status = Utils<Facade::ITransformFacade>()->TransformItems( iter->second, Transform::PasteboardCoordinates(), kZeroPoint, 
					 Transform::TranslateBy(iter->first.X(), iter->first.Y()));

			if(status != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				break;
			}
			++iter;
		}

	} while(kFalse);
	
	if (seq != nil)
	{
		if (status != kSuccess)
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
		
		CmdUtils::EndCommandSequence(seq);
	}
	else
		throw TCLError(error);
	
}

K2Vector<K2Pair<PMPoint, UIDList> > Func_DEtoile::ComputeOffset(IDataBase * db, UID spreadUID, PMReal deltaX, PMReal deltaY, int32 startContentLayer, int32 endContentLayer)
{
	K2Vector<K2Pair<PMPoint, UIDList> > offsetList;
	UIDList leftList(db), rightList(db);
	K2Vector<KeyValuePair<PMReal, UIDList> > unisexList;

	InterfacePtr<ISpread> theSpread (db, spreadUID, UseDefaultIID());
	int32 pageCount = theSpread->GetNumPages();

	InterfacePtr<IHierarchy> spreadHier (theSpread, UseDefaultIID());

	int32 childCount = spreadHier->GetChildCount();
	for(int32 j = startContentLayer ; j <= endContentLayer ; ++j)
	{
		InterfacePtr<IHierarchy> spreadLayerHier (db, spreadHier->GetChildUID(j), UseDefaultIID());
		if(spreadLayerHier == nil)
			break;

		int32 nbItems = spreadLayerHier->GetChildCount();
		
		// Iterate through childs (the page items) of the spread layer (2nd z-order level)
		for(int32 itemIndex = 0 ; itemIndex < nbItems ; ++itemIndex)
		{
			InterfacePtr<IHierarchy> itemHier (db, spreadLayerHier->GetChildUID(itemIndex), UseDefaultIID());
	
			UID ownerPageUID = Utils<ILayoutUtils>()->GetOwnerPageUID(itemHier);
			int32 pageIndex= theSpread->GetPageIndex(ownerPageUID);

			if(pageIndex == -1) // means that the item is outside pages
			{
				InterfacePtr<IGeometry> itemGeo (itemHier, UseDefaultIID());
				PMRect itemBounds = itemGeo->GetStrokeBoundingBox();				
				::TransformInnerRectToPasteboard(itemGeo, &itemBounds);

				PBPMRect pageBounds = theSpread->GetPagesBounds(Transform::PasteboardCoordinates());

				PMReal offsetX = 0.0;
				if(itemBounds.Right() < pageBounds.Left())
				{
					if(deltaX < 0)
					{
						InterfacePtr<IColumns> nearestCol (theSpread->QueryNearestColumnBounds(itemBounds.GetCenter(), kTrue, nil, &pageIndex));
						PageType pageType = Utils<ILayoutUtils>()->GetPageType(UIDRef(db, theSpread->GetNthPageUID(pageIndex) ));
						if( pageType != kRightPage )
						{
							if(pageType == kLeftPage && pageCount == 1)
								offsetX = ( (-deltaX)* PMReal(pageCount) ); 
							else
								offsetX = ( (-deltaX)* PMReal(pageCount) ) / 2.0; 							
						}
						pageIndex = -1;
					}					

					int32 pos = ::FindLocation(unisexList, offsetX);
					if(pos == -1)
					{
						UIDList newList(itemHier);
						unisexList.push_back(KeyValuePair<PMReal, UIDList>(offsetX, newList));
					}
					else				
						unisexList[pos].Value().Append(::GetUID(itemHier));	
				}
				else if(itemBounds.Left() > pageBounds.Right())
				{
					if(deltaX < 0)
					{
						InterfacePtr<IColumns> nearestCol (theSpread->QueryNearestColumnBounds(itemBounds.GetCenter(), kTrue, nil, &pageIndex));
						PageType pageType = Utils<ILayoutUtils>()->GetPageType(UIDRef(db, theSpread->GetNthPageUID(pageIndex) ));
						if( pageType != kLeftPage )
						{
							if(pageType == kRightPage && pageCount == 1)
								offsetX = ( deltaX* PMReal(pageCount) ); 
							else
								offsetX = ( deltaX* PMReal(pageCount) ) / 2.0; 										
						}
						pageIndex = -1;
					}
					int32 pos = ::FindLocation(unisexList, offsetX);
					if(pos == -1)
					{
						UIDList newList(itemHier);
						unisexList.push_back(KeyValuePair<PMReal, UIDList>(offsetX, newList));
					}
					else				
						unisexList[pos].Value().Append(::GetUID(itemHier));	
				}
				else
				{
					// Get index of nearest page
					InterfacePtr<IColumns> nearestCol (theSpread->QueryNearestColumnBounds(itemBounds.GetCenter(), kTrue, nil, &pageIndex));
					ownerPageUID = theSpread->GetNthPageUID(pageIndex);
				}								
			}
	
			if(pageIndex != -1)
			{
				PageType pageType = Utils<ILayoutUtils>()->GetPageType(UIDRef(db, ownerPageUID));
				switch(pageType)
				{					
					case kLeftPage :
						leftList.Append(::GetUID(itemHier));
						break;

					case kRightPage :
						rightList.Append(::GetUID(itemHier));
						break;

					case kUnisexPage :
						PMReal offsetX = (pageIndex - (PMReal(pageCount)/2.0) ) * deltaX;

						int32 pos = ::FindLocation(unisexList, offsetX);
						if(pos == -1)
						{
							UIDList newList(itemHier);
							unisexList.push_back(KeyValuePair<PMReal, UIDList>(offsetX, newList));
						}
						else
						{
							unisexList[pos].Value().Append(::GetUID(itemHier));
						}

						break;
				}
			}
		}
	}

	// Group all offset in a singleList;
	if(!rightList.IsEmpty())
		offsetList.push_back(K2Pair<PMPoint,UIDList>(PMPoint(0,-deltaY/2), rightList));

	if(!leftList.IsEmpty())
		offsetList.push_back(K2Pair<PMPoint,UIDList>(PMPoint(-deltaX,-deltaY/2), leftList));

	for(int32 i = 0 ; i < unisexList.size() ; ++i)
	{
		offsetList.push_back(K2Pair<PMPoint,UIDList>(PMPoint(unisexList[i].Key(), -deltaY/2), unisexList[i].Value()));
	}

	return offsetList;
}