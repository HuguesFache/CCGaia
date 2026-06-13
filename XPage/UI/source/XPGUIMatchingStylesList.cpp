#include "VCPlugInHeaders.h"

#include "CPMUnknown.h"
#include "IPMStream.h"
#include "IMatchingStylesList.h"
#include "XPGUIID.h"


class XPGUIMatchingStylesList : public CPMUnknown<IMatchingStylesList>
{
public:

	XPGUIMatchingStylesList(IPMUnknown* boss);		

	~XPGUIMatchingStylesList();	
	
	virtual void SetMatchingStylesList(const K2Vector<MatchingStyle>& newList);	
	virtual const K2Vector<MatchingStyle>& GetMatchingStylesList() const;	

private:
	
	K2Vector<MatchingStyle> matchingStyleList;
};

CREATE_PMINTERFACE(XPGUIMatchingStylesList, kXPGUIMatchingStylesListImpl)


XPGUIMatchingStylesList::XPGUIMatchingStylesList(IPMUnknown* boss) :CPMUnknown<IMatchingStylesList>(boss)
{
	matchingStyleList.clear();
}

/* Destructor
*/
XPGUIMatchingStylesList::~XPGUIMatchingStylesList(){}

void XPGUIMatchingStylesList::SetMatchingStylesList(const K2Vector<MatchingStyle>& newList){	
	matchingStyleList = newList;	
}

const K2Vector<MatchingStyle>& XPGUIMatchingStylesList::GetMatchingStylesList() const{
	return matchingStyleList;
}
