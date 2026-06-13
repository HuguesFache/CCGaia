/*
//
//	File: PrsPreferences.cpp
//
//  Author: Trias
//
//	Date:	19-Oct-2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#include "VCPlugInHeaders.h"

// API includes
#include "CPMUnknown.h"

// Project includes:
#include "IPageMakeUpPrefs.h"
#include "IPMStream.h"
#include "GlobalDefs.h"

#include "CAlert.h"

/** PrsPreferences
	Persistent implementation of IPreferences
*/
class PrsPreferences : public CPMUnknown<IPageMakeUpPrefs>
{
public:
	/**
		Constructor.

		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsPreferences(IPMUnknown* boss);


	/** Destructor
	*/
	~PrsPreferences();

	virtual bool16 getStopOnError();
	virtual void setStopOnError(bool16 stop);

	virtual bool16 getStopDisplay();
	virtual void setStopDisplay(bool16 stop);

	virtual bool16 getDisplayTCLRef();
	virtual void setDisplayTCLRef(bool16 display);

	virtual bool16 getDisplayPermRef();
	virtual void setDisplayPermRef(bool16 display);

	virtual int16 getExportType();
	virtual void setExportType(int16 type);

	virtual void getPicturePrefs(bool16 * useImportPath, bool16 * import, PMString * picPath, bool16 * textbox,
									bool16 * nonPrinting, int32 * colorIndex, int32 * colorTint);
	virtual void setPicturePrefs(bool16 useImportPath, bool16 import, PMString picPath, bool16 textbox,
								bool16 nonPrinting, int32 colorIndex, int32 colorTint);

	virtual void getSavePrefs(bool16 * usePath, PMString * savePath);
	virtual void setSavePrefs(bool16 usePath, PMString savePath);

	virtual void getOpenPrefs(bool16 * usePath, PMString * openPath);
	virtual void setOpenPrefs(bool16 usePath, PMString openPath);

	virtual void getExportHeaderPrefs(bool16 * exportStyles, bool16 * exportCJ, bool16 * exportColors,
									  bool16 * exportLayers, bool16 * exportMasters);
	virtual void setExportHeaderPrefs(bool16 exportStyles, bool16 exportCJ, bool16 exportColors,
									  bool16 exportLayers, bool16 exportMasters);

	virtual void getExportGeometryPrefs(bool16 * exportTextFrame, bool16 * exportImageFrame, bool16 * exportRule);
	virtual void setExportGeometryPrefs(bool16 exportTextFrame, bool16 exportImageFrame, bool16 exportRule);

	virtual void getExportTextPrefs(bool16 * exportText, bool16 * exportParaCmd, bool16 * exportTextCmd, bool16 * exportStyleCmd);
	virtual void setExportTextPrefs(bool16 exportText, bool16 exportParaCmd, bool16 exportTextCmd, bool16 exportStyleCmd);

	virtual bool16 getPrintTCLRef();
	virtual void setPrintTCLRef(bool16 print);

	virtual void CopyFrom(IPageMakeUpPrefs * source);

private:
	
	bool16 stopOnError, stopDisplay, displayTCLRef, displayPermRef;
	
	bool16 useImportPath, import, textbox, nonPrinting;
	PMString picPath;
	int32 colorIndex;
	int32 colorTint;

	int16 exportType;

	bool16 useSavePath, useOpenPath;
	PMString savePath, openPath;

	bool16 exportStyles, exportCJ, exportColors, exportLayers, exportMasters;
	bool16 exportTextFrame, exportImageFrame, exportRule;
	bool16 exportText, exportParaCmd, exportTextCmd, exportStyleCmd;
	bool16 print;
};

CREATE_PMINTERFACE(PrsPreferences, kPrsPreferencesImpl)

/* Constructor
*/
PrsPreferences::PrsPreferences(IPMUnknown* boss)
: CPMUnknown<IPageMakeUpPrefs>(boss)
{
	this->stopOnError = kTrue;
	this->stopDisplay = kFalse;
	this->displayTCLRef = kFalse;
	this->displayPermRef = kFalse;
	this->exportType = PDF_EXPORT; // Default export : PDF

	this->useImportPath = kFalse;
	this->import = kTrue;
	this->picPath = PMString("");
	this->textbox = kTrue;
	this->nonPrinting = kTrue;
	this->colorIndex = 9;
	this->colorTint = 100;

	this->useSavePath = kFalse;
	this->savePath = PMString("");
	this->useOpenPath = kFalse;
	this->openPath = PMString("");

	this->exportStyles = this->exportCJ = this->exportColors = this->exportLayers = this->exportMasters = kTrue;

	this->exportTextFrame = this->exportImageFrame = this->exportRule = kTrue;

	this->exportText = this->exportParaCmd = this->exportTextCmd = this->exportStyleCmd = kTrue;
	this->print = kFalse;
}

/* Destructor
*/
PrsPreferences::~PrsPreferences()
{
}


void PrsPreferences::setStopOnError(bool16 stop)
{
	this->stopOnError = stop;
}

bool16 PrsPreferences::getStopOnError()
{
	return this->stopOnError;
}

void PrsPreferences::setStopDisplay(bool16 stop)
{
	this->stopDisplay = stop;
}

bool16 PrsPreferences::getStopDisplay()
{
	return this->stopDisplay;
}

bool16 PrsPreferences::getDisplayTCLRef()
{
	return this->displayTCLRef;
}

void PrsPreferences::setDisplayTCLRef(bool16 display)
{
	this->displayTCLRef = display;
}

bool16 PrsPreferences::getDisplayPermRef()
{
	return this->displayPermRef;
}

void PrsPreferences::setDisplayPermRef(bool16 display)
{
	this->displayPermRef = display;
}

