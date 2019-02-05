/*
 *  File:       X3DStorage.h
 *  Summary:	Base class for QuickDraw 3D storage objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DStorage.h,v $
 *		Revision 1.3  2000/12/13 08:54:14  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:20:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DStorage.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:54:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	 12/03/97	JDJ		No longer an abstract class.
 *		 <1>	  3/01/97	JDJ		Created.
 */

#pragma once

#include <X3DSharedObject.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DStorage
//!		Base class for Quesa storage objects.
// ===================================================================================
class QUESA_EXPORT X3DStorage : public X3DSharedObject {

	typedef X3DSharedObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DStorage();
												
						X3DStorage(TQ3StorageObject object);

	virtual X3DStorage* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
			uint32 		GetSize() const;
			
			void 		GetData(uint32 offset, uint32 bytes, void* buffer) const;
						/**< Reads bytes worth of data starting at offset into buffer. */
						
			void 		SetData(uint32 offset, uint32 bytes, const void* buffer);
						/**< Writes bytes worth of data from buffer into storage starting 
						at offset. */

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DStorage. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
