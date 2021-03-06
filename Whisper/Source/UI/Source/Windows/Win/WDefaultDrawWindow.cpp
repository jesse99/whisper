/*
 *  File:       WDefaultDrawWindow.cpp
 *  Summary:   	IDraw implementation for windows.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WDefaultDrawWindow.cpp,v $
 *		Revision 1.6  2001/02/21 08:16:12  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.5  2000/12/14 08:30:33  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.4  2000/12/08 08:21:15  jesjones
 *		No real changes.
 *		
 *		Revision 1.3  2000/11/25 02:21:04  jesjones
 *		OnDraw special cases alerts and dialogs.
 *		
 *		Revision 1.2  2000/11/09 12:06:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDrawHelper.h>

#include <IChildNode.h>
#include <IDrawExtensible.h>
#include <IGeometry.h>
#include <IWindow.h>
#include <XDrawContext.h>
#include <XShapes.h>

namespace Whisper {


// ===================================================================================
//	class XDefaultDrawWindow
//!		IDraw implementation for windows.
// ===================================================================================
class XDefaultDrawWindow : public IDrawHelper, public IChildNode {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDefaultDrawWindow();
	
						XDefaultDrawWindow(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnAdopted();
	virtual void 		OnOrphaned();

	virtual void 		OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);
};

DEFINE_INTERFACE_FACTORY(XDefaultDrawWindow)

//---------------------------------------------------------------
//
// XDefaultDrawWindow::~XDefaultDrawWindow
//
//---------------------------------------------------------------
XDefaultDrawWindow::~XDefaultDrawWindow()
{
}


//---------------------------------------------------------------
//
// XDefaultDrawWindow::XDefaultDrawWindow
//
//---------------------------------------------------------------
XDefaultDrawWindow::XDefaultDrawWindow(XBoss* boss) 
{	
	IDrawHelper::DoSetBoss(boss);
	IChildNode::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDefaultDrawWindow::OnAdopted	
//
//---------------------------------------------------------------
void XDefaultDrawWindow::OnAdopted()
{
	// Mac uses this, we don't...
}


//---------------------------------------------------------------
//
// XDefaultDrawWindow::OnOrphaned	
//
//---------------------------------------------------------------
void XDefaultDrawWindow::OnOrphaned()
{
	// Mac uses this, we don't...
}


//---------------------------------------------------------------
//
// XDefaultDrawWindow::OnDraw	
//
//---------------------------------------------------------------
void XDefaultDrawWindow::OnDraw(XDrawContext& context, const XRegion& dirtyRgn, bool isActive)
{
	UNUSED(isActive);
	
	IDrawHelper* thisPtr = this;
	IWindowPtr window(thisPtr);
	const SWindowStyle& style = window->GetStyle();
	
	HBRUSH brush = nil;
	if (style.type == kModalDialog || style.type == kAlert)
		brush = ::GetSysColorBrush(COLOR_3DFACE);	// returns a cached value so we shouldn't have to cache it ourselves
	else
		brush = ::GetSysColorBrush(COLOR_WINDOW);
	VERIFY(::FillRgn(context.GetOSContext(), dirtyRgn, brush));
}


}	// namespace Whisper
