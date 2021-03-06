/*
 *  File:       MRegion.cpp
 *  Summary:   	Cross platform region class.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MRegion.cpp,v $
 *		Revision 1.5  2001/04/21 03:28:12  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/02/28 09:58:27  jesjones
 *		Fixed elliptical ctor.
 *		
 *		Revision 1.3  2000/12/10 04:05:09  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:18:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XRegion.h>

#include <MQuickDrawUtils.h>
#include <XAutoPtr.h>
#include <XCriticalSection.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XQueue.h>

namespace Whisper {


// ===================================================================================
//	class ZRegionPool
// ===================================================================================
class ZRegionPool {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~ZRegionPool();
						
						ZRegionPool();

	static ZRegionPool* Instance();

private:
						ZRegionPool(const ZRegionPool& rhs);
						
			ZRegionPool& operator=(const ZRegionPool& rhs);

//-----------------------------------
//	API
//
public:	
			RgnHandle 	AcquireRegion();
			
			void 		ReleaseRegion(RgnHandle rgn);
			
//-----------------------------------
//	Member Data
//
protected:
	XQueue<RgnHandle>				mRegions;
 	XCriticalSection				mMutex;

private:
	static XAutoPtr<ZRegionPool>	msInstance;			
	static XCriticalSection			msCreateMutex;
};

XAutoPtr<ZRegionPool>	ZRegionPool::msInstance;		
XCriticalSection 		ZRegionPool::msCreateMutex;

//---------------------------------------------------------------
//
// ZRegionPool::~ZRegionPool
//
//---------------------------------------------------------------
ZRegionPool::~ZRegionPool()
{
	while (!mRegions.empty()) {
		RgnHandle rgn = mRegions.back();
		mRegions.pop_back();
		
		DisposeRgn(rgn);
	}
}


//---------------------------------------------------------------
//
// ZRegionPool::ZRegionPool
//
//---------------------------------------------------------------
ZRegionPool::ZRegionPool()
{
}


//---------------------------------------------------------------
//
// ZRegionPool::Instance								[static]
//
// $$$ This will cause problems with static XRegions (because the
// $$$ pool will be destructed before the static XRegion's dtor
// $$$ runs). If the performance has been improved in Carbon we
// $$$ can maybe get rid of this class altogether.
//
//---------------------------------------------------------------
ZRegionPool* ZRegionPool::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msCreateMutex);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new ZRegionPool);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// ZRegionPool::AcquireRegion
//
//---------------------------------------------------------------
RgnHandle ZRegionPool::AcquireRegion()
{
	RgnHandle rgn = nil;
	
	XEnterCriticalSection enter(mMutex);

	if (!mRegions.empty()) {
		rgn = mRegions.back();		
		mRegions.pop_back();
	
	} else {
		rgn = NewRgn();
		ThrowIfNil(rgn);
	}
	
	return rgn;
}


//---------------------------------------------------------------
//
// ZRegionPool::ReleaseRegion
//
//---------------------------------------------------------------
void ZRegionPool::ReleaseRegion(RgnHandle rgn)
{
	PRECONDITION(rgn != nil);
	
	SetEmptyRgn(rgn);
	
	XEnterCriticalSection enter(mMutex);

	mRegions.push_back(rgn);
}

#pragma mark -

// ===================================================================================
//	Friend Functions
// ===================================================================================

//---------------------------------------------------------------
//
// operator+ (XRegion, XRegion)
//
//---------------------------------------------------------------
XRegion operator+(const XRegion& lhs, const XRegion& rhs)
{
	XRegion newRgn;
	
	UnionRgn(lhs.mRgn, rhs.mRgn, newRgn.mRgn);
	ThrowIfQDError();

	return newRgn;
}


//---------------------------------------------------------------
//
// XRegion::operator+ (XRegion, XPoint)
//
//---------------------------------------------------------------
XRegion operator+(const XRegion& rgn, const XPoint& offset)
{
	XRegion result = rgn;

	Rect src, dst;
	
	src = dst = result.GetEnclosingRect().GetOSRect();
	
	dst.left 	+= offset.x; 
	dst.top 	+= offset.y; 
	dst.right 	+= offset.x; 
	dst.bottom 	+= offset.y; 

	::MapRgn(result, &src, &dst);

	return result;
}


//---------------------------------------------------------------
//
// operator- (XRegion, XRegion)
//
//---------------------------------------------------------------
XRegion operator-(const XRegion& lhs, const XRegion& rhs)
{
	XRegion newRgn;
	
	DiffRgn(lhs.mRgn, rhs.mRgn, newRgn.mRgn);
	ThrowIfQDError();

	return newRgn;
}


//---------------------------------------------------------------
//
// XRegion::operator- (XRegion, XPoint)
//
//---------------------------------------------------------------
XRegion operator-(const XRegion& rgn, const XPoint& offset)
{
	XRegion result = rgn;

	Rect src, dst;
	
	src = dst = result.GetEnclosingRect().GetOSRect();
	
	dst.left 	-= offset.x; 
	dst.top 	-= offset.y; 
	dst.right 	-= offset.x; 
	dst.bottom 	-= offset.y; 

	::MapRgn(result, &src, &dst);

	return result;
}


//---------------------------------------------------------------
//
// operator& (XRegion, XRegion)
//
//---------------------------------------------------------------
XRegion operator&(const XRegion& lhs, const XRegion& rhs)
{
	XRegion newRgn;
	
	SectRgn(lhs.mRgn, rhs.mRgn, newRgn.mRgn);
	ThrowIfQDError();

	return newRgn;
}


//---------------------------------------------------------------
//
// operator^ (XRegion, XRegion)
//
//---------------------------------------------------------------
XRegion operator^(const XRegion& lhs, const XRegion& rhs)
{
	XRegion newRgn;
	
	XorRgn(lhs.mRgn, rhs.mRgn, newRgn.mRgn);
	ThrowIfQDError();

	return newRgn;
}

#pragma mark -

// ===================================================================================
//	class XRegion
// ===================================================================================

//---------------------------------------------------------------
//
// Destructor
//
//---------------------------------------------------------------
XRegion::~XRegion()
{
	ZRegionPool::Instance()->ReleaseRegion(mRgn);
}
	
	
//---------------------------------------------------------------
//
// XRegion::XRegion ()
//
//---------------------------------------------------------------
XRegion::XRegion()
{
	mRgn = ZRegionPool::Instance()->AcquireRegion();
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XRect)
//
//---------------------------------------------------------------
XRegion::XRegion(const XRect& rect)
{
	mRgn = ZRegionPool::Instance()->AcquireRegion();

	Rect temp = rect.GetOSRect();
	RectRgn(mRgn, &temp);
	ThrowIfQDError();
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XRect*, uint32)
//
//---------------------------------------------------------------
XRegion::XRegion(const XRect* rects, uint32 count)
{
	PRECONDITION(rects != nil);
	PRECONDITION(count < 100*1024L);
	
	mRgn = ZRegionPool::Instance()->AcquireRegion();

	RgnHandle temp = ZRegionPool::Instance()->AcquireRegion();

	for (uint32 index = 0; index < count; ++index) {
		Rect rect = rects[index].GetOSRect();
		RectRgn(temp, &rect);
		ThrowIfQDError();

		UnionRgn(mRgn, temp, mRgn);
		ThrowIfQDError();
	}
	
	ZRegionPool::Instance()->ReleaseRegion(temp);
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XRect, int32, int32)
//
//---------------------------------------------------------------
XRegion::XRegion(const XRect& rect, int32 ovalWidth, int32 ovalHeight)
{
	PRECONDITION(ovalWidth >= 0);
	PRECONDITION(ovalHeight >= 0);
	
	MSetPort setter(GetWorkPort());
	
	mRgn = ZRegionPool::Instance()->AcquireRegion();

	OpenRgn();
	Rect temp = rect.GetOSRect();
	FrameRoundRect(&temp, numeric_cast<int16>(ovalWidth), numeric_cast<int16>(ovalHeight));
	CloseRgn(mRgn);

	ThrowIfQDError();
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XPoint, int32, int32)
//
//---------------------------------------------------------------
XRegion::XRegion(const XPoint& center, int32 ovalWidth, int32 ovalHeight)
{
	PRECONDITION(ovalWidth >= 0);
	PRECONDITION(ovalHeight >= 0);
	
	MSetPort setter(GetWorkPort());
	
	mRgn = ZRegionPool::Instance()->AcquireRegion();
	
	XRect frame;
	frame.left   = center.x - ovalWidth/2;
	frame.top    = center.y - ovalHeight/2;
	frame.right  = frame.left + ovalWidth;
	frame.bottom = frame.top + ovalHeight;

	OpenRgn();
	Rect temp = frame.GetOSRect();
	FrameOval(&temp);
	CloseRgn(mRgn);

	ThrowIfQDError();
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XPoint*, uint32)
//
//---------------------------------------------------------------
XRegion::XRegion(const XPoint* vertices, uint32 count)
{
	PRECONDITION(vertices != nil);
	PRECONDITION(count < 100*1024L);
	
	MSetPort setter(GetWorkPort());
	
	mRgn = ZRegionPool::Instance()->AcquireRegion();

	OpenRgn();
	::MoveTo(numeric_cast<int16>(vertices[0].x), numeric_cast<int16>(vertices[0].y));
	for (uint32 index = 1; index < count; index++) {
		LineTo(numeric_cast<int16>(vertices[index].x), numeric_cast<int16>(vertices[index].y));
	}
	LineTo(numeric_cast<int16>(vertices[0].x), numeric_cast<int16>(vertices[0].y));
	CloseRgn(mRgn);

	ThrowIfQDError();
}


//---------------------------------------------------------------
//
// XRegion::XRegion (RgnHandle)
//
//---------------------------------------------------------------
XRegion::XRegion(RgnHandle rgn)
{
	PRECONDITION(rgn != nil);
	
	mRgn = ZRegionPool::Instance()->AcquireRegion();

	CopyRgn(rgn, mRgn);
	ThrowIfQDError();
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XRegion)
//
//---------------------------------------------------------------
XRegion::XRegion(const XRegion& rgn)
{
	mRgn = ZRegionPool::Instance()->AcquireRegion();
	
	CopyRgn(rgn.mRgn, mRgn);
	ThrowIfQDError();
}


//---------------------------------------------------------------
//
// XRegion::operator=
//
//---------------------------------------------------------------
XRegion& XRegion::operator=(const XRegion& rhs)
{
	if (mRgn != rhs.mRgn) {
		RgnHandle temp = ZRegionPool::Instance()->AcquireRegion();

		CopyRgn(rhs.mRgn, temp);
		ThrowIfQDError();

		ZRegionPool::Instance()->ReleaseRegion(mRgn);
		mRgn = temp;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator+= (XRegion)
//
//---------------------------------------------------------------
XRegion& XRegion::operator+=(const XRegion& rhs)
{
	RgnHandle temp = ZRegionPool::Instance()->AcquireRegion();

	UnionRgn(mRgn, rhs.mRgn, temp);
	ThrowIfQDError();

	ZRegionPool::Instance()->ReleaseRegion(mRgn);
	mRgn = temp;

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator+= (XPoint)
//
//---------------------------------------------------------------
XRegion& XRegion::operator+=(const XPoint& offset)
{
	Rect src, dst;
	
	src = dst = this->GetEnclosingRect().GetOSRect();
	
	dst.left 	+= offset.x; 
	dst.top 	+= offset.y; 
	dst.right 	+= offset.x; 
	dst.bottom 	+= offset.y; 

	::MapRgn(mRgn, &src, &dst);

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator-= (XRegion)
//
//---------------------------------------------------------------
XRegion& XRegion::operator-=(const XRegion& rhs)
{
	RgnHandle temp = ZRegionPool::Instance()->AcquireRegion();

	DiffRgn(mRgn, rhs.mRgn, temp);
	ThrowIfQDError();

	ZRegionPool::Instance()->ReleaseRegion(mRgn);
	mRgn = temp;

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator-= (XPoint)
//
//---------------------------------------------------------------
XRegion& XRegion::operator-=(const XPoint& offset)
{
	Rect src, dst;
	
	src = dst = this->GetEnclosingRect().GetOSRect();
	
	dst.left 	-= offset.x; 
	dst.top 	-= offset.y; 
	dst.right 	-= offset.x; 
	dst.bottom 	-= offset.y; 

	::MapRgn(mRgn, &src, &dst);

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator&=
//
//---------------------------------------------------------------
XRegion& XRegion::operator&=(const XRegion& rhs)
{
	RgnHandle temp = ZRegionPool::Instance()->AcquireRegion();

	SectRgn(mRgn, rhs.mRgn, temp);
	ThrowIfQDError();

	ZRegionPool::Instance()->ReleaseRegion(mRgn);
	mRgn = temp;

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator^=
//
//---------------------------------------------------------------
XRegion& XRegion::operator^=(const XRegion& rhs)
{
	RgnHandle temp = ZRegionPool::Instance()->AcquireRegion();

	XorRgn(mRgn, rhs.mRgn, temp);
	ThrowIfQDError();

	ZRegionPool::Instance()->ReleaseRegion(mRgn);
	mRgn = temp;

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::MakeEmpty
//
//---------------------------------------------------------------
void XRegion::MakeEmpty()
{
	SetEmptyRgn(mRgn);
}


//---------------------------------------------------------------
//
// XRegion::MoveTo
//
//---------------------------------------------------------------
void XRegion::MoveTo(const XPoint& pt)
{
	XPoint offset(pt - this->GetEnclosingRect()[topLeft]);
	
	::OffsetRgn(mRgn, numeric_cast<int16>(offset.x), numeric_cast<int16>(offset.y));
}


//---------------------------------------------------------------
//
// XRegion::Map
//
//---------------------------------------------------------------
void XRegion::Map(const XRect& srcRect, const XRect& dstRect)	
{
	RgnHandle temp = ZRegionPool::Instance()->AcquireRegion();

	CopyRgn(mRgn, temp);
	ThrowIfQDError();

	Rect src = srcRect.GetOSRect();
	Rect dst = dstRect.GetOSRect();

	::MapRgn(temp, &src, &dst);
	ThrowIfQDError();

	ZRegionPool::Instance()->ReleaseRegion(mRgn);
	mRgn = temp;
}


//---------------------------------------------------------------
//
// XRegion::Contains (XRect)
//
//---------------------------------------------------------------
bool XRegion::Contains(const XRect& rect) const				
{
	XRegion rhs(rect);
	
	XRegion intersection = *this & rhs;
	
	bool contains = rhs == intersection;
	
	return contains;
}


}	// namespace Whisper


