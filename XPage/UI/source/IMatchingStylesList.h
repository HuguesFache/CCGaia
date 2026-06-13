#ifndef __IMatchingStylesList_h__
#define __IMatchingStylesList_h__

#include "XPGUIID.h"
#include "K2Vector.h"
#include "K2Vector.tpp"

class IPMUnknown;

typedef struct matchingStyle {

	typedef object_type data_type; // Pour l'utiliser avec des K2Vector

	PMString styleName, styleTag; 

	matchingStyle(){
		styleName = kNullString;
		styleTag = kNullString;
	}
    inline bool operator ==  (const struct matchingStyle& style) const	{
		return ( this->styleName.Compare(kFalse, style.styleName) == 0);		
    }
		
} MatchingStyle;

class IMatchingStylesList : public IPMUnknown 
{

	public:	

		enum	{ kDefaultIID = IID_IMATCHINGSTYLESLIST };

		virtual void SetMatchingStylesList(const K2Vector<MatchingStyle>& newList) =0;

		virtual const K2Vector<MatchingStyle>& GetMatchingStylesList() const =0;			
};

#endif // __IMatchingStylesList_h__
