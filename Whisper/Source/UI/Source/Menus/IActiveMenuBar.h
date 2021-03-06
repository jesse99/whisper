/*
 *  File:       IActiveMenuBar.h
 *  Summary:   	Provides access to the current menubar.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IActiveMenuBar.h,v $
 *		Revision 1.4  2001/03/05 05:36:45  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:19:36  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 08:59:36  jesjones
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
class IMenuBar;
typedef XInterfacePtr<IMenuBar> IMenuBarPtr;


// ===================================================================================
//	class IActiveMenuBar
//!		Provides access to the current menubar.
// ===================================================================================
class IActiveMenuBar : public XUnknown {

	typedef XUnknown Inherited;

//-----------------------------------
//	API
//
public:					
	virtual IMenuBarPtr Get() const = 0;
	
	virtual void 		Set(const IMenuBarPtr& menubar) = 0;
};

typedef XInterfacePtr<IActiveMenuBar> IActiveMenuBarPtr;
typedef XInterfacePtr<const IActiveMenuBar> IConstActiveMenuBarPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
