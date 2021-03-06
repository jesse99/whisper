/*
 *  File:       CMouseHandler.cpp
 *  Summary:   	IMouseDownHelper implementation that spawns off a tracker to add a rectangle.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CMouseHandler.cpp,v $
 *		Revision 1.4  2001/02/17 10:24:58  jesjones
 *		Tweaked some comments.
 *		
 *		Revision 1.3  2001/02/04 03:30:58  jesjones
 *		Removed implementation inheritance from the mouse handling code.
 *		
 *		Revision 1.2  2001/01/01 03:43:08  jesjones
 *		Uses a helper function when posting an undo action.
 *		
 *		Revision 1.1  2000/12/31 04:23:37  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include <IMouseDownHelper.h>

#include <ICommand.h>
#include <ICommandQueue.h>
#include <IHierarchy.h>
#include <ITracker.h>
#include <IUndoActions.h>
#include <XBind.h>
#include <XCursor.h>
#include <XMouseEvents.h>
#include <XStringUtils.h>

#include "IDocInfo.h"


// ===================================================================================
//	class CMouseHandler
//!		IMouseDownHelper implementation that spawns off a tracker to add a rectangle.
// ===================================================================================
class CMouseHandler : public IMouseDownHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CMouseHandler();
	
						CMouseHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnMouseDown(const XMouseDownEvent& event);
};

DEFINE_INTERFACE_FACTORY(CMouseHandler)

//---------------------------------------------------------------
//
// CMouseHandler::~CMouseHandler
//
//---------------------------------------------------------------
CMouseHandler::~CMouseHandler()
{
}


//---------------------------------------------------------------
//
// CMouseHandler::CMouseHandler
//
//---------------------------------------------------------------
CMouseHandler::CMouseHandler(XBoss* boss) 
{
	IMouseDownHelper::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CMouseHandler::OnMouseDown
//
//---------------------------------------------------------------
bool CMouseHandler::OnMouseDown(const XMouseDownEvent& event)	
{
	ITrackerPtr tracker(L"Rubber Band Tracker");		// it would probably be better to use a custom tracker, but we're trying to minimize the amount of files people need to muck with
	tracker->Init();
	
	tracker->TrackMouse(this, event.GetPosition());		// returns when the user releases the mouse
	
	XRect area = tracker->GetArea();	
	if (area != kZeroRect) {
		IDocInfoPtr doc = GetParent<IDocInfo>(this);

		XCallback2<void, XRect, bool> callback1(doc.Get(), &IDocInfo::AddRect);
		XCallback0<void> doIt = Bind2(callback1, area, true);

		XCallback2<void, XRect, bool> callback2(doc.Get(), &IDocInfo::RemoveRect);
		XCallback0<void> undoIt = Bind2(callback2, area, true);

		IUndoCallbackAction::Post(LoadAppString(L"Add Rect"), doIt, undoIt);
	}
	
	return kHandled;
}




