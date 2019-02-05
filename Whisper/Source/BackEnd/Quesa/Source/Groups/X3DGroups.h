/*
 *  File:       X3DGroups.h
 *  Summary:	QuickDraw 3D group objects.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DDisplayGroup				- Contains drawable objects.
 *					X3DOrderedDisplayGroup	- Renders the objects in a predefined order (see class).
 *					X3DProxyDisplayGroup	- Contains several representations of a single figure (for I/O).
 *				X3DInfoGroup				- Contains string objects.
 *				X3DLightGroup				- Contains light objects.
 *
 *  Copyright © 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DGroups.h,v $
 *		Revision 1.4  2001/04/14 07:29:02  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:51:56  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:16:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	  4/19/00	JDJ		Added the group culling routines from QD3D 1.6
 *		 <3>	 12/03/97	JDJ		Iterators return objects by value instead of via a counted pointer.
 *		 <2>	  9/28/97	JDJ		Uses a custom iterator class in place of reverse_bidirectional_iterator
 *									(can't use the default because X3DGroupIterator::operator* returns by
 *									value instead of a reference).
 *		 <1>	  2/15/97	JDJ		Created.
 */

#pragma once

#include <Quesa.h>
#include <QuesaGroup.h>

#include <X3DGroup.h>
#include <X3DLight.h>
#include <X3DPrimitives.h>
#include <X3DString.h>
#include <XCountedPtr.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Types
//
typedef X3DGroupIterator<X3DSharedObject>	X3DDrawableIterator;	//!< operator*() returns a X3DSharedObject
typedef X3DGroupIterator<X3DString> 		X3DInfoIterator;		//!< operator*() returns a X3DString
typedef X3DGroupIterator<X3DLight> 			X3DLightIterator;		//!< operator*() returns a X3DLight


// ===================================================================================
//	class X3DDisplayGroup
//!		Contains drawable objects.
// ===================================================================================
class QUESA_EXPORT X3DDisplayGroup : public X3DGroup {

	typedef X3DGroup Inherited;

//-----------------------------------
//	Types
//
public:
	typedef X3DSharedObject 		value_type;
	typedef X3DSharedObject& 		reference;
    typedef X3DSharedObject*		pointer;
    typedef const X3DSharedObject& 	const_reference;
    typedef X3DDrawableIterator		iterator;

	class reverse_iterator {					// like reverse_bidirectional_iterator except operator* doesn't return a reference
	public:
	    reverse_iterator() {}
	    reverse_iterator(const iterator& x) : current(x) {}
	    iterator base() { return current; }
	    value_type operator*() const {
	        iterator tmp = current;
	        return *--tmp;
	    }
	    reverse_iterator& operator++() {
	        --current;
	        return *this;
	    }
	    reverse_iterator operator++(int) {
	        reverse_iterator tmp = *this;
	        --current;
	        return tmp;
	    }
	    reverse_iterator& operator--() {
	        ++current;
	        return *this;
	    }
	    reverse_iterator operator--(int) {
	        reverse_iterator tmp = *this;
	        ++current;
	        return tmp;
	    }
	    friend bool operator==(const reverse_iterator& x, const reverse_iterator& y)	{return x.current == y.current;}
	protected:
	    iterator current;
	};
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DDisplayGroup();
						
						X3DDisplayGroup();
						
						X3DDisplayGroup(TQ3GroupObject object);
						
	virtual X3DDisplayGroup* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name State
	//@{
			TQ3DisplayGroupState GetState() const;

			void 		SetState(TQ3DisplayGroupState state);

			void 		AddState(TQ3DisplayGroupState state);
						/**< Or's state with the current state. */

			void 		RemoveState(TQ3DisplayGroupState state);
						/**< And's state's complement with the current state. */
	//@}

	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const;
	//@}

	//! @name Adding and Removing Objects
	//@{
			X3DDrawableIterator AddObject(const X3DSharedObject& object);
						/**< Returns the position of the new object. */

			X3DDrawableIterator AddObjectBefore(const X3DDrawableIterator& position, const X3DSharedObject& object);

			X3DDrawableIterator AddObjectAfter(const X3DDrawableIterator& position, const X3DSharedObject& object);

			void 		RemovePosition(const X3DDrawableIterator& position);
						/**< Deletes the object at the given position. */
	//@}
						
	//! @name Display Group Culling (QD3D 1.6)
	//@{
			void 		SetAndUseBoundingBox(const X3DRect& box);
						/**< Assigns a bounding box to the group so that it can be culled without
						building the group. Note that this will throw if the box is empty. */
			
