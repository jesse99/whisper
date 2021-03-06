/*
 *  File:       CPlugin.cpp
 *  Summary:   	The plugin's startup code.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPlugin.cpp,v $
 *		Revision 1.2  2000/11/09 08:56:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 3/30/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include "CPlugin.h"

#include <XObjectModel.h>
#include <XUnknown.h>

#include "IPlugins.h"


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// Startup
//
//---------------------------------------------------------------
void Startup()
{
	REGISTER_INTERFACE_FACTORY(CCountLines, L"C++ Count Lines");

	IPluginsPtr plugins(L"Plugins Boss");
	plugins->Register(L"C++ Plugin");
}


//---------------------------------------------------------------
//
// Shutdown
//
//---------------------------------------------------------------
void Shutdown()
{
	IPluginsPtr plugins(L"Plugins Boss");
	plugins->UnRegister(L"C++ Plugin");
}


