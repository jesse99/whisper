/*
 *  File:       IKeyExtension.h
 *	Function:	Interface for objects that modify the way their owner responds to key events.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IKeyExtension.h,v $
 *		Revision 1.4  2001/03/05 05:37:50  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:23:10  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:03:07  jesjones
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
class XCharEvent;
class XKeyDownEvent;
class XKeyUpEvent;


// ===================================================================================
//	class IKeyExtension
//!		Interface for objects that modify the way their owner responds to key events.
// ===================================================================================
class IKeyExtension : public XUnknown {
	
//-----------------------------------
//	API 
//
public:
	// ----- Name -----
	virtual std::wstring GetName() const = 0;
	
	virtual void 		SetName(const std::wstring& name) = 0;
										
	// ----- Action -----
	virtual bool 		HandleKeyDown(XKeyDownEvent& event) = 0;
	virtual bool 		HandleChar(XCharEvent& event) = 0;
	virtual bool 		HandleKeyUp(XKeyUpEvent& event) = 0;
						/**< Return true if you handled the event (and don't want the owner
						to also handle the event). */
};

typedef XInterfacePtr<IKeyExtension> IKeyExtensionPtr;
typedef XInterfacePtr<const IKeyExtension> IConstKeyExtensionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
