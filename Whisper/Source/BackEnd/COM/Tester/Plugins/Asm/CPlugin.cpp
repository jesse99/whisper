/*
 *  File:       CPlugin.cpp
 *  Summary:   	The plugin's startup code.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPlugin.cpp,v $
 *		Revision 1.3  2001/03/13 11:02:43  jesjones
 *		Changed a comment.
 *		
 *		Revision 1.2  2000/11/09 08:56:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
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
	REGISTER_INTERFACE_FACTORY(CCountLines, L"Asm Count Lines");
	
	IPluginsPtr plugins(L"Plugins Boss");
	plugins->Register(L"Asm Plugin");
}


//---------------------------------------------------------------
//
// Shutdown
//
//---------------------------------------------------------------
void Shutdown()
{
	IPluginsPtr plugins(L"Plugins Boss");
	plugins->UnRegister(L"Asm Plugin");
}

