/*
 *  File:       IKeyExtensible.h
 *  Summary:   	An interface used to manage IKeyExtension objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IKeyExtensible.h,v $
 *		Revision 1.4  2001/03/05 05:37:47  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:23:06  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:03:04  jesjones
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

class IKeyExtension;
typedef XInterfacePtr<IKeyExtension> IKeyExtensionPtr;


// ===================================================================================
//	class IKeyExtensible
//!		An interface used to manage IKeyExtension objects.
// ===================================================================================
class IKeyExtensible : public XUnknown {	

//-----------------------------------
//	API
//
public:
	//! @name Adding/Removing
	//@{
	virtual void 		AddExtension(const std::wstring& name, const std::wstring& implementationName) = 0;

	virtual IKeyExtensionPtr RemoveExtension(const std::wstring& name) = 0;

	virtual void		RemoveExtensions() = 0;
	//@}

	//! @name Getters
	//@{
	virtual bool		HasExtension(const std::wstring& name) const = 0;

	virtual IKeyExtensionPtr GetExtension(const std::wstring& name) const = 0;
	//@}

	//! @name Actions
	//@{
	virtual bool		HandleKeyDown(XKeyDownEvent& event) = 0;
	virtual bool		HandleChar(XCharEvent& event) = 0;
	virtual bool		HandleKeyUp(XKeyUpEvent& event) = 0;
						/**< Iterates over all the key extensions until one handles the event.
						Returns true if a key extension handled the event (and the owner should not). */
	//@}
};

typedef XInterfacePtr<IKeyExtensible> IKeyExtensiblePtr;
typedef XInterfacePtr<const IKeyExtensible> IConstKeyExtensiblePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
