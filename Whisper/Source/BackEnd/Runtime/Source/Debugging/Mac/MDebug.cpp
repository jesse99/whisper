/*
 *  File:       XDebug.cpp
 *  Summary:   	Debugging functions and macros.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDebug.cpp,v $
 *		Revision 1.6  2001/07/13 22:08:09  jesjones
 *		Added a fieldWidth argument to some of the ToStr overloads.
 *		
 *		Revision 1.5  2001/04/13 07:49:19  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.4  2001/03/02 11:22:37  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/11/12 07:54:40  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.2  2000/11/09 09:11:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	10/30/00	JDJ		Rewrote IsLowLevelDebuggerPresent using Debugging.h 
 *		 <3>	 9/26/99	JDJ		IsDebuggerPresent was testing to see if IsLowLevelDebuggerPresent 
 *									was nil instead of making a function call. AlertStr now uses
 *									StandardAlert.
 *		 <2>	 2/26/99	JDJ		TRACE and DEBUGSTR functions take wchar_t pointers
 *									instead of char pointers.
 *		 <1>	11/23/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XDebug.h>

#include <cstdarg>
#include <cstdio>
#include <cwchar>

#include <AERegistry.h>
#include <AppleEvents.h>
#include <Controls.h>
#include <Debugging.h>
#include <MetroNubUtils.h>
#include <Memory.h>
#include <Processes.h>
#include <SIOUXWindows.h>
#include <TextEdit.h>
#include <Types.h>
#include <Windows.h>

namespace Whisper {

using namespace std;


//-----------------------------------
//	Globals
//
bool gMonkeyLives = false;				

#if DEBUG
	bool gIntenseDebugging = false;
	bool gBreakOnAssert = true;

	DebugFunction gDebugStrFn = BreakStrToDebugger;			
	DebugFunction gTraceFn = nil;			
	TraceFlowPred gTraceFlowPred = nil;			
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// Length
//
//---------------------------------------------------------------
#if DEBUG
#pragma profile off
static int32 Length(const char* str)		// interrupt code
{
	int32 len = 0;
	
	while (*str++)
		len++;
	
	return len;
}
#pragma profile reset
#endif	// DEBUG


//---------------------------------------------------------------
//
// Append (char*, const char*, int32)
//
//---------------------------------------------------------------
#if DEBUG
#pragma profile off
static void Append(char* buffer, const char* str, int32 bufferLen)		// interrupt code
{
	if (buffer != nil && str != nil) {
		int32 index = Length(buffer);
		
		while (*str && index+1 < bufferLen)
			buffer[index++] = *str++;
			
		buffer[index] = '\0';
	}
}
#pragma profile reset
#endif	// DEBUG


//---------------------------------------------------------------
//
// Append (char*, int32, int32)
//
//---------------------------------------------------------------
#if DEBUG
#pragma profile off
static void Append(char* buffer, int32 num, int32 bufferLen)		// interrupt code
{
	if (buffer != nil) {
		// Convert num to a str starting at index.
		char str[12];
		str[11] = '\0';
		
		bool negative = num < 0;
		
		short index = 11;
		do {
			int32 digit = num % 10;
			num /= 10;
			
			str[--index] = (char) ('0' + (negative ? -digit : digit));
		} while (num != 0 && index > 0);

		if (negative && index > 0)
			str[--index] = '-';
			
		// Do the concatenation
		Append(buffer, str + index, bufferLen);
	}
}
#pragma profile reset
#endif	// DEBUG

#pragma mark -

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// BreakToDebugger
//
//---------------------------------------------------------------
void BreakToDebugger()
{
	Debugger();
}


//---------------------------------------------------------------
//
// BreakStrToDebugger (const char*)
//
// $$$ We should probably be using the new DebugAssert call, but
// $$$ when I tried this with CW 5 there wasn't a stack crawl in
// $$$ the high level debugger. However I tried this with CW 6
// $$$ and CarbonLib at work and it worked fine...
//
//---------------------------------------------------------------
void BreakStrToDebugger(const char* mesg)
{
	unsigned char s[256];
	
	int32 len = (int32) strlen(mesg);
	if (len > 255)
		len = 255;
		
	s[0] = (unsigned char) len;
	BlockMoveData(mesg, s+1, len);
	
	DebugStr(s);
}


//---------------------------------------------------------------
//
// BreakStrToDebugger (const wchar_t*)
//
//---------------------------------------------------------------
void BreakStrToDebugger(const wchar_t* mesg)
{
	unsigned char s[256];
	
	uint32 len = wcslen(mesg);
	if (len > 255)
		len = 255;
		
	s[0] = (unsigned char) len;
	for (uint32 index = 0; index < len; ++index)	
		s[index + 1] = (unsigned char) (mesg[index] & 0xFF);
	
	DebugStr(s);
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

	sprintf(mesg, "ASSERT(%s) in %s at line %d failed.", expr, file, line);
	
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
	char mesg[256];							
	sprintf(mesg, "There has been a fatal error in %s at line %d.", file, line);
	
	BreakStrToDebugger(mesg);			// will pop up an alert if there's no debugger running
	
	ExitToShell();
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
#endif // !DEBUG

		
//---------------------------------------------------------------
//
// SAFE_DEBUGSTR (const char*)
//
//---------------------------------------------------------------
#if DEBUG
#pragma profile off
void SAFE_DEBUGSTR(const char* str)			// interrupt code
{
	unsigned char mesg[81];				// MACSBUG seems to choke on large strings...
	
	int32 len = Length(str);		
	if (len > 80)
		len = 80;
		
	mesg[0] = (unsigned char) len;
	for (uint32 index = 0; index < len; ++index)	
		mesg[index + 1] = (unsigned char) (str[index] & 0xFF);
	
	DebugStr(mesg);
}
#pragma profile reset
#endif	// DEBUG


//---------------------------------------------------------------
//
// SAFE_DEBUGSTR (const char*)
//
//---------------------------------------------------------------
#if DEBUG
#pragma profile off
void SAFE_DEBUGSTR(const char* mesg, int32 num)		// interrupt code
{
	char buffer[256];
	
	buffer[0] = '\0';
	
	Append(buffer, mesg, 256);
	Append(buffer, num, 256);
	
	SAFE_DEBUGSTR(buffer);
}
#pragma profile reset
#endif	// DEBUG


//---------------------------------------------------------------
//
// SafeAssertFailed
//
//---------------------------------------------------------------
#if DEBUG
#pragma profile off
void SafeAssertFailed(const char* expr, const char* file, int line)		// interrupt code
{
	char buffer[256];
	
	buffer[0] = '\0';
	
	Append(buffer, "ASSERT(", 256);
	Append(buffer, expr, 256);
	Append(buffer, ") in ", 256);
	Append(buffer, file, 256);
	Append(buffer, " at line ", 256);
	Append(buffer, line, 256);
	Append(buffer, " failed.", 256);
	
	SAFE_DEBUGSTR(buffer);
}
#pragma profile reset
#endif	// DEBUG


//---------------------------------------------------------------
//
// GetSIOUXWindow
//
// If you're using console.stubs.c you may need to add SIOUXIsAppWindow
// to console.stubs.c (just return false).
//
//---------------------------------------------------------------
#if DEBUG
WindowRef GetSIOUXWindow()
{
	WindowRef window = FrontWindow();
	
	while (window != nil && !SIOUXIsAppWindow(window))
		window = GetNextWindow(window);
	
	return window;
}
#endif	// DEBUG


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
// ToStr (short, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(short value, int32 fieldWidth)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%*d", fieldWidth, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (unsigned short, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(unsigned short value, int32 fieldWidth)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%*d", fieldWidth, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (int, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(int value, int32 fieldWidth)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%*ld", fieldWidth, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (unsigned int, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(unsigned int value, int32 fieldWidth)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%*lu", fieldWidth, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (long, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(long value, int32 fieldWidth)
{
	wchar_t str[32];							
	std::swprintf(str, 32, L"%*ld", fieldWidth, value);
	
	return str;
}


//---------------------------------------------------------------
//
// ToStr (unsigned long, int32)
//
//---------------------------------------------------------------
std::wstring ToStr(unsigned long value, int32 fieldWidth)
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

