/*
//	File:	TLBStyleHelper.h
//
//  Author: Trias
//
//	Date:	18-Jun-2003
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __TLBStyleHelper_h__
#define __TLBStyleHelper_h__

// API includes
#include "CPMUnknown.h"

// Project includes
#include "IStyleHelper.h"

/** TLBStyleHelper
	Implements the IStyleHelper interface
*/
class TLBStyleHelper : public CPMUnknown<IStyleHelper> 
{

	public :
	
		/**
			Constructor.
			@param boss interface ptr from boss object on which this interface is aggregated.
		*/
		TLBStyleHelper(IPMUnknown * boss);

		/** 
			Destructor
		*/
		~TLBStyleHelper();

		// ------------------
		//  Style management
		// ------------------
		virtual ErrorCode CreateParaStyleCmd(PMString name, UIDRef workspace,const AttributeBossList * overrides, UID basedOn, UID nextStyle, bool8 allowUndo);
		virtual ErrorCode DefaultTextStyleCmd(UIDRef workspace,ClassID strand, UID style, bool8 allowUndo);
		virtual ErrorCode CreateCharStyleCmd(PMString name, UIDRef workspace,const AttributeBossList * overrides, UID basedOn, bool8 allowUndo);
		virtual ErrorCode EditTextStyleAttributesCmd(UIDRef workspace, UID styleToEdit, const AttributeBossList * attributes, bool8 allowUndo);

};

#endif // __TLBStyleHelper_h__