/*
 *  File:       XMemUtils.h
 *  Summary:   	Misc memory utilities.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMemUtils.h,v $
 *		Revision 1.2  2000/11/09 12:42:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <3>	 4/19/99	JDJ		Removed BlockMoveData definitions (these are now in XGeneric.h).
 *		 <2>	12/20/98	JDJ		ClearMemory uses BlockZero on Macs (this is about
 *									twice as fast as SetMemory or memset).
 *		 <1>	11/02/97	JDJ		Created (from Raven).
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Allocations
// ===================================================================================
CORE_EXPORT void* PrimitiveNewPtr(uint32 bytes);
			// Allocates memory using the OS (not malloc or operator new!).
			// Returns nil if the memory couldn't be allocated. Note that this should
			// not be used for small allocations (eg on Windows this will round bytes
			// up to the system's page size).

CORE_EXPORT uint32 GetPrimitivePtrSize(const void* ptr);
			// Returns the number of bytes allocated at ptr. Note that this may be 
			// rounded upwards from the number passed into PrimitiveNewPtr.

CORE_EXPORT void PrimitiveFreePtr(void* ptr);
			// Releases the allocated memory. Does nothing if ptr is nil.


// ===================================================================================
//	Munging
// ===================================================================================
CORE_EXPORT void SetMemory(void* adr, uint8 value, uint32 bytes);
	
#if WIN
inline void ClearMemory(void* adr, uint32 bytes)								{SetMemory(adr, 0, bytes);}

#elif MAC
inline void ClearMemory(void* adr, uint32 bytes)								{BlockZero(adr, (int32) bytes);}
#endif

CORE_EXPORT void InvertMemory(void* adr, uint32 bytes);
	
inline bool EqualMemory(const void* first, const void* second, uint32 bytes)	{return STD::memcmp(first, second, bytes) == 0;}


// ===================================================================================
//	Bit twiddling
//		Note that the zero bit is the first bit in memory so bitNum can be arbitrarily 
//		large.
// ===================================================================================
CORE_EXPORT bool TestBit(const void* bytePtr, uint32 bitNum);

CORE_EXPORT void SetBit(void* bytePtr, uint32 bitNum);

CORE_EXPORT void ClearBit(void* bytePtr, uint32 bitNum);

CORE_EXPORT void InvertBit(void* bytePtr, uint32 bitNum);


// ===================================================================================
//	Mac Handles
// ===================================================================================
#if MAC
#if DEBUG
	CORE_EXPORT void ValidateHandle(Handle hand);
#endif

	CORE_EXPORT bool EqualHandle(Handle lhs, Handle rhs);

	CORE_EXPORT bool IsLocked(Handle hand);

	CORE_EXPORT bool IsPurgeable(Handle hand);
	
	CORE_EXPORT bool IsResource(Handle hand);
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

