/*
 *  File:       CFractalFunction.h
 *  Summary:   	Abstract base class for objects used to iterate fractal formulas. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: CFractalFunction.h,v $
 *		Revision 1.1  2000/12/28 02:49:27  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:54:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 1/30/98	JDJ		Created
 */

#pragma once

#include "CHyperComplex.h"
#include "IFractalFunction.h"


// ===================================================================================
//	class CFractalFunction
// ===================================================================================
class CFractalFunction : public IFractalFunction {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CFractalFunction();
	
  						CFractalFunction(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		UsesConstant() const					{return mUsesConstant;}
	virtual bool 		UsesLambda() const						{return mUsesLambda;}
			
	virtual float 		ComputeDepth(const SFractalInfo& info, const X3DPoint& startPt, const X3DVector& eyeRay, float viewDepth, int32 depth) const;

//-----------------------------------
//	Internal API
//
protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const = 0;

//-----------------------------------
//	Member Data
//
protected:
	bool 					mUsesConstant;
	bool 					mUsesLambda;

	mutable CHyperComplex	mLambda;
};

