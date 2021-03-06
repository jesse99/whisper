/*
 *  File:       XWhisperHeader.h
 *  Summary:   	Whisper precompiled header.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XWhisperHeader.h,v $
 *		Revision 1.9  2001/10/03 02:14:20  jesjones
 *		Added a temporary UI_3D_EXPORT macro.
 *		
 *		Revision 1.8  2001/04/17 01:45:23  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.7  2001/03/14 10:36:38  jesjones
 *		Defined _MSL_NO_LOCALE on the Mac side when building multi-fragment apps.
 *		
 *		Revision 1.6  2001/03/13 11:11:47  jesjones
 *		Several changes to support Intel DLL targets.
 *		
 *		Revision 1.5  2001/03/02 11:20:02  jesjones
 *		No longer includes <iostreams>.
 *		
 *		Revision 1.4  2001/02/28 10:02:25  jesjones
 *		Temporarily added GRAPHICS3D_EXPORT.
 *		
 *		Revision 1.3  2000/11/20 05:28:52  jesjones
 *		Shuffled some includes.
 *		
 *		Revision 1.2  2000/11/09 12:59:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <15>	11/05/00	JDJ		Added <iostream> include.
 *		 <14>	 1/19/00	JDJ		Removed PRECOMPILE_WHISPER_HEADERS, BEGIN_NAMESPACE, and
 *									END_NAMESPACE.
 *		 <13>	10/23/99	JDJ		Added EXPORT define.
 *		 <12>	 8/24/99	JDJ		Moved C++ includes after system includes.
 *		 <11>	 7/31/99	JDJ		Includes ansi_prefix.win32.h or ansi_prefix.mac.h.
 *		 <10>	 6/18/99	JDJ		Defines MSIPL_WCHART on Pro 3.
 *		 <9>	 6/11/99	JDJ		Defines MSL_USE_PRECOMPILED_HEADERS.
 *		 <8>	 5/21/99	JDJ		Fixed two of the PRECOMPILE_WHISPER_HEADERS includes.
 *		 <7>	 4/19/99	JDJ		Added CODE_WARRIOR and MSVC defines.
 *		 <6>	 4/15/99	JDJ		Tweaked for compiling with Win CW.
 *		 <5>	 4/12/99	JDJ		Moved MSIPL_DEBUG_MODE definition from MWhisperHeader
 *									to XWhisperHeader.
 *		 <4>	 4/05/99	JDJ		MAC is now undefined on Windows (works better with MSVC 6).
 *		 <3>	 1/23/99	JDJ		Changed XChar from a uint16 to a wchar_t.
 *		 <2>	12/08/98	JDJ		Added BIG_ENDIAN.
 *		 <1>	 8/01/98	JDJ		Created.
 */
 
// In order for precompiled headers to work on MSVC this header must be included
// before *anything* else.

#if _MSC_VER >= 1100
	#pragma message("Compiling XWhisperHeader.h (this message should only appear once per project)")
#endif

#ifndef	XWHISPERHEADER_H
#define	XWHISPERHEADER_H


// ===================================================================================
//	Whisper Defines
// ===================================================================================
#if macintosh
	#define MAC						1
	#define WIN						0
	#define BIG_ENDIAN				1
#else
//	#define MAC						0			// leave this undefined (causes problems with MSVC 6)
	#define WIN						1
	#define BIG_ENDIAN				0
#endif

#if __MWERKS__
	#define CODE_WARRIOR			__MWERKS__
#elif defined(_MSC_VER)							// note that Win CW defines _MSC_VER to be 900! (in x86_prefix.h)
	#define MSVC					_MSC_VER
#endif

#define WHISPER						0x0200		// Version 2.0.0

#ifdef _DEBUG
	#define	DEBUG					1
	#define RELEASE					0	
	#define __STL_DEBUG							// Enable SGI STL debugging features
#else		
	#define	DEBUG					0
	
	#if __profile__
		#define RELEASE				0
	#else
		#define RELEASE				1	
	#endif
	
	#if	!defined(NDEBUG)
		#define	NDEBUG							// used by <assert.h>
	#endif
#endif

#ifndef ASSERTS_THROW
#define ASSERTS_THROW				0			
#endif

#ifndef WHISPER_OPERATOR_NEW
#if !MULTI_FRAGMENT_APP
#define WHISPER_OPERATOR_NEW 		1
#endif
#endif

#ifndef PRECOMPILE_SYSTEM_HEADERS
#define PRECOMPILE_SYSTEM_HEADERS 	1
#endif

#ifndef PRECOMPILE_STD_HEADERS
#define PRECOMPILE_STD_HEADERS	 	1
#endif

#if __MWERKS__ && MAC
	#define PRAGMA_EXPORT_SUPPORTED	1
