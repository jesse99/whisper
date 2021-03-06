/*
 *  File:       X3DSharedObject.h
 *  Summary:	Base class for QuickDraw 3D shared objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DSharedObject.h,v $
 *		Revision 1.3  2000/12/13 08:54:10  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:20:37  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DSharedObject.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:54:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <3>	 12/03/97	JDJ		No longer an abstract class.
 *		 <2>	 12/02/97	JDJ		Added copy ctor, assignment operator, HasMoreThanOneReference,
 *									and Detach. No longer descends from TReferenceCountedMixin.
 *		 <1>	  1/13/97	JDJ		Created.
 */

#pragma once

#include <X3DObject.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DSharedObject
//!		Base class for Quesa shareable objects.
// ===================================================================================
class QUESA_EXPORT X3DSharedObject : public X3DObject {

	typedef X3DObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DSharedObject();
						
						X3DSharedObject(TQ3SharedObject object);
						
						X3DSharedObject(const X3DSharedObject& rhs);
						/**< Note that these two functions merely increment the QD 3D's
						object's ref count: they do not copy the data! If you want
						a copy of the object you can use one of these functions
						and call Detach on the new object or you can use X3DObject::
						Clone. */
						
			X3DSharedObject& operator=(const X3DSharedObject& rhs);
						
	virtual X3DSharedObject* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3SharedObject() const		{return mObject;}
	//@}

	//! @name Reference Counts
	//@{
			bool 		HasMoreThanOneReference() const;
						/**< Returns true if the QD 3D object's ref count is larger than one. */
						
			void 		Detach();
						/**< If the QD 3D object's ref count is larger than one this will
						duplicate the QD 3D object so 'this' has sole ownership of the
						object. */
	//@}

	//! @name Cache Management
	//@{
			uint32 		GetEditIndex() const					{return Q3Shared_GetEditIndex(mObject);}
						/**< Returns the 'serial number' of the object. Useful for 
						caching object information. Returns 0 on error.
					
						Hold onto this number to determine if an object has changed
						since you last built your caches... To validate, do:
						
						if (this->GetEditIndex() != mEditIndex) {
							this->RebuildCache();
							mEditIndex = this->GetEditIndex();
						} */
						
			void 		Edited();
						/**< Bumps the 'serial number' of the object to a different value. 
						This call is intended to be used solely from a plug-in object 
						which is shared. Call this whenever your private instance data 
						changes. */
	//@}
					
	//! @name Tracking editing in read-in objects with custom elements
	//@{
			void 		ClearEditTracking();
			
			bool 		GetEditTrackingState() const;
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DSharedObject. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
