/*
 *  File:       X3DRenderingLoops.h
 *  Summary:	Classes encapsulating QuickDraw 3D rendering loops.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DRenderLoop			- Draws the submitted shapes into the view's draw context.
 *				X3DBoundingBoxLoop 		- Computes a bounding box for the submitted shapes.
 *				X3DBoundingSphereLoop	- Computes a bounding sphere for the submitted shapes.
 *				X3DPickLoop				- Gets pick information from the submitted shapes.
 *				X3DWriteLoop			- Writes data in a X3DView object to a X3DFile.
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DRenderingLoops.h,v $
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/13 08:53:52  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:20:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <->	  2/23/97	JDJ		Created.
 */

#pragma once

#include <X3DRenderingLoop.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class X3DFile;
class X3DRect;
class X3DSphere;
class X3DPick;


// ===================================================================================
//	class X3DRenderLoop
//!		Draws the submitted shapes into the view's draw context.
// ===================================================================================
class QUESA_EXPORT X3DRenderLoop : public X3DRenderingLoop {

	typedef X3DRenderingLoop Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DRenderLoop();
						
						X3DRenderLoop(TQ3ViewObject view);
						
//-----------------------------------
//	New API
//
public:
			void 		Flush();
			
			void 		Sync();
						/**< Asks the renderer to finish a frame and blocks until the 
						frame is complete. */
						
//-----------------------------------
//	Inherited API
//
public:
	virtual	bool 		IsRendering();
};


// ===================================================================================
//	class X3DBoundingBoxLoop
//!		Computes a bounding box for the submitted shapes.
// ===================================================================================
class QUESA_EXPORT X3DBoundingBoxLoop : public X3DRenderingLoop {

	typedef X3DRenderingLoop Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DBoundingBoxLoop();
						
						X3DBoundingBoxLoop(TQ3ViewObject view, X3DRect* box, TQ3ComputeBounds bounds);
						/**< Bounds can be kQ3ComputeBoundsExact or kQ3ComputeBoundsApproximate. */
						
//-----------------------------------
//	Inherited API
//
public:
	virtual	bool 		IsRendering();

//-----------------------------------
//	Member Data
//
public:
	X3DRect*		mBox;
};


// ===================================================================================
//	class X3DBoundingSphereLoop
//!		Computes a bounding sphere for the submitted shapes.
// ===================================================================================
class QUESA_EXPORT X3DBoundingSphereLoop : public X3DRenderingLoop {

	typedef X3DRenderingLoop Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DBoundingSphereLoop();
						
						X3DBoundingSphereLoop(TQ3ViewObject view, X3DSphere* sphere, TQ3ComputeBounds bounds);
						/**< Bounds can be kQ3ComputeBoundsExact or kQ3ComputeBoundsApproximate. */
						
//-----------------------------------
//	Inherited API
//
public:
	virtual	bool 		IsRendering();

//-----------------------------------
//	Member Data
//
public:
	X3DSphere*	mSphere;
};


// ===================================================================================
//	class X3DPickLoop
//!		Gets pick information from the submitted shapes.
// ===================================================================================
class QUESA_EXPORT X3DPickLoop : public X3DRenderingLoop {

	typedef X3DRenderingLoop Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPickLoop();
						
						X3DPickLoop(TQ3ViewObject view, X3DPick* pick);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual	bool 		IsRendering();
};


// ===================================================================================
//	class X3DWriteLoop
//!		Writes data in a X3DView object to a X3DFile.
// ===================================================================================
class QUESA_EXPORT X3DWriteLoop : public X3DRenderingLoop {

	typedef X3DRenderingLoop Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DWriteLoop();
						
						X3DWriteLoop(TQ3ViewObject view, X3DFile* file);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual	bool 		IsRendering();
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
