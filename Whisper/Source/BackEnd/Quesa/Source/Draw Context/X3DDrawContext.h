/*
 *  File:       X3DDrawContext.h
 *  Summary:	Base class for QuickDraw 3D draw contexts.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DDrawContext.h,v $
 *		Revision 1.4  2001/04/14 07:28:11  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:51:16  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:15:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaDrawContext.h>

#include <X2DPrimitives.h>
#include <X3DColors.h>
#include <X3DSharedObject.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DDrawContext
//!		Base class for objects that Quesa can render to.
// ===================================================================================
class QUESA_EXPORT X3DDrawContext : public X3DSharedObject {

	typedef X3DSharedObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DDrawContext();
						
						X3DDrawContext(TQ3DrawContextObject object);
						
	virtual X3DDrawContext* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3DrawContextObject() const				{return mObject;}
	//@}

	//! @name Clear Color
	//@{
			X3DColorARGB GetClearImageColor() const;
						
			void 		SetClearImageColor(const X3DColorARGB& color);
	//@}
			
	//! @name Pane Area
	//@{
			X2DRect 	GetPane() const;
						/**< Returns the portion of the context that QD 3D is drawing into. */
						
			void 		SetPane(const X2DRect& area);
	//@}
			
	//! @name Pane State
	//@{
			bool 		GetPaneState() const;
						/**< Returns true if pane area is being used.
						Returns false if entire context is being used. */
						
			void 		SetPaneState(bool state);
	//@}
			
	//! @name Clear Image Method
	//@{
			TQ3DrawContextClearImageMethod GetClearImageMethod() const;
			
			void 		SetClearImageMethod(TQ3DrawContextClearImageMethod method);
	//@}

	//! @name Mask
	//@{
			TQ3Bitmap 	GetMask() const;
			
			void 		SetMask(const TQ3Bitmap& mask);
	//@}

	//! @name Mask State
	//@{
			bool 		GetMaskState() const;
						/**< Returns true if the mask is in use. */
			
			void 		SetMaskState(bool state);
	//@}

	//! @name Double Buffer State
	//@{
			bool 		GetDoubleBufferState() const;
						/**< Returns true if the context is double buffering. */
			
			void 		SetDoubleBufferState(bool state);
	//@}

	//! @name Data
	//@{
			TQ3DrawContextData GetData() const;

			void 		SetData(const TQ3DrawContextData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DDrawContext. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

