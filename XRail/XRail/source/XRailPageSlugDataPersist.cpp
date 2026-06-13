
#include "VCPluginHeaders.h"
 
// Interface includes:
#include "IPMStream.h"
 
// General includes:
#include "K2Vector.h"
#include "K2Vector.tpp"
#include "CPMUnknown.h"

// Project includes:
#include "XRLID.h"
#include "IXRailPageSlugData.h"

/**	XRailDataPersist
*/
class XRailDataPersist : public CPMUnknown<IXRailPageSlugData>
{
	public:
		XRailDataPersist(IPMUnknown* boss);

		virtual ~XRailDataPersist();
	
		virtual void SetID(const int32 id);

		virtual int32 GetID();

#if MULTIBASES == 1
        virtual void SetBaseName(const PMString base);

		virtual PMString GetBaseName();
#endif
		virtual void ReadWrite(IPMStream* s, ImplementationID prop);
		
	private:
		
		//K2Vector<int32> pageIDs;

		int32 XRailID;
#if MULTIBASES == 1
    PMString baseName;
#endif
		
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its 
 ImplementationID making the C++ code callable by the 
 application.
*/
CREATE_PERSIST_PMINTERFACE(XRailDataPersist, kXRLDataPersistImpl)

/* Constructor
*/
XRailDataPersist::XRailDataPersist(IPMUnknown* boss) : 	
		CPMUnknown<IXRailPageSlugData>(boss), XRailID(0)
{
}

/* Destructor.
*/
XRailDataPersist::~XRailDataPersist()
{
}


/* SetID
*/
void XRailDataPersist::SetID(const int32 id)
{
	PreDirty ();
	XRailID = id;	
}

/* GetID
*/
int32 XRailDataPersist::GetID()
{
	return XRailID;
}

#if MULTIBASES == 1
/* SetID
*/
void XRailDataPersist::SetBaseName(const PMString base)
{
	PreDirty ();
	baseName = base;	
}

/* GetID
*/
PMString XRailDataPersist::GetBaseName()
{
	return baseName;
}
#endif

void XRailDataPersist::ReadWrite(IPMStream* s, ImplementationID prop)
{
#if MULTIBASES == 1
	baseName.ReadWrite(s);
#endif
	s->XferInt32(XRailID);
}

// End, SnapPrefsDataPersist.cpp.


