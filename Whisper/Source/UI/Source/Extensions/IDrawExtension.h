/*
 *	File:		IDrawExtension.h
 *	Function:	An extension object that changes the way it's owner is drawn.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: IDrawExtension.h,v $
 *		Revision 1.4  2001/02/07 04:55:54  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:21:31  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:01:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XRect.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References 
//
class XDrawContext;


// ===================================================================================
//	class IDrawExtension
//!		An extension object that changes the way it's owner is drawn.
// ===================================================================================
class IDrawExtension : public XUnknown {
		
//-----------------------------------
//	API 
//
public:
	//! @name Name
	//@{
	virtual std::wstring GetName() const = 0;
	
	virtual void 		SetName(const std::wstring& name) = 0;
	//@}
									
	//! @name Extent
	//@{
	virtual	XRect 		GetIndent() const = 0;

	virtual	void 		SetIndent(const XRect& indent) = 0;
						/**< Extension's extent is owner's extent plus mIndent.
						Currently mIndent members must be non-negative. */
	//@}

	//! @name Draw Order
	//@{
	virtual bool 		DrawsBeforeOwner() const = 0;
						/**< Returns true if the extension should draw before the owner
						(false should be the default). */

	virtual void 		SetDrawBeforeOwner(bool drawBefore = true) = 0;
	//@}

	//! @name Action
	//@{
	virtual	bool		Draw(XDrawContext& canvas, const XRect& ownerExtent, bool enabled) = 0;
						/**< Return false if the owner shouldn't draw. */
	//@}
};


typedef XInterfacePtr<IDrawExtension> IDrawExtensionPtr;
typedef XInterfacePtr<const IDrawExtension> IConstDrawExtensionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
