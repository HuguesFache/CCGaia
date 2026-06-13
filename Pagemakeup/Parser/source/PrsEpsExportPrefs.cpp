/*
//	File:	PrsEpsExportPrefs.cpp
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


class PrsEpsExportPrefs : public CPMUnknown<IEPSExportPreferences>
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on 
			which this interface is aggregated.
		*/
		PrsEpsExportPrefs(IPMUnknown* boss);

		/**
			Destructor.
		*/
		virtual ~PrsEpsExportPrefs() {}

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
CREATE_PMINTERFACE(PrsEpsExportPrefs, kPrsEpsExportPrefsImpl)


/*Constructor
*/
PrsEpsExportPrefs::PrsEpsExportPrefs(IPMUnknown* boss) 
	: CPMUnknown<IEPSExportPreferences>(boss)
{
	pageOption = IEPSExportPreferences::kDontChange; 
	readerSpread = IEPSExportPreferences::kDontChange;
	postScriptLevel = IEPSExportPreferences::kDontChange;
	bleedOnOff = IEPSExportPreferences::kDontChange;
	colorSpace = IEPSExportPreferences::kDontChange;
	preview = IEPSExportPreferences::kDontChange;
	dataFormat = IEPSExportPreferences::kDontChange;
	bitmapSampling = IEPSExportPreferences::kDontChange;
	includeFonts = IEPSExportPreferences::kDontChange; 
	opiReplace = IEPSExportPreferences::kDontChange;

	// not suppored
	allPageMarks = IEPSExportPreferences::kDontChange;
	cropMarks = IEPSExportPreferences::kDontChange;
	bleedMarks = IEPSExportPreferences::kDontChange;
	pageInfo = IEPSExportPreferences::kDontChange;
	regMarks = IEPSExportPreferences::kDontChange;
	colorBars = IEPSExportPreferences::kDontChange;
	
	omitPDF = IEPSExportPreferences::kDontChange;
	omitEPS = IEPSExportPreferences::kDontChange;
	omitBitmapImages = IEPSExportPreferences::kDontChange;
	spotOverPrint = IEPSExportPreferences::kDontChange;
	ignoreOverrides = IEPSExportPreferences::kDontChange;
	style = kInvalidUID;
	bleedTop = bleedBottom = bleedInside = bleedOutside = 0.0;
	pageRange = "";
}

/* ReadWrite
*/

void PrsEpsExportPrefs::SetEPSExPageOption(const int32 pageOption)
{
	this->pageOption = pageOption;
}

void PrsEpsExportPrefs::SetEPSExPageRange(const PMString& pageRange)
{
	this->pageRange = pageRange;
}

void PrsEpsExportPrefs::SetEPSExReaderSpread(const int32 readerSpread)
{
	this->readerSpread = readerSpread;
}

void PrsEpsExportPrefs::SetEPSExPSLevel(const int32 postScriptLevel)
{
	this->postScriptLevel = postScriptLevel;
}

void PrsEpsExportPrefs::SetEPSExBleedOnOff(const int32 bleedOnOff)
{
	this->bleedOnOff = bleedOnOff;
}

void PrsEpsExportPrefs::SetEPSExBleedTop(const PMReal bleedAmt)
{
	this->bleedTop = bleedAmt;
}

void PrsEpsExportPrefs::SetEPSExBleedBottom(const PMReal bleedAmt)
{
	this->bleedBottom = bleedAmt;
}

void PrsEpsExportPrefs::SetEPSExBleedInside(const PMReal bleedAmt)
{
	this->bleedInside = bleedAmt;
}

void PrsEpsExportPrefs::SetEPSExBleedOutside(const PMReal bleedAmt)
{
	this->bleedOutside = bleedAmt;
}

void PrsEpsExportPrefs::SetEPSExColorSpace(const int32 colorSpace)
{
	this->colorSpace = colorSpace;
}

void PrsEpsExportPrefs::SetEPSExPreview(const int32 preview)
{
	this->preview = preview;
}

void PrsEpsExportPrefs::SetEPSExDataFormat(const int32 dataFormat)
{
	this->dataFormat = dataFormat;
}

void PrsEpsExportPrefs::SetEPSExBitmapSampling(const int32 bitmapSampling)
{
	this->bitmapSampling = bitmapSampling;
}

void PrsEpsExportPrefs::SetEPSExIncludeFonts(const int32 includeFonts)
{
	this->includeFonts = includeFonts;
}

void PrsEpsExportPrefs::SetEPSExOPIReplace(const int32 opiReplace)
{
	this->opiReplace = opiReplace;
}

void PrsEpsExportPrefs::SetEPSExAllPageMarks(const int32 allPageMarks)
{
	this->allPageMarks = allPageMarks;
}

