/*
//	File:	PrsEpsExportPrefsPersist.cpp
//
//	Date:	20-Dec-2005
//
//  Author : Trias
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IEPSExportPreferences.h"
#include "IPMStream.h"

// Implementation Includes
#include "CPMUnknown.h"

// Project includes:
#include "PrsID.h"


class PrsEpsExportPrefsPersist : public CPMUnknown<IEPSExportPreferences>
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on 
			which this interface is aggregated.
		*/
		PrsEpsExportPrefsPersist(IPMUnknown* boss);

		/**
			Destructor.
		*/
		virtual ~PrsEpsExportPrefsPersist() {}

		virtual void SetEPSExPageOption(const int32 pageOption);

		virtual void SetEPSExPageRange(const PMString& pageRange);

		virtual void SetEPSExReaderSpread(const int32 readerSpread);

		virtual void SetEPSExPSLevel(const int32 postScriptLevel);

		virtual void SetEPSExBleedOnOff(const int32 bleedOnOff);

		virtual void SetEPSExBleedTop(const PMReal bleedAmt);

		virtual void SetEPSExBleedBottom(const PMReal bleedAmt);

		virtual void SetEPSExBleedInside(const PMReal bleedAmt);

		virtual void SetEPSExBleedOutside(const PMReal bleedAmt);

		virtual void SetEPSExColorSpace(const int32 colorSpace);

		virtual void SetEPSExPreview(const int32 preview);

		virtual void SetEPSExDataFormat(const int32 dataFormat);

		virtual void SetEPSExBitmapSampling(const int32 bitmapSampling);

		virtual void SetEPSExIncludeFonts(const int32 includeFonts);

		virtual void SetEPSExOPIReplace(const int32 opiReplace);

		virtual void SetEPSExAllPageMarks(const int32 allPageMarks);

		virtual void SetEPSExCropMarks(const int32 cropMarks);

		virtual void SetEPSExBleedMarks(const int32 bleedMarks);

		virtual void SetEPSExPageInfo(const int32 pageInfo);

		virtual void SetEPSExRegMarks(const int32 regMarks);

		virtual void SetEPSExColorBars(const int32 colorBars);

		virtual void SetEPSExOmitPDF(const int32 omitPDF);

		virtual void SetEPSExOmitEPS(const int32 omitEPS);

		virtual void SetEPSExOmitBitmapImages(const int32 omitBitmapImages);

		virtual void SetEPSExSpotOverPrint(int32 spotOverPrint);

		virtual void SetEPSExFlattenerStyle(UID const &style);

		virtual void SetEPSExIgnoreFlattenerSpreadOverrides(const int32 ignoreOverrides);

		virtual int32 GetEPSExPageOption() const;

		virtual PMString GetEPSExPageRange() const;

		virtual int32 GetEPSExReaderSpread() const;

		virtual int32 GetEPSExPSLevel() const;

		virtual int32 GetEPSExBleedOnOff() const;

		virtual PMReal GetEPSExBleedTop() const;

		virtual PMReal GetEPSExBleedBottom() const;

		virtual PMReal GetEPSExBleedInside() const;

		virtual PMReal GetEPSExBleedOutside() const;

		virtual int32 GetEPSExColorSpace() const;

		virtual int32 GetEPSExPreview() const;

		virtual int32 GetEPSExDataFormat() const;

		virtual int32 GetEPSExBitmapSampling() const;

		virtual int32 GetEPSExIncludeFonts() const;

		virtual int32 GetEPSExOPIReplace() const;

		virtual int32 GetEPSExAllPageMarks() const;

		virtual int32 GetEPSExCropMarks() const;

		virtual int32 GetEPSExBleedMarks() const;

		virtual int32 GetEPSExPageInfo() const;

		virtual int32 GetEPSExRegMarks() const;

		virtual int32 GetEPSExColorBars() const;

		virtual int32 GetEPSExOmitPDF() const;

		virtual int32 GetEPSExOmitEPS() const;

		virtual int32 GetEPSExOmitBitmapImages() const;

		virtual int32 GetEPSExSpotOverPrint() const;

		virtual UID   GetEPSExFlattenerStyle() const;

		virtual int32 GetEPSExIgnoreFlattenerSpreadOverrides() const;

		void ReadWrite(IPMStream* stream, ImplementationID implementation);
		
		
	private :
	
	int32 pageOption, readerSpread, postScriptLevel, bleedOnOff, colorSpace, preview,
		dataFormat, bitmapSampling, includeFonts, opiReplace, allPageMarks, cropMarks,
		bleedMarks, pageInfo, regMarks, colorBars, omitPDF, omitEPS, omitBitmapImages,
		spotOverPrint, ignoreOverrides;
	UID style;
	PMReal bleedTop, bleedBottom, bleedInside, bleedOutside;
	PMString pageRange;
};
		

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PERSIST_PMINTERFACE(PrsEpsExportPrefsPersist, kPrsEpsExportPrefsPersistImpl)


