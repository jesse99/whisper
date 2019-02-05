/*
 *  File:       X3DObject.h
 *  Summary:	Base class for QuickDraw 3D objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DObject.h,v $
 *		Revision 1.4  2001/04/14 07:30:13  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:52:52  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:18:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DObject
//!		Base class for Quesa objects.
// ===================================================================================
class QUESA_EXPORT X3DObject {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DObject();
						
						X3DObject(TQ3Object object);
						
private:
						X3DObject(const X3DObject& rhs);
						/**< X3DObjects can only be cloned. X3DSharedObject can be both
						copied and cloned. */
						
			X3DObject&	operator=(const X3DObject& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3Object() const			{return mObject;}
	//@}

	//! @name Object Routines
	//@{
	virtual X3DObject*	Clone() const;
		
			bool 		IsDrawable() const					{return Q3Object_IsDrawable(mObject);}
	
			bool 		IsWritable(TQ3FileObject file) const;
	//@}

	//! @name Object Type Routines
	//@{
			TQ3ObjectType GetType() const					{return Q3Object_GetType(mObject);}

			TQ3ObjectType GetLeafType() const				{return Q3Object_GetLeafType(mObject);}
			
			bool 		IsType(TQ3ObjectType type) const	{return Q3Object_IsType(mObject, type);}
			
			void 		ConfirmType(TQ3ObjectType type) const;
						/**< Throws a X3DBadObjectException exception if mObject doesn't 
						match type. */
	//@}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3Object	mObject;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

