/*
//	File:	ExportUtils.h
//
//	Date:	21-Dec-2005
//
//  Author : Trias
//
*/


#ifndef __ExportUtils_h__
#define __ExportUtils_h__

// This class provides different static functions dealing with export
class ExportUtils 
{

public :

	static ErrorCode IDMLExport(IDocument * doc, PMString destfileFullpath, bool16 eraseExisting);
	
	static ErrorCode EpsExport(IDocument * doc, bool16 eraseExisting, bool16 allPages, int16 startPage = 0, int16 endPage = 0);

	static ErrorCode PdfExport(IDocument * doc, PMString pdfFullName, bool16 eraseExisting, 
							   bool16 allPages, int16 startPage = 0, int16 endPage = 0, UIDList pageItemList = UIDList(),
							   UIDRef bookRef = UIDRef::gNull, PMString bookExportStyle = kNullString);

};

#endif // __ExportUtils_h__