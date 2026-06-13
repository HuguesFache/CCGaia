
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IStringListData.h"
#include "IPMStream.h"

// General includes:
#include "XPGID.h"
#include "K2Vector.h"

class PersistStringListData : public CPMUnknown<IStringListData>
{
public:

	/** Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.*/
	PersistStringListData(IPMUnknown* boss);

	void SetStringList(const K2Vector<PMString>& newList);

	const K2Vector<PMString>& GetStringList();

	virtual void ReadWrite  (IPMStream *  s, ImplementationID  prop);
 
	void Add(const PMString &inString);

	bool16 Contains(const PMString& inString);

	void Clear(void);

private:

	K2Vector<PMString> stringList;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PERSIST_PMINTERFACE(PersistStringListData, kPersistStringListDataImpl)

/* Constructor
*/
PersistStringListData::PersistStringListData(IPMUnknown* boss) : CPMUnknown<IStringListData>(boss)	
{	
}

void PersistStringListData::SetStringList(const K2Vector<PMString>& newList)
{
		PreDirty();
		stringList = newList;
	
}

const K2Vector<PMString>&  PersistStringListData::GetStringList()
{
	return stringList;
}

void PersistStringListData::ReadWrite(IPMStream *  s, ImplementationID  prop)
{
	if(s->IsReading())
	{
		int32 nbString = 0;
		s->XferInt32(nbString);

		for(int32 i = 0 ; i < nbString ; ++i)
		{
			PMString string;	
			string.ReadWrite(s);

			stringList.push_back(string);
		}
	}
	else
	{
		int32 nbString = stringList.size();
		s->XferInt32(nbString);
		for(int32 i = 0 ; i < nbString ; ++i)
		{
			stringList[i].ReadWrite(s);
		}
	}
}


void PersistStringListData::Add(const PMString &inString) {

}

bool16 PersistStringListData::Contains(const PMString& inString) {
	return false;
}

void PersistStringListData::Clear(void) {

}