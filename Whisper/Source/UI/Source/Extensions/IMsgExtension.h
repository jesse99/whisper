/*
 *  File:       IMsgExtension.h
 *	Function:	Interface for objects that modify the way an extensible object 
 *				responds to Windows messages.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMsgExtension.h,v $
 *		Revision 1.4  2000/12/14 08:24:01  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/17 08:56:12  jesjones
 *		Corrected the header comment.
 *		
 *		Revision 1.2  2000/11/09 09:04:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 5/27/00	JDJ		Created
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {
#if WIN

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IMsgExtension
//!		Interface for objects that modify the way an extensible object responds to Windows messages.
// ===================================================================================
class IMsgExtension : public XUnknown {
	
//-----------------------------------
//	API 
//
public:
	// ----- Name -----
	virtual std::wstring GetName() const = 0;
	
	virtual void 		SetName(const std::wstring& name) = 0;
									
	// ----- Action -----
	virtual bool 		HandleMessage(MSG& message) = 0;
						/**< Return true if you handled the message (and don't want the 
						owner to also handle the message). */
};

typedef XInterfacePtr<IMsgExtension> IMsgExtensionPtr;
typedef XInterfacePtr<const IMsgExtension> IConstMsgExtensionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

#endif	// WIN
}		// namespace Whisper
