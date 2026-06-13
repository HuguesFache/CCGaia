/*
//	File:	PrsID.h
//
//	Author:	Trias
//
//	Date:	11-Jun-2003
//
//	Defines IDs used by the Prs plug-in.
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//	
//	NOTICE: Adobe permits you to use, modify, and distribute this file in
//	accordance with the terms of the Adobe license agreement accompanying it.
//	If you have received this file from a source other than Adobe, then your
//	use, modification, or distribution of it requires the prior written
//	permission of Adobe.
//
*/

#ifndef __PrsID_h__
#define __PrsID_h__

#include "SDKDef.h"
#include "GlobalDefs.h"

// Product using this plugin
#if COMPOSER
	#define kProductName "Composer"
#else
	#define kProductName "PageMakeUp"
#endif

// Plug-in:
#define kPrsPluginName		"Parser"		// Name of this plug-in.
#define kPrsPrefixNumber	0xd0442			// Unique prefix number for this plug-in(*Must* be obtained from Adobe Developer Support).
// Plug-in version string. Pulled from the shared TriasVersion.h header so
// every Trias plug-in bumps in lock-step. Edit Gaia\TriasVersion.h to
// change the family-wide version.
#include "../../../Gaia/TriasVersion.h"
#define kPrsVersion			kTriasVersionString			// Version of this plug-in (for the About Box).
#define kPrsAuthor			"Trias"			// Author of this plug-in (for the About Box).

// Plug-in Prefix: (please change kPrsPrefixNumber above to modify the prefix.)
#define kPrsPrefix			RezLong(kPrsPrefixNumber)				// The unique numeric prefix for all object model IDs for this plug-in.
#define kPrsStringPrefix	SDK_DEF_STRINGIZE(kPrsPrefixNumber)	// The string equivalent of the unique prefix number for  this plug-in.

// PluginID:
DECLARE_PMID(kPlugInIDSpace, kPrsPluginID, kPrsPrefix + 0)

// ClassIDs:
DECLARE_PMID(kClassIDSpace, kTCLReadWriteBoss,	kPrsPrefix + 0)
DECLARE_PMID(kClassIDSpace, kTCLParserBoss,	kPrsPrefix + 1)
DECLARE_PMID(kClassIDSpace, kPrsSetDocTCLReferencesListCmdBoss, kPrsPrefix + 2)
DECLARE_PMID(kClassIDSpace, kPrsItemDelRespBoss, kPrsPrefix + 3)
DECLARE_PMID(kClassIDSpace, kPermRefsAttrBoss, kPrsPrefix + 7)
DECLARE_PMID(kClassIDSpace, kReverseTCLBoss, kPrsPrefix + 8)
DECLARE_PMID(kClassIDSpace, kPermRefsTagTextAdornmentBoss, kPrsPrefix + 9)
DECLARE_PMID(kClassIDSpace, kPrsTCLAdornmentBoss, kPrsPrefix + 10)
DECLARE_PMID(kClassIDSpace, kPrsCustomServiceProviderBoss, kPrsPrefix + 11)
DECLARE_PMID(kClassIDSpace, kPrsSetPreferencesCmdBoss,	kPrsPrefix + 12)
DECLARE_PMID(kClassIDSpace, kPrsErrorStringServiceBoss,	kPrsPrefix + 13)
DECLARE_PMID(kClassIDSpace, kPrsScriptProviderBoss,	kPrsPrefix + 14)
DECLARE_PMID(kClassIDSpace, kPrsSetRegistrationInfoCmdBoss,	kPrsPrefix + 15)
DECLARE_PMID(kClassIDSpace, kPrsSetTCLRefDataCmdBoss,	kPrsPrefix + 16)
DECLARE_PMID(kClassIDSpace, kPrsSerialNumberDialogBoss,		kPrsPrefix + 17)
DECLARE_PMID(kClassIDSpace, kPrsStartupShutdownBoss,	kPrsPrefix + 18)
DECLARE_PMID(kClassIDSpace, kPrsEnterSerialNumberCmdBoss,	kPrsPrefix + 19)
DECLARE_PMID(kClassIDSpace, kPrsSetDocEpsExportPrefsCmdBoss,	kPrsPrefix + 20)
DECLARE_PMID(kClassIDSpace, kPrsConversionProviderBoss,	kPrsPrefix + 21)
DECLARE_PMID(kClassIDSpace, kPrsSetDocPdfExportPrefsCmdBoss,	kPrsPrefix + 22)
DECLARE_PMID(kClassIDSpace, kPrsSetImageInfosCmdBoss,	kPrsPrefix + 23)
DECLARE_PMID(kClassIDSpace, kPermRefsPreProcessServiceBoss,	kPrsPrefix + 24)
DECLARE_PMID(kClassIDSpace, kPermRefsPostProcessServiceBoss,	kPrsPrefix + 25)
DECLARE_PMID(kClassIDSpace, kPrsInsertTextCmdBoss,	kPrsPrefix + 26)
DECLARE_PMID(kClassIDSpace, kPrsTCLRefScriptProviderBoss,	kPrsPrefix + 27)
DECLARE_PMID(kClassIDSpace, kPrsSetTOPPositionCmdBoss,	kPrsPrefix + 28)

