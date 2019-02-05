/*
 *  File:       WBrushWindowDrawer.cpp
 *  Summary:   	IDraw interface that draws windows using a brush.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WBrushWindowDrawer.cpp,v $
 *		Revision 1.4  2001/02/21 08:15:24  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:29:47  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:05:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDrawHelper.h>

#include <IBrushDrawer.h>
#include <IWindow.h>
#include <XBrush.h>
#include <XDrawContext.h>
#include <XExceptions.h>
#include <XRGBColor.h>
#include <XShapes.h>

namespace Whisper {


// ===================================================================================
//	class XBrushWindowDrawer
//!		IDraw interface that draws windows using a brush.
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
						
//-----------------------------------
//	Member Data
//
private:
	XBrush mActiveBrush;
	XBrush mInactiveBrush;
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
	IBrushDrawer::DoSetBoss(boss);
	IDrawHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XBrushWindowDrawer::SetBrushes
//
//---------------------------------------------------------------
void XBrushWindowDrawer::SetBrushes(const XBrush& active, const XBrush& inactive)
{
	mActiveBrush = active;
	mInactiveBrush = inactive; 
}


//---------------------------------------------------------------
//
// XBrushWindowDrawer::SetColors
//
//---------------------------------------------------------------
void XBrushWindowDrawer::SetColors(const XRGBColor& active, const XRGBColor& inactive)
{
	mActiveBrush = active;
	mInactiveBrush = inactive; 
}


//---------------------------------------------------------------
//
// XBrushWindowDrawer::OnDraw
//
//---------------------------------------------------------------
void XBrushWindowDrawer::OnDraw(XDrawContext& context, const XRegion& dirtyRgn, bool isActive)
{
	if (isActive)
		XRegionShape::Fill(context, dirtyRgn, mActiveBrush);
	else
		XRegionShape::Fill(context, dirtyRgn, mInactiveBrush);
}


}	// namespace Whisper
