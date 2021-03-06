/*
 *  File:		XCountFiles.h
 *  Summary:	The thread used to count the numbers of files to process (so we can use a progress bar).
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCountFiles.h,v $
 *		Revision 1.4  2001/02/07 04:57:54  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:33:45  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:29:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <vector>

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
//	class XCountFiles
//!		The thread used to count the numbers of files to process (so we can use a progress bar).
// ===================================================================================
class UI_EXPORT XCountFiles : public XThread {

	typedef XThread Inherited;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XCountFiles();
	
  						XCountFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, ErrorCallback err);
  						
//-----------------------------------
//	New API
//
public:
			uint32 		GetCount() const					{ASSERT(!mRunning); ASSERT(!mAborted); return mCount;}

			void 		Abort()								{mAborted = true;}
			
protected:
	virtual void 		OnCount(const XFileSpec& spec);
						/**< May be overridden if you want to have more finely grained
						progress info. */
			
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
	volatile uint32					mCount;
	
	XFileFilter*					mFileFilter;
	XFolderFilter*					mFolderFilter;
	ErrorCallback					mErrHandler;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
