/*
 *  File:       XCommand.cpp
 *  Summary:   	Default ICommand implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCommand.cpp,v $
 *		Revision 1.5  2001/04/27 04:31:07  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/03/05 05:40:48  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2001/01/30 07:24:09  jesjones
 *		Added ICommandHelper and IUndoActionHelper.
 *		
 *		Revision 1.2  2000/11/09 12:28:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <ICommand.h>

#include <ICommandHelper.h>
#include <XDebug.h>
#include <XError.h>
#include <XMiscUtils.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XCommand
//!		Default ICommand implementation.
// ===================================================================================
class XCommand : public ICommand {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XCommand();
	
						XCommand(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		HandleExecute();
						
	virtual bool		IsReadyToExecute() const;
		
	virtual bool		KeepInQueue() const;
};

DEFINE_INTERFACE_FACTORY(XCommand)

//---------------------------------------------------------------
//
// XCommand::~XCommand
//
//---------------------------------------------------------------
XCommand::~XCommand()
{
}


//---------------------------------------------------------------
//
// XCommand::XCommand
//
//---------------------------------------------------------------
XCommand::XCommand(XBoss* boss)
{
	ICommand::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XCommand::HandleExecute
//
//---------------------------------------------------------------
bool XCommand::HandleExecute()
{
	PRECONDITION(this->IsReadyToExecute());
	
	bool finished = false;
	
	try {
		ICommandHelperPtr helper(this);
		helper->OnExecute();
		
		finished = true;
		
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't complete the operation."), e);
	
	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't complete the operation."));
	}
			
	return finished;
}


//---------------------------------------------------------------
//
// XCommand::IsReadyToExecute
//
//---------------------------------------------------------------
bool XCommand::IsReadyToExecute() const		
{
	return true;
}


//---------------------------------------------------------------
//
// XCommand::KeepInQueue
//
//---------------------------------------------------------------
bool XCommand::KeepInQueue() const		
{
	return false;
}


}	// namespace Whisper
