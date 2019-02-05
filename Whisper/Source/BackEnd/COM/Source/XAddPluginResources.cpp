/*
 *  File:		XAddPluginResources.cpp
 *  Summary:	Stack based class that lets the XURIResolver get resources from a plugin.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XAddPluginResources.cpp,v $
 *		Revision 1.4  2001/04/27 09:20:06  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.3  2001/04/21 03:39:19  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:25:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XAddPluginResources.h>

#include <XBoss.h>
#include <XExceptions.h>
#include <XFragment.h>
#include <XHandle.h>
#include <XIntConversions.h>
#include <XResource.h>
#include <XLocker.h>
#include <XPlugin.h>
#include <XStringUtils.h>
#include <XUnknown.h>
#include <XURIResolver.h>

#if MAC
	#include <MResUtils.h>
	#include <MSystemInfo.h>
#elif WIN
	#include <WUtils.h>
	#include <WSystemInfo.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XAddPluginResources
// ===================================================================================

//---------------------------------------------------------------
//
// XAddPluginResources::~XAddPluginResources
//
//---------------------------------------------------------------
XAddPluginResources::~XAddPluginResources()
{
	XURIResolver::Instance()->RemoveFetcher(mFetcher);
	
	delete mFetcher;
}


//---------------------------------------------------------------
//
// XAddPluginResources::XAddPluginResources (XPlugin*)
//
//---------------------------------------------------------------
XAddPluginResources::XAddPluginResources(XPlugin* plugin)
{
	mFetcher = new XPluginURIFetcher(plugin);
	
	XURIResolver::Instance()->AddFetcher(L"resource", mFetcher);
}
						

//---------------------------------------------------------------
//
// XAddPluginResources::XAddPluginResources (XUnknown*)
//
//---------------------------------------------------------------
XAddPluginResources::XAddPluginResources(XUnknown* interface)
{
	PRECONDITION(interface != nil);
	
	XBoss* boss = interface->GetBoss();
	XPlugin* plugin = boss->GetPlugin();
	
	mFetcher = new XPluginURIFetcher(plugin);
	
	XURIResolver::Instance()->AddFetcher(L"resource", mFetcher);
}
						
#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPluginURIFetcher
// ===================================================================================

//---------------------------------------------------------------
//
// XPluginURIFetcher::~XPluginURIFetcher
//
//---------------------------------------------------------------
XPluginURIFetcher::~XPluginURIFetcher()
{
#if MAC	
	if (mRefNum != MSystemInfo::GetAppRefNum())
		CloseResFile(mRefNum);
#endif
}


//---------------------------------------------------------------
//
// XPluginURIFetcher::XPluginURIFetcher
//
//---------------------------------------------------------------
XPluginURIFetcher::XPluginURIFetcher(XPlugin* plugin)
{	
#if MAC	
	if (plugin != nil) {
		XFileSpec spec = plugin->GetSpec();
		
		mRefNum = FSpOpenResFile(&spec.GetOSSpec(), fsRdPerm);	// by using fsRdPerm we're guaranteed to get a refNum we can close
		ThrowIfResError();

	} else 
		mRefNum = MSystemInfo::GetAppRefNum();
	
#elif WIN
	if (plugin != nil) {
		XFragment* fragment = plugin->GetFragment();
		mModuleH = fragment->GetOSFragment();
		
	} else 
		mModuleH = WSystemInfo::GetAppInstance();
#endif
}


//---------------------------------------------------------------
//
// XPluginURIFetcher::OnResolve
//
//---------------------------------------------------------------
XResource* XPluginURIFetcher::OnResolve(const XURI& uri)
{
	XResource* data = nil;
	
	std::wstring path = uri.GetPath();	
	if (path.length() > 0) {
		uint32 index = path.rfind('.');
		ASSERT(index != std::wstring::npos);
		
		std::wstring root = path.substr(0, index);
		std::wstring extension = path.substr(index+1);
		
#if MAC
		MSaveResFile save(mRefNum);
		
		ResType type = StrToID(extension);

		if (IsArabicDigit(root[0])) {
			ResID id = StrToInt16(root);
			if (Whisper::Has1Resource(type, id))
				data = new XResource(type, id);
				
		} else {
			if (Whisper::Has1Resource(type, root))
				data = new XResource(type, root);
		}

#elif WIN
		if (IsArabicDigit(root[0])) {
			int16 id = StrToInt16(root);
			if (Whisper::HasResource(ToPlatformStr(extension).c_str(), id, mModuleH))
				data = new XResource(ToPlatformStr(extension).c_str(), id, mModuleH);
		
		} else {
			if (Whisper::HasResource(ToPlatformStr(extension).c_str(), root, mModuleH))
				data = new XResource(ToPlatformStr(extension).c_str(), root, mModuleH);
		}
#endif
		
		if (data != nil)
			ASSERT(uri == data->GetURI());
	}

	return data;
}


}	// namespace Whisper

