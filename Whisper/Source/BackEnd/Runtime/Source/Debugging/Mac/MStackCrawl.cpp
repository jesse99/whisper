/*
 *  File:       MStackCrawl.cpp
 *  Summary:   	Stack crawl class based on the class in the OpenDoc utilities.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MStackCrawl.cpp,v $
 *		Revision 1.6  2001/04/21 03:28:40  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/13 07:53:24  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.4  2001/02/12 09:45:02  jesjones
 *		DEBUGSTR no longer includes the error number (too much bloat, sigh).
 *		
 *		Revision 1.3  2000/11/27 07:16:26  jesjones
 *		Updated for Pro 6.
 *		
 *		Revision 1.2  2000/11/09 09:19:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <4>	 4/22/99	JDJ		Uses XOptimize.h.
 *		 <3>	 3/05/99	JDJ		Rewrote GetMem using code from Brian Fitzgerald.
 *		 <2>	12/08/98	JDJ		Disabled unmangle.
 *		 <1>	12/06/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XStackCrawl.h>

#include <Errors.h>
#include <Files.h>
#include <LowMem.h>
#include <MachineExceptions.h>
#include <Setjmp.h>
#include <cstring>
#include <cstdio>

#include <MNubEmbeddedSymbols.h>
#include <XDebug.h>

#if 0
	#include <Unmangler.h>				// this isn't handling namespaces correctly (with the 3.2 Universal Headers)		
#else
	inline int unmangle(char *dst, char *src, int limit)
	{
		std::strncpy(dst, src, (unsigned long) limit);
		dst[limit] = '\0';
		return 0;
	}
#endif

namespace Whisper {


//==============================================================================
// Stack-crawl types & constants
//==============================================================================
struct LinkAreaPPC {
	void*	backChain;
	void*	savedCR;
	void*	savedLR;
	void*	reserved;
	void*	savedTOC;
};

struct LinkArea68k {
	void*	backChain;
	void*	returnAddress;
};

union LinkArea {
	LinkAreaPPC fPPC;
	LinkArea68k f68k;
};

const uint32 kMagicA6 = 0xFFFFFFFF;				// Signals 68k->PPC switch
const uint32 kPPCInstrLen = 4;					// Gotta love them RISCs

static volatile bool sReadFailure;
static ExceptionHandlerTPP sOldHandler;


//==============================================================================
// Locating Stack Pointer
//==============================================================================
#ifdef __MWERKS__
	#if __MWERKS__ >= 8
		asm static void* GetSP( ) {
			mr r3,SP
			blr
		}
	#else
		const long kjmp_bufStackFrameIndex = 3;		// For CW8 and earlier, not CW9
	#endif
#elif defined(__MRC__)
	const long kjmp_bufStackFrameIndex = 2;			// For MRC
#else
	#error "Don't know offset of SP in jmp_buf for this compiler"
#endif


// ===================================================================================
//	class StackID
//		This is just a cookie: it's only used to provide some type safety in the interface.
// ===================================================================================
class StackID {
};


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// HandleExceptions
//
//---------------------------------------------------------------
static OSStatus HandleExceptions(ExceptionInformation* exception)
{
	OSStatus err = noErr;
	
	// If this is a memory-related exception (or if there was no
	// previous exception handler), it's ours
	if (sOldHandler == nil || 
		exception->theKind == kAccessException || 
		exception->theKind == kUnmappedMemoryException || 
		exception->theKind == kExcludedMemoryException) {
		sReadFailure = true;
		exception->machineState->PC.lo += 4;     // skip current instruction
	
	// If not, pass it to the previous exception handler, if there 
	// is one. This lets us do things like use the Metrowerks debugger.
	} else
		err = InvokeExceptionHandlerUPP(exception, sOldHandler);
		
	return err;
}


//---------------------------------------------------------------
//
// GetMem
//
// This code (and HandleExceptions) is from Brian Fitzgerald 
// (bfitz@acm.org) who had some help from Hideaki Iimori 
// (iimori@lib.bekkoame.ne.jp).
//
//---------------------------------------------------------------
static OSErr GetMem(void* loc, ULongWord size, void* buffer, std::va_list args )
{
	#pragma unused(args)
	
#if 1
    OSErr err = noErr;			// $$$ not thread safe

    // Install our exception handler and save the old one. 
    sReadFailure = false;
    static ExceptionHandlerTPP ourHandler = NewExceptionHandlerUPP(HandleExceptions);
    sOldHandler = InstallExceptionHandler(ourHandler);

    // If this is a 4-byte read, handle it directly (this is the most
    // common case).
    if (size == 4) {
        *(long *) buffer = *(long *) loc;
        if (sReadFailure)
            err = -1;
    }
    
    // Otherwise, we need to read all the data. We do it one byte at 
    // a time (this could be optimized). Don't use BlockMove because 
    // our exception handler will go off constantly for a bad range.
    else {
        char* p = (char*) loc;
        char* d = (char*) buffer;
        while (size-- > 0 && err == noErr) {
            volatile char ch = *p++;
            if (sReadFailure) 
                err = -1;
			else
	            *d++ = ch;
        }
    }
    
    // Uninstall the exception handler and return the error code.
    InstallExceptionHandler(sOldHandler);
    sOldHandler = nil;
    
    return err;

#else
	// Try to get away without a CPU exception handler by sanity checking:
	// This test does not work when virtual memory is on (which�means the 
	// stack crawl has hex addresses instead of names).
	if (loc < &SystemZone()->heapData || (void*) loc > LMGetBufPtr()
									  || (void*) ((uint32) loc+size) > LMGetBufPtr())
		return -1;
		
	else {
		BlockMoveData(loc, buffer, size);
		return noErr;
	}
#endif
}

#pragma mark -

// ===================================================================================
//	class XStackCrawl
// ===================================================================================

//---------------------------------------------------------------
//
// XStackCrawl::~XStackCrawl
//
//---------------------------------------------------------------
XStackCrawl::~XStackCrawl()
{
}


//---------------------------------------------------------------
//
// XStackCrawl::XStackCrawl
//
//---------------------------------------------------------------
XStackCrawl::XStackCrawl(uint32 startFrame, uint32 numFrames)
{
	PRECONDITION(startFrame < 16*1024L*1024L);
	PRECONDITION(numFrames > 0);
	
	if (numFrames > kMaxFrames)
		numFrames = kMaxFrames;
	
	const void* stackTop = nil;
	
	// Skanky ways to read the CPU registers:
	#if defined(__MWERKS__) && __MWERKS__ >= 8
		stackTop = GetSP();
	#else
	{
		jmp_buf regs;
		(void) setjmp(regs);
		stackTop = regs[kjmp_bufStackFrameIndex];
	}
	#endif
	
	const LinkArea* stackFrame = (LinkArea*) stackTop;
	const LinkArea* lastStackFrame = nil;
	bool isNative = true;
	const void* pc = nil;
	
	// Crawl up the stack:
	uint32 nFrames = 0;
	uint32 endAt = startFrame + numFrames - 1;
	while (stackFrame != nil && stackFrame > lastStackFrame && nFrames <= endAt) {
		const LinkArea *nextStackFrame;
		
		if (!isNative) {
			nextStackFrame = (LinkArea*) stackFrame->f68k.backChain;
			if ((char*) nextStackFrame >= LMGetCurStackBase())
				break;
			else if (((uint32*) nextStackFrame)[-1] == kMagicA6) {	// 68k->PPC switch
				isNative = true;
				stackFrame = nextStackFrame;						// Skip switch frame
			} else
				pc = stackFrame->f68k.returnAddress;
		}

		if (isNative) {
			nextStackFrame = (const LinkArea*) stackFrame->fPPC.backChain;
			if ((char*) nextStackFrame >= LMGetCurStackBase())
				break;
			else if ((uint32) nextStackFrame & 1) {					// PPC->68k switch
				nextStackFrame = (LinkArea*) ((uint32)nextStackFrame -1);
				isNative = false;
				pc = nextStackFrame->f68k.returnAddress;
				nextStackFrame = (const LinkArea*) nextStackFrame->f68k.backChain;
			} else {
				pc = (void*) ((uint32) nextStackFrame->fPPC.savedLR - kPPCInstrLen);
			}
		}

		// If we've reached startFrame record the PC value in our list.
		if (nFrames >= startFrame)
			mFrame[nFrames - startFrame] = (const void*) ((uint32) pc | isNative);

		// Advance to next frame:
		lastStackFrame = stackFrame;
		stackFrame = (LinkArea*) nextStackFrame;
		nFrames++;
		
		// Stop if we've hit the base of the stack:
		uint32 postFrame = (uint32)stackFrame + (isNative ? sizeof(LinkAreaPPC)
														  : sizeof(LinkArea68k));
		if (postFrame >= (uint32) LMGetCurStackBase())
			break;
	}
	
	if (nFrames > startFrame)
		mNumFrames = nFrames - startFrame;
	else
		mNumFrames = 0;						// can happen if allocate blocks within DumpLeaks
}


//---------------------------------------------------------------
//
// XStackCrawl::GetFrame (uint32)
//
//---------------------------------------------------------------
SStackFrame XStackCrawl::GetFrame(uint32 index) const
{
	PRECONDITION(index < 16*1024L*1024L);
	PRECONDITION(index < mNumFrames);
	
	uint32 pc   = (uint32) mFrame[index] & ~1;
	bool native = (bool) ((uint32) mFrame[index] & 1);
	
	return XStackCrawl::DoGetFrame(pc, native);
}


//---------------------------------------------------------------
//
// XStackCrawl::GetID
//
//---------------------------------------------------------------
StackFrameID XStackCrawl::GetID(uint32 index) const
{
	PRECONDITION(index < 16*1024L*1024L);
	PRECONDITION(index < mNumFrames);
		
	StackFrameID id = (StackFrameID) mFrame[index];
	
	return id;
}


//---------------------------------------------------------------
//
// XStackCrawl::GetFrame (StackFrameID)					[static]
//
//---------------------------------------------------------------
SStackFrame XStackCrawl::GetFrame(StackFrameID id)
{
	PRECONDITION(id != nil);
	
	uint32 pc   = (uint32) id & ~1;
	bool native = (bool) ((uint32) id & 1);
	
	return XStackCrawl::DoGetFrame(pc, native);
}


//---------------------------------------------------------------
//
// XStackCrawl::GetCaller								[static]
//
//---------------------------------------------------------------
SStackFrame XStackCrawl::GetCaller()
{
	XStackCrawl crawl(2, 1);
	
	return crawl.GetFrame(0UL);
}


//---------------------------------------------------------------
//
// XStackCrawl::DoGetFrame 								[static]
//
//---------------------------------------------------------------
SStackFrame XStackCrawl::DoGetFrame(uint32 pc, bool native)
{
	uint32 begin, end;
	
	OSStatus err;
	char symbol[256];
	if (native)
		err = LookupPowerPCSym(pc, symbol, &begin, &end, &GetMem);
	else
		err = Lookup68KSym(pc, symbol, &begin, &end, &GetMem);
		
	SStackFrame frame;
	if (err == noErr && symbol[0] != 0) {
		err = unmangle(frame.name, &symbol[1+(symbol[1] == '.')], 255);	// For some reason the traceback names seem to start with "."
		if (err == noErr)
			std::strncpy(frame.name, symbol, sizeof(frame.name));		// name wasn't mangled (and unmangle didn't copy!)
		else if (err < 0)
			DEBUGSTR("Got an error calling unmangle!");
	
	} else
		std::sprintf(frame.name,"%p (%s)", pc, native ? "PPC" : "68k");

//	DEBUGSTR(frame.name);

	frame.start  = (const void*) begin;
	frame.offset = pc - begin;
	frame.native = native;
	
	return frame;
}


}	// namespace Whisper

