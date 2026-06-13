/*
//	File:	PrsEpsDocumentNamePersist.cpp
//
//  Author: Trias
//
//	Date:	21/12/2005
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


class PrsEpsDocumentNamePersist : public CPMUnknown<IEpsDocumentName>
{
	public:
		/**
			Constructor.
			@param boss interface ptr from boss object on 
			which this interface is aggregated.
		*/
		PrsEpsDocumentNamePersist(IPMUnknown* boss);

		/**
			Destructor.
		*/
		virtual ~PrsEpsDocumentNamePersist() {}
		
		virtual PMString getEpsFileName();
	
		virtual PMString getEpsFileExt();
		
		virtual void setEpsFileName(PMString fileName);
		
		virtual void setEpsFileExt(PMString fileExt);

		void ReadWrite(IPMStream* stream, ImplementationID implementation);
		
	private :
	
		PMString fileName, fileExt;
};
		

/* CREATE_PERSIST_PMINTERFACE
 Binds the C++ implementation class onto its ImplementationID 
 making the C++ code callable by the application.
*/
CREATE_PERSIST_PMINTERFACE(PrsEpsDocumentNamePersist, kPrsEpsDocumentNamePersistImpl)


/* Constructor
*/
PrsEpsDocumentNamePersist::PrsEpsDocumentNamePersist(IPMUnknown* boss) 
	: CPMUnknown<IEpsDocumentName>(boss)
{
	fileName = kNullString;
	fileExt = kNullString;
}

/* ReadWrite
*/
void PrsEpsDocumentNamePersist::ReadWrite(IPMStream* stream, ImplementationID implementation)
{
	fileName.ReadWrite(stream);
	fileExt.ReadWrite(stream);
}


/* getEpsFileName
*/
PMString PrsEpsDocumentNamePersist::getEpsFileName()
{
	return fileName;
}

/* getEpsFileExt
*/
PMString PrsEpsDocumentNamePersist::getEpsFileExt()
{
	return fileExt;
}

/* setEpsFileName
*/
void PrsEpsDocumentNamePersist::setEpsFileName(PMString fileName)
{
	PreDirty();
	this->fileName = fileName;
}

/* setEpsFileExt
*/
void PrsEpsDocumentNamePersist::setEpsFileExt(PMString fileExt)
{
	PreDirty();
	this->fileExt = fileExt;	
}
