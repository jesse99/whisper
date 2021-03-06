/*
 *  File:       WStackCrawl.cpp
 *  Summary:   	Cross platform stack crawl class.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WStackCrawl.cpp,v $
 *		Revision 1.3  2001/04/21 03:33:57  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:12:01  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 9/04/99	JDJ		Instead of asm code GetCurrentThread is used to get the PC address.
 *		 <3>	 5/27/99	JDJ		Symbol info buffer is no longer static.
 *		 <2>	 4/18/99	JDJ		GetLogicalAddress checks for nil module handle.
 *									GetFrame checks GetLogicalAddress return flag.
 *		 <1>	10/05/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XStackCrawl.h>

#include <algorithm>

#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetAppInstance					
//
//---------------------------------------------------------------
static HINSTANCE GetAppInstance()			// $$$ don't think this will work in a multi-fragment app				
{
	static HINSTANCE instance = nil;
	
	if (instance == nil) {
		instance = GetModuleHandle(nil);	// per _Advanced Windows_ by Richter
		ASSERT(instance != nil);
	}
	
	return instance;
}


//---------------------------------------------------------------
//
// GetLogicalAddress
// 
// From Microsoft Systems Journal, "Under the Hood", May edition.
//
//---------------------------------------------------------------
static bool GetLogicalAddress(const void* addr, char* moduleName, uint32 len, uint32& section, uint32& offset )
{
    MEMORY_BASIC_INFORMATION memoryInfo;
    if (!VirtualQuery(addr, &memoryInfo, sizeof(memoryInfo)))
        return false;

    uint32 moduleH = (uint32) memoryInfo.AllocationBase;
    if (moduleH == 0)
    	return false;

    if (!GetModuleFileNameA((HMODULE) moduleH, moduleName, len))
        return false;

    // Point to the DOS header in memory
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER) moduleH;

    // From the DOS header, find the NT (PE) header
    PIMAGE_NT_HEADERS peHeader = (PIMAGE_NT_HEADERS) (moduleH + dosHeader->e_lfanew);

    PIMAGE_SECTION_HEADER sectionPtr = IMAGE_FIRST_SECTION(peHeader);

    uint32 rva = (uint32) addr - moduleH; // RVA is offset from module load address

    // Iterate through the section table, looking for the one that encompasses
    // the linear address.
    for (uint32 i = 0; i < peHeader->FileHeader.NumberOfSections; i++, sectionPtr++) {
        uint32 sectionStart = sectionPtr->VirtualAddress;
        uint32 sectionEnd = sectionStart + std::max(sectionPtr->SizeOfRawData, sectionPtr->Misc.VirtualSize);

        // Is the address in this section???
        if (rva >= sectionStart && rva <= sectionEnd) {
            // Yes, address is in the section.  Calculate section and offset,
            // and store in the "section" & "offset" params, which were
            // passed by reference.
            section = i+1;
            offset = rva - sectionStart;
            return true;
        }
    }
    
    DEBUGSTR("Should never get here!");

    return false;  
}


//---------------------------------------------------------------
//
// GetModuleBase
// 
// From MFC 6.0. MFC uses this instead of SymGetModuleBase, but 
// it's far too slow to use with leak checking (for some reason 
// SymLoadModule is called over and over for the same modules).
//
//---------------------------------------------------------------
static uint32 __stdcall GetModuleBase(HANDLE processH, uint32 returnAddress)
{
	uint32 baseAddr = 0;
	
	IMAGEHLP_MODULE moduleInfo;
	if (SymGetModuleInfo(processH, returnAddress, &moduleInfo)) {
		baseAddr = moduleInfo.BaseOfImage;

	} else {
		MEMORY_BASIC_INFORMATION memoryInfo;
		uint32 succeeded = VirtualQueryEx(processH, (LPVOID) returnAddress, &memoryInfo, (int32) sizeof(memoryInfo));
//		DEBUGSTR_IF(!succeeded, "Couldn't get info about the return address");
	
		if (succeeded) {
			char file[MAX_PATH] = {0};
			uint32 count = GetModuleFileNameA((HINSTANCE) memoryInfo.AllocationBase, file, (int32) MAX_PATH);

			succeeded = SymLoadModule(processH, nil, count > 0 ? file : nil, nil, (uint32) memoryInfo.AllocationBase, 0);
//			DEBUGSTR_IF(!succeeded, "Couldn't load the sym module info for ", file);

			if (succeeded)
				baseAddr = (uint32) memoryInfo.AllocationBase;
		}
	}

	return baseAddr;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class StackID
//		This is just a cookie: it's only used to provide some type safety in the interface.
// ===================================================================================
class StackID {
};


// ===================================================================================
//	class ZSymInitialize
// ===================================================================================
class ZSymInitialize {

public:
						~ZSymInitialize();
						
						ZSymInitialize();
};


//---------------------------------------------------------------
//
// ZSymInitialize::~ZSymInitialize
//
// $$$ This is a bit tricky: the stack crawl code is currently
// $$$ used only by the leak checking code. The problem with this
// $$$ is that we wind up calling SymGetSymFromAddr after all
// $$$ static objects have been destroyed. If we call SymCleanup
// $$$ here we won't wind up with symbols. For now we'll take the
// $$$ easy route and not call SymCleanup in debug builds (the
// $$$ docs warn that this can cause leaks, but I suspect that
// $$$ this refers to debuggers loading symbol tables for external
// $$$ processes).
//
//---------------------------------------------------------------
ZSymInitialize::~ZSymInitialize()
{
#if !DEBUG
	HANDLE processH = GetCurrentProcess();
	VERIFY(SymCleanup(processH));
#endif
}


//---------------------------------------------------------------
//
// ZSymInitialize::ZSymInitialize
//
//---------------------------------------------------------------
ZSymInitialize::ZSymInitialize()
{
	HANDLE processH = GetCurrentProcess();
	int32 succeeded = SymInitialize(processH, nil, false);
	REQUIRE(succeeded);

#if 1
	char buffer[MAX_PATH];					// don't use WSystemInfo::GetAppPath because of order of initialization problems
	uint32 succeeded2 = ::GetModuleFileNameA(GetAppInstance(), buffer, sizeof(buffer));
	REQUIRE(succeeded2);

	succeeded2 = SymLoadModule(processH,	// process handle
							   nil,			// file handle
							   buffer,		// path to exe
							   nil,			// module name
							   0,			// load address
							   0);			// module size
	REQUIRE(succeeded2);
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XStackCrawl
// ===================================================================================

static ZSymInitialize sSymInitter;

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
	PRECONDITION(numFrames > 0);

	if (numFrames > kMaxFrames)
		numFrames = kMaxFrames;
	
	mNumFrames = 0;

	// Initialize the symbol table manager. (We'll follow MFC's lead
	// and do this each time. It'd be possible to do this using a 
	// helper function, but that's a bit painful in the presence of
	// threads).
//	HANDLE processH = ::GetCurrentProcess();
//	int32 succeeded = ::SymInitialize(processH, nil, false);
//	DEBUGSTR_IF(!succeeded, "Couldn't initialize IMAGEHLP");

	int32 succeeded = true;
	if (succeeded) {

		// Use GetThreadContext to get the addresses of the program
		// counter, stack pointer, and stack frame.
		CONTEXT threadContext;
		threadContext.ContextFlags = CONTEXT_FULL;

 		HANDLE threadH = ::GetCurrentThread();
		succeeded = ::GetThreadContext(threadH, &threadContext);
		DEBUGSTR_IF(!succeeded, "Couldn't get the thread context!");

		if (succeeded) {	
			
			// Initialize the struct used by StackWalk.
			STACKFRAME stackFrame;
			memset(&stackFrame, 0, sizeof(stackFrame)); 

			stackFrame.AddrPC.Offset    = threadContext.Eip;
			stackFrame.AddrPC.Mode		= AddrModeFlat;
			stackFrame.AddrStack.Offset = threadContext.Esp;
			stackFrame.AddrStack.Mode   = AddrModeFlat;
			stackFrame.AddrFrame.Offset = threadContext.Ebp;
			stackFrame.AddrFrame.Mode   = AddrModeFlat;

			// Our caller will hardly ever want to be included in the
			// stack crawl so we'll skip the first frame.
			++startFrame;						

			// Walk the stack and save the program counter address for
			// each of our callers.
			for (uint32 i = 0; i < startFrame + numFrames && succeeded; ++i) { 
				succeeded = StackWalk(IMAGE_FILE_MACHINE_I386,	// machine type
									  GetCurrentProcess(),		// process handle
									  GetCurrentThread(),		// thread handle
									  &stackFrame,				// returned stack frame
									  nil,						// context frame (not needed for Intel)
									  nil,						// ReadMemoryRoutine 
									  SymFunctionTableAccess,	// FunctionTableAccessRoutine 
									  SymGetModuleBase,			// GetModuleBaseRoutine
//									  GetModuleBase,			// GetModuleBaseRoutine
									  nil);						// TranslateAddress 

				if (succeeded && i >= startFrame)
					mFrame[mNumFrames++] = reinterpret_cast<const void*>(stackFrame.AddrPC.Offset);
			} 
		}
	}
}


//---------------------------------------------------------------
//
// XStackCrawl::GetFrame (uint32)
//
//---------------------------------------------------------------
SStackFrame XStackCrawl::GetFrame(uint32 index) const
{
	PRECONDITION(index < mNumFrames);

	return this->GetFrame((StackFrameID) mFrame[index]);
}


//---------------------------------------------------------------
//
// XStackCrawl::GetID
//
//---------------------------------------------------------------
StackFrameID XStackCrawl::GetID(uint32 index) const
{
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

	const void* adr = id;
	
	SStackFrame frame;

	const uint32 kMaxSymbolName = 512;
	uint8 buffer[sizeof(IMAGEHLP_SYMBOL) + kMaxSymbolName];

	IMAGEHLP_SYMBOL* symbolName = reinterpret_cast<IMAGEHLP_SYMBOL*>(buffer);
	symbolName->SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL);
	symbolName->MaxNameLength = kMaxSymbolName;
	
	// Note that SymGetSymFromAddr only works with Win98 and NT 4 or later.
	HANDLE processH = GetCurrentProcess();
	if (SymGetSymFromAddr(processH, (uint32) adr, &frame.offset, symbolName)) { 
		strncpy(frame.name, symbolName->Name, sizeof(frame.name));
		frame.start = (const void *) symbolName->Address;

	} else { 
		char moduleName[MAX_PATH];
		uint32 section = 0;
		uint32 offset = 0;

		// It's possible to use adr with the map file to determine the
		// function name, but it's rather awkward since you have to use
		// the "Rva+Base" column in the map file. By calling GetLogicalAddress
		// we get the address used by the first column in the map file which
		// is much easier to handle.
		if (!GetLogicalAddress(adr, moduleName, sizeof(moduleName), section, offset))
			moduleName[0] = '\0';
			
		frame.start  = (const void *) section;
		frame.offset = offset;
		
		const char* end  = moduleName + strlen(moduleName);	// strip off the path info
		const char* name = end;
		while (name > moduleName && name[0] != '\\')
			--name;
		
		if (name[0] == '\\')
			strncpy(frame.name, name+1, sizeof(frame.name));
		else
			strncpy(frame.name, name, sizeof(frame.name));
	} 

	return frame;
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


}	// namespace Whisper

