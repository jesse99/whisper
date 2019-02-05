/*
 *  File:       WDrawContexts.cpp
 *  Summary:	Classes for objects that can be drawn into.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: WDrawContexts.cpp,v $
 *		Revision 1.3  2001/04/21 03:31:10  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:06:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDrawContexts.h>

#include <XExceptions.h>
#include <XLocker.h>
#include <XPixMap.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateDC
//
//---------------------------------------------------------------
static HDC CreateDC(XPixMap* pixMap)
{
	PRECONDITION(pixMap != nil);
	
	HDC dc = CreateCompatibleDC(pixMap->GetOSContext());
	ThrowIfBadHandle(dc);
	
	HGDIOBJ oldObject = SelectObject(dc, pixMap->GetOSPixMap());
	if (oldObject == nil || oldObject == (HGDIOBJ) GDI_ERROR) {
		(void) DeleteDC(dc);
		ThrowErr(GDI_ERROR);
	}
	
	return dc;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XWindowContext
// ===================================================================================

//---------------------------------------------------------------
//
// XWindowContext::~XWindowContext
//
//---------------------------------------------------------------
XWindowContext::~XWindowContext()
{
	VERIFY(ReleaseDC(mWindow, mContext));
	mContext = nil;					// so XDrawContext dtor doesn't go off in the weeds

	mWindow = nil;
}


//---------------------------------------------------------------
//
// XWindowContext::XWindowContext
//
//---------------------------------------------------------------
XWindowContext::XWindowContext(OSWindow wind) : XDrawContext(GetDC(wind))
{
	mWindow = wind;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPixMapContext
// ===================================================================================

//---------------------------------------------------------------
//
// XPixMapContext::~XPixMapContext
//
//---------------------------------------------------------------
XPixMapContext::~XPixMapContext()
{
	mPixMap->Unlock();
	mPixMap = nil;
}


//---------------------------------------------------------------
//
// XPixMapContext::XPixMapContext
//
//---------------------------------------------------------------
XPixMapContext::XPixMapContext(XPixMap* pixMap) : XDrawContext(pixMap->GetOSContext())
{
	PRECONDITION(pixMap != nil);
	
	mOffsceenCanvas = true;

	mPixMap = pixMap;
	mPixMap->Lock();
}


}	// namespace Whisper

