/*
 *  File:		XProcessFiles.h
 *  Summary:	The thread that processes files dropped on a drag and drop app.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XProcessFiles.h,v $
 *		Revision 1.4  2001/02/07 04:58:49  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:36:56  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:47:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <vector>

#include <XCallbacks.h>
#include <XFileSystemSpec.h>
#include <XThread.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileFilter;
class XFileSpec;
class XFolderFilter;


//-----------------------------------
//	Types
//
typedef XCallback2<void, std::wstring, std::wstring> ErrorCallback;


// ===================================================================================
//	class XProcessFiles
//!		The thread that processes files dropped on a drag and drop app.
// ===================================================================================
class UI_EXPORT XProcessFiles : public XThread {

	typedef XThread Inherited;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XProcessFiles();
	
  						XProcessFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, ErrorCallback err, uint32 stackBytes = 0);
  						
//-----------------------------------
//	New API
//
public:
			uint32 		GetCount() const						{return mCount;}
			
			std::wstring GetMesg() const;
			
			void 		Abort()									{mAborted = true;}
			
protected:
	virtual void 		OnProcessFolder(const XFolderSpec& folder);

	virtual void 		OnProcessFile(const XFileSpec& file) = 0;
	
	virtual void 		OnError(const std::wstring& summary, const std::wstring& narrative);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnRun();
	
	virtual void 		OnException(const std::exception* e);

//-----------------------------------
//	Member Data
//
protected:
	std::vector<XFileSystemSpec>	mSpecs;
	XFileSystemSpec 				mSpec;
	
	volatile uint32					mCount;				//!< only one thread increments this so we don't need to bother with an XAtomicCounter

	std::wstring					mMessage;
	std::wstring					mMesgPrefix;		//!< defaults to "Processing: "
	mutable XCriticalSection		mMesgMutex;

	XFileFilter*					mFileFilter;
	XFolderFilter*					mFolderFilter;
	ErrorCallback					mErrHandler;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
