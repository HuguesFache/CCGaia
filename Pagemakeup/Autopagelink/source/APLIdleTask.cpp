/*
//
//	Author: Trias
//
//	Date: 01-Mar-2006
//
*/

#include "VCPlugInHeaders.h"

// interface includes
#include "IApplication.h"
#include "IIdleTaskMgr.h"
#include "ITCLParser.h"
#include "ICoreFilename.h"
#include "IPMStream.h"

// General includes:
#include "CIdleTask.h"
#include "FileUtils.h"
#include "StreamUtil.h"

#include "PlatformFolderTraverser.h"
#include "FileUtils.h"

#include <string>
#include <fstream>

using namespace std;

// Project includes
#include "APLID.h"
#include "PrsID.h"
#include "GlobalDefs.h"

#include "CAlert.h"

const static PMString ConfigFileName = "AutoPageLink.ini";

// Refresh  in case we add new images


/** Implements IIdleTask

@ingroup paneltreeview
*/

class APLIdleTask : public CIdleTask
{
public:

	/**	Constructor
	@param boss boss object on which this interface is aggregated
	*/
	APLIdleTask(IPMUnknown* boss);

	/**	Destructor
	*/
	virtual ~APLIdleTask() {}


	/**	Called by the idle task manager in the app core when the task is running
	@param appFlags [IN] specifies set of flags which task can check to see if it should do something
	this time round
	@param timeCheck [IN] specifies how many milliseconds before the next active task becomes overdue.
	@return uint32 giving the interval (msec) that should elapse before calling this back
	*/
	virtual uint32 RunTask(uint32 appFlags, IdleTimer* timeCheck);

	virtual void  InstallTask(uint32 millisecsBeforeFirstRun);

	/**	Get name of task
	@return const char* name of task
	*/
	virtual const char* TaskName();

private:

	/**
	Load data from configuration file
	*/
	ErrorCode GetConfigData();
	void ReadLine(std::string& line);

	/**
	Inspect input folder and process TCL file if any
	*/
	bool16 ProcessTCLFile(IDFile inputFolder);

	//PlatformFileSystemIterator iter;

	IDFile inFolder, inFolder2, outFolder, errorFolder;

	int32 kAPLExecInterval;

};


/* CREATE_PMINTERFACE
Binds the C++ implementation class onto its ImplementationID
making the C++ code callable by the application.
*/
CREATE_PMINTERFACE(APLIdleTask, kAPLIdleTaskImpl)


/* Constructor
*/
APLIdleTask::APLIdleTask(IPMUnknown *boss)
	:CIdleTask(boss), kAPLExecInterval(1000)
{
}

void APLIdleTask::InstallTask(uint32 millisecsBeforeFirstRun)
{

	if (this->GetConfigData() == kSuccess)
	{
		PMString inFolderStr, inFolder2Str, outFolderStr, errorFolderStr;
		inFolderStr = FileUtils::SysFileToPMString(inFolder);
		inFolder2Str = FileUtils::SysFileToPMString(inFolder2);
		outFolderStr = FileUtils::SysFileToPMString(outFolder);
		errorFolderStr = FileUtils::SysFileToPMString(errorFolder);

		if (inFolderStr.Compare(kFalse, outFolderStr) == 0 || inFolderStr.Compare(kFalse, errorFolderStr) == 0
			|| inFolder2Str.Compare(kFalse, outFolderStr) == 0 || inFolder2Str.Compare(kFalse, errorFolderStr) == 0
			)
		{
			CAlert::WarningAlert(PMString(kAPLErrorInFolderStringKey));
		}
		else
		{
			CIdleTask::InstallTask(millisecsBeforeFirstRun);
		}
	}
}

/* RunTask
*/
uint32 APLIdleTask::RunTask(
	uint32 appFlags, IdleTimer* timeCheck)
{
	if (appFlags &
		(IIdleTaskMgr::kMouseTracking
			| IIdleTaskMgr::kUserActive
			| IIdleTaskMgr::kMenuUp))
	{
		return kOnFlagChange;
	}

	/* TEMPORARY VERSION 7.0.0 */
	InterfacePtr<IBoolData> parserThreadRunning(GetExecutionContextSession(), IID_ITCLPARSER);
	if (parserThreadRunning->Get() == kTrue)
	{
		return kAPLExecInterval;
	}
	/* TEMPORARY VERSION 7.0.0 */

	// First check input folder with higher priority
	while (this->ProcessTCLFile(inFolder2));

	// Then check the other input folder
	this->ProcessTCLFile(inFolder);

	return kAPLExecInterval;
}

