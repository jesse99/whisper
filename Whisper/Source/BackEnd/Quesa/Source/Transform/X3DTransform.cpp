/*
 *  File:       X3DTransform.cpp
 *  Summary:	Base class for QuickDraw 3D transforms.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTransform.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:28  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:32:50  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:22:28  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DTransform.h>

#include <QuesaTransform.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DTransform
// ===================================================================================

//---------------------------------------------------------------
//
// X3DTransform::~X3DTransform
//
//---------------------------------------------------------------
X3DTransform::~X3DTransform()
{
}


//---------------------------------------------------------------
//
// X3DTransform::X3DTransform
//
//---------------------------------------------------------------
X3DTransform::X3DTransform(TQ3TransformObject object) : X3DSharedObject(object)
{
	this->ConfirmType(kQ3ShapeTypeTransform);
}


//---------------------------------------------------------------
//
// X3DTransform::Clone
//
//---------------------------------------------------------------
X3DTransform* X3DTransform::Clone() const
{
	X3DTransform* object = new X3DTransform(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DTransform::GetMatrix
//
//---------------------------------------------------------------
X3DMatrix X3DTransform::GetMatrix() const
{
	X3DMatrix matrix;
	
	Q3Transform_GetMatrix(mObject, matrix);
	
	return matrix;
}


//---------------------------------------------------------------
//
// X3DTransform::Submit
//
//---------------------------------------------------------------
void X3DTransform::Submit(TQ3ViewObject view) const
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Transform_Submit(mObject, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTransform::IsCompatible				 			[static]
//	
//---------------------------------------------------------------
bool X3DTransform::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ShapeTypeTransform);
	
	return compatible;
}


}	// namespace Whisper
