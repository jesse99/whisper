/*
 *  File:       X3DSurfaceShaders.h
 *  Summary:	QuickDraw 3D surface shaders.
 *  Written by: Jesse Jones
 *
 *	Classes:	T3TextureShader	- Uses a pixmap to when shading.
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DSurfaceShaders.h,v $
 *		Revision 1.3  2000/12/13 08:54:54  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:21:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DSurfaceShaders.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:54:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <3>	 12/03/97	JDJ		GetTexture returns an object by value (instead
 *									of via a counted pointer).
 *		 <2>	 12/03/97	JDJ		Renamed X3DTextureShader (from T3TextureShader).
 *		 <1>	  3/07/97	JDJ		Created.
 */

#pragma once

#include <X3DSurfaceShader.h>
#include <X3DTexture.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DTextureShader
//!		Shader that uses a pixmap to do texture mapping.
// ===================================================================================
class QUESA_EXPORT X3DTextureShader : public X3DSurfaceShader {

	typedef X3DSurfaceShader Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DTextureShader();
	
						X3DTextureShader(const X3DTexture& texture);

						X3DTextureShader(TQ3SurfaceShaderObject object);

	virtual X3DTextureShader* Clone() const;
		
//-----------------------------------
//	New API
//
public:
			X3DTexture 	GetTexture() const;

			void 		SetTexture(const X3DTexture& texture);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DTextureShader. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
