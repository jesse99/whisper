/*
 *  File:       WDebug.cpp
 *  Summary:   	Debugging functions and macros.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WDebug.cpp,v $
 *		Revision 1.5  2001/03/02 11:23:05  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.4  2000/12/31 12:48:02  jesjones
 *		Fixed some compile errors.
 *		
 *		Revision 1.3  2000/11/20 05:44:47  jesjones
 *		Added std:: to a few function calls.
 *		
 *		Revision 1.2  2000/11/09 12:06:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	 8/26/00	JDJ		Instead of always popping up an alert BreakStrToDebugger calls OutputDebugString
 *									and breaks into the debugger (if one is running).
 *		 <5>	 8/25/00	JDJ		Alerts are now posed from within a thread so that the
 *									thread that caused the debug call can be suspended.
 *		 <4>	 5/28/99	JDJ		Break functions deal with reentrancy more gracefully.
 *		 <3>	 3/04/99	JDJ		Added IsDebuggerPresent95.
 *		 <2>	 2/26/99	JDJ		TRACE and DEBUGSTR functions take wchar_t pointers
 *									instead of char pointers.
 *		 <1>	11/02/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <XDebug.h>

#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <process.h>
#include <tlhelp32.h>
#include <wstring.h>

namespace Whisper {


// MFC uses this instead of DebugBreak (on some configurations at least).
// We use it as well since it drops you into the debugger at the source window 
// (instead of the disasm window).
#if MSVC
	#define MyDebugBreak() _asm {int 3}
#else
	#define MyDebugBreak() asm {int 3}
#endif


//-----------------------------------
//	Types
//
struct SAlertData {
	uint32 		flags;
	const char* mesg;
	const char* title;
};


//-----------------------------------
//	Variables
//
bool gMonkeyLives = false;				

#if DEBUG
	bool gIntenseDebugging = false;
	bool gBreakOnAssert = true;

	DebugFunction gDebugStrFn = BreakStrToDebugger;			
	DebugFunction gTraceFn = nil;			
	TraceFlowPred gTraceFlowPred = nil;			
#endif

static int32 sBreaking = -1;


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// HasUnicode
//
//---------------------------------------------------------------
static bool HasUnicode()							
{
	static bool is = false;
	static bool inited = false;
	
	if (!inited) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (GetVersionEx(&info))
			is = info.dwPlatformId == VER_PLATFORM_WIN32_NT;

		inited = true;
	}
	
	return is;
}


//---------------------------------------------------------------
//
// IsDebuggerPresent95
//
// From March 1999 Windows Developer's Journal. This should only
// be called if we're running on Win 95 (normally I'd add an 
// ASSERT, but that's a bit dicy since this is called by ASSERT...)
//
//---------------------------------------------------------------
static bool IsDebuggerPresent95()
{
	bool present = false;

	const uint32 kDebuggerPresentFlag  = 0x000000001;
	const uint32 kProcessDatabaseBytes = 190;
	const uint32 kOffsetFlags          = 8;
	
	uint32 threadID   = GetCurrentThreadId();
	uint32 processID  = GetCurrentProcessId();
	uint32 obfuscator = 0;
	
#if MSVC
	_asm {
		mov	ax, fs
		mov	es, ax
		mov	eax, 18h
		mov	eax, es:[eax]
		sub	eax, 10h
		xor	eax, [threadID]
		mov	[obfuscator], eax
	}
#else
	asm {
		mov	ax, fs
		mov	es, ax
		mov	eax, 0x18
		mov	eax, es:[eax]
		sub	eax, 0x10
		xor	eax, [threadID]
		mov	[obfuscator], eax
	}
#endif
		
	const uint32* processDatabase = reinterpret_cast<const uint32*>(processID ^ obfuscator);
	if (!IsBadReadPtr(processDatabase, kProcessDatabaseBytes)) {
		uint32 flags = processDatabase[kOffsetFlags];
		present = (flags & kDebuggerPresentFlag) != 0;
	}

	return present;
}


//---------------------------------------------------------------
//
// DoPoseAlert
//
// Pop up the alert inside a thread so that we can suspend the
// thread that asserted.
//
//---------------------------------------------------------------
static unsigned int CALLBACK DoPoseAlert(void* refCon)
{
	const SAlertData* data = static_cast<const SAlertData*>(refCon);
	
	int32 result = MessageBoxA(nil, data->mesg, data->title, data->flags);
		
	return (unsigned int) result;
}


//---------------------------------------------------------------
//
// PoseAlert
//
//---------------------------------------------------------------
static uint32 PoseAlert(uint32 flags, const char* mesg, const char* title)
{
	// Remove pending quit messages so the message box sticks around
	MSG msg;								
	bool quitting = (bool) ::PeekMessage(&msg, nil, WM_QUIT, WM_QUIT, PM_REMOVE);
	
	// Setup the data we'll pass to the thread
	SAlertData data;
	data.flags = flags;
	data.mesg  = mesg;
	data.title = title;

	// Create the thread
	unsigned int id;
	HANDLE threadH = (HANDLE) _beginthreadex(nil,				// security attributes
							 		  		 0,					// stack size
							 		  		 DoPoseAlert,		// entry point
							 		  		 &data,				// refCon
							 		  		 CREATE_SUSPENDED,	// flags
							 		  		 &id);				// thread id
	if (threadH == nil || threadH == INVALID_HANDLE_VALUE)
		MyDebugBreak();

	// Start it up
	uint32 suspendCount = ::ResumeThread(threadH);
	if (suspendCount == 0xFFFFFFFF)
		MyDebugBreak();

	// And block the current thread until it returns
	uint32 result = ::WaitForSingleObject(threadH, INFINITE);
	if (result == WAIT_ABANDONED || result == WAIT_FAILED)
		MyDebugBreak();
	
	// Find out which button the user clicked on
	int succeeded = ::GetExitCodeThread(threadH, &result);
	if (!succeeded)
		MyDebugBreak();
	
	// Do some cleanup
	succeeded = ::CloseHandle(threadH);
	if (!succeeded)
		MyDebugBreak();

	if (quitting)
		PostQuitMessage((int32) msg.wParam);
		
	return result;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// IsDebuggerPresent
//
//---------------------------------------------------------------
bool IsDebuggerPresent()
{
	bool present = false;

	typedef BOOL (WINAPI *IsDebuggerPresentProc)();

	HINSTANCE kernelH = LoadLibrary("KERNEL32.DLL");
	if (kernelH != nil) {								// should never fail			
		IsDebuggerPresentProc proc = (IsDebuggerPresentProc) ::GetProcAddress(kernelH, "IsDebuggerPresent");
		if (proc != nil)								// only present in NT and Win 98
			present = proc() != 0;
		
		else
			present = IsDebuggerPresent95();
	}

	return present;
}


//---------------------------------------------------------------
//
// BreakToDebugger
//
//---------------------------------------------------------------
void BreakToDebugger()
{
	if (InterlockedIncrement(&sBreaking) > 0) {		// don't popup more than one dialog at once
		if (IsDebuggerPresent()) 
			MyDebugBreak();

	} else {
		if (IsDebuggerPresent()) {
			MyDebugBreak();
	
		} else {
			uint32 flags = MB_ABORTRETRYIGNORE +	// want abort and ignore buttons (too bad we can't ditch the retry button...)
						   MB_ICONERROR +			// display the icon for errors
						   MB_DEFBUTTON3 +			// select the ignore button
						   MB_TASKMODAL +			// don't let the user do anything else in the app
						   MB_SETFOREGROUND;		// bring the app to the front
	
			uint32 result = Whisper::PoseAlert(flags, "Press abort to exit the app and ignore to continue.", "Debug Break");
	
			if (result == IDABORT)
				std::abort();
		}
	}

	InterlockedDecrement(&sBreaking);
}


//---------------------------------------------------------------
//
// BreakStrToDebugger (const char*)
//
//---------------------------------------------------------------
void BreakStrToDebugger(const char* mesg)
{
	char text[2500];
	uint32 result;

	uint32 flags = MB_YESNO +			// want yes and no buttons 
				   MB_ICONERROR +		// display the icon for errors
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	if (InterlockedIncrement(&sBreaking) > 0) {	// don't popup more than one dialog at once
		if (IsDebuggerPresent()) 
			MyDebugBreak();

	} else {
		if (IsDebuggerPresent()) {
			OutputDebugStringA(mesg);
		
			MyDebugBreak();
	
		} else {
			strcpy(text, mesg);
			strcat(text, "\nDo you want to continue running the app?");
	
			result = Whisper::PoseAlert(flags, text, "Debug Break");
	
			if (result == IDNO)
				std::abort();
		}
	}
	
	InterlockedDecrement(&sBreaking);
}


//---------------------------------------------------------------
//
// BreakStrToDebugger (const wchar_t*)
//
//---------------------------------------------------------------
void BreakStrToDebugger(const wchar_t* mesg)
{
	wchar_t text[2500];
	char buffer[2500];
	uint32 result;

	uint32 flags = MB_YESNO +			// want yes and no buttons 
				   MB_ICONERROR +		// display the icon for errors
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	if (InterlockedIncrement(&sBreaking) > 0) {	// don't popup more than one dialog at once
		if (IsDebuggerPresent()) 
			MyDebugBreak();

	} else {
		if (IsDebuggerPresent()) {
			std::wcscpy(text, mesg);	
			(void) std::wcstombs(buffer, text, sizeof(buffer));	
						
			OutputDebugStringA(buffer);

			MyDebugBreak();
	
		} else {
			std::wcscpy(text, mesg);
			std::wcscat(text, L"\nDo you want to continue running the app?");
	
			(void) std::wcstombs(buffer, text, sizeof(buffer));
			result = Whisper::PoseAlert(flags, buffer, "Debug Break");
			
			if (result == IDNO)
				std::abort();
		}
	}
	
	InterlockedDecrement(&sBreaking);
}


//---------------------------------------------------------------
//
// AssertFailed
//
//---------------------------------------------------------------
#if DEBUG || ASSERTS_THROW
void AssertFailed(const char* expr, const char* file, int line)
{
	char mesg[512];							

	const char* fileName = file + strlen(file);		// strip off path
	while (fileName > file && fileName[-1] != '\\')
		--fileName;

	std::sprintf(mesg, "ASSERT(%s) in '%s' at line %d failed.", expr, fileName, line);
	
#if DEBUG
	if (gBreakOnAssert)
		BreakStrToDebugger(mesg);
#endif

#if ASSERTS_THROW
	char buffer[512] = L"Internal Error: ";						
	strcat(buffer, mesg);
	
	throw XAssertException(buffer); 
#endif
}
#endif	// DEBUG || ASSERTS_THROW


//---------------------------------------------------------------
//
// RequireFailed
//
//---------------------------------------------------------------
#if !DEBUG
void RequireFailed(const char*, const char* file, int line)
{	
	char mesg[512];							

	const char* fileName = file + strlen(file);		// strip off path
	while (fileName > file && fileName[-1] != '\\')
		--fileName;

	std::sprintf(mesg, "There has been a fatal error in '%s' at line %d.", fileName, line);

	if (IsDebuggerPresent()) {
		OutputDebugStringA(mesg);

		MyDebugBreak();

	} else {
		uint32 flags = MB_OK +				// just an OK button
					   MB_ICONERROR +		// display the icon for errors
					   MB_DEFBUTTON1 +		// select the OK button
					   MB_TASKMODAL +		// don't let the user do anything else in the app
					   MB_SETFOREGROUND;	// bring the app to the front
	
		(void) Whisper::PoseAlert(flags, mesg, "Fatal Error");
	
		std::abort();
	}
}
#endif // !DEBUG


//---------------------------------------------------------------
//
// CallNarrowDebugFunc
//
//---------------------------------------------------------------
#if DEBUG
void CallNarrowDebugFunc(const char* s, DebugFunction proc)
{
	wchar_t text[2048];							
	std::swprintf(text, 2048, L"%s", s);
	proc(text);
}
#endif


//---------------------------------------------------------------
//
// ToStr (bool)
//
//---------------------------------------------------------------
std::wstring ToStr(bool value)
{
	return value ? L"true" : L"false";
}


//---------------------------------------------------------------
//
// ToStr (char)
//
//---------------------------------------------------------------
std::wstring ToStr(char value)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%c", value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (int16, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(int16 value, int32 fieldWidth)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%*d", fieldWidth, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (uint16, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(uint16 value, int32 fieldWidth)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%*d", fieldWidth, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (int32, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(int32 value, int32 fieldWidth)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%*ld", fieldWidth, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (uint32, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(uint32 value, int32 fieldWidth)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%*lu", fieldWidth, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (float, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(float value, int32 precision)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%.*f", precision, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (double, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(double value, int32 precision)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%.*f", precision, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (const char*)
//
//---------------------------------------------------------------
std::wstring ToStr(const char* value)
{
	wchar_t str[2048];							
	std::swprintf(str, 2048, L"%s", value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (std::string)
//
//---------------------------------------------------------------
std::wstring ToStr(const std::string& value)
{
	wchar_t str[2048];							
	std::swprintf(str, 2048, L"%s", value.c_str());
	
	return str;
}


}	// namespace Whisper
