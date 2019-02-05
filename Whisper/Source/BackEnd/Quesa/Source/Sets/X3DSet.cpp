/*
 *  File:       X3DSet.cpp
 *  Summary:	QuickDraw 3D set object.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DSet.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:48  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:31:34  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:20:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DSet.h>

#include <X3DUtils.h>
#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class X3DSet
// ===================================================================================

//---------------------------------------------------------------
//
// X3DSet::~X3DSet
//
//---------------------------------------------------------------
X3DSet::~X3DSet()
{
}


//---------------------------------------------------------------
//
// X3DSet::X3DSet
//
//---------------------------------------------------------------
X3DSet::X3DSet() : X3DSharedObject(Q3Set_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSet::X3DSet
//
//---------------------------------------------------------------
X3DSet::X3DSet(TQ3SetObject object) : X3DSharedObject(object)
{
	this->ConfirmType(kQ3SharedTypeSet);
}


//---------------------------------------------------------------
//
// X3DSet::Clone
//
//---------------------------------------------------------------
X3DSet* X3DSet::Clone() const
{
	X3DSet* object = new X3DSet(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DSet::Add
//
//---------------------------------------------------------------
void X3DSet::Add(TQ3ElementType type, const void* data)
{
	PRECONDITION(data != nil);
	
	TQ3Status status = Q3Set_Add(mObject, type, data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSet::Get
//
//---------------------------------------------------------------
void X3DSet::Get(TQ3ElementType type, void* data) const
{
	PRECONDITION(data != nil);
	
	TQ3Status status = Q3Set_Get(mObject, type, data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSet::Contains
//
//---------------------------------------------------------------
bool X3DSet::Contains(TQ3ElementType type) const
{
	TQ3Boolean contains = Q3Set_Contains(mObject, type);
	
	return contains;
}


//---------------------------------------------------------------
//
// X3DSet::Clear
//
//---------------------------------------------------------------
void X3DSet::Clear(TQ3ElementType type)
{
	TQ3Status status = Q3Set_Clear(mObject, type);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSet::Empty
//
//---------------------------------------------------------------
void X3DSet::Empty()
{
	TQ3Status status = Q3Set_Empty(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSet::GetNextType
//
//---------------------------------------------------------------
TQ3ElementType X3DSet::GetNextType(TQ3ElementType lastType) const
{
	TQ3ElementType nextType = lastType;
	
	TQ3Status status = Q3Set_GetNextElementType(mObject, &nextType);
	ThrowIf3DError(status);
	
	return nextType;
}


//---------------------------------------------------------------
//
// X3DSet::IsCompatible 								[static]
//	
//---------------------------------------------------------------
bool X3DSet::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3SharedTypeSet);
	
	return compatible;
}


}	// namespace Whisper
