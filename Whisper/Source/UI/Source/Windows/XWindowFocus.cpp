/*
 *  File:       XWindowFocus.cpp
 *  Summary:   	The interface that handles focusing on windows.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XWindowFocus.cpp,v $
 *		Revision 1.5  2001/04/27 04:38:06  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/01/30 07:17:23  jesjones
 *		Added HasFocus.
 *		
 *		Revision 1.3  2000/12/14 08:37:58  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:59:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IFocus.h>

#include <ICurrentFocus.h>
#include <IWindow.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XWindowFocus
//!		The interface that handles focusing on windows.
// ===================================================================================
class XWindowFocus : public IFocus {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XWindowFocus();
	
						XWindowFocus(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		WantsFocus() const;
	virtual void		SetFocus();
	virtual bool 		HasFocus() const;
};

DEFINE_INTERFACE_FACTORY(XWindowFocus)

//---------------------------------------------------------------
//
// XWindowFocus::~XWindowFocus
//
//---------------------------------------------------------------
XWindowFocus::~XWindowFocus()
{
}


//---------------------------------------------------------------
//
// XWindowFocus::XWindowFocus
//
//---------------------------------------------------------------
XWindowFocus::XWindowFocus(XBoss* boss)
{	
	IFocus::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XWindowFocus::WantsFocus
//
//---------------------------------------------------------------
bool XWindowFocus::WantsFocus() const
{
	XInterfacePtr<const IWindow> window(this);

	const SWindowStyle& style = window->GetStyle();
	bool wantsFocus = window->IsVisible() && style.targetable;
	
	return wantsFocus;
}


//---------------------------------------------------------------
//
// XWindowFocus::SetFocus
//
//---------------------------------------------------------------
void XWindowFocus::SetFocus()
{
	IWindowPtr window(this);
	PRECONDITION(window->GetStyle().targetable);

	OSWindow wptr = window->GetOSWindow();
	
#if MAC
	OSErr err = ::ClearKeyboardFocus(wptr);	
	ThrowIfOSErr(err);
#elif WIN
	(void) ::SetFocus(wptr);
#endif
}


//---------------------------------------------------------------
//
// XWindowFocus::HasFocus
//
//---------------------------------------------------------------
bool XWindowFocus::HasFocus() const
{
	ICurrentFocusPtr current(L"Application");
	bool has = current->GetFocus() == this;
	
	return has;
}


}	// namespace Whisper
