/*
//	File:	TLBTxtAttrHelper.cpp
//
//  Author: Trias
//
//	Date:	18-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/


// Plug-in includes
#include "VCPlugInHeaders.h"

// General includes
#include "CmdUtils.h"
#include "UIDList.h"
#include "PreferenceUtils.h"
#include "ErrorUtils.h"
#include "CAlert.h"
#include "InstStrokeFillID.h"
#include "ScotchRulesID.h"
#include "FontMgrID.h"
#include "HZID.h"

// Interfaces includes
#include "ITextAttrUID.h"
#include "ITextAttrRealNumber.h"
#include "ITextAttrFont.h"
#include "ITextAttrUnderlineMode.h"
#include "ITextAttrBoolean.h"
#include "ITextAttrCapitalMode.h"
#include "ITextAttrPositionMode.h"
#include "IDrawingStyle.h"
#include "ILanguageList.h"
#include "ILanguage.h"
#include "ICommand.h"
#include "IUIDData.h"
#include "ITextAttrGridAlignment.h"
#include "ITextAttrInt16.h"
#include "ITextAttrAlign.h"
#include "ICompositionStyle.h"
#include "ITextAttrParaRuleMode.h"
#include "ITextAttrTabSettings.h"
#include "ITextAttrHyphenMode.h"
#include "IHyphenationStyle.h"
#include "ITextAttrStrokeType.h"
#include "IDashedAttributeValues.h"
#include "IDocument.h"
#include "ITextAttrClassID.h"

// Project includes
#include "TLBTxtAttrHelper.h"

CREATE_PMINTERFACE(TLBTxtAttrHelper,kTLBTxtAttrHelperImpl)

/* Constructor
*/
TLBTxtAttrHelper::TLBTxtAttrHelper(IPMUnknown * boss)
: CPMUnknown<ITxtAttrHelper>(boss)
{
}

/* Destructor
*/
TLBTxtAttrHelper::~TLBTxtAttrHelper()
{
}

/* AddAutoLeadModeAttribute
*/
ErrorCode TLBTxtAttrHelper::AddAutoLeadModeAttribute(AttributeBossList * attrList, PMReal value)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddAutoLeadModeAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrLeadBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddAutoLeadModeAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(value);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddAutoLeadAttribute
*/
ErrorCode TLBTxtAttrHelper::AddAutoLeadAttribute(AttributeBossList * attrList, PMReal percent)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddAutoLeadAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrAutoLeadBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddAutoLeadAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(percent + 1.0);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddPairKernAttribute
*/
ErrorCode TLBTxtAttrHelper::AddPairKernAttribute(AttributeBossList * attrList, PMReal value)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddPairKernAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPairKernBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddPairKernAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(value);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddColorAttribute
*/
ErrorCode TLBTxtAttrHelper::AddColorAttribute(AttributeBossList * attrList, UID colorUID)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		InterfacePtr<ITextAttrUID> textAttrUID_Color(::CreateObject2<ITextAttrUID>(kTextAttrColorBoss));
		if (textAttrUID_Color == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddColorAttribute -> attrList nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		
		if (colorUID == kInvalidUID)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddColorAttribute -> colorUID nil");
			error = PMString(kTlbNilParameter);
			break;
		}
		
		// Set our color attribute and store it:
		textAttrUID_Color->SetUIDData(colorUID);
		
		//add to the list
		attrList->ApplyAttribute(textAttrUID_Color);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddOverPrintFillAttribute
*/
ErrorCode TLBTxtAttrHelper::AddOverPrintFillAttribute(AttributeBossList * attrList, bool8 overPrint)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddOverPrintFillAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrBoolean> txtAttrBool ((ITextAttrBoolean *) ::CreateObject(kTextAttrOverprintBoss, IID_ITEXTATTRBOOLEAN));
		if(txtAttrBool == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddOverPrintFillAttribute -> txtAttrBool nil");
			break;
		}

		txtAttrBool->Set(overPrint);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrBool);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}
ErrorCode TLBTxtAttrHelper::AddFontSizeAttribute(AttributeBossList * attrList, PMReal fontSize)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// First, we create a text attribute real number to hold the font size:
		InterfacePtr<ITextAttrRealNumber> textAttrRealNumber(::CreateObject2<ITextAttrRealNumber>(kTextAttrPointSizeBoss));

		if (textAttrRealNumber == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddFontSizeAttribute: textAttrRealNumber invalid");
			error = PMString(kTlbNilInterface);
			break;
		}

		// Now we'll set the attribute's value:
		//const PMReal font_size(fontSize); // The size we want to change to.			
		textAttrRealNumber->SetRealNumber(fontSize);
		
		//add to the list
		attrList->ApplyAttribute(textAttrRealNumber);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddFontStyleAttribute
*/
ErrorCode TLBTxtAttrHelper::AddFontStyleAttribute(AttributeBossList * attrList, int16 fontStyle)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		// create the text attr font style attribute
		InterfacePtr<ITextAttrFont> txtAttrFontStyle ((ITextAttrFont *) ::CreateObject(kTextAttrFontStyleBoss, IID_ITEXTATTRFONT));
		if(txtAttrFontStyle == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddFontStyleAttribute -> txtAttrFontStyle nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		
		PMString style;

		// set the KeepLines attribute
		switch (fontStyle)
		{
		case 0 :
			style.Append("Regular");
			break;
		case 1 :
			style.Append("Bold");
			break;
		case 2 :
			style.Append("Italic");
			break;
		case 3 :
			style.Append("Bold Italic");
			break;
		default :
			style.Append("Regular");
			break;
		}

		style.SetTranslatable(kFalse);
		txtAttrFontStyle->SetFontName(style);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrFontStyle) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddXScaleAttribute
