/*
 *  File:       XPlugin.coo
 *  Summary:   	The class we use to load and unload plugins.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPlugin.cpp,v $
 *		Revision 1.2  2000/11/09 12:46:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	12/02/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XPlugin.h>

#include <XExceptions.h>
#include <XFragment.h>
#include <XObjectModel.h>
#include <XStringUtils.h>
#include <XURI.h>

namespace Whisper {


// ===================================================================================
//	class XPlugin
// ===================================================================================

//---------------------------------------------------------------
//
// XPlugin::~XPlugin
//
//---------------------------------------------------------------
XPlugin::~XPlugin()
{
	// Remove the plugin's bosses
	XObjectModel::Instance()->UnloadBosses(this);

	// Call the plugin's shutdown routine
	void* adr = mFragment->GetFunction(L"Shutdown");
	if (adr != nil) {
		ShutdownProc shutdown = static_cast<ShutdownProc>(adr);
		shutdown();
	}
		
	// Unload the fragment (this is where static objects in the
	// plugin are destructed and therefore where interfaces are 
	// typically removed).
	delete mFragment;
}


//---------------------------------------------------------------
//
// XPlugin::XPlugin (XFileSpec)
//
//---------------------------------------------------------------
XPlugin::XPlugin(const XFileSpec& spec) : mSpec(spec)
{
	// Load the fragment
	mFragment = new XFragment(mSpec);
		
	// Call the plugin's startup routine (this should register the 
	// plugin's interfaces).
	void* adr = mFragment->GetFunction(L"Startup");
	if (adr == nil)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the Startup function for the #1 plugin.", mSpec.GetName())));
		
	StartupProc startup = static_cast<StartupProc>(adr);
	startup();
	
	// Register the plugin's bosses
	XURI uri(L"resource://Bosses.bXML");
	XObjectModel::Instance()->LoadBosses(this, uri);

	// Call the plugin's main entry point
	adr = mFragment->GetFunction(L"Main");
	if (adr != nil) {
		MainProc entryPoint = static_cast<MainProc>(adr);
		entryPoint();
	}
}


//---------------------------------------------------------------
//
// XPlugin::XPlugin (XFileSpec, XURI)
//
//---------------------------------------------------------------
XPlugin::XPlugin(const XFileSpec& spec, const XURI& bosses) : mSpec(spec)
{
	// Load the fragment
	mFragment = new XFragment(mSpec);
	
	// Call the plugin's startup routine (this should register the 
	// plugin's interfaces).
	void* adr = mFragment->GetFunction(L"Startup");
	if (adr == nil)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the Startup function for the #1 plugin.", mSpec.GetName())));
		
	StartupProc proc = static_cast<StartupProc>(adr);
	proc();
	
	// Register the plugin's bosses
	XObjectModel::Instance()->LoadBosses(this, bosses);

	// Call the plugin's main entry point
	adr = mFragment->GetFunction(L"Main");
	if (adr != nil) {
		MainProc entryPoint = static_cast<MainProc>(adr);
		entryPoint();
	}
}


}	// namespace Whisper
