/*
//	File:	PermRefsTagTextAdornment.cpp
//
//  Author: Trias
//
//	Date:	21-oct-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#include "VCPlugInHeaders.h"

//Interfaces:
#include "IGraphicsPort.h"
#include "IWaxGlyphs.h"
#include "IWaxLineShape.h"
#include "IDocument.h"
#include "IFontMgr.h"
#include "IPMFont.h"
#include "IFontInstance.h"
#include "ILayoutUIUtils.h"
#include "ILayoutControlData.h"
#include "IPanorama.h"

// Implementation:
#include "CPMUnknown.h"
#include "PMLineSeg.h"
#include "PMMatrix.h"
#include "TransformUtils.h"
#include "IPermRefsTag.h"

// Project:
#include "PrsID.h"
#include "IPageMakeUpPrefs.h"
#include "GlobalDefs.h"


/** From SDK sample; ITextAdornment implementation that hilites a PermRefs data tag
	text attribute, kPermRefsAttrBoss. The
	adornment draws a hilite behind the text to which the attribute applies.
*/
class PermRefsTagTextAdornment : public CPMUnknown<ITextAdornment>
{
	public:
		/** 
			Constructor 
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PermRefsTagTextAdornment(IPMUnknown* boss) : CPMUnknown<ITextAdornment>(boss) {}

		/** Draws behind the text. See ITextAdornment::GetDrawPriority.
		*/
		virtual Text::DrawPriority	GetDrawPriority() {return Text::DrawPriority(Text::kTAPassPriBackground+-0.54);}

		/**
			Returns the inkBounds and a flag indicating whether the adornment would draw 
			on this run. 

			The ink bounds passed into the GetInkBounds() call are empty.  Our adornment 
			draws within the selection bounds of the wax line so it doesn't have to 
			modify the ink bounds (our Draw method shows how the bounds can be calculated).

			We always want our Draw method to be called if we exist so we always return 
			wouldDraw as true.

			See ITextAdornment::GetInkBounds.
		*/
		virtual void GetInkBounds
			(
				PMRect* inkBounds, 
				const PMRect& runInkBounds,
				IWaxRun* waxRun,
				IWaxRenderData* renderData,
				IWaxGlyphs* glyphs, 
				const ITextAdornmentData* data
			);

		/**
			Hilites the text to which our character attribute applies.  Since we cannot 
			rely on ITextAdornment::GetIsActive always being called we check on our preference settings 
			to see if hiliting is turned on before we draw.
			
			See ITextAdornment::Draw
		*/
		virtual void Draw
			(
				GraphicsData* gd, 
				int32 flags, 
				IWaxRun* run,
				IWaxRenderData* renderData, 
				IWaxGlyphs* glyphs,
				const ITextAdornmentData* data
			);

		/**
			See ITextAdornment::SplitAt
		*/
	    virtual ITextAdornmentData* SplitAt
			( 
				int32 offsetInRun, 
				IWaxRun* waxRun, 
				IWaxRun* newWaxRun,
				ITextAdornmentData* data 
			);

	    
	private:
		/**	Get the widht of the wax run.
			@param runGlyphs is the glyph of the wax run.
			@return the width of the run to be hilited, adjusted for carriage return
		*/
		PMReal GetRunWidth
		(
			IWaxGlyphs* runGlyphs
		);

};

/* Makes the implementation available to the application.
*/
CREATE_PMINTERFACE(PermRefsTagTextAdornment, kPermRefsTagTextAdornmentImpl)


/*
*/
void PermRefsTagTextAdornment::GetInkBounds
		(
			PMRect* inkBounds, 
			const PMRect& runInkBounds,
			IWaxRun* run,
			IWaxRenderData* renderData, 
			IWaxGlyphs* glyphs,
			const ITextAdornmentData* data
		)
{
}