*/
ErrorCode TLBTxtAttrHelper::AddXScaleAttribute(AttributeBossList * attrList, PMReal percent)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddXScaleAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrXGlyphScaleBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddXScaleAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(percent);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddYScaleAttribute
*/
ErrorCode TLBTxtAttrHelper::AddYScaleAttribute(AttributeBossList * attrList, PMReal percent)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddXScaleAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrYGlyphScaleBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddXScaleAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(percent);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddTrackKernAttribute
*/
ErrorCode TLBTxtAttrHelper::AddTrackKernAttribute(AttributeBossList * attrList, PMReal value)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddTrackKernAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrTrackKernBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddTrackKernAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(value);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddBLShiftAttribute
*/
ErrorCode TLBTxtAttrHelper::AddBLShiftAttribute(AttributeBossList * attrList, PMReal value)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddTrackKernAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrBLShiftBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddTrackKernAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(value);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddTintAttribute
*/
ErrorCode TLBTxtAttrHelper::AddTintAttribute(AttributeBossList * attrList, PMReal percent)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddTrackKernAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrTintBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddTrackKernAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(percent);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddFontAttribute
*/
ErrorCode TLBTxtAttrHelper::AddFontAttribute(AttributeBossList * attrList, UID fontUID)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		InterfacePtr<ITextAttrUID> txtAttrUID((ITextAttrUID*)::CreateObject(kTextAttrFontUIDBoss, IID_ITEXTATTRUID));
		if (txtAttrUID == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddFontAttribute -> txtAttrUID nil");
			error = PMString(kTlbNilInterface);
			return kFailure;
		}

		txtAttrUID->SetUIDData(fontUID);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrUID) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddFontStyleAttribute
