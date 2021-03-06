/*
 *  File:       WCursor.cpp
 *  Summary:    Mouse cursor classes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WCursor.cpp,v $
 *		Revision 1.5  2001/04/06 04:45:04  jesjones
 *		Fixed XCursor so that it works with custom cursors.
 *		
 *		Revision 1.4  2000/11/20 05:32:27  jesjones
 *		Commented out some unused code.
 *		
 *		Revision 1.3  2000/11/13 00:53:06  jesjones
 *		Uses Bind instead of Adapt.
 *		
 *		Revision 1.2  2000/11/09 12:05:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 3/19/00	JDJ		Disabled ObscureCursor.
 *		 <4>	 9/11/99	JDJ		ObscureCursor only spawns a thread if the cursor is visible.
 *		 <3>	 6/12/99	JDJ		Implemented ObscureCursor.
 *		 <2>	12/23/98	JDJ		Added kResizeLeftRightCursor and kResizeUpDownCursor.
 *		 <1>	 1/13/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XCursor.h>

#include <WSystemInfo.h>
#include <XBind.h>
#include <XError.h>
#include <XExceptions.h>
#include <XThread.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const ResID kArrowID     = 32512;		// $$ it'd be better to use one of the constants in WinUser.h, but they're formatted as strings...
const ResID kIBeamID     = 32513;
const ResID kWatchID     = 32514;		// wait cursor
const ResID kCrossHairID = 32515;
const ResID kLeftRightID = 32644;
const ResID kUpDownID    = 32645;		


// ===================================================================================
//	class ZObscureCursor
// ===================================================================================
class ZObscureCursor : public XThread {
	
	typedef XThread Inherited;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZObscureCursor();
	
  						ZObscureCursor();
  						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnRun();
	
	virtual void 		OnException(const std::exception* e);

//-----------------------------------
//	Member Data
//
protected:
	XPoint	mCursorLoc;
};


//---------------------------------------------------------------
//
// ZObscureCursor::~ZObscureCursor
//
//---------------------------------------------------------------
ZObscureCursor::~ZObscureCursor()
{
}


//---------------------------------------------------------------
//
// ZObscureCursor::ZObscureCursor
//
//---------------------------------------------------------------
ZObscureCursor::ZObscureCursor()
{
	VERIFY(::GetCursorPos(mCursorLoc));
}


//---------------------------------------------------------------
//
// ZObscureCursor::OnRun
//
//---------------------------------------------------------------
void ZObscureCursor::OnRun()
{
	while (true) {
		XPoint loc;
		int32 succeeded = ::GetCursorPos(loc);
		ThrowIf(!succeeded);
		
		if (loc != mCursorLoc)
			break;
		else
			::Sleep(100);
	}

	// Microsoft's docs don't seem to mention this but ShowCursor
	// doesn't appear to do anything when called from outside the
	// main thread (in NT 4).
//	XCallback1<int32, bool> callback(::ShowCursor);
//	XCallback0<void> closure = Bind1(callback, true);
//	XCommand* command = new XCallbackCommand(closure);		// $$$ XCallbackCommand is in the UI layer...
//	command->Post();										// $$$ maybe there should be some sort of deferred callback handler?
}


//---------------------------------------------------------------
//
// ZObscureCursor::OnException
//
//---------------------------------------------------------------
void ZObscureCursor::OnException(const std::exception* e)
{
	DEBUGSTR("Got an error in ZObscureCursor ", XError::Instance()->GetText(e));
	
//	XCallback1<int32, bool> callback(::ShowCursor);
//	XCallback0<void> closure = Bind1(callback, true);
//	XCommand* command = new XCallbackCommand(closure);
//	command->Post();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XCursor
// ===================================================================================

const XCursor kArrowCursor(kArrowID);
const XCursor kIBeamCursor(kIBeamID);
const XCursor kCrossHairCursor(kCrossHairID);
const XCursor kWatchCursor(kWatchID);
const XCursor kLeftRightCursor(kLeftRightID);
const XCursor kUpDownCursor(kUpDownID);

//---------------------------------------------------------------
//
// XCursor::~XCursor
//
//---------------------------------------------------------------
XCursor::~XCursor()
{
	mCursor = nil;			// don't need to delete this in Win32
	mID = nil;				// keep Lint happy
}


//---------------------------------------------------------------
//
// XCursor::XCursor (ResID)
//
//---------------------------------------------------------------
XCursor::XCursor(ResID id)
{
	mID = MAKEINTRESOURCE(id);
	
	mCursor = ::LoadCursor(nil, mID);	// for system cursors
	if (mCursor == nil)
		mCursor = ::LoadCursor(WSystemInfo::GetAppInstance(), mID);	// for the app's cursors
		
	ThrowIfNil(mCursor);
}


//---------------------------------------------------------------
//
// XCursor::XCursor (XCursor)
//
//---------------------------------------------------------------
XCursor::XCursor(const XCursor& rhs)
{
	mCursor = rhs.mCursor;					// since these aren't destroyed we don't need to copy...
	mID = rhs.mID;
}


//---------------------------------------------------------------
//
// XCursor::operator=
//
//---------------------------------------------------------------
XCursor& XCursor::operator=(const XCursor& rhs)
{
	if (this != &rhs && *this != rhs) {
		mCursor = rhs.mCursor;				// since these aren't destroyed we don't need to copy...
		mID = rhs.mID;
	}

	return *this;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XCursorUtils
// ===================================================================================

//---------------------------------------------------------------
//
// XCursorUtils::Init									[static]
//
//---------------------------------------------------------------
void XCursorUtils::Init()
{
	XCursorUtils::SetCursor(kArrowCursor);
}


//---------------------------------------------------------------
//
// XCursorUtils::SetCursor								[static]
//
//---------------------------------------------------------------
void XCursorUtils::SetCursor(const XCursor& cursor)
{
	(void) ::SetCursor(cursor.mCursor);
}


//---------------------------------------------------------------
//
// XCursorUtils::ForceBusy ()							[static]
//
//---------------------------------------------------------------
void XCursorUtils::ForceBusy()
{
	(void) ::SetCursor(kWatchCursor.mCursor);
}


//---------------------------------------------------------------
//
// XCursorUtils::ForceBusy (XCursor)					[static]
//
//---------------------------------------------------------------
void XCursorUtils::ForceBusy(const XCursor& cursor)
{
	(void) ::SetCursor(cursor.mCursor);
}


//---------------------------------------------------------------
//
// XCursorUtils::StillBusy								[static]
//
//---------------------------------------------------------------
void XCursorUtils::StillBusy()
{
	// Windows doesn't support busy cursor timing out
}


//---------------------------------------------------------------
//
// XCursorUtils::SetTimeout								[static]
//
//---------------------------------------------------------------
void XCursorUtils::SetTimeout(MilliSecond duration)
{
	UNUSED(duration);

	// Windows doesn't support busy cursor timing out
}


//---------------------------------------------------------------
//
// XCursorUtils::GetTimeout								[static]
//
//---------------------------------------------------------------
MilliSecond XCursorUtils::GetTimeout()
{
	return 0;
}


//---------------------------------------------------------------
//
// XCursorUtils::GetPosition							[static]
//
//---------------------------------------------------------------
XPoint XCursorUtils::GetPosition()
{
	XPoint loc;
	
	int32 succeeded = ::GetCursorPos(loc);
	ThrowIf(!succeeded);
	
	return loc;
}


//---------------------------------------------------------------
//
// XCursorUtils::CanMove								[static]
//
//---------------------------------------------------------------
bool XCursorUtils::CanMove()
{
	return true;
}


//---------------------------------------------------------------
//
// XCursorUtils::MoveTo									[static]
//
//---------------------------------------------------------------
void XCursorUtils::MoveTo(int32 x, int32 y)
{
	int32 succeeded = ::SetCursorPos(x, y);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XCursorUtils::MoveBy									[static]
//
//---------------------------------------------------------------
void XCursorUtils::MoveBy(int32 dx, int32 dy)
{
	XPoint loc = XCursorUtils::GetPosition();
	
	int32 succeeded = ::SetCursorPos(loc.x + dx, loc.y + dy);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XCursorUtils::ObscureCursor							[static]
//
//---------------------------------------------------------------
void XCursorUtils::ObscureCursor()							
{
#if 0											// $$$ relies on XCallbackCommand which is in the UI layer...
	int32 count = ::ShowCursor(false);
	if (count == -1) {
		XThread* thread = new ZObscureCursor;	// if the cursor was visible then spawn off a thread to show the cursor once the mouse moves
		thread->Start();
		thread->RemoveReference();				// thread will be deleted after it exits
	
	} else 
		(void) ::ShowCursor(true);				// if the cursor was hidden then restore the cursor visibility counter
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XForceBusyCursor
// ===================================================================================

//---------------------------------------------------------------
//
// XForceBusyCursor::~XForceBusyCursor
//
//---------------------------------------------------------------
XForceBusyCursor::~XForceBusyCursor()
{
	(void) ::SetCursor(mOldCursor);
	mOldCursor = nil;
}


//---------------------------------------------------------------
//
// XForceBusyCursor::XForceBusyCursor ()
//
//---------------------------------------------------------------
XForceBusyCursor::XForceBusyCursor() : mOldCursor(::GetCursor())
{
	XCursorUtils::ForceBusy();
}


//---------------------------------------------------------------
//
// XForceBusyCursor::XForceBusyCursor (XCursor)
//
//---------------------------------------------------------------
XForceBusyCursor::XForceBusyCursor(const XCursor& cursor) : mOldCursor(::GetCursor())
{
	XCursorUtils::SetCursor(cursor);
}


}	// namespace Whisper