/*
*/
void PermRefsTagTextAdornment::Draw
		(
			GraphicsData* gd, 
			int32 flags, 
			IWaxRun* run,
			IWaxRenderData* renderData,
			IWaxGlyphs* runGlyphs,
			const ITextAdornmentData* data
		)
{
	do
	{

		//
		// We only hilite on screen (not print or pdf).
		//
		if ((flags & IShape::kPrinting))
			break;

		//
		// Get the line of wax to be hilited.
		//
		const IWaxLine* waxLine = run->GetWaxLine();

		//
		// Check our preferences to see if hiliting is on.
		//
		InterfacePtr<IPageMakeUpPrefs> prefs ((IPageMakeUpPrefs *) GetExecutionContextSession()->QueryWorkspace(IID_IPAGEMAKEUPPREFS));
		if (prefs == nil)
			break;

		if(!prefs->getDisplayPermRef())
			break;

	 	// Get the width of the run.
	 

		PMReal runWidth = GetRunWidth(runGlyphs);
		if (runWidth <= 0.0)
			break;
		
	 
		// Calculate the rect to be hilited.
	      
		PMReal xPosition = run->GetXPosition();
		InterfacePtr<IWaxLineShape> waxLineShape(waxLine, UseDefaultIID());
		PMLineSeg selection;
		waxLineShape->GetSelectionLine(&selection);


		// Get color from referenceTag

		InterfacePtr<IPermRefsTag> txtAttrPermRef (this, IID_IPERMREFSTAG);
		if(txtAttrPermRef == nil)
			break;
		//
		// Draw the hilite.
		//
		IGraphicsPort* gPort = gd->GetGraphicsPort();
		if(gPort == nil)
			break;

		gPort->newpath();
		gPort->rectpath(xPosition, selection.Top(), runWidth, selection.DeltaY());
		gPort->setrgbcolor(PMReal(0.8), PMReal(1.0), PMReal(0.8));
		gPort->fill();

		// Beyond the highlight, draw the reference number on top so the user
		// can see WHICH ref is at each position (multiple refs can sit in
		// the same block). Zoom-independent sizing — same approach as the
		// block-ID badge in PrsTCLAdornmentShape.
		PermRefStruct refData = txtAttrPermRef->getReference();
		if (refData.ref == 0)
			break;  // No useful label for the null/default permref.

		PMString refLabel;
		refLabel.AppendNumber(static_cast<int32>(refData.ref));
		int32 numUTF16 = 0;
		const textchar* labelBuf = refLabel.GrabUTF16Buffer(&numUTF16);
		if (labelBuf == nil || numUTF16 == 0)
			break;

		InterfacePtr<IFontMgr> fontMgr(GetExecutionContextSession(), UseDefaultIID());
		if (fontMgr == nil)
			break;
		InterfacePtr<IPMFont> theFont(fontMgr->QueryFont(fontMgr->GetDefaultFontName()));
		if (theFont == nil)
			break;

		PMReal fontSize = 8.0;  // Smaller than the block-ID badge (13pt) so
		                        // it stays unobtrusive on top of the highlight.
		PMReal zoom = 1.0;
#if !SERVER
		InterfacePtr<ILayoutControlData> layoutControlData(Utils<ILayoutUIUtils>()->QueryFrontLayoutData());
		InterfacePtr<IPanorama> panorama(layoutControlData, UseDefaultIID());
		if (panorama)
			zoom = panorama->GetXScaleFactor();
#endif
		fontSize = fontSize * (1.0 / zoom);

		PMMatrix matrix(fontSize, 0.0, 0.0, fontSize, 0.0, 0.0);
		InterfacePtr<IFontInstance> fontInst(fontMgr->QueryFontInstance(theFont, matrix));
		if (fontInst == nil)
			break;

		// Draw the label slightly indented from the left of the highlight,
		// near the top of the line. Black text on the green highlight is
		// readable enough without an extra background box.
		PMReal labelX = xPosition + (2.0 * (1.0 / zoom));
		PMReal labelY = selection.Top() + fontInst->GetAscent();
		gPort->setrgbcolor(0, 0, 0);
		gPort->selectfont(theFont, fontSize);
		gPort->show(labelX, labelY, numUTF16, labelBuf);

	} while (false); // Only do once.
}


/*
*/
ITextAdornmentData* PermRefsTagTextAdornment::SplitAt( int32 offsetInRun, IWaxRun* waxRun, IWaxRun* newWaxRun,ITextAdornmentData* data )
{
    //The only kind of adornment data we know how to split is our own
    //and we don't have any
    ASSERT_MSG( data == nil, "can't split someone else's adornment data" ) ;
    return nil ;
}

/*
*/
PMReal PermRefsTagTextAdornment::GetRunWidth
		(
			IWaxGlyphs* runGlyphs
		)
{
	//
	// Get the width of the run to be hilited.
	//
	PMReal runWidth = runGlyphs->GetWidth();

	//
	// Adjust the width so we don't hilite the  
	// carriage return that terminates a story.
	//
	int32 numGlyphs = runGlyphs->GetGlyphCount();
	const Text::GlyphID* glyphArray = runGlyphs->PeekGlyphArray();
	const float* widthArray = runGlyphs->PeekWidthsArray();						

	// BUG FIX : certaines references taguees composent un wax run sans glyphe
	// (run vide, objet ancre, fin d'article...). Sans cette garde, l'acces
	// glyphArray[numGlyphs - 1] lit glyphArray[-1] (ou un tableau nul) et fait
	// planter InDesign au redessin juste apres la pose du tag.
	if (numGlyphs <= 0 || glyphArray == nil || widthArray == nil)
		return runWidth;

	if (glyphArray[numGlyphs - 1] == kInvalidGlyphID)
		runWidth -= ::ToPMReal(widthArray[--numGlyphs]);
	
	return runWidth;
}


