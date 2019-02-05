/*
 *  File:       MProfiler.cpp
 *  Summary:   	Stack based class used to profile blocks of code.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MProfiler.cpp,v $
 *		Revision 1.1  2001/08/19 22:53:46  jesjones
 *		Checked in
 *		
 *
 *		 <5>	 9/26/99	JDJ		Tweaked the messages in the MProfiler dtor. Ctor takes 
 *									int32's instead of int16's.
 *		 <4>	 7/13/99	JDJ		Corrected the dtor numFunctions and stackDepth checks.
 *		 <3>	 6/15/99	JDJ		GetValidName uses a char for "…" instead of an XChar.
 *		 <2>	12/08/98	JDJ		Fixed some minor compiler errors.
 *		 <1>	12/15/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <MProfiler.h>

#include <cstdio>

#include <XDebug.h>
#include <XExceptions.h>
#include <XStringUtils.h>

#if	__profile__			// all methods are stubbed out in the header if profiling is off

#pragma profile off		// don’t profile these

namespace Whisper {

#if DEBUG						// profiling is often done with DEBUG off so we'll define our own ASSERT to ensure that they're executed
	#define PROFILE_ASSERT		ASSERT
#else
	static void AssertError(const char* expr, const char* file, int32 line)
	{
		char mesg[256];							
		std::sprintf(mesg, "ASSERT(%s) in %s at line %d failed.", expr, file, line);
		
		BreakStrToDebugger(mesg);
	}

	#define PROFILE_ASSERT(x)	do {if (!(x)) AssertError(#x, __FILE__, __LINE__);} while (false)
#endif


//================================================================================
// class MProfiler
//================================================================================

//---------------------------------------------------------------
//
// GetValidName
//
// From MFileSystem.cpp
//
//---------------------------------------------------------------
static std::wstring GetValidName(const std::wstring& name, int32 maxLen = 31)
{
	PROFILE_ASSERT(maxLen <= 31);
	
	std::wstring outName = name;
	
	// Replace colons with dashes (like the Finder).
	for (int16 index = 0; index < outName.length(); index++) {
		if (outName[index] == ':')
			outName[index] = '-';
	}
	
	// Truncate from the middle if the name is too large.
	if (outName.length() > maxLen) {
		std::size_t overflow = (outName.length() - maxLen) + 1;
		
		std::size_t first = outName.length()/2 - overflow/2;	// first character we're going to drop
		std::size_t last  = first + overflow - 1;				// last character we're going to drop
		
		outName = outName.substr(0, first) + kEllipsisChar + outName.substr(last + 1);
		PROFILE_ASSERT(outName.length() <= maxLen);
	}
	
	return outName;
}

#pragma mark -

//================================================================================
// class MProfiler
//================================================================================

bool MProfiler::msConstructed = false;

//---------------------------------------------------------------
//
// MProfiler::~MProfiler
//
//---------------------------------------------------------------
MProfiler::~MProfiler()
{
	char str[256];

	int32 functionSize, stackSize;
	ProfilerGetDataSizes(&functionSize, &stackSize);
	int32 numFunctions = functionSize/12;				// divide by 12 because we're using collectDetailed

	if (numFunctions >= mNumFunctions) {
		std::sprintf(str, "numFunctions is too small. It should be more than %d.", numFunctions);	// Profiling is often done in !DEBUG builds so we can't use TRACE or DEBUGSTR.
		BreakStrToDebugger(str);
	}

	if (stackSize >= mStackSize) {		
		std::sprintf(str, "stackDepth is too small. It should be more than %d.", stackSize);
		BreakStrToDebugger(str);
	}
		
	OSErr err = ProfilerDump(const_cast<unsigned char*>(ToPascalStr(mFileName).c_str()));
	if (err != noErr)
		BreakStrToDebugger("Got an error calling ProfilerDump");

	ProfilerTerm();
	
	msConstructed = false;
}


//---------------------------------------------------------------
//
// MProfiler::MProfiler
//
//---------------------------------------------------------------
MProfiler::MProfiler(const std::wstring& fileName, int32 numFunctions, int32 stackDepth)
{
	PROFILE_ASSERT(fileName.length() > 0);
	PROFILE_ASSERT(numFunctions > 0);
	PROFILE_ASSERT(stackDepth > 0);
	PROFILE_ASSERT(!msConstructed);					// profiler library isn't nestable
	PROFILE_ASSERT(numFunctions < SHRT_MAX);		// profiler uses shorts (we use an assert since not everyone compiles with narrowing warnings enabled)
	PROFILE_ASSERT(stackDepth < SHRT_MAX);

	mFileName = GetValidName(fileName, 29);			// ProfilerDump fails with a weird error if you use colons or the name is too long (CW 11)
	mNumFunctions = numFunctions;
	mStackSize = stackDepth;
	
	OSErr err = ProfilerInit(collectDetailed, bestTimeBase, numFunctions, stackDepth);
	ThrowIfOSErr(err);
	
	mEnableCount = 1;
	
	msConstructed = true;
}


//---------------------------------------------------------------
//
// MProfiler::IsEnabled
//
//---------------------------------------------------------------
bool MProfiler::IsEnabled() const			
{
	return mEnableCount > 0;
}


//---------------------------------------------------------------
//
// MProfiler::Enable
//
//---------------------------------------------------------------
void MProfiler::Enable()
{
	mEnableCount++;
	
	if (mEnableCount == 1)
		ProfilerSetStatus(true);
}


//---------------------------------------------------------------
//
// MProfiler::Disable
//
//---------------------------------------------------------------
void MProfiler::Disable()
{	
	mEnableCount--;
	
	if (mEnableCount == 0)
		ProfilerSetStatus(false);
}

#pragma mark -

//================================================================================
// class MProfileEnabler
//================================================================================

//---------------------------------------------------------------
//
// MProfileEnabler::~MProfileEnabler
//
//---------------------------------------------------------------
MProfileEnabler::~MProfileEnabler()
{
	ProfilerSetStatus(mWasEnabled);
}


//---------------------------------------------------------------
//
// MProfileEnabler::MProfileEnabler
//
//---------------------------------------------------------------
MProfileEnabler::MProfileEnabler(bool enable)
{
	mWasEnabled = (bool) ProfilerGetStatus();
	ProfilerSetStatus(enable);
}


}	// namespace Whisper

#endif	// __profile__

