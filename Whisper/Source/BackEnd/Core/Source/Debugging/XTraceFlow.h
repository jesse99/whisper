/*
 *  File:       XTrace.h
 *  Summary:    A class that allows flexible control over what debug output is printed.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTraceFlow.h,v $
 *		Revision 1.2  2000/11/09 12:56:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		  <1>	11/02/97		JDJ		created (from Raven)
 */

#pragma once

#if DEBUG

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XTraceFlow
// ===================================================================================
class CORE_EXPORT XTraceFlow {

//-----------------------------------
//	API
//
public:
	// ----- Tracing -----
	static 	void 		Trace(const std::wstring& mesg);
						// Note that these functions optionally echo everything to a file.

	static 	void 		TraceFlow(const std::wstring& category, const std::wstring& mesg);

	// ----- Traceflow Category Enabling -----
	static 	void 		EnableCategory(const std::wstring& category);
						// See cpp for a list of the categories in Whisper and the ones 
						// that are enabled by default.
		
	static 	void 		DisableCategory(const std::wstring& category);
	
	static 	bool 		IsCategoryEnabled(const std::wstring& category);
	
	// ----- Iterating over Traceflow Categories -----
	static 	uint32 		GetNumCategories();
	
	static 	void		ResetCursor();
			
	static 	std::wstring GetNextCategory();
			
	static 	bool		CursorAtEnd();
	
//-----------------------------------
//	Data Members
//
public:										// public cuz there's no real reason to encapsulate these
	static bool	msEchoToDebugger;
	static bool	msEchoToFile;
	static bool	msEchoAllToFile;			// if true disabled TRACEFLOW categories get echoed to the debug file
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

#endif	// DEBUG
