
#include "VCPluginHeaders.h"

// Interface includes:
#include "IPMStream.h"
#include "IXPageUtils.h"

// Project includes:
#include "FileUtils.h"
#include "XPGUIStyleMatchingNodeID.h"
#include "CAlert.h"

/**
	Private constructors
*/

XPGUIStyleMatchingNodeID::XPGUIStyleMatchingNodeID(PMString id, PMString data, XPGMatchingStyleDataNode::NodeType paramtype, const XPGMatchingStyleDataNode::MatchingStyleData * parammatchingStyleData)
{
	this->fIDData = id;
	this->fData = data;
	this->type = paramtype;
	this->matchingStyleData = nil;
	if(parammatchingStyleData){
		this->matchingStyleData = new XPGMatchingStyleDataNode::MatchingStyleData();
		this->matchingStyleData->name = parammatchingStyleData->name;
		this->matchingStyleData->styleTag = parammatchingStyleData->styleTag;
	}
}

/* Compare
*/
int32 XPGUIStyleMatchingNodeID::Compare(const NodeIDClass* nodeID) const
{
	const XPGUIStyleMatchingNodeID* fileNode = static_cast<const XPGUIStyleMatchingNodeID*>(nodeID);
	ASSERT(nodeID);
	
	const XPGUIStyleMatchingNodeID* thisNode = const_cast<const XPGUIStyleMatchingNodeID*>(this);
	if(thisNode->type == fileNode->GetType())
	{
		return thisNode->fIDData.Compare(kFalse, fileNode->GetID());
	}
	else
	{
		if(this->type < fileNode->GetType())
			return -1;
		else
			return 1;
	}
}


/* Clone
*/
NodeIDClass* XPGUIStyleMatchingNodeID::Clone() const
{
	return new XPGUIStyleMatchingNodeID(this->GetID(), this->fData, this->type, this->matchingStyleData);
}


/* Read
*/
void XPGUIStyleMatchingNodeID::Read(IPMStream* stream)
{
	
	fIDData.ReadWrite(stream);
	fData.ReadWrite(stream);
	int16 typeIntValue = XPGMatchingStyleDataNode::kMatchingStyleNode;
	stream->XferInt16(typeIntValue);
	type = (XPGMatchingStyleDataNode::NodeType) typeIntValue;
	bool16 matchingStyleDataExist;
	stream->XferBool(matchingStyleDataExist);
	if(matchingStyleDataExist){
		if(!matchingStyleData)
			matchingStyleData = new XPGMatchingStyleDataNode::MatchingStyleData();

		matchingStyleData->name.ReadWrite(stream);
		matchingStyleData->styleTag.ReadWrite(stream);
	
	}
	else
	{
		if(matchingStyleData)
			delete matchingStyleData;
		matchingStyleData = nil;
	}
}


/* Write
*/
void XPGUIStyleMatchingNodeID::Write(IPMStream* stream) const
{
	(const_cast<XPGUIStyleMatchingNodeID*>(this)->fIDData).ReadWrite(stream);
	(const_cast<XPGUIStyleMatchingNodeID*>(this)->fData).ReadWrite(stream);
	int16 typeIntValue = type;
	stream->XferInt16(typeIntValue);
	bool16 matchingStyleDataExist = (matchingStyleData != nil);
	if(matchingStyleDataExist)
	{
		stream->XferBool(matchingStyleDataExist);
		matchingStyleData->name.ReadWrite(stream);
		matchingStyleData->styleTag.ReadWrite(stream);

	}
	else
		stream->XferBool(matchingStyleDataExist);
}


