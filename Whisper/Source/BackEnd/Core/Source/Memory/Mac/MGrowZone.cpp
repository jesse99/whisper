/*
 *  File:       MGrowZone.cpp
 *  Summary:   	A class that maintains a list of function pointers to be called when
 *				memory goes low.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MGrowZone.cpp,v $
 *		Revision 1.7  2001/04/21 03:26:32  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/04/13 07:50:41  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2001/03/03 23:57:28  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.4  2000/12/10 04:04:09  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/27 07:16:00  jesjones
 *		Uses XCallback::IsValid.
 *		
 *		Revision 1.2  2000/11/09 09:14:42  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MGrowZone.h>

#include <XExceptions.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	class MGrowZone
// ===================================================================================

XAutoPtr<MGrowZone>	MGrowZone::msInstance;		
XCriticalSection 	MGrowZone::msCreateMutex;

//---------------------------------------------------------------
//
// MGrowZone::~MGrowZone
//
//---------------------------------------------------------------
MGrowZone::~MGrowZone()
{
	::SetGrowZone(nil);
}


//---------------------------------------------------------------
//
// MGrowZone::MGrowZone
//
//---------------------------------------------------------------
MGrowZone::MGrowZone()
{
	mExtraBytes = 32*1024L;
	mMinBytes   = 64*1024L;
	
	mExecuting = false;
	mGZHandle  = nil;

	GrowZoneUPP gzUPP = NewGrowZoneUPP(MGrowZone::DoGrowZone);
	ThrowIfNil(gzUPP);

	::SetGrowZone(gzUPP);
}


//---------------------------------------------------------------
//
// MGrowZone::Instance									[static]
//
//---------------------------------------------------------------
MGrowZone* MGrowZone::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msCreateMutex);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new MGrowZone);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// MGrowZone::AddToFront
//
//---------------------------------------------------------------
void MGrowZone::AddToFront(HookProc proc)			
{
	PRECONDITION(proc.IsValid()); 
	
	mFunctions.push_front(proc);
}


//---------------------------------------------------------------
//
// MGrowZone::AddToBack
//
//---------------------------------------------------------------
void MGrowZone::AddToBack(HookProc proc)			
{
	PRECONDITION(proc.IsValid()); 
	
	mFunctions.push_back(proc);
}

#pragma mark �

//---------------------------------------------------------------
//
// MGrowZone::DoPurging
//
//---------------------------------------------------------------
uint32 MGrowZone::DoPurging(uint32 needed)
{	
	uint32 purgedBytes = 0;						
	uint32 purged = 1;
	
	iterator iter = this->begin();
	while (iter != this->end() && purged != 0 && purgedBytes < needed) {
		HookProc proc = *iter;
		++iter;
		
		purged = proc(needed - purgedBytes, mGZHandle);
		purgedBytes += purged;
	}
	
	return purgedBytes;
}


//---------------------------------------------------------------
//
// MGrowZone::DoGrowZone								[static]
//
//---------------------------------------------------------------
pascal int32 MGrowZone::DoGrowZone(int32 needed)
{
	int32 oldA5 = SetCurrentA5();		// $$$ if this code is used in Carbon will need to look at thread safety

	uint32 purgedBytes = 0;						

	if (!MGrowZone::Instance()->mExecuting) {	
		try {			
#if !TARGET_CARBON
			SAFE_ASSERT(GetZone() == ApplicationZone());	// $$$ not in Carbon
#endif

			MGrowZone::Instance()->mExecuting = true;
			MGrowZone::Instance()->mGZHandle  = GZSaveHnd();

			needed += MGrowZone::Instance()->mExtraBytes;
			
			int32 freeMem, contigSize;
			uint32 purged = 0;
			do {
				purged = MGrowZone::Instance()->DoPurging(numeric_cast<uint32>(needed));
				purgedBytes += purged;

				PurgeSpace(&freeMem, &contigSize);
			} while (purged != 0 && freeMem < MGrowZone::Instance()->mMinBytes);

			MGrowZone::Instance()->mGZHandle  = nil;
			MGrowZone::Instance()->mExecuting = false;
		
		} catch (...) {
			DEBUGSTR("Got an error in MGrowZone::DoGrowZone");	
		}
				
	} else
		SAFE_DEBUGSTR("Grow zone proc can't be called recursively!");

	(void) SetA5(oldA5);

	return numeric_cast<int32>(purgedBytes);
}


}	// namespace Whisper