			void 		CalcAndUseBoundingBox(TQ3ComputeBounds bounds, TQ3ViewObject view);
						/**< Builds the group and uses the result to assign a bounding box. Note
						that the box may be incorrect if an object inside the group is changed.
						Bounds can be kQ3ComputeBoundsExact or kQ3ComputeBoundsApproximate. */
									
			X3DRect 	GetBoundingBox() const;
			
			void 		RemoveBoundingBox();
						/**< Removes the bounding box (if one is present). */
	//@}
			
	//! @name Iterating Over All Objects
	//@{
			X3DDrawableIterator GetFirstPosition();

			X3DDrawableIterator GetLastPosition();
	//@}

	//! @name Iterating Over Objects by Type
	//@{
			X3DDrawableIterator GetFirstPositionOfType(TQ3ObjectType type);
						
			X3DDrawableIterator GetLastPositionOfType(TQ3ObjectType type);
	//@}

	//! @name Iterating Over a Specific Object
	//@{
			X3DDrawableIterator GetFirstObjectPosition(const X3DSharedObject& object);

			X3DDrawableIterator GetLastObjectPosition(const X3DSharedObject& object);
	//@}

	//! @name STL Style Iterating
	//@{
			iterator 			begin()						{return this->GetFirstPosition();}

			iterator 			begin(TQ3ObjectType type)	{return this->GetFirstPositionOfType(type);}
			
			iterator 			end()						{return X3DDrawableIterator(*this);}

			reverse_iterator 	rbegin()					{return reverse_iterator(this->end());}
			
			reverse_iterator 	rend()						{return reverse_iterator(this->begin());}
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DDisplayGroup. */
	//@}
};


// ===================================================================================
//	class X3DOrderedDisplayGroup
//!		Renders the objects in a predefined order.
//			1) transforms
//			2) styles
//			3) attribute sets
//			4) shaders
//			5) geometric objects
//			6) groups
//			7) unknown objects
// ===================================================================================
class QUESA_EXPORT X3DOrderedDisplayGroup : public X3DDisplayGroup {

	typedef X3DDisplayGroup Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DOrderedDisplayGroup();
						
						X3DOrderedDisplayGroup();
						
						X3DOrderedDisplayGroup(TQ3GroupObject object);

	virtual X3DOrderedDisplayGroup* Clone() const;
		
	static	bool 		IsCompatible(TQ3Object object);
						// Returns true if the QD 3D object can be used to construct a X3DOrderedDisplayGroup.
};


// ===================================================================================
//	class X3DProxyDisplayGroup
//!		Contains several representations of a single figure (for I/O).
//		This should contain one or more representations of a single geometric figure. 
//		This is useful when writing figures out to a metafile since readers may not be
//		able to handle every QuickDraw 3D geometry. Objects should be stored so that 
//		the preferred formats are towards the front. When an I/O proxy group is drawn/
//		picked/bounded the first object that is not unknown is used: the remaining 
//		objects are ignored.
// ===================================================================================
class QUESA_EXPORT X3DProxyDisplayGroup : public X3DDisplayGroup {

	typedef X3DDisplayGroup Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DProxyDisplayGroup();
						
						X3DProxyDisplayGroup();
						
						X3DProxyDisplayGroup(TQ3GroupObject object);

	virtual X3DProxyDisplayGroup* Clone() const;
		
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DProxyDisplayGroup. */
};


// ===================================================================================
//	class X3DInfoGroup
//!		Contains string objects.
// ===================================================================================
class QUESA_EXPORT X3DInfoGroup : public X3DGroup {

	typedef X3DGroup Inherited;

//-----------------------------------
//	Types
//
public:
	typedef X3DString 			value_type;
	typedef X3DString& 			reference;
    typedef X3DString*			pointer;
    typedef const X3DString&	const_reference;
    typedef X3DInfoIterator		iterator;

	class reverse_iterator {					// like reverse_bidirectional_iterator except operator* doesn't return a reference
	public:
	    reverse_iterator() {}
	    reverse_iterator(const iterator& x) : current(x) {}
	    iterator base() { return current; }
	    value_type operator*() const {
	        iterator tmp = current;
	        return *--tmp;
	    }
	    reverse_iterator& operator++() {
	        --current;
	        return *this;
	    }
	    reverse_iterator operator++(int) {
	        reverse_iterator tmp = *this;
	        --current;
	        return tmp;
	    }
	    reverse_iterator& operator--() {
	        ++current;
	        return *this;
	    }
	    reverse_iterator operator--(int) {
	        reverse_iterator tmp = *this;
	        ++current;
	        return tmp;
	    }
	    friend bool operator==(const reverse_iterator& x, const reverse_iterator& y)	{return x.current == y.current;}
	protected:
	    iterator current;
	};
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DInfoGroup();
						
