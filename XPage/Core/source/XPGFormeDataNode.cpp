
#include "VCPluginHeaders.h"

// Interface includes

// Other API includes
#include "K2Vector.tpp"

// Project includes
#include "XPGFormeDataNode.h"

/**
	@ingroup paneltreeview
*/

/* Constructor
*/
XPGFormeDataNode::XPGFormeDataNode(NodeType paramtype) :
	fParent(nil),
	fIDData(kNullString), 
	fData(kNullString), 
	formeData(nil)
{
	this->type = paramtype;

	if(type != kClasseurNode){

		formeData = new FormeData();
		formeData->name = kNullString;
		formeData->classeurName = kNullString;
		formeData->posX = kNullString;
		formeData->posY = kNullString;
		formeData->matchingFile = IDFile();
		formeData->formeFile = IDFile();
	}
}


/* Destructor
*/
XPGFormeDataNode::~XPGFormeDataNode()
{
	if(type != kClasseurNode)
	{
		if(formeData)
			delete formeData;
	}
}


/* Copy constructor
*/
XPGFormeDataNode::XPGFormeDataNode(const XPGFormeDataNode& rhs) 
: fParent(nil), fIDData(kNullString), formeData(nil)
{
	deepcopy(rhs);
}

/* Overloaded =
*/
XPGFormeDataNode& XPGFormeDataNode::operator=(const XPGFormeDataNode& rhs)
{
	deepcopy(rhs);
	return *this;
}


/* Overloaded ==
*/
bool XPGFormeDataNode::operator==(const XPGFormeDataNode& rhs) const
{
	if(this->type == rhs.type)
	{
		bool16 equal = this->fParent == rhs.fParent && 
					this->fIDData == rhs.fIDData &&
					this->fData == rhs.fData &&
					this->fChildren == rhs.fChildren;
		if(formeData)
			return	equal &&
				   formeData->name == rhs.formeData->name
				&& formeData->classeurName == rhs.formeData->classeurName
					&& formeData->posX == rhs.formeData->posX
					&& formeData->posY == rhs.formeData->posY
				&& formeData->matchingFile == rhs.formeData->matchingFile
				&& formeData->formeFile == rhs.formeData->formeFile
				&& formeData->isPhotoCarton == rhs.formeData->isPhotoCarton;
		else
			return equal;
	}
	else
		return kFalse;
}


/* deepcopy
*/
void XPGFormeDataNode::deepcopy(const XPGFormeDataNode& rhs)
{
	if(formeData)
		delete formeData;
	
	this->fChildren.clear();
	this->fChildren.assign( rhs.fChildren.begin(), rhs.fChildren.end());
	this->fParent = rhs.fParent;
	this->fIDData = rhs.fIDData;
	this->type = rhs.type;
	this->fData = rhs.fData;
	
	if(this->type != kClasseurNode)
	{
		formeData = new FormeData();
		formeData->name = rhs.formeData->name;
		formeData->classeurName = rhs.formeData->classeurName;
		formeData->posX = rhs.formeData->posX;
		formeData->posY = rhs.formeData->posY;
		formeData->matchingFile = rhs.formeData->matchingFile;
		formeData->formeFile = rhs.formeData->formeFile;
		formeData->isPhotoCarton = rhs.formeData->isPhotoCarton;
	}
}


/* GetNthChild
*/
const XPGFormeDataNode& XPGFormeDataNode::GetNthChild(
	int32 indexInParent) const
{
	return this->fChildren.at(indexInParent);
}


/* AddChild
*/
void XPGFormeDataNode::AddChild(const XPGFormeDataNode& o)
{
	this->fChildren.push_back(o);
}


/* RemoveChild
*/
void XPGFormeDataNode::RemoveChild(const XPGFormeDataNode& o)
{
	K2Vector<XPGFormeDataNode>::iterator result = std::find(fChildren.begin(), 
													fChildren.end(), o);

	if(result != fChildren.end())
	{
		fChildren.erase(result);
	}
}


/* GetParent
*/
XPGFormeDataNode* XPGFormeDataNode::GetParent() const
{
	return this->fParent;
}


/* SetParent
*/
void XPGFormeDataNode::SetParent(const XPGFormeDataNode* p)
{
	this->fParent = const_cast<XPGFormeDataNode* >(p);
}


/* ChildCount
*/
int32 XPGFormeDataNode::ChildCount() const
{
	return this->fChildren.size();
}


/* SetID
*/
void XPGFormeDataNode::SetID(const PMString& id)
{
	this->fIDData = id;
}


/* GetID
*/
PMString XPGFormeDataNode::GetID() const
{
	return this->fIDData;
}

PMString XPGFormeDataNode::GetData() const
{
	return this->fData;
}

void XPGFormeDataNode::SetData(const PMString& data)
{
	this->fData = data;
}

XPGFormeDataNode::NodeType XPGFormeDataNode::GetType() const
{
	return this->type;
}

void XPGFormeDataNode::SetFormeData(const PMString& name, const PMString& classeurName, const IDFile& matchingFile, const IDFile& formeFile, const PMString& posX, const PMString& posY, const bool16 isPhotoCarton)
{
	if(formeData)
	{
		formeData->name = name;
		formeData->classeurName = classeurName;
		formeData->posX = posX;
		formeData->posY = posY;
		formeData->matchingFile = matchingFile;
		formeData->formeFile = formeFile;
		formeData->isPhotoCarton = isPhotoCarton;
	}
}

const XPGFormeDataNode::FormeData * XPGFormeDataNode::GetFormeData() const
{
	if(type == kClasseurNode)
		return nil;
	else
		return formeData;
}
//	end, File:	XPGFormeDataNode.cpp
