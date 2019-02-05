/*
 *  File:       XTraceSinks.cpp
 *  Summary:    Concrete classes for TRACE destinations.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTraceSinks.cpp,v $
 *		Revision 1.5  2001/04/27 09:21:03  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.4  2001/04/27 04:37:32  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/03/09 10:14:48  jesjones
 *		Disabled the close command in the Windows console window (kills the app).
 *		
 *		Revision 1.2  2000/11/09 12:57:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */
 
#include <XWhisperHeader.h>
#include <XTraceSinks.h>

#include <XTranscode.h>

#if WIN
	#include <WSystemInfo.h>
#endif

#if DEBUG
namespace Whisper {

using namespace std;


// ===================================================================================
//	class XDebuggerSink
// ===================================================================================

//---------------------------------------------------------------
//
// XDebuggerSink::~XDebuggerSink
//
//---------------------------------------------------------------
XDebuggerSink::~XDebuggerSink()
{
}


//---------------------------------------------------------------
//
// XDebuggerSink::XDebuggerSink
//
//---------------------------------------------------------------
XDebuggerSink::XDebuggerSink()
{
#if WIN
	mConsoleH = nil;
#endif
}


//---------------------------------------------------------------
//
// XDebuggerSink::OnGetName
//
//---------------------------------------------------------------
const wchar_t* XDebuggerSink::OnGetName() const
{
#if MAC
	return L"SIOUX";
		
#elif WIN
	if (Whisper::IsDebuggerPresent())
		return L"Debugger";
	else 
		return L"Console Window";
#endif
}


//---------------------------------------------------------------
//
// XDebuggerSink::OnPrint
//
//---------------------------------------------------------------
void XDebuggerSink::OnPrint(const wchar_t* str)
{
#if MAC
	fprintf(stderr, "%ls", str);
		
#elif WIN
	if (Whisper::IsDebuggerPresent()) {
		if (WSystemInfo::HasUnicode())
			OutputDebugStringW(str);		// if you're using CodeWarrior you'll need to enable the "Log System Messages" checkbox to get this working
		else
			OutputDebugStringA(ToPlatformStr(str).c_str());	
	
	} else {
		if (mConsoleH == nil) 				// if there's no debugger we'll use a console window
			this->DoCreateConsoleWindow();
		
		if (mConsoleH != nil) {
			uint32 written;
			if (WSystemInfo::HasUnicode())
				VERIFY(WriteConsoleW(mConsoleH, str, std::wcslen(str), &written, nil));
			else
				VERIFY(WriteConsoleA(mConsoleH, ToPlatformStr(str).c_str(), std::wcslen(str), &written, nil));
		}
	}
#endif
}


//---------------------------------------------------------------
//
// XDebuggerSink::DoCreateConsoleWindow
//
//---------------------------------------------------------------
#if WIN
void XDebuggerSink::DoCreateConsoleWindow()
{
	PRECONDITION(mConsoleH == nil);
		
	// Create the console window
	if (::AllocConsole()) {
	
		// Get the console window's handle
		mConsoleH = ::GetStdHandle(STD_ERROR_HANDLE);
		if (mConsoleH == INVALID_HANDLE_VALUE)
			mConsoleH = nil;
			
		// Set some options
		char name[256];
		std::sprintf(name, "Debug Log (%ld)", GetCurrentProcessId());	// need to use a name that won't conflict with any other windows so our search gets the right window
		
		if (mConsoleH != nil) {
			VERIFY(::SetConsoleTextAttribute(mConsoleH, FOREGROUND_GREEN));	// green text on a black background (there doesn't appear to be a way to get black text)
			VERIFY(::SetConsoleTitle(name));
			
			COORD size = {80, 120};
			VERIFY(::SetConsoleScreenBufferSize(mConsoleH, size));
			
		} else
			DEBUGSTR("Couldn't get the console window's handle!");
			
		// Get rid of the Close menu item (selecting it will kill our
		// app). First we need to find the window, but it may not be
		// created for a bit so we have to loop.
		HWND windH = nil;
		for (int32 i = 0; i < 10 && windH == nil; ++i) {	// wait a maximum of one second
			windH = ::FindWindowEx(nil, nil, nil, name);
			if (windH == nil)
				Sleep(100);
		}

		if (windH != nil) {
			VERIFY(::SetConsoleTitle("Debug Log"));

			HMENU menuH = GetSystemMenu(windH, false);
			VERIFY(DeleteMenu(menuH, SC_CLOSE, MF_BYCOMMAND));
		
		} else
			DEBUGSTR("Couldn't find the console window!");
		
	} else
		DEBUGSTR("Couldn't allocate the console window!");
}
#endif


#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XDebugLogSink
// ===================================================================================

//---------------------------------------------------------------
//
// XDebugLogSink::~XDebugLogSink
//
//---------------------------------------------------------------
XDebugLogSink::~XDebugLogSink()
{
}


//---------------------------------------------------------------
//
// XDebugLogSink::XDebugLogSink
//
//---------------------------------------------------------------
XDebugLogSink::XDebugLogSink(const char* name) : mFile(name)
{
}


//---------------------------------------------------------------
//
// XDebugLogSink::OnGetName
//
//---------------------------------------------------------------
const wchar_t* XDebugLogSink::OnGetName() const
{
	return L"Log File";
}


//---------------------------------------------------------------
//
// XDebugLogSink::OnPrint
//
//---------------------------------------------------------------
void XDebugLogSink::OnPrint(const wchar_t* str)
{
	mFile.Print(str);
	mFile.Flush();
}


}		// namespace Whisper
#endif	// DEBUG