void PrsEpsExportPrefs::SetEPSExCropMarks(const int32 cropMarks)
{
	this->cropMarks = cropMarks;
}

void PrsEpsExportPrefs::SetEPSExBleedMarks(const int32 bleedMarks)
{
	this->bleedMarks = bleedMarks;
}

void PrsEpsExportPrefs::SetEPSExPageInfo(const int32 pageInfo)
{
	this->pageInfo = pageInfo;
}

void PrsEpsExportPrefs::SetEPSExRegMarks(const int32 regMarks)
{
	this->regMarks = regMarks;
}

void PrsEpsExportPrefs::SetEPSExColorBars(const int32 colorBars)
{
	this->colorBars = colorBars;
}

void PrsEpsExportPrefs::SetEPSExOmitPDF(const int32 omitPDF)
{
	this->omitPDF = omitPDF;
}

void PrsEpsExportPrefs::SetEPSExOmitEPS(const int32 omitEPS)
{
	this->omitEPS = omitEPS;
}

void PrsEpsExportPrefs::SetEPSExOmitBitmapImages(const int32 omitBitmapImages)
{
	this->omitBitmapImages = omitBitmapImages;
}

void PrsEpsExportPrefs::SetEPSExSpotOverPrint(int32 spotOverPrint)
{
	this->spotOverPrint = spotOverPrint;
}

void PrsEpsExportPrefs::SetEPSExFlattenerStyle(UID const &style)
{
	this->style = style;
}

void PrsEpsExportPrefs::SetEPSExIgnoreFlattenerSpreadOverrides(const int32 ignoreOverrides)
{
	this->ignoreOverrides = ignoreOverrides;
}

int32 PrsEpsExportPrefs::GetEPSExPageOption() const
{
	return this->pageOption;
}

PMString PrsEpsExportPrefs::GetEPSExPageRange() const
{
	return this->pageRange;
}

int32 PrsEpsExportPrefs::GetEPSExReaderSpread() const
{
	return this->readerSpread;
}

int32 PrsEpsExportPrefs::GetEPSExPSLevel() const
{
	return this->postScriptLevel;
}

int32 PrsEpsExportPrefs::GetEPSExBleedOnOff() const
{
	return this->bleedOnOff;
}

PMReal PrsEpsExportPrefs::GetEPSExBleedTop() const
{
	return this->bleedTop;
}

PMReal PrsEpsExportPrefs::GetEPSExBleedBottom() const
{
	return this->bleedBottom;
}

PMReal PrsEpsExportPrefs::GetEPSExBleedInside() const
{
	return this->bleedInside;
}

PMReal PrsEpsExportPrefs::GetEPSExBleedOutside() const
{
	return this->bleedOutside;
}

int32 PrsEpsExportPrefs::GetEPSExColorSpace() const
{
	return this->colorSpace;
}

int32 PrsEpsExportPrefs::GetEPSExPreview() const
{
	return this->preview;
}

int32 PrsEpsExportPrefs::GetEPSExDataFormat() const
{
	return this->dataFormat;
}

int32 PrsEpsExportPrefs::GetEPSExBitmapSampling() const
{
	return this->bitmapSampling;
}

int32 PrsEpsExportPrefs::GetEPSExIncludeFonts() const
{
	return this->includeFonts;
}

int32 PrsEpsExportPrefs::GetEPSExOPIReplace() const
{
	return this->opiReplace;
}

int32 PrsEpsExportPrefs::GetEPSExAllPageMarks() const
{
	return this->allPageMarks;
}

int32 PrsEpsExportPrefs::GetEPSExCropMarks() const
{
	return this->cropMarks;
}

int32 PrsEpsExportPrefs::GetEPSExBleedMarks() const
{
	return this->bleedMarks;
}

int32 PrsEpsExportPrefs::GetEPSExPageInfo() const
{
	return this->pageInfo;
}

int32 PrsEpsExportPrefs::GetEPSExRegMarks() const
{
	return this->regMarks;
}

int32 PrsEpsExportPrefs::GetEPSExColorBars() const
{
	return this->colorBars;
}

int32 PrsEpsExportPrefs::GetEPSExOmitPDF() const
{
	return this->omitPDF;
}

int32 PrsEpsExportPrefs::GetEPSExOmitEPS() const
{
	return this->omitEPS;
}

int32 PrsEpsExportPrefs::GetEPSExOmitBitmapImages() const
{
	return this->omitBitmapImages;
}

int32 PrsEpsExportPrefs::GetEPSExSpotOverPrint() const
{
	return this->spotOverPrint;
}

UID   PrsEpsExportPrefs::GetEPSExFlattenerStyle() const
{
	return this->style;
}

int32 PrsEpsExportPrefs::GetEPSExIgnoreFlattenerSpreadOverrides() const
{
	return this->ignoreOverrides;
}


