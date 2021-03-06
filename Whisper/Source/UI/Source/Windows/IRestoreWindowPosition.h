/*
 *  File:       IRestoreWindowPosition.h
 *  Summary:   	Used to save and restore window positions.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IRestoreWindowPosition.h,v $
 *		Revision 1.2  2000/12/14 08:24:39  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/11/20 00:56:18  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IRestoreWindowPosition
//!		Used to save and restore window positions.
// ===================================================================================
class IRestoreWindowPosition : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		Save() = 0;
						/**< Queries for an IWindow and saves it's position and size. */

	virtual void 		Restore() = 0;
						/**< Restores the window to the saved state. Note that the
						caller is responsible for ensuring that the window is on
						screen. */
};

typedef XInterfacePtr<IRestoreWindowPosition> IRestoreWindowPositionPtr;
typedef XInterfacePtr<const IRestoreWindowPosition> IConstRestoreWindowPositionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
