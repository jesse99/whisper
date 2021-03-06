/*
 *  File:       IShowHelp.h
 *  Summary:   	The interface used to display help messages (eg using balloon help or
 *				the Windows status bar).
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IShowHelp.h,v $
 *		Revision 1.5  2001/03/09 10:18:02  jesjones
 *		Where is in global instead of local coordinates.
 *		
 *		Revision 1.4  2001/03/05 05:38:34  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:25:11  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:12  jesjones
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
//	class IShowHelp
//!		The interface used to display help messages (eg using balloon help or the Windows status bar).
// ===================================================================================
class IShowHelp : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		Show(const std::wstring& message, const XPoint& where) const = 0;
						/**< Where is in global coordinates. */
};


typedef XInterfacePtr<IShowHelp> IShowHelpPtr;
typedef XInterfacePtr<const IShowHelp> IConstShowHelpPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
