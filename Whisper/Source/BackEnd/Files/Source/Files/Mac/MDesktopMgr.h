/*
 *  File:		MDesktopMgr.h
 *  Summary:	Some handy functions for dealing with the Desktop Manager.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDesktopMgr.h,v $
 *		Revision 1.2  2000/11/09 09:12:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <2>	 2/06/00	JDJ		Moved LaunchApp code from MProcess to here.
 *		 <1>	 1/17/98	JDJ		Created (from Raven)
 */

#pragma once

#include <Files.h>
#include <Icons.h>

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif

//-----------------------------------
//	Forward References
//
class XFileSpec;


// ===================================================================================
//	class MDesktopMgr
// ===================================================================================
class FILES_EXPORT MDesktopMgr {

public:
	// ----- Desktop Info -----
	static	std::wstring GetComment(const FSSpec& spec);
						// Returns the Get Info comment for a file or directory. 

	static	void 		SetComment(const FSSpec& spec, const std::wstring& comment);
						// Comments longer than 200 characters are truncated.

	static CIconHandle 	GetIcon(int16 volume, OSType type, OSType creator);
						// Returns an 8-bit 32x32 icon for files with the specified type 
						// and creator. You can use PlotCIcon or PlotCIconHandle to draw
						// the icon. Use DisposeCIcon when you're done with the icon. 

	static Handle 		GetIconData(int16 volume, OSType type, OSType creator, SignedByte iconType);
						// Returns the icon data for files with the specified type and creator.
						// iconType can be kLargeIcon, kLarge4BitIcon, kLarge8BitIcon, kSmallIcon, 
						// kSmall4BitIcon, or kSmall8BitIcon. Note that you cannot use this
						// with PlotCIcon or PlotIconSuite without some additional processing.

	// ----- App Locations -----
	static	FSSpec 		GetApp(int16 volume, OSType signature);
						// Returns a file spec pointing to an app with the specified 
						// signature. Throws afpItemNotFound if nothing was found.

	static	FSSpec 		GetApp(OSType signature, bool allowRemoteVolumes = true);
						// Searches for the app on local volumes and then, optionally,
						// remote volumes.

	// ----- App Launching -----
	static 	void 		LaunchApp(OSType signature, bool keepRunning = true, bool doSwitch = true, bool allowRemoteVolumes = true);
						// If keepRunning is false the the app is killed. If doSwitch is false 
						// the sub-lauched app isn't automatically brought to the front. If 
						// allowRemoteVolumes is set remote volumes will be searched if the app 
						// was not found on a local volume. Throws XSystemException with error 
						// code afpItemNotFound if the app wasn't found.

	static 	void 		LaunchApp(OSType signature, const XFileSpec& file, bool keepRunning = true, bool doSwitch = true, bool allowRemoteVolumes = true);
						// Launch the app (if need be) and tell it to open the specified file.

};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
