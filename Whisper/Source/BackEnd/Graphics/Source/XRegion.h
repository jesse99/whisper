/*
 *  File:       XRegion.h
 *  Summary:   	Cross platform region class.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegion.h,v $
 *		Revision 1.5  2001/03/16 10:36:49  jesjones
 *		Exported the friend functions.
 *		
 *		Revision 1.4  2000/12/10 08:55:04  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/12/10 04:11:14  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:48:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#if MAC
	#include <QuickDraw.h>
#endif

#include <XNumbers.h>
#include <XRect.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XRegion
//!		Cross platform region class.
// ===================================================================================
class GRAPHICS_EXPORT XRegion {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XRegion();
		
						XRegion();
						
						XRegion(const XRect& rect);
						/**< rectangle */
											
						XRegion(const XRect* rects, uint32 count);		// ($$$ may want to use template member function here)
						/**< list of rectangles */

						XRegion(const XRect& rect, int32 ovalWidth, int32 ovalHeight);
						/**< round rectangle */
						
						XRegion(const XPoint& center, int32 width, int32 height);
						/**< ellipse */
						
						XRegion(const XPoint* vertices, uint32 count);	// ($$$ may want to use template member function here)
						/**< polygon */
								
						XRegion(OSRegion rgn);
						/**< copy of OS region */
						
						XRegion(const XRegion& rhs);
						
//-----------------------------------
//	API
//
public:
	//! @name Conversion Operators
	//@{
						operator OSRegion() const					{return mRgn;}
	//@}

	//! @name Assignment
	//@{
			XRegion& 	operator=(const XRegion& rhs);
	
			XRegion& 	operator+=(const XRegion& rhs);		
			XRegion& 	operator-=(const XRegion& rhs);
			XRegion& 	operator&=(const XRegion& rhs);			
			XRegion& 	operator^=(const XRegion& rhs);
	
			XRegion& 	operator+=(const XPoint& offset);
			XRegion& 	operator-=(const XPoint& offset);
	//@}
			
	//! @name Arithmetic
	//@{
	friend 	GRAPHICS_EXPORT XRegion 	operator+(const XRegion& lhs, const XRegion& rhs);	
	friend 	GRAPHICS_EXPORT XRegion 	operator-(const XRegion& lhs, const XRegion& rhs);	
	friend 	GRAPHICS_EXPORT XRegion 	operator&(const XRegion& lhs, const XRegion& rhs);		
	friend 	GRAPHICS_EXPORT XRegion 	operator^(const XRegion& lhs, const XRegion& rhs);	
	
			void 		MakeEmpty();
	//@}

	//! @name Moving
	//@{
	friend 	GRAPHICS_EXPORT XRegion 	operator+(const XRegion& lhs, const XPoint& offset);
	friend 	GRAPHICS_EXPORT XRegion 	operator-(const XRegion& lhs, const XPoint& offset);
	
			void 		MoveTo(const XPoint& pt);
						/**< Moves region so that origin is at pt. */
	//@}
	
	//! @name Comparison
	//@{
			bool 		operator==(const XRegion& rhs) const;
			bool 		operator!=(const XRegion& rhs) const			{return !this->operator==(rhs);}
	//@}
	
	//! @name Inquiry
	//@{
			bool		IsEmpty() const;
	
			XRect 		GetEnclosingRect() const;
						/**< Returns a rectangle completely enclosing the region. */
				
			bool 		Contains(const XPoint& pt) const;
			bool 		Contains(const XRect& rect) const;
			
			bool 		Intersects(const XRect& rect) const;
	//@}

	//! @name Mac Only
	//@{
#if MAC
			void 		Inset(int32 dx, int32 dy)						{::InsetRgn(mRgn, numeric_cast<int16>(dx), numeric_cast<int16>(dy));}
			void 		Inset(const XPoint& delta)						{::InsetRgn(mRgn, numeric_cast<int16>(delta.x), numeric_cast<int16>(delta.y));}
			
			void 		Map(const XRect& srcRect, const XRect& dstRect);
						/**< Scales the region by the ratio of the sizes of srcRect and 
						dstRect. Also moves the region to a location within dstRect 
						similiar to its position within srcRect. */
#endif
	//@}

//-----------------------------------
//	Member Data
//
private:
	OSRegion	mRgn;
};


// ===================================================================================
//	Inlines
// ===================================================================================
#if MAC
	inline bool XRegion::operator==(const XRegion& rhs) const
	{
		return ::EqualRgn(mRgn, rhs.mRgn);	
	}

	inline bool XRegion::IsEmpty() const									
	{
		return ::EmptyRgn(mRgn);
	}
		
	inline XRect XRegion::GetEnclosingRect() const						
	{
#if TARGET_CARBON
		Rect bounds;
		return *::GetRegionBounds(mRgn, &bounds);
#else
		return (**mRgn).rgnBBox;
#endif
	}
					
	inline bool XRegion::Contains(const XPoint& pt) const				
	{
		return ::PtInRgn(pt.GetOSPoint(), mRgn);
	}
				
	inline bool XRegion::Intersects(const XRect& rect) const				
	{
		Rect temp = rect.GetOSRect();
		return ::RectInRgn(&temp, mRgn);
	}

#elif WIN
	inline bool XRegion::operator==(const XRegion& rhs)	const
	{
		return ::EqualRgn(mRgn, rhs.mRgn) != 0;
	}

	inline bool XRegion::IsEmpty() const									
	{
		return this->GetEnclosingRect() == kZeroRect;
	}
		
	inline bool XRegion::Contains(const XPoint& pt) const				
	{
		return ::PtInRegion(mRgn, pt.x, pt.y) != 0;
	}
				
	inline bool XRegion::Intersects(const XRect& rect) const				
	{
		return ::RectInRegion(mRgn, rect) != 0;
	}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


