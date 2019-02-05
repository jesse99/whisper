/*
 *  File:       X3DSharedObject.cpp
 *  Summary:	Base class for QuickDraw 3D shared objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DSharedObject.cpp,v $
 *		Revision 1.3  2001/04/14 07:31:56  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:20:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	 12/02/97	JDJ		Added copy ctor, assignment operator, HasMoreThanOneReference,
 *									and Detach. No longer descends from TReferenceCountedMixin.
 *		 <1>	  1/13/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DSharedObject.h>

#include <QuesaIO.h>

#include <X3DUtils.h>
#include <XDebug.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class X3DSharedObject
// ===================================================================================

//---------------------------------------------------------------
//
// X3DSharedObject::~X3DSharedObject
//
//---------------------------------------------------------------
X3DSharedObject::~X3DSharedObject()
{
}


//---------------------------------------------------------------
//
// X3DSharedObject::X3DSharedObject (TQ3SharedObject)
//
//---------------------------------------------------------------
X3DSharedObject::X3DSharedObject(TQ3SharedObject object) : X3DObject(object)
{
	this->ConfirmType(kQ3ObjectTypeShared);
	
	// Since we're not performing a take we need to acquire a new
	// rewference to the object.
	(void) Q3Shared_GetReference(mObject);
}


//---------------------------------------------------------------
//
// X3DSharedObject::X3DSharedObject (X3DSharedObject)
//
//---------------------------------------------------------------
X3DSharedObject::X3DSharedObject(const X3DSharedObject& rhs) : X3DObject(rhs.mObject)
{
	// Bump up the QD 3D object's ref count (ie instead of copying
	// the 3D data we're copying a handle to the data).
	(void) Q3Shared_GetReference(mObject);		
}


//---------------------------------------------------------------
//
// X3DSharedObject::Clone
//
//---------------------------------------------------------------
X3DSharedObject* X3DSharedObject::Clone() const
{
	X3DSharedObject* object = new X3DSharedObject(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DSharedObject::operator=
//
//---------------------------------------------------------------
X3DSharedObject& X3DSharedObject::operator=(const X3DSharedObject& rhs)
{
	if (mObject != rhs.mObject) {
		TQ3Status status = Q3Object_Dispose(mObject);
		ASSERT(status == kQ3Success);
		
		mObject = rhs.mObject;
		(void) Q3Shared_GetReference(mObject);		
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DSharedObject::HasMoreThanOneReference
//
//---------------------------------------------------------------
bool X3DSharedObject::HasMoreThanOneReference() const
{
	bool has = Q3Shared_IsReferenced(mObject);
	
	return has;
}


//---------------------------------------------------------------
//
// X3DSharedObject::Detach
//
//---------------------------------------------------------------
void X3DSharedObject::Detach()
{
	if (this->HasMoreThanOneReference()) {
		TQ3Object newObject = Q3Object_Duplicate(mObject);
		ThrowIfNil(newObject);

		TQ3Status status = Q3Object_Dispose(mObject);
		ASSERT(status == kQ3Success);
		
		mObject = newObject;
	}	
}


//---------------------------------------------------------------
//
// X3DSharedObject::Edited
//
//---------------------------------------------------------------
void X3DSharedObject::Edited()
{
	TQ3Status status = Q3Shared_Edited(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSharedObject::ClearEditTracking
//
//---------------------------------------------------------------
void X3DSharedObject::ClearEditTracking()
{
	TQ3Status status = Q3Shared_ClearEditTracking(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSharedObject::GetEditTrackingState
//
//---------------------------------------------------------------
bool X3DSharedObject::GetEditTrackingState() const
{
	TQ3Boolean state = Q3Shared_GetEditTrackingState(mObject);
	
	return state;
}


//---------------------------------------------------------------
//
// X3DSharedObject::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DSharedObject::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ObjectTypeShared);
	
	return compatible;
}


}	// namespace Whisper

