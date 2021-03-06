/*
 *  File:       X3DRenderer.h
 *  Summary:	QuickDraw 3D renderer object.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DRenderer.h,v $
 *		Revision 1.4  2001/04/14 07:31:12  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:53:35  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:19:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaRenderer.h>

#include <X3DSharedObject.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DRenderer
//!		A 3D renderer.
// ===================================================================================
class QUESA_EXPORT X3DRenderer : public X3DSharedObject {

	typedef X3DSharedObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DRenderer();
						
						X3DRenderer(TQ3ObjectType type);
						/**< Type can be kQ3RendererTypeInteractive, kQ3RendererTypeWireFrame,
						or kQ3RendererTypeGeneric. */

						X3DRenderer(TQ3RendererObject object);
						
	virtual X3DRenderer* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3RendererObject() const				{return mObject;}
	//@}

	//! @name Settings Dialog
	//@{
			bool 		HasModalConfigure() const;
						/**< Returns true if the renderer has a modal settings dialog. */

			bool 		ModalConfigure(TQ3DialogAnchor anchor);
						/**< Pops up a dialog allowing the user to configure the renderers
						settings. Returns false if the user canceled out of the dialog. */
	//@}

	//! @name Settings Data
	//@{
			uint32 		GetConfigurationSize() const;
						/**< Returns the size of the renderer's settings data. */

			void 		GetConfigurationData(void* buffer, uint32 bufferSize) const;
						/**< Copies the renderer's private settings data to buffer.
						This can be used for user preferences or in a style template.
						The data should be tagged with the renderer's object name. */
						
			void 		SetConfigurationData(const void* buffer, uint32 bufferSize);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DRenderer. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
