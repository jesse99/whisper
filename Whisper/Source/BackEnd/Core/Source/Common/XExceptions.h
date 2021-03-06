/*
 *  File:       XExceptions.h
 *  Summary:   	ANSI derived exception classes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones.  
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Classes:	exception					- ANSI base exception class
 *					logic_error				- Errors that could have been caught before running the program.
 *						domain_error		- A function's argument is outside the domain of the function.
 *						invalid_argument	- An argument was completely bogus.
 *						length_error		- An object was asked to grow past a hard-wired limit.

 *						bad_cast			- dynamic_cast was asked to do an illegal cast to a reference
 *						XAssertException	- Exception thrown by ASSERT and VERIFY (if ASSERTS_THROW is true).
 *					runtime_error			- Errors that can only be caught at runtime.
 *						range_error			- A function tried to produce a result that was outside its range.
 *						overflow_error		- An arithmatic overflow occured.
 *						XSystemException	- A system error occured.
 *						XCancelException	- The user has canceled an operation.
 *					bad_alloc				- Not enough memory for operator new or other memory request.
 *						XSysMemoryException	- Request for system memory failed (ie anything but operator new).
 *
 *  Change History (most recent first):	
 *
 *		$Log: XExceptions.h,v $
 *		Revision 1.4  2001/05/06 10:42:06  jesjones
 *		Added a ThrowIf overload that takes a string.
 *		
 *		Revision 1.3  2001/04/13 08:01:21  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.2  2000/11/09 12:34:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */
 
#pragma once

#include <stdexcept>
#include <string>

#include <XTranscode.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Whisper Exceptions
// ===================================================================================
class CORE_EXPORT XSysMemoryException : public std::bad_alloc { 
public: 
						XSysMemoryException() 			 {}
};


class CORE_EXPORT XSystemException : public std::runtime_error { 
public: 
						XSystemException(OSError err, const std::wstring& mesg) : std::runtime_error(ToUTF8Str(mesg)) {mError = err;}
public:
	OSError	mError;
};


#if ASSERTS_THROW
class CORE_EXPORT XAssertException : public std::logic_error { 
public: 
						XAssertException(const std::wstring& mesg) : std::logic_error(ToUTF8Str(mesg)) {}
};
#endif	


class CORE_EXPORT XCancelException : public std::runtime_error { 
public: 
						XCancelException(const std::wstring& mesg) : std::runtime_error(ToUTF8Str(mesg)) {}
};


// ===================================================================================
//	OS Specific Functions
// ===================================================================================
CORE_EXPORT void 	ThrowErr(OSError err);		

#if WIN
	inline void ThrowIfErr(uint32 err)						{if (err != NO_ERROR) ThrowErr(err);}
	inline void ThrowIfErr()								{ThrowIfErr(GetLastError());}
				// Be careful when using ThrowIfErr(): most Windows functions only set
				// the error flag when they're unsuccesful.

	inline void ThrowIfNil(const void* ptr)					{if (ptr == nil) {ThrowIfErr(GetLastError()); ThrowErr(ERROR_NOT_ENOUGH_MEMORY);}}
	inline void ThrowIfNull(const void* ptr)				{if (ptr == nil) {ThrowIfErr(GetLastError()); ThrowErr(ERROR_NOT_ENOUGH_MEMORY);}}
	inline void	ThrowIfBadHandle(HANDLE h)					{if (h == nil || h == INVALID_HANDLE_VALUE) {ThrowIfErr(GetLastError()); ThrowErr(ERROR_INVALID_HANDLE);}}

	inline void	ThrowIf(bool flag)							{if (flag) {ThrowIfErr((uint32) GetLastError()); ThrowErr((uint32) E_FAIL);}}

	inline void ThrowIfFailed(HRESULT result)				{if (FAILED(result)) ThrowErr((uint32) result);}

#elif MAC
	inline void ThrowIfOSErr(OSStatus err)					{if (err != noErr) ThrowErr(err);}	
	inline void ThrowIfResError()							{ThrowIfOSErr(ResError());}	
	inline void ThrowIfMemError()							{ThrowIfOSErr(MemError());}	
	inline void ThrowIfQDError()							{ThrowIfOSErr(QDError());}

	CORE_EXPORT void ThrowIfNil(const void* ptr);

	CORE_EXPORT void ThrowIfResFail(const void* ptr);										
	CORE_EXPORT void ThrowIfMemFail(const void* ptr);				
#endif

	inline void	ThrowIf(bool flag, const std::wstring& s)	{if (flag) throw std::runtime_error(ToUTF8Str(s).c_str());}

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

