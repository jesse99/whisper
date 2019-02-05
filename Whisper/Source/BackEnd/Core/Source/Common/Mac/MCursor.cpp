/*
 *  File:       MCursor.cpp
 *  Summary:    Mouse cursor classes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MCursor.cpp,v $
 *		Revision 1.6  2001/04/21 03:24:51  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/13 07:49:14  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.4  2001/03/16 10:20:00  jesjones
 *		Added some code that won't compile with Carbon.
 *		
 *		Revision 1.3  2000/12/10 04:03:07  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:11:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 7/21/99	JDJ		Tweaked for Carbon. 
 *		 <4>	 7/19/99	JDJ		Disabled XCursorUtils::MoveTo and MoveBy on Carbon. 
 *		 <3>	 4/28/99	JDJ		Added kOpenHandCursor, kClosedHandCursor, kEyeDropperCursor, 
 *									kBrushCursor, and kPencilCursor..
 *		 <2>	12/23/98	JDJ		Added kResizeLeftRightCursor and kResizeUpDownCursor.
 *		 <1>	 1/13/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XCursor.h>

#include <Appearance.h>
#include <LowMem.h>
#include <ToolUtils.h>
#include <Traps.h>

#include <MSystemInfo.h>
#include <MTimeMgrTimer.h>
#include <SLFunctions.h>
#include <XCriticalSection.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XNumbers.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const ResID kArrowID     = 0;
const ResID kIBeamID     = 1;
const ResID kCrossHairID = 2;
const ResID kPlusID      = 3;
const ResID kBeachBallID = 200;			// animated beach ball
const ResID kWatchID     = 210;			// animated watch
const ResID kLeftRightID = 132;
const ResID kUpDownID    = 136;

const ResID kOpenHandID   = 128;
const ResID kClosedHandID = 134;
const ResID kEyeDropperID = 129;
const ResID kBrushID      = 130;
const ResID kPencilID     = 131;

const XCursor kArrowCursor(kArrowID);
const XCursor kIBeamCursor(kIBeamID);
const XCursor kCrossHairCursor(kCrossHairID);
const XCursor kBeachBallCursor(kBeachBallID);
const XCursor kWatchCursor(kWatchID);
const XCursor kResizeLeftRightCursor(kLeftRightID);
const XCursor kResizeUpDownCursor(kUpDownID);

const XCursor kOpenHandCursor(kOpenHandID);			
const XCursor kClosedHandCursor(kClosedHandID);			
const XCursor kEyeDropperCursor(kEyeDropperID);
const XCursor kBrushCursor(kBrushID);
const XCursor kPencilCursor(kPencilID);


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// SetThemeCursor
//
//---------------------------------------------------------------
static bool SetThemeCursor(ResID id)
{
	bool handled = false;
	
	if (MSystemInfo::GetAppearanceVersion() >= 0x0110) {
		ThemeCursor themeID = ULONG_MAX;
		
		switch (id) {
			case kArrowID:
				themeID = kThemeArrowCursor;
				break;
				
			case kIBeamID:
				themeID = kThemeIBeamCursor;
				break;

			case kCrossHairID:
				themeID = kThemeCrossCursor;
				break;

			case kBeachBallID:
				themeID = kThemeSpinningCursor;
				break;

			case kWatchID:
				themeID = kThemeWatchCursor;
				break;

			case kLeftRightID:
				themeID = kThemeResizeLeftRightCursor;
				break;

//			case kUpDownID:
//				themeID = ???;		// $$$ missing from 3.2 Universal Headers (which is really lame!)
//				break;

			case kOpenHandID:
				themeID = kThemeOpenHandCursor;
				break;

			case kClosedHandID:
				themeID = kThemeClosedHandCursor;
				break;
		}
		
		if (themeID != ULONG_MAX) {
			OSStatus err = ::SetThemeCursor(themeID);
			ASSERT(err == noErr);
			
			handled = true;
		}
	}
	
	return handled;
}

#pragma mark -

// ===================================================================================
//	class XCursor
// ===================================================================================

//---------------------------------------------------------------
//
// XCursor::~XCursor
//
//---------------------------------------------------------------
XCursor::~XCursor()
{
	if (mColorCursor != nil)
		DisposeCCursor(mColorCursor);

	delete mBwCursor;
}


//---------------------------------------------------------------
//
// XCursor::XCursor (ResID)
//
//---------------------------------------------------------------
XCursor::XCursor(ResID id)
{
	mColorCursor = nil;
	mBwCursor = nil;
	mID = id;

	SLDisable();							// Spotlight will complain if there's no color cursor
	mColorCursor = ::GetCCursor(mID);
	SLEnable();

	if (mColorCursor == nil) {
		mBwCursor = new Cursor;

		CursHandle cursor = nil;
		if (mID != 0) {
			cursor = GetCursor(mID);
//			ThrowIfResFail(cursor);			// backend packages don't require resources to be present...
	
			if (cursor != nil) {
				*mBwCursor = **cursor;
	
				HPurge((Handle) cursor);
			}
		}
			
		if (cursor == nil) {
#if TARGET_CARBON
			Cursor arrow;
			GetQDGlobalsArrow(&arrow);
			*mBwCursor = arrow;
#else
			*mBwCursor = qd.arrow;
#endif
		}
	}
}


//---------------------------------------------------------------
//
// XCursor::XCursor (XCursor)
//
//---------------------------------------------------------------
XCursor::XCursor(const XCursor& rhs)
{
	mColorCursor = nil;
	mBwCursor = nil;
	mID = rhs.mID;

	if (rhs.mColorCursor != nil) {
		mColorCursor = ::GetCCursor(rhs.mID);
		ThrowIfResFail(mColorCursor);
	}

	if (rhs.mBwCursor != nil) 
		mBwCursor = new Cursor(*rhs.mBwCursor);
}


//---------------------------------------------------------------
//
// XCursor::operator=
//
//---------------------------------------------------------------
XCursor& XCursor::operator=(const XCursor& rhs)
{
	if (this != &rhs && *this != rhs) {
		CCrsrHandle colorCursor = nil;
		Cursor*		bwCursor = nil;

		try {
			if (rhs.mColorCursor != nil) {
				colorCursor = ::GetCCursor(rhs.mID);
				ThrowIfResFail(colorCursor);
			}

			if (rhs.mBwCursor != nil) 
				bwCursor = new Cursor(*rhs.mBwCursor);

			if (mColorCursor != nil)
				DisposeCCursor(mColorCursor);
			delete mBwCursor;

			mColorCursor = colorCursor;
			mBwCursor = bwCursor;
			mID = rhs.mID;

		} catch(...) {
			if (colorCursor != nil)
				DisposeCCursor(colorCursor);

			delete bwCursor;

			throw;
		}
	}

	return *this;
}

#pragma mark -

// ===================================================================================
//	class ZCursorSpinner
// ===================================================================================
const MilliSecond kSpinFreq = 60;		

class ZCursorSpinner : public MTimeMgrTimerMixIn {

	typedef MTimeMgrTimerMixIn Inherited;

public:
	virtual 			~ZCursorSpinner();

						ZCursorSpinner(ResID id, MilliSecond timeOut);

	virtual void 		StartTimer();
						
			void 		SetCursor(ResID newID);

			void 		SetTimeout(MilliSecond timeOut);

			MilliSecond GetTimeout() const			{return (int32) (1000*mDuration/60);}

protected:
	virtual void 		OnTime();

private:
	ResID		mID;
	Cursor*		mCursors;
	int16		mIndex;
	int16		mNumCursors;

	uint32		mStartTime;			// times are in ticks
	uint32		mDuration;
};


//---------------------------------------------------------------
//
// ZCursorSpinner::~ZCursorSpinner
//
//---------------------------------------------------------------
ZCursorSpinner::~ZCursorSpinner()
{
	this->StopTimer();					// Don't want VBL task running while we tear down the object!

	delete [] mCursors;
}


//---------------------------------------------------------------
//
// ZCursorSpinner::ZCursorSpinner
//
//---------------------------------------------------------------
ZCursorSpinner::ZCursorSpinner(ResID id, MilliSecond timeOut) : MTimeMgrTimerMixIn(kSpinFreq, false)
{
	PRECONDITION(id >= 128 && id < 32767);

	mCursors = nil;

	mIndex = 0;
	mNumCursors = 0;
	mID = 0;

	this->SetTimeout(timeOut);
	this->SetCursor(id);
}


//---------------------------------------------------------------
//
// ZCursorSpinner::StartTimer
//
//---------------------------------------------------------------
void ZCursorSpinner::StartTimer()
{
	if (mCursors != nil && mNumCursors > 0) {
		ASSERT(mIndex >= 0 && mIndex < mNumCursors);
		
		::SetCursor(&mCursors[mIndex]);
	}

	mStartTime = (uint32) TickCount();

	Inherited::StartTimer();
}


//---------------------------------------------------------------
//
// 'acur' struct
//
//---------------------------------------------------------------
#if defined(__POWERPC__)
#pragma options align=mac68k
#endif

struct SAcurEntry {
	int16	cursorID;
	int16	filler;
};

struct SAcurRecord {
	int16		count;
	int16		filler;

	SAcurEntry	cursors[1];			// variable length array
};

#if defined(__POWERPC__)
#pragma options align=reset
#endif

typedef SAcurRecord* 	AcurRecordPtr;
typedef AcurRecordPtr*	AcurRecordHandle;


//---------------------------------------------------------------
//
// ZCursorSpinner::SetCursor
//
//---------------------------------------------------------------
void ZCursorSpinner::SetCursor(ResID newID)
{
	PRECONDITION(newID >= 128 && newID < 32767);

	if (mID != newID) {
		AcurRecordHandle	acurRsrc = nil;
		Cursor* 			newCursors = nil;
		bool				wasRunning = this->TimerIsRunning();

		try {
			this->StopTimer();
			
			acurRsrc = (AcurRecordHandle) GetResource('acur', newID);
			ThrowIfResFail(acurRsrc);

			HNoPurge((Handle) acurRsrc);
			ThrowIfMemError();
			
			int16 count = (**acurRsrc).count;
			newCursors = new Cursor[count];

			for (int16 index = 0; index < count; index++) {
				ResID cursorID = (**acurRsrc).cursors[index].cursorID;

				CursHandle cursor = GetCursor(cursorID);
				ThrowIfResFail(cursor);
				
				LoadResource((Handle) cursor);

				newCursors[index] = **cursor;

				HPurge((Handle) cursor);
			}

			mIndex = 0;
			mNumCursors = count;

			delete [] mCursors;
			mCursors = newCursors;

			HPurge((Handle) acurRsrc);
			
			if (wasRunning)
				this->StartTimer();

		} catch (...) {
			DEBUGSTR("Couldn't initialize the spinning cursor!");

			delete [] newCursors;

			if (acurRsrc != nil)
				HPurge((Handle) acurRsrc);

			// Don't rethrow
		}
	}
}


//---------------------------------------------------------------
//
// ZCursorSpinner::SetTimeout
//
//---------------------------------------------------------------
void ZCursorSpinner::SetTimeout(MilliSecond timeOut) 
{
	mDuration = (uint32) (60*timeOut/1000);
}


//---------------------------------------------------------------
//
// ZCursorSpinner::OnTime
//
//---------------------------------------------------------------
#pragma profile off
void ZCursorSpinner::OnTime()		// interrupt code
{
	SAFE_ASSERT(mTimerFreq > 20);
		
	if (mCursors != nil && mNumCursors > 0) {
		if (TickCount() <= mStartTime + mDuration) {
#if !TARGET_CARBON
			if (!LMGetCrsrBusy()) 	// $$$ may cause the cursor to tear...
#endif
			{
				mIndex = numeric_cast<int16>((mIndex + 1) % mNumCursors);

				SAFE_ASSERT(mIndex >= 0);
				SAFE_ASSERT(mIndex < mNumCursors);

				::SetCursor(&mCursors[mIndex]);
			}
		}
	}
}
#pragma profile reset

#pragma mark -

// ===================================================================================
//	class XCursorUtils
// ===================================================================================

const MilliSecond kDefaultTimeout = 10*1000;

XCursor				XCursorUtils::msBusyCursor(kWatchID);
XCursor				XCursorUtils::msCurrentCursor(kArrowID);
ZCursorSpinner* 	XCursorUtils::msCursorSpinner = nil;

XCriticalSection	XCursorUtils::msMutex;

//---------------------------------------------------------------
//
// XCursorUtils::Init									[static]
//
//---------------------------------------------------------------
void XCursorUtils::Init()
{
	if (msCursorSpinner == nil) {
		XEnterCriticalSection enter(msMutex);

		if (msCursorSpinner == nil) {
	
			// ZCursorSpinner does a lot of stuff that we don't want to
			// do at static construction time so we'll create one the
			// first time Init is called.
			static ZCursorSpinner spinner(kWatchID, kDefaultTimeout);
			msCursorSpinner = &spinner;
		}
	}
	
	// Stop spinning the busy cursor (we do this because it's legal
	// to call Init at any time and we want to switch back to an arrow).
	if (msCursorSpinner->TimerIsRunning())
		msCursorSpinner->StopTimer();			

	// Switch the cursor to an arrow.
	::InitCursor();
	if (msCurrentCursor != kArrowCursor)
		msCurrentCursor = kArrowCursor;
}


//---------------------------------------------------------------
//
// XCursorUtils::SetCursor								[static]
//
//---------------------------------------------------------------
void XCursorUtils::SetCursor(const XCursor& cursor)
{
	if (msCursorSpinner == nil)
		XCursorUtils::Init();
		
	if (cursor == msBusyCursor)
		XCursorUtils::ForceBusy();

	else {
		msCursorSpinner->StopTimer();

		if (!Whisper::SetThemeCursor(cursor.GetID())) {
			if (cursor.mColorCursor != nil)
				::SetCCursor(cursor.mColorCursor);
			else
				::SetCursor(cursor.mBwCursor);
		}
	}

	if (msCurrentCursor != cursor)
		msCurrentCursor = cursor;
}


//---------------------------------------------------------------
//
// XCursorUtils::ForceBusy ()							[static]
//
//---------------------------------------------------------------
void XCursorUtils::ForceBusy()
{
	if (msCursorSpinner == nil)
		XCursorUtils::Init();
		
	if (msCurrentCursor != msBusyCursor)
		msCurrentCursor = msBusyCursor;

	msCursorSpinner->StartTimer();
}


//---------------------------------------------------------------
//
// XCursorUtils::ForceBusy (XCursor)					[static]
//
//---------------------------------------------------------------
void XCursorUtils::ForceBusy(const XCursor& cursor)
{
	if (cursor != msBusyCursor)
		XCursorUtils::InstallCustomBusyCursor(cursor);

	XCursorUtils::ForceBusy();
}


//---------------------------------------------------------------
//
// XCursorUtils::StillBusy								[static]
//
//---------------------------------------------------------------
void XCursorUtils::StillBusy()
{
	static int32 lastTime = 0;			// this is often called in a tight loop so we'll only tickle the cursor if enough time elapses
	
	int32 time = (int32) TickCount();
		
	if (msCurrentCursor == msBusyCursor && time != lastTime) {
		if (msCursorSpinner == nil)
			XCursorUtils::Init();
		
		msCursorSpinner->StartTimer();
		
		lastTime = time;
	}
}


//---------------------------------------------------------------
//
// XCursorUtils::SetTimeout								[static]
//
//---------------------------------------------------------------
void XCursorUtils::SetTimeout(MilliSecond duration)
{
	if (msCursorSpinner == nil)
		XCursorUtils::Init();
		
	PRECONDITION(duration > 0);

	msCursorSpinner->SetTimeout(duration);
}


//---------------------------------------------------------------
//
// XCursorUtils::GetTimeout								[static]
//
//---------------------------------------------------------------
MilliSecond XCursorUtils::GetTimeout()
{
	if (msCursorSpinner == nil)
		XCursorUtils::Init();
		
	return msCursorSpinner->GetTimeout();
}


//---------------------------------------------------------------
//
// XCursorUtils::InstallCustomBusyCursor				[static]
//
//---------------------------------------------------------------
void XCursorUtils::InstallCustomBusyCursor(const XCursor& newCursor)
{
	if (msCursorSpinner == nil)
		XCursorUtils::Init();
		
	msBusyCursor = newCursor;

	msCursorSpinner->SetCursor(msBusyCursor.mID);
}


//---------------------------------------------------------------
//
// XCursorUtils::GetPosition							[static]
//
//---------------------------------------------------------------
XPoint XCursorUtils::GetPosition()
{
	Point loc;
	GetGlobalMouse(&loc);

	return loc;
}


//---------------------------------------------------------------
//
// XCursorUtils::CanMove								[static]
//
//---------------------------------------------------------------
bool XCursorUtils::CanMove()
{
	if (msCursorSpinner == nil)
		XCursorUtils::Init();
		
	return false;				// $$$ might be able to do this on OS X
}


//---------------------------------------------------------------
//
// XCursorUtils::MoveTo									[static]
//
//---------------------------------------------------------------
void XCursorUtils::MoveTo(int32 x, int32 y)
{
	PRECONDITION(XCursorUtils::CanMove());
	UNUSED(x);
	UNUSED(y);
}


//---------------------------------------------------------------
//
// XCursorUtils::MoveBy									[static]
//
//---------------------------------------------------------------
void XCursorUtils::MoveBy(int32 dx, int32 dy)
{
	PRECONDITION(XCursorUtils::CanMove());
	UNUSED(dx);
	UNUSED(dy);
}

#pragma mark -

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
	if (mWasBusy)
		if (mOldCursor == XCursorUtils::msBusyCursor)
			XCursorUtils::ForceBusy();
		else
			XCursorUtils::ForceBusy(mOldCursor);
			
	else
		XCursorUtils::SetCursor(mOldCursor);
}


//---------------------------------------------------------------
//
// XForceBusyCursor::XForceBusyCursor ()
//
//---------------------------------------------------------------
XForceBusyCursor::XForceBusyCursor() : mOldCursor(XCursorUtils::msCurrentCursor)
{
	if (XCursorUtils::msCursorSpinner == nil)
		XCursorUtils::Init();
		
	mWasBusy = XCursorUtils::msCursorSpinner->TimerIsRunning();
		
	XCursorUtils::ForceBusy();
}


//---------------------------------------------------------------
//
// XForceBusyCursor::XForceBusyCursor (XCursor)
//
//---------------------------------------------------------------
XForceBusyCursor::XForceBusyCursor(const XCursor& cursor) : mOldCursor(XCursorUtils::msCurrentCursor)
{
	if (XCursorUtils::msCursorSpinner == nil)
		XCursorUtils::Init();
		
	mWasBusy = XCursorUtils::msCursorSpinner->TimerIsRunning();
		
	XCursorUtils::ForceBusy(cursor);
}


}	// namespace Whisper

