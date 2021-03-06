/*
 *  File:       X3DGroup.inc
 *  Summary:	Base class for QuickDraw 3D group objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DGroup.inc,v $
 *		Revision 1.3  2001/04/14 07:28:49  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:16:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 12/04/97	JDJ		X3DGroupIterator has a TQ3GroupObject (instead of a X3DGroup*).
 *		 <4>	 12/03/97	JDJ		Iterators return objects by value instead of via a counted pointer.
 *		 <3>	 12/03/97	JDJ		Included X3DUtils.h
 *		 <2>	  9/28/97	JDJ		Added some missing <T>'s.
 *		 <1>	  1/13/97	JDJ		Created.
 */

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DGroupIterator
// ===================================================================================

//---------------------------------------------------------------
//
// X3DGroupIterator::X3DGroupIterator (TQ3GroupObject, TQ3GroupPosition, TQ3ObjectType)
//
//---------------------------------------------------------------
template <class T>
X3DGroupIterator<T>::X3DGroupIterator(TQ3GroupObject group, TQ3GroupPosition position, TQ3ObjectType type)
{
	ASSERT(group != nil);
	
	mGroup    = group;
	mPosition = position;
	mType     = type;
	mObject   = nil;
}


//---------------------------------------------------------------
//
// X3DGroupIterator::X3DGroupIterator (TQ3GroupObject, TQ3GroupPosition, TQ3Object)
//
//---------------------------------------------------------------
template <class T>
X3DGroupIterator<T>::X3DGroupIterator(TQ3GroupObject group, TQ3GroupPosition position, TQ3Object object)
{
	ASSERT(group != nil);
	ASSERT(object != nil);
	
	mGroup    = group;
	mPosition = position;
	mType     = '????';
	mObject   = object;
}


//---------------------------------------------------------------
//
// X3DGroupIterator::X3DGroupIterator (TQ3GroupObject)
//
//---------------------------------------------------------------
template <class T>
X3DGroupIterator<T>::X3DGroupIterator(TQ3GroupObject group)
{
	mGroup    = group;
	mPosition = nil;			// past end marker
	mType     = '????';
	mObject   = nil;
}


//---------------------------------------------------------------
//
// X3DGroupIterator::operator*
//
//---------------------------------------------------------------
template <class T>
T X3DGroupIterator<T>::operator*() const
{
	ASSERT(mPosition != nil);
	
	TQ3Object object;
	TQ3Status status = Q3Group_GetPositionObject(mGroup, mPosition, &object);
	ThrowIf3DError(status);
		
	T obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DGroupIterator::operator++ ()
//
//---------------------------------------------------------------
template <class T>
X3DGroupIterator<T>& X3DGroupIterator<T>::operator++()
{
	TQ3Status status = kQ3Success;
	
	if (mType != '????')
		if (mPosition == nil)
			status = Q3Group_GetFirstPositionOfType(mGroup, mType, &mPosition);
		else
			status = Q3Group_GetNextPositionOfType(mGroup, mType, &mPosition);
	
	else if (mObject != nil)
		if (mPosition == nil)
			status = Q3Group_GetFirstObjectPosition(mGroup, mObject, &mPosition);
		else
			status = Q3Group_GetNextObjectPosition(mGroup, mObject, &mPosition);
	
	else
		if (mPosition == nil)
			status = Q3Group_GetFirstPosition(mGroup, &mPosition);
		else
			status = Q3Group_GetNextPosition(mGroup, &mPosition);

	ThrowIf3DError(status);
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DGroupIterator::operator++ (int)
//
//---------------------------------------------------------------
template <class T>
X3DGroupIterator<T> X3DGroupIterator<T>::operator++(int)
{
	X3DGroupIterator temp = *this;
	
	this->operator++();
	
	return temp;
}


//---------------------------------------------------------------
//
// X3DGroupIterator::operator-- ()
//
//---------------------------------------------------------------
template <class T>
X3DGroupIterator<T>& X3DGroupIterator<T>::operator--()
{
	TQ3Status status = kQ3Success;
	
	if (mType != '????')
		if (mPosition == nil)
			status = Q3Group_GetLastPositionOfType(mGroup, mType, &mPosition);
		else
			status = Q3Group_GetPreviousPositionOfType(mGroup, mType, &mPosition);
	
	else if (mObject != nil)
		if (mPosition == nil)
			status = Q3Group_GetLastObjectPosition(mGroup, mObject, &mPosition);
		else
			status = Q3Group_GetPreviousObjectPosition(mGroup, mObject, &mPosition);
	
	else
		if (mPosition == nil)
			status = Q3Group_GetLastPosition(mGroup, &mPosition);
		else
			status = Q3Group_GetPreviousPosition(mGroup, &mPosition);

	ThrowIf3DError(status);
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DGroupIterator::operator-- (int)
//
//---------------------------------------------------------------
template <class T>
X3DGroupIterator<T> X3DGroupIterator<T>::operator--(int)
{
	X3DGroupIterator temp = *this;
	
	this->operator--();
	
	return temp;
}


}	// namespace Whisper
