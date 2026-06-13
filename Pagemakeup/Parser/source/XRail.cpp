/*
//	File:	XRail.cpp
//
//	Date:	19-Apr-2006
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"
#include "ErrorUtils.h"

// Interface includes
#include "IDocument.h"
#include "IPageList.h"
#include "IMasterSpreadList.h"
#include "IMasterSpread.h"
#include "IMasterSpreadUtils.h"
#include "ISpread.h"
#include "IMasterPage.h"
#include "ISection.h"
#include "ISectionList.h"

#include "IIntData.h"
#include "IGeometry.h"
#include "IHierarchy.h"
#include "ILayerList.h"
#include "IGuidePrefs.h"
#include "IPageItemTypeUtils.h"
#include "IRefPointUtils.h"
#include "IUnitOfMeasure.h"
#include "GlobalDefs.h"

#if XPAGE
#include "IXPageUtils.h"
#include "IResaRedacDataList.h"
#include "IFormeData.h"
#include <map>
#endif

#if XRAIL
	#include "IXRailPageSlugData.h"
#endif

// Project includes
#include "XRail.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "DebugClassUtils.h"
#include "TransformUtils.h"

/* Constructor
*/
Func_MN::Func_MN(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

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
		IDCall_MN(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

/* IDCall_MN
*/
void Func_MN::IDCall_MN(ITCLParser * parser)
{	
	ErrorCode status = kFailure;
	PMString error("");

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

		// Get document size (based on the first page's dimensions)
		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		
		UID firstPage = pageList->GetNthPageUID(0);
		
		InterfacePtr<IGeometry> pageGeo (db, firstPage, UseDefaultIID());
		if(pageGeo == nil)
			throw TCLError(error);
		
		PMRect bbox = pageGeo->GetStrokeBoundingBox();
		PMReal height = bbox.Height();
		PMReal width = bbox.Width();

		PMString output("[LARGEUR]", PMString::kUnknownEncoding);
		output.AppendNumber(width, 2, kTrue);
		parser->WriteLogFile(output);

		output.SetCString("[HAUTEUR]", PMString::kUnknownEncoding);
		output.AppendNumber(height, 2, kTrue);
		output += kNewLine;
		parser->WriteLogFile(output);

		// Get master info
		InterfacePtr<IMasterSpreadList> masterSpreadList (doc, UseDefaultIID());
		
		UIDList sortedMasterList(db);
		Utils<IMasterSpreadUtils>()->TopoSortMasterSpread(masterSpreadList, &sortedMasterList);

		int32 nbMasters = masterSpreadList->GetMasterSpreadCount();
		for(int32 i = nbMasters - 1 ; i >= 0 ; --i)
		{
			UID masterUID = sortedMasterList[i];
			InterfacePtr<IMasterSpread> master (db, masterUID, UseDefaultIID());

			output.SetCString("[MAQUETTE]",PMString::kUnknownEncoding);
			parser->WriteLogFile(output);

			output.SetCString("Num : ", PMString::kUnknownEncoding);
			output.AppendNumber(nbMasters - i);
			parser->WriteLogFile(output);

			output.SetCString("Nom : ", PMString::kUnknownEncoding);			
			PMString name;
			master->GetName(&name);
			output += name;
			parser->WriteLogFile(output);

			// Export number of pages
			InterfacePtr<ISpread> masterSpreadData (master, UseDefaultIID());
			
			output.SetCString("NbPages : ", PMString::kUnknownEncoding);			
			output.AppendNumber(masterSpreadData->GetNumPages());	
			output += kNewLine;
			parser->WriteLogFile(output);
		}

		// Get page info
		int32 nbPage = pageList->GetPageCount();
		InterfacePtr<ISectionList> sectionList (doc, UseDefaultIID());
		for(int32 i = 0 ; i < nbPage ; ++i)
		{
			UID pageUID = pageList->GetNthPageUID(i);

			InterfacePtr<IMasterPage> theNthPage (db, pageUID, UseDefaultIID());

			UID masterSpread = theNthPage->GetMasterSpreadUID();

			output.SetCString("[PAGE]", PMString::kUnknownEncoding);
			parser->WriteLogFile(output);
			
			output.SetCString("Num : ", PMString::kUnknownEncoding);
			output.AppendNumber(pageList->GetPageIndex(pageUID) + 1);
			parser->WriteLogFile(output);

			output.SetCString("Maquette : ", PMString::kUnknownEncoding);
			if(masterSpread == kInvalidUID)
				output += "XX";
			else
			{
				InterfacePtr<IMasterSpread> master (db, masterSpread, UseDefaultIID());
				PMString name;
				master->GetName(&name);
				output += name;
			}			
			parser->WriteLogFile(output);			

			InterfacePtr<ISection> theNthPageSection (sectionList->QueryNthSection(pageList->GetSectionIndex(pageUID)));

			output.SetCString("Type : ", PMString::kUnknownEncoding);
			ClassID style = kInvalidClass;
			theNthPageSection->GetStyleInfo(&style);

			// Page Numbering styles : 1 -> Arabic, 2-> Roman Upper, 3-> Roman Lower, 4-> Letters Upper, 5 -> Letters Lower
			if(style == kArabicPageNumberingStyleServiceBoss) output.AppendNumber(1);
			else if(style == kRomanUpperPageNumberingStyleServiceBoss) output.AppendNumber(2);
			else if(style == kRomanLowerPageNumberingStyleServiceBoss) output.AppendNumber(3);
			else if(style == kLettersUpperPageNumberingStyleServiceBoss) output.AppendNumber(4);
			else if(style == kLettersLowerPageNumberingStyleServiceBoss) output.AppendNumber(5);

			parser->WriteLogFile(output);

			output.SetCString("Prefixe : ", PMString::kUnknownEncoding);
			PMString name;
			theNthPageSection->GetName(&name);
			output += name;
			parser->WriteLogFile(output);

			output.SetCString("Section : ", PMString::kUnknownEncoding);
			output.AppendNumber(sectionList->GetPageNumber(pageUID));
			output += kNewLine;
			parser->WriteLogFile(output);
		}

#if XPAGE

		// Create millimeters converters to output dimensions in this unit measure
		InterfacePtr<IUnitOfMeasure> uom ((IUnitOfMeasure *)::CreateObject(kMillimetersBoss,IID_IUNITOFMEASURE));

		InterfacePtr<IResaRedacDataList> docResaDataList (doc, UseDefaultIID());
		if (docResaDataList == nil)
		{
			// Résa Rédactionnelles feature was removed — nothing to export.
			status = kSuccess;
			break;
		}

		std::map<PMString, UIDList> allFormeItems = Utils<IXPageUtils>()->GetFormeDataList(doc, kTrue);

		int32 nbResas = docResaDataList->GetNbResaRedac();
		for(int32 i = 0 ; i < nbResas ; ++i)
		{
			PMString idForme = kNullString;
			PMString classeurName = kNullString;
			PMString formeName = kNullString;
			ResaRedac resa = docResaDataList->GetNthResaRedac(i, idForme);

			if(idForme == kNullString || resa == kInvalidResaRedac)
				continue;

			// Export resa only if its forme is on a master spread
			std::map<PMString, UIDList>::iterator formeIter = allFormeItems.find(idForme);
			if(formeIter == allFormeItems.end())
				continue;
			
			UIDList formeItems = formeIter->second;
			if(formeItems.IsEmpty())
				continue;

			InterfacePtr<IHierarchy> firstFormeItem (formeItems.GetRef(0), UseDefaultIID());
			InterfacePtr<IMasterSpread> masterSpread (db, firstFormeItem->GetSpreadUID(), UseDefaultIID());
			if(!masterSpread)
				continue;
			
            InterfacePtr<IFormeData> formeData (formeItems.GetRef(0), UseDefaultIID());
			if(formeData)
			{
				Utils<IXPageUtils>()->SplitFormeDescription(formeData->GetUniqueName(), classeurName, formeName);
			}
            
			output.SetCString("[RESA]", PMString::kUnknownEncoding);
			parser->WriteLogFile(output);
			
			output.SetCString("ID : ", PMString::kUnknownEncoding);
			output.AppendNumber(resa.id);
			parser->WriteLogFile(output);

			PMString masterName;
			masterSpread->GetName(&masterName);
			output.SetCString("Maquette : ", PMString::kUnknownEncoding);
			output += masterName;
			parser->WriteLogFile(output);

			output.SetCString("Sujet : ", PMString::kUnknownEncoding);
			output += resa.topic;
			parser->WriteLogFile(output);

			output.SetCString("NbSignes : ", PMString::kUnknownEncoding);
			output.AppendNumber(resa.nbSigns);
			parser->WriteLogFile(output);

			output.SetCString("NbPhotos : ", PMString::kUnknownEncoding);
			output.AppendNumber(resa.nbImages);			
			parser->WriteLogFile(output);

			output.SetCString("Commentaire : ", PMString::kUnknownEncoding);
			output += resa.comment;
			parser->WriteLogFile(output);
            
			output.SetCString("Classeur : ", PMString::kUnknownEncoding);
			output += classeurName;
			parser->WriteLogFile(output);
            
			output.SetCString("Forme : ", PMString::kUnknownEncoding);
			output += formeName;
			parser->WriteLogFile(output);
            
			// Get forme position in pasteboard coordinates
			PMPoint formeLeftTop = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kTopLeftReferencePoint, formeItems/*, nil, &Geometry::OuterStrokeBounds()*/);
			PMPoint formeRightBottom = Utils<IRefPointUtils>()->CalculateReferencePoint_3(IReferencePointData::kBottomRightReferencePoint, formeItems/*, nil, &Geometry::OuterStrokeBounds()*/);

			PMReal width = abs(formeLeftTop.X() - formeRightBottom.X());
			PMReal height = abs(formeLeftTop.Y() - formeRightBottom.Y());

			// Transform position to page coordinates
			InterfacePtr<ISpread> masterSpreadData (masterSpread, UseDefaultIID());
			int32 pageIndex = 0;
			InterfacePtr<IGeometry> ownerPageGeo (masterSpreadData->QueryNearestPage(formeLeftTop, &pageIndex));
			::TransformPasteboardPointToInner(ownerPageGeo, &formeLeftTop);

			output.SetCString("IndexPage : ", PMString::kUnknownEncoding);
			output.AppendNumber(pageIndex);
			parser->WriteLogFile(output);

			output.SetCString("X : ", PMString::kUnknownEncoding);
			output.AppendNumber(uom->PointsToUnits(formeLeftTop.X()));
			parser->WriteLogFile(output);

			output.SetCString("Y : ", PMString::kUnknownEncoding);
			output.AppendNumber(uom->PointsToUnits(formeLeftTop.Y()));
			parser->WriteLogFile(output);

			output.SetCString("Largeur : ", PMString::kUnknownEncoding);
			output.AppendNumber(uom->PointsToUnits(width));
			parser->WriteLogFile(output);

			output.SetCString("Hauteur : ", PMString::kUnknownEncoding);
			output.AppendNumber(uom->PointsToUnits(height));
			output += kNewLine;
			parser->WriteLogFile(output);

		}
#endif

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
}

#if XRAIL

/* Constructor
*/
Func_XY::Func_XY(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		reader->MustEgal();
		numPage = reader->ExtractInt(1,1,MAX_PAGE);
		idPage = reader->ExtractInt(0,0,MAX_BLOC);
		nomBase = reader->ExtractString(255);
		if(!reader->IsEndOfCommand())
		{
			// dummy parameters to ensure compatibility with Quark version
			
			reader->ExtractString(255);
			reader->ExtractString(255);
			reader->ExtractString(255);
			reader->ExtractString(255);
		}
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
		IDCall_XY(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));
}

