/*
 *  File:       WCurrentFocus.cpp
 *  Summary:   	Implementation that returns the object that gets first crack at keyboard events.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WCurrentFocus.cpp,v $
 *		Revision 1.3  2001/04/21 03:30:53  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2001/02/24 09:42:32  jesjones
 *		Removed an assert.
 *		
 *		Revision 1.1  2001/01/30 07:27:41  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <ICurrentFocus.h>

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
	
	HWND focusH = ::GetFocus();
	if (focusH != nil) {
		IWindowPtr window = IWindow::GetWindow(focusH);	
		if (window)
			focus = IFocusPtr(window);
			
		else {
			IControlPtr control = IControl::GetControl(focusH);
			focus = IFocusPtr(control);
		}
	}

	if (!focus)
		focus = IFocusPtr(L"Application");
	else
		POSTCONDITION(focus->WantsFocus());

	POSTCONDITION(focus);

	return focus;
}


}	// namespace Whisper