//========================================================================================
// <Message ID>
// ----- Message IDs -----
// Messages sent by various Data Objects
// -- these are ClassIDs but have no classes!
// -- they are only used for sending messages
//========================================================================================

DECLARE_PMID(kMessageIDSpace, kNewStreamMessage, kPrsPrefix + 5)
DECLARE_PMID(kMessageIDSpace, kBytesReadMessage, kPrsPrefix + 6)
DECLARE_PMID(kMessageIDSpace, kParserThreadFinishMessage, kPrsPrefix + 7)

// InterfaceIDs:
DECLARE_PMID(kInterfaceIDSpace, IID_ITCLREADER, kPrsPrefix + 0)
DECLARE_PMID(kInterfaceIDSpace, IID_ITCLPARSER, kPrsPrefix + 1)
DECLARE_PMID(kInterfaceIDSpace, IID_TCLREFERENCESLIST, kPrsPrefix + 2)
DECLARE_PMID(kInterfaceIDSpace, IID_IPARSERSUITE, kPrsPrefix + 3)
DECLARE_PMID(kInterfaceIDSpace, IID_IEPSDOCUMENTNAME, kPrsPrefix + 4)
DECLARE_PMID(kInterfaceIDSpace, IID_IPERMREFSTAG, kPrsPrefix + 5)
DECLARE_PMID(kInterfaceIDSpace, IID_IREVERSETCL, kPrsPrefix + 6)
DECLARE_PMID(kInterfaceIDSpace, IID_ITCLWRITER, kPrsPrefix + 7)
DECLARE_PMID(kInterfaceIDSpace, IID_IPERMREFSSUITE, kPrsPrefix + 8)
DECLARE_PMID(kInterfaceIDSpace, IID_IPAGEMAKEUPPREFS, kPrsPrefix + 9)
DECLARE_PMID(kInterfaceIDSpace, IID_IREGISTRATION, kPrsPrefix + 10)
DECLARE_PMID(kInterfaceIDSpace, IID_IIMAGESCALE, kPrsPrefix + 11)
DECLARE_PMID(kInterfaceIDSpace, IID_IIMAGESHIFT, kPrsPrefix + 12)
DECLARE_PMID(kInterfaceIDSpace, IID_IIMAGEPATH, kPrsPrefix + 13)
DECLARE_PMID(kInterfaceIDSpace, IID_IWIDESTRINGDATA, kPrsPrefix + 14)
DECLARE_PMID(kInterfaceIDSpace, IID_PASTEIMPORTITEMOBSERVER, kPrsPrefix + 15)
DECLARE_PMID(kInterfaceIDSpace, IID_IPRSLASTIMPORTEDSNIPPET, kPrsPrefix + 16)
DECLARE_PMID(kInterfaceIDSpace, IID_ITCLREFDATA, kPrsPrefix + 17)
DECLARE_PMID(kInterfaceIDSpace, IID_ISTARTPOSDATA, kPrsPrefix + 18)
DECLARE_PMID(kInterfaceIDSpace, IID_IENDPOSDATA, kPrsPrefix + 19)

