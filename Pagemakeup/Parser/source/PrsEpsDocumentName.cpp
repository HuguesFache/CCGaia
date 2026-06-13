/*
//	File:	PrsEpsDocumentName.cpp
//
//  Author: Trias
//
//	Date:	27/07/2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2005 Trias Developpement. All rights reserved.
//
*/

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IEpsDocumentName.h"

// Implementation Includes
#include "CPMUnknown.h"

// Project includes:
#include "PrsID.h"


class PrsEpsDocumentName : public CPMUnknown<IEpsDocumentName>
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on 
			which this interface is aggregated.
		*/
		PrsEpsDocumentName(IPMUnknown* boss);

		/**
			Destructor.
		*/
		virtual ~PrsEpsDocumentName() {}
		
		virtual PMString getEpsFileName();
	
		virtual PMString getEpsFileExt();
		
		virtual void setEpsFileName(PMString fileName);
		
		virtual void setEpsFileExt(PMString fileExt);
		
	private :
	
		PMString fileName, fileExt;
};
		

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(PrsEpsDocumentName, kPrsEpsDocumentNameImpl)


/* Constructor
*/
PrsEpsDocumentName::PrsEpsDocumentName(IPMUnknown* boss) 
	: CPMUnknown<IEpsDocumentName>(boss)
{
	fileName = kNullString;
	fileExt = kNullString;
}


/* getEpsFileName
*/
PMString PrsEpsDocumentName::getEpsFileName()
{
	return fileName;
}

/* getEpsFileExt
*/
PMString PrsEpsDocumentName::getEpsFileExt()
{
	return fileExt;
}

/* setEpsFileName
*/
void PrsEpsDocumentName::setEpsFileName(PMString fileName)
{
	this->fileName = fileName;
}

/* setEpsFileExt
*/
void PrsEpsDocumentName::setEpsFileExt(PMString fileExt)
{
	this->fileExt = fileExt;	
}


