/*
 *  File:		XURIFetchers.cpp
 *  Summary:	Classes used to load Uniform Resource Identifiers (URIs).
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XURIFetchers.cpp,v $
 *		Revision 1.3  2001/04/27 09:21:10  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.2  2000/11/09 12:58:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XURIFetchers.h>

#include <XFile.h>
#include <XFileSystem.h>
#include <XFolderSpec.h>
#include <XHandle.h>
#include <XIntConversions.h>
#include <XResource.h>
#include <XLocker.h>
#include <XStringUtils.h>
#include <XTranscode.h>

#if MAC
	#include <MResUtils.h>
#elif WIN
	#include <WUtils.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XFileURIFetcher
// ===================================================================================

//---------------------------------------------------------------
//
// XFileURIFetcher::~XFileURIFetcher
//
//---------------------------------------------------------------
XFileURIFetcher::~XFileURIFetcher()
{
}


//---------------------------------------------------------------
//
// XFileURIFetcher::XFileURIFetcher
//
//---------------------------------------------------------------
XFileURIFetcher::XFileURIFetcher()
{
}


//---------------------------------------------------------------
//
// XFileURIFetcher::OnResolve
//
//---------------------------------------------------------------
XResource* XFileURIFetcher::OnResolve(const XURI& uri)
{
	XResource* data = nil;

	std::wstring path = uri.GetPath();
#if MAC
	path = Whisper::Replace(path, '/', ':');
#elif WIN
	path = Whisper::Replace(path, '/', '\\');
#endif

	if (path.length() > 0 && !XFileSystem::IsFolder(XFileSystemSpec(path))) {	// check for folder so XFileSpec doesn't throw
		XFileSpec spec(path);
		
		XHandle buffer;
		bool found = XFileSystem::FileExists(spec);
		if (found) {
			XFile file(spec);
			file.Open(kReadPermission);
			
			uint32 bytes = file.GetLength();
			buffer.SetSize(bytes);

			{
			XLocker lock(buffer);
				file.Read(buffer.GetPtr(), bytes);
			}
			
			file.Close();
			
			data = new XResource(uri, buffer);
		}
	}

	return data;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XResourceURIFetcher
// ===================================================================================

//---------------------------------------------------------------
//
// XResourceURIFetcher::~XResourceURIFetcher
//
//---------------------------------------------------------------
XResourceURIFetcher::~XResourceURIFetcher()
{
}


//---------------------------------------------------------------
//
// XResourceURIFetcher::XResourceURIFetcher
//
//---------------------------------------------------------------
XResourceURIFetcher::XResourceURIFetcher()
{
}


//---------------------------------------------------------------
//
// XResourceURIFetcher::OnResolve
//
//---------------------------------------------------------------
XResource* XResourceURIFetcher::OnResolve(const XURI& uri)
{
	XResource* data = nil;
	
	std::wstring path = uri.GetPath();	
	if (path.length() > 0) {
		uint32 index = path.rfind('.');
		ASSERT(index != std::wstring::npos);
		
		std::wstring root = path.substr(0, index);
		std::wstring extension = path.substr(index+1);
		
#if MAC
		ResType type = StrToID(extension);

		if (IsArabicDigit(root[0])) {
			ResID id = StrToInt16(root);
			if (Whisper::HasResource(type, id))
				data = new XResource(type, id);
				
		} else {
			if (Whisper::HasResource(type, root))
				data = new XResource(type, root);
		}

#elif WIN
		if (IsArabicDigit(root[0])) {
			int16 id = StrToInt16(root);
			if (Whisper::HasResource(ToPlatformStr(extension).c_str(), id))
				data = new XResource(ToPlatformStr(extension).c_str(), id);
		
		} else {
			if (Whisper::HasResource(ToPlatformStr(extension).c_str(), root))
				data = new XResource(ToPlatformStr(extension).c_str(), root);
		}
#endif
		
		if (data != nil)
			ASSERT(uri == data->GetURI());
	}

	return data;
}


}	// namespace Whisper

