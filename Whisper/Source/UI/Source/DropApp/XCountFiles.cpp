/*
 *  File:		XCountFiles.cpp
 *  Summary:	The thread used to count the numbers of files to process (so we can use a progress bar).
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCountFiles.cpp,v $
 *		Revision 1.5  2001/04/27 04:31:32  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2000/11/20 05:48:23  jesjones
 *		XCallback again uses IsValid.
 *		
 *		Revision 1.3  2000/11/12 07:55:16  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.2  2000/11/09 12:29:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XCountFiles.h>

#include <ICommand.h>
#include <ICommandQueue.h>
#include <ICommands.h>
#include <XBind.h>
#include <XError.h>
#include <XFileFilter.h>
#include <XFileIterator.h>
#include <XFileSystem.h>
#include <XFolderFilter.h>
#include <XMiscUtils.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XCountFiles
// ===================================================================================

//---------------------------------------------------------------
//
// XCountFiles::~XCountFiles
//
//---------------------------------------------------------------
XCountFiles::~XCountFiles()
{
	mFileFilter->RemoveReference();
	mFolderFilter->RemoveReference();
}


//---------------------------------------------------------------
//
// XCountFiles::XCountFiles
//
//---------------------------------------------------------------
XCountFiles::XCountFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, ErrorCallback err) : mSpecs(specs), mErrHandler(err)
{
	PRECONDITION(fileFilter != nil);
	PRECONDITION(folderFilter != nil);
	PRECONDITION(err.IsValid());
	
	mCount = 0;
	
	mFileFilter = fileFilter;
	mFolderFilter = folderFilter;
	
	mFileFilter->AddReference();
	mFolderFilter->AddReference();
}


//---------------------------------------------------------------
//
// XCountFiles::OnRun
//
//---------------------------------------------------------------
void XCountFiles::OnRun()
{
	MilliSecond start = GetMilliSeconds();
	
	uint32 count = mSpecs.size();
	for (uint32 index = 0; index < count && !mAborted; ++index) {
		const XFileSystemSpec& spec = mSpecs[index];
		XThread::Yield();

		if (XFileSystem::IsFolder(spec)) {
			XFolderSpec folder(spec);

			mFileFilter->AddReference();			// XFileIterator takes the filters
			mFolderFilter->AddReference();

			XFileIterator iter = folder.begin(mFileFilter, mFolderFilter, kRecursive);
			while (iter != folder.end() && !mAborted) {
				this->OnCount(*iter);
				++iter;			
				
				XThread::Yield();
			}
		
		} else
			this->OnCount(XFileSpec(spec));
	}

//	MilliSecond time = GetMilliSeconds() - start;
//	TRACE("Count files time = %.3f\n", time/1000.0);
}


//---------------------------------------------------------------
//
// XCountFiles::OnException
//
// Calling HandleError within a worker thread has much potential
// for synchronization problems so we'll use a XClosureCommand to
// defer calling HandleError until we're back in the main thread.
//
//---------------------------------------------------------------
void XCountFiles::OnException(const std::exception* e)
{
	std::wstring summary   = LoadWhisperString(L"There was an error while counting the files.");
	std::wstring narrative = XError::Instance()->GetText(e);
	
	XInterfacePtr<ICommand> command(L"Callback Command");
	XInterfacePtr<ICallbackCommand> callback(command);
	callback->Init(Bind2(mErrHandler, summary, narrative));
	
	XInterfacePtr<ICommandQueue> queue(L"Application");
	queue->Post(command);
}


//---------------------------------------------------------------
//
// XCountFiles::OnCount
//
//---------------------------------------------------------------
void XCountFiles::OnCount(const XFileSpec& spec)
{
	UNUSED(spec);

	++mCount;
}


}	// namespace Whisper
