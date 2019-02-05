/*
 *  File:       X3DStrings.h
 *  Summary:	QuickDraw 3D string objects.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DCString	- C-style string.
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DStrings.h,v $
 *		Revision 1.3  2000/12/13 08:54:27  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:21:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DStrings.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:54:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  2/16/97	JDJ		Created.
 */

#pragma once

#include <String>

#include <X3DString.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DCString
//!		A C-style string.
// ===================================================================================
class QUESA_EXPORT X3DCString : public X3DString {

	typedef X3DString Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DCString();
						
						X3DCString(const std::wstring& str);
						
						X3DCString(TQ3StringObject object);

//-----------------------------------
//	New API
//
public:
			uint32 		GetLength() const;
			
			void 		SetString(const std::wstring& str);
			
			std::wstring GetString() const;

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DCString. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

