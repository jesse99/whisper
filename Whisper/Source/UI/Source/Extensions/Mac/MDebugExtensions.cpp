/*
 *  File:       MDebugExtensions.cpp
 *	Function:	Objects that when attached to an XApplication add debug features to the Debug menu.
 *  Written by: Jesse Jones
 *
 *	Classes:	MMacsbugMenuHandler	 - Adds a command that causes the app to drop into the low level debugger.
 *				MZoneRangerExtension - Adds a command that launches Zone Ranger.
 *				MSIOUXExtension		 - Adds some commands relating to the SIOUX debug window.
 *				MSystemInfoExtension - Dumps system info.
 *				MSystemHeapExtension - Adds commands relating to the OS heaps.
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDebugExtensions.cpp,v $
 *		Revision 1.12  2001/04/19 04:06:59  jesjones
 *		Added MSystemHeapExtension (checks the OS heaps).
 *		
 *		Revision 1.11  2001/04/13 07:49:26  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.10  2001/03/02 11:22:43  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.9  2001/02/12 09:40:28  jesjones
 *		Disabled MSIOUXExtension::DoEnableCopy for DLL targets.
 *		
 *		Revision 1.8  2001/01/30 07:18:39  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.7  2001/01/22 02:01:00  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.6  2001/01/21 00:26:55  jesjones
 *		Started rewriting to match the new menu code.
 *		
 *		Revision 1.5  2001/01/01 03:48:40  jesjones
 *		Stopped using those crazy iostream manipulators.
 *		
 *		Revision 1.4  2000/12/14 08:27:20  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/27 07:15:37  jesjones
 *		Tweaked for CW 6's SIOUX.
 *		
 *		Revision 1.2  2000/11/09 09:11:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	 9/09/99	JDJ		Nuked MZapPortExtension (XControlExtension handles this).
 *		 <6>	 7/21/99	JDJ		Tweaked MSIOUXExtension::OnStatus.
 *		 <5>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <4>	 3/30/99	JDJ		Extensions include help messages.
 *		 <3>	 1/25/99	JDJ		MSystemInfoExtension dumps TEC version.
 *		 <2>	 1/06/99	JDJ		Added MSystemInfoExtension.
 *		 <1>	 1/17/98	JDJ		Created
 */
 
#include <XWhisperHeader.h>
#include <IInstallMenuHandler.h>

#include <iomanip>

#include <SIOUX.h>
#include <SIOUXGlobals.h>

#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <MDesktopMgr.h>
#include <MSystemInfo.h>
#include <XBind.h>
#include <XDialogUtils.h>
#include <XIntConversions.h>
#include <XSystemInfo.h>

#if DEBUG
namespace Whisper {


//-----------------------------------
//	Constants
//
const XMenuCommand kMacsbugCmd		= L"macsbug";
const XMenuCommand kZoneRangerCmd	= L"launch zoneranger";
const XMenuCommand kShowSIOUXCmd	= L"show/hide sioux";
const XMenuCommand kSystemInfoCmd	= L"dump system info";
const XMenuCommand kCheckOSHeapsCmd	= L"check os heaps";


// ===================================================================================
//	class MMacsbugMenuHandler
//!		Adds a command that causes the app to drop into the low level debugger.
// ===================================================================================
class MMacsbugMenuHandler : public IInstallMenuHandler {
	
//-----------------------------------
//	Initialization/Destruction
//
public:	
						MMacsbugMenuHandler(XBoss* boss);

//-----------------------------------
//	Inherited API 
//
public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();
};

DEFINE_INTERFACE_FACTORY(MMacsbugMenuHandler)

//---------------------------------------------------------------
//
// MMacsbugMenuHandler::MMacsbugMenuHandler
//
//---------------------------------------------------------------
MMacsbugMenuHandler::MMacsbugMenuHandler(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// MACSBUG
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"Drops into the low-level debugger (at a time that is known to be good).");

	SAccelerator accelerator('M', kCommandModifier + kOptionModifier);
	menu->AppendItem(LoadWhisperString(L"MACSBUG"), kMacsbugCmd, help, accelerator);
}


