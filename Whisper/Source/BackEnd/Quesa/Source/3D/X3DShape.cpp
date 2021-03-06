/*
 *  File:       X3DShape.cpp
 *  Summary:	Base class for QuickDraw 3D shape objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DShape.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:55  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:31:53  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:20:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DShape.h>

#include <X3DUtils.h>
#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class X3DShape
// ===================================================================================

//---------------------------------------------------------------
//
// X3DShape::~X3DShape
//
//---------------------------------------------------------------
X3DShape::~X3DShape()
{
}


//---------------------------------------------------------------
//
// X3DShape::X3DShape
//
//---------------------------------------------------------------
X3DShape::X3DShape(TQ3ShapeObject object) : X3DSharedObject(object)
{
	this->ConfirmType(kQ3SharedTypeShape);
}


//---------------------------------------------------------------
//
// X3DShape::Clone
//
//---------------------------------------------------------------
X3DShape* X3DShape::Clone() const
{
	X3DShape* object = new X3DShape(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DShape::AddElement
//
//---------------------------------------------------------------
void X3DShape::AddElement(TQ3ElementType type, const void* data)
{
	PRECONDITION(data != nil);
	
	TQ3Status status = Q3Shape_AddElement(*this, type, data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DShape::GetElement
//
// $$ It's be more convenient if the set methods were dropped
// $$ and this and some of the other methods also checked the
// $$ set?
//
//---------------------------------------------------------------
void X3DShape::GetElement(TQ3ElementType type, void* data) const
{
	PRECONDITION(data != nil);
	
	TQ3Status status = Q3Shape_GetElement(mObject, type, data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DShape::ContainsElement
//
//---------------------------------------------------------------
bool X3DShape::ContainsElement(TQ3ElementType type) const
{	
	bool has = Q3Shape_ContainsElement(mObject, type);
	
	return has;
}


//---------------------------------------------------------------
//
// X3DShape::GetNextElementType
//
//---------------------------------------------------------------
TQ3ElementType X3DShape::GetNextElementType(TQ3ElementType inType) const
{
	TQ3ElementType type = inType;

	TQ3Status status = Q3Shape_GetNextElementType(mObject, &type);
	ThrowIf3DError(status);
	
	return type;
}


//---------------------------------------------------------------
//
// X3DShape::EmptyElements
//
//---------------------------------------------------------------
void X3DShape::EmptyElements()
{
	TQ3Status status = Q3Shape_EmptyElements(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DShape::ClearElement
//
//---------------------------------------------------------------
void X3DShape::ClearElement(TQ3ElementType type)
{
	if (type != kQ3ElementTypeNone) {
		TQ3Status status = Q3Shape_ClearElement(mObject, type);
		ThrowIf3DError(status);
	}
}


//---------------------------------------------------------------
//
// X3DShape::GetSet
//
//---------------------------------------------------------------
TQ3SetObject X3DShape::GetSet() const
{
	TQ3SetObject theSet = nil;

	TQ3Status status = Q3Shape_GetSet(mObject, &theSet);
	ThrowIf3DError(status);
	
	return theSet;
}


//---------------------------------------------------------------
//
// X3DShape::SetSet
//
//---------------------------------------------------------------
void X3DShape::SetSet(TQ3SetObject theSet)
{
	TQ3Status status = Q3Shape_SetSet(mObject, theSet);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DShape::IsCompatible 								[static]
//	
//---------------------------------------------------------------
bool X3DShape::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3SharedTypeShape);
	
	return compatible;
}


}	// namespace Whisper

