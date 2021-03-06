/*
 *  File:       X3DView.cpp
 *  Summary:	Encapsulates a QuickDraw 3D TQ3ViewObject.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DView.cpp,v $
 *		Revision 1.4  2001/04/21 03:39:06  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/03/03 23:58:47  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.2  2000/11/09 12:24:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DView.h>

#include <X3DPick.h>
#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class X3DViewIdler
// ===================================================================================

//---------------------------------------------------------------
//
// X3DViewIdler::~X3DViewIdler
//
//---------------------------------------------------------------
X3DViewIdler::~X3DViewIdler()
{
}


//---------------------------------------------------------------
//
// X3DViewIdler::X3DViewIdler
//
//---------------------------------------------------------------
X3DViewIdler::X3DViewIdler()
{
}


//---------------------------------------------------------------
//
// X3DViewIdler::HandleIdle
//
//---------------------------------------------------------------
bool X3DViewIdler::HandleIdle()
{
	bool proceed = this->OnIdle();
	
	return proceed;
}

#pragma mark -

// ===================================================================================
//	class X3DViewProgressIdler
// ===================================================================================

//---------------------------------------------------------------
//
// X3DViewProgressIdler::~X3DViewProgressIdler
//
//---------------------------------------------------------------
X3DViewProgressIdler::~X3DViewProgressIdler()
{
}


//---------------------------------------------------------------
//
// X3DViewProgressIdler::X3DViewProgressIdler
//
//---------------------------------------------------------------
X3DViewProgressIdler::X3DViewProgressIdler()
{
}


//---------------------------------------------------------------
//
// X3DViewProgressIdler::HandleProgress
//
//---------------------------------------------------------------
bool X3DViewProgressIdler::HandleProgress(uint32 current, uint32 completed)
{
	bool proceed = true;
	
	if (current == 0 && completed > 0)
		this->OnProgressStart();
		
	else if (current > 0 && current < completed)
		proceed = this->OnProgress((float) current/completed);
		
	else if (current > 0 && current == completed)
		this->OnProgressEnd();
		
	else
		this->OnNoProgressInfo();			// will only be called once
		
	return proceed;
}

#pragma mark -

// ===================================================================================
//	class X3DEndFrame
// ===================================================================================

//---------------------------------------------------------------
//
// X3DEndFrame::~X3DEndFrame
//
//---------------------------------------------------------------
X3DEndFrame::~X3DEndFrame()
{
}


//---------------------------------------------------------------
//
// X3DEndFrame::X3DEndFrame
//
//---------------------------------------------------------------
X3DEndFrame::X3DEndFrame()
{
}


//---------------------------------------------------------------
//
// X3DEndFrame::HandleEndFrame
//
//---------------------------------------------------------------
void X3DEndFrame::HandleEndFrame()
{
	this->OnEndFrame();
}

#pragma mark -

// ===================================================================================
//	class X3DView
// ===================================================================================

//---------------------------------------------------------------
//
// X3DView::~X3DView
//
//---------------------------------------------------------------
X3DView::~X3DView()
{	
	delete mIdler;
	delete mProgress;
	delete mEndFrame;
}


//---------------------------------------------------------------
//
// X3DView::X3DView ()
//
//---------------------------------------------------------------
X3DView::X3DView() : X3DObject(Q3View_New())
{
	mIdler    = nil;
	mProgress = nil;
	mEndFrame = nil;
}


//---------------------------------------------------------------
//
// X3DView::X3DView (TQ3ViewObject)
//
//---------------------------------------------------------------
X3DView::X3DView(TQ3ViewObject object) : X3DObject(object)
{
	this->ConfirmType(kQ3ObjectTypeView);

	mIdler    = nil;
	mProgress = nil;
	mEndFrame = nil;
}


//---------------------------------------------------------------
//
// X3DView::Clone
//
//---------------------------------------------------------------
X3DView* X3DView::Clone() const
{
	TQ3Object newObject = Q3Object_Duplicate(mObject);
	ThrowIfNil(newObject);
	
	X3DView* object = new X3DView(newObject);
	
	return object;
}


//---------------------------------------------------------------
//
// X3DView::IsBoundingBoxVisible
//
//---------------------------------------------------------------
bool X3DView::IsBoundingBoxVisible(const X3DRect& inBox) const
{
	TQ3Boolean visible = kQ3False;
	
	if (!inBox.IsEmpty()) {
		TQ3BoundingBox box;
		box.min     = inBox.origin;
		box.max     = inBox.origin + inBox.size;
		box.isEmpty = kQ3False;
		
		visible = Q3View_IsBoundingBoxVisible(mObject, &box);
	}
	
	return visible;
}


//---------------------------------------------------------------
//
// X3DView::GetCamera
//
//---------------------------------------------------------------
X3DCamera X3DView::GetCamera() const
{
	TQ3CameraObject object;
	
	TQ3Status status = Q3View_GetCamera(mObject, &object);
	ThrowIf3DError(status);
	
	X3DCamera obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DView::SetCamera
//
//---------------------------------------------------------------
void X3DView::SetCamera(const X3DCamera& camera)
{
	TQ3Status status = Q3View_SetCamera(mObject, camera);
	ThrowIf3DError(status);
}
 		

//---------------------------------------------------------------
//
// X3DView::GetLightGroup
//
//---------------------------------------------------------------
X3DLightGroup X3DView::GetLightGroup() const
{
	TQ3GroupObject object;
	
	TQ3Status status = Q3View_GetLightGroup(mObject, &object);
	ThrowIf3DError(status);
	
	X3DLightGroup obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DView::SetLightGroup
//
//---------------------------------------------------------------
void X3DView::SetLightGroup(const X3DLightGroup& lights)
{
	TQ3Status status = Q3View_SetLightGroup(mObject, lights);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DView::SetRendererByType
//
//---------------------------------------------------------------
void X3DView::SetRendererByType(TQ3ObjectType type)
{
	TQ3Status status = Q3View_SetRendererByType(mObject, type);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DView::SetRenderer
//
//---------------------------------------------------------------
void X3DView::SetRenderer(const X3DRenderer& renderer)
{
	TQ3Status status = Q3View_SetRenderer(mObject, renderer);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DView::GetRenderer
//
//---------------------------------------------------------------
X3DRenderer X3DView::GetRenderer() const
{
	TQ3RendererObject object;
	
	TQ3Status status = Q3View_GetRenderer(mObject, &object);
	ThrowIf3DError(status);
	
	X3DRenderer obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DView::GetDrawContext
//
//---------------------------------------------------------------
X3DDrawContext X3DView::GetDrawContext() const
{
	TQ3DrawContextObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mObject, &object);
	ThrowIf3DError(status);
	
	X3DDrawContext obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DView::SetDrawContext
//
//---------------------------------------------------------------
void X3DView::SetDrawContext(const X3DDrawContext& context)
{
	TQ3Status status = Q3View_SetDrawContext(mObject, context);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DView::SetIdleMethod (TQ3ViewIdleMethod, const void*)
//
//---------------------------------------------------------------
void X3DView::SetIdleMethod(TQ3ViewIdleMethod callback, const void* refCon)
{
	PRECONDITION(mIdler == nil);
	
	TQ3Status status = Q3View_SetIdleMethod(mObject, callback, refCon);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DView::SetIdleMethod (X3DViewIdler*)
//
//---------------------------------------------------------------
void X3DView::SetIdleMethod(X3DViewIdler* callback)
{
	delete mIdler;
	mIdler = callback;
	
	TQ3Status status = kQ3Success;
	if (mIdler != nil)
		status = Q3View_SetIdleMethod(mObject, X3DView::DoIdle, this);
	else
		status = Q3View_SetIdleMethod(mObject, nil, nil);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DView::SetIdleProgressMethod (TQ3ViewIdleProgressMethod, const void*)
//
//---------------------------------------------------------------
void X3DView::SetIdleProgressMethod(TQ3ViewIdleProgressMethod callback, const void* refCon)
{
	PRECONDITION(mProgress == nil);
	
	TQ3Status status = Q3View_SetIdleProgressMethod(mObject, callback, refCon);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DView::SetIdleProgressMethod (X3DViewProgressIdler*)
//
//---------------------------------------------------------------
void X3DView::SetIdleProgressMethod(X3DViewProgressIdler* callback)
{
	delete mProgress;
	mProgress = callback;
	
	TQ3Status status = kQ3Success;
	if (mIdler != nil)
		status = Q3View_SetIdleProgressMethod(mObject, X3DView::DoIdleProgress, this);
	else
		status = Q3View_SetIdleProgressMethod(mObject, nil, nil);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DView::SetEndFrameMethod (TQ3ViewEndFrameMethod, const void*)
//
//---------------------------------------------------------------
void X3DView::SetEndFrameMethod(TQ3ViewEndFrameMethod callback, const void* refCon)
{
	PRECONDITION(mEndFrame == nil);
	
	TQ3Status status = Q3View_SetEndFrameMethod(mObject, callback, const_cast<void*>(refCon));
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DView::SetEndFrameMethod (X3DEndFrame*)
//
//---------------------------------------------------------------
void X3DView::SetEndFrameMethod(X3DEndFrame* callback)
{
	delete mEndFrame;
	mEndFrame = callback;
	
	TQ3Status status = kQ3Success;
	if (mIdler != nil)
		status = Q3View_SetEndFrameMethod(mObject, X3DView::DoEndFrame, this);
	else
		status = Q3View_SetEndFrameMethod(mObject, nil, nil);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DView::AllowAllGroupCulling
//
//---------------------------------------------------------------
void X3DView::AllowAllGroupCulling(bool allowCulling)
{
	TQ3Status status = Q3View_AllowAllGroupCulling(mObject, (TQ3Boolean) allowCulling);
	ThrowIf3DError(status);
}

#pragma mark �

//---------------------------------------------------------------
//
// X3DView::DoIdle										[static]
//
//---------------------------------------------------------------
extern "C" TQ3Status X3DView::DoIdle(TQ3ViewObject, const void* refCon)
{
	TQ3Status status = kQ3Success;
	
	try {
		X3DView* view = static_cast<X3DView*>(const_cast<void*>(refCon));
		PRECONDITION(view->mIdler != nil);
		
		bool proceed = view->mIdler->HandleIdle();
		if (!proceed)
			status = kQ3Failure;
			
	} catch (...) {
		status = kQ3Failure;
	}
	
	return status;
}


//---------------------------------------------------------------
//
// X3DView::DoIdleProgress								[static]
//
//---------------------------------------------------------------
extern "C" TQ3Status X3DView::DoIdleProgress(TQ3ViewObject, const void* refCon, uint32 current, uint32 completed)
{
	TQ3Status status = kQ3Success;
	
	try {
		X3DView* view = static_cast<X3DView*>(const_cast<void*>(refCon));
		PRECONDITION(view->mProgress != nil);
		
		bool proceed = view->mProgress->HandleProgress(current, completed);
		if (!proceed)
			status = kQ3Failure;
			
	} catch (...) {
		status = kQ3Failure;
	}
	
	return status;
}


//---------------------------------------------------------------
//
// X3DView::DoEndFrame									[static]
//
//---------------------------------------------------------------
extern "C" void X3DView::DoEndFrame(TQ3ViewObject, void* refCon)
{
	try {
		X3DView* view = static_cast<X3DView*>(refCon);
		PRECONDITION(view->mEndFrame != nil);
		
		view->mEndFrame->HandleEndFrame();
			
	} catch (...) {
		DEBUGSTR("Got an error in X3DView::DoEndFrame");	// eat exceptions (since we can't propagate them)
	}
}


//---------------------------------------------------------------
//
// X3DView::IsCompatible								[static]
//	
//---------------------------------------------------------------
bool X3DView::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ObjectTypeView);
	
	return compatible;
}


}	// namespace Whisper
