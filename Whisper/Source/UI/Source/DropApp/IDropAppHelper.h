/*
 *  File:		IDropAppHelper.h
 *  Summary:	Helper interface used by XDropApp.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDropAppHelper.h,v $
 *		Revision 1.2  2001/05/30 01:14:08  jesjones
 *		Added Startup.
 *		
 *		Revision 1.1  2001/01/05 06:14:17  jesjones
 *		Refactored the drag and drop code so that it uses new IDropApp and IDropAppHelper interfaces.
 */

#pragma once

#include <XFileDialog.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XCountFiles;
class XProcessFiles;


// ===================================================================================
//	class IDropAppHelper
//!		Helper interface used by XDropApp.
// ===================================================================================
class IDropAppHelper : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Initialization -----
	virtual void 		Startup(const std::vector<XFileSystemSpec>& specs) = 0;	
						/**< Called right before processsing starts. You can use this to
						display a dialog allowing the user to set options. if the user
						wants to cancel out throw XCancelException. */

	virtual XProcessFiles* CreateProcessThread(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs) = 0;	
	virtual XCountFiles* CreateCounterThread(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs) = 0;

	virtual XOpenReply 	GetFile(XFileFilter* fileFilter, XFolderFilter* folderFilter) = 0;
						/**< Called when the app is launched by double clicking. */

	// ----- Termination -----
	virtual void 		ExitingNormally() = 0;
						/**< Pop up an alert telling the user what your app did. */
	
	virtual void 		ExitingViaCancel() = 0;
						/**< The user has pressed the cancel button. You'll usually do nothing. */

	virtual void 		ExitingViaError() = 0;
						/**< An error aborted the app. An alert will have been posted
						describing the error so you'll usually do nothing. */
};

typedef XInterfacePtr<IDropAppHelper> IDropAppHelperPtr;
typedef XInterfacePtr<const IDropAppHelper> IConstDropAppHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
