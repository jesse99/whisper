/*
 *  File:       X3DUnknownShape.h
 *  Summary:	QuickDraw 3D unknown shape object.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DUnknownShape.h,v $
 *		Revision 1.3  2000/12/13 08:55:38  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:23:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DUnknownShape.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:55:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  2/14/97	JDJ		Created.
 */

#pragma once

#include <X3DShape.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DUnknownShape
//!		The class returned by X3DDisplayIterator when it can't find a C++ class matching the display object.
// ===================================================================================
class QUESA_EXPORT X3DUnknownShape : public X3DShape {

	typedef X3DShape Inherited;

public:
	virtual				~X3DUnknownShape();
						
						X3DUnknownShape(TQ3ShapeObject object);

	virtual X3DUnknownShape* Clone() const;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
