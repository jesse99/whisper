/*
 *  File:       WDrawControl.cpp
 *  Summary:   	IDraw implementation for controls.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WDrawControl.cpp,v $
 *		Revision 1.4  2001/04/21 03:31:14  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/02/21 08:16:25  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.2  2000/11/09 12:06:49  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDraw.h>

#include <IControl.h>
#include <IDrawExtensible.h>
#include <IDrawHelper.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IScrollableControl.h>
#include <XShapes.h>

namespace Whisper {


// ===================================================================================
//	class XDrawControl
//!		IDraw implementation for controls.
// ===================================================================================
class XDrawControl : public IDraw {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDrawControl();
	
						XDrawControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		HandleDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);
};

DEFINE_INTERFACE_FACTORY(XDrawControl)

//---------------------------------------------------------------
//
// XDrawControl::~XDrawControl
//
//---------------------------------------------------------------
XDrawControl::~XDrawControl()
{
}


//---------------------------------------------------------------
//
// XDrawControl::XDrawControl
//
//---------------------------------------------------------------
XDrawControl::XDrawControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDrawControl::HandleDraw
//
//---------------------------------------------------------------
void XDrawControl::HandleDraw(XDrawContext& context, const XRegion& dirtyWindowRgn, bool isActive)
{	
	IControlPtr control(this);	
	PRECONDITION(control->IsVisible());
	
	try {
		// Set the draw context's origin
		XPoint origin = kZeroPt;
		IScrollableControlPtr scrollable = GetInterface<IScrollableControl>(this);
		if (scrollable) 
			origin = scrollable->GetScrollOffset();
		context.SetOrigin(origin);

		// Set the draw context's clip region to the control's extent
		XRegion myClip = dirtyWindowRgn - origin;
		context.SetClip(myClip);	
		
		// If we have something to draw then,
		if (!myClip.IsEmpty()) {	
			IGeometryPtr geometry(this);
		
			// draw the pre-adorners,			
			bool handled = false;
			IDrawExtensiblePtr extensible(this);
			if (extensible)
				handled = extensible->HandleDraw(context, geometry->GetExtent(), true, isActive);
			
			// if the adorners say it's OK to draw the control then,
			if (!handled) {

				// draw the control,
				IDrawHelperPtr helper(this);
				if (helper)						
					helper->OnDraw(context, myClip, isActive);
				
				else {
					HWND controlH = control->GetOSControl();
					WNDPROC proc = control->GetSystemWindowProc();
					(void) ::CallWindowProc(proc, controlH, WM_PAINT, (uint32) context.GetOSContext(), 0);
				}
									
				// and let the post-adorners draw.
				if (extensible)
					(void) extensible->HandleDraw(context, geometry->GetExtent(), false, isActive);
			}
		}
			
	} catch (...) {
		DEBUGSTR("Caught an exception in XDrawControl::HandleDraw!");	// don't propagate (don't want to get into an infinite loop because an error dialog causes a re-paint)
	}
}


}	// namespace Whisper
