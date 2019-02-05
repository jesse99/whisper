/*
 *  File:       ICommand.h
 *  Summary:   	A class that can be placed in a queue and executed the next time the
 *				main event loop executes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICommand.h,v $
 *		Revision 1.4  2001/03/05 05:36:59  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:20:21  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:28  jesjones
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
//	class ICommand
//!		A class that can be placed in a queue and executed the next time the main event loop executes.
// ===================================================================================
class ICommand : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		HandleExecute() = 0;
						/**< Normally you'll call ICommandQueue:Post which will execute the
						command the next time through the main event loop. However you
						can call HandleExecute if you want to immediately execute the
						command. Returns true if there were no exceptions. */

	virtual bool		IsReadyToExecute() const = 0;

	virtual bool		KeepInQueue() const = 0;
						/**< Return true if you want the command to be kept in the queue (eg
						for periodic commands). */
};

typedef XInterfacePtr<ICommand> ICommandPtr;
typedef XInterfacePtr<const ICommand> IConstCommandPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

