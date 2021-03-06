/*
 *  File:       XDrawContexts.h
 *  Summary:	Classes for objects that can be drawn into.
 *  Written by: Jesse Jones
 *
 *	Classes:	XWindowContext	- A context used to draw into a graf port/DC.
 *				XPixMapContext	- A context that draws into an XPixMap.
 *
 *  Copyright � 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XDrawContexts.h,v $
 *		Revision 1.4  2001/03/05 05:41:17  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:51:51  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:33:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XDrawContext.h>
#include <XPen.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XPixMap;


// ===================================================================================
//	class XWindowContext
//!		A context used to draw into a graf port/DC. Note that, on a Mac, this will work with any GrafPort.
// ===================================================================================
class GRAPHICS_EXPORT XWindowContext  : public XDrawContext {

	typedef XDrawContext Inherited;
		
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XWindowContext();
						
						XWindowContext(OSWindow wind);

//-----------------------------------
//	Member Data
//
public:
	OSWindow	mWindow;
};


// ===================================================================================
//	class XPixMapContext
//!		A context that draws into an XPixMap.
// ===================================================================================
class GRAPHICS_EXPORT XPixMapContext  : public XDrawContext {

	typedef XDrawContext Inherited;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XPixMapContext();
						
						XPixMapContext(XPixMap* pixMap);

//-----------------------------------
//	Member Data
//
protected:
	XPixMap*	mPixMap;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

