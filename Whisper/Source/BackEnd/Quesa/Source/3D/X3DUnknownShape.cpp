/*
 *  File:       X3DUnknownShape.cpp
 *  Summary:	QuickDraw 3D unknown shape object.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DUnknownShape.cpp,v $
 *		Revision 1.3  2001/04/14 07:33:19  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:23:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <->	  2/14/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DUnknownShape.h>

namespace Whisper {


// ===================================================================================
//	class X3DUnknownShape
// ===================================================================================

//---------------------------------------------------------------
//
// X3DUnknownShape::~X3DUnknownShape
//
//---------------------------------------------------------------
X3DUnknownShape::~X3DUnknownShape()
{
}


//---------------------------------------------------------------
//
// X3DUnknownShape::X3DUnknownShape (TQ3LightObject)
//
//---------------------------------------------------------------
X3DUnknownShape::X3DUnknownShape(TQ3LightObject object) : X3DShape(object)
{
}


//---------------------------------------------------------------
//
// X3DUnknownShape::Clone
//
//---------------------------------------------------------------
X3DUnknownShape* X3DUnknownShape::Clone() const
{
	X3DUnknownShape* object = new X3DUnknownShape(mObject);
	object->Detach();
	
	return object;
}


}	// namespace Whisper
