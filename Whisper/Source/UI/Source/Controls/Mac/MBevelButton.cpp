/*
 *  File:       MBevelButton.cpp
 *  Summary:   	A control that displays a button containing text or an image and an
 *				optional menu.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MBevelButton.cpp,v $
 *		Revision 1.1  2001/03/20 11:58:03  jesjones
 *		Checked in (sans menu support and with minimal testing)
 *		
 */

#include <XWhisperHeader.h>
#include <IBevelButton.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IText.h>
#include <MQuickDrawUtils.h>
#include <XExceptions.h>
#include <XLocker.h>
#include <XPixMap.h>

namespace Whisper {


// ===================================================================================
//	class XBevelButton
//!		A control that displays a button containing text or an image and an optional menu.
// ===================================================================================
class XBevelButton : public IBevelButton {

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

	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const ControlButtonContentInfo& content,
							 EBevelBehavior behavior,
							 int16 align = kControlBevelButtonAlignCenter,
							 uint32 style = kDefaultBevelButtonStyle,
							 uint32 procID = kDefaultBevelButtonProcID);
						
	virtual void 		SetTextAlignment(EBevelTextAlignment align, bool redraw = kRedraw);
	virtual void 		SetTextPlacement(int16 placement, bool redraw = kRedraw);
	
	virtual void 		SetImage(ResID iconID, bool redraw = kRedraw);
	virtual void 		SetImage(const XPixMap& image, bool redraw = kRedraw);
	virtual void 		SetImageAlignment(int16 align, bool redraw = kRedraw);
	
//-----------------------------------
//	Internal API
//
private:
			ControlHandle DoCreateButton(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, EBevelBehavior behavior, int16 contentType, uint32 procID);

//-----------------------------------
//	Member Data
//
private:
	PicHandle	mPicture;
};

DEFINE_INTERFACE_FACTORY(XBevelButton)

//---------------------------------------------------------------
//
// XBevelButton::~XBevelButton
//
//---------------------------------------------------------------
XBevelButton::~XBevelButton()
{
	if (mPicture != nil)
		KillPicture(mPicture);		
}


//---------------------------------------------------------------
//
// XBevelButton::XBevelButton
//
//---------------------------------------------------------------
XBevelButton::XBevelButton(XBoss* boss) 
{	
	IBevelButton::DoSetBoss(boss);
	
	mPicture = nil;
}


//---------------------------------------------------------------
//
// XBevelButton::Init (IHierarchyPtr, wstring, XRect, wstring, EBevelBehavior, EBevelTextAlignment, uint32, uint32)
//
//---------------------------------------------------------------
void XBevelButton::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, const std::wstring& inText, EBevelBehavior behavior, EBevelTextAlignment align, uint32 style, uint32 procID)
{
	UNUSED(style);
	
	this->DoCreateButton(parent, name, frame, behavior, kControlContentTextOnly, procID);
	
	ITextPtr text(this);
	text->SetText(inText, kDontRedraw);
	this->SetTextAlignment(align, kDontRedraw);
}


//---------------------------------------------------------------
//
// XBevelButton::Init (IHierarchyPtr, wstring& name, XRect, ResID, EBevelBehavior, uint32, uint32)
//
//---------------------------------------------------------------
void XBevelButton::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, ResID iconID, EBevelBehavior behavior, uint32 style, uint32 procID)
{
	UNUSED(style);
	
	this->DoCreateButton(parent, name, frame, behavior, kControlContentTextOnly, procID);
	
	this->SetImage(iconID, kDontRedraw);
}


//---------------------------------------------------------------
//
// XBevelButton::Init (IHierarchyPtr, wstring, XRect, XPixMap, EBevelBehavior, uint32, uint32)
//
//---------------------------------------------------------------
void XBevelButton::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, const XPixMap& image, EBevelBehavior behavior, uint32 style, uint32 procID)
{
	UNUSED(style);
	
	this->DoCreateButton(parent, name, frame, behavior, kControlContentTextOnly, procID);
	
	this->SetImage(image, kDontRedraw);
}


//---------------------------------------------------------------
//
// XBevelButton::Init (IHierarchyPtr, wstring, XRect, ControlButtonContentInfo, EBevelBehavior, int16, uint32, uint32)
//
//---------------------------------------------------------------
void XBevelButton::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, const ControlButtonContentInfo& info, EBevelBehavior behavior, int16 align, uint32 style, uint32 procID)
{
	UNUSED(style);
	
	ControlHandle controlH = this->DoCreateButton(parent, name, frame, behavior, kControlContentTextOnly, procID);
	
	IControlPtr super = GetParent<IControl>(this);
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	OSStatus err = SetControlData(controlH, kControlEntireControl, kControlBevelButtonContentTag, sizeof(info), (Ptr) &info);
	::ShowPen();
	ASSERT(err == noErr);
	
	this->SetImageAlignment(align, kDontRedraw);
}