// ImplementationIDs:
DECLARE_PMID(kImplementationIDSpace, kPrsTCLReaderImpl,	kPrsPrefix + 0)
DECLARE_PMID(kImplementationIDSpace, kPrsTCLParserImpl,	kPrsPrefix + 1)
DECLARE_PMID(kImplementationIDSpace, kPrsTCLReferencesListPersistImpl, kPrsPrefix + 2)
DECLARE_PMID(kImplementationIDSpace, kPrsTCLReferencesListImpl, kPrsPrefix + 3)
DECLARE_PMID(kImplementationIDSpace, kPrsSetDocTCLReferencesListImpl, kPrsPrefix + 4)
DECLARE_PMID(kImplementationIDSpace, kPrsDeleteCleanupImpl, kPrsPrefix + 5)
DECLARE_PMID(kImplementationIDSpace, kPrsSuiteASBImpl, kPrsPrefix + 7)
DECLARE_PMID(kImplementationIDSpace, kPrsSuiteLayoutCSBImpl, kPrsPrefix + 8)
DECLARE_PMID(kImplementationIDSpace, kPrsEpsDocumentNamePersistImpl, kPrsPrefix + 12)
DECLARE_PMID(kImplementationIDSpace, kPermRefsTagImpl, kPrsPrefix + 13)
DECLARE_PMID(kImplementationIDSpace, kPrsReverseTCLImpl, kPrsPrefix + 14)
DECLARE_PMID(kImplementationIDSpace, kPrsTCLWriterImpl, kPrsPrefix + 15)
DECLARE_PMID(kImplementationIDSpace, kPermRefsTagReportImpl, kPrsPrefix + 16)
DECLARE_PMID(kImplementationIDSpace, kPermRefsTagTextAdornmentImpl, kPrsPrefix + 17)
DECLARE_PMID(kImplementationIDSpace, kPrsTCLAdornmentShapeImpl, kPrsPrefix + 18)
DECLARE_PMID(kImplementationIDSpace, kPermRefsSuiteASBImpl, kPrsPrefix + 19)
DECLARE_PMID(kImplementationIDSpace, kPermRefsSuiteTextCSBImpl, kPrsPrefix + 20)
DECLARE_PMID(kImplementationIDSpace, kPrsSetPreferencesCmdImpl,   kPrsPrefix + 21)
DECLARE_PMID(kImplementationIDSpace, kPrsPreferencesImpl,   kPrsPrefix + 22)
DECLARE_PMID(kImplementationIDSpace, kPrsPreferencesPersistImpl,   kPrsPrefix + 23)
DECLARE_PMID(kImplementationIDSpace, kPrsCustomServiceProviderImpl, kPrsPrefix + 24)
DECLARE_PMID(kImplementationIDSpace, kPrsRespServiceImpl, kPrsPrefix + 25)
DECLARE_PMID(kImplementationIDSpace, kPrsErrorStringServiceImpl, kPrsPrefix + 26)
DECLARE_PMID(kImplementationIDSpace, kPrsScriptProviderImpl, kPrsPrefix + 27)
DECLARE_PMID(kImplementationIDSpace, kPrsRegistrationImpl, kPrsPrefix + 28)
DECLARE_PMID(kImplementationIDSpace, kPrsRegistrationPersistImpl, kPrsPrefix + 29)
DECLARE_PMID(kImplementationIDSpace, kPrsSetRegistrationInfoCmdImpl, kPrsPrefix + 30)
DECLARE_PMID(kImplementationIDSpace, kPrsTCLRefDataImpl, kPrsPrefix + 31)
DECLARE_PMID(kImplementationIDSpace, kPrsInsertTextCmdImpl, kPrsPrefix + 32)
DECLARE_PMID(kImplementationIDSpace, kPrsStartupShutdownImpl, kPrsPrefix + 33)
DECLARE_PMID(kImplementationIDSpace, kPrsSerialNumberDialogControllerImpl, kPrsPrefix + 34)
DECLARE_PMID(kImplementationIDSpace, kPrsEnterSerialNumberCmdImpl, kPrsPrefix + 35)
DECLARE_PMID(kImplementationIDSpace, kPrsEpsExportPrefsPersistImpl, kPrsPrefix + 36)
DECLARE_PMID(kImplementationIDSpace, kPrsEpsExportPrefsImpl, kPrsPrefix + 37)
DECLARE_PMID(kImplementationIDSpace, kPrsEpsDocumentNameImpl, kPrsPrefix + 38)
DECLARE_PMID(kImplementationIDSpace, kPrsSetDocEpsExportPrefsCmdImpl, kPrsPrefix + 39)
DECLARE_PMID(kImplementationIDSpace, kPrsSetDocPdfExportPrefsCmdImpl, kPrsPrefix + 40)
DECLARE_PMID(kImplementationIDSpace, kPrsPersistStringDataImpl, kPrsPrefix + 41)
DECLARE_PMID(kImplementationIDSpace, kPrsPersistImageScaleImpl, kPrsPrefix + 42)
DECLARE_PMID(kImplementationIDSpace, kPrsPersistImageShiftImpl, kPrsPrefix + 43)
DECLARE_PMID(kImplementationIDSpace, kPrsSetImageInfosCmdImpl, kPrsPrefix + 44)
DECLARE_PMID(kImplementationIDSpace, kPermRefsTextEditPreProcessImpl, kPrsPrefix + 45)
DECLARE_PMID(kImplementationIDSpace, kPermRefsTextEditPostProcessImpl, kPrsPrefix + 46)
DECLARE_PMID(kImplementationIDSpace, kPrsWideStringDataImpl, kPrsPrefix + 47)
DECLARE_PMID(kImplementationIDSpace, kPrsPasteImportItemObserverImpl, kPrsPrefix + 48)
DECLARE_PMID(kImplementationIDSpace, kPrsTCLRefScriptProviderImpl, kPrsPrefix + 49)
DECLARE_PMID(kImplementationIDSpace, kPrsSetTCLRefDataCmdImpl, kPrsPrefix + 50)
DECLARE_PMID(kImplementationIDSpace, kPrsParserThreadImpl, kPrsPrefix + 51)
DECLARE_PMID(kImplementationIDSpace, kPrsSetTOPPositionCmdImpl, kPrsPrefix + 52)

