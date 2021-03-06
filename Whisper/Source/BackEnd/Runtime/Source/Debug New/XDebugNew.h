/*
 *  File:       XDebugNew.h
 *  Summary:    Debugging version of operator new.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XDebugNew.h,v $
 *		Revision 1.4  2001/03/05 05:41:05  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:51:29  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:30:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <new>

namespace Whisper {
#if WHISPER_OPERATOR_NEW && DEBUG

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XDebugMalloc;


//-----------------------------------
//	Globals
//
extern RUNTIME_EXPORT XDebugMalloc* gDebugMalloc;

extern RUNTIME_EXPORT bool gValidateHeapOnNew;
extern RUNTIME_EXPORT bool gValidateHeapOnDelete;
extern RUNTIME_EXPORT bool gFreeDeletedBlocks;	


// ===================================================================================
//	New and Delete
// ===================================================================================
void* 		WhisperOperatorNew(uint32 size, bool isArray);
			/**< Note that you can disable Whisper's operator new by defining WHISPER_OPERATOR_NEW
			as 0. */

void 		WhisperOperatorDelete(void* obj, bool isArray);


// ===================================================================================
//	class XDisableLeakChecking
//!		While one of these objects is alive newly allocated blocks are not leak checked.
// ===================================================================================
class RUNTIME_EXPORT XDisableLeakChecking {

public:
						~XDisableLeakChecking();
						
						XDisableLeakChecking();
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

#endif	// WHISPER_OPERATOR_NEW && DEBUG
}		// namespace Whisper


