/*
 *  File:       X3DGeometry.cpp
 *  Summary:	Base class for QuickDraw 3D geometry objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DGeometry.cpp,v $
 *		Revision 1.4  2001/04/21 03:35:55  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:28:39  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:15:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XWhisperHeader.h>
#include <X3DGeometry.h>

#include <X3DUtils.h>
#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class X3DGeometry
// ===================================================================================

//---------------------------------------------------------------
//
//	X3DGeometry::~X3DGeometry
//
//---------------------------------------------------------------
X3DGeometry::~X3DGeometry()
{
}


//---------------------------------------------------------------
//
//	X3DGeometry::X3DGeometry
//
//---------------------------------------------------------------
X3DGeometry::X3DGeometry(TQ3GeometryObject object) : X3DShape(object)
{
	this->ConfirmType(kQ3ShapeTypeGeometry);
}


//---------------------------------------------------------------
//
// X3DGeometry::Clone
//
//---------------------------------------------------------------
X3DGeometry* X3DGeometry::Clone() const
{
	X3DGeometry* object = new X3DGeometry(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
//	X3DGeometry::Submit
//
//---------------------------------------------------------------
void X3DGeometry::Submit(TQ3ViewObject view) const
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Geometry_Submit(mObject, view);
	ThrowIf3DError(status);
}

			
//---------------------------------------------------------------
//
//	X3DGeometry::GetAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DGeometry::GetAttributeSet() const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3Geometry_GetAttributeSet(mObject, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}


//---------------------------------------------------------------
//
//	X3DGeometry::SetAttributeSet
//
//---------------------------------------------------------------
void X3DGeometry::SetAttributeSet(TQ3AttributeSet attributes)
{
	TQ3Status status = Q3Geometry_SetAttributeSet(mObject, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DGeometry::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DGeometry::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ShapeTypeGeometry);
	
	return compatible;
}


}	// namespace Whisper