//---------------------------------------------------------------
//
// XBevelButton::SetTextAlignment
//
//---------------------------------------------------------------
void XBevelButton::SetTextAlignment(EBevelTextAlignment inAlign, bool redraw)
{
	int16 align;
	switch (inAlign) {
		case kAlignBevelTextLeft:
			align = kControlBevelButtonAlignTextFlushLeft;
			break;
			
		case kAlignBevelTextRight:
			align = kControlBevelButtonAlignTextFlushRight;
			break;

		case kAlignBevelTextCenter:
			align = kControlBevelButtonAlignTextCenter;
			break;

		case kAlignBevelTextSystem:
			align = kControlBevelButtonAlignTextSysDirection;
			break;

		default:
			align = kControlBevelButtonAlignTextCenter;
			DEBUGSTR(L"Bad bevel button text alignment");
	}

	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	IControlPtr super = GetParent<IControl>(this);
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	OSStatus err = SetControlData(controlH, kControlEntireControl, kControlBevelButtonTextAlignTag, sizeof(align), (Ptr) &align);
	::ShowPen();
	ASSERT(err == noErr);
		
	if (redraw)
		control->Invalidate();
}

	
//---------------------------------------------------------------
//
// XBevelButton::SetTextPlacement
//
//---------------------------------------------------------------
void XBevelButton::SetTextPlacement(int16 placement, bool redraw)
{
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	IControlPtr super = GetParent<IControl>(this);
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	OSStatus err = SetControlData(controlH, kControlEntireControl, kControlBevelButtonTextAlignTag, sizeof(placement), (Ptr) &placement);
	::ShowPen();
	ASSERT(err == noErr);
		
	if (redraw)
		control->Invalidate();
}


//---------------------------------------------------------------
//
// XBevelButton::SetImage (ResID, bool)
//
//---------------------------------------------------------------
void XBevelButton::SetImage(ResID iconID, bool redraw)
{
	ControlButtonContentInfo info;
	info.contentType = kControlContentCIconRes;
	info.u.resID     = iconID;
	
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	IControlPtr super = GetParent<IControl>(this);
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	OSStatus err = SetControlData(controlH, kControlEntireControl, kControlBevelButtonContentTag, sizeof(info), (Ptr) &info);
	::ShowPen();
	ThrowIfOSErr(err);
		
	if (redraw)
		control->Invalidate();
}


//---------------------------------------------------------------
//
// XBevelButton::SetImage (XPixMap, bool)
//
//---------------------------------------------------------------
void XBevelButton::SetImage(const XPixMap& image, bool redraw)
{
	if (mPicture != nil) {
		KillPicture(mPicture);
		mPicture = nil;
	}

	{
	Rect extent = image.GetExtent().GetOSRect();
	
	::SetPort(GetWorkPort());
	MSetClip setter(extent);
	mPicture = ::OpenPicture(&extent);
	ThrowIfNil(mPicture);
	
	XLocker lock(image);
#if TARGET_CARBON
	::CopyBits((BitMap*) *GetPortPixMap(image.GetWorld()), (BitMap*) *GetPortPixMap(GetQDGlobalsThePort()), &extent, &extent, srcCopy, nil);
#else
	::CopyBits((BitMap*) &(image.GetWorld()->portPixMap), &qd.thePort->portBits, &extent, &extent, srcCopy, nil);
#endif
	::ClosePicture();
	}

	ControlButtonContentInfo info;
	info.contentType = kControlContentPictHandle;
	info.u.picture   = mPicture;					// docs imply that we need to keep this around until the control goes away
	
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	IControlPtr super = GetParent<IControl>(this);
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	OSStatus err = SetControlData(controlH, kControlEntireControl, kControlBevelButtonContentTag, sizeof(info), (Ptr) &info);
	::ShowPen();
	ThrowIfOSErr(err);
		
	if (redraw)
		control->Invalidate();
}


//---------------------------------------------------------------
//
// XBevelButton::SetImageAlignment
//
//---------------------------------------------------------------
void XBevelButton::SetImageAlignment(int16 align, bool redraw)
{
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	IControlPtr super = GetParent<IControl>(this);
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	OSStatus err = SetControlData(controlH, kControlEntireControl, kControlBevelButtonGraphicAlignTag, sizeof(align), (Ptr) &align);
	::ShowPen();
	ASSERT(err == noErr);
		
	if (redraw)
		control->Invalidate();
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XBevelButton::DoCreateButton
//
//---------------------------------------------------------------
ControlHandle XBevelButton::DoCreateButton(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, EBevelBehavior behavior, int16 contentType, uint32 procID)
{
	// Get the initial menu selection and the button flags
	int16 initial = 0;
//	int16 initial = int16_cast(menu != nil ? 1 : 0);
		
	int16 min = contentType;
	switch (behavior) {
		case kBevelResets:
			min |= kControlBehaviorPushbutton;
			break;
			
		case kBevelToggles:
			min |= kControlBehaviorToggles;
			break;

		case kBevelSticks:
			min |= kControlBehaviorSticky;
			break;

		default:
			DEBUGSTR(L"Bad bevel button behavior");
	}
	
	// Get the control's bounding box in window coordinates
	IControlPtr super = GetInterface<IControl>(parent);
	XRect bounds = super->LocalToWindow(frame);
	
	// Set the port and hide the pen (we want to wait for an
	// update event before we draw anything)
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	
	// Create the control and show the pen
	Rect temp = bounds.GetOSRect();
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, initial, min, 0, (int16) procID, 0);

	OSStatus err = noErr;
#if 0	
	if (controlH != nil && menu != nil) {
		MenuHandle menuH = menu->GetOSMenu();
		err = SetControlData(controlH, kControlEntireControl, kControlBevelButtonMenuHandleTag, sizeof(menuH), (Ptr) &menuH);

		if (err == noErr) {
			int16 value = 0;
			err = SetControlData(controlH, kControlEntireControl, kControlBevelButtonMenuValueTag, sizeof(value), (Ptr) &value);
		}
	}
#endif

	::ShowPen();
	ThrowIfNil(controlH);
	if (err != noErr) {
		DisposeControl(controlH);
		ThrowIfOSErr(err);
	}
	
	// Let the IControl interface do a bit of initialization
	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
	
	return controlH;
}


}	// namespace Whisper
