/*
 *  File:       X3DCamera.h
 *  Summary:	Base class for QuickDraw 3D cameras.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DCamera.h,v $
 *		Revision 1.4  2001/04/14 07:27:44  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:50:34  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:14:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaCamera.h>

#include <X3DMatrix.h>
#include <X2DPrimitives.h>
#include <X3DPrimitives.h>
#include <X3DShape.h>
#include <X3DVectors.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DCamera
//!		Base class for Quesa cameras.
// ===================================================================================
class QUESA_EXPORT X3DCamera : public X3DShape {

	typedef X3DShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DCamera();
						
						X3DCamera(TQ3CameraObject object);
						
	virtual X3DCamera* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operato
	//@{
						operator TQ3CameraObject() const				{return mObject;}
	//@}

	//! @name Placement
	//@{
			TQ3CameraPlacement GetPlacement() const;
						
			void 		SetPlacement(const TQ3CameraPlacement& location);
			void 		SetPlacement(const X3DPoint& loc, const X3DPoint& pointOfInterest, const X3DVector& upVector);
	//@}
			
	//! @name Range
	//@{
			TQ3CameraRange GetRange() const;

			void 		SetRange(const TQ3CameraRange& range);
	//@}
			
	//! @name View Port
	//@{
			TQ3CameraViewPort GetViewPort() const;
			
			void 		SetViewPort(const TQ3CameraViewPort& viewPort);
	//@}

	//! @name Data
	//@{
			TQ3CameraData GetData() const;

			void 		SetData(const TQ3CameraData& data);
	//@}

	//! @name Coordinate Transforms
	//@{
			X3DMatrix 	GetWorldToView() const;

			X3DMatrix 	GetWorldToFrustum() const;

			X3DMatrix 	GetViewToFrustum() const;
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DCamera. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
