/*
 *  File:       CCamera.cpp
 *  Summary:   	Abstract base class for objects that generate the rays from the eye to the image plane. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: CCamera.cpp,v $
 *		Revision 1.5  2001/04/21 02:23:15  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/05 23:50:44  jesjones
 *		Added cast for the pickier Intel compiler.
 *		
 *		Revision 1.3  2001/04/05 03:57:56  jesjones
 *		Added streaming functions to the structs.
 *		
 *		Revision 1.2  2001/03/26 04:33:43  jesjones
 *		Setter methods dirty the doc.
 *		
 *		Revision 1.1  2000/12/28 02:48:14  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:52:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "CCamera.h"

#include <X3DMatrix.h>
#include <XNumbers.h>
#include <XStreaming.h>
#include <XXMLAttribute.h>
#include <XXMLItems.h>

#include "IDocument.h"


// ===================================================================================
//	MakeRotateAboutAxis
// ===================================================================================

//---------------------------------------------------------------
//
// MakeRotateAboutAxis
//
// These equations are from the description of the glRotate
// function in the OpenGL Reference Manual, but they're slightly
// more involved since we support a non-zero origin.
//
//---------------------------------------------------------------
static X3DMatrix MakeRotateAboutAxis(const X3DPoint& origin, const X3DVector& orientation, double angle)
{
	PRECONDITION(Equal(orientation.Length(), 1.0));
	
	X3DMatrix matrix;
	
	double c = cos(angle);							// 0.081 msecs (note that the time doesn't change if floats are used throughout)
	double s = sin(-angle);							// QD3D was 0.061 msecs (all times are on a PowerCenter Pro 210 [210 Mhz 604e])
													// the difference is probably due to CW Pro2's sucky optimizer...
	double x = orientation.x;
	double y = orientation.y;
	double z = orientation.z;

	matrix.value[0][0] = (float) (x*x*(1.0f - c) + c);
	matrix.value[0][1] = (float) (x*y*(1.0f - c) - z*s);
	matrix.value[0][2] = (float) (x*z*(1.0f - c) + y*s);
	matrix.value[0][3] = 0.0f;

	matrix.value[1][0] = (float) (y*x*(1.0f - c) + z*s);
	matrix.value[1][1] = (float) (y*y*(1.0f - c) + c);
	matrix.value[1][2] = (float) (y*z*(1.0f - c) - x*s);
	matrix.value[1][3] = 0.0f;

	matrix.value[2][0] = (float) (x*z*(1.0f - c) - y*s);
	matrix.value[2][1] = (float) (y*z*(1.0f - c) + x*s);
	matrix.value[2][2] = (float) (z*z*(1.0f - c) + c);
	matrix.value[2][3] = 0.0f;

	matrix.value[3][0] = (float) (-origin.x*matrix.value[0][0] - origin.y*matrix.value[1][0] - origin.z*matrix.value[2][0] + origin.x);
	matrix.value[3][1] = (float) (-origin.x*matrix.value[0][1] - origin.y*matrix.value[1][1] - origin.z*matrix.value[2][1] + origin.y);
	matrix.value[3][2] = (float) (-origin.x*matrix.value[0][2] - origin.y*matrix.value[1][2] - origin.z*matrix.value[2][2] + origin.z);
	matrix.value[3][3] = 1.0f;
	
#if DEBUG && 0
	if (TGestalt::HasQuickDraw3D()) {		
		X3DMatrix transform;
		transform.SetRotateAboutAxis(origin, orientation, (float) angle);

		bool equal = true;
		
		for (int32 i = 0; i < 4 && equal; i++)
			for (int32 j = 0; j < 4 && equal; j++)		
				equal = Equal(matrix.value[i][j], transform.value[i][j], 1.0e-4);
		
		ASSERT(equal);	
	}
#endif
	
	return matrix;
}


//---------------------------------------------------------------
//
// MakeRotateVectorToVector
//
// These equations are from the description of the glRotate
// function in the OpenGL Reference Manual, but they're slightly
// more involved since we support a non-zero origin.
//
//---------------------------------------------------------------
static X3DMatrix MakeRotateVectorToVector(const X3DVector& lhs, const X3DVector& rhs)
{
	PRECONDITION(Equal(lhs.Length(), 1.0));		
	PRECONDITION(Equal(rhs.Length(), 1.0));
	
	X3DVector axis = Normalize(CrossProduct(lhs, rhs));

	double dp = MinMax(-1.0, DotProduct(lhs, rhs), 1.0);
	double angle = acos(dp);
	
	double cx = axis.x;
	double cy = axis.y;
	double cz = axis.z;
	
	double d = sqrt(cy*cy + cz*cz);
	
	double cosA = cos(angle);						
	double sinA = sin(angle);						
													
	double d2 = d*d;
	double di2 = 1.0/d2;
	
	double r1 = (-cosA + 1.0)*cx;
	double r2 = (cy*cx*sinA - cz*cosA)*di2;				
	double r3 = (cy*cx*cosA + cz*sinA)*cx*di2 + cy;
	double r4 = (cz*cx*sinA + cy*cosA)*di2;
	double r5 = (cz*cx*cosA - cy*sinA)*cx*di2 + cz;

	X3DMatrix matrix;
	matrix.value[0][0] = (float) (cosA*d2  + cx*cx);
	matrix.value[0][1] = (float) (sinA*cz  + r1*cy);
	matrix.value[0][2] = (float) (-sinA*cy + r1*cz);
	matrix.value[0][3] = 0.0f;

	matrix.value[1][0] = (float) (-cy*cx*cosA - cz*sinA + cy*cx);
	matrix.value[1][1] = (float) (-r2*cz      + r3*cy);
	matrix.value[1][2] = (float) (r2*cy       + r3*cz);
	matrix.value[1][3] = 0.0f;

	matrix.value[2][0] = (float) (-cz*cx*cosA + cy*sinA + cz*cx);
	matrix.value[2][1] = (float) (-r4*cz      + r5*cy);
	matrix.value[2][2] = (float) (r4*cy       + r5*cz);
	matrix.value[2][3] = 0.0f;
	
	matrix.value[3][0] = 0.0f;
	matrix.value[3][1] = 0.0f;
	matrix.value[3][2] = 0.0f;
	matrix.value[3][3] = 1.0f;
	
#if DEBUG && 0
	if (TGestalt::HasQuickDraw3D()) {			
		X3DMatrix transform;
		transform.SetRotateVectorToVector(lhs, rhs);

		bool equal = true;
		
		for (int32 i = 0; i < 4 && equal; i++)
			for (int32 j = 0; j < 4 && equal; j++)		
				equal = Equal(matrix.value[i][j], transform.value[i][j], 1.0e-4);
		
		ASSERT(equal);	
	}
#endif
	
	return matrix;
}

#pragma mark -

// ===================================================================================
//	struct SCameraPlacement
// ===================================================================================

//---------------------------------------------------------------
//
// SCameraPlacement::SCameraPlacement ()
//
//---------------------------------------------------------------
SCameraPlacement::SCameraPlacement()
{
	location        = X3DPoint(1.0f, 0.0f, 2.0f);
	pointOfInterest = kZero3DPt;
	upVector        = kYVector;
}


//---------------------------------------------------------------
//
// SCameraPlacement::SCameraPlacement (XXMLElement)
//
//---------------------------------------------------------------
SCameraPlacement::SCameraPlacement(const XXMLElement& element)
{
	const XXMLAttribute& l = element.FindAttribute(L"loc");
	l.Get(location);

	const XXMLAttribute& p = element.FindAttribute(L"focus_pt");
	p.Get(pointOfInterest);

	const XXMLAttribute& u = element.FindAttribute(L"up_vector");
	u.Get(upVector);
}


//---------------------------------------------------------------
//
// SCameraPlacement::CreateElement
//
//---------------------------------------------------------------
XXMLElement* SCameraPlacement::CreateElement() const
{
	XXMLElement* element = new XXMLElement(L"Placement");
	
	element->AppendAttribute(XXMLAttribute(L"loc", location));
	element->AppendAttribute(XXMLAttribute(L"focus_pt", pointOfInterest));
	element->AppendAttribute(XXMLAttribute(L"up_vector", upVector));

	return element;
}


//---------------------------------------------------------------
//
// operator>> (SCameraPlacement)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, SCameraPlacement& value)
{
	int32 version;
	stream 	>> version
			>> value.location		
			>> value.pointOfInterest		
			>> value.upVector;
	ASSERT(version == 1);
		
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (SCameraPlacement)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const SCameraPlacement& value)
{
	int32 version = 1;
	stream 	<< version
			<< value.location	
			<< value.pointOfInterest		
			<< value.upVector;
				
	return stream;
}

#pragma mark -

// ===================================================================================
//	struct SCameraRange
// ===================================================================================

//---------------------------------------------------------------
//
// SCameraRange::SCameraRange ()
//
//---------------------------------------------------------------
SCameraRange::SCameraRange()
{
	hither = 0.001f;
	yon    = 4.0f;
}


//---------------------------------------------------------------
//
// SCameraRange::SCameraRange (XXMLElement)
//
//---------------------------------------------------------------
SCameraRange::SCameraRange(const XXMLElement& element)
{
	const XXMLAttribute& h = element.FindAttribute(L"hither");
	h.Get(hither);

	const XXMLAttribute& y = element.FindAttribute(L"yon");
	y.Get(yon);
}


//---------------------------------------------------------------
//
// SCameraRange::CreateElement
//
//---------------------------------------------------------------
XXMLElement* SCameraRange::CreateElement() const
{
	XXMLElement* element = new XXMLElement(L"Range");
	
	element->AppendAttribute(XXMLAttribute(L"hither", hither));
	element->AppendAttribute(XXMLAttribute(L"yon", yon));

	return element;
}


//---------------------------------------------------------------
//
// operator>> (SCameraRange)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, SCameraRange& value)
{
	int32 version;
	stream 	>> version
			>> value.hither
			>> value.yon;
	ASSERT(version == 1);
		
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (SCameraRange)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const SCameraRange& value)
{
	int32 version = 1;
	stream 	<< version
			<< value.hither
			<< value.yon;
				
	return stream;
}

#pragma mark -

// ===================================================================================
//	class CCamera
// ===================================================================================

//---------------------------------------------------------------
//
// CCamera::~CCamera
//
//---------------------------------------------------------------
CCamera::~CCamera()
{
}


//---------------------------------------------------------------
//
// CCamera::CCamera
//
//---------------------------------------------------------------
CCamera::CCamera(XBoss* boss)
{
	ICamera::DoSetBoss(boss);

	mPlacement.location        = X3DPoint(1.0f, 0.0f, 2.0f);
	mPlacement.pointOfInterest = X3DPoint(0.0f, 0.0f, 0.0f);
	mPlacement.upVector        = X3DVector(0.0f, 1.0f, 0.0f);

	mRange.hither = 0.001f;
	mRange.yon = 4.0f;

	this->DoReset();
	
	CALL_INVARIANT;		
}


//---------------------------------------------------------------
//
// CCamera::SetPlacement
//
//---------------------------------------------------------------
void CCamera::SetPlacement(const SCameraPlacement& placement, bool changeAxes, bool dirtyDoc)
{
	CHECK_INVARIANT;

	mPlacement = placement;
	
	mPlacement.upVector = Normalize(mPlacement.upVector);
	
	if (changeAxes)
		this->DoReset();
		
	if (dirtyDoc) {
		IDocumentPtr doc(this);
		doc->SetDirty();
	}
}


//---------------------------------------------------------------
//
// CCamera::SetRange
//
//---------------------------------------------------------------
void CCamera::SetRange(const SCameraRange& range, bool dirtyDoc)				
{
	PRECONDITION(range.hither >= 0.0);
	PRECONDITION(range.hither < range.yon);
	CHECK_INVARIANT;

	mRange = range;
	
	this->DoReset();
		
	if (dirtyDoc) {
		IDocumentPtr doc(this);
		doc->SetDirty();
	}
}


//---------------------------------------------------------------
//
// CCamera::GetHitherPoint
//
//---------------------------------------------------------------
X3DPoint CCamera::GetHitherPoint(const X2DPoint& pixel) const
{
	PRECONDITION(pixel.x >= 0.0);
	PRECONDITION(pixel.x <= 1.0);
	PRECONDITION(pixel.y >= 0.0);
	PRECONDITION(pixel.y <= 1.0);
	CHECK_INVARIANT;
	
	X3DPoint pt = this->OnGetHitherPoint(pixel);
	
	return pt;
}


//---------------------------------------------------------------
//
// CCamera::GetEyeRay
//
//---------------------------------------------------------------
X3DVector CCamera::GetEyeRay(const X3DPoint& pt) const
{
	CHECK_INVARIANT;
	
	X3DVector ray = this->OnGetEyeRay(pt);
	
	POSTCONDITION(Equal(ray.LengthSquared(), 1.0));

	return ray;
}


//---------------------------------------------------------------
//
// CCamera::OrbitX
//
//---------------------------------------------------------------
void CCamera::OrbitX(double theta, bool dirtyDoc)
{
	CHECK_INVARIANT;
	
	X3DVector oldOrientation = Normalize(X3DVector(mPlacement.location - mPlacement.pointOfInterest));

	// Rotate the camera location around the camera axis.	
	X3DMatrix transform = MakeRotateAboutAxis(mPlacement.pointOfInterest, mXAxis, kTwoPi*theta/360.0);		
	mPlacement.location = mPlacement.location*transform;

	// Rotate the camera axes so that the z vector points towards
	// the point of interest.
	X3DVector newOrientation = Normalize(X3DVector(mPlacement.location - mPlacement.pointOfInterest));

	transform = MakeRotateVectorToVector(newOrientation, oldOrientation);

	mPlacement.upVector = mPlacement.upVector*transform;
	
	this->DoReset();
		
	if (dirtyDoc) {
		IDocumentPtr doc(this);
		doc->SetDirty();
	}
}


//---------------------------------------------------------------
//
// CCamera::OrbitY
//
//---------------------------------------------------------------
void CCamera::OrbitY(double theta, bool dirtyDoc)
{
	CHECK_INVARIANT;
		
	X3DVector oldOrientation = Normalize(X3DVector(mPlacement.location - mPlacement.pointOfInterest));

	// Rotate the camera location around the camera axis.	
	X3DMatrix transform = MakeRotateAboutAxis(mPlacement.pointOfInterest, mYAxis, kTwoPi*theta/360.0);				
	mPlacement.location = mPlacement.location*transform;

	// Rotate the camera axes so that the z vector points towards
	// the point of interest.
	X3DVector newOrientation = Normalize(X3DVector(mPlacement.location - mPlacement.pointOfInterest));

	transform = MakeRotateVectorToVector(newOrientation, oldOrientation);

	mPlacement.upVector = mPlacement.upVector*transform;
	
	this->DoReset();
		
	if (dirtyDoc) {
		IDocumentPtr doc(this);
		doc->SetDirty();
	}
}


//---------------------------------------------------------------
//
// CCamera::OrbitZ
//
//---------------------------------------------------------------
void CCamera::OrbitZ(double theta, bool dirtyDoc)
{
	CHECK_INVARIANT;
		
	X3DMatrix transform = MakeRotateAboutAxis(kZero3DPt, mZAxis, kTwoPi*theta/360.0);				

	mPlacement.upVector = mPlacement.upVector*transform;
	
	this->DoReset();
		
	if (dirtyDoc) {
		IDocumentPtr doc(this);
		doc->SetDirty();
	}
}


//---------------------------------------------------------------
//
// CCamera::Zoom
//
//---------------------------------------------------------------
void CCamera::Zoom(double scaleBy, bool dirtyDoc)
{
	PRECONDITION(scaleBy > 0.0);
	CHECK_INVARIANT;
	
	double distance = Distance(mPlacement.location, mPlacement.pointOfInterest);
	distance *= scaleBy;
	
	X3DVector viewV = Normalize(X3DVector(mPlacement.location - mPlacement.pointOfInterest));
	
	mPlacement.location = mPlacement.pointOfInterest + distance*viewV;
	
	mRange.yon *= (float) scaleBy;
	
	this->DoReset();
		
	if (dirtyDoc) {
		IDocumentPtr doc(this);
		doc->SetDirty();
	}
}

#pragma mark ~

//---------------------------------------------------------------
//
// CCamera::Invariant
//
//---------------------------------------------------------------
void CCamera::Invariant() const
{
	ICamera::Invariant();
	
	ASSERT(mRange.hither >= 0.0);
	ASSERT(mRange.hither < mRange.yon);
	
	ASSERT(Equal(mXAxis.LengthSquared(), 1.0));
	ASSERT(Equal(mXAxis.LengthSquared(), 1.0));
	ASSERT(Equal(mZAxis.LengthSquared(), 1.0));
	
	ASSERT(Equal(DotProduct(mYAxis, mXAxis), 0.0));		// vectors should be orthogonal
	ASSERT(Equal(DotProduct(mYAxis, mZAxis), 0.0));
	ASSERT(Equal(DotProduct(mXAxis, mZAxis), 0.0));
}

#pragma mark ~

//---------------------------------------------------------------
//
// CCamera::DoReset
//
//---------------------------------------------------------------
void CCamera::DoReset()
{
	mZAxis = Normalize(X3DVector(mPlacement.pointOfInterest - mPlacement.location));
	
	mYAxis = Normalize(mPlacement.upVector - DotProduct(mPlacement.upVector, mZAxis)*mZAxis);
	mXAxis = Normalize(CrossProduct(mZAxis, mYAxis));
}

