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

/*	Lit le tag EXIF Orientation (0x0112) dans un segment APP1 ("Exif"). 'seg' pointe
	sur la charge utile de l'APP1 (apres FF E1 lenHi lenLo), de longueur segPayloadLen.
	Renvoie 1..8, ou 1 (normal) si absent/illisible. Bornage strict : tolere les EXIF
	malformes sans jamais lire hors du segment.
*/
inline int32 XPGUIParseExifOrientation(const unsigned char* seg, size_t len)
{
	if(len < 14) return 1;
	if(!(seg[0]=='E' && seg[1]=='x' && seg[2]=='i' && seg[3]=='f' && seg[4]==0 && seg[5]==0)) return 1;

	const unsigned char* t = seg + 6;			// debut de l'en-tete TIFF
	const size_t tlen = len - 6;
	if(tlen < 8) return 1;

	bool16 little;
	if(t[0]==0x49 && t[1]==0x49)      little = kTrue;	// "II"
	else if(t[0]==0x4D && t[1]==0x4D) little = kFalse;	// "MM"
	else return 1;

	#define XPGUI_RD16(off) ( (off)+1 >= tlen ? 0xFFFFFFFFu : \
		(little ? ((uint32)t[off] | ((uint32)t[(off)+1]<<8)) \
				: (((uint32)t[off]<<8) | (uint32)t[(off)+1])) )
	#define XPGUI_RD32(off) ( (off)+3 >= tlen ? 0xFFFFFFFFu : \
		(little ? ((uint32)t[off] | ((uint32)t[(off)+1]<<8) | ((uint32)t[(off)+2]<<16) | ((uint32)t[(off)+3]<<24)) \
				: (((uint32)t[off]<<24) | ((uint32)t[(off)+1]<<16) | ((uint32)t[(off)+2]<<8) | (uint32)t[(off)+3])) )

	const uint32 ifd0 = XPGUI_RD32(4);
	if(ifd0 == 0xFFFFFFFFu || ifd0 + 2 > tlen) { return 1; }
	const uint32 nEntries = XPGUI_RD16(ifd0);
	if(nEntries == 0xFFFFFFFFu) { return 1; }

	for(uint32 k = 0; k < nEntries; ++k) {
		const size_t e = (size_t)ifd0 + 2 + (size_t)k * 12;
		if(e + 12 > tlen) break;
		if(XPGUI_RD16(e) == 0x0112) {				// tag Orientation (SHORT, valeur en place)
			const uint32 v = XPGUI_RD16(e + 8);
			return (v >= 1 && v <= 8) ? (int32)v : 1;
		}
	}
	#undef XPGUI_RD16
	#undef XPGUI_RD32
	return 1;
}

/*	Lit 'file' entierement dans outBuf. Si c'est un JPEG, retire tous les segments
	APP1. Renvoie kTrue si outBuf contient des donnees exploitables (a passer a
	StreamUtil::CreatePointerStreamRead). En cas d'echec, renvoie kFalse : l'appelant
	retombe alors sur un flux fichier classique.

	outImgWidth / outImgHeight : dimensions natives de l'image lues dans le marqueur
	SOF du JPEG (0 si inconnues / non-JPEG). Servent a inscrire l'apercu dans le
	widget en preservant le ratio.
	outOrientation : tag EXIF Orientation 1..8 (1 si absent), lu AVANT de retirer
	l'APP1, pour pouvoir reorienter l'apercu.
*/
inline bool16 XPGUIReadFileStrippingExif(const IDFile& file, std::vector<char>& outBuf,
										 int32& outImgWidth, int32& outImgHeight,
										 int32& outOrientation)
{
	outBuf.clear();
	outImgWidth   = 0;
	outImgHeight  = 0;
	outOrientation = 1;

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
		// SOF (Start Of Frame) : on y lit les dimensions natives. Tous les SOFn
		// sauf 0xC4 (DHT), 0xC8 (JPG), 0xCC (DAC). Charge: precision(1), hauteur(2),
		// largeur(2). On garde le premier rencontre (avant le SOS).
		if(outImgWidth == 0 &&
		   marker >= 0xC0 && marker <= 0xCF &&
		   marker != 0xC4 && marker != 0xC8 && marker != 0xCC &&
		   i + 8 < len) {
			outImgHeight = (static_cast<int32>(p[i + 5]) << 8) | static_cast<int32>(p[i + 6]);
			outImgWidth  = (static_cast<int32>(p[i + 7]) << 8) | static_cast<int32>(p[i + 8]);
		}
		if(marker == 0xE1) {				// APP1 (Exif/XMP) : on lit l'orientation puis on saute
			if(outOrientation == 1) {		// payload apres FF E1 lenHi lenLo, longueur segLen-2
				outOrientation = XPGUIParseExifOrientation(p + i + 4, static_cast<size_t>(segLen) - 2);
			}
			i += segTotal;
			continue;
		}
		outBuf.insert(outBuf.end(), raw.begin() + i, raw.begin() + i + segTotal);
		i += segTotal;
	}

	return !outBuf.empty();
}

#endif // __XPGUIStripExif_h__
