/*
 *  File:       X3DGroup.cpp
 *  Summary:	Base class for QuickDraw 3D group objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DGroup.cpp,v $
 *		Revision 1.3  2001/04/14 07:28:45  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:15:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DGroup.h>

#include <QuesaGroup.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DGroup
// ===================================================================================

//---------------------------------------------------------------
//
// X3DGroup::~X3DGroup
//
//---------------------------------------------------------------
X3DGroup::~X3DGroup()
{
}


//---------------------------------------------------------------
//
// X3DGroup::X3DGroup ()
//
//---------------------------------------------------------------
X3DGroup::X3DGroup() : X3DShape(Q3Group_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);	// X3DSharedObject ctor will have added a reference
	ASSERT(status == kQ3Success);
}


//---------------------------------------------------------------
//
// X3DGroup::X3DGroup (TQ3GroupObject)
//
//---------------------------------------------------------------
X3DGroup::X3DGroup(TQ3GroupObject object) : X3DShape(object)
{
	this->ConfirmType(kQ3ShapeTypeGroup);
}


//---------------------------------------------------------------
//
// X3DGroup::Clone
//
//---------------------------------------------------------------
X3DGroup* X3DGroup::Clone() const
{
	X3DGroup* object = new X3DGroup(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DGroup::CountObjects
//
//---------------------------------------------------------------
uint32 X3DGroup::CountObjects() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Group_CountObjects(mObject, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DGroup::CountObjectsOfType
//
//---------------------------------------------------------------
uint32 X3DGroup::CountObjectsOfType(TQ3ObjectType type) const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Group_CountObjectsOfType(mObject, type, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DGroup::EmptyObjects
//
//---------------------------------------------------------------
void X3DGroup::EmptyObjects()
{
	TQ3Status status = Q3Group_EmptyObjects(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DGroup::EmptyObjectsOfType
//
//---------------------------------------------------------------
void X3DGroup::EmptyObjectsOfType(TQ3ObjectType type)
{
	TQ3Status status = Q3Group_EmptyObjectsOfType(mObject, type);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DGroup::IsCompatible 								[static]
//	
//---------------------------------------------------------------
bool X3DGroup::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ShapeTypeGroup);
	
	return compatible;
}


}	// namespace Whisper
