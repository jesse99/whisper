/*
 *  File:       XColorSwatch.cpp
 *  Summary:   	A control that draws a colored square and lets the user change the
 *				color using an OS color picker dialog.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XColorSwatch.cpp,v $
 *		Revision 1.9  2001/02/21 08:17:40  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.8  2001/02/08 03:54:34  jesjones
 *		Context menus no longer take a menu handler.
 *		
 *		Revision 1.7  2001/02/07 02:29:23  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.6  2001/02/04 03:47:47  jesjones
 *		Removed an obsolete include.
 *		
 *		Revision 1.5  2001/02/04 03:38:23  jesjones
 *		Removed implementation inheritance from the mouse handling classes.
 *		
 *		Revision 1.4  2001/01/30 07:23:39  jesjones
 *		Tweaked menu code.
 *		
 *		Revision 1.3  2001/01/21 00:42:58  jesjones
 *		Reworked the menu handling code.
 *		
 *		Revision 1.2  2000/12/24 03:24:48  jesjones
 *		Fixed a silly bug in the menu handling code.
 *		
 *		Revision 1.1  2000/12/17 03:38:59  jesjones
 *		Checked in	
 */

#include <XWhisperHeader.h>
#include <IColorSwatch.h>

#include <IContextMenu.h>
#include <IContextMenuHelper.h>
#include <IControl.h>
#include <IDrawHelper.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IInstallMenuHandler.h>
#include <IMenu.h>
#include <IMenuHandler.h>
#include <IMouseDownHelper.h>
#include <IRootControl.h>
#include <XBind.h>
#include <XHSVColor.h>
#include <XIntConversions.h>
#include <XMouseEvents.h>
#include <XShapes.h>

namespace Whisper {

// ===================================================================================
//	class XColorSwatch
//!		A control that draws a colored square and lets the user change the color using an OS color picker dialog.
// ===================================================================================
class XColorSwatch : public IColorSwatch, public IDrawHelper, public IInstallMenuHandler, public IContextMenuHelper, public IMouseDownHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XColorSwatch();
	
						XColorSwatch(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual XRGBColor 	GetColor() const									{return mColor;}
	virtual void 		SetColor(const XRGBColor& color, bool redraw = kRedraw);

	virtual void 		SetPrompt(const std::wstring& prompt)				{mPrompt = prompt;}

	virtual void 		SetColorSet(const XColorSet& colors);

	virtual void 		SetChangedCallback(const ChangeCallback& callback)	{mChangedCallback = callback;}

	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

protected:
	virtual void 		OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);

	virtual bool 		OnMouseDown(const XMouseDownEvent& event);

	virtual bool 		OnContextMenu(const XMouseDownEvent& event);

//-----------------------------------
//	Internal API
//
protected:
			void 		DoUserSetColor(const XRGBColor& color);

			SCommandStatus DoEnableColor(const XRGBColor& color);

//-----------------------------------
//	Member Data
//
protected:
	XRGBColor		mColor;
	std::wstring	mPrompt;
	XColorSet		mColorSet;

	ChangeCallback	mChangedCallback;
};

DEFINE_INTERFACE_FACTORY(XColorSwatch)

//---------------------------------------------------------------
//
// XColorSwatch::~XColorSwatch
//
//---------------------------------------------------------------
XColorSwatch::~XColorSwatch()
{
}


//---------------------------------------------------------------
//
// XColorSwatch::XColorSwatch
//
//---------------------------------------------------------------
XColorSwatch::XColorSwatch(XBoss* boss) 
{	
	IInstallMenuHandler::DoSetBoss(boss);
	IContextMenuHelper::DoSetBoss(boss);
	IDrawHelper::DoSetBoss(boss);
	IMouseDownHelper::DoSetBoss(boss);
	IColorSwatch::DoSetBoss(boss);
	
	mColorSet = kCrayolaColors;
}


//---------------------------------------------------------------
//
// XColorSwatch::SetColor
//
//---------------------------------------------------------------
void XColorSwatch::SetColor(const XRGBColor& color, bool redraw)
{
	if (color != mColor) {
		mColor = color;
		
		if (redraw) {
			IColorSwatch* thisPtr = this;
			IControlPtr control(thisPtr);
			control->Invalidate();
		}
	}
}


//---------------------------------------------------------------
//
// XColorSwatch::SetColorSet
//
//---------------------------------------------------------------
void XColorSwatch::SetColorSet(const XColorSet& colors)				
{
	mColorSet = colors;

	IColorSwatch* thisPtr = this;
	IControlPtr control(thisPtr);
	if (control->IsActive()) {
		this->RemoveCommands();
		this->InstallCommands();
	}
}


