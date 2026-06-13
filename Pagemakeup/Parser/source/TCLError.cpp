/*
//	File:	TCLError.cpp
//
//	Date:	23-Apr-2003
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// General includes
#include "CAlert.h"

// Project includes
#include "TCLError.h"
#include "PrsID.h"
#include "PLBID.h"
#include "DLBID.h"
#include "TLBID.h"
#include "FLBID.h"

/* Constructor
*/
TCLError::TCLError(PMString errorKey)
{
	this->message = errorKey;
	SetErrorCode(errorKey);
}

/* Destructor
*/
TCLError::~TCLError()
{
}

/* getMessage
*/
inline PMString TCLError::getMessage()
{
	return this->message;
}

/* setMessage
*/
inline void TCLError::setMessage(PMString message)
{
	this->message = message;
}

/* DisplayError
*/
void TCLError::DisplayError()
{	
	if(!message.HasTranslated())
		message.Translate();

	CAlert::WarningAlert(message);
}

/* GetErrorCode
*/
int32 TCLError::GetErrorCode()
{
	return errorCode;	
}

/* GetErrorCode
*/
void TCLError::SetErrorCode(PMString tclError)
{
	errorCode = 0;

	if(tclError.Compare(kFalse,PMString(kErrEOF)) == 0) errorCode = 1;
	else if(tclError.Compare(kFalse,PMString(kErrUnknownCommand)) == 0) errorCode = 2;
	else if(tclError.Compare(kFalse,PMString(kErrNoEndOfCommand)) == 0) errorCode = 3;
	
	
	else if(tclError.Compare(kFalse,PMString(kErrNoStringHeader)) == 0) errorCode = 6;	
	else if(tclError.Compare(kFalse,PMString(kErrNoEOString)) == 0) errorCode = 7;
	else if(tclError.Compare(kFalse,PMString(kErrLength)) == 0) errorCode = 8;	
	else if(tclError.Compare(kFalse,PMString(kErrEOVariable)) == 0) errorCode = 9;
	else if(tclError.Compare(kFalse,PMString(kErrNoString)) == 0) errorCode = 10;
	
	else if(tclError.Compare(kFalse,PMString(kErrEgal)) == 0) errorCode = 11;
	else if(tclError.Compare(kFalse,PMString(kErrNoEgal)) == 0) errorCode = 12;
	
	else if(tclError.Compare(kFalse,PMString(kErrIntTooBig)) == 0) errorCode = 14;
	else if(tclError.Compare(kFalse,PMString(kErrBadInt)) == 0) errorCode = 15;
	else if(tclError.Compare(kFalse,PMString(kErrSupMax)) == 0) errorCode = 16;
	else if(tclError.Compare(kFalse,PMString(kErrInfMin)) == 0) errorCode = 17;
	
	
	else if(tclError.Compare(kFalse,PMString(kErrNoUnite)) == 0) errorCode = 18;
	else if(tclError.Compare(kFalse,PMString(kErrBadUnite)) == 0) errorCode = 19;
	else if(tclError.Compare(kFalse,PMString(kErrBadEndUnite)) == 0) errorCode = 20;
	
	
	else if(tclError.Compare(kFalse,PMString(kErrBadBool)) == 0) errorCode = 22;
	else if(tclError.Compare(kFalse,PMString(kErrBadEndBool)) == 0) errorCode = 23;
	
	else if(tclError.Compare(kFalse,PMString(kErrPEntTooBig)) == 0) errorCode = 25;
	else if(tclError.Compare(kFalse,PMString(kErrBadReal)) == 0) errorCode = 27;

	else if(tclError.Compare(kFalse,PMString(kErrBadPourcent)) == 0) errorCode = 33;

	else if(tclError.Compare(kFalse,PMString(kErrCharHeader)) == 0) errorCode = 36;
	else if(tclError.Compare(kFalse,PMString(kErrNoEOChar)) == 0) errorCode = 37;
	else if(tclError.Compare(kFalse,PMString(kErrNoChar)) == 0) errorCode = 39;


	else if(tclError.Compare(kFalse,PMString(kErrItemNotOnSameSpread)) == 0) errorCode = 45;
	
	else if(tclError.Compare(kFalse,PMString(kAlreadyDefiningStyle)) == 0) errorCode = 60;
	else if(tclError.Compare(kFalse,PMString(kErrNotDefining)) == 0) errorCode = 62;
	else if(tclError.Compare(kFalse,PMString(kErrNotAllowedDuringDefine)) == 0) errorCode = 63;
	else if(tclError.Compare(kFalse,PMString(kErrStyleNotFound)) == 0) errorCode = 64;
	
	else if(tclError.Compare(kFalse,PMString(kErrNeedDoc)) == 0) errorCode = 68;
	else if(tclError.Compare(kFalse,PMString(kErrNotEnoughItem)) == 0) errorCode = 69;
	
	else if(tclError.Compare(kFalse,PMString(kErrCJNotFound)) == 0) errorCode = 73;
	
	else if(tclError.Compare(kFalse,PMString(kErrCannotReadFile)) == 0) errorCode = 77;

	else if(tclError.Compare(kFalse,PMString(kErrFontNotFound)) == 0) errorCode = 80;
	
	else if(tclError.Compare(kFalse,PMString(kErrUserBreak)) == 0) errorCode = 83;
	
	else if(tclError.Compare(kFalse,PMString(kErrCannotOpenFile)) == 0) errorCode = 92;
	else if(tclError.Compare(kFalse,PMString(kErrNoSuchColor)) == 0) errorCode = 93;
	else if(tclError.Compare(kFalse,PMString(kErrTextBoxRequired)) == 0) errorCode = 94; 
	else if(tclError.Compare(kFalse,PMString(kErrJVImpossible)) == 0) errorCode = 95;
	
	else if(tclError.Compare(kFalse,PMString(kErrUnknownObject)) == 0) errorCode = 97;
	
	else if(tclError.Compare(kFalse,PMString(kErrNotEnoughArgument)) == 0) errorCode = 99; // Use by BG & BD & EV

	else if(tclError.Compare(kFalse,PMString(kErrLongIntTooBig)) == 0) errorCode = 101;	
	
	else if(tclError.Compare(kFalse,PMString(kErrDocNotFound)) == 0) errorCode = 112;
	else if(tclError.Compare(kFalse,PMString(kErrOpenDoc)) == 0) errorCode = 113;
	
	else if(tclError.Compare(kFalse,PMString(kErrInvalidMaster)) == 0) errorCode = 119;
	
	// New error codes
	else if(tclError.Compare(kFalse,PMString(kErrImpossibleToSave)) == 0) errorCode = 200;
	else if(tclError.Compare(kFalse,PMString(kErrTooManySavedItems)) == 0) errorCode = 201;
	else if(tclError.Compare(kFalse,PMString(kErrCmdFailed)) == 0) errorCode = 202;
	else if(tclError.Compare(kFalse,PMString(kErrInsertTxt)) == 0) errorCode = 203;
	else if(tclError.Compare(kFalse,PMString(kErrRefInvalid)) == 0) errorCode = 205;
	
	else if(tclError.Compare(kFalse,PMString(kErrRealTooBig)) == 0) errorCode = 206; 
	else if(tclError.Compare(kFalse,PMString(kErrRealTooSmall)) == 0) errorCode = 207;
	
	else if(tclError.Compare(kFalse,PMString(kErrConv)) == 0) errorCode = 208; // Conversion error (new)
	
	else if(tclError.Compare(kFalse,PMString(kErrImageBoxRequired)) == 0) errorCode = 209; // self explanatory
	else if(tclError.Compare(kFalse,PMString(kErrBoxRequired)) == 0) errorCode = 210; // A rule is needed
	else if(tclError.Compare(kFalse,PMString(kErrItemOutOfSpread)) == 0) errorCode = 211; // self explanatory
	else if(tclError.Compare(kFalse,PMString(kErrNotRightArgumentNumber)) == 0) errorCode = 212; // Use by BG & EB
	else if(tclError.Compare(kFalse,PMString(kErrColorNotFound)) == 0) errorCode = 213;
	
	else if(tclError.Compare(kFalse,PMString(kErrSpreadHierarchy)) == 0) errorCode = 214;
	else if(tclError.Compare(kFalse,PMString(kErrNumPageTooBig)) == 0) errorCode = 215;
	else if(tclError.Compare(kFalse,PMString(kErrNumPageTooSmall)) == 0) errorCode = 216;
	else if(tclError.Compare(kFalse,PMString(kErrImpossibleToDelete)) == 0) errorCode = 217;
	else if(tclError.Compare(kFalse,PMString(kErrFontNotFound)) == 0) errorCode = 218;
	else if(tclError.Compare(kFalse,PMString(kErrSpreadHierarchy)) == 0) errorCode = 219;
	
	else if(tclError.Compare(kFalse,PMString(kErrCannotWriteFile)) == 0) errorCode = 220;

	else if(tclError.Compare(kFalse,PMString(kErrMinSpaceTooBig)) == 0) errorCode = 221;
	
	else if(tclError.Compare(kFalse,PMString(kErrSpreadHierarchy)) == 0) errorCode = 222;
	else if(tclError.Compare(kFalse,PMString(kErrNoSelection)) == 0) errorCode = 223;
	else if(tclError.Compare(kFalse,PMString(kErrBadPath)) == 0) errorCode = 224;
	
	else if(tclError.Compare(kFalse,PMString(kErrNoTabsDefined)) == 0) errorCode = 225;
	else if(tclError.Compare(kFalse,PMString(kErrNumTab)) == 0) errorCode = 226;

	else if(tclError.Compare(kFalse,PMString(kErrNotAnInline)) == 0) errorCode = 227;
	else if(tclError.Compare(kFalse,PMString(kErrInvalidItem)) == 0) errorCode = 228;  // No items use this TCL reference
	else if(tclError.Compare(kFalse,PMString(kErrInvalidLayer)) == 0) errorCode = 229;
	else if(tclError.Compare(kFalse,PMString(kErrLayerExist)) == 0) errorCode = 230;
	else if(tclError.Compare(kFalse,PMString(kErrParamEH)) == 0) errorCode = 231;
	else if(tclError.Compare(kFalse,PMString(kErrReverseInline)) == 0) errorCode = 232;
	else if(tclError.Compare(kFalse,PMString(kErrUnknownObject)) == 0) errorCode = 233;
	else if(tclError.Compare(kFalse,PMString(kErrPermRefsHR)) == 0) errorCode = 234;
	else if(tclError.Compare(kFalse,PMString(kErrPermRefsHS)) == 0) errorCode = 235;
	else if(tclError.Compare(kFalse,PMString(kErrPermRefsInvalidRef)) == 0) errorCode = 236;
	else if(tclError.Compare(kFalse,PMString(kErrTCLAdornment)) == 0) errorCode = 237;
	else if(tclError.Compare(kFalse,PMString(kErrTCLItemRequired)) == 0) errorCode = 238;
	else if(tclError.Compare(kFalse,PMString(kErrTooManyVariables)) == 0) errorCode = 239;
	else if(tclError.Compare(kFalse,PMString(kErrItemNotOnSameSpread)) == 0) errorCode = 240;

	else if(tclError.Compare(kFalse,PMString(kErrLanguageNotDefined)) == 0) errorCode = 241;
	
	else if(tclError.Compare(kFalse,PMString(kDefaultStyleCmdFailed)) == 0) errorCode = 242;
	else if(tclError.Compare(kFalse,PMString(kErrInvalidPages)) == 0) errorCode = 243;
	else if(tclError.Compare(kFalse,PMString(kErrInvalidBasedOn)) == 0) errorCode = 244;
	else if(tclError.Compare(kFalse,PMString(kErrCannotOpenFile)) == 0) errorCode = 245;
	else if(tclError.Compare(kFalse,PMString(kErrNotImageFile)) == 0) errorCode = 246;
	else if(tclError.Compare(kFalse,PMString(kErrReverseInline)) == 0) errorCode = 247;
	
	else if(tclError.Compare(kFalse,PMString(kErrNotTextFrame)) == 0) errorCode = 248;
	else if(tclError.Compare(kFalse,PMString(kErrNoSuchPage)) == 0) errorCode = 249;
	else if(tclError.Compare(kFalse,PMString(kErrUnableToGroup)) == 0) errorCode = 250;
	else if(tclError.Compare(kFalse,PMString(kErrReverseNotEnoughRef)) == 0) errorCode = 251;

	else if(tclError.Compare(kFalse,PMString(kErrASCannotAdjust)) == 0) errorCode = 260;	
	
	else errorCode = SHOULD_NOT_OCCUR_ERROR;

}