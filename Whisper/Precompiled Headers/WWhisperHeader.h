/*
 *  File:       WWhisperHeader.h
 *  Summary:   	OS specific stuff for inclusion into a precompiled header (typically
 *				XWhisperHeader.h).
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WWhisperHeader.h,v $
 *		Revision 1.6  2001/03/13 11:07:34  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.5  2001/01/03 05:52:40  jesjones
 *		No longer targets Win98 and Win2K (caused weird problems with CoCreateInstance).
 *		
 *		Revision 1.4  2000/11/27 03:26:53  jesjones
 *		Targets Win 2000.
 *		
 *		Revision 1.3  2000/11/21 09:23:45  jesjones
 *		Removed the dollar signs from the initial checkin comment.
 *		
 *		Revision 1.2  2000/11/09 12:12:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */


// ===================================================================================
//	Windows Includes
// ===================================================================================
#define WIN32_LEAN_AND_MEAN				// exclude some of the less common APIs
#define NOSERVICE
#define NOMCX
#define NOIME

#define STRICT					1		// enable as much type checking as the lame Windows API will allow
#define NOMINMAX						// don't let Windows define (lower case!) min and max macros (conflicts with STL)

#include <windows.h>
#include <wtypes.h>
#include <commdlg.h>
#include <tchar.h>


// ===================================================================================
//	MSVC Warnings
// ===================================================================================
#if _MSC_VER
#ifndef ENABLE_EXTRA_WARNINGS
#define ENABLE_EXTRA_WARNINGS	1			// if non-zero selected level 4 warnings are enabled
#endif	

#if ENABLE_EXTRA_WARNINGS
	#pragma warning(error : 4057)			// The pointer expressions used with the given operator had different base types.
//	#pragma warning(error : 4100)			// The given formal parameter was never referenced in the body of the function for which it was declared.
	#pragma warning(error : 4130)			// The operator was used with the address of a string literal.
	#pragma warning(error : 4131)			// The specified function declaration is not in prototype form.
	#pragma warning(error : 4132)			// The constant was not initialized.
	#pragma warning(error : 4152)			// A pointer to a function was converted to a pointer to data, or visa versa.
	#pragma warning(error : 4208)			// nonstandard extension used : delete [exp] - exp evaluated but ignored
	#pragma warning(error : 4211)			// nonstandard extension used : redefined extern to static
	#pragma warning(error : 4212)			// nonstandard extension used : function declaration used ellipsis
	#pragma warning(error : 4222)			// nonstandard extension used : 'identifier' : 'static' should not be used on member functions defined at file scope
	#pragma warning(error : 4223)			// nonstandard extension used : non-lvalue array converted to pointer
	#pragma warning(error : 4355)			// 'this' : used in base member initializer list
	#pragma warning(error : 4663)			// C++ language change: to explicitly specialize class template 'identifier' use the following syntax:
	#pragma warning(error : 4665)			// C++ language change: assuming 'declaration' is an explicit specialization of a function template
	#pragma warning(error : 4701)			// local variable 'name' may be used without having been initialized
	#pragma warning(error : 4705)			// statement has no effect
	#pragma warning(error : 4706)			// assignment within conditional expression
#endif

#pragma warning(disable : 4800)				// casting integer to bool
#pragma warning(disable : 4786)				// identifier was truncated to '255' characters in the browser

#endif


// ===================================================================================
//	Whisper Defines
// ===================================================================================
#ifndef nil
#define nil					NULL
#endif

