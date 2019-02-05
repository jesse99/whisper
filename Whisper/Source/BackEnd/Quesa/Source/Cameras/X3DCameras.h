/*
 *  File:       X3DCameras.h
 *  Summary:	QuickDraw 3D camera objects.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DViewAngleAspectCamera - Uses perspective projection.
 *				X3DViewPlaneCamera		 - Uses perspective projection (for looking at specific objects)
 *				X3DOrthographicCamera	 - Uses parallel projection.
 *
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DCameras.h,v $
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/13 08:50:39  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:14:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DCameras.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.4  2000/12/15 09:07:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Yet more doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:50:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  2/16/97	JDJ		Created.
 */

#pragma once

#include <X3DCamera.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DViewAngleAspectCamera
//!		A camera that uses perspective projection.
// ===================================================================================
class QUESA_EXPORT X3DViewAngleAspectCamera : public X3DCamera {

	typedef X3DCamera Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DViewAngleAspectCamera();
						
						X3DViewAngleAspectCamera(const TQ3ViewAngleAspectCameraData& data);

						X3DViewAngleAspectCamera(TQ3CameraObject object);
						
	virtual X3DViewAngleAspectCamera* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Field of View
	//@{
			float 		GetFOV() const;
						
			void 		SetFOV(float fov);
	//@}
			
	//! @name Aspect Ratio
	//@{
			float 		GetAspectRatio() const;
						
			void 		SetAspectRatio(float ratio);
	//@}
			
	//! @name Data
	//@{
			TQ3ViewAngleAspectCameraData GetData() const;

			void 		SetData(const TQ3ViewAngleAspectCameraData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DViewAngleAspectCamera. */
	//@}
};


// ===================================================================================
//	class X3DViewPlaneCamera
//!		A camera that uses perspective projection (for looking at specific objects).
// ===================================================================================
class QUESA_EXPORT X3DViewPlaneCamera : public X3DCamera {

	typedef X3DCamera Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DViewPlaneCamera();
						
						X3DViewPlaneCamera(const TQ3ViewPlaneCameraData& data);

						X3DViewPlaneCamera(TQ3CameraObject object);
						
	virtual X3DViewPlaneCamera* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name View Plane
	//@{
			float 		GetViewPlane() const;
						
			void 		SetViewPlane(float plane);
	//@}
			
	//! @name Half Size
	//@{
			X2DPoint 	GetHalfSize() const;
						
			void 		SetHalfSize(const X2DPoint& size);
	//@}
			
	//! @name Center
	//@{
			X2DPoint 	GetCenter() const;
						
			void 		SetCenter(const X2DPoint& center);
	//@}
			
	//! @name Data
	//@{
			TQ3ViewPlaneCameraData GetData() const;

			void 		SetData(const TQ3ViewPlaneCameraData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DViewPlaneCamera. */
	//@}
};


// ===================================================================================
//	class X3DOrthographicCamera
//!		A camera that uses parallel projection.
// ===================================================================================
class QUESA_EXPORT X3DOrthographicCamera : public X3DCamera {

	typedef X3DCamera Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DOrthographicCamera();
						
						X3DOrthographicCamera(const TQ3OrthographicCameraData& data);

						X3DOrthographicCamera(TQ3CameraObject object);
						
	virtual X3DOrthographicCamera* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Box
	//@{
			X2DRect 	GetBox() const;
						
			void 		SetBox(const X2DRect& box);
	//@}
			
	//! @name Data
	//@{
			TQ3OrthographicCameraData GetData() const;

			void 		SetData(const TQ3OrthographicCameraData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DOrthographicCamera. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
