/*
 *  File:       MCurrentFocus.cpp
 *  Summary:   	Implementation that returns the object that gets first crack at keyboard events.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MCurrentFocus.cpp,v $
 *		Revision 1.4  2001/04/21 03:24:46  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/03 04:40:37  jesjones
 *		GetFocus returns the app if we're in the background. Inactive frontmost windows no longer have the focus (this may happen if we haven't received the activate event yet).
 *		
 *		Revision 1.2  2001/02/12 09:39:44  jesjones
 *		DoGetChildFocus checks to see if the control is enabled.
 *		
 *		Revision 1.1  2001/01/30 07:27:37  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <ICurrentFocus.h>

#include <MProcess.h>
#include <IControl.h>
#include <IWindow.h>

namespace Whisper {


// ===================================================================================
//	class XCurrentFocus
//!		Implementation that returns the object that gets first crack at keyboard events.
// ===================================================================================
class XCurrentFocus : public ICurrentFocus {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XCurrentFocus();
	
						XCurrentFocus(XBoss* boss);
	
//-----------------------------------
//	Inherited API
//
public:
	virtual IFocusPtr 	GetFocus() const;

//-----------------------------------
//	Internal API
//
private:	
	virtual	IFocusPtr 	DoGetChildFocus(const IWindowPtr& window) const;
};

DEFINE_INTERFACE_FACTORY(XCurrentFocus)

//---------------------------------------------------------------
//
// XCurrentFocus::~XCurrentFocus
//
//---------------------------------------------------------------
XCurrentFocus::~XCurrentFocus()
{
}


//---------------------------------------------------------------
//
// XCurrentFocus::XCurrentFocus
//
//---------------------------------------------------------------
XCurrentFocus::XCurrentFocus(XBoss* boss)
{
	ICurrentFocus::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XCurrentFocus::GetFocus
//
//---------------------------------------------------------------
IFocusPtr XCurrentFocus::GetFocus() const
{	
	IFocusPtr focus;
	
	if (MProcess::InFront()) {
		IWindowMgrPtr windowMgr(L"Application");
		
		// If a modal window is up then it has the focus (except in the
		// rare case that the dialog doesn't have a focused child and
		// doesn't want the focus).
		IWindowPtr window = windowMgr->GetTopModal();	
		if (window) {
			if (window->IsActive()) {					// don't switch in the new focus until the window activates
				focus = this->DoGetChildFocus(window);
				
				if (!focus) {
					focus = IFocusPtr(window);
					
					if (focus && !focus->WantsFocus())
						focus.Release();
				}
			}
			
		} else {
			// Otherwise we'll see if the frontmost floater has a 
			// focused child.
			window = windowMgr->GetTopFloater();	
			if (window)
				focus = this->DoGetChildFocus(window);

			// If the floater doesn't have the focus we'll try to give 
			// it to the frontmost regular window.
			if (!focus) {
				window = windowMgr->GetTopRegular();	
				if (window) {
					if (window->IsActive()) {					// don't switch in the new focus until the window activates
						focus = this->DoGetChildFocus(window);
						
						if (!focus) {
							focus = IFocusPtr(window);
							
							if (focus && !focus->WantsFocus())
								focus.Release();
						}
					}
				}
			}
		}
	}
		
	// If none of the windows has the focus we'll give it to the app.
	if (!focus)
		focus = IFocusPtr(L"Application");
		
	POSTCONDITION(focus && focus->WantsFocus());

	return focus;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XCurrentFocus::DoGetChildFocus
//
//---------------------------------------------------------------
IFocusPtr XCurrentFocus::DoGetChildFocus(const IWindowPtr& window) const
{	
	IFocusPtr focus;
	
	if (window->GetStyle().targetable) {
		ControlHandle controlH = nil;		
		OSErr err = ::GetKeyboardFocus(window->GetOSWindow(), &controlH);

		if (err == noErr && controlH != nil) {
			IControlPtr control = IControl::GetControl(controlH);
			ASSERT(control);
			
			focus = IFocusPtr(control);
			
		} else
			focus = IFocusPtr(window);
	}
		
	if (focus && !focus->WantsFocus())
		focus.Release();

	return focus;
}


}	// namespace Whisper
