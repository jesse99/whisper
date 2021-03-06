/*
 *  File:       MShapes.cpp
 *  Summary:	Classes that draw into an XDrawContext.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MShapes.cpp,v $
 *		Revision 1.8  2001/04/27 04:22:20  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.7  2001/04/21 03:28:33  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/04/13 07:53:17  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2001/03/03 11:09:48  jesjones
 *		Added XPixMapShape::Draw method that takes a clip region.
 *		
 *		Revision 1.4  2001/02/28 09:59:11  jesjones
 *		Fixed XPixmapShape so that it works better with transparent draw mode.
 *		
 *		Revision 1.3  2000/12/10 04:05:18  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:19:37  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	  7/19/99	JDJ		Tweaked for Carbon.
 *		 <4>	  6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <3>	  3/07/99	JDJ		Removed XBoundedShape.
 *		 <2>	 12/30/98	JDJ		XTextShape::Draw no longer leaves where un-initialized when
 *									align == kDefaultAlign. Call to StyledLineBreak was passing
 *									an int where a Fixed was expected.
 *		 <1>	 12/11/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XShapes.h>

#include <LowMem.h>

#include <MQuickDrawUtils.h>
#include <XNumbers.h>
#include <XPixMap.h>
#include <XSelectAttribute.h>
#include <XTranscode.h>

namespace Whisper {


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
	
	::MoveTo(numeric_cast<int16>(pt1.x), numeric_cast<int16>(pt1.y));
	::LineTo(numeric_cast<int16>(pt2.x), numeric_cast<int16>(pt2.y));
}


//---------------------------------------------------------------
//
// XLineShape::Draw (XDrawContext, XPoint, XSize, XPen)	[static]
//
//---------------------------------------------------------------
void XLineShape::Draw(XDrawContext& context, const XPoint& pt, const XSize& delta, const XPen& pen)
{
	XSelectAttribute select(context, pen);
	
	::MoveTo(numeric_cast<int16>(pt.x), numeric_cast<int16>(pt.y));
	::Line(numeric_cast<int16>(delta.width), numeric_cast<int16>(delta.height));
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


//---------------------------------------------------------------
//
// XLineShape::Draw
//
//---------------------------------------------------------------
void XLineShape::Draw(XDrawContext& context) const
{
	XLineShape::Draw(context, mPt1, mPt2, mPen);
}

#pragma mark -

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

#pragma mark -

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
	
	Rect temp = rect.GetOSRect();
	::FrameRect(&temp);	
}


//---------------------------------------------------------------
//
// XRectShape::Fill										[static]
//
//---------------------------------------------------------------
void XRectShape::Fill(XDrawContext& context, const XRect& rect, const XBrush& brush)
{
	XSelectAttribute select(context, brush);
	
	Rect temp = rect.GetOSRect();
	::PaintRect(&temp);
}


//---------------------------------------------------------------
//
// XRectShape::Invert									[static]
//
//---------------------------------------------------------------
void XRectShape::Invert(XDrawContext& context, const XRect& rect)
{
	context.MakeCurrent();
	
	Rect temp = rect.GetOSRect();
	::InvertRect(&temp);
}


//---------------------------------------------------------------
//
// XRectShape::Hilite									[static]
//
//---------------------------------------------------------------
void XRectShape::Hilite(XDrawContext& context, const XRect& rect)
{
	context.MakeCurrent();
	
	LMSetHiliteMode(0);					// QuickDraw will reset this in the next drawing statement
	
	Rect temp = rect.GetOSRect();
	::InvertRect(&temp);
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

#pragma mark -

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
	
	Rect temp = rect.GetOSRect();
	::FrameRoundRect(&temp, numeric_cast<int16>(ovalSize.width), numeric_cast<int16>(ovalSize.height));
}


//---------------------------------------------------------------
//
// XRoundRectShape::Fill								[static]
//
//---------------------------------------------------------------
void XRoundRectShape::Fill(XDrawContext& context, const XRect& rect, const XSize& ovalSize, const XBrush& brush)
{
	XSelectAttribute select(context, brush);
	
	Rect temp = rect.GetOSRect();
	::PaintRoundRect(&temp, numeric_cast<int16>(ovalSize.width), numeric_cast<int16>(ovalSize.height));
}


//---------------------------------------------------------------
//
// XRoundRectShape::Invert								[static]
//
//---------------------------------------------------------------
void XRoundRectShape::Invert(XDrawContext& context, const XRect& rect, const XSize& ovalSize)
{
	context.MakeCurrent();
	
	Rect temp = rect.GetOSRect();
	::InvertRoundRect(&temp, numeric_cast<int16>(ovalSize.width), numeric_cast<int16>(ovalSize.height));
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

#pragma mark -

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
	
	Rect temp = rect.GetOSRect();
	::FrameOval(&temp);
}


//---------------------------------------------------------------
//
// XOvalShape::Fill										[static]
//
//---------------------------------------------------------------
void XOvalShape::Fill(XDrawContext& context, const XRect& rect, const XBrush& brush)
{
	XSelectAttribute select(context, brush);
	
	Rect temp = rect.GetOSRect();
	::PaintOval(&temp);
}


//---------------------------------------------------------------
//
// XOvalShape::Invert									[static]
//
//---------------------------------------------------------------
void XOvalShape::Invert(XDrawContext& context, const XRect& rect)
{
	context.MakeCurrent();
	
	Rect temp = rect.GetOSRect();
	::InvertOval(&temp);
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

#pragma mark -

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
	
	Rect temp = rect.GetOSRect();
	::FrameArc(&temp, numeric_cast<int16>(startAngle), numeric_cast<int16>(arcAngle));
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
	
	Rect temp = rect.GetOSRect();
	::PaintArc(&temp, numeric_cast<int16>(startAngle), numeric_cast<int16>(arcAngle));
}


//---------------------------------------------------------------
//
// XArcShape::Invert									[static]
//
//---------------------------------------------------------------
void XArcShape::Invert(XDrawContext& context, const XRect& rect, int32 startAngle, int32 arcAngle)
{
	PRECONDITION(arcAngle >= 0);

	context.MakeCurrent();
	
	Rect temp = rect.GetOSRect();
	::InvertArc(&temp, numeric_cast<int16>(startAngle), numeric_cast<int16>(arcAngle));
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

#pragma mark -

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
	XSelectAttribute select(context, brush);
	
	PenState state;
	GetPenState(&state);

	::PenSize(numeric_cast<int16>(width), numeric_cast<int16>(height));	
	::FrameRgn(rgn);
	::PenSize(state.pnSize.h, state.pnSize.v);	
}


//---------------------------------------------------------------
//
// XRegionShape::Fill									[static]
//
//---------------------------------------------------------------
void XRegionShape::Fill(XDrawContext& context, const XRegion& rgn, const XBrush& brush)
{
	XSelectAttribute select(context, brush);
	
	::PaintRgn(rgn);
}


//---------------------------------------------------------------
//
// XRegionShape::Invert									[static]
//
//---------------------------------------------------------------
void XRegionShape::Invert(XDrawContext& context, const XRegion& rgn)
{
	context.MakeCurrent();
	
	::InvertRgn(rgn);
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

#pragma mark -

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
// XTextShape::XTextShape (string, XPoint, XFont)
//
//---------------------------------------------------------------
XTextShape::XTextShape(const std::wstring& text, const XPoint& where, const XFont& font) : mBounds(kZeroRect), mText(text), mFont(font)
{
	SFontMetrics metrics = mFont.GetMetrics();

	mBounds.left   = where.x;
	mBounds.right  = 32000;
	mBounds.top    = where.y - metrics.ascent;
	mBounds.bottom = where.y + metrics.descent;

	mAlign = kLeftTopAlign;

	mWordWrap = false;
}


//---------------------------------------------------------------
//
// XTextShape::XTextShape (string, XRect, XFont, ETextAlignment, bool)
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
// XTextShape::Draw (XDrawContext, string, XPoint, XFont) [static]
//
//---------------------------------------------------------------
void XTextShape::Draw(XDrawContext& context, const std::wstring& text, const XPoint& where, const XFont& font)
{
	context.MakeCurrent();	
	XSelectAttribute select(context, font);

	::MoveTo(numeric_cast<int16>(where.x), numeric_cast<int16>(where.y));
	::DrawText(ToPlatformStr(text).c_str(), 0, numeric_cast<int16>(text.length()));
}

	
//---------------------------------------------------------------
//
// XTextShape::Draw (XDrawContext, string, XRect, XFont, ETextAlignment, bool) [static]
//
//---------------------------------------------------------------
void XTextShape::Draw(XDrawContext& context, const std::wstring& text, const XRect& bounds, const XFont& font, ETextAlignment align, bool wordWrap)
{
	context.MakeCurrent();	
	XSelectAttribute select(context, font);

	XRegion oldClip = context.GetClip();
	context.SetClip(oldClip & bounds);
		
	XPoint where;
	uint32 count;
	
	SFontMetrics metrics = font.GetMetrics();
	
	if (align == kLeftTopAlign || align == kCenterTopAlign || align == kRightTopAlign || align == kDefaultAlign) {
		where.x = bounds.left;
		where.y = bounds.top + metrics.ascent;
		
		int32 line = 0;
		uint32 start = 0;
		while (where.y < bounds.bottom && start < text.length()) {
			count = XTextShape::DoBreakLine(text, bounds.GetWidth(), start, wordWrap);

			if (align == kRightTopAlign || (align == kDefaultAlign && ::GetSysDirection() < 0))
				where.x = bounds.right - font.GetStringSize(text.substr(start, count)).width;

			else if (align == kCenterTopAlign)
				where.x = bounds.left + (bounds.GetWidth() - font.GetStringSize(text.substr(start, count)).width)/2;
			
			::MoveTo(numeric_cast<int16>(where.x), numeric_cast<int16>(where.y));
			::DrawText(ToPlatformStr(text).c_str(), numeric_cast<int16>(start), numeric_cast<int16>(count));
			
			start += count;
			where.y += metrics.ascent + metrics.descent + metrics.leading;
			
			if (start < text.length() && text[start] == '\n')
				++start;
		}
	
	} else {
		count = XTextShape::DoBreakLine(text, bounds.GetWidth(), 0, wordWrap);

		switch (align) {
			case kLeftBottomAlign:
				where.x = bounds.left;
				where.y = bounds.bottom - metrics.descent;
				break;
				
			case kCenterBottomAlign:
				where.x = bounds.left + (bounds.GetWidth() - font.GetStringSize(text.substr(0, count)).width)/2;
				where.y = bounds.bottom - metrics.descent;
				break;
				
			case kRightBottomAlign:
				where.x = bounds.right - font.GetStringSize(text.substr(0, count)).width;
				where.y = bounds.bottom - metrics.descent;
				break;

			default:
				DEBUGSTR("Bad align flag in XTextShape::Draw");
		}
				
		::MoveTo(numeric_cast<int16>(where.x), numeric_cast<int16>(where.y));
		::DrawText(ToPlatformStr(text).c_str(), 0, numeric_cast<int16>(count));
	}
	
	context.SetClip(oldClip);
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


//---------------------------------------------------------------
//
// XTextShape::DoBreakLine								[static]	
//
//---------------------------------------------------------------
uint32 XTextShape::DoBreakLine(const std::wstring& text, int32 pixelWidth, uint32 startIndex, bool wordWrap)
{
	uint32 count = XTextShape::DoFindLineLength(text, startIndex);
	
	if (wordWrap) {
		uint32 count2 = XTextShape::DoWrapLine(text, pixelWidth, startIndex);
		if (count2 < count)
			count = count2;
	}
	
	return count;
}


//---------------------------------------------------------------
//
// XTextShape::DoFindLineLength							[static]	
//
// Returns the number of chars from startIndex up to (and including)
// the next carriage return.
//
//---------------------------------------------------------------
uint32 XTextShape::DoFindLineLength(const std::wstring& text, uint32 startIndex)
{	
	uint32 index = startIndex;
	while (index < text.length() && text[index] != '\n')
		++index;

	if (index < text.length() && text[index] == '\n')
		++index;
		
	uint32 count = index - startIndex;
	
	return count;
}


//---------------------------------------------------------------
//
// XTextShape::DoWrapLine								[static]	
//
// Returns the number of chars from startIndex whose total width
// does not exceed pixelWidth.
//
//---------------------------------------------------------------
uint32 XTextShape::DoWrapLine(const std::wstring& text, int32 pixelWidth, uint32 startIndex)
{
	PRECONDITION(pixelWidth > 0);
	
	uint32 numChars = text.length() - startIndex;

	int32 lineBytes = 1;
	Fixed width = pixelWidth << 16;
	(void) ::StyledLineBreak((Ptr) ToPlatformStr(text).c_str() + startIndex, 	// pointer to the text
							 numeric_cast<int32>(numChars), 				// number of bytes of text
							 0, 								// byte offset to the beginning of the style run
							 numeric_cast<int32>(numChars), 				// byte offset to the end of the style run
							 0,									// flags (reserved)
							 &width,	 						// max length of line to display in pixels
							 &lineBytes);						// byte count of number of chars in broken line

	return numeric_cast<uint32>(lineBytes);
}

#pragma mark -

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
	context.MakeCurrent();	
		
	CGrafPtr srcPort = pixMap.GetWorld();
	CGrafPtr dstPort = GetQDGlobalsThePort();

	bool keepBackColor = mode == kTransparentMode;  // don't change backColor for transparent mode
	PrepCopyBits(keepBackColor);
				
	{
	XLocker lock(pixMap);
		Rect src = srcRect.GetOSRect();
		Rect dst = dstRect.GetOSRect();
		
		CopyColorBits(srcPort, dstPort, &src, &dst, mode, nil);
	}
}

	
//---------------------------------------------------------------
//
// XPixMapShape::Draw (XDrawContext, XPixMap, XRect, XRect, XRegion, EDrawMode) [static]
//
//---------------------------------------------------------------
void XPixMapShape::Draw(XDrawContext& context, const XPixMap& pixMap, const XRect& srcRect, const XRect& dstRect, const XRegion& clipRgn, EDrawMode mode)
{
	context.MakeCurrent();	
		
	CGrafPtr srcPort = pixMap.GetWorld();
	CGrafPtr dstPort = GetQDGlobalsThePort();

	bool keepBackColor = mode == kTransparentMode;  // don't change backColor for transparent mode
	PrepCopyBits(keepBackColor);
				
	{
	XLocker lock(pixMap);
		Rect src = srcRect.GetOSRect();
		Rect dst = dstRect.GetOSRect();
		
		CopyColorBits(srcPort, dstPort, &src, &dst, mode, clipRgn);
	}
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

