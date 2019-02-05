/*
 *  File:       MPushButton.cpp
 *  Summary:   	Push button control.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MPushButton.cpp,v $
 *		Revision 1.6  2001/04/21 03:27:53  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2000/12/31 04:32:58  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.4  2000/12/14 08:28:32  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/05 09:10:32  jesjones
 *		Added SimulateClick and some methods for default and cancel buttons.
 *		
 *		Revision 1.2  2000/11/09 09:18:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IPushButton.h>

#include <IClickExtensible.h>
#include <IControl.h>
#include <IHierarchy.h>
#include <IText.h>
#include <IWindow.h>
#include <MSystemInfo.h>
#include <XExceptions.h>
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

	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 ResID iconID, uint32 style, uint32 procID);
	
	virtual bool 		IsDefaultButton() const;
	virtual void 		MakeDefaultButton(bool make = true);

	virtual bool 		IsCancelButton() const;
	virtual void 		MakeCancelButton(bool make = true);

	virtual void 		SimulateClick();

private:
			void 		DoCreateControl(const IHierarchyPtr& parent, const std::wstring& name, 
							 			const XRect& frame, const std::wstring& text, 
							 			ResID iconID, uint32 procID);
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
}


//---------------------------------------------------------------
//
// XPushButton::Init (IHierarchyPtr, wstring, XRect, wstring, uint32, uint32)
//
//---------------------------------------------------------------
void XPushButton::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, const std::wstring& text, 
					   uint32 style, uint32 procID)
{
	UNUSED(style);
	
	this->DoCreateControl(parent, name, frame, text, 1, procID);
}


//---------------------------------------------------------------
//
// XPushButton::Init (IHierarchyPtr, wstring, XRect&, wstring, ResID, uint32, uint32)
//
//---------------------------------------------------------------
void XPushButton::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, const std::wstring& text, 
					   ResID iconID,  uint32 style, uint32 procID)
{
	UNUSED(style);
	
	this->DoCreateControl(parent, name, frame, text, iconID, procID);
}


//---------------------------------------------------------------
//
// XPushButton::IsDefaultButton
//
//---------------------------------------------------------------
bool XPushButton::IsDefaultButton() const
{
	Boolean is = false;
	
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	int32 actualLength = 0;
	OSErr err = GetControlData(controlH, kControlEntireControl, kControlPushButtonDefaultTag, sizeof(is), (Ptr) &is, &actualLength);
	
	return err == noErr && is;
}


//---------------------------------------------------------------
//
// XPushButton::MakeDefaultButton
//
//---------------------------------------------------------------
void XPushButton::MakeDefaultButton(bool make)
{
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	Boolean is = make;
	OSErr err = SetControlData(controlH, kControlEntireControl, kControlPushButtonDefaultTag, sizeof(is), (Ptr) &is);
	ASSERT(err == noErr);
		
	POSTCONDITION(this->IsDefaultButton() == make);
}


//---------------------------------------------------------------
//
// XPushButton::IsCancelButton
//
//---------------------------------------------------------------
bool XPushButton::IsCancelButton() const
{
	Boolean is = false;
	
	OSErr err = noErr;
	if (MSystemInfo::GetAppearanceVersion() >= 0x0110) {
		IConstControlPtr control(this);
		ControlHandle controlH = control->GetOSControl();
	
		int32 actualLength = 0;
		err = GetControlData(controlH, kControlEntireControl, kControlPushButtonCancelTag, sizeof(is), (Ptr) &is, &actualLength);
	}
	
	return err == noErr && is;
}


//---------------------------------------------------------------
//
// XPushButton::MakeCancelButton
//
//---------------------------------------------------------------
void XPushButton::MakeCancelButton(bool make)
{
	if (MSystemInfo::GetAppearanceVersion() >= 0x0110) {
		IConstControlPtr control(this);
		ControlHandle controlH = control->GetOSControl();
	
		Boolean is = make;
		OSErr err = SetControlData(controlH, kControlEntireControl, kControlPushButtonCancelTag, sizeof(is), (Ptr) &is);
		ASSERT(err == noErr);
	}
	
	POSTCONDITION(MSystemInfo::GetAppearanceVersion() < 0x0110 || this->IsCancelButton() == make);
}


//---------------------------------------------------------------
//
// XPushButton::SimulateClick
//
//---------------------------------------------------------------
void XPushButton::SimulateClick()
{
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	if (control->IsEnabled()) {
		::HiliteControl(controlH, kControlButtonPart);
		
		uint32 ticks;
		::Delay(8, &ticks);
		
		::HiliteControl(controlH, 0);
		
		IWindowPtr window = GetParent<IWindow>(this);

		EventRecord event;
		event.what      = mouseDown;
		event.message   = 0;				// undefined for mouseDown events
		event.when      = TickCount();
		event.where     = window->WindowToGlobal(control->LocalToWindow(XPoint(1, 1))).GetOSPoint();
		event.modifiers = 0;
		
		IClickExtensiblePtr extensible(this);
		if (extensible)
			extensible->HandleClick(event);
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XPushButton::DoCreateControl
//
//---------------------------------------------------------------
void XPushButton::DoCreateControl(const IHierarchyPtr& parent, const std::wstring& name, 
				 				  const XRect& frame, const std::wstring& inText, 
				 				  ResID iconID, uint32 procID)
{
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
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, 0, 0, iconID, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
	
	// Set the control's title		
	ITextPtr text(this);
	text->SetText(inText, kDontRedraw);
}


}	// namespace Whisper
