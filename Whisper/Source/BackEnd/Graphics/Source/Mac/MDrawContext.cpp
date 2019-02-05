/*
 *  File:       MDrawContext.cpp
 *  Summary:	Abstract base class for objects that can be drawn into.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: MDrawContext.cpp,v $
 *		Revision 1.5  2001/04/21 03:25:24  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/13 07:49:53  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 04:03:27  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:12:49  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDrawContext.h>

#include <MQuickDrawUtils.h>

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
	::SetGWorld(mContext, GetMainDevice());	

	::SetOrigin(numeric_cast<int16>(mOldOrigin.x), numeric_cast<int16>(mOldOrigin.y));
	::SetClip(mOldClip);
}


//---------------------------------------------------------------
//
// XSaveContext::XSaveContext
//
//---------------------------------------------------------------
XSaveContext::XSaveContext(OSDrawContext context)
{	
	PRECONDITION(context != nil);
	
	mContext = context;
	
	::SetGWorld(mContext, GetMainDevice());	
	
#if TARGET_CARBON
	Rect bounds;
	::GetPortBounds(mContext, &bounds);
	mOldOrigin.x = bounds.left;
	mOldOrigin.y = bounds.top;
#else
	XRect portRect = mContext->portRect;
	mOldOrigin = portRect[topLeft];
#endif

	::GetClip(mOldClip);
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
#if DEBUG
#if TARGET_CARBON
	if (GetQDGlobalsThePort() == this->GetOSContext())
#else
	if (qd.thePort == this->GetOSContext())
#endif
		::SetPort(nil);
#endif
}


//---------------------------------------------------------------
//
// XDrawContext::XDrawContext (OSDrawContext)
//
//---------------------------------------------------------------
XDrawContext::XDrawContext(OSDrawContext context) : mSaveContext(context)
{
	PRECONDITION(context != nil);
	PRECONDITION(IsColorPort(context));
	
	mContext = context;
	
	mDynamicCanvas  = true;
	mOffsceenCanvas = false;
}


//---------------------------------------------------------------
//
// XDrawContext::GetExtent
//
//---------------------------------------------------------------
XRect XDrawContext::GetExtent() const
{
	Rect bounds;
	::GetPortBounds(mContext, &bounds);

	return XRect(0, 0, bounds.right - bounds.left, bounds.bottom - bounds.top);
}


//---------------------------------------------------------------
//
// XDrawContext::GetOrigin
//
//---------------------------------------------------------------
XPoint XDrawContext::GetOrigin() const
{
	Rect bounds;
	::GetPortBounds(mContext, &bounds);

	return XPoint(-bounds.left, -bounds.top);
}


//---------------------------------------------------------------
//
// XDrawContext::GetClip
//
//---------------------------------------------------------------
XRegion XDrawContext::GetClip() const
{	
	::SetGWorld(mContext, GetMainDevice());	
	
	XRegion clipRgn;
	::GetClip(clipRgn);

	return clipRgn;
}

	
//---------------------------------------------------------------
//
// XDrawContext::SetClip
//
//---------------------------------------------------------------
void XDrawContext::SetClip(const XRegion& clipRgn)
{
	::SetGWorld(mContext, GetMainDevice());	

	::SetClip(clipRgn);

	POSTCONDITION(this->GetClip() == clipRgn);	
}


//---------------------------------------------------------------
//
// XDrawContext::SetOrigin
//
//---------------------------------------------------------------
void XDrawContext::SetOrigin(const XPoint& newOrigin)
{
	::SetGWorld(mContext, GetMainDevice());	
	
	XPoint oldOrigin = this->GetOrigin();
	if (oldOrigin != newOrigin) {
		::SetOrigin(numeric_cast<int16>(-newOrigin.x), numeric_cast<int16>(-newOrigin.y));
		
//		XPoint delta = newOrigin - oldOrigin;	
//		XRegion clipRgn = this->GetClip();
//		this->SetClip(clipRgn + delta);
	}

	POSTCONDITION(this->GetOrigin() == newOrigin);	
}


//---------------------------------------------------------------
//
// XDrawContext::MakeCurrent
//
//---------------------------------------------------------------
void XDrawContext::MakeCurrent()
{
	::SetGWorld(mContext, GetMainDevice());	
}


}	// namespace Whisper