*/
ErrorCode TLBTxtAttrHelper::AddFontStyleAttribute(AttributeBossList * attrList, PMString fontStyle)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrFont>	attrFont(::CreateObject2<ITextAttrFont>(kTextAttrFontStyleBoss));
		if (attrFont == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddFontAttribute -> attrFont nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the font style.
		attrFont->SetFontName(fontStyle, kFalse);

		// apply the attribute
		attrList->ApplyAttribute(attrFont) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddUnderlineAttribute
*/

ErrorCode TLBTxtAttrHelper::AddUnderlineAttribute(AttributeBossList * attrList, bool8 underline)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrUnderlineMode> txtAttrUnderline(::CreateObject2<ITextAttrUnderlineMode>(kTextAttrUnderlineBoss));
		if (txtAttrUnderline == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddUnderlineAttribute -> txtAttrUnderline nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the underline mode
		if (underline)
			txtAttrUnderline->SetMode(IDrawingStyle::kUnderlineSingle);
		else
			txtAttrUnderline->SetMode(IDrawingStyle::kUnderlineNone);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrUnderline) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddStrikeThruAttribute
*/

ErrorCode TLBTxtAttrHelper::AddStrikeThruAttribute(AttributeBossList * attrList, bool8 strikeThru)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrBoolean> txtAttrStrikeThru(::CreateObject2<ITextAttrBoolean>(kTextAttrStrikethruBoss));
		if (txtAttrStrikeThru == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddStrikeThruAttribute -> txtAttrStrikeThru nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the strike thru attribute
		txtAttrStrikeThru->Set(strikeThru);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrStrikeThru) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddCapitalAttribute
*/

ErrorCode TLBTxtAttrHelper::AddCapitalAttribute(AttributeBossList * attrList, int32 capitalMode)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the capital text attribute
		InterfacePtr<ITextAttrCapitalMode> txtAttrCapital(::CreateObject2<ITextAttrCapitalMode>(kTextAttrCapitalModeBoss));
		if (txtAttrCapital == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddCapitalAttribute -> txtAttrCapital nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the capital attribute
		switch(capitalMode)
		{
		case 0:
			txtAttrCapital->SetMode(IDrawingStyle::kCapNormal);
			break;
		case 1:
			txtAttrCapital->SetMode(IDrawingStyle::kCapSmallLowercase );
			break;
		case 2:
			txtAttrCapital->SetMode(IDrawingStyle::kCapAll );
			break;
		default:
			txtAttrCapital->SetMode(IDrawingStyle::kCapNormal);
		}
		// apply the attribute
		attrList->ApplyAttribute(txtAttrCapital) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddPositionAttribute
*/
ErrorCode TLBTxtAttrHelper::AddPositionAttribute(AttributeBossList * attrList, int32 positionMode)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		// create the position text attribute
		InterfacePtr<ITextAttrPositionMode> txtAttrPos(::CreateObject2<ITextAttrPositionMode>(kTextAttrPositionModeBoss));
		if (txtAttrPos == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddPositionAttribute -> txtAttrPos nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the position attribute
		switch(positionMode)
		{
		case 0:
			txtAttrPos->SetMode(IDrawingStyle::kPosNormal);
			break;
		case 1:
		case 3:
			txtAttrPos->SetMode(IDrawingStyle::kPosSuperscript);
			break;
		case 2:
		case 4:
			txtAttrPos->SetMode(IDrawingStyle::kPosSubscript);
			break;
		//case 3:
		//	txtAttrPos->SetMode(IDrawingStyle::kPosSuperior);
		//	break;
		//case 4:
		//	txtAttrPos->SetMode(IDrawingStyle::kPosInferior);
		//	break;
		default:
			txtAttrPos->SetMode(IDrawingStyle::kPosNormal);
		}
		// apply the attribute
		attrList->ApplyAttribute(txtAttrPos) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddNoBreakAttribute
*/
ErrorCode TLBTxtAttrHelper::AddNoBreakAttribute(AttributeBossList * attrList, bool8 noBreak)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrBoolean> txtAttrNoBreak(::CreateObject2<ITextAttrBoolean>(kTextAttrNoBreakBoss));
		if (txtAttrNoBreak == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddNoBreakAttribute -> txtAttrNoBreak nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the strike thru attribute
		txtAttrNoBreak->Set(noBreak);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrNoBreak) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

ErrorCode TLBTxtAttrHelper::AddDropCapAttributes(AttributeBossList * attrList, int16 numberOfChars, int16 numberOfLines)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddDropCapAttributes -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrInt16> txtAttrChars ((ITextAttrInt16 *) ::CreateObject(kTextAttrDropCapCharsBoss,  IID_ITEXTATTRINT16));
		if(txtAttrChars == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddDropCapAttributes -> txtAttrChars nil");
			break;
		}

		txtAttrChars->Set(numberOfChars);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrChars);
		
		
		// Create an attribute boss
		InterfacePtr<ITextAttrInt16> txtAttrLines ((ITextAttrInt16 *) ::CreateObject(kTextAttrDropCapLinesBoss,  IID_ITEXTATTRINT16));
		if(txtAttrLines == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddDropCapAttributes -> txtAttrLines nil");
			break;
		}

		txtAttrLines->Set(numberOfLines);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrLines);

		status = kSuccess;
			
	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddSpaceBeforeAttribute
*/
ErrorCode TLBTxtAttrHelper::AddSpaceBeforeAttribute(AttributeBossList * attrList, PMReal spaceBefore)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddSpaceBeforeAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrSpaceBeforeBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddSpaceBeforeAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(spaceBefore);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddSpaceAfterAttribute
*/

ErrorCode TLBTxtAttrHelper::AddSpaceAfterAttribute(AttributeBossList * attrList, PMReal spaceAfter)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddSpaceAfterAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrSpaceAfterBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddSpaceBeforeAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(spaceAfter);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddGridAlignmentAttribute
*/

ErrorCode TLBTxtAttrHelper::AddGridAlignmentAttribute(AttributeBossList * attrList, bool8 gridAlign)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddGridAlignmentAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrGridAlignment> txtGridAlign ((ITextAttrGridAlignment *) ::CreateObject(kTextAttrGridAlignmentBoss, IID_ITEXTATTRGRIDALIGNMENT));
		if(txtGridAlign == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddGridAlignmentAttribute -> txtGridAlign nil");
			break;
		}

		if (gridAlign)
			txtGridAlign->SetGridAlignment(Text::kGABaseline);
		else
			txtGridAlign->SetGridAlignment(Text::kGANone);

		// Add it to the list
		attrList->ApplyAttribute(txtGridAlign);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddKeepWithNextAttribute
*/
ErrorCode TLBTxtAttrHelper::AddKeepWithNextAttribute(AttributeBossList * attrList, int16 numberOfLines)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrInt16> txtAttrKeepWithNext(::CreateObject2<ITextAttrInt16>(kTextAttrKeepWithNextBoss));
		if (txtAttrKeepWithNext == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddKeepWithNextAttribute -> txtAttrKeepWithNext nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the KeepLines attribute
		txtAttrKeepWithNext->Set(numberOfLines);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrKeepWithNext) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddKeepLinesAttribute
*/
ErrorCode TLBTxtAttrHelper::AddKeepLinesAttribute(AttributeBossList * attrList, bool8 keepLines)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrBoolean> txtAttrKeepLines(::CreateObject2<ITextAttrBoolean>(kTextAttrKeepLinesBoss));
		if (txtAttrKeepLines == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddKeepLinesAttribute -> txtAttrKeepLines nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the KeepLines attribute
		txtAttrKeepLines->Set(keepLines);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrKeepLines) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddKeepTogetherAttribute
*/

ErrorCode TLBTxtAttrHelper::AddKeepTogetherAttribute(AttributeBossList * attrList, bool8 keepTogether)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrBoolean> txtAttrKeepTogether(::CreateObject2<ITextAttrBoolean>(kTextAttrKeepTogetherBoss));
		if (txtAttrKeepTogether == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddKeepTogetherAttribute -> txtAttrKeepTogether nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the KeepLines attribute
		txtAttrKeepTogether->Set(keepTogether);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrKeepTogether) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddFirstLinesAttribute
*/

ErrorCode TLBTxtAttrHelper::AddFirstLinesAttribute(AttributeBossList * attrList, int32 firstLines)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrInt16> txtAttrFirstLines(::CreateObject2<ITextAttrInt16>(kTextAttrKeepFirstNLinesBoss));
		if (txtAttrFirstLines == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddFirstLinesAttribute -> txtAttrFirstLines nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the KeepLines attribute
		txtAttrFirstLines->Set(firstLines);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrFirstLines) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddLastLinesAttribute
*/

ErrorCode TLBTxtAttrHelper::AddLastLinesAttribute(AttributeBossList * attrList, int32 lastLines)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrInt16> txtAttrLastLines(::CreateObject2<ITextAttrInt16>(kTextAttrKeepLastNLinesBoss));
		if (txtAttrLastLines == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddLastLinesAttribute -> txtAttrLastLines nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the KeepLines attribute
		txtAttrLastLines->Set(lastLines);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrLastLines) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddAlignAttribute
*/

ErrorCode TLBTxtAttrHelper::AddAlignAttribute(AttributeBossList * attrList, int32 align)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrAlign> txtAttrAlign(::CreateObject2<ITextAttrAlign>(kTextAttrAlignmentBoss));
		if (txtAttrAlign == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddAlignAttribute -> txtAttrAlign nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the KeepLines attribute
		switch (align)
		{
		case 0 :
			txtAttrAlign->Set(ICompositionStyle::kTextAlignLeft);
			break;
		case 1 :
			txtAttrAlign->Set(ICompositionStyle::kTextAlignCenter);
			break;
		case 2 :
			txtAttrAlign->Set(ICompositionStyle::kTextAlignRight);
			break;
		case 3 :
			txtAttrAlign->Set(ICompositionStyle::kTextAlignJustifyFull);
			break;
		case 4 :
			txtAttrAlign->Set(ICompositionStyle::kTextAlignJustifyLeft);
			break;
		case 5 :
			txtAttrAlign->Set(ICompositionStyle::kTextAlignJustifyCenter);
			break;
		case 6 :
			txtAttrAlign->Set(ICompositionStyle::kTextAlignJustifyRight);
			break;
		case 7 :
			txtAttrAlign->Set(ICompositionStyle::kTextAlignToBinding);
			break;
		case 8 :
			txtAttrAlign->Set(ICompositionStyle::kTextAlignAwayBinding);
			break;
		default :
			txtAttrAlign->Set(ICompositionStyle::kTextAlignJustifyFull);
			break;
		}

		// apply the attribute
		attrList->ApplyAttribute(txtAttrAlign) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddParaRuleAboveColorAttribute
*/
ErrorCode TLBTxtAttrHelper::AddParaRuleAboveColorAttribute(AttributeBossList * attrList, UID colorUID)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		InterfacePtr<ITextAttrUID> textAttrUID_Color(::CreateObject2<ITextAttrUID>(kTextAttrPRAColorBoss));
		if (textAttrUID_Color == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveColorAttribute -> attrList nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		
		if (colorUID == kInvalidUID)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveColorAttribute -> colorUID nil");
			error = PMString(kTlbNilParameter);
			break;
		}
		
		// Set our color attribute and store it:
		textAttrUID_Color->SetUIDData(colorUID);
		
		//add to the list
		attrList->ApplyAttribute(textAttrUID_Color);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddParaRuleAboveIndentLAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleAboveIndentLAttribute(AttributeBossList * attrList, PMReal indentL)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveIndentLAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPRAIndentLBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveIndentLAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(indentL);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddParaRuleAboveIndentRAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleAboveIndentRAttribute(AttributeBossList * attrList, PMReal indentR)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveIndentRAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPRAIndentRBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveIndentRAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(indentR);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddParaRuleAboveModeAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleAboveModeAttribute(AttributeBossList * attrList, int32 mode)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveModeAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrParaRuleMode> txtAttrMode ((ITextAttrParaRuleMode *) ::CreateObject(kTextAttrPRAModeBoss, IID_ITEXTATTRPARARULEMODE));
		if(txtAttrMode == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveModeAttribute -> txtAttrReal nil");
			break;
		}

		switch(mode)
		{
		case 0 :
			txtAttrMode->Set(ICompositionStyle::kRule_ColumnWidth);
			break;
		case 1 :
			txtAttrMode->Set(ICompositionStyle::kRule_TextWidth);
			break;
		default:
			txtAttrMode->Set(ICompositionStyle::kRule_None);
			break;
		}
		

		// Add it to the list
		attrList->ApplyAttribute(txtAttrMode);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddParaRuleAboveOffsetAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleAboveOffsetAttribute(AttributeBossList * attrList, PMReal offset)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveOffsetAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPRAOffsetBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveOffsetAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(offset);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddParaRuleAboveOnAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleAboveOnAttribute(AttributeBossList * attrList, bool8 ruleOn)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrBoolean> txtAttrRuleOn(::CreateObject2<ITextAttrBoolean>(kTextAttrPRARuleOnBoss));
		if (txtAttrRuleOn == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveOnAttribute -> txtAttrKeepTogether nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the KeepLines attribute
		txtAttrRuleOn->Set(ruleOn);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrRuleOn) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddParaRuleAboveStrokeAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleAboveStrokeAttribute(AttributeBossList * attrList, PMReal stroke)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveStrokeAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPRAStrokeBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveStrokeAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(stroke);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddParaRuleAboveTintAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleAboveTintAttribute(AttributeBossList * attrList, PMReal tint)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveTintAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPRATintBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleAboveTintAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(tint);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

ErrorCode TLBTxtAttrHelper::AddParaRuleAboveTypeAttribute(AttributeBossList * attrList, int8 type)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrStrokeType> txtAttrType ((ITextAttrStrokeType *) ::CreateObject(kTextAttrPRAStrokeTypeBoss, IID_ITEXTATTRSTROKETYPE));
		if(txtAttrType == nil)
		{
			error = PMString(kTlbNilParameter);
			break;
		}
		
		ClassID ruleImpl = kInvalidClass;
		switch(type)
		{
			case 0 : ruleImpl = kSolidPathStrokerBoss; break;
			case 1 : ruleImpl = kDashedPathStrokerBoss; break;
			case 2 : ruleImpl = kThinThinPathStrokerBoss; break;
			case 3 : ruleImpl = kThickThickPathStrokerBoss; break;
			case 4 : ruleImpl = kDashedPathStrokerBoss; break;
			case 5 : ruleImpl = kCannedDotPathStrokerBoss; break;
			case 6 : ruleImpl = kThinThickPathStrokerBoss; break;
			case 7 : ruleImpl = kThickThinPathStrokerBoss; break;
			case 8 : ruleImpl = kThinThickThinPathStrokerBoss; break;
			case 9 : ruleImpl = kThickThinThickPathStrokerBoss; break;
			case 10 : ruleImpl = kTriplePathStrokerBoss; break;
			case 11 : ruleImpl = kCannedDash3x2PathStrokerBoss; break;
			case 12 : ruleImpl = kCannedDash4x4PathStrokerBoss; break;
			case 13 : ruleImpl = kLeftSlantHashPathStrokerBoss; break;
			case 14 : ruleImpl = kRightSlantHashPathStrokerBoss; break;
			case 15 : ruleImpl = kStraightHashPathStrokerBoss; break;
			case 16 : ruleImpl = kSingleWavyPathStrokerBoss; break;
			case 17 : ruleImpl = kWhiteDiamondPathStrokerBoss; break;
			case 18 : ruleImpl = kJapaneseDotsPathStrokerBoss; break;
			default : break;
		}

		txtAttrType->SetClassIDData(ruleImpl);
		txtAttrType->SetUIDData(kInvalidUID);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrType);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddParaRuleBelowColorAttribute
*/
ErrorCode TLBTxtAttrHelper::AddParaRuleBelowColorAttribute(AttributeBossList * attrList, UID colorUID)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		InterfacePtr<ITextAttrUID> textAttrUID_Color(::CreateObject2<ITextAttrUID>(kTextAttrPRBColorBoss));
		if (textAttrUID_Color == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowColorAttribute -> attrList nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		
		if (colorUID == kInvalidUID)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowColorAttribute -> colorUID nil");
			error = PMString(kTlbNilParameter);
			break;
		}
		
		// Set our color attribute and store it:
		textAttrUID_Color->SetUIDData(colorUID);
		
		//add to the list
		attrList->ApplyAttribute(textAttrUID_Color);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddParaRuleBelowIndentLAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleBelowIndentLAttribute(AttributeBossList * attrList, PMReal indentL)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowIndentLAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPRBIndentLBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowIndentLAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(indentL);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddParaRuleBelowIndentRAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleBelowIndentRAttribute(AttributeBossList * attrList, PMReal indentR)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowIndentRAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPRBIndentRBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowIndentRAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(indentR);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddParaRuleBelowModeAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleBelowModeAttribute(AttributeBossList * attrList, int32 mode)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowModeAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrParaRuleMode> txtAttrMode ((ITextAttrParaRuleMode *) ::CreateObject(kTextAttrPRBModeBoss, IID_ITEXTATTRPARARULEMODE));
		if(txtAttrMode == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowModeAttribute -> txtAttrReal nil");
			break;
		}

		switch(mode)
		{
		case 0 :
			txtAttrMode->Set(ICompositionStyle::kRule_ColumnWidth);
			break;
		case 1 :
			txtAttrMode->Set(ICompositionStyle::kRule_TextWidth);
			break;
		default:
			txtAttrMode->Set(ICompositionStyle::kRule_None);
			break;
		}
		

		// Add it to the list
		attrList->ApplyAttribute(txtAttrMode);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddParaRuleBelowOffsetAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleBelowOffsetAttribute(AttributeBossList * attrList, PMReal offset)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowOffsetAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPRBOffsetBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowOffsetAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(offset);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddParaRuleBelowOnAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleBelowOnAttribute(AttributeBossList * attrList, bool8 ruleOn)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the text attr font style attribute
		InterfacePtr<ITextAttrBoolean> txtAttrRuleOn(::CreateObject2<ITextAttrBoolean>(kTextAttrPRBRuleOnBoss));
		if (txtAttrRuleOn == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowOnAttribute -> txtAttrKeepTogether nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the KeepLines attribute
		txtAttrRuleOn->Set(ruleOn);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrRuleOn) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddParaRuleBelowStrokeAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleBelowStrokeAttribute(AttributeBossList * attrList, PMReal stroke)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowStrokeAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPRBStrokeBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowStrokeAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(stroke);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddParaRuleBelowTintAttribute
*/

ErrorCode TLBTxtAttrHelper::AddParaRuleBelowTintAttribute(AttributeBossList * attrList, PMReal tint)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowTintAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrPRBTintBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddParaRuleBelowTintAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(tint);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

ErrorCode TLBTxtAttrHelper::AddParaRuleBelowTypeAttribute(AttributeBossList * attrList, int8 type)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrStrokeType> txtAttrType ((ITextAttrStrokeType *) ::CreateObject(kTextAttrPRBStrokeTypeBoss, IID_ITEXTATTRSTROKETYPE));
		if(txtAttrType == nil)
		{
			error = PMString(kTlbNilParameter);
			break;
		}

		ClassID ruleImpl = kInvalidClass;
		switch(type)
		{
			case 0 : ruleImpl = kSolidPathStrokerBoss; break;
			case 1 : ruleImpl = kDashedPathStrokerBoss; break;
			case 2 : ruleImpl = kThinThinPathStrokerBoss; break;
			case 3 : ruleImpl = kThickThickPathStrokerBoss; break;
			case 4 : ruleImpl = kDashedPathStrokerBoss; break;
			case 5 : ruleImpl = kCannedDotPathStrokerBoss; break;
			case 6 : ruleImpl = kThinThickPathStrokerBoss; break;
			case 7 : ruleImpl = kThickThinPathStrokerBoss; break;
			case 8 : ruleImpl = kThinThickThinPathStrokerBoss; break;
			case 9 : ruleImpl = kThickThinThickPathStrokerBoss; break;
			case 10 : ruleImpl = kTriplePathStrokerBoss; break;
			case 11 : ruleImpl = kCannedDash3x2PathStrokerBoss; break;
			case 12 : ruleImpl = kCannedDash4x4PathStrokerBoss; break;
			case 13 : ruleImpl = kLeftSlantHashPathStrokerBoss; break;
			case 14 : ruleImpl = kRightSlantHashPathStrokerBoss; break;
			case 15 : ruleImpl = kStraightHashPathStrokerBoss; break;
			case 16 : ruleImpl = kSingleWavyPathStrokerBoss; break;
			case 17 : ruleImpl = kWhiteDiamondPathStrokerBoss; break;
			case 18 : ruleImpl = kJapaneseDotsPathStrokerBoss; break;
			default : break;
		}

		txtAttrType->SetClassIDData(ruleImpl);
		txtAttrType->SetUIDData(kInvalidUID);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrType);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddIndent1LAttribute
