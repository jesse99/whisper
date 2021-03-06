/*
 *  File:       IMenuExtensible.h
 *  Summary:   	An interface used to manage IMenuExtension objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMenuExtensible.h,v $
 *		Revision 1.6  2001/01/30 07:37:28  jesjones
 *		Re-added my changes that were lost when I reverted the delete.
 *		
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
struct SCommandStatus;
class XEvent;

class IMenuExtension;
typedef XInterfacePtr<IMenuExtension> IMenuExtensionPtr;


// ===================================================================================
//	class IMenuExtensible
//!		An interface used to manage IMenuExtension objects.
/*!		Extension objects provide a type-safe way to modify the behavior of objects at 
 *		runtime. This is done with the aid of extension and extensible interfaces. The 
 *		extension interface does the actual work and the extensible interface eases the 
 *		task of dealing with multiple extensions on a boss. Note that you can also add 
 *		extensions to a boss in the XML boss declarations.
 *
 *		Extension objects can be very handy, especially if you're doing something out of 
 *		the ordinary.  For example, a view editor could use a draw extension to draw 
 *		resize handles around controls and a mouse extension to intercept mouse events. */
// ===================================================================================
class IMenuExtensible : public XUnknown {	

//-----------------------------------
//	API
//
public:
	//! @name Adding/Removing
	//@{
	virtual void 		AddExtension(const std::wstring& name, const std::wstring& implementationName) = 0;

	virtual IMenuExtensionPtr RemoveExtension(const std::wstring& name) = 0;

	virtual void		RemoveExtensions() = 0;
	//@}

	//! @name Getters
	//@{
	virtual bool		HasExtension(const std::wstring& name) const = 0;

	virtual IMenuExtensionPtr GetExtension(const std::wstring& name) const = 0;
	//@}

	//! @name Actions
	//@{
	virtual bool		HandleCommand(const XMenuCommand& command, const XEvent& event) = 0;
	virtual bool		HandleEnableCommand(const XMenuCommand& command, SCommandStatus& status, const XEvent& event) = 0;
						/**< Iterates over all the menu extensions until one handles the command.
						Returns true if a menu extension handled the command (and the owner should not). */
	//@}
};

typedef XInterfacePtr<IMenuExtensible> IMenuExtensiblePtr;
typedef XInterfacePtr<const IMenuExtensible> IConstMenuExtensiblePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
