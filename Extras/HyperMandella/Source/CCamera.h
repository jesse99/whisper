/*
 *  File:       CCamera.h
 *  Summary:   	Abstract base class for objects that generate the rays from the eye to the image plane. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: CCamera.h,v $
 *		Revision 1.2  2001/03/26 04:33:47  jesjones
 *		Setter methods dirty the doc.
 *		
 *		Revision 1.1  2000/12/28 02:48:19  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:52:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 1/30/98	JDJ		Created
 */

#pragma once

#include "ICamera.h"


// ===================================================================================
//	class CCamera
// ===================================================================================
class CCamera : public ICamera {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CCamera();
	
  						CCamera(XBoss* boss);
  						
//-----------------------------------
//	New API
//
public:
	virtual const SCameraPlacement& GetPlacement() const				{return mPlacement;}
	virtual void 		SetPlacement(const SCameraPlacement& placement, bool changeAxes = true, bool dirtyDoc = false);

	virtual const SCameraRange& GetRange() const						{return mRange;}
	virtual void 		SetRange(const SCameraRange& range, bool dirtyDoc = false);
	
	virtual X3DPoint 	GetHitherPoint(const X2DPoint& pixel) const;
	virtual X3DVector 	GetEyeRay(const X3DPoint& pt) const;
						
	virtual void 		OrbitX(double theta, bool dirtyDoc = false);
	virtual void 		OrbitY(double theta, bool dirtyDoc = false);
	virtual void 		OrbitZ(double theta, bool dirtyDoc = false);

	virtual void 		Pan(const X2DPoint& delta, bool dirtyDoc = false) = 0;	
	virtual void 		Zoom(double scaleBy, bool dirtyDoc = false);

	virtual X3DVector 	GetXAxis() const								{return mXAxis;}
	virtual X3DVector 	GetYAxis() const								{return mYAxis;}
	virtual X3DVector 	GetZAxis() const								{return mZAxis;}

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		Invariant() const;

//-----------------------------------
//	Internal API
//
protected:
	virtual X3DPoint 	OnGetHitherPoint(const X2DPoint& pixel) const = 0;
	
	virtual X3DVector 	OnGetEyeRay(const X3DPoint& pt) const = 0;
			
			void 		DoReset();

//-----------------------------------
//	Member Data
//
protected:
	SCameraPlacement	mPlacement;
	SCameraRange		mRange;
	
	X3DVector			mXAxis;			// unit vectors used to define the camera coordinate system	
	X3DVector			mYAxis;			// mPlacement.upVector - mPlacement.upVector projected onto mZAxis	
	X3DVector			mZAxis;	
};