*/

ErrorCode TLBTxtAttrHelper::AddIndent1LAttribute(AttributeBossList * attrList, PMReal value)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddIndent1LAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrIndent1LBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddIndent1LAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(value);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddIndentLAttribute
*/

ErrorCode TLBTxtAttrHelper::AddIndentLAttribute(AttributeBossList * attrList, PMReal value)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddIndentLAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrIndentBLBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddIndentLAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(value);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddIndentRAttribute
*/

ErrorCode TLBTxtAttrHelper::AddIndentRAttribute(AttributeBossList * attrList, PMReal value)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddIndentRAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrRealNumber> txtAttrReal ((ITextAttrRealNumber *) ::CreateObject(kTextAttrIndentBRBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrReal == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddIndentRAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrReal->SetRealNumber(value);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrReal);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddTabsAttribute
*/

ErrorCode TLBTxtAttrHelper::AddTabsAttribute(AttributeBossList * attrList, TabStopTable * tabTable)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddTabsAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrTabSettings> txtAttrTabs ((ITextAttrTabSettings *) ::CreateObject(kTextAttrTabsBoss, IID_ITEXTATTRTABS));
		if(txtAttrTabs == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddTabsAttribute -> txtAttrReal nil");
			break;
		}

		txtAttrTabs->SetTabStopTable(*tabTable);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrTabs);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}




/* AddHyphenCapAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddHyphenCapAttribute(AttributeBossList * attrList, bool8 hyphenCap)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddHyphenCapAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrBoolean> txtAttrHyphenCap ((ITextAttrBoolean *) ::CreateObject(kTextAttrHyphenCapBoss, IID_ITEXTATTRBOOLEAN));
		if(txtAttrHyphenCap == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddHyphenCapAttribute -> txtAttrHyphenCap nil");
			break;
		}

		txtAttrHyphenCap->Set(hyphenCap);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrHyphenCap);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddHyphenWeightAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddHyphenWeightAttribute(AttributeBossList * attrList, int16 hyphenWeight)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddHyphenCapAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrInt16> txtAttrHyphenWeight ((ITextAttrInt16 *) ::CreateObject(kTextAttrHyphenWeightBoss, IID_ITEXTATTRINT16));
		if(txtAttrHyphenWeight == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddHyphenWeightAttribute -> txtAttrHyphenWeight nil");
			break;
		}

		txtAttrHyphenWeight->Set(hyphenWeight);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrHyphenWeight);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddHyphenZoneAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddHyphenZoneAttribute(AttributeBossList * attrList, PMReal hyphenZone)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddHyphenZoneAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrRealNumber> txtAttrHyphenZone ((ITextAttrRealNumber *) ::CreateObject(kTextAttrHyphenZoneBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrHyphenZone == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddHyphenZoneAttribute -> txtAttrHyphenWeight nil");
			break;
		}

		txtAttrHyphenZone->SetRealNumber(hyphenZone);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrHyphenZone);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddMinAfterAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddMinAfterAttribute(AttributeBossList * attrList, int16 minAfter)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddMinAfterAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrInt16> txtAttrMinAfter ((ITextAttrInt16 *) ::CreateObject(kTextAttrMinAfterBoss, IID_ITEXTATTRINT16));
		if(txtAttrMinAfter == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddMinAfterAttribute -> txtAttrMinAfter nil");
			break;
		}

		txtAttrMinAfter->Set(minAfter);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrMinAfter);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddMinBeforeAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddMinBeforeAttribute(AttributeBossList * attrList, int16 minBefore)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddMinBeforeAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrInt16> txtAttrMinBefore ((ITextAttrInt16 *) ::CreateObject(kTextAttrMinBeforeBoss, IID_ITEXTATTRINT16));
		if(txtAttrMinBefore == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddMinBeforeAttribute -> txtAttrMinBefore nil");
			break;
		}

		txtAttrMinBefore->Set(minBefore);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrMinBefore);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddHyphenModeAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddHyphenModeAttribute(AttributeBossList * attrList, bool8 actived)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddHyphenModeAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrHyphenMode> txtAttrHyphenMode ((ITextAttrHyphenMode *) ::CreateObject(kTextAttrHyphenModeBoss, IID_ITEXTATTRHYPHENMODE));
		if(txtAttrHyphenMode == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddHyphenModeAttribute -> txtAttrHyphenMode nil");
			break;
		}

		if (actived)
			txtAttrHyphenMode->SetMethod(IHyphenationStyle::kHyphensAlgorithm);
		else
			txtAttrHyphenMode->SetMethod(IHyphenationStyle::kHyphensOff);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrHyphenMode);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddHyphenLastWordAttribute
*/
ErrorCode TLBTxtAttrHelper::AddHyphenLastWordAttribute(AttributeBossList * attrList, bool8 lastWord)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrBoolean> txtAttrHyphenLastWord ((ITextAttrBoolean *) ::CreateObject(kTextAttrHyphenLastBoss, IID_ITEXTATTRBOOLEAN));
		if(txtAttrHyphenLastWord == nil)
		{
			break;
		}

		txtAttrHyphenLastWord->Set(lastWord);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrHyphenLastWord);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddLetterspaceDesAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddLetterspaceDesAttribute(AttributeBossList * attrList, PMReal letterspace)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddLetterspaceDesAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrRealNumber> txtAttrLetterspaceDes ((ITextAttrRealNumber *) ::CreateObject(kTextAttrLetterspaceDesBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrLetterspaceDes == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddLetterspaceDesAttribute -> txtAttrHyphenWeight nil");
			break;
		}

		txtAttrLetterspaceDes->SetRealNumber(letterspace);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrLetterspaceDes);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddLetterspaceMaxAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddLetterspaceMaxAttribute(AttributeBossList * attrList, PMReal letterspace)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddLetterspaceMaxAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrRealNumber> txtAttrLetterspaceMax ((ITextAttrRealNumber *) ::CreateObject(kTextAttrLetterspaceMaxBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrLetterspaceMax == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddLetterspaceMaxAttribute -> txtAttrHyphenWeight nil");
			break;
		}

		txtAttrLetterspaceMax->SetRealNumber(letterspace);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrLetterspaceMax);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddLetterspaceMinAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddLetterspaceMinAttribute(AttributeBossList * attrList, PMReal letterspace)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddLetterspaceMinAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrRealNumber> txtAttrLetterspaceMin ((ITextAttrRealNumber *) ::CreateObject(kTextAttrLetterspaceMinBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrLetterspaceMin == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddLetterspaceMinAttribute -> txtAttrHyphenWeight nil");
			break;
		}

		txtAttrLetterspaceMin->SetRealNumber(letterspace);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrLetterspaceMin);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddWordspaceDesAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddWordspaceDesAttribute(AttributeBossList * attrList, PMReal wordspace)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddWordspaceDesAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrRealNumber> txtAttrWordspaceDes ((ITextAttrRealNumber *) ::CreateObject(kTextAttrWordspaceDesBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrWordspaceDes == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddWordspaceDesAttribute -> txtAttrHyphenWeight nil");
			break;
		}

		txtAttrWordspaceDes->SetRealNumber(wordspace);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrWordspaceDes);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddWordspaceMaxAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddWordspaceMaxAttribute(AttributeBossList * attrList, PMReal wordspace)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddWordspaceMaxAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrRealNumber> txtAttrWordspaceMax ((ITextAttrRealNumber *) ::CreateObject(kTextAttrWordspaceMaxBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrWordspaceMax == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddWordspaceMaxAttribute -> txtAttrHyphenWeight nil");
			break;
		}

		txtAttrWordspaceMax->SetRealNumber(wordspace);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrWordspaceMax);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddWordspaceMinAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddWordspaceMinAttribute(AttributeBossList * attrList, PMReal wordspace)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddWordspaceMinAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrRealNumber> txtAttrWordspaceMin ((ITextAttrRealNumber *) ::CreateObject(kTextAttrWordspaceMinBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrWordspaceMin == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddWordspaceMinAttribute -> txtAttrHyphenWeight nil");
			break;
		}

		txtAttrWordspaceMin->SetRealNumber(wordspace);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrWordspaceMin);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}



