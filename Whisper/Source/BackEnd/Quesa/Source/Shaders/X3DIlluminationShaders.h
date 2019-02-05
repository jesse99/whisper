/*
 *  File:       X3DIlluminationShaders.h
 *  Summary:	QuickDraw 3D illumination shaders.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DPhongShader	 - A shader using ambient, diffuse reflections, and specular reflection.
 *				X3DLambertShader - A shader using only ambient and diffuse light.
 *				X3DNullShader	 - A shader that uses diffuse colors (ie it ignores any lights).
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DIlluminationShaders.h,v $
 *		Revision 1.3  2000/12/13 08:52:06  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:16:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DIlluminationShaders.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:52:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  2/17/97	JDJ		Created.
 */

#pragma once

#include <X3DIlluminationShader.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DPhongShader
//!		A shader using ambient, diffuse reflections, and specular reflection.
// ===================================================================================
class QUESA_EXPORT X3DPhongShader : public X3DIlluminationShader {

	typedef X3DIlluminationShader Inherited;

public:
	virtual				~X3DPhongShader();
	
						X3DPhongShader();
						
						X3DPhongShader(TQ3ShaderObject object);

	virtual X3DPhongShader* Clone() const;
		
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPhongShader. */
};


// ===================================================================================
//	class X3DLambertShader
//!		A shader using only ambient and diffuse light.
// ===================================================================================
class QUESA_EXPORT X3DLambertShader : public X3DIlluminationShader {

	typedef X3DIlluminationShader Inherited;

public:
	virtual				~X3DLambertShader();
	
						X3DLambertShader();
						
						X3DLambertShader(TQ3ShaderObject object);

	virtual X3DLambertShader* Clone() const;
		
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DLambertShader. */
};


// ===================================================================================
//	class X3DNullShader
//!		A shader that uses diffuse colors (ie it ignores any lights).
// ===================================================================================
class QUESA_EXPORT X3DNullShader : public X3DIlluminationShader {

	typedef X3DIlluminationShader Inherited;

public:
	virtual				~X3DNullShader();
	
						X3DNullShader();
						
						X3DNullShader(TQ3ShaderObject object);

	virtual X3DNullShader* Clone() const;
		
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DNullShader. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
