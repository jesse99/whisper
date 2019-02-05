/*
 *  File:       XDraw.cpp
 *  Summary:   	IDraw helper class.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDraw.cpp,v $
 *		Revision 1.5  2001/02/17 09:28:18  jesjones
 *		Removed UI_EXPORT.
 *		
 *		Revision 1.4  2001/01/22 02:02:03  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/08 08:09:54  jesjones
 *		Uses typedefs, renamed a variable.
 *		
 *		Revision 1.2  2000/11/09 12:33:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDraw.h>

#include <IDrawExtensible.h>
#include <IDrawHelper.h>
#include <IGeometry.h>
#include <XShapes.h>

namespace Whisper {


// ===================================================================================
//	class XDraw
//!		IDraw helper class.
// ===================================================================================
class XDraw : public IDraw {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDraw();
	
						XDraw(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		HandleDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);
};

DEFINE_INTERFACE_FACTORY(XDraw)

//---------------------------------------------------------------
//
// XDraw::~XDraw
//
//---------------------------------------------------------------
XDraw::~XDraw()
{
}


//---------------------------------------------------------------
//
// XDraw::XDraw
//
//---------------------------------------------------------------
XDraw::XDraw(XBoss* boss) 
{	
	IDraw::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDraw::HandleDraw
//
//---------------------------------------------------------------
void XDraw::HandleDraw(XDrawContext& context, const XRegion& dirtyRgn, bool isActive)
{		
	if (!dirtyRgn.IsEmpty()) {	
#if DEBUG
		// Draw the dirty area in bright red to make it easier
		// to see which parts are being redrawn.
		if (gIntenseDebugging) 
			XRegionShape::Fill(context, dirtyRgn, kRGBRed);
#endif

		IGeometryPtr geometry(this);
		XRect extent = geometry->GetExtent();

		bool handled = false;
		IDrawExtensiblePtr extensible(this);
		if (extensible)
			handled = extensible->HandleDraw(context, extent, true, isActive);
		
		if (!handled) {
			IDrawHelperPtr draw(this);
			draw->OnDraw(context, dirtyRgn, isActive);
		}
		
		if (extensible)
			(void) extensible->HandleDraw(context, extent, false, isActive);
	}
}


}	// namespace Whisper