/*Constructor
*/
PrsEpsExportPrefsPersist::PrsEpsExportPrefsPersist(IPMUnknown* boss) 
	: CPMUnknown<IEPSExportPreferences>(boss)
{
	pageOption = IEPSExportPreferences::kExportAllPages; 
	readerSpread = IEPSExportPreferences::kExportReaderSpreadOFF;
	postScriptLevel = IEPSExportPreferences::kExportPSLevel2;
	bleedOnOff = IEPSExportPreferences::kExportBleedOFF;
	colorSpace = IEPSExportPreferences::kExportPSColorSpaceCMYK;
	preview = IEPSExportPreferences::kExportPreviewTIFF;
	dataFormat = IEPSExportPreferences::kExportASCIIData;
	bitmapSampling = IEPSExportPreferences::kExportBMSampleNormal;
	includeFonts = IEPSExportPreferences::kExportIncludeFontsWhole; 
	opiReplace = IEPSExportPreferences::kExportOPIReplaceOFF;

	// not suppored
	allPageMarks = IEPSExportPreferences::kExportAllPageMarksOFF;
	cropMarks = IEPSExportPreferences::kExportCropMarksOFF;
	bleedMarks = IEPSExportPreferences::kExportBleedMarksOFF;
	pageInfo = IEPSExportPreferences::kExportPageInfoOFF;
	regMarks = IEPSExportPreferences::kExportRegMarksOFF;
	colorBars = IEPSExportPreferences::kExportColorBarsOFF;
	
	omitPDF = IEPSExportPreferences::kExportOmitPDFOFF;
	omitEPS = IEPSExportPreferences::kExportOmitEPSOFF;
	omitBitmapImages = IEPSExportPreferences::kExportOmitBitmapImagesOFF;
	spotOverPrint = IEPSExportPreferences::kExportLegacy;
	ignoreOverrides = IEPSExportPreferences::kExportIgnoreSpreadOverridesOFF;
	style = kInvalidUID;
	bleedTop = bleedBottom = bleedInside = bleedOutside = 0.0;
	pageRange = "";
}

/* ReadWrite
*/
void PrsEpsExportPrefsPersist::ReadWrite(IPMStream* stream, ImplementationID implementation)
{
	int16 version = 0x01;
	stream->XferInt16(version);
	stream->XferInt32(pageOption);
	stream->XferInt32(readerSpread);
	stream->XferInt32(postScriptLevel);
	stream->XferInt32(bleedOnOff);
	stream->XferInt32(colorSpace); 
	stream->XferInt32(preview);
	stream->XferInt32(dataFormat);
	stream->XferInt32(bitmapSampling);
	stream->XferInt32(includeFonts);
	stream->XferInt32(opiReplace);
	stream->XferInt32(allPageMarks);
	stream->XferInt32(cropMarks);
	stream->XferInt32(bleedMarks);
	stream->XferInt32(pageInfo);
	stream->XferInt32(regMarks);
	stream->XferInt32(colorBars);
	stream->XferInt32(omitPDF);
	stream->XferInt32(omitEPS);
	stream->XferInt32(omitBitmapImages);
	stream->XferInt32(spotOverPrint); 
	stream->XferInt32(ignoreOverrides);
	stream->XferReference(style);
	stream->XferRealNumber(bleedTop);
	stream->XferRealNumber(bleedBottom);
	stream->XferRealNumber(bleedInside);
	stream->XferRealNumber(bleedOutside);
	pageRange.ReadWrite(stream);
}


