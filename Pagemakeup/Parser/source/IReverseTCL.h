/*
//	File:	IReverseTCL.h
//
//  Author: Trias
//
//	Date:	16-Sep-2005
//
//	ADOBE SYSTEMS INCORPORATED
//	Copyright 2003 Trias Developpement. All rights reserved.
//
*/

#ifndef __IReverseTCL_h__
#define __IReverseTCL_h__

#include "PrsID.h"

class IPMUnknown;

/** IReverseTCL
	Provides method to write TCL files.
*/
class IReverseTCL : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_IREVERSETCL };

		
		/**
			Set the TCL absolute file name to be generated 
		*/
		virtual bool8 SetOutputFile(IDFile file) =0;
		
		/**
			Start the reverse TCL process
		*/
		virtual ErrorCode StartReverse() =0;

		/**
			Export preferences
		*/
		virtual void setExportHeaderPrefs(bool16 exportStyles, bool16 exportCJ, bool16 exportColors, bool16 exportLayers, bool16 exportMasters) =0;

		virtual void setExportGeometryPrefs(bool16 exportTextFrame, bool16 exportImageFrame, bool16 exportRule) =0;

		virtual void setExportTextPrefs(bool16 exportText, bool16 exportParaCmd, bool16 exportTextCmd, bool16 exportStyleCmd) =0;

};	
#endif // __ITCLWriter_h__