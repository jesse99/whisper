/*
 *  File:       IClickExtensible.h
 *  Summary:   	An interface used to manage IClickExtension objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IClickExtensible.h,v $
 *		Revision 1.4  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/14 08:20:09  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 1/03/98	JDJ		Created
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

class IClickExtension;
typedef XInterfacePtr<IClickExtension> IClickExtensionPtr;


// ===================================================================================
//	class IClickExtensible
//!		An interface used to manage IClickExtension objects. 
/*! 	Extension objects provide a type-safe way to modify the behavior of objects at 
 *		runtime. This is done with the aid of extension and extensible interfaces. The
 * 		extension interface does the actual work and the extensible interface eases the
 *		task of dealing with multiple extensions on a boss. Note that you can also add 
 *		extensions to a boss in the XML boss declarations.
 *
 *		Extension objects can be very handy, especially if you're doing something out of 
 *		the ordinary.  For example, a view editor could use a draw extension to draw 
 *		resize handles around controls and a mouse extension to intercept mouse events. */
// ===================================================================================
class IClickExtensible : public XUnknown {	

//-----------------------------------
//	API
//
public:
	//! @name Adding/Removing
	//@{
	virtual void 		AddExtension(const std::wstring& name, const std::wstring& implementationName) = 0;

	virtual IClickExtensionPtr RemoveExtension(const std::wstring& name) = 0;

	virtual void		RemoveExtensions() = 0;
	//@}

	//! @name Getters
	//@{
	virtual bool		HasExtension(const std::wstring& name) const = 0;

	virtual IClickExtensionPtr GetExtension(const std::wstring& name) const = 0;
	//@}

	//! @name Action
	//@{
	virtual void		HandleClick(const XMouseDownEvent& event) = 0;
						/**< Passes the event to each extension. */
	//@}
};

typedef XInterfacePtr<IClickExtensible> IClickExtensiblePtr;
typedef XInterfacePtr<const IClickExtensible> IConstClickExtensiblePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
