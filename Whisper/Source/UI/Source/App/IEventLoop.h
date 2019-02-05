/*
 *  File:       IEventLoop.h
 *  Summary:   	Interface that encapsulates an event loop.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IEventLoop.h,v $
 *		Revision 1.4  2001/01/05 06:16:58  jesjones
 *		Added SetErrHandler.
 *		
 *		Revision 1.3  2000/12/14 08:21:53  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:02:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 11/05/00	JDJ		Created
 */

#pragma once

#include <XCallbacks.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IEventLoop
//!		Interface that encapsulates an event loop.
// ===================================================================================
class IEventLoop : public XUnknown {

//-----------------------------------
//	Types
//
public:
	typedef XCallback1<void, std::exception> ErrHandler;

//-----------------------------------
//	API
//
public:
	virtual void 		Run() = 0;
						/**< For application event loops this won't return until the
						app quits, for dialogs this won't return until the dialog
						is dismissed. */
						
	virtual void 		SetErrHandler(ErrHandler handler) = 0;
						/**< Use this to change what happens when an exception is thrown
						from within Run. The default pops up an alert and continues
						processing events. */
};

typedef XInterfacePtr<IEventLoop> IEventLoopPtr;
typedef XInterfacePtr<const IEventLoop> IConstEventLoopPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

