/*
 *  File:       X3DAttributeSet.cpp
 *  Summary:	QuickDraw 3D attribute set object.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DAttributeSet.cpp,v $
 *		Revision 1.4  2001/04/21 03:34:59  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:27:17  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:13:37  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DAttributeSet.h>

#include <X3DUtils.h>
#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class X3DAttributeSet
// ===================================================================================

//---------------------------------------------------------------
//
// X3DAttributeSet::~X3DAttributeSet
//
//---------------------------------------------------------------
X3DAttributeSet::~X3DAttributeSet()
{
}


//---------------------------------------------------------------
//
// X3DAttributeSet::X3DAttributeSet
//
//---------------------------------------------------------------
X3DAttributeSet::X3DAttributeSet() : X3DSet(Q3AttributeSet_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DAttributeSet::X3DAttributeSet
//
//---------------------------------------------------------------
X3DAttributeSet::X3DAttributeSet(TQ3SetObject object) : X3DSet(object)
{
	this->ConfirmType(kQ3SetTypeAttribute);
}


//---------------------------------------------------------------
//
// X3DAttributeSet::Clone
//
//---------------------------------------------------------------
X3DAttributeSet* X3DAttributeSet::Clone() const
{
	X3DAttributeSet* object = new X3DAttributeSet(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DAttributeSet::Submit (TQ3AttributeType, const void*, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DAttributeSet::Submit(TQ3AttributeType type, const void* data, TQ3ViewObject view)
{
	PRECONDITION(data != nil);
	
	TQ3Status status = Q3Attribute_Submit(type, data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DAttributeSet::Submit (TQ3AttributeSet, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DAttributeSet::Submit(TQ3AttributeSet attributes, TQ3ViewObject view)
{
	TQ3Status status = Q3AttributeSet_Submit(attributes, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DAttributeSet::Submit (TQ3ViewObject)				[static]
//
//---------------------------------------------------------------
void X3DAttributeSet::Submit(TQ3ViewObject view)
{
	TQ3Status status = Q3AttributeSet_Submit(mObject, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DAttributeSet::IsCompatible 						[static]
//
//---------------------------------------------------------------
bool X3DAttributeSet::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3SetTypeAttribute);
	
	return compatible;
}


}	// namespace Whisper

