/*
 *  File:       XLogFile.h
 *  Summary:    Dumps information out to a file in the app's folder.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLogFile.h,v $
 *		Revision 1.4  2001/03/05 05:42:35  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:53:59  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:42:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XLogFile
//!		Dumps information out to a file in the app's folder.
// ===================================================================================
class CORE_EXPORT XLogFile {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XLogFile();
						
						XLogFile(const char* fileName);
						/**< XMemoryHeap creates a log file after all static objects are
						destroyed so we avoid using Whisper objects like std::wstring. */
						
private:
						XLogFile(const XLogFile& rhs);
						
			XLogFile& 	operator=(const XLogFile& rhs);
						
//-----------------------------------
//	API
//
public:
			void 		Print(const char* format, ...);
			void 		Print(const wchar_t* format, ...);
			
			void 		Flush();
				
//-----------------------------------
//	Internal API
//
public:
			void 		DoOpen();
						/**< The log file isn't opened until something is written. */
						
			void 		DoPrintTimeStamp();

//-----------------------------------
//	Member Data
//
protected:					
	STD::FILE*	mFile;
	
#if MAC
	FSSpec 		mSpec;
#elif WIN
	char 		mPath[_MAX_PATH];
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
