/*
 *  File:       XMenuHandler.cpp
 *  Summary:   	IMenuHandler implementation that routes menu commands to registered callbacks.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMenuHandler.cpp,v $
 *		Revision 1.9  2001/04/27 04:34:29  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.8  2001/02/17 09:28:23  jesjones
 *		Removed UI_EXPORT.
 *		
 *		Revision 1.7  2001/02/12 09:50:25  jesjones
 *		Fixed a silly bug in the kEnabledIfDocWindow code.
 *		
 *		Revision 1.6  2001/02/09 04:32:21  jesjones
 *		Added kEnabledIfDocWindow.
 *		
 *		Revision 1.5  2001/01/30 07:08:14  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.4  2001/01/21 00:23:26  jesjones
 *		Menu handling now uses callbacks.
 *		
 *		Revision 1.3  2001/01/03 05:57:46  jesjones
 *		Uses typedefs instead of XInterfacePtr.
 *		
 *		Revision 1.2  2000/11/09 12:43:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IMenuHandler.h>

#include <IApplication.h>
#include <IDocView.h>
#include <IHierarchy.h>
#include "IMenuExtensible.h"
#include <IWindowMgr.h>
#include <XBind.h>
#include <XError.h>

namespace Whisper {


// ===================================================================================
//	class XMenuHandler
//!		IMenuHandler implementation that routes menu commands to registered callbacks.
// ===================================================================================
class XMenuHandler : public IMenuHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMenuHandler();
	
						XMenuHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:													
	virtual bool 		HandleCommand(const XMenuCommand& command, const XEvent& event);
	virtual bool 		HandleEnableCommand(const XMenuCommand& command, SCommandStatus& status, const XEvent& event);

	virtual void 		RegisterCommand(const XMenuCommand& command, Action action, uint32 enableFlags, const XUnknownPtr& object, bool prefix = false);
	virtual void 		RegisterCommand(const XMenuCommand& command, Action action, Enabler enabler, const XUnknownPtr& object, bool prefix = false);
	virtual void 		RegisterCommand(const XMenuCommand& command, Action action, SimpleEnabler enabler, const XUnknownPtr& object, bool prefix = false);

	virtual void 		UnRegisterCommand(Action action);
	
	virtual const XEvent& GetEvent() const					{PRECONDITION(mEvent != nil); return *mEvent;}
	virtual const XMenuCommand& GetCommand() const			{PRECONDITION(mCommand.length() > 0); return mCommand;}
										
//-----------------------------------
//	Internal Types
//
private:
	struct SHandler {
		XMenuCommand	command;
		Action			action;
		Enabler			enabler;
		XUnknown*		object;
		bool			prefix;
		
			bool 	operator==(const XMenuCommand& rhs) const	{return prefix ? rhs.find(command) == 0 : command == rhs;}
			bool 	operator==(const Action& rhs) const			{return action == rhs;}
	};
									
//-----------------------------------
//	Internal API
//
private:
			SCommandStatus DoEnable();
			SCommandStatus DoDisable();
			SCommandStatus DoDisableIf(uint32 flags);
			SCommandStatus DoSimpleEnable(SimpleEnabler enabler);
			
//-----------------------------------
//	New API
//
private:
	std::list<SHandler>	mHandlers;
	const XEvent*		mEvent;
	XMenuCommand		mCommand;
};

DEFINE_INTERFACE_FACTORY(XMenuHandler)

//---------------------------------------------------------------
//
// XMenuHandler::~XMenuHandler
//
//---------------------------------------------------------------
XMenuHandler::~XMenuHandler()
{
	PRECONDITION(mHandlers.empty());
}


//---------------------------------------------------------------
//
// XMenuHandler::XMenuHandler
//
//---------------------------------------------------------------
XMenuHandler::XMenuHandler(XBoss* boss)
{
	this->DoSetBoss(boss);
	
	mEvent = nil;
}


//---------------------------------------------------------------
//
// XMenuHandler::HandleCommand
//
//---------------------------------------------------------------
bool XMenuHandler::HandleCommand(const XMenuCommand& command, const XEvent& event)
{
	bool handled = false;
	
	mEvent = &event;
	mCommand = command;

	try {
		std::list<SHandler>::reverse_iterator iter = mHandlers.rbegin();
		while (iter != mHandlers.rend() && !handled) {
			const SHandler& handler = *iter;
			++iter;
			
			if (handler == command) {
				SCommandStatus status = handler.enabler();	// $$ bit icky to do this twice
				if (status.enabled) {
					IMenuExtensiblePtr extensible(handler.object);
					if (extensible)
						handled = extensible->HandleCommand(command, event);
					
					if (!handled) {
						handler.action();
						handled = true;
					}
				}
			}
		}
			
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't complete the operation."), e);
	
	} catch(...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't complete the operation."));
	}
	
	mEvent = nil;
	mCommand.clear();

	return handled;
}


//---------------------------------------------------------------
//
// XMenuHandler::HandleEnableCommand
//
//---------------------------------------------------------------
bool XMenuHandler::HandleEnableCommand(const XMenuCommand& command, SCommandStatus& status, const XEvent& event)
{
	bool handled = false;
	
	mEvent = &event;
	mCommand = command;

	try {
		std::list<SHandler>::reverse_iterator iter = mHandlers.rbegin();
		while (iter != mHandlers.rend() && !handled) {
			const SHandler& handler = *iter;
			++iter;
			
			if (handler == command) {
				IMenuExtensiblePtr extensible(handler.object);
				if (extensible)
					handled = extensible->HandleEnableCommand(command, status, event);
				
				if (!handled) {
					status = handler.enabler();
					handled = status.enabled;			// keep going until we find a handler that works
				}
			}
		}
			
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't complete the operation."), e);
	
	} catch(...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't complete the operation."));
	}
	
	mEvent = nil;
	mCommand.clear();

	return handled;
}


//---------------------------------------------------------------
//
// XMenuHandler::RegisterCommand (XMenuCommand, Action, uint32, XUnknownPtr, bool)
//
//---------------------------------------------------------------
void XMenuHandler::RegisterCommand(const XMenuCommand& command, Action action, uint32 enableFlags, const XUnknownPtr& object, bool prefix)
{
	PRECONDITION(command.length() > 0);
	PRECONDITION(action.IsValid());

	SHandler handler;
	handler.command = command;
	handler.action  = action;
	handler.object  = object.Get();
	handler.prefix  = prefix;
	
	if (enableFlags == kAlwaysEnabled)
		handler.enabler = XCallback0<SCommandStatus>(this, &XMenuHandler::DoEnable);
		
	else if (enableFlags == kNeverEnabled)
		handler.enabler = XCallback0<SCommandStatus>(this, &XMenuHandler::DoDisable);
		
	else {
		XCallback1<SCommandStatus, uint32> callback(this, &XMenuHandler::DoDisableIf);
		handler.enabler = Bind1(callback, enableFlags);
	}
	
	mHandlers.push_back(handler);
}


//---------------------------------------------------------------
//
// XMenuHandler::RegisterCommand (XMenuCommand, Action, SimpleEnabler, XUnknownPtr, bool)
//
//---------------------------------------------------------------
void XMenuHandler::RegisterCommand(const XMenuCommand& command, Action action, SimpleEnabler enabler, const XUnknownPtr& object, bool prefix)
{
	PRECONDITION(command.length() > 0);
	PRECONDITION(action.IsValid());
	PRECONDITION(enabler.IsValid());

	SHandler handler;
	handler.command = command;
	handler.action  = action;
	handler.object  = object.Get();
	handler.prefix  = prefix;
	
	XCallback1<SCommandStatus, SimpleEnabler> enable(this, &XMenuHandler::DoSimpleEnable);
	handler.enabler = Bind1(enable, enabler);
	
	mHandlers.push_back(handler);
}


//---------------------------------------------------------------
//
// XMenuHandler::RegisterCommand (XMenuCommand, Action, Enabler, XUnknownPtr, bool)
//
//---------------------------------------------------------------
void XMenuHandler::RegisterCommand(const XMenuCommand& command, Action action, Enabler enabler, const XUnknownPtr& object, bool prefix)
{
	PRECONDITION(command.length() > 0);
	PRECONDITION(action.IsValid());
	PRECONDITION(enabler.IsValid());

	SHandler handler;
	handler.command = command;
	handler.action  = action;
	handler.enabler = enabler;
	handler.object  = object.Get();
	handler.prefix  = prefix;
	
	mHandlers.push_back(handler);
}


//---------------------------------------------------------------
//
// XMenuHandler::UnRegisterCommand
//
//---------------------------------------------------------------
void XMenuHandler::UnRegisterCommand(Action action)
{
	PRECONDITION(action.IsValid());
	
	std::list<SHandler>::iterator iter = std::find(mHandlers.begin(), mHandlers.end(), action);
	PRECONDITION(iter != mHandlers.end());
	
	mHandlers.erase(iter);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XMenuHandler::DoEnable	
//
//---------------------------------------------------------------
SCommandStatus XMenuHandler::DoEnable()
{
	SCommandStatus status;
	status.enabled = true;
	
	return status;
}


//---------------------------------------------------------------
//
// XMenuHandler::DoDisable
//
//---------------------------------------------------------------
SCommandStatus XMenuHandler::DoDisable()
{
	SCommandStatus status;
	status.enabled = false;
	
	return status;
}


//---------------------------------------------------------------
//
// XMenuHandler::DoSimpleEnable
//
//---------------------------------------------------------------
SCommandStatus XMenuHandler::DoSimpleEnable(SimpleEnabler enabled)
{
	SCommandStatus status;
	status.enabled = enabled();
	
	return status;
}


//---------------------------------------------------------------
//
// XMenuHandler::DoDisableIf
//
//---------------------------------------------------------------
SCommandStatus XMenuHandler::DoDisableIf(uint32 flags)
{
	SCommandStatus status;
	
	status.enabled = true;
	
	if (flags & kDisabledIfLowMemory) {
		IApplicationPtr app(this);
		status.enabled = !app->IsMemoryLow();
	}
	
	if (status.enabled && (flags & kEnabledIfDocWindow)) {
		IWindowMgrPtr windowMgr(this);
		status.enabled = windowMgr->GetTopModal() == nil;		

		if (status.enabled) {
			IDocViewPtr doc = windowMgr->GetTopRegular();
			status.enabled = doc != nil;		
		}
	}
	
	if (status.enabled && (flags & kDisabledIfDialog)) {
		IWindowMgrPtr windowMgr(this);
		status.enabled = windowMgr->GetTopModal() == nil;		
	}
	
	return status;
}


}	// namespace Whisper
