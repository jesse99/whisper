/*
 *  File:       X3DSurfaceShader.h
 *  Summary:	Base class for QuickDraw 3D surface shaders.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DSurfaceShader.h,v $
 *		Revision 1.3  2000/12/13 08:54:50  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:21:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DSurfaceShader.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:54:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	 12/03/97	JDJ		No longer an abstract class.
 *		 <1>	  3/07/97	JDJ		Created.
 */

#pragma once

#include <X3DShader.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DSurfaceShader
//!		Base class for Quesa surface shaders.
// ===================================================================================
class QUESA_EXPORT X3DSurfaceShader : public X3DShader {

	typedef X3DShader Inherited;

public:
	virtual				~X3DSurfaceShader();
	
						X3DSurfaceShader(TQ3SurfaceShaderObject object);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DSurfaceShader. */

	virtual X3DSurfaceShader* Clone() const;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
