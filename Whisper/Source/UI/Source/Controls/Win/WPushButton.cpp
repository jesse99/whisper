/*
 *  File:       WPushButton.cpp
 *  Summary:   	Push button control.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WPushButton.cpp,v $
 *		Revision 1.7  2001/03/20 12:02:58  jesjones
 *		Fixed a comment.
 *		
 *		Revision 1.6  2000/12/31 04:44:23  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.5  2000/12/14 08:31:46  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.4  2000/12/07 09:50:19  jesjones
 *		Fixed a compile error.
 *		
 *		Revision 1.3  2000/12/05 09:10:54  jesjones
 *		Added SimulateClick and some methods for default and cancel buttons.
 *		
 *		Revision 1.2  2000/11/09 12:11:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IPushButton.h>

#include <windowsx.h>

#include <IClickExtensible.h>
#include <IControl.h>
#include <IHierarchy.h>
#include <IText.h>
#include <IWindow.h>
#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XMiscUtils.h>
#include <XMouseEvents.h>

namespace Whisper {


// ===================================================================================
//	class XPushButton
//!		Push button control.
// ===================================================================================
class XPushButton : public IPushButton {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XPushButton();
	
						XPushButton(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 uint32 style = kDefaultPushButtonStyle,
							 uint32 procID = kDefaultPushButtonProcID);
							 
	virtual bool 		IsDefaultButton() const;
	virtual void 		MakeDefaultButton(bool make = true);

	virtual bool 		IsCancelButton() const;
	virtual void 		MakeCancelButton(bool make = true);

	virtual void 		SimulateClick();

//-----------------------------------
//	Member Data
//
private:
	bool	mIsDefault;
	bool	mIsCancel;
};

DEFINE_INTERFACE_FACTORY(XPushButton)

//---------------------------------------------------------------
//
// XPushButton::~XPushButton
//
//---------------------------------------------------------------
XPushButton::~XPushButton()
{
}


//---------------------------------------------------------------
//
// XPushButton::XPushButton
//
//---------------------------------------------------------------
XPushButton::XPushButton(XBoss* boss) 
{	
	this->DoSetBoss(boss);

	mIsDefault = false;
	mIsCancel = false;
}


//---------------------------------------------------------------
//
// XPushButton::Init
//
//---------------------------------------------------------------
void XPushButton::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, const std::wstring& inText, 
					   uint32 style, uint32 procID)
{
	UNUSED(procID);
	
	IControlPtr super = GetInterface<IControl>(parent);

	// Create the control
	HWND controlH = ::CreateWindowEx(0,						// extended style
									 _TEXT("BUTTON"),		// class name
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
	
	// Set the control's title		
	ITextPtr text(this);
	text->SetText(inText, kDontRedraw);
}


//---------------------------------------------------------------
//
// XPushButton::IsDefaultButton
//
//---------------------------------------------------------------
bool XPushButton::IsDefaultButton() const
{
	return mIsDefault;
}


//---------------------------------------------------------------
//
// XPushButton::MakeDefaultButton
//
//---------------------------------------------------------------
void XPushButton::MakeDefaultButton(bool make)
{
	mIsDefault = make;
}


//---------------------------------------------------------------
//
// XPushButton::IsCancelButton
//
//---------------------------------------------------------------
bool XPushButton::IsCancelButton() const
{
	return mIsCancel;
}


//---------------------------------------------------------------
//
// XPushButton::MakeCancelButton
//
//---------------------------------------------------------------
void XPushButton::MakeCancelButton(bool make)
{
	mIsCancel = make;
}


//---------------------------------------------------------------
//
// XPushButton::SimulateClick
//
//---------------------------------------------------------------
void XPushButton::SimulateClick()
{
	IConstControlPtr control(this);
	HWND controlH = control->GetOSControl();
	
	if (control->IsEnabled()) {
		int32 oldState = Button_GetState(controlH);
		Button_SetState(controlH, true);
	
		MilliSecondDelay(133);
		
		Button_SetState(controlH, oldState);

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
	}
}


}	// namespace Whisper
