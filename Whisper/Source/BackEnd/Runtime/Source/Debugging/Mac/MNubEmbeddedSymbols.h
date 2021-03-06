/*
 *  File:       MNubEmbeddedSymbols.h
 *  Summary:   	Embedded Symbol Extraction (This is used by MStackCrawl.cpp)
 *  Written by: Apple Computer
 *
 *  Copyright � 1995-97 by Apple Computer, Inc., all rights reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: MNubEmbeddedSymbols.h,v $
 *		Revision 1.3  2001/04/13 07:51:35  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.2  2000/11/09 09:16:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <size_t.h>
#include <cstdarg>

#include <Types.h>

namespace Whisper {


//==============================================================================
// Declarations for embedded-symbol stuff
//==============================================================================

typedef unsigned long	ULongWord;
typedef unsigned short	UWord;
typedef unsigned char	UByte;

typedef std::size_t		TargetAddress;

typedef OSErr (*ReadMemFn)(void* loc, ULongWord size, void* buffer, std::va_list args);

#if UNIVERSAL_INTERFACES_VERSION < 0x0301
#define kSymbolNotFound fragSymbolNotFound
#else
#define kSymbolNotFound cfragNoSymbolErr
#endif


extern "C" OSErr LookupPowerPCSym(TargetAddress addr, char* name, TargetAddress* fnBegin, 
					   TargetAddress* fnEnd, ReadMemFn readMemCallback, ...);
extern "C" OSErr Lookup68KSym(TargetAddress addr, char* name, TargetAddress* fnBegin, 
				   TargetAddress* fnEnd, ReadMemFn readMemCallback, ...);


}	// namespace Whisper

