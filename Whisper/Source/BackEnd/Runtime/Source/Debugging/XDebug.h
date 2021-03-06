/*
 *  File:       XDebug.h
 *  Summary:   	Debugging functions and macros.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Abstract:	This file provides the following macros:
 *
 *				PRECONDITION (p)
 *				Use this to verify that the system's state allows a call to a function or 
 *				method. Typically you'll use this macro to validate the routine's arguments.
 *
 *				POSTCONDITION (p)
 *				This is used to verify that the routine did what it was supposed to. If the
 *				PRECONDITION checks passed the routine should either throw or do what it's 
 *				supposed to do.
 *
 *				OBSERVE (type, name, variable)
 *				This is used to save a piece of the object's state so that it can be used 
 *				later with the POSTCONDITION macro. For example:
 *					void Names::AddName(const std::string& name) 
 *					{
 *						PRECONDITION(!name.empty());
 *						PRECONDITION(!this->HasName(name));
 *						OBSERVE(uint32, oldCount, mNames.size());
 *						CHECK_INVARIANT;		// this is defined in XInvariant.h
 *						
 *						(void) mNames.insert(name);
 *
 *						POSTCONDITION(oldCount+1 == mCount);
 *						POSTCONDITION(this->HasName(name));
 *					}
 *				
 *				ASSERT (p)
 *				If p is false and DEBUG is true drops into the debugger, if ASSERTS_THROW 
 *				is true throws a XAssertException exception.
 *
 *				VERIFY (p)
 *				Like ASSERT except that p is evaluated in release builds.
 *
 *				REQUIRE (p)
 *				Like VERIFY except that it exits the app if the test fails (this is needed 
 *				because the boot strap code can't safely throw).
 *
 *				COMPILE_CHECK (p)
 *				An ASSERT that fires at compile time. This is useful for doing things like 
 *				verifying the layout of items within a struct. Note that this check is also
 *				done in release builds.
 *
 *				DEBUGSTR (arg1, arg2, ...)
 *				Uses ToStr functions to convert the arguments into a string and drops into 
 *				the debugger with the string.
 *
 *				DEBUGSTR_IF (p, arg1, arg2, ...)
 *				DEBUGSTR that is executed only when the predicate is true.
 *
 *				TRACE (arg1, arg2, ...)
 *				Writes a string to a debug window without suspending the program.
 *
 *				TRACEFLOW (category, formatStr, n1, n2, ...)
 *				Like TRACE, but with the addition of a category string. The category string 
 *				allows users to turn off entire categories of messages. 
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDebug.h,v $
 *		Revision 1.14  2001/07/13 22:08:17  jesjones
 *		Added a fieldWidth argument to some of the ToStr overloads.
 *		
 *		Revision 1.13  2001/04/27 04:24:51  jesjones
 *		Fixed DEBUGSTR so that it works with ToStr functions defined outside the Whisper namespace.
 *		
 *		Revision 1.12  2001/04/21 01:45:59  jesjones
 *		Added PRECONDITION and POSTCONDITION (from XInvariant.h). These no longer call the invariant so they can be used anywhere. Added OBSERVE.
 *		
 *		Revision 1.11  2001/04/17 01:43:26  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.10  2001/04/13 08:01:09  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.9  2001/03/16 10:35:20  jesjones
 *		Includes <string>.
 *		
 *		Revision 1.8  2001/03/13 11:08:30  jesjones
 *		Decorated CallNarrowDebugFunc with RUNTIME_EXPORT.
 *		
 *		Revision 1.7  2001/03/02 11:24:14  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.6  2001/03/01 11:44:16  jesjones
 *		Added std::string and std::wstring overloads to the debug functions to cut down on iostreams bloat.
 *		
 *		Revision 1.5  2001/02/28 10:01:18  jesjones
 *		DEBUGSTR, TRACE, and TRACEFLOW are now in the global namespace.
 *		
 *		Revision 1.4  2000/12/10 04:09:36  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/10 05:39:25  jesjones
 *		Undefed DEBUGSTR to avoid conflicts with the macro in Debugging.h
 *		
 *		Revision 1.2  2000/11/09 12:30:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <9>	10/02/00	JDJ		Removed TRACE_IF and TRACEFLOW_IF. DEBUGSTR and TRACE
 *									are now based on iostream's instead of printf.
 *		 <8>	 7/04/00	JDJ		Rewrote the release VERIFY so that it compiles with warn_no_side_effect enabled.
 *		 <8>	 7/01/00	JDJ		Tweaked COMPILE_CHECK so that it compiles with warn_no_side_effect enabled.
 *		 <7>	 4/19/00	JDJ		Rewrote the ASSERT macros so that they don't break
 *									CW 5's inlining.
 *		 <6>	 1/19/00	JDJ		Moved PRECONDITION, POSTCONDITION, and CHECK_INVARIANT
 *									into XInvariant.h.
 *		 <5>	 9/27/99	JDJ		The CHECK_INVARIANT macro scopes XInvariantMixin.
 *		 <4>	 9/05/99	JDJ		Tweaked release VERIFY for MSVC 6.
 *		 <3>	 7/08/99	JDJ		Rewrote CHECK_INVARIANT so that it's only one statement.
 *		 <2>	 2/26/99	JDJ		TRACE and DEBUGSTR functions take wchar_t pointers
 *									instead of char pointers.
 *		 <1>	11/12/97	JDJ		Created (from Raven).
 */

