/*
//	File:	XPGUIStripExif.h
//
//	Helper partage par les panneaux d'apercu image (XPGUIDisplayImagePanelView
//	et XPGUIDisplayImageFormePanelView).
//
//	Lit un fichier en memoire en retirant les segments JPEG APP1 (Exif/XMP).
//	C'est la qu'est stockee la vignette EXIF embarquee. Certaines vignettes mal
//	formees (tronquees / paddees de zeros) font corrompre le tas a Adobe dans
//	IImportPreview::Create24bitRGBPreview -> InDesign saute (EXCEPTION_HEAP_CORRUPTION).
//	On ne peut pas attraper une corruption de tas avec __try/__except : la seule
//	parade fiable est de ne jamais livrer la vignette pourrie au decodeur. Sans
//	la vignette, l'apercu retombe sur le decodage de l'image principale, comme
//	un import normal (qui, lui, ne plante pas).
*/

#pragma once
#ifndef __XPGUIStripExif_h__
#define __XPGUIStripExif_h__

#include "IPMStream.h"
#include "StreamUtil.h"
#include "FileUtils.h"

#include <vector>

/*	Lit 'file' entierement dans outBuf. Si c'est un JPEG, retire tous les segments
	APP1. Renvoie kTrue si outBuf contient des donnees exploitables (a passer a
	StreamUtil::CreatePointerStreamRead). En cas d'echec, renvoie kFalse : l'appelant
	retombe alors sur un flux fichier classique.
*/
inline bool16 XPGUIReadFileStrippingExif(const IDFile& file, std::vector<char>& outBuf)
{
	outBuf.clear();

	const uint32 size = FileUtils::GetFileSize(file);
	if(size < 4) {
		return kFalse;
	}

	InterfacePtr<IPMStream> s(StreamUtil::CreateFileStreamRead(file));
	if(s == nil) {
		return kFalse;
	}

	std::vector<char> raw(size);
	int32 total = 0;
	while(total < static_cast<int32>(size)) {
		int32 n = s->XferByte(reinterpret_cast<uchar*>(raw.data() + total),
							   static_cast<int32>(size) - total);
		if(n <= 0) {
			break;
		}
		total += n;
	}
	s->Close();
	if(total <= 0) {
		return kFalse;
	}
	raw.resize(total);

	const unsigned char* p = reinterpret_cast<const unsigned char*>(raw.data());
	const size_t len = raw.size();

	// Pas un JPEG (SOI FFD8) ? On renvoie le contenu tel quel.
	if(!(len >= 2 && p[0] == 0xFF && p[1] == 0xD8)) {
		outBuf.swap(raw);
		return kTrue;
	}

	outBuf.reserve(len);
	outBuf.push_back(static_cast<char>(0xFF));	// SOI
	outBuf.push_back(static_cast<char>(0xD8));

	size_t i = 2;
	while(i + 1 < len) {
		if(p[i] != 0xFF) {					// desynchro : on recopie le reste tel quel
			outBuf.insert(outBuf.end(), raw.begin() + i, raw.end());
			break;
		}
		const unsigned char marker = p[i + 1];

		if(marker == 0xFF) {				// octet de bourrage
			outBuf.push_back(static_cast<char>(0xFF));
			++i;
			continue;
		}
		// Marqueurs sans segment (SOI, EOI, RSTn, TEM)
		if(marker == 0xD8 || marker == 0xD9 || marker == 0x01 ||
		   (marker >= 0xD0 && marker <= 0xD7)) {
			outBuf.push_back(static_cast<char>(0xFF));
			outBuf.push_back(static_cast<char>(marker));
			i += 2;
			continue;
		}
		// SOS : suit le flux compresse, on recopie tout jusqu'a la fin.
		if(marker == 0xDA) {
			outBuf.insert(outBuf.end(), raw.begin() + i, raw.end());
			break;
		}
		// Segment avec longueur (2 octets, longueur incluse).
		if(i + 3 >= len) {
			outBuf.insert(outBuf.end(), raw.begin() + i, raw.end());
			break;
		}
		const uint32 segLen = (static_cast<uint32>(p[i + 2]) << 8) | static_cast<uint32>(p[i + 3]);
		const size_t segTotal = 2 + static_cast<size_t>(segLen);	// marqueur + longueur + charge
		if(segLen < 2 || i + segTotal > len) {		// longueur incoherente : on recopie le reste
			outBuf.insert(outBuf.end(), raw.begin() + i, raw.end());
			break;
		}
		if(marker == 0xE1) {				// APP1 (Exif/XMP) : on saute le segment
			i += segTotal;
			continue;
		}
		outBuf.insert(outBuf.end(), raw.begin() + i, raw.begin() + i + segTotal);
		i += segTotal;
	}

	return !outBuf.empty();
}

#endif // __XPGUIStripExif_h__
