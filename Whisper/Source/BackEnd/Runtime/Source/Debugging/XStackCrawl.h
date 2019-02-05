/*
 *  File:       XStackCrawl.h
 *  Summary:   	Cross platform stack crawl class.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStackCrawl.h,v $
 *		Revision 1.4  2001/03/05 05:43:27  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:55:56  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:52:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <cstddef>

#if WIN
	#include <imagehlp.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class StackID;


//-----------------------------------
//	Types
//
typedef const StackID* StackFrameID;	// magic cookie


// ===================================================================================
//	struct SStackFrame
//!		Encapsulates an entry in the stack frame.
// ===================================================================================
struct RUNTIME_EXPORT SStackFrame {
	char			name[256];		//!< name of the function
	const void*		start;			//!< pointer to the start of the function
	uint32			offset;			//!< offset from the start of the function to the PC
#if MAC
	bool			native;			//!< true if function is PPC code
#endif
};


// ===================================================================================
//	class XStackCrawl
//!		Cross platform stack crawl class.
// ===================================================================================
class RUNTIME_EXPORT XStackCrawl {

	enum {kMaxFrames = 32};

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XStackCrawl();

						XStackCrawl(uint32 startFrame = 0, uint32 numFrames = kMaxFrames);
						/**< Frame numbers start at 0 which is the caller of the XStackCrawl 
						constructor and work upwards to the base of the stack. */
				
//-----------------------------------
//	API
//
public:
			uint32 		GetNumFrames() const					{return mNumFrames;}
			
			SStackFrame GetFrame(uint32 frame) const;
			
			StackFrameID GetID(uint32 frame) const;
						/**< Returns an identifier (or cookie) that can be used later to 
						generate the stack frame. */
			
	static	SStackFrame GetFrame(StackFrameID id);
	
	static 	SStackFrame GetCaller();
						/**< Returns the stack frame of the function that called the
						function that called GetCaller. */
						
//-----------------------------------
//	Internal API
//
protected:
#if MAC
	static 	SStackFrame DoGetFrame(uint32 pc, bool native);
#endif

//-----------------------------------
//	Member Data
//
protected:
	uint32  			mNumFrames;
	const void*			mFrame[kMaxFrames];		// frames starting at startFrame
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

