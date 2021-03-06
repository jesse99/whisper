/*
 *  File:       XFragment.cpp
 *  Summary:   	Wrapper around code fragments/DLLs.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFragment.cpp,v $
 *		Revision 1.7  2001/04/27 09:20:44  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.6  2001/04/27 04:32:50  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.5  2001/03/14 09:47:53  jesjones
 *		Uses the spec's path when loading DLLs on Windows.
 *		
 *		Revision 1.4  2001/03/03 11:13:26  jesjones
 *		Added XFragmentException dtor to get rid of a linker warning when building DLL targets.
 *		
 *		Revision 1.3  2000/12/10 04:09:54  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:37:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XFragment.h>

#include <XExceptions.h>
#include <XFileSpec.h>
#include <XIntConversions.h>
#include <XNumbers.h>
#include <XStringUtils.h>

#if WIN
	#include <WSystemInfo.h>
#endif

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetErrorMesg (OSError, wstring, wstring)
//
//---------------------------------------------------------------
static std::wstring GetErrorMesg(OSError err, const std::wstring& name, const std::wstring& errName)
{
	std::wstring narrative;
	
	if (
#if MAC
	err == fnfErr
#elif WIN
	err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND
#endif
	)
		if (name == errName)
			narrative = LoadWhisperString(L"Couldn't find the '#1' shared library.", name);		// bad alias?
		else 
			narrative = LoadWhisperString(L"Couldn't load the '#1' shared library (it depends on '#2' which couldn't be found).", name, errName);
	else 
		if (name == errName) 
#if MAC
			narrative = LoadWhisperString(L"There was a ##1 error loading '#2'.", Int32ToStr(err), name);
#elif WIN
			narrative = LoadWhisperString(L"There was a ##1 error loading '#2'.", UInt32ToStr(err), name);
#endif
		else 
#if MAC
			narrative = LoadWhisperString(L"Couldn't load the '#1' shared library (it depends on '#2' which failed to load with error ##3).", name, errName, Int32ToStr(err));
#elif WIN
			narrative = LoadWhisperString(L"Couldn't load the '#1' shared library (it depends on '#2' which failed to load with error ##3).", name, errName, UInt32ToStr(err));
#endif

	return narrative;
}


//---------------------------------------------------------------
//
// GetErrorMesg (exception, wstring, wstring)
//
//---------------------------------------------------------------
static std::wstring GetErrorMesg(const std::exception& e, const std::wstring& name, const std::wstring& errName)
{
	std::wstring narrative;
	
	if (name == errName) 
		narrative = LoadWhisperString(L"Couldn't load the '#1' library (#2).", name, FromUTF8Str(e.what()));
	else 
		narrative = LoadWhisperString(L"Couldn't load the '#1' shared library (it depends on '#2' which failed to load with error '#3').", name, errName, FromUTF8Str(e.what()));

	return narrative;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XLoadFragmentException
// ===================================================================================

//---------------------------------------------------------------
//
// XLoadFragmentException::~XLoadFragmentException
//
//---------------------------------------------------------------
XLoadFragmentException::~XLoadFragmentException()		// get rid of a linker warning when building DLL targets by outlining a virtual function...
{
}


//---------------------------------------------------------------
//
// XLoadFragmentException::XLoadFragmentException (OSError, wstring, wstring)
//
//---------------------------------------------------------------
XLoadFragmentException::XLoadFragmentException(OSError err, const std::wstring& name, const std::wstring& errName) : std::runtime_error(ToUTF8Str(GetErrorMesg(err, name, errName))), mName(name), mErrName(errName), mError(err)
{
}


//---------------------------------------------------------------
//
// XLoadFragmentException::XLoadFragmentException (exception, wstring, wstring)
//
//---------------------------------------------------------------
XLoadFragmentException::XLoadFragmentException(const std::exception& e, const std::wstring& name, const std::wstring& errName) : std::runtime_error(ToUTF8Str(GetErrorMesg(e, name, errName))), mName(name), mErrName(errName), mError(OSError(-1))
{
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFragment
// ===================================================================================

//---------------------------------------------------------------
//
// XFragment::~XFragment
//
//---------------------------------------------------------------
XFragment::~XFragment()
{
#if MAC
	OSErr err = ::CloseConnection(&mFragment);
	ASSERT(err == noErr);							// don't want to throw from a dtor
	
#elif WIN
	VERIFY(::FreeLibrary(mFragment));
#endif
}


//---------------------------------------------------------------
//
// XFragment::XFragment (wstring, EFragOptions)
//
//---------------------------------------------------------------
XFragment::XFragment(const std::wstring& name, EFragOptions options) : mName(name)
{
#if MAC
	Str255 errName;
	CFragLoadOptions flags = options == kUseExistingFragment ? kReferenceCFrag : kPrivateCFragCopy;
	OSErr err = ::GetSharedLibrary(ToPascalStr(mName).c_str(),	// library name
								   kPowerPCCFragArch,	// instruction set architecture
								   flags,				// find flags
								   &mFragment,			// connection id
								   &mMainAdr,			// main address
								   errName);			// name of the fragment that couldn't be loaded
	if (err != noErr)
		throw XLoadFragmentException(err, mName, FromPascalStr(errName));
		
#elif WIN
	UNUSED(options);
	
	if (WSystemInfo::HasUnicode())
		mFragment = ::LoadLibraryW(mName.c_str());
	else
		mFragment = ::LoadLibraryA(ToPlatformStr(mName).c_str());
		
	if (mFragment == nil)
		throw XLoadFragmentException(GetLastError(), mName, mName);
#endif
}


//---------------------------------------------------------------
//
// XFragment::XFragment (XFileSpec, EFragOptions)
//
//---------------------------------------------------------------
XFragment::XFragment(const XFileSpec& spec, EFragOptions options) : mName(spec.GetName())
{
#if MAC
	Str255 errName;
	CFragLoadOptions flags = options == kUseExistingFragment ? kReferenceCFrag : kPrivateCFragCopy;
	OSErr err = ::GetDiskFragment(&spec.GetOSSpec(),	// library FSSpec
								  0,					// offset
								  kCFragGoesToEOF,		// length
								  ToPascalStr(mName).c_str(),	// fragment name
								  flags,				// find flags
								  &mFragment,			// connection id
								  &mMainAdr,			// main address
								  errName);				// name of the fragment that couldn't be loaded
	if (err != noErr)
		throw XLoadFragmentException(err, mName, FromPascalStr(errName));
		
#elif WIN
	UNUSED(options);
	
	if (WSystemInfo::HasUnicode())
		mFragment = ::LoadLibraryW(spec.GetPath().c_str());
	else
		mFragment = ::LoadLibraryA(ToPlatformStr(spec.GetPath()).c_str());
		
	if (mFragment == nil)
		throw XLoadFragmentException(GetLastError(), mName, mName);
#endif
}


//---------------------------------------------------------------
//
// XFragment::XFragment (XFileSpec, uint32, uint32, EFragOptions)
//
//---------------------------------------------------------------
#if MAC
XFragment::XFragment(const XFileSpec& spec, uint32 offset, uint32 bytes, EFragOptions options) : mName(spec.GetName())
{
	Str255 errName;
	CFragLoadOptions flags = options == kUseExistingFragment ? kReferenceCFrag : kPrivateCFragCopy;
	OSErr err = ::GetDiskFragment(&spec.GetOSSpec(),	// library FSSpec
								  offset,				// offset
								  bytes,				// length
								  ToPascalStr(mName).c_str(),	// fragment name
								  flags,				// find flags
								  &mFragment,			// connection id
								  &mMainAdr,			// main address
								  errName);				// name of the fragment that couldn't be loaded
	if (err != noErr)
		throw XLoadFragmentException(err, mName, FromPascalStr(errName));
}
#endif


//---------------------------------------------------------------
//
// XFragment::XFragment (wstring, const void*, uint32, EFragOptions)
//
//---------------------------------------------------------------
#if MAC
XFragment::XFragment(const std::wstring& name, const void* adr, uint32 bytes, EFragOptions options) : mName(name)
{
	Str255 errName;
	CFragLoadOptions flags = options == kUseExistingFragment ? kReferenceCFrag : kPrivateCFragCopy;
	OSErr err = ::GetMemFragment(const_cast<void*>(adr),// fragment address
								 bytes,					// length
								 ToPascalStr(mName).c_str(),	// fragment name
								 flags,					// find flags
								 &mFragment,			// connection id
								 &mMainAdr,				// main address
								 errName);				// name of the fragment that couldn't be loaded
	if (err != noErr)
		throw XLoadFragmentException(err, mName, FromPascalStr(errName));
}
#endif


//---------------------------------------------------------------
//
// XFragment::GetFunction
//
//---------------------------------------------------------------
void* XFragment::GetFunction(const std::wstring& name) const
{
#if MAC
	Ptr adr = nil;
	CFragSymbolClass type;
	(void) ::FindSymbol(mFragment, ToPascalStr(name).c_str(), &adr, &type);
	
	if (type == kCodeCFragSymbol)
		adr = nil;
	else if (type == kDataCFragSymbol)
		adr = nil;
	
#elif WIN
	void* adr = ::GetProcAddress(mFragment, ToPlatformStr(name).c_str());
#endif

	return adr;
}


//---------------------------------------------------------------
//
// XFragment::GetData
//
//---------------------------------------------------------------
#if MAC
void* XFragment::GetData(const std::wstring& name) const
{
	Ptr adr = nil;
	CFragSymbolClass type;
	(void) ::FindSymbol(mFragment, ToPascalStr(name).c_str(), &adr, &type);
	
	if (type == kCodeCFragSymbol)
		adr = nil;
	else if (type == kTVectorCFragSymbol)
		adr = nil;
	
	return adr;
}
#endif


//---------------------------------------------------------------
//
// XFragment::GetNumSymbols
//
//---------------------------------------------------------------
#if MAC
uint32 XFragment::GetNumSymbols() const
{
	int32 count = 0;
	OSErr err = ::CountSymbols(mFragment, &count);
	ThrowIfOSErr(err);
	
	return numeric_cast<uint32>(count);
}
#endif


//---------------------------------------------------------------
//
// XFragment::GetSymbol
//
//---------------------------------------------------------------
#if MAC
SSymbol XFragment::GetSymbol(uint32 index) const
{
	Ptr adr = nil;
	Str255 name;
	CFragSymbolClass type;
	OSErr err = ::GetIndSymbol(mFragment, numeric_cast<int32>(index), name, &adr, &type);
	ThrowIfOSErr(err);
	
	SSymbol symbol;
	symbol.name = FromPascalStr(name);
	symbol.ptr    = adr;
	symbol.isCode = type != kDataCFragSymbol;
	
	return symbol;
}
#endif


}	// namespace Whisper
