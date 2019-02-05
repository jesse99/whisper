/*
 *  File:       ITracker.h
 *  Summary:   	Interface used to handle mouse dragging.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ITracker.h,v $
 *		Revision 1.3  2001/02/17 10:22:40  jesjones
 *		Renamed HandleStart TrackMouse.
 *		
 *		Revision 1.2  2001/02/07 04:55:58  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/31 04:24:35  jesjones
 *		Checked in	
 */

#pragma once

#include <XInterfacePtr.h>
#include <XRect.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class IControl;
typedef XInterfacePtr<IControl> IControlPtr;

class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;

class IWindow;
typedef XInterfacePtr<IWindow> IWindowPtr;


//-----------------------------------
// 	Constants
//
const bool kTrackNonMovement     = true;	//!< ITracker does something if the mouse isn't moving (eg auto-scrolling)
const bool kDontTrackNonMovement = false;	//!< ITracker does nothing if the mouse isn't moving


// ===================================================================================
//	class ITracker
//!		Interface used to handle mouse dragging.
// ===================================================================================
class ITracker : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual void 		Init(bool trackNonMovement = kDontTrackNonMovement, bool useDragSound = false) = 0;
	
	virtual void 		TrackMouse(const IHierarchyPtr& context, const XPoint& startPt) = 0;
						/**< Context should be a window or a control. StartPt should be in context's
						coordinate system. Doesn't return until the user releases the mouse. */
	
	virtual XRect 		GetArea() const = 0;
						/**< If the tracker is still tracking this returns a rectangle enclosing 
						startPt and the current (constrained) mouse location. If the tracker has 
						finished it uses startPt and the (constrained) location the mouse was
						released at. Coordinates are in context's coordinate system. */
						
	virtual IControlPtr GetControl() const = 0;
						/**< Returns the control the user started tracking in (may be nil). */
						
	virtual IWindowPtr 	GetWindow() const = 0;
						/**< Returns the window the user started tracking in. */
};

typedef XInterfacePtr<ITracker> ITrackerPtr;
typedef XInterfacePtr<const ITracker> IConstTrackerPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