//---------------------------------------------------------------
//
// MMacsbugMenuHandler::InstallCommands
//
//---------------------------------------------------------------
void MMacsbugMenuHandler::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	XCallback0<void> action(BreakToDebugger);
	handler->RegisterCommand(kMacsbugCmd, action, kAlwaysEnabled, this);				
}


//---------------------------------------------------------------
//
// MMacsbugMenuHandler::RemoveCommands
//
//---------------------------------------------------------------
void MMacsbugMenuHandler::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	XCallback0<void> action(BreakToDebugger);
	handler->UnRegisterCommand(action);			
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class MZoneRangerExtension
//!		Adds a command that launches Zone Ranger.
// ===================================================================================
class MZoneRangerExtension : public IInstallMenuHandler {
	
//-----------------------------------
//	Initialization/Destruction
//
public:	
						MZoneRangerExtension(XBoss* boss);

//-----------------------------------
//	Inherited API 
//
public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();
};

DEFINE_INTERFACE_FACTORY(MZoneRangerExtension)

//---------------------------------------------------------------
//
// MZoneRangerExtension::MZoneRangerExtension
//
//---------------------------------------------------------------
MZoneRangerExtension::MZoneRangerExtension(XBoss* boss) 
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// Launch Zone Ranger
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"Launches MetroWerk's heap viewing tool.");
	menu->AppendItem(LoadWhisperString(L"Launch ZoneRanger"), kZoneRangerCmd, help);
}


//---------------------------------------------------------------
//
// MZoneRangerExtension::InstallCommands
//
//---------------------------------------------------------------
void MZoneRangerExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	void (*ptr)(OSType, bool, bool, bool) = MDesktopMgr::LaunchApp;	// LaunchApp is overloaded so we have to help the compiler out
	XCallback4<void, OSType, bool, bool, bool> callback(ptr);
	XCallback0<void> action = Bind4(callback, 'mwZR', true, true, true);
	handler->RegisterCommand(kZoneRangerCmd, action, kAlwaysEnabled, this);				
}


//---------------------------------------------------------------
//
// MZoneRangerExtension::RemoveCommands
//
//---------------------------------------------------------------
void MZoneRangerExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	void (*ptr)(OSType, bool, bool, bool) = MDesktopMgr::LaunchApp;	// LaunchApp is overloaded so we have to help the compiler out
	XCallback4<void, OSType, bool, bool, bool> callback(ptr);
	XCallback0<void> action = Bind4(callback, 'mwZR', true, true, true);
	handler->UnRegisterCommand(action);			
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class MSIOUXExtension
//!		Adds some commands relating to the SIOUX debug window.
// ===================================================================================
class MSIOUXExtension : public IInstallMenuHandler {
	
//-----------------------------------
//	Initialization/Destruction
//
public:	
						MSIOUXExtension(XBoss* boss);

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
			void 		DoToggleSIOUX();
			void 		DoCopy();
			void 		DoSelectAll();

			SCommandStatus DoEnableToggle();
			SCommandStatus DoEnableCopy();
			SCommandStatus DoEnableSelectAll();
};

DEFINE_INTERFACE_FACTORY(MSIOUXExtension)

//---------------------------------------------------------------
//
// MSIOUXExtension::MSIOUXExtension
//
//---------------------------------------------------------------
MSIOUXExtension::MSIOUXExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// MACSBUG
	IMenu::SHelp help;
	help.enabled  = LoadWhisperString(L"Shows and hides the SIOUX debug window.");
	help.disabled = LoadWhisperString(L"Shows and hides the SIOUX debug window. Not enabled because the SIOUX window hasn't been created yet.");

	menu->AppendItem(LoadWhisperString(L"Show SIOUX"), kShowSIOUXCmd, help);
}


