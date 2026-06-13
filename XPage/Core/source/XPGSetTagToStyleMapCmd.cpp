
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IXPGTagToStyleMap.h"
#include "ITextModel.h"
#include "IStyleGroupManager.h"
#include "IStyleGroupHierarchy.h"
#include "IStyleInfo.h"
#include "IAttributeStrand.h"

// General includes:
#include "CmdUtils.h"
#include "Command.h"
#include "ErrorUtils.h"
#include "CAlert.h"
#include "XMLID.h"
#include "XPGID.h"

/** Implementation of command to set tag to style map in spline item
*/
class XPGSetTagToStyleMapCmd : public Command
{
public:
	/** Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.*/
	XPGSetTagToStyleMapCmd(IPMUnknown* boss);

	/** Destructor
	*/
	~XPGSetTagToStyleMapCmd();

protected:

	/** Do
	 	@see Command::Do
	*/
	virtual void Do();	
		
	/** CreateName
	 	@see Command::CreateName
	*/
	virtual PMString* CreateName();

private:

	UIDList GetUsedParagraphStyles(UIDRef& textStory);

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGSetTagToStyleMapCmd, kXPGSetTagToStyleMapCmdImpl)

/* Constructor
*/
XPGSetTagToStyleMapCmd::XPGSetTagToStyleMapCmd(IPMUnknown* boss) : Command(boss)	
{	
}

/** Destructor
*/
XPGSetTagToStyleMapCmd::~XPGSetTagToStyleMapCmd()
{
}

/* Do
*/
void XPGSetTagToStyleMapCmd::Do(){

	ErrorCode status = kFailure;
	do{
		if(fItemList.Length() == 0)
			break;

		IDataBase * db = fItemList.GetDataBase();
		if(!db)
			break;
		
		InterfacePtr<IXPGTagToStyleMap> cmdData (this, UseDefaultIID());	

		for(int32 i=0; i<fItemList.Length(); ++i){
			InterfacePtr<IXPGTagToStyleMap> storyMap (fItemList.GetRef(i), UseDefaultIID());
			if(!storyMap)
				continue;

			UIDRef itemRef = fItemList.GetRef(i);
			InterfacePtr<ITextModel> isTextModel (itemRef, UseDefaultIID());
			if(isTextModel){			
				UIDList storyParaStyles = this->GetUsedParagraphStyles(itemRef);
				int32 tagToStyleMapCount = cmdData->GetMappingCount();
				for(int32 j = 0; j < tagToStyleMapCount; j++){
					PMString tagName = cmdData->GetTagAt(j);
					UID paraStyleMappedUID = cmdData->GetParaStyleMappedToTag(tagName);
					if(storyParaStyles.Contains(paraStyleMappedUID))
						storyMap->AddTagToParaStyleMapping(tagName, paraStyleMappedUID);						
				}
			}						
		}
		status = kSuccess;

	} while (kFalse);       

	// Handle any errors
	if (status != kSuccess) 
		ErrorUtils::PMSetGlobalErrorCode(status);
	
}


/* CreateName
*/
PMString* XPGSetTagToStyleMapCmd::CreateName(){
	return new PMString(kSetTagToStyleMapCmdKey);
}



UIDList XPGSetTagToStyleMapCmd::GetUsedParagraphStyles(UIDRef& textStory)
{
	UIDList styles(textStory.GetDataBase());

	do
	{
		InterfacePtr<ITextModel> txtModel (textStory, UseDefaultIID());
		if(!txtModel)
			break;

		InterfacePtr<IAttributeStrand> paraStrand ((IAttributeStrand*) txtModel->QueryStrand(kParaAttrStrandBoss,IID_IATTRIBUTESTRAND));

		TextIndex pos = 0;
		int32 totalLength = txtModel->TotalLength() - 1;
		while(pos < totalLength)
		{
			int32 count = 0;
			UID paraStyle = paraStrand->GetStyleUID(pos, &count);

			if(styles.DoesNotContain(paraStyle))
				styles.Append(paraStyle);

			pos += count;
		}

	} while(kFalse);

	return styles;
}

// End, XPGSetTagToStyleMapCmd.cpp.


