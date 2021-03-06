/*
 *  File:       X3DViewHints.h
 *  Summary:	An object that is saved in a metafile to give the reader hints on
 *				how to render a scene.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DViewHints.h,v $
 *		Revision 1.5  2001/04/14 07:33:48  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/13 08:55:59  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:24:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
\ */

#pragma once

#include <QuesaIO.h>

#include <X3DAttributeSet.h>
#include <X3DCamera.h>
#include <X3DGroups.h>
#include <X3DRenderer.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class X3DView;


// ===================================================================================
//	class X3DViewHints
//!		An object that is saved in a metafile to give the reader hints on how to render a scene.
// ===================================================================================
class QUESA_EXPORT X3DViewHints : public X3DSharedObject {

	typedef X3DSharedObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DViewHints();
						
						X3DViewHints(const X3DView& view);
						
						X3DViewHints(TQ3ViewHintsObject object);

	virtual X3DViewHints* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3ViewHintsObject() const				{return mObject;}
	//@}

	//! @name Renderer
	//@{
			X3DRenderer GetRenderer() const;
			
			void 		SetRenderer(const X3DRenderer& renderer);
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

	//! @name Attributes
	//@{
			X3DAttributeSet GetAttributeSet() const;
			
			void 		SetAttributeSet(const X3DAttributeSet& attributes);
	//@}

	//! @name Dimension State
	//@{
			bool 		GetDimensionState() const;
			
			void 		SetDimensionState(bool state);
	//@}

	//! @name Dimensions
	//@{
			XSize 	GetDimensions() const;
			
			void 		SetDimensions(const XSize& size);
	//@}

	//! @name Mask State
	//@{
			bool 		GetMaskState() const;
			
			void 		SetMaskState(bool state);
	//@}

	//! @name Mask
	//@{
			void 		GetMask(TQ3Bitmap* mask) const;
						/**< Call Q3Bitmap_Empty when you're done with the mask. */
			
			void 		SetMask(const TQ3Bitmap* mask);
	//@}

	//! @name Clear Image Method
	//@{
			TQ3DrawContextClearImageMethod GetClearImageMethod() const;

			void 		SetClearImageMethod(TQ3DrawContextClearImageMethod method);
						/**< Method can be kQ3ClearMethodNone or kQ3ClearMethodWithColor. */
	//@}

	//! @name Clear Color
	//@{
			X3DColorARGB GetClearColor() const;

			void 		SetClearColor(const X3DColorARGB& color);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DViewHints. */
	//@}
};



#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
