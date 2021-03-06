/*
 *  File:       WTabber.cpp
 *  Summary:   	ITabber implementation for Windows.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WTabber.cpp,v $
 *		Revision 1.1  2001/01/01 00:29:04  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.2  2000/12/27 03:59:35  jesjones
 *		Fixed the header comment.
 *		
 *		Revision 1.1  2000/12/27 00:55:18  jesjones
 *		Checked in  ff
 *		
 */

#include <XWhisperHeader.h>
#include <ITabber.h>

#include <IControl.h>
#include <IRootControl.h>
#include <ITextBox.h>
#include <IWindow.h>

namespace Whisper {


// ===================================================================================
//	class XTabber
//!		ITabber implementation for Windows.
// ===================================================================================
class XTabber : public ITabber {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XTabber();
	
						XTabber(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		HandleTab(bool forward);
						
	virtual bool 		HandleArrowKey(int16 key);
};

DEFINE_INTERFACE_FACTORY(XTabber)

//---------------------------------------------------------------
//
// XTabber::~XTabber
//
//---------------------------------------------------------------
XTabber::~XTabber()
{
}


//---------------------------------------------------------------
//
// XTabber::XTabber
//
//---------------------------------------------------------------
XTabber::XTabber(XBoss* boss)
{	
	ITabber::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XTabber::HandleTab
//
//---------------------------------------------------------------
bool XTabber::HandleTab(bool forward)
{
	bool handled = false;
	
	HWND startWindow = ::GetFocus();
	if (startWindow != nil) {
		IWindowPtr window(this);
		HWND wptr = nil;
		
		IRootControlPtr root = window->GetRootControl();
		if (root)
			wptr = IControlPtr(root)->GetOSControl();
		else
			wptr = window->GetOSWindow();
		
		HWND nextWindow = ::GetNextDlgTabItem(wptr, startWindow, !forward);
		if (nextWindow != nil && nextWindow != startWindow) {
			IControlPtr control = IControl::GetControl(nextWindow);
			ITextBoxPtr textbox(control);
			if (textbox)
				textbox->Select();
			else
				(void) ::SetFocus(nextWindow);
			handled = true;
		}
	}
	
	return handled;
}

						
//---------------------------------------------------------------
//
// XTabber::HandleArrowKey
//
//---------------------------------------------------------------
bool XTabber::HandleArrowKey(int16 key)
{	
	bool forward = key == kRightArrowKey || key == kDownArrowKey;
	
	bool handled = this->HandleTab(forward);
	
	return handled;
}


}	// namespace Whisper
