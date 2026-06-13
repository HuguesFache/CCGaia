
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IDocument.h"
#include "IAssignmentMgr.h"
#include "IAssignmentUtils.h"
#include "IAttributeStrand.h"
#include "ITextModel.h"
#include "IStyleGroupManager.h"
#include "IStyleGroupHierarchy.h"
#include "IStyleInfo.h"
#include "IUIDData.h"
#include "IStoryList.h"
#include "IInCopyDocUtils.h"
#include "IInCopyWorkFlowUtils.h"
#include "IDataLink.h"
#include "IHierarchy.h"
// General includes:
#include "CmdUtils.h"
#include "Command.h"
#include "ErrorUtils.h"
#include "XPGID.h"
#include "FileUtils.h"

/** Implementation of command to set tag to style map in spline item
*/
class XPGFilterStyleCmd : public Command
{
public:
	/** Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.*/
	XPGFilterStyleCmd(IPMUnknown* boss);

	/** Destructor
	*/
	~XPGFilterStyleCmd();

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
CREATE_PMINTERFACE(XPGFilterStyleCmd, kXPGFilterStyleCmdImpl)

/* Constructor
*/
XPGFilterStyleCmd::XPGFilterStyleCmd(IPMUnknown* boss) : Command(boss)	
{
	SetUndoability(ICommand::kAutoUndo);
}

/** Destructor
*/
XPGFilterStyleCmd::~XPGFilterStyleCmd()
{
}

/* Do
*/
void XPGFilterStyleCmd::Do()
{
	do                      
	{
		InterfacePtr<IAssignmentMgr> assignmentMgr (GetExecutionContextSession(), UseDefaultIID());		

		InterfacePtr<IDocument> theDoc (fItemList.GetRef(0), IID_IDOCUMENT);		
		
		// Check whether it's is a saved assignment
		IDataBase * db = fItemList.GetDataBase();
		if(!db->GetSysFile())
				break;

		IDFile assignFile = *db->GetSysFile();

		IAssignedDocument * assignDoc = assignmentMgr->GetAssignedDocument(db);
		if(!assignDoc)
			break;
		
		InterfacePtr<IAssignment> assignment (assignmentMgr->QueryAssignment(assignDoc, FileUtils::SysFileToPMString(assignFile)));
		if(assignment == nil)
			break;

		const UIDList storyList = assignment->GetStories();
		UIDList usedParaStyles(db);

		for(int32 i = 0 ; i < storyList.size() ; ++i)			
		{			
			InterfacePtr<IAssignedStory> assignedStory (storyList.GetRef(i), UseDefaultIID());		

			// Check whether the story's page item is a text frame
			InterfacePtr<ITextModel> isTextModel (assignedStory->GetUIDRef(), UseDefaultIID());
			
			if(isTextModel)
			{
				// Gather paragraph styles used in this story
				UIDRef textRef = assignedStory->GetUIDRef();
				UIDList storyParaStyles = this->GetUsedParagraphStyles(textRef);

				int32 nbStyles = storyParaStyles.Length();
				for(int32 j = 0 ; j < nbStyles ; ++j)
				{
					if(usedParaStyles.DoesNotContain(storyParaStyles[j]))
						usedParaStyles.Append(storyParaStyles[j]);
				}								
			}			
		}
		
		if(usedParaStyles.IsEmpty())
			break;

		// Clean paragraph style table so that there ara only left paragraph styles used in stories
		InterfacePtr<IStyleGroupManager> paraStyleTable (theDoc->GetDocWorkSpace(), IID_IPARASTYLEGROUPMANAGER);
		UIDList styleUIDs(db);
		paraStyleTable->GetRootHierarchy()->GetDescendents(&styleUIDs,IID_ISTYLEINFO);
		
		int32 nbStyles = styleUIDs.Length();
		for(int32 i = 0 ; i < nbStyles ; ++i)
		{
			UID styleUID = styleUIDs[i];
			if (styleUID == paraStyleTable->GetRootStyleUID())	// deleting the root style is illegal.	  		
				continue;
			
			// deleting the basic paragraph style is also a bad thing.
			InterfacePtr<IStyleInfo> iStyleInfo(db, styleUID, UseDefaultIID());
			PMString nameToDelete(iStyleInfo->GetName());
			if (nameToDelete == "NormalParagraphStyle")					
				continue;				

			if(usedParaStyles.DoesNotContain(styleUID))
			{
				InterfacePtr<ICommand> deleteStyleCmd(CmdUtils::CreateCommand(kDeleteParaStyleCmdBoss));						
				deleteStyleCmd->SetItemList(UIDList(theDoc->GetDocWorkSpace()));

				InterfacePtr<IUIDData> uidData(deleteStyleCmd, UseDefaultIID());
				uidData->Set(db, styleUID);
				
				InterfacePtr<IUIDData> uidReplaceData(deleteStyleCmd, IID_IREPLACEUIDDATA);
				uidReplaceData->Set(db, paraStyleTable->GetRootStyleUID());

				CmdUtils::ScheduleCommand(deleteStyleCmd);
			}
		}			

	} while (kFalse);       
}



/* CreateName
*/
PMString* XPGFilterStyleCmd::CreateName()
{
	return new PMString("Filter Style");
}

/* GetUsedParagraphStyles
*/
UIDList XPGFilterStyleCmd::GetUsedParagraphStyles(UIDRef& textStory)
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

// End, XPGFilterStyleCmd.cpp.


