/*
 *	File:		XDrawExtensions.cpp
 *	Function:	IDrawExtensionHelper implementations.
 *	Written by:	Jesse Jones
 *
 *	Classes:	XFillExtension
 *					XEraseExtension			- Erases the object.
 *				XStrokeExtension
 *					XLineLeftExtension		- Draws a line on the left side of the object.
 *					XLineRightExtension		- Draws a line on the right side of the object.
 *					XLineTopExtension		- Draws a line on the top side of the object.
 *					XLineBottomExtension	- Draws a line on the bottom side of the object.
 *					XFrameExtension			- Draws a rectangle around the object.
 *					XShadowedFrameExtension	- Draws a rectangle with a drop shadow around the object.
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XDrawExtensions.cpp,v $
 *		Revision 1.4  2001/02/07 04:58:11  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:35:29  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:34:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 8/05/99	JDJ		Added support for streaming.
 *		 <2>	 4/20/99	JDJ		Line and frame extension default to using a gray
 *									pen when their owner is disabled.
 *		 <1>	 1/03/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <IDrawExtensionHelper.h>

#include <IBrush.h>
#include <IPen.h>
#include <XShapes.h>

namespace Whisper {


// ===================================================================================
//	class XEraseExtension
//!		IDrawExtensionHelper implementation that erases the object.
// ===================================================================================
class XEraseExtension : public IDrawExtensionHelper {
			
public:	
						XEraseExtension(XBoss* boss);
						
	virtual void		OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled);
};

DEFINE_INTERFACE_FACTORY(XEraseExtension)

//---------------------------------------------------------------
//
// XEraseExtension::XEraseExtension
//
//---------------------------------------------------------------
XEraseExtension::XEraseExtension(XBoss* boss)
{
	IDrawExtensionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XEraseExtension::OnDraw
//
//---------------------------------------------------------------
void XEraseExtension::OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled)
{
	IBrushPtr brush(this);
	XRectShape::Fill(canvas, extent, enabled ? brush->GetEnabledBrush() : brush->GetDisabledBrush());
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XLineLeftExtension
//!		IDrawExtensionHelper implementation that draws a line on the left side of the object.
// ===================================================================================
class XLineLeftExtension : public IDrawExtensionHelper {

public:
						XLineLeftExtension(XBoss* boss);

	virtual void		OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled);
};

DEFINE_INTERFACE_FACTORY(XLineLeftExtension)

//---------------------------------------------------------------
//
// XLineLeftExtension::XLineLeftExtension
//
//---------------------------------------------------------------
XLineLeftExtension::XLineLeftExtension(XBoss* boss) 
{
	IDrawExtensionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XLineLeftExtension::OnDraw
//
//---------------------------------------------------------------
void XLineLeftExtension::OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled)
{
	IPenPtr pp(this);
	XPen pen = enabled ? pp->GetEnabledPen() : pp->GetDisabledPen();
	
	XLineShape::Draw(canvas, XPoint(extent.left, extent.top), XPoint(extent.left, extent.bottom), pen);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XLineRightExtension
//!		IDrawExtensionHelper implementation that draws a line on the right side of the object.
// ===================================================================================
class XLineRightExtension : public IDrawExtensionHelper {

public:
						XLineRightExtension(XBoss* boss);

	virtual void		OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled);
};

DEFINE_INTERFACE_FACTORY(XLineRightExtension)

//---------------------------------------------------------------
//
// XLineRightExtension::XLineRightExtension
//
//---------------------------------------------------------------
XLineRightExtension::XLineRightExtension(XBoss* boss) 
{
	IDrawExtensionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XLineRightExtension::OnDraw
//
//---------------------------------------------------------------
void XLineRightExtension::OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled)
{
	IPenPtr pp(this);
	XPen pen = enabled ? pp->GetEnabledPen() : pp->GetDisabledPen();
	
	XLineShape::Draw(canvas, XPoint(extent.right - 1, extent.top), XPoint(extent.right - 1, extent.bottom), pen);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XLineTopExtension
//!		IDrawExtensionHelper implementation that draws a line on the top side of the object.
// ===================================================================================
class XLineTopExtension : public IDrawExtensionHelper {

public:
						XLineTopExtension(XBoss* boss);

	virtual void		OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled);
};

DEFINE_INTERFACE_FACTORY(XLineTopExtension)

//---------------------------------------------------------------
//
// XLineTopExtension::XLineTopExtension
//
//---------------------------------------------------------------
XLineTopExtension::XLineTopExtension(XBoss* boss) 
{
	IDrawExtensionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XLineTopExtension::OnDraw
//
//---------------------------------------------------------------
void XLineTopExtension::OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled)
{
	IPenPtr pp(this);
	XPen pen = enabled ? pp->GetEnabledPen() : pp->GetDisabledPen();
	
	XLineShape::Draw(canvas, XPoint(extent.left, extent.top), XPoint(extent.right, extent.top), pen);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XLineBottomExtension
//!		IDrawExtensionHelper implementation that draws a line on the bottom side of the object.
// ===================================================================================
class XLineBottomExtension : public IDrawExtensionHelper {

public:
						XLineBottomExtension(XBoss* boss);

	virtual void		OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled);
};

DEFINE_INTERFACE_FACTORY(XLineBottomExtension)

//---------------------------------------------------------------
//
// XLineBottomExtension::XLineBottomExtension
//
//---------------------------------------------------------------
XLineBottomExtension::XLineBottomExtension(XBoss* boss) 
{
	IDrawExtensionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XLineBottomExtension::OnDraw
//
//---------------------------------------------------------------
void XLineBottomExtension::OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled)
{
	IPenPtr pp(this);
	XPen pen = enabled ? pp->GetEnabledPen() : pp->GetDisabledPen();
	
	XLineShape::Draw(canvas, XPoint(extent.left, extent.top), XPoint(extent.right, extent.bottom), pen);
//	XLineShape::Draw(canvas, XPoint(extent.left, extent.bottom - 1), XPoint(extent.right, extent.bottom - 1), pen);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFrameExtension
//!		IDrawExtensionHelper implementation that draws a rectangle around the object.
// ===================================================================================
class XFrameExtension : public IDrawExtensionHelper {

public:
						XFrameExtension(XBoss* boss);

	virtual void		OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled);
};

DEFINE_INTERFACE_FACTORY(XFrameExtension)

//---------------------------------------------------------------
//
// XFrameExtension::XFrameExtension
//
//---------------------------------------------------------------
XFrameExtension::XFrameExtension(XBoss* boss) 
{
	IDrawExtensionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XFrameExtension::OnDraw
//
//---------------------------------------------------------------
void XFrameExtension::OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled)
{
	IPenPtr pp(this);
	XPen pen = enabled ? pp->GetEnabledPen() : pp->GetDisabledPen();
	
	XRectShape::Frame(canvas, extent, pen);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XShadowedFrameExtension
//!		IDrawExtensionHelper implementation that draws a rectangle with a drop shadow around the object.
// ===================================================================================
class XShadowedFrameExtension : public IDrawExtensionHelper {

public:
						XShadowedFrameExtension(XBoss* boss);

	virtual void		OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled);
};

DEFINE_INTERFACE_FACTORY(XShadowedFrameExtension)

//---------------------------------------------------------------
//
// XShadowedFrameExtension::XShadowedFrameExtension
//
//---------------------------------------------------------------
XShadowedFrameExtension::XShadowedFrameExtension(XBoss* boss) 
{
	IDrawExtensionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XShadowedFrameExtension::OnDraw
//
//---------------------------------------------------------------
void XShadowedFrameExtension::OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled)
{
	UNUSED(enabled);
	
	XRect area(extent.left, extent.top, extent.right - 1, extent.bottom - 1);

	IPenPtr pp(this);
	XPen pen = pp->GetEnabledPen();
	
	XRectShape::Frame(canvas, area, pen);
	XLineShape::Draw(canvas, XPoint(area.left + 1, area.bottom), XPoint(area.right, area.bottom), pen);
	XLineShape::Draw(canvas, XPoint(area.right, area.bottom), XPoint(area.right, area.top + 1), pen);
}


}	// namespace Whisper

