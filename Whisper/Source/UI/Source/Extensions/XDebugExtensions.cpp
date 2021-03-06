/*
 *  File:       XDebugExtensions.cpp
 *	Function:	Concrete XDebugExtension objects.
 *  Written by: Jesse Jones
 *
 *	Classes:	XAssertExtension	    - Adds a command that toggles breaking on ASSERTs on and off.
 *				XIntenseExtension	    - Adds a command that toggles intense debugging on and off.
 *				XObjectHeapExtension	- Adds operator new debug support.
 *				XTraceExtension		    - Adds commands for TRACE and TRACEFLOW.
 *				XUnitTestExtension	    - Adds a submenu for executing unit tests.
 *				XControlExtension	    - Adds IControl related debug commands to the Debug menu.
 *				XFocusExtension		   	- Adds some commands relating to the focus chain.
 *				XSeperatorExtension		- Adds a menu seperator to the Debug menu.
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDebugExtensions.cpp,v $
 *		Revision 1.13  2001/04/27 09:20:14  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.12  2001/04/27 04:31:46  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.11  2001/04/19 04:06:21  jesjones
 *		Trace extension uses the new XPreference specialization for std::map to write out the prefs.
 *		
 *		Revision 1.10  2001/03/15 09:09:16  jesjones
 *		Added a work around for an internal compiler error for Intel DLL targets.
 *		
 *		Revision 1.9  2001/03/06 07:32:12  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.8  2001/03/02 11:24:23  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.7  2001/02/12 09:48:53  jesjones
 *		Disabled XObjectHeapExtension for DLL targets.
 *		
 *		Revision 1.6  2001/01/30 07:07:56  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.5  2001/01/22 02:01:28  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.4  2001/01/21 00:43:49  jesjones
 *		Temporarily disabled.
 *		
 *		Revision 1.3  2000/12/14 08:33:57  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:30:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IInstallMenuHandler.h>

#include <iomanip>
#include <iterator>
#include <typeinfo>

#include <IActiveMenuBar.h>
#include <IControl.h>
#include <ICurrentFocus.h>
#include <IFocus.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IKeyHandler.h>
#include <IMenu.h>
#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <ITimer.h>
#include <IWindow.h>
#include <IWindowMgr.h>
#include <XDebugMalloc.h>
#include <XDebugNew.h>
#include <XFile.h>
#include <XFolderSpec.h>
#include <XHandleStream.h>
#include <XIntConversions.h>
#include <XPreference.h>
#include <XStreaming.h>
#include <XStringUtils.h>
#include <XTranscode.h>
#include <XTrace.h>
#include <XTraceSink.h>
#include <XUnitTest.h>
#include <XUnitTests.h>

#if MAC
	#include <IEventDispatcher.h> 
#endif

#if DEBUG
struct SBlockCount {								
	uint32	size;
	uint32	current;
	uint32	total;
	
	bool operator==(const SBlockCount& rhs) const	{return total == rhs.total;}
	bool operator<(const SBlockCount& rhs) const	{return total < rhs.total;}
};

namespace Whisper {


//-----------------------------------
//	Constants
//
const XMenuCommand kAssertCmd		= L"toggle break on assert";
const XMenuCommand kIntenseCmd		= L"toggle intense debugging";
const XMenuCommand kCheckHeapCmd	= L"check object heap";
const XMenuCommand kFreeBlocksCmd	= L"free deleted blocks";
const XMenuCommand kDumpHeapCmd		= L"dump object heap";
const XMenuCommand kEchoToSinkCmd	= L"echo traces to: ";
const XMenuCommand kTraceFlowCmd	= L"traceflow category";
const XMenuCommand kDoAllUnitTestCmd= L"do all unit tests";
const XMenuCommand kUnitTestCmd		= L"unit test: ";

const XMenuCommand kDelayPaneCmd 	= L"delay pane drawing";
const XMenuCommand kHilitePaneCmd 	= L"hilite pane drawing";
const XMenuCommand kDumpControlsCmd = L"dump control hierarchy";
const XMenuCommand kZapPortCmd 		= L"zap port";
const XMenuCommand kDumpChainCmd	= L"dump command chain";

const MilliSecond kUpdateFreq = 1000;		// interval at which XTraceExtension and XUnitTestExtension refresh themselves


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// InsertMenuItem
//
//---------------------------------------------------------------
static void InsertMenuItem(IMenuPtr& menu, const std::wstring& name, uint32 startItem, uint32 count, const XMenuCommand& cmd)
{
	bool found = false;

#if 0
	if (count > 0) {
		for (uint32 index = startItem; index <= startItem + count - 1 && !found; index++) {
			std::wstring item = menu->GetName(index);
			
			found = item == name;
		}
	}
#endif
	
	if (!found) {
		if (count == 0)
			menu->InsertItem(name, startItem, cmd);
		else
			menu->InsertSortedItem(name, startItem, count, cmd);
	}
}


//---------------------------------------------------------------
//
// DumpBoss
//
//---------------------------------------------------------------
static void DumpBoss(const IHierarchyPtr& node, const std::wstring& padding)
{
	TRACE(padding, "Boss: ", node->GetBoss()->GetName(), "\n");

	IGeometryPtr geometry(node);

	IControlPtr control(node);
	if (control) {
		XRect frame = geometry->GetFrame();
		
		TRACE(padding, L"IControl: '", control->GetName(), "' frame = ", frame, "\n");
	}

	IWindowPtr window(node);
	if (window) {
		XRect frame = geometry->GetFrame();
		
		TRACE(padding, L"IWindow: '", window->GetName(), "' frame = ", frame, "\n");
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XAssertExtension
//!		Adds a command that toggles breaking on ASSERTs on and off.
// ===================================================================================
class XAssertExtension : public IInstallMenuHandler {
	
public:	
						XAssertExtension(XBoss* boss);

public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

private:	
			void 		DoToggleAssert();
			SCommandStatus DoEnableAssert();
};

DEFINE_INTERFACE_FACTORY(XAssertExtension)

//---------------------------------------------------------------
//
// XAssertExtension::XAssertExtension
//
//---------------------------------------------------------------
XAssertExtension::XAssertExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// Break on ASSERTs
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"Select this to enable or disable breaking to the debugger when an ASSERT fires.");
	menu->AppendItem(LoadWhisperString(L"Break on ASSERTs"), kAssertCmd, help);
}


//---------------------------------------------------------------
//
// XAssertExtension::InstallCommands
//
//---------------------------------------------------------------
void XAssertExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	XCallback0<void> action(this, &XAssertExtension::DoToggleAssert);
	XCallback0<SCommandStatus> enabler(this, &XAssertExtension::DoEnableAssert);
	handler->RegisterCommand(kAssertCmd, action, enabler, this);				
}


//---------------------------------------------------------------
//
// XAssertExtension::RemoveCommands
//
//---------------------------------------------------------------
void XAssertExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	XCallback0<void> action(this, &XAssertExtension::DoToggleAssert);
	handler->UnRegisterCommand(action);			
}


//---------------------------------------------------------------
//
// XAssertExtension::DoToggleAssert
//
//---------------------------------------------------------------
void XAssertExtension::DoToggleAssert()
{
	gBreakOnAssert = !gBreakOnAssert;
}


//---------------------------------------------------------------
//
// XAssertExtension::DoEnableAssert
//
//---------------------------------------------------------------
SCommandStatus XAssertExtension::DoEnableAssert()
{
	SCommandStatus status;

	if (gBreakOnAssert)
		status.name = LoadWhisperString(L"Ignore ASSERTs");
	else
		status.name = LoadWhisperString(L"Break on ASSERTs");
	status.enabled = true;

	return status;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XIntenseExtension
//!		Adds a command that toggles intense debugging on and off.
// ===================================================================================
class XIntenseExtension : public IInstallMenuHandler {
	
public:	
						XIntenseExtension(XBoss* boss);

public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

private:	
			void 		DoToggleIntense();
			SCommandStatus DoEnableIntense();
};

DEFINE_INTERFACE_FACTORY(XIntenseExtension)

//---------------------------------------------------------------
//
// XIntenseExtension::XIntenseExtension
//
//---------------------------------------------------------------
XIntenseExtension::XIntenseExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// Enable Intense Debugging
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"Select this to enable or disable extra debugging checks.");
	menu->AppendItem(LoadWhisperString(L"Enable Intense Debugging"), kIntenseCmd, help);
}


//---------------------------------------------------------------
//
// XIntenseExtension::InstallCommands
//
//---------------------------------------------------------------
void XIntenseExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	XCallback0<void> action(this, &XIntenseExtension::DoToggleIntense);
	XCallback0<SCommandStatus> enabler(this, &XIntenseExtension::DoEnableIntense);
	handler->RegisterCommand(kIntenseCmd, action, enabler, this);				
}


//---------------------------------------------------------------
//
// XIntenseExtension::RemoveCommands
//
//---------------------------------------------------------------
void XIntenseExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	XCallback0<void> action(this, &XIntenseExtension::DoToggleIntense);
	handler->UnRegisterCommand(action);			
}


//---------------------------------------------------------------
//
// XIntenseExtension::DoToggleIntense
//
//---------------------------------------------------------------
void XIntenseExtension::DoToggleIntense()
{
	gIntenseDebugging = !gIntenseDebugging;
}


//---------------------------------------------------------------
//
// XIntenseExtension::DoEnableIntense
//
//---------------------------------------------------------------
SCommandStatus XIntenseExtension::DoEnableIntense()
{
	SCommandStatus status;

	if (gIntenseDebugging)
		status.name = LoadWhisperString(L"Disable Intense Debugging");
	else
		status.name = LoadWhisperString(L"Enable Intense Debugging");
	status.enabled = true;

	return status;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XObjectHeapExtension
//!		Adds operator new debug support.
// ===================================================================================
#if !MULTI_FRAGMENT_APP && WHISPER_OPERATOR_NEW
class XObjectHeapExtension : public IInstallMenuHandler {
	
public:	
						XObjectHeapExtension(XBoss* boss);

public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

private:
			void 		DoToggleCheckHeap();
			void 		DoToggleFreeingBlocks();
			void 		DoDumpObjectHeap();

			SCommandStatus DoEnableCheckHeap();
			SCommandStatus DoEnableFreeingBlocks();

			void 		DoDumpCommonBlockSizes();
};

DEFINE_INTERFACE_FACTORY(XObjectHeapExtension)

//---------------------------------------------------------------
//
// XObjectHeapExtension::XObjectHeapExtension
//
//---------------------------------------------------------------
XObjectHeapExtension::XObjectHeapExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// Enable Heap Checking
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"Select this to enable or disable validating the operator new heap before each new.");
	menu->AppendItem(LoadWhisperString(L"Enable Heap Checking"), kCheckHeapCmd, help);
	
	// Free Deleted Blocks
	help.enabled = LoadWhisperString(L"Select this to enable or disable leaving allocated blocks allocated.");
	menu->AppendItem(LoadWhisperString(L"Free Deleted Blocks"), kFreeBlocksCmd, help);
	
	// Dump Object Heap
	help.enabled = LoadWhisperString(L"Select this to TRACE out information about the operator new heap.");
	menu->AppendItem(LoadWhisperString(L"Dump Object Heap"), kDumpHeapCmd, help);
}


//---------------------------------------------------------------
//
// XObjectHeapExtension::InstallCommands
//
//---------------------------------------------------------------
void XObjectHeapExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	// Enable Heap Checking
	XCallback0<void> action(this, &XObjectHeapExtension::DoToggleCheckHeap);
	XCallback0<SCommandStatus> enabler(this, &XObjectHeapExtension::DoEnableCheckHeap);
	handler->RegisterCommand(kCheckHeapCmd, action, enabler, this);				

	// Free Deleted Blocks
	action = XCallback0<void>(this, &XObjectHeapExtension::DoToggleFreeingBlocks);
	enabler = XCallback0<SCommandStatus>(this, &XObjectHeapExtension::DoEnableFreeingBlocks);
	handler->RegisterCommand(kFreeBlocksCmd, action, enabler, this);				

	// Dump Object Heap
	action = XCallback0<void>(this, &XObjectHeapExtension::DoDumpObjectHeap);
	handler->RegisterCommand(kDumpHeapCmd, action, kAlwaysEnabled, this);				
}


//---------------------------------------------------------------
//
// XObjectHeapExtension::RemoveCommands
//
//---------------------------------------------------------------
void XObjectHeapExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	// Enable Heap Checking
	XCallback0<void> action(this, &XObjectHeapExtension::DoToggleCheckHeap);
	handler->UnRegisterCommand(action);			

	// Free Deleted Blocks
	action = XCallback0<void>(this, &XObjectHeapExtension::DoToggleFreeingBlocks);	
	handler->UnRegisterCommand(action);			

	// Dump Object Heap
	action = XCallback0<void>(this, &XObjectHeapExtension::DoDumpObjectHeap);
	handler->UnRegisterCommand(action);			
}


//---------------------------------------------------------------
//
// XObjectHeapExtension::DoToggleCheckHeap
//
//---------------------------------------------------------------
void XObjectHeapExtension::DoToggleCheckHeap()
{
	gValidateHeapOnNew = !gValidateHeapOnNew;
}


//---------------------------------------------------------------
//
// XObjectHeapExtension::DoToggleFreeingBlocks
//
//---------------------------------------------------------------
void XObjectHeapExtension::DoToggleFreeingBlocks()
{
	gFreeDeletedBlocks = !gFreeDeletedBlocks;
}


//---------------------------------------------------------------
//
// XObjectHeapExtension::DoEnableCheckHeap
//
//---------------------------------------------------------------
SCommandStatus XObjectHeapExtension::DoEnableCheckHeap()
{
	SCommandStatus status;

	if (gValidateHeapOnNew)
		status.name = LoadWhisperString(L"Disable Heap Checking");
	else
		status.name = LoadWhisperString(L"Enable Heap Checking");
	status.enabled = true;

	return status;
}


//---------------------------------------------------------------
//
// XObjectHeapExtension::DoEnableFreeingBlocks
//
//---------------------------------------------------------------
SCommandStatus XObjectHeapExtension::DoEnableFreeingBlocks()
{
	SCommandStatus status;

	if (gFreeDeletedBlocks)
		status.name = LoadWhisperString(L"Don't Free Deleted Blocks");
	else
		status.name = LoadWhisperString(L"Free Deleted Blocks");
	status.enabled = true;

	return status;
}

//---------------------------------------------------------------
//
// XObjectHeapExtension::DoDumpCommonBlockSizes
//
//---------------------------------------------------------------
void XObjectHeapExtension::DoDumpCommonBlockSizes()
{
	uint32 index;

	SBlockCount blocks[256];
	for (index = 0; index < 256; index++) {
		blocks[index].size    = index;
		blocks[index].current = gDebugMalloc->GetCurrentBlockCount(index);
		blocks[index].total   = gDebugMalloc->GetTotalBlockCount(index);
	}
		
	std::sort(blocks, blocks + 256);

	TRACE("   Number of blocks allocated for sizes under 256:\n");
	TRACE("      Bytes   Current    Total\n");
	for (index = 255; index >= 255 - 10; index--)
//		TRACE("        %3d   %7d  %7d\n", blocks[index].size, blocks[index].current, blocks[index].total);
		TRACE("        ", ToStr(blocks[index].size, 3), "   ", ToStr(blocks[index].current, 7), "  ", ToStr(blocks[index].total, 7), "\n");

	TRACE("\n");
}

	
//---------------------------------------------------------------
//
// XObjectHeapExtension::DoDumpObjectHeap
//
//---------------------------------------------------------------
void XObjectHeapExtension::DoDumpObjectHeap()
{
	gDebugMalloc->ValidateHeap();
	
	TRACE("Object heap:\n");
	
	TRACE("   Highwater mark: ",   Whisper::BytesToStr(gDebugMalloc->GetHighwaterMark()), "\n");
	TRACE("   Bytes in use:   ",   Whisper::BytesToStr(gDebugMalloc->GetCurrentBytes()), "\n");
	TRACE("   Debug overhead: ", gDebugMalloc->GetDebugOverhead(), "%\n");

	TRACE("\n");
	TRACE("   Number of active blocks: ", gDebugMalloc->GetBlockCount(), "\n");
	this->DoDumpCommonBlockSizes();
	
	TRACE("\n");
}
#endif	// WHISPER_OPERATOR_NEW

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XTraceExtension
//!		Adds commands for TRACE and TRACEFLOW.
// ===================================================================================
class XTraceExtension : public IInstallMenuHandler {
	
public:	
	virtual				~XTraceExtension();
						
						XTraceExtension(XBoss* boss);

public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

private:
			void 		DoToggleSink();
			void 		DoToggleCategory();
			void 		DoUpdate();

			SCommandStatus DoEnableSink();
			SCommandStatus DoEnableCategory();

			void 		DoWritePrefs();	
			void 		DoReadPrefs();
			
private:
#if WIN
	std::wstring	mPrefName;
#endif
};

DEFINE_INTERFACE_FACTORY(XTraceExtension)

//---------------------------------------------------------------
//
// XTraceExtension::~XTraceExtension
//
//---------------------------------------------------------------
XTraceExtension::~XTraceExtension()
{
	this->DoWritePrefs();
}


//---------------------------------------------------------------
//
// XTraceExtension::XTraceExtension
//
//---------------------------------------------------------------
XTraceExtension::XTraceExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);
	
	this->DoReadPrefs();

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// TRACE Sinks
	XTrace::sink_iter iter = XTrace::Instance()->sink_begin();
	while (iter != XTrace::Instance()->sink_end()) {
		XTraceSink* sink = *iter;
		++iter;
		
		const wchar_t* name = sink->GetName();
		menu->AppendItem(LoadWhisperString(L"Echo TRACEs to #1", name), kEchoToSinkCmd + name);
	}

	// TRACEFLOW Menu
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"Use this menu to toggle TRACEFLOW categories off and on.");

	IMenu::SHelp help2;
	help2.enabled = LoadWhisperString(L"Select this item to TRACE out TRACEFLOWs using this category.");
	help2.checked = LoadWhisperString(L"Select this item to stop TRACEing out TRACEFLOWs using this category.");

	IMenuPtr subMenu(menu->GetBoss()->GetName());
	subMenu->Init(LoadWhisperString(L"TRACEFLOW Categories"), help, help2, 'DbgM');
	menu->AppendSubMenu(subMenu);
}


//---------------------------------------------------------------
//
// XTraceExtension::InstallCommands
//
//---------------------------------------------------------------
void XTraceExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	ITimerPtr timer(L"Application");
	XCallback0<void> callback(this, &XTraceExtension::DoUpdate);
	timer->AddTimer(callback, kUpdateFreq);

	// TRACE Sinks
	XCallback0<void> action(this, &XTraceExtension::DoToggleSink);
	XCallback0<SCommandStatus> enabler(this, &XTraceExtension::DoEnableSink);
	handler->RegisterCommand(kEchoToSinkCmd, action, enabler, this, true);				

	// TRACEFLOW Categories
	action = XCallback0<void>(this, &XTraceExtension::DoToggleCategory);
	enabler = XCallback0<SCommandStatus>(this, &XTraceExtension::DoEnableCategory);
	handler->RegisterCommand(kTraceFlowCmd, action, enabler, this, true);				
}


//---------------------------------------------------------------
//
// XTraceExtension::RemoveCommands
//
//---------------------------------------------------------------
void XTraceExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	ITimerPtr timer(L"Application");
	XCallback0<void> callback(this, &XTraceExtension::DoUpdate);
	timer->RemoveTimer(callback);

	// TRACE Sinks
	XCallback0<void> action(this, &XTraceExtension::DoToggleSink);
	handler->UnRegisterCommand(action);			

	// TRACEFLOW Categories
	action = XCallback0<void>(this, &XTraceExtension::DoToggleCategory);
	handler->UnRegisterCommand(action);				
}


//---------------------------------------------------------------
//
// XTraceExtension::DoToggleSink
//
//---------------------------------------------------------------
void XTraceExtension::DoToggleSink()
{
	IMenuHandlerPtr handler(L"Application");	
	const XMenuCommand& command = handler->GetCommand();
	
	std::wstring name = After(command, kEchoToSinkCmd);
	XTraceSink* sink = XTrace::Instance()->GetSink(name);
	sink->Enable(!sink->IsEnabled());
}


//---------------------------------------------------------------
//
// XTraceExtension::DoToggleCategory
//
//---------------------------------------------------------------
void XTraceExtension::DoToggleCategory()
{
	IMenuHandlerPtr handler(L"Application");	
	const XMenuCommand& command = handler->GetCommand();
	
	std::wstring category = After(command, kTraceFlowCmd + L" ");
	bool enabled = XTrace::Instance()->IsEnabled(ToUTF8Str(category).c_str());
	XTrace::Instance()->Enable(ToUTF8Str(category).c_str(), !enabled);
}


//---------------------------------------------------------------
//
// XTraceExtension::DoEnableSink
//
//---------------------------------------------------------------
SCommandStatus XTraceExtension::DoEnableSink()
{
	SCommandStatus status;

	IMenuHandlerPtr handler(L"Application");	
	const XMenuCommand& command = handler->GetCommand();
	
	std::wstring name = After(command, kEchoToSinkCmd);
	XTraceSink* sink = XTrace::Instance()->Instance()->GetSink(name);
	
	status.enabled   = true;
	status.checkmark = sink->IsEnabled() ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;

	return status;
}


//---------------------------------------------------------------
//
// XTraceExtension::DoEnableCategory
//
//---------------------------------------------------------------
SCommandStatus XTraceExtension::DoEnableCategory()
{
	SCommandStatus status;

	IActiveMenuBarPtr active(L"Application");
	IMenuBarPtr menubar = active->Get();
	IMenuHandlerPtr handler(L"Application");	
	const XMenuCommand& command = handler->GetCommand();
	
	std::wstring category = After(command, kTraceFlowCmd + L" ");
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"TRACEFLOW Categories"));
	
	status.enabled   = true;
	status.checkmark = XTrace::Instance()->IsEnabled(ToUTF8Str(category).c_str()) ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;

	return status;
}


//---------------------------------------------------------------
//
// XTraceExtension::DoUpdate
//
//---------------------------------------------------------------
void XTraceExtension::DoUpdate()
{
	IActiveMenuBarPtr active(L"Application");
	IMenuBarPtr menubar = active->Get();

	if (menubar) {
		IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"TRACEFLOW Categories"));
		
		uint32 count = XTrace::Instance()->GetNumCategories();
		if (count > 0 && menu && count != menu->GetCount()) {
			menu->RemoveItems();
			
			XTrace::cat_iter iter = XTrace::Instance()->category_begin();
			while (iter != XTrace::Instance()->category_end()) {
				std::wstring category = FromUTF8Str(iter->category.c_str());
				++iter;

				XMenuCommand cmd = kTraceFlowCmd + L" " + category;				
				Whisper::InsertMenuItem(menu, category, 0, menu->GetCount(), cmd);
			}
					
			menubar->Invalidate();
		}
	}
}


//---------------------------------------------------------------
//
// XTraceExtension::DoWritePrefs
//
//---------------------------------------------------------------
void XTraceExtension::DoWritePrefs()
{
	try {
		// Write out the sink states
		XPreference<std::map<std::wstring, bool> > sinks(L"Trace Sinks");
		
		sinks->clear();
		for (XTrace::sink_iter iter = XTrace::Instance()->sink_begin(); iter != XTrace::Instance()->sink_end(); ++iter) {
			XTraceSink* sink = *iter;
			sinks->insert(std::map<std::wstring, bool>::value_type(sink->GetName(), sink->IsEnabled()));
		}
	
		// Write out the TRACEFLOW categories
		XPreference<std::map<std::wstring, bool> > categories(L"Traceflow Categories");

		categories->clear();
		for (XTrace::cat_iter iter = XTrace::Instance()->category_begin(); iter != XTrace::Instance()->category_end(); ++iter) {
			const XTrace::SCategory& category = *iter;
			categories->insert(std::map<std::wstring, bool>::value_type(FromUTF8Str(category.category.c_str()), category.enabled));
		}
		
	} catch (...) {
		// eat exceptions
	}
}


//---------------------------------------------------------------
//
// XTraceExtension::DoReadPrefs
//
//---------------------------------------------------------------
void XTraceExtension::DoReadPrefs()
{
	try {
		static bool inited = false;		// work around for multiple menubar's

		if (!inited) {
			// Read in the sink states
			XPreference<std::map<std::wstring, bool> > sinks(L"Trace Sinks");
			
			for (std::map<std::wstring, bool>::const_iterator iter = sinks->begin(); iter != sinks->end(); ++iter) {
				if (XTrace::Instance()->HasSink(iter->first)) {
					XTraceSink* sink = XTrace::Instance()->GetSink(iter->first);
					sink->Enable(iter->second);
				}
			}
		
			// Read in the TRACEFLOW categories
			XPreference<std::map<std::wstring, bool> > categories(L"Traceflow Categories");
			
			for (std::map<std::wstring, bool>::const_iterator iter = categories->begin(); iter != categories->end(); ++iter) {
				XTrace::Instance()->Enable(ToUTF8Str(iter->first).c_str(), iter->second);
			}

			inited = true;
		}
	
	} catch (...) {
		// eat exceptions
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XUnitTestExtension
//!		Adds a submenu for executing unit tests.
// ===================================================================================
class XUnitTestExtension : public IInstallMenuHandler {
	
public:	
	virtual				~XUnitTestExtension();
						
						XUnitTestExtension(XBoss* boss);

public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

private:
			void 		DoUnitTest();
			void 		DoUpdate();
			void 		DoRunAllTests();
			
private:
	uint32	mKey;
};

DEFINE_INTERFACE_FACTORY(XUnitTestExtension)

//---------------------------------------------------------------
//
// XUnitTestExtension::~XUnitTestExtension
//
//---------------------------------------------------------------
XUnitTestExtension::~XUnitTestExtension()
{
}


//---------------------------------------------------------------
//
// XUnitTestExtension::XUnitTestExtension
//
//---------------------------------------------------------------
XUnitTestExtension::XUnitTestExtension(XBoss* boss)
{	
	IInstallMenuHandler::DoSetBoss(boss);
	
	mKey = 0;

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// Unit Test Submenu
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"Use this menu to run a unit test.");
	
	IMenu::SHelp help2;
	help2.enabled = LoadWhisperString(L"Select this item to run the unit test.");
		
	IMenuPtr subMenu(menu->GetBoss()->GetName());
	subMenu->Init(LoadWhisperString(L"Unit Tests"), help, help2, 'UntT');
	menu->AppendSubMenu(subMenu);
}


//---------------------------------------------------------------
//
// XUnitTestExtension::InstallCommands
//
//---------------------------------------------------------------
void XUnitTestExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	ITimerPtr timer(L"Application");
	XCallback0<void> callback(this, &XUnitTestExtension::DoUpdate);
	timer->AddTimer(callback, kUpdateFreq);	

	// Run All Tests
	XCallback0<void> action(this, &XUnitTestExtension::DoRunAllTests);
	handler->RegisterCommand(kDoAllUnitTestCmd, action, kAlwaysEnabled, this);				

	// Unit Tests
	action = XCallback0<void>(this, &XUnitTestExtension::DoUnitTest);
	handler->RegisterCommand(kUnitTestCmd, action, kAlwaysEnabled, this, true);				
}


//---------------------------------------------------------------
//
// XUnitTestExtension::RemoveCommands
//
//---------------------------------------------------------------
void XUnitTestExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	ITimerPtr timer(L"Application");
	XCallback0<void> callback(this, &XUnitTestExtension::DoUpdate);
	timer->RemoveTimer(callback);

	// Run All Tests
	XCallback0<void> action(this, &XUnitTestExtension::DoRunAllTests);
	handler->UnRegisterCommand(action);			

	// Unit Tests
	action = XCallback0<void>(this, &XUnitTestExtension::DoUnitTest);
	handler->UnRegisterCommand(action);			
}


//---------------------------------------------------------------
//
// XUnitTestExtension::DoUnitTest
//
//---------------------------------------------------------------
void XUnitTestExtension::DoUnitTest()
{
	IMenuHandlerPtr handler(L"Application");	
	const XMenuCommand& command = handler->GetCommand();

	std::wstring temp  = After(command, kUnitTestCmd);
	std::wstring suite = Before(temp, L"-");
	std::wstring name  = After(temp, L"-");

	XUnitTests::Instance()->RunTest(suite, name);
}


//---------------------------------------------------------------
//
// XUnitTestExtension::DoRunAllTests
//
// Get internal compiler errors with Pro 6 when trying to use
// RunAllTests directly.
//
//---------------------------------------------------------------
void XUnitTestExtension::DoRunAllTests()
{
	XUnitTests::Instance()->RunAllTests();
}


//---------------------------------------------------------------
//
// XUnitTestExtension::DoUpdate
//
//---------------------------------------------------------------
void XUnitTestExtension::DoUpdate()
{
	// Try to get a pointer to our menu,
	IActiveMenuBarPtr active(L"Application");
	IMenuBarPtr menubar = active->Get();

	if (menubar) {
		IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Unit Tests"));
		if (menu) {
		
			// see if we need to rebuild the menu,
			uint32 key = XUnitTests::Instance()->GetEditKey();
			if (key != mKey) {
				mKey = key;
			
				// if we do then get rid of all the old stuff,
				menu->RemoveItems();
		
				// add the run all menu item,
				IMenu::SHelp help;
				help.enabled = LoadWhisperString(L"Select this item to run all the unit tests.");
				menu->AppendItem(LoadWhisperString(L"Run All Tests"), kDoAllUnitTestCmd, help);
				
				menu->AppendSeparator();

				// and add a submenu for each suite.
				XUnitTests::iterator iter = XUnitTests::Instance()->begin();
				while (iter != XUnitTests::Instance()->end()) {
					const XUnitTests::SSuite& suite = *iter;
					++iter;
					
					if (!suite.suite.empty()) {
						IMenuPtr subMenu(menu->GetBoss()->GetName());
						subMenu->Init(suite.name);
						
						menu->AppendSubMenu(subMenu);
	
						for (uint32 index = 0; index < suite.suite.size(); ++index) {
							const XUnitTest* test = suite.suite[index];
						
							XMenuCommand command = kUnitTestCmd + suite.name + L"-" + test->GetName();
							Whisper::InsertMenuItem(subMenu, test->GetName(), 0, menu->GetCount(), command);
						}
					}
				}
				
				menubar->Invalidate();
			}
		}
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XControlExtension
//!		Adds IControl related debug commands to the Debug menu.
// ===================================================================================
class XControlExtension : public IInstallMenuHandler {
	
public:	
						XControlExtension(XBoss* boss);

public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

private:
			void 		DoDumpWindow();
			void 		DoDumpBoss(const IHierarchyPtr& node, const std::wstring& padding);

			SCommandStatus DoEnableDumpWindow();
#if MAC
			SCommandStatus DoEnableZapPort();
#endif
};

DEFINE_INTERFACE_FACTORY(XControlExtension)

//---------------------------------------------------------------
//
// XControlExtension::XControlExtension
//
//---------------------------------------------------------------
XControlExtension::XControlExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// Dump Window
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"Select this to TRACE out the control's in the front regular window.");
	menu->AppendItem(LoadWhisperString(L"Dump Window Hierarchy"), kDumpControlsCmd, help);

//	this->DoAddItem(LoadWhisperString(L"Delay Pane Drawing"), kDelayPaneCmd);
//	this->DoAddItem(LoadWhisperString(L"Hilite Pane Drawing"), kHilitePaneCmd);
	
#if MAC
	// Set GrafPort to Nil
	help.enabled = LoadWhisperString(L"Select this to set the current GrafPort to nil each time thru the main event loop.");
	menu->AppendItem(LoadWhisperString(L"Set GrafPort to Nil"), kZapPortCmd, help);
#endif
}


//---------------------------------------------------------------
//
// XControlExtension::InstallCommands
//
//---------------------------------------------------------------
void XControlExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	// Dump Window
	XCallback0<void> action(this, &XControlExtension::DoDumpWindow);
	XCallback0<SCommandStatus> enabler(this, &XControlExtension::DoEnableDumpWindow);
	handler->RegisterCommand(kDumpControlsCmd, action, enabler, this);				

#if MAC
	// Set GrafPort to Nil
	IEventDispatcherPtr dispatcher(L"Application");
	action = XCallback0<void>(dispatcher.Get(), &IEventDispatcher::ToggleZapPort);
	enabler = XCallback0<SCommandStatus>(this, &XControlExtension::DoEnableZapPort);
	handler->RegisterCommand(kZapPortCmd, action, enabler, this);			
#endif
}


//---------------------------------------------------------------
//
// XControlExtension::RemoveCommands
//
//---------------------------------------------------------------
void XControlExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	// MACSBUG
	XCallback0<void> action(this, &XControlExtension::DoDumpWindow);
	handler->UnRegisterCommand(action);			

#if MAC
	// Set GrafPort to Nil
	IEventDispatcherPtr dispatcher(L"Application");
	action = XCallback0<void>(dispatcher.Get(), &IEventDispatcher::ToggleZapPort);
	handler->UnRegisterCommand(action);			
#endif
}


//---------------------------------------------------------------
//
// XControlExtension::DoDumpWindow
//
//---------------------------------------------------------------
void XControlExtension::DoDumpWindow()
{
	IWindowMgrPtr mgr(L"Application");
	IWindowPtr wind = mgr->GetTopRegular();
	
	if (wind) {
		TRACE("Dumping window '", wind->GetName(), "':\n");
		this->DoDumpBoss(IHierarchyPtr(wind), L"   ");
	}
}


//---------------------------------------------------------------
//
// XControlExtension::DoEnableDumpWindow
//
//---------------------------------------------------------------
SCommandStatus XControlExtension::DoEnableDumpWindow()
{
	SCommandStatus status;
	
	IWindowMgrPtr mgr(L"Application");
	IWindowPtr wind = mgr->GetTopRegular();

	status.enabled = wind;
	if (wind)
		status.name = LoadWhisperString(L"Dump '#1' Hierarchy", wind->GetName());
	else
		status.name = LoadWhisperString(L"Dump Window Hierarchy");

	return status;
}


//---------------------------------------------------------------
//
// XControlExtension::DoEnableZapPort
//
//---------------------------------------------------------------
#if MAC
SCommandStatus XControlExtension::DoEnableZapPort()
{
	SCommandStatus status;
	
	IEventDispatcherPtr dispatcher(L"Application");
	if (dispatcher->GetZapPort())
		status.name = LoadWhisperString(L"Don't Set GrafPort to Nil");
	else
		status.name = LoadWhisperString(L"Set GrafPort to Nil");
	status.enabled = true;

	return status;
}
#endif


//---------------------------------------------------------------
//
// XControlExtension::DoDumpBoss
//
//---------------------------------------------------------------
void XControlExtension::DoDumpBoss(const IHierarchyPtr& node, const std::wstring& padding)
{
	Whisper::DumpBoss(node, padding);
	TRACE("\n");

	IHierarchy::iterator iter = node->begin();
	while (iter != node->end()) {
		IHierarchyPtr child = *iter;
		++iter;
		
		this->DoDumpBoss(child, padding + L"   ");
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFocusExtension
//!		Adds some commands relating to the focus chain.
// ===================================================================================
class XFocusExtension : public IInstallMenuHandler {
	
public:	
						XFocusExtension(XBoss* boss);

public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

private:
			void 		DoDumpCommandChain();
};

DEFINE_INTERFACE_FACTORY(XFocusExtension)

//---------------------------------------------------------------
//
// XFocusExtension::XFocusExtension
//
//---------------------------------------------------------------
XFocusExtension::XFocusExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	
	// Dump Focus Chain
	IMenu::SHelp help;
	help.enabled = LoadWhisperString(L"Select this to TRACE out the current focus chain.");
	menu->AppendItem(LoadWhisperString(L"Dump Focus Chain"), kDumpChainCmd, help);
}


//---------------------------------------------------------------
//
// XFocusExtension::InstallCommands
//
//---------------------------------------------------------------
void XFocusExtension::InstallCommands()
{	
	IMenuHandlerPtr handler(L"Application");	

	XCallback0<void> action(this, &XFocusExtension::DoDumpCommandChain);
	handler->RegisterCommand(kDumpChainCmd, action, kAlwaysEnabled, this);				
}


//---------------------------------------------------------------
//
// XFocusExtension::RemoveCommands
//
//---------------------------------------------------------------
void XFocusExtension::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	XCallback0<void> action(this, &XFocusExtension::DoDumpCommandChain);
	handler->UnRegisterCommand(action);			
}


//---------------------------------------------------------------
//
// XFocusExtension::DoDumpCommandChain	
//
//---------------------------------------------------------------
void XFocusExtension::DoDumpCommandChain()
{
	ICurrentFocusPtr current(L"Application");
	IHierarchyPtr target = current->GetFocus();

	TRACE("Dumping focus chain:\n");

	std::wstring padding = L"   ";
	while (target) {
		Whisper::DumpBoss(target, padding);
	
		IKeyHandlerPtr keys(target);
		if (keys) 
			TRACE(padding, "handles keystrokes\n");
	
		IMenuHandlerPtr menus(target);
		if (menus) 
			TRACE(padding, L"handles menu commands\n");
		
		TRACE("\n");

		target = target->GetParent();
		padding += L"   ";
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XSeperatorExtension
//!		Adds a menu seperator to the Debug menu.
// ===================================================================================
class XSeperatorExtension : public IInstallMenuHandler {
	
public:	
						XSeperatorExtension(XBoss* boss);

public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();
};

DEFINE_INTERFACE_FACTORY(XSeperatorExtension)

//---------------------------------------------------------------
//
// XSeperatorExtension::XSeperatorExtension
//
//---------------------------------------------------------------
XSeperatorExtension::XSeperatorExtension(XBoss* boss)
{
	IInstallMenuHandler::DoSetBoss(boss);

	IMenuBarPtr menubar(this);
	IMenuPtr menu = menubar->GetMenuByName(LoadWhisperString(L"Debug Menu Title"));
	menu->AppendSeparator();
}


//---------------------------------------------------------------
//
// XSeperatorExtension::InstallCommands
//
//---------------------------------------------------------------
void XSeperatorExtension::InstallCommands()
{	
}


//---------------------------------------------------------------
//
// XSeperatorExtension::RemoveCommands
//
//---------------------------------------------------------------
void XSeperatorExtension::RemoveCommands()
{
}


}		// namespace Whisper
#endif	// DEBUG