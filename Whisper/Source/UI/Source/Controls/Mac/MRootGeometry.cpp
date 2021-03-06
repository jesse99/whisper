/*
 *  File:       MRootGeometry.cpp
 *  Summary:   	IGeometry that works with root controls.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MRootGeometry.cpp,v $
 *		Revision 1.3  2000/12/14 08:28:46  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:19:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: MRootGeometry.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/14 08:28:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	  8/04/00	JDJ		Mixes in IParentChangedSize.
 *		 <1>	  7/14/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IGeometry.h>
#include <IParentChangedSize.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IWindow.h>

namespace Whisper {


// ===================================================================================
//	class MRootGeometry
//!		IGeometry that works with root controls.
// ===================================================================================
class MRootGeometry : public IGeometry, public IParentChangedSize {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~MRootGeometry();
	
						MRootGeometry(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual XPoint 		GetLocation() const;
	virtual void 		SetLocation(const XPoint& newLoc, bool redraw = kRedraw);
	
	virtual int32 		GetWidth() const;	
	virtual int32 		GetHeight() const;
	virtual XSize 		GetSize() const;
	virtual void 		SetSize(const XSize& newSize, bool redraw = kRedraw);

	virtual XRect 		GetFrame() const;
	virtual XRect 		GetExtent() const;
	virtual XRegion 	GetVisibleExtent() const;
	virtual void 		SetFrame(const XRect& newFrame, bool redraw = kRedraw);
			
	virtual XSize 		GetMinSize() const;
	virtual XSize 		GetMaxSize() const;

	virtual void		SetMinSize(const XSize& newMin, bool redraw = kRedraw);
	virtual void		SetMaxSize(const XSize& newMax, bool redraw = kRedraw);
	virtual void 		SetMinMaxSize(const XSize& newMin, const XSize& newMax, bool redraw = kRedraw);

	virtual void 		HandleParentSizeChange(const XSize& delta, bool redraw = kRedraw);
};

DEFINE_INTERFACE_FACTORY(MRootGeometry)

//---------------------------------------------------------------
//
// MRootGeometry::~MRootGeometry
//
//---------------------------------------------------------------
MRootGeometry::~MRootGeometry()
{
}


//---------------------------------------------------------------
//
// MRootGeometry::MRootGeometry
//
//---------------------------------------------------------------
MRootGeometry::MRootGeometry(XBoss* boss) 
{	
	IGeometry::DoSetBoss(boss);
	IParentChangedSize::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// MRootGeometry::GetLocation
//
//---------------------------------------------------------------
XPoint MRootGeometry::GetLocation() const
{
	return kZeroPt;
}


//---------------------------------------------------------------
//
// MRootGeometry::SetLocation
//
//---------------------------------------------------------------
void MRootGeometry::SetLocation(const XPoint& newLoc, bool redraw)
{
	UNUSED(newLoc);
	UNUSED(redraw);
	
	DEBUGSTR("Can't set the location of a root control!");
}

	
//---------------------------------------------------------------
//
// MRootGeometry::GetSize
//
// Note that we don't want to use the MControlGeometry implementation:
// the root control contrlRect is always (-32767, -32767, 32767, 32767)
// which causes overflows when we need to convert back to 16-bit
// coordinates. 
//
//---------------------------------------------------------------
XSize MRootGeometry::GetSize() const
{
	const IGeometry* thisPtr = this;
	IWindowPtr window = GetParent<IWindow>(thisPtr);
	IGeometryPtr geometry(window);
	 
	XSize size = geometry->GetSize();
	
	return size;
}


//---------------------------------------------------------------
//
// MRootGeometry::GetWidth
//
//---------------------------------------------------------------
int32 MRootGeometry::GetWidth() const
{
	const IGeometry* thisPtr = this;
	IWindowPtr window = GetParent<IWindow>(thisPtr);
	IGeometryPtr geometry(window);
	 
	XSize size = geometry->GetSize();
	
	return size.width;
}


//---------------------------------------------------------------
//
// MRootGeometry::GetHeight
//
//---------------------------------------------------------------
int32 MRootGeometry::GetHeight() const
{
	const IGeometry* thisPtr = this;
	IWindowPtr window = GetParent<IWindow>(thisPtr);
	IGeometryPtr geometry(window);
	 
	XSize size = geometry->GetSize();
	
	return size.height;
}


//---------------------------------------------------------------
//
// MRootGeometry::SetSize
//
//---------------------------------------------------------------
void MRootGeometry::SetSize(const XSize& newSize, bool redraw)
{
	UNUSED(newSize);
	UNUSED(redraw);
	
	DEBUGSTR("Can't set the size of a root control!");
}


//---------------------------------------------------------------
//
// MRootGeometry::GetFrame
//
//---------------------------------------------------------------
XRect MRootGeometry::GetFrame() const
{
	XRect extent(kZeroPt, this->GetSize());

	return extent;
}


//---------------------------------------------------------------
//
// MRootGeometry::GetExtent
//
//---------------------------------------------------------------
XRect MRootGeometry::GetExtent() const
{
	XRect extent(kZeroPt, this->GetSize());

	return extent;
}


//---------------------------------------------------------------
//
// MRootGeometry::GetVisibleExtent
//
//---------------------------------------------------------------
XRegion MRootGeometry::GetVisibleExtent() const
{	
	XRegion extent = this->GetExtent();

	const IGeometry* thisPtr = this;
	IConstGeometryPtr parent = GetConstParent<IGeometry>(thisPtr);
	extent &= parent->GetVisibleExtent();
		
	return extent;
}


//---------------------------------------------------------------
//
// MRootGeometry::SetFrame
//
//---------------------------------------------------------------
void MRootGeometry::SetFrame(const XRect& newFrame, bool redraw)
{
	UNUSED(newFrame);
	UNUSED(redraw);
	
	DEBUGSTR("Can't set the frame of a root control!");
}


//---------------------------------------------------------------
//
// MRootGeometry::GetMinSize
//
//---------------------------------------------------------------
XSize MRootGeometry::GetMinSize() const 				
{
	const IGeometry* thisPtr = this;
	IWindowPtr window = GetParent<IWindow>(thisPtr);
	IGeometryPtr geometry(window);
	
	XSize minSize = geometry->GetMinSize();
	
	return minSize;
}


//---------------------------------------------------------------
//
// MRootGeometry::GetMaxSize
//
//---------------------------------------------------------------
XSize MRootGeometry::GetMaxSize() const 				
{
	const IGeometry* thisPtr = this;
	IWindowPtr window = GetParent<IWindow>(thisPtr);
	IGeometryPtr geometry(window);
	
	XSize maxSize = geometry->GetMaxSize();
	
	return maxSize;
}


//---------------------------------------------------------------
//
// MRootGeometry::SetMinSize
//
//---------------------------------------------------------------
void MRootGeometry::SetMinSize(const XSize& newMin, bool redraw)
{
	UNUSED(newMin);
	UNUSED(redraw);
	
	DEBUGSTR("Can't set the min size of a root control!");
}


//---------------------------------------------------------------
//
// MRootGeometry::SetMaxSize
//
//---------------------------------------------------------------
void MRootGeometry::SetMaxSize(const XSize& newMax, bool redraw)
{
	UNUSED(newMax);
	UNUSED(redraw);
	
	DEBUGSTR("Can't set the max size of a root control!");
}


//---------------------------------------------------------------
//
// MRootGeometry::SetMinMaxSize
//
//---------------------------------------------------------------
void MRootGeometry::SetMinMaxSize(const XSize& newMin, const XSize& newMax, bool redraw)
{
	UNUSED(newMin);
	UNUSED(newMax);
	UNUSED(redraw);
	
	DEBUGSTR("Can't set the min and max sizes of a root control!");
}


//---------------------------------------------------------------
//
// MRootGeometry::HandleParentSizeChange
//
//---------------------------------------------------------------
void MRootGeometry::HandleParentSizeChange(const XSize& delta, bool redraw)
{
	IGeometry* thisPtr = this;
	IHierarchyPtr root(thisPtr);
	
	IHierarchy::iterator iter = root->begin();
	while (iter != root->end()) {
		IHierarchyPtr child = *iter;
		++iter;
		
		IParentChangedSizePtr notify(child);
		if (notify)
			notify->HandleParentSizeChange(delta, redraw);
	}
}


}	// namespace Whisper
