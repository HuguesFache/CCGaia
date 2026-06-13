#include "VCPluginHeaders.h"

// Interface includes:
#include "IKeyValueData.h"
#include "CAlert.h"

// Project includes:
#include "XPGID.h"

/** 
*/
class XPGKeyValueData : public CPMUnknown<IKeyValueData>
{
public:


	/**	Constructor
		@param boss 
	 */
    XPGKeyValueData(IPMUnknown* boss );

	/**	Destructor
	 */
	virtual ~XPGKeyValueData();

	virtual void SetKeyValueList(const K2Vector<IDValuePair> & keyValueList);

	virtual K2Vector<IDValuePair> GetKeyValueList() const;

private:

	K2Vector<IDValuePair> dataList;

};

CREATE_PMINTERFACE(XPGKeyValueData, kXPGKeyValueDataImpl)

/* Constructor
*/
XPGKeyValueData::XPGKeyValueData(IPMUnknown *boss) : CPMUnknown<IKeyValueData>(boss)
{
	dataList.clear();
}

/* Destructor
*/
XPGKeyValueData::~XPGKeyValueData(){}

void XPGKeyValueData::SetKeyValueList(const K2Vector<IDValuePair> & keyValueList)
{
	dataList = keyValueList;
}

K2Vector<IDValuePair> XPGKeyValueData::GetKeyValueList() const
{
	return dataList;
}
