/*
 *  File:       XShapes.h
 *  Summary:	Classes that draw into an XDrawContext.
 *  Written by: Jesse Jones
 *
 *	Note:		GDI and QuickDraw handle coordinates differently. In GDI coordinates
 *				directly index pixels, so to frame a 2x2 rectangle you could use
 *				XRect(0, 0, 1, 1). QuickDraw, on the other hand, considers coordinates
 *				to be the vertices between pixels with the pen hanging below and to
 *				the right. When shapes are framed the interior of the rectangle is
 *				filled (ie wide pens always draw inside the rectangle). So, the above
 *				rectangle would be drawn using XRect(0, 0, 2, 2).
 *
 *				I don't see any compelling reason for choosing either approach. I've
 *				elected to use the QuickDraw approach because it's the one I'm more
 *				familiar with and it has the minor advantage of easily computing the
 *				number of pixels plotted (eg when framing rect the width of the
 *				rectangle on the screen is rect.GetWidth() instead of rect.GetWidth() + 1).
 *
 *				Note that coordinates for lines are the same for both systems. Also
 *				regions are considerably more difficult to handle. Currently they will
 *				draw slightly differently unless the client accounts for the differences.
 *				$$$ It might be possible to construct a new region by using GetRegionData
 *				to get the rectangles making up the region, but this is awfully ugly,
 *				especially for complex regions. $$$
 *
 *	Classes:	XLineShape			- A shape that draws a line.
 *				XContourShape		- Abstract base class for shapes that can be filled or framed.
 *					XRectShape		- A shape that draws rectangles.
 *					XRoundRectShape	- A shape that draws round rectangles.
 *					XOvalShape 		- A shape that draws ovals.
 *					XArcShape		- A shape that draws arcs.
 *					XRegionShape	- A shape that draws regions.
 *				XTextShape			- A shape that draws text.
 *				XPixMapShape		- A shape that draws pixel maps.
 *				XGroupShape			- A shape that draws a list of shapes.
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XShapes.h,v $
 *		Revision 1.6  2001/03/06 07:33:17  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.5  2001/03/03 11:10:10  jesjones
 *		Added XPixMapShape::Draw method that takes a clip region.
 *		
 *		Revision 1.4  2001/02/25 08:22:49  jesjones
 *		Added an XLineShape comment.
 *		
 *		Revision 1.3  2000/12/10 08:55:39  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:51:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <list>

#include <XConstants.h>
#include <XFont.h>
#include <XPen.h>
#include <XRegion.h>
#include <XShape.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XPixMap;


// ===================================================================================
//	class XLineShape
//!		A shape that draws a line.
// ===================================================================================
class GRAPHICS_EXPORT XLineShape : public XShape {

	typedef XShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XLineShape();
						
						XLineShape(const XPoint& pt1, const XPoint& pt2, const XPen& pen = kBlackPen);
						/**< $$$ Note that this will draw differently on the two platforms for wide pens.
						On the Mac all drawing will be below and to the right of the path.
						On Windows the drawing will be centered on the path. */
						
//-----------------------------------
//	New API
//
public:
	//! @name Drawing
	//@{
	static	void 		Draw(XDrawContext& context, const XPoint& pt1, const XPoint& pt2, const XPen& pen = kBlackPen);

	static	void 		Draw(XDrawContext& context, const XPoint& pt, const XSize& delta, const XPen& pen = kBlackPen);
						/**< Same as Draw(content, pt, pt+delta). */
	//@}

	//! @name Pen
	//@{
			XPen 		GetPen() const									{return mPen;}

	virtual	void 		SetPen(const XPen& pen);
	//@}

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Draw(XDrawContext& context) const;
	
	virtual XRect 		GetBounds() const;

	virtual void 		MoveBy(const XSize& delta);

	virtual XLineShape*	Clone() const;

//-----------------------------------
//	Member Data
//
protected:
	XPoint	mPt1;
	XPoint	mPt2;
	XPen	mPen;
};


// ===================================================================================
//	class XContourShape
//!		Abstract base class for shapes that can be filled or framed.
// ===================================================================================
class GRAPHICS_EXPORT XContourShape : public XShape {