//---------------------------------------------------------------
//
// XColorSwatch::InstallCommands
//
//---------------------------------------------------------------
void XColorSwatch::InstallCommands()
{
	IMenuHandlerPtr handler(L"Application");
		
	XCallback1<void, XRGBColor> actionCallback(this, &XColorSwatch::DoUserSetColor);
	XCallback1<SCommandStatus, XRGBColor> enableCallback(this, &XColorSwatch::DoEnableColor);

	uint32 count = mColorSet.size();
	for (uint32 i = 0; i < count; ++i) {
		XCallback0<void> action = Bind1(actionCallback, mColorSet[i].color);
		XCallback0<SCommandStatus> enabler = Bind1(enableCallback, mColorSet[i].color);
	
		IColorSwatch* thisPtr = this;
		handler->RegisterCommand(L"set color: " + UInt32ToStr(i), action, enabler, thisPtr);
	}
}


//---------------------------------------------------------------
//
// XColorSwatch::RemoveCommands
//
//---------------------------------------------------------------
void XColorSwatch::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");
	
	XCallback1<void, XRGBColor> actionCallback(this, &XColorSwatch::DoUserSetColor);

	uint32 count = mColorSet.size();
	for (uint32 i = 0; i < count; ++i) {
		XCallback0<void> action = Bind1(actionCallback, mColorSet[i].color);
		handler->UnRegisterCommand(action);
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XColorSwatch::OnDraw
//
//---------------------------------------------------------------
void XColorSwatch::OnDraw(XDrawContext& context, const XRegion& dirtyRgn, bool isActive)
{
	UNUSED(dirtyRgn);
	
	IColorSwatch* thisPtr = this;
	IGeometryPtr geometry(thisPtr);
	IControlPtr control(thisPtr);
	XRect extent = geometry->GetExtent();

#if MAC
	// Draw the swatch (we do this first because it's not clear how
	// much we have to inset by).
	XRect content = extent;
	content.Inset(1, 1);
	
	XRGBColor color = mColor;
	if (!isActive && color != kRGBWhite) {
		XHSVColor hsv(color);
		hsv.saturation = .4f;			// $$ the Finder's preference window does something like this...	
		hsv.value      = .7f;
		color = XRGBColor(hsv);
	}	
	XRectShape::Fill(context, content, color);	
	
	// Draw the well.
	IControlPtr root = GetParent<IRootControl>(thisPtr);
	XControlContext rootContext(root);
	rootContext.MakeCurrent();

	Rect temp = control->LocalToWindow(extent).GetOSRect();
	OSStatus err = ::DrawThemeGenericWell(&temp, isActive ? kThemeStateActive : kThemeStateInactive, false);
	ASSERT(err == noErr);				// don't want to throw from a draw method
		
#else
	UNUSED(isActive);

	XRectShape::Frame(context, extent, kRGBBlack);	
	extent.Inset(1, 1);
	
	XRectShape::Frame(context, extent, kRGBWhite);
	extent.Inset(1, 1);

	XRectShape::Fill(context, extent, mColor);	
#endif
}


//---------------------------------------------------------------
//
// XColorSwatch::OnContextMenu
//
//---------------------------------------------------------------
bool XColorSwatch::OnContextMenu(const XMouseDownEvent& event)
{
	uint32 count = mColorSet.size();
	if (count > 0) {
		IColorSwatch* thisPtr = this;

		IContextMenuPtr popup(L"Context Menu");
		popup->Init(event.GetGlobalPosition());
	
		IMenuPtr menu(popup);
		menu->Init(L"Context Menu");
		
		for (uint32 i = 0; i < count; ++i) {
			const SNamedColor& color = mColorSet[i];
		
			menu->AppendItem(LoadWhisperString(color.name), L"set color: " + UInt32ToStr(i));
		}
	
		popup->Execute();
	}
				
	return kHandled;
}


//---------------------------------------------------------------
//
// XColorSwatch::OnMouseDown
//
//---------------------------------------------------------------
bool XColorSwatch::OnMouseDown(const XMouseDownEvent& event)
{
	UNUSED(event);
	
	XRGBColor color = mColor;
	XCallback1<void, XRGBColor> callback(this, &XColorSwatch::DoUserSetColor);
	
	if (XRGBColor::Pick(mPrompt, &color, callback))
		this->DoUserSetColor(color);		// need to do this on OS's that don't support callbacks
	else
		this->DoUserSetColor(color);		// need to do this on OS's that support callbacks
	
	return kHandled;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XColorSwatch::DoUserSetColor
//
//---------------------------------------------------------------
void XColorSwatch::DoUserSetColor(const XRGBColor& color)
{
	if (color != mColor) {
		this->SetColor(color);
		
		if (mChangedCallback.IsValid())			
			mChangedCallback(mColor);
	}
}


//---------------------------------------------------------------
//
// XColorSwatch::DoEnableColor
//
//---------------------------------------------------------------
SCommandStatus XColorSwatch::DoEnableColor(const XRGBColor& color)
{
	SCommandStatus status;
	
	status.enabled = true;
	status.checkmark = color == mColor ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;

	return status;
}


}	// namespace Whisper
