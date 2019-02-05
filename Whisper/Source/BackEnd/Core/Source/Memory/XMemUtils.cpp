/*
 *  File:       XMemUtils.cpp
 *  Summary:   	Misc memory utilities.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMemUtils.cpp,v $
 *		Revision 1.4  2001/04/27 04:34:21  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/04/13 08:01:29  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.2  2000/11/09 12:42:42  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XMemUtils.h>

#include <XDebug.h>

#if MAC
	#include <Gestalt.h>
#endif

namespace Whisper {


// ===================================================================================
//	Allocations
// ===================================================================================

//---------------------------------------------------------------
//
// PrimitiveNewPtr
//
//---------------------------------------------------------------
void* PrimitiveNewPtr(uint32 bytes)
{
#if MAC
	void* ptr = NewPtr(bytes);
#elif WIN
     void* ptr = VirtualAlloc(nil, bytes, MEM_COMMIT, PAGE_READWRITE);
#endif

	return ptr;
}


//---------------------------------------------------------------
//
// GetPrimitivePtrSize
//
//---------------------------------------------------------------
uint32 GetPrimitivePtrSize(const void* ptr)
{
	PRECONDITION(ptr != nil);

#if MAC
	uint32 size = (uint32) GetPtrSize((Ptr) ptr);
	ASSERT(MemError() == noErr);

#elif WIN
	MEMORY_BASIC_INFORMATION info;
    uint32 bytesUsed = VirtualQuery(ptr, &info, sizeof(info));
	
	ASSERT(bytesUsed == sizeof(info));
	ASSERT(info.AllocationProtect & PAGE_READWRITE);
	ASSERT(info.State == MEM_COMMIT);

	uint32 size = info.RegionSize;
#endif

	return size;
}


//---------------------------------------------------------------
//
// PrimitiveFreePtr
//
//---------------------------------------------------------------
void PrimitiveFreePtr(void* ptr)
{
	if (ptr != nil) {
#if MAC
		DisposePtr((Ptr) ptr);
		ASSERT(MemError() == noErr);

#elif WIN
		VERIFY(VirtualFree(ptr, 0, MEM_RELEASE));
#endif
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Munging
// ===================================================================================

//---------------------------------------------------------------
//
// SetMemory
//
// Much thanks for this improved version to Scott D. Schmitz (from MacApp)
//
//---------------------------------------------------------------
void SetMemory(void* destPtr, uint8 setVal, uint32 byteCount)
{
#if powerc
	const unsigned long	kBytesPerPunch = 8;
	typedef double		Punch, *PunchPtr;		// select type double for best performance
#else
	const unsigned long		kBytesPerPunch = 4;
	typedef unsigned long	Punch, *PunchPtr;	// select type long for best performance
#endif

	uint8 *begPtr	= (uint8 *) destPtr;
	uint8 *endPtr	= (uint8 *) (begPtr + byteCount);

	// this looks like the right cutoff, since for the optimization 
	// O(n) = kBPP + kBPP/2 + n/kBPP + kBPP/2 = 2*kBPP + n/kBPP
	// a simple linear fill has O(n) = n
	// n > 2*kBPP + n/kBPP if n > 2*kBPP, since 1/kBPP Å 0.

	if (byteCount >= (4 * kBytesPerPunch)) {
		// progress to nearest 4 byte boundary. O(n) = kBytesPerPunch/2
		while ((long) begPtr & (kBytesPerPunch - 1))
			*begPtr++ = setVal;

		// lets get a kBytesPerPunch byte 'punch'. O(n) = kBytesPerPunch
		register PunchPtr punchSetValPtr = (PunchPtr)begPtr;	// save pointer to the first value
#if powerc
		*begPtr++ = setVal;										// fill it in
		*begPtr++ = setVal;
		*begPtr++ = setVal;
		*begPtr++ = setVal;
#endif
		*begPtr++ = setVal;
		*begPtr++ = setVal;
		*begPtr++ = setVal;
		*begPtr++ = setVal;
		register Punch punchSetVal = *punchSetValPtr++;			// pick it up

		// truncate to nearest kBytesPerPunch bytes
		PunchPtr punchEndPtr = (PunchPtr)((long) endPtr & (0xFFFFFFFF - (kBytesPerPunch - 1)));

		// assign in kBytesPerPunch byte chunks what we can. O(n) = n/kBytesPerPunch
		while (punchSetValPtr < punchEndPtr)
			*punchSetValPtr++ = punchSetVal;

		begPtr = (uint8 *) punchSetValPtr;
	}

	// now finish assigning odd bytes. O(n) = kBytesPerPunch/2 if optimization chosen.
	while (begPtr < endPtr)
		*begPtr++ = setVal;
}


//---------------------------------------------------------------
//
// InvertMemory
//
//---------------------------------------------------------------
void InvertMemory(void* adr, uint32 byteCount)
{
	PRECONDITION(adr != nil);
	PRECONDITION(byteCount > 0);

	uint8* ptr = (uint8 *) adr;

	if (((uint32) ptr & 0x1) || (byteCount & 0x3)) {
		uint8* bytes = (uint8 *) ptr;

		while (byteCount--)
			*bytes++ ^= (uint8) (~0x0);

	} else {
		uint32* longs = (uint32 *) ptr;
		uint32 longCount = byteCount >> 2;

		while (longCount--)
			*longs++ ^= (uint32) (~0x0);
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Bit twiddling
// ===================================================================================

//---------------------------------------------------------------
//
// TestBit
//
//---------------------------------------------------------------
bool TestBit(const void* bytePtr, uint32 bit)
{
	uint32	index = (uint32) (bit >> 3);
	uint32	mask = (uint32) (1 << ((bit & 0x07) ^ 0x07));

	return (((uint8 *) bytePtr)[index] & mask) != 0;
}


//---------------------------------------------------------------
//
// SetBit
//
//---------------------------------------------------------------
void SetBit(void* bytePtr, uint32 bit)
{
	uint32	index = (uint32) (bit >> 3);
	uint8	mask = (uint8) (1 << ((bit & 0x07) ^ 0x07));

	((uint8 *) bytePtr)[index] |= mask;
}


//---------------------------------------------------------------
//
// ClearBit
//
//---------------------------------------------------------------
void ClearBit(void* bytePtr, uint32 bit)
{
	uint32	index = (uint32) (bit >> 3);
	uint32	mask = (uint8) (1 << ((bit & 0x07) ^ 0x07));

	((uint8 *) bytePtr)[index] &= (uint8) (~mask);
}


//---------------------------------------------------------------
//
// InvertBit
//
//---------------------------------------------------------------
void InvertBit(void* bytePtr, uint32 bit)
{
	uint32	index = (uint32) (bit >> 3);
	uint8	mask = (uint8) (1 << ((bit & 0x07) ^ 0x07));

	((uint8 *) bytePtr)[index] ^= mask;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Mac Handles
// ===================================================================================

//---------------------------------------------------------------
//
// ValidateHandle
//
//---------------------------------------------------------------
#if MAC && DEBUG
void ValidateHandle(Handle hand)
{
	ASSERT(::IsHandleValid(hand));	
}
#endif // MAC && DEBUG


//---------------------------------------------------------------
//
// EqualHandle
//
//---------------------------------------------------------------
#if MAC
bool EqualHandle(Handle lhs, Handle rhs)
{
	bool eq = false;
	
	if (lhs != nil && rhs != nil) {
#if DEBUG
		ValidateHandle(lhs);
		ValidateHandle(rhs);
#endif

		if (GetHandleSize(lhs) == GetHandleSize(rhs))
			eq = EqualMemory(*lhs, *rhs, (uint32) GetHandleSize(lhs));
		
	} else if (lhs == nil && rhs == nil)
		eq = true;
		
	return eq;
}
#endif // MAC 


//---------------------------------------------------------------
//
// IsLocked
//
//---------------------------------------------------------------
#if MAC
bool IsLocked(Handle hand)
{
#if DEBUG
	ValidateHandle(hand);
#endif
	
	char state = HGetState(hand);
	ASSERT(MemError() == noErr);
	return (state & 0x80) != 0;
}
#endif // MAC


//---------------------------------------------------------------
//
// IsPurgeable
//
//---------------------------------------------------------------
#if MAC
bool IsPurgeable(Handle hand)
{
	PRECONDITION(hand != nil);
	
	char state = HGetState(hand);
	OSErr err = MemError();
	
	if (err == noErr)
		return (state & 0x40) != 0;
	else if (err == nilHandleErr)
		return true;								// Handle has been purged.
	else
		DEBUGSTR("Error ", err, " in IsPurgeable.");
		
	return false;
}
#endif // MAC


//---------------------------------------------------------------
//
// IsResource
//
//---------------------------------------------------------------
#if MAC
bool IsResource(Handle hand)
{
#if DEBUG
	ValidateHandle(hand);				// Might want to treat this like IsPurgeable.
#endif

	char state = HGetState(hand);
	ASSERT(MemError() == noErr);
	return (state & 0x20) != 0;
}
#endif // MAC


}	// namespace Whisper

