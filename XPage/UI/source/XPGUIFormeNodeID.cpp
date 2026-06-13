
#include "VCPluginHeaders.h"

// Interface includes:
#include "IPMStream.h"
#include "IXPageUtils.h"

// Project includes:
#include "FileUtils.h"
#include "XPGUIFormeNodeID.h"
#include "CAlert.h"

/**
	Private constructors
*/

XPGUIFormeNodeID::XPGUIFormeNodeID(PMString id, PMString data, XPGFormeDataNode::NodeType paramtype, const XPGFormeDataNode::FormeData * paramformeData)
{
	this->fIDData = id;
	this->fData = data;
	this->type = paramtype;
	this->formeData = nil;
	if(paramformeData)
	{
		this->formeData = new XPGFormeDataNode::FormeData();
		this->formeData->name = paramformeData->name;
		this->formeData->classeurName = paramformeData->classeurName;
		this->formeData->formeFile = paramformeData->formeFile;
		this->formeData->matchingFile = paramformeData->matchingFile;
		this->formeData->posX = paramformeData->posX;
		this->formeData->posY = paramformeData->posY;
	}
}

/* Compare
*/
int32 XPGUIFormeNodeID::Compare(const NodeIDClass* nodeID) const
{
	const XPGUIFormeNodeID* fileNode = static_cast<const XPGUIFormeNodeID*>(nodeID);
	ASSERT(nodeID);
	
	const XPGUIFormeNodeID* thisNode = const_cast<const XPGUIFormeNodeID*>(this);
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
NodeIDClass* XPGUIFormeNodeID::Clone() const
{
	return new XPGUIFormeNodeID(this->GetID(), this->fData, this->type, this->formeData);
}


/* Read
*/
void XPGUIFormeNodeID::Read(IPMStream* stream)
{
	
	fIDData.ReadWrite(stream);
	fData.ReadWrite(stream);
	int16 typeIntValue = XPGFormeDataNode::kClasseurNode;
	stream->XferInt16(typeIntValue);
	type = (XPGFormeDataNode::NodeType) typeIntValue;
	bool16 formeDataExist;
	stream->XferBool(formeDataExist);
	if(formeDataExist){
		if(!formeData)
			formeData = new XPGFormeDataNode::FormeData();

		formeData->name.ReadWrite(stream);
		formeData->classeurName.ReadWrite(stream);		
		formeData->posX.ReadWrite(stream);		
		formeData->posY.ReadWrite(stream);		

		PMString formeFilePath, matchingFilePath;

		formeFilePath.ReadWrite(stream);
		matchingFilePath.ReadWrite(stream);
		formeData->formeFile = FileUtils::PMStringToSysFile(formeFilePath);
		formeData->matchingFile = FileUtils::PMStringToSysFile(matchingFilePath);
	}
	else
	{
		if(formeData)
			delete formeData;
		formeData = nil;
	}
}


/* Write
*/
void XPGUIFormeNodeID::Write(IPMStream* stream) const
{
	(const_cast<XPGUIFormeNodeID*>(this)->fIDData).ReadWrite(stream);
	(const_cast<XPGUIFormeNodeID*>(this)->fData).ReadWrite(stream);
	int16 typeIntValue = type;
	stream->XferInt16(typeIntValue);
	bool16 formeDataExist = (formeData != nil);
	if(formeDataExist)
	{
		stream->XferBool(formeDataExist);
		formeData->name.ReadWrite(stream);
		formeData->classeurName.ReadWrite(stream);

		FileUtils::SysFileToPMString(formeData->formeFile).ReadWrite(stream);
		FileUtils::SysFileToPMString(formeData->matchingFile).ReadWrite(stream);		
	}
	else
		stream->XferBool(formeDataExist);
}


