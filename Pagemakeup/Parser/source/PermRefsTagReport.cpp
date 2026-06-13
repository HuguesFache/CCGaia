/*
//	File:	PermRefsTagReport.cpp
//
//  Author: Trias
//
//	Date:	20/10/2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2005 Trias Developpement. All rights reserved.
//
*/

#include "VCPlugInHeaders.h"

// Interfaces:
#include "IDrawingStyle.h"
#include "IPermRefsTag.h"
#include "IAttrReport.h"

// General:
#include "CPMUnknown.h"

#include "CAlert.h"

/** From SDK sample; IAttrReport implementation for a PermRefs data tag
	text attribute, kPermRefsAttrBoss
*/
class PermRefsTagReport : public CPMUnknown<IAttrReport>
{
public:
		/** 
			Constructor 
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		PermRefsTagReport(IPMUnknown* boss) : CPMUnknown<IAttrReport>(boss) {}
	
		/** Return kFalse because we implement a character attribute.
			@return kFalse because we implement a character attribute.
		*/
		virtual bool16 IsParagraphAttribute() const;

		/** Return kTrue because we don't want style changes to blow away our data tags.
			@return kTrue because we don't want style changes to blow away our data tags.
		*/
		virtual bool16 LastsThroughStyleChange() const;

		/** Return IAttrReport::kNeverContinue because we apply the CHDataMerger
			data tag text attribute to fixed ranges of text, if the user types text at the 
			end of the run of characters to which the attribute applies the attribute
			should not be extended.
			
			@return IAttrReport::kNeverContinue.
		*/
		virtual IAttrReport::ContinuationType AutomaticContinuation() const;

        /** Return kFalse because this attribute should only be applied
			as an override, it should never be in a paragraph or character style.

			@return kFalse.
		*/
		virtual bool16 CanBeInStyle() const;

		/** Return kTrue because teh CHDatamerger data tag can stay after a search/replace operation.
			@return kTrue;
		*/
		virtual bool16 SurvivesReplace() const;

		/**	Return kFalse because this attribute should not be omitted from the focus cache.
			@return kFalse.
		*/
		virtual bool16 OmitFromFocusCache() const;

		/**
			Called by the composition as it composes the text in a frame. This is the hook we use 
			to add our adornment to the text so that when the frame is drawn our hilite will be 
			drawn on screen.

			TellComposition may be called by the application _before_ our attribute has been applied 
			to any text. In this situation we must be careful not to activate our adornment (otherwise 
			it will be applied to text to which our attribute does not apply). We detect this situation 
			via a flag in our attribute's interface. We store strings in this interface and we know 
			that GetTable() should never return a empty string if it contains a valid data tag.  We use 
			this as a test to  determine if we have a valid attribute. If so we activate our adornment.

			@param style provides access to indent, space before/after, keep with next, paragraph rule and the 
					effective value of othet text attributes.
			@param draw provides access to font, point size, leading and the effective values of other text attributes.
		*/
		virtual void TellComposition(ICompositionStyle* style, IDrawingStyle* draw) const;

		/** Return IAttrReport::kInvalOnly because changes to this attribute
			don't require the text to be recomposed.
			
			@return IAttrReport::kInvalOnly.
		*/
		virtual IAttrReport::RecompositionLevel RequiresRecomposition() const;

		/** Appends description of the CHDatamerger data tag. 
		
			@param into IN/OUT The PMString that holds the description.  
			@param theDB The current database. (Not used)
			@param attrBossList The attribute boss list that contains other attributes applied. (Not used). 
		*/
		virtual void AppendDescription(PMString* into, IDataBase* theDB, const AttributeBossList* attrBossList) const;
};

/* Make the implementation available to the application.
*/
CREATE_PMINTERFACE(PermRefsTagReport, kPermRefsTagReportImpl)


/*	IsParagraphAttribute
*/
bool16 PermRefsTagReport::IsParagraphAttribute() const
{
	return kFalse;
}


/*	LastsThroughStyleChange
*/
bool16 PermRefsTagReport::LastsThroughStyleChange() const
{
	return kTrue;
}


/*	CanBeInStyle
*/
bool16 PermRefsTagReport::CanBeInStyle() const
{
	return kFalse;
}


/*	SurvivesReplace
*/
bool16 PermRefsTagReport::SurvivesReplace() const
{
	return kTrue;
}


/*	OmitFromFocusCache
*/
bool16 PermRefsTagReport::OmitFromFocusCache() const
{
	return kFalse;
}


/*	AutomaticContinuation
	return kFalse so our attributes are not extended automatically when the user types text 
	at the end of the run of characters to which the attribute applies.
*/
IAttrReport::ContinuationType PermRefsTagReport::AutomaticContinuation() const
{
	return IAttrReport::kNeverContinue;
}


/*	TellComposition
*/
void PermRefsTagReport::TellComposition(ICompositionStyle* iCompositionStyle, IDrawingStyle* iDrawingStyle) const
{
	//
	// Activate our adornment if our attribute is valid
	//
	InterfacePtr<IPermRefsTag> permRefsTag(this, UseDefaultIID());

	if(permRefsTag->getReference() != kNullPermRef
#ifdef MACINTOSH
		&& permRefsTag->getReference().ref != -1  
#endif 	
		){
		
#if AUTO_TAGGING
			if(permRefsTag->getReference().param2 == 1)
				iDrawingStyle->AddTextAdornment(kPermRefsAttrBoss, kPermRefsTag1TextAdornmentBoss, nil);
			else if(permRefsTag->getReference().param2 == 2)
				iDrawingStyle->AddTextAdornment(kPermRefsAttrBoss, kPermRefsTag2TextAdornmentBoss, nil);
			else
				iDrawingStyle->AddTextAdornment(kPermRefsAttrBoss, kPermRefsTagTextAdornmentBoss, nil);
#else
		iDrawingStyle->AddTextAdornment(kPermRefsAttrBoss, kPermRefsTagTextAdornmentBoss, nil);
#endif
		}
}


/*	RequiresRecomposition
*/
IAttrReport::RecompositionLevel PermRefsTagReport::RequiresRecomposition() const
{
	return IAttrReport::kInvalOnly;
}


/*	AppendDescription
*/
void PermRefsTagReport::AppendDescription(PMString* into, IDataBase* theDB, const AttributeBossList* attrBossList) const
{
	if (into) {
		InterfacePtr<IPermRefsTag> permRefsTagTextAttr(this, UseDefaultIID());
		if (permRefsTagTextAttr) {
			PMString conjunction(" + ");
			conjunction.Translate();
			into->Append(conjunction);
			PMString attrDescr(kPermRefsTagNameAbbr);
			attrDescr.Translate();
			attrDescr += ": ";
            attrDescr.AppendNumber(permRefsTagTextAttr->getReference().ref);
			into->Append(attrDescr);
		}
		else {
			CAlert::InformationAlert("permref no tag");
			PMString conjunction(" - ");
			conjunction.Translate();
			into->Append(conjunction);
			PMString attrDescr(kPermRefsTagNameAbbr);
			attrDescr.Translate();
			into->Append(attrDescr);
		}
	}
}


// End, PermRefsTagReport.cpp.