void PrsEpsExportPrefsPersist::SetEPSExPageOption(const int32 pageOption)
{
	PreDirty();
	this->pageOption = pageOption;
}

void PrsEpsExportPrefsPersist::SetEPSExPageRange(const PMString& pageRange)
{
	PreDirty();
	this->pageRange = pageRange;
}

void PrsEpsExportPrefsPersist::SetEPSExReaderSpread(const int32 readerSpread)
{
	PreDirty();
	this->readerSpread = readerSpread;
}

void PrsEpsExportPrefsPersist::SetEPSExPSLevel(const int32 postScriptLevel)
{
	PreDirty();
	this->postScriptLevel = postScriptLevel;
}

void PrsEpsExportPrefsPersist::SetEPSExBleedOnOff(const int32 bleedOnOff)
{
	PreDirty();
	this->bleedOnOff = bleedOnOff;
}

void PrsEpsExportPrefsPersist::SetEPSExBleedTop(const PMReal bleedAmt)
{
	PreDirty();
	this->bleedTop = bleedAmt;
}

void PrsEpsExportPrefsPersist::SetEPSExBleedBottom(const PMReal bleedAmt)
{
	PreDirty();
	this->bleedBottom = bleedAmt;
}

void PrsEpsExportPrefsPersist::SetEPSExBleedInside(const PMReal bleedAmt)
{
	PreDirty();
	this->bleedInside = bleedAmt;
}

void PrsEpsExportPrefsPersist::SetEPSExBleedOutside(const PMReal bleedAmt)
{
	PreDirty();
	this->bleedOutside = bleedAmt;
}

void PrsEpsExportPrefsPersist::SetEPSExColorSpace(const int32 colorSpace)
{
	PreDirty();
	this->colorSpace = colorSpace;
}

void PrsEpsExportPrefsPersist::SetEPSExPreview(const int32 preview)
{
	PreDirty();
	this->preview = preview;
}

void PrsEpsExportPrefsPersist::SetEPSExDataFormat(const int32 dataFormat)
{
	PreDirty();
	this->dataFormat = dataFormat;
}

void PrsEpsExportPrefsPersist::SetEPSExBitmapSampling(const int32 bitmapSampling)
{
	PreDirty();
	this->bitmapSampling = bitmapSampling;
}

void PrsEpsExportPrefsPersist::SetEPSExIncludeFonts(const int32 includeFonts)
{
	PreDirty();
	this->includeFonts = includeFonts;
}

void PrsEpsExportPrefsPersist::SetEPSExOPIReplace(const int32 opiReplace)
{
	PreDirty();
	this->opiReplace = opiReplace;
}

void PrsEpsExportPrefsPersist::SetEPSExAllPageMarks(const int32 allPageMarks)
{
	PreDirty();
	this->allPageMarks = allPageMarks;
}

void PrsEpsExportPrefsPersist::SetEPSExCropMarks(const int32 cropMarks)
{
	PreDirty();
	this->cropMarks = cropMarks;
}

void PrsEpsExportPrefsPersist::SetEPSExBleedMarks(const int32 bleedMarks)
{
	PreDirty();
	this->bleedMarks = bleedMarks;
}

void PrsEpsExportPrefsPersist::SetEPSExPageInfo(const int32 pageInfo)
{
	PreDirty();
	this->pageInfo = pageInfo;
}

void PrsEpsExportPrefsPersist::SetEPSExRegMarks(const int32 regMarks)
{
	PreDirty();
	this->regMarks = regMarks;
}

void PrsEpsExportPrefsPersist::SetEPSExColorBars(const int32 colorBars)
{
	PreDirty();
	this->colorBars = colorBars;
}

void PrsEpsExportPrefsPersist::SetEPSExOmitPDF(const int32 omitPDF)
{
	PreDirty();
	this->omitPDF = omitPDF;
}

void PrsEpsExportPrefsPersist::SetEPSExOmitEPS(const int32 omitEPS)
{
	PreDirty();
	this->omitEPS = omitEPS;
}

