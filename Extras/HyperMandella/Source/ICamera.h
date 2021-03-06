/*
 *  File:       ICamera.h
 *  Summary:   	Interface used to generate the rays from the eye to the image plane. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICamera.h,v $
 *		Revision 1.5  2001/04/05 03:58:16  jesjones
 *		Added streaming functions to the structs.
 *		
 *		Revision 1.4  2001/03/27 06:30:03  jesjones
 *		Added comparison methods to the structs.
 *		
 *		Revision 1.3  2001/03/26 04:25:46  jesjones
 *		Setter methods now dirty the doc.
 *		
 *		Revision 1.2  2001/03/25 06:08:01  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:02  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 09:00:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <X2DPrimitives.h>
#include <X3DPrimitives.h>
#include <X3DVectors.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


//-----------------------------------
//	Forward References
//
namespace Whisper {
	class XInStream;
	class XOutStream;
}


// ===================================================================================
//	struct SCameraPlacement
// ===================================================================================
struct SCameraPlacement {
	X3DPoint	location;				// eye location
	X3DPoint	pointOfInterest;		// the point we're interested in
	X3DVector	upVector;				// defines which way is up
	
				SCameraPlacement();
				SCameraPlacement(const XXMLElement& element);
				
	bool 		operator==(const SCameraPlacement& rhs) const	{return location == rhs.location && pointOfInterest == rhs.pointOfInterest && upVector == rhs.upVector;}
	bool 		operator!=(const SCameraPlacement& rhs) const	{return !(*this == rhs);}

	XXMLElement* CreateElement() const;

	friend XInStream&  operator>>(XInStream& stream, SCameraPlacement& value);
	friend XOutStream& operator<<(XOutStream& stream, const SCameraPlacement& value);
};


// ===================================================================================
//	struct SCameraRange
// ===================================================================================
struct SCameraRange {
	float	hither;						// distance from the eye to the near clipping plane
	float	yon;						// distance from the eye to the far clipping plane
	
				SCameraRange();
				SCameraRange(const XXMLElement& element);

	bool 		operator==(const SCameraRange& rhs) const	{return hither == rhs.hither && yon == rhs.yon;}
	bool 		operator!=(const SCameraRange& rhs) const	{return !(*this == rhs);}

	XXMLElement* CreateElement() const;

	friend XInStream&  operator>>(XInStream& stream, SCameraRange& value);
	friend XOutStream& operator<<(XOutStream& stream, const SCameraRange& value);
};


// ===================================================================================
//	class ICamera
//!		Interface used to generate the rays from the eye to the image plane. 
// ===================================================================================
class ICamera : public XUnknown {

//-----------------------------------
//	New API
//
public:
	// ----- Camera Placement -----
	virtual const SCameraPlacement& GetPlacement() const = 0;

	virtual void 		SetPlacement(const SCameraPlacement& placement, bool changeAxes = true, bool dirtyDoc = false) = 0;

	// ----- Camera Range -----
	virtual const SCameraRange& GetRange() const = 0;

	virtual void 		SetRange(const SCameraRange& range, bool dirtyDoc = false) = 0;
	
	// ----- Ray Casting -----
	virtual X3DPoint 	GetHitherPoint(const X2DPoint& pixel) const = 0;
						/**< Returns the point in world space corresponding to the
						(normalized) pixel on the hither plane. (0, 0) is
						the bottom left coordinate. */

	virtual X3DVector 	GetEyeRay(const X3DPoint& pt) const = 0;
						/**< Returns a unit vector from the eye to the specified pt. */
						
	// ----- Positioning -----
	virtual void 		OrbitX(double theta, bool dirtyDoc = false) = 0;
	virtual void 		OrbitY(double theta, bool dirtyDoc = false) = 0;
	virtual void 		OrbitZ(double theta, bool dirtyDoc = false) = 0;
						/**< Rotates the camera around a vector passing through the pointOfInterest
						which is parallel to a camera coordinate axis. */

	virtual void 		Pan(const X2DPoint& delta, bool dirtyDoc = false) = 0;
						/**< Translates the camera location and the point of interest by (camera
						relative) delta. */
						
	virtual void 		Zoom(double scaleBy, bool dirtyDoc = false) = 0;
						/**< Camera's distance from pointOfInterest is set to scaleBy times old
						distance. */

	// ----- Axes -----
	virtual X3DVector 	GetXAxis() const = 0;
	virtual X3DVector 	GetYAxis() const = 0;
	virtual X3DVector 	GetZAxis() const = 0;
};

typedef XInterfacePtr<ICamera> ICameraPtr;
typedef XInterfacePtr<const ICamera> IConstCameraPtr;