/* AddShortestWordAttribute 
*/

ErrorCode TLBTxtAttrHelper::AddShortestWordAttribute(AttributeBossList * attrList, int16 length)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddShortestWordAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrInt16> txtAttrShort ((ITextAttrInt16 *) ::CreateObject(kTextAttrShortestWordBoss, IID_ITEXTATTRINT16));
		if(txtAttrShort == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddShortestWordAttribute -> txtAttrShort nil");
			break;
		}

		txtAttrShort->Set(length);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrShort);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}


/* AddLadderAttribute 
*/
ErrorCode TLBTxtAttrHelper::AddLadderAttribute(AttributeBossList * attrList, int16 limit)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddLadderAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrInt16> txtAttrLadder ((ITextAttrInt16 *) ::CreateObject(kTextAttrHyphenLadderBoss, IID_ITEXTATTRINT16));
		if(txtAttrLadder == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddLadderAttribute -> txtAttrLadder nil");
			break;
		}

		txtAttrLadder->Set(limit);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrLadder);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddSkewAngleAttribute 
*/
ErrorCode TLBTxtAttrHelper::AddSkewAngleAttribute(AttributeBossList * attrList, PMReal angle) 
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);
	
	do
	{

		if(attrList == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddSkewAngleAttribute -> attrList nil");
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss 
		InterfacePtr<ITextAttrRealNumber> txtAttrSkew ((ITextAttrRealNumber *) ::CreateObject(kTextAttrSkewAngleBoss, IID_ITEXTATTRREALNUMBER));
		if(txtAttrSkew == nil)
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddSkewAngleAttribute -> txtAttrSkew nil");
			break;
		}

		txtAttrSkew->Set(angle);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrSkew);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddLigaturesAttribute
