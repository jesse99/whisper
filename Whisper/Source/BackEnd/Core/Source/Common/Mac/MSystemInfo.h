/*
 *  File:       MSystemInfo.h
 *	Summary:	Provides information about the system.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MSystemInfo.h,v $
 *		Revision 1.6  2001/11/25 23:37:34  jesjones
 *		Added HasOSX and HasAqua.
 *		
 *		Revision 1.5  2001/03/05 05:39:44  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.4  2000/12/11 07:19:57  jesjones
 *		Fixed a doxygen bug.
 *		
 *		Revision 1.3  2000/12/10 08:49:16  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:20:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Files.h>
#include <Processes.h>

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Types
//
typedef uint32 Version;			//!< packed version number, eg 0x0753 means 7.5.3


// ===================================================================================
//	class MSystemInfo
//!		Provides information about the system.
// ===================================================================================
class CORE_EXPORT MSystemInfo {

//-----------------------------------
//	API
//
public:
	//! @name App
	//@{
	static	FSSpec 		GetAppSpec();
						/**< volume, dir, and name of app */

	static	int16 		GetAppRefNum();
						/**< applicationÕs resource refNum */

	static	uint32 		GetAppSizeFlags();
						/**< SIZE resource flags (use mode constants in Processes.h to parse these) */

	static	ProcessSerialNumber GetAppPSN();
	//@}

	//! @name System
	//@{
	static	Version 	GetSystemVersion();

	static	bool 		HasSystem7();

	static	bool 		HasSystem8();

	static	bool 		HasOSX();

	static	bool 		HasTempMem();

	static	bool 		HasAqua();

	static	bool 		HasASLM();

	static	bool 		HasCFM();

	static	bool 		HasThreadMgr();
	//@}

	//! @name Graphics
	//@{
	static	Version 	GetQuickDrawVersion();
						/**< 32-bit QuickDraw starts with 0x0200 */

	static	bool 		HasColorQuickDraw();

	static	bool 		Has32BitQuickDraw();

	static	bool 		HasQuickDrawGX();

	static	bool 		HasGXPrinting();
	//@}

	//! @name Text
	//@{
	static	Version 	GetTextEditVersion();

	static	Version 	GetScriptMgrVersion();

	static	bool 		HasGrayishText();

	static	bool 		HasTrueType();
	//@}

	//! @name Display Manager
	//@{
	static	Version 	GetDisplayMgrVersion();

	static	bool 		HasDisplayMgr();
	//@}

	//! @name QuickTime
	//@{
	static	Version 	GetQuickTimeVersion();

	static	bool 		HasQuickTime();
	//@}

	//! @name QuickDraw 3D
	//@{
#if !TARGET_CARBON
	static	Version 	GetQuickDraw3DVersion();

	static	Version 	GetQuickDraw3DViewerVersion();

	static	bool 		HasQuickDraw3D();

	static	void 		SetHasQuickDraw3D(bool has)				{msHasQuickDraw3D = has;}
						/**< Set by Init3D if Q3Initialize returns an error. */
#endif
	//@}

	//! @name Sound
	//@{
	static	Version 	GetSoundMgrVersion();

	static	bool 		HasSoundMgr3();

	static	bool 		Has16BitSound();
	//@}

	//! @name Drag Manager
	//@{
	static	bool 		HasDragMgr();

	static	bool 		HasTranslucentDrag();
	//@}

	//! @name Networking
	//@{
	static	bool 		HasMacTCP();

	static	bool 		HasOpenTransport();

	static	Version 	GetOpenTransportVersion();
	//@}

	//! @name Sprockets
	//@{
#if !TARGET_CARBON
	static	bool 		HasDrawSprocket();

	static	bool 		HasInputSprocket();

	static	bool 		HasNetSprocket();

	static	bool 		HasSoundSprocket();

	static	Version 	GetInputSprocketVersion();
#endif
	//@}

	//! @name OS 8 Managers
	//@{
	static	bool 		HasAppearanceMgr();

	static	Version 	GetAppearanceVersion();

	static	bool 		HasWindowMgr2();

	static	bool 		HasFloatingWindows();

	static	bool 		HasNavServices();

	static	bool 		HasContextMenus();
	
	static	bool 		HasTextEncodingConverter();

	static	Version 	GetTextEncodingConverterVersion();
	//@}

//-----------------------------------
//	Internal API
//
public:
	static	void 		SetHasContextMenus(bool has)			{msHasContextMenus = has;}
						/**< Called by XBootStrap::Boot if InitContextualMenus returns an error. */

//-----------------------------------
//	Member Data
//
protected:
	static bool		msHasQuickDraw3D;
	static bool		msHasContextMenus;
};


// ===================================================================================
//	Helper Functions
// ===================================================================================
#if !TARGET_CARBON
CORE_EXPORT bool TrapAvailable(uint16 theTrap);			
			// returns whether given trap is available
#endif

CORE_EXPORT bool TestGestaltMask(uint32 selector, int32 bitmap);	
			// ask Gestalt whether a feature is available
						
inline bool	TestGestaltBit(uint32 selector, int32 bitNum)		{return TestGestaltMask(selector, 1 << bitNum);}
			// like TestGestaltMask(), but takes a bit number instead of a bit mask

CORE_EXPORT bool	CheckCFMLink(const void* fnPtr);
			// return true if the CFM library function has been successfully linked


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

