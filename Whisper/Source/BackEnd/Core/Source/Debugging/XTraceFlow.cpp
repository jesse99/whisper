/*
 *  File:       XTrace.cpp
 *  Summary:    A class that allows flexible control over what debug output is printed.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTraceFlow.cpp,v $
 *		Revision 1.3  2001/04/27 04:37:25  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:56:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		  <6>	11/27/99	JDJ		Uses XLogFile.
 *		  <5>	 5/28/99	JDJ		Made thread safe.
 *		  <4>	 4/27/99	JDJ		OutputDebugString is now used with CodeWarrior.
 *		  <3>	 4/19/99	JDJ		ZTraceFile::Print compensates if the file name is too long.
 *		  <2>	 4/18/99	JDJ		Uses a console window with WinCW or when no debugger is present.
 *		  <1>	11/02/97	JDJ		Created (from Raven)
 */
 
#include <XWhisperHeader.h>
#include <XTrace.h>

#if DEBUG

#include <map>

#include <XCriticalSection.h>
#include <XDebug.h>
#include <XLogFile.h>
#include <XTextTranscoder.h>		

#if MAC
	#include <Processes.h>
#elif WIN
	#include <WSystemInfo.h>
#endif

namespace Whisper {

using namespace std;


//-----------------------------------
//	Types
//
typedef map<std::wstring, bool> ZCategoryMap;


//-----------------------------------
//	Variables
//
static ZCategoryMap 	sCategories;
static XCriticalSection	sCategorySection;

static ZCategoryMap::iterator sCursor;

static XCriticalSection	sTraceSection;

#if WIN
	static HANDLE sConsole = nil;
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateConsoleWindow
//
//---------------------------------------------------------------
#if WIN
static void CreateConsoleWindow()
{
	PRECONDITION(sConsole == nil);
		
	// Create the console window
	if (::AllocConsole()) {
	
		// Get the console window's handle
		sConsole = ::GetStdHandle(STD_ERROR_HANDLE);
		if (sConsole == INVALID_HANDLE_VALUE)
			sConsole = nil;
			
		// Set some options
		if (sConsole != nil) {
			VERIFY(::SetConsoleTextAttribute(sConsole, FOREGROUND_GREEN));	// green text on a black background (there doesn't appear to be a way to get black text)
			VERIFY(::SetConsoleTitle("Debug Log"));
			
			COORD size = {80, 120};
			VERIFY(::SetConsoleScreenBufferSize(sConsole, size));
			
		} else
			DEBUGSTR("Couldn't get the console window's handle!");
	
	} else
		DEBUGSTR("Couldn't allocate the console window!");
}
#endif


//---------------------------------------------------------------
//
// OutputString
//
//---------------------------------------------------------------
#if WIN
static void OutputString(const std::wstring& mesg)
{
	if (Whisper::IsDebuggerPresent()) {		
		if (WSystemInfo::HasUnicode())
			OutputDebugStringW(mesg.c_str());	// if you're using CodeWarrior you'll need to enable the "Log System Messages" checkbox to get this working
		else
			OutputDebugStringA(ToPlatformStr(mesg));	
	
	} else {
		if (sConsole == nil) 					// otherwise we'll use a console window
			CreateConsoleWindow();
		
		if (sConsole != nil) {
			uint32 written;
			if (WSystemInfo::HasUnicode())
				VERIFY(WriteConsoleW(sConsole, mesg.c_str(), mesg.length(), &written, nil));
			else
				VERIFY(WriteConsoleA(sConsole, ToPlatformStr(mesg), mesg.length(), &written, nil));
		}
	}
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class UTraceFlow
// ===================================================================================

static XLogFile sFile("Debug Log.txt");

bool XTraceFlow::msEchoToDebugger = true;
bool XTraceFlow::msEchoToFile     = true;
bool XTraceFlow::msEchoAllToFile  = false;

//---------------------------------------------------------------
//
// XTraceFlow::EnableCategory							[static]
//
//---------------------------------------------------------------
void XTraceFlow::EnableCategory(const std::wstring& category)
{	
	XEnterCriticalSection enter(sCategorySection);

	if (sCategories.find(category) != sCategories.end())
		sCategories[category] = true;
	else
		sCategories.insert(ZCategoryMap::value_type(category, true));
}


//---------------------------------------------------------------
//
// XTraceFlow::DisableCategory							[static]
//
//---------------------------------------------------------------
void XTraceFlow::DisableCategory(const std::wstring& category)
{
	XEnterCriticalSection enter(sCategorySection);

	if (sCategories.find(category) != sCategories.end())
		sCategories[category] = false;
	else
		sCategories.insert(ZCategoryMap::value_type(category, false));
}


//---------------------------------------------------------------
//
// XTraceFlow::IsCategoryEnabled						[static]
//
//---------------------------------------------------------------
bool XTraceFlow::IsCategoryEnabled(const std::wstring& category)
{
	bool enabled = true;
		
	XEnterCriticalSection enter(sCategorySection);

	if (sCategories.find(category) != sCategories.end())
		enabled = sCategories[category];
		
	else if (category != L"")
		sCategories.insert(ZCategoryMap::value_type(category, enabled));	
	
	return enabled;
}


//---------------------------------------------------------------
//
// XTraceFlow::GetNumCategories							[static]
//
//---------------------------------------------------------------
uint32 XTraceFlow::GetNumCategories()
{
	XEnterCriticalSection enter(sCategorySection);

	return sCategories.size();
}

	
//---------------------------------------------------------------
//
// XTraceFlow::ResetCursor								[static]
//
//---------------------------------------------------------------
void XTraceFlow::ResetCursor()
{
	XEnterCriticalSection enter(sCategorySection);

	sCursor = sCategories.begin();
}

		
//---------------------------------------------------------------
//
// XTraceFlow::GetNextCategory							[static]
//
//---------------------------------------------------------------
std::wstring XTraceFlow::GetNextCategory()
{
	std::wstring str;
	
	{
	XEnterCriticalSection enter(sCategorySection);
		const ZCategoryMap::value_type& entry = *sCursor;
		++sCursor;
		
		str = entry.first;
	}
	
	return str;
}

		
//---------------------------------------------------------------
//
// XTraceFlow::CursorAtEnd								[static]
//
//---------------------------------------------------------------
bool XTraceFlow::CursorAtEnd()
{
	XEnterCriticalSection enter(sCategorySection);

	return sCursor == sCategories.end();
}


//---------------------------------------------------------------
//
// XTraceFlow::Trace									[static]
//
//---------------------------------------------------------------
void XTraceFlow::Trace(const std::wstring& mesg)
{
	XEnterCriticalSection enter(sTraceSection);

	if (msEchoToDebugger)
#if MAC
		fprintf(stderr, "%s", ToPlatformStr(mesg));
#elif WIN
		Whisper::OutputString(mesg);
#endif

	if (msEchoToFile) {
		sFile.Print("%ls", mesg.c_str());
		sFile.Flush();
	}
}


//---------------------------------------------------------------
//
// XTraceFlow::TraceFlow								[static]
//
//---------------------------------------------------------------
void XTraceFlow::TraceFlow(const std::wstring& category, const std::wstring& mesg)
{
	XEnterCriticalSection enter(sTraceSection);

	if (XTraceFlow::IsCategoryEnabled(category)) {
		if (msEchoToDebugger)
#if MAC
			fprintf(stderr, "%s", ToPlatformStr(mesg));
#elif WIN
			Whisper::OutputString(mesg);
#endif
		
		if (msEchoToFile && !msEchoAllToFile) {
			sFile.Print("%ls", mesg.c_str());
			sFile.Flush();
		}
	}
		
	if (msEchoAllToFile) {
		sFile.Print("%ls", mesg.c_str());
		sFile.Flush();
	}
}


}		// namespace Whisper
#endif	// DEBUG