void PrsEpsExportPrefsPersist::SetEPSExOmitBitmapImages(const int32 omitBitmapImages)
{
	PreDirty();
	this->omitBitmapImages = omitBitmapImages;
}

void PrsEpsExportPrefsPersist::SetEPSExSpotOverPrint(int32 spotOverPrint)
{
	PreDirty();
	this->spotOverPrint = spotOverPrint;
}

void PrsEpsExportPrefsPersist::SetEPSExFlattenerStyle(UID const &style)
{
	PreDirty();
	this->style = style;
}

void PrsEpsExportPrefsPersist::SetEPSExIgnoreFlattenerSpreadOverrides(const int32 ignoreOverrides)
{
	PreDirty();
	this->ignoreOverrides = ignoreOverrides;
}

int32 PrsEpsExportPrefsPersist::GetEPSExPageOption() const
{
	return this->pageOption;
}

PMString PrsEpsExportPrefsPersist::GetEPSExPageRange() const
{
	return this->pageRange;
}

int32 PrsEpsExportPrefsPersist::GetEPSExReaderSpread() const
{
	return this->readerSpread;
}

int32 PrsEpsExportPrefsPersist::GetEPSExPSLevel() const
{
	return this->postScriptLevel;
}

int32 PrsEpsExportPrefsPersist::GetEPSExBleedOnOff() const
{
	return this->bleedOnOff;
}

PMReal PrsEpsExportPrefsPersist::GetEPSExBleedTop() const
{
	return this->bleedTop;
}

PMReal PrsEpsExportPrefsPersist::GetEPSExBleedBottom() const
{
	return this->bleedBottom;
}

PMReal PrsEpsExportPrefsPersist::GetEPSExBleedInside() const
{
	return this->bleedInside;
}

PMReal PrsEpsExportPrefsPersist::GetEPSExBleedOutside() const
{
	return this->bleedOutside;
}

int32 PrsEpsExportPrefsPersist::GetEPSExColorSpace() const
{
	return this->colorSpace;
}

int32 PrsEpsExportPrefsPersist::GetEPSExPreview() const
{
	return this->preview;
}

int32 PrsEpsExportPrefsPersist::GetEPSExDataFormat() const
{
	return this->dataFormat;
}

int32 PrsEpsExportPrefsPersist::GetEPSExBitmapSampling() const
{
	return this->bitmapSampling;
}

int32 PrsEpsExportPrefsPersist::GetEPSExIncludeFonts() const
{
	return this->includeFonts;
}

int32 PrsEpsExportPrefsPersist::GetEPSExOPIReplace() const
{
	return this->opiReplace;
}

int32 PrsEpsExportPrefsPersist::GetEPSExAllPageMarks() const
{
	return this->allPageMarks;
}

int32 PrsEpsExportPrefsPersist::GetEPSExCropMarks() const
{
	return this->cropMarks;
}

int32 PrsEpsExportPrefsPersist::GetEPSExBleedMarks() const
{
	return this->bleedMarks;
}

int32 PrsEpsExportPrefsPersist::GetEPSExPageInfo() const
{
	return this->pageInfo;
}

int32 PrsEpsExportPrefsPersist::GetEPSExRegMarks() const
{
	return this->regMarks;
}

int32 PrsEpsExportPrefsPersist::GetEPSExColorBars() const
{
	return this->colorBars;
}

int32 PrsEpsExportPrefsPersist::GetEPSExOmitPDF() const
{
	return this->omitPDF;
}

int32 PrsEpsExportPrefsPersist::GetEPSExOmitEPS() const
{
	return this->omitEPS;
}

int32 PrsEpsExportPrefsPersist::GetEPSExOmitBitmapImages() const
{
	return this->omitBitmapImages;
}

int32 PrsEpsExportPrefsPersist::GetEPSExSpotOverPrint() const
{
	return this->spotOverPrint;
}

UID   PrsEpsExportPrefsPersist::GetEPSExFlattenerStyle() const
{
	return this->style;
}

int32 PrsEpsExportPrefsPersist::GetEPSExIgnoreFlattenerSpreadOverrides() const
{
	return this->ignoreOverrides;
}