//---------------------------------------------------------------
//
// MSIOUXExtension::InstallCommands
//
//---------------------------------------------------------------
void MSIOUXExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	// Show SIOUX
	XCallback0<void> action(this, &MSIOUXExtension::DoToggleSIOUX);
	XCallback0<SCommandStatus> enabler(this, &MSIOUXExtension::DoEnableToggle);
	handler->RegisterCommand(kShowSIOUXCmd, action, enabler, this);				

	// Copy
	action = XCallback0<void>(this, &MSIOUXExtension::DoCopy);
	enabler = XCallback0<SCommandStatus>(this, &MSIOUXExtension::DoEnableCopy);
	handler->RegisterCommand(kCopyCmd, action, enabler, this);				

	// Select All
	action = XCallback0<void>(this, &MSIOUXExtension::DoSelectAll);
	enabler = XCallback0<SCommandStatus>(this, &MSIOUXExtension::DoEnableSelectAll);
	handler->RegisterCommand(kSelectAllCmd, action, enabler, this);				
}


//---------------------------------------------------------------
//
// MSIOUXExtension::RemoveCommands
//
//---------------------------------------------------------------
void MSIOUXExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	// Show SIOUX
	XCallback0<void> action(this, &MSIOUXExtension::DoToggleSIOUX);
	XCallback0<SCommandStatus> enabler(this, &MSIOUXExtension::DoEnableToggle);
	handler->UnRegisterCommand(action);			

	// Copy
	action = XCallback0<void>(this, &MSIOUXExtension::DoCopy);
	handler->UnRegisterCommand(action);				

	// Select All
	action = XCallback0<void>(this, &MSIOUXExtension::DoSelectAll);
	handler->UnRegisterCommand(action);				
}


//---------------------------------------------------------------
//
// MSIOUXExtension::DoToggleSIOUX
//
//---------------------------------------------------------------
void MSIOUXExtension::DoToggleSIOUX()
{
	if (IsWindowVisible(GetSIOUXWindow()))
		HideWindow(GetSIOUXWindow());
	else
		ShowWindow(GetSIOUXWindow());
}


//---------------------------------------------------------------
//
// MSIOUXExtension::DoCopy
//
//---------------------------------------------------------------
void MSIOUXExtension::DoCopy()
{
	WindowRef wptr = ::FrontWindow();
	
	EventRecord event;
	event.what      = keyDown;
	event.message   = (0x08 << 8) + 'C';
	event.when      = TickCount();
	event.modifiers = cmdKey;
	
	(void) SIOUXHandleOneEvent(&event);
}


//---------------------------------------------------------------
//
// MSIOUXExtension::DoSelectAll	
//
//---------------------------------------------------------------
void MSIOUXExtension::DoSelectAll()
{
	WindowRef wptr = ::FrontWindow();
	
	EventRecord event;
	event.what      = keyDown;
	event.message   = (0x08 << 8) + 'A';
	event.when      = TickCount();
	event.modifiers = cmdKey;
	
	(void) SIOUXHandleOneEvent(&event);
}


//---------------------------------------------------------------
//
// MSIOUXExtension::DoEnableToggle
//
//---------------------------------------------------------------
SCommandStatus MSIOUXExtension::DoEnableToggle()
{
	SCommandStatus status;
	
	status.enabled = GetSIOUXWindow() != nil;
	if (status.enabled) {
		if (IsWindowVisible(GetSIOUXWindow()))
			status.name = LoadWhisperString(L"Hide SIOUX");
		else
			status.name = LoadWhisperString(L"Show SIOUX");
	}
	
	return status;
}


//---------------------------------------------------------------
//
// MSIOUXExtension::DoEnableCopy
//
//---------------------------------------------------------------
SCommandStatus MSIOUXExtension::DoEnableCopy()
{
	SCommandStatus status;
	
#if !MULTI_FRAGMENT_APP					// $$$ SIOUXTextWindow isn't exported...
	WindowRef wptr = ::FrontWindow();

	if (wptr != nil && GetSIOUXWindow() == wptr && IsWindowVisible(wptr)) {
		TEHandle textH = SIOUXTextWindow != nil ? SIOUXTextWindow->edit : nil;
		status.enabled = textH != nil ? (**textH).selEnd > (**textH).selStart : false;
	}
#endif

	return status;
}


