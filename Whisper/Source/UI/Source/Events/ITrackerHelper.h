/*
 *  File:       ITrackerHelper.h
 *  Summary:   	Interface used by XTracker.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ITrackerHelper.h,v $
 *		Revision 1.1  2001/02/07 05:01:31  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XPoint;
class XRect;


// ===================================================================================
//	class ITrackerHelper
//!		Interface used by XTracker.
// ===================================================================================
class ITrackerHelper : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual	void 		OnStart(const XPoint& startPt) = 0;
						/**< Perform any setup that may be required. */
		
	virtual	void 		OnConstrain(const XPoint& anchorPt, const XPoint& prevPt, XPoint& nextPt, const XRect& bounds) = 0;
						/**< Bounds is the control/window's extent. Normally you'll pin nextPt to bounds. */
	
	virtual	void 		OnTrack(const XPoint& anchorPt, const XPoint& prevPt, const XPoint& nextPt, bool mouseMoved) = 0;
						/**< Perform the track action. Note that this is guaranteed to be called 
						at least once. */
	
	virtual	void 		OnStop(const XPoint& stopPt) = 0;
						/**< Perform any required cleanup. */
};

typedef XInterfacePtr<ITrackerHelper> ITrackerHelperPtr;
typedef XInterfacePtr<const ITrackerHelper> IConstTrackerHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
