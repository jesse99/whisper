/*
 *  File:       XRestoreWindowPosition.cpp
 *  Summary:   	Saves and restores window positions using a pref.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRestoreWindowPosition.cpp,v $
 *		Revision 1.4  2001/04/02 23:35:54  jesjones
 *		Tweaked for CW Pro 5.3
 *		
 *		Revision 1.3  2000/12/14 08:37:15  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/12/08 08:16:22  jesjones
 *		No longer changes the window's size if its not resizeable.
 *		
 *		Revision 1.1  2000/11/20 00:56:22  jesjones
 *		Checked in	
 */

#include <XWhisperHeader.h>
#include <IRestoreWindowPosition.h>

#include <IGeometry.h>
#include <IWindow.h>
#include <XPreference.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// IsResizeable
//
//---------------------------------------------------------------
static bool IsResizeable(const IConstWindowPtr& window) 	// $$ move this into IWindow?
{
	const SWindowStyle& style = window->GetStyle();

#if MAC
	bool resizeable = (style.attributes & kWindowResizableAttribute) == kWindowResizableAttribute;
#elif WIN
	bool resizeable = (style.style & WS_THICKFRAME) == WS_THICKFRAME || style.drawSizeGrip;
#endif

	return resizeable;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XRestoreWindowPosition
//!		Saves and restores window positions using a pref.
// ===================================================================================
class XRestoreWindowPosition : public IRestoreWindowPosition {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XRestoreWindowPosition();
	
						XRestoreWindowPosition(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Save();

	virtual void 		Restore();
};

DEFINE_INTERFACE_FACTORY(XRestoreWindowPosition)

//---------------------------------------------------------------
//
// XRestoreWindowPosition::~XRestoreWindowPosition
//
//---------------------------------------------------------------
XRestoreWindowPosition::~XRestoreWindowPosition()
{
}


//---------------------------------------------------------------
//
// XRestoreWindowPosition::XRestoreWindowPosition
//
//---------------------------------------------------------------
XRestoreWindowPosition::XRestoreWindowPosition(XBoss* boss)
{	
	IRestoreWindowPosition::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XRestoreWindowPosition::Save
//
//---------------------------------------------------------------
void XRestoreWindowPosition::Save()
{
	IGeometryPtr geometry(this);
	XRect frame = geometry->GetFrame();
	
	IWindowPtr window(this);
	XPreference<XRect> pref(window->GetName() + L" Position", kZeroRect);
	*pref = frame;
}


//---------------------------------------------------------------
//
// XRestoreWindowPosition::Restore
//
//---------------------------------------------------------------
void XRestoreWindowPosition::Restore()
{	
	IConstWindowPtr window(this);
	XPreference<XRect> pref(window->GetName() + L" Position", kZeroRect);
	if (*pref != kZeroRect) {
		IGeometryPtr geometry(this);
		if (IsResizeable(window))
			geometry->SetFrame(*pref);		// our caller will call ForceOnScreen
//		else
//			geometry->SetLocation((*pref)[topLeft]);
	}
}


}	// namespace Whisper
