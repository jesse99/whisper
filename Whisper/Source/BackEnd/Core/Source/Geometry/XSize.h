/*
 *  File:       XSize.h
 *  Summary:	Size class
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XSize.h,v $
 *		Revision 1.6  2001/03/15 09:03:45  jesjones
 *		Exported the friend functions.
 *		
 *		Revision 1.5  2001/03/03 11:16:10  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.4  2001/03/02 11:25:22  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/10 08:55:46  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:52:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <cmath>

#include <XDebug.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XPoint;
class XSize;


//-----------------------------------
//	Constants
//
extern CORE_EXPORT const XSize kZeroSize;


// ===================================================================================
//	class XSize
//!		2D size class
// ===================================================================================
class CORE_EXPORT XSize {

//-----------------------------------
//	Initialization/Destruction
//
public:
						XSize()									{}
						XSize(int32 inWidth, int32 inHeight)	{width = inWidth; height = inHeight;}
	explicit			XSize(const XPoint& pt);
	
#if WIN
						XSize(const SIZE& rhs)					{width = rhs.cx; height = rhs.cy;}
#endif

			void 		Set(int32 inWidth, int32 inHeight)		{width = inWidth; height = inHeight;}

//-----------------------------------
//	API
//
public:
	//! @name Conversion Operators
	//@{
#if MAC
			OSSize 		GetOSSize() const;
			OSPoint 	GetOSPoint() const						{return this->GetOSSize();}
#elif WIN
						operator SIZE() const					{return *(const SIZE *) this;}
						operator POINT() const					{return *(const POINT *) this;}
						operator SIZE*()						{return (SIZE*) this;}
						operator POINT*()						{return (POINT*) this;}

			OSSize 		GetOSSize() const						{return *(SIZE *) this;}
			OSPoint 	GetOSPoint() const						{return *(POINT *) this;}
#endif
	//@}

	//! @name Arithmetic operators
	//@{
			XSize 		operator-() const						{return XSize(-width, -height);}
	
			XSize 		operator+(const XSize& rhs) const		{return XSize(width + rhs.width, height + rhs.height);}
			XSize 		operator-(const XSize& rhs) const		{return XSize(width - rhs.width, height - rhs.height);}	
		
			XSize 		operator*(int32 num) const				{return XSize(width*num, height*num);}
	friend 	XSize 		operator*(int32 num, const XSize& rhs)	{return XSize(rhs.width*num, rhs.height*num);}
			XSize 		operator/(int32 num) const				{ASSERT(num != 0); return XSize(width/num, height/num);}

			XSize& 		operator+=(const XSize& rhs);			
			XSize& 		operator-=(const XSize& rhs);			
		
			XSize& 		operator*=(int32 num)					{width *= num; height *= num; return *this;}
			XSize&	 	operator/=(int32 num)					{ASSERT(num != 0); width /= num; height /= num; return *this;}
	//@}

	//! @name Relational Operators
	//@{
			bool 		operator==(const XSize& rhs) const		{return width == rhs.width && height == rhs.height;}
			bool 		operator!=(const XSize& rhs) const		{return width != rhs.width || height != rhs.height;}
	//@}

	//! @name Misc
	//@{
			int32 		GetArea() const							{return width * height;}
			
	friend 	CORE_EXPORT void 			FromStr(const std::wstring& str, XSize& value);
	friend 	CORE_EXPORT std::wstring	ToStr(const XSize& value);
	//@}

//-----------------------------------
//	Member Data
//
public:
	int32 width;
	int32 height;
};


// ===================================================================================
//	Inlines
// ===================================================================================
inline XSize& XSize::operator+=(const XSize& rhs)		
{
	width 	+= rhs.width; 
	height 	+= rhs.height; 
	return *this;
}

inline XSize& XSize::operator-=(const XSize& rhs)	
{
	width 	-= rhs.width; 
	height 	-= rhs.height; 
	return *this;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


DECLARE_POD(Whisper::XSize)