*/
ErrorCode TLBTxtAttrHelper::AddLigaturesAttribute(AttributeBossList * attrList, bool8 useLigatures)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		// create the ligatures text attribute
		InterfacePtr<ITextAttrBoolean> txtAttrLigatures(::CreateObject2<ITextAttrBoolean>(kTextAttrLigaturesBoss));
		if (txtAttrLigatures == nil) 
		{
			ASSERT_FAIL("TLBTxtAttrHelper::AddLigaturesAttribute -> txtAttrLigatures nil");
			error = PMString(kTlbNilInterface);
			break;
		}
		// set the strike thru attribute
		txtAttrLigatures->Set(useLigatures);

		// apply the attribute
		attrList->ApplyAttribute(txtAttrLigatures) ;
		
		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}

/* AddLanguageAttribute
*/
ErrorCode TLBTxtAttrHelper::AddLanguageAttribute(AttributeBossList * attrList, IDocument * doc, LanguageID lang)
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{

		InterfacePtr<ILanguageList> docLanglist ((ILanguageList *) ::QueryPreferences(IID_ILANGUAGELIST,doc));
		if(docLanglist == nil)
			break;

		InterfacePtr<ILanguage> language (docLanglist->QueryLanguageByID(lang));
		UID languageUID = kInvalidUID;
		if(language == nil)
		{
			InterfacePtr<ILanguageList>appLanguageList((ILanguageList*)::QuerySessionPreferences(IID_ILANGUAGELIST));
			if(appLanguageList == nil)
				break;

			InterfacePtr<ILanguage> appLanguage (appLanguageList->QueryLanguageByID(lang));
			if (appLanguage)
			{
				InterfacePtr<ICommand> newLangCmd (CmdUtils::CreateCommand(kNewLanguageCmdBoss));
				if(newLangCmd == nil) 
					break;

				InterfacePtr<ILanguage> cmdData (newLangCmd, UseDefaultIID());
				if(cmdData == nil)	
					break;

				newLangCmd->SetItemList(UIDList(docLanglist));
				cmdData->CopyFrom(appLanguage);

				if(CmdUtils::ProcessCommand(newLangCmd) != kSuccess)
					break;

				InterfacePtr<ILanguageList> newDocLanguageList ((ILanguageList*)::QueryPreferences(IID_ILANGUAGELIST, doc));
				if(newDocLanguageList == nil) 
					break;

				InterfacePtr<const ILanguage> newLanguage (newDocLanguageList->QueryLanguageByID(lang));
				if(newLanguage == nil)	
					break;

				languageUID = ::GetUID(newLanguage);
			}
			else
			{
				error = PMString(kErrLanguageNotDefined);
				break;
			}
		}
		else
			languageUID = ::GetUID(language);

		InterfacePtr<ITextAttrUID> txtAttrUID ((ITextAttrUID *) ::CreateObject(kTextAttrLanguageBoss, IID_ITEXTATTRUID));
		txtAttrUID->Set(languageUID);

		attrList->ApplyAttribute(txtAttrUID);	

		status = kSuccess;

	} while(false);
	
	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;

}

