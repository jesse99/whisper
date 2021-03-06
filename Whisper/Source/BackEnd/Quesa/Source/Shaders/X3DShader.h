/*
 *  File:       X3DShader.h
 *  Summary:	Base class for QuickDraw 3D shaders.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DShader.h,v $
 *		Revision 1.4  2001/04/14 07:31:48  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:54:01  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:20:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaShader.h>

#include <X3DShape.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DShader
//!		Base class for Quesa shaders.
// ===================================================================================
class QUESA_EXPORT X3DShader : public X3DShape {

	typedef X3DShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DShader();
						
						X3DShader(TQ3ShaderObject object);
						
	virtual X3DShader* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3ShaderObject() const				{return mObject;}
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const;
	//@}
									
	//! @name UV Transform
	//@{
			TQ3Matrix3x3 GetUVTransform() const;
						
			void 		SetUVTransform(const TQ3Matrix3x3& transform);
	//@}
			
	//! @name U Boundary
	//@{
			TQ3ShaderUVBoundary GetUBoundary() const;
			
			void 		SetUBoundary(const TQ3ShaderUVBoundary& boundary);
	//@}

	//! @name V Boundary
	//@{
			TQ3ShaderUVBoundary GetVBoundary() const;
			
			void 		SetVBoundary(const TQ3ShaderUVBoundary& boundary);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DShader. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
