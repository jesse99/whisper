/*
 *  File:       MTabber.cpp
 *  Summary:   	ITabber implementation for the Mac.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MTabber.cpp,v $
 *		Revision 1.1  2001/01/01 00:28:56  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.1  2000/12/27 03:57:05  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <ITabber.h>

#include <IWindow.h>

namespace Whisper {


// ===================================================================================
//	class XTabber
//!		ITabber implementation for the Mac.
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
	IWindowPtr window(this);
	OSWindow wptr = window->GetOSWindow();
	
	OSErr err = noErr;
	
	if (forward)
		err = ::AdvanceKeyboardFocus(wptr);
	else
		err = ::ReverseKeyboardFocus(wptr);
		
	if (err != noErr && err != errCouldntSetFocus)
		ASSERT(err == noErr);
	
	return kHandled;
}

						
//---------------------------------------------------------------
//
// XTabber::HandleArrowKey
//
//---------------------------------------------------------------
bool XTabber::HandleArrowKey(int16 key)
{	
	UNUSED(key);
	
	return kNotHandled;
}


}	// namespace Whisper