	typedef XShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XContourShape();
												
						XContourShape(const XPen& pen);
						// framing

						XContourShape(const XBrush& brush);
						// filling
						
//-----------------------------------
//	New API
//
public:
	//! @name Framing
	//@{
			bool 		IsFraming() const						{return mFraming;}

	virtual	void  		StartFraming(bool frame = true);
	//@}

	//! @name Filling
	//@{
			bool 		IsFilling() const						{return !mFraming;}

			void  		StartFilling(bool fill = true)			{this->StartFraming(!fill);}
	//@}

	//! @name Pen
	//@{
			XPen 		GetPen() const							{return mPen;}
	
	virtual	void 		SetPen(const XPen& pen);
	//@}

	//! @name Brush
	//@{
			XBrush 		GetBrush() const						{return mBrush;}
	
	virtual	void 		SetBrush(const XBrush& brush);
	//@}

//-----------------------------------
//	Member Data
//
protected:
	bool		mFraming;
	XPen		mPen;
	XBrush		mBrush;
};


// ===================================================================================
//	class XRectShape
//!		A shape that draws rectangles.
// ===================================================================================
class GRAPHICS_EXPORT XRectShape : public XContourShape {

	typedef XContourShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XRectShape();
						
						XRectShape(const XRect& rect, const XPen& pen = kBlackPen);

						XRectShape(const XRect& rect, const XBrush& brush);
						
//-----------------------------------
//	New API
//
public:
	static	void 		Frame(XDrawContext& context, const XRect& rect, const XPen& pen = kBlackPen);
						/**< Always draws inside the rect. */

	static	void 		Fill(XDrawContext& context, const XRect& rect, const XBrush& brush = kBlackBrush);

	static	void 		Invert(XDrawContext& context, const XRect& rect);

	static	void 		Hilite(XDrawContext& context, const XRect& rect);

	virtual void 		SetBounds(const XRect& bounds)				{mBounds = bounds;}

//-----------------------------------
//	Inherited API
//
public:
	virtual XRectShape*	Clone() const;

	virtual XRect 		GetBounds() const							{return mBounds;}

	virtual void 		MoveBy(const XSize& delta)					{mBounds += delta;}

	virtual void 		Draw(XDrawContext& context) const;

//-----------------------------------
//	Member Data
//
protected:
	XRect	mBounds;
};


// ===================================================================================
//	class XRoundRectShape
//!		A shape that draws round rectangles.
// ===================================================================================
class GRAPHICS_EXPORT XRoundRectShape : public XContourShape {

	typedef XContourShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XRoundRectShape();
						
						XRoundRectShape(const XRect& rect, const XSize& ovalSize, const XPen& pen = kBlackPen);

						XRoundRectShape(const XRect& rect, const XSize& ovalSize, const XBrush& brush);
						
//-----------------------------------
//	New API
//
public:
	//! @name Drawing
	//@{
	static	void 		Frame(XDrawContext& context, const XRect& rect, const XSize& ovalSize, const XPen& pen = kBlackPen);
						/**< Always draws inside the rect. */

	static	void 		Fill(XDrawContext& context, const XRect& rect, const XSize& ovalSize, const XBrush& brush = kBlackBrush);

	static	void 		Invert(XDrawContext& context, const XRect& rect, const XSize& ovalSize);
	//@}

	//! @name Oval Size
	//@{
			XSize 		GetOvalSize() const							{return mOvalSize;}
			
	virtual void 		SetOvalSize(const XSize& ovalSize);
	//@}

	//! @name Misc
	//@{
	virtual void 		SetBounds(const XRect& bounds)				{mBounds = bounds;}
	//@}

//-----------------------------------
//	Inherited API
//
public:
	virtual XRoundRectShape* Clone() const;

	virtual XRect 		GetBounds() const							{return mBounds;}

	virtual void 		MoveBy(const XSize& delta)					{mBounds += delta;}

	virtual void 		Draw(XDrawContext& context) const;

//-----------------------------------
//	Member Data
//
protected:
	XSize	mOvalSize;
	XRect	mBounds;
};


// ===================================================================================
//	class XOvalShape
//!		A shape that draws ovals.
// ===================================================================================
class GRAPHICS_EXPORT XOvalShape : public XContourShape {

