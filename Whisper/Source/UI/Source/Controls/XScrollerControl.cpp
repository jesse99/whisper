/*
 *  File:       XScrollerControl.cpp
 *  Summary:   	A user control used to scroll an IScrollableControl.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XScrollerControl.cpp,v $
 *		Revision 1.5  2001/10/03 02:13:08  jesjones
 *		Changed the way MatchInterface works (Pro 7 bug).
 *		
 *		Revision 1.4  2001/04/27 04:36:17  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/12/14 08:37:34  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:50:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IScrollerControl.h>

#include <IBinding.h>
#include <IClickExtensions.h>
#include <IGeometry.h>
#include <IScrollableControl.h>
#include <IScrollBar.h>
#include <IScrollerControl.h>
#include <IHierarchy.h>
#include <IValueControl.h>

namespace Whisper {

// ===================================================================================
//	class XScrollerControl
//!		A user control used to scroll an IScrollableControl.
// ===================================================================================
class XScrollerControl : public IScrollerControl {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XScrollerControl();
	
						XScrollerControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(bool hasVertBar = true, bool hasHorzBar = true, const XRect& indent = kZeroRect, bool fillWindow = true);

	virtual XSize		GetScrollUnits() const;
	virtual void		SetScrollUnits(const XSize& newUnits);

	virtual XRect		GetContentExtent() const;
						
	virtual void 		ScrollAreaHasChanged(bool redraw = kRedraw);

//-----------------------------------
//	Internal API
//
protected:
			void 		DoScrollHorz();			
			void 		DoScrollVert();
			
			void 		DoMakeVertScrollBar(int32 topIndent, int32 bottomIndent);
			void 		DoMakeHorzScrollBar(int32 leftIndent, int32 rightIndent);

//-----------------------------------
//	Internal API
//
private:
	IScrollableControlPtr	mScrollable;
	bool					mFillWindow;	
	
	IScrollBarPtr			mHorzScrollBar;
	IScrollBarPtr			mVertScrollBar;
};

DEFINE_INTERFACE_FACTORY(XScrollerControl)

//---------------------------------------------------------------
//
// XScrollerControl::~XScrollerControl
//
//---------------------------------------------------------------
XScrollerControl::~XScrollerControl()
{
}


//---------------------------------------------------------------
//
// XScrollerControl::XScrollerControl
//
//---------------------------------------------------------------
XScrollerControl::XScrollerControl(XBoss* boss)
{	
	IScrollerControl::DoSetBoss(boss);
	
	mFillWindow = false;
}


//---------------------------------------------------------------
//
// XScrollerControl::Init
//
//---------------------------------------------------------------
void XScrollerControl::Init(bool hasVertBar, bool hasHorzBar, const XRect& indent, bool fillWindow)
{
	PRECONDITION(!mScrollable);
	
	mFillWindow = fillWindow;

	IBindingPtr binding(this);
	if (binding)
		binding->SetBinding(SBinding(true, true, true, true));
	
	if (hasVertBar)
		this->DoMakeVertScrollBar(indent.top, indent.bottom);

	if (hasHorzBar)
		this->DoMakeHorzScrollBar(indent.left, indent.right);
}


//---------------------------------------------------------------
//
// XScrollerControl::GetScrollUnits
//
//---------------------------------------------------------------
XSize XScrollerControl::GetScrollUnits() const
{
	XSize units = kZeroSize;

	if (mHorzScrollBar)
		units.width = mHorzScrollBar->GetArrowDelta();
		
	if (mVertScrollBar)
		units.height = mVertScrollBar->GetArrowDelta();

	return units;
}


//---------------------------------------------------------------
//
// XScrollerControl::SetScrollUnits
//
//---------------------------------------------------------------
void XScrollerControl::SetScrollUnits(const XSize& newUnits)
{
	PRECONDITION(newUnits.width > 0);
	PRECONDITION(newUnits.height > 0);

	if (mHorzScrollBar)
		mHorzScrollBar->SetArrowDelta(newUnits.width);
		
	if (mVertScrollBar)
		mVertScrollBar->SetArrowDelta(newUnits.height);
}


//---------------------------------------------------------------
//
// XScrollerControl::GetContentExtent
//
//---------------------------------------------------------------
XRect XScrollerControl::GetContentExtent() const
{
	IConstGeometryPtr geometry(this);
	XRect extent = geometry->GetExtent();
	
	if (mHorzScrollBar) {
		IConstGeometryPtr horzGeom(mHorzScrollBar);
		extent.bottom -= horzGeom->GetHeight();
#if MAC
		if (mFillWindow)
			extent.bottom++;
#endif
	}
	
	if (mVertScrollBar) {
		IConstGeometryPtr vertGeom(mVertScrollBar);
		extent.right -= vertGeom->GetWidth();
#if MAC
		if (mFillWindow)
			extent.right++;
#endif
	}
	
	return extent;
}

												
//---------------------------------------------------------------
//
// XScrollerControl::ScrollAreaHasChanged
//
//---------------------------------------------------------------
void XScrollerControl::ScrollAreaHasChanged(bool redraw)
{
	if (!mScrollable) {			// can't do this in Init because the scrollable control may not be constructed then
		XCallback1<bool, IHierarchyPtr> predicate = MatchInterface<IScrollableControl>();
	
		IHierarchyPtr parent(this);
		IHierarchyPtr child = parent->FindChild(predicate);
		mScrollable = IScrollableControlPtr(child);
	}
	
	XRect revealedRect = mScrollable->GetRevealedRect();
	XPoint scrollPos = revealedRect[topLeft];
	
	XSize imageSize = mScrollable->GetImageSize();
	XSize viewSize  = revealedRect.GetSize();
	
	if (mHorzScrollBar) {
		mHorzScrollBar->UpdateThumb(imageSize.width, viewSize.width, redraw);

		IValueControlPtr horzValue(mHorzScrollBar);
		horzValue->SetValue(scrollPos.x, redraw);

		int32 dx = Max(revealedRect.GetWidth() - mHorzScrollBar->GetArrowDelta(), 0L);
		mHorzScrollBar->SetPageDelta(dx);
	}

	if (mVertScrollBar) {
		mVertScrollBar->UpdateThumb(imageSize.height, viewSize.height, redraw);
		
		IValueControlPtr vertValue(mVertScrollBar);
		vertValue->SetValue(scrollPos.y, redraw);

		int32 dy = Max(revealedRect.GetHeight() - mVertScrollBar->GetArrowDelta(), 0L);
		mVertScrollBar->SetPageDelta(dy);
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XScrollerControl::DoScrollHorz
//
//---------------------------------------------------------------
void XScrollerControl::DoScrollHorz()
{
	XPoint scrollPos;

	IValueControlPtr value(mHorzScrollBar);
	scrollPos.x = value->GetValue();
	scrollPos.y = mScrollable->GetRevealedRect().top;

	mScrollable->ScrollTo(scrollPos);
}


//---------------------------------------------------------------
//
// XScrollerControl::DoScrollVert
//
//---------------------------------------------------------------
void XScrollerControl::DoScrollVert()
{
	XPoint scrollPos;

	IValueControlPtr value(mVertScrollBar);
	scrollPos.x = mScrollable->GetRevealedRect().left;
	scrollPos.y = value->GetValue();
	
	mScrollable->ScrollTo(scrollPos);
}


//---------------------------------------------------------------
//
// XScrollerControl::DoMakeHorzScrollBar
//
//---------------------------------------------------------------
void XScrollerControl::DoMakeHorzScrollBar(int32 leftIndent, int32 rightIndent)
{
	PRECONDITION(!mHorzScrollBar);
	
#if MAC
	const int32 kScrollBarWidth  = 16;
	const int32 kScrollBarHeight = 16;
#elif WIN
	const int32 kScrollBarWidth  = ::GetSystemMetrics(SM_CXHSCROLL);
	const int32 kScrollBarHeight = ::GetSystemMetrics(SM_CYHSCROLL);
#endif
	
	IGeometryPtr geometry(this);
	XRect extent = geometry->GetExtent();
	
	XRect frame;
#if MAC
	if (mFillWindow) {
		frame.left   = extent.left + leftIndent - 1;
		frame.right  = frame.left + extent.GetWidth() - leftIndent - rightIndent - kScrollBarWidth + 3;
		frame.bottom = extent.bottom + 1;
		frame.top    = frame.bottom - kScrollBarHeight;
	} else
#endif
	{
		frame.left   = extent.left + leftIndent;
		frame.right  = frame.left + extent.GetWidth() - leftIndent - rightIndent - kScrollBarWidth + 1;
		frame.bottom = extent.bottom;
		frame.top    = extent.bottom - kScrollBarHeight;
	}
	
	mHorzScrollBar = IScrollBarPtr(L"Scroll Bar");
	mHorzScrollBar->Init(this, L"Horizontal Scrollbar", frame, 0 , 0);
	
	IBindingPtr binding(mHorzScrollBar);
	if (binding)
		binding->SetBinding(SBinding(true, false, true, true));
			
	ICallbackClickExtensionPtr extension(mHorzScrollBar);
	XCallback0<void> callback(this, &XScrollerControl::DoScrollHorz);	
	extension->Init(callback, 1);
}


//---------------------------------------------------------------
//
// XScrollerControl::DoMakeVertScrollBar
//
//---------------------------------------------------------------
void XScrollerControl::DoMakeVertScrollBar(int32 topIndent, int32 bottomIndent)
{
	PRECONDITION(!mVertScrollBar);
	
#if MAC
	const int32 kScrollBarWidth  = 16;		// $$$ Carbon has a function for this
	const int32 kScrollBarHeight = 16;
#elif WIN
	const int32 kScrollBarWidth  = ::GetSystemMetrics(SM_CXHSCROLL);
	const int32 kScrollBarHeight = ::GetSystemMetrics(SM_CYHSCROLL);
#endif
	
	IGeometryPtr geometry(this);
	XRect extent = geometry->GetExtent();
	
	XRect frame;
#if MAC
	if (mFillWindow) {
		frame.right  = extent.right + 1;
		frame.left   = frame.right - kScrollBarWidth;
		frame.top    = extent.top + topIndent - 1;
		frame.bottom = frame.top + extent.GetHeight() - topIndent - bottomIndent - kScrollBarHeight + 3;
	} else
#endif
	{
		frame.right  = extent.right;
		frame.left   = frame.right - kScrollBarWidth;
		frame.top    = extent.top + topIndent;
		frame.bottom = frame.top + extent.GetHeight() - topIndent - bottomIndent - kScrollBarHeight + 1;
	}
	
	mVertScrollBar = IScrollBarPtr(L"Scroll Bar");
	mVertScrollBar->Init(this, L"Vertical Scrollbar", frame, 0 , 0);
			
	IBindingPtr binding(mVertScrollBar);
	if (binding)
		binding->SetBinding(SBinding(false, true, true, true));
			
	ICallbackClickExtensionPtr extension(mVertScrollBar);
	XCallback0<void> callback(this, &XScrollerControl::DoScrollVert);	
	extension->Init(callback, 1);
}


}	// namespace Whisper
