/*
 *  File:       X3DRenderingLoop.h
 *  Summary:	Base class for QuickDraw 3D rendering loops.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DRenderingLoop.h,v $
 *		Revision 1.5  2001/04/14 07:31:24  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/13 08:53:47  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:19:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaView.h>

#include <X3DMatrix.h>
#include <X3DStyles.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DPushState
//!		Stack based class used to save a TQ3ViewObject's style and atribute state.
// ===================================================================================
class QUESA_EXPORT X3DPushState {

public:
						~X3DPushState();
						
						X3DPushState(TQ3ViewObject view);
						
private:
	TQ3ViewObject	mView;
};


// ===================================================================================
//	class X3DRenderingLoop
//!		Base class for render loop objects.
/*!		All of the remdering loops are used in a similar way: \code
 *			X3DRenderLoop loop(mView);
 *			do {
 *				// submit drawable objects
 *			} while (loop.IsRendering()); \endcode */ 
// ===================================================================================
class QUESA_EXPORT X3DRenderingLoop {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DRenderingLoop();
						
						X3DRenderingLoop(TQ3ViewObject view);
						
//-----------------------------------
//	API
//
public:
	//! @name Rendering
	//@{
	virtual	bool 		IsRendering() = 0;
						/**< Call this after submitting your shapes to the view. Continue
						submitting the shapes until IsRendering returns false (which 
						will happen when the renderer consumes all the shapes or the 
						user cancels). */
						
			bool 		WasCanceled() const						{return mStatus == kQ3ViewStatusCancelled;}
	//@}

	//! @name Transform
	//@{
			X3DMatrix 	GetLocalToWorldMatrixState() const;

			X3DMatrix 	GetWorldToFrustumMatrixState() const;

			X3DMatrix 	GetFrustumToWindowMatrixState() const;
	//@}

	//! @name Push/Pop
	//@{
			void 		Push();
						/**< Saves the view's style and attribute state (to be exception safe 
						you should use X3DPushState). */
			
			void 		Pop();
	//@}

	//! @name Style State
	//@{
			X3DBackfacingStyle GetBackfacingStyleState() const;
			
			X3DInterpolationStyle GetInterpolationStyleState() const;
			
			X3DFillStyle GetFillStyleState() const;
			
			X3DHighlightStyle GetHighlightStyleState() const;
			
			X3DSubdivisionStyle GetSubdivisionStyleState() const;
			
			X3DOrientationStyle GetOrientationStyleState() const;
			
			X3DReceiveShadowsStyle GetReceiveShadowsStyleState() const;
			
			X3DPickIDStyle GetPickIDStyleState() const;
			
			X3DPickPartsStyle GetPickPartsStyleState() const;
			
			X3DAntiAliasStyle GetAntiAliasStyleState() const;
	//@}

	//! @name Attribute State
	//@{
			TQ3AttributeSet GetDefaultAttributeSet() const;
			
			void 		SetDefaultAttributeSet(TQ3AttributeSet attributes);

			TQ3AttributeSet GetAttributeSetState() const;
			
			void 		GetAttributeState(TQ3AttributeType type, void* data) const;
	//@}

	//! @name Misc
	//@{
			void 		Cancel();
	//@}

//-----------------------------------
//	Member Data
//
protected:
	TQ3ViewObject	mView;
	TQ3ViewStatus	mStatus;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
