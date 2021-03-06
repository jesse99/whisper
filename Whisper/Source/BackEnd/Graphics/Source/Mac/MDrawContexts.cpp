/*
 *  File:       MDrawContexts.cpp
 *  Summary:	Classes for objects that can be drawn into.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: MDrawContexts.cpp,v $
 *		Revision 1.3  2001/04/21 03:25:27  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:12:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDrawContexts.h>

#include <MQuickDrawUtils.h>
#include <XExceptions.h>
#include <XLocker.h>
#include <XPixMap.h>

namespace Whisper {


// ===================================================================================
//	class XWindowContext
// ===================================================================================

//---------------------------------------------------------------
//
// XWindowContext::~XWindowContext
//
//---------------------------------------------------------------
XWindowContext::~XWindowContext()
{
}


//---------------------------------------------------------------
//
// XWindowContext::XWindowContext
//
//---------------------------------------------------------------
XWindowContext::XWindowContext(OSWindow wind) : XDrawContext(GetWindowPort(wind))
{
	mWindow = wind;
}

#pragma mark -

// ===================================================================================
//	class XPixMapContext
// ===================================================================================

//---------------------------------------------------------------
//
// XPixMapContext::~XPixMapContext
//
//---------------------------------------------------------------
XPixMapContext::~XPixMapContext()
{
	mPixMap->Unlock();
}


//---------------------------------------------------------------
//
// XPixMapContext::XPixMapContext
//
//---------------------------------------------------------------
XPixMapContext::XPixMapContext(XPixMap* pixMap) : XDrawContext(pixMap->GetOSContext())
{
	PRECONDITION(pixMap != nil);
	
	mOffsceenCanvas = true;

	mPixMap = pixMap;
	mPixMap->Lock();
}


}	// namespace Whisper

