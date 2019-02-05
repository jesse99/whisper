/*
 *  File:       COrthographicCamera.cpp
 *  Summary:   	A non-persepective camera that shoots our parallel rays.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: COrthographicCamera.cpp,v $
 *		Revision 1.4  2001/04/21 02:24:26  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/05 23:51:03  jesjones
 *		Added casts for the pickier Intel compiler.
 *		
 *		Revision 1.2  2001/03/26 04:27:29  jesjones
 *		Setter methods now dirty the doc.
 *		
 *		Revision 1.1  2000/12/28 02:50:04  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:55:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "CCamera.h"
#include "IOrthographicCamera.h"

#include <XNumbers.h>

#include "IDocument.h"


// ===================================================================================
//	class COrthographicCamera
// ===================================================================================
class COrthographicCamera : public CCamera, public IOrthographicCamera {

	typedef CCamera Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~COrthographicCamera();
	
  						COrthographicCamera(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual X2DSize 	GetViewSize() const						{return mViewSize;}
	virtual void 		SetViewSize(const X2DSize& size, bool dirtyDoc = false);

	virtual void 		SetPlacement(const SCameraPlacement& placement, bool changeAxes = true, bool dirtyDoc = false);

	virtual void 		OrbitX(double theta, bool dirtyDoc = false);
	virtual void 		OrbitY(double theta, bool dirtyDoc = false);
	virtual void 		OrbitZ(double theta, bool dirtyDoc = false);

	virtual void 		Pan(const X2DPoint& delta, bool dirtyDoc = false);
	virtual void 		Zoom(double scaleBy, bool dirtyDoc = false);

protected:
	virtual void 		Invariant() const;

	virtual X3DPoint 	OnGetHitherPoint(const X2DPoint& pixel) const;

	virtual X3DVector 	OnGetEyeRay(const X3DPoint& pt) const;

//-----------------------------------
//	Member Data
//
protected:	
	X2DSize			mViewSize;
	X3DPoint		mViewOrigin;			// bottom left of view port
};

DEFINE_INTERFACE_FACTORY(COrthographicCamera)

//---------------------------------------------------------------
//
// COrthographicCamera::~COrthographicCamera
//
//---------------------------------------------------------------
COrthographicCamera::~COrthographicCamera()
{
}


//---------------------------------------------------------------
//
// COrthographicCamera::COrthographicCamera
//
//---------------------------------------------------------------
COrthographicCamera::COrthographicCamera(XBoss* boss) : CCamera(boss)
{
	IOrthographicCamera::DoSetBoss(boss);

	mViewSize = X2DSize(2.5f, 2.0f);
	
	mViewOrigin = mPlacement.location - 0.5*mViewSize.width*mXAxis - 0.5*mViewSize.height*mYAxis;
}


//---------------------------------------------------------------
//
// COrthographicCamera::SetViewSize
//
//---------------------------------------------------------------
void COrthographicCamera::SetViewSize(const X2DSize& size, bool dirtyDoc)	
{
	PRECONDITION(size.width > 0.0);
	PRECONDITION(size.height > 0.0);

	mViewSize = size;

	mViewOrigin = mPlacement.location - 0.5*mViewSize.width*mXAxis - 0.5*mViewSize.height*mYAxis;
		
	if (dirtyDoc) {
		ICamera* thisPtr = this;
		IDocumentPtr doc(thisPtr);
		doc->SetDirty();
	}
}


//---------------------------------------------------------------
//
// COrthographicCamera::SetPlacement
//
//---------------------------------------------------------------
void COrthographicCamera::SetPlacement(const SCameraPlacement& placement, bool changeAxes, bool dirtyDoc)
{
	Inherited::SetPlacement(placement, changeAxes, dirtyDoc);
	
	mViewOrigin = mPlacement.location - 0.5*mViewSize.width*mXAxis - 0.5*mViewSize.height*mYAxis;
}


//---------------------------------------------------------------
//
// COrthographicCamera::OrbitX
//
//---------------------------------------------------------------
void COrthographicCamera::OrbitX(double theta, bool dirtyDoc)
{	
	Inherited::OrbitX(theta, dirtyDoc);
	
	mViewOrigin = mPlacement.location - 0.5*mViewSize.width*mXAxis - 0.5*mViewSize.height*mYAxis;
}


//---------------------------------------------------------------
//
// COrthographicCamera::OrbitY
//
//---------------------------------------------------------------
void COrthographicCamera::OrbitY(double theta, bool dirtyDoc)
{
	Inherited::OrbitY(theta, dirtyDoc);
	
	mViewOrigin = mPlacement.location - 0.5*mViewSize.width*mXAxis - 0.5*mViewSize.height*mYAxis;
}


//---------------------------------------------------------------
//
// COrthographicCamera::OrbitZ
//
//---------------------------------------------------------------
void COrthographicCamera::OrbitZ(double theta, bool dirtyDoc)
{
	Inherited::OrbitZ(theta, dirtyDoc);
	
	mViewOrigin = mPlacement.location - 0.5*mViewSize.width*mXAxis - 0.5*mViewSize.height*mYAxis;
}


//---------------------------------------------------------------
//
// COrthographicCamera::Pan
//
//---------------------------------------------------------------
void COrthographicCamera::Pan(const X2DPoint& inDelta, bool dirtyDoc)
{
	X3DVector delta = inDelta.x*mViewSize.width*mXAxis + inDelta.y*mViewSize.height*mYAxis;
	
	mPlacement.location        += delta;
	mPlacement.pointOfInterest += delta;
	
	Inherited::DoReset();
	
	mViewOrigin = mPlacement.location - 0.5*mViewSize.width*mXAxis - 0.5*mViewSize.height*mYAxis;
		
	if (dirtyDoc) {
		ICamera* thisPtr = this;
		IDocumentPtr doc(thisPtr);
		doc->SetDirty();
	}
}

						
//---------------------------------------------------------------
//
// COrthographicCamera::Zoom
//
//---------------------------------------------------------------
void COrthographicCamera::Zoom(double scaleBy, bool dirtyDoc)
{
	Inherited::Zoom(scaleBy, dirtyDoc);
	
	mViewSize.width  *= (float) scaleBy;
	mViewSize.height *= (float) scaleBy;
	
	mViewOrigin = mPlacement.location - 0.5*mViewSize.width*mXAxis - 0.5*mViewSize.height*mYAxis;
}

#pragma mark ~

//---------------------------------------------------------------
//
// COrthographicCamera::Invariant
//
//---------------------------------------------------------------
void COrthographicCamera::Invariant() const
{
	Inherited::Invariant();
	IOrthographicCamera::Invariant();
	
	ASSERT(mViewSize.width > 0.0);
	ASSERT(mViewSize.height > 0.0);
}


//---------------------------------------------------------------
//
// COrthographicCamera::OnGetHitherPoint
//
//---------------------------------------------------------------
X3DPoint COrthographicCamera::OnGetHitherPoint(const X2DPoint& pixel) const
{
	X3DVector temp1 = mXAxis;
	temp1 *= mViewSize.width*pixel.x;					// use assignment operators since they're faster than the inline versions
	
	X3DVector temp2 = mYAxis;
	temp2 *= mViewSize.height*pixel.y;
	
	X3DVector temp3 = mZAxis;
	temp3 *= mRange.hither;

	X3DPoint pt = mViewOrigin;
	pt += temp1;
	pt += temp2;
	pt += temp3;

	return pt;
}


//---------------------------------------------------------------
//
// COrthographicCamera::OnGetEyeRay
//
//---------------------------------------------------------------
X3DVector COrthographicCamera::OnGetEyeRay(const X3DPoint& pt) const
{
	UNUSED(pt);
		
	return mZAxis;			// all eye rays are parallel
}