// Widgets
DECLARE_PMID(kWidgetIDSpace, kPrsSerialNumberDialogWidgetID,		kPrsPrefix + 0) 
 
DECLARE_PMID(kWidgetIDSpace, kNumberPart1WidgetID,		kPrsPrefix + 2)
DECLARE_PMID(kWidgetIDSpace, kNumberPart2WidgetID,		kPrsPrefix + 3)
DECLARE_PMID(kWidgetIDSpace, kNumberPart3WidgetID,		kPrsPrefix + 4)
DECLARE_PMID(kWidgetIDSpace, kNumberPart4WidgetID,		kPrsPrefix + 5)
DECLARE_PMID(kWidgetIDSpace, kNumberPart5WidgetID,		kPrsPrefix + 6)

//Events
DECLARE_PMID(kScriptInfoIDSpace, kPrsExecTCLEventScriptElement, kPrsPrefix + 0)
DECLARE_PMID(kScriptInfoIDSpace, kPrsGetPermRefsTagEventScriptElement, kPrsPrefix + 1)
DECLARE_PMID(kScriptInfoIDSpace, kPrsTCLRelPropScriptElement, kPrsPrefix + 2)
DECLARE_PMID(kScriptInfoIDSpace, kPrsPermRefsTagPropScriptElement, kPrsPrefix + 3)

