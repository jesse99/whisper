/*
 *  File:       XDebugNew.cpp
 *  Summary:    Debugging version of operator new.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XDebugNew.cpp,v $
 *		Revision 1.3  2001/04/27 04:31:58  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:30:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDebugNew.h>

#include <XDebug.h>
#include <XDebugMalloc.h>

#if WIN && MSVC
	extern "C" int _callnewh(size_t size);
#endif

#if MULTI_FRAGMENT_APP && CODE_WARRIOR && MAC
	namespace std {
//		char		__throws_bad_alloc = 1;	// normally defined in newop.cpp
		nothrow_t 	nothrow;                
	}
#endif

#if WHISPER_OPERATOR_NEW && DEBUG


//-----------------------------------
//	Globals
//
namespace Whisper {
	XDebugMalloc* gDebugMalloc = nil;
	
	bool gValidateHeapOnNew    = false;
	bool gValidateHeapOnDelete = false;
	bool gFreeDeletedBlocks    = true;	
}	


// ===================================================================================
//	Global New and Delete
// ===================================================================================

//---------------------------------------------------------------
//
// operator new (size_t)
//
//---------------------------------------------------------------
void* operator new(std::size_t size) throw(std::bad_alloc)
{
	return Whisper::WhisperOperatorNew(size, false);
}


//---------------------------------------------------------------
//
// operator new (size_t, nothrow_t)
//
//---------------------------------------------------------------
#if MAC
void* operator new(std::size_t size, const std::nothrow_t&) throw()
{
	void* ptr = nil;
	
	try {
   		ptr = Whisper::WhisperOperatorNew(size, false);
   		
   	} catch (...) {
   	}
   	
   	return ptr;
}
#endif


//---------------------------------------------------------------
//
// operator new[] (size_t)
//
//---------------------------------------------------------------
void* operator new[](std::size_t size) throw(std::bad_alloc)
{
	return Whisper::WhisperOperatorNew(size, true);
}


//---------------------------------------------------------------
//
// operator new[] (size_t, nothrow_t)
//
//---------------------------------------------------------------
void* operator new[](std::size_t size, const std::nothrow_t&) throw()
{
	void* ptr = nil;
	
	try {
		ptr = Whisper::WhisperOperatorNew(size, true);
   		
   	} catch (...) {
   	}
   	
   	return ptr;
}


//---------------------------------------------------------------
//
// operator delete 
//
//---------------------------------------------------------------
void operator delete(void* obj)
{
	Whisper::WhisperOperatorDelete(obj, false);
}


//---------------------------------------------------------------
//
// operator delete[]
//
//---------------------------------------------------------------
void operator delete[](void* obj)
{
	Whisper::WhisperOperatorDelete(obj, true);
}

#if __MWERKS__
#pragma mark -
#endif

namespace Whisper {

// ===================================================================================
//	class XDisableLeakChecking
// ===================================================================================

//---------------------------------------------------------------
//
// XDisableLeakChecking::~XDisableLeakChecking
//
// It's be nicer to stub these out in the header, but that causes
// problems if you use them in a library.
//
//---------------------------------------------------------------
XDisableLeakChecking::~XDisableLeakChecking()
{
#if DEBUG
	gDebugMalloc->EnableLeakChecking();
#endif
}


//---------------------------------------------------------------
//
// XDisableLeakChecking::XDisableLeakChecking
//
//---------------------------------------------------------------
XDisableLeakChecking::XDisableLeakChecking()
{
#if DEBUG
	PRECONDITION(gDebugMalloc != nil);
	
	gDebugMalloc->DisableLeakChecking();
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// WhisperOperatorNew
//
//---------------------------------------------------------------
void* WhisperOperatorNew(uint32 size, bool isArray)
{
	if (gDebugMalloc == nil)
		gDebugMalloc = new XDebugMalloc;			// calls XDebugMalloc::operator new

	void* block = nil;

	if (gValidateHeapOnNew)
		gDebugMalloc->ValidateHeap();

	// Try to create the block. Will throw if the new handler can't 
	// free up any memory or there is no new handler.
	while (block == nil) {
		block = gDebugMalloc->Allocate(size, isArray);
		
		if (block == nil) {
#if MSVC >= 1100
			if (!_callnewh(size))							// $$$ MSVC doesn't support set_new_handler...
				throw XSysMemoryException();
#else
			std::new_handler handler = std::set_new_handler(nil);	// there's no get_new_handler!
			std::set_new_handler(handler);
			
			if (handler != nil)
				(*handler)();								// if handler can't free up any memory it will throw
			else
				throw std::bad_alloc();
#endif
		}
	}
	
	return block;
}


//---------------------------------------------------------------
//
// WhisperOperatorDelete
//
//---------------------------------------------------------------
void WhisperOperatorDelete(void* obj, bool isArray)
{
	REQUIRE(gDebugMalloc != nil || obj == nil);
	
	if (gValidateHeapOnDelete)
		gDebugMalloc->ValidateHeap();

	if (gFreeDeletedBlocks)
		if (obj != nil)
			gDebugMalloc->Deallocate(obj, isArray);
}


}		// namespace Whisper
#endif	// WHISPER_OPERATOR_NEW && DEBUG