#pragma once

#include <cassert>
#include <string>

#if MAC
	#include <Debugging.h>
	#include <Windows.h>
	
	#undef DEBUGSTR
#endif

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// Synch with ANSI definitions.
#if DEBUG && defined(NDEBUG)
	#error DEBUG and NDEBUG are out of sync!
#endif

#if !DEBUG && !defined(NDEBUG)
	#error DEBUG and NDEBUG are out of sync!
#endif

// Synch with MSL definitions.
#if __MSL__ && DEBUG != defined(MSIPL_DEBUG_MODE)
	#error DEBUG and MSIPL_DEBUG_MODE are out of sync!
#endif

// Synch with MSVC.
#if MSVC && DEBUG != defined(_DEBUG)
	#error DEBUG and _DEBUG are out of sync!
#endif


namespace Whisper {

extern RUNTIME_EXPORT bool gMonkeyLives;					// true if the monkey is awake

RUNTIME_EXPORT void BreakToDebugger();						// these ignore the DEBUG flag so be very careful how you use them!
RUNTIME_EXPORT void BreakStrToDebugger(const char* mesg);
RUNTIME_EXPORT void BreakStrToDebugger(const wchar_t* mesg);

#if WIN
	RUNTIME_EXPORT bool IsDebuggerPresent();
#endif

RUNTIME_EXPORT std::wstring ToStr(bool value);				// I originally used iostreams, but that caused a ridiculous amount of bloat in object files (but much less so in the exe)
RUNTIME_EXPORT std::wstring ToStr(char value);
inline 		   std::wstring ToStr(wchar_t value)						{return std::wstring(1, value);}
RUNTIME_EXPORT std::wstring ToStr(short value, int32 fieldWidth = 1);
RUNTIME_EXPORT std::wstring ToStr(unsigned short value, int32 fieldWidth = 1);
RUNTIME_EXPORT std::wstring ToStr(int value, int32 fieldWidth = 1);
RUNTIME_EXPORT std::wstring ToStr(unsigned int value, int32 fieldWidth = 1);
RUNTIME_EXPORT std::wstring ToStr(long value, int32 fieldWidth = 1);
RUNTIME_EXPORT std::wstring ToStr(unsigned long value, int32 fieldWidth = 1);
RUNTIME_EXPORT std::wstring ToStr(float value, int32 precision = 6);
RUNTIME_EXPORT std::wstring ToStr(double value, int32 precision = 6);
RUNTIME_EXPORT std::wstring ToStr(const char* value);
inline 		   std::wstring ToStr(const wchar_t* value)					{return value;}
RUNTIME_EXPORT std::wstring ToStr(const std::string& value);
inline 		   std::wstring ToStr(const std::wstring& value)			{return value;}


#ifndef _lint
	#define COMPILE_CHECK(p)					{struct _CC {char a[(p) ? 1 : -1];};} (void) 0
#else
	#define COMPILE_CHECK(p)					
#endif
}	// namespace Whisper

