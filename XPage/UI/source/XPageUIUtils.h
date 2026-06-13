
#ifndef __XPageUIUtils_h__
#define __XPageUIUtils_h__

#include "IXPageUtils.h"

class IPanelControlData;

//========================================================================================
// CLASS XPageUtils
//========================================================================================

class XPageUIUtils
{
public:

	/** Display Choose Matching File Dialog and return selected matching file
	*/
	static ErrorCode DisplayChooseMatchingDialog(IDFile& matchingFile);

	/** Display Display New Forme Dialog
	*/
	static void DisplayNewFormeDialog(const PMString& classeurName, bool16 assemblageDialog = kFalse);

	/** Display the New Classeur dialog (prompts for a classeur name and
		creates it in either the Forme or the Assemblage filesystem model).
		On success, the corresponding palette refreshes its classeur dropdown
		and selects the newly-created entry.
		@param classeurAssemblage kFalse for a Formes-palette (XDA) classeur,
			kTrue for an Assemblages-palette classeur. Default kFalse.
	*/
	static void DisplayNewClasseurDialog(bool16 classeurAssemblage = kFalse);

	/** Fill an XPage dropdownlist (kXPGUIDropDownListWidgetBoss) with the elements in parameters
	*/
	static void FillDropdownlist(const IPanelControlData * panelCtrlData, const WidgetID& dropdownlist, IDValueList elements, bool16 notify);

	/** Fill a classic dropdownlist  with the elements in parameters, selecting the first element in the list
	*/
	static void FillDropdownlist(const IPanelControlData * panelCtrlData, const WidgetID& dropdownlist, K2Vector<PMString> elements, bool16 notify);

	/** Fill a classic dropdownlist and manage selection of a particular element
	*/
	static void FillDropdownlistAndSelect(const IPanelControlData * panelCtrlData, const WidgetID& dropdownlist, K2Vector<PMString> elements, const PMString& elementToSelect, bool16 notify);

	static void DisplayLinkArticleDialog(const PMString& articleId, const PMString& articleSnippetFile,
										 const PMString& articleSubject, IDFile& matchingFile, PMPoint currentPoint, 
										 const PMString& idStatus, const PMString& libelleStatus,
										 const PMString& couleurStatus, 
										 UIDRef targetSpread, const int32 typeArt, const PMString& articleXMLFile,
										 const PMString& artRub, const PMString& artSubRub);

	static bool16 IsValidName(const PMString& fileName);

	/** Re-fetch XPage preferences from the server and apply them to
		IXPGPreferences. Called by the panel observers when XRail broadcasts
		kXRLUILoginChangedMsg, so that prefs that defaulted at startup
		(server unreachable) get refreshed once login confirms the server is
		back up. Silent on failure.
	*/
	static void RefreshXPagePrefsFromServer();

	/** Pre-flight validation for "Créer carton". Walks every paragraph of
		the selected text frames and checks three rules :

		  1. No paragraph style has local overrides (the "+" indicator next
		     to the style name in the Paragraph Styles panel).
		  2. No paragraph style is used in two stories that aren't threaded
		     together. Threaded frames share one ITextModel, so the dedup
		     key here is the text model UID.
		  3. No paragraph uses "[No paragraph style]" or "[Basic Paragraph]".

		On failure, errorMessage is filled with a localized human-readable
		alert text listing the offending styles per category — the caller
		just feeds it to CAlert::ErrorAlert().

		@param textFrames [IN] selected text/story refs (typically from
			IXPageSuite::GetSelectedItems()).
		@param errorMessage [OUT] populated when the function returns kFalse.
		@return kTrue if the selection passes all three rules.
	*/
	static bool16 ValidateSelectionForCarton(const UIDList& textFrames, PMString& errorMessage);

	/** Returns kTrue if at least one graphic frame of the front document has
		a placed-image link whose path contains "<imageId>." — i.e. the
		image is already placed somewhere in the layout. Used by the
		Photos liées palette widget mgr to render a green-check status icon
		on placed rows.

		Matches by Gaia image id (e.g. "51226") rather than full path so it's
		robust to HR/BR variants and absolute path differences between
		machines. The id is bracketed with a trailing dot so "51226" doesn't
		match "512260" inside an unrelated filename.
	*/
	static bool16 IsImagePlacedInFrontDoc(const PMString& imageId);

