// Setting file for XML tags

#ifndef __XMLDefs_h__
#define __XMLDefs_h__

#include "VCPlugInHeaders.h"
#include "XPGID.h"

		const WideString rootArticleTag = WideString("Objet");
		const WideString txtTag = WideString("Contenu");
	    
		// Type Article XRail
	 	const int8 kMEPJavaStory          =  0;
		const int8 kIncopyStory           =  1;

		const PMString articleSuffix=".OBJRART.xml";

		const PMString imgSuffixes=".PHO.xml;.EPS.xml;.PDF.xml";
		const WideString creditTag = WideString("CR");
		const WideString legendTag = WideString("LG");
		const PMString kNoPageFolio = "0";

#endif

