/*
 *  File:		XProcessFiles.cpp
 *  Summary:	The thread that processes files dropped on a drag and drop app.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XProcessFiles.cpp,v $
 *		Revision 1.6  2001/04/27 04:35:24  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.5  2001/03/17 12:33:17  jesjones
 *		Calls CoInitialize (need to do this for every thread that uses files).
 *		
 *		Revision 1.4  2000/11/20 05:48:23  jesjones
 *		XCallback again uses IsValid.
 *		
 *		Revision 1.3  2000/11/12 07:55:39  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.2  2000/11/09 12:47:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XProcessFiles.h>

#if WIN
	#include <objbase.h>
#endif

#include <ICommand.h>
#include <ICommandQueue.h>
#include <ICommands.h>
#include <XBind.h>
#include <XError.h>
#include <XExceptions.h>
#include <XFileFilter.h>
#include <XFileIterator.h>
#include <XFileSystem.h>
#include <XFolderFilter.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XProcessFiles
// ===================================================================================

//---------------------------------------------------------------
//
// XProcessFiles::~XProcessFiles
//
//---------------------------------------------------------------
XProcessFiles::~XProcessFiles()
{
	mFileFilter->RemoveReference();
	mFolderFilter->RemoveReference();
}


//---------------------------------------------------------------
//
// XProcessFiles::XProcessFiles
//
//---------------------------------------------------------------
XProcessFiles::XProcessFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, ErrorCallback err, uint32 stackBytes) : XThread(stackBytes), mSpecs(specs), mErrHandler(err)
{
	PRECONDITION(fileFilter != nil);
	PRECONDITION(folderFilter != nil);
	PRECONDITION(err.IsValid());
	
	mCount = 0;
		
	mFileFilter = fileFilter;
	mFolderFilter = folderFilter;
	
	mFileFilter->AddReference();
	mFolderFilter->AddReference();

	mMesgPrefix = LoadWhisperString(L"Processing: ");
}


//---------------------------------------------------------------
//
// XProcessFiles::GetMesg
//
//---------------------------------------------------------------
std::wstring XProcessFiles::GetMesg() const
{
	std::wstring mesg;
	
	{
	XEnterCriticalSection enter(mMesgMutex);
		mesg = mMessage;
	}
	
	return mesg;
}


//---------------------------------------------------------------
//
// XProcessFiles::OnRun
//
//---------------------------------------------------------------
void XProcessFiles::OnRun()
{
#if WIN
#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM) 
	HRESULT result = CoInitializeEx(nil, COINIT_APARTMENTTHREADED);	// IShellLink doesn't seem to work with COINIT_SPEED_OVER_MEMORY
#else
	HRESULT result = CoInitialize(nil);
#endif
	ThrowIfFailed(result);
#endif

	uint32 count = mSpecs.size();
	for (uint32 index = 0; index < count && !mAborted; ++index) {
		mSpec = mSpecs[index];
		XThread::Yield();

		bool isFile = !XFileSystem::IsFolder(mSpec);
		
		if (isFile) {
			XFileSpec file(mSpec);
			if ((*mFileFilter)(file)) {
				{
				XEnterCriticalSection enter(mMesgMutex);
					mMessage = mMesgPrefix + file.GetName();
				}
				
				this->OnProcessFile(file);
				++mCount;
			}
			
		} else {
			XFolderSpec folder(mSpec);
			if ((*mFolderFilter)(folder))
				this->OnProcessFolder(folder);
		}
	}
}


//---------------------------------------------------------------
//
// XProcessFiles::OnException
//
// Calling HandleError within a worker thread has much potential
// for synchronization problems so we'll use a XClosureCommand to
// defer calling HandleError until we're back in the main thread.
//
//---------------------------------------------------------------
void XProcessFiles::OnException(const std::exception* e)
{
	std::wstring summary;
	if (mSpec.GetName().length() == 0)
		summary = LoadWhisperString(L"There was an error processing the files.");

	else if (XFileSystem::IsFolder(mSpec))
		summary = LoadWhisperString(L"There was an error processing '#1'.", XFolderSpec(mSpec).GetName());

	else
		summary = LoadWhisperString(L"There was an error processing '#1'.", mSpec.GetName());

	std::wstring narrative = XError::Instance()->GetText(e);	
	
	this->OnError(summary, narrative);
}


//---------------------------------------------------------------
//
// XProcessFiles::OnError
//
//---------------------------------------------------------------
void XProcessFiles::OnError(const std::wstring& summary, const std::wstring& narrative)
{
	XInterfacePtr<ICommand> command(L"Callback Command");
	XInterfacePtr<ICallbackCommand> callback(command);
	callback->Init(Bind2(mErrHandler, summary, narrative));
	
	XInterfacePtr<ICommandQueue> queue(L"Application");
	queue->Post(command);
}


//---------------------------------------------------------------
//
// XProcessFiles::OnProcessFolder
//
//---------------------------------------------------------------
void XProcessFiles::OnProcessFolder(const XFolderSpec& folder)
{	
	mFileFilter->AddReference();			// XFileIterator takes the filters
	mFolderFilter->AddReference();

	XFileIterator iter = folder.begin(mFileFilter, mFolderFilter, kRecursive);
	while (iter != folder.end() && !mAborted) {
		XFileSpec file = *iter;
		++iter;
	
		mSpec = file;						// need this for OnException

		{
		XEnterCriticalSection enter(mMesgMutex);
			mMessage = mMesgPrefix + file.GetName();
		}
				
		this->OnProcessFile(file);
		++mCount;

		XThread::Yield();
	} 
}


}	// namespace Whisper
