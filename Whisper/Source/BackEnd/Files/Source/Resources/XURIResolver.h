/*
 *  File:		XURIResolver.h
 *  Summary:	Singleton uses to load Uniform Resource Identifiers (URIs).
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XURIResolver.h,v $
 *		Revision 1.5  2001/03/05 05:44:30  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.4  2001/03/01 11:42:46  jesjones
 *		Outlined XURIFetcher ctor and dtor.
 *		
 *		Revision 1.3  2000/12/10 08:57:28  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:59:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <vector>

#include <XAutoPtr.h>
#include <XCriticalSection.h>
#include <XReferenceCounted.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XResource;
class XURI;


// ===================================================================================
//	class XURIFetcher
//!		Singleton uses to load Uniform Resource Identifiers (URIs).
// ===================================================================================
class FILES_EXPORT XURIFetcher : public XReferenceCountedMixin {

	typedef XReferenceCountedMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XURIFetcher();

						XURIFetcher();
						
private:
						XURIFetcher(const XURIFetcher& rhs);
						
			XURIFetcher& operator=(const XURIFetcher& rhs);
						
//-----------------------------------
//	API
//
public:
	virtual XResource* 	OnResolve(const XURI& uri) = 0;
						// Return nil if the uri cannot be found.
};


// ===================================================================================
//	class XURIResolver
// ===================================================================================
class FILES_EXPORT XURIResolver {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XURIResolver();

						XURIResolver();
						/**< Note that this will automatically add XFileURIFetcher and
						XResourceURIFetcher fetchers. */
						
	static XURIResolver* Instance();

private:
						XURIResolver(const XURIResolver& rhs);
						
			XURIResolver& operator=(const XURIResolver& rhs);
						
//-----------------------------------
//	API
//
public:
	// ----- Resolving -----
			XResource* 	Resolve(const XURI& uri);
						/**< Returns nil if the uri cannot be found. */
			
	// ----- Fetchers -----
			void 		AddFetcher(const std::wstring& scheme, XURIFetcher* fetcher);
						/**< If scheme matches a scheme added earlier the new fetcher
						will override the old one. Note that this bumps the
						fetcher's ref count. */

			XURIFetcher* GetFetcher(const std::wstring& scheme) const;
						/**< This will not bump the fetcher's ref count! Returns nil
						if the fetcher cannot be found. */
						
			void 		RemoveFetcher(XURIFetcher* fetcher);
			
//-----------------------------------
//	Internal Types
//
protected:
	struct SFetcher {
		std::wstring	scheme;
		XURIFetcher*	fetcher;
		
					SFetcher() : fetcher(nil)											{}
					SFetcher(const std::wstring& s, XURIFetcher* f) : scheme(s), fetcher(f)	{}
			
			bool 	operator==(const std::wstring& str) const								{return str == scheme;}
			bool 	operator!=(const std::wstring& str) const								{return str != scheme;}

			bool 	operator==(const XURIFetcher* f) const								{return f == fetcher;}
			bool 	operator!=(const XURIFetcher* f) const								{return f != fetcher;}
	};	

//-----------------------------------
//	Member Data
//
protected:
	std::vector<SFetcher>			mFetchers;	// use a vector instead of a map because we're unlikely to have very many fetchers
	mutable XCriticalSection		mMutex;

private:
	static XAutoPtr<XURIResolver>	msInstance;			
	static XCriticalSection			msCreateMutex;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

