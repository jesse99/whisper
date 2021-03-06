/*
 *  File:       XKeyHandler.cpp
 *  Summary:   	Implementation for objects that respond to keystrokes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XKeyHandler.cpp,v $
 *		Revision 1.6  2001/04/27 04:33:52  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.5  2001/01/30 07:36:54  jesjones
 *		Removed a bogus include.
 *		
 *		Revision 1.4  2001/01/30 07:09:42  jesjones
 *		Key events are now handled via callbacks.
 *		
 *		Revision 1.3  2000/12/19 07:57:45  jesjones
 *		Uses typedefs for the interfaces.
 *		
 *		Revision 1.2  2000/11/09 12:40:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IKeyHandler.h>

#include <IHierarchy.h>
#include <IKeyExtensible.h>
#include <XConstants.h>
#include <XKeyEvents.h>

namespace Whisper {


// ===================================================================================
//	class XKeyHandler
//!		Implementation for objects that respond to keystrokes.
// ===================================================================================
class XKeyHandler : public IKeyHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XKeyHandler();
	
						XKeyHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:													
	virtual bool 		HandleKeyDown(const XKeyDownEvent& event);
	virtual bool 		HandleChar(const XCharEvent& event);
	
	virtual void 		RegisterHandler(int16 keyCode, SingleKeyHandler handler, const XUnknownPtr& object);
	virtual void 		RegisterHandler(KeyHandler handler1, CharHandler handler2, const XUnknownPtr& object);

	virtual void 		UnRegisterHandler(SingleKeyHandler handler);
	virtual void 		UnRegisterHandler(KeyHandler handler);
									
	virtual const XKeyDownEvent& GetEvent() const					{PRECONDITION(mEvent != nil); return *mEvent;}

//-----------------------------------
//	Internal Types
//
private:
	struct SHandler {
		int16 				keyCode;
		
		SingleKeyHandler	handler;
		KeyHandler			keyHandler;
		CharHandler			charHandler;
		
		XUnknown*			object;
		
			bool 	operator==(const SingleKeyHandler& rhs) const	{return handler == rhs;}
			bool 	operator==(const KeyHandler& rhs) const			{return keyHandler == rhs;}

			bool 	operator==(const XKeyDownEvent& rhs) const		{return keyCode != 0 && keyCode == rhs.GetKey();}
			bool 	operator==(const XCharEvent& rhs) const			{return keyCode != 0 && keyCode == rhs.ComputeKey();}
	};
									
//-----------------------------------
//	New API
//
private:
	std::list<SHandler>		mHandlers;
	const XKeyDownEvent*	mEvent;
};

DEFINE_INTERFACE_FACTORY(XKeyHandler)

//---------------------------------------------------------------
//
// XKeyHandler::~XKeyHandler
//
//---------------------------------------------------------------
XKeyHandler::~XKeyHandler()
{
}


//---------------------------------------------------------------
//
// XKeyHandler::XKeyHandler
//
//---------------------------------------------------------------
XKeyHandler::XKeyHandler(XBoss* boss)
{
	IKeyHandler::DoSetBoss(boss);

	mEvent = nil;
}


//---------------------------------------------------------------
//
// XKeyHandler::HandleKeyDown
//
//---------------------------------------------------------------
bool XKeyHandler::HandleKeyDown(const XKeyDownEvent& inEvent)
{
	bool handled = false;
	
	XKeyDownEvent event = inEvent;
	
	std::list<SHandler>::reverse_iterator iter = mHandlers.rbegin();
	while (iter != mHandlers.rend() && !handled) {
		const SHandler& handler = *iter;
		++iter;
		
		if (handler.keyHandler.IsValid()) {
			IKeyExtensiblePtr extensible(handler.object);
			if (extensible)
				handled = extensible->HandleKeyDown(event);
			
			if (!handled)
				handled = handler.keyHandler(event);
		
		} else if (handler.handler.IsValid() && handler == event) {
			IKeyExtensiblePtr extensible(handler.object);
			if (extensible)
				handled = extensible->HandleKeyDown(event);
			
			if (!handled) {
				mEvent = &event;
				handler.handler();
				mEvent = nil;
				handled = true;
			}
		}
	}
	
	return handled;
}


//---------------------------------------------------------------
//
// XKeyHandler::HandleChar
//
//---------------------------------------------------------------
bool XKeyHandler::HandleChar(const XCharEvent& inEvent)
{
	bool handled = false;
	
	XCharEvent event = inEvent;
	
	std::list<SHandler>::reverse_iterator iter = mHandlers.rbegin();
	while (iter != mHandlers.rend() && !handled) {
		const SHandler& handler = *iter;
		++iter;
		
		if (handler.charHandler.IsValid()) {
			IKeyExtensiblePtr extensible(handler.object);
			if (extensible)
				handled = extensible->HandleChar(event);
			
			if (!handled)
				handled = handler.charHandler(event);
		
		} else if (handler.handler.IsValid() && handler == event)
			handled = true;				// the key has already been handled so we'll eat this event
	}
	
	return handled;
}


//---------------------------------------------------------------
//
// XKeyHandler::RegisterHandler (int16, SingleKeyHandler, XUnknownPtr)
//
//---------------------------------------------------------------
void XKeyHandler::RegisterHandler(int16 keyCode, SingleKeyHandler handler0, const XUnknownPtr& object)
{
	PRECONDITION(keyCode != 0);
	PRECONDITION(handler0.IsValid());

	SHandler handler;
	handler.keyCode  = keyCode;
	handler.handler  = handler0;
	handler.object   = object.Get();
	
	mHandlers.push_back(handler);
}


//---------------------------------------------------------------
//
// XKeyHandler::RegisterHandler (KeyHandler, CharHandler, XUnknownPtr)
//
//---------------------------------------------------------------
void XKeyHandler::RegisterHandler(KeyHandler handler1, CharHandler handler2, const XUnknownPtr& object)
{
	PRECONDITION(handler1.IsValid());
	PRECONDITION(handler2.IsValid());

	SHandler handler;
	handler.keyCode     = 0;
	handler.keyHandler  = handler1;
	handler.charHandler = handler2;
	handler.object      = object.Get();
	
	mHandlers.push_back(handler);
}


//---------------------------------------------------------------
//
// XKeyHandler::UnRegisterHandler (SingleKeyHandler)
//
//---------------------------------------------------------------
void XKeyHandler::UnRegisterHandler(SingleKeyHandler handler)
{
	PRECONDITION(handler.IsValid());
	
	std::list<SHandler>::iterator iter = std::find(mHandlers.begin(), mHandlers.end(), handler);
	PRECONDITION(iter != mHandlers.end());
	
	mHandlers.erase(iter);
}


//---------------------------------------------------------------
//
// XKeyHandler::UnRegisterHandler (KeyHandler)
//
//---------------------------------------------------------------
void XKeyHandler::UnRegisterHandler(KeyHandler handler)
{
	PRECONDITION(handler.IsValid());
	
	std::list<SHandler>::iterator iter = std::find(mHandlers.begin(), mHandlers.end(), handler);
	PRECONDITION(iter != mHandlers.end());
	
	mHandlers.erase(iter);
}


}	// namespace Whisper
