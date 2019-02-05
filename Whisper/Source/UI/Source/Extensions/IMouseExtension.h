/*
 *  File:       IMouseExtension.h
 *	Function:	Interface for objects that modify the way an extensible object 
 *				responds to mouse events.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMouseExtension.h,v $
 *		Revision 1.4  2001/03/05 05:38:02  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:23:50  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:04:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XMouseDownEvent;
class XMouseMoveEvent;
class XMouseUpEvent;


// ===================================================================================
//	class IMouseExtension
//!		Interface for objects that modify the way an extensible object responds to mouse events.
// ===================================================================================
class IMouseExtension : public XUnknown {
	
//-----------------------------------
//	API 
//
public:
	// ----- Name -----
	virtual std::wstring GetName() const = 0;
	
	virtual void 		SetName(const std::wstring& name) = 0;
									
	// ----- Action -----
	virtual bool 		HandleMouseDown(XMouseDownEvent& event) = 0;
	virtual bool 		HandleMouseMove(XMouseMoveEvent& event) = 0;
	virtual bool 		HandleMouseUp(XMouseUpEvent& event) = 0;
						/**< Return true if you handled the event (and don't want the 
						owner to also handle the event). */
};

typedef XInterfacePtr<IMouseExtension> IMouseExtensionPtr;
typedef XInterfacePtr<const IMouseExtension> IConstMouseExtensionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
