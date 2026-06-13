/*
//
//	File:	PrsWideStringData.cpp
//
//  Author: Trias
//
//	Date:	27-mar-2006
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2006 Trias Developpement. All rights reserved.
//
*/

#include "VCPluginHeaders.h"

// General includes
#include "ErrorUtils.h"
#include "Command.h"
#include "CPMUnknown.h"

// Interface includes
#include "ISubject.h"

// Project includes
#include "IWideStringData.h"

/** PrsWideStringData
*/
class PrsWideStringData :  public CPMUnknown<IWideStringData>
{
public:
	/**
		Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.
	*/
	PrsWideStringData(IPMUnknown* boss);

	/**
		Destructor.
	*/
	~PrsWideStringData();

	virtual void Set(const WideString& newString);
 
    virtual const WideString& Get() const;

private:

	WideString data;
};


CREATE_PMINTERFACE(PrsWideStringData, kPrsWideStringDataImpl)

/* Constructor
*/
PrsWideStringData::PrsWideStringData(IPMUnknown* boss) 
: CPMUnknown<IWideStringData>(boss)
{
}

/* Destructor
*/
PrsWideStringData::~PrsWideStringData()
{
}

void PrsWideStringData::Set(const WideString& newString)
{
	data = newString;
}

const WideString& PrsWideStringData::Get() const
{
	return data;
}

