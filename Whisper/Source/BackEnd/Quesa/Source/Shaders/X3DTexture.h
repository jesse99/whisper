/*
 *  File:       X3DTexture.h
 *  Summary:	Base class for QuickDraw 3D texture map objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTexture.h,v $
 *		Revision 1.3  2000/12/13 08:54:58  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:22:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DTexture.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:54:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	 12/03/97	JDJ		No longer an abstract class.
 *		 <1>	  3/07/97	JDJ		Created.
 */

#pragma once

#include <X3DSharedObject.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DTexture
//!		Base class for Quesa texture map objects.
// ===================================================================================
class QUESA_EXPORT X3DTexture : public X3DSharedObject {

	typedef X3DSharedObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DTexture();
						
						X3DTexture(TQ3TextureObject object);
						
	virtual X3DTexture* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	// ----- Conversion Operator -----
						operator TQ3TextureObject() const				{return mObject;}

	// ----- Dimensions -----
			uint32 		GetWidth() const;
						
			uint32 		GetHeight() const;

	// ----- Misc -----
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DTexture. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