// ErrorCodes
DECLARE_PMID(kErrorIDSpace, kPrsExecTCLFailedErrorCode, kPrsPrefix + 0)

// Error strings keys
#define kErrorLineKey		kPrsStringPrefix "kErrorLineKey"
#define kCommandKey			kPrsStringPrefix "kCommandKey"
#define kParameterKey		kPrsStringPrefix "kParameterKey"

#define kErrEOF				kPrsStringPrefix "kErrEOF"
#define kErrNoString		kPrsStringPrefix "kErrNoString"
#define kErrNoStringHeader	kPrsStringPrefix "kErrStringHeader"
#define kErrNoEOString		kPrsStringPrefix "kErrNoEOString"
#define kErrEOVariable		kPrsStringPrefix "kErrEOVariable"
#define kErrLength			kPrsStringPrefix "kErrLength"
#define kErrNoChar			kPrsStringPrefix "kErrNoChar"
#define kErrCharHeader		kPrsStringPrefix "kErrCharHeader"
#define kErrNoEOChar		kPrsStringPrefix "kErrNoEOChar"
#define kErrEgal			kPrsStringPrefix "kErrEgal"
#define kErrNoEgal			kPrsStringPrefix "kErrNoEgal"
#define kErrIntTooBig		kPrsStringPrefix "kErrIntTooBig"
#define kErrSupMax			kPrsStringPrefix "kErrSupMax"
#define kErrInfMin			kPrsStringPrefix "kErrInfMin"
#define kErrBadInt			kPrsStringPrefix "kErrBadInt"
#define kErrLongIntTooBig	kPrsStringPrefix "kErrLongIntTooBig"
#define kErrNoUnite			kPrsStringPrefix "kErrNoUnite"
#define kErrBadUnite		kPrsStringPrefix "kErrBadUnite"
#define kErrBadEndUnite		kPrsStringPrefix "kErrBadEndUnite"
#define kErrBadBool			kPrsStringPrefix "kErrBadBool"
#define kErrBadEndBool		kPrsStringPrefix "kErrBadEndBool"
#define kErrConv			kPrsStringPrefix "kErrConv"
#define kErrBadReal			kPrsStringPrefix "kErrBadReal"
#define kErrRealTooBig		kPrsStringPrefix "kErrRealTooBig"
#define kErrRealTooSmall	kPrsStringPrefix "kErrRealTooSmall"
#define kErrBadPourcent		kPrsStringPrefix "kErrBadPourcent"
#define kErrPEntTooBig		kPrsStringPrefix "kErrPEntTooBig"
#define kErrNilInterface	kPrsStringPrefix "kErrNilInterface"
#define kErrTooManySavedItems	kPrsStringPrefix "kErrTooManySavedItems"
#define kErrUnknownCommand		kPrsStringPrefix "kErrUnknownCommand"
#define kErrCmdFailed			kPrsStringPrefix "kErrCmdFailed"
#define kErrInsertTxt			kPrsStringPrefix "kErrInsertTxt"
#define kTooManySelectedItems	kPrsStringPrefix "kTooManySelectedItems"
#define kErrRefInvalid			kPrsStringPrefix "kErrRefInvalid"
#define kErrNoEndOfCommand		kPrsStringPrefix "kErrNoEndOfCommand"
#define kErrNotEnoughArgument	kPrsStringPrefix "kErrNotEnoughArgument"
#define kErrNeedDoc				kPrsStringPrefix "kErrNeedDoc"
#define kErrNotEnoughItem		kPrsStringPrefix "kErrNotEnoughItem"
#define kErrTextBoxRequired		kPrsStringPrefix "kErrTextBoxRequired"
#define kErrImageBoxRequired	kPrsStringPrefix "kErrImageBoxRequired"
#define kErrBoxRequired			kPrsStringPrefix "kErrBoxRequired"
#define kErrItemOutOfSpread		kPrsStringPrefix "kErrItemOutOfSpread"
#define kErrNoSuchColor			kPrsStringPrefix "kErrNoSuchColor"
#define kErrNotRightArgumentNumber kPrsStringPrefix "kErrNotRightArgumentNumber"
#define kAlreadyDefiningStyle	kPrsStringPrefix "kAlreadyDefiningStyle"
#define kErrStyleNotFound		kPrsStringPrefix "kErrStyleNotFound"
#define kErrNotDefining			kPrsStringPrefix "kErrNotDefining"
#define kErrNotAllowedDuringDefine kPrsStringPrefix "kErrNotAllowedDuringDefine"
#define kErrColorNotFound		kPrsStringPrefix "kErrColorNotFound"
#define kErrFontNotFound		kPrsStringPrefix "kErrFontNotFound"
#define kErrSpreadHierarchy		kPrsStringPrefix "kErrSpreadHierarchy"
#define kErrNumPageTooBig		kPrsStringPrefix "kNumPageTooBig"
#define kErrNumPageTooSmall		kPrsStringPrefix "kNumPageTooSmall"
#define kErrImpossibleToDelete	kPrsStringPrefix "kErrImpossibleToDelete"
#define kErrUserBreak			kPrsStringPrefix "kErrUserBreak"
#define kErrMinSpaceTooBig		kPrsStringPrefix "kErrMinSpaceTooBig"
#define kErrCJNotFound			kPrsStringPrefix "kErrCJNotFound"
#define kErrNoSelection					kPrsStringPrefix "kErrNoSelection"
#define kErrBadPath						kPrsStringPrefix "kErrBadPath"
#define kErrNoTabsDefined				kPrsStringPrefix "kErrNoTabsDefined"
#define kErrNumTab						kPrsStringPrefix "kErrNumTab"
#define kErrOpenDoc						kPrsStringPrefix "kErrOpenDoc"
#define kErrNotAnInline					kPrsStringPrefix "kErrNotAnInline"
#define kErrInvalidItem					kPrsStringPrefix "kErrInvalidItem"
#define kErrInvalidLayer				kPrsStringPrefix "kErrInvalidLayer"
#define kErrLayerExist					kPrsStringPrefix "kErrLayerExist"
#define kErrParamEH						kPrsStringPrefix "kErrParamEH"
#define kErrReverseInline				kPrsStringPrefix "kErrReverseInline"
#define kErrUnknownObject				kPrsStringPrefix "kErrUnknownObject"
#define kPermRefsTagNameAbbr			kPrsStringPrefix "kPermRefsTagNameAbbr"
#define kErrPermRefsHR					kPrsStringPrefix "kErrPermRefsHR"
#define kErrPermRefsHS					kPrsStringPrefix "kErrPermRefsHS"
#define kErrPermRefsInvalidRef			kPrsStringPrefix "kErrPermRefsInvalidRef"
#define kErrTCLAdornment				kPrsStringPrefix "kErrTCLAdornment"
#define kErrTCLItemRequired				kPrsStringPrefix "kErrTCLItemRequired"
#define kErrTooManyVariables			kPrsStringPrefix "kErrTooManyVariables"
#define kErrItemNotOnSameSpread			kPrsStringPrefix "kErrItemNotOnSameSpread"
#define kErrExecTCLFailedStringkey		kPrsStringPrefix "kErrExecTCLFailedStringkey"
#define kErrCannotWriteFile				kPrsStringPrefix "kErrCannotWriteFile"
#define kErrCannotReadFile				kPrsStringPrefix "kErrCannotReadFile"
#define kErrReverseNotEnoughRef			kPrsStringPrefix "kErrReverseNotEnoughRef"
#define kErrInvalidGroupArgument		kPrsStringPrefix "kErrInvalidGroupArgument"
#define kEndExecuteTCL					kPrsStringPrefix "kEndExecuteTCL"
#define kEndReverseTCL					kPrsStringPrefix "kEndReverseTCL"
#define kErrPDFStyleNotFound			kPrsStringPrefix "kErrPDFStyleNotFound"
#define kErrTableInsertionNotPossible	kPrsStringPrefix "kErrTableInsertionNotPossible"
#define kErrNoCurrentTable				kPrsStringPrefix "kErrNoCurrentTable"
#define kErrNoCurrentCells				kPrsStringPrefix "kErrNoCurrentCells"
#define kErrInvalidMasterTxtFrame		kPrsStringPrefix "kErrInvalidMasterTxtFrame"
#define kErrNotAMasterItem				kPrsStringPrefix "kErrNotAMasterItem"
#define kErrInvalidCellSelection		kPrsStringPrefix "kErrInvalidCellSelection"
#define kErrFileNotExist				kPrsStringPrefix "kErrFileNotExist"
#define kErrInvalidIndex				kPrsStringPrefix "kErrInvalidIndex"
#define kErrNoCurrentBook				kPrsStringPrefix "kErrNoCurrentBook"
#define kErrSnippetImportFailed			kPrsStringPrefix "kErrSnippetImportFailed"
#define kErrItemNotOverriden			kPrsStringPrefix "kErrItemNotOverriden"
#define kErrAlreadyUsed					kPrsStringPrefix "kErrAlreadyUsed"
#define kErrTiffImageRequired			kPrsStringPrefix "kErrTiffImageRequired"
#define kErrSnapshotCreationFailed		kPrsStringPrefix "kErrSnapshotCreationFailed"
#define kErrCrossRefPluginMissing		kPrsStringPrefix "kErrCrossRefPluginMissing"
#define kErrBkNameAlreadyExists			kPrsStringPrefix "kErrBkNameAlreadyExists"
#define kErrBkParentNotExists			kPrsStringPrefix "kErrBkParentNotExists"
#define kErrHLDestNameAlreadyExists		kPrsStringPrefix "kErrHLDestNameAlreadyExists"
#define kErrHyperlinkNameEmpty			kPrsStringPrefix "kErrHyperlinkNameEmpty"
#define kErrHyperlinkDestNotExist		kPrsStringPrefix "kErrHyperlinkDestNotExist"
#define kErrInvalidHLCall				kPrsStringPrefix "kErrInvalidHLCall"
#define kErrInvalidFolder				kPrsStringPrefix "kErrInvalidFolder"
#define kErrInvalidHyperlinkIndex		kPrsStringPrefix "kErrInvalidHyperlinkIndex"
#define kErrInvalidParameter			kPrsStringPrefix "kErrInvalidParameter"
#define kErrInvalidRange				kPrsStringPrefix "kErrInvalidRange"
#define kErrUnknownType					kPrsStringPrefix "kErrUnknownType"
#define kErrUnknownChar					kPrsStringPrefix "kErrUnknownChar"
#define kErrUnknownTextPosition			kPrsStringPrefix "kErrUnknownTextPosition"
#define kErrUnknownCapMode				kPrsStringPrefix "kErrUnknownCapMode"
#define kErrUnknownColor				kPrsStringPrefix "kErrUnknownColor"
#define kErrUnknownLanguage				kPrsStringPrefix "kErrUnknownLanguage"
#define kErrInvalidColorType			kPrsStringPrefix "kErrInvalidColorType"
#define kErrGroupOfGroup				kPrsStringPrefix "kErrGroupOfGroup"
#define kErrInconsistentItem			kPrsStringPrefix "kErrInconsistentItem"
#define kErrTOPFound					kPrsStringPrefix "kErrTOPFound"
#define kErrLinkWithNoDest				kPrsStringPrefix "kErrLinkWithNoDest"
#define kErrClippingPath				kPrsStringPrefix "kErrClippingPath"
#define kErrInconsistentLink			kPrsStringPrefix "kErrInconsistentLink"
#define kErrCrossRefMarkerNotExist		kPrsStringPrefix "kErrCrossRefMarkerNotExist"
#define kErrCrossRefMarkerNotExistInDoc kPrsStringPrefix "kErrCrossRefMarkerNotExistInDoc"
#define kErrCrossRefMarkerAlreadyExist  kPrsStringPrefix "kErrCrossRefMarkerAlreadyExist"
#define kErrItemOutOfColumns			kPrsStringPrefix "kErrItemOutOfColumns"
#define kErrOversetFrame				kPrsStringPrefix "kErrOversetFrame"
#define kErrTooManyLevels				kPrsStringPrefix "kErrTooManyLevels"
#define kErrFailedOpenTextEdit			kPrsStringPrefix "kErrFailedOpenTextEdit"
#define kErrEpsMissingLink				kPrsStringPrefix "kErrEpsMissingLink"
#define kErrASCannotAdjust				kPrsStringPrefix "kErrASCannotAdjust"
#define kErrTrafficPluginMissing		kPrsStringPrefix "kErrTrafficPluginMissing"
#define kErrNullLegaleHeight			kPrsStringPrefix "kErrNullLegaleHeight"
#define kErrInvalidLegaleRef			kPrsStringPrefix "kErrInvalidLegaleRef"
#define kErrTOPRequired					kPrsStringPrefix "kErrTOPRequired"
#define kErrInvalidResa					kPrsStringPrefix "kErrInvalidResa"
#define kErrInvalidFormeID				kPrsStringPrefix "kErrInvalidFormeID"