/** AddPairKernMethodAttribute
*/
ErrorCode TLBTxtAttrHelper::AddPairKernMethodAttribute(AttributeBossList * attrList, int32 method) 
{
	ErrorCode status = kFailure;
	PMString error(kTlbNilInterface);

	do
	{
		if(attrList == nil)
		{
			error = PMString(kTlbNilParameter);
			break;
		}

		// Create an attribute boss
		InterfacePtr<ITextAttrClassID> txtAttrMethod ((ITextAttrClassID *) ::CreateObject(kTextAttrPairKernMethodBoss, IID_ITEXTATTRCLASSID));
		if(txtAttrMethod == nil)
			break;
		
		ClassID methodID = kInvalidClass;
		switch(method)
		{
			case 0 : methodID = kDefaultPairKerningBoss; break;
			case 1 : methodID = kKFPairKerningBoss; break;
			case 2 : methodID = kPairKerningOffBoss; break;
			default :  methodID = kDefaultPairKerningBoss; break;
		}

		txtAttrMethod->Set(methodID);

		// Add it to the list
		attrList->ApplyAttribute(txtAttrMethod);

		status = kSuccess;

	} while(false);

	if(status != kSuccess)
		ErrorUtils::PMSetGlobalErrorCode(status);

	return status;
}
