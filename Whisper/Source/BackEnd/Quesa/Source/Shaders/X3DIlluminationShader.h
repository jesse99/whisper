/*
 *  File:       X3DIlluminationShader.h
 *  Summary:	Base class for QuickDraw 3D illumination shaders.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DIlluminationShader.h,v $
 *		Revision 1.3  2000/12/13 08:52:02  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:16:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DIlluminationShader.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:52:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	 12/03/97	JDJ		No longer an abstract class.
 *		 <1>	  2/17/97	JDJ		Created.
 */

#pragma once

#include <X3DShader.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DIlluminationShader
//!		Base class for Quesa illumination shaders.
// ===================================================================================
class QUESA_EXPORT X3DIlluminationShader : public X3DShader {

	typedef X3DShader Inherited;

public:
	virtual				~X3DIlluminationShader();
	
						X3DIlluminationShader(TQ3ShaderObject object);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DIlluminationShader. */

	virtual X3DIlluminationShader* Clone() const;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
