/*
//	File:	PermRefsTag1TextAdornment.cpp
//
//  Author: Wilfried LEFEVRE
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

// Implementation:
#include "CPMUnknown.h"
#include "PMLineSeg.h"
#include "TransformUtils.h"
#include "IPermRefsTag.h"
#include "ILastPermRefsTag.h"
#include "IWorkspace.h"
#include "CAlert.h"

// Project:
#include "PrsID.h"
#include "IPageMakeUpPrefs.h"


/** From SDK sample; ITextAdornment implementation that hilites a PermRefs data tag
	text attribute, kPermRefsAttrBoss. The
	adornment draws a hilite behind the text to which the attribute applies.
*/
class PermRefsTag1TextAdornment : public CPMUnknown<ITextAdornment>
{
	public:
		/** 
			Constructor 
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PermRefsTag1TextAdornment(IPMUnknown* boss) : CPMUnknown<ITextAdornment>(boss) {}

		/** Draws behind the text. See ITextAdornment::GetDrawPriority.
		*/
		virtual Text::DrawPriority	GetDrawPriority() {
			return Text::DrawPriority(Text::kTAPassPriBackground+-0.54);
		}

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
CREATE_PMINTERFACE(PermRefsTag1TextAdornment, kPermRefsTag1TextAdornmentImpl)


/*
*/
void PermRefsTag1TextAdornment::GetInkBounds(
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
void PermRefsTag1TextAdornment::Draw
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

		// Draw the hilite.		
		IGraphicsPort* gPort = gd->GetGraphicsPort();
		if(gPort == nil)
			break;

		gPort->newpath();
		gPort->rectpath(xPosition, selection.Top(), runWidth, selection.DeltaY());	
		gPort->setrgbcolor(PMReal(0.0), PMReal(0.69), PMReal(0.89));	
		gPort->fill();

	} while (false); // Only do once.
}

/*
*/
ITextAdornmentData* PermRefsTag1TextAdornment::SplitAt( int32 offsetInRun, IWaxRun* waxRun, IWaxRun* newWaxRun,ITextAdornmentData* data )
{
    //The only kind of adornment data we know how to split is our own
    //and we don't have any
    ASSERT_MSG( data == nil, "can't split someone else's adornment data" ) ;
    return nil ;
}

/*
*/
PMReal PermRefsTag1TextAdornment::GetRunWidth
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


