/*
 *  File:       X3DGeometry.h
 *  Summary:	Base class for QuickDraw 3D geometry objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DGeometry.h,v $
 *		Revision 1.4  2001/04/14 07:28:42  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:51:43  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:15:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaGeometry.h>

#include <X3DShape.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DGeometry
//!		Base class for Quesa geometry objects.
// ===================================================================================
class QUESA_EXPORT X3DGeometry : public X3DShape {

	typedef X3DShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DGeometry();
						
						X3DGeometry(TQ3GeometryObject object);
						
	virtual X3DGeometry* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3GeometryObject() const				{return mObject;}
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const;
	//@}
									
	//! @name Attributes
	//@{
			TQ3AttributeSet GetAttributeSet() const;
						
			void 		SetAttributeSet(TQ3AttributeSet attributes);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DGeometry. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

