/*
 *  File:       MControlMouseHandler.cpp
 *  Summary:   	IMouseDownHelper implementation that handles clicking on controls.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MControlMouseHandler.cpp,v $
 *		Revision 1.4  2001/02/04 03:35:17  jesjones
 *		Removed the implementation inheritance from the mouse handling classes.
 *		
 *		Revision 1.3  2001/01/30 07:18:17  jesjones
 *		Tweaked focus code.
 *		
 *		Revision 1.2  2000/11/09 09:11:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IMouseDownHelper.h>

#include <IClickExtensible.h>
#include <IControl.h>
#include <IControlPressedHelper.h>
#include <ICurrentFocus.h>
#include <IFocus.h>
#include <XMouseEvents.h>

namespace Whisper {


// ===================================================================================
//	class XControlMouseHandler
//!		IMouseDownHelper implementation that handles clicking on controls.
// ===================================================================================
class XControlMouseHandler : public IMouseDownHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XControlMouseHandler();
	
						XControlMouseHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnMouseDown(const XMouseDownEvent& event);
						
//-----------------------------------
//	Internal API
//
private:
			void 		DoSetFocus();
			void 		DoClick(const XMouseDownEvent& event);
};

DEFINE_INTERFACE_FACTORY(XControlMouseHandler)

//---------------------------------------------------------------
//
// XControlMouseHandler::~XControlMouseHandler
//
//---------------------------------------------------------------
XControlMouseHandler::~XControlMouseHandler()
{
}


//---------------------------------------------------------------
//
// XControlMouseHandler::XControlMouseHandler
//
//---------------------------------------------------------------
XControlMouseHandler::XControlMouseHandler(XBoss* boss)
{
	IMouseDownHelper::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XControlMouseHandler::OnMouseDown
//
//---------------------------------------------------------------
bool XControlMouseHandler::OnMouseDown(const XMouseDownEvent& event)	
{
	bool handled = false;

	IControlPtr control(this);
	ASSERT(control->IsVisible());
	
	if (control->IsEnabled()) {	
		this->DoSetFocus();
		this->DoClick(event);
		
		handled = true;
	}
					
	return handled;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XControlMouseHandler::DoSetFocus
//
//---------------------------------------------------------------
void XControlMouseHandler::DoSetFocus()
{
	IControlPtr control(this);
	IFocusPtr focus(this);
	if (focus && !focus->HasFocus() && focus->WantsFocus()) {	// $$ Control Manager doesn't seem to do this automatically, even when kControlGetsFocusOnClick is set...
		uint32 features = 0;
		OSErr err = ::GetControlFeatures(control->GetOSControl(), &features);
		ASSERT(err == noErr);
		
		if (err == noErr && (features & kControlGetsFocusOnClick))
			focus->SetFocus();						
	}	
}


//---------------------------------------------------------------
//
// XControlMouseHandler::DoClick
//
//---------------------------------------------------------------
void XControlMouseHandler::DoClick(const XMouseDownEvent& event)
{
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();

	XPoint loc = event.GetPosition();
	loc = control->LocalToWindow(loc);
	
	uint16 modifiers = (uint16) event.GetOSEvent().modifiers;
	int16 partCode = ::HandleControlClick(controlH, loc.GetOSPoint(), modifiers, nil);
	if (partCode != 0) {
		IClickExtensiblePtr extensible(this);
		if (extensible)
			extensible->HandleClick(event);
                
		IControlPressedHelperPtr pressed(this);
		if (pressed)
			pressed->OnPressed(event);
	}
}


}	// namespace Whisper
