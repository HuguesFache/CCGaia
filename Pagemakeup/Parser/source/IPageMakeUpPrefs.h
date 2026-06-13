/*
//	File:	IPageMakeUpPrefs.h
//
//  Author: Trias
//
//	Date:	18-Oct-2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __IPageMakeUpPrefs_h__
#define __IPageMakeUpPrefs_h__

#include "PrsID.h"

class IPMUnknown;

/** Provides get and set methods to define PageMakeUp preferences
*/
class IPageMakeUpPrefs : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_IPAGEMAKEUPPREFS };

		virtual bool16 getStopOnError() =0;
		virtual void setStopOnError(bool16 stop) =0;

		virtual bool16 getStopDisplay() =0;
		virtual void setStopDisplay(bool16 stop) =0;

		virtual bool16 getDisplayTCLRef() =0;
		virtual void setDisplayTCLRef(bool16 display) =0;

		virtual bool16 getDisplayPermRef() =0;
		virtual void setDisplayPermRef(bool16 display) =0;

		virtual int16 getExportType() =0;
		virtual void setExportType(int16 type) =0;

		virtual void getPicturePrefs(bool16 * useImportPath, bool16 * import, PMString * picPath, bool16 * textbox,
									 bool16 * nonPrinting, int32 * colorIndex, int32 * colorTint) =0;
		virtual void setPicturePrefs(bool16 useImportPath, bool16 import, PMString picPath, bool16 textbox,
									bool16 nonPrinting, int32 colorIndex, int32 colorTint) =0;
	
		virtual void getSavePrefs(bool16 * usePath, PMString * savePath) =0;
		virtual void setSavePrefs(bool16 usePath, PMString savePath) =0;

		virtual void getOpenPrefs(bool16 * usePath, PMString * openPath) =0;
		virtual void setOpenPrefs(bool16 usePath, PMString openPath) =0;

		virtual void getExportHeaderPrefs(bool16 * exportStyles, bool16 * exportCJ, bool16 * exportColors, 
										  bool16 * exportLayers, bool16 * exportMasters) =0;
		virtual void setExportHeaderPrefs(bool16 exportStyles, bool16 exportCJ, bool16 exportColors,
										  bool16 exportLayers, bool16 exportMasters) =0;

		virtual void getExportGeometryPrefs(bool16 * exportTextFrame, bool16 * exportImageFrame, bool16 * exportRule) =0;
		virtual void setExportGeometryPrefs(bool16 exportTextFrame, bool16 exportImageFrame, bool16 exportRule) =0;

		virtual void getExportTextPrefs(bool16 * exportText, bool16 * exportParaCmd, bool16 * exportTextCmd, bool16 * exportStyleCmd) =0;
		virtual void setExportTextPrefs(bool16 exportText, bool16 exportParaCmd, bool16 exportTextCmd, bool16 exportStyleCmd) =0;

		virtual bool16 getPrintTCLRef() =0;
		virtual void setPrintTCLRef(bool16 print) =0;

		virtual void CopyFrom(IPageMakeUpPrefs * source) =0;
};

#endif // __IPageMakeUpPrefs_h__
