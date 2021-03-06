/*
 *  File:       XShape.h
 *  Summary:	An abstract base class for objects that know how to draw themselves
 *				into an XDrawContext.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XShape.h,v $
 *		Revision 1.4  2001/03/05 05:43:23  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:55:33  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:51:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XRect.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XDrawContext;


// ===================================================================================
//	class XShape
//!		An abstract base class for objects that know how to draw themselves into an XDrawContext.
//		$$$ Add HitTest method (ODF has this...) 
// ===================================================================================
class GRAPHICS_EXPORT XShape {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XShape();
						
						XShape();
						
private:
						XShape(const XShape& rhs);
						
			XShape&		operator=(const XShape& rhs);

//-----------------------------------
//	API
//
public:
	virtual void 		Draw(XDrawContext& context) const = 0;
						/**< Draws the shape into the context. */

	virtual XRect 		GetBounds() const = 0;
						/**< Returns a rectangle enclosing the shape. */
						
	virtual void 		MoveBy(const XSize& delta) = 0;
			void 		MoveTo(const XPoint& loc)				{this->MoveBy(XSize(loc - this->GetBounds()[topLeft]));}

	virtual XShape*		Clone() const = 0;
						/**< Returns a copy of the shape. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

