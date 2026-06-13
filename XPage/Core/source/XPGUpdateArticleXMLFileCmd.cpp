
#include "VCPlugInHeaders.h"

// Interface includes:
#include "IDocument.h"
#include "IK2ServiceRegistry.h"
#include "IK2ServiceProvider.h"
#include "ISAXServices.h"
#include "ISAXContentHandler.h"
#include "ISAXEntityResolver.h"
#include "ISAXParserOptions.h"
#include "IStringListData.h"
#include "ICoreFilename.h"
#include "ISubject.h"
#include "IStringListData.h"
#include "IStringData.h"

// General includes:
#include "CmdUtils.h"
#include "Command.h"
#include "ErrorUtils.h"
#include "FileUtils.h"
#include "StreamUtil.h"
#include "StringUtils.h"
#include "IDTime.h"

#include "XPGID.h"

/** Implementation of command to set article id data in kTextStoryBoss
*/
class XPGUpdateArticleXMLFileCmd : public Command
{
public:
	/** Constructor.
		@param boss interface ptr from boss object on which this interface is aggregated.*/
	XPGUpdateArticleXMLFileCmd(IPMUnknown* boss);

protected:

	/** Do
	 	@see Command::Do
	*/
	virtual void Do();
	

	virtual void DoNotify();

	/** CreateName
	 	@see Command::CreateName
	*/
	virtual PMString* CreateName();

};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(XPGUpdateArticleXMLFileCmd, kXPGUpdateArticleXMLFileCmdImpl)

/* Constructor
*/
XPGUpdateArticleXMLFileCmd::XPGUpdateArticleXMLFileCmd(IPMUnknown* boss) : Command(boss)
{	
	SetUndoability(ICommand::kRegularUndo );
	SetTarget(Command::kSessionTarget);
}

/* Do
*/
void XPGUpdateArticleXMLFileCmd::Do()
{
	ErrorCode status = kFailure;
	PMString error = kNullString;
	do                      
	{
		// Get file path of article to update
		InterfacePtr<IStringData> articleData (this, UseDefaultIID());
		PMString articleFilePath = articleData->Get();

		InterfacePtr<IK2ServiceRegistry> serviceRegistry(GetExecutionContextSession(), UseDefaultIID()); 
		InterfacePtr<IK2ServiceProvider> xmlProvider(serviceRegistry->QueryServiceProviderByClassID(kXMLParserService, kXMLParserServiceBoss)); 
		
		InterfacePtr<ISAXServices> saxServices(xmlProvider, UseDefaultIID()); 

		InterfacePtr<ISAXContentHandler> contentHandler(
			::CreateObject2<ISAXContentHandler>(kXPGXMLDescriptionUpdaterBoss, ISAXContentHandler::kDefaultIID)); 
		K2Vector<PMString> parameters;
		parameters.push_back(articleFilePath); // Only one parameter, the article file path	
		InterfacePtr<IStringListData> parametersData (contentHandler, UseDefaultIID());
		parametersData->SetStringList(parameters); 

		contentHandler->Register(saxServices);

		InterfacePtr<ISAXEntityResolver> entityResolver(
			::CreateObject2<ISAXEntityResolver>(kXMLEmptyEntityResolverBoss, ISAXEntityResolver::kDefaultIID));

		InterfacePtr<ISAXParserOptions> parserOptions(saxServices, UseDefaultIID());
		
		// These commitments: follow 
		// the existing ImportXMLFileCmd
		parserOptions->SetNamespacesFeature(kFalse);
	    parserOptions->SetShowWarningAlert(kFalse);

		IDFile artFile = FileUtils::PMStringToSysFile(articleFilePath);
		InterfacePtr<IPMStream> xmlFileStream(StreamUtil::CreateFileStreamRead(artFile));
		
		if(xmlFileStream)
		{
			saxServices->ParseStream(xmlFileStream, contentHandler, entityResolver);	
			xmlFileStream->Close();
			xmlFileStream.reset(nil);
		}
		else 
			ErrorUtils::PMSetGlobalErrorCode(kFailure);
		
		if(ErrorUtils::PMGetGlobalErrorCode() != kSuccess)
		{
			// Reset Error Code			
			error = PMString(kXPGErrParseArticleFailed);
			error.Translate();
			StringUtils::ReplaceStringParameters(&error, articleFilePath); 
			PMString error2(kXPGErrDescFileNotUpdatedKey);
			error2.Translate();
			error += " " + error2;		
			break;
		}
		else
		{
			// Make the temporary file the new up-to-date description file
			InterfacePtr<ICoreFilename> fileToDelete (::CreateObject2<ICoreFilename>(kCoreFilenameBoss)); 

			fileToDelete->Initialize(&artFile);
			
			IDTime start, now;
			start.CurrentTime();
			bool16 deleted = (fileToDelete->Delete() == 0);
			while(!deleted)
			{			
				now.CurrentTime();
				uint64 diff = (now.GetTime() - start.GetTime()) / GlobalTime::kOneSecond;
				if(diff > 10.0)
					break;

				deleted = (fileToDelete->Delete() == 0);
			}
			
			if(deleted)
			{
				IDFile updatedDesc = FileUtils::PMStringToSysFile(articleFilePath + ".tmp");
				FileUtils::SwapFiles(updatedDesc, artFile);				
			}
			else
			{
				error = PMString(kXPGErrDescFileNotUpdatedKey);
				break;
			}			
		}

		status = kSuccess;

	} while (kFalse);       

	// Handle any errors
	if (status != kSuccess) {
		ErrorUtils::PMSetGlobalErrorCode(status, &error);
	}
}


/* DoNotify
*/
void XPGUpdateArticleXMLFileCmd::DoNotify()
{
	// Send notification so that texte panel is updated
	InterfacePtr<ISubject> sessionSubject (this, UseDefaultIID());
	sessionSubject->Change(kXPGRefreshMsg, IID_IREFRESHPROTOCOL,this);
}

/* CreateName
*/
PMString* XPGUpdateArticleXMLFileCmd::CreateName()
{
	return new PMString(kXPGUpdateArticleXMLFileCmdKey);
}

// End, XPGUpdateArticleXMLFileCmd.cpp.