int16 PrsPreferences::getExportType()
{
	return this->exportType;
}

void PrsPreferences::setExportType(int16 type)
{
	this->exportType = type;
}

void PrsPreferences::getPicturePrefs(bool16 * useImportPath, bool16 * import, PMString * picPath, bool16 * textbox,
						bool16 * nonPrinting, int32 * colorIndex, int32 * colorTint)
{
	if(useImportPath != nil)
		*useImportPath = this->useImportPath;
	if(import != nil)
		*import = this->import;
	if(picPath != nil)
		*picPath = this->picPath;
	if(textbox != nil)
		*textbox = this->textbox;
	if(nonPrinting != nil)
		*nonPrinting = this->nonPrinting;
	if(colorIndex != nil)
		*colorIndex = this->colorIndex;
	if(colorTint != nil)
		*colorTint = this->colorTint;
}

void PrsPreferences::setPicturePrefs(bool16 useImportPath, bool16 import, PMString picPath, bool16 textbox,
					bool16 nonPrinting, int32 colorIndex, int32 colorTint)
{
	this->useImportPath = useImportPath;
	this->import = import;
	this->picPath = picPath;
	this->textbox = textbox;
	this->nonPrinting = nonPrinting;
	this->colorIndex = colorIndex;
	this->colorTint = colorTint;
}

void PrsPreferences::getSavePrefs(bool16 * usePath, PMString * savePath)
{
	if(usePath != nil)
		*usePath = this->useSavePath;
	if(savePath != nil)
		*savePath = this->savePath;
}

void PrsPreferences::setSavePrefs(bool16 usePath, PMString savePath)
{
	this->useSavePath = usePath;
	this->savePath = savePath;
}

void PrsPreferences::getOpenPrefs(bool16 * usePath, PMString * openPath)
{
	if(usePath != nil)
		*usePath = this->useOpenPath;
	if(openPath != nil)
		*openPath = this->openPath;
}

void PrsPreferences::setOpenPrefs(bool16 usePath, PMString openPath)
{
	this->useOpenPath = usePath;
	this->openPath = openPath;
}

void PrsPreferences::getExportHeaderPrefs(bool16 * exportStyles, bool16 * exportCJ, bool16 * exportColors,
												 bool16 * exportLayers, bool16 * exportMasters)
{
	if(exportStyles != nil)
		*exportStyles = this->exportStyles;
	if(exportCJ != nil)
		*exportCJ = this->exportCJ;
	if(exportColors != nil)
		*exportColors = this->exportColors;
	if(exportLayers != nil)
		*exportLayers = this->exportLayers;
	if(exportMasters != nil)
		*exportMasters = this->exportMasters;
}

void PrsPreferences::setExportHeaderPrefs(bool16 exportStyles, bool16 exportCJ, bool16 exportColors,
												 bool16 exportLayers, bool16 exportMasters)
{
	this->exportStyles = exportStyles;
	this->exportCJ = exportCJ;
	this->exportColors = exportColors;
	this->exportLayers = exportLayers;
	this->exportMasters = exportMasters;
}

void PrsPreferences::getExportGeometryPrefs(bool16 * exportTextFrame, bool16 * exportImageFrame, bool16 * exportRule)
{
	if(exportTextFrame != nil)
		*exportTextFrame = this->exportTextFrame;
	if(exportImageFrame != nil)
		*exportImageFrame = this->exportImageFrame;
	if(exportRule != nil)
		*exportRule = this->exportRule;
}

void PrsPreferences::setExportGeometryPrefs(bool16 exportTextFrame, bool16 exportImageFrame, bool16 exportRule)
{
	this->exportTextFrame = exportTextFrame;
	this->exportImageFrame = exportImageFrame;
	this->exportRule = exportRule;
}

void PrsPreferences::getExportTextPrefs(bool16 * exportText, bool16 * exportParaCmd, bool16 * exportTextCmd, bool16 * exportStyleCmd)
{
	if(exportText != nil)
		*exportText = this->exportText;
	if(exportParaCmd != nil)
		*exportParaCmd = this->exportParaCmd;
	if(exportTextCmd != nil)
		*exportTextCmd = this->exportTextCmd;
	if(exportStyleCmd != nil)
		*exportStyleCmd = this->exportStyleCmd;
}

void PrsPreferences::setExportTextPrefs(bool16 exportText, bool16 exportParaCmd, bool16 exportTextCmd, bool16 exportStyleCmd)
{
	this->exportText = exportText;
	this->exportParaCmd = exportParaCmd;
	this->exportTextCmd = exportTextCmd;
	this->exportStyleCmd = exportStyleCmd;
}

bool16 PrsPreferences::getPrintTCLRef()
{
	return this->print;
}

void PrsPreferences::setPrintTCLRef(bool16 print)
{
	this->print = print;
}

void PrsPreferences::CopyFrom(IPageMakeUpPrefs * source)
{
	stopOnError = source->getStopOnError();
	stopDisplay = source->getStopDisplay();
	displayTCLRef = source->getDisplayTCLRef();
	displayPermRef = source->getDisplayPermRef();
	exportType = source->getExportType();

	source->getPicturePrefs(&useImportPath,&import,&picPath,&textbox,&nonPrinting,&colorIndex,&colorTint);

	source->getOpenPrefs(&useOpenPath,&openPath);
	source->getSavePrefs(&useSavePath,&savePath);

	source->getExportHeaderPrefs(&exportStyles,&exportCJ,&exportColors,&exportLayers,&exportMasters);
	source->getExportGeometryPrefs(&exportTextFrame,&exportImageFrame,&exportRule);
	source->getExportTextPrefs(&exportText,&exportParaCmd,&exportTextCmd,&exportStyleCmd);
	print = source->getPrintTCLRef();
}



