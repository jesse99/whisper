/*
 *  File:		XAddPluginResources.h
 *  Summary:	Stack based class that lets the XURIResolver get resources from a plugin.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XAddPluginResources.h,v $
 *		Revision 1.3  2000/12/11 01:29:58  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:25:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XAddPluginResources.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/11 01:29:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	12/22/99	JDJ		Created
 */

#pragma once

#include <XURI.h>
#include <XURIResolver.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XPlugin;
class XPluginURIFetcher;
class XUnknown;


// ===================================================================================
//	class XAddPluginResources
//!		Stack based class that lets the XURIResolver get resources from a plugin.
// ===================================================================================
class COM_EXPORT XAddPluginResources {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XAddPluginResources();

						XAddPluginResources(XPlugin* plugin);
						XAddPluginResources(XUnknown* interface);
						
//-----------------------------------
//	Inherited API
//
protected:
	XPluginURIFetcher* mFetcher;
};


// ===================================================================================
//	class XPluginURIFetcher
//!		Helper class used by XAddPluginResources (you shouldn't have to use this yourself).
// ===================================================================================
class COM_EXPORT XPluginURIFetcher : public XURIFetcher {

	typedef XURIFetcher Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XPluginURIFetcher();

						XPluginURIFetcher(XPlugin* plugin);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual XResource* 	OnResolve(const XURI& uri);

//-----------------------------------
//	Member Data
//
protected:
#if MAC	
	int16		mRefNum;
#elif WIN
	HINSTANCE	mModuleH;
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

