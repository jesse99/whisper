/*
 *  File:       XDrawers.cpp
 *  Summary:   	Some standard IDraw implementations.
 *  Written by: Jesse Jones
 *
 *	Classes:	XBrushDrawer	- draws using a user defined brush
 *				XNullDrawer		- draws nothing (but allows draw extensions to draw)
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDrawers.cpp,v $
 *		Revision 1.4  2001/01/22 02:02:07  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:35:05  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:33:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDrawHelper.h>

#include <IBrushDrawer.h>
#include <XShapes.h>

namespace Whisper {


// ===================================================================================
//	class XBrushDrawer
//!		IDraw implementation that uses a user defined brush.
// ===================================================================================
class XBrushDrawer : public IDrawHelper, public IBrushDrawer {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XBrushDrawer();
	
						XBrushDrawer(XBoss* boss);
						
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

DEFINE_INTERFACE_FACTORY(XBrushDrawer)

//---------------------------------------------------------------
//
// XBrushDrawer::~XBrushDrawer
//
//---------------------------------------------------------------
XBrushDrawer::~XBrushDrawer()
{
}


//---------------------------------------------------------------
//
// XBrushDrawer::XBrushDrawer
//
//---------------------------------------------------------------
XBrushDrawer::XBrushDrawer(XBoss* boss)
{	
	IDrawHelper::DoSetBoss(boss);
	IBrushDrawer::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XBrushDrawer::SetBrushes
//
//---------------------------------------------------------------
void XBrushDrawer::SetBrushes(const XBrush& active, const XBrush& inactive)
{
	mActiveBrush = active;
	mInactiveBrush = inactive; 
}


//---------------------------------------------------------------
//
// XBrushDrawer::SetColors
//
//---------------------------------------------------------------
void XBrushDrawer::SetColors(const XRGBColor& active, const XRGBColor& inactive)
{
	mActiveBrush = active;
	mInactiveBrush = inactive; 
}


//---------------------------------------------------------------
//
// XBrushDrawer::OnDraw
//
//---------------------------------------------------------------
void XBrushDrawer::OnDraw(XDrawContext& context, const XRegion& dirtyRgn, bool isActive)
{
	if (isActive)
		XRegionShape::Fill(context, dirtyRgn, mActiveBrush);
	else
		XRegionShape::Fill(context, dirtyRgn, mInactiveBrush);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XNullDrawer
//!		IDraw implementation that draws nothing (but allows draw extensions to draw).
// ===================================================================================
class XNullDrawer : public IDrawHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XNullDrawer();
	
						XNullDrawer(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);
};

DEFINE_INTERFACE_FACTORY(XNullDrawer)

//---------------------------------------------------------------
//
// XNullDrawer::~XNullDrawer
//
//---------------------------------------------------------------
XNullDrawer::~XNullDrawer()
{
}


//---------------------------------------------------------------
//
// XNullDrawer::XNullDrawer
//
//---------------------------------------------------------------
XNullDrawer::XNullDrawer(XBoss* boss)
{	
	IDrawHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XNullDrawer::OnDraw
//
//---------------------------------------------------------------
void XNullDrawer::OnDraw(XDrawContext& context, const XRegion& dirtyRgn, bool isActive)
{
	UNUSED(context);
	UNUSED(dirtyRgn);
	UNUSED(isActive);
}


}	// namespace Whisper