						X3DInfoGroup();
						
						X3DInfoGroup(TQ3GroupObject object);
						
	virtual X3DInfoGroup* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Adding and Removing Strings
	//@{
			X3DInfoIterator AddObject(const X3DString& str);
						/**< Returns the position of the new string. */

			X3DInfoIterator AddObjectBefore(const X3DInfoIterator& position, const X3DString& str);

			X3DInfoIterator AddObjectAfter(const X3DInfoIterator& position, const X3DString& str);

			void 		RemovePosition(const X3DInfoIterator& position);
						/**< Deletes the string at the given position. */
	//@}
						
	//! @name Iterating Over All Strings
	//@{
			X3DInfoIterator GetFirstPosition();

			X3DInfoIterator GetLastPosition();
	//@}

	//! @name STL Style Iterating
	//@{
			iterator 		begin()						{return this->GetFirstPosition();}
			
			iterator 		end()						{return X3DInfoIterator(*this);}

			reverse_iterator rbegin()					{return reverse_iterator(this->end());}
			
			reverse_iterator rend()						{return reverse_iterator(this->begin());}
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DInfoGroup. */
	//@}
};


// ===================================================================================
//	class X3DLightGroup
//!		Contains light objects.
// ===================================================================================
class QUESA_EXPORT X3DLightGroup : public X3DGroup {

	typedef X3DGroup Inherited;

//-----------------------------------
//	Types
//
public:
	typedef X3DLight 			value_type;
	typedef X3DLight& 			reference;
    typedef X3DLight*			pointer;
    typedef const X3DLight&		const_reference;
    typedef X3DLightIterator	iterator;

	class reverse_iterator {					// like reverse_bidirectional_iterator except operator* doesn't return a reference
	public:
	    reverse_iterator() {}
	    reverse_iterator(const iterator& x) : current(x) {}
	    iterator base() { return current; }
	    value_type operator*() const {
	        iterator tmp = current;
	        return *--tmp;
	    }
	    reverse_iterator& operator++() {
	        --current;
	        return *this;
	    }
	    reverse_iterator operator++(int) {
	        reverse_iterator tmp = *this;
	        --current;
	        return tmp;
	    }
	    reverse_iterator& operator--() {
	        ++current;
	        return *this;
	    }
	    reverse_iterator operator--(int) {
	        reverse_iterator tmp = *this;
	        ++current;
	        return tmp;
	    }
	    friend bool operator==(const reverse_iterator& x, const reverse_iterator& y)	{return x.current == y.current;}
	protected:
	    iterator current;
	};
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DLightGroup();
						
						X3DLightGroup();
						
						X3DLightGroup(TQ3GroupObject object);
						
	virtual X3DLightGroup* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Adding and Removing Lights
	//@{
			X3DLightIterator AddObject(const X3DLight& light);
						/**< Returns the position of the new light. */

			X3DLightIterator AddObjectBefore(const X3DLightIterator& position, const X3DLight& light);

			X3DLightIterator AddObjectAfter(const X3DLightIterator& position, const X3DLight& light);

			void 		RemovePosition(const X3DLightIterator& position);
						/**< Deletes the light at the given position. */
	//@}
					
	//! @name Iterating Over All Lights
	//@{
			X3DLightIterator GetFirstPosition();

			X3DLightIterator GetLastPosition();
	//@}

	//! @name Iterating Over Lights by Type
	//@{
			X3DLightIterator GetFirstPositionOfType(TQ3ObjectType type);
						
			X3DLightIterator GetLastPositionOfType(TQ3ObjectType type);
	//@}

	//! @name Iterating Over a Specific Light
	//@{
			X3DLightIterator GetFirstObjectPosition(const X3DLight& light);

			X3DLightIterator GetLastObjectPosition(const X3DLight& light);
	//@}

	//! @name STL Style Iterating
	//@{
			iterator 		begin()						{return this->GetFirstPosition();}

			iterator 		begin(TQ3ObjectType type)	{return this->GetFirstPositionOfType(type);}
			
			iterator 		end()						{return X3DLightIterator(*this);}

			reverse_iterator rbegin()					{return reverse_iterator(this->end());}
			
			reverse_iterator rend()						{return reverse_iterator(this->begin());}
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DLightGroup. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
