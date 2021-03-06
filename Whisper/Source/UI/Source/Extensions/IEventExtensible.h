/*
 *  File:       IEventExtensible.h
 *  Summary:   	An interface used to manage IEventExtension objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IEventExtensible.h,v $
 *		Revision 1.4  2001/03/05 05:37:31  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:21:44  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:01:59  jesjones
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


//-----------------------------------
//	Forward References
//
class IEventExtension;
typedef XInterfacePtr<IEventExtension> IEventExtensionPtr;


// ===================================================================================
//	class IEventExtensible
//!		An interface used to manage IEventExtension objects.
// ===================================================================================
class IEventExtensible : public XUnknown {	

//-----------------------------------
//	API
//
public:
	//! @name Adding/Removing
	//@{
	virtual void 		AddExtension(const std::wstring& name, const std::wstring& implementationName) = 0;

	virtual IEventExtensionPtr RemoveExtension(const std::wstring& name) = 0;

	virtual void		RemoveExtensions() = 0;
	//@}

	//! @name Getters
	//@{
	virtual bool		HasExtension(const std::wstring& name) const = 0;

	virtual IEventExtensionPtr GetExtension(const std::wstring& name) const = 0;
	//@}

	//! @name Actions
	//@{
	virtual bool		HandleEvent(EventRecord& event) = 0;
						/**< Iterates over all the event extensions until one handles the event.
						Returns true if a event extension handled the event (and the owner should not). */
	//@}
};

typedef XInterfacePtr<IEventExtensible> IEventExtensiblePtr;
typedef XInterfacePtr<const IEventExtensible> IConstEventExtensiblePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

#endif	// MAC
}		// namespace Whisper