	typedef XContourShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XOvalShape();
						
						XOvalShape(const XRect& rect, const XPen& pen = kBlackPen);

						XOvalShape(const XRect& rect, const XBrush& brush);
						
//-----------------------------------
//	New API
//
public:
	static	void 		Frame(XDrawContext& context, const XRect& rect, const XPen& pen = kBlackPen);
						/**< Always draws inside the rect. */

	static	void 		Fill(XDrawContext& context, const XRect& rect, const XBrush& brush = kBlackBrush);

	static	void 		Invert(XDrawContext& context, const XRect& rect);

	virtual void 		SetBounds(const XRect& bounds)				{mBounds = bounds;}

//-----------------------------------
//	Inherited API
//
public:
	virtual XOvalShape*	Clone() const;

	virtual XRect 		GetBounds() const							{return mBounds;}

	virtual void 		MoveBy(const XSize& delta)					{mBounds += delta;}

	virtual void 		Draw(XDrawContext& context) const;

//-----------------------------------
//	Member Data
//
protected:
	XRect	mBounds;
};


// ===================================================================================
//	class XArcShape
//!		A shape that draws arcs.
// ===================================================================================
class GRAPHICS_EXPORT XArcShape : public XContourShape {

	typedef XContourShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XArcShape();
						
						XArcShape(const XRect& rect, int32 startAngle, int32 arcAngle, const XPen& pen = kBlackPen);

						XArcShape(const XRect& rect, int32 startAngle, int32 arcAngle, const XBrush& brush);
						
//-----------------------------------
//	New API
//
public:
	//! @name Drawing
	//@{
	static	void 		Frame(XDrawContext& context, const XRect& rect, int32 startAngle, int32 arcAngle, const XPen& pen = kBlackPen);
						/**< Always draws inside the rect. */

	static	void 		Fill(XDrawContext& context, const XRect& rect, int32 startAngle, int32 arcAngle, const XBrush& brush = kBlackBrush);

	static	void 		Invert(XDrawContext& context, const XRect& rect, int32 startAngle, int32 arcAngle);
	//@}

	//! @name Start Angle
	//@{
			int32 		GetStartAngle() const						{return mStartAngle;}
			
	virtual void 		SetStartAngle(int32 angle);
	//@}

	//! @name Arc Angle
	//@{
			int32 		GetArcAngle() const							{return mArcAngle;}
			
	virtual void 		SetArcAngle(int32 angle);
	//@}

	//! @name Misc
	//@{
	virtual void 		SetBounds(const XRect& bounds)				{mBounds = bounds;}
	//@}

//-----------------------------------
//	Inherited API
//
public:
	virtual XArcShape*	Clone() const;

	virtual XRect 		GetBounds() const							{return mBounds;}

	virtual void 		MoveBy(const XSize& delta)					{mBounds += delta;}

	virtual void 		Draw(XDrawContext& context) const;

//-----------------------------------
//	Member Data
//
protected:
	int32	mStartAngle;
	int32	mArcAngle;
	XRect	mBounds;
};


// ===================================================================================
//	class XRegionShape
//!		A shape that draws regions.
// ===================================================================================
class GRAPHICS_EXPORT XRegionShape : public XContourShape {

	typedef XContourShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XRegionShape();
						
						XRegionShape(const XRegion& rgn, const XBrush& brush, bool framing);
						
//-----------------------------------
//	New API
//
public:
	//! @name Drawing
	//@{
	static	void 		Frame(XDrawContext& context, const XRegion& rgn, const XBrush& brush = kBlackBrush, int32 width = 1, int32 height = 1);
						/**< This takes a brush because unlike every other shape GDI frames
						regions using brushes instead of pens... Also the same region
						will be one pixel wider and taller on Windows then on the Mac. */

	static	void 		Fill(XDrawContext& context, const XRegion& rgn, const XBrush& brush = kBlackBrush);

	static	void 		Invert(XDrawContext& context, const XRegion& rgn);
	//@}
	
	//! @name Region
	//@{
			XRegion 	GetRegion() const							{return mRegion;}
						
