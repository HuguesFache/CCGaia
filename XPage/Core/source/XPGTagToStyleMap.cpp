
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IXPGTagToStyleMap.h"
#include "IPMStream.h"

// General includes:
#include "XPGID.h"
#include "K2Vector.h"
#include "K2Vector.tpp"
#include "KeyValuePair.h"
#include "CAlert.h"


typedef K2Vector<KeyValuePair<PMString, UID> > TagToStyle;

typedef K2Vector<KeyValuePair<PMString, TagToStyle> > Tag2CharStyleMaps;

class XPGTagToStyleMap : public CPMUnknown<IXPGTagToStyleMap>
{
public:

	/** Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.*/
	XPGTagToStyleMap(IPMUnknown* boss);

	virtual void Copy(IXPGTagToStyleMap * otherMap);
	virtual void ClearMap();
	virtual int32 GetMappingCount() const;
	virtual void AddTagToParaStyleMapping(const PMString& tag, UID paraStyle);
	virtual PMString GetTagAt(int32 pos) const;
	virtual UID GetParaStyleMappedToTag(const PMString& tag) const;
	virtual void ReadWrite  (IPMStream *  s, ImplementationID  prop);
 
private:

	TagToStyle tag2ParaStyleMap;
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PERSIST_PMINTERFACE(XPGTagToStyleMap, kXPGTagToStyleMapImpl)

/* Constructor
*/
XPGTagToStyleMap::XPGTagToStyleMap(IPMUnknown* boss) : CPMUnknown<IXPGTagToStyleMap>(boss){}

void XPGTagToStyleMap::Copy(IXPGTagToStyleMap * otherMap){
	ClearMap();
	int32 nbTags = otherMap->GetMappingCount();	
	for(int32 i = 0 ; i < nbTags ; ++i){
		PMString tag = otherMap->GetTagAt(i);
		UID style = otherMap->GetParaStyleMappedToTag(tag);

		tag2ParaStyleMap.push_back(KeyValuePair<PMString, UID>(tag, style));
	}
}

void XPGTagToStyleMap::ClearMap(){
	tag2ParaStyleMap.clear();
}

int32 XPGTagToStyleMap::GetMappingCount() const{
	return tag2ParaStyleMap.size();
}

void XPGTagToStyleMap::AddTagToParaStyleMapping(const PMString& tag, UID paraStyle){
	PreDirty();	
	::InsertOrReplaceKeyValue(tag2ParaStyleMap, tag, paraStyle);
}

PMString XPGTagToStyleMap::GetTagAt(int32 pos) const
{
	if(pos >= 0 && pos < tag2ParaStyleMap.size())
		return tag2ParaStyleMap[pos].Key();
	else
		return kNullString;
}

UID XPGTagToStyleMap::GetParaStyleMappedToTag(const PMString& tag) const
{
	int32 pos = ::FindLocation(tag2ParaStyleMap, tag);
	if(pos != -1)
		return tag2ParaStyleMap[pos].Value();
	else
		return kInvalidUID;
}

void XPGTagToStyleMap::ReadWrite(IPMStream *  s, ImplementationID  prop){
	if(s->IsReading()){
		ClearMap();

		int32 nbTags = 0;
		s->XferInt32(nbTags);		
		tag2ParaStyleMap.reserve(nbTags);

		for(int32 i = 0 ; i < nbTags ; ++i){

			PMString tag;
			UID style;		

			tag.ReadWrite(s);
			s->XferReference(style);
			tag2ParaStyleMap.push_back(KeyValuePair<PMString, UID>(tag, style));			
			/*int32 nbCharStyles = 0;
			s->XferInt32(nbCharStyles);
			TagToStyle charStyleMap (nbCharStyles);
			for(int32 j = 0 ; j < nbCharStyles ; ++j){
				tag.ReadWrite(s);
				s->XferReference(style);
				charStyleMap.push_back(KeyValuePair<PMString, UID>(tag, style));
			}*/
		}
	}
	else
	{
		int32 nbTags = tag2ParaStyleMap.size();
		s->XferInt32(nbTags);
		for(int32 i = 0 ; i < nbTags ; ++i)
		{
			tag2ParaStyleMap[i].Key().ReadWrite(s);
			s->XferReference(tag2ParaStyleMap[i].Value());
		}
	}
}
