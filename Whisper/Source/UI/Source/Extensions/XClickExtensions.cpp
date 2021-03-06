/*
 *  File:       XClickExtensions.cpp
 *	Function:	Classes for objects that affect the way their owner responds to clicks.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XClickExtensions.cpp,v $
 *		Revision 1.5  2001/04/27 04:30:59  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/01/21 00:42:34  jesjones
 *		Renamed an IMenuHandler method.
 *		
 *		Revision 1.3  2000/12/14 08:33:12  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:27:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IClickExtensions.h>

#include <IClickExtension.h>
#include <ICommand.h>
#include <ICommandQueue.h>
#include <IMenuHandler.h>
#include <XMouseEvents.h>

namespace Whisper {


// ===================================================================================
//	class XMenuCommandClickExtension
//!		Sends a XMenuCommand to a specified input target.
// ===================================================================================
class XMenuCommandClickExtension : public IClickExtension, public IMenuCommandClickExtension {
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMenuCommandClickExtension();
	
						XMenuCommandClickExtension(XBoss* boss);
						
//-----------------------------------
//	Inherited API 
//
public:
	virtual void 		Init(const IMenuHandlerPtr& target, const XMenuCommand& command, uint32 clickCount, const std::wstring& name);

	virtual std::wstring GetName() const;
	
	virtual void 		SetName(const std::wstring& name);
									
	virtual uint32 		GetClickCount() const;

	virtual void 		SetClickCount(uint32 clickCount);
	
	virtual void		HandleClick(const XMouseDownEvent& event);

//-----------------------------------
//	Member Data
//
private:
	IMenuHandlerPtr	mTarget;
	XMenuCommand	mCommand;
	
	std::wstring	mName;
	uint32			mClickCount;
};

DEFINE_INTERFACE_FACTORY(XMenuCommandClickExtension)

//---------------------------------------------------------------
//
// XMenuCommandClickExtension::~XMenuCommandClickExtension
//
//---------------------------------------------------------------
XMenuCommandClickExtension::~XMenuCommandClickExtension()
{
}


//---------------------------------------------------------------
//
// XMenuCommandClickExtension::XMenuCommandClickExtension
//
//---------------------------------------------------------------
XMenuCommandClickExtension::XMenuCommandClickExtension(XBoss* boss)
{	
	IClickExtension::DoSetBoss(boss);
	IMenuCommandClickExtension::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XMenuCommandClickExtension::Init
//
//---------------------------------------------------------------
void XMenuCommandClickExtension::Init(const IMenuHandlerPtr& target, const XMenuCommand& command, uint32 clickCount, const std::wstring& name)
{
	PRECONDITION(command.length() > 0);
	PRECONDITION(clickCount >= 0);
	
	mTarget = target;
	mCommand = command;
	mClickCount = clickCount;
	mName = name;
}


//---------------------------------------------------------------
//
// XMenuCommandClickExtension::GetName
//
//---------------------------------------------------------------
std::wstring XMenuCommandClickExtension::GetName() const
{
	return mName;
}

	
//---------------------------------------------------------------
//
// XMenuCommandClickExtension::SetName
//
//---------------------------------------------------------------
void XMenuCommandClickExtension::SetName(const std::wstring& name)
{
	mName = name;
}

									
//---------------------------------------------------------------
//
// XMenuCommandClickExtension::GetClickCount
//
//---------------------------------------------------------------
uint32 XMenuCommandClickExtension::GetClickCount() const
{
	return mClickCount;
}


//---------------------------------------------------------------
//
// XMenuCommandClickExtension::SetClickCount
//
//---------------------------------------------------------------
void XMenuCommandClickExtension::SetClickCount(uint32 clickCount)
{
	PRECONDITION(clickCount >= 0);
	
	mClickCount = clickCount;
}

	
//---------------------------------------------------------------
//
// XMenuCommandClickExtension::HandleClick
//
//---------------------------------------------------------------
void XMenuCommandClickExtension::HandleClick(const XMouseDownEvent& event)
{
	PRECONDITION(mClickCount == 0 || mClickCount == event.GetClickCount());
	
	(void) mTarget->HandleCommand(mCommand, event);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XCommandClickExtension
//!		Posts an ICommand.
// ===================================================================================
class XCommandClickExtension : public IClickExtension, public ICommandClickExtension {
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XCommandClickExtension();
	
						XCommandClickExtension(XBoss* boss);
						
//-----------------------------------
//	Inherited API 
//
public:
	virtual void 		Init(const ICommandPtr& command, uint32 clickCount, const std::wstring& name);

	virtual std::wstring GetName() const;
	
	virtual void 		SetName(const std::wstring& name);
									
	virtual uint32 		GetClickCount() const;

	virtual void 		SetClickCount(uint32 clickCount);
	
	virtual void		HandleClick(const XMouseDownEvent& event);

//-----------------------------------
//	Member Data
//
private:
	ICommandPtr		mCommand;
	
	std::wstring	mName;
	uint32			mClickCount;
};

DEFINE_INTERFACE_FACTORY(XCommandClickExtension)

//---------------------------------------------------------------
//
// XCommandClickExtension::~XCommandClickExtension
//
//---------------------------------------------------------------
XCommandClickExtension::~XCommandClickExtension()
{
}


//---------------------------------------------------------------
//
// XCommandClickExtension::XCommandClickExtension
//
//---------------------------------------------------------------
XCommandClickExtension::XCommandClickExtension(XBoss* boss)
{	
	IClickExtension::DoSetBoss(boss);
	ICommandClickExtension::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XCommandClickExtension::Init
//
//---------------------------------------------------------------
void XCommandClickExtension::Init(const ICommandPtr& command, uint32 clickCount, const std::wstring& name)
{
	PRECONDITION(clickCount >= 0);

	mCommand = command;
	mClickCount = clickCount;
	mName = name;
}


//---------------------------------------------------------------
//
// XCommandClickExtension::GetName
//
//---------------------------------------------------------------
std::wstring XCommandClickExtension::GetName() const
{
	return mName;
}

	
//---------------------------------------------------------------
//
// XCommandClickExtension::SetName
//
//---------------------------------------------------------------
void XCommandClickExtension::SetName(const std::wstring& name)
{
	mName = name;
}

									
//---------------------------------------------------------------
//
// XCommandClickExtension::GetClickCount
//
//---------------------------------------------------------------
uint32 XCommandClickExtension::GetClickCount() const
{
	return mClickCount;
}


//---------------------------------------------------------------
//
// XCommandClickExtension::SetClickCount
//
//---------------------------------------------------------------
void XCommandClickExtension::SetClickCount(uint32 clickCount)
{
	PRECONDITION(clickCount >= 0);
	
	mClickCount = clickCount;
}

	
//---------------------------------------------------------------
//
// XCommandClickExtension::HandleClick
//
//---------------------------------------------------------------
void XCommandClickExtension::HandleClick(const XMouseDownEvent& event)
{
	PRECONDITION(mClickCount == 0 || mClickCount == event.GetClickCount());
	
	XInterfacePtr<ICommandQueue> queue(L"Application");
	queue->Post(mCommand);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XCallbackClickExtension
//!		Executes an XCallback object.
// ===================================================================================
class XCallbackClickExtension : public IClickExtension, public ICallbackClickExtension {
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XCallbackClickExtension();
	
						XCallbackClickExtension(XBoss* boss);
						
//-----------------------------------
//	Inherited API 
//
public:
	virtual void 		Init(const XCallback0<void>& callback, uint32 clickCount, const std::wstring& name);

	virtual std::wstring GetName() const;
	
	virtual void 		SetName(const std::wstring& name);
									
	virtual uint32 		GetClickCount() const;

	virtual void 		SetClickCount(uint32 clickCount);
	
	virtual void		HandleClick(const XMouseDownEvent& event);

//-----------------------------------
//	Member Data
//
private:
	XCallback0<void>	mCallback;
	
	std::wstring		mName;
	uint32				mClickCount;
};

DEFINE_INTERFACE_FACTORY(XCallbackClickExtension)

//---------------------------------------------------------------
//
// XCallbackClickExtension::~XCallbackClickExtension
//
//---------------------------------------------------------------
XCallbackClickExtension::~XCallbackClickExtension()
{
}


//---------------------------------------------------------------
//
// XCallbackClickExtension::XCallbackClickExtension
//
//---------------------------------------------------------------
XCallbackClickExtension::XCallbackClickExtension(XBoss* boss)
{	
	IClickExtension::DoSetBoss(boss);
	ICallbackClickExtension::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XCallbackClickExtension::Init
//
//---------------------------------------------------------------
void XCallbackClickExtension::Init(const XCallback0<void>& callback, uint32 clickCount, const std::wstring& name)
{
	PRECONDITION(clickCount >= 0);

	mCallback = callback;
	mClickCount = clickCount;
	mName = name;
}


//---------------------------------------------------------------
//
// XCallbackClickExtension::GetName
//
//---------------------------------------------------------------
std::wstring XCallbackClickExtension::GetName() const
{
	return mName;
}

	
//---------------------------------------------------------------
//
// XCallbackClickExtension::SetName
//
//---------------------------------------------------------------
void XCallbackClickExtension::SetName(const std::wstring& name)
{
	mName = name;
}

									
//---------------------------------------------------------------
//
// XCallbackClickExtension::GetClickCount
//
//---------------------------------------------------------------
uint32 XCallbackClickExtension::GetClickCount() const
{
	return mClickCount;
}


//---------------------------------------------------------------
//
// XCallbackClickExtension::SetClickCount
//
//---------------------------------------------------------------
void XCallbackClickExtension::SetClickCount(uint32 clickCount)
{
	PRECONDITION(clickCount >= 0);
	
	mClickCount = clickCount;
}

	
//---------------------------------------------------------------
//
// XCallbackClickExtension::HandleClick
//
//---------------------------------------------------------------
void XCallbackClickExtension::HandleClick(const XMouseDownEvent& event)
{
	PRECONDITION(mClickCount == 0 || mClickCount == event.GetClickCount());
		
	mCallback();
}


}	// namespace Whisper

