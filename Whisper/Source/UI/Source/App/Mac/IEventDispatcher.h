/*
 *  File:       IEventDispatcher.h
 *  Summary:   	Mac class that dispatches toolbox events to the proper objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: IEventDispatcher.h,v $
 *		Revision 1.6  2001/04/27 09:09:22  jesjones
 *		Nuked HandleIdle.
 *		
 *		Revision 1.5  2001/03/28 09:40:45  jesjones
 *		Exposed UpdateFocus.
 *		
 *		Revision 1.4  2001/03/05 05:37:28  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:21:40  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:01:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IEventDispatcher
//!		Mac class that dispatches toolbox events to the proper objects.
// ===================================================================================
class IEventDispatcher : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		DispatchEvent(const EventRecord& event) = 0;
						/**< Executes the event extensions and, if none of them return true,
						routes the event to the appropiate object. Note that you can
						install a extension to change the way events are processed. */
							
	virtual void 		HandleTime() = 0;
						/**< Called (at a minimum) after every event. */

#if MAC					
	virtual void 		HandleAdjustCursor(const EventRecord& event) = 0;
#endif

	virtual void 		UpdateFocus() = 0;
						/**< Hack to to notify controls when they get get and lose the focus. */
	
//-----------------------------------
//	Internal API
//
public:
#if DEBUG
	virtual bool 		GetZapPort() const = 0;
						// If this is true the current GrafPort will be set to nil after
						// each event is processed. Defaults to false.
			
	virtual void 		ToggleZapPort() = 0;
#endif
};

typedef XInterfacePtr<IEventDispatcher> IEventDispatcherPtr;
typedef XInterfacePtr<const IEventDispatcher> IConstEventDispatcherPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
