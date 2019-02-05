/*
 *  File:       X3DRenderers.h
 *  Summary:	QuickDraw 3D renderers.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DInteractiveRenderer	- The standard QuickDraw 3D renderer.
 *				X3DWireframeRenderer	- The standard wireframe renderer.
 *
 *  Copyright © 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DRenderers.h,v $
 *		Revision 1.3  2000/12/13 08:53:43  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:19:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DRenderers.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:53:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <3>	  4/19/00	JDJ		Added X3DInteractiveRenderer::CountRAVEDrawContexts 
 *									and GetRAVEDrawContext.
 *		 <2>	 12/03/97	JDJ		Added missing SetVendorID.
 *		 <1>	  2/17/97	JDJ		Created.
 */

#pragma once

#include <X3DRenderer.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DInteractiveRenderer
//!		The standard Quesa renderer.
// ===================================================================================
class QUESA_EXPORT X3DInteractiveRenderer : public X3DRenderer {

	typedef X3DRenderer Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DInteractiveRenderer();
						
						X3DInteractiveRenderer();

						X3DInteractiveRenderer(TQ3RendererObject object);
						
	virtual X3DInteractiveRenderer* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Constructive Solid Geometry
	//@{
			TQ3CSGEquation GetCSGEquation() const;

			void 		SetCSGEquation(TQ3CSGEquation equation);
	//@}

	//! @name Engine ID
	//@{
			long 		GetEngineID() const;
			
			void 		SetEngineID(long engine);
	//@}

	//! @name Vendor ID
	//@{
			long 		GetVendorID() const;
			
			void 		SetVendorID(long vendor);
						/**< Use kQAVendor_BestChoice to get the best renderer. */
	//@}

	//! @name Double Buffer Bypassing
	//@{
			bool 		GetDoubleBufferBypass() const;

			void 		SetDoubleBufferBypass(bool bypass);
	//@}

	//! @name RAVE Context Hints
	//@{
			uint32 		GetRAVEContextHints() const;

			void 		SetRAVEContextHints(uint32 hints);
	//@}

	//! @name RAVE Texture Filter
	//@{
			uint32 		GetRAVETextureFilter() const;

			void 		SetRAVETextureFilter(uint32 filter);
	//@}

	//! @name RAVE Draw Contexts (QD3D 1.6)
	//@{
			uint32 		CountRAVEDrawContexts() const;
			
			void 		GetRAVEDrawContext(uint32 index, TQADrawContext*& context, TQAEngine*& engine) const;
						/**< Note that the returned context may become invalid if the view is changed. */
	//@}
			
	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DInteractiveRenderer. */
	//@}
};


// ===================================================================================
//	class X3DWireframeRenderer
//!		A wireframe renderer.
// ===================================================================================
class QUESA_EXPORT X3DWireframeRenderer : public X3DRenderer {

	typedef X3DRenderer Inherited;

public:
	virtual				~X3DWireframeRenderer();
						
						X3DWireframeRenderer();

						X3DWireframeRenderer(TQ3RendererObject object);

	virtual X3DWireframeRenderer* Clone() const;
		
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DWireframeRenderer. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