#define kErrInvalidIDSerial				kPrsStringPrefix "kErrInvalidIDSerial"
#define kErrCorruptedLicense			kPrsStringPrefix "kErrCorruptedLicense"
#define kErrTooManyCopies				kPrsStringPrefix "kErrTooManyCopies"
#define kPrsEnterSerialKey				kPrsStringPrefix "kPrsEnterSerialKey"
#define kPrsSerialNumberDialogTitleKey	kPrsStringPrefix "kPrsSerialNumberDialogTitleKey"
#define kPrsDemoExpired					kPrsStringPrefix "kPrsDemoExpired"
#define kPrsDemoRemainingDays			kPrsStringPrefix "kPrsDemoRemainingDays"
#define kPrsMissingLinkMsg				kPrsStringPrefix "kPrsMissingLinkMsg"
#define kPrsAMCommandFailed				kPrsStringPrefix "kPrsAMCommandFailed"

#define kPrsThreadKey					kPrsStringPrefix "kPrsThreadKey"

// Initial data format version numbers
#define kPrsFirstMajorFormatNumber  kSDKDef_30_PersistMajorVersionNumber
#define kPrsFirstMinorFormatNumber  kSDKDef_30_PersistMinorVersionNumber

// Data format version numbers for the PluginVersion resource 
#define kPrsSecondMinorFormatNumber kPrsFirstMinorFormatNumber + 1
#define kPrsThirdMinorFormatNumber	kPrsFirstMinorFormatNumber + 2
#define kPrsFourthMinorFormatNumber kPrsFirstMinorFormatNumber + 3

// Plugin version numbers
#define kPrsCurrentMajorVersionNumber kSDKDefPlugInMajorVersionNumber // most recent major format change
#define kPrsCurrentMinorVersionNumber kSDKDefPlugInMinorVersionNumber + 3 // most recent minor format change

#define kPrsSerialNumberDialogResourceID kSDKDefPanelResourceID

#endif // __PrsID_h__
//  Generated by Dolly build 17: template "IfPanelMenu".
// End, PrsID.h.



