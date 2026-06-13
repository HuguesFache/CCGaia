/*
//
//	File: PrsPreferencesPersist.cpp
//
//  Author: Trias
//
//	Date:	18-Oct-2005
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

/** PrsPreferencesPersist
	Persistent implementation of IPreferences
*/
class PrsPreferencesPersist : public CPMUnknown<IPageMakeUpPrefs>
{
public:
	/**
		Constructor.

		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsPreferencesPersist(IPMUnknown* boss);


	/** Destructor
	*/
	~PrsPreferencesPersist();

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

	/**
		@param stream contains the stream to be written or read.
		@param implementation the implementation ID.
	*/
	void ReadWrite(IPMStream* stream, ImplementationID implementation);

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

CREATE_PERSIST_PMINTERFACE(PrsPreferencesPersist, kPrsPreferencesPersistImpl)

/* Constructor
*/
PrsPreferencesPersist::PrsPreferencesPersist(IPMUnknown* boss)
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
PrsPreferencesPersist::~PrsPreferencesPersist()
{
}


void PrsPreferencesPersist::setStopOnError(bool16 stop)
{
	PreDirty();
	this->stopOnError = stop;	
}

bool16 PrsPreferencesPersist::getStopOnError()
{
	return this->stopOnError;
}

void PrsPreferencesPersist::setStopDisplay(bool16 stop)
{
	PreDirty();
	this->stopDisplay = stop;
}

bool16 PrsPreferencesPersist::getStopDisplay()
{
	return this->stopDisplay;
}

bool16 PrsPreferencesPersist::getDisplayTCLRef()
{
	return this->displayTCLRef;
}

void PrsPreferencesPersist::setDisplayTCLRef(bool16 display)
{
	PreDirty();
	this->displayTCLRef = display;
}

bool16 PrsPreferencesPersist::getDisplayPermRef()
{
	return this->displayPermRef;
}

void PrsPreferencesPersist::setDisplayPermRef(bool16 display)
{
	PreDirty();
	this->displayPermRef = display;
}

int16 PrsPreferencesPersist::getExportType()
{
	return this->exportType;
}

void PrsPreferencesPersist::setExportType(int16 type)
{
	PreDirty();
	this->exportType = type;
}

void PrsPreferencesPersist::getPicturePrefs(bool16 * useImportPath, bool16 * import, PMString * picPath, bool16 * textbox,
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

void PrsPreferencesPersist::setPicturePrefs(bool16 useImportPath, bool16 import, PMString picPath, bool16 textbox,
					bool16 nonPrinting, int32 colorIndex, int32 colorTint)
{
	PreDirty();
	this->useImportPath = useImportPath;
	this->import = import;
	this->picPath = picPath;
	this->textbox = textbox;
	this->nonPrinting = nonPrinting;
	this->colorIndex = colorIndex;
	this->colorTint = colorTint;
}

void PrsPreferencesPersist::getSavePrefs(bool16 * usePath, PMString * savePath)
{
	if(usePath != nil)
		*usePath = this->useSavePath;
	if(savePath != nil)
		*savePath = this->savePath;
}

void PrsPreferencesPersist::setSavePrefs(bool16 usePath, PMString savePath)
{
	PreDirty();
	this->useSavePath = usePath;
	this->savePath = savePath;
}

void PrsPreferencesPersist::getOpenPrefs(bool16 * usePath, PMString * openPath)
{
	if(usePath != nil)
		*usePath = this->useOpenPath;
	if(openPath != nil)
		*openPath = this->openPath;
}

void PrsPreferencesPersist::setOpenPrefs(bool16 usePath, PMString openPath)
{
	PreDirty();
	this->useOpenPath = usePath;
	this->openPath = openPath;
}

void PrsPreferencesPersist::getExportHeaderPrefs(bool16 * exportStyles, bool16 * exportCJ, bool16 * exportColors,
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

void PrsPreferencesPersist::setExportHeaderPrefs(bool16 exportStyles, bool16 exportCJ, bool16 exportColors,
												 bool16 exportLayers, bool16 exportMasters)
{
	PreDirty();
	this->exportStyles = exportStyles;
	this->exportCJ = exportCJ;
	this->exportColors = exportColors;
	this->exportLayers = exportLayers;
	this->exportMasters = exportMasters;
}

void PrsPreferencesPersist::getExportGeometryPrefs(bool16 * exportTextFrame, bool16 * exportImageFrame, bool16 * exportRule)
{
	if(exportTextFrame != nil)
		*exportTextFrame = this->exportTextFrame;
	if(exportImageFrame != nil)
		*exportImageFrame = this->exportImageFrame;
	if(exportRule != nil)
		*exportRule = this->exportRule;
}

void PrsPreferencesPersist::setExportGeometryPrefs(bool16 exportTextFrame, bool16 exportImageFrame, bool16 exportRule)
{
	PreDirty();
	this->exportTextFrame = exportTextFrame;
	this->exportImageFrame = exportImageFrame;
	this->exportRule = exportRule;
}

void PrsPreferencesPersist::getExportTextPrefs(bool16 * exportText, bool16 * exportParaCmd, bool16 * exportTextCmd, bool16 * exportStyleCmd)
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

void PrsPreferencesPersist::setExportTextPrefs(bool16 exportText, bool16 exportParaCmd, bool16 exportTextCmd, bool16 exportStyleCmd)
{
	PreDirty();
	this->exportText = exportText;
	this->exportParaCmd = exportParaCmd;
	this->exportTextCmd = exportTextCmd;
	this->exportStyleCmd = exportStyleCmd;
}

bool16 PrsPreferencesPersist::getPrintTCLRef()
{
	return this->print;
}

void PrsPreferencesPersist::setPrintTCLRef(bool16 print)
{
	PreDirty();
	this->print = print;
}

void PrsPreferencesPersist::CopyFrom(IPageMakeUpPrefs * source)
{
	PreDirty();
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

void PrsPreferencesPersist::ReadWrite(IPMStream* stream, ImplementationID implementation)
{
	// These methods (Xfer...) will read if the stream is a 'read-stream'
	// and write if the stream is a write stream:
	int16 version = 0x02;
	stream->XferInt16(version);

	stream->XferBool(this->stopOnError); 
	stream->XferBool(this->stopDisplay); 
	stream->XferBool(this->displayTCLRef);
	stream->XferBool(this->displayPermRef);
	if(version == 0x02)
		stream->XferInt16(this->exportType);
	
	stream->XferBool(this->useImportPath);
	stream->XferBool(this->import);
	this->picPath.ReadWrite(stream);
	stream->XferBool(this->textbox);
	stream->XferBool(this->nonPrinting);
	stream->XferInt32(this->colorIndex);
	stream->XferInt32(this->colorTint);

	stream->XferBool(this->useSavePath);
	this->savePath.ReadWrite(stream);
	stream->XferBool(this->useOpenPath);
	this->openPath.ReadWrite(stream);

	stream->XferBool(this->exportStyles);
	stream->XferBool(this->exportCJ);
	stream->XferBool(this->exportColors);
	stream->XferBool(this->exportLayers);
	stream->XferBool(this->exportMasters);

	stream->XferBool(this->exportTextFrame);
	stream->XferBool(this->exportImageFrame);
	stream->XferBool(this->exportRule);

	stream->XferBool(this->exportText);
	stream->XferBool(this->exportParaCmd);
	stream->XferBool(this->exportTextCmd);
	stream->XferBool(this->exportStyleCmd);
	stream->XferBool(this->print);
}

