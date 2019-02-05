/*
 *  File:       IHelpText.h
 *  Summary:   	The interface used to acquire help messages for things like balloon help.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IHelpText.h,v $
 *		Revision 1.5  2001/03/09 10:17:37  jesjones
 *		Takes an XPoint instead of an XMouseMovedEvent.
 *		
 *		Revision 1.4  2001/03/05 05:37:39  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:22:19  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:02:30  jesjones
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
class XPoint;


// ===================================================================================
//	class IHelpText
//!		The interface used to acquire help messages for things like balloon help.
// ===================================================================================
class IHelpText : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual std::wstring GetMessage(const XPoint& localPt) const = 0;
};


typedef XInterfacePtr<IHelpText> IHelpTextPtr;
typedef XInterfacePtr<const IHelpText> IConstHelpTextPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
