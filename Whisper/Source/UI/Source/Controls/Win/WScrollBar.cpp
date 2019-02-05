/*
 *  File:       WScrollBar.cpp
 *  Summary:   	A control that displays an OS scroll bar.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WScrollBar.cpp,v $
 *		Revision 1.6  2001/04/21 03:33:35  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2000/12/31 04:45:02  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.4  2000/12/14 08:32:05  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 04:07:52  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:11:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IScrollBar.h>

#include <IClickExtensible.h>
#include <IControl.h>
#include <IHierarchy.h>
#include <IRootControl.h>
#include <IValueControl.h>
#include <IWindow.h>
#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XMouseEvents.h>

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

	virtual void 		HandleTrack(int32 code);
	
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
						const XRect& inFrame, int32 minValue, int32 maxValue,
						uint32 style, uint32 procID)
{
	UNUSED(procID);
	
	mPageDelta  = 1;
	mArrowDelta = 1;

	IControlPtr super = GetInterface<IControl>(parent);

	// Set the scrollbar width and tweak the window style
	XRect frame = inFrame;
	
	if (frame.GetWidth() > frame.GetHeight()) {
		style = style | SBS_HORZ;
		frame.bottom = frame.top + ::GetSystemMetrics(SM_CYHSCROLL);
	
	} else {
		style = style | SBS_VERT;
		frame.right = frame.left + ::GetSystemMetrics(SM_CXHSCROLL);
	}
	
	// Create the control
	HWND controlH = ::CreateWindowEx(0,						// extended style
									 _TEXT("SCROLLBAR"),	// class name
									 _TEXT(""),				// window name
									 style,					// style
									 frame.left,			// location
									 frame.top,
									 frame.GetWidth(),		// size
									 frame.GetHeight(),
									 super->GetOSControl(),	// parent
									 nil,					// menu
									 WSystemInfo::GetAppInstance(),	// app instance
									 nil);					// user data
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
	
	if (viewSize > 0)
		++viewSize;

	IValueControlPtr value(this);
	value->SetMaxValue(imageSize, redraw);

	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask  = SIF_PAGE;
	info.nPage  = numeric_cast<uint32>(viewSize);

    IControlPtr control(this);
    HWND controlH = control->GetOSControl();
    
	(void) ::SetScrollInfo(controlH, SB_CTL, &info, redraw);
}


//---------------------------------------------------------------
//
// XScrollBar::HandleTrack
//
//---------------------------------------------------------------
void XScrollBar::HandleTrack(int32 code)
{
	IValueControlPtr value(this);
	int32 newValue = value->GetValue();
	
    IControlPtr control(this);
    HWND controlH = control->GetOSControl();
    
	switch (LOWORD(code)) {
		case SB_LINELEFT:
			newValue -= 1;
			break;

		case SB_LINERIGHT:
			newValue += 1;
			break;

		case SB_PAGELEFT:
			newValue -= this->GetPageDelta();
			break;

		case SB_PAGERIGHT:
			newValue += this->GetPageDelta();
			break;

		case SB_LEFT:
			newValue = value->GetMinValue();
			break;

		case SB_RIGHT:
			newValue = value->GetMaxValue();
			break;

		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			{
			SCROLLINFO info;
			info.cbSize = sizeof(info);
			info.fMask  = SIF_TRACKPOS;
		
			if (::GetScrollInfo(controlH, SB_CTL, &info))
				newValue = info.nTrackPos;
			}
			break;
	}

	if (value->SetValue(newValue)) {
		IWindowPtr window = GetParent<IWindow>(this);
		XPoint pt = window->WindowToGlobal(control->LocalToWindow(XPoint(1, 1)));

		MSG msg;
		msg.hwnd    = controlH;
		msg.message = WM_LBUTTONDOWN;
		msg.wParam  = 0;				// key flags
		msg.lParam  = MAKELONG(1, 1);	// cursor position (client coordinates)
		msg.time    = ::GetCurrentTime();
		msg.pt.x    = pt.x;
		msg.pt.y    = pt.y;
		
		IClickExtensiblePtr extensible(this);
		if (extensible)
			extensible->HandleClick(msg);

		window->HandleUpdate();			// will handle any pending update in the window
	}
}


}	// namespace Whisper
