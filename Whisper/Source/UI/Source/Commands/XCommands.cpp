/*
 *  File:       XCommands.cpp
 *  Summary:   	Some handy command classes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCommands.cpp,v $
 *		Revision 1.7  2001/04/27 04:31:14  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.6  2001/04/04 04:53:38  jesjones
 *		Updated IDeferredMenuCommand so that it uses the app's command handler.
 *		
 *		Revision 1.5  2001/01/30 07:24:13  jesjones
 *		Added ICommandHelper and IUndoActionHelper.
 *		
 *		Revision 1.4  2001/01/21 00:43:19  jesjones
 *		Renamed an IMenuHandler method.
 *		
 *		Revision 1.3  2000/12/14 08:33:25  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:28:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 6/03/99	JDJ		Added XCallbackCommand. 
 *		 <3>	 6/03/99	JDJ		XDeferredErrorCommand uses GetErrorText to convert the
 *									exception into a string. XDeferredErrorCommand takes
 *									an exception pointer instead of a reference.
 *		 <2>	 3/12/99	JDJ		Added XDeferredErrorCommand.
 *		 <1>	10/10/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <ICommands.h>

#include <ICommandHelper.h>
#include <IMenuHandler.h>
#include <IUndoAction.h>
#include <XError.h>
#include <XEvent.h>
#include <XExceptions.h>
#include <XInterfaceMacros.h>
#include <XMiscUtils.h>

namespace Whisper {


// ===================================================================================
//	class XDeferredMenuCommand
//!		Sends a menu command to the installed menu handlers.
// ===================================================================================
class XDeferredMenuCommand : public ICommandHelper, public IDeferredMenuCommand {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDeferredMenuCommand();
	
						XDeferredMenuCommand(XBoss* boss);
						
	virtual void 		Init(const XMenuCommand& command);

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnExecute();
			
//-----------------------------------
//	Member Data
//
private:
	XMenuCommand 	mCommand;
};

DEFINE_INTERFACE_FACTORY(XDeferredMenuCommand)

//---------------------------------------------------------------
//
// XDeferredMenuCommand::~XDeferredMenuCommand
//
//---------------------------------------------------------------
XDeferredMenuCommand::~XDeferredMenuCommand()
{
}


//---------------------------------------------------------------
//
// XDeferredMenuCommand::XDeferredMenuCommand
//
//---------------------------------------------------------------
XDeferredMenuCommand::XDeferredMenuCommand(XBoss* boss)
{
	ICommandHelper::DoSetBoss(boss);
	IDeferredMenuCommand::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDeferredMenuCommand::Init
//
//---------------------------------------------------------------
void XDeferredMenuCommand::Init(const XMenuCommand& command)
{
	PRECONDITION(command != kNothingCmd);

	mCommand = command;
}


//---------------------------------------------------------------
//
// XDeferredMenuCommand::OnExecute
//
//---------------------------------------------------------------
void XDeferredMenuCommand::OnExecute()
{
	XEvent event;
	
	IMenuHandlerPtr handler(L"Application");
	handler->HandleCommand(mCommand, event);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XDeferredErrorCommand
//!		Calls ReportError.
// ===================================================================================
class XDeferredErrorCommand : public ICommandHelper, public IDeferredErrorCommand {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDeferredErrorCommand();
	
						XDeferredErrorCommand(XBoss* boss);
						
	virtual void 		Init(const std::wstring& summary, const std::exception* e);

	virtual void 		Init(const std::wstring& summary, const std::wstring& narrative);

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnExecute();
			
//-----------------------------------
//	Member Data
//
private:
	std::wstring	mSummary;
	std::wstring	mNarrative;
};

DEFINE_INTERFACE_FACTORY(XDeferredErrorCommand)

//---------------------------------------------------------------
//
// XDeferredErrorCommand::~XDeferredErrorCommand
//
//---------------------------------------------------------------
XDeferredErrorCommand::~XDeferredErrorCommand()
{
}


//---------------------------------------------------------------
//
// XDeferredErrorCommand::XDeferredErrorCommand
//
//---------------------------------------------------------------
XDeferredErrorCommand::XDeferredErrorCommand(XBoss* boss) 
{
	ICommandHelper::DoSetBoss(boss);
	IDeferredErrorCommand::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDeferredErrorCommand::Init (wstring, const std::exception*)
//
//---------------------------------------------------------------
void XDeferredErrorCommand::Init(const std::wstring& summary, const std::exception* e)
{
	mSummary = summary;
	mNarrative = XError::Instance()->GetText(e);
}


//---------------------------------------------------------------
//
// XDeferredErrorCommand::Init (wstring, wstring)
//
//---------------------------------------------------------------
void XDeferredErrorCommand::Init(const std::wstring& summary, const std::wstring& narrative)
{
	mSummary = summary;
	mNarrative = narrative;
}


//---------------------------------------------------------------
//
// XDeferredErrorCommand::OnExecute
//
//---------------------------------------------------------------
void XDeferredErrorCommand::OnExecute()
{
	XError::Instance()->ReportError(mSummary, mNarrative);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XCallbackCommand
//!		Calls an arbitrary function with a preset list of arguments.
// ===================================================================================
class XCallbackCommand : public ICommandHelper, public ICallbackCommand {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XCallbackCommand();
	
						XCallbackCommand(XBoss* boss);
						
	virtual void 		Init(const XCallback0<void>& callback);

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnExecute();
			
//-----------------------------------
//	Member Data
//
private:
	XCallback0<void>	mCallback;
};

DEFINE_INTERFACE_FACTORY(XCallbackCommand)

//---------------------------------------------------------------
//
// XCallbackCommand::~XCallbackCommand
//
//---------------------------------------------------------------
XCallbackCommand::~XCallbackCommand()
{
}


//---------------------------------------------------------------
//
// XCallbackCommand::XCallbackCommand 
//
//---------------------------------------------------------------
XCallbackCommand::XCallbackCommand(XBoss* boss)
{
	ICommandHelper::DoSetBoss(boss);
	ICallbackCommand::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XCallbackCommand::Init 
//
//---------------------------------------------------------------
void XCallbackCommand::Init(const XCallback0<void>& callback)
{
	mCallback = callback;
}


//---------------------------------------------------------------
//
// XCallbackCommand::OnExecute
//
//---------------------------------------------------------------
void XCallbackCommand::OnExecute()
{
	mCallback();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XUndoCommand
// ===================================================================================
class XUndoCommand : public ICommandHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XUndoCommand();
	
						XUndoCommand(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
private:
	virtual void 		OnExecute();
};

DEFINE_INTERFACE_FACTORY(XUndoCommand)

//---------------------------------------------------------------
//
// XUndoCommand::~XUndoCommand
//
//---------------------------------------------------------------
XUndoCommand::~XUndoCommand()
{
}


//---------------------------------------------------------------
//
// XUndoCommand::XUndoCommand 
//
//---------------------------------------------------------------
XUndoCommand::XUndoCommand(XBoss* boss)
{
	ICommandHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XUndoCommand::OnExecute
//
//---------------------------------------------------------------
void XUndoCommand::OnExecute()
{
	IUndoActionPtr action(this);
	action->HandleDo();
}


}	// namespace Whisper
