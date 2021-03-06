/*
 *  File:       X3DTransform.h
 *  Summary:	Base class for QuickDraw 3D transforms.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTransform.h,v $
 *		Revision 1.3  2000/12/13 08:55:10  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:22:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DTransform.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:55:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	 12/03/97	JDJ		No longer an abstract class.
 *		 <1>	  2/27/97	JDJ		Created.
 */

#pragma once

#include <X3DMatrix.h>
#include <X3DSharedObject.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DTransform
//!		Base class for Quesa transform objects.
// ===================================================================================
class QUESA_EXPORT X3DTransform : public X3DSharedObject {

	typedef X3DSharedObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DTransform();
						
						X3DTransform(TQ3TransformObject object);
						
	virtual X3DTransform* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	// ----- Conversion Operator -----
						operator TQ3TransformObject() const					{return mObject;}

	// ----- Matrix -----
			X3DMatrix 	GetMatrix() const;

	// ----- Submitting (Retained Mode) -----
			void 		Submit(TQ3ViewObject view) const;

	// ----- Misc -----
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DTransform. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
