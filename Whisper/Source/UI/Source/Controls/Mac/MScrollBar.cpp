/*
 *  File:       MScrollBar.cpp
 *  Summary:   	A control that displays an OS scroll bar.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MScrollBar.cpp,v $
 *		Revision 1.5  2001/04/21 03:28:27  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2000/12/31 04:33:43  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:28:51  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:19:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IScrollBar.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IRootControl.h>
#include <IValueControl.h>
#include <IWindow.h>
#include <MSystemInfo.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XScrollBar
//!		A control that displays an OS scroll bar.
// ===================================================================================
class XScrollBar : public IScrollBar {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XScrollBar();
	
						XScrollBar(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, int32 minValue, int32 maxValue,
							 uint32 style = kDefaultScrollBarStyle,
							 uint32 procID = kDefaultScrollBarProcID);

	virtual int32		GetPageDelta() const                {return mPageDelta;}
	virtual void		SetPageDelta(int32 newDelta);

	virtual int32		GetArrowDelta() const               {return mArrowDelta;}
	virtual void		SetArrowDelta(int32 newDelta);

	virtual void 		UpdateThumb(int32 imageSize, int32 viewSize, bool redraw = kRedraw);

//-----------------------------------
//	Member Data
//
protected:
	int32	mPageDelta;
	int32	mArrowDelta;
};

DEFINE_INTERFACE_FACTORY(XScrollBar)

//---------------------------------------------------------------
//
// XScrollBar::~XScrollBar
//
//---------------------------------------------------------------
XScrollBar::~XScrollBar()
{
}


//---------------------------------------------------------------
//
// XScrollBar::XScrollBar
//
//---------------------------------------------------------------
XScrollBar::XScrollBar(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XScrollBar::Init 
//
//---------------------------------------------------------------
void XScrollBar::Init(const IHierarchyPtr& parent, const std::wstring& name, 
						const XRect& frame, int32 minValue, int32 maxValue,
						uint32 style, uint32 procID)
{
	UNUSED(style);
	
	mPageDelta  = 1;
	mArrowDelta = 1;

	// Get the scrollbar width $$$ I think Carbon has a function for this $$$
	int32 width = 16;
	IWindowPtr window = GetParent<IWindow>(parent);
	if (window)
		if (window->GetType() == kFloatingWindow)
			width = 11;
		
	// Get the control's bounding box in window coordinates
	IControlPtr super = GetInterface<IControl>(parent);
	XRect bounds = super->LocalToWindow(frame);
	
	if (frame.GetWidth() > frame.GetHeight())
		bounds.bottom = bounds.top + width;
	else
		bounds.right = bounds.left + width;
		
	// Set the port and hide the pen (we want to wait for an
	// update event before we draw anything)
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	
	// Create the control and show the pen
	Rect temp = bounds.GetOSRect();
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, 0, 0, 0, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
	
	// Set the min and max values
	IValueControlPtr value(this);
	value->SetMinMaxValue(minValue, maxValue, kDontRedraw);
}


//---------------------------------------------------------------
//
// XScrollBar::SetPageDelta
//
//---------------------------------------------------------------
void XScrollBar::SetPageDelta(int32 newDelta)
{
	PRECONDITION(newDelta >= 0);

	mPageDelta = newDelta;
}


//---------------------------------------------------------------
//
// XScrollBar::SetArrowDelta
//
//---------------------------------------------------------------
void XScrollBar::SetArrowDelta(int32 newDelta)
{
	PRECONDITION(newDelta >= 0);

	mArrowDelta = newDelta;
}


//---------------------------------------------------------------
//
// XScrollBar::UpdateThumb
//
//---------------------------------------------------------------
void  XScrollBar::UpdateThumb(int32 imageSize, int32 viewSize, bool redraw)
{
	PRECONDITION(imageSize >= 0);
	PRECONDITION(viewSize >= 0);
	
	IValueControlPtr valuePtr(this);
	int32 value = valuePtr->GetValue();

	int32 diff = Max(imageSize - viewSize, value);
	int32 max  = Max(diff, 0L);

	valuePtr->SetMaxValue(max, redraw);

#if UNIVERSAL_INTERFACES_VERSION >= 0x0320 
	if (MSystemInfo::GetAppearanceVersion() >= 0x0110) {
    	IControlPtr control(this);
    	ControlHandle controlH = control->GetOSControl();

		if (::GetControlViewSize(controlH) != viewSize) {
			IControlPtr root = GetParent<IRootControl>(this);
			XControlContext context(root);
			context.MakeCurrent();
	
			if (!redraw)
				::HidePen();

			::SetControlViewSize(controlH, viewSize);	
	
			if (!redraw)
				::ShowPen();
		}
	}
#endif
}


}	// namespace Whisper