	/** Resolve the RGB (each in [0,1]) colour of an EtatImage by its id: looks
		the id up in IXPGPreferences::GetEtatsImages() and parses its Couleur
		string (hex "#RRGGBB" or a French colour name). Returns kFalse when the
		id is 0/unknown or the colour can't be parsed — callers then paint a
		neutral swatch. Shared by the Photos liées widget mgr (initial paint)
		and the swatch event handler (repaint after a state change).
	*/
	static bool16 GetEtatImageColor(int32 etatId, PMReal& r, PMReal& g, PMReal& b);

	/** Import an image from `imageFile` into the graphic frame `photoFrameRef`,
		then run the same article-graft + credit/legend import the Photos liées
		drop target performs when an image is dropped on an existing frame.

		Extracted from XPGUIXRailImageDropTarget::ProcessDragDropCommand so the
		Photos-liées-to-empty-space flow (which lands on a freshly placed
		photo carton) can run the exact same logic per photo frame, rather
		than duplicating it. Behaviour is identical to dropping the image
		directly on `photoFrameRef`.

		@param photoFrameRef the destination graphic frame.
		@param imageFile     filesystem path of the image to import.
		@param creditText    credit text to flow into kCredit siblings of
		                     the photo frame on the same spread (may be empty).
		@param legendText    legend text to flow into kLegend siblings (may
		                     be empty).
		@param draggedArticleId Gaia article id carried by the drag — used to
		                     graft article tags onto the target frame and its
		                     siblings when the target doesn't already belong
		                     to the same article. Empty disables grafting.
		@return kSuccess on a complete import (image + place + credit/legend).
	*/
	static ErrorCode ImportPhotoIntoFrame(const UIDRef& photoFrameRef,
	                                       const IDFile& imageFile,
	                                       const PMString& creditText,
	                                       const PMString& legendText,
	                                       const PMString& draggedArticleId,
	                                       const PMString& cropData = kNullString);

	/** Encode/decode the crop payload carried by the image drag (flavor
		IID_IPHOTOCROPDATA). Packs the crop rectangle (pixels in the BR
		thumbnail's space) and the BR thumbnail path as
		"cropX|cropY|cropW|cropH|brPath" — the four crop values are integers
		scaled x1000 (locale-independent, 0.001px precision), and '|' is a
		safe separator since it is illegal in Windows filenames. An empty or
		malformed string decodes to all-zero crop (= no crop) and empty brPath.
	*/
	static PMString PackCropPayload(const PMReal& cropX, const PMReal& cropY,
	                                const PMReal& cropW, const PMReal& cropH,
	                                const PMString& brPath);
	static void UnpackCropPayload(const PMString& packed, PMReal& cropX, PMReal& cropY,
	                              PMReal& cropW, PMReal& cropH, PMString& brPath);

	/** Open the modal "Choose photo carton" dialog with the dragged image
		payload pre-loaded onto the dialog boss. Triggered by the layout-level
		image drop target (XPGUIImageInLayoutDropTarget) when an image is
		dropped on empty pasteboard. The dialog's OK action places the
		selected photo carton at `dropPoint` on `targetSpread` and runs
		ImportPhotoIntoFrame for each kPhoto frame of the placed carton.

		If no photo carton is registered anywhere, the dialog is opened
		with empty dropdowns AND an information alert is surfaced first
		(per spec — option B: "open dialog with error message").
	*/
	static void DisplayChoosePhotoCartonDialog(const IDFile& imageFile,
	                                            const PMString& creditText,
	                                            const PMString& legendText,
	                                            const PMString& draggedArticleId,
	                                            const PMPoint& dropPoint,
	                                            const UIDRef& targetSpread,
	                                            const PMString& cropData = kNullString);

private:
	XPageUIUtils(); // this is an util class, only to be used with static methods calls
};

#endif // __XPageUIUtils_h__
