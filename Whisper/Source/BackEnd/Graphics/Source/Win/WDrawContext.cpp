/*
 *  File:       WDrawContext.cpp
 *  Summary:	Abstract base class for objects that can be drawn into.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: WDrawContext.cpp,v $
 *		Revision 1.3  2001/04/21 03:31:06  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:06:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDrawContext.h>

#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XSaveContext
// ===================================================================================

//---------------------------------------------------------------
//
// XSaveContext::~XSaveContext
//
//---------------------------------------------------------------
XSaveContext::~XSaveContext()
{
	VERIFY(::SetWindowOrgEx(mContext, mOldWindowOrigin.x, mOldWindowOrigin.y, nil));
	VERIFY(::SetViewportOrgEx(mContext, mOldViewOrigin.x, mOldViewOrigin.y, nil));
	
	if (mHadClip)
		VERIFY(::SelectClipRgn(mContext, mOldClip));

	mContext = nil;
}


//---------------------------------------------------------------
//
// XSaveContext::XSaveContext
//
//---------------------------------------------------------------
XSaveContext::XSaveContext(OSDrawContext context)
{
	ThrowIfBadHandle(context);

	mContext = context;

	int32 succeeded = ::GetWindowOrgEx(mContext, mOldWindowOrigin);
	ThrowIf(!succeeded);
	
	succeeded = ::GetViewportOrgEx(mContext, mOldViewOrigin);
	ThrowIf(!succeeded);
	
	int32 result = ::GetClipRgn(mContext, mOldClip);
	ThrowIf(result < 0);
	mHadClip = result == 1;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XDrawContext
// ===================================================================================

//---------------------------------------------------------------
//
// XDrawContext::~XDrawContext
//
//---------------------------------------------------------------
XDrawContext::~XDrawContext()
{
	mContext = nil;
}


//---------------------------------------------------------------
//
// XDrawContext::XDrawContext
//
//---------------------------------------------------------------
XDrawContext::XDrawContext(OSDrawContext context) : mSaveContext(context)
{
	ThrowIfBadHandle(context);
	
	mContext = context;
	
	mDynamicCanvas  = true;
	mOffsceenCanvas = false;

	int32 succeeded = ::SetWindowOrgEx(mContext, 0, 0, nil);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XDrawContext::GetExtent
//
//---------------------------------------------------------------
XRect XDrawContext::GetExtent() const
{
	XSize size;
	int32 succeeded = ::GetViewportExtEx(mContext, size);
	ThrowIf(!succeeded);
	
	return XRect(kZeroPt, size);
}


//---------------------------------------------------------------
//
// XDrawContext::GetOrigin
//
//---------------------------------------------------------------
XPoint XDrawContext::GetOrigin() const
{
	XPoint origin;

	int32 succeeded = ::GetViewportOrgEx(mContext, origin);
	ThrowIf(!succeeded);

	return origin;
}


//---------------------------------------------------------------
//
// XDrawContext::GetClip
//
//---------------------------------------------------------------
XRegion XDrawContext::GetClip() const
{
	XRegion clipRgn;
	
	XPoint offset = kZeroPt;
	int32 succeeded = ::LPtoDP(mContext, offset, 1);
	ThrowIf(!succeeded);

	int32 result = ::GetClipRgn(mContext, clipRgn);
	ThrowIf(result < 0);

	return clipRgn - offset;
}

	
//---------------------------------------------------------------
//
// XDrawContext::SetClip
//
//---------------------------------------------------------------
void XDrawContext::SetClip(const XRegion& clip)
{
	XPoint offset = kZeroPt;
	int32 succeeded = ::LPtoDP(mContext, offset, 1);
	ThrowIf(!succeeded);

	succeeded = ::SelectClipRgn(mContext, clip + offset);
	ThrowIf(!succeeded);

	POSTCONDITION(this->GetClip() == clip);	
}


//---------------------------------------------------------------
//
// XDrawContext::SetOrigin
//
//---------------------------------------------------------------
void XDrawContext::SetOrigin(const XPoint& origin)
{
	int32 succeeded = ::SetViewportOrgEx(mContext, origin.x, origin.y, nil);
	ThrowIf(!succeeded);

	POSTCONDITION(this->GetOrigin() == origin);	
}


}	// namespace Whisper

