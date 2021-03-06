/*
 *  File:       IEventExtension.h
 *	Function:	Interface for objects that modify the way an extensible object 
 *				responds to Mac events.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IEventExtension.h,v $
 *		Revision 1.4  2001/03/05 05:37:35  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:21:50  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:02:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {
#if MAC

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IEventExtension
//!		Interface for objects that modify the way an extensible object responds to Mac events.
// ===================================================================================
class IEventExtension : public XUnknown {
	
//-----------------------------------
//	API 
//
public:
	// ----- Name -----
	virtual std::wstring GetName() const = 0;
	
	virtual void 		SetName(const std::wstring& name) = 0;
									
	// ----- Action -----
	virtual bool 		HandleEvent(EventRecord& event) = 0;
						/**< Return true if you handled the event (and don't want the 
						owner to also handle the event). */
};

typedef XInterfacePtr<IEventExtension> IEventExtensionPtr;
typedef XInterfacePtr<const IEventExtension> IConstEventExtensionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

#endif	// MAC
}		// namespace Whisper
