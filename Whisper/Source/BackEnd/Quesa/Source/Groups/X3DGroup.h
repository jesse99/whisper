/*
 *  File:       X3DGroup.h
 *  Summary:	Base class for QuickDraw 3D group objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DGroup.h,v $
 *		Revision 1.3  2000/12/13 08:51:51  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:16:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DGroup.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:51:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <4>	 12/04/97	JDJ		X3DGroupIterator has a TQ3GroupObject (instead of a X3DGroup*).
 *		 <3>	 12/03/97	JDJ		Iterators return objects by value instead of via a counted pointer.
 *		 <2>	 12/03/97	JDJ		No longer an abstract class.
 *		 <1>	  1/13/97	JDJ		Created.
 */

#pragma once

#include <Iterator>

#include <X3DShape.h>
#include <XDebug.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class X3DGroup;


// ===================================================================================
//	class X3DGroupIterator
//!		Iterates over the objects in a X3DGroup.
// ===================================================================================
template<class T> class X3DGroupIterator : public std::iterator<std::bidirectional_iterator_tag, T> {

//-----------------------------------
//	Types
//
public:
    typedef const T& const_reference;    
    typedef const T* const_pointer;

//-----------------------------------
//	Initialization/Destruction
//
public:
								X3DGroupIterator(TQ3GroupObject group, TQ3GroupPosition position, 
												 TQ3ObjectType type = '????');

								X3DGroupIterator(TQ3GroupObject group, TQ3GroupPosition position, 
												 TQ3Object object);

								X3DGroupIterator(TQ3GroupObject group = nil);

//-----------------------------------
//	Initialization/Destruction
//
public:
			T 					operator*() const;

			X3DGroupIterator& 	operator++();
			X3DGroupIterator 	operator++(int);

			X3DGroupIterator& 	operator--();
			X3DGroupIterator 	operator--(int);
			
			TQ3GroupPosition 	GetPosition() const								{return mPosition;}
			
			bool 				operator==(const X3DGroupIterator& rhs) const	{ASSERT(mGroup == rhs.mGroup); return mPosition == rhs.mPosition;}	
			bool 				operator!=(const X3DGroupIterator& rhs) const	{ASSERT(mGroup == rhs.mGroup); return mPosition != rhs.mPosition;}

//-----------------------------------
//	Member Data
//
protected:
	TQ3GroupObject		mGroup;
	TQ3GroupPosition	mPosition;
	TQ3ObjectType		mType;
	TQ3Object			mObject;
};


// ===================================================================================
//	class X3DGroup
//!		Base class for Quesa objects that can have children.
// ===================================================================================
class QUESA_EXPORT X3DGroup : public X3DShape {

	typedef X3DShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DGroup();
						
						X3DGroup();

						X3DGroup(TQ3GroupObject object);
						
	virtual X3DGroup* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3GroupObject() const					{return mObject;}
	//@}

	//! @name Object Counts
	//@{
			uint32 		CountObjects() const;
			
			uint32 		CountObjectsOfType(TQ3ObjectType type) const;
			
			void 		EmptyObjects();
						/**< Delete all the objects in the group. */
			
			void 		EmptyObjectsOfType(TQ3ObjectType type);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DGroup. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

#include <X3DGroup.inc>

