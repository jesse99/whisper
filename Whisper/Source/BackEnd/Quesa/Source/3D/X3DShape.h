/*
 *  File:       X3DShape.h
 *  Summary:	Base class for QuickDraw 3D shape objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DShape.h,v $
 *		Revision 1.3  2000/12/13 08:54:06  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:20:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DShape.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:54:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	 12/03/97	JDJ		No longer an abstract class.
 *		 <1>	  1/13/97	JDJ		Created.
 */

#pragma once

#include <X3DSharedObject.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DShape
//!		Base class for Quesa shape objects.
// ===================================================================================
class QUESA_EXPORT X3DShape : public X3DSharedObject {

	typedef X3DSharedObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DShape();
						
						X3DShape(TQ3ShapeObject object);
						
	virtual X3DShape* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3ShapeObject() const						{return mObject;}
	//@}

	//! @name Elements
	//@{
			void 		AddElement(TQ3ElementType type, const void* data);
						/**< Allows you to attach arbitrary data to a QD 3D shape.
						$$ This should take a X3DElement pointer instead of a void*.
						$$ÊBut if this is done the saved data should be compatible
						$$Êwith a procedural implementation! (Develop 26 has a good
						$$Êarticle on elements and attributes). */

			void 		GetElement(TQ3ElementType type, void* data) const;
			
			bool 		ContainsElement(TQ3ElementType type) const;
			
			TQ3ElementType GetNextElementType(TQ3ElementType type) const;
						/**< Use kQ3ElementTypeNone to get the first type. Returns 
						kQ3ElementTypeNone when there are no more types. */
			
			void 		EmptyElements();
						/**< Zaps all the shape's elements. */
			
			void 		ClearElement(TQ3ElementType type);
						/**< Zaps a particular element. */
	//@}

	//! @name Sets
	//@{
			TQ3SetObject GetSet() const;
						/**< Sets provide an alternate way to attach data to a shape. */
			
			void 		SetSet(TQ3SetObject set);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DShape. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
