/*
 *  File:       WBevelButton.cpp
 *  Summary:   	A control that displays a button containing text or an image and an
 *				optional menu.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WBevelButton.cpp,v $
 *		Revision 1.1  2001/03/20 11:58:20  jesjones
 *		Checked in (sans menu support and with no testing)
 *		
 */

#include <XWhisperHeader.h>
#include <IBevelButton.h>

#include <windowsx.h>

#include <IControl.h>
#include <IControlPressedHelper.h>
#include <IHierarchy.h>
#include <IText.h>
#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XPixMap.h>

namespace Whisper {


// ===================================================================================
//	class XBevelButton
//!		A control that displays a button containing text or an image and an optional menu.
// ===================================================================================
class XBevelButton : public IBevelButton, public IControlPressedHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XBevelButton();
	
						XBevelButton(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text,
							 EBevelBehavior behavior,
							 EBevelTextAlignment align = kAlignBevelTextCenter,
							 uint32 style = kDefaultBevelButtonStyle,
							 uint32 procID = kDefaultBevelButtonProcID);

	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, ResID iconID,
							 EBevelBehavior behavior,
							 uint32 style = kDefaultBevelButtonStyle,
							 uint32 procID = kDefaultBevelButtonProcID);

	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const XPixMap& image,
							 EBevelBehavior behavior,
							 uint32 style = kDefaultBevelButtonStyle,
							 uint32 procID = kDefaultBevelButtonProcID);
						
	virtual void 		SetTextAlignment(EBevelTextAlignment align, bool redraw = kRedraw);
	
	virtual void 		SetImage(ResID iconID, bool redraw = kRedraw);
	virtual void 		SetImage(const XPixMap& image, bool redraw = kRedraw);

protected:
	virtual void 		OnPressed(const XMouseDownEvent& event);
	
//-----------------------------------
//	Internal API
//
private:
			void 		DoCreateButton(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, uint32 style);

//-----------------------------------
//	Member Data
//
private:
	EBevelBehavior	mBehavior;
	HBITMAP			mBitMap;
};

DEFINE_INTERFACE_FACTORY(XBevelButton)

//---------------------------------------------------------------
//
// XBevelButton::~XBevelButton
//
//---------------------------------------------------------------
XBevelButton::~XBevelButton()
{
	if (mBitMap != nil) {
		::DeleteObject(mBitMap);
		mBitMap = nil;
	}
}


//---------------------------------------------------------------
//
// XBevelButton::XBevelButton
//
//---------------------------------------------------------------
XBevelButton::XBevelButton(XBoss* boss) 
{	
	IBevelButton::DoSetBoss(boss);
	IControlPressedHelper::DoSetBoss(boss);

	mBitMap = nil;
}


//---------------------------------------------------------------
//
// XBevelButton::Init (IHierarchyPtr, wstring, XRect, wstring, EBevelBehavior, EBevelTextAlignment, uint32, uint32)
//
//---------------------------------------------------------------
void XBevelButton::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, const std::wstring& inText, EBevelBehavior behavior, EBevelTextAlignment align, uint32 style, uint32 procID)
{
	UNUSED(procID);
	
	mBehavior = behavior;

	this->DoCreateButton(parent, name, frame, style);
	
	IBevelButton* thisPtr = this;
	ITextPtr text(thisPtr);
	text->SetText(inText, kDontRedraw);
	this->SetTextAlignment(align, kDontRedraw);
}


//---------------------------------------------------------------
//
// XBevelButton::Init (IHierarchyPtr, wstring, XRect, ResID, EBevelBehavior, uint32, uint32)
//
//---------------------------------------------------------------
void XBevelButton::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, ResID iconID, EBevelBehavior behavior, uint32 style, uint32 procID)
{
	UNUSED(procID);
	
	mBehavior = behavior;

	this->DoCreateButton(parent, name, frame, style);
	
	this->SetImage(iconID, kDontRedraw);
}


