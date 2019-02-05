/*
 *  File:       WDebugExtensions.cpp
 *	Function:	Objects that when attached to an XApplication add debug features to the Debug menu.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WDebugExtensions.cpp,v $
 *		Revision 1.5  2001/03/02 11:23:09  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.4  2001/02/21 08:16:08  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.3  2001/01/22 02:01:19  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.2  2000/11/09 12:06:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IInstallMenuHandler.h>

#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <WSystemInfo.h>
#include <XStringUtils.h>
#include <XSystemInfo.h>

#if DEBUG
namespace Whisper {


//-----------------------------------
//	Constants
//
const XMenuCommand kSystemInfoCmd	= L"dump system info";


// ===================================================================================
//	class WSystemInfoExtension
// ===================================================================================
class WSystemInfoExtension : public IInstallMenuHandler {
	
//-----------------------------------
//	Initialization/Destruction
//
public:	
						WSystemInfoExtension(XBoss* boss);

//-----------------------------------
//	Inherited API 
//
public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

//-----------------------------------
//	Internal API 
//
private:
			void 		DoDumpInfo();
};

DEFINE_INTERFACE_FACTORY(WSystemInfoExtension)

//---------------------------------------------------------------
//
// WSystemInfoExtension::WSystemInfoExtension
//
//---------------------------------------------------------------
WSystemInfoExtension::WSystemInfoExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// Dump System Info
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"TRACEs out information about the machine the app is running on.");
	menu->AppendItem(LoadWhisperString(L"Dump System Info"), kSystemInfoCmd, help);
}


//---------------------------------------------------------------
//
// WSystemInfoExtension::InstallCommands
//
//---------------------------------------------------------------
void WSystemInfoExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	XCallback0<void> action(this, &WSystemInfoExtension::DoDumpInfo);
	handler->RegisterCommand(kSystemInfoCmd, action, kAlwaysEnabled, this);				
}


//---------------------------------------------------------------
//
// WSystemInfoExtension::RemoveCommands
//
//---------------------------------------------------------------
void WSystemInfoExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	XCallback0<void> action(this, &WSystemInfoExtension::DoDumpInfo);
	handler->UnRegisterCommand(action);			
}


//---------------------------------------------------------------
//
// WSystemInfoExtension::DoDumpInfo
//
//---------------------------------------------------------------
void WSystemInfoExtension::DoDumpInfo()
{
	TRACE("System Info:\n");
	
	TRACE("   Number of Processors: ", XSystemInfo::GetProcessorCount(), "\n");
	TRACE("   Has FPU: ", XSystemInfo::HasFPU(), "\n");
	TRACE("   Physical Memory: ", XSystemInfo::GetPhysicalRAM()/(1024L*1024L), "M\n");
	TRACE("   Virtual Memory: ", XSystemInfo::GetLogicalRAM()/(1024L*1024L), "M\n");
	TRACE("   Virtual Memory On: ", XSystemInfo::IsVirtualMemOn(), "\n");
	TRACE("\n");
	
	TRACE("   Computer Name: ", XSystemInfo::GetComputerName(), "\n");
	TRACE("   User Name: ", XSystemInfo::GetUserName(), "\n");
	TRACE("   App Path: ", WSystemInfo::GetAppPath(), "\n");
	TRACE("\n");

	TRACE("   Is Win32s: ", WSystemInfo::IsWin32s(), "\n");
	TRACE("   Is Win32: ", WSystemInfo::IsWin32(), "\n");
	TRACE("   Is Win95 OSR2: ", WSystemInfo::IsWin95OSR2(), "\n");
	TRACE("   Is Win98: ", WSystemInfo::IsWin98(), "\n");
	TRACE("   Is NT: ", WSystemInfo::IsNT(), "\n");
	TRACE("   Major OS Version: ", (uint32) WSystemInfo::GetMajorOSVersion(), "\n");
	TRACE("   Minor OS Version: ", (uint32) WSystemInfo::GetMinorOSVersion(), "\n");
	TRACE("\n");

	TRACE("   Page Size: ", WSystemInfo::GetPageSize(), "\n");
	TRACE("\n");

	TRACE("\n");
}


}	// namespace Whisper
#endif	// DEBUG