
#include "VCPluginHeaders.h"

// Interface includes

// Other API includes
#include "K2Vector.tpp"

// Project includes
#include "XPGImageDataNode.h"

/**
	@ingroup paneltreeview
*/

/* Constructor
*/
XPGImageDataNode::XPGImageDataNode(NodeType paramtype) : fParent(nil), fIDData(kNullString), fData(kNullString), imageData(nil)
{
	this->type = paramtype;
	if(type != kRubriqueNode)
	{
		imageData = new ImageData();
		imageData->name = kNullString;
		imageData->articleID = kNullString;
		imageData->legend = kNullString;
		imageData->credit = kNullString;
		imageData->type = kNullString;
		imageData->brFile = kNullString;
		imageData->hrFileNB = kNullString;
	}
}


/* Destructor
*/
XPGImageDataNode::~XPGImageDataNode()
{
	if(type != kRubriqueNode)
	{
		if(imageData)
			delete imageData;
	}
}


/* Copy constructor
*/
XPGImageDataNode::XPGImageDataNode(const XPGImageDataNode& rhs) 
: fParent(nil), fIDData(kNullString), imageData(nil)
{
	deepcopy(rhs);
}

/* Overloaded =
*/
XPGImageDataNode& XPGImageDataNode::operator=(const XPGImageDataNode& rhs)
{
	deepcopy(rhs);
	return *this;
}


/* Overloaded ==
*/
bool XPGImageDataNode::operator==(const XPGImageDataNode& rhs) const
{
	if(this->type == rhs.type)
	{
		bool16 equal = this->fParent == rhs.fParent && 
					this->fIDData == rhs.fIDData &&
					this->fData == rhs.fData &&
					this->fChildren == rhs.fChildren;
		if(imageData)
			return	equal &&
				imageData->name == rhs.imageData->name &&
				imageData->articleID == rhs.imageData->articleID &&
				imageData->type == rhs.imageData->type &&
				imageData->brFile == rhs.imageData->brFile &&
				imageData->legend == rhs.imageData->legend &&
				imageData->credit == rhs.imageData->credit &&
				imageData->hrFileNB == rhs.imageData->hrFileNB &&
				// New OBJRART-sourced fields. Crop / focal compared as
				// PMReal — the model only ever stores parsed numeric values
				// so equality is meaningful here.
				imageData->comment == rhs.imageData->comment &&
				imageData->hrFile == rhs.imageData->hrFile &&
				imageData->cropH == rhs.imageData->cropH &&
				imageData->cropW == rhs.imageData->cropW &&
				imageData->cropX == rhs.imageData->cropX &&
				imageData->cropY == rhs.imageData->cropY &&
				imageData->focalX == rhs.imageData->focalX &&
				imageData->focalY == rhs.imageData->focalY &&
				imageData->etatImage == rhs.imageData->etatImage;
		else
			return equal;
	}
	else
		return kFalse;
}


/* deepcopy
*/
void XPGImageDataNode::deepcopy(const XPGImageDataNode& rhs)
{
	if(imageData)
		delete imageData;
	
	this->fChildren.clear();
	this->fChildren.assign( rhs.fChildren.begin(), rhs.fChildren.end());
	this->fParent = rhs.fParent;
	this->fIDData = rhs.fIDData;
	this->type = rhs.type;
	this->fData = rhs.fData;
	
	if(this->type != kRubriqueNode)
	{
		imageData = new ImageData();
		imageData->name = rhs.imageData->name;
		imageData->articleID = rhs.imageData->articleID;
		imageData->legend = rhs.imageData->legend;
		imageData->credit = rhs.imageData->credit;
		imageData->type = rhs.imageData->type;
		imageData->brFile = rhs.imageData->brFile;
		imageData->hrFileNB = rhs.imageData->hrFileNB;
		// OBJRART-sourced fields.
		imageData->comment = rhs.imageData->comment;
		imageData->hrFile = rhs.imageData->hrFile;
		imageData->cropH = rhs.imageData->cropH;
		imageData->cropW = rhs.imageData->cropW;
		imageData->cropX = rhs.imageData->cropX;
		imageData->cropY = rhs.imageData->cropY;
		imageData->focalX = rhs.imageData->focalX;
		imageData->focalY = rhs.imageData->focalY;
		imageData->etatImage = rhs.imageData->etatImage;
	}
}


/* GetNthChild
*/
const XPGImageDataNode& XPGImageDataNode::GetNthChild(
	int32 indexInParent) const
{
	return this->fChildren.at(indexInParent);
}


/* AddChild
*/
void XPGImageDataNode::AddChild(const XPGImageDataNode& o)
{
	this->fChildren.push_back(o);
}


/* RemoveChild
*/
void XPGImageDataNode::RemoveChild(const XPGImageDataNode& o)
{
	K2Vector<XPGImageDataNode>::iterator result = std::find(fChildren.begin(), 
													fChildren.end(), o);

	if(result != fChildren.end())
	{
		fChildren.erase(result);
	}
}


/* GetParent
*/
XPGImageDataNode* XPGImageDataNode::GetParent() const
{
	return this->fParent;
}


/* SetParent
*/
void XPGImageDataNode::SetParent(const XPGImageDataNode* p)
{
	this->fParent = const_cast<XPGImageDataNode* >(p);
}


/* ChildCount
*/
int32 XPGImageDataNode::ChildCount() const
{
	return this->fChildren.size();
}


/* SetID
*/
void XPGImageDataNode::SetID(const PMString& id)
{
	this->fIDData = id;
}


/* GetID
*/
PMString XPGImageDataNode::GetID() const
{
	return this->fIDData;
}

PMString XPGImageDataNode::GetData() const
{
	return this->fData;
}

void XPGImageDataNode::SetData(const PMString& data)
{
	this->fData = data;
}

XPGImageDataNode::NodeType XPGImageDataNode::GetType() const
{
	return this->type;
}

void XPGImageDataNode::SetImageData(const PMString& name, const PMString& articleID, const PMString& legend,
									 const PMString& credit, const PMString& paramtype, const PMString& brFile,
									 const PMString& hrFileNB)
{
	if(imageData)
	{
		imageData->name = name;
		imageData->articleID = articleID;
		imageData->legend = legend;
		imageData->credit = credit;
		imageData->type = paramtype;
		imageData->brFile = brFile;
		imageData->hrFileNB = hrFileNB;
	}
}


/* SetImageData (full struct overload)
 *
 * Used by the OBJRART SAX handler: copies every field including the new
 * OBJRART-sourced ones (comment, hrFile, crop / focal values).
*/
void XPGImageDataNode::SetImageData(const ImageData& data)
{
	if (imageData == nil)
		return;
	*imageData = data;
}

const XPGImageDataNode::ImageData * XPGImageDataNode::GetImageData() const
{
	if(type == kRubriqueNode)
		return nil;
	else
		return imageData;
}
//	end, File:	XPGImageDataNode.cpp
