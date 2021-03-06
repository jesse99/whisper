/*
 *  File:       XTraceSinks.h
 *  Summary:    Concrete classes for TRACE destinations.
 *  Written by: Jesse Jones
 *
 *	Classes:	XDebuggerSink	- Writes to the high level debugger.
 *				XDebugLogSink	- Writes to a log file.
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTraceSinks.h,v $
 *		Revision 1.5  2001/03/17 09:03:32  jesjones
 *		Removed an extra call to #pragma export reset.
 *		
 *		Revision 1.4  2001/03/05 05:44:15  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:56:57  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:57:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XLogFile.h>
#include <XTraceSink.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XDebuggerSink
//!		Writes to the high level debugger.
// ===================================================================================
class CORE_EXPORT XDebuggerSink : public XTraceSink {

	typedef XTraceSink Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDebuggerSink();
						
						XDebuggerSink();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnPrint(const wchar_t* str);
	
	virtual const wchar_t* OnGetName() const;

//-----------------------------------
//	Internal API
//
protected:
#if WIN
			void 		DoCreateConsoleWindow();
						/**< Uses a console window is there isn't a debugger running. */
#endif

//-----------------------------------
//	Member Data
//
protected:
#if WIN
	HANDLE mConsoleH;
#endif
};



// ===================================================================================
//	class XDebugLogSink
//!		Writes to a log file.
// ===================================================================================
class CORE_EXPORT XDebugLogSink : public XTraceSink {

	typedef XTraceSink Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDebugLogSink();
						
						XDebugLogSink(const char* name);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnPrint(const wchar_t* str);

	virtual const wchar_t* OnGetName() const;

//-----------------------------------
//	Member Data
//
protected:
	XLogFile	mFile;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif


}		// namespace Whisper
#endif	// DEBUG