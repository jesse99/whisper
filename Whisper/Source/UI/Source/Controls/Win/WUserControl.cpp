/*
 *  File:       WUserControl.cpp
 *  Summary:   	Control used with custom controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WUserControl.cpp,v $
 *		Revision 1.5  2001/04/21 03:34:16  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/03/05 05:40:25  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:32:18  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:12:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IUserControl.h>

#include <IControl.h>
#include <IFocus.h>
#include <IHierarchy.h>
#include <WSystemInfo.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XUserControl
//!		Control used with custom controls.
// ===================================================================================
class XUserControl : public IUserControl {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XUserControl();
	
						XUserControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, uint32 style = kDefaultUserStyle,
							 uint32 procID = kDefaultUserProcID);
};

DEFINE_INTERFACE_FACTORY(XUserControl)

//---------------------------------------------------------------
//
// XUserControl::~XUserControl
//
//---------------------------------------------------------------
XUserControl::~XUserControl()
{
}


//---------------------------------------------------------------
//
// XUserControl::XUserControl
//
//---------------------------------------------------------------
XUserControl::XUserControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XUserControl::Init
//
//---------------------------------------------------------------
void XUserControl::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, uint32 style, uint32 procID)
{
	UNUSED(procID);
	PRECONDITION((style & WS_CHILD) != 0);
	
	IControlPtr super = GetInterface<IControl>(parent);

	IFocusPtr focus(this);
	if (focus)
		style |= WS_TABSTOP;
		
	HWND controlH = ::CreateWindowEx(0,						// extended style
									 _TEXT("STATIC"),		// class name
									 _TEXT(""),				// window name
									 style,					// style
									 frame.left,			// location
									 frame.top,
									 frame.GetWidth(),		// size
									 frame.GetHeight(),
									 super->GetOSControl(),	// parent
									 nil,					// menu
									 WSystemInfo::GetAppInstance(),	// app instance
									 nil);					// user data
	ThrowIfNil(controlH);

	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
}


}	// namespace Whisper
