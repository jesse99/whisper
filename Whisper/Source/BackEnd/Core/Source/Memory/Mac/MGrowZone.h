/*
 *  File:       MGrowZone.h
 *  Summary:   	A class that maintains a list of function pointers to be called when
 *				memory goes low.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MGrowZone.h,v $
 *		Revision 1.4  2001/03/05 05:39:16  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:48:35  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:14:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <list>

#include <XAutoPtr.h>
#include <XCallbacks.h>
#include <XCriticalSection.h>
#include <XDebug.h>
#include <XTypes.h>

namespace Whisper {


// ===================================================================================
//	class MGrowZone
//!		A class that maintains a list of function pointers to be called when memory goes low.
// ===================================================================================
class MGrowZone {

//-----------------------------------
//	Types
//
public:
	typedef XCallback2<uint32, uint32, Handle> HookProc;	//!< takes needed bytes, the handle the Memory Manager is working with and returns number of bytes freed

	typedef std::list<HookProc> 	FunctionList;
	typedef FunctionList::iterator 	iterator;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
				   		~MGrowZone();

						MGrowZone();
						
	static MGrowZone* 	Instance();

private:
						MGrowZone(const MGrowZone& rhs);
						
			MGrowZone& 	operator=(const MGrowZone& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Hook Addition and Removal
	//@{
			void 		AddToFront(HookProc proc);
						/**< Proc will be the first function executed by DoGrowZone. If
						it's able to free up enough memory the following hooks will
						not be called. Note that the only Memory Manager calls it
						can make are SetHandleSize/SetPtrSize (shrinking), DisposeHandle/
						DisposePtr, EmptyHandle, HPurge, HSetState, and UnloadSeg (this
						is according to TN 1042). */
			
			void 		AddToBack(HookProc proc);
			
			void 		Remove(const iterator& pos)			{ASSERT(pos != mFunctions.end()); mFunctions.erase(pos);}
	//@}
				
	//! @name Hook Iterating
	//@{
			iterator 	begin()								{return mFunctions.begin();}

			iterator 	end()								{return mFunctions.end();}
	//@}

	//! @name Misc
	//@{
			void 		SetExtraBytes(uint32 extra)			{mExtraBytes = extra;}
						/**< The amount of memory that the grow zone proc will attempt to 
						free above and beyond what the Memory Manager requires.
						Defaults to 32K. */
						
			void 		SetMinBytes(uint32 min)				{mMinBytes = min;}
						/**< The grow zone block will continue purging until the needed
						bytes are freed and the heap has at least min free bytes.
						This defaults to 64K. */
			
			bool 		IsExecuting()						{return mExecuting;}
						/**< Returns true if the grow zone proc is purging objects. If this
						returns true you can dispose of blocks but you cannot allocate
						new blocks. */
						
			Handle 		GetGZHandle()						{return mGZHandle;}
						/**< This may be non-nil while the grow zone proc is executing and
						returns the handle the memory manager is operating on. It cannot
						be resized or deleted. (This handle is also passed to the hook 
						functions so that clients won't forget to check it). */
	//@}

//-----------------------------------
//	Internal API
//
protected:
			uint32	 	DoPurging(uint32 needed);

	static pascal int32 DoGrowZone(int32 needed);
						/**< This will be called after the Memory Manager purges and compacts
						blocks, but fails to create enough space. Note that it will
						continue to be called until either enough memory is freed or
						it returns zero (meaning no bytes were freed). */

//-----------------------------------
//	Member Data
//
protected:
	FunctionList				mFunctions;	
	uint32						mExtraBytes;
	uint32						mMinBytes;
	
	bool						mExecuting;
	Handle						mGZHandle;

private:
	static XAutoPtr<MGrowZone>	msInstance;		
	static XCriticalSection	 	msCreateMutex;
};


}	// namespace Whisper
