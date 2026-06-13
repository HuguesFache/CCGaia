
#ifndef __DocUtils_h__
#define __DocUtils_h__

#include "RsrcSpec.h"
#include "IDialog.h"

		int32 GetPageID(UIDRef docUIDRef, int32 index, PMString &baseName);
		void SetPageID(UIDRef docUIDRef, int32 indexofpage, int32 id, PMString baseName);
			 
		/** Sauvegarde d'un document*/
		ErrorCode DoSaveDoc(UIDRef documentUIDRef, IDFile sysFile);
		ErrorCode SaveAsDoc(UIDRef doc, PMString absoluteName);
		
		/** Generation de la vignette*/
		ErrorCode DoMakePreview(const UIDRef& documentUIDRef, IDFile& jpgFile, int32 page, int32 scale, int32 resolution);
ErrorCode DoMakeIDML(const UIDRef& documentUIDRef, IDFile& idmlFile);
	
		//Lire et ecrire des preferences
void SetPrefs(PMString urlXR, PMString lastUser, bool16 modeRobot, K2Vector<PMString> listeBases, K2Vector<PMString> IPBases);
void GetPrefs(PMString& urlXR, PMString& lastUser, bool16& modeRobot, K2Vector<PMString>& listeBases, K2Vector<PMString>& IPBases);

// "Se souvenir de moi" credentials (login dialog). These live alongside the
// other XRail prefs but are accessed directly on the workspace so they are
// not affected by the main SetPrefs command.
void GetRememberedCredentials(bool16& rememberMe, PMString& password);
void SetRememberedCredentials(bool16 rememberMe, const PMString& password);
		
PMString GetGoodUrlXR(PMString baseName);
		
		IDialog * CreateNewDialog(PluginID pluginID, RsrcID dialogID, IDialog::DialogType dialogType = IDialog::kMovableModal);
		
		//recuperer nombre de pages du document
		int32 GetNbPages(const UIDRef& documentUIDRef);
		
		// creer une reserve de pub
		ErrorCode CreerResaPub(UIDRef parent, const PMString& resaID, const PMString& resaAnnonceur, const PMPoint& leftTop, const PMReal& width, const PMReal& height, UID& resaUID, bool16 VerrouPos);

		// Convert an existing text or graphic frame to resa pub
		ErrorCode ConvertToResaPub(UIDRef resaFrame, const PMString& resaID, const PMString& resaAnnonceur);

		// If the pub file was already imported, check whether its link is out of date before importing.
		// respectTemplateFitting=kTrue keeps the FrameFittingOption set on
		// the target frame untouched (used when the frame came from an IDMS
		// template, which already carries the desired fitting). Default
		// kFalse runs the legacy GetPubProp()-driven fit (kFitContent /
		// kFitFrame) so existing callers keep their behaviour.
		void ImportPubFile(UIDRef resaPub, const IDFile& pubFile, PMString pubFileName,
			bool16 respectTemplateFitting = kFalse);

		// Verrouillage de position du bloc pub
		void LockPageItemCmd(UIDRef itemToSet, const bool8 toLock, const bool8 allowUndo);

		// Import an IDMS template (single top-level frame expected) into the
		// target spread layer, then move + resize it to the requested
		// pasteboard rectangle. Returns kSuccess and fills outFrameUID with
		// the imported frame's UID. Used by ImportPublicites for the
		// configurable ad-block templates (paths come from IXRailPrefsData).
		// Fails (returns kFailure, outFrameUID = kInvalidUID) if the file
		// can't be opened, the import fails, or the template doesn't yield
		// exactly one top-level frame — caller is expected to fall back to
		// the legacy CreerResaPub / ImportPubFile path on failure.
		ErrorCode ImportAdTemplateBlock(UIDRef ownerSpreadLayer, const IDFile& templateFile,
			const PMPoint& leftTop, const PMReal& width, const PMReal& height,
			UID& outFrameUID);

		// In-place find/replace inside the story of a text frame. Walks the
		// story once per (key, value) pair. Used to substitute placeholders
		// like {ORDRE_ID} / {ORDRE_ANNONCEUR} in the ad template's text
		// after import. No-op (kSuccess) if the frame has no story or no
		// placeholder appears.
		ErrorCode ReplaceTextInFrame(UIDRef frameRef,
			const K2Vector<PMString>& keys, const K2Vector<PMString>& values);

		// Set the section folio (start page number) on the page at index
		// `indexofpage`. If a section already starts on that page, its start
		// number is updated; otherwise a new section is created starting on
		// that page. Page numbering style stays Arabic, prefix/marker stay
		// empty, autoNumber + includeSectionPrefix stay false.
		void SetPageFolio(UIDRef docUIDRef, int32 indexofpage, int32 folio);

		// Apply a Pagemakeup TCL snippet (e.g. [HS=2000]value[HR]…) to the
		// current front document by writing it to a temp file and feeding it
		// to kTCLParserBoss. Returns kSuccess on parser success, kFailure
		// otherwise. The temp file is removed before return.
		ErrorCode RunPagemakeupString(const PMString& tclContent);



#endif // __DocUtils_h__