//---------------------------------------------------------------
//
// XBevelButton::Init (IHierarchyPtr, wstring, XRect, XPixMap, EBevelBehavior, uint32, uint32)
//
//---------------------------------------------------------------
void XBevelButton::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, const XPixMap& image, EBevelBehavior behavior, uint32 style, uint32 procID)
{
	UNUSED(procID);
	
	mBehavior = behavior;

	this->DoCreateButton(parent, name, frame, style);
	
	this->SetImage(image, kDontRedraw);
}

						
//---------------------------------------------------------------
//
// XBevelButton::SetTextAlignment
//
//---------------------------------------------------------------
void XBevelButton::SetTextAlignment(EBevelTextAlignment align, bool redraw)
{
	IBevelButton* thisPtr = this;
	IControlPtr control(thisPtr);
	HWND controlH = control->GetOSControl();
	
	int32 style = ::GetWindowLong(controlH, GWL_STYLE);
	ThrowIf(style == 0);
	
	style &= ~(BS_LEFT | BS_CENTER | BS_RIGHT);

	switch (align) {
		case kAlignBevelTextLeft:
			style |= BS_LEFT;
			break;
			
		case kAlignBevelTextCenter:
			style |= BS_CENTER;
			break;

		case kAlignBevelTextRight:
			style |= BS_RIGHT;
			break;

		case kAlignBevelTextSystem:
			style |= BS_LEFT;		// $$$ is there a way to find out if we're running on a right-justified system?
			break;

		default:
			style |= BS_LEFT;
			DEBUGSTR(L"Unsupported bevel alignment flag!");
	}			
	
	if (!redraw)
		VERIFY(::LockWindowUpdate(controlH));

	::SetLastError(0);
	int32 oldValue = ::SetWindowLong(controlH, GWL_STYLE, style);
	uint32 err = GetLastError();

	if (!redraw)
		VERIFY(::LockWindowUpdate(nil));
	ThrowIf(oldValue == 0 && err != noErr);
}

	
//---------------------------------------------------------------
//
// XBevelButton::SetImage (ResID, bool)
//
//---------------------------------------------------------------
void XBevelButton::SetImage(ResID iconID, bool redraw)
{
	IBevelButton* thisPtr = this;
	IControlPtr control(thisPtr);
	HWND controlH = control->GetOSControl();
	
	HICON iconH = ::LoadIcon(nil, MAKEINTRESOURCE(iconID));		// deleted by the OS
	ThrowIfNil(iconH);
	
	if (!redraw)
		VERIFY(::LockWindowUpdate(controlH));

	(void) SendMessage(controlH, BM_SETIMAGE, IMAGE_ICON, (LPARAM) iconH);

	if (!redraw)
		VERIFY(::LockWindowUpdate(nil));

}

	
//---------------------------------------------------------------
//
// XBevelButton::SetImage (XPixMap, bool)
//
// The documentation neglects to mention this, but BM_SETIMAGE
// does not work with DIB-sections so we convert the pixmap to
// a device-dependant bitmap.
//
//---------------------------------------------------------------
void XBevelButton::SetImage(const XPixMap& image, bool redraw)
{
	IBevelButton* thisPtr = this;
	IControlPtr control(thisPtr);
	HWND controlH = control->GetOSControl();
	
	if (mBitMap != nil) {
		::DeleteObject(mBitMap);
		mBitMap = nil;
	}
	
	mBitMap = image.CreateCompatibleBitmap(control->GetOSDrawContext());
	
	if (!redraw)
		VERIFY(::LockWindowUpdate(controlH));

	(void) SendMessage(controlH, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) mBitMap);

	if (!redraw)
		VERIFY(::LockWindowUpdate(nil));
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XBevelButton::OnPressed
//
//---------------------------------------------------------------
void XBevelButton::OnPressed(const XMouseDownEvent& event) 
{
	UNUSED(event);
	
	IBevelButton* thisPtr = this;
	IControlPtr control(thisPtr);
	HWND controlH = control->GetOSControl();
	
	int32 value = Button_GetCheck(controlH);
	switch (mBehavior) {
		case kBevelResets:
			value = 0;
			break;
			
		case kBevelToggles:
			value = value ? 0 : 1;
			break;
			
		case kBevelSticks:
			value = 1;
			break;

		default:
			DEBUGSTR(L"Bad bevel button behavior");
	}
		
	Button_SetCheck(controlH, value);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XBevelButton::DoCreateButton
//
//---------------------------------------------------------------
void XBevelButton::DoCreateButton(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, uint32 style)
{
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
	IBevelButton* thisPtr = this;
	IControlPtr control(thisPtr);
	control->DoPostCreate(name, parent, controlH);
}


}	// namespace Whisper
