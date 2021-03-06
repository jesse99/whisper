/*
 *  File:       XDebugMalloc.h
 *  Summary:    Wrapper around malloc that uses a stack crawl to track leaks and checks for over
 *				and under writes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDebugMalloc.h,v $
 *		Revision 1.3  2000/12/10 08:51:24  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:30:49  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XDebugMalloc.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:51:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 1/10/00	JDJ		Created (from XMemoryHeap)
 */

#pragma once

#include <cstdio>
#include <new>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
struct SMemoryBlock;


// ===================================================================================
//	class XDebugMalloc
//!		Wrapper around malloc that uses a stack crawl to track leaks and checks for over and under writes.
// ===================================================================================
class RUNTIME_EXPORT XDebugMalloc {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XDebugMalloc();

						XDebugMalloc();
						
private:
						XDebugMalloc(const XDebugMalloc& rhs);
						
			XDebugMalloc& operator=(const XDebugMalloc& rhs);
				
//-----------------------------------
//	API
//
public:
	//! @name Allocations
	//@{
			void*		Allocate(uint32 bytes, bool isArray);
						/**< Returns nil if allocation failed. */
						
			void 		Deallocate(void* block, bool isArray);
	//@}
				
	//! @name Heap Info
	//@{			
			uint32 		GetCurrentBytes() const							{return mCurrentBytes;}
						/**< Returns the current number of bytes allocated in the heap. */
						
			uint32 		GetHighwaterMark() const						{return mHighwater;}
						/**< Returns the largest number of bytes ever allocated by the heap. */
									
			uint32 		GetDebugOverhead() const						{return 100*mDebugOverhead/mCurrentBytes;}
						/**< Returns the percent of the current number of allocated bytes used
						by debugging fields. */
	//@}
			
	//! @name Block Info
	//@{
			uint32 		GetBlockCount() const							{return mBlockCount;}
						/**< Returns the current number of allocated blocks. */
						
			uint32 		GetTotalBlockCount(uint32 size) const;
						/**< Returns the number of blocks that were ever allocated with the given size
						(for sizes under 256). */

			uint32 		GetCurrentBlockCount(uint32 size) const;
						/**< Returns the number of currently allocated blocks with the given size (for 
						sizes under 256). */
	//@}
						
	//! @name Zapping
	//@{
			bool 		IsZapping() const								{return mZap;}
						/**< Returns true if new and deleted blocks are being zapped. */
						
			void 		SetZap(bool zap)								{mZap = zap;}
						/**< Zapping defaults to on if DEBUG is true. */
	//@}

	//! @name Validation
	//@{
			void 		ValidateBlock(const void* ptr) const;
			
		 	void 		ValidateHeap() const;
	//@}

	//! @name Leak Checking
	//@{
			bool 		IsLeakChecking() const							{return mLeakChecking > 0;}
			
			bool 		IsLeakChecked(const void* ptr) const;
			
			uint32 		GetLeakCount() const							{return mLeakCount;}

			void 		DumpLeaks() const;

			void		EnableStackCrawl(bool enable = true)			{mUseStackCrawl = enable;}
	//@}

//-----------------------------------
//	Internal API
//
public:
			void 		DisableLeakChecking()							{mLeakChecking--;}

			void 		EnableLeakChecking()							{mLeakChecking++;}
			
	static	void* 		operator new(std::size_t size);
						/**< We're called by the global operator new so we need to ensure
						that we don't call it when we're newed. */

	static	void 		operator delete(void* ptr);
	
protected:			
			void 		DoAddDebugInfo(SMemoryBlock* block, uint32 actualBytes, bool isArray);
				
			void 		DoDumpLeak(const void* ptr, std::FILE* file) const;

			std::FILE* 	DoOpenLeaksLog() const;
			
//-----------------------------------
//	Member Data
//
protected:
	uint32			mBlockCount;
	uint32			mLeakCount;
		
	SMemoryBlock*	mFirstBlock;
	
	uint32			mCurrentBytes;
	uint32			mHighwater;			
	uint32 			mDebugOverhead;
	
	bool			mZap;
	int32			mLeakChecking;
	bool			mUseStackCrawl;

	uint32 			mTotalBlockCounts[256];
	uint32 			mCurrentBlockCounts[256];
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
#endif	// DEBUG