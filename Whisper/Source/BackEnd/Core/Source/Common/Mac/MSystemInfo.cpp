/*
 *  File:       MSystemInfo.cpp
 *	Summary:	Provides information about the system.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MSystemInfo.cpp,v $
 *		Revision 1.6  2001/11/25 23:37:30  jesjones
 *		Added HasOSX and HasAqua.
 *		
 *		Revision 1.5  2001/10/04 06:13:01  jesjones
 *		GetVersion no longer masks off the high byte (need this when getting the system version number on OS X).
 *		
 *		Revision 1.4  2001/04/21 03:28:52  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/13 07:53:34  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.2  2000/11/09 09:20:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	 4/21/00	JDJ		Rewrote GetAppRefNum.
 *		 <5>	 7/19/99	JDJ		Ifdefed out TrapAvailable if TARGET_CARBON. Disabled sprocket and QD3D
 *									functions on Carbon.
 *		 <4>	 3/22/99	JDJ		HasFloatingWindows returns false if system version is less than 8.6.
 *		 <3>	 1/25/99	JDJ		Added HasTextEncodingConverter and GetTextEncodingConverterVersion.
 *		 <2>	 1/06/99	JDJ		Fixed GetQuickTimeVersion.
 *		 <1>	11/16/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <MSystemInfo.h>

#include <Appearance.h>
#include <CodeFragments.h>
#include <Displays.h>
#include <Gestalt.h>
#include <Movies.h>
#include <Navigation.h>
#include <OpenTransport.h>
#include <OSUtils.h>
#include <Processes.h>
#include <Resources.h>
#include <Sound.h>
#include <Threads.h>
#include <Traps.h>

#if !TARGET_CARBON
	#include <DrawSprocket.h>
	#include <InputSprocket.h>
	#include <NetSprocket.h>
	#include <QD3D.h>
	#include <QD3DViewer.h>
	#include <SoundSprocket.h>
#endif

#include <XDebug.h>
#include <XExceptions.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const int32	kGestaltTempMemBits	= (1 << gestaltTempMemSupport) | (1 << gestaltRealTempMemory) | (1 << gestaltTempMemTracked);

#if UNIVERSAL_INTERFACES_VERSION < 0x0301
	enum {gestaltDragMgrHasImageSupport = 3};
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CheckCFMLink
//
//---------------------------------------------------------------
bool CheckCFMLink(const void* fnPtr)
{
	return fnPtr != nil;
}


//---------------------------------------------------------------
//
// TestGestaltMask
//
// Ask Gestalt whether a feature is available.
//
//---------------------------------------------------------------
bool TestGestaltMask(uint32 selector, int32 bitmap)
{
	int32 result;
	bool avail = (Gestalt(selector, &result) == noErr && (result & bitmap) == bitmap);
	
	return avail;
}


//---------------------------------------------------------------
//
// GetVersion
//
// Return a packed version number for NumVersion.
//
//---------------------------------------------------------------
static Version GetVersion(const NumVersion& num)
{
	Version version = ((uint32) num.majorRev << 8) + num.minorAndBugRev;
	
	return version;
}


//---------------------------------------------------------------
//
// GetVersion
//
// Return a packed version number for the given selector.
//
//---------------------------------------------------------------
static Version GetVersion(uint32 selector)
{
	Version	version = 0;
	
	int32 result;
	if (Gestalt(selector, &result) == noErr)
		version = (Version) (result & 0xFFFF);
	
	return version;
}

#pragma mark -

// ===================================================================================
//	class MSystemInfo
// ===================================================================================

bool MSystemInfo::msHasQuickDraw3D  = false;
bool MSystemInfo::msHasContextMenus = false;

//---------------------------------------------------------------
//
// MSystemInfo::GetAppSpec								[static]
//
//---------------------------------------------------------------
FSSpec MSystemInfo::GetAppSpec()
{
	PRECONDITION(MSystemInfo::HasSystem7());
	
	static FSSpec spec;
	static bool inited = false;
	
	if (!inited) {
		ProcessSerialNumber psn = MSystemInfo::GetAppPSN();

		ProcessInfoRec info;
		info.processInfoLength = sizeof(ProcessInfoRec);
		info.processName       = nil;
		info.processAppSpec    = &spec;
		
		OSErr err = GetProcessInformation(&psn, &info);
		ThrowIfOSErr(err);
		
		inited = true;
	}
	
	return spec;
}


//---------------------------------------------------------------
//
// MSystemInfo::GetAppRefNum							[static]
//
// There doesn't appear to be a good way to do this. Most frameworks
// seem to call CurResFile during startup, but this is problematic
// because hard-linked libraries can open resource forks as they're
// loaded. We'll simply call FSpOpenResFile. This is OK because 
// we're using fsRdPerm which means that the system won't add a 
// second copy of the app's resource map to the resource chain. 
//
//---------------------------------------------------------------
int16 MSystemInfo::GetAppRefNum()
{	
	static int16 refNum = -1;
	
	if (refNum == -1) {
		FSSpec spec = MSystemInfo::GetAppSpec();
		
		refNum = ::FSpOpenResFile(&spec, fsRdPerm);	// $$$ Carbon has a new call that will return an existing refNum if the resource fork is already open
		ThrowIfResError();
	}

	return refNum;
}


//---------------------------------------------------------------
//
// MSystemInfo::GetAppSizeFlags							[static]
//
//---------------------------------------------------------------
uint32 MSystemInfo::GetAppSizeFlags()
{
	PRECONDITION(MSystemInfo::HasSystem7());
	
	static uint32 flags = 0;
	static bool inited = false;
	
	if (!inited) {
		ProcessSerialNumber psn = MSystemInfo::GetAppPSN();

		ProcessInfoRec info;
		info.processInfoLength = sizeof(ProcessInfoRec);
		info.processName       = nil;
		info.processAppSpec    = nil;
		
		OSErr err = GetProcessInformation(&psn, &info);
		ThrowIfOSErr(err);
		
		flags = info.processMode;

		inited = true;
	}
	
	return flags;
}


//---------------------------------------------------------------
//
// MSystemInfo::GetAppPSN								[static]
//
//---------------------------------------------------------------
ProcessSerialNumber MSystemInfo::GetAppPSN()
{
	PRECONDITION(MSystemInfo::HasSystem7());
	
	static ProcessSerialNumber psn;
	static bool inited = false;
	
	if (!inited) {
		OSErr err = GetCurrentProcess(&psn);
		ThrowIfOSErr(err);

		inited = true;
	}
	
	return psn;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MSystemInfo::GetSystemVersion						[static]
//
//---------------------------------------------------------------
Version MSystemInfo::GetSystemVersion()
{
	static Version version = GetVersion(gestaltSystemVersion);
	
	return version;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasSystem7								[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasSystem7()
{	
	static bool has = MSystemInfo::GetSystemVersion() >= 0x0700;

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasSystem8								[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasSystem8()
{	
	static bool has = MSystemInfo::GetSystemVersion() >= 0x0800;

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasOSX									[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasOSX()
{	
	static bool has = MSystemInfo::GetSystemVersion() >= 0x0A00;

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasAqua									[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasAqua()
{	
	static bool has = TestGestaltMask(gestaltMenuMgrAttr, gestaltMenuMgrAquaLayoutMask);

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasTempMem								[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasTempMem()
{	
	static bool has = TestGestaltMask(gestaltOSAttr, kGestaltTempMemBits);

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasThreadMgr							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasThreadMgr()
{
	int32 gestaltMask = 1 << gestaltThreadMgrPresent;

	// On PowerMacs, we'll need the native (shared library) 
	// version of the Thread Manager.  
	gestaltMask	|= 1 << gestaltThreadsLibraryPresent;

	static bool has = TestGestaltMask(gestaltThreadMgrAttr, gestaltMask) && CheckCFMLink(::NewThread);
	
	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasASLM									[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasASLM()
{	
	static bool has = TestGestaltMask('aslm', 0);

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasCFM									[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasCFM()
{	
	static bool has = TestGestaltMask(gestaltCFMAttr, gestaltCFMPresent);

	return has;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MSystemInfo::GetQuickDrawVersion						[static]
//
//---------------------------------------------------------------
Version MSystemInfo::GetQuickDrawVersion()
{
	static Version version = GetVersion(gestaltQuickdrawVersion);
	
	return version;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasColorQuickDraw						[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasColorQuickDraw()
{	
	int32 result;
	static bool has = Gestalt(gestaltQuickdrawVersion, &result) == noErr && result != gestaltOriginalQD;

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::Has32BitQuickDraw						[static]
//
//---------------------------------------------------------------
bool MSystemInfo::Has32BitQuickDraw()
{	
	int32 result;
	static bool has = Gestalt(gestaltQuickdrawVersion, &result) == noErr && result > gestalt8BitQD;

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasQuickDrawGX							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasQuickDrawGX()
{	
	static bool has = TestGestaltMask('grfx', 0);

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasGXPrinting							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasGXPrinting()
{	
	static bool has = TestGestaltMask('pmgr', 0);

	return has;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MSystemInfo::GetTextEditVersion						[static]
//
//---------------------------------------------------------------
Version MSystemInfo::GetTextEditVersion()
{
	static Version version = GetVersion(gestaltTextEditVersion);
	
	return version;
}


//---------------------------------------------------------------
//
// MSystemInfo::GetScriptMgrVersion						[static]
//
//---------------------------------------------------------------
Version MSystemInfo::GetScriptMgrVersion()
{
	static Version version = GetVersion(gestaltScriptMgrVersion);
	
	return version;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasGrayishText							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasGrayishText()
{	
	static bool has = TestGestaltMask(gestaltQuickdrawFeatures, 1 << gestaltHasGrayishTextOr);

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasTrueType								[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasTrueType()
{	
	static bool has = TestGestaltMask(gestaltFontMgrAttr, gestaltOutlineFonts);

	return has;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MSystemInfo::GetDisplayMgrVersion					[static]
//
// $$$ This selector isn't documented in Inside Mac and doesn't 
// $$$ appear to correspond to the others. The sample code from 
// $$$ Apple uses the low byte of the high word for the major
// $$$ version number so we'll use that until we learn how the
// $$$ lower word is formatted.
//
//---------------------------------------------------------------
Version MSystemInfo::GetDisplayMgrVersion()
{
	static Version version = 0;
	static bool inited = false;
	
	if (!inited) {
		int32 result;
		if (Gestalt(gestaltDisplayMgrVers, &result) == noErr)
			version = (Version) (result & 0xFFF00);

		// If DisplayLib was weak linked we can't use DM 2.0 if 
		// it's not installed.
		if (version >= 0x0200 && !CheckCFMLink(DMGetDisplayMode))
			version = 0x0100;
	
		inited = true;
	}
	
	return version;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasDisplayMgr							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasDisplayMgr()
{	
	static bool has = TestGestaltMask(gestaltDisplayMgrAttr, 1 << gestaltDisplayMgrPresent);

	return has;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MSystemInfo::GetQuickTimeVersion						[static]
//
//---------------------------------------------------------------
Version MSystemInfo::GetQuickTimeVersion()
{
    Version version = 0;
    NumVersion numVersion;
    if (::Gestalt(gestaltQuickTimeVersion, (int32*) &numVersion) == noErr)
    	version = GetVersion(numVersion);
    
    return version;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasQuickTime							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasQuickTime()
{	
#if __POWERPC__
	static bool has = MSystemInfo::GetQuickTimeVersion() != 0 && CheckCFMLink(CompressImage) && TestGestaltBit(gestaltQuickTimeFeatures, gestaltPPCQuickTimeLibPresent);
#else
	static bool has = MSystemInfo::GetQuickTimeVersion() != 0 && CheckCFMLink(CompressImage);
#endif

	return has;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MSystemInfo::GetSoundMgrVersion						[static]
//
//---------------------------------------------------------------
Version MSystemInfo::GetSoundMgrVersion()
{
	static Version version = GetVersion(SndSoundManagerVersion());
	
	return version;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasSoundMgr3							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasSoundMgr3()
{	
	static bool has = MSystemInfo::GetSoundMgrVersion() >= 0x300;

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::Has16BitSound							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::Has16BitSound()
{	
	static bool has = TestGestaltBit(gestaltSoundAttr, gestalt16BitSoundIO);

	return has;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MSystemInfo::HasDragMgr								[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasDragMgr()
{	
	static bool has = TestGestaltBit(gestaltDragMgrAttr, gestaltDragMgrPresent);

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasTranslucentDrag						[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasTranslucentDrag()
{	
#if __POWERPC__
	static bool has = TestGestaltBit(gestaltDragMgrAttr, gestaltDragMgrHasImageSupport);
#else
	static bool has = false;
#endif

	return has;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MSystemInfo::HasDrawSprocket							[static]
//
//---------------------------------------------------------------
#if !TARGET_CARBON						// $$$ Carbon is supposed to have these...
bool MSystemInfo::HasDrawSprocket()
{
	static bool has = CheckCFMLink(DSpStartup);

	return has;
}
#endif


//---------------------------------------------------------------
//
// MSystemInfo::HasInputSprocket						[static]
//
//---------------------------------------------------------------
#if !TARGET_CARBON
bool MSystemInfo::HasInputSprocket()
{
	static bool has = CheckCFMLink(ISpInit);

	return has;
}
#endif


//---------------------------------------------------------------
//
// MSystemInfo::HasNetSprocket							[static]
//
//---------------------------------------------------------------
#if !TARGET_CARBON
bool MSystemInfo::HasNetSprocket()
{
	static bool has = CheckCFMLink(NSpInitialize);

	return has;
}
#endif


//---------------------------------------------------------------
//
// MSystemInfo::HasSoundSprocket						[static]
//
//---------------------------------------------------------------
#if !TARGET_CARBON
bool MSystemInfo::HasSoundSprocket()
{
	static bool has = CheckCFMLink(SSpConfigureSpeakerSetup);

	return has;
}
#endif


//---------------------------------------------------------------
//
// MSystemInfo::GetInputSprocketVersion					[static]
//
//---------------------------------------------------------------
#if !TARGET_CARBON
Version MSystemInfo::GetInputSprocketVersion()
{
	static Version version = MSystemInfo::HasInputSprocket() ? GetVersion(ISpGetVersion()) : 0;
	
	return version;
}
#endif

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MSystemInfo::HasMacTCP								[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasMacTCP()
{
	int32 result;
	static bool has = ::Gestalt('mtcp', &result) == noErr;

	return has;
}

	
//---------------------------------------------------------------
//
// MSystemInfo::HasOpenTransport						[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasOpenTransport()
{
	int32 result;
	static bool has = ::Gestalt(gestaltOpenTpt, &result) == noErr;

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::GetOpenTransportVersion					[static]
//
//---------------------------------------------------------------
Version MSystemInfo::GetOpenTransportVersion()
{
	static Version version = 0;
	static bool inited = false;
	
	if (!inited) {
		if (MSystemInfo::HasOpenTransport()) {
			NumVersion theVersion;
			OSErr err = ::Gestalt(gestaltOpenTptVersions, (long*) &theVersion);
			
			if (err == noErr)
				version = GetVersion(theVersion);
		}
	
		inited = true;
	}
	
	return version;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MSystemInfo::HasAppearanceMgr						[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasAppearanceMgr()
{	
	static bool has = TestGestaltMask(gestaltAppearanceAttr, gestaltAppearanceExists);

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::GetAppearanceVersion					[static]
//
//---------------------------------------------------------------
Version MSystemInfo::GetAppearanceVersion()
{
	static Version version = 0;
	static bool inited = false;
	
	if (!inited) {
		if (MSystemInfo::HasAppearanceMgr()) {
			OSErr err = ::Gestalt(gestaltAppearanceVersion, (int32*) &version);
			
			if (err != noErr)
				version = 0x0100;		// gestaltAppearanceVersion is only present starting with 1.0.1
		}
	
		inited = true;
	}
	
	return version;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasWindowMgr2							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasWindowMgr2()
{
#if UNIVERSAL_INTERFACES_VERSION >= 0x0320 
	static bool has = TestGestaltMask(gestaltWindowMgrAttr, gestaltWindowMgrPresent);
#else
	static bool has = false;
#endif

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasFloatingWindows						[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasFloatingWindows()
{
#if NATIVE_FLOATING_WINDOWS && UNIVERSAL_INTERFACES_VERSION >= 0x0320 
	static bool has = MSystemInfo::HasWindowMgr2() && MSystemInfo::GetSystemVersion() >= 0x0860;	// Mac OS 5.5 floating windows are flaky...
#else
	static bool has = false;
#endif

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasNavServices							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasNavServices()
{	
	static bool has = NavServicesAvailable();

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasContextMenus							[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasContextMenus()
{	
	static bool inited = false;
	
	if (!inited) {
		msHasContextMenus = CheckCFMLink(InitContextualMenus);
		
		inited = true;
	}

	return msHasContextMenus;
}


//---------------------------------------------------------------
//
// MSystemInfo::HasTextEncodingConverter				[static]
//
//---------------------------------------------------------------
bool MSystemInfo::HasTextEncodingConverter()
{	
	static bool has = CheckCFMLink(TECGetInfo);

	return has;
}


//---------------------------------------------------------------
//
// MSystemInfo::GetTextEncodingConverterVersion			[static]
//
//---------------------------------------------------------------
Version MSystemInfo::GetTextEncodingConverterVersion()
{
	static Version version = 0;
	static bool inited = false;
	
	if (!inited) {
		if (MSystemInfo::HasTextEncodingConverter()) {
			TECInfoHandle infoH = nil;
			OSStatus err = TECGetInfo(&infoH);
			if (err == noErr && infoH != nil) {
				version = (**infoH).tecVersion;
				DisposeHandle((Handle) infoH);
			}
		}
	
		inited = true;
	}
	
	return version;
}


}	// namespace Whisper
