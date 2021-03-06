/*
 *  File:       WShapes.cpp
 *  Summary:	Classes that draw into an XDrawContext.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WShapes.cpp,v $
 *		Revision 1.8  2001/04/27 09:19:35  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.7  2001/04/21 03:33:50  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/03/03 11:10:02  jesjones
 *		Added XPixMapShape::Draw method that takes a clip region.
 *		
 *		Revision 1.5  2001/02/25 08:23:36  jesjones
 *		Fixed an off-by-one-error in XLineShapeDraw (XPoint, XSize).
 *		
 *		Revision 1.4  2000/12/10 04:07:59  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/20 05:42:00  jesjones
 *		Added std:: to a few function calls.
 *		
 *		Revision 1.2  2000/11/09 12:11:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	  4/07/00	JDJ		Tweaked XLineShape::Draw (XDrawContext, XPoint, XSize, XPen)
 *									so that it doesn't draw an extra pixel.
 *		 <5>	  6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <4>	  5/13/99	JDJ		Fill and Invert methods no longer subtract one
 *									from right and bottom of the rectangle.
 *		 <3>	  3/07/99	JDJ		Removed XBoundedShape.
 *		 <2>	  1/15/99	JDJ		XRegionShape::Frame uses a brush instead of a pen.
 *		 <1>	 12/11/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XShapes.h>

#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XPixMap.h>
#include <XPointer.h>
#include <XSelectAttribute.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ComputeArcPoint
//
//---------------------------------------------------------------
static XPoint ComputeArcPoint(const XRect& rect, double angle)
{
	int32 middleX = (rect.right + rect.left)/2;
	int32 middleY = (rect.bottom + rect.top)/2;

	double halfX = rect.GetWidth()/2.0;
	double halfY = rect.GetHeight()/2.0;

	double radians = kPi*angle/180.0;

	int32 shortX = (int32) (halfX * std::sin(radians));
	int32 shortY = (int32) (-halfY * std::cos(radians));

	return XPoint(shortX + middleX, shortY + middleY);
}


//---------------------------------------------------------------
//
// ComputeArcPoints
//
//---------------------------------------------------------------
static void ComputeArcPoints(const XRect& rect, int32 startAngle, int32 arcAngle, XPoint& startPoint, XPoint& endPoint)
{
	PRECONDITION(arcAngle >= 0);
		
	int32 angle1 = startAngle;
		
	// Normalize the start angle
	angle1 = angle1 % 360;
	if (angle1 < 0)
		angle1 += 360;
		
	// Compute and normalize the end angle
	int32 angle2 = angle1 + arcAngle;
	if (arcAngle < 0) {
		int32 temp = angle1;
		angle1 = angle2;
		angle2 = temp;
	}

	endPoint   = ComputeArcPoint(rect, angle1);		// on Windows, arc and pie are drawn counterclockwise
	startPoint = ComputeArcPoint(rect, angle2);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XLineShape
// ===================================================================================

//---------------------------------------------------------------
//
// XLineShape::~XLineShape
//
//---------------------------------------------------------------
XLineShape::~XLineShape()
{
}


//---------------------------------------------------------------
//
// XLineShape::XLineShape
//
//---------------------------------------------------------------
XLineShape::XLineShape(const XPoint& pt1, const XPoint& pt2, const XPen& pen)
{
	mPt1 = pt1;
	mPt2 = pt2;
	mPen = pen;
}


//---------------------------------------------------------------
//
// XLineShape::Clone
//
//---------------------------------------------------------------
XLineShape* XLineShape::Clone() const
{
	return new XLineShape(mPt1, mPt2, mPen);
}


//---------------------------------------------------------------
//
// XLineShape::Draw (XDrawContext, XPoint, XPoint, XPen) [static]
//
//---------------------------------------------------------------
void XLineShape::Draw(XDrawContext& context, const XPoint& pt1, const XPoint& pt2, const XPen& pen)
{
	XSelectAttribute select(context, pen);
	
	int32 succeeded = ::MoveToEx(context.GetOSContext(), pt1.x, pt1.y, nil);
	ThrowIf(!succeeded);
	
	succeeded = ::LineTo(context.GetOSContext(), pt2.x, pt2.y);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XLineShape::Draw (XDrawContext, XPoint, XSize, XPen)	[static]
//
//---------------------------------------------------------------
void XLineShape::Draw(XDrawContext& context, const XPoint& pt, const XSize& delta, const XPen& pen)
{
	XSelectAttribute select(context, pen);
	
	int32 succeeded = ::MoveToEx(context.GetOSContext(), pt.x, pt.y, nil);
	ThrowIf(!succeeded);
	
	succeeded = ::LineTo(context.GetOSContext(), pt.x + delta.width, pt.y + delta.height);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XLineShape::SetPen
//
//---------------------------------------------------------------
void XLineShape::SetPen(const XPen& pen)
{
	mPen = pen;
}


//---------------------------------------------------------------
//
// XLineShape::Draw
//
//---------------------------------------------------------------
void XLineShape::Draw(XDrawContext& context) const
{
	XLineShape::Draw(context, mPt1, mPt2, mPen);
}


//---------------------------------------------------------------
//
// XLineShape::GetBounds
//
//---------------------------------------------------------------
XRect XLineShape::GetBounds() const
{
	XRect bounds;
	
	bounds.left   = Min(mPt1.x, mPt2.x);
	bounds.right  = Max(mPt1.x, mPt2.x);
	bounds.top    = Min(mPt1.y, mPt2.y);
	bounds.bottom = Max(mPt1.y, mPt2.y);
	
	return bounds;
}


//---------------------------------------------------------------
//
// XLineShape::MoveBy
//
//---------------------------------------------------------------
void XLineShape::MoveBy(const XSize& delta)
{
	mPt1 += delta;
	mPt2 += delta;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XContourShape
// ===================================================================================

//---------------------------------------------------------------
//
// XContourShape::~XContourShape
//
//---------------------------------------------------------------
XContourShape::~XContourShape()
{
}

					
//---------------------------------------------------------------
//
// XContourShape::XContourShape (XPen)
//
//---------------------------------------------------------------
XContourShape::XContourShape(const XPen& pen) : mPen(pen), mBrush(kBlackBrush)
{
	mFraming = true;
}


//---------------------------------------------------------------
//
// XContourShape::XContourShape (XBrush)
//
//---------------------------------------------------------------
XContourShape::XContourShape(const XBrush& brush) : mBrush(brush)
{
	mFraming = false;
}


//---------------------------------------------------------------
//
// XContourShape::StartFraming
//
//---------------------------------------------------------------
void XContourShape::StartFraming(bool frame)			
{
	mFraming = frame;
}


//---------------------------------------------------------------
//
// XContourShape::SetPen
//
//---------------------------------------------------------------
void XContourShape::SetPen(const XPen& pen)
{
	mPen = pen;
}


//---------------------------------------------------------------
//
// XContourShape::SetBrush
//
//---------------------------------------------------------------
void XContourShape::SetBrush(const XBrush& brush)
{
	mBrush = brush;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XRectShape
// ===================================================================================

//---------------------------------------------------------------
//
// XRectShape::~XRectShape
//
//---------------------------------------------------------------
XRectShape::~XRectShape()
{
}


//---------------------------------------------------------------
//
// XRectShape::XRectShape (XRect, XPen)
//
//---------------------------------------------------------------
XRectShape::XRectShape(const XRect& rect, const XPen& pen) : XContourShape(pen), mBounds(rect)
{
}


//---------------------------------------------------------------
//
// XRectShape::XRectShape (XRect, XBrush)
//
//---------------------------------------------------------------
XRectShape::XRectShape(const XRect& rect, const XBrush& brush) : XContourShape(brush), mBounds(rect)
{
}


//---------------------------------------------------------------
//
// XRectShape::Clone
//
//---------------------------------------------------------------
XRectShape* XRectShape::Clone() const
{
	if (mFraming)
		return new XRectShape(mBounds, mPen);
	else
		return new XRectShape(mBounds, mBrush);
}


//---------------------------------------------------------------
//
// XRectShape::Frame									[static]
//
//---------------------------------------------------------------
void XRectShape::Frame(XDrawContext& context, const XRect& rect, const XPen& pen)
{
	XSelectAttribute select(context, pen);

	HBRUSH nullBrush = (HBRUSH) ::GetStockObject(NULL_BRUSH);
	ThrowIfBadHandle(nullBrush);
	
	HGDIOBJ oldBrush = ::SelectObject(context.GetOSContext(), nullBrush);
	ThrowIf(oldBrush == nil || (uint32) oldBrush == GDI_ERROR);

	int32 succeeded = ::Rectangle(context.GetOSContext(), rect.left, rect.top, rect.right, rect.bottom);	
	(void) ::SelectObject(context.GetOSContext(), oldBrush);
	
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XRectShape::Fill										[static]
//
//---------------------------------------------------------------
void XRectShape::Fill(XDrawContext& context, const XRect& rect, const XBrush& brush)
{	
	XSelectAttribute select(context, brush);
	
	int32 succeeded = ::FillRect(context.GetOSContext(), rect, brush);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XRectShape::Invert									[static]
//
//---------------------------------------------------------------
void XRectShape::Invert(XDrawContext& context, const XRect& rect)
{	
	int32 succeeded = ::InvertRect(context.GetOSContext(), rect);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XRectShape::Hilite									[static]
//
//---------------------------------------------------------------
void XRectShape::Hilite(XDrawContext& context, const XRect& rect)
{	
	int32 succeeded = ::InvertRect(context.GetOSContext(), rect);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XRectShape::Draw
//
//---------------------------------------------------------------
void XRectShape::Draw(XDrawContext& context) const
{
	if (mFraming)
		XRectShape::Frame(context, mBounds, mPen);
	else
		XRectShape::Fill(context, mBounds, mBrush);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XRoundRectShape
// ===================================================================================

//---------------------------------------------------------------
//
// XRoundRectShape::~XRoundRectShape
//
//---------------------------------------------------------------
XRoundRectShape::~XRoundRectShape()
{
}


//---------------------------------------------------------------
//
// XRoundRectShape::XRoundRectShape (XRect, XSize, XPen)
//
//---------------------------------------------------------------
XRoundRectShape::XRoundRectShape(const XRect& rect, const XSize& ovalSize, const XPen& pen) : XContourShape(pen), mBounds(rect)
{
	PRECONDITION(ovalSize.width >= 0);
	PRECONDITION(ovalSize.height >= 0);

	mOvalSize = ovalSize;
}


//---------------------------------------------------------------
//
// XRoundRectShape::XRoundRectShape (XRect, XSize, XBrush)
//
//---------------------------------------------------------------
XRoundRectShape::XRoundRectShape(const XRect& rect, const XSize& ovalSize, const XBrush& brush) : XContourShape(brush), mBounds(rect)
{
	PRECONDITION(ovalSize.width >= 0);
	PRECONDITION(ovalSize.height >= 0);

	mOvalSize = ovalSize;
}


//---------------------------------------------------------------
//
// XRoundRectShape::Clone
//
//---------------------------------------------------------------
XRoundRectShape* XRoundRectShape::Clone() const
{
	if (mFraming)
		return new XRoundRectShape(mBounds, mOvalSize, mPen);
	else
		return new XRoundRectShape(mBounds, mOvalSize, mBrush);
}


//---------------------------------------------------------------
//
// XRoundRectShape::Frame								[static]
//
//---------------------------------------------------------------
void XRoundRectShape::Frame(XDrawContext& context, const XRect& rect, const XSize& ovalSize, const XPen& pen)
{
	XSelectAttribute select(context, pen);
	
	HBRUSH nullBrush = (HBRUSH) ::GetStockObject(NULL_BRUSH);
	ThrowIfBadHandle(nullBrush);
	
	HGDIOBJ oldBrush = ::SelectObject(context.GetOSContext(), nullBrush);
	ThrowIf(oldBrush == nil || (uint32) oldBrush == GDI_ERROR);
	
	int32 succeeded = ::RoundRect(context.GetOSContext(), rect.left, rect.top, rect.right, rect.bottom, ovalSize.width, ovalSize.height);	
	(void) ::SelectObject(context.GetOSContext(), oldBrush);
	
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XRoundRectShape::Fill								[static]
//
//---------------------------------------------------------------
void XRoundRectShape::Fill(XDrawContext& context, const XRect& rect, const XSize& ovalSize, const XBrush& brush)
{
	XSelectAttribute select(context, brush);
	
	XRegion rgn(rect, ovalSize.width, ovalSize.height);
	
	int32 succeeded = ::FillRgn(context.GetOSContext(), rgn, brush);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XRoundRectShape::Invert								[static]
//
//---------------------------------------------------------------
void XRoundRectShape::Invert(XDrawContext& context, const XRect& rect, const XSize& ovalSize)
{	
	XRegion rgn(rect, ovalSize.width, ovalSize.height);

	int32 succeeded = ::InvertRgn(context.GetOSContext(), rgn);
	ThrowIf(!succeeded);
}

	
//---------------------------------------------------------------
//
// XRoundRectShape::SetOvalSize
//
//---------------------------------------------------------------
void XRoundRectShape::SetOvalSize(const XSize& ovalSize)
{
	PRECONDITION(ovalSize.width >= 0);
	PRECONDITION(ovalSize.height >= 0);

	mOvalSize = ovalSize;
}


//---------------------------------------------------------------
//
// XRoundRectShape::Draw
//
//---------------------------------------------------------------
void XRoundRectShape::Draw(XDrawContext& context) const
{
	if (mFraming)
		XRoundRectShape::Frame(context, mBounds, mOvalSize, mPen);
	else
		XRoundRectShape::Fill(context, mBounds, mOvalSize, mBrush);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XOvalShape
// ===================================================================================

//---------------------------------------------------------------
//
// XOvalShape::~XOvalShape
//
//---------------------------------------------------------------
XOvalShape::~XOvalShape()
{
}


//---------------------------------------------------------------
//
// XOvalShape::XOvalShape (XRect, XPen)
//
//---------------------------------------------------------------
XOvalShape::XOvalShape(const XRect& rect, const XPen& pen) : XContourShape(pen), mBounds(rect)
{
}

						
//---------------------------------------------------------------
//
// XOvalShape::XOvalShape (XRect, XBrush)
//
//---------------------------------------------------------------
XOvalShape::XOvalShape(const XRect& rect, const XBrush& brush) : XContourShape(brush), mBounds(rect)
{
}

						
//---------------------------------------------------------------
//
// XOvalShape::Clone
//
//---------------------------------------------------------------
XOvalShape* XOvalShape::Clone() const
{
	if (mFraming)
		return new XOvalShape(mBounds, mPen);
	else
		return new XOvalShape(mBounds, mBrush);
}


//---------------------------------------------------------------
//
// XOvalShape::Frame									[static]
//
//---------------------------------------------------------------
void XOvalShape::Frame(XDrawContext& context, const XRect& rect, const XPen& pen)
{
	XSelectAttribute select(context, pen);
	
	HBRUSH nullBrush = (HBRUSH) ::GetStockObject(NULL_BRUSH);
	ThrowIfBadHandle(nullBrush);
	
	HGDIOBJ oldBrush = ::SelectObject(context.GetOSContext(), nullBrush);
	ThrowIf(oldBrush == nil || (uint32) oldBrush == GDI_ERROR);
	
	int32 succeeded = ::Ellipse(context.GetOSContext(), rect.left, rect.top, rect.right, rect.bottom);	
	(void) ::SelectObject(context.GetOSContext(), oldBrush);
	
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XOvalShape::Fill										[static]
//
//---------------------------------------------------------------
void XOvalShape::Fill(XDrawContext& context, const XRect& rect, const XBrush& brush)
{
	XSelectAttribute select(context, brush);
	
	XPoint center(rect.left + rect.GetWidth()/2, rect.top + rect.GetHeight()/2);
	XRegion rgn(center, rect.GetWidth(), rect.GetHeight());

	int32 succeeded = ::FillRgn(context.GetOSContext(), rgn, brush);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XOvalShape::Invert									[static]
//
//---------------------------------------------------------------
void XOvalShape::Invert(XDrawContext& context, const XRect& rect)
{	
	XPoint center(rect.left + rect.GetWidth()/2, rect.top + rect.GetHeight()/2);
	XRegion rgn(center, rect.GetWidth(), rect.GetHeight());

	int32 succeeded = ::InvertRgn(context.GetOSContext(), rgn);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XOvalShape::Draw
//
//---------------------------------------------------------------
void XOvalShape::Draw(XDrawContext& context) const
{
	if (mFraming)
		XOvalShape::Frame(context, mBounds, mPen);
	else
		XOvalShape::Fill(context, mBounds, mBrush);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XArcShape
// ===================================================================================

//---------------------------------------------------------------
//
// XArcShape::~XArcShape
//
//---------------------------------------------------------------
XArcShape::~XArcShape()
{
}


//---------------------------------------------------------------
//
// XArcShape::XArcShape (XRect, int32, int32, XPen)
//
//---------------------------------------------------------------
XArcShape::XArcShape(const XRect& rect, int32 startAngle, int32 arcAngle, const XPen& pen) : XContourShape(pen), mBounds(rect)
{
	PRECONDITION(arcAngle >= 0);

	mStartAngle = startAngle;
	mArcAngle   = arcAngle;
}

						
//---------------------------------------------------------------
//
// XArcShape::XArcShape (XRect, int32, int32, XBrush)
//
//---------------------------------------------------------------
XArcShape::XArcShape(const XRect& rect, int32 startAngle, int32 arcAngle, const XBrush& brush) : XContourShape(brush), mBounds(rect)
{
	PRECONDITION(arcAngle >= 0);

	mStartAngle = startAngle;
	mArcAngle   = arcAngle;
}

						
//---------------------------------------------------------------
//
// XArcShape::Clone
//
//---------------------------------------------------------------
XArcShape* XArcShape::Clone() const
{
	if (mFraming)
		return new XArcShape(mBounds, mStartAngle, mArcAngle, mPen);
	else
		return new XArcShape(mBounds, mStartAngle, mArcAngle, mBrush);
}


//---------------------------------------------------------------
//
// XArcShape::Frame										[static]
//
//---------------------------------------------------------------
void XArcShape::Frame(XDrawContext& context, const XRect& rect, int32 startAngle, int32 arcAngle, const XPen& pen)
{
	PRECONDITION(arcAngle >= 0);
	
	XSelectAttribute select(context, pen);
	
	XPoint startPoint, endPoint;
	ComputeArcPoints(rect, startAngle, arcAngle, startPoint, endPoint);

	int32 succeeded = ::Arc(context.GetOSContext(), rect.left, rect.top, rect.right, rect.bottom, startPoint.x, startPoint.y, endPoint.x, endPoint.y);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XArcShape::Fill										[static]
//
//---------------------------------------------------------------
void XArcShape::Fill(XDrawContext& context, const XRect& rect, int32 startAngle, int32 arcAngle, const XBrush& brush)
{
	PRECONDITION(arcAngle >= 0);

	XSelectAttribute select(context, brush);
	
	HPEN nullPen = (HPEN) ::GetStockObject(NULL_PEN);
	ThrowIfBadHandle(nullPen);
	
	HPEN oldPen = (HPEN) ::SelectObject(context.GetOSContext(), nullPen);
	ThrowIf(oldPen == nil || (uint32) oldPen == GDI_ERROR);

	int32 radius = Max(rect.GetWidth(), rect.GetHeight());	// radial can be outside rect so we'll use take the easy route and use a circle
	XPoint center = rect.GetCenter();

	int32 startX = center.x + numeric_cast<int32>(radius*std::sin(startAngle));
	int32 startY = center.y + numeric_cast<int32>(radius*std::cos(startAngle));
	
	int32 endX = center.x + numeric_cast<int32>(radius*std::sin(startAngle + arcAngle));
	int32 endY = center.y + numeric_cast<int32>(radius*std::cos(startAngle + arcAngle));
	
	int32 succeeded = ::Pie(context.GetOSContext(), rect.left, rect.top, rect.right, rect.bottom, startX, startY, endX, endY);	
	(void) ::SelectObject(context.GetOSContext(), oldPen);
	
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XArcShape::Invert									[static]
//
//---------------------------------------------------------------
void XArcShape::Invert(XDrawContext& context, const XRect& rect, int32 startAngle, int32 arcAngle)
{	
	PRECONDITION(arcAngle >= 0);
	
	HPEN nullPen = (HPEN) ::GetStockObject(NULL_PEN);
	ThrowIfBadHandle(nullPen);
	
	HPEN oldPen = (HPEN) ::SelectObject(context.GetOSContext(), nullPen);
	ThrowIf(oldPen == nil || (uint32) oldPen == GDI_ERROR);
	
	int32 oldMode = ::SetROP2(context.GetOSContext(), R2_XORPEN);
	
	int32 radius = Max(rect.GetWidth(), rect.GetHeight());	// radial can be outside rect so we'll use take the easy route and use a circle
	XPoint center = rect.GetCenter();

	int32 startX = center.x + numeric_cast<int32>(radius*std::sin(startAngle));
	int32 startY = center.y + numeric_cast<int32>(radius*std::cos(startAngle));
	
	int32 endX = center.x + numeric_cast<int32>(radius*std::sin(startAngle + arcAngle));
	int32 endY = center.y + numeric_cast<int32>(radius*std::cos(startAngle + arcAngle));
	
	int32 succeeded = ::Pie(context.GetOSContext(), rect.left, rect.top, rect.right, rect.bottom, startX, startY, endX, endY);	
	(void) ::SelectObject(context.GetOSContext(), oldPen);
	(void) ::SetROP2(context.GetOSContext(), oldMode);
	
	ThrowIf(!succeeded);
}

	
//---------------------------------------------------------------
//
// XArcShape::SetStartAngle
//
//---------------------------------------------------------------
void XArcShape::SetStartAngle(int32 angle)
{
	mStartAngle = angle;
}


//---------------------------------------------------------------
//
// XArcShape::SetArcAngle
//
//---------------------------------------------------------------
void XArcShape::SetArcAngle(int32 angle)
{
	PRECONDITION(angle >= 0);

	mArcAngle = angle;
}


//---------------------------------------------------------------
//
// XArcShape::Draw
//
//---------------------------------------------------------------
void XArcShape::Draw(XDrawContext& context) const
{
	if (mFraming)
		XArcShape::Frame(context, mBounds, mStartAngle, mArcAngle, mPen);
	else
		XArcShape::Fill(context, mBounds, mStartAngle, mArcAngle, mBrush);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XRegionShape
// ===================================================================================

//---------------------------------------------------------------
//
// XRegionShape::~XRegionShape
//
//---------------------------------------------------------------
XRegionShape::~XRegionShape()
{
}


//---------------------------------------------------------------
//
// XRegionShape::XRegionShape (XRegion, XBrush, bool)
//
//---------------------------------------------------------------
XRegionShape::XRegionShape(const XRegion& rgn, const XBrush& brush, bool framing) : XContourShape(brush)
{
	mRegion = rgn;
	mFraming = framing;
}

						
//---------------------------------------------------------------
//
// XRegionShape::Clone
//
//---------------------------------------------------------------
XRegionShape* XRegionShape::Clone() const
{
	return new XRegionShape(mRegion, mBrush, mFraming);
}


//---------------------------------------------------------------
//
// XRegionShape::Frame									[static]
//
//---------------------------------------------------------------
void XRegionShape::Frame(XDrawContext& context, const XRegion& rgn, const XBrush& brush, int32 width, int32 height)
{
#if 1
	if (!rgn.IsEmpty()) {						// FrameRgn returns an error code for empty regions (with Win98)...
		XSelectAttribute select(context, brush);

		int32 succeeded = ::FrameRgn(context.GetOSContext(), rgn, brush, width, height);
		ThrowIf(!succeeded);
	}

#else
	EXTLOGPEN info;
	int32 bytes = ::GetObject(pen, sizeof(info), &info);
	ThrowIf(bytes == 0);

	HBRUSH brush = nil;
	if (info.elpBrushStyle == BS_SOLID)
		brush = CreateSolidBrush(info.elpColor);	// $$$ should cache brush
	else if (info.elpBrushStyle == BS_HATCHED)
		brush = CreateHatchBrush(info.elpHatch, info.elpColor);	
	else if (info.elpBrushStyle == BS_DIBPATTERN)
		brush = CreatePatternBrush((HBITMAP) info.elpHatch);	
	else if (info.elpBrushStyle == BS_DIBPATTERNPT)
		brush = CreateDIBPatternBrushPt((const BITMAPINFO*) info.elpHatch, info.elpColor);	
	else 
		throw std::range_error("Can only frame a region with solid, hatched, or bitmap pens");
	
	ThrowIf(brush == nil);

	int32 newMode = pen.GetMode();
	int32 oldMode = ::SetROP2(context.GetOSContext(), newMode);

	int32 succeeded = ::FrameRgn(context.GetOSContext(), rgn, brush, info.elpWidth, info.elpWidth);
	(void) ::SetROP2(context.GetOSContext(), oldMode);

	::DeleteObject(brush);

	ThrowIf(!succeeded);
#endif
}


//---------------------------------------------------------------
//
// XRegionShape::Fill									[static]
//
//---------------------------------------------------------------
void XRegionShape::Fill(XDrawContext& context, const XRegion& rgn, const XBrush& brush)
{
	XSelectAttribute select(context, brush);

	int32 succeeded = ::FillRgn(context.GetOSContext(), rgn, brush);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XRegionShape::Invert									[static]
//
//---------------------------------------------------------------
void XRegionShape::Invert(XDrawContext& context, const XRegion& rgn)
{	
	int32 succeeded = ::InvertRgn(context.GetOSContext(), rgn);
	ThrowIf(!succeeded);
}

	
//---------------------------------------------------------------
//
// XRegionShape::SetRegion
//
//---------------------------------------------------------------
void XRegionShape::SetRegion(const XRegion& rgn)
{
	mRegion = rgn;
}


//---------------------------------------------------------------
//
// XRegionShape::Draw
//
//---------------------------------------------------------------
void XRegionShape::Draw(XDrawContext& context) const
{
	if (mFraming)
		XRegionShape::Frame(context, mRegion, mBrush);
	else
		XRegionShape::Fill(context, mRegion, mBrush);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XTextShape
// ===================================================================================

//---------------------------------------------------------------
//
// XTextShape::~XTextShape
//
//---------------------------------------------------------------
XTextShape::~XTextShape()
{
}


//---------------------------------------------------------------
//
// XTextShape::XTextShape (wstring, XPoint, XFont)
//
//---------------------------------------------------------------
XTextShape::XTextShape(const std::wstring& text, const XPoint& where, const XFont& font) : mText(text), mFont(font)
{
	SFontMetrics metrics = mFont.GetMetrics();

	mBounds.left   = where.x;
	mBounds.right  = 32000;
	mBounds.top    = where.y - metrics.ascent;
	mBounds.bottom = where.y + metrics.descent;

	mAlign = kLeftTopAlign;

	mWordWrap = false;
	
	mBounds = kZeroRect;
}


//---------------------------------------------------------------
//
// XTextShape::XTextShape (wstring, XRect, XFont, ETextAlignment, bool)
//
//---------------------------------------------------------------
XTextShape::XTextShape(const std::wstring& text, const XRect& bounds, const XFont& font, ETextAlignment align, bool wordWrap) : mBounds(bounds), mText(text), mFont(font)
{
	mAlign = align;
	mWordWrap = wordWrap;
}


//---------------------------------------------------------------
//
// XTextShape::Clone
//
//---------------------------------------------------------------
XTextShape* XTextShape::Clone() const
{
	return new XTextShape(mText, mBounds, mFont, mAlign, mWordWrap);
}


//---------------------------------------------------------------
//
// XTextShape::Draw (XDrawContext, wstring, XPoint, XFont) [static]
//
//---------------------------------------------------------------
void XTextShape::Draw(XDrawContext& context, const std::wstring& text, const XPoint& where, const XFont& font)
{
	SFontMetrics metrics = font.GetMetrics();

	XRect bounds;

	bounds.left   = where.x;
	bounds.right  = 32000;
	bounds.top    = where.y - metrics.ascent;
	bounds.bottom = where.y + metrics.descent;
	
	XTextShape::Draw(context, text, bounds, font, kLeftTopAlign, false);
}

	
//---------------------------------------------------------------
//
// XTextShape::Draw (XDrawContext, wstring, XRect, XFont, ETextAlignment, bool) [static]
//
//---------------------------------------------------------------
void XTextShape::Draw(XDrawContext& context, const std::wstring& text, const XRect& bounds, const XFont& font, ETextAlignment align, bool wordWrap)
{
	XSelectAttribute select(context, font);
	
	uint32 flags = DT_EXTERNALLEADING + DT_NOPREFIX;
	if (wordWrap)
		flags += DT_WORDBREAK;

	switch (align) {
		case kLeftBottomAlign:
			flags += DT_LEFT + DT_BOTTOM;
			break;
			
		case kLeftTopAlign:
			flags = flags + DT_LEFT + DT_TOP;
			break;
			
		case kCenterBottomAlign:
			flags += DT_CENTER + DT_BOTTOM;
			break;
			
		case kCenterTopAlign:
			flags += DT_CENTER + DT_TOP;
			break;
			
		case kRightBottomAlign:
			flags += TA_RIGHT + DT_BOTTOM;
			break;

		case kRightTopAlign:
			flags += TA_RIGHT + DT_TOP;
			break;
					
		case kDefaultAlign:
			flags += DT_RTLREADING + DT_TOP;
			break;
			
		default:
			DEBUGSTR("Bad align flag in XTextShape::Draw");
	}				
	
	RECT temp = bounds.GetOSRect();
	if (WSystemInfo::HasUnicode())
		(void) ::DrawTextW(context.GetOSContext(), text.c_str(), numeric_cast<int32>(text.length()), &temp, flags);
	else
		(void) ::DrawTextA(context.GetOSContext(), ToPlatformStr(text).c_str(), numeric_cast<int32>(text.length()), &temp, flags);
}

	
//---------------------------------------------------------------
//
// XTextShape::SetText
//
//---------------------------------------------------------------
void XTextShape::SetText(const std::wstring& text)
{
	mText = text;
}


//---------------------------------------------------------------
//
// XTextShape::SetFont
//
//---------------------------------------------------------------
void XTextShape::SetFont(const XFont& font)
{
	mFont = font;
}


//---------------------------------------------------------------
//
// XTextShape::SetWrapping
//
//---------------------------------------------------------------
void XTextShape::SetWrapping(bool wrap)
{
	mWordWrap = wrap;
}


//---------------------------------------------------------------
//
// XTextShape::Draw
//
//---------------------------------------------------------------
void XTextShape::Draw(XDrawContext& context) const
{
	XTextShape::Draw(context, mText, mBounds, mFont, mAlign, mWordWrap);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPixMapShape
// ===================================================================================

//---------------------------------------------------------------
//
// XPixMapShape::~XPixMapShape
//
//---------------------------------------------------------------
XPixMapShape::~XPixMapShape()
{	
	delete mPixMap;
}


//---------------------------------------------------------------
//
// XPixMapShape::XPixMapShape (XPixMap*, XPoint, EDrawMode)
//
//---------------------------------------------------------------
XPixMapShape::XPixMapShape(XPixMap* takePixels, const XPoint& where, EDrawMode mode) : mDstRect(XRect(where, takePixels->GetSize()))
{
	PRECONDITION(takePixels != nil);
	
	mPixMap  = takePixels;
	mSrcRect = mPixMap->GetExtent();
	mMode    = mode;
}


//---------------------------------------------------------------
//
// XPixMapShape::XPixMapShape (XPixMap*, XRect, EDrawMode)
//
//---------------------------------------------------------------
XPixMapShape::XPixMapShape(XPixMap* takePixels, const XRect& dstRect, EDrawMode mode) : mDstRect(dstRect)
{
	PRECONDITION(takePixels != nil);
	
	mPixMap  = takePixels;
	mSrcRect = mPixMap->GetExtent();
	mMode    = mode;
}


//---------------------------------------------------------------
//
// XPixMapShape::XPixMapShape (XPixMap*, XRect, XRect, EDrawMode)
//
//---------------------------------------------------------------
XPixMapShape::XPixMapShape(XPixMap* takePixels, const XRect& srcRect, const XRect& dstRect, EDrawMode mode) : mDstRect(dstRect)
{
	PRECONDITION(takePixels != nil);
	
	mPixMap  = takePixels;
	mSrcRect = srcRect;
	mMode    = mode;
}


//---------------------------------------------------------------
//
// XPixMapShape::Clone
//
//---------------------------------------------------------------
XPixMapShape* XPixMapShape::Clone() const
{
	return new XPixMapShape(dynamic_cast<XPixMap*>(mPixMap->Clone()), mSrcRect, mDstRect, mMode);
}


//---------------------------------------------------------------
//
// XPixMapShape::Draw (XDrawContext, XPixMap, XPoint, EDrawMode) [static]
//
//---------------------------------------------------------------
void XPixMapShape::Draw(XDrawContext& context, const XPixMap& pixMap, const XPoint& where, EDrawMode mode)
{
	XRect srcRect = pixMap.GetExtent();
	XRect dstRect = XRect(where, srcRect.GetSize());
	
	XPixMapShape::Draw(context, pixMap, srcRect, dstRect, mode);
}

	
//---------------------------------------------------------------
//
// XPixMapShape::Draw (XDrawContext, XPixMap, XRect, EDrawMode)	[static]
//
//---------------------------------------------------------------
void XPixMapShape::Draw(XDrawContext& context, const XPixMap& pixMap, const XRect& dstRect, EDrawMode mode)
{
	XRect srcRect = pixMap.GetExtent();

	XPixMapShape::Draw(context, pixMap, srcRect, dstRect, mode);
}

	
//---------------------------------------------------------------
//
// XPixMapShape::Draw (XDrawContext, XPixMap, XRect, XRect, EDrawMode) [static]
//
//---------------------------------------------------------------
void XPixMapShape::Draw(XDrawContext& context, const XPixMap& pixMap, const XRect& srcRect, const XRect& dstRect, EDrawMode mode)
{		
	PRECONDITION(mode == kCopyMode);		// $$ the only other really useful mode is kTransparentMode which is unsupported by GDI...

	XLocker lock(pixMap);

	const BITMAPINFO* info = pixMap.GetInfo();

	int32 result = ::StretchDIBits(context.GetOSContext(),	// device context
								   dstRect.left,			// destination rect
								   dstRect.top,
								   dstRect.GetWidth(),
								   dstRect.GetHeight(),
								   srcRect.left,			// source rect
								   srcRect.top,
								   srcRect.GetWidth(),
								   srcRect.GetHeight(),
								   pixMap.GetBuffer(),		// bitmap bits
								   info,					// bitmap data
								   DIB_RGB_COLORS,			// usage
								   SRCCOPY);				// raster operation code

	ThrowIf(result == GDI_ERROR);	
}

 
//---------------------------------------------------------------
//
// XPixMapShape::Draw (XDrawContext, XPixMap, XRect, XRect, XRegion, EDrawMode) [static]
//
//---------------------------------------------------------------
void XPixMapShape::Draw(XDrawContext& context, const XPixMap& pixMap, const XRect& srcRect, const XRect& dstRect, const XRegion& clipRgn, EDrawMode mode)
{
	PRECONDITION(mode == kCopyMode);		// $$ the only other really useful mode is kTransparentMode which is unsupported by GDI...

	XLocker lock(pixMap);

	const BITMAPINFO* info = pixMap.GetInfo();

	XRegion oldClip = context.GetClip();
	XRegion newClip = oldClip & clipRgn;
	context.SetClip(newClip);
	
	int32 result = ::StretchDIBits(context.GetOSContext(),	// device context
								   dstRect.left,			// destination rect
								   dstRect.top,
								   dstRect.GetWidth(),
								   dstRect.GetHeight(),
								   srcRect.left,			// source rect
								   srcRect.top,
								   srcRect.GetWidth(),
								   srcRect.GetHeight(),
								   pixMap.GetBuffer(),		// bitmap bits
								   info,					// bitmap data
								   DIB_RGB_COLORS,			// usage
								   SRCCOPY);				// raster operation code

	context.SetClip(oldClip);
	ThrowIf(result == GDI_ERROR);	
}

	
//---------------------------------------------------------------
//
// XPixMapShape::SetPixMap
//
//---------------------------------------------------------------
void XPixMapShape::SetPixMap(XPixMap* pixMap)
{
	PRECONDITION(pixMap != nil);
	
	delete mPixMap;
	mPixMap = pixMap;
}


//---------------------------------------------------------------
//
// XPixMapShape::SetSrcRect
//
//---------------------------------------------------------------
void XPixMapShape::SetSrcRect(const XRect& srcRect)
{
	mSrcRect = srcRect;
}


//---------------------------------------------------------------
//
// XPixMapShape::SetMode
//
//---------------------------------------------------------------
void XPixMapShape::SetMode(EDrawMode mode)
{
	mMode = mode;
}


//---------------------------------------------------------------
//
// XPixMapShape::Draw
//
//---------------------------------------------------------------
void XPixMapShape::Draw(XDrawContext& context) const
{
	XPixMapShape::Draw(context, *mPixMap, mSrcRect, mDstRect, mMode);
} 


}	// namespace Whisper