void Func_XY::IDCall_XY(ITCLParser * parser)
{
#if XRAIL
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		IDocument * doc = parser->getCurDoc();

		// Check if there is a document open
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<IPageList> pageList (doc, UseDefaultIID());
		if(numPage > pageList->GetPageCount())
		{
			error = PMString(kErrNumPageTooBig);
			break;
		}		

		InterfacePtr<ICommand> setPageSlugCmd (CmdUtils::CreateCommand(kXRLSetPageSlugDataCmdBoss));
		setPageSlugCmd->SetItemList(UIDList(::GetUIDRef(doc)));

		InterfacePtr<IXRailPageSlugData> cmdData (setPageSlugCmd, UseDefaultIID());
		cmdData->SetID(idPage);
#if MULTIBASES == 1
		cmdData->SetBaseName(nomBase);
#endif		
		InterfacePtr<IIntData> pageIndexData (setPageSlugCmd, UseDefaultIID());
		pageIndexData->Set(numPage - 1);

		if(CmdUtils::ProcessCommand(setPageSlugCmd) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess;

	} while(kFalse);

	if(status != kSuccess)
		throw TCLError(error);
#endif
}

#endif

/* Constructor
*/
Func_XZ::Func_XZ(ITCLParser *  parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil) throw TCLError(PMString(kErrNilInterface));

	try
	{
		// dummy parameters to ensure compatibility with Quark version
		reader->MustEgal();
		reader->ExtractInt(1,0,MAX_PAGE);
		reader->ExtractString(255);
		reader->ExtractString(255);		
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

	if(!reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));
}