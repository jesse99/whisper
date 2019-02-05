/*
 *  File:       XScrollableKeyHandler.cpp
 *  Summary:   	Key handler that handles keystrokes directed at scrollable controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XScrollableKeyHandler.cpp,v $
 *		Revision 1.4  2001/01/30 07:09:46  jesjones
 *		Key events are now handled via callbacks.
 *		
 *		Revision 1.3  2000/12/14 08:37:29  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:50:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IFocused.h>

#include <IKeyHandler.h>
#include <IScrollableControl.h>
#include <XKeyEvents.h>

namespace Whisper {


// ===================================================================================
//	class XScrollableKeyHandler
//!		Key handler that handles keystrokes directed at scrollable controls.
// ===================================================================================
class XScrollableKeyHandler : public IFocused {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XScrollableKeyHandler();
	
						XScrollableKeyHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnAcquiredFocus();
	virtual void 		OnLostFocus();

			void 		DoHome();
			void 		DoEnd();
			void 		DoPageUp();
			void 		DoPageDown();

//-----------------------------------
//	Internal API
//
private:			
#if MAC
			void 		DoLeftArrow();
			void 		DoRightArrow();
			void 		DoUpArrow();
			void 		DoDownArrow();
#endif
};

DEFINE_INTERFACE_FACTORY(XScrollableKeyHandler)

//---------------------------------------------------------------
//
// XScrollableKeyHandler::~XScrollableKeyHandler
//
//---------------------------------------------------------------
XScrollableKeyHandler::~XScrollableKeyHandler()
{
}


//---------------------------------------------------------------
//
// XScrollableKeyHandler::XScrollableKeyHandler
//
//---------------------------------------------------------------
XScrollableKeyHandler::XScrollableKeyHandler(XBoss* boss) 
{	
	IFocused::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif
			
//---------------------------------------------------------------
//
// XScrollableKeyHandler::OnAcquiredFocus
//
//---------------------------------------------------------------
void XScrollableKeyHandler::OnAcquiredFocus()
{
	IKeyHandlerPtr handler(L"Application");
		
	XCallback0<void> handle(this, &XScrollableKeyHandler::DoHome);
	handler->RegisterHandler(kHomeKey, handle, this);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoEnd);
	handler->RegisterHandler(kEndKey, handle, this);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoPageUp);
	handler->RegisterHandler(kPageUpKey, handle, this);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoPageDown);
	handler->RegisterHandler(kPageDownKey, handle, this);
	
#if MAC
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoLeftArrow);
	handler->RegisterHandler(kLeftArrowKey, handle, this);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoRightArrow);
	handler->RegisterHandler(kRightArrowKey, handle, this);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoUpArrow);
	handler->RegisterHandler(kUpArrowKey, handle, this);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoDownArrow);
	handler->RegisterHandler(kDownArrowKey, handle, this);
#endif
}


//---------------------------------------------------------------
//
// XScrollableKeyHandler::OnLostFocus
//
//---------------------------------------------------------------
void XScrollableKeyHandler::OnLostFocus()
{
	IKeyHandlerPtr handler(L"Application");
		
	XCallback0<void> handle(this, &XScrollableKeyHandler::DoHome);
	handler->UnRegisterHandler(handle);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoEnd);
	handler->UnRegisterHandler(handle);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoPageUp);
	handler->UnRegisterHandler(handle);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoPageDown);
	handler->UnRegisterHandler(handle);
	
#if MAC
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoLeftArrow);
	handler->UnRegisterHandler(handle);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoRightArrow);
	handler->UnRegisterHandler(handle);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoUpArrow);
	handler->UnRegisterHandler(handle);
	
	handle = XCallback0<void>(this, &XScrollableKeyHandler::DoDownArrow);
	handler->UnRegisterHandler(handle);
#endif
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XScrollableKeyHandler::DoHome
//
//---------------------------------------------------------------
void XScrollableKeyHandler::DoHome()
{
	IScrollableControlPtr scrollable(this);
	
	XRect revealed = scrollable->GetRevealedRect();
	XPoint oldLoc = revealed[topLeft];
	XPoint newLoc = kZeroPt;
		
	if (newLoc != oldLoc)
		scrollable->ScrollTo(newLoc);
}


//---------------------------------------------------------------
//
// XScrollableKeyHandler::DoEnd
//
//---------------------------------------------------------------
void XScrollableKeyHandler::DoEnd()
{
	IScrollableControlPtr scrollable(this);
	
	XRect revealed  = scrollable->GetRevealedRect();
	XSize imageSize = scrollable->GetImageSize();

	XPoint oldLoc = revealed[topLeft];
	XPoint newLoc = XPoint(0, imageSize.height - 1);
		
	if (newLoc != oldLoc)
		scrollable->ScrollTo(newLoc);
}


//---------------------------------------------------------------
//
// XScrollableKeyHandler::DoPageUp
//
//---------------------------------------------------------------
void XScrollableKeyHandler::DoPageUp()
{
	IScrollableControlPtr scrollable(this);
	
	XRect revealed  = scrollable->GetRevealedRect();
	XSize imageSize = scrollable->GetImageSize();
	XSize size = revealed.GetSize();

	XPoint oldLoc = revealed[topLeft];
	XPoint newLoc = oldLoc;
	newLoc.y = Max(oldLoc.y - size.height, 0L);
		
	if (newLoc != oldLoc)
		scrollable->ScrollTo(newLoc);
}


//---------------------------------------------------------------
//
// XScrollableKeyHandler::DoPageDown
//
//---------------------------------------------------------------
void XScrollableKeyHandler::DoPageDown()
{
	IScrollableControlPtr scrollable(this);
	
	XRect revealed  = scrollable->GetRevealedRect();
	XSize imageSize = scrollable->GetImageSize();
	XSize size = revealed.GetSize();

	XPoint oldLoc = revealed[topLeft];
	XPoint newLoc = oldLoc;
	newLoc.y = Min(oldLoc.y + size.height, imageSize.height - 1);
		
	if (newLoc != oldLoc)
		scrollable->ScrollTo(newLoc);
}


//---------------------------------------------------------------
//
// XScrollableKeyHandler::DoLeftArrow
//
//---------------------------------------------------------------
#if MAC
void XScrollableKeyHandler::DoLeftArrow()
{
	IKeyHandlerPtr handler(L"Application");
	const XKeyDownEvent& event = handler->GetEvent();
	if ((event.GetOSEvent().modifiers & cmdKey) != 0) {
		IScrollableControlPtr scrollable(this);
		
		XRect revealed = scrollable->GetRevealedRect();

		XPoint oldLoc = revealed[topLeft];
		XPoint newLoc = oldLoc;
		newLoc.x = 0;
			
		if (newLoc != oldLoc)
			scrollable->ScrollTo(newLoc);
	}
}
#endif


//---------------------------------------------------------------
//
// XScrollableKeyHandler::DoRightArrow
//
//---------------------------------------------------------------
#if MAC
void XScrollableKeyHandler::DoRightArrow()
{
	IKeyHandlerPtr handler(L"Application");
	const XKeyDownEvent& event = handler->GetEvent();
	if ((event.GetOSEvent().modifiers & cmdKey) != 0) {
		IScrollableControlPtr scrollable(this);
		
		XRect revealed  = scrollable->GetRevealedRect();
		XSize imageSize = scrollable->GetImageSize();

		XPoint oldLoc = revealed[topLeft];
		XPoint newLoc = oldLoc;
		newLoc.x = imageSize.width - 1;
			
		if (newLoc != oldLoc)
			scrollable->ScrollTo(newLoc);
	}
}
#endif


//---------------------------------------------------------------
//
// XScrollableKeyHandler::DoUpArrow
//
//---------------------------------------------------------------
#if MAC
void XScrollableKeyHandler::DoUpArrow()
{
	IKeyHandlerPtr handler(L"Application");
	const XKeyDownEvent& event = handler->GetEvent();
	if ((event.GetOSEvent().modifiers & cmdKey) != 0) {
		IScrollableControlPtr scrollable(this);
		
		XRect revealed = scrollable->GetRevealedRect();

		XPoint oldLoc = revealed[topLeft];
		XPoint newLoc = oldLoc;
		newLoc.y = 0;
			
		if (newLoc != oldLoc)
			scrollable->ScrollTo(newLoc);
	}
}
#endif


//---------------------------------------------------------------
//
// XScrollableKeyHandler::DoDownArrow
//
//---------------------------------------------------------------
#if MAC
void XScrollableKeyHandler::DoDownArrow()
{
	IKeyHandlerPtr handler(L"Application");
	const XKeyDownEvent& event = handler->GetEvent();
	if ((event.GetOSEvent().modifiers & cmdKey) != 0) {
		IScrollableControlPtr scrollable(this);
		
		XRect revealed  = scrollable->GetRevealedRect();
		XSize imageSize = scrollable->GetImageSize();

		XPoint oldLoc = revealed[topLeft];
		XPoint newLoc = oldLoc;
		newLoc.y = imageSize.height - 1;
			
		if (newLoc != oldLoc)
			scrollable->ScrollTo(newLoc);
	}
}
#endif


}	// namespace Whisper
