/*
 *	File:		XResource.cpp
 *	Function:	Abstraction for data from an arbitrary source.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XResource.cpp,v $
 *		Revision 1.5  2001/04/27 09:20:52  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.4  2001/04/27 04:35:47  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/04/17 01:44:42  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:49:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 12/23/99	JDJ		Windows ctors take an optional module handle.
 *		 <2>	  2/06/99	JDJ		Uses an XURI instead of a name.
 *		 <1>	 12/08/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XResource.h>

#include <XExceptions.h>
#include <XFile.h>
#include <XIntConversions.h>
#include <XLocker.h>
#include <XNumbers.h>
#include <XStringUtils.h>

#if MAC
	#include <MResUtils.h>
#elif WIN
	#include <WSystemInfo.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XResource
// ===================================================================================

//---------------------------------------------------------------
//
// XResource::~XResource
//
//---------------------------------------------------------------
XResource::~XResource()
{
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XResource::XResource ()
//
//---------------------------------------------------------------
XResource::XResource() 
{
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XResource::XResource (XFileSpec)
//
//---------------------------------------------------------------
XResource::XResource(const XFileSpec& spec) : mURI(spec)
{
	{
	XFile file(spec);
		file.Open(kReadPermission);
		
		uint32 bytes = file.GetLength();
		mData.SetSize(bytes);
		
		{
		XLocker lock(mData);
			file.Read(mData.GetPtr(), bytes);
		}
		
		file.Close();
	}

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XResource::XResource (ResType, ResID)	
//
//---------------------------------------------------------------
#if MAC
XResource::XResource(ResType type, ResID id) 
{
	mData = Whisper::ReadResource(type, id);
	
	std::wstring extension = StripLeading(IDToStr(type), L" ");
	mURI = XURI(L"resource://" + UInt32ToStr(id) + L"." + extension);

	CALL_INVARIANT;
}
#endif


//---------------------------------------------------------------
//
// XResource::XResource (ResType, wstring)	
//
//---------------------------------------------------------------
#if MAC
XResource::XResource(ResType type, const std::wstring& name) 
{
	mData = Whisper::ReadResource(type, name);
	
	std::wstring extension = StripTrailing(StripLeading(IDToStr(type), L" "));
	mURI = XURI(L"resource://" + name + L"." + extension);

	CALL_INVARIANT;
}
#endif


//---------------------------------------------------------------
//
// XResource::XResource (const TCHAR*, ResID, HINSTANCE)
//
//---------------------------------------------------------------
#if WIN
XResource::XResource(const TCHAR* type, ResID id, HINSTANCE moduleH) 
{
	PRECONDITION(type != nil);
	
	HRSRC rsrc = ::FindResource(moduleH, MAKEINTRESOURCE(id), type);
	ThrowIfNil(rsrc);

	HGLOBAL dataH = ::LoadResource(moduleH, rsrc);
	ThrowIfNil(dataH);
	
	const void* dataP = ::LockResource(dataH);		// don't need to unlock or release in Win32
	ThrowIfNil(dataP);
	
	uint32 bytes = ::SizeofResource(moduleH, rsrc);

	mData.SetSize(bytes);
	BlockMoveData(dataP, mData.GetUnsafePtr(), bytes);
	
	if (type == RT_RCDATA)
		mURI = XURI(L"resource://" + Int32ToStr(id));
	else if (HIWORD(type) == 0)
		mURI = XURI(L"resource://" + Int32ToStr(id) + L"." + UInt32ToStr((uint16) type));
	else
		mURI = XURI(L"resource://" + Int32ToStr(id) + L"." + FromPlatformStr(type));

	CALL_INVARIANT;
}
#endif


//---------------------------------------------------------------
//
// XResource::XResource (TCHAR*, wstring, HINSTANCE)
//
//---------------------------------------------------------------
#if WIN
XResource::XResource(const TCHAR* type, const std::wstring& name, HINSTANCE moduleH) 
{
	PRECONDITION(type != nil);
	
	HRSRC rsrc = ::FindResource(moduleH, ToPlatformStr(name).c_str(), type);
	ThrowIfNil(rsrc);

	HGLOBAL dataH = ::LoadResource(moduleH, rsrc);
	ThrowIfNil(dataH);
	
	const void* dataP = ::LockResource(dataH);		// don't need to unlock or release in Win32
	ThrowIfNil(dataP);
	
	uint32 bytes = ::SizeofResource(moduleH, rsrc);

	mData.SetSize(bytes);
	BlockMoveData(dataP, mData.GetUnsafePtr(), bytes);
	
	if (type == RT_RCDATA)
		mURI = XURI(L"resource://" + name);
	else if (HIWORD(type) == 0)
		mURI = XURI(L"resource://" + name + L"." + UInt32ToStr((uint16) type));
	else
		mURI = XURI(L"resource://" + name + L"." + FromPlatformStr(type));

	CALL_INVARIANT;
}
#endif


//---------------------------------------------------------------
//
// XResource::XResource (XURI, XHandle)
//
//---------------------------------------------------------------
XResource::XResource(const XURI& uri, const XHandle& data) : mURI(uri), mData(data)
{
	CALL_INVARIANT;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XResource::Invariant
//
//---------------------------------------------------------------
void XResource::Invariant() const
{	
	if (mData.GetSize() > 0)
		ASSERT(mURI.GetPath().length() > 0);
}


//---------------------------------------------------------------
//
// XResource::OnLock
//
//---------------------------------------------------------------
void XResource::OnLock(bool moveHigh)
{
	mData.Lock(moveHigh);
}


//---------------------------------------------------------------
//
// XResource::OnUnlock
//
//---------------------------------------------------------------
void XResource::OnUnlock()
{
	mData.Unlock();
}


}	// namespace Whisper