//---------------------------------------------------------------
//
// MSIOUXExtension::DoEnableSelectAll
//
//---------------------------------------------------------------
SCommandStatus MSIOUXExtension::DoEnableSelectAll()
{
	SCommandStatus status;
	
	WindowRef wptr = ::FrontWindow();
	status.enabled = wptr != nil && GetSIOUXWindow() == wptr && IsWindowVisible(wptr);
	
	return status;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class MSystemInfoExtension
//!		Dumps system info.
// ===================================================================================
class MSystemInfoExtension : public IInstallMenuHandler {
	
//-----------------------------------
//	Initialization/Destruction
//
public:	
						MSystemInfoExtension(XBoss* boss);

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

DEFINE_INTERFACE_FACTORY(MSystemInfoExtension)

//---------------------------------------------------------------
//
// MSystemInfoExtension::MSystemInfoExtension
//
//---------------------------------------------------------------
MSystemInfoExtension::MSystemInfoExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// System Info
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"TRACEs out information about the machine the app is running on.");
	menu->AppendItem(LoadWhisperString(L"Dump System Info"), kSystemInfoCmd, help);
}


//---------------------------------------------------------------
//
// MSystemInfoExtension::InstallCommands
//
//---------------------------------------------------------------
void MSystemInfoExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	XCallback0<void> action(this, &MSystemInfoExtension::DoDumpInfo);
	handler->RegisterCommand(kSystemInfoCmd, action, kAlwaysEnabled, this);				
}


//---------------------------------------------------------------
//
// MSystemInfoExtension::RemoveCommands
//
//---------------------------------------------------------------
void MSystemInfoExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	XCallback0<void> action(this, &MSystemInfoExtension::DoDumpInfo);
	handler->UnRegisterCommand(action);			
}


//---------------------------------------------------------------
//
// MSystemInfoExtension::DoDumpInfo
//
//---------------------------------------------------------------
void MSystemInfoExtension::DoDumpInfo()
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
	TRACE("\n");

	TRACE("   System Version: 0x", UInt32ToHex(MSystemInfo::GetSystemVersion()), "\n"); 
	TRACE("   Has System 7: ", MSystemInfo::HasSystem7(), "\n");
	TRACE("   Has System 8: ", MSystemInfo::HasSystem8(), "\n");
	TRACE("   Has Temporary Memory: ", MSystemInfo::HasTempMem(), "\n");
	TRACE("   Has ASLM: ", MSystemInfo::HasASLM(), "\n");
	TRACE("   Has CFM: ", MSystemInfo::HasCFM(), "\n");
	TRACE("   Has Thread Manager: ", MSystemInfo::HasThreadMgr(), "\n");
	TRACE("\n");

	TRACE("   QuickDraw Version: 0x", UInt32ToHex(MSystemInfo::GetQuickDrawVersion()), "\n"); 
	TRACE("   Has Color QuickDraw: ", MSystemInfo::HasColorQuickDraw(), "\n");
	TRACE("   Has 32-bit QuickDraw: ", MSystemInfo::Has32BitQuickDraw(), "\n");
	TRACE("   Has QuickDraw GX: ", MSystemInfo::HasQuickDrawGX(), "\n");
	TRACE("   Has GX Printing: ", MSystemInfo::HasGXPrinting(), "\n");
	TRACE("\n");

	TRACE("   TextEdit Version: 0x", UInt32ToHex(MSystemInfo::GetTextEditVersion()), "\n"); 
	TRACE("   Script Manager Version: 0x", UInt32ToHex(MSystemInfo::GetScriptMgrVersion()), "\n"); 
	TRACE("   Has Grayish Text: ", MSystemInfo::HasGrayishText(), "\n");
	TRACE("   Has True Type: ", MSystemInfo::HasTrueType(), "\n");
	TRACE("\n");

	TRACE("   Display Manager Version: 0x", UInt32ToHex(MSystemInfo::GetDisplayMgrVersion()), "\n"); 
	TRACE("   Has Display Manager: ", MSystemInfo::HasDisplayMgr(), "\n");
	TRACE("\n");

