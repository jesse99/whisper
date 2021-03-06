/*
 *  File:       MBrushWindowDrawer.cpp
 *  Summary:   	IDraw implementation that draws windows using a color or PixPatHandle.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MBrushWindowDrawer.cpp,v $
 *		Revision 1.5  2001/04/21 03:24:12  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/01/22 02:00:08  jesjones
 *		Uses IDrawHelper.
 *		
 *		Revision 1.3  2000/12/14 08:26:42  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:10:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDrawHelper.h>

#include <IBrushDrawer.h>
#include <IWindow.h>
#include <XExceptions.h>
#include <XRGBColor.h>

namespace Whisper {


// ===================================================================================
//	class XBrushWindowDrawer
//!		IDraw implementation that draws windows using a color or PixPatHandle.
// ===================================================================================
class XBrushWindowDrawer : public IDrawHelper, public IBrushDrawer {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XBrushWindowDrawer();
	
						XBrushWindowDrawer(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		SetBrushes(const XBrush& active, const XBrush& inactive);

	virtual void 		SetColors(const XRGBColor& active, const XRGBColor& inactive);

protected:
	virtual void 		OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);
};

DEFINE_INTERFACE_FACTORY(XBrushWindowDrawer)

//---------------------------------------------------------------
//
// XBrushWindowDrawer::~XBrushWindowDrawer
//
//---------------------------------------------------------------
XBrushWindowDrawer::~XBrushWindowDrawer()
{
}


//---------------------------------------------------------------
//
// XBrushWindowDrawer::XBrushWindowDrawer
//
//---------------------------------------------------------------
XBrushWindowDrawer::XBrushWindowDrawer(XBoss* boss)
{	
	IDrawHelper::DoSetBoss(boss);
	IBrushDrawer::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XBrushWindowDrawer::SetBrushes
//
//---------------------------------------------------------------
void XBrushWindowDrawer::SetBrushes(const XBrush& active, const XBrush& inactive)
{
	UNUSED(active);
	UNUSED(inactive);

	// $$$ I guess the way to handle this is to have XBrush
	// $$$ synthesize a PixPatHandle when it's passed in a color
	// $$$ or hatched brush.
	DEBUGSTR("XBrushWindowDrawer doesn't support brushes");	
}


//---------------------------------------------------------------
//
// XBrushWindowDrawer::SetColors
//
//---------------------------------------------------------------
void XBrushWindowDrawer::SetColors(const XRGBColor& active, const XRGBColor& inactive)
{
	PRECONDITION(active == inactive);
	
	IDrawHelper* thisPtr = this;
	IWindowPtr window(thisPtr);
	
	OSColor temp = active.GetOSColor();
	OSStatus err = ::SetWindowContentColor(window->GetOSWindow(), &temp);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// XBrushWindowDrawer::OnDraw
//
//---------------------------------------------------------------
void XBrushWindowDrawer::OnDraw(XDrawContext& context, const XRegion& dirtyRgn, bool isActive)
{
	UNUSED(context);
	UNUSED(dirtyRgn);
	UNUSED(isActive);
}


}	// namespace Whisper
