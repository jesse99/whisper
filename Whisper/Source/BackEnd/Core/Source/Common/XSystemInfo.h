/*
 *  File:       XSystemInfo.h
 *	Summary:	Provides information about the system.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSystemInfo.h,v $
 *		Revision 1.4  2001/03/05 05:43:47  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:56:19  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:55:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XSystemInfo
//!		Provides information about the system.
// ===================================================================================
class CORE_EXPORT XSystemInfo {

//-----------------------------------
//	API
//
public:
	//! @name Hardware
	//@{
	static	uint32		GetProcessorCount();

	static	bool 		HasFPU();

	static	uint32 		GetLogicalRAM();
						/**< includes virtual memory */
	
	static	uint32 		GetPhysicalRAM();
						/**< does not include virtual memory */

	static	bool 		IsVirtualMemOn();
	//@}

	//! @name System Info
	//@{
	static 	std::wstring GetAppName();
						/**< Note that this does not include the "exe" extension. */

	static	std::wstring GetComputerName();

	static	std::wstring GetUserName();
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
