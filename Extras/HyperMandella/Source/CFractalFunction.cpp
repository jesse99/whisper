/*
 *  File:       CFractalFunction.cpp
 *  Summary:   	Abstract base class for objects used to iterate fractal formulas. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: CFractalFunction.cpp,v $
 *		Revision 1.2  2001/04/21 02:24:19  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2000/12/28 02:49:23  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:54:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "CFractalFunction.h"

#include <XOptimize.h>

#include "IDocInfo.h"


// ===================================================================================
//	class CFractalFunction
// ===================================================================================

//---------------------------------------------------------------
//
// CFractalFunction::~CFractalFunction
//
//---------------------------------------------------------------
CFractalFunction::~CFractalFunction()
{
}


//---------------------------------------------------------------
//
// CFractalFunction::CFractalFunction
//
//---------------------------------------------------------------
CFractalFunction::CFractalFunction(XBoss* boss)
{
	IFractalFunction::DoSetBoss(boss);

	mUsesConstant = false;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CFractalFunction::ComputeDepth	
//
//---------------------------------------------------------------
float CFractalFunction::ComputeDepth(const SFractalInfo& info, const X3DPoint& startPt, const X3DVector& eyeRay, float viewDepth, int32 pixelDepth) const
{	
	PRECONDITION(Equal(eyeRay.LengthSquared(), 1.0));
	
	double delta = viewDepth/pixelDepth;
	ASSERT(delta > 0.0);

	double bailout  = info.bailout;				// make sure everything in the loop is in a register!
	uint32 maxDwell = info.maxDwell;
	int32  pixels   = pixelDepth;
	uint32 count    = 0;
	uint32 dwell    = 0;
	
	double px = startPt.x;
	double py = startPt.y;
	double pz = startPt.z;
	double pw = info.w;
		
	double cx = info.constant.x;
	double cy = info.constant.y;
	double cz = info.constant.z;
	double cw = info.constant.w;
	
	double sx = delta*eyeRay.x;
	double sy = delta*eyeRay.y;
	double sz = delta*eyeRay.z;	

	mLambda = info.lambda;
		
	// Find the first point that's inside the fractal.
	do {		
		dwell = this->OnComputeDwell(px, py, pz, pw, cx, cy, cz, cw, bailout, maxDwell);	// $$$ last I checked performance was measurably better if a function pointer is used instead of a virtual method
																							// $$$ since we're not currently supporting plugins we could even write some assembler PPC glue to skip the call to ptr_glue
		px += sx;						
		py += sy;
		pz += sz;
	} while (dwell < maxDwell && ++count < pixels);
	
	// If we hit the fractal then do a binary search to get a better
	// depth estimate. (Note that the profiler says that this contributes
	// essentially nothing to the render time).
	double depth = INFINITY;
	if (dwell >= maxDwell) {
		depth = delta*count;

		if (count > 0) {
			const double kThreshold = delta/64.0;
	
			while (delta > kThreshold) {
				delta = scalbn(delta, -1);
				if (dwell >= maxDwell)
					depth -= delta;
				else
					depth += delta;

				px = startPt.x + depth*eyeRay.x;
				py = startPt.y + depth*eyeRay.y;
				pz = startPt.z + depth*eyeRay.z;

				dwell = this->OnComputeDwell(px, py, pz, pw, cx, cy, cz, cw, bailout, maxDwell);
			}
		}	
	}
	
	return (float) depth;
}



