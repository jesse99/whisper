/*
 *  File:       IText.h
 *  Summary:   	Used to get text into and out of an object.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IText.h,v $
 *		Revision 1.5  2001/03/26 04:17:02  jesjones
 *		Added IsEmpty.
 *		
 *		Revision 1.4  2001/03/05 05:38:45  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:25:25  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IText
//!		Used to get text into and out of an object.
// ===================================================================================
class IText : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		IsEmpty() const = 0;
	
	virtual std::wstring GetText() const = 0;
	virtual void 		SetText(const std::wstring& text, bool redraw = kRedraw) = 0;
};


typedef XInterfacePtr<IText> ITextPtr;
typedef XInterfacePtr<const IText> IConstTextPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
