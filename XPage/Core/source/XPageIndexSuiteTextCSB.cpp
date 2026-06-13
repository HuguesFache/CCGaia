/*
//	File:	XPageIndexSuiteTextCSB.cpp
//
//	Author:	Trias Developpement
//
//	Date:	23/07/2006
//
*/

#include "VCPlugInHeaders.h"								

// Interface headers
#include "IXPageIndexSuite.h"	
#include "ITextTarget.h"
#include "TextIterator.h"
#include "ITextModel.h"

// General includes
#include "RangeData.h"

class XPageIndexSuiteTextCSB : public CPMUnknown<IXPageIndexSuite>
{
public:
	/**
		Constructor.
		@param iBoss interface ptr from boss object on which this interface is aggregated.
	*/
	XPageIndexSuiteTextCSB(IPMUnknown *iBoss);
	
	/** Destructor. */
	virtual ~XPageIndexSuiteTextCSB(void);

	/**
		See IXPageIndexSuite::GetSelectedText
	*/
	virtual void GetSelectedText(WideString& txt, UIDRef& txtModel, bool16& multiLineSelection, TextIndex& startIndex, TextIndex& endIndex);
};

/* CREATE_PMINTERFACE
 	Binds the C++ implementation class onto its ImplementationID making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(XPageIndexSuiteTextCSB, kXPGIndexSuiteTextCSBImpl)

/* XPageIndexSuiteTextCSB Constructor
*/
XPageIndexSuiteTextCSB::XPageIndexSuiteTextCSB(IPMUnknown* iBoss) :
	CPMUnknown<IXPageIndexSuite>(iBoss)
{
}

/* XPageIndexSuiteTextCSB Destructor
*/
XPageIndexSuiteTextCSB::~XPageIndexSuiteTextCSB(void)
{
}


/* GetSelectedText
*/
void XPageIndexSuiteTextCSB::GetSelectedText(WideString& txt, UIDRef& textModel, bool16& multiLineSelection, TextIndex& startIndex, TextIndex& endIndex)
{
	InterfacePtr<ITextTarget> txtTarget (this, UseDefaultIID());

    InterfacePtr<ITextModel> txtModel (txtTarget->QueryTextModel());

    textModel = ::GetUIDRef(txtModel);
	
	// Get selected range
    RangeData range = txtTarget->GetRange();

    startIndex = range.Start(nil);
	endIndex = range.End();

    TextIterator begin(txtModel, startIndex);
    TextIterator end(txtModel, endIndex);
    
    // Iterate text selection to filter out some characters
    multiLineSelection = kFalse;
    txt.Clear();

    for (TextIterator iter = begin; iter != end && !multiLineSelection; iter++) {
        const UTF32TextChar characterCode = *iter;

        // Filter character codes
        switch(characterCode.GetValue())
	    {
		    case kTextChar_CR : 
            case kTextChar_LF :
                if(iter - begin != range.Length()-1)           
			        multiLineSelection = kTrue;
			    break;

		    case kTextChar_EmSpace :
		    case kTextChar_EnSpace : 
		    case kTextChar_ThinSpace : 
		    case kTextChar_FigureSpace : 
		    case kTextChar_IndentToHere : 
		    case kTextChar_RightAlignedTab : 
            case kTextChar_Tab : 
                txt.Append(kTextChar_Space);
                break;

		    case kTextChar_ObjectReplacementCharacter : break;
		    case kTextChar_ZeroSpaceNoBreak : break;
		    default : 			
			    txt.Append(characterCode); 
			    break;
	    }
    }   

}
