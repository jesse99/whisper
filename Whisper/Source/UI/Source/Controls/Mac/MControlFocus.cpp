/*
 *  File:       XControlFocus.cpp
 *  Summary:   	IFocus implementation for Mac controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MControlFocus.cpp,v $
 *		Revision 1.5  2001/01/30 07:17:16  jesjones
 *		Added HasFocus.
 *		
 *		Revision 1.4  2000/12/31 04:26:17  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:27:08  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:11:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IFocus.h>

#include <IControl.h>
#include <ICurrentFocus.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XControlFocus
//!		IFocus implementation for Mac controls.
// ===================================================================================
class XControlFocus : public IFocus {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XControlFocus();
	
						XControlFocus(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		WantsFocus() const;
	virtual void		SetFocus();
	virtual bool 		HasFocus() const;
};

DEFINE_INTERFACE_FACTORY(XControlFocus)

//---------------------------------------------------------------
//
// XControlFocus::~XControlFocus
//
//---------------------------------------------------------------
XControlFocus::~XControlFocus()
{
}


//---------------------------------------------------------------
//
// XControlFocus::XControlFocus
//
//---------------------------------------------------------------
XControlFocus::XControlFocus(XBoss* boss)
{	
	IFocus::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XControlFocus::WantsFocus
//
//---------------------------------------------------------------
bool XControlFocus::WantsFocus() const
{
	bool wantsFocus = false;
	
	IConstControlPtr control(this);
	
	if (control->IsVisible()) {
		if (control->IsEnabled()) {
			ControlHandle controlH = control->GetOSControl();
			
			uint32 features = 0;
			OSErr err = ::GetControlFeatures(controlH, &features);
			if (err == noErr)
				wantsFocus = (features & kControlSupportsFocus) != 0;
			else
				DEBUGSTR("Got a ", err, " error calling GetControlFeatures");
		}
	}
	
	return wantsFocus;
}

						
//---------------------------------------------------------------
//
// XControlFocus::SetFocus
//
//---------------------------------------------------------------
void XControlFocus::SetFocus()
{
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();

	WindowRef wptr = control->GetOSWindow();

	ControlHandle focusH = nil;
	(void) ::GetKeyboardFocus(wptr, &focusH);
	
	if (focusH != controlH) {
		OSErr err = ::SetKeyboardFocus(wptr, controlH, kControlFocusNextPart);	// $$$ docs don't discuss what the part code should be but, from experimentation, this seems to be the value to use...
		ThrowIfOSErr(err);
	}
}


//---------------------------------------------------------------
//
// XControlFocus::HasFocus
//
//---------------------------------------------------------------
bool XControlFocus::HasFocus() const
{
	ICurrentFocusPtr current(L"Application");
	bool has = current->GetFocus() == this;
	
	return has;
}


}	// namespace Whisper
