/*
 *  File:       X3DView.h
 *  Summary:	Encapsulates a QuickDraw 3D TQ3ViewObject.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DView.h,v $
 *		Revision 1.4  2001/04/14 07:33:44  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:55:54  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:24:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaView.h>

#include <X3DCamera.h>
#include <X3DDrawContext.h>
#include <X3DGroups.h>
#include <X3DObject.h>
#include <X3DRenderer.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DViewIdler
//!		Abstract base class for objects called by X3DView during long operations (eg rendering).
// ===================================================================================
class QUESA_EXPORT X3DViewIdler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DViewIdler();
						
						X3DViewIdler();
						
//-----------------------------------
//	API
//
public:
	virtual bool 		HandleIdle();

protected:
	virtual bool 		OnIdle() = 0;
						/**< Return false to abort the operation. */
};


// ===================================================================================
//	class X3DViewProgressIdler
//!		Like X3DViewIdler except that progress information is supplied. Note that not all renderers support this kind of idler. 
// ===================================================================================
class X3DViewProgressIdler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DViewProgressIdler();
						
						X3DViewProgressIdler();
						
//-----------------------------------
//	API
//
public:
	virtual bool 		HandleProgress(uint32 current, uint32 completed);
	
protected:
	virtual void 		OnProgressStart()				{}
	
	virtual bool 		OnProgress(float percent) = 0;
						/**< Return false to abort the operation. */
	
	virtual void 		OnProgressEnd()					{}
	
	virtual void 		OnNoProgressInfo()				{}
						/**< Called once if the renderer doesn't support progress idlers. */
};


// ===================================================================================
//	class X3DEndFrame
//!		Abstract base class for objects that are called when the renderer finishes a frame.
// ===================================================================================
class X3DEndFrame {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DEndFrame();
						
						X3DEndFrame();
						
//-----------------------------------
//	API
//
public:
	virtual void 		HandleEndFrame();

protected:
	virtual void 		OnEndFrame() = 0;
};


// ===================================================================================
//	class X3DView
//!		TQ3ViewObject wrapper.
// ===================================================================================
class X3DView : public X3DObject {

	typedef X3DObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DView();
						
						X3DView();
						
						X3DView(TQ3ViewObject object);

	virtual X3DView* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3ViewObject() const					{return mObject;}
	//@}

	//! @name Bounding Box
	//@{
			bool 		IsBoundingBoxVisible(const X3DRect& box) const;
						/**< Checks to see if the bounding box is visible in the viewing frustrum.
						Can be used by applications to cull out the parts of the scene that
						are not going to be visible. */
	//@}
								
	//! @name Camera
	//@{
			X3DCamera 	GetCamera() const;
			
			void 		SetCamera(const X3DCamera& camera); 		
	//@}

	//! @name Lights
	//@{
			X3DLightGroup GetLightGroup() const;
			
			void 		SetLightGroup(const X3DLightGroup& lights);
	//@}

	//! @name Renderer
	//@{
			void 		SetRendererByType(TQ3ObjectType type);
			
			void 		SetRenderer(const X3DRenderer& renderer);
			
			X3DRenderer GetRenderer() const;
	//@}
			
	//! @name Draw Context
	//@{
			X3DDrawContext GetDrawContext() const;
			
			void 		SetDrawContext(const X3DDrawContext& context);
	//@}

	//! @name Idle Method
	//@{
			void 		SetIdleMethod(TQ3ViewIdleMethod callback, const void* refCon);
						/**< Registers a callback that will be called during long operations
						(like rendering). If the callback returns kQ3Failure the operation
						is aborted. */
						
			void 		SetIdleMethod(X3DViewIdler* takeCallback);
	//@}

	//! @name Idle Progress Method
	//@{
			void 		SetIdleProgressMethod(TQ3ViewIdleProgressMethod callback, const void* refCon);
						/**< Registers an idle method that receives progress information. Note
						that not all renderers support this (if the renderer doesn't support
						a progress idler the callback will be called once with current == 0
						and completed == 0). */
						
			void 		SetIdleProgressMethod(X3DViewProgressIdler* takeCallback);
						/**< Registers a callback that causes OnProgressStart, OnProgress, and 
						OnProgressEnd to be called during long operations. */
	//@}

	//! @name End Frame Method
	//@{
			void 		SetEndFrameMethod(TQ3ViewEndFrameMethod callback, const void* refCon);
						/**< Registers a callback that is called when the renderer finishes a
						frame. If Sync is called before the callback executes the callback
						will not be called. */
						
			void 		SetEndFrameMethod(X3DEndFrame* takeCallback);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DView. */

			void 		AllowAllGroupCulling(bool allowCulling = true);
						/**< Enable and disable group culling. Defaults to allowing culling.
						QD3D 1.6 */
	//@}

//-----------------------------------
//	Internal API
//
private:
	static	TQ3Status 	DoIdle(TQ3ViewObject view, const void* refCon);

	static	TQ3Status 	DoIdleProgress(TQ3ViewObject view, const void* refCon, uint32 current, uint32 completed);

	static	void 		DoEndFrame(TQ3ViewObject view, void* refCon);

//-----------------------------------
//	Member Data
//
protected:
	X3DViewIdler*			mIdler;
	X3DViewProgressIdler*	mProgress;
	X3DEndFrame*			mEndFrame;
};



#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
