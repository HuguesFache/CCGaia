
#include "VCPluginHeaders.h"

// Interface includes:
#include "IPMStream.h"

// Project includes:
#include "XPGUIImageNodeID.h"

/**
	Private constructors
*/

XPGUIImageNodeID::XPGUIImageNodeID(PMString id, PMString data, XPGImageDataNode::NodeType paramtype, const XPGImageDataNode::ImageData * dataImage)
{
	this->fIDData = id;
	this->fData = data;
	this->type = paramtype;
	this->imageData = nil;
	if(dataImage)
	{
		this->imageData = new XPGImageDataNode::ImageData();
		this->imageData->name = dataImage->name;
		this->imageData->articleID = dataImage->articleID;
		this->imageData->legend = dataImage->legend;
		this->imageData->credit = dataImage->credit;
		this->imageData->type = dataImage->type;
		this->imageData->brFile = dataImage->brFile;
		this->imageData->hrFileNB = dataImage->hrFileNB;
		// OBJRART-sourced fields. These were silently dropped before:
		// the model parsed them out of <article>.OBJRART.xml but the tree
		// view nodeID copy lost them, so the panel observer always read
		// empty strings / zeroes. (Notably this is why the Commentaire
		// edit box stayed blank even when the XML carried a value.)
		this->imageData->comment = dataImage->comment;
		this->imageData->hrFile = dataImage->hrFile;
		this->imageData->cropH = dataImage->cropH;
		this->imageData->cropW = dataImage->cropW;
		this->imageData->cropX = dataImage->cropX;
		this->imageData->cropY = dataImage->cropY;
		this->imageData->focalX = dataImage->focalX;
		this->imageData->focalY = dataImage->focalY;
		this->imageData->etatImage = dataImage->etatImage;
	}
}

/* Compare
*/
int32 XPGUIImageNodeID::Compare(const NodeIDClass* nodeID) const
{
	const XPGUIImageNodeID* fileNode = static_cast<const XPGUIImageNodeID*>(nodeID);
	ASSERT(nodeID);
	
	const XPGUIImageNodeID* thisNode = const_cast<const XPGUIImageNodeID*>(this);
	if(thisNode->type == fileNode->GetType())
	{
		int32 compareResult = thisNode->fIDData.Compare(kFalse, fileNode->GetID());

		if(compareResult != 0)
			return compareResult;

		else if(imageData)
		{
			bool equal = thisNode->imageData->name == fileNode->imageData->name &&
						thisNode->imageData->articleID == fileNode->imageData->articleID &&
						thisNode->imageData->legend == fileNode->imageData->legend &&
						thisNode->imageData->credit == fileNode->imageData->credit &&
						thisNode->imageData->type == fileNode->imageData->type &&
						thisNode->imageData->brFile == fileNode->imageData->brFile &&
						thisNode->imageData->hrFileNB == fileNode->imageData->hrFileNB;

			if(equal)
				return 0;
			else
				return -1;
		}
		else
			return 0;
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
NodeIDClass* XPGUIImageNodeID::Clone() const
{
	return new XPGUIImageNodeID(this->GetID(), this->fData, this->type, this->imageData);
}


/* Read
*/
void XPGUIImageNodeID::Read(IPMStream* stream)
{
	fIDData.ReadWrite(stream);
	fData.ReadWrite(stream);
	int16 typeIntValue = XPGImageDataNode::kRubriqueNode;
	stream->XferInt16(typeIntValue);
	type = (XPGImageDataNode::NodeType) typeIntValue;
	bool16 imageDataExist;
	stream->XferBool(imageDataExist);
	if(imageDataExist)
	{
		if(!imageData)
			imageData = new XPGImageDataNode::ImageData();

		imageData->name.ReadWrite(stream);
		imageData->articleID.ReadWrite(stream);
		imageData->legend.ReadWrite(stream);
		imageData->credit.ReadWrite(stream);
		imageData->type.ReadWrite(stream);
		imageData->brFile.ReadWrite(stream);
		imageData->hrFileNB.ReadWrite(stream);
	}
	else
	{
		if(imageData)
			delete imageData;
		imageData = nil;
	}
}


/* Write
*/
void XPGUIImageNodeID::Write(IPMStream* stream) const
{
	(const_cast<XPGUIImageNodeID*>(this)->fIDData).ReadWrite(stream);
	(const_cast<XPGUIImageNodeID*>(this)->fData).ReadWrite(stream);
	int16 typeIntValue = type;
	stream->XferInt16(typeIntValue);
	bool16 imageDataExist = (imageData != nil);
	if(imageDataExist)
	{
		stream->XferBool(imageDataExist);
		imageData->name.ReadWrite(stream);
		imageData->articleID.ReadWrite(stream);
		imageData->legend.ReadWrite(stream);
		imageData->credit.ReadWrite(stream);
		imageData->type.ReadWrite(stream);
		imageData->brFile.ReadWrite(stream);
		imageData->hrFileNB.ReadWrite(stream);
	}
	else
		stream->XferBool(imageDataExist);
}


