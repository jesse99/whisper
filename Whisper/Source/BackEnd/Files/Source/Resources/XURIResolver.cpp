/*
 *  File:		XURIResolver.cpp
 *  Summary:	Singleton uses to load Uniform Resource Identifiers (URIs).
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XURIResolver.cpp,v $
 *		Revision 1.5  2001/04/27 04:38:03  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/03/03 23:58:56  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.3  2001/03/01 11:42:43  jesjones
 *		Outlined XURIFetcher ctor and dtor.
 *		
 *		Revision 1.2  2000/11/09 12:59:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <8>	12/22/99	JDJ		RemoveFetcher actually removes the fetcher from the list. 
 *		 <7>	 8/09/99	JDJ		Resolve will try multiple fetchers if the first one 
 *									can't resolve the uri.
 *		 <6>	 8/01/99	JDJ		Resolve traps exceptions.
 *		 <5>	 7/08/99	JDJ		Added GetFetcher.
 *		 <4>	 6/21/99	JDJ		Added __MSL_FIX_ITERATORS__ (for Pro 3).
 *		 <3>	 5/28/99	JDJ		Made thread safe.
 *		 <2>	 5/27/99	JDJ		Made Instance method thread safe.
 *		 <1>	 2/06/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XURIResolver.h>

#include <XStringUtils.h>
#include <XURI.h>
#include <XURIFetchers.h>

namespace Whisper {


// ===================================================================================
//	class XURIFetcher
// ===================================================================================

//---------------------------------------------------------------
//
// XURIFetcher::~XURIFetcher
//
//---------------------------------------------------------------
XURIFetcher::~XURIFetcher()		
{
}


//---------------------------------------------------------------
//
// XURIFetcher::XURIFetcher
//
//---------------------------------------------------------------
XURIFetcher::XURIFetcher()		
{
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XURIResolver
// ===================================================================================

XAutoPtr<XURIResolver>	XURIResolver::msInstance;		
XCriticalSection 		XURIResolver::msCreateMutex;

//---------------------------------------------------------------
//
// XURIResolver::~XURIResolver
//
//---------------------------------------------------------------
XURIResolver::~XURIResolver()
{
	std::vector<SFetcher>::iterator iter = mFetchers.begin();
	while (iter != mFetchers.end()) {
		iter->fetcher->RemoveReference();
		++iter;
	}
}


//---------------------------------------------------------------
//
// XURIResolver::XURIResolver
//
//---------------------------------------------------------------
XURIResolver::XURIResolver()
{
	XURIFetcher* fetcher = new XFileURIFetcher;
	this->AddFetcher(L"file", fetcher);	
	fetcher->RemoveReference();

	fetcher = new XResourceURIFetcher;
	this->AddFetcher(L"resource", fetcher);	
	fetcher->RemoveReference();
}


//---------------------------------------------------------------
//
// XURIResolver::Instance								[static]
//
//---------------------------------------------------------------
XURIResolver* XURIResolver::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msCreateMutex);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new XURIResolver);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// XURIResolver::Resolve
//
//---------------------------------------------------------------
XResource* XURIResolver::Resolve(const XURI& uri)
{
	XResource* data = nil;
	
	try {
		std::wstring scheme = uri.GetScheme();
		
		XEnterCriticalSection enter(mMutex);
	
		std::vector<SFetcher>::reverse_iterator iter = mFetchers.rbegin();	// use reverse iterators to allow AddFetcher to override prior fetchers
		while (iter != mFetchers.rend() && data == nil) {
			SFetcher& fetcher = *iter;
			++iter;
			
			if (fetcher == scheme)
				data = fetcher.fetcher->OnResolve(uri);	
		}
	
	} catch (...) {
		DEBUGSTR("Got an exception in XURIResolver::Resolve");	
	}
	
	return data;
}


//---------------------------------------------------------------
//
// XURIResolver::AddFetcher
//
//---------------------------------------------------------------
void XURIResolver::AddFetcher(const std::wstring& scheme, XURIFetcher* fetcher)
{
	PRECONDITION(scheme.length() > 0);
	PRECONDITION(fetcher != nil);
	
	XEnterCriticalSection enter(mMutex);

	mFetchers.push_back(SFetcher(ConvertToLowerCase(scheme), fetcher));			// "For resiliency, programs interpreting URI should treat upper case letters as equivalent to lower case in scheme names"
	fetcher->AddReference();
}


//---------------------------------------------------------------
//
// XURIResolver::GetFetcher
//
//---------------------------------------------------------------
XURIFetcher* XURIResolver::GetFetcher(const std::wstring& scheme) const
{
	XURIFetcher* fetcher = nil;

	XEnterCriticalSection enter(mMutex);

	std::vector<SFetcher>::const_reverse_iterator iter = std::find(mFetchers.rbegin(), mFetchers.rend(), scheme);		// use reverse iterators to allow AddFetcher to override prior fetchers
	if (iter != mFetchers.rend()) 
		fetcher = iter->fetcher;	

	return fetcher;
}


//---------------------------------------------------------------
//
// XURIResolver::RemoveFetcher
//
//---------------------------------------------------------------
void XURIResolver::RemoveFetcher(XURIFetcher* fetcher)
{
	PRECONDITION(fetcher != nil);
	
	XEnterCriticalSection enter(mMutex);

	std::vector<SFetcher>::iterator iter = std::find(mFetchers.begin(), mFetchers.end(), fetcher);	
	if (iter != mFetchers.end()) {
		mFetchers.erase(iter);
		fetcher->RemoveReference();
		
	} else
		DEBUGSTR("XURIResolver::RemoveFetcher couldn't find the fetcher.");
}


}	// namespace Whisper