	virtual void 		SetRegion(const XRegion& rgn);
	//@}

//-----------------------------------
//	Inherited API
//
public:
	virtual XRegionShape* Clone() const;

	virtual XRect 		GetBounds() const							{return mRegion.GetEnclosingRect();}

	virtual void 		MoveBy(const XSize& delta)					{mRegion += XPoint(delta);}

	virtual void 		Draw(XDrawContext& context) const;

//-----------------------------------
//	Member Data
//
protected:
	XRegion		mRegion;
};


// ===================================================================================
//	class XTextShape
//!		A shape that draws text.
// ===================================================================================
class GRAPHICS_EXPORT XTextShape : public XShape {

	typedef XShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XTextShape();
						
						XTextShape(const std::wstring& text, const XPoint& where, const XFont& font);
						/**< Draws a single line of text starting at where. */
	
						XTextShape(const std::wstring& text, const XRect& bounds, const XFont& font, ETextAlignment align = kDefaultAlign, bool wordWrap = true);
						/**< Draws multiple lines (seperated by carriage returns) inside 
						bounds. If wordWrap is true words that would be clipped are 
						moved to the next line. */
												
//-----------------------------------
//	New API
//
public:
	//! @name Drawing
	//@{
	static	void 		Draw(XDrawContext& context, const std::wstring& text, const XPoint& where, const XFont& font);
						/**< where is the text's base line */
	
	static	void 		Draw(XDrawContext& context, const std::wstring& text, const XRect& bounds, const XFont& font, ETextAlignment align = kDefaultAlign, bool wordWrap = true);
	//@}
	
	//! @name Text
	//@{
			std::wstring GetText() const								{return mText;}
						
	virtual void 		SetText(const std::wstring& text);
	//@}

	//! @name Font
	//@{
			XFont 		GetFont() const								{return mFont;}
						
	virtual void 		SetFont(const XFont& font);
	//@}

	//! @name Alignment
	//@{
			ETextAlignment GetAlignment() const						{return mAlign;}
						
	virtual void 		SetAlignment(ETextAlignment align)			{mAlign = align;}
	//@}

	//! @name Wrapping
	//@{
			bool 		GetWrapping() const							{return mWordWrap;}
						
	virtual void 		SetWrapping(bool wrap);
	//@}

//-----------------------------------
//	Inherited API
//
public:
	virtual XTextShape*	Clone() const;
	
	virtual XRect 		GetBounds() const							{return mBounds;}

	virtual void 		MoveBy(const XSize& delta)					{mBounds += delta;}

	virtual void 		Draw(XDrawContext& context) const;

//-----------------------------------
//	Internal API
//
protected:
#if MAC
	static	uint32 		DoBreakLine(const std::wstring& text, int32 pixelWidth, uint32 startIndex, bool wordWrap);

	static	uint32 		DoFindLineLength(const std::wstring& text, uint32 startIndex);

	static	uint32 		DoWrapLine(const std::wstring& text, int32 pixelWidth, uint32 startIndex);
#endif

//-----------------------------------
//	Member Data
//
protected:
	std::wstring	mText;
	XFont			mFont;
	ETextAlignment	mAlign;
	bool			mWordWrap;

	XRect			mBounds;
};


// ===================================================================================
//	class XPixMapShape
//!		A shape that draws pixel maps.
// ===================================================================================
class GRAPHICS_EXPORT XPixMapShape : public XShape {

	typedef XShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XPixMapShape();
						
						XPixMapShape(XPixMap* takePixels, const XPoint& where = kZeroPt, EDrawMode mode = kCopyMode);
						
						XPixMapShape(XPixMap* takePixels, const XRect& dstRect, EDrawMode mode = kCopyMode);
						
						XPixMapShape(XPixMap* takePixels, const XRect& srcRect, const XRect& dstRect, EDrawMode mode = kCopyMode);
						
//-----------------------------------
//	New API
//
public:
	//! @name Drawing
	//@{
	static	void 		Draw(XDrawContext& context, const XPixMap& pixmap, const XPoint& where, EDrawMode mode = kCopyMode);
	
	static	void 		Draw(XDrawContext& context, const XPixMap& pixmap, const XRect& dstRect, EDrawMode mode = kCopyMode);
	
