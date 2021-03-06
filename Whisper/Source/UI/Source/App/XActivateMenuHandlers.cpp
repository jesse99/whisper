/*
 *  File:       XActivateMenuHandlers.cpp
 *  Summary:   	IActivated implementation that installs and removes all the menu handlers on the boss.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XActivateMenuHandlers.cpp,v $
 *		Revision 1.4  2001/04/21 03:39:15  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/02/23 10:08:25  jesjones
 *		Dtor asserts that it's not active.
 *		
 *		Revision 1.2  2001/01/30 07:07:43  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.1  2001/01/21 00:50:53  jesjones
 *		Checked in	
 */

#include <XWhisperHeader.h>
#include <IActivated.h>

#include <IHierarchy.h>
#include <IInstallMenuHandler.h>

namespace Whisper {


// ===================================================================================
//	class XActivateMenuHandlers
//!		IActivated implementation that installs and removes all the menu handlers on the boss.
// ===================================================================================
class XActivateMenuHandlers : public IActivated {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XActivateMenuHandlers();
	
						XActivateMenuHandlers(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		OnActivated();
	virtual void 		OnDeactivated();
						
//-----------------------------------
//	Member Data
//
private:
	bool	mActive;
};

DEFINE_INTERFACE_FACTORY(XActivateMenuHandlers)

//---------------------------------------------------------------
//
// XActivateMenuHandlers::~XActivateMenuHandlers
//
//---------------------------------------------------------------
XActivateMenuHandlers::~XActivateMenuHandlers()
{
	PRECONDITION(!mActive);
}


//---------------------------------------------------------------
//
// XActivateMenuHandlers::XActivateMenuHandlers
//
//---------------------------------------------------------------
XActivateMenuHandlers::XActivateMenuHandlers(XBoss* boss)
{	
	IActivated::DoSetBoss(boss);
	mActive = false;
}


//---------------------------------------------------------------
//
// XActivateMenuHandlers::OnActivated
//
//---------------------------------------------------------------
void XActivateMenuHandlers::OnActivated() 
{
	PRECONDITION(!mActive);
	
	CallRepeatedMethod(this, &IInstallMenuHandler::InstallCommands);

	mActive = true;
}


//---------------------------------------------------------------
//
// XActivateMenuHandlers::OnDeactivated
//
//---------------------------------------------------------------
void XActivateMenuHandlers::OnDeactivated()
{
	PRECONDITION(mActive);

	CallRepeatedMethod(this, &IInstallMenuHandler::RemoveCommands);

	mActive = false;
}


}	// namespace Whisper