//	TRACE("   QuickTime Version: 0x", UInt32ToHex(MSystemInfo::GetQuickTimeVersion()), "\n"); 
//	TRACE("   Has QuickTime: ", MSystemInfo::HasQuickTime(), "\n");
//	TRACE("\n");

	TRACE("   Sound Manager Version: 0x", UInt32ToHex(MSystemInfo::GetSoundMgrVersion()), "\n"); 
	TRACE("   Has Sound Manager 3: ", MSystemInfo::HasSoundMgr3(), "\n");
	TRACE("   Has 16-bit Sound: ", MSystemInfo::Has16BitSound(), "\n");
	TRACE("\n");

	TRACE("   Has Drag Manager 3: ", MSystemInfo::HasDragMgr(), "\n");
	TRACE("   Has Translucent Dragging: ", MSystemInfo::HasTranslucentDrag(), "\n");
	TRACE("   Has Mac TCP: ", MSystemInfo::HasMacTCP(), "\n");
	TRACE("   Has Open Transport: ", MSystemInfo::HasOpenTransport(), "\n");
	TRACE("   Open Transport Version: 0x", UInt32ToHex(MSystemInfo::GetOpenTransportVersion()), "\n"); 
	TRACE("\n");

	TRACE("   Appearance Manager Version: 0x", UInt32ToHex(MSystemInfo::GetAppearanceVersion()), "\n"); 
	TRACE("   Has Appearance Manager 3: ", MSystemInfo::HasAppearanceMgr(), "\n");
	TRACE("   Has Window Manager 2: ", MSystemInfo::HasWindowMgr2(), "\n");
	TRACE("   Has Floating Windows: ", MSystemInfo::HasFloatingWindows(), "\n");
	TRACE("   Has Nav Services: ", MSystemInfo::HasNavServices(), "\n");
	TRACE("   Has Contextual Menus: ", MSystemInfo::HasContextMenus(), "\n");
	TRACE("   Text Encoding Version: 0x", UInt32ToHex(MSystemInfo::GetTextEncodingConverterVersion()), "\n"); 
	TRACE("\n");

	TRACE("\n");
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class MSystemHeapExtension
//!		Adds commands relating to the OS heaps..
// ===================================================================================
class MSystemHeapExtension : public IInstallMenuHandler {
	
public:	
						MSystemHeapExtension(XBoss* boss);

public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

private:
			void 		DoCheckSystemHeap();
};

DEFINE_INTERFACE_FACTORY(MSystemHeapExtension)

//---------------------------------------------------------------
//
// MSystemHeapExtension::MSystemHeapExtension
//
//---------------------------------------------------------------
MSystemHeapExtension::MSystemHeapExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	IMenu::SHelp help;

	// Check OS Heaps
	help.enabled = LoadWhisperString(L"Select this to check the validity of the OS heaps.");
	menu->AppendItem(LoadWhisperString(L"Check OS Heaps"), kCheckOSHeapsCmd, help);
}


//---------------------------------------------------------------
//
// MSystemHeapExtension::InstallCommands
//
//---------------------------------------------------------------
void MSystemHeapExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	// Check OS Heaps
	XCallback0<void> action(this, &MSystemHeapExtension::DoCheckSystemHeap);
	handler->RegisterCommand(kCheckOSHeapsCmd, action, kAlwaysEnabled, this);				
}


//---------------------------------------------------------------
//
// MSystemHeapExtension::RemoveCommands
//
//---------------------------------------------------------------
void MSystemHeapExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	// Check OS Heaps
	XCallback0<void> action(this, &MSystemHeapExtension::DoCheckSystemHeap);
	handler->UnRegisterCommand(action);			
}


//---------------------------------------------------------------
//
// MSystemHeapExtension::DoCheckSystemHeap
//
//---------------------------------------------------------------
void MSystemHeapExtension::DoCheckSystemHeap()
{
	bool valid = ::CheckAllHeaps();
	if (valid)
		DoNote(LoadWhisperString(L"OS Heaps are OK"), L"");
	else
		DoStop(LoadWhisperString(L"OS Heaps are *not* valid"), L"");
}


}		// namespace Whisper
#endif	// DEBUG