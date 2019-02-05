/*
 *  File:       XCommandQueue.cpp
 *  Summary:   	A list of ICommand objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCommandQueue.cpp,v $
 *		Revision 1.5  2001/04/27 04:31:10  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/03/05 05:40:51  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:33:20  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:28:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <ICommandQueue.h>

#include <list>

#include <ICommand.h>
#include <XInterfaceMacros.h>
#include <XMiscUtils.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XCommandQueue
//!		A list of ICommand objects.
// ===================================================================================
class XCommandQueue : public ICommandQueue {

	typedef ICommandQueue Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XCommandQueue();
	
						XCommandQueue(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Post(const ICommandPtr& command);
							
	virtual bool		ExecuteCommands();
						
	virtual bool 		HasCommands() const;
		
	virtual void 		Clear();

//-----------------------------------
//	Internal Types
//
protected:
	typedef std::list<ICommandPtr> CommandQueue;

//-----------------------------------
//	Member Data
//
private:
	bool						mExecutingCommands;
	
	CommandQueue				mCommands;
	CommandQueue				mTempQueue;
	mutable XCriticalSection	mMutex;
};

DEFINE_INTERFACE_FACTORY(XCommandQueue)

//---------------------------------------------------------------
//
// XCommandQueue::~XCommandQueue
//
//---------------------------------------------------------------
XCommandQueue::~XCommandQueue()
{
}


//---------------------------------------------------------------
//
// XCommandQueue::XCommandQueue
//
//---------------------------------------------------------------
XCommandQueue::XCommandQueue(XBoss* boss)					
{
	this->DoSetBoss(boss);

	mExecutingCommands = false;
}


//---------------------------------------------------------------
//
// XCommandQueue::Post
//
//---------------------------------------------------------------
void XCommandQueue::Post(const ICommandPtr& command)				
{
	XEnterCriticalSection enter(mMutex);

	if (mExecutingCommands)
		mTempQueue.push_back(command);
	else 
		mCommands.push_back(command);
}


//---------------------------------------------------------------
//
// XCommandQueue::HasCommands						
//
//---------------------------------------------------------------
bool XCommandQueue::HasCommands() const
{
	bool has = false;
	
	XEnterCriticalSection enter(mMutex);

	CommandQueue::const_iterator iter = mCommands.begin();
	while (iter != mCommands.end() && !has) {
		const ICommandPtr& command = *iter;
		++iter;
		
		has = command->IsReadyToExecute();
	}	
	
	return has;
}


//---------------------------------------------------------------
//
// XCommandQueue::Clear						
//
//---------------------------------------------------------------
void XCommandQueue::Clear()
{
	mCommands.clear();
}


//---------------------------------------------------------------
//
// XCommandQueue::ExecuteCommands					
//
//---------------------------------------------------------------
bool XCommandQueue::ExecuteCommands()
{
	bool didOne = false;
	
	PRECONDITION(!mExecutingCommands);
	mExecutingCommands = true;
	
	// Execute all the available commands.
	XEnterCriticalSection enter(mMutex);

	CommandQueue::iterator iter = mCommands.begin();
	while (!mCommands.empty() && iter != mCommands.end()) {
		CommandQueue::iterator temp = iter;

		ICommandPtr command = *iter;
		++iter;
		
		if (command->IsReadyToExecute()) {
			bool completed = command->HandleExecute();	// returns false if an exception is thrown
			
			if (!completed || !command->KeepInQueue())
				mCommands.erase(temp);
				
			didOne = true;
		}
	}	
	
	// Move commands from mTempQueue to mCommands. (New commands 
	// are placed in mTempQueue to prevent them from executing while
	// we're processing the original command queue).
	mCommands.splice(mCommands.end(), mTempQueue);
	
	mExecutingCommands = false;
		
	return didOne;
}


}	// namespace Whisper
