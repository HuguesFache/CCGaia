/*
//	File:	IResaRedacDataList.h
//
//	STUB header. The original "Résa Rédactionnelles" feature was removed from
//	XPage along with its palette and persistent data model. The TCL commands
//	in this Parser (XPage.cpp Func_AC / Func_AA / etc., XRail.cpp export)
//	still reference IResaRedacDataList; rather than rip out every call site
//	we keep this minimal interface declaration so the code compiles.
//
//	No implementation is registered, so:
//	    InterfacePtr<IResaRedacDataList>(doc, UseDefaultIID())
//	always resolves to nil. Call sites must guard against that — the helpers
//	below are only meaningful when an impl exists, which it no longer does.
//
//	If "Résa Rédactionnelles" is ever revived, replace this stub with the
//	full interface from the historical XPage codebase.
*/

#ifndef __IResaRedacDataList_h__
#define __IResaRedacDataList_h__

#include "IPMUnknown.h"
#include "PMString.h"
#include "prsID.h"		// for kPrsPrefix — we use a free slot in Parser's space

// PMIDs used by the leftover code paths. No class or impl is registered
// against them; their only role is to make the source compile. Slots
// chosen well above the highest PMID in prsID.h (currently kPrsPrefix + ~52).
DECLARE_PMID(kInterfaceIDSpace, IID_IRESAREDACDATALIST,				kPrsPrefix + 100)
DECLARE_PMID(kClassIDSpace,     kXPGSetResaRedacDataListCmdBoss,	kPrsPrefix + 101)


/** Per-reservation payload. The original struct had several fields;
	we keep just the ones the leftover Parser code reads / writes.
*/
struct ResaRedac
{
	int32    id;
	PMString topic;
	int32    nbSigns;
	int32    nbImages;
	PMString comment;

	ResaRedac() : id(-1), nbSigns(0), nbImages(0) {}

	bool16 operator==(const ResaRedac& other) const
	{
		return id == other.id
		    && topic == other.topic
		    && nbSigns == other.nbSigns
		    && nbImages == other.nbImages
		    && comment == other.comment;
	}
};

const ResaRedac kInvalidResaRedac;


/** Stub interface — used by leftover TCL command bodies in XPage.cpp /
	XRail.cpp. No implementation is registered, so all
	`InterfacePtr<IResaRedacDataList>` queries return nil; call sites
	must guard against that.
*/
class IResaRedacDataList : public IPMUnknown
{
public:
	enum { kDefaultIID = IID_IRESAREDACDATALIST };

	virtual int32     GetNbResaRedac() = 0;
	virtual ResaRedac GetNthResaRedac(int32 index, PMString& outIDForme) = 0;
	virtual PMString  GetIDForme(int32 idResa) = 0;
	virtual void      AddResaRedac(const PMString& idForme, const ResaRedac& resa) = 0;
	virtual void      RemoveResaRedac(const PMString& idForme) = 0;
	virtual void      CopyFrom(IResaRedacDataList* source) = 0;
};

#endif // __IResaRedacDataList_h__
