/*
//	File:	Textes.cpp
//
//	Date:	24-Sep-2003
//
//  Author : Nicolas Metaye
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"
#include "SDKUtilities.h"
#include "ILayoutUtils.h"
#include "ErrorUtils.h"

// Interface includes
#include "IDocument.h"
#include "IWorkSpace.h"
#include "ITextAttributes.h"
#include "ITextAttrRealNumber.h"
#include "ISwatchList.h"
#include "ISwatchUtils.h"
#include "ICompositionStyle.h"
#include "IStyleGroupManager.h"
#include "IGradientFill.h"
#include "IRenderingObject.h"
#include "IGraphicStateUtils.h"

//Project includes
#include "Reglages.h"
#include "GlobalDefs.h"
#include "ITCLParser.h"
#include "ITCLReader.h"
#include "TCLError.h"
#include "ITextFrameHelper.h"
#include "ITxtAttrHelper.h"
#include "IStyleHelper.h"

#include "ITextModel.h"
#include "ITextFocus.h"


/* Constructor
*/
Func_DC::Func_DC(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DC::Func_DC -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		while (!reader->IsEndOfCommand())
		{
			int32 ref = reader->ExtractInt(0,MIN_COLOR,MAX_COLOR) ;
			PMString name = reader->ExtractString(255);

			theColors.insert(theColors.end(), Elt(ref,name));
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
		IDCall_DC(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_DC::~Func_DC()
{
}


/* IDCall_DC
*/
void Func_DC::IDCall_DC(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		// get the para style nametable
		IDocument * doc = parser->getCurDoc();
		
		InterfacePtr<IWorkspace> iWorkspace (doc->GetDocWorkSpace(),IID_IWORKSPACE);
		if (iWorkspace == nil)
		{
			ASSERT_FAIL("Func_DC::IDCall_DC -> iWorkspace nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		InterfacePtr<ISwatchList> swatchList(iWorkspace,UseDefaultIID());
		if (swatchList == nil)
		{
			ASSERT_FAIL("Func_DC::IDCall_DC -> swatchList nil");
			error = PMString(kErrNilInterface);
			return;
		}

		Vector::iterator iter;

		bool8 quit = kFalse ;
		for (iter = theColors.begin(); iter < theColors.end(); iter++)
		{
			UIDRef swatchRef = swatchList->FindSwatch(iter->Value());
			if (swatchRef.GetUID() == kInvalidUID)
			{
				ASSERT_FAIL("Func_DC::IDCall_DC -> colorUID == kInvalidUID");
				error = PMString(kErrColorNotFound);
				quit = kTrue;
				break;
			}

			UID colUID = swatchRef.GetUID();
			sColor col ;
			col.name = iter->Value();
			col.colorUID = colUID ;
			parser->setColorList(iter->Key(),col); // Previous color is erased
			
		}
		if (quit)
			break;


		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_CN::Func_CN(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CN::Func_CN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		refColor = reader->ExtractInt(0,MIN_COLOR,MAX_COLOR);
		swtchName = reader->ExtractString(255);
		reader->ExtractBooleen(kFalse); // isSeparated
		//cyan = reader->ExtractReel2('+',0,0,PMReal(0),PMReal(100))/100 ;
		//magenta = reader->ExtractReel2('+',0,0,PMReal(0),PMReal(100))/100 ;
		//yellow = reader->ExtractReel2('+',0,0,PMReal(0),PMReal(100))/100 ;
		//black = reader->ExtractReel2('+',0,0,PMReal(0),PMReal(100))/100 ;
		
		cyan = reader->ExtractInt(0,0,65535);
		magenta = reader->ExtractInt(0,0,65535);
		yellow = reader->ExtractInt(0,0,65535);
		black = reader->ExtractInt(0,0,65535);

		red = reader->ExtractInt(0,0,65535);
		green = reader->ExtractInt(0,0,65535);
		blue = reader->ExtractInt(0,0,65535);

		classid = reader->ExtractInt(0,0,kMaxInt32);
		reader->ExtractString(255); // name for Pantone ??
		isSpot = reader->ExtractBooleen(kFalse);
		reader->ExtractBooleen(kFalse); // isDefault -> not used in ID
		isHidden = reader->ExtractBooleen(kFalse);
		reader->ExtractBooleen(kFalse); // isReserved
		reader->ExtractBooleen(kFalse); // isVarnish
		// screen functions : not used in ID
		reader->ExtractInt(0,-1,kMaxInt32);
		reader->ExtractInt(0,-1,kMaxInt32); 
		reader->ExtractInt(0,-1,kMaxInt32); 
		reader->ExtractInt(0,-1,kMaxInt32);
		reader->ExtractInt(0,-1,kMaxInt32); 
		reader->ExtractInt(0,-1,kMaxInt32); 
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
		IDCall_CN(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_CN::~Func_CN()
{
}


/* IDCall_CN
*/
void Func_CN::IDCall_CN(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		IDocument * doc = parser->getCurDoc();
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		swtchName.SetTranslatable(kFalse);
		InterfacePtr<IWorkspace> workspace (doc->GetDocWorkSpace (),UseDefaultIID());
		if(workspace == nil)
		{		
			ASSERT_FAIL("Func_CN::IDCall_CN -> workspace nil");
			error = PMString(kErrNilInterface);
			break;
		}

		IDataBase * db = parser->getCurDataBase();
		// Get the active swatch list
		InterfacePtr<ISwatchList> swList (workspace,UseDefaultIID());
		if(swList == nil)
		{		
			ASSERT_FAIL("Func_CN::IDCall_CN -> swList nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		sColor col;
		col.name = swtchName;
		col.colorUID = swList->FindSwatch(swtchName).GetUID();

		// If color doesn't already exist, create it
		if(col.colorUID == kInvalidUID)
		{
			switch(classid)
			{
				case CCID_CMYK :
					{
						cyan = cyan / 65535;
						magenta = magenta / 65535;
						yellow = yellow / 65535;
						black = black / 65535;
						//PMString debug;
						//debug += "cyan : "; debug.AppendNumber(cyan); debug += "\r\n";
						//debug += "magenta : "; debug.AppendNumber(magenta); debug += "\r\n";
						//debug += "yellow : "; debug.AppendNumber(yellow); debug += "\r\n";
						//debug += "black : "; debug.AppendNumber(black); debug += "\r\n";
						//CAlert::InformationAlert(debug);
						if(isSpot)
							col.colorUID  = Utils<ISwatchUtils>()->CreateSpotCMYKSwatch(workspace,swtchName,cyan,magenta,yellow,black);
						else
							col.colorUID  = Utils<ISwatchUtils>()->CreateProcessCMYKSwatch(workspace,swtchName,cyan,magenta,yellow,black);
					}
					break;

				case CCID_LAB :	 
					{
						magenta -= 128;
						yellow -= 128;
						if(isSpot)
							col.colorUID  = Utils<ISwatchUtils>()->CreateSpotLABSwatch(workspace,swtchName,cyan,magenta,yellow);
						else
							col.colorUID  = Utils<ISwatchUtils>()->CreateProcessLABSwatch(workspace,swtchName,cyan,magenta,yellow);
					}
					break;
				case CCID_RGB :
					
				default:
					{
						red = red /65535;
						green = green /65535;
						blue = blue /65535;
						if(isSpot)
							col.colorUID  = Utils<ISwatchUtils>()->CreateSpotRGBSwatch(workspace,swtchName,red,green,blue);
						else
							col.colorUID  = Utils<ISwatchUtils>()->CreateProcessRGBSwatch(workspace,swtchName,red,green,blue);
					}
					break;
			}
		}

		parser->setColorList(refColor,col);

		swtchName.Clear();
		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}





/* Constructor
*/
Func_DP::Func_DP(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DP::Func_DP -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		while (!reader->IsEndOfCommand())
		{
			int32 ref = reader->ExtractInt(0,MIN_FONT,MAX_FONT) ;
			PMString name = reader->ExtractString(255);

			theFonts.insert(theFonts.end(), Elt(ref,name));
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
		IDCall_DP(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_DP::~Func_DP()
{

}


/* IDCall_DP
*/
void Func_DP::IDCall_DP(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		Vector::iterator iter;
		bool8 quit = kFalse ;

		for (iter = theFonts.begin(); iter < theFonts.end(); iter++)
		{
			if(parser->AddFont(iter->Key(),iter->Value()) != kSuccess)
			{
				error = ErrorUtils::PMGetGlobalErrorString();
				quit = kTrue;
				break;
			}
		}

		if (quit)
			break;

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}


/* Constructor
*/
Func_DJ::Func_DJ(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DJ::Func_DJ -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();
		reader->ExtractPourcent('+',100,0);
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

	if(!reader->IsEndOfCommand())
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* Constructor
*/
Func_DG::Func_DG(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_DG::Func_DG -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{
		reader->MustEgal();

		autoKern = reader->ExtractBooleen(kFalse);
		reader->ExtractReel('D','+',10,0);
		reader->ExtractReel('D','+',50,0);
		autoLead = reader->ExtractBooleen(kFalse);
		reader->ExtractBooleen(kTrue);
		reader->ExtractBooleen(kTrue);
		useLigatures = reader->ExtractInt(0,0,MAX_BLOC);
		reader->ExtractReel('D','+',40,0);
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
		IDCall_DG(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_DG::~Func_DG()
{
}


/* IDCall_DG
*/
void Func_DG::IDCall_DG(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		if(parser->getCurDoc() == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		InterfacePtr<ITxtAttrHelper> txtAttrHelper (parser->QueryTxtAttrHelper());
		if(txtAttrHelper == nil)
		{		
			ASSERT_FAIL("Func_DG::IDCall_DG -> txtAttrHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		InterfacePtr<IStyleHelper> styleHelper (parser->QueryStyleHelper());
		if(styleHelper == nil)
		{		
			ASSERT_FAIL("Func_DG::IDCall_DG -> styleHelper nil");
			error = PMString(kErrNilInterface);
			break;
		}

		boost::shared_ptr<AttributeBossList> attrList(new AttributeBossList);

		PMReal value = 12.0; // default : 12 pt leading
		if(autoLead)
			value = -1.0; // -1 to set auto leading
		
		if(txtAttrHelper->AddAutoLeadModeAttribute(attrList.get(),value) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		value = 0.0;
		if(autoKern)
			value = kAutoKernMagicNumber;
		
		if(txtAttrHelper->AddPairKernAttribute(attrList.get(),value) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		if(txtAttrHelper->AddLigaturesAttribute(attrList.get(),(useLigatures > 0)) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		// Edit default paragraph style called "NormalParagraphStyle" internally, "[Basic Paragraph]" in the GUI
		InterfacePtr<IStyleGroupManager> paraStyleNameTable (parser->getCurDoc()->GetDocWorkSpace(),IID_IPARASTYLEGROUPMANAGER);
		if(paraStyleNameTable == nil)
		{		
			ASSERT_FAIL("Func_DG::IDCall_DG -> paraStyleNameTable nil");
			error = PMString(kErrNilInterface);
			break;
		}

		PMString defaultStyleName("NormalParagraphStyle", PMString::kUnknownEncoding);
		UID defaultStyleUID = paraStyleNameTable->FindByName(defaultStyleName);
		if (defaultStyleUID == kInvalidUID)
				defaultStyleUID = paraStyleNameTable->GetDefaultStyleUID();

		if(styleHelper->EditTextStyleAttributesCmd(parser->getCurDoc()->GetDocWorkSpace(),defaultStyleUID,attrList.get()) != kSuccess)
		{
			error = ErrorUtils::PMGetGlobalErrorString();
			break;
		}

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}

/* Constructor
*/
Func_GN::Func_GN(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_GN::Func_GN -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		refGradient = reader->ExtractInt(0,MIN_COLOR,MAX_COLOR);
		swtchName = reader->ExtractString(255);
		type = reader->ExtractInt(1,1,2);
		nbStop = reader->ExtractInt(2,2,100);

		for(int32 i = 0 ; i < nbStop ; ++i)
		{
			refColors.push_back(reader->ExtractInt(0,0,MAX_COLOR));
			stopPositions.push_back(reader->ExtractPourcent('+',0,0));
			midPoints.push_back(reader->ExtractPourcent('+',50,0));
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
		IDCall_GN(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}


/* Destructor
*/
Func_GN::~Func_GN()
{
}


/* IDCall_GN
*/
void Func_GN::IDCall_GN(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		IDocument * doc = parser->getCurDoc();
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}

		swtchName.SetTranslatable(kFalse);

		InterfacePtr<IWorkspace> workspace (doc->GetDocWorkSpace (),UseDefaultIID());
		if(workspace == nil)
		{		
			ASSERT_FAIL("Func_CN::IDCall_CN -> workspace nil");
			error = PMString(kErrNilInterface);
			break;
		}

		IDataBase * db = parser->getCurDataBase();
		// Get the active swatch list
		InterfacePtr<ISwatchList> swList (workspace,UseDefaultIID());
		if(swList == nil)
		{		
			ASSERT_FAIL("Func_CN::IDCall_CN -> swList nil");
			error = PMString(kErrNilInterface);
			break;
		}
		
		sColor col;
		col.name = swtchName;
		col.colorUID = swList->FindSwatch(swtchName).GetUID();

		// If color doesn't already exist, create it
		if(col.colorUID == kInvalidUID)
		{
			InterfacePtr<IRenderingObject> renderObject(Utils<IGraphicStateUtils>()->CreateTemporaryRenderObject(kGradientRenderingObjectBoss));
			renderObject->SetSwatchName(swtchName);
			renderObject->SetVisibility(kTrue);

			InterfacePtr<IGradientFill> gradientData (renderObject, UseDefaultIID());

			gradientData->SetNumberOfStop(nbStop);	
			gradientData->SetGradientFillType((GradientType)type);

			stopPositions.push_back(1);
			for(int32 i = 0 ; i < nbStop ; ++i)
			{
				UID colorUID = parser->getColorList(refColors[i]).colorUID;
				if(colorUID == kInvalidUID)
					colorUID = Utils<ISwatchUtils>()->GetBlackSwatchUID(db);

				gradientData->SetNthGradientColorUID(i, colorUID);
				gradientData->SetNthStopPosition(i, stopPositions[i]);
				gradientData->SetNthGradientMidPoint(i, stopPositions[i] +  (midPoints[i] * (stopPositions[i+1] - stopPositions[i])));	
			}

			col.colorUID = Utils<ISwatchUtils>()->CreateNewSwatch(kGradientRenderingObjectBoss, gradientData, db, kFalse);
		}

		parser->setColorList(refGradient,col);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}

/* Constructor
*/
Func_CDeux::Func_CDeux(ITCLParser * parser)
{
	InterfacePtr<ITCLReader> reader(parser->QueryTCLReader());
	if(reader == nil)
	{		
		ASSERT_FAIL("Func_CDeux::Func_CDeux -> reader nil");
		throw TCLError(PMString(kErrNilInterface));
	}

	try
	{

		reader->MustEgal();

		refTint = reader->ExtractInt(0,MIN_COLOR,MAX_COLOR);
		baseColor = reader->ExtractInt(0, 0, MAX_COLOR);
		tint =  reader->ExtractPourcent('+',100,0);
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
		IDCall_CDeux(parser);
	else
		throw TCLError(PMString(kErrNoEndOfCommand));	
}

/* IDCall_CDeux
*/
void Func_CDeux::IDCall_CDeux(ITCLParser * parser)
{
	ErrorCode status = kFailure;
	PMString error("");

	do
	{
		// Check if there is a document open
		IDocument * doc = parser->getCurDoc();
		if(doc == nil)
		{
			error = PMString(kErrNeedDoc);		
			break;
		}


		InterfacePtr<IWorkspace> workspace (doc->GetDocWorkSpace (),UseDefaultIID());

		IDataBase * db = parser->getCurDataBase();
		// Get the active swatch list
		InterfacePtr<ISwatchList> swList (workspace,UseDefaultIID());
		
		// Find base color UID
		UID baseColorUID =  parser->getColorList(baseColor).colorUID;

		if(baseColorUID == kInvalidUID)
		{
			error = PMString(kErrColorNotFound);
			break;
		}

		InterfacePtr<IRenderingObject> baseColorObj (db, baseColorUID, UseDefaultIID());

		sColor col;
		col.name = baseColorObj->GetSwatchName();
		col.colorUID = Utils<ISwatchUtils>()->CreateTintedSwatch(UIDRef(db, baseColorUID), tint*100);

		parser->setColorList(refTint,col);

		status = kSuccess ;

	}while(false);
	
	if (status != kSuccess)
		throw TCLError(error);
		
}