#if DEBUG
	namespace Whisper {
	extern RUNTIME_EXPORT bool gIntenseDebugging;		// defaults to false
	extern RUNTIME_EXPORT bool gBreakOnAssert;			// defaults to true
	
	typedef void (*DebugFunction)(const wchar_t* str);
	typedef bool (*TraceFlowPred)(const char* category);
	
	extern RUNTIME_EXPORT DebugFunction gDebugStrFn;	// inited to BreakStrToDebugger			
	extern RUNTIME_EXPORT DebugFunction gTraceFn;		// inited by XTrace.cpp in the Core package
	extern RUNTIME_EXPORT TraceFlowPred gTraceFlowPred;			
		
	RUNTIME_EXPORT void AssertFailed(const char* expr, const char* file, int line);
	
	#undef assert

	#define ASSERT(p)						(!(p) ? Whisper::AssertFailed(#p, __FILE__, __LINE__) : (void) 0)

	#define ASSERT_IF(p, x)					((p) && !(x) ? Whisper::AssertFailed(#x, __FILE__, __LINE__) : (void) 0)
	#define assert(p) 						ASSERT(p)
	#define VERIFY(p)						ASSERT(p)
	#define REQUIRE(p)						ASSERT(p)
	#define	PRECONDITION(p)					ASSERT(p)
	#define	POSTCONDITION(p)				ASSERT(p)
	
    #define OBSERVE(type, name, var) 		type name(var)

	#if MAC
		WindowRef GetSIOUXWindow();

		// Interrupt safe debug macros
		RUNTIME_EXPORT void SAFE_DEBUGSTR(const char* str);
		RUNTIME_EXPORT void SAFE_DEBUGSTR(const char* str, long num);
		RUNTIME_EXPORT void SafeAssertFailed(const char* expr, const char* file, int line);

		#define SAFE_ASSERT(p)				(!(p) ? Whisper::SafeAssertFailed(#p, __FILE__, __LINE__) : (void) 0)
	#endif	// MAC

	// helpers
	RUNTIME_EXPORT void CallNarrowDebugFunc(const char* s, DebugFunction proc);
	
}	// namespace Whisper
	
	// DEBUGSTR
	template <typename A1>
	inline void DEBUGSTR(const char* s)					{Whisper::CallNarrowDebugFunc(s, Whisper::gDebugStrFn);}
	inline void DEBUGSTR(const std::string& s)			{Whisper::CallNarrowDebugFunc(s.c_str(), Whisper::gDebugStrFn);}
	inline void DEBUGSTR(const wchar_t* s)				{Whisper::gDebugStrFn(s);}
	inline void DEBUGSTR(const std::wstring& s)			{Whisper::gDebugStrFn(s.c_str());}

	template <typename A1>
	inline void DEBUGSTR(const A1& a1)					{using namespace Whisper; DEBUGSTR(ToStr(a1));}

	template <typename A1, typename A2>
	inline void DEBUGSTR(const A1& a1, const A2& a2)	{using namespace Whisper; DEBUGSTR(ToStr(a1) + ToStr(a2));}

	template <typename A1, typename A2, typename A3>
	inline void DEBUGSTR(const A1& a1, const A2& a2, const A3& a3)					{using namespace Whisper; DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3));}

	template <typename A1, typename A2, typename A3, typename A4>
	inline void DEBUGSTR(const A1& a1, const A2& a2, const A3& a3, const A4& a4)	{using namespace Whisper; DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5>
	inline void DEBUGSTR(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)					{using namespace Whisper; DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	inline void DEBUGSTR(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)	{using namespace Whisper; DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	inline void DEBUGSTR(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)					{using namespace Whisper; DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	inline void DEBUGSTR(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)	{using namespace Whisper; DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8));}
	
	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	inline void DEBUGSTR(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)					{using namespace Whisper; DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8) + ToStr(a9));}
	
	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	inline void DEBUGSTR(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)	{using namespace Whisper; DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8) + ToStr(a9) + ToStr(a10));}
	
	// DEBUGSTR_IF
	inline void DEBUGSTR_IF(bool p, const char* s)				{if (p) Whisper::CallNarrowDebugFunc(s, Whisper::gDebugStrFn);}
	inline void DEBUGSTR_IF(bool p, const std::string& s)		{if (p) Whisper::CallNarrowDebugFunc(s.c_str(), Whisper::gDebugStrFn);}
	inline void DEBUGSTR_IF(bool p, const wchar_t* s)			{if (p) Whisper::gDebugStrFn(s);}
	inline void DEBUGSTR_IF(bool p, const std::wstring& s)		{if (p) Whisper::gDebugStrFn(s.c_str());}

	template <typename A1>
	inline void DEBUGSTR_IF(bool p, const A1& a1)				{using namespace Whisper; if (p) DEBUGSTR(ToStr(a1));}

	template <typename A1, typename A2>
	inline void DEBUGSTR_IF(bool p, const A1& a1, const A2& a2)	{using namespace Whisper; if (p) DEBUGSTR(ToStr(a1) + ToStr(a2));}

	template <typename A1, typename A2, typename A3>
	inline void DEBUGSTR_IF(bool p, const A1& a1, const A2& a2, const A3& a3)				{using namespace Whisper; if (p) DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3));}

	template <typename A1, typename A2, typename A3, typename A4>
	inline void DEBUGSTR_IF(bool p, const A1& a1, const A2& a2, const A3& a3, const A4& a4)	{using namespace Whisper; if (p) DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5>
	inline void DEBUGSTR_IF(bool p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)				{using namespace Whisper; if (p) DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	inline void DEBUGSTR_IF(bool p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)	{using namespace Whisper; if (p) DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	inline void DEBUGSTR_IF(bool p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)				{using namespace Whisper; if (p) DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	inline void DEBUGSTR_IF(bool p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)	{using namespace Whisper; if (p) DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8));}
	
	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	inline void DEBUGSTR_IF(bool p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)					{using namespace Whisper; if (p) DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8) + ToStr(a9));}
	
	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	inline void DEBUGSTR_IF(bool p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)	{using namespace Whisper; if (p) DEBUGSTR(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8) + ToStr(a9) + ToStr(a10));}
	
	// TRACE
	inline void TRACE(const char* s)				{Whisper::CallNarrowDebugFunc(s, Whisper::gTraceFn);}
	inline void TRACE(const std::string& s)			{Whisper::CallNarrowDebugFunc(s.c_str(), Whisper::gTraceFn);}
	inline void TRACE(const wchar_t* s)				{Whisper::gTraceFn(s);}
	inline void TRACE(const std::wstring& s)		{Whisper::gTraceFn(s.c_str());}

	template <typename A1>
	inline void TRACE(const A1& a1)					{using namespace Whisper; TRACE(ToStr(a1));}

	template <typename A1, typename A2>
	inline void TRACE(const A1& a1, const A2& a2)	{using namespace Whisper; TRACE(ToStr(a1) + ToStr(a2));}

	template <typename A1, typename A2, typename A3>
	inline void TRACE(const A1& a1, const A2& a2, const A3& a3)					{using namespace Whisper; TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3));}

	template <typename A1, typename A2, typename A3, typename A4>
	inline void TRACE(const A1& a1, const A2& a2, const A3& a3, const A4& a4)	{using namespace Whisper; TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5>
	inline void TRACE(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)					{using namespace Whisper; TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	inline void TRACE(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)	{using namespace Whisper; TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	inline void TRACE(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)					{using namespace Whisper; TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	inline void TRACE(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)	{using namespace Whisper; TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8));}
	
	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	inline void TRACE(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)					{using namespace Whisper; TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8) + ToStr(a9));}
	
	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	inline void TRACE(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)	{using namespace Whisper; TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8) + ToStr(a9) + ToStr(a10));}
	
	// TRACEFLOW
	inline void TRACEFLOW(const char* category, const char* s)				{if (Whisper::gTraceFlowPred(category)) TRACE(s);}
	inline void TRACEFLOW(const char* category, const std::string& s)		{if (Whisper::gTraceFlowPred(category)) TRACE(s);}
	inline void TRACEFLOW(const char* category, const wchar_t* s)			{if (Whisper::gTraceFlowPred(category)) TRACE(s);}
	inline void TRACEFLOW(const char* category, const std::wstring& s)		{if (Whisper::gTraceFlowPred(category)) TRACE(s);}

	template <typename A1>
	inline void TRACEFLOW(const char* category, const A1& a1)				{using namespace Whisper; if (Whisper::gTraceFlowPred(category)) TRACE(ToStr(a1));}

	template <typename A1, typename A2>
	inline void TRACEFLOW(const char* category, const A1& a1, const A2& a2)	{using namespace Whisper; if (Whisper::gTraceFlowPred(category)) TRACE(ToStr(a1) + ToStr(a2));}

	template <typename A1, typename A2, typename A3>
	inline void TRACEFLOW(const char* category, const A1& a1, const A2& a2, const A3& a3)				{using namespace Whisper; if (Whisper::gTraceFlowPred(category)) TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3));}

	template <typename A1, typename A2, typename A3, typename A4>
	inline void TRACEFLOW(const char* category, const A1& a1, const A2& a2, const A3& a3, const A4& a4)	{using namespace Whisper; if (Whisper::gTraceFlowPred(category)) TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5>
	inline void TRACEFLOW(const char* category, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)				{using namespace Whisper; if (Whisper::gTraceFlowPred(category)) TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	inline void TRACEFLOW(const char* category, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)	{using namespace Whisper; if (Whisper::gTraceFlowPred(category)) TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	inline void TRACEFLOW(const char* category, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)				{using namespace Whisper; if (Whisper::gTraceFlowPred(category)) TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7));}

	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	inline void TRACEFLOW(const char* category, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)	{using namespace Whisper; if (Whisper::gTraceFlowPred(category)) TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8));}
	
	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	inline void TRACEFLOW(const char* category, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)					{using namespace Whisper; if (Whisper::gTraceFlowPred(category)) TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8) + ToStr(a9));}
	
	template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	inline void TRACEFLOW(const char* category, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)	{using namespace Whisper; if (Whisper::gTraceFlowPred(category)) TRACE(ToStr(a1) + ToStr(a2) + ToStr(a3) + ToStr(a4) + ToStr(a5) + ToStr(a6) + ToStr(a7) + ToStr(a8) + ToStr(a9) + ToStr(a10));}	

#else	// #if DEBUG
	inline void DUMMY_TRACE(...)			{}

	#define DEBUGSTR						1 ? ((void) 0) : DUMMY_TRACE
	#define TRACE							1 ? ((void) 0) : DUMMY_TRACE
	#define	TRACEFLOW						1 ? ((void) 0) : DUMMY_TRACE

	#define DEBUGSTR_IF						1 ? ((void) 0) : DUMMY_TRACE

	#if ASSERTS_THROW
		namespace Whisper {
		RUNTIME_EXPORT void AssertFailed(const char*, const char*, int);
		}	// namespace Whisper
		
		#define ASSERT(p)					(!(p) ? Whisper::AssertFailed(#p, __FILE__, __LINE__) : (void) 0)
		
		#define ASSERT_IF(p, x)				((p) && !(x) ? Whisper::AssertFailed(#x, __FILE__, __LINE__) : (void) 0)
		#define VERIFY(p)					ASSERT(p)
		#define	PRECONDITION(p)				ASSERT(p)
		#define	POSTCONDITION(p)			ASSERT(p)
		#define OBSERVE(type, name, var) 	type name(var)
	#else
		#define	ASSERT(p)					((void) 0)

		#define	ASSERT_IF(p, x)				((void) 0)
		#define VERIFY(p)					(void) ((p) ? 0 : 0)
		#define	PRECONDITION(p)				((void) 0)
		#define	POSTCONDITION(p)			((void) 0)
    	#define OBSERVE(type, name, var) 	((void)0)
	#endif

	namespace Whisper {
	RUNTIME_EXPORT void RequireFailed(const char*, const char*, int);
	}	// namespace Whisper
	
	#define REQUIRE(p)						(!(p) ? Whisper::RequireFailed(#p, __FILE__, __LINE__) : (void) 0)
	
	#if MAC
		#define SAFE_DEBUGSTR				1 ? ((void) 0) : DUMMY_TRACE
		#define	SAFE_ASSERT(p)				((void) 0)
	#endif	// MAC
#endif	// DEBUG

#if ASSERTS_THROW
	namespace Whisper {
	class RUNTIME_EXPORT XAssertException : public std::logic_error { 
	public: 
			XAssertException(const char* mesg) : std::logic_error(mesg) {}
	};
}	// namespace Whisper
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif


