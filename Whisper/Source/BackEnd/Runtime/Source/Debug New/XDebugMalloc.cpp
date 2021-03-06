/*
 *  File:       XDebugMalloc.cpp
 *  Summary:    Wrapper around malloc that uses a stack crawl to track leaks and checks for over
 *				and under writes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDebugMalloc.cpp,v $
 *		Revision 1.7  2001/11/25 23:39:34  jesjones
 *		Deallocate zaps the entire block instead of just the user area (so double deletes can be caught). Allocate and Deallocate will now only zap up to 1K of data.
 *		
 *		Revision 1.6  2001/10/04 06:13:23  jesjones
 *		Disabled stack crawl code on OS X.
 *		
 *		Revision 1.5  2001/04/27 04:31:54  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/04/19 04:12:16  jesjones
 *		DumpLeaks only prints out the first 100 leaks (this allows the app to quit a lot faster if there are a zillion leaks).
 *		
 *		Revision 1.3  2000/11/20 05:44:21  jesjones
 *		Added std:: to a few function calls.
 *		
 *		Revision 1.2  2000/11/09 12:30:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDebugMalloc.h>

#if MAC
	#include <cstdio>
	#include <cstring>
	#include <Errors.h>
	#include <Gestalt.h>
	#include <Processes.h>
	#include <TextUtils.h>
	#include <Threads.h>
#endif

#include <climits>
#include <cstdlib>

#include <XDebug.h>
#include <XStackCrawl.h>

#if DEBUG
namespace Whisper {


//-----------------------------------
//	Constants
//
const int32 kStackDepth = 8;			// use an even number to keep data aligned on an 8-byte boundary

const int32 kHeaderLen  = kStackDepth*sizeof(StackFrameID) + 2*sizeof(int32) + 2*sizeof(SMemoryBlock*);
const int32 kTrailerLen = 4;			// can be arbitrarily large

const int32 kOverhead = kHeaderLen + kTrailerLen;

// DebugMalloc zaps memory when it's created and freed. This works well for objects,
// but it's not as useful for buffers and it can cause a performance hit with large
// buffers because it will touch every byte forcing the entire buffer to be committed
// to real memory. So, what we'll do instead is just zap the first N bytes. This should
// catch pretty much every object and most buffers.
const uint32 kMaxZapBytes = 1024;


// ===================================================================================
//	struct SMemoryBlock
// ===================================================================================
struct SMemoryBlock {
	StackFrameID	crawl[kStackDepth];	// stack crawl (first is nil if no stack crawl)
	unsigned		isArray : 1;
	unsigned		bytes   : 31;		// exact size of user data
	int32			marker;				// 0xF1F1F1F1
	
	SMemoryBlock*	prevBlock;
	SMemoryBlock*	nextBlock;
	
	uint8			data[4];			// variable length data
//	uint8			marker2;			// four or more 0xF2 bytes
};


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetBlock (void*)
//
//---------------------------------------------------------------
inline SMemoryBlock* GetBlock(void* ptr)
{
	SMemoryBlock* block = reinterpret_cast<SMemoryBlock*>((uint8*) ptr - kHeaderLen);

	return block;
}


//---------------------------------------------------------------
//
// GetBlock (const void*)
//
//---------------------------------------------------------------
inline const SMemoryBlock* GetBlock(const void* ptr)
{
	const SMemoryBlock* block = reinterpret_cast<const SMemoryBlock*>((uint8*) ptr - kHeaderLen);

	return block;
}


//---------------------------------------------------------------
//
// GetSystemVersion
//
//---------------------------------------------------------------
#if MAC
static uint32 GetSystemVersion()
{	
	uint32 version = 0;
	
	int32 result;
	if (Gestalt(gestaltSystemVersion, &result) == noErr)
		version = (uint32) (result & 0xFFFF);

	return version;
}
#endif


//---------------------------------------------------------------
//
// HasOSX
//
//---------------------------------------------------------------
#if MAC
static bool HasOSX()
{	
	static bool has = GetSystemVersion() >= 0x0A00;

	return has;
}
#endif


//---------------------------------------------------------------
//
// FSSpecOpen
//
//---------------------------------------------------------------
#if MAC
static std::FILE* FSSpecOpen(const FSSpec& spec, const char* mode)
{
	std::FILE* file = nil;

	short oldVolume;
	int32 oldDir;
	OSErr err = HGetVol(nil, &oldVolume, &oldDir);
	if (err == noErr) {

		err = HSetVol(0, spec.vRefNum, spec.parID);
		if (err == noErr) {
			char fName[256];

			int32 len = spec.name[0];
			BlockMoveData(spec.name + 1, fName, len);
			fName[len] = '\0';

			file = std::fopen(fName, mode);
		}

		HSetVol(nil, oldVolume, oldDir);
	}

	return file;
}
#endif


//---------------------------------------------------------------
//
// IsWin95
//
//---------------------------------------------------------------
#if WIN
static bool IsWin95()
{
	static bool is = false;
	static bool inited = false;
	
	if (!inited) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (GetVersionEx(&info)) 
			if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) 
				is = info.dwMinorVersion < 10;

		inited = true;
	}
	
	return is;
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZLockHeap
// ===================================================================================
class ZLockHeap {

public:
						~ZLockHeap();

						ZLockHeap();
						
private:
						ZLockHeap(const ZLockHeap& rhs);
						
			ZLockHeap& 	operator=(const ZLockHeap& rhs);
				
//-----------------------------------
//	Member Data
//
protected:
#if WIN
	static CRITICAL_SECTION	msObject;
	static bool				msInited;
#elif MAC
	static ThreadID			msThread;
	static int32			msLockCount;
#endif
};

#if WIN
	CRITICAL_SECTION	ZLockHeap::msObject;
	bool				ZLockHeap::msInited = false;
#elif MAC
	ThreadID			ZLockHeap::msThread    = kNoThreadID;
	int32				ZLockHeap::msLockCount = 0;
#endif

//---------------------------------------------------------------
//
// ZLockHeap::~ZLockHeap
//
//---------------------------------------------------------------
ZLockHeap::~ZLockHeap()
{
#if WIN
	::LeaveCriticalSection(&msObject);

#elif MAC
	if (GetCurrentThread != nil) {
		if (--msLockCount == 0)
			msThread = kNoThreadID;
	}
#endif
}


//---------------------------------------------------------------
//
// ZLockHeap::ZLockHeap
//
//---------------------------------------------------------------
ZLockHeap::ZLockHeap()
{
#if WIN
	if (!msInited) {
		::InitializeCriticalSection(&msObject);
		msInited = true;
	}
	
	::EnterCriticalSection(&msObject);

#elif MAC
	if (GetCurrentThread != nil) {
		ThreadID thread;
		OSErr err = ::GetCurrentThread(&thread);
		ASSERT(err == noErr);					// probably better to throw, but this is debug code and ThrowIfOSErr isn't available
	
		if (++msLockCount == 1)
			msThread = thread;
		else
			ASSERT(thread == msThread);			// assert if we're yielding inside the allocator
	}
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XDebugMalloc
// ===================================================================================

//---------------------------------------------------------------
//
// XDebugMalloc::~XDebugMalloc
//
//---------------------------------------------------------------
XDebugMalloc::~XDebugMalloc()
{
}


//---------------------------------------------------------------
//
// XDebugMalloc::XDebugMalloc
//
//---------------------------------------------------------------
XDebugMalloc::XDebugMalloc()
{
	COMPILE_CHECK(offsetof(SMemoryBlock, data) % 8 == 0);
	COMPILE_CHECK(offsetof(SMemoryBlock, data) == kHeaderLen);

	mBlockCount = 0;
	mLeakCount  = 0;
				
	mFirstBlock = nil;
	
	mHighwater     = 0;
	mCurrentBytes  = 0;
	mDebugOverhead = 0;

	mZap           = true;
	mLeakChecking  = 0;	
	mUseStackCrawl = true;
	
	for (uint32 index = 0; index < 256; index++) {
		mTotalBlockCounts[index]   = 0;
		mCurrentBlockCounts[index] = 0;
	}
}


//---------------------------------------------------------------
//
// XDebugMalloc::GetTotalBlockCount
//
//---------------------------------------------------------------
uint32 XDebugMalloc::GetTotalBlockCount(uint32 size) const
{
	PRECONDITION(size < 256); 
	
	uint32 count = 0;

	uint32 bytes = ((size + 3) & ~3) + kOverhead;
	if (bytes < 256)
		count = mTotalBlockCounts[bytes];
	
	return count;
}


//---------------------------------------------------------------
//
// XDebugMalloc::GetCurrentBlockCount
//
//---------------------------------------------------------------
uint32 XDebugMalloc::GetCurrentBlockCount(uint32 size) const
{
	PRECONDITION(size < 256); 
	
	uint32 count = 0;
	
	uint32 bytes = ((size + 3) & ~3) + kOverhead;
	if (bytes < 256)
		count = mCurrentBlockCounts[bytes];
	
	return count;
}


//---------------------------------------------------------------
//
// XDebugMalloc::Allocate
//
//---------------------------------------------------------------
void* XDebugMalloc::Allocate(uint32 actualBytes, bool isArray)
{
	PRECONDITION(actualBytes < 16*1024L*1024L);
	
	SMemoryBlock* block = nil;
	
	ZLockHeap locker;

	// Round bytes up to the next 4-byte boundary (so that our
	// debugging and other bookkeeping fields are aligned) and
	// make room for our debugging info.
	uint32 bytes = ((actualBytes + 3) & ~3) + kOverhead;
	if (bytes == 0)
		bytes = 4;
	
	block = reinterpret_cast<SMemoryBlock*>(std::malloc(bytes));
			
	// If the block was allocated we:
	if (block != nil) {	

		// add a header, trailer, and stack crawl cookie
		this->DoAddDebugInfo(block, actualBytes, isArray);	

		// fill the data portion of the block with D3's
		if (mZap)
			std::memset(block->data, 0xD3, std::min((uint32) block->bytes, kMaxZapBytes));

		// link in the block
		if (mFirstBlock == nil) {
			mFirstBlock = block;
			mFirstBlock->prevBlock = block;
			mFirstBlock->nextBlock = block;

		} else {
			block->prevBlock = mFirstBlock->prevBlock;
			block->nextBlock = mFirstBlock;

			mFirstBlock->prevBlock->nextBlock = block;
			mFirstBlock->prevBlock = block;
		}
				
		// and update some member data used for statistical purposes.
		if (this->IsLeakChecked(block->data))
			++mLeakCount;
			
		mCurrentBytes  += bytes;				// doesn't include malloc overhead
		mDebugOverhead += kOverhead;		
		++mBlockCount;

		if (mCurrentBytes > mHighwater)
			mHighwater = mCurrentBytes;

		if (bytes < 256) {
			mTotalBlockCounts[bytes]   += 1;
			mCurrentBlockCounts[bytes] += 1;
		}
	}
						
	return block != nil ? block->data : nil;
}


//---------------------------------------------------------------
//
// XDebugMalloc::Deallocate
//
//---------------------------------------------------------------
void XDebugMalloc::Deallocate(void* ptr, bool isArray)
{
	if (ptr != nil) {
		ZLockHeap locker;

		SMemoryBlock* block = GetBlock(ptr);
		
		// Make sure the block is OK,
		ASSERT(isArray == block->isArray);
		ASSERT(mFirstBlock != nil);

		this->ValidateBlock(block->data);
		
		// update our statistics,
		if (this->IsLeakChecked(block->data)) {
			ASSERT(mLeakCount > 0);
			--mLeakCount;
		}

		uint32 bytes = ((block->bytes + 3) & ~3) + kOverhead;
		ASSERT(mCurrentBytes >= bytes);
		
		mCurrentBytes -= bytes;
		mDebugOverhead -= kOverhead;
		--mBlockCount;
		
		if (bytes < 256) {
			ASSERT(mCurrentBlockCounts[bytes] > 0);
			mCurrentBlockCounts[bytes] -= 1;
		}
		
		// unlink the block,
		block->prevBlock->nextBlock = block->nextBlock;
		block->nextBlock->prevBlock = block->prevBlock;

		if (block == mFirstBlock)
			mFirstBlock = mBlockCount == 0 ? nil : block->nextBlock;
		
#if DEBUG
		if (mBlockCount > 1) {
			ASSERT(mFirstBlock != nil);
			ASSERT(mFirstBlock->prevBlock != mFirstBlock);
			ASSERT(mFirstBlock->nextBlock != mFirstBlock);
			
		} else if (mBlockCount == 1) {
			ASSERT(mFirstBlock != nil);
			ASSERT(mFirstBlock->prevBlock == mFirstBlock);
			ASSERT(mFirstBlock->nextBlock == mFirstBlock);
		
		} else
			ASSERT(mFirstBlock == nil);
#endif

		// fill the entire block with F3's,
		if (mZap)
			std::memset(block, 0xF3, std::min(block->bytes + kOverhead, kMaxZapBytes));

		// and free the memory.
		std::free(block);
	}
}


//---------------------------------------------------------------
//
// XDebugMalloc::ValidateHeap
//
//---------------------------------------------------------------
void XDebugMalloc::ValidateHeap() const
{
	ZLockHeap locker;

	if (mFirstBlock != nil) {
		const SMemoryBlock* block = mFirstBlock;
		do {
			this->ValidateBlock(block->data);
			
			block = block->nextBlock;
		} while (block != mFirstBlock);
	}
}


//---------------------------------------------------------------
//
// XDebugMalloc::ValidateBlock
//
//---------------------------------------------------------------
void XDebugMalloc::ValidateBlock(const void* ptr) const
{
	ASSERT(ptr != nil);
			
	const SMemoryBlock* block = GetBlock(ptr);

	if (block->crawl[0] == nil)
		for (uint32 index = 1; index < kStackDepth; index++)
			ASSERT(block->crawl[index] == nil);
						
	ASSERT(block->prevBlock != nil);
	ASSERT(block->nextBlock != nil);
	ASSERT(((uint32) block->prevBlock & 3) == 0);
	ASSERT(((uint32) block->nextBlock & 3) == 0);

	ASSERT(block->marker == 0xF1F1F1F1);

	const uint8* trailer = reinterpret_cast<const uint8*>(block) + kHeaderLen + block->bytes;
	const uint8* end     = trailer + kTrailerLen;

	while (trailer < end)
		ASSERT(*trailer++ == 0xF2);
}


//---------------------------------------------------------------
//
// XDebugMalloc::IsLeakChecked
//
//---------------------------------------------------------------
bool XDebugMalloc::IsLeakChecked(const void* ptr) const
{
	PRECONDITION(ptr != nil);
		
	const SMemoryBlock* block = GetBlock(ptr);
	bool leakChecked = block->crawl[0] != nil;
	
	return leakChecked;
}


//---------------------------------------------------------------
//
// XDebugMalloc::DumpLeaks
//
//---------------------------------------------------------------
void XDebugMalloc::DumpLeaks() const
{
	std::FILE* file = nil;	

	file = this->DoOpenLeaksLog();
	if (file != nil) {
		if (mLeakCount == 0)
			fprintf(file, "No memory leaks.\n");
			
		else {
#if WIN
			if (IsWin95())
				fprintf(file, "Win95 doesn't have symbol names: you'll have to use the map file to determine the function names.\n\n");
#endif

			if (!mUseStackCrawl)
				fprintf(file, "Stack crawl is disabled (this is typically done in the bootstrap code).\n\n");

			if (mLeakCount == 1)
				fprintf(file, "You have one leak:\n\n");
			else if (mLeakCount < 100)
				fprintf(file, "You have %d leaks (oldest first):\n\n", mLeakCount);
			else
				fprintf(file, "You have over 100 leaks (oldest first):\n\n", mLeakCount);

			uint32 count = 0;
			const SMemoryBlock* block = mFirstBlock;
			do {
				this->DoDumpLeak(block->data, file);
				
				block = block->nextBlock;
			} while (block != mFirstBlock && ++count < 100);
		}

		fclose(file);

	} else if (mLeakCount > 0)
		DEBUGSTR("You have leaks, but there was an error opening the 'Leaks Log.txt' file");
}


//---------------------------------------------------------------
//	
// XDebugMalloc::operator new								[static]
//
//---------------------------------------------------------------
void* XDebugMalloc::operator new(std::size_t size)
{
	void* ptr = std::malloc(size);
	REQUIRE(ptr != nil);
	
	return ptr;
}


//---------------------------------------------------------------
//
// XDebugMalloc::operator delete							[static]
//
//---------------------------------------------------------------
void XDebugMalloc::operator delete(void* ptr)
{
	if (ptr != nil)
		std::free(ptr);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDebugMalloc::DoAddDebugInfo
//
//---------------------------------------------------------------
void XDebugMalloc::DoAddDebugInfo(SMemoryBlock* block, uint32 actualBytes, bool isArray)
{
	PRECONDITION(block != nil);
		
	block->bytes   = actualBytes;
	block->isArray = isArray;
	block->marker  = 0xF1F1F1F1;
	
	uint32 index;
	for (index = 0; index < kStackDepth; index++)
		block->crawl[index] = nil;

	uint8* trailer = reinterpret_cast<uint8*>(block) + kHeaderLen + actualBytes;
	uint8* end     = trailer + kTrailerLen;

	while (trailer < end)
		*trailer++ = 0xF2;
		
	if (this->IsLeakChecking()) {	
#if MAC
		if (mUseStackCrawl && !HasOSX()) {			// $$$ need to rewrite stack crawl code for OS X
#else
		if (mUseStackCrawl) {
#endif
			XStackCrawl crawl(4, kStackDepth);
	
			uint32 count = crawl.GetNumFrames();			// stack depth may be smaller than kStackDepth
			for (index = 0; index < count; index++) 
				block->crawl[index] = crawl.GetID(index);	
			
		} else 
			block->crawl[0] = (StackFrameID) 1;	
	}
}


//---------------------------------------------------------------
//
// XDebugMalloc::DoDumpLeak
//
//---------------------------------------------------------------
void XDebugMalloc::DoDumpLeak(const void* ptr, std::FILE* file) const
{		
	this->ValidateBlock(ptr);
	
	const SMemoryBlock* block = GetBlock(ptr);
		
	if (block->crawl[0] != nil && block->crawl[0] != (StackFrameID) 1) {
		fprintf(file, "Block size = %d\n", block->bytes);
		
		for (uint32 index = kStackDepth - 1; index != ULONG_MAX; index--) {
			StackFrameID id = block->crawl[index];
			if (id != nil) {									// stack may be smaller than kStackDepth
				SStackFrame frame = XStackCrawl::GetFrame(id);
												
				fprintf(file, "%s + 0x%lX\n", frame.name, frame.offset);
			}
		}
		fprintf(file, "\n");
	}
}


//---------------------------------------------------------------
//
// XDebugMalloc::DoOpenLeaksLog
//
// Because this function is called after all objects are destroyed
// we have to be very careful about what we do here...
//
//---------------------------------------------------------------
std::FILE* XDebugMalloc::DoOpenLeaksLog() const
{
	std::FILE* file = nil;

#if MAC
	FSSpec spec;

	ProcessSerialNumber psn;
	OSErr err = GetCurrentProcess(&psn);
	if (err == noErr) {	
		ProcessInfoRec info;		
		info.processInfoLength = sizeof(info);
		info.processName       = 0;
		info.processAppSpec    = &spec;

		err = GetProcessInformation(&psn, &info);
		if (err == noErr) {	
			unsigned char* name = "\pLeaks Log.txt";
			BlockMoveData(name, spec.name, name[0] + 1L);

			file = FSSpecOpen(spec, "w");	
		}
	}

#elif WIN
	// Get a path to the exe.
	TCHAR path[MAX_PATH];
	uint32 succeeded = ::GetModuleFileName(GetModuleHandle(nil), path, sizeof(path));
	if (succeeded) {

		// Find out where the app name starts.
		uint32 pos = ULONG_MAX;	
		for (uint32 j = _tcslen(path) - 1; pos == ULONG_MAX && j < ULONG_MAX; --j) {
			if (path[j] == '\\')
				pos = j;
		}

		// Copy "Leaks.log" over the app name.
		_tcscpy(path + pos + 1, _T("Leaks Log.txt"));

		// Open the file.
		file = std::_tfopen(path, "w");
	}
#endif

	return file;
}


}		// namespace Whisper
#endif	// DEBUG

