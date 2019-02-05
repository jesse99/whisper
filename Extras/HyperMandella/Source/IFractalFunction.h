/*
 *  File:       IFractalFunction.h
 *  Summary:   	Interface used to iterate fractal formulas. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: IFractalFunction.h,v $
 *		Revision 1.2  2001/03/25 06:08:17  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:16  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 09:02:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 1/30/98	JDJ		Created
 */

#pragma once

#include <X3DPrimitives.h>
#include <X3DVectors.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


//-----------------------------------
//	Forward References
//
struct SFractalInfo;


// ===================================================================================
//	class IFractalFunction
//!		Interface used to iterate fractal formulas.
// ===================================================================================
class IFractalFunction : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		UsesConstant() const = 0;

	virtual bool 		UsesLambda() const = 0;
			
	virtual float 		ComputeDepth(const SFractalInfo& info, const X3DPoint& startPt, const X3DVector& eyeRay, float viewDepth, int32 depth) const = 0;
						/**< Returns INFINITY if the fractal surface wasn't hit or the
						distance from startPt to the surface if it was. */
						
	virtual std::wstring GetFormula() const = 0;
						/**< Returns something like "q^2 + c". */
};

typedef XInterfacePtr<IFractalFunction> IFractalFunctionPtr;
typedef XInterfacePtr<const IFractalFunction> IConstFractalFunctionPtr;
