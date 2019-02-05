/*
 *  File:       CDrawDoc.cpp
 *  Summary:   	IDrawHelper implementation used with the custom DropSkeleton control.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDrawDoc.cpp,v $
 *		Revision 1.3  2001/02/17 10:24:54  jesjones
 *		Tweaked some comments.
 *		
 *		Revision 1.2  2001/02/07 02:27:55  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/31 04:23:31  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IDrawHelper.h>

#include <IHierarchy.h>
#include <XShapes.h>

#include "IDocInfo.h"


// ===================================================================================
//	class CDrawDoc
//!		IDrawHelper implementation used with the custom DropSkeleton control.
// ===================================================================================
class CDrawDoc : public IDrawHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CDrawDoc();
	
						CDrawDoc(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);
};

DEFINE_INTERFACE_FACTORY(CDrawDoc)

//---------------------------------------------------------------
//
// CDrawDoc::~CDrawDoc
//
//---------------------------------------------------------------
CDrawDoc::~CDrawDoc()
{
}


//---------------------------------------------------------------
//
// CDrawDoc::CDrawDoc
//
//---------------------------------------------------------------
CDrawDoc::CDrawDoc(XBoss* boss)
{	
	IDrawHelper::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CDrawDoc::OnDraw
//
//---------------------------------------------------------------
void CDrawDoc::OnDraw(XDrawContext& context, const XRegion& dirtyRgn, bool isActive)
{	
	UNUSED(isActive);
	
	IConstDocInfoPtr doc = GetParent<IDocInfo>(this);
	
	XPen pen(kRGBBlack, doc->GetPenWidth());
	XBrush brush(doc->GetColor());

	XRegionShape::Fill(context, dirtyRgn, kRGBWhite);	

	uint32 count = doc->GetNumRects();
	for (uint32 index = 0; index < count; ++index) {
		XRect rect = doc->GetRect(index);
			
		if (dirtyRgn.Intersects(rect)) {
			XRectShape::Fill(context, rect, brush);	
			XRectShape::Frame(context, rect, pen);	
		}
	}
}
