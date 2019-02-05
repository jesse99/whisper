/*
 *  File:       XMouseHandler.cpp
 *  Summary:   	Standard IMouseHandler implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMouseHandler.cpp,v $
 *		Revision 1.9  2001/04/13 08:03:17  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.8  2001/03/17 12:34:05  jesjones
 *		HandleAdjustCursor sets the cursor to an arrow if no one else set it.
 *		
 *		Revision 1.7  2001/03/09 10:22:35  jesjones
 *		HandleHelp is now Mac only.
 *		
 *		Revision 1.6  2001/02/08 03:56:43  jesjones
 *		Context menus no longer take a menu handler.
 *		
 *		Revision 1.5  2001/02/04 03:39:13  jesjones
 *		Removed the implementation inheritance from the mouse handling classes.
 *		
 *		Revision 1.4  2000/12/31 04:49:18  jesjones
 *		Tweaked includes.
 *		
 *		Revision 1.3  2000/12/08 08:14:04  jesjones
 *		Added OnClicked to support lame Windows.
 *		
 *		Revision 1.2  2000/11/09 12:43:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IMouseHandler.h>

#include <IAdjustCursorHelper.h>
#include <IContextMenu.h>
#include <IContextMenuHelper.h>
#include <IHelpText.h>
#include <IMenuHandler.h>
#include <IMouseDownHelper.h>
#include <IMouseExtensible.h>
#include <IMouseUpHelper.h>
#include <IShowHelp.h>
#include <IWindow.h>
#include <XConstants.h>
#include <XCursor.h>
#include <XMouseEvents.h>

#if MAC
	#include <MSystemInfo.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XMouseHandler
//!		Standard IMouseHandler implementation.
// ===================================================================================
class XMouseHandler : public IMouseHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMouseHandler();
	
						XMouseHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:													
	virtual bool 		HandleMouseDown(const XMouseDownEvent& event);
	virtual bool 		HandleMouseUp(const XMouseUpEvent& event);
	virtual bool 		HandleContextMenu(const XMouseDownEvent& event);
		
	virtual bool 		HandleAdjustCursor(const XMouseMoveEvent& event);									
#if MAC
	virtual bool 		HandleHelp(const XMouseMoveEvent& event);
#endif

//-----------------------------------
//	New API
//
private:
			bool 		DoContextMenu(const XMouseDownEvent& event);
#if MAC
			bool 		DoHelp(const XMouseMoveEvent& event);
#endif
};

DEFINE_INTERFACE_FACTORY(XMouseHandler)

//---------------------------------------------------------------
//
// XMouseHandler::~XMouseHandler
//
//---------------------------------------------------------------
XMouseHandler::~XMouseHandler()
{
}


//---------------------------------------------------------------
//
// XMouseHandler::XMouseHandler
//
//---------------------------------------------------------------
XMouseHandler::XMouseHandler(XBoss* boss)
{
	IMouseHandler::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XMouseHandler::HandleMouseDown
//
//---------------------------------------------------------------
bool XMouseHandler::HandleMouseDown(const XMouseDownEvent& inEvent)
{		
	XMouseDownEvent event = inEvent;

	bool handled = false;
	IMouseExtensiblePtr extensible(this);
	if (extensible)
		handled = extensible->HandleMouseDown(event);
		
	if (!handled) {
		IMouseDownHelperPtr helper(this);
		if (helper)
			handled = helper->OnMouseDown(event);
	}
			
	return handled;
}


//---------------------------------------------------------------
//
// XMouseHandler::HandleMouseUp
//
//---------------------------------------------------------------
bool XMouseHandler::HandleMouseUp(const XMouseUpEvent& inEvent)
{		
	XMouseUpEvent event = inEvent;
	
	bool handled = false;
	IMouseExtensiblePtr extensible(this);
	if (extensible)
		handled = extensible->HandleMouseUp(event);
		
	if (!handled) {
		IMouseUpHelperPtr helper(this);
		if (helper)
			handled = helper->OnMouseUp(event);
	}
				
	return handled;
}

	
//---------------------------------------------------------------
//
// XMouseHandler::HandleContextMenu
//
//---------------------------------------------------------------
bool XMouseHandler::HandleContextMenu(const XMouseDownEvent& event)
{		
	bool handled = false;
	
	IContextMenuHelperPtr helper(this);
	if (helper) 
		handled = helper->OnContextMenu(event);
		
	if (!handled)
		handled = this->DoContextMenu(event);
		
	return handled;
}


//---------------------------------------------------------------
//
// XMouseHandler::HandleAdjustCursor
//
//---------------------------------------------------------------
bool XMouseHandler::HandleAdjustCursor(const XMouseMoveEvent& inEvent)
{		
	XMouseMoveEvent event = inEvent;
	
	bool handled = false;
	IMouseExtensiblePtr extensible(this);
	if (extensible)
		handled = extensible->HandleMouseMove(event);
		
	if (!handled) {
		IAdjustCursorHelperPtr helper(this);
		if (helper)
			handled = helper->OnAdjustCursor(event); 
	}
	
	if (!handled) {
		XCursorUtils::SetCursor(kArrowCursor);	// $$ better to ask our parent?
		handled = true;
	}
	
	return handled;
}


//---------------------------------------------------------------
//
// XMouseHandler::HandleHelp
//
//---------------------------------------------------------------
#if MAC
bool XMouseHandler::HandleHelp(const XMouseMoveEvent& event)
{			
	bool handled = this->DoHelp(event); 
	
	return handled;
}
#endif

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XMouseHandler::DoContextMenu
//
// Note that the context menu docs say that a context menu should
// always appear, even if all it has is a disabled Help command.
//
//---------------------------------------------------------------
bool XMouseHandler::DoContextMenu(const XMouseDownEvent& event)	
{
	UNUSED(event);
	
	bool handled = false;

#if MAC
	IWindowPtr window(this);
	if (window) {
		SWindowStyle style = window->GetStyle();

		if (MSystemInfo::HasContextMenus() || (style.attributes & kWindowCloseBoxAttribute)) {	
			IContextMenuPtr popup(L"Context Menu");
			popup->Init(event.GetGlobalPosition());
	
			IMenuPtr menu(popup);
			menu->Init(L"");
	
			if (style.attributes & kWindowCloseBoxAttribute)
				menu->AppendItem(LoadWhisperString(L"Close Menu Item"), kCloseCmd);
				
			popup->Execute();
			handled = true;
		}
	}
#endif

	return handled;
}


//---------------------------------------------------------------
//
// XMouseHandler::DoHelp
//
//---------------------------------------------------------------
#if MAC
bool XMouseHandler::DoHelp(const XMouseMoveEvent& event)
{
	bool handled = false;
	
	IHelpTextPtr help(this);
	if (help) {
		std::wstring message = help->GetMessage(event.GetPosition());
		if (message.length() > 0) {
			IShowHelpPtr display(this);
			if (display) {
				display->Show(message, event.GetGlobalPosition());
						
				handled = true;
			}
		}
	}
			
	return handled;
}
#endif


}	// namespace Whisper
