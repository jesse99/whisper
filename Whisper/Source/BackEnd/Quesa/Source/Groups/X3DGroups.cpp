/*
 *  File:       X3DGroups.cpp
 *  Summary:	QuickDraw 3D group objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DGroups.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:00  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:28:54  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:16:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DGroups.h>

#include <QuesaGroup.h>

#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateDisplayGroup
//
//---------------------------------------------------------------
static X3DObject* CreateDisplayGroup(TQ3Object inObject)
{
	X3DDisplayGroup* object = new X3DDisplayGroup(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateOrderedDisplayGroup
//
//---------------------------------------------------------------
static X3DObject* CreateOrderedDisplayGroup(TQ3Object inObject)
{
	X3DOrderedDisplayGroup* object = new X3DOrderedDisplayGroup(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateProxyDisplayGroup
//
//---------------------------------------------------------------
static X3DObject* CreateProxyDisplayGroup(TQ3Object inObject)
{
	X3DProxyDisplayGroup* object = new X3DProxyDisplayGroup(inObject);
	
	return object;
}

//---------------------------------------------------------------
//
// CreateInfoGroup
//
//---------------------------------------------------------------
static X3DObject* CreateInfoGroup(TQ3Object inObject)
{
	X3DInfoGroup* object = new X3DInfoGroup(inObject);
	
	return object;
}

//---------------------------------------------------------------
//
// CreateLightGroup
//
//---------------------------------------------------------------
static X3DObject* CreateLightGroup(TQ3Object inObject)
{
	X3DLightGroup* object = new X3DLightGroup(inObject);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DDisplayGroup
// ===================================================================================

//---------------------------------------------------------------
//
// X3DDisplayGroup::~X3DDisplayGroup
//
//---------------------------------------------------------------
X3DDisplayGroup::~X3DDisplayGroup()
{
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::X3DDisplayGroup ()
//
//---------------------------------------------------------------
X3DDisplayGroup::X3DDisplayGroup() : X3DGroup(Q3DisplayGroup_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::X3DDisplayGroup (TQ3GroupObject)
//
//---------------------------------------------------------------
X3DDisplayGroup::X3DDisplayGroup(TQ3GroupObject object) : X3DGroup(object)
{
	this->ConfirmType(kQ3GroupTypeDisplay);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::Clone
//
//---------------------------------------------------------------
X3DDisplayGroup* X3DDisplayGroup::Clone() const
{
	X3DDisplayGroup* object = new X3DDisplayGroup(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::GetState
//
//---------------------------------------------------------------
TQ3DisplayGroupState X3DDisplayGroup::GetState() const
{
	TQ3DisplayGroupState state;
	
	TQ3Status status = Q3DisplayGroup_GetState(mObject, &state);
	ThrowIf3DError(status);

	return state;
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::SetState
//
//---------------------------------------------------------------
void X3DDisplayGroup::SetState(TQ3DisplayGroupState newState)
{
	TQ3Status status = Q3DisplayGroup_SetState(mObject, newState);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::AddState
//
//---------------------------------------------------------------
void X3DDisplayGroup::AddState(TQ3DisplayGroupState inState)
{
	TQ3DisplayGroupState state;
	
	TQ3Status status = Q3DisplayGroup_GetState(mObject, &state);
	ThrowIf3DError(status);
	
	state |= inState;

	status = Q3DisplayGroup_SetState(mObject, state);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::RemoveState
//
//---------------------------------------------------------------
void X3DDisplayGroup::RemoveState(TQ3DisplayGroupState inState)
{
	TQ3DisplayGroupState state;
	
	TQ3Status status = Q3DisplayGroup_GetState(mObject, &state);
	ThrowIf3DError(status);
	
	state &= ~inState;

	status = Q3DisplayGroup_SetState(mObject, state);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::SetAndUseBoundingBox
//
//---------------------------------------------------------------
void X3DDisplayGroup::SetAndUseBoundingBox(const X3DRect& inBox)
{	
	TQ3BoundingBox box;
	box.min     = inBox.origin;
	box.max     = inBox.origin + inBox.size;
	box.isEmpty = kQ3False;
		
	TQ3Status status = Q3DisplayGroup_SetAndUseBoundingBox(mObject, &box);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::CalcAndUseBoundingBox
//
//---------------------------------------------------------------
void X3DDisplayGroup::CalcAndUseBoundingBox(TQ3ComputeBounds bounds, TQ3ViewObject view)
{
	TQ3Status status = Q3DisplayGroup_CalcAndUseBoundingBox(mObject, bounds, view);
	ThrowIf3DError(status);
}

						
//---------------------------------------------------------------
//
// X3DDisplayGroup::GetBoundingBox
//
//---------------------------------------------------------------
X3DRect X3DDisplayGroup::GetBoundingBox() const
{
	TQ3BoundingBox box;
	TQ3Status status = Q3DisplayGroup_GetBoundingBox(mObject, &box);
	ThrowIf3DError(status);

	return box.isEmpty ? kZero3DRect : X3DRect(box.min, box.max);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::RemoveBoundingBox
//
//---------------------------------------------------------------
void X3DDisplayGroup::RemoveBoundingBox()
{
	TQ3Status status = Q3DisplayGroup_RemoveBoundingBox(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::Submit
//
//---------------------------------------------------------------
void X3DDisplayGroup::Submit(TQ3ViewObject view) const
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3DisplayGroup_Submit(mObject, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::AddObject
// 
//---------------------------------------------------------------
X3DDrawableIterator X3DDisplayGroup::AddObject(const X3DSharedObject& object)
{
//	PRECONDITION(Q3Object_IsDrawable(object));	// $$$ Quesa bug

	TQ3GroupPosition position = Q3Group_AddObject(mObject, object);
	ThrowIfNil(position);
	
	return X3DDrawableIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::AddObjectBefore
//
//---------------------------------------------------------------
X3DDrawableIterator X3DDisplayGroup::AddObjectBefore(const X3DDrawableIterator& pos, const X3DSharedObject& object)
{
	PRECONDITION(Q3Object_IsDrawable(object));

	TQ3GroupPosition position = Q3Group_AddObjectBefore(mObject, pos.GetPosition(), object);
	ThrowIfNil(position);
	
	return X3DDrawableIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::AddObjectAfter
//
//---------------------------------------------------------------
X3DDrawableIterator X3DDisplayGroup::AddObjectAfter(const X3DDrawableIterator& pos, const X3DSharedObject& object)
{
	PRECONDITION(Q3Object_IsDrawable(object));

	TQ3GroupPosition position = Q3Group_AddObjectAfter(mObject, pos.GetPosition(), object);
	ThrowIfNil(position);
	
	return X3DDrawableIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::RemovePosition
//
//---------------------------------------------------------------
void X3DDisplayGroup::RemovePosition(const X3DDrawableIterator& pos)
{
	TQ3Object object = Q3Group_RemovePosition(mObject, pos.GetPosition());
	ThrowIfNil(object);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::GetFirstPosition
//
//---------------------------------------------------------------
X3DDrawableIterator X3DDisplayGroup::GetFirstPosition()
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetFirstPosition(mObject, &position);
	ThrowIf3DError(status);

	return X3DDrawableIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::GetLastPosition
//
//---------------------------------------------------------------
X3DDrawableIterator X3DDisplayGroup::GetLastPosition()
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetLastPosition(mObject, &position);
	ThrowIf3DError(status);

	return X3DDrawableIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::GetFirstPositionOfType
//
//---------------------------------------------------------------
X3DDrawableIterator X3DDisplayGroup::GetFirstPositionOfType(TQ3ObjectType type)
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetFirstPositionOfType(mObject, type, &position);
	ThrowIf3DError(status);

	return X3DDrawableIterator(*this, position, type);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::GetLastPositionOfType
//
//---------------------------------------------------------------
X3DDrawableIterator X3DDisplayGroup::GetLastPositionOfType(TQ3ObjectType type)
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetLastPositionOfType(mObject, type, &position);
	ThrowIf3DError(status);

	return X3DDrawableIterator(*this, position, type);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::GetFirstObjectPosition
//
//---------------------------------------------------------------
X3DDrawableIterator X3DDisplayGroup::GetFirstObjectPosition(const X3DSharedObject& object)
{
	PRECONDITION(Q3Object_IsDrawable(object));

	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetFirstObjectPosition(mObject, object, &position);
	ThrowIf3DError(status);

	return X3DDrawableIterator(*this, position, object);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::GetLastObjectPosition
//
//---------------------------------------------------------------
X3DDrawableIterator X3DDisplayGroup::GetLastObjectPosition(const X3DSharedObject& object)
{
	PRECONDITION(Q3Object_IsDrawable(object));

	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetLastObjectPosition(mObject, object, &position);
	ThrowIf3DError(status);

	return X3DDrawableIterator(*this, position, object);
}


//---------------------------------------------------------------
//
// X3DDisplayGroup::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DDisplayGroup::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GroupTypeDisplay);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DOrderedDisplayGroup
// ===================================================================================

//---------------------------------------------------------------
//
// X3DOrderedDisplayGroup::~X3DOrderedDisplayGroup
//
//---------------------------------------------------------------
X3DOrderedDisplayGroup::~X3DOrderedDisplayGroup()
{
}


//---------------------------------------------------------------
//
// X3DOrderedDisplayGroup::X3DOrderedDisplayGroup ()
//
//---------------------------------------------------------------
X3DOrderedDisplayGroup::X3DOrderedDisplayGroup() : X3DDisplayGroup(Q3OrderedDisplayGroup_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DOrderedDisplayGroup::X3DOrderedDisplayGroup (TQ3GroupObject)
//
//---------------------------------------------------------------
X3DOrderedDisplayGroup::X3DOrderedDisplayGroup(TQ3GroupObject object) : X3DDisplayGroup(object)
{
	this->ConfirmType(kQ3DisplayGroupTypeOrdered);
}


//---------------------------------------------------------------
//
// X3DOrderedDisplayGroup::Clone
//
//---------------------------------------------------------------
X3DOrderedDisplayGroup* X3DOrderedDisplayGroup::Clone() const
{
	X3DOrderedDisplayGroup* object = new X3DOrderedDisplayGroup(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DOrderedDisplayGroup::IsCompatible 				[static]
//	
//---------------------------------------------------------------
bool X3DOrderedDisplayGroup::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3DisplayGroupTypeOrdered);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DProxyDisplayGroup
// ===================================================================================

//---------------------------------------------------------------
//
// X3DProxyDisplayGroup::~X3DProxyDisplayGroup
//
//---------------------------------------------------------------
X3DProxyDisplayGroup::~X3DProxyDisplayGroup()
{
}


//---------------------------------------------------------------
//
// X3DProxyDisplayGroup::X3DProxyDisplayGroup ()
//
//---------------------------------------------------------------
X3DProxyDisplayGroup::X3DProxyDisplayGroup() : X3DDisplayGroup(Q3IOProxyDisplayGroup_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DProxyDisplayGroup::X3DProxyDisplayGroup (TQ3GroupObject)
//
//---------------------------------------------------------------
X3DProxyDisplayGroup::X3DProxyDisplayGroup(TQ3GroupObject object) : X3DDisplayGroup(object)
{
	this->ConfirmType(kQ3DisplayGroupTypeIOProxy);
}


//---------------------------------------------------------------
//
// X3DProxyDisplayGroup::Clone
//
//---------------------------------------------------------------
X3DProxyDisplayGroup* X3DProxyDisplayGroup::Clone() const
{
	X3DProxyDisplayGroup* object = new X3DProxyDisplayGroup(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DProxyDisplayGroup::IsCompatible 					[static]
//	
//---------------------------------------------------------------
bool X3DProxyDisplayGroup::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3DisplayGroupTypeIOProxy);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DInfoGroup
// ===================================================================================

//---------------------------------------------------------------
//
// X3DInfoGroup::~X3DInfoGroup
//
//---------------------------------------------------------------
X3DInfoGroup::~X3DInfoGroup()
{
}


//---------------------------------------------------------------
//
// X3DInfoGroup::X3DInfoGroup ()
//
//---------------------------------------------------------------
X3DInfoGroup::X3DInfoGroup() : X3DGroup(Q3InfoGroup_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DInfoGroup::X3DInfoGroup (TQ3GroupObject)
//
//---------------------------------------------------------------
X3DInfoGroup::X3DInfoGroup(TQ3GroupObject object) : X3DGroup(object)
{
	this->ConfirmType(kQ3GroupTypeInfo);
}


//---------------------------------------------------------------
//
// X3DInfoGroup::Clone
//
//---------------------------------------------------------------
X3DInfoGroup* X3DInfoGroup::Clone() const
{
	X3DInfoGroup* object = new X3DInfoGroup(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DInfoGroup::AddObject
//
//---------------------------------------------------------------
X3DInfoIterator X3DInfoGroup::AddObject(const X3DString& str)
{
	TQ3GroupPosition position = Q3Group_AddObject(mObject, str);
	ThrowIfNil(position);
	
	return X3DInfoIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DInfoGroup::AddObjectBefore
//
//---------------------------------------------------------------
X3DInfoIterator X3DInfoGroup::AddObjectBefore(const X3DInfoIterator& pos, const X3DString& str)
{
	TQ3GroupPosition position = Q3Group_AddObjectBefore(mObject, pos.GetPosition(), str);
	ThrowIfNil(position);
	
	return X3DInfoIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DInfoGroup::AddObjectAfter
//
//---------------------------------------------------------------
X3DInfoIterator X3DInfoGroup::AddObjectAfter(const X3DInfoIterator& pos, const X3DString& str)
{
	TQ3GroupPosition position = Q3Group_AddObjectAfter(mObject, pos.GetPosition(), str);
	ThrowIfNil(position);
	
	return X3DInfoIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DInfoGroup::RemovePosition
//
//---------------------------------------------------------------
void X3DInfoGroup::RemovePosition(const X3DInfoIterator& pos)
{
	TQ3Object object = Q3Group_RemovePosition(mObject, pos.GetPosition());
	ThrowIfNil(object);
}


//---------------------------------------------------------------
//
// X3DInfoGroup::GetFirstPosition
//
//---------------------------------------------------------------
X3DInfoIterator X3DInfoGroup::GetFirstPosition()
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetFirstPosition(mObject, &position);
	ThrowIf3DError(status);

	return X3DInfoIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DInfoGroup::GetLastPosition
//
//---------------------------------------------------------------
X3DInfoIterator X3DInfoGroup::GetLastPosition()
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetLastPosition(mObject, &position);
	ThrowIf3DError(status);
	
	return X3DInfoIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DInfoGroup::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DInfoGroup::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GroupTypeInfo);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DLightGroup
// ===================================================================================

//---------------------------------------------------------------
//
// X3DLightGroup::~X3DLightGroup
//
//---------------------------------------------------------------
X3DLightGroup::~X3DLightGroup()
{
}


//---------------------------------------------------------------
//
// X3DLightGroup::X3DLightGroup ()
//
//---------------------------------------------------------------
X3DLightGroup::X3DLightGroup() : X3DGroup(Q3LightGroup_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLightGroup::X3DLightGroup (TQ3GroupObject)
//
//---------------------------------------------------------------
X3DLightGroup::X3DLightGroup(TQ3GroupObject object) : X3DGroup(object)
{
	this->ConfirmType(kQ3GroupTypeLight);
}


//---------------------------------------------------------------
//
// X3DLightGroup::Clone
//
//---------------------------------------------------------------
X3DLightGroup* X3DLightGroup::Clone() const
{
	X3DLightGroup* object = new X3DLightGroup(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DLightGroup::AddObject
//
//---------------------------------------------------------------
X3DLightIterator X3DLightGroup::AddObject(const X3DLight& light)
{
	TQ3GroupPosition position = Q3Group_AddObject(mObject, light);
	ThrowIfNil(position);
	
	return X3DLightIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DLightGroup::AddObjectBefore
//
//---------------------------------------------------------------
X3DLightIterator X3DLightGroup::AddObjectBefore(const X3DLightIterator& pos, const X3DLight& light)
{
	TQ3GroupPosition position = Q3Group_AddObjectBefore(mObject, pos.GetPosition(), light);
	ThrowIfNil(position);
	
	return X3DLightIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DLightGroup::AddObjectAfter
//
//---------------------------------------------------------------
X3DLightIterator X3DLightGroup::AddObjectAfter(const X3DLightIterator& pos, const X3DLight& light)
{
	TQ3GroupPosition position = Q3Group_AddObjectAfter(mObject, pos.GetPosition(), light);
	ThrowIfNil(position);
	
	return X3DLightIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DLightGroup::RemovePosition
//
//---------------------------------------------------------------
void X3DLightGroup::RemovePosition(const X3DLightIterator& pos)
{
	TQ3Object object = Q3Group_RemovePosition(mObject, pos.GetPosition());
	ThrowIfNil(object);
}


//---------------------------------------------------------------
//
// X3DLightGroup::GetFirstPosition
//
//---------------------------------------------------------------
X3DLightIterator X3DLightGroup::GetFirstPosition()
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetFirstPosition(mObject, &position);
	ThrowIf3DError(status);

	return X3DLightIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DLightGroup::GetLastPosition
//
//---------------------------------------------------------------
X3DLightIterator X3DLightGroup::GetLastPosition()
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetLastPosition(mObject, &position);
	ThrowIf3DError(status);

	return X3DLightIterator(*this, position);
}


//---------------------------------------------------------------
//
// X3DLightGroup::GetFirstPositionOfType
//
//---------------------------------------------------------------
X3DLightIterator X3DLightGroup::GetFirstPositionOfType(TQ3ObjectType type)
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetFirstPositionOfType(mObject, type, &position);
	ThrowIf3DError(status);

	return X3DLightIterator(*this, position, type);
}


//---------------------------------------------------------------
//
// X3DLightGroup::GetLastPositionOfType
//
//---------------------------------------------------------------
X3DLightIterator X3DLightGroup::GetLastPositionOfType(TQ3ObjectType type)
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetLastPositionOfType(mObject, type, &position);
	ThrowIf3DError(status);

	return X3DLightIterator(*this, position, type);
}


//---------------------------------------------------------------
//
// X3DLightGroup::GetFirstObjectPosition
//
//---------------------------------------------------------------
X3DLightIterator X3DLightGroup::GetFirstObjectPosition(const X3DLight& light)
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetFirstObjectPosition(mObject, light, &position);
	ThrowIf3DError(status);

	return X3DLightIterator(*this, position, light);
}


//---------------------------------------------------------------
//
// X3DLightGroup::GetLastObjectPosition
//
//---------------------------------------------------------------
X3DLightIterator X3DLightGroup::GetLastObjectPosition(const X3DLight& light)
{
	TQ3GroupPosition position = nil;
	
	TQ3Status status = Q3Group_GetLastObjectPosition(mObject, light, &position);
	ThrowIf3DError(status);

	return X3DLightIterator(*this, position, light);
}


//---------------------------------------------------------------
//
// X3DLightGroup::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DLightGroup::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GroupTypeLight);
	
	return compatible;
}


}	// namespace Whisper