bool16 APLIdleTask::ProcessTCLFile(IDFile inputFolder)
{
	bool16 fileFound = kFalse;
	PlatformFolderTraverser folderTraverser(inputFolder, kFalse, kTrue, kFalse, kTrue);

	//iter.SetStartingPath(inputFolder);
	IDFile tclFile;
	PMString filter("\\*.tcl");

	while (folderTraverser.Next(&tclFile))
	{
		PMString ext;
		FileUtils::GetExtension(tclFile, ext);

		if (ext.Compare(kFalse, "tcl") != 0)
		{
			continue;
		}
		else
		{
			//if(!iter.IsDirectory(tclFile))
			{
				fileFound = kTrue;
				// Make sure file is fully written before processing

				InterfacePtr<IPMStream> checkTCLStream(StreamUtil::CreateFileStreamWrite(tclFile, kOpenOut | kOpenApp));
				if (checkTCLStream == nil)
				{
					//CAlert::InformationAlert("Fichier IMPOSSIBLE ﾈ ouvrir");
					continue;
				}
				else
				{
					checkTCLStream->Close();
					checkTCLStream.reset();
#if SERVER
					CAlert::InformationAlert("Processing " + FileUtils::SysFileToPMString(tclFile));
#endif			
					InterfacePtr<ITCLParser> parser((ITCLParser *) ::CreateObject(kTCLParserBoss, IID_ITCLPARSER));
					if (parser == nil || !parser->SetInputFile(tclFile))
					{
						PMString msg(kAPLErrorOpenTCLStringKey);
						msg.Translate();
						msg.Append(tclFile.GetFileName());
						msg.SetTranslatable(kFalse);
						CAlert::WarningAlert(msg);

						// Move to error folder
						IDFile movedFile = errorFolder;
						FileUtils::AppendPath(&movedFile, tclFile.GetFileName());
						if (FileUtils::DoesFileExist(movedFile))
						{
							InterfacePtr<ICoreFilename> coreFile(::CreateObject2<ICoreFilename>(kCoreFilenameBoss));
							coreFile->Initialize(&movedFile);
							coreFile->Delete();
						}

						FileUtils::SwapFiles(tclFile, movedFile);

						//hasNext = kFalse;
					}
					else
					{
						// Don't display messages
						parser->SetStopDisplay(kTrue);
						int32 res = parser->LaunchParser();
						IDFile savedFile;
						if (res == 0)
							savedFile = outFolder;
						else
							savedFile = errorFolder;

						FileUtils::AppendPath(&savedFile, tclFile.GetFileName());

						if (FileUtils::DoesFileExist(savedFile))
						{
							InterfacePtr<ICoreFilename> coreFile(::CreateObject2<ICoreFilename>(kCoreFilenameBoss));
							coreFile->Initialize(&savedFile);
							coreFile->Delete();
						}

						FileUtils::SwapFiles(tclFile, savedFile);

						//hasNext = kFalse;
					}
				}
			}
			//else
			//	hasNext = iter.FindNextFile(tclFile);
		}
	}
	return fileFound;
}

/* TaskName
*/
const char* APLIdleTask::TaskName()
{
	return kAPLIdleTaskKey;
}

ErrorCode APLIdleTask::GetConfigData()
{
	ErrorCode status = kFailure;

	IDFile configFile;

#if APL_INI_USE_USERFOLDER == 0
	FileUtils::GetAppInstallationFolder(&configFile);
	FileUtils::AppendPath(&configFile, ConfigFileName);
#else
	// GD 28/07/2022 ++
	IDFile approamingdatafolder, parent1, parent2, parent3, parent4, parent5;
	PMString message;
	PMString subFolderName; // on laisse volontairement vide.
	FileUtils::GetAppRoamingDataFolder(&approamingdatafolder, subFolderName);
	FileUtils::GetParentDirectory(approamingdatafolder, parent1);
	FileUtils::GetParentDirectory(parent1, parent2);
	FileUtils::GetParentDirectory(parent2, parent3);
	FileUtils::GetParentDirectory(parent3, parent4);
	FileUtils::GetParentDirectory(parent4, parent5);
	FileUtils::GetParentDirectory(parent5, configFile);
	FileUtils::AppendPath(&configFile, ConfigFileName);
	// GD 28/07/2022 --
#endif

	do
	{
		InterfacePtr<IPMStream> input(StreamUtil::CreateFileStreamRead(configFile));

		if (!input)
			break;

		string line;

		uchar c;
		while (input->GetStreamState() == kStreamStateGood)
		{
			string line;
			// Get line
			while ((input->GetStreamState() == kStreamStateGood) && (input->XferByte(c) != '\r'))
			{
				if (c != '\n')
					line += c;
			}
			ReadLine(line);
		}

		input->Close();

		status = kSuccess;

	} while (kFalse);

	return status;
}

void APLIdleTask::ReadLine(std::string& line)
{
	PlatformChar equal;
	equal.Set('=');

	PMString str(line.c_str());
	if (!str.IsEmpty())
	{
		if (str[str.NumUTF16TextChars() - 1].IsChar('\r'))
			str.Remove(str.NumUTF16TextChars() - 1);

		int32 pos = str.IndexOfCharacter(equal);
		if (pos != -1)
		{
			PMString * keyFound = str.Substring(0, pos);
			if (keyFound)
			{
				keyFound->StripWhiteSpace();

				PMString * value = str.Substring(pos + 1);
				if (value)
				{
					if (!value->IsEmpty())
					{
						value->StripWhiteSpace(PMString::kLeadingAndTrailingWhiteSpace);

						if (keyFound->Compare(kFalse, "IN_FOLDER") == 0)
							inFolder = FileUtils::PMStringToSysFile(*value);
						if (keyFound->Compare(kFalse, "IN_FOLDER2") == 0)
							inFolder2 = FileUtils::PMStringToSysFile(*value);
						else if (keyFound->Compare(kFalse, "OUT_FOLDER") == 0)
							outFolder = FileUtils::PMStringToSysFile(*value);
						else if (keyFound->Compare(kFalse, "ERROR_FOLDER") == 0)
							errorFolder = FileUtils::PMStringToSysFile(*value);
						else if (keyFound->Compare(kFalse, "CHECK_INTERVAL") == 0)
						{
							PMString::ConversionError error = PMString::kNoError;
							kAPLExecInterval = value->GetAsNumber(&error);

							if (error == PMString::kNoError)
								kAPLExecInterval = (kAPLExecInterval * 1000) / 60;
							else
								kAPLExecInterval = 1000;  // Default is 1 second
						}
					}

					delete value;
				}
				delete keyFound;
			}
		}
	}
}

