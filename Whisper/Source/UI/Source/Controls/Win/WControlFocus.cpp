/*
 *  File:       XControlFocus.cpp
 *  Summary:   	IFocus implementation for Windows controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WControlFocus.cpp,v $
 *		Revision 1.5  2001/02/21 08:15:53  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.4  2000/12/31 04:41:22  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:30:21  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:05:33  jesjones
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
//!		IFocus implementation for Windows controls.
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
			int32 style = ::GetWindowLong(control->GetOSControl(), GWL_STYLE);
			wantsFocus = (style & WS_TABSTOP) != 0;
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
	(void) ::SetFocus(control->GetOSControl());
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