	static	void 		Draw(XDrawContext& context, const XPixMap& pixmap, const XRect& srcRect, const XRect& dstRect, EDrawMode mode = kCopyMode);

	static	void 		Draw(XDrawContext& context, const XPixMap& pixmap, const XRect& srcRect, const XRect& dstRect, const XRegion& clipRgn, EDrawMode mode = kCopyMode);
						/**< clipRgn is in the destination coordinate system. */
	//@}

	//! @name Image
	//@{
			const XPixMap* GetPixMap() const						{return mPixMap;}
	
			XPixMap* 	GetPixMap()									{return mPixMap;}
						
	virtual void 		SetPixMap(XPixMap* takePixels);
	//@}

	//! @name Src Rect
	//@{					
			XRect 		GetSrcRect() const							{return mSrcRect;}

	virtual void 		SetSrcRect(const XRect& srcRect);
	//@}

	//! @name Draw Mode
	//@{						
			EDrawMode 	GetMode() const								{return mMode;}
						
	virtual void 		SetMode(EDrawMode mode);
	//@}

//-----------------------------------
//	Inherited API
//
public:
	virtual XPixMapShape* Clone() const;
	
	virtual XRect 		GetBounds() const							{return mDstRect;}

	virtual void 		MoveBy(const XSize& delta)					{mDstRect += delta;}

	virtual void 		Draw(XDrawContext& context) const;

//-----------------------------------
//	Member Data
//
protected:
	XPixMap*	mPixMap;
	XRect		mSrcRect;
	XRect		mDstRect;
	EDrawMode	mMode;
};


// ===================================================================================
//	class XGroupShape
//!		A shape that draws a list of shapes.
// ===================================================================================
class GRAPHICS_EXPORT XGroupShape : public XShape {

	typedef XShape Inherited;

//-----------------------------------
//	Types
//
public:
	typedef std::list<XShape*> 		Shapes;
	typedef Shapes::const_iterator	const_iterator;
	typedef Shapes::iterator		iterator;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XGroupShape();
						
						XGroupShape(const XPoint& where = kZeroPt);
												
//-----------------------------------
//	New API
//
public:
	//! @name Access
	//@{
			const XShape* Front() const					{ASSERT(!mShapes.empty()); return mShapes.front();}
			const XShape* Back() const					{ASSERT(!mShapes.empty()); return mShapes.back();}

			XShape*		Front()							{ASSERT(!mShapes.empty()); mDirty = true; return mShapes.front();}
			XShape* 	Back()							{ASSERT(!mShapes.empty()); mDirty = true; return mShapes.back();}			
			
			const_iterator begin() const				{return mShapes.begin();}
			const_iterator end() const					{return mShapes.end();}

			iterator 	begin() 						{mDirty = true; return mShapes.begin();}
			iterator 	end() 							{mDirty = true; return mShapes.end();}
			
			uint32 		GetCount() const				{return mShapes.size();}
	//@}

	//! @name Adding
	//@{
			void 		PushFront(XShape* takeShape)	{this->Insert(this->begin(), takeShape);}
			void 		PushBack(XShape* takeShape)		{this->Insert(this->end(), takeShape);}
			
			void 		Insert(const iterator& iter, XShape* takeShape)	{ASSERT(takeShape != nil); mDirty = true; mShapes.insert(iter, takeShape);}
	//@}
			
	//! @name Removing
	//@{
			void 		PopFront()						{this->Erase(this->begin());}
			void 		PopBack()						{ASSERT(!mShapes.empty()); this->Erase(--this->end());}
			
			void 		Erase(const iterator& iter);
			void 		Clear();
	//@}

//-----------------------------------
//	Inherited API
//
public:
	virtual XGroupShape* Clone() const;

	virtual XRect 		GetBounds() const;

	virtual void 		MoveBy(const XSize& delta);

	virtual void 		Draw(XDrawContext& context) const;
						/**< Shapes are drawn relative to the group shape. */

//-----------------------------------
//	Member Data
//
protected:
	XPoint			mLoc;
	Shapes			mShapes;

	mutable XRect	mExtent;		// top and left may be negative
	mutable bool	mDirty;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

