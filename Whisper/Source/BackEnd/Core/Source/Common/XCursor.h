/*
 *  File:       XCursor.h
 *  Summary:    Mouse cursor classes.
 *  Written by: Jesse Jones
 *
 *	Classes:	XCursor				- Encapsulates color and b/w cursors.
 *				XCursorUtils		- Cursor utility functions.
 *				XForceBusyCursor	- Stack based class to display a busy cursor.
 *				XHideCursor			- Stack based class to hide the cursor.
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCursor.h,v $
 *		Revision 1.6  2001/04/13 08:01:04  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2000/12/11 07:19:42  jesjones
 *		Fixed a doxygen bug.
 *		
 *		Revision 1.4  2000/12/10 08:51:14  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/12 07:55:21  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.2  2000/11/09 12:29:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 4/28/99	JDJ		Added kOpenHandCursor, kClosedHandCursor, kEyeDropperCursor, 
 *									kBrushCursor, and kPencilCursor..
 *		 <2>	12/23/98	JDJ		Added kResizeLeftRightCursor and kResizeUpDownCursor.
 *		 <1>	 1/13/98	JDJ		Created (from Raven)
 */

#pragma once

#if MAC
	#include <QuickDraw.h>
	#include <Resources.h>

	#if !TARGET_CARBON
		#include <CursorDevices.h>	
	#endif
#endif

#include <XCriticalSection.h>
#include <XPoint.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XCursor;
class ZCursorSpinner;


// ===================================================================================
//	Standard Cursors
// ===================================================================================
extern CORE_EXPORT const XCursor kArrowCursor;			// Standard cursors
extern CORE_EXPORT const XCursor kIBeamCursor;
extern CORE_EXPORT const XCursor kCrossHairCursor;
extern CORE_EXPORT const XCursor kWatchCursor;
extern CORE_EXPORT const XCursor kResizeLeftRightCursor;
extern CORE_EXPORT const XCursor kResizeUpDownCursor;

#if MAC
	extern CORE_EXPORT const XCursor kOpenHandCursor;		
	extern CORE_EXPORT const XCursor kClosedHandCursor;	
	extern CORE_EXPORT const XCursor kEyeDropperCursor;
	extern CORE_EXPORT const XCursor kBrushCursor;
	extern CORE_EXPORT const XCursor kPencilCursor;

	extern CORE_EXPORT const XCursor kBeachBallCursor;
#endif


// ===================================================================================
//	class XCursor
//!		Encapsulates color and b/w cursors.
// ===================================================================================
class CORE_EXPORT XCursor {

	friend class XCursorUtils;

public:
						~XCursor();
	
	explicit 			XCursor(ResID id);
	
						XCursor(const XCursor& rhs);
			
						XCursor& operator=(const XCursor& rhs);
			
			ResID 		GetID() const								{return (ResID) mID;}

			bool 		operator==(const XCursor& rhs) const		{return mID == rhs.mID;}
			bool 		operator!=(const XCursor& rhs) const		{return mID != rhs.mID;}
			bool 		operator<(const XCursor& rhs) const			{return mID < rhs.mID;}

private:
#if MAC
	CCrsrHandle	mColorCursor;
	Cursor*		mBwCursor;
	ResID		mID;
#elif WIN
	HCURSOR		mCursor;
	const char*	mID;			// note that this is not a real char*, it's a ResID that's been cast to a char* (another example of the lovely Windows API :-) )
#endif
};


// ===================================================================================
//	class XCursorUtils
//!		Cursor utility functions.
// ===================================================================================
class CORE_EXPORT XCursorUtils {

	friend class XForceBusyCursor;
	
//-----------------------------------
//	API
//
public:
	//! @name Initialization
	//@{
	static	void 		Init();
						/**< Makes the cursor visible and sets it to kArrowCursor. Note
						that this can be called more than once. */
	//@}

	//! @name Cursor Operations
	//@{
	static 	void 		SetCursor(const XCursor& cursor);
						/**< Changes the cursor and stops the busy cursor animation
						(if cursor != msBusyCursor). */
	
	static 	void 		ObscureCursor();
						/**< Hides the cursor until the mouse moves. */
	//@}
		
	//! @name Busy Cursor
	//@{
	static 	void 		StillBusy();
						/**< Call this for long tasks to avoid timing out. Note that 
						this does nothing if the busy cursor isn't already up.
						Does nothing on Windows. */

	static 	void 		ForceBusy();
						/**< Puts up a busy cursor that animates until it times out or
						SetCursor is called with a different cursor. */
	
	static 	void 		ForceBusy(const XCursor& cursor);
	
	static 	void 		SetTimeout(MilliSecond duration);
						/**< Defaults to 5 seconds. */
		
	static 	MilliSecond GetTimeout();
	
#if MAC
	static 	void 		InstallCustomBusyCursor(const XCursor& newCursor);
						/**< Defaults to kWatchCursor. */
#endif
	//@}

	//! @name Cursor Moving
	//@{
	static	XPoint		GetPosition();
						/**< Returns the mouse position in global coordinates. */

	static 	bool 		CanMove();
						/**< Returns false if there's a cursor device that cannot be moved 
						(like a TouchWindow). */
	
	static	void 		MoveTo(int32 x, int32 y);
						/**< These should be in global coordinates. */
						
	static	void 		MoveBy(int32 dx, int32 dy);
						/**< Move the mouse a relative distance. */
	//@}

//-----------------------------------
//	Member Data
//
private:
#if MAC
	static XCursor				msBusyCursor;
	static XCursor				msCurrentCursor;
	static ZCursorSpinner* 		msCursorSpinner;	
	
	static XCriticalSection		msMutex;
#endif
};


// ===================================================================================
//	class XForceBusyCursor
//!		Stack based class to display a busy cursor..
// ===================================================================================
class CORE_EXPORT XForceBusyCursor {

public:
						~XForceBusyCursor();
						/**< You should use a busy cursor if an operation may take longer
						than 0.1 seconds. If it may take longer than a second spawn
						a thread or popup a progress bar. */

						XForceBusyCursor();

						XForceBusyCursor(const XCursor& cursor);
						/**< cursor is ignored on Windows */

			void 		StillBusy()									{XCursorUtils::StillBusy();}

private:
#if MAC
	XCursor		mOldCursor;
	bool		mWasBusy;
#elif WIN
	HCURSOR		mOldCursor;
#endif
};


// ===================================================================================
//	class XHideCursor
//!		Stack based class to hide the cursor.
// ===================================================================================
class CORE_EXPORT XHideCursor {

public:
						~XHideCursor();

						 XHideCursor();
};


// ===================================================================================
//	Inlines
// ===================================================================================
#if MAC
	inline void XCursorUtils::ObscureCursor()							
	{
		::ObscureCursor();
	}

	inline XHideCursor::~XHideCursor()								
	{
		::ShowCursor();
	} 


	inline XHideCursor::XHideCursor()								
	{
		::HideCursor();
	} 
	
#elif WIN
	inline XHideCursor::~XHideCursor()								
	{
		(void) ::ShowCursor(true);
	} 


	inline XHideCursor::XHideCursor()								
	{
		(void) ::ShowCursor(false);
	} 
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
