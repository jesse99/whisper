/*
 *  File:		XPointer.h
 *  Summary:	A class that encapsulates non-relocatable memory.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XPointer.h,v $
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/10 08:54:32  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:47:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	 2/01/00	JDJ		Made swap a friend function.
 *		 <1>	 1/07/98	JDJ		Created.
 */

#pragma once

#include <XConstants.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class ZPointerRef;


// ===================================================================================
//	class XPointer
//!		A class that encapsulates non-relocatable memory.
/*!		XPointer uses reference counting so that when an XPointer is copied the old and 
 *		new objects point to the same block of memory. This means that XPointer objects 
 *		can be treated just like regular pointer's. In particular, they can be passed by 
 *		value without allocating a new chunk of memory.
 *
 *		There are three ways to allocate memory in Whisper: operator new, XHandle, and 
 *		XPointer. The new operator should be used to allocate small amounts of memory 
 *		that won't change size. XMemHandle should be used for large chunks of memory. 
 *		XPointer should be used for large chunks of memory that cannot move. */
// ===================================================================================
class CORE_EXPORT XPointer {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XPointer();

	explicit 			XPointer(uint32 bytes = 0, bool zeroBytes = kDontZeroBytes);

						XPointer(const XPointer& rhs);

			XPointer& 	operator=(const XPointer& rhs);
	
//-----------------------------------
//	API
//
public:
			const uint8* GetPtr() const;

			uint8* 		GetPtr();

			uint32 		GetSize() const;
			
			void 		SetSize(uint32 bytes, bool zeroBytes = kDontZeroBytes);	
						/**< Note that this may invalidate references to the pointer! */

			void 		Detach();
						/**< If the letter is being shared this will create a new letter
						referenced only by 'this'. */
	
			bool		operator==(const XPointer& rhs) const;
						/**< Byte by byte comparison. */

			bool		operator!=(const XPointer& rhs) const		{return !this->operator==(rhs);}

	friend 	void 		swap(XPointer& lhs, XPointer& rhs)			{std::swap(lhs.mPointerRef, rhs.mPointerRef);}

//-----------------------------------
//	Member Data
//
private:
	ZPointerRef*	mPointerRef;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
