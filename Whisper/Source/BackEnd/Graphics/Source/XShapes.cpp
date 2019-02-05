/*
 *  File:       XShapes.cpp
 *  Summary:	Classes that draw into an XDrawContext.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XShapes.cpp,v $
 *		Revision 1.4  2001/04/27 04:36:20  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/03/06 07:33:11  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.2  2000/11/09 12:51:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XShapes.h>

#include <XAutoPtr.h>

namespace Whisper {


// ===================================================================================
//	class XGroupShape
// ===================================================================================

//---------------------------------------------------------------
//
// XGroupShape::~XGroupShape
//
//---------------------------------------------------------------
XGroupShape::~XGroupShape()
{	
	this->Clear();
}


//---------------------------------------------------------------
//
// XGroupShape::XGroupShape 
//
//---------------------------------------------------------------
XGroupShape::XGroupShape(const XPoint& where) 
{
	mLoc = where;
	mExtent = kZeroRect;
	mDirty = false;
}


//---------------------------------------------------------------
//
// XGroupShape::Clone
//
//---------------------------------------------------------------
XGroupShape* XGroupShape::Clone() const
{
	XAutoPtr<XGroupShape> group(new XGroupShape(mLoc));
	
	const_iterator iter = mShapes.begin();
	while (iter != mShapes.end()) {
		const XShape* shape = *iter;
		++iter;
		
		group->PushBack(shape->Clone());
	}
	
	return group.Release();
}


//---------------------------------------------------------------
//
// XGroupShape::Erase
//
//---------------------------------------------------------------
void XGroupShape::Erase(const iterator& iter)		
{
	PRECONDITION(iter != this->end());
	
	mDirty = true; 
	
	delete *iter;
	
	mShapes.erase(iter);
}


//---------------------------------------------------------------
//
// XGroupShape::Clear
//
//---------------------------------------------------------------
void XGroupShape::Clear()							
{
	if (!mShapes.empty()) {
		mDirty = true; 
	
		iterator iter = mShapes.begin();
		while (iter != mShapes.end()) {
			const XShape* shape = *iter;
			++iter;
			
			delete shape;
		}

		mShapes.clear();
	}
}


//---------------------------------------------------------------
//
// XGroupShape::GetBounds
//
//---------------------------------------------------------------
XRect XGroupShape::GetBounds() const
{
	if (mDirty) {
		mExtent = kZeroRect;
	
		const_iterator iter = mShapes.begin();
		while (iter != mShapes.end()) {
			const XShape* shape = *iter;
			++iter;
			
			mExtent |= shape->GetBounds();
		}

		mDirty = false;
	}
	
	XRect bounds = mExtent + mLoc;
		
	return bounds;
}


//---------------------------------------------------------------
//
// XGroupShape::MoveBy
//
//---------------------------------------------------------------
void XGroupShape::MoveBy(const XSize& delta)
{
	mLoc += delta;
}


//---------------------------------------------------------------
//
// XGroupShape::Draw
//
//---------------------------------------------------------------
void XGroupShape::Draw(XDrawContext& context) const
{
	XSaveContext saver(context.GetOSContext());
	
	XPoint origin = context.GetOrigin();
	context.SetOrigin(origin + mLoc);
	
	const_iterator iter = mShapes.begin();
	while (iter != mShapes.end()) {
		const XShape* shape = *iter;
		++iter;
		
		shape->Draw(context);
	}
}


}	// namespace Whisper

