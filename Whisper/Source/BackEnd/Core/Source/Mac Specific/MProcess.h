/*
 *  File:		MProcess.h
 *  Summary:	Some handy functions dealing with the Process Manager.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: MProcess.h,v $
 *		Revision 1.4  2001/03/05 05:39:32  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:48:56  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:18:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Processes.h>

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
//	class MProcess
//!		Some handy functions dealing with the Process Manager.
// ===================================================================================
class CORE_EXPORT MProcess {

//-----------------------------------
//	API
//
public:
	//! @name My App
	//@{
	static bool 		InFront();
						/**< Returns true if the app is in the foreground. */
						
	static void 		PullToFront();
						/**< The app will be brought to the front the next time it calls WNE. */
						
	static void 		Kill();
						/**< Sends a quit apple event to the app. */
	//@}

	//! @name Other Apps
	//@{
	static bool 		InFront(const ProcessSerialNumber& psn);

	static void 		PullToFront(const ProcessSerialNumber& psn);
	
	static void 		Kill(const ProcessSerialNumber& psn);
	//@}

	//! @name Process Serial Numbers
	//@{
	static ProcessSerialNumber GetFrontSerialNumber();
						/**< Returns the serial number of the app in front. */

	static ProcessSerialNumber GetSerialNumber();
						/**< Returns the serial number of my app. */

	static ProcessSerialNumber GetSerialNumber(OSType signature);

	static ProcessSerialNumber GetSerialNumber(OSType type, OSType creator);
	//@}
	
	//! @name Misc
	//@{
	static bool 		IsRunning(OSType signature);

	static bool 		IsRunning(OSType type, OSType creator);

	static ProcessInfoRec GetInfo(const ProcessSerialNumber& psn);
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