#else
	#define PRAGMA_EXPORT_SUPPORTED	0
#endif


// ===================================================================================
//	Types (we define these here to cut circular dependencies between XTypes.h and XString.h)
// ===================================================================================
#ifdef _lint							// didn't used to have to do this...
	#define _MSC_VER	1100
#endif

#if MSVC
	typedef signed char			int8;
	typedef short				int16;
	typedef long				int32;
	typedef __int64				int64;

	typedef unsigned char		uint8;
	typedef unsigned short		uint16;
	typedef unsigned long		uint32;
	typedef unsigned __int64	uint64;

	#include <stdio.h>							 

#elif __MWERKS__
	typedef signed char			int8;
	typedef short				int16;
	typedef long				int32;
	typedef long long			int64;

	typedef unsigned char		uint8;
	typedef unsigned short		uint16;
	typedef unsigned long		uint32;
	typedef unsigned long long	uint64;

#else
	#error unsupported compiler
#endif


// ===================================================================================
//	CodeWarrior Tweaks
// ===================================================================================
#if __MWERKS__
	#if DEBUG
		#define MSIPL_DEBUG_MODE
	#endif
		
	#define MSL_USE_PRECOMPILED_HEADERS 0
	
	#if WIN
		#if MULTI_FRAGMENT_APP
			#include <UseDLLPrefix.h>
		#endif
		#include <ansi_prefix.win32.h>
		
	#elif MAC
		#if MULTI_FRAGMENT_APP
			#define _MSL_NO_LOCALE
		#endif
		#include <ansi_prefix.mac.h>
	#endif
#endif


// ===================================================================================
//	Export Defines
// ===================================================================================
#ifndef RUNTIME_EXPORT
	#if MULTI_FRAGMENT_APP && WIN
		#define RUNTIME_EXPORT 	__declspec(import)
	#else
		#define RUNTIME_EXPORT   
	#endif
#endif

#ifndef CORE_EXPORT
	#if MULTI_FRAGMENT_APP && WIN
		#define CORE_EXPORT 	__declspec(import)	
	#else
		#define CORE_EXPORT   
	#endif
#endif

#ifndef FILES_EXPORT
	#if MULTI_FRAGMENT_APP && WIN
		#define FILES_EXPORT 	__declspec(import)
	#else
		#define FILES_EXPORT   
	#endif
#endif

#ifndef GRAPHICS_EXPORT
	#if MULTI_FRAGMENT_APP && WIN
		#define GRAPHICS_EXPORT __declspec(import)
	#else
		#define GRAPHICS_EXPORT   
	#endif
#endif

#ifndef PARSE_EXPORT
	#if MULTI_FRAGMENT_APP && WIN
		#define PARSE_EXPORT 	__declspec(import)
	#else
		#define PARSE_EXPORT   
	#endif
#endif

#ifndef XML_EXPORT
	#if MULTI_FRAGMENT_APP && WIN
		#define XML_EXPORT 	__declspec(import)
	#else
		#define XML_EXPORT   
	#endif
#endif

#ifndef COM_EXPORT
	#if MULTI_FRAGMENT_APP && WIN
		#define COM_EXPORT 	__declspec(import)
	#else
		#define COM_EXPORT   
	#endif
#endif

#ifndef QUESA_EXPORT
	#if MULTI_FRAGMENT_APP && WIN
		#define QUESA_EXPORT 	__declspec(import)
	#else
		#define QUESA_EXPORT   
	#endif
#endif

#ifndef UI_EXPORT
	#if MULTI_FRAGMENT_APP && WIN
		#define UI_EXPORT 	__declspec(import)
	#else
		#define UI_EXPORT   
	#endif
#endif
	
#ifndef OPENGL_EXPORT					// $$$ need to get rid of this
	#if MULTI_FRAGMENT_APP && WIN
		#define OPENGL_EXPORT 	__declspec(import)
	#else
		#define OPENGL_EXPORT   
	#endif
#endif

#ifndef UI_3D_EXPORT					// $$$ need to get rid of this
	#if MULTI_FRAGMENT_APP && WIN
		#define UI_3D_EXPORT 	__declspec(import)
	#else
		#define UI_3D_EXPORT   
	#endif
#endif


// ===================================================================================
//	System Includes
// ===================================================================================
#if macintosh
	#include <MWhisperHeader.h>
#else
	#include <WWhisperHeader.h>
#endif


// ===================================================================================
//	C++ Includes
// ===================================================================================
#if PRECOMPILE_STD_HEADERS
	#include <algorithm>
	#include <cstdio>
	#include <limits>
	#include <list>						
	#include <map>
	#include <set>
	#include <string>
	#include <vector>
#endif	// PRECOMPILE_STD_HEADERS


#endif	// XWHISPERHEADER_H