/*
 *  File:       ICommandQueue.h
 *  Summary:   	A list of ICommand objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICommandQueue.h,v $
 *		Revision 1.4  2001/03/05 05:37:05  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:20:27  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:35  jesjones
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
class ICommand;
typedef XInterfacePtr<ICommand> ICommandPtr;


// ===================================================================================
//	class ICommandQueue
//!		A list of ICommand objects.
// ===================================================================================
class ICommandQueue : public XUnknown {

	typedef XUnknown Inherited;

//-----------------------------------
//	API
//
public:
	virtual void 		Post(const ICommandPtr& command) = 0;
						/**< Call this to place a command in the queue. */
							
	virtual bool		ExecuteCommands() = 0;
						/**< Iterates through all the commands and executes all the ones
						that are ready to be executed. Returns true if a command was
						executed. */
						
	virtual bool 		HasCommands() const = 0;
						/**< Returns true if there's a command that's ready to execute. */
						
	virtual void 		Clear() = 0;
						/**< Release all the commands in the queue. */
};

typedef XInterfacePtr<ICommandQueue> ICommandQueuePtr;
typedef XInterfacePtr<const ICommandQueue> IConstCommandQueuePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

