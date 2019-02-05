/*
 *  File:		IDropApp.h
 *  Summary:	Interface used for drag and drop applications.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDropApp.h,v $
 *		Revision 1.1  2001/01/05 06:14:13  jesjones
 *		Refactored the drag and drop code so that it uses new IDropApp and IDropAppHelper interfaces.
 *		
 */

#pragma once

#include <XFileSystemSpec.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileFilter;
class XFolderFilter;


// ===================================================================================
//	class IDropApp
//!		Interface used for drag and drop applications.
// ===================================================================================
class IDropApp : public XUnknown {

//-----------------------------------
//	Types
//
public:
	typedef std::vector<XFileSystemSpec> SpecList;

//-----------------------------------
//	API
//
public:
	virtual void 		Start(XFileFilter* takeFile, XFolderFilter* takeFolder) = 0;
						/**< Spins off a thread to process the files dropped on the app
						(will pop up a system file chooser dialog if no files were dropped
						on the app). */
						
	virtual const SpecList& GetSelection() const = 0;
						/**< Returns the files/folders that were dropped on the app. Note
						that this does not include all the processed files since folders
						are normally recursively processed. */

	virtual void 		HandleCancel() = 0;
						/**< Aborts the thread and calls IDropAppHelper::ExitingViaCancel. */

	virtual void 		HandleError(const std::wstring& summary, const std::wstring& narrative) = 0;
						/**< Called when one of the threads catches an exception. Quits the
						app and calls IDropAppHelper::ExitingViaError. */
};

typedef XInterfacePtr<IDropApp> IDropAppPtr;
typedef XInterfacePtr<const IDropApp> IConstDropAppPